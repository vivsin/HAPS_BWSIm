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

ChannelCoder::ChannelCoder() :harqProcess(0)  {coderType=_CODERTYPE_UNINIT_;};

void ChannelCoder::initChannelCoder(cTBTables* tbTable, int nRnti, CRCTYPE CRCType, _UE_CATEGORY_ ueClass, int maximumHarqProcesses, int maximumHarqTransmissions, bool isTurboCoderEnabled, bool isSubblockInterleaverEnabled, bool isScramblerEnabled, CoderType_E codertype)
{ 
  nRNTI = nRnti;
  ueModel = ueClass;
  nrCRC.setCRCTypeNR(CRCType); 
  currentHarqInfo.harqProcessID = -1;
  coderType=codertype;
  tbTables=tbTable;
  this->maximumHarqTransmissions = maximumHarqTransmissions;
  harqProcess.set_length(maximumHarqProcesses);
  for(int iharqProcess=0; iharqProcess<harqProcess.length(); iharqProcess++) {
    harqProcess(iharqProcess).isTurboCoderEnabled = isTurboCoderEnabled;
    harqProcess(iharqProcess).isSubblockInterleaverEnabled = isSubblockInterleaverEnabled;
    harqProcess(iharqProcess).isScramblerEnabled = isScramblerEnabled;    
  }
  flushAllHarqProcess();
}

void ChannelCoder::initTBConfig(DataGenInfo_S *dataGenInfo, bool isDownlink){
  
  tbConfig.isDownlink=isDownlink;
  tbConfig.cellID=dataGenInfo->cellID;
  tbConfig.nCodeWords = dataGenInfo->nCodeWords;
  tbConfig.maxHARQ = maximumHarqTransmissions;
  tbConfig.mimoRank = dataGenInfo->nLayers/(dataGenInfo->symbolRepetitionFactor); 	
  tbConfig.nREsAllocated = sum(dataGenInfo->resourceElementsPerRB);
  tbConfig.allocatedPRBs = dataGenInfo->scheduledPRBs;
  tbConfig.allocatedSymbols = dataGenInfo->scheduledSymbols;
  tbConfig.modAlphabet.set_size(tbConfig.nCodeWords);
  tbConfig.rvIndex = currentHarqInfo.rvIndicesPerCodeword;
  tbConfig.mcsIndices=dataGenInfo->mcsIndices;
  tbConfig.nCodeBlocksPerCodeWord.set_size(tbConfig.nCodeWords);
  tbConfig.nCodeBlocksPerCodeWord.clear();
  tbConfig.tbIndexPerCodeWord.set_size(tbConfig.nCodeWords);
  for(int iCodeWord=0; iCodeWord<currentHarqInfo.codeWordIndicesToConsider.length(); iCodeWord++)
  {
    int currentMCS=find(tbTables->transportBlockMCSMapping.mcsIndex,tbConfig.mcsIndices[iCodeWord]);
    if(currentMCS!=-1)
    {
      if(isDownlink)
      {
	tbConfig.modAlphabet(iCodeWord) = (MOD_ALPHABET_SIZE) tbTables->transportBlockMCSMapping.modulationAlphabetPDSCH(currentMCS);
	if(dataGenInfo->transmissionScheme == _TRANSMISSION_SCHEME_NR_CL_)
	  tbConfig.tbIndexPerCodeWord(iCodeWord) = currentMCS;
	else
	  tbConfig.tbIndexPerCodeWord(iCodeWord) = tbTables->transportBlockMCSMapping.transportBlockIndexPDSCH(currentMCS);
      }
      else
      {
	tbConfig.modAlphabet(iCodeWord) = (MOD_ALPHABET_SIZE) tbTables->transportBlockMCSMapping.modulationAlphabetPUSCH(currentMCS);
	if(dataGenInfo->transmissionScheme == _TRANSMISSION_SCHEME_NR_CL_)
	  tbConfig.tbIndexPerCodeWord(iCodeWord) = currentMCS;
	else
	  tbConfig.tbIndexPerCodeWord(iCodeWord) = tbTables->transportBlockMCSMapping.transportBlockIndexPUSCH(currentMCS);
      }
    }
    else
    {
      cout << "Unable to find the TB index in initTBConfig()..... " << endl;
      abort();
    }
  }
}


/*! \brief Function called \@ Transmitter to set currentHarqInfo.harqProcessID & initialize corresponding harqProcess */
void ChannelCoder::initializeHarqProcess(DataGenInfo_S *dataGenInfo,HarqInfo &currentHarqInfo_, int subframeNo, bool isDownlink)// may be configured for rx
{
  currentHarqInfo = currentHarqInfo_;
  harqProcess(currentHarqInfo.harqProcessID).isNewTransmission.set_length(currentHarqInfo.codeWordIndicesToConsider.length());
  
  if(!currentHarqInfo.codeWordIndicesToConsider.length()) currentHarqInfo.codeWordIndicesToConsider = getIntegers(0,currentHarqInfo.newDataIndicator.length()-1);
  
  permuteHarqProcessWrtCodeWords(harqProcess(currentHarqInfo.harqProcessID),currentHarqInfo.codeWordIndicesToConsider);
  
  for(int iCodeWord=0; iCodeWord<currentHarqInfo.codeWordIndicesToConsider.length(); iCodeWord++)
  {
    if(subframeNo == 0)
      harqProcess(currentHarqInfo.harqProcessID).isNewTransmission(iCodeWord) = 1;
    else
      // FIXME : NDI not used to determine isNewTransmission because of issue in Sidelink transmission - Dhiv      
      harqProcess(currentHarqInfo.harqProcessID).isNewTransmission(iCodeWord) =   currentHarqInfo.isNewTransmission(iCodeWord);    
            
            
    if(harqProcess(currentHarqInfo.harqProcessID).isNewTransmission(iCodeWord))
      flushHarqProcess(currentHarqInfo.harqProcessID,iCodeWord);
  }
  
  harqProcess(currentHarqInfo.harqProcessID).lastTxframe = subframeNo;
  harqProcess(currentHarqInfo.harqProcessID).newDataIndicator_old.set_subvector(0,currentHarqInfo.newDataIndicator);
  
  #ifdef _DEBUG_CHANNELCODER_ENABLE
  cout<<"\n*initializeHarqProcess for currentHarqInfo.harqProcessID ="<<currentHarqInfo.harqProcessID;
  #endif
  
  
  initTBConfig(dataGenInfo,isDownlink);
  
  currentHarqInfo_.infoBitsperCodeWord = getInfoBitsPerCodeWord(currentHarqInfo_.infoBitsperCodeWord);
  currentHarqInfo_.TBSizePerCodewordInBits=tbConfig.transportBlockSize;
  
}

