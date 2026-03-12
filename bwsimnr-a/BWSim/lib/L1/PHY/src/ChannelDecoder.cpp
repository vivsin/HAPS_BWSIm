/*************************************************************************
*
* CEWiT CONFIDENTIAL
* __________________
*
* All Rights Reserved © 2012 CEWiT, India
*
* NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
* and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
* Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
* express, printed and signed license for use is strictly forbidden.
*/
#include "../include/ChannelCoder.h"

/*! \brief
\param[in] rxSoftCircularBuffer	Rate-Matched Softbits for the codeword(Circular-Buffer)
\param[in] rxCircularBufferSize 	Size of the Codeword(Output of Turbo-Encoder)
\param[in] permIndex	Indicate absolute Codeword index
\param[in] iCodeWord	Layer Index
\param[in] iCodewWord	Indicate relative Codeword index within a layer
\return tempVec		Input to the Turbo-Decoder (Circular-Buffer) */
vec Decoder::performDeRateMatching(vec &rxSoftCircularBuffer,int rxCircularBufferSize,int permIndex,int iCodeWord,int iCodeBlock,CoderInfo_S &coderInfo)
{
    vec tempVec;
    tempVec = zeros(rxCircularBufferSize);
   
    for (int iIndex = 0;iIndex < coderInfo.puncturedLocations(permIndex).length();iIndex ++)
      tempVec(coderInfo.puncturedLocations(permIndex)(iIndex)) += rxSoftCircularBuffer(iIndex);
    processHarqReception(tempVec,iCodeWord,iCodeBlock);

	return tempVec;
}
/*! \brief Do the Soft-Demodulation
\param [in] rxSymbols           Received symbols after applying equalizer
\param [in] equalizedChannel	Actual Channel x Equilizer
\param [in] N0 		        Noise Variance
\return			        SoftBits after Demodulation
        */
Array<vec> Decoder::performDemodulation(Array<cvec> &rxSymbols,Array<cvec> &equalizedChannel,double NO)
{       
        Array<vec> tempchannelCoderRxCircularBuffer;
	int iCodeWord,alphabetSize;
        tempchannelCoderRxCircularBuffer.set_size(rxSymbols.size());
	for (iCodeWord = 0;iCodeWord < currentHarqInfo.codeWordIndicesToConsider.length();iCodeWord ++)
	{
		alphabetSize = (int) pow2((double) tbConfig.modAlphabet(iCodeWord));

        if(alphabetSize==2) //Pi/2 BPSK
            rxSymbols(iCodeWord)=removePiBy2Rotation(rxSymbols(iCodeWord));
        
#ifdef USING_ITPP
        if(alphabetSize==2) 
        {
            rxSymbols(iCodeWord)/= complex<double> (1.0/sqrt(2.0),1.0/sqrt(2.0)); // ITPP modulator generates +1, -1
            tempchannelCoderRxCircularBuffer(iCodeWord) = bpskModem.demodulate_soft_bits(rxSymbols(iCodeWord),equalizedChannel(iCodeWord),NO,APPROX);
        }
        else
#endif
        {
            qamModem.set_M(alphabetSize);
            tempchannelCoderRxCircularBuffer(iCodeWord) = qamModem.demodulate_soft_bits(rxSymbols(iCodeWord),equalizedChannel(iCodeWord),NO,APPROX);
        }
            
        #ifdef _DEBUG_CHANNELCODER_ENABLE
                cout<<"iCodeWord ="<<iCodeWord<<" alphabetSize ="<<alphabetSize<<"\nrxSymbols "<<" length ="<<rxSymbols(iCodeWord).length()<<endl;
                cout<<"tempchannelCoderRxCircularBuffer(iCodeWord) ="<<" length ="<<length(tempchannelCoderRxCircularBuffer(iCodeWord))<<endl;
		#endif

        }
        return tempchannelCoderRxCircularBuffer;
}
// bool Decoder::performCodeBlockConcatenation(BlockSegments &blockSegments, bvec &inBits){
//         bvec outBits;
// 	int totalLength,startIndex = 0;
// 	outBits.set_length(0);
// 	totalLength = inBits.length();
// 	startIndex = blockSegments.nFillerBitsUsed;
// 	outBits = inBits.mid(startIndex,(totalLength - startIndex));
// 	return crc_LTE.performCRCDecoding(outBits);
// }

