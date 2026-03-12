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
#ifndef OLDSCH_H
#define OLDSCH_H

#include "../../../L1/PHY/include/PowerControl.h"
#include "Old_OuterLoopLA.h"

struct UEInfo_S{
  
  bool isCQIReported;
  int UEid;
  TransmissionMode_E txMode;
  ivec vrbNumbers;
  Array<vec> sinrInDB;
  eCQIType cqiType;
  int nLayers;
  ivec pmi;
  int i1;
  int i1_1; // Wide band NR PMI 1
  int i1_2; // Wide band NR PMI 2
  int i1_3; // Wide band NR PMI 3
  int i1_4;
  ivec i2;  // subband PMI per RB
  bool isPrecoderAvailable;
  Array<cmat> precoder;
  Array<cmat> reciproPrecoder;//reciprocity
  Array< cmat > sbChannel;
  
  UEInfo_S(){
    
    isCQIReported=false;
    UEid = -1;
    txMode = _TRANSMISSION_MODE_UNINIT_;
    vrbNumbers.set_size(0);
    cqiType = CQIType_UNINIT;
    nLayers = -1;
    pmi.set_size(0);
    i1=-1;
    i1_1=-1;
    i1_2=-1;
    i1_3=-1;
    i1_4=-1;
    i2.set_size(0);
    isPrecoderAvailable=false;
    precoder.set_size(0);
    reciproPrecoder.set_length(0);
    sbChannel.set_length(0);
  }
  
  void printUEInfo()
  {
    cout<<"========= Printing UEInfo_S structure ==========="<<endl;
    cout<<"UEid :               "<<UEid<<endl;
    cout<<"TM mode :            "<<(TransmissionMode_E)txMode<<endl;
    cout<<"vrbNumbers :         "<<vrbNumbers<<endl;
    cout<<"sinrInDB :                "<<sinrInDB<<endl;
    cout<<"cqiType :            "<<(eCQIType)cqiType<<endl;
    cout<<"nLayers :            "<<nLayers<<endl;
    cout<<"pmi :                "<<pmi<<endl;
    cout<<"i1 :                 "<<i1<<endl;
    cout<<"i2 :                 "<<i2<<endl;
    cout<<"i1_1 :               "<<i1_1<<endl;
    cout<<"i1_2 :               "<<i1_2<<endl;
    cout<<"i1_3 :               "<<i1_3<<endl;
    cout<<"i1_4 :               "<<i1_4<<endl;
    cout<<"precoder : 		"<<precoder<<endl;
  }
};


struct UEBufferInfo_S
{
    int mUEId;
    int mDataInBufferInBytes;
    bool isInfiniteBuffer;
    int mRBId;
    
    ivec tbSizeInBitsForReTx; //  length will be nCodewords used in previous transmission
    ivec codewordIndices;
    UEBufferInfo_S()
    {
        mUEId = -1;
        mDataInBufferInBytes = 0;
	       tbSizeInBitsForReTx.set_length(0);
           codewordIndices.set_length(0);
	isInfiniteBuffer=false;
    mRBId=-1;
    }
};


struct PairingInfo_S{
  
  ivec ueIDs;
  ivec pairingPositionPerUE;
  
  PairingInfo_S(){
      
      ueIDs.set_size(0);
      pairingPositionPerUE.set_size(0);
  }
};


struct MUMIMOPairingInfo_S{
  
  ivec mUEIds;
  ivec nLayers;
  Array<cmat> modifiedMuMimoPrecoders;
  Array<vec> SINRoffsetInDB;    //SINR per Codeword per UE
  Array<vec> expectedSINRInDB;
  PairingInfo_S pairingInfo;
  
  MUMIMOPairingInfo_S(){
    
    mUEIds.set_size(0);
    nLayers.set_size(0);
    SINRoffsetInDB.set_size(0);
    modifiedMuMimoPrecoders.set_size(0);
    expectedSINRInDB.set_size(0);
  }
};