/*! \brief Function called \@ Rx to set currentHarqInfo.harqProcessID & initialize corresponding harqProcess */
void Decoder::initDecoderHarqProcess(DataGenInfo_S *dataGenInfo,HarqInfo &currentHarqInfo_, int subframeNo, bool isDownlink)
{
        currentHarqInfo = currentHarqInfo_;
        harqProcess(currentHarqInfo.harqProcessID).isNewTransmission.set_length(currentHarqInfo.codeWordIndicesToConsider.length());

        if(!currentHarqInfo.codeWordIndicesToConsider.length()) currentHarqInfo.codeWordIndicesToConsider = getIntegers(0,currentHarqInfo.newDataIndicator.length()-1);

        permuteHarqProcessWrtCodeWords(harqProcess(currentHarqInfo.harqProcessID),currentHarqInfo.codeWordIndicesToConsider);

	for(int iCodeWord=0; iCodeWord<currentHarqInfo.codeWordIndicesToConsider.length(); iCodeWord++)
        {
            
//           harqProcess(currentHarqInfo.harqProcessID).isNewTransmission(iCodeWord) = currentHarqInfo.newDataIndicator(iCodeWord) ^ harqProcess(currentHarqInfo.harqProcessID).newDataIndicator_old(iCodeWord);
	  
            // FIXME : NDI not used to determine isNewTransmission because of issue in Sidelink transmission - Dhiv      
            harqProcess(currentHarqInfo.harqProcessID).isNewTransmission(iCodeWord) =   currentHarqInfo.isNewTransmission(iCodeWord);    
            
          if(harqProcess(currentHarqInfo.harqProcessID).isNewTransmission(iCodeWord))
	    flushHarqProcess(currentHarqInfo.harqProcessID,iCodeWord);
        }

	harqProcess(currentHarqInfo.harqProcessID).lastTxframe = subframeNo;

        harqProcess(currentHarqInfo.harqProcessID).newDataIndicator_old.set_subvector(0,currentHarqInfo.newDataIndicator);

	#ifdef _DEBUG_CHANNELCODER_ENABLE
	cout<<"\n*initializeHarqProcess for currentHarqInfo.harqProcessID ="<<currentHarqInfo.harqProcessID;
	#endif
	initTBConfig(dataGenInfo,isDownlink);
	decoderInitBits(currentHarqInfo_.infoBitsperCodeWord);
	currentHarqInfo_.TBSizePerCodewordInBits=tbConfig.transportBlockSize;	
}
/*! \brief Generate modulated Symbols(LTE) looking at no of PRBs & MCS index.
\param [in] subframeNo Needed for HARQ-Retransmission to keep \a minDelayBetweenSuccessiveTransmissions
\return Array<cvec>, each cvec corresponds to each CodeWord. length of each cvec is dataGenInfo-> scheduledPRBs*dataGenInfo->allocatedREsPerRB;
\note Here layers are generated independantly. Different MCS & turbo-coders are used.		*/
  Array<cvec>  Encoder::getModulatedSymbols(DataGenInfo_S *dataGenInfo,HarqInfo &currentHarqInfo_,int subframeNumber,bool isDownlink)
{
        verifyInputs(dataGenInfo,currentHarqInfo_,subframeNumber);
        initializeHarqProcess(dataGenInfo,currentHarqInfo_,subframeNumber,isDownlink);
    
// 	cout<<"node ID : "<<dataGenInfo->rnti<<endl;
    //Assuming CRC is common for both Turbo and LDPC
    int nCodeWords  = currentHarqInfo_.infoBitsperCodeWord.length();
    Array<bvec> transportBlockPerCodeWord(nCodeWords);
    for(int cwd_cnt=0;cwd_cnt<nCodeWords;cwd_cnt++)
        transportBlockPerCodeWord(cwd_cnt) = BoolVectortoBVec(nrCRC.performCRCEncodingNR(BVecToBoolVec(currentHarqInfo_.infoBitsperCodeWord(cwd_cnt))));
         
    #ifdef _DEBUG_CHANNELCODER_ENABLE
    cout<<"\nFn:getModulatedSymbols infoBitsForAllCodeWords ="<<currentHarqInfo_.infoBitsperCodeWord<<"\n length ="<<currentHarqInfo_.infoBitsperCodeWord.length()<<endl;
        #endif
            
   /* if(coderType==_CODERTYPE_TURBO_)
        return (performTurboCoding(transportBlockPerCodeWord,tbConfig));
    else */if(coderType==_CODERTYPE_LDPC_)
        return (performLDPCCoding(transportBlockPerCodeWord,tbConfig));
    else
    {
        cout<<"CoderType not valid.."<<endl;abort();
    }
        
}
 void Decoder::initDemodParameters(DataGenInfo_S *dataGenInfo,HarqInfo &currentHarqInfo_,int subframeNumber,bool isDownlink)
{
        verifyInputs(dataGenInfo,currentHarqInfo_,subframeNumber);
        initDecoderHarqProcess(dataGenInfo,currentHarqInfo_,subframeNumber,isDownlink);
	//TBlock initialized to zeros for emulating encoder so as to calculate the decoder parameters
	Array<bvec> transportBlockPerCodeWord;
	int crcParitylength = nrCRC.getNumParity(); 
	transportBlockPerCodeWord.set_length(tbConfig.nCodeWords,true);
	for(int cnt=0;cnt<tbConfig.nCodeWords;cnt++)
	transportBlockPerCodeWord(cnt)= zeros_b(tbConfig.transportBlockSize(cnt) + crcParitylength );
	#ifdef _DEBUG_CHANNELCODER_ENABLE
	cout<<"\nFn:getModulatedSymbols infoBitsForAllCodeWords ="<<currentHarqInfo_.infoBitsperCodeWord<<"\n length ="<<currentHarqInfo_.infoBitsperCodeWord.length()<<endl;
        #endif
	initDecoder(transportBlockPerCodeWord,tbConfig);
}

/*! \brief Just abort the simulation if inputs to getModulatedSymbols function is not proper... */
void ChannelCoder::verifyInputs(DataGenInfo_S *dataGenInfo, HarqInfo currentHarqInfo_,int /*subframeNumber*/)
{
	(void) currentHarqInfo_; //WarningFix: unused
        if(!dataGenInfo->isReady()) { cout<<"[both:] DatagenInfo to channelCoder is incomplete..."<<endl;abort();}
        //! Verify the input currentHarqInfo_
        assert(currentHarqInfo_.newDataIndicator.length()==dataGenInfo->nCodeWords  && "CodeWords specified in dataGenInfo & currentHarqInfo_.newDataIndicator not matching!!!");
        assert(currentHarqInfo_.rvIndicesPerCodeword.length()==dataGenInfo->nCodeWords && "CodeWords specified in dataGenInfo & currentHarqInfo_.rvIndicesPerCodeword not matching!!!");
        assert((currentHarqInfo_.codeWordIndicesToConsider.length()==0 || currentHarqInfo_.codeWordIndicesToConsider.length()==dataGenInfo->nCodeWords) && "CodeWords specified in dataGenInfo & currentHarqInfo_.codeWordIndicesToConsider not matching!!!");
	assert((currentHarqInfo_.codeBlockIndicesToConsider.length()==0 || currentHarqInfo_.codeBlockIndicesToConsider.length()==dataGenInfo->nCodeWords) && "CodeWords specified in dataGenInfo & currentHarqInfo_.codeBlockIndicesToConsider not matching!!!");
}



/*! \brief Returns the modulated Symbols.
\param [in] transportBlockPerCodeWordWithCRC \a Array<bvec> each bvec corresponds to DataBits for each Layer
\param codeBlockIndex is segment Index (after segmentation)
\paragraph Steps for channel coding
1. Do the segmentations.
2. Attach CRC to each segment
3. Feed the segments to Trubo-encoder which will output 3 blocks(\a Array<bvec> turboEncoderOutput).
4. Do the Sub-Block interleaving of each of the block output of Turbo-encoder
5. Concatenate the outputs of Sub-block interleaver to form circular-buffer(\a bvec subBlockInterleaverOutputInfo->circularBuffer)
6. Compute the number of Actual bits(\c EoutBits) that can be put into PRBs, & pick those many bits from circular-buffer
                */
