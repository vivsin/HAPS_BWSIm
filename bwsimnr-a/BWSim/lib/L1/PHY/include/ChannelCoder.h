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

#ifndef CHANNELCODER_H
#define CHANNELCODER_H

#include "SupportingFunctions.h"
#include "cNRTables.h"


#include "RaterMatcher.h"
#include <limits.h>

#include "../../../Frozen/channelCoder/LDPC/include/dataChannelEncoder.h"
#include "../../../Frozen/channelCoder/LDPC/include/dataChannelDecoder.h"


#include "../../../Frozen/channelCoder/Polar/include/polarCodec.hpp"
#include "../../../Frozen/channelCoder/Polar/include/polarDecoder.hpp"



//! if _DEBUG_CHANNELCODER_ENABLE is defined, then operations in channel coder is printed..

/*! \brief If input is a vector, then make '0' of the index specified of the input.
 *         If input is Array, then make '0' size of the index specified of the input. * */
template<class Num_T> inline
void extendAndClearDimensions(Vec<Num_T>& input,int index)
{
    int inputLength = input.length();
    if(inputLength <= index) {
      input.set_size(index+1,true);
      for(int outputIndex=inputLength; outputIndex<index+1; outputIndex++)
	input(outputIndex) = 0;
    }
    else
      	input(index) = 0;
}

template<class Num_T> inline	
void extendAndClearDimensions(Array< Vec<Num_T> >& input,int index)
{
    int inputLength = input.length();
    if(inputLength <= index) {
      input.set_size(index+1,true);
      for(int outputIndex=inputLength; outputIndex<index+1; outputIndex++)
        input(outputIndex).set_size(0);
    }
    else
      input(index).set_size(0);
}

template<class Num_T> inline
void extendAndClearDimensions(Array< Array< Vec<Num_T> > >& input,int index)
{
    int inputLength = input.length();
    if(inputLength <= index) {
      input.set_size(index+1,true);
      for(int outputIndex=inputLength; outputIndex<index+1; outputIndex++)
        input(outputIndex).set_size(0);
    }
    else
      input(index).set_size(0);
}



inline
void extendAndClearDimensions(bvec& input,int index)
{
  int inputLength = input.length();
  if(inputLength <= index) {
    input.set_size(index+1,true);
    for(int outputIndex=inputLength; outputIndex<index+1; outputIndex++)
      input(outputIndex) = 0;
  }
  else
    input(index) = 0;
}

inline	
void extendAndClearDimensions(Array< bvec >& input,int index)
{
  int inputLength = input.length();
  if(inputLength <= index) {
    input.set_size(index+1,true);
    for(int outputIndex=inputLength; outputIndex<index+1; outputIndex++)
      input(outputIndex).set_size(0);
  }
  else
    input(index).set_size(0);
}

inline
void extendAndClearDimensions(Array< Array< bvec > >& input,int index)
{
  int inputLength = input.length();
  if(inputLength <= index) {
    input.set_size(index+1,true);
    for(int outputIndex=inputLength; outputIndex<index+1; outputIndex++)
      input(outputIndex).set_size(0);
  }
  else
    input(index).set_size(0);
}

// void extendAndClearDimensionsSTD(Array< std::vector< std::vector<bool> > > &input, int index)
// {
//    int inputLength = input.size();
//     if(inputLength <= index) 
//     {
//         input.set_size(index+1,true);
//         for(int outputIndex=inputLength; outputIndex<index+1; outputIndex++)
//             input(outputIndex).resize(0);
//     }
//     else
//       input(index).resize(0);
// }

inline int findB_(int ipSize, int BG)
{
    auto C = getNumberOfCodeblocks(ipSize,BG);
    auto L=24;
//     std::cout << "No.of CB: "<<C<<std::endl;
    
    int B_ = ipSize+C*(!(C==1))*L; // CRC is not performed when C=1
    return B_;
}


struct BlockSegments
{
  int nCodesPlus;
  int nCodesMinus;
  int blocksFailed;
  int blockSizePlus;
  int blockSizeMinus;
  int nFillerBitsUsed;
  BlockSegments():nCodesPlus(0),nCodesMinus(0),blocksFailed(0),blockSizePlus(0),blockSizeMinus(0),nFillerBitsUsed(0) {}
};

