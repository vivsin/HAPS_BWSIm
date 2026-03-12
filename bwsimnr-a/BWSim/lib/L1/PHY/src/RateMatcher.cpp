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

#include "../include/RaterMatcher.h"

RateMatcher::RateMatcher()
{
        CTCsubBlock = 32;
        permutationSequence = "0 16 8 24 4 20 12 28 2 18 10 26 6 22 14 30 1 17 9 25 5 21 13 29 3 19 11 27 7 23 15 31";
}

/*! \brief Compute the number of Columns & # Rows of the Matrix for Sub-block interleaver
 * \param[in]  CTCsubBlock No of Columns(32 for LTE) of the Matrix.
 * \param[out] RTCsubBlock No of Rows of the Matrix.
 * \param[out] paddedZeros No of 0's padded to initial position of Matrix to have #Columns = CTCsubBlock(32).*/
void RateMatcher::updateRTCsubBlock(int blockSize)
{
        RTCsubBlock = (int) ceil(blockSize / (double)CTCsubBlock);
        paddedZeros = RTCsubBlock * CTCsubBlock - blockSize;
}

/*! \brief Following function is called after Sub-block Interleaving of each of the block of Turbo-encoder output, to form circularBuffer.
 * \abstract In this function Turbo-encoder output Block1 & Block2 are interleaved together  */
void RateMatcher::interleavePartiyBits(ivec &turboEncoderOutput)
{
        ivec concatBits;
        int blockSize,startIndex;
        blockSize = turboEncoderOutput.length() / 3;

        startIndex = blockSize;
        concatBits = turboEncoderOutput;
        for (int iIndex = 0;iIndex < blockSize;iIndex ++)
        {
                turboEncoderOutput(startIndex + 2 * iIndex) = concatBits(blockSize + iIndex);
                turboEncoderOutput(startIndex + 2 * iIndex + 1) = concatBits(2 * blockSize + iIndex);
        }
}

/*! \brief Output the subBlockInterleaverOutputInfo.circularBuffer containing FillerBits & paddedZeros(as -1's), for a turbo-encoder..
 * \param[in] turboEncoderOutput An Array of 3 \a bvec contains the output of Turbo-encoder
 * \param[out] subBlockInterleaverOutputInfo.circularBuffer A \a bvec containing the outputs of all 3 sub-block interleaver. This is same as subBlockInterleaverOutputInfo.circularBuffer (contains filler-Bits & padded-Zeros)
 * \param[out] subBlockInterleaverOutputInfo.circularBufferDataBitIndex Index of the DataBits(excludes filler bits & Padded Zeros) in subBlockInterleaverOutputInfo.circularBuffer (reverse Mapping), \note -1 indicate Filler/Padded bit position..
 * \param nBlocks	No of bvecs output of Turbo-encoder (Usuallly 3)
 * \param blockMatrix A \a bmat with nColumns =32, used to take PaddedZeros(& Filler Bits), \a turboEncoderOutput(i) {i = 0,1,2}
 * \param[in] paddedZeros Indicate no of '-1' s padded to the beginning of blockMatrix to make it's nColumns =32
 * \ref ts36.213-v10.2.0, section 5.1.4.1.1               */