// Array<cvec> Encoder::performTurboCoding(Array<bvec> &transportBlockPerCodeWordWithCRC,TBConfig &tbConfig)
// {
// 	bvec tempBvec;
// 	ivec tempIvec;
// 	Array<bvec> turboEncoderOutput;
// 	int codeBlockIndex,iCodeWord,nFillers;
// 	TurboCoderLTE turboCoder;
//         turboCoder.setLTETables((cLTETables*)tbTables);
// 	turboEncoderOutput.set_size(0);
// 	harqProcess(currentHarqInfo.harqProcessID).coderInfo.puncturedLocations.set_size(0);
// 	for (iCodeWord = 0;iCodeWord < currentHarqInfo.codeWordIndicesToConsider.length();iCodeWord ++)
// 	{
// 	  //! For New Transmission...
// 	  if(harqProcess(currentHarqInfo.harqProcessID).isNewTransmission(iCodeWord) == true)
// 	  {	  
// 	    extendAndClearDimensions(harqProcess(currentHarqInfo.harqProcessID).coderInfo.scramblerSeq,iCodeWord);
// 	    harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.init(iCodeWord);
// 	    extendAndClearDimensions(harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer,iCodeWord);
// 	    harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer(iCodeWord).set_size(0);
// 	    performCodeBlockSegmentation(transportBlockPerCodeWordWithCRC(iCodeWord),harqProcess(currentHarqInfo.harqProcessID).coderInfo,iCodeWord);	    
// 	    harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.nCodeBlocksPerCodeWord[iCodeWord] = tbConfig.nCodeBlocksPerCodeWord[iCodeWord];
// 	    harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.codeBlockSizes(iCodeWord).set_size(harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.nCodeBlocksPerCodeWord[iCodeWord]);
// 	    harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.ratematchedOutputSize(iCodeWord).set_size(harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.nCodeBlocksPerCodeWord[iCodeWord]);
// 	    harqProcess(currentHarqInfo.harqProcessID).coderInfo.subBlockInterleaverOutputInfo(iCodeWord).set_size(tbConfig.nCodeBlocksPerCodeWord[iCodeWord]);
// 	    for (codeBlockIndex = 0;codeBlockIndex < tbConfig.nCodeBlocksPerCodeWord[iCodeWord];codeBlockIndex ++)
// 	    {
// 	      nFillers = 0;
// 	      #ifdef _DEBUG_CHANNELCODER_ENABLE
// 	      cout<<"\nFn:performChannelDecoding Before Turbo-encoding. codeBlockIndex ="<<codeBlockIndex<<"\n bitsPerCodewordPerCodeblock(iCodeWord) length ="
// 	      <<length(harqProcess(currentHarqInfo.harqProcessID).coderInfo.bitsPerCodewordPerCodeblock(iCodeWord)(codeBlockIndex))<<endl;
// 	      #endif
// 	      if(harqProcess(currentHarqInfo.harqProcessID).isTurboCoderEnabled)
// 		turboEncoderOutput = turboCoder.encode(harqProcess(currentHarqInfo.harqProcessID).coderInfo.bitsPerCodewordPerCodeblock(iCodeWord)(codeBlockIndex));
// 	      else {
// 		turboEncoderOutput.set_size(1);
// 		turboEncoderOutput(0) = harqProcess(currentHarqInfo.harqProcessID).coderInfo.bitsPerCodewordPerCodeblock(iCodeWord)(codeBlockIndex);
// 	      }
// 
// 	      #ifdef _DEBUG_CHANNELCODER_ENABLE
// 	      cout<<" After Turbo-encoding turboEncoderOutput size ="<<turboEncoderOutput.size()<<" length ="<<length(turboEncoderOutput(0))<<endl;
// 	      #endif
// 
// 	      if (!codeBlockIndex) nFillers = harqProcess(currentHarqInfo.harqProcessID).coderInfo.blockSegments(iCodeWord).nFillerBitsUsed;
// 	      harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.codeBlockSizes(iCodeWord)[codeBlockIndex] = harqProcess(currentHarqInfo.harqProcessID).coderInfo.bitsPerCodewordPerCodeblock(iCodeWord)(codeBlockIndex).size();
// 	    
// 	      harqProcess(currentHarqInfo.harqProcessID).coderInfo.subBlockInterleaverOutputInfo(iCodeWord)(codeBlockIndex) = rMatcher.performSubBlockInterleaving(turboEncoderOutput,nFillers,harqProcess(currentHarqInfo.harqProcessID).isSubblockInterleaverEnabled);
// 	      tempBvec = rMatcher.performRateMatching(harqProcess(currentHarqInfo.harqProcessID).coderInfo.subBlockInterleaverOutputInfo(iCodeWord)(codeBlockIndex),tbConfig,ueModel,codeBlockIndex,iCodeWord,tempIvec);
// 	      append(harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer(iCodeWord),tempBvec);
// 	      append(harqProcess(currentHarqInfo.harqProcessID).coderInfo.puncturedLocations,tempIvec);
// 	      harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.ratematchedOutputSize(iCodeWord)[codeBlockIndex] = tempIvec.length();
// 	    }
// 	  }
// 	  //! For Re-transmission, only pick the bits from backed-up channelCoderTxCircularBuffer
// 	  else
// 	  {
// 	    tbConfig.nCodeBlocksPerCodeWord[iCodeWord]=harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.nCodeBlocksPerCodeWord[iCodeWord];
// 	    
// // 	    ivec codeBlockIndicesForReTx;	    
// // 	    codeBlockIndicesForReTx=currentHarqInfo.codeBlockIndicesToConsider(iCodeWord);
// // 	 
// 	    harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer(iCodeWord).set_size(0);
// 	    
// 	    for (codeBlockIndex = 0;codeBlockIndex < tbConfig.nCodeBlocksPerCodeWord[iCodeWord];codeBlockIndex ++)
// 	      harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.ratematchedOutputSize(iCodeWord)[codeBlockIndex] = 0;
// 	    
// 	    for (codeBlockIndex = 0;codeBlockIndex < tbConfig.nCodeBlocksPerCodeWord[iCodeWord];codeBlockIndex ++)
// 	    { 
// // 	      codeBlockIndex=codeBlockIndicesForReTx(count);
// 	      tempBvec = rMatcher.performRateMatching(harqProcess(currentHarqInfo.harqProcessID).coderInfo.subBlockInterleaverOutputInfo(iCodeWord)(codeBlockIndex),tbConfig,ueModel,codeBlockIndex,iCodeWord,tempIvec);
// 	      append(harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer(iCodeWord),tempBvec);
// 	      append(harqProcess(currentHarqInfo.harqProcessID).coderInfo.puncturedLocations,tempIvec);
// 	      harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.ratematchedOutputSize(iCodeWord)[codeBlockIndex] = tempIvec.length();
// 	    }
// 	  }
// 
// 	}
// 	if(harqProcess(currentHarqInfo.harqProcessID).isScramblerEnabled)
// 	  performScrambling(harqProcess(currentHarqInfo.harqProcessID).coderInfo);
//     
//     	return(performModulation(harqProcess(currentHarqInfo.harqProcessID).coderInfo));
// }




Array<cvec> Encoder::performLDPCCoding(Array<bvec> &transportBlockPerCodeWordWithCRC,TBConfig &tbConfig)
{
    bvec tempBvec;
    ivec tempIvec;
    int iCodeWord;
    harqProcess(currentHarqInfo.harqProcessID).coderInfo.puncturedLocations.set_size(0);
     int nCodeWords = currentHarqInfo.codeWordIndicesToConsider.length();
    std::vector<int> E_vec(nCodeWords); 
	for (iCodeWord = 0;iCodeWord < nCodeWords;iCodeWord ++)
    {
      //! For New Transmission...
	  
	  
	 if(currentHarqInfo.codeWordIndicesToConsider.length()>harqProcess(currentHarqInfo.harqProcessID).coderInfo.encoderOutputPerCodeBlockSTD.length()) 
	 {
	   harqProcess(currentHarqInfo.harqProcessID).coderInfo.encoderOutputPerCodeBlockSTD.set_size(currentHarqInfo.codeWordIndicesToConsider.length(),true);
	   harqProcess(currentHarqInfo.harqProcessID).coderInfo.zPerCodeBlockPerCodeWord.set_size(currentHarqInfo.codeWordIndicesToConsider.length(),true);
	    harqProcess(currentHarqInfo.harqProcessID).coderInfo.BGperCodeWord.resize(currentHarqInfo.codeWordIndicesToConsider.length());
	 }
	 
	 if(harqProcess(currentHarqInfo.harqProcessID).isNewTransmission(iCodeWord) == true)
	  {
	    extendAndClearDimensions(harqProcess(currentHarqInfo.harqProcessID).coderInfo.scramblerSeq,iCodeWord);
	    harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.init(iCodeWord);
	    extendAndClearDimensions(harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer,iCodeWord);
         
	    
	    
	    
	    std::vector<std::vector<bool>> dV;
	    int Q, rvid, BG;
	    double codeRate;
	auto NL = tbConfig.numLayersPerCodeWord(iCodeWord); 
	
	rvid = tbConfig.rvIndex(iCodeWord);
	Q = (int)tbConfig.modAlphabet(iCodeWord);
	E_vec[iCodeWord] = tbConfig.nREsAllocated* NL*Q ;
	
// 	cout<<"tbConfig.nREsAllocated : "<<tbConfig.nREsAllocated<<endl;
	
        codeRate = (double)tbConfig.transportBlockSize(iCodeWord)/(double)E_vec[iCodeWord];
	if(codeRate>0.92578125)
	{
		cout<<"TB size: "<<tbConfig.transportBlockSize(iCodeWord)<<endl;
	 cout<<"NL: "<<NL<<" Q: "<<Q;
	 cout<<" nREs: "<<tbConfig.nREsAllocated<<endl;
	 cout<<"Evec: "<<E_vec[iCodeWord]<<endl;
	 cout<<"Coderate(Tbsize/Evec): "<<codeRate<<endl;
	 cout << "Abnormal codeRate!! .. aborting !!"<<endl;abort();
	}
	E_vec[iCodeWord] = E_vec[iCodeWord]/Q; // No. of output symbols (not bits) allocated per codeword, later these symbols are divided for codeblocks
	if(tbConfig.transportBlockSize(iCodeWord)<=292 || (tbConfig.transportBlockSize(iCodeWord)<=3824 && codeRate<=0.67) || codeRate<= 0.25) 
	    BG=2;
	else
	    BG=1;
	
	harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.nCodeBlocksPerCodeWord[iCodeWord] = tbConfig.nCodeBlocksPerCodeWord[iCodeWord];
        harqProcess(currentHarqInfo.harqProcessID).coderInfo.BGperCodeWord[iCodeWord]=BG;
	harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer(iCodeWord)=BoolVectortoBVec(dataChannelEncoder(BVecToBoolVec(transportBlockPerCodeWordWithCRC(iCodeWord)),E_vec[iCodeWord], Q, rvid, BG , dV, harqProcess(currentHarqInfo.harqProcessID).coderInfo.zPerCodeBlockPerCodeWord(iCodeWord)));
	
	harqProcess(currentHarqInfo.harqProcessID).coderInfo.encoderOutputPerCodeBlockSTD(iCodeWord)=dV;
      }
      //! For Re-transmission, only pick the bits from backed-up channelCoderTxCircularBuffer
      else
      {
	    tbConfig.nCodeBlocksPerCodeWord[iCodeWord]=harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.nCodeBlocksPerCodeWord[iCodeWord];
	    
	harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer(iCodeWord).set_size(0);
        if(harqProcess(currentHarqInfo.harqProcessID).coderInfo.encoderOutputPerCodeBlockSTD(iCodeWord).size()==0)
	{ 
	  cout<<"Previous transmitted data was not stored properly ... aborting"<<endl;
	  abort();
	}
	std::vector<std::vector<bool>> dV= harqProcess(currentHarqInfo.harqProcessID).coderInfo.encoderOutputPerCodeBlockSTD(iCodeWord);
	if(dV.size()==0)
	{
	  cout<<"Previous transmitted data was not stored properly ... aborting"<<endl;
	  abort();
	}
	int Q, rvid, BG;
	BG = harqProcess(currentHarqInfo.harqProcessID).coderInfo.BGperCodeWord[iCodeWord];
	if(BG<1 || BG>2)
	{
	  cout<<"Wrong BG choosen!!   aborting"<<endl;
	  abort();
	}
	auto NL = tbConfig.numLayersPerCodeWord(iCodeWord); 
	
	E_vec[iCodeWord] = tbConfig.nREsAllocated* NL ;  // No. of output symbols (not bits) allocated per codeword, later these symbols are divided for codeblocks

	rvid = tbConfig.rvIndex(iCodeWord);
	Q = (int)tbConfig.modAlphabet(iCodeWord);
	
	bvec ldpcOutput=BoolVectortoBVec(dataChannelEncoderRetransmit( dV, E_vec[iCodeWord], Q, rvid, BG,harqProcess(currentHarqInfo.harqProcessID).coderInfo.zPerCodeBlockPerCodeWord(iCodeWord)));
	harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer(iCodeWord)=ldpcOutput;
      }
  
    }
    
    if(harqProcess(currentHarqInfo.harqProcessID).isScramblerEnabled)
      performScrambling(harqProcess(currentHarqInfo.harqProcessID).coderInfo);

    return(performModulation(harqProcess(currentHarqInfo.harqProcessID).coderInfo));
}