/*! \brief Do the channel Decoding of the rxSymbols using equalizedChannel & Noise Variance N0.
\param [in] rxSymbols 		Received Symbols multiplied with Equalizer.
\param [in] equalizedChannel	ActualChannel x Equlizer
\param [in] N0			Noise Variance
                */
TransmissionStatus_S Decoder::performLDPCChannelDecoding(Array<cvec> &rxSymbols,Array<cvec> &equalizedChannel,double NO)
{
  bvec tempBvec;
  bool flagProper;
  Array<vec> channelCoderRxCircularBuffer;
  Array<bvec> decodedBits;
  vec tempVec,rxSoftCircularBuffer;
  std::vector<bool> decodedBitsBool;
  int iCodeWord;
    
  TransmissionStatus_S transmissionStatus;
  transmissionStatus.isSuccess.set_length(currentHarqInfo.codeWordIndicesToConsider.length());
  transmissionStatus.isSuccessPerCB.set_length(currentHarqInfo.codeWordIndicesToConsider.length());
  transmissionStatus.isNewTransmission=currentHarqInfo.isNewTransmission;
  transmissionStatus.harqProcessID=currentHarqInfo.harqProcessID;
  transmissionStatus.infoBitsBlockSizePerCodeword.set_length(currentHarqInfo.codeWordIndicesToConsider.length());
  transmissionStatus.codeWordIndicesToConsider=currentHarqInfo.codeWordIndicesToConsider;
  transmissionStatus.nBitsInError.set_length(currentHarqInfo.codeWordIndicesToConsider.length());
  transmissionStatus.txInfoBitsPerCodeWord = harqProcess(currentHarqInfo.harqProcessID).coderInfo.infoBitsPerCodeWordBackup;
  transmissionStatus.rxInfoBitsPerCodeWord.set_length(currentHarqInfo.codeWordIndicesToConsider.length());
  harqProcess(currentHarqInfo.harqProcessID).receiverRetransmitBuffer.set_length(currentHarqInfo.codeWordIndicesToConsider.length());
//   harqProcess(currentHarqInfo.harqProcessID).coderInfo.BGperCodeWord.resize(currentHarqInfo.codeWordIndicesToConsider.length());
// 	TurboCoderLTE turboCoderd;
//      turboCoderd.setLTETables(lteTables);

// 	runIndex = 0;
  decodedBits.set_size(currentHarqInfo.codeWordIndicesToConsider.length());
  channelCoderRxCircularBuffer = performDemodulation(rxSymbols,equalizedChannel,NO);
    
  if(harqProcess(currentHarqInfo.harqProcessID).isScramblerEnabled)
    performDeScrambling(channelCoderRxCircularBuffer);
  int nCodeWords = currentHarqInfo.codeWordIndicesToConsider.length();
  std::vector<int> E_vec(nCodeWords); 
  double codeRate;
  for (iCodeWord = 0;iCodeWord < currentHarqInfo.codeWordIndicesToConsider.length();iCodeWord ++)
  {
    if(transmissionStatus.isNewTransmission(iCodeWord))//for first transmission create recbuffer
    {
	harqProcess(currentHarqInfo.harqProcessID).receiverRetransmitBuffer(iCodeWord).clear();
	// Check if below code is needed as it was deledt in fixLDPC branch - Dhiv
	std::vector<double> a;
	harqProcess(currentHarqInfo.harqProcessID).receiverRetransmitBuffer(iCodeWord) = a;
    //             harqProcess(currentHarqInfo.harqProcessID).receiverRetransmitBuffer=concat(harqProcess(currentHarqInfo.harqProcessID).receiverRetransmitBuffer,a);
    }
        int B_=-1, Q=-1, rvid=-1, BG=-1;
        
    auto NL = tbConfig.numLayersPerCodeWord(iCodeWord); 
//      cout <<"tbConfig.nREsAllocated: " <<tbConfig.nREsAllocated<< endl;
//      cout <<"NL: " <<NL<< endl;
//     cout << "tbConfig.nREsAllocated: "<<tbConfig.nREsAllocated<<endl;
     Q = (int)tbConfig.modAlphabet(iCodeWord);
//          cout <<"Q: " <<Q<< endl;
    E_vec[iCodeWord] = tbConfig.nREsAllocated* NL* Q ;  // No. of output symbols (not bits) allocated per codeword, later these symbols are divided for codeblocks
    
//     cout << "\t nREsAllocated : " <<tbConfig.nREsAllocated <<"E_vec size in bits: " <<E_vec[iCodeWord]<< endl;
//     cout<<"TB Size in bits: "<<tbConfig.transportBlockSize(iCodeWord)<<endl;

    rvid = tbConfig.rvIndex(iCodeWord);
    

    codeRate = (double)tbConfig.transportBlockSize(iCodeWord)/(double)E_vec[iCodeWord];

    E_vec[iCodeWord] = E_vec[iCodeWord]/Q;
    if(transmissionStatus.isNewTransmission(iCodeWord))
    {
      if(tbConfig.transportBlockSize(iCodeWord)<=292 || (tbConfig.transportBlockSize(iCodeWord)<=3824 && codeRate<=0.67) || codeRate<= 0.25)
	  BG=2;
      else
	  BG=1;
      harqProcess(currentHarqInfo.harqProcessID).coderInfo.BGperCodeWord[iCodeWord] = BG;
      
      if(codeRate>0.92578125)
      {
		cout<<"Tb Size: "<<tbConfig.transportBlockSize(iCodeWord)<<endl;
		cout<<"nREs: "<<tbConfig.nREsAllocated<<" NL: "<<NL<<" Q: "<<Q<<endl;
		cout<<"E_vec(nREs*NL*Q): "<<E_vec[iCodeWord]*Q<<endl; 
		cout << "CodeRate (Tbsize/Evec): "<< codeRate<<"In performLDPCChannelDecoding, Abnormal codeRate!! .. aborting !!"<<endl;abort();
      }
    }
    else
    {
      BG = harqProcess(currentHarqInfo.harqProcessID).coderInfo.BGperCodeWord[iCodeWord];
      rvid = 0; // hard coding
//       if(codeRate>0.92578125)
//       {
// 	cout << "Retransmit : CodeRate: "<< codeRate<<"In performLDPCChannelDecoding, Abnormal codeRate!! .. aborting !!"<<endl;abort();
//       }
    }
    
//     cout <<"Receiver BG: "<<BG<<endl;
    B_ = findB_(tbConfig.transportBlockSize(iCodeWord)+24,BG); // 24 is added for CRC
    harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.errorFlag[iCodeWord] = 0;
    decodedBits(iCodeWord).set_size(0);
    // 		cout << "channelCoderRxCircularBuffer(iCodeWord)): "<<endl<<channelCoderRxCircularBuffer(iCodeWord)<<endl;

    // 		cout << "ReTX BUFFER: "<<endl;
    // 		for(auto x : harqProcess(currentHarqInfo.harqProcessID).receiverRetransmitBuffer(iCodeWord))
    // 		{ 
    // 		  std::cout<< x<<" ";
    // 		}
    // 		std::cout<<std::endl;
//     cout <<"ReTx length 1: "<<harqProcess(currentHarqInfo.harqProcessID).receiverRetransmitBuffer(iCodeWord).size()<<endl;
//     cout <<"Iput to decoder: " <<channelCoderRxCircularBuffer(iCodeWord)<<endl;
    decodedBitsBool = dataChannelDecoder(ITPPVectorstdVector(channelCoderRxCircularBuffer(iCodeWord)), harqProcess(currentHarqInfo.harqProcessID).receiverRetransmitBuffer(iCodeWord), E_vec[iCodeWord], B_, Q, rvid, BG,transmissionStatus.isSuccessPerCB(iCodeWord));   
//     cout <<"ReTx length 2: "<<harqProcess(currentHarqInfo.harqProcessID).receiverRetransmitBuffer(iCodeWord).size()<<endl;
    flagProper = nrCRC.performCRCCheckNR(decodedBitsBool);
    //         cout << "FLAG PROPER:   "<<flagProper<< endl;
    //debug
        
        
    decodedBits(iCodeWord) =  BoolVectortoBVec(nrCRC.removeCRCNR(decodedBitsBool));
	    
    transmissionStatus.rxInfoBitsPerCodeWord(iCodeWord) = decodedBits(iCodeWord);
    transmissionStatus.nBitsInError(iCodeWord) = length(find(transmissionStatus.rxInfoBitsPerCodeWord(iCodeWord)-transmissionStatus.txInfoBitsPerCodeWord(iCodeWord)));
    
    if (!flagProper)
    {
      transmissionStatus.isSuccess(iCodeWord)=false;
      #ifdef _DEBUG_CHANNELCODER_ENABLE
      cout<<"\n***FAILURE*** for currentHarqInfo.harqProcessID ="<<currentHarqInfo.harqProcessID;
      #endif
    }
    else
    {
      #ifdef _DEBUG_CHANNELCODER_ENABLE
      if(flagProper == true)
      cout<<"\n***SUCCESS*** for currentHarqInfo.harqProcessID ="<<currentHarqInfo.harqProcessID;
      #endif
      transmissionStatus.isSuccess(iCodeWord)=true;
    }
    transmissionStatus.infoBitsBlockSizePerCodeword(iCodeWord) = transmissionStatus.txInfoBitsPerCodeWord(iCodeWord).length();
  }

  transmissionStatus.usedResourceElements+=tbConfig.nREsAllocated;
  #ifdef _DEBUG_CHANNELCODER_ENABLE
  cout<<"\nFn:performLDPCChannelDecoding decodedBits ="<<decodedBits<<"\n decodedBits.size() ="<<decodedBits.size()<<" length ="<<decodedBits(0).length()<<endl;
  #endif

  return(transmissionStatus);
}