struct CQIFeedback_S
{
  int i1; // wide band PMI LTE
  int i1_1;// Wide band NR PMI 1
  int i1_2;// Wide band NR PMI 2
  int i1_3; // Wide band NR PMI 3
  int i1_4;
  
  ivec i2;  // subband PMI
  
  ivec pmi; // Actual index
  
  bool isTypeIIPrecoderEnabled;
  Array<cmat> sbPrecoder;
  
  Array<ivec> cqi; 
  Array<vec>  effSINRs; ///For UL
  Array< cmat > reciproPrecoder;
  int ri_reciprocity;//reciprocity
  Array<ivec> cqi_reciprocity;
  int ri;
  
  Array< cmat> sbChannel;
  
  CQIFeedback_S(){
    i1=-1;
    i1_1=-1;
    i1_2=-1;
    i1_3=-1;
    i1_4=-1;
    i2.set_size(0);
    pmi.set_size(0);
    isTypeIIPrecoderEnabled=false;
    sbPrecoder.set_length(0);
    ri=-1;
    cqi.set_length(0);
    reciproPrecoder.set_length(0);
    cqi_reciprocity.set_length(0);
    ri_reciprocity=-1;
    sbChannel.set_length(0);
  }
//   void concat(CQIFeedback_S &input){
//     
//     i1=input.i1;
//     i1_1=input.i1_1;
//     i1_2=input.i1_2;
//     i1_3=input.i1_3;
//     i2=input.i2;
//     pmi=input.pmi;
//     append(sbPrecoder,input.sbPrecoder);
//     ri=input.ri;
//     append(cqi,input.cqi);
//     append(effSINRs,input.effSINRs);
//   }
};

// struct ScheduledUEInfo_S{
//   
//   int ueid;
//   Array<ivec> cqiIndices;
//   Array<vec>  effSINRs; ///For UL
//   eCQIType cqiType;
//   TransmissionMode_E txMode;
//   double mcsRate;
//   ivec mcsIndxPerCodeword;
//   ivec tbSizePerCodeword;
//   int pmi;
//   int nLayers;
//   ivec vrbs;
//   Array<imat> rbStructurePerRB;
//   
//   ScheduledUEInfo_S(){
//     
//     ueid=-1;
//     cqiIndices.set_length(0);
//     effSINRs.set_length(0);
//     cqiType=CQIType_UNINIT;
//     txMode=_TRANSMISSION_MODE_UNINIT_;
//     mcsRate=-1;
//     nLayers=-1;
//     vrbs.set_length(0);
//     rbStructurePerRB.set_length(0);
//     tbSizePerCodeword.set_size(0);
//     mcsIndxPerCodeword.set_size(0);
//   }
// };

struct SchedulingInfo_S{
  ivec ueIDs;
  ivec rbIDs;
  Array<DataBlkInfo_S> dataBlkInfo;
  
};

struct ScheduledUEInfo_S{
  
  ivec ueid;
  Array<ivec> symPos;
//   Array< Array<ivec> > cqiIndices;
  Array< Array<vec> > effSINRs; 
  eCQIType cqiType;
  TransmissionMode_E txMode;
  vec mcsRate;
  vec instantMcsRate;
  Array<ivec> mcsIndxPerCodeword;
  Array<ivec> tbSizeInBytesPerCodeword;
//   ivec pmi;
//   type1PMI_S wbPMIIndStruct;
  vector < vector<cmat> > precoderPerRB;
  ivec nLayers;
  ivec vrbs;
  Array<imat> rbStructurePerRB;
  bool isPaired;
  int carrierIndex;
  double pfMetric;
  MUMIMOPairingInfo_S MUMIMOPair;
  