Array<cvec> Decoder::initDecoder(Array<bvec> &transportBlockPerCodeWordWithCRC,TBConfig &tbConfig)
{
	bvec tempBvec;
	ivec tempIvec;
// 	Array<bvec> turboEncoderOutput;
	int codeBlockIndex,iCodeWord,nFillers;
// if(coderType==_CODERTYPE_TURBO_)	  
// {
// 	TurboCoderLTE turboCoder;
// 	turboCoder.setLTETables((cLTETables*)tbTables);
// 	turboEncoderOutput.set_size(0);
// 	harqProcess(currentHarqInfo.harqProcessID).coderInfo.puncturedLocations.set_size(0);
// 	for (iCodeWord = 0;iCodeWord < currentHarqInfo.codeWordIndicesToConsider.length();iCodeWord ++)
// 	{
// 	  
// 	  //! For New Transmission...
// 	  if(harqProcess(currentHarqInfo.harqProcessID).isNewTransmission(iCodeWord) == true)
// 	  {	  
// 	    extendAndClearDimensions(harqProcess(currentHarqInfo.harqProcessID).coderInfo.scramblerSeq,iCodeWord);
// 	    harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.init(iCodeWord);
// 	    extendAndClearDimensions(harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer,iCodeWord);
// 	    harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer(iCodeWord).set_size(0);
// 	    
// 	    if(coderType==_CODERTYPE_TURBO_)
// 	      performCodeBlockSegmentation(transportBlockPerCodeWordWithCRC(iCodeWord),harqProcess(currentHarqInfo.harqProcessID).coderInfo,iCodeWord);	    
// 	    
// 	    harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.nCodeBlocksPerCodeWord[iCodeWord] = tbConfig.nCodeBlocksPerCodeWord[iCodeWord];
// 	    harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.codeBlockSizes(iCodeWord).set_size(harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.nCodeBlocksPerCodeWord[iCodeWord]);
// 	    harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.ratematchedOutputSize(iCodeWord).set_size(harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.nCodeBlocksPerCodeWord[iCodeWord]);
// 	    harqProcess(currentHarqInfo.harqProcessID).coderInfo.subBlockInterleaverOutputInfo(iCodeWord).set_size(tbConfig.nCodeBlocksPerCodeWord[iCodeWord]);
// 	    for (codeBlockIndex = 0;codeBlockIndex < tbConfig.nCodeBlocksPerCodeWord[iCodeWord];codeBlockIndex ++)
// 	    {
// 	      nFillers = 0;
// 	      #ifdef _DEBUG_CHANNELCODER_ENABLE
// 	      cout<<"\nFn:performChannelDecoding Before Turbo-encoding. codeBlockIndex ="<<codeBlockIndex<<"\n bitsPerCodewordPerCodeblock(iCodeWord) length ="
// 	      <<length(harqProcess(currentHarqInfo.harqProcessID).coderInfo.bitsPerCodewordPerCodeblock(iCodeWord)(codeBlockIndex))<<endl;
// 	      #endif
// 	      int turboencoderBitsLen=harqProcess(currentHarqInfo.harqProcessID).coderInfo.bitsPerCodewordPerCodeblock(iCodeWord)(codeBlockIndex).length();
// 	      if(harqProcess(currentHarqInfo.harqProcessID).isTurboCoderEnabled)
// 	      { turboencoderBitsLen+=4;
// 		turboEncoderOutput.set_size(3);
// 		turboEncoderOutput(0) = zeros_b(turboencoderBitsLen);
// 		turboEncoderOutput(1) = zeros_b(turboencoderBitsLen);
// 		turboEncoderOutput(2) = zeros_b(turboencoderBitsLen);
// 	      }
// 	      else {
// 		turboEncoderOutput.set_size(1);
// 		turboEncoderOutput(0) = zeros_b(turboencoderBitsLen);
// 		    }
// 	      #ifdef _DEBUG_CHANNELCODER_ENABLE
// 	      cout<<" After Turbo-encoding turboEncoderOutput size ="<<turboEncoderOutput.size()<<" length ="<<length(turboEncoderOutput(0))<<endl;
// 	      #endif
// 
// 	      if (!codeBlockIndex) nFillers = harqProcess(currentHarqInfo.harqProcessID).coderInfo.blockSegments(iCodeWord).nFillerBitsUsed;
// 	      harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.codeBlockSizes(iCodeWord)[codeBlockIndex] = harqProcess(currentHarqInfo.harqProcessID).coderInfo.bitsPerCodewordPerCodeblock(iCodeWord)(codeBlockIndex).size();
// 	      harqProcess(currentHarqInfo.harqProcessID).coderInfo.subBlockInterleaverOutputInfo(iCodeWord)(codeBlockIndex) = rMatcher.performSubBlockInterleaving(turboEncoderOutput,nFillers,harqProcess(currentHarqInfo.harqProcessID).isSubblockInterleaverEnabled);
// 	      tempBvec = rMatcher.performRateMatching(harqProcess(currentHarqInfo.harqProcessID).coderInfo.subBlockInterleaverOutputInfo(iCodeWord)(codeBlockIndex),tbConfig,ueModel,codeBlockIndex,iCodeWord,tempIvec);
// 	      append(harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer(iCodeWord),tempBvec);
// 	      append(harqProcess(currentHarqInfo.harqProcessID).coderInfo.puncturedLocations,tempIvec);
// 	      harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.ratematchedOutputSize(iCodeWord)[codeBlockIndex] = tempIvec.length();
// 	    }
// 	  }
// 	    //! For Re-transmission, only pick the bits from backed-up channelCoderTxCircularBuffer
// 	  else
// 	  {
// 	    tbConfig.nCodeBlocksPerCodeWord[iCodeWord]=harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.nCodeBlocksPerCodeWord[iCodeWord];
// 	    
// // 	    ivec codeBlockIndicesForReTx;	    
// // 	    codeBlockIndicesForReTx=currentHarqInfo.codeBlockIndicesToConsider(iCodeWord);
// // 	    
// 	    harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer(iCodeWord).set_size(0);
// 	    
// 	     for (codeBlockIndex = 0;codeBlockIndex < tbConfig.nCodeBlocksPerCodeWord[iCodeWord];codeBlockIndex ++)
// 	       harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.ratematchedOutputSize(iCodeWord)[codeBlockIndex] = 0;
// 	    
// 	     for (codeBlockIndex = 0;codeBlockIndex < tbConfig.nCodeBlocksPerCodeWord[iCodeWord];codeBlockIndex ++)
// 	     { 
// // 	      codeBlockIndex=codeBlockIndicesForReTx(count);
// 	      tempBvec = rMatcher.performRateMatching(harqProcess(currentHarqInfo.harqProcessID).coderInfo.subBlockInterleaverOutputInfo(iCodeWord)(codeBlockIndex),tbConfig,ueModel,codeBlockIndex,iCodeWord,tempIvec);
// 	      append(harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer(iCodeWord),tempBvec);
// 	      append(harqProcess(currentHarqInfo.harqProcessID).coderInfo.puncturedLocations,tempIvec);
// 	      harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.ratematchedOutputSize(iCodeWord)[codeBlockIndex] = tempIvec.length();
// 	    }
// 	  }
// 
// 	}
// }
// else
{
       harqProcess(currentHarqInfo.harqProcessID).coderInfo.puncturedLocations.set_size(0);
      int nCodeWords = currentHarqInfo.codeWordIndicesToConsider.length();
       std::vector<int> E_vec(nCodeWords); 
       //     std::cout << "No of CodeWords: " << nCodeWords<< std::endl;
       for (iCodeWord = 0;iCodeWord < currentHarqInfo.codeWordIndicesToConsider.length();iCodeWord ++)
       {
	  if(currentHarqInfo.codeWordIndicesToConsider.length()>harqProcess(currentHarqInfo.harqProcessID).coderInfo.encoderOutputPerCodeBlockSTD.length()) 
	  {
	    harqProcess(currentHarqInfo.harqProcessID).coderInfo.encoderOutputPerCodeBlockSTD.set_size(currentHarqInfo.codeWordIndicesToConsider.length(),true);
	    harqProcess(currentHarqInfo.harqProcessID).coderInfo.zPerCodeBlockPerCodeWord.set_size(currentHarqInfo.codeWordIndicesToConsider.length(),true);
	      harqProcess(currentHarqInfo.harqProcessID).coderInfo.BGperCodeWord.resize(currentHarqInfo.codeWordIndicesToConsider.length());
	  }
	
	 //! For New Transmission...
	 if(harqProcess(currentHarqInfo.harqProcessID).isNewTransmission(iCodeWord) == true)
	 {	  
	   extendAndClearDimensions(harqProcess(currentHarqInfo.harqProcessID).coderInfo.scramblerSeq,iCodeWord);
	   harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.init(iCodeWord);
	   extendAndClearDimensions(harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer,iCodeWord);
	   
	   //         extendAndClearDimensions(harqProcess(currentHarqInfo.harqProcessID).coderInfo.encoderOutputPerCodeBlock,iCodeWord);
	   
	   
	   
	   //         harqProcess(currentHarqInfo.harqProcessID).coderInfo.encoderOutputPerCodeBlock(iCodeWord).set_size(0);
	   
	   
	   
	   std::vector<std::vector<bool>> dV;
	   int Q, rvid, BG;
	   double codeRate;
	   
	   
	   // //        totalE = tbConfig.nREsAllocated*(int)tbConfig.modAlphabet(iCodeWord);
	   //         totalRE = tbConfig.nREsAllocated*tbConfig.numLayersPerCodeWord(iCodeWord);
	   // //         std::cout<< "totalRE: " << totalRE <<std::endl;
	   // //         std::cout<< "Modulation alphabet Q:  " << (int)tbConfig.modAlphabet(iCodeWord) <<std::endl;
	   //         auto rem = totalRE%nCodeWords;
	   //         for(auto i =0; i<nCodeWords; i++)
	   //         {
	   //             if(i<rem)
	   //                 E_vec[i] = (totalRE/nCodeWords)+1;
	   //             else
	   //                E_vec[i] = totalRE/nCodeWords;  // Integer division resuts the floored integer
	   //         }
	   //         E_vec[iCodeWord] = E_vec[iCodeWord]*(int)tbConfig.modAlphabet(iCodeWord);
	   
	   
	   auto NL = tbConfig.numLayersPerCodeWord(iCodeWord); 
	   
	   
	   rvid = tbConfig.rvIndex(iCodeWord);
	   Q = (int)tbConfig.modAlphabet(iCodeWord);
	   
	   E_vec[iCodeWord] = tbConfig.nREsAllocated* NL*Q ;
	   
	   codeRate = (double)tbConfig.transportBlockSize(iCodeWord)/(double)E_vec[iCodeWord];
       if(codeRate>0.92578125)
       {
         cout << "Abnormal codeRate!! .. aborting !!"<<endl;abort();
       }
	   E_vec[iCodeWord] = E_vec[iCodeWord]/Q; // No. of output symbols (not bits) allocated per codeword, later these symbols are divided for codeblocks
	   if(tbConfig.transportBlockSize(iCodeWord)<=292 || (tbConfig.transportBlockSize(iCodeWord)<=3824 && codeRate<=0.67) || codeRate<= 0.25) 
	     BG=2;
	   else
	     BG=1;
	   
	   tbConfig.nCodeBlocksPerCodeWord[iCodeWord] = getNumberOfCodeblocks(transportBlockPerCodeWordWithCRC(iCodeWord).size(),BG);
	   harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.nCodeBlocksPerCodeWord[iCodeWord] = tbConfig.nCodeBlocksPerCodeWord[iCodeWord];
	   
	   //         std::cout << "TB Size( "<< iCodeWord << " ) is: "<< tbConfig.transportBlockSize(iCodeWord)<<std::endl;
	   //         std::cout << "BG for codeword ( "<< iCodeWord << " ) is: "<< BG<<std::endl; 
	   //         std::cout<< "E_vec size: " << E_vec[iCodeWord] <<std::endl;
	   harqProcess(currentHarqInfo.harqProcessID).coderInfo.BGperCodeWord[iCodeWord]=BG;
	   //debug
	   //         cout<< "TX Harq ID: "<< currentHarqInfo.harqProcessID <<endl;
	   //debug
	   harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer(iCodeWord)=BoolVectortoBVec(dataChannelEncoder(BVecToBoolVec(transportBlockPerCodeWordWithCRC(iCodeWord)),E_vec[iCodeWord], Q, rvid, BG , dV, harqProcess(currentHarqInfo.harqProcessID).coderInfo.zPerCodeBlockPerCodeWord(iCodeWord)));
	   
	   //debug
	   //         std::cout << "....................ENCODED BITS: " <<std::endl;  
	   //         std::cout << harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer(iCodeWord)(0,30) << endl;
	   //debug
	   //         harqProcess(currentHarqInfo.harqProcessID).coderInfo.encoderOutputPerCodeBlock(iCodeWord)=stdVecVecboolToArrayBvec(dV);
	   
	   harqProcess(currentHarqInfo.harqProcessID).coderInfo.encoderOutputPerCodeBlockSTD(iCodeWord)=dV;
	 }
	 //! For Re-transmission, only pick the bits from backed-up channelCoderTxCircularBuffer
	 else
	 {
	   tbConfig.nCodeBlocksPerCodeWord[iCodeWord]=harqProcess(currentHarqInfo.harqProcessID).coderInfo.layerCBW.nCodeBlocksPerCodeWord[iCodeWord];
	   // 	    tbConfig.nCodeBlocksPerCodeWord[iCodeWord]=harqProcess(currentHarqInfo.harqProcessID).coderInfo.encoderOutputPerCodeBlock(iCodeWord).length();
	   harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer(iCodeWord).set_size(0);
	   
	   std::vector<std::vector<bool>> dV= harqProcess(currentHarqInfo.harqProcessID).coderInfo.encoderOutputPerCodeBlockSTD(iCodeWord);
	   int Q, rvid, BG;
	   BG = harqProcess(currentHarqInfo.harqProcessID).coderInfo.BGperCodeWord[iCodeWord];
	   
	   //         totalRE = tbConfig.nREsAllocated*(int)tbConfig.modAlphabet(iCodeWord);
	   //         totalRE = tbConfig.nREsAllocated*tbConfig.numLayersPerCodeWord(iCodeWord);
	   //         auto rem = totalRE%nCodeWords;
	   //         for(auto i =0; i<nCodeWords; i++)
	   //         {
	   //             if(i<rem)
	   //                 E_vec[i] = ((totalRE/nCodeWords)+1)*(int)tbConfig.modAlphabet(iCodeWord);  
	   //             else
	   //                E_vec[i] = (totalRE/nCodeWords)*(int)tbConfig.modAlphabet(iCodeWord);  // Integer division resuts the floored integer
	   //         }
	   auto NL = tbConfig.numLayersPerCodeWord(iCodeWord); 
	   
	   E_vec[iCodeWord] = tbConfig.nREsAllocated* NL ;  // No. of output symbols (not bits) allocated per codeword, later these symbols are divided for codeblocks
	   
	   rvid = tbConfig.rvIndex(iCodeWord);
	   rvid=0; // Hard coding
	   Q = (int)tbConfig.modAlphabet(iCodeWord);
	   
	   bvec ldpcOutput=BoolVectortoBVec(dataChannelEncoderRetransmit( dV, E_vec[iCodeWord], Q, rvid, BG, harqProcess(currentHarqInfo.harqProcessID).coderInfo.zPerCodeBlockPerCodeWord(iCodeWord)));
	   
	   
	   harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer(iCodeWord)=ldpcOutput;
	 }
	 
       }
}
       
       if(harqProcess(currentHarqInfo.harqProcessID).isScramblerEnabled)
       {
	double Cinit;
	Cinit = nRNTI * pow(2.0,14.0) + (tbConfig.mimoRank - 1.0) * pow(2.0,13.0) + floor(1 / 2.0) * pow(2.0,9.0) + tbConfig.cellID;
	for (int iCodeWord = 0;iCodeWord < currentHarqInfo.codeWordIndicesToConsider.length();iCodeWord ++)
	{
	  harqProcess(currentHarqInfo.harqProcessID).coderInfo.scramblerSeq(iCodeWord) = generateGoldSequence(Cinit,harqProcess(currentHarqInfo.harqProcessID).coderInfo.channelCoderTxCircularBuffer(iCodeWord).length()); 
	}
       }
       return 0;
}