struct SegmentConfig
{
  ivec errorFlag;
  ivec nCodeBlocksPerCodeWord;
  Array<ivec> codeBlockSizes;
  Array<ivec> ratematchedOutputSize;
  SegmentConfig(){

    errorFlag.set_size(0);
    nCodeBlocksPerCodeWord.set_size(0);
    codeBlockSizes.set_size(0);
  }
  void clear(){

    errorFlag.set_size(0);
    nCodeBlocksPerCodeWord.set_size(0);
    codeBlockSizes.set_size(0);
  }
  void clear(int codeWordIndex){
    extendAndClearDimensions(errorFlag,codeWordIndex);
    extendAndClearDimensions(nCodeBlocksPerCodeWord,codeWordIndex);
    extendAndClearDimensions(codeBlockSizes,codeWordIndex);
  }
  void init(int codeWordIndex){
    extendAndClearDimensions(errorFlag,codeWordIndex);
    extendAndClearDimensions(nCodeBlocksPerCodeWord,codeWordIndex);
    extendAndClearDimensions(codeBlockSizes,codeWordIndex);
    extendAndClearDimensions(ratematchedOutputSize,codeWordIndex);
 }
};

struct CoderInfo_S{
  Array<bvec> infoBitsPerCodeWordBackup;   //! Contains infoBits+CRC. (infoBits length is specified in 3GPP TS 36.213 Table 7.1.7.2.1-1)
  Array<ivec> puncturedLocations;
  SegmentConfig layerCBW;
  Array< Array<bvec> > bitsPerCodewordPerCodeblock;
  Array<bvec> channelCoderTxCircularBuffer;	//! stores final FEC-block per codeWord.. 
  Array<ivec> scramblerSeq;
  Array<BlockSegments> blockSegments;
  Array< Array<SubBlockInterleaverOutputInfo_S> >  subBlockInterleaverOutputInfo;

  //ForLDPC
  Array< Array<bvec> > encoderOutputPerCodeBlock;
  Array< std::vector< std::vector<bool> > > encoderOutputPerCodeBlockSTD;
  std::vector<int> BGperCodeWord;
//   std::vector<std::vector<int>> zPerCodeBlockPerCodeWord;
  Array<std::vector<int>> zPerCodeBlockPerCodeWord;
  
  
  CoderInfo_S(){
    infoBitsPerCodeWordBackup.set_length(0);
    puncturedLocations.set_length(0);
    bitsPerCodewordPerCodeblock.set_length(0);
    channelCoderTxCircularBuffer.set_length(0);
    scramblerSeq.set_length(0);
    blockSegments.set_length(0);
    subBlockInterleaverOutputInfo.set_size(0);
    BGperCodeWord.resize(0);
    encoderOutputPerCodeBlockSTD.set_length(0);
    encoderOutputPerCodeBlock.set_length(0);
    zPerCodeBlockPerCodeWord.set_length(0);
    BGperCodeWord.resize(0);
  }
  void clear(){

    layerCBW.clear();
    puncturedLocations.set_length(0);
    bitsPerCodewordPerCodeblock.set_length(0);
    channelCoderTxCircularBuffer.set_length(0);
    scramblerSeq.set_length(0);
    blockSegments.set_length(0);
    subBlockInterleaverOutputInfo.set_size(0);
    //LDPC
    encoderOutputPerCodeBlock.set_length(0);
    encoderOutputPerCodeBlockSTD.set_size(0);
    BGperCodeWord.clear();
    zPerCodeBlockPerCodeWord.set_length(0);
  }
  void clear(int codeWordIndex){

   layerCBW.clear(codeWordIndex);
   extendAndClearDimensions(puncturedLocations,codeWordIndex);
   extendAndClearDimensions(bitsPerCodewordPerCodeblock,codeWordIndex);
   extendAndClearDimensions(channelCoderTxCircularBuffer,codeWordIndex);
   extendAndClearDimensions(scramblerSeq,codeWordIndex);
   if(blockSegments.length()<=codeWordIndex)
     blockSegments.set_size(codeWordIndex+1); 	
   if(subBlockInterleaverOutputInfo.length()<=codeWordIndex)
     subBlockInterleaverOutputInfo.set_size(codeWordIndex+1);
 }

};

struct RetransmissionInfo_S{

  ivec serviceNodeIDs;
  ivec txCount;

  RetransmissionInfo_S(){

    serviceNodeIDs.set_length(0);
  }

};