  ScheduledUEInfo_S(){
    
    ueid.set_length(0);
    symPos.set_size(0);
    cqiType=CQIType_UNINIT;
    txMode=_TRANSMISSION_MODE_UNINIT_;
    mcsRate.set_length(0);
    instantMcsRate.set_length(0);
    mcsIndxPerCodeword.set_size(0);
        tbSizeInBytesPerCodeword.set_size(0);
    //pmi.set_size(0);
    precoderPerRB.resize(0);
    nLayers.set_length(0);
    vrbs.set_length(0);
    rbStructurePerRB.set_length(0);
    isPaired=false;
    carrierIndex=-1;
    pfMetric = -1.0;
    }
  
};

struct ScheduledRBsInfo_S{
  
  //Per UE
  ivec ueIDs;
  ivec RanksPerUE;
  Array<ivec> scheduledRBsPerUE; // Array over UEs
  Array< Array<vec> > effSINRsPerUE; // Array over UEs
  Array<LayerInfo_S> layersInfoPerUE;
  
  
  //Per RB
  ivec rbNumbers;
  Array<ivec> scheduledLayersPerRB; 
  Array<ivec> scheduledUEsPerRB; // Array over RBs
  Array<imat> rbStructurePerScheduledRB; // Array over RBs
  
  ivec pairingPositionPerUE;
  
  ScheduledRBsInfo_S()
  {
    ueIDs.set_length(0);
    RanksPerUE.set_length(0);
    scheduledRBsPerUE.set_length(0);
    effSINRsPerUE.set_length(0); 
    pairingPositionPerUE.set_length(0);
    
    rbNumbers.set_length(0);
    scheduledUEsPerRB.set_length(0); 
    rbStructurePerScheduledRB.set_length(0); 
  }
  
  void getScheduledRBsInfo(int ue, Array<vec> &effSINR , Array<imat> &rbStructures)
  {
    effSINR.set_length(0);
    rbStructures.set_length(0);
    
    if(ueIDs.length())
    {
      int ueIndx=find(ueIDs,ue);
      if(ueIndx!=-1)
      {
	effSINR=effSINRsPerUE(ueIndx);
	ivec scheduleRBIndices=find(rbNumbers,scheduledRBsPerUE(ueIndx));
	rbStructures=getFromArray(rbStructurePerScheduledRB,scheduleRBIndices);
      }
    }
  }
  ivec getScheduledRBs(int ue)
  {
      ivec output(0);
      if(ueIDs.length())
      {
          int ueIndx=find(ueIDs,ue);
          if(ueIndx!=-1)
          {
              output=scheduledRBsPerUE(ueIndx);
          }
      }
      return output;
  }
  void setScheduledRBsInfo(int ue, int nLayers, ivec rbs, Array<vec> effSINR , Array<imat> rbStructures)
  {
    if(ueIDs.length())
    {
      int ueIndx=find(ueIDs,ue);
      if(ueIndx!=-1)
      {
	append(scheduledRBsPerUE(ueIndx),rbs);
	append(effSINRsPerUE(ueIndx),effSINR);
    
      }
      else
      {
	append(ueIDs,ue);
    append(RanksPerUE,nLayers);
	append(scheduledRBsPerUE,rbs);
	append(effSINRsPerUE,effSINR);
    
      }
      
      ivec existingRBIndices=find(rbNumbers,rbs);
      ivec newRBs=rbs(find(existingRBIndices==-1));
      existingRBIndices=remove(existingRBIndices,-1);
      
      if(existingRBIndices.length())
      {
	for(int cnt=0;cnt<existingRBIndices.length();cnt++)
	{
	  if(find(scheduledUEsPerRB(existingRBIndices(cnt)),ue)!=-1) {cout<<"Error. RB already scheduled for the UE."<<endl;abort();}
	  append(scheduledUEsPerRB(existingRBIndices(cnt)),ue);
	  append(scheduledLayersPerRB(existingRBIndices(cnt)),nLayers);
	  //RB structure not updated for already existing RB
	}
      }
      if(newRBs.length())
      {
	append(rbNumbers,newRBs);
	append(rbStructurePerScheduledRB,getFromArray(rbStructures,find(rbs,newRBs)));
      
	Array<ivec> temp1(newRBs.length());
	for(int rb_cnt=0;rb_cnt<newRBs.length();rb_cnt++)
	  temp1(rb_cnt)=to_ivec(ue);
	append(scheduledUEsPerRB,temp1);
      
	Array<ivec> temp2(newRBs.length());
	for(int rb_cnt=0;rb_cnt<newRBs.length();rb_cnt++)
	  temp2(rb_cnt)=to_ivec(nLayers);
	append(scheduledLayersPerRB,temp2);
      }
    }
    else
    {
      append(ueIDs,ue);
      append(RanksPerUE,nLayers);
      append(scheduledRBsPerUE,rbs);
      append(effSINRsPerUE,effSINR);
      
      
      append(rbNumbers,rbs);
      append(rbStructurePerScheduledRB,rbStructures);
      
      scheduledUEsPerRB.set_length(rbs.length());
      for(int rb_cnt=0;rb_cnt<rbs.length();rb_cnt++)
	scheduledUEsPerRB(rb_cnt)=to_ivec(ue);
      
      scheduledLayersPerRB.set_length(rbs.length());
      for(int rb_cnt=0;rb_cnt<rbs.length();rb_cnt++)
	scheduledLayersPerRB(rb_cnt)=to_ivec(nLayers);
    }
  }
  void updateLayersMapInfo(int dmrsType,int maxLength);
  void assignCDMPerUE(ivec &CDMPerUE, ivec &nCDMsPerUE,ivec& CDMMapLengths,int dmrsType,int maxLength);
  inline void checkDMRSportOverlapping();
};