// void ChannelCoder::performCodeBlockSegmentation(bvec &inBits,CoderInfo_S &coderInfo,int currentCodeWord)
// {
//       coderInfo.bitsPerCodewordPerCodeblock.set_size(currentCodeWord+1,true);
//       
//       if(!harqProcess(currentHarqInfo.harqProcessID).isTurboCoderEnabled) {
// 	coderInfo.bitsPerCodewordPerCodeblock(currentCodeWord).set_size(1);
// 	coderInfo.bitsPerCodewordPerCodeblock(currentCodeWord)(0) = inBits;
// 	coderInfo.blockSegments(currentCodeWord).nCodesPlus = 1;
// 	coderInfo.blockSegments(currentCodeWord).nCodesMinus = 0;
// 	coderInfo.blockSegments(currentCodeWord).blockSizePlus = inBits.length();
// 	coderInfo.blockSegments(currentCodeWord).nFillerBitsUsed = 0;
// 	coderInfo.blockSegments(currentCodeWord).blockSizeMinus = 0;
// 	tbConfig.nCodeBlocksPerCodeWord[currentCodeWord] = coderInfo.blockSegments(currentCodeWord).nCodesMinus + coderInfo.blockSegments(currentCodeWord).nCodesPlus;
// 	return;
//       }
//   
// 	bvec tempBvec,rInbits;
// 	int maxSize,minSize,iBlock,bitIndex,blockSize,runIndex;
// 	double nFillerBits,nCodes_,nCodes,nSegmentSize,nSegmentSize_,deltaSegmentSize,nBitsEffective,nBits,crcLength;
// 
// 	nBits = inBits.length();
// 	maxSize = 6144;minSize = 40;
// 	(void) minSize; //WarningFix: unused
// 	if (nBits <= maxSize)
// 	{
// 		crcLength = 0;
// 		nCodes = 1;
// 		nBitsEffective = nBits;
// 	}
// 	else
// 	{
// 		crcLength = 24;
// 		nCodes = ceil((double) nBits / (double)(maxSize - crcLength));
// 		nBitsEffective = nBits + nCodes * crcLength;
// 	}
// 
// 	for (iBlock = 0;iBlock < ((cLTETables*)tbTables)->interleaverParameters.K.length();iBlock ++)
// 	  if (nCodes * ((cLTETables*)tbTables)->interleaverParameters.K(iBlock) >= nBitsEffective)
// 			break;
// 
// 	  nSegmentSize = ((cLTETables*)tbTables)->interleaverParameters.K(iBlock);
// 
// 	if (nCodes == 1)
// 	{
// 		nCodes_ = 0;
// 		nSegmentSize_ = 0;
// 	}
// 	else
// 	{
// 	  for (iBlock = ((cLTETables*)tbTables)->interleaverParameters.K.length() - 1;iBlock >= 0;iBlock --)
// 	    if (nSegmentSize > ((cLTETables*)tbTables)->interleaverParameters.K(iBlock))
// 				break;
// 
// 		  nSegmentSize_ = ((cLTETables*)tbTables)->interleaverParameters.K(iBlock);
// 		deltaSegmentSize = nSegmentSize - nSegmentSize_;
// 		nCodes_ = floor((nCodes * nSegmentSize - nBitsEffective) / deltaSegmentSize);
// 
// 		nCodes = nCodes - nCodes_;
// 	}
// 
// 	nFillerBits = nCodes * nSegmentSize + nCodes_ * nSegmentSize_ - nBitsEffective;
// 
// 	runIndex = 0;
// 	rInbits = zeros_b(nFillerBits);
// 	append(rInbits,inBits);
// 	coderInfo.bitsPerCodewordPerCodeblock(currentCodeWord).set_size(nCodes + nCodes_);
// 	for (iBlock = 0;iBlock < coderInfo.bitsPerCodewordPerCodeblock(currentCodeWord).size();iBlock ++)
// 	{
// 		if (iBlock < nCodes_)
// 		{
// 			blockSize = nSegmentSize_ - crcLength;
// 		}
// 		else
// 		{
// 			blockSize = nSegmentSize - crcLength;
// 		}
// 
// 		tempBvec = zeros_b(blockSize);
// 		for (bitIndex = 0;bitIndex < blockSize;bitIndex ++)
// 		{
// 			tempBvec(bitIndex) = rInbits(runIndex ++);
// 		}
// 
// 		if (crcLength)
// 		{
// 		  coderInfo.bitsPerCodewordPerCodeblock(currentCodeWord)(iBlock) = crc_LTE.performCRCEncoding(tempBvec);
// 		}
// 		else
// 		{
// 		  coderInfo.bitsPerCodewordPerCodeblock(currentCodeWord)(iBlock) = tempBvec;
// 		}
// 	}
// 
// 
// 	coderInfo.blockSegments(currentCodeWord).nCodesPlus = nCodes;
// 	coderInfo.blockSegments(currentCodeWord).nCodesMinus = nCodes_;
// 	coderInfo.blockSegments(currentCodeWord).blockSizePlus = nSegmentSize;
// 	coderInfo.blockSegments(currentCodeWord).nFillerBitsUsed = nFillerBits;
// 	coderInfo.blockSegments(currentCodeWord).blockSizeMinus = nSegmentSize_;
// 	tbConfig.nCodeBlocksPerCodeWord[currentCodeWord] = coderInfo.blockSegments(currentCodeWord).nCodesMinus + coderInfo.blockSegments(currentCodeWord).nCodesPlus;
// 
// 	#ifdef _DEBUG_CHANNELCODER_ENABLE
// 	cout << "Codes+ - " << "\t" << coderInfo.blockSegments(currentCodeWord).nCodesPlus << "\tSize+ - " << "\t" << coderInfo.blockSegments(currentCodeWord).blockSizePlus << "\tCodes- - ";
// 	cout << coderInfo.blockSegments(currentCodeWord).nCodesMinus << "\tSize- - " << coderInfo.blockSegments(currentCodeWord).blockSizeMinus << "\tnFillers - " << nFillerBits << endl;
// 	#endif
// 
// }