SubBlockInterleaverOutputInfo_S RateMatcher::performSubBlockInterleaving(const Array<bvec> &turboEncoderOutput, int nFillers,bool isSubblockInterleaverEnabled)
{
	SubBlockInterleaverOutputInfo_S subBlockInterleaverOutputInfo;

	if(!isSubblockInterleaverEnabled) {
		for(int bvecIndex=0; bvecIndex<turboEncoderOutput.length(); bvecIndex++) {
	  		append(subBlockInterleaverOutputInfo.circularBuffer,to_ivec(turboEncoderOutput(bvecIndex)));
		}
		subBlockInterleaverOutputInfo.circularBufferDataBitIndex=getIntegers(0,subBlockInterleaverOutputInfo.circularBuffer.length()-1);
		return(subBlockInterleaverOutputInfo);
 	}	  
        imat blockMatrix,blockMatrixIndex;
        ivec tempParity,tempParityIndex,runIndexv;
        int nBlocks,iBlock,iCTC,iRTC,/*runIndex,*/tempnFillers,startIndex;

        tempParity.set_size(0);
        nBlocks = turboEncoderOutput.size();
        for (iBlock = 0;iBlock < nBlocks;iBlock ++)
        {
		updateRTCsubBlock(turboEncoderOutput(iBlock).length());
		blockMatrix = zeros_i(RTCsubBlock,CTCsubBlock);
		blockMatrixIndex = zeros_i(RTCsubBlock,CTCsubBlock);
		
                switch(iBlock)
                {
                    case 0: startIndex = nFillers;                                  break;
                    case 1: startIndex = turboEncoderOutput(iBlock).length() + nFillers; break;
                    case 2: startIndex = 2*turboEncoderOutput(iBlock).length();          break;
                    default: cout<<"[both:] iBlock != {0,1,2} in function performSubBlockInterleaving"<<endl;abort();
                }

                tempnFillers = (iBlock == 2)? 0:nFillers; 
		blockMatrix = transpose(reshape(concat(-ones_i(paddedZeros+tempnFillers),to_ivec(turboEncoderOutput(iBlock))),CTCsubBlock,RTCsubBlock));	     
		blockMatrixIndex = transpose(reshape(concat(-ones_i(paddedZeros+tempnFillers),getIntegers(startIndex,startIndex+ RTCsubBlock*CTCsubBlock-paddedZeros-tempnFillers-1)),CTCsubBlock,RTCsubBlock));
                if (iBlock < (nBlocks - 1))
                {       for (iCTC = 0;iCTC < CTCsubBlock;iCTC ++)
                        {      
                                append(subBlockInterleaverOutputInfo.circularBuffer,blockMatrix.get_col(permutationSequence(iCTC)));
                                append(subBlockInterleaverOutputInfo.circularBufferDataBitIndex,blockMatrixIndex.get_col(permutationSequence(iCTC)));	  
			}
	        }
                else
                {
                        for (iRTC = 0;iRTC < RTCsubBlock;iRTC ++)
                        {
                                append(tempParity,blockMatrix.get_row(iRTC));
                                append(tempParityIndex,blockMatrixIndex.get_row(iRTC));
                        }
                        vec vCTC = linspace(0,CTCsubBlock * RTCsubBlock-1,CTCsubBlock*RTCsubBlock);
			runIndexv = to_ivec((vec)rem( (permutationSequence(to_ivec((vec)vCTC/(double)RTCsubBlock)) + CTCsubBlock*rem((vec)vCTC, (double)RTCsubBlock) + 1), (double)(RTCsubBlock * CTCsubBlock)));
                        append(subBlockInterleaverOutputInfo.circularBuffer,tempParity(runIndexv));
			append(subBlockInterleaverOutputInfo.circularBufferDataBitIndex,tempParityIndex(runIndexv));
                }
        }
        interleavePartiyBits(subBlockInterleaverOutputInfo.circularBuffer);
        interleavePartiyBits(subBlockInterleaverOutputInfo.circularBufferDataBitIndex);
	return(subBlockInterleaverOutputInfo);
}

 
/*! \brief Following function is called after Sub-block interleaving & before Scrambling.
 * Compute the number of bits for to be outputed to LayerMapper, then do the rate-Matching (called for each codeword)
\param[in] turboEncoderOutput 	Turbo-encoder output (Contains 3 bvec)
\param[in] tbConfig 	Contains all parameters for the transport block(MCS, nPRBs, nCodeWords)
\param[in] ueClass	Don't know??
\param[in] codeBloskIndex	Indicate Segment(Codeword) index
\param[in] currentCodeWord Indicate LayerIndex
\param[in] iFillers	Indicate nFillerBits used for the segment.
\param[in] subBlockInterleaverOutputInfo.circularBuffer Output of the sub-block Interleaver concatenated. It contains both FillerBits(Added while Segmentation) & Padded-Zeros(Added for Sub-block Interleaving)
\param[in] subBlockInterleaverOutputInfo.circularBufferDataBitIndex Contains index of data-bits in subBlockInterleaverOutputInfo->circularBuffer.
\param[out] puncturedLocations	ivec containg
\param[out] EoutBits	Total number of Bits to be outputted.
TODO6 -how to add following to a seperate page?
        1. Segment Input to Turbo-encoder (bvec)
        2. Codeword Output of Turbo-encoder (Array of 3 bvecs)
\ref ts36.212-v10.2.0, Section 5.1.4.1.2 (page:17)) */
//TODO6 -change name codeBlockIndex to segmentIndex.... Call inputs to TurboCoder as segment & output as codeblock..
bvec RateMatcher::performRateMatching(const SubBlockInterleaverOutputInfo_S& subBlockInterleaverOutputInfo,const TBConfig& tbConfig,_UE_CATEGORY_ ueClass,int codeBlockIndex,int currentCodeWord,ivec &puncturedLocations)
{
        int Nir,Ncb,Kw,Mlimit,G,G_,NL,Gamma,EoutBits,K0;

        Mlimit = 8;
        Kw = subBlockInterleaverOutputInfo.circularBuffer.length();
        puncturedLocations.set_size(0);
        //TODO6 -What are these things(Nir,Ncb)????
        //! \todo need to verify followings (Nir,Ncb,NL,G)
        Nir = floor((double) ueClass / ((double) tbConfig.mimoRank * std::min((tbConfig.maxHARQ+1),Mlimit)));
        Ncb = std::min((int) floor((double) Nir / (double)(tbConfig.nCodeBlocksPerCodeWord[currentCodeWord])),Kw);
        NL = tbConfig.numLayersPerCodeWord(currentCodeWord);    //! TODO Need to verify this for SFBC from 'ts36.212-v10.2.0, Section 5.1.4.1.2'
        
        G = tbConfig.nREsAllocated * tbConfig.modAlphabet(currentCodeWord) * NL;      //! TODO Need to verify this..
        G_ = G / (NL * tbConfig.modAlphabet(currentCodeWord));
        Gamma = G_ % (tbConfig.nCodeBlocksPerCodeWord[currentCodeWord]);
        if (codeBlockIndex > (tbConfig.nCodeBlocksPerCodeWord[currentCodeWord] - Gamma - 1))
        {
                EoutBits = NL * tbConfig.modAlphabet(currentCodeWord) * (int) ceil((double) G_ / ((double) (tbConfig.nCodeBlocksPerCodeWord[currentCodeWord])));
        }
        else
        {
                EoutBits = NL * tbConfig.modAlphabet(currentCodeWord) * (int) floor((double) G_ / ((double) (tbConfig.nCodeBlocksPerCodeWord[currentCodeWord])));
        }
        K0 = (int) RTCsubBlock * (2 * ceil((double) Ncb / ((double)(8 * RTCsubBlock))) * tbConfig.rvIndex[currentCodeWord] + 2);
        
        rateMatchedBits.set_size(0);

        ivec dataIndexInCB = find((subBlockInterleaverOutputInfo.circularBuffer==0)+(subBlockInterleaverOutputInfo.circularBuffer==1));   //! Data Indices in circularBuffer
        dataIndexInCB = dataIndexInCB(find(dataIndexInCB<Ncb));                                                                             //! Making Circular Buffer Size= Ncb (trimming the end)...
        ivec dataIndexInCBorder = to_ivec(rem((linspace(0,EoutBits-1,EoutBits)+find(dataIndexInCB>=(K0%Ncb))(0)),dataIndexInCB.length()));  //! K0 is the starting poing of circularBuffer
        rateMatchedBits = to_bvec(subBlockInterleaverOutputInfo.circularBuffer(dataIndexInCB(dataIndexInCBorder)));
        puncturedLocations = subBlockInterleaverOutputInfo.circularBufferDataBitIndex(dataIndexInCB(dataIndexInCBorder));
        #ifdef _DEBUG_CHANNELCODER_ENABLE
        cout << "nCodes - " << tbConfig.nCodeBlocksPerCodeWord[currentCodeWord] << "\t" << "modSymbol - " << tbConfig.modAlphabet(0) << "\t" << "Layer - "
        << NL << "\t" << "G - " << G << "\t" << "Eout - " << EoutBits << "\t" << "iCodeWord - " << currentCodeWord << "\t" << "Shuf" << "\t"
        << puncturedLocations.length() << endl;
        #endif

        return rateMatchedBits;
}