enum PairingMethod_E 
{
    _UNINIT_PAIRING_=-1,_IDEAL_PAIRING_,_ZERO_FORCING_,_SUCCESSIVE_MMSE_,_BLOCK_DIAGONALISATION_
};

class Scheduler 
{
  
private:
  ivec mUEIds;
  Array<sBeamPairLink> beamPair;
  Waveform_E mWaveform=_Waveform_OFDMA_;
  vec mUEAvgRate;
  bool isCQIAvailable=false;
  Array<sCqiList> cqiList;
//   ivec mContendingUEsInCurrentSubframe;
  ivec mScheduledUEsInCurrentSubframe;
  ivec mBestSubbandIndexList;
  vec mMcsRatesPerScheduledUEs;
  bool mIsPF;
  double mPFTc;
  PowerControlInfo_S powerControlInfo;
  ivec TPCcmdFieldperUE;
  sBWPartInfo mBWPartInfo;
  cTBTables *mTBTables;
  FrameStructure *mFrameStructure;
  Array<Codebook*> mCodebookPerBeam;
  Codebook* mCodebook;
  bool mIsDownlink;
  FrameReservationInfo_S *mFrameReservationInfo;
  PortInfo_S *mPortsInfo;
  int mRBGCount;
  int mRBGSize;
  int mPRBBundlingSize;
  int mRoundRobinCounter;
  int mPFCounter;
  int mCarrierIndex;
  ivec mUEAntennaCount;
  Array<ivec> pairedUEsInCurrentSubframe;
  
  int nSymbolsForMinimumTimeAllocation;
  Array<ivec> symbolsPerTTI;
  
  //Flags
  bool mIsEPDCCHEnabled;
  bool isSemiOpenloopScheme;
  bool mIsSubbandCQIEnabled;
  bool mIsMUMIMOEnabled;
  bool mForceMUMIMO=true;
  
  int defaultCQI; // 
  
//   bool isToRetainMUMIMOPairs=false;
//   bool isToUseMUMIMOPairs=false;
//   Array<MUMIMOPairingInfo_S> retainedMUMIMOPairs;