/*! \brief Do the Modulation of channelCoderTxCircularBuffer & put it in modulatedSymbols
\param [out] modulatedSYmbols \a Array<cvec> Final modulated-symbols for a UE to be put into Physical PRBs with each cvec corresponds to each layer */
Array<cvec> Encoder::performModulation(CoderInfo_S &coderInfo)
{
	int iCodeWord,alphabetSize;
	Array<cvec> modulatedSymbols;
	modulatedSymbols.set_size(currentHarqInfo.codeWordIndicesToConsider.length());
	for (iCodeWord = 0;iCodeWord < currentHarqInfo.codeWordIndicesToConsider.length();iCodeWord ++)
	{
		alphabetSize = (int) pow2((double) tbConfig.modAlphabet(iCodeWord));

#ifdef USING_ITPP
        if(alphabetSize==2)  
        {
            modulatedSymbols(iCodeWord) = bpskModem.modulate_bits(coderInfo.channelCoderTxCircularBuffer(iCodeWord));
            modulatedSymbols(iCodeWord)*=complex<double> (1.0/sqrt(2.0),1.0/sqrt(2.0)); // ITPP modulator generates +1, -1
        }
        else
#endif		
        {
            qamModem.set_M(alphabetSize);
            modulatedSymbols(iCodeWord) = qamModem.modulate_bits(coderInfo.channelCoderTxCircularBuffer(iCodeWord));
        }
        
		if(alphabetSize==2) // pi/2 BPSK
		  modulatedSymbols(iCodeWord)=applyPiBy2Rotation(modulatedSymbols(iCodeWord));
		
	}
	return(modulatedSymbols);
}

