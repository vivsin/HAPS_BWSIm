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
#ifndef _RATE_MATCHER_DECL
#define _RATE_MATCHER_DECL

#include "../../../Frozen/simSupport/include/CommonStructsAndEnums.h"



/*! \brief Contains the configuration for one MAC-PDU(Transport block) for a UE, for all Codewords combined..
\relatesalso ChannelCoder
\param maxHARQ 		Maximum Transmissions possible for HARQ
\param mimoRank 	No of Layers(Streams) for transmission if nCodeWords = false
\param allocatedPRBs 	Total number of PRBs allocated for the UE
\param rvIndex		Index for Retransmission used both at Transmitter & Receiver (0 means first transmission)
\param nCodeBlocksPerCodeWord	Indicate number of Segments(Codewords) for each Codeword
\param transportBlockSize	(MAC-PDU Size)/(No of Layers)
\param modAlphabet	Indicate modulation Sceme (QPSK=2, 16QAM=4, 64QAM=6)	*/
struct TBConfig
{
    bool isDownlink;
    int cellID;
    int maxHARQ;
    int mimoRank;
    ivec numLayersPerCodeWord;
    int nREsAllocated;
    int allocatedPRBs;
    int allocatedSymbols;
    int nCodeWords;
    ivec rvIndex;
    ivec tbIndexPerCodeWord;
    ivec nCodeBlocksPerCodeWord;
    ivec mcsIndices;
    ivec transportBlockSize;
    Array<MOD_ALPHABET_SIZE> modAlphabet;
};

struct SubBlockInterleaverOutputInfo_S {

    //! Output of Sub-block interleaver
    ivec circularBuffer;
    //! Data-Bit positions in the Circular-Buffer
    ivec circularBufferDataBitIndex;

    SubBlockInterleaverOutputInfo_S() {
        circularBuffer.set_size(0);
        circularBufferDataBitIndex.set_size(0);
    }
};


class RateMatcher
{
private :

    int paddedZeros;
    int CTCsubBlock;
    int RTCsubBlock;
    bvec rateMatchedBits;
    ivec permutationSequence;

private :

    void updateRTCsubBlock(int);

public :

    static void interleavePartiyBits(ivec &);
    RateMatcher();
    SubBlockInterleaverOutputInfo_S performSubBlockInterleaving(const Array<bvec> &turboEncoderOutput,int nFillers,bool isSubblockInterleaverEnabled=true);
    bvec performRateMatching(const SubBlockInterleaverOutputInfo_S& subBlockInterleaverOutputInfo,const TBConfig& tbConfig,_UE_CATEGORY_ ueClass,int currentBlockIndex,int currentCodeWord,ivec &puncturedLocations);
};

#endif