  bool mRetainMUMIMOPairsOverFeedbackPeriod=false;
  bool isToRetainMUMIMOPairs=false;
  bool isToUseMUMIMOPairs=false;
  Array<MUMIMOPairingInfo_S> retainedMUMIMOPairs;
  
  bool mIsModeAdaptationEnabled;
  // flag for reciprocity precoder use enble/ disable
  bool useReciproPrecode;
  
  bool isCQIPortAvailable=false;
  bool isTRSPortAvailable=false;
  TRSConfig_S mTrsConf;
  bool updateThroughputEachRBG;
  
  int nUserPairing;
  
  PairingMethod_E pairingMethod;
  int nCRSPorts;
  Array<TransmissionMode_E> dlTransmissionMode;
  Array<TransmissionMode_E> ulTransmissionMode;
  
  int nDlLayers;
  int nUlLayers;
  
  ivec eNodeBAntennaCountPerBeam;
  int eNodeBAntennaCount;
  ivec ueAntennaCount;
  
  double eNodeBTxSCPowerIndBm;
  vec UETxSCPowerIndBm;
    
  int cellID;
  int dmrsType;
  int dmrsLength;
  ivec nScheduledRBsPerUE;
  
 public:
     void initScheduler(string configFilename, string l2ConfigFilename, int cellid, ivec associatedUEIDs, Array< sBeamPairLink > beamPairLink, ivec antennaCountPerBeam, ivec srvcNodeAntenna, double serverNodeTxSCPowerIndBm, vec serviceNodeTxSCPowerIndBm, cTBTables* tbTables, FrameStructure* frameStructure, FrameReservationInfo_S* frameReservationInfo, PortInfo_S* portsInfo, Array< Codebook* > codebook, sBWPartInfo bwPartInfo, int RBGSize, int prbBundlingSize, bool isDownlink, PowerControlInfo_S powControlInfo = PowerControlInfo_S());
  SchedulingInfo_S schedule(int subframe, int beamID, Array< UEBufferInfo_S >& tUEData, OuterLoopLinkAdaptation_S& tOuterLoopLinkAdaptation, int eNBID, bool isCQIportAvailable = 0, bool forceContiguousAllocation = 0,ivec TPCcmdField_="", TRSConfig_S mTRSConf = TRSConfig_S(),string muMIMOInfoFileName="",string schInfoFilename="");
  ivec fFindUniqueValues(ivec tUEIds);