/*! \brief Do the scrambling of channelCoderTxCircularBuffer with gold sequence.
\param [in,out] channelCoderTxCircularBuffer \a Array<bvec> each bvec is total bits to be put into Physical PRBs for a Layer.
\todo need to check whether scrambling is dependant upon CodeWords/Layers
\note Gold sequence is dependent upon CellID */
void Encoder::performScrambling(CoderInfo_S &coderInfo)
{
	double Cinit;
	bvec tempBvec;
	Cinit = nRNTI * pow(2.0,14.0) + (tbConfig.mimoRank - 1.0) * pow(2.0,13.0) + floor(1 / 2.0) * pow(2.0,9.0) + tbConfig.cellID;
	for (int iCodeWord = 0;iCodeWord < currentHarqInfo.codeWordIndicesToConsider.length();iCodeWord ++)
	{
	  coderInfo.scramblerSeq(iCodeWord) = generateGoldSequence(Cinit,coderInfo.channelCoderTxCircularBuffer(iCodeWord).length()); 
	}

	for (int iCodeWord = 0;iCodeWord < currentHarqInfo.codeWordIndicesToConsider.length();iCodeWord ++)
	{
	  tempBvec.set_size(coderInfo.channelCoderTxCircularBuffer(iCodeWord).length());
	  for (int iBit = 0;iBit < coderInfo.channelCoderTxCircularBuffer(iCodeWord).length();iBit ++)
	  {
	      tempBvec(iBit) = coderInfo.channelCoderTxCircularBuffer(iCodeWord)(iBit) ^ (bin)(coderInfo.scramblerSeq(iCodeWord)(iBit));
	  }

	  coderInfo.channelCoderTxCircularBuffer(iCodeWord) = tempBvec;
	}
}


ivec ChannelCoder::getHARQTBSize(int harqID) {
  
  ivec output(harqProcess(harqID).coderInfo.infoBitsPerCodeWordBackup.length());
  for(int cwd_cnt=0;cwd_cnt<harqProcess(harqID).coderInfo.infoBitsPerCodeWordBackup.length();cwd_cnt++)
    output(cwd_cnt)=harqProcess(harqID).coderInfo.infoBitsPerCodeWordBackup(cwd_cnt).length();
  return(output);
}

ivec ChannelCoder::getCodeBlocksPerCodeword(int harqID){
  
  
  if(harqID==-1)
    harqID=currentHarqInfo.harqProcessID;

  return(harqProcess(harqID).coderInfo.layerCBW.nCodeBlocksPerCodeWord);
}

/*! \brief Verify the inputted infoBitsPerCodeWord, or create infoBitsPerCodeWord if it's empty.
 *  \details: Following operations are done individually for each codeWord...
 *      1. If inputted infoBitsPerCodeWord is not empty, then return the same after verification(of size)
 *         Else, Create infoBitsPerCodeWord as random bvec.
 *  \note Do not assume infoBitsPerCodeWord generated/inputted in this function will get thrown away if it's a retransmission.. */
Array<bvec> ChannelCoder::getInfoBitsPerCodeWord(Array<bvec> infoBitsPerCodeWord) {

  tbConfig.numLayersPerCodeWord = getNumLayersPerCodeWord(currentHarqInfo.newDataIndicator.length(),tbConfig.mimoRank);

  tbConfig.transportBlockSize.set_size(tbConfig.nCodeWords);
  infoBitsPerCodeWord.set_length(tbConfig.nCodeWords,true);

  //! First compute the transportBlockSize //
  for(int iCodeWord=0; iCodeWord<currentHarqInfo.codeWordIndicesToConsider.length(); iCodeWord++)
    if(harqProcess(currentHarqInfo.harqProcessID).isNewTransmission(iCodeWord)== 1) //! New transmission
    {
        if(harqProcess(currentHarqInfo.harqProcessID).isTurboCoderEnabled)
	  tbConfig.transportBlockSize[iCodeWord] = tbTables->getTBSizeForAllLayersCombined(tbConfig.tbIndexPerCodeWord(iCodeWord),tbConfig.nREsAllocated,tbConfig.allocatedPRBs,tbConfig.numLayersPerCodeWord(iCodeWord),tbConfig.allocatedSymbols,tbConfig.isDownlink);
        else
          tbConfig.transportBlockSize[iCodeWord] = tbConfig.numLayersPerCodeWord(iCodeWord)*tbConfig.nREsAllocated*tbConfig.modAlphabet(iCodeWord)-nrCRC.getNumParity();
        harqProcess(currentHarqInfo.harqProcessID).transportBlockSize[iCodeWord] = tbConfig.transportBlockSize[iCodeWord];
    }
    else                                                //! Re-transmission
        tbConfig.transportBlockSize[iCodeWord] = harqProcess(currentHarqInfo.harqProcessID).transportBlockSize[iCodeWord];

  //! Now verfiy/make up the infoBitsPerCodeWord... ///
  for(int iCodeWord=0; iCodeWord<currentHarqInfo.codeWordIndicesToConsider.length(); iCodeWord++)
    { 
    if(harqProcess(currentHarqInfo.harqProcessID).isNewTransmission(iCodeWord)==1 && !infoBitsPerCodeWord(iCodeWord).length())      //! If New-transmission && infoBitsPerCodeWord is empty, then create infoBitsPerCodeWord...
    {
     infoBitsPerCodeWord(iCodeWord) = randb(tbConfig.transportBlockSize[iCodeWord]);
        

//         std::cout << "TX BITS GENERATED EMPTY: " <<std::endl;
//           for (int hj=0; hj<30;hj++)
//           {
//               std::cout << infoBitsPerCodeWord(iCodeWord)(hj) << " ";
// 
//           }
//           std::cout << "" <<std::endl;
    }
    else if(harqProcess(currentHarqInfo.harqProcessID).isNewTransmission(iCodeWord)==1)                                             //! If New-transmission && infoBitsPerCodeWord is not empty, then check whether infoBitsPerCodeWord having proper size..
    {
      
      assert(infoBitsPerCodeWord(iCodeWord).length()==tbConfig.transportBlockSize[iCodeWord] && "transportBlockSize doesn't match with infoBitsPerCodeWord!!!");     
//       std::cout << "TX BITS GENERATED NON-EMPTY: " <<std::endl;
//           for (int hj=0; hj<30;hj++)
//           {
//               std::cout << infoBitsPerCodeWord(iCodeWord)(hj) << " ";
// 
//           }
//           std::cout << "" <<std::endl;
    }
    else //! For re-transmission, infoBitsPerCodeWord must be empty!!!
    {
        assert(infoBitsPerCodeWord(iCodeWord).length()==0 && "infoBitsPerCodeWord length not 0 for re-transmission!!!");
//         std::cout << "TX BITS GENERATED RETRANSMISSION: " <<std::endl;
//           for (int hj=0; hj<30;hj++)
//           {
//               std::cout << infoBitsPerCodeWord(iCodeWord)(hj) << " ";
// 
//           }
//           std::cout << "" <<std::endl;
    }  
    }
  //! Backup the infoBitsPerCodeWord in harqProcess if it's new transmissions ///

  for(int iCodeWord=0; iCodeWord<currentHarqInfo.codeWordIndicesToConsider.length(); iCodeWord++)
  {
    if(harqProcess(currentHarqInfo.harqProcessID).isNewTransmission(iCodeWord)== 1) //! New transmission
        harqProcess(currentHarqInfo.harqProcessID).coderInfo.infoBitsPerCodeWordBackup(iCodeWord) = infoBitsPerCodeWord(iCodeWord);
    else                                                //! Re-transmission
        infoBitsPerCodeWord(iCodeWord) = harqProcess(currentHarqInfo.harqProcessID).coderInfo.infoBitsPerCodeWordBackup(iCodeWord);     //! \note: Currently this is not used anywhere...
  }
  return infoBitsPerCodeWord;
}