// TransmissionStatus_S Decoder::performChannelDecoding(Array<cvec> &rxSymbols,Array<cvec> &equalizedChannel,double NO)
// {
// 	TransmissionStatus_S transmissionStatus;
//         transmissionStatus.isSuccessPerCB.set_length(currentHarqInfo.codeWordIndicesToConsider.length());
// 	
// 	bvec tempBvec;
// 	bool flagProper;
// 	Array<vec> channelCoderRxCircularBuffer;
// 	Array<bvec> decodedBits;
//     vec tempVec,rxSoftCircularBuffer;
//     int iCodeWord,iCodeBlock,rxCircularBufferSize,startLocation,runIndex;
// 	TurboCoderLTE turboCoder;
// 	turboCoder.setLTETables((cLTETables*)tbTables);
// 	
// 	runIndex = 0;
// 	decodedBits.set_size(currentHarqInfo.codeWordIndicesToConsider.length());
// 	bvec flagPerCB;
// 	channelCoderRxCircularBuffer = performDemodulation(rxSymbols,equalizedChannel,NO);
//     
//     
// 	if(harqProcess(currentHarqInfo.harqProcessID).isScramblerEnabled)
// 	  performDeScrambling(channelCoderRxCircularBuffer);
// 
// 	for (iCodeWord = 0;iCodeWord < currentHarqInfo.codeWordIndicesToConsider.length();iCodeWord ++)
// 	{
// 		startLocation = 0;
// 		harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.errorFlag[iCodeWord] = 0;
// 		decodedBits(iCodeWord).set_size(0);
// 		flagPerCB.set_size(harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.nCodeBlocksPerCodeWord[iCodeWord]);
// 
// 		for (iCodeBlock = 0;iCodeBlock < harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.nCodeBlocksPerCodeWord[iCodeWord];iCodeBlock ++)
// 		{
//             if(harqProcess(currentHarqInfo.harqProcessID).isTurboCoderEnabled)
//                 rxCircularBufferSize = harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.codeBlockSizes(iCodeWord)[iCodeBlock] * 3 + 12;
//             else
//                 rxCircularBufferSize = harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.codeBlockSizes(iCodeWord)[iCodeBlock];
//             
// 	    rxSoftCircularBuffer = channelCoderRxCircularBuffer(iCodeWord).mid(startLocation,harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.ratematchedOutputSize(iCodeWord)[iCodeBlock]);
// 	    
// 			startLocation += harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.ratematchedOutputSize(iCodeWord)[iCodeBlock];
//             tempVec = performDeRateMatching(rxSoftCircularBuffer,rxCircularBufferSize,runIndex,iCodeWord,iCodeBlock,harqProcess(currentHarqInfo.harqProcessID).coderInfo);
// 			if(harqProcess(currentHarqInfo.harqProcessID).isTurboCoderEnabled)
// 				tempBvec = turboCoder.decode(tempVec,harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.codeBlockSizes(iCodeWord)[iCodeBlock]/*,NO*--for zerowarnings*/);
// 			else
// 				tempBvec = tempVec<=0;
// 
// 			#ifdef _DEBUG_CHANNELCODER_ENABLE
// 			cout<<"\noutBits ="<<tempBvec<<endl;
// 			#endif
// 			
// 			runIndex ++;
// 			append(decodedBits(iCodeWord),tempBvec);
// 
// 			if (!iCodeBlock)
// 			{
// 			int startIndex=harqProcess(currentHarqInfo.harqProcessID).coderInfo.blockSegments(iCodeWord).nFillerBitsUsed;
// 			int totalLength=tempBvec.length();
// 			tempBvec=tempBvec.mid(startIndex,(totalLength - startIndex));
// 			flagPerCB(iCodeBlock)=crc_LTE.performCRCDecoding(tempBvec);
// 			}
// 			else
//                         flagPerCB(iCodeBlock)=crc_LTE.performCRCDecoding(tempBvec);			
// 		}
// 		transmissionStatus.isSuccessPerCB(iCodeWord)=flagPerCB;
// 	}
// 
// 	
// 	transmissionStatus.isSuccess.set_length(currentHarqInfo.codeWordIndicesToConsider.length());
// 	transmissionStatus.isNewTransmission=currentHarqInfo.isNewTransmission;
// 	transmissionStatus.harqProcessID=currentHarqInfo.harqProcessID;
// 	transmissionStatus.infoBitsBlockSizePerCodeword.set_length(currentHarqInfo.codeWordIndicesToConsider.length());
// 	transmissionStatus.codeWordIndicesToConsider=currentHarqInfo.codeWordIndicesToConsider;
// 	transmissionStatus.nBitsInError.set_length(currentHarqInfo.codeWordIndicesToConsider.length());
//     transmissionStatus.txInfoBitsPerCodeWord = harqProcess(currentHarqInfo.harqProcessID).coderInfo.infoBitsPerCodeWordBackup;
//     transmissionStatus.rxInfoBitsPerCodeWord.set_length(currentHarqInfo.codeWordIndicesToConsider.length());
// 	for (iCodeWord = 0;iCodeWord < currentHarqInfo.codeWordIndicesToConsider.length();iCodeWord ++)
// 	{
// 	  bool isTBCRCPassed=performCodeBlockConcatenation(harqProcess(currentHarqInfo.harqProcessID).coderInfo.blockSegments(iCodeWord),decodedBits(iCodeWord));
// 	  
// 	  if(isTBCRCPassed) 
// 	    flagProper = (bool)sum(to_ivec(transmissionStatus.isSuccessPerCB(iCodeWord)));
// 	  else // TB CRC failed => TB Failed
// 	    flagProper = false;
// 	  
// 	  decodedBits(iCodeWord) = removeCRCBitsOfBlockSegments(harqProcess(currentHarqInfo.harqProcessID).coderInfo.blockSegments(iCodeWord),decodedBits(iCodeWord),getNumOfCRCBits(crc_LTE.crcType));
// 
//       //! If Segmentation happens (ie infoBits Size > 6144), then only remove the second CRC.
//       if(harqProcess(currentHarqInfo.harqProcessID).coderInfo.blockSegments(iCodeWord).nCodesPlus+harqProcess(currentHarqInfo.harqProcessID).coderInfo.blockSegments(iCodeWord).nCodesMinus >1)
//         transmissionStatus.rxInfoBitsPerCodeWord(iCodeWord) = trimCRCBits(decodedBits(iCodeWord),getNumOfCRCBits(crc_LTE.crcType));
//       else
//       	transmissionStatus.rxInfoBitsPerCodeWord(iCodeWord) = decodedBits(iCodeWord);
// 
//      	transmissionStatus.nBitsInError(iCodeWord) = length(find(transmissionStatus.rxInfoBitsPerCodeWord(iCodeWord)-transmissionStatus.txInfoBitsPerCodeWord(iCodeWord)));
// 
// 		if (!flagProper)
// 		{
//                         transmissionStatus.isSuccess(iCodeWord)=false;
// 			#ifdef _DEBUG_CHANNELCODER_ENABLE
// 			  cout<<"\n***FAILURE*** for currentHarqInfo.harqProcessID ="<<currentHarqInfo.harqProcessID;
// 			#endif
// 		}
// 		else
// 		{
// 			#ifdef _DEBUG_CHANNELCODER_ENABLE
//                         if(flagProper == true)
// 			  cout<<"\n***SUCCESS*** for currentHarqInfo.harqProcessID ="<<currentHarqInfo.harqProcessID;
// 			#endif
// 			transmissionStatus.isSuccess(iCodeWord)=true;
// 		}
// 		transmissionStatus.infoBitsBlockSizePerCodeword(iCodeWord) = transmissionStatus.txInfoBitsPerCodeWord(iCodeWord).length();
// 	}
// 
// 	transmissionStatus.usedResourceElements+=tbConfig.nREsAllocated;
//         #ifdef _DEBUG_CHANNELCODER_ENABLE
//         cout<<"\nFn:performChannelDecoding decodedBits ="<<decodedBits<<"\n decodedBits.size() ="<<decodedBits.size()<<" length ="<<decodedBits(0).length()<<endl;
// 	#endif
// 
// 	return(transmissionStatus);
// }