  ScheduledUEInfo_S fFindBestUEusingSubbandCSI(int subframe, ivec& contendingUEs, ivec symbolsToSchedule, Array< sCqiList >& cqiList, ScheduledRBsInfo_S& scheduledRBsInfo, sScheduleReturn& schReturn, int eNBAntennaCount, Array< ivec > vrbNumbers, OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, ivec& scheduledRBGIndices, int& selectedRBG, ivec invalidUEsForSUMIMO, int eNBID);
  ScheduledUEInfo_S fFindBestUEsFornonSlotusingSubbandCSI(int subframe, ivec& contendingUEs, ivec& miniSlotIndxPerUE, Array< sCqiList >& cqiList, ScheduledRBsInfo_S& scheduledRBsInfo, sScheduleReturn& schReturn, int eNBAntennaCount, Array< ivec > vrbNumbers, OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, ivec& scheduledRBGIndices, int& selectedRBG, ivec invalidUEsForSUMIMO, int eNBID);
  ScheduledUEInfo_S fFindBestUEusingRR(int subframe, ivec contendingUEs, ivec symbolsToSchedule, Array< sCqiList >& cqiList, ScheduledRBsInfo_S& scheduledRBsInfo, sScheduleReturn& schReturn, int eNBAntennaCount, Array< ivec > subbands, int selectedSB, OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, int eNBID);
  ScheduledUEInfo_S fFindBestUEsForMUMIMOusingSubbandCSI(int subframe, ivec& contendingUEs, ivec symbolsToSchedule, Array< sCqiList >& cqiList, ScheduledRBsInfo_S& scheduledRBsInfo, sScheduleReturn& schReturn, int eNBAntennaCount, Array< ivec > vrbNumbers, OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, ivec& scheduledRBGIndices, int& selectedRBG, int eNBID, Array< ivec >& pairedUEs, Array< vec >& SINRInDBOfPairedUEs,int beamID,string PairingFileName);
  //   ScheduledUEInfo_S fFindBestUEforULNewFornonSlot(ivec& contendingUEs, ivec &miniSlotIndxPerUE,Array< sCqiList >& cqiList, ivec& scheduledUEsPerRB, Array< ivec >& cqiForScheduledUEsPerRB, Array< vec >& effSINRsForScheduledUEsPerRB, Array< imat >& rbStructureForScheduledUEsPerRB, sScheduleReturn& schReturn, ivec vrbNumbers, OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, int prevUEId);
//   ScheduledUEInfo_S fFindBestUEforULNew(ivec& contendingUEs, Array< sCqiList >& cqiList, ivec& scheduledUEsPerRB, Array< ivec >& cqiForScheduledUEsPerRB, Array< vec >& effSINRsForScheduledUEsPerRB, Array< imat >& rbStructureForScheduledUEsPerRB, sScheduleReturn& schReturn, ivec vrbNumbers, ivec tsymbols, OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, int prevUEId);

  
//   scheduledUEInfo_S fFindBestUEforULNewFornonSlot(ivec& contendingUEs, ivec &miniSlotIndxPerUE,Array< sCqiList >& cqiList, ivec& scheduledUEsPerRB, Array< ivec >& cqiForScheduledUEsPerRB, Array< vec >& effSINRsForScheduledUEsPerRB, Array< imat >& rbStructureForScheduledUEsPerRB, sScheduleReturn& schReturn, ivec vrbNumbers, OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, int prevUEId);
//   scheduledUEInfo_S fFindBestUEforULNew(ivec& contendingUEs, Array< sCqiList >& cqiList, ivec& scheduledUEsPerRB, Array< ivec >& cqiForScheduledUEsPerRB, Array< vec >& effSINRsForScheduledUEsPerRB, Array< imat >& rbStructureForScheduledUEsPerRB, sScheduleReturn& schReturn, ivec vrbNumbers, ivec tsymbols, OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, int prevUEId);

//   
  
//   ScheduledUEInfo_S SemiOLSUMIMOScheduler(ivec& contentingUEs, ivec tsymbols, Array< sCqiList >& cqiList, ivec scheduledUEsPerRB, Array< ivec >& cqiForScheduledUEsPerRB, Array< imat >& rbStructureForScheduledUEsPerRB, sScheduleReturn& schReturn, int txAntennaCount, Array< ivec > vrbNumbers, OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, ivec& scheduledRBGIndices, int& selectedRBG, ivec invalidUEsForSUMIMO, int eNBID);
//   ScheduledUEInfo_S SemiOLMUMIMOScheduler(ivec& contentingUEs, ivec tsymbols, Array< sCqiList >& cqiList, ivec scheduledUEsPerRB, ivec& isMUMIMOPerRB, Array< ivec >& cqiForScheduledUEsPerRB, Array< imat >& rbStructureForScheduledUEsPerRB, sScheduleReturn& schReturn, int txAntennaCount, Array< ivec > vrbNumbers, OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, ivec& scheduledRBGIndices, int& selectedRBG, int eNBID);
//   
  void fFindMUMIMOPairingOptionsForNR(Array<MUMIMOPairingInfo_S>& MUMIMOPairingOptions,Array< UEInfo_S >& tUEInfoPerUEs, Array<ivec> &pairedUEs);