/*! \brief Structure for HARQ Process
     */
struct HARQProcess
{
    bool isTurboCoderEnabled;	//! If this variable set to false, then ther's now turbo-coder (Used for no-FEC case)
    bool isSubblockInterleaverEnabled;	//! If this variable set to false, then ther's no sub-block interleaver (Used for no-FEC case)
    bool isScramblerEnabled;	//! If this variable set to false, then ther's no scrambler (Used for no-FEC case)
    int lastTxframe; 		//! Indicate the most recent subframe number in which HARQ packet is transmitted.
    ivec transportBlockSize;
    bvec isNewTransmission;     //! Indicate whether it's new transmission Per CodeWord
    bvec newDataIndicator_old;  //! Store the previous transmission's newDataIndicator Per CodeWord
    Array< Array<vec> > channelCoderRxCircularBufferPerCodeBlock;       //! Format: < CodeWord Index >: < codeBlock(or Segment) Index >
    Array< std::vector<double> > receiverRetransmitBuffer;
    CoderInfo_S coderInfo;
    ivec existingCodeWordPermutaion;    //! Contains the 0..length(existingCodeWordPermutaion)-1, in permuted manner...
    HARQProcess() :lastTxframe(-1) {
      isTurboCoderEnabled=false;
      isSubblockInterleaverEnabled=false;
      isScramblerEnabled=false;
      existingCodeWordPermutaion.set_length(0);
      receiverRetransmitBuffer.set_length(0);
      transportBlockSize.set_length(0);
      isNewTransmission.set_length(0);
      newDataIndicator_old.set_length(0);
      channelCoderRxCircularBufferPerCodeBlock.set_length(0);
    };
};

enum CoderType_E
{
    _CODERTYPE_UNINIT_,
    _CODERTYPE_TURBO_,
    _CODERTYPE_LDPC_,
    _CODERTYPE_POLAR_
};


inline cvec applyPiBy2Rotation(cvec input){ivec i=getIntegers(0,input.length()-1); vec iMod2=to_vec(mod(i,2));return elem_mult(to_cvec(cos(pi/2*iMod2),sin(pi/2*iMod2)),input);};
inline cvec removePiBy2Rotation(cvec input){ivec i=getIntegers(0,input.length()-1); vec iMod2=to_vec(mod(i,2));return elem_mult(to_cvec(cos(pi/2*iMod2),-sin(pi/2*iMod2)),input);};

class ChannelCoder
{

  protected :

    cTBTables *tbTables;
    QAM qamModem;
#ifdef USING_ITPP    
    BPSK_c bpskModem;
#endif
    NrCRC nrCRC;    
    int nRNTI;
    _UE_CATEGORY_ ueModel;
    TBConfig tbConfig;
    HarqInfo currentHarqInfo; 		//! Indicate the HarqInfo of current subframe.
    int maximumHarqTransmissions;	//! Indicate maximum transmissions(including fresh-transmission and re-transmissions) possible for a packet.
    Array<HARQProcess> harqProcess;   	//! Pointer structure containing the all information regarding HARQ
    
    
    
    RateMatcher rMatcher;
      
  public:
    CoderType_E coderType=_CODERTYPE_TURBO_;
    ChannelCoder();
    Array<bvec> getInfoBitsPerCodeWord(Array<bvec> infoBitsPerCodeWord);
//     void performCodeBlockSegmentation(bvec &inBits,CoderInfo_S &coderInfo,int currentCodeWord);
    void initTBConfig(DataGenInfo_S *dataGenInfo, bool isDownlink);
    void initializeHarqProcess(DataGenInfo_S *dataGenInfo,HarqInfo &currentHarqInfo_, int subframeNo, bool isDownlink);
    void flushHarqProcess(int harqIDtoFlush,int codeWordIndex=-1);
    void flushAllHarqProcess();
    inline bvec harqTxStatus(int harqID);         
    void verifyInputs(DataGenInfo_S *dataGenInfo,HarqInfo currentHarqInfo_,int subframeNumber);
    void permuteHarqProcessWrtCodeWords(HARQProcess& harqProcess_, ivec& codeWordIndicesToConsider);
    ivec removeObjectIndexFromPermutationSequence(ivec existingCodeWordPermutaion, int indexToRemove);  
    void initChannelCoder(cTBTables *tbTable,int nRnti=0,CRCTYPE CRCType = CRC24B,_UE_CATEGORY_ ueClass = CATEGORY_8,int maximumHarqProcesses=8,int maximumHarqTransmissions=4,bool isTurboCoderEnabled=true,bool isSubblockInterleaverEnabled=true,bool isScramblerEnabled=true, CoderType_E codertype = _CODERTYPE_LDPC_);
    void setCellID(int cellID);
    ivec getHARQTBSize(int harqID);
    ivec getCodeBlocksPerCodeword(int harqID=-1);
    
};
   