void Decoder::performDeScrambling(Array<vec> &channelCoderRxCircularBuffer)
{
	for (int iCodeWord = 0;iCodeWord < currentHarqInfo.codeWordIndicesToConsider.length();iCodeWord ++)
      { 
	 for (int iBit = 0;iBit < harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer(iCodeWord).length();iBit ++)
		{
		  channelCoderRxCircularBuffer(iCodeWord)(iBit) = channelCoderRxCircularBuffer(iCodeWord)(iBit) * (1.0 - 2.0 * harqProcess(currentHarqInfo.harqProcessID).coderInfo.scramblerSeq(iCodeWord)(iBit));
		}
	  }
}

/*! \brief Function to permute the codeWords by picking the permutation sequence.. */
ivec ChannelCoder::removeObjectIndexFromPermutationSequence(ivec existingPermutationSequence, int indexToRemove)
{
    if(existingPermutationSequence.length() == 0) return existingPermutationSequence;

    int idOfRemovedObject = existingPermutationSequence(indexToRemove);
    existingPermutationSequence.del(indexToRemove);
    bvec indices=existingPermutationSequence>idOfRemovedObject;

    if(indices.length())
    {
        ivec temp = find(indices);
        existingPermutationSequence(temp) -= 1;
    }
    return existingPermutationSequence;
}