  void fFindMUMIMOPairingOptions(Array<MUMIMOPairingInfo_S>& MUMIMOPairingOptions,Array< UEInfo_S >& tUEInfoPerUEs, Codebook* mCodeBook, int eNBAntennaCount, TransmissionMode_E txMode, Array< ivec >& pairedUEs, Array< vec >& SINRInDBOfPairedUEs);
  void fRemoveInvalidUEsForMUMIMO(Array<UEInfo_S>& tUEInfoPerUEs, ivec invalidUEsForMUMIMO, TransmissionMode_E txMode);
//   void fSearchMUMIMOPairingForTM5(Array<MUMIMOPairingInfo_S>& MUMIMOPairingOptions, Array<UEInfo_S>& tUEInfoPerUEs, Codebook& mCodeBook, int txAntennaCount, int nUserPairing);
//   void fSearchMUMIMOPairingForTM9(Array<MUMIMOPairingInfo_S>& MUMIMOPairingOptions, Array<UEInfo_S>& tUEInfoPerUEs, Codebook& mCodeBook, int txAntennaCount, int nUserPairing);
  void fSearchMUMIMOPairingForNR(Array< MUMIMOPairingInfo_S >& MUMIMOPairingOptions, Array< UEInfo_S >& tUEInfoPerUEs, Codebook* mCodeBook, int eNBAntennaCount, Array<ivec> &pairedUEs , Array<vec> &SINRInDBOfPairedUEs);
  
  void fFindRank1UEs(Array<UEInfo_S>& tUEInfoPerUEs);
  void fFindRank2UEs(Array<UEInfo_S>& tUEInfoPerUEs);
  ivec fFindBestSubbandIndexList(ivec &contendingUEs, Array<sCqiList> &cqiList, ivec txAntennaCount, Array<ivec> vrbNumbers,OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation); 
  ScheduledUEInfo_S fFindBestUECoMP(ivec &contendingUEs, Array<sCqiList> &cqiList, ivec &scheduledUEsPerRB, Array<ivec> &cqiForScheduledUEsPerRB,Array<imat> &rbStructureForScheduledUEsPerRB,sScheduleReturn &schReturn, ivec txAntennaCount, ivec vrbNumbers, int &roundRobinIndx,OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, ivec COMP_UEIDs);    
  LinkAdaptation_S fDoLinkAdaptation(int UEid, int nLayers, TransmissionMode_E txMode,ivec rbs, ivec symbolsToSchedule, Array< imat >& possibleRBStructures, Array< vec > possibleEffSINRsInDB, OuterLoopLinkAdaptation_S& tOuterLoopLinkAdaptation, bool isCQIReported = false);
  UEInfo_S fCollectUEInfo(sCqiList cqiList, TransmissionMode_E txMode, ivec vrbNumbers);

  vec fCollectAvgRate(ivec& contendingUEs);
  sCqiList fGetCQI(int ueID)
  {
    int indx=find(mUEIds,ueID);
    if(indx!=-1)
      return cqiList(indx);
    else
    {
      cout<<"Unknown UE to fGetCQI().. "<<endl;
      abort();
    }
    
  }
  void fUpdateCQI(int ueID, sCqiList ueCQI)
  {
    int indx=find(mUEIds,ueID);
    if(indx!=-1)
      cqiList(indx)=ueCQI;
    else
    {
      cout<<"Unknown UE to fUpdateCQI().. "<<endl;
      abort();
    }
    isCQIAvailable=true;
    
    if(mRetainMUMIMOPairsOverFeedbackPeriod)
      isToRetainMUMIMOPairs=true;
  }
  void fSetCarrierIndex(int nCC)
  {
    mCarrierIndex = nCC;
  }
  bool fGetSchdulerType()
  {
    return(mIsPF);
  }
   void fAddUE(int tUEId);
  int fFindInVec(vector< int > arg1, int arg2);
  void fRemoveUE(int tUEId);
  
  FrameReservationInfo_S* fGetFrameReservationInfo()
  {
    return mFrameReservationInfo; 
  }
  