class Encoder: public ChannelCoder
{   
    public:
     Array<cvec> getModulatedSymbols(DataGenInfo_S *dataGenInfo,HarqInfo &currentHarqInfo_,int subframeNumber,bool isDownlink); 
     Array<cvec> performModulation(CoderInfo_S &coderInfo);
     void performScrambling(CoderInfo_S &coderInfo); 
//      Array<cvec> performTurboCoding(Array<bvec> &macPDUBits,TBConfig &tbConfig);
     Array<cvec> performLDPCCoding(Array<bvec> &macPDUBits,TBConfig &tbConfig);
    
     
private:
    std::vector<bool> performLDPCCoding(std::vector<bool> inBits, int E, int Q, int rvid, int BG);
};

class Decoder: public ChannelCoder
{  
  public :
    Array<cvec> initDecoder(Array<bvec> &macPDUBits,TBConfig &tbConfig); 
    void performDeScrambling(Array<vec> &channelCoderRxCircularBuffer);
    vec performDeRateMatching(vec &rmWorthSoft,int rmBlockSize,int permIndex,int iCodeWord,int iCodeBlock,CoderInfo_S &coderInfo);
    Array<vec> performDemodulation(Array<cvec> &rxSymbols,Array<cvec> &equalizedChannel,double NO);
//     bool performCodeBlockConcatenation(BlockSegments& blockSegments, bvec& inBits);
    bvec removeCRCBitsOfBlockSegments(BlockSegments &blockSegments, bvec &inBits, int numOfCRCBits);
    bvec trimCRCBits(bvec &inBits, int numOfCRCBits);
    void decoderInitBits(Array<bvec> infoBitsPerCodeWord);    
//     TransmissionStatus_S performChannelDecoding(Array<cvec> &rxSymbols,Array<cvec> &equalizedChannel,double NO=1);
    TransmissionStatus_S performLDPCChannelDecoding(Array< cvec >& rxSymbols, Array< cvec >& equalizedChannel, double NO = 1);
    void initDecoderHarqProcess(DataGenInfo_S *dataGenInfo,HarqInfo &currentHarqInfo_, int subframeNo, bool isDownlink);
    void initDemodParameters(DataGenInfo_S *dataGenInfo,HarqInfo &currentHarqInfo_,int subframeNumber,bool isDownlink);
    void processHarqReception(vec& codeWord,int iCodeWord,int iCodeBlock);
};

bvec ChannelCoder::harqTxStatus(int harqID)
{
return harqProcess(harqID).isNewTransmission;
}

struct sHARQAbstractionModel{
  
  int HarqProcessID;
  int ueID;
  ivec TransmissionCount;
  Array<ivec> modulationOrderPerCW;
  Array<vec> effMMIBperReceptionPerCW;
  Array<vec> nCodedBitsPerReceptionPerCW;
  
  sHARQAbstractionModel(){
    
    HarqProcessID=-1;
    ueID=-1;
    TransmissionCount=zeros_i(2);
    modulationOrderPerCW.set_length(2);
    effMMIBperReceptionPerCW.set_length(2);
    nCodedBitsPerReceptionPerCW.set_length(2);
  }
};

class SourceAndSink{

  cLTETables *lteTables;
  cTBTables *nrTables;
  
public:
  
    bool isInitialized=false; 
    ivec serviceNodeIDs;
  cTBTables* tbTables;
  Array<Encoder> channelCoders;	//! channelCoder is specific for a link (eg: Macro-UE link).
  Array<Decoder> channelDecoders;
  Array< Array<sHARQAbstractionModel> > mHarqAbstractionModel;
  