/*! \brief Function called \@ performDeRateMatching for each codeword, to update harqProcess
\param[in,out] codeBlock Input to the Turbo-Decoder  */
void Decoder::processHarqReception(vec& codeBlock,int iCodeWord,int iCodeBlock)
{
	#ifdef _DEBUG_CHANNELCODER_ENABLE
	cout<<"\n*processHarqReception for currentHarqInfo.harqProcessID ="<<currentHarqInfo.harqProcessID;
	#endif

        if(harqProcess(currentHarqInfo.harqProcessID).channelCoderRxCircularBufferPerCodeBlock(iCodeWord).size() <= iCodeBlock)
	  harqProcess(currentHarqInfo.harqProcessID).channelCoderRxCircularBufferPerCodeBlock(iCodeWord).set_size(iCodeBlock+1, true);

        if(!harqProcess(currentHarqInfo.harqProcessID).isNewTransmission(iCodeWord))
        {
          codeBlock += harqProcess(currentHarqInfo.harqProcessID).channelCoderRxCircularBufferPerCodeBlock(iCodeWord)(iCodeBlock);
        }

	harqProcess(currentHarqInfo.harqProcessID).channelCoderRxCircularBufferPerCodeBlock(iCodeWord)(iCodeBlock) = codeBlock;

}