void Decoder::decoderInitBits(Array<bvec> infoBitsPerCodeWord) {

  tbConfig.numLayersPerCodeWord = getNumLayersPerCodeWord(currentHarqInfo.newDataIndicator.length(),tbConfig.mimoRank);
  tbConfig.transportBlockSize.set_size(tbConfig.nCodeWords);
  //! First compute the transportBlockSize ///
  for(int iCodeWord=0; iCodeWord<currentHarqInfo.codeWordIndicesToConsider.length(); iCodeWord++)
    if(harqProcess(currentHarqInfo.harqProcessID).isNewTransmission(iCodeWord)== 1) //! New transmission
    {
        if(harqProcess(currentHarqInfo.harqProcessID).isTurboCoderEnabled)
	  tbConfig.transportBlockSize[iCodeWord] = tbTables->getTBSizeForAllLayersCombined(tbConfig.tbIndexPerCodeWord(iCodeWord),tbConfig.nREsAllocated,tbConfig.allocatedPRBs,tbConfig.numLayersPerCodeWord(iCodeWord),tbConfig.allocatedSymbols,tbConfig.isDownlink);
        else
          tbConfig.transportBlockSize[iCodeWord] = tbConfig.numLayersPerCodeWord(iCodeWord)*tbConfig.nREsAllocated*tbConfig.modAlphabet(iCodeWord)-nrCRC.getNumParity();
        harqProcess(currentHarqInfo.harqProcessID).transportBlockSize[iCodeWord] = tbConfig.transportBlockSize[iCodeWord];
    }
    else                                                //! Re-transmission
        tbConfig.transportBlockSize[iCodeWord] = harqProcess(currentHarqInfo.harqProcessID).transportBlockSize[iCodeWord];

    
  //! Backup the infoBitsPerCodeWord in harqProcess of the coder object into decoder object if it's new transmissions(for BER calc) 
  //!NOTE Not required in decoder
  {
  for(int iCodeWord=0; iCodeWord<currentHarqInfo.codeWordIndicesToConsider.length(); iCodeWord++)
  {
    if(harqProcess(currentHarqInfo.harqProcessID).isNewTransmission(iCodeWord)== 1) //! New transmission
        harqProcess(currentHarqInfo.harqProcessID).coderInfo.infoBitsPerCodeWordBackup(iCodeWord) = infoBitsPerCodeWord(iCodeWord);
//    std::cout << "TX BITS BEFORE CRC: " <<std::endl;
//        for (int hj=0; hj<30;hj++)
//        {
//            std::cout << infoBitsPerCodeWord(iCodeWord)(hj) << " ";
            
//        }
//        std::cout << "" <<std::endl;
  } 
  }
}
/*! \brief Function to permute the codeWords by picking the permutation sequence..
 *  \details To make codeWordIndicesToConsider, invisible to channel coder, All entities with codeWords are permuted apriori, as per inputted codeWordIndicesToConsider...
 */
void ChannelCoder::permuteHarqProcessWrtCodeWords(HARQProcess& harqProcess_, ivec& codeWordIndicesToConsider)
{
  assert(length(codeWordIndicesToConsider)<=2 && min(codeWordIndicesToConsider)>=0 && max(codeWordIndicesToConsider)<=2 && "Error: Invalid codeWordIndicesToConsider!!!");

  append(harqProcess_.existingCodeWordPermutaion,remove(to_ivec(linspace(0,max(codeWordIndicesToConsider),max(codeWordIndicesToConsider)+1)),harqProcess_.existingCodeWordPermutaion));

  ivec newPermutation = concat(codeWordIndicesToConsider,remove(harqProcess_.existingCodeWordPermutaion,codeWordIndicesToConsider));


  harqProcess_.channelCoderRxCircularBufferPerCodeBlock.set_size(newPermutation.length(),true);
  harqProcess_.transportBlockSize.set_size(newPermutation.length(),true);
  harqProcess_.coderInfo.blockSegments.set_size(newPermutation.length(),true);
  harqProcess_.coderInfo.channelCoderTxCircularBuffer.set_size(newPermutation.length(),true);
  harqProcess_.coderInfo.infoBitsPerCodeWordBackup.set_size(newPermutation.length(),true);
  harqProcess_.coderInfo.puncturedLocations.set_size(newPermutation.length(),true);
  harqProcess_.coderInfo.scramblerSeq.set_size(newPermutation.length(),true);
  harqProcess_.coderInfo.bitsPerCodewordPerCodeblock.set_size(newPermutation.length(),true);
  harqProcess_.coderInfo.subBlockInterleaverOutputInfo.set_size(newPermutation.length(),true);
  harqProcess_.coderInfo.layerCBW.nCodeBlocksPerCodeWord.set_size(newPermutation.length(),true);
  harqProcess_.coderInfo.layerCBW.errorFlag.set_size(newPermutation.length(),true);
  harqProcess_.coderInfo.layerCBW.codeBlockSizes.set_size(newPermutation.length(),true);
  harqProcess_.coderInfo.layerCBW.ratematchedOutputSize.set_size(newPermutation.length(),true);
  if(newPermutation.length()>harqProcess_.newDataIndicator_old.length()) 
    append(harqProcess_.newDataIndicator_old,ones_b(newPermutation.length()-harqProcess_.newDataIndicator_old.length()));

  harqProcess_.channelCoderRxCircularBufferPerCodeBlock = getFromArray(harqProcess_.channelCoderRxCircularBufferPerCodeBlock,find(harqProcess_.existingCodeWordPermutaion,newPermutation));
  harqProcess_.transportBlockSize = harqProcess_.transportBlockSize(find(harqProcess_.existingCodeWordPermutaion,newPermutation));
  harqProcess_.coderInfo.blockSegments = getFromArray(harqProcess_.coderInfo.blockSegments,find(harqProcess_.existingCodeWordPermutaion,newPermutation));
  harqProcess_.coderInfo.channelCoderTxCircularBuffer = getFromArray(harqProcess_.coderInfo.channelCoderTxCircularBuffer,find(harqProcess_.existingCodeWordPermutaion,newPermutation));
  harqProcess_.coderInfo.infoBitsPerCodeWordBackup = getFromArray(harqProcess_.coderInfo.infoBitsPerCodeWordBackup,find(harqProcess_.existingCodeWordPermutaion,newPermutation));
  harqProcess_.coderInfo.puncturedLocations = getFromArray(harqProcess_.coderInfo.puncturedLocations,find(harqProcess_.existingCodeWordPermutaion,newPermutation));
  harqProcess_.coderInfo.scramblerSeq = getFromArray(harqProcess_.coderInfo.scramblerSeq,find(harqProcess_.existingCodeWordPermutaion,newPermutation));
  harqProcess_.coderInfo.bitsPerCodewordPerCodeblock = getFromArray(harqProcess_.coderInfo.bitsPerCodewordPerCodeblock,find(harqProcess_.existingCodeWordPermutaion,newPermutation));
  harqProcess_.coderInfo.subBlockInterleaverOutputInfo = getFromArray(harqProcess_.coderInfo.subBlockInterleaverOutputInfo,find(harqProcess_.existingCodeWordPermutaion,newPermutation));
  harqProcess_.coderInfo.layerCBW.codeBlockSizes = getFromArray(harqProcess_.coderInfo.layerCBW.codeBlockSizes,find(harqProcess_.existingCodeWordPermutaion,newPermutation));
  harqProcess_.coderInfo.layerCBW.ratematchedOutputSize = getFromArray(harqProcess_.coderInfo.layerCBW.ratematchedOutputSize,find(harqProcess_.existingCodeWordPermutaion,newPermutation));
  harqProcess_.coderInfo.layerCBW.nCodeBlocksPerCodeWord = harqProcess_.coderInfo.layerCBW.nCodeBlocksPerCodeWord(find(harqProcess_.existingCodeWordPermutaion,newPermutation));
  harqProcess_.coderInfo.layerCBW.errorFlag = harqProcess_.coderInfo.layerCBW.errorFlag(find(harqProcess_.existingCodeWordPermutaion,newPermutation));
  harqProcess_.newDataIndicator_old = harqProcess_.newDataIndicator_old(find(harqProcess_.existingCodeWordPermutaion,newPermutation));

  
  
  // LDPC
  harqProcess_.coderInfo.BGperCodeWord.resize(newPermutation.length());
  harqProcess_.coderInfo.zPerCodeBlockPerCodeWord.set_size(newPermutation.length(),true);
  harqProcess_.coderInfo.encoderOutputPerCodeBlockSTD.set_size(newPermutation.length(),true);
  
  ivec temp=stdVectortoITPPVec(harqProcess_.coderInfo.BGperCodeWord);
  temp=temp(find(harqProcess_.existingCodeWordPermutaion,newPermutation));
  harqProcess_.coderInfo.BGperCodeWord=ITPPVectorstdVector(temp);
  harqProcess_.coderInfo.zPerCodeBlockPerCodeWord=getFromArray(harqProcess_.coderInfo.zPerCodeBlockPerCodeWord,find(harqProcess_.existingCodeWordPermutaion,newPermutation));
  harqProcess_.coderInfo.encoderOutputPerCodeBlockSTD=getFromArray(harqProcess_.coderInfo.encoderOutputPerCodeBlockSTD,find(harqProcess_.existingCodeWordPermutaion,newPermutation));
  
  
  harqProcess_.existingCodeWordPermutaion = newPermutation;
  
  
}