  SourceAndSink(){

    serviceNodeIDs.set_length(0);
    channelCoders.set_length(0);
    channelDecoders.set_length(0);
    mHarqAbstractionModel.set_length(8); //Length is hard coded to 8 HARQProcess
  }
  void init(ivec serviceNodes, cLTETables *ltetables, cTBTables *nrtables,ivec rnti, bool initDecoder,string harqType="IR",int maximumHarqProcesses=8,int minDelayBetweenSuccessiveTransmissions=8,int maximumHarqTransmissions=4,bool isTurboCoderEnabled=true,bool isSubblockInterleaverEnabled=true,bool isScramblerEnabled=true, int ueCategory=8,CoderType_E coderType=_CODERTYPE_LDPC_){

      isInitialized=true;
    serviceNodeIDs=serviceNodes;
    lteTables=ltetables;
    nrTables=nrtables;
    
    if(coderType==_CODERTYPE_LDPC_) // NR simulation
      tbTables=nrTables;
    else
      tbTables=lteTables;
    
    channelCoders.set_length(serviceNodeIDs.length());
    if(initDecoder)
      channelDecoders.set_length(serviceNodeIDs.length());
    for(int cnt=0;cnt<serviceNodeIDs.length();cnt++)
    {
      channelCoders(cnt).initChannelCoder(tbTables,rnti(cnt),CRC24B,findUEcategory(ueCategory),maximumHarqProcesses,maximumHarqTransmissions,isTurboCoderEnabled,isSubblockInterleaverEnabled,isScramblerEnabled,coderType);
      if(initDecoder)
	channelDecoders(cnt).initChannelCoder(tbTables,rnti(cnt),CRC24B,findUEcategory(ueCategory),maximumHarqProcesses,maximumHarqTransmissions,isTurboCoderEnabled,isSubblockInterleaverEnabled,isScramblerEnabled,coderType);
    }
  }
  void addServiceNode(int serviceNode, int rnti,string harqType="IR",int maximumHarqProcesses=8,int minDelayBetweenSuccessiveTransmissions=8,int maximumHarqTransmissions=4,bool isTurboCoderEnabled=true,bool isSubblockInterleaverEnabled=true,bool isScramblerEnabled=true, int ueCategory=8){

    if(find(serviceNodeIDs,serviceNode)==-1)
    {
      append(serviceNodeIDs,serviceNode);
      Encoder channelCoder;
      Decoder channelDecoder;
      channelCoder.initChannelCoder(tbTables,rnti,CRC24B,findUEcategory(ueCategory),maximumHarqProcesses,maximumHarqTransmissions,isTurboCoderEnabled,isSubblockInterleaverEnabled,isScramblerEnabled);
      channelDecoder.initChannelCoder(tbTables,rnti,CRC24B,CATEGORY_3,maximumHarqProcesses,maximumHarqTransmissions,isTurboCoderEnabled,isSubblockInterleaverEnabled,isScramblerEnabled);
      append(channelCoders,channelCoder);
      append(channelDecoders,channelDecoder);
    }
    else
    {
      cout<<"ServiceNode already available in SourceAndSink while calling addServiceNode()..."<<endl;
    }
  }
  void moveServiceNode(int serviceNode,SourceAndSink *nativeSourceAndSink){

    int nativeIndx=find(nativeSourceAndSink->serviceNodeIDs,serviceNode);
    if(nativeIndx!=-1)
    {
      append(serviceNodeIDs,serviceNode);
      nativeSourceAndSink->serviceNodeIDs.del(nativeIndx);
      nativeSourceAndSink->channelCoders(nativeIndx).flushAllHarqProcess();
      append(channelCoders,nativeSourceAndSink->channelCoders(nativeIndx));
      deleteInArray(nativeSourceAndSink->channelCoders,to_ivec(nativeIndx));
       append(channelDecoders,nativeSourceAndSink->channelDecoders(nativeIndx));
      deleteInArray(nativeSourceAndSink->channelDecoders,to_ivec(nativeIndx));
 
    }
    else
    {
      cout<<"Trying to move an unknown serviceNode in moveServiceNode()..."<<endl;
    }   
  }
  void deleteServiceNode(int serviceNode){

    int indx=find(serviceNodeIDs,serviceNode);
    if(indx!=-1)
    {
      serviceNodeIDs.del(indx);
      deleteInArray(channelCoders,to_ivec(indx));
       deleteInArray(channelDecoders,to_ivec(indx));
    }
    else
    {
      cout<<"Unknown ServiceNode to deleteServiceNode()..."<<endl;
    }
  }