/*! \brief Function called \@ receiver to flush successful or timed out harqProcess */
void ChannelCoder::flushHarqProcess(int harqIDtoFlush, int codeWordIndex)
{
	#ifdef _DEBUG_CHANNELCODER_ENABLE
	cout<<"\n*flushHarqProcess for harqIDtoFlush ="<<harqIDtoFlush;
	#endif
	if(codeWordIndex==-1) {	
	
	  for(int k =0; k < harqProcess(harqIDtoFlush).isNewTransmission.size(); k++)
	    {
	      harqProcess(harqIDtoFlush).newDataIndicator_old(k)= 1;
	    }
	  harqProcess(harqIDtoFlush).coderInfo.clear();
	  for (int iCodeWord = 0;iCodeWord < currentHarqInfo.codeWordIndicesToConsider.length();iCodeWord ++) {
	    extendAndClearDimensions(harqProcess(harqIDtoFlush).channelCoderRxCircularBufferPerCodeBlock,iCodeWord);
	    extendAndClearDimensions(harqProcess(harqIDtoFlush).transportBlockSize,iCodeWord);
            removeObjectIndexFromPermutationSequence(harqProcess(harqIDtoFlush).existingCodeWordPermutaion,iCodeWord);
	  }
	}
	else {					
	  harqProcess(harqIDtoFlush).coderInfo.clear(codeWordIndex);
	  extendAndClearDimensions(harqProcess(harqIDtoFlush).channelCoderRxCircularBufferPerCodeBlock,codeWordIndex);
	  extendAndClearDimensions(harqProcess(harqIDtoFlush).transportBlockSize,codeWordIndex);
          removeObjectIndexFromPermutationSequence(harqProcess(harqIDtoFlush).existingCodeWordPermutaion,codeWordIndex);
	}
}