  void setMUMIMOPairUpdateFlag(bool value){ isToRetainMUMIMOPairs=value; }
  void clearMUMIMOPairs(){ retainedMUMIMOPairs.set_length(0); }
  
  
private:
  ivec fgetRBsScheduled(vector< vector< bool > > arg1, vector<int> arg2, int arg3);
  int fFindNumOfRBsScheduled(vector< vector< bool > > arg1, vector< int > arg2, int arg3);
  int fFindDataAllocated(Array< ivec > arg1, int arg2);
  int fFindDataAllocated(Array< ivec > arg1, int arg2, int arg3);
  CQIFeedback_S fGetCQIInfo(sCqiList &cqiList, ivec prbs);
  CQIFeedback_S fGetDLCQIInfo(sCqiList& cqiList, ivec prbs);
  CQIFeedback_S fGetULCQIInfo(sCqiList &cqiList, ivec prbs);
//   CQIFeedback_S fGetCQIInfo(sCqiList& cqiList, ivec prbs, int nCodeWords);
//   CQIFeedback_S fGetCQIInfoCoMP(sCompCqiList& cqiList, ivec prbs);
  TransmissionMode_E fGetTMMode (sCqiList& cqiList);
  bool fCheckForFFTSizes(int arg1);
  void fUpdateAvgThroughput(ivec contentedUEs,int selectedUE, double possibleMCSRate);
  void fUpdateAvgThroughputV2(ivec contentedUEs ,ivec selectedUEs, vec possibleMCSRate);
  int fUpdateSchInfo(sScheduleReturn& schReturn, Array< ivec >& schInfo, int cwdIndx, sUEswithData& tUEData, int ueid, ivec vrbs, TransmissionMode_E txMode, int mcs,int nLayers, int pmi, int dataSendableInBytes, int tIndx,bool paired=false);
  
  ivec fGetContendingUEList(Array<UEBufferInfo_S>& tUEBufferInfoList);
  ivec fGetContendingUEList(Array<UEBufferInfo_S>& tUEBufferInfoList, ivec& tUEs);
  void fUpdateUEBufferInfo(Array< UEBufferInfo_S >& tVec, ScheduledUEInfo_S tSchInfo);
  void fUpdateSchReturn(sScheduleReturn& tSch, ScheduledUEInfo_S tSchInfo);
  int fGetBufferSize(Array<UEBufferInfo_S>& tVec, int tUEId);
  int fFindIndex(Array<UEBufferInfo_S>& tVec, int tVal);
  
  int fGetUEAntennaCount(int tUEId);
  double fGetUESCpowerIndBm(int tUEId);
  ivec fGetUEIds()
  {
    return mUEIds;
  }
  void updateRBInfo(SchedulingInfo_S& schInfo);
};

Array<LayerInfo_S> getLayersInfo(ivec mUEIds, ivec nLayers, ivec nCDM, ivec CDM,int dmrsType,int maxLength);
bool isValidPair(Array< ivec >& pairedUEs, ivec newPair, int nUserPairing, PairingInfo_S& pairingInfo,int dmrsType,int maxLength);
bool isValidPair(Array< ivec >& pairedUEs, ivec newPair, int nUserPairing, PairingInfo_S& pairingInfo,int dmrsType,int maxLength,ivec nLayersforNewPair,Array<ivec>& nLayersPerPairedUEs);
void printMUMIMOpairingInfo(ScheduledUEInfo_S& schUEInfo,int subframenumber,int eNBID,int beamID, int RBG,string MUMIMOLogsfilename);
void printSchInfoLogs(SchedulingInfo_S& schedulingInfo,int subframenumber,int eNBID,int beamID,int nRBs, int nMUMIMO, int nSUMIMO,double avgRankPerSB,string schInfoLogsFileName);
Array<cmat> getZFMuMimoPrecoders(Array<cmat>& precodersOfComb, bool &isZFpossible);
  
PairingMethod_E getPairingMethod(string name);
#endif // OLDSCH_H