  Array<cvec> getModulatedSymbols(int serviceNodeID,DataGenInfo_S *dataGenInfo,HarqInfo &currentHarqInfo_, int subframeNumber, bool isDownlink){

    int indx=find(serviceNodeIDs,serviceNodeID);
    if(indx!=-1)
      return(channelCoders(indx).getModulatedSymbols(dataGenInfo,currentHarqInfo_,subframeNumber,isDownlink));
    else
      {cout<<"[both:] Unknown serviceNodeID to getModulatedSymbols()..."<<endl;abort();}
  }
  
  ivec getCodeBlocksPerCodeword(int serviceNodeID,int harqID=-1){
    
    int indx=find(serviceNodeIDs,serviceNodeID);
    if(indx!=-1)
      return(channelCoders(indx).getCodeBlocksPerCodeword(harqID));
    else
    {cout<<"[both:] Unknown serviceNodeID to getCodeBlocksPerCodeword()..."<<endl;abort();}
  }

    void initializeDecoder(int serviceNodeID,DataGenInfo_S *dataGenInfo,HarqInfo &currentHarqInfo_, int subframeNumber, bool isDownlink){

    int indx=find(serviceNodeIDs,serviceNodeID);
    if(indx!=-1)
    {
    (channelDecoders(indx).initDemodParameters(dataGenInfo,currentHarqInfo_,subframeNumber,isDownlink));
    }
      else
      {cout<<"[both:] Unknown serviceNodeID to getModulatedSymbols()..."<<endl;abort();}
  }
   
  void initializeHarqProcess(int serviceNodeID,DataGenInfo_S *dataGenInfo,HarqInfo &currentHarqInfo_, int subframeNumber, bool isDownlink){
    
    int indx=find(serviceNodeIDs,serviceNodeID);
    if(indx!=-1)
      channelCoders(indx).initializeHarqProcess(dataGenInfo,currentHarqInfo_,subframeNumber,isDownlink);
    else
    {cout<<"[both:] Unknown serviceNodeID to initializeHarqProcess()..."<<endl;abort();}
  }

  void initializeRxHarqProcess(int serviceNodeID,DataGenInfo_S *dataGenInfo,HarqInfo &currentHarqInfo_, int subframeNumber, bool isDownlink)
   {    
    int indx=find(serviceNodeIDs,serviceNodeID);
    if(indx!=-1)
    {
     channelDecoders(indx).initializeHarqProcess(dataGenInfo,currentHarqInfo_,subframeNumber,isDownlink);
    }
    else
    {cout<<"[both:] Unknown serviceNodeID to initializeHarqProcess()..."<<endl;abort();}
  }
  
  //! \brief returns newTransmission status per codeword, specified in currentHarqInfo.codeWordIndicesToConsider. (Length of ouput is the actual number of codeWords)
  bvec isCurrentTransmissionNew(int serviceNodeID,int harqID){

    int indx=find(serviceNodeIDs,serviceNodeID);
    if(indx!=-1)
      return(channelCoders(indx).harqTxStatus(harqID));
    else
    {cout<<"[both:] Unknown serviceNodeID to isRetransmitting()..."<<endl;abort();}
  }
  TransmissionStatus_S demodulateAndDecode(EqualizerOutput_S &equalizerOutputForDataBlk, int serviceNodeID);
  
  TransmissionStatus_S getAbstractedTransmissionStatus(int serviceNodeID, DataGenInfo_S& dataGenInfo, HarqInfo& harqInfo, Array< vec >& postProcessingSinr, bvec isNewTransmission, bool isDownlink);
  TransmissionStatus_S getIdealAbstractedTransmissionStatus(int serviceNodeID, DataGenInfo_S& dataGenInfo, HarqInfo& harqInfo, Array< vec >& postProcessingSinr, bvec isNewTransmission, bool isDownlink);
  void updateHarqAbstractionModel(int serviceNodeID, int harqProcessID, bool isNewTransmission, int codewordCnt, double effMMIB, int modOrder, int nCodedBits, int tbSizeWithCRC, double& effCodeRatewithReTx, double& effSINRwithReTx);
  int getTransmissionCount(int serviceNodeID,int harqProcessID,int codewordCnt);
  void clearHarqAbstractionModel(int serviceNodeID,int harqProcessID, int codewordCnt);
  
};


#endif