void ChannelCoder::flushAllHarqProcess()
{

  for(int harqIDCnt=0;harqIDCnt<harqProcess.length();harqIDCnt++)
  {
    flushHarqProcess(harqIDCnt);
  }
}


/*! \brief Function called \@ receiver to remove the CRC bits of segments(codeBlocks), if segmentation happened.. */
bvec Decoder::removeCRCBitsOfBlockSegments(BlockSegments &blockSegments, bvec &inBits, int numOfCRCBits){
        if(blockSegments.nCodesPlus==0 && blockSegments.nCodesMinus==0)
            return inBits;

        bvec outBits;
        int startIndex = 0;
        outBits.set_length(0);
        startIndex = blockSegments.nFillerBitsUsed;

        for(int iblockSegment=0; iblockSegment< blockSegments.nCodesPlus;iblockSegment++ ) {
          int nInfoBitsInCurrentSegment = (iblockSegment==0)? blockSegments.blockSizePlus-blockSegments.nFillerBitsUsed-numOfCRCBits :blockSegments.blockSizePlus-numOfCRCBits;
          append(outBits,inBits.mid(startIndex,nInfoBitsInCurrentSegment));
          startIndex += nInfoBitsInCurrentSegment+numOfCRCBits;
        }

        for(int iblockSegment=0; iblockSegment< blockSegments.nCodesMinus;iblockSegment++ ) {
          int nInfoBitsInCurrentSegment = blockSegments.blockSizeMinus-numOfCRCBits;
          append(outBits,inBits.mid(startIndex,nInfoBitsInCurrentSegment));
          startIndex += nInfoBitsInCurrentSegment+numOfCRCBits;
        }
        return outBits;
}

//! \brief Function to remove CRC bits of input bvec..
bvec Decoder::trimCRCBits(bvec &inBits, int numOfCRCBits) {
    return inBits(0,inBits.length()-numOfCRCBits-1);
}

TransmissionStatus_S SourceAndSink::demodulateAndDecode(EqualizerOutput_S &equalizerOutputForDataBlk, int serviceNodeID){

    int indx=find(serviceNodeIDs,serviceNodeID);
    if(indx!=-1)
    {
        /*if(channelDecoders(indx).coderType ==_CODERTYPE_TURBO_)
        {
            TransmissionStatus_S rxStatus = channelDecoders(indx).performChannelDecoding(equalizerOutputForDataBlk.equalizedSymbols,equalizerOutputForDataBlk.equalizedChannel);
            rxStatus.serviceNodeID=serviceNodeID;
            return rxStatus;
        }
        else */if(channelDecoders(indx).coderType ==_CODERTYPE_LDPC_)
        {      
            TransmissionStatus_S rxStatus = channelDecoders(indx).performLDPCChannelDecoding(equalizerOutputForDataBlk.equalizedSymbols,equalizerOutputForDataBlk.equalizedChannel);
            rxStatus.serviceNodeID=serviceNodeID;
            return rxStatus;
        }
        else
        {
            cout<<"CoderType not valid.."<<endl;abort();
        }  
    }
    else
    {
      cout<<"[both:] Error... Unknown serviceNodeID to demodulateAndDecode()..."<<endl;
      abort();
    }

  }
