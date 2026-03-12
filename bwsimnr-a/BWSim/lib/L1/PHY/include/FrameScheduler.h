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

#ifndef _FRMCSCH_H_
#define _FRMCSCH_H_

#include "ChannelCoder.h"
#include "Precoder.h"
#include "FrameStructure.h"
#include "ReferenceSignalGenerator.h"


struct RBGroupInfo_S{
  
  ivec resourceBlocks;
  Array< DataGenInfo_S > dataGenInfo;
  ivec pmiIndexPerUser;
  cmat precoder;
  RBGroupInfo_S(){
    
    resourceBlocks.set_size(0);
    dataGenInfo.set_length(0);
  }
};

struct SchedulerToTransmitterInfo_S{
  
  int txNodeID;
  int txNUID;
  bool isTransmitting;
  int cellID;
  ivec rxNodeIDs;
  ivec rsRxNodeIDs;
  int ttiCount;
  double carrierFreqInMHz;
  int txBeamID;
  TransmissionType txType;  
  
//   PHICHParam_S PHICHParamters;
  Array<DataBlkInfo_S> dataBlkInfo;
  ivec serviceNodesForSourceAndSink;
  double txSubcarrierPowerInDBmPerAntenna,rsTxSubcarrierPowerInDBmPerAntenna;
//   Array<EPDCCHParams_S> epdcchParams;
//   Array<EPDCCHCommonInfo_s> epdcchCommonParams;
  double Pb;
  
  SchedulerToTransmitterInfo_S(){
    
    txNodeID=-1;
    txNUID=-1;
    isTransmitting=true;
    cellID=-1;
    rxNodeIDs.set_length(0);
    ttiCount=-1;
    dataBlkInfo.set_length(0);
    serviceNodesForSourceAndSink.set_length(0);
    txSubcarrierPowerInDBmPerAntenna=0;
    rsTxSubcarrierPowerInDBmPerAntenna=0;
    txType=_TRANSMISSION_TYPE_UNINIT_;
   }
  
  bool isReady(){
    
    bool ready=true;
    if(txNodeID==-1){cout<<"txNodeID not set in SchedulerToTransmitterInfo_S ... "<<endl;ready=false;}
    return(ready);
  }
  
  
};


struct SchedulerToReceiverInfo_S{
  
  int rxNodeID;
  int rxNUID;
  ivec txNodeIDs,rsTxNodeIDs;
  double carrierFreqInMHz;
  int ttiCount;
  int rxBeamID;
  TransmissionType txType;  
//   PHICHParam_S PHICHParamters;
  Array<DataBlkInfo_S> dataBlkInfo;
  ivec serviceNodesForSourceAndSink;
  vec txSubcarrierPowerInDBmPerAntenna,rsTxSubcarrierPowerInDBmPerAntenna;
//   EPDCCHCommonInfo_s epdcchCommonParams;
  
  double Pb;
  SchedulerToReceiverInfo_S(){
    
    rxNodeID=-1;
    rxNUID=-1;
    rxBeamID=-1;
    txNodeIDs.set_length(0);
    rsTxNodeIDs.set_length(0);
    dataBlkInfo.set_length(0);
    serviceNodesForSourceAndSink.set_length(0);
    txSubcarrierPowerInDBmPerAntenna.set_length(0);
    rsTxSubcarrierPowerInDBmPerAntenna.set_length(0);
    txType=_TRANSMISSION_TYPE_UNINIT_;
    
  }
  bool isReady(){
  void resourceMapper();
    
    bool ready=true;
    if(rxNodeID==-1){cout<<"rxNodeID not set in SchedulerToReceiverInfo_S ... "<<endl;ready=false;}
    return(ready);
  }
  
};

struct SchedulerOutput_S{
  
  ivec txNodeIDs;
  ivec txNuIDs;
  ivec rxNodeIDs;
  ivec rxNuIDs;
  ivec txBeamIDs;
  ivec rxBeamIDs; // who is initializing these beams
  Array<SchedulerToTransmitterInfo_S> schedulerToTransmitterInfo;
  Array<SchedulerToReceiverInfo_S> schedulerToReceiverInfo;
  
  SchedulerOutput_S(){
    
    txNodeIDs.set_length(0);
    rxNodeIDs.set_length(0);
    txNuIDs.set_length(0);
    rxNuIDs.set_length(0);
    txBeamIDs.set_length(0);
    rxBeamIDs.set_length(0);
    schedulerToTransmitterInfo.set_length(0);
    schedulerToReceiverInfo.set_length(0);
  }
  void addTxForRSTransmission(int txNodeID, ivec rxNodes, int nuID, double carrierFreqInMHz, int ttiCount,double txSubcarrierPowerInDBm,TransmissionType txType, int txBeamID)
  {
    int schToTxIndx=getSchedulerToTransmitterInfoIndx(txNodeID,nuID,txBeamID);
    if(schToTxIndx==-1)
    {
      SchedulerToTransmitterInfo_S schToTxInfo;
      schToTxInfo.txNodeID=txNodeID;
      schToTxInfo.txNUID=nuID;
      schToTxInfo.carrierFreqInMHz=carrierFreqInMHz;
      schToTxInfo.ttiCount=ttiCount;
      schToTxInfo.isTransmitting=true;
      schToTxInfo.txBeamID=txBeamID;
      schToTxInfo.rsRxNodeIDs=rxNodes;
      schToTxInfo.txType=txType;
      schToTxIndx=schedulerToTransmitterInfo.length();
      append(schedulerToTransmitterInfo,schToTxInfo);
      append(txNodeIDs,txNodeID);
      append(txNuIDs,nuID);
      append(txBeamIDs,txBeamID);
      schedulerToTransmitterInfo(schedulerToTransmitterInfo.length()-1).rsTxSubcarrierPowerInDBmPerAntenna=txSubcarrierPowerInDBm;
      return;
    }
    else
    {
        if(txType==_TRANSMISSION_TYPE_DL_)
      {
	if(schedulerToTransmitterInfo(schToTxIndx).rsRxNodeIDs.length()!=0)
	{
	  cout<<"Already scheduled node can't be scheduled for RS Transmission."<<endl;abort();
	}
	schedulerToTransmitterInfo(schToTxIndx).rsRxNodeIDs=rxNodes;
	schedulerToTransmitterInfo(schToTxIndx).rsTxSubcarrierPowerInDBmPerAntenna=txSubcarrierPowerInDBm;
      }
      else
      {
	cout<<"Already scheduled node can't be scheduled for RS Transmission."<<endl;abort();
      }
    }
  }
  void addRxForRSTransmission(int rxNodeID, int txNodeID, int nuID, double carrierFreqInMHz, int ttiCount,double txSubcarrierPowerInDBm,TransmissionType txType, int rxBeamID)
  {
    int schToRxIndx=getSchedulerToReceiverInfoIndx(rxNodeID,nuID,rxBeamID);
    if(schToRxIndx==-1)
    {
      SchedulerToReceiverInfo_S schTorxInfo;
      schTorxInfo.rxNodeID=rxNodeID;
      schTorxInfo.rxNUID=nuID;
      schTorxInfo.carrierFreqInMHz=carrierFreqInMHz;
      schTorxInfo.ttiCount=ttiCount;
      schTorxInfo.rxBeamID=rxBeamID;
      schTorxInfo.txType=txType;
      append(schTorxInfo.rsTxNodeIDs,txNodeID);
      schToRxIndx=schedulerToReceiverInfo.length();
      append(schedulerToReceiverInfo,schTorxInfo);
      append(rxNodeIDs,rxNodeID);
      append(rxNuIDs,nuID);
      append(rxBeamIDs,rxBeamID);
      append(schedulerToReceiverInfo(schedulerToReceiverInfo.length()-1).rsTxSubcarrierPowerInDBmPerAntenna,txSubcarrierPowerInDBm);
      return;
    }
    else
    {
        if(txType==_TRANSMISSION_TYPE_UL_)
      {
	append(schedulerToReceiverInfo(schToRxIndx).rsTxNodeIDs,txNodeID);
	append(schedulerToReceiverInfo(schToRxIndx).rsTxSubcarrierPowerInDBmPerAntenna,txSubcarrierPowerInDBm);
      }
      else
      {
	cout<<"Already scheduled node can't be scheduled for RS Transmission."<<endl;abort();
      }
    }
  }
  void addTxDataBlk(int txNodeID, int rxNodeID, int nuID, double carrierFreqInMHz, int ttiCount, DataBlkInfo_S &dataBlkInfo,double txSubcarrierPowerInDBm,TransmissionType txType, int txBeamID)
  {
    int schToTxIndx=getSchedulerToTransmitterInfoIndx(txNodeID,nuID,txBeamID);
    
    if(schToTxIndx==-1)
    {
      SchedulerToTransmitterInfo_S schToTxInfo;
      schToTxInfo.txNodeID=txNodeID;
      schToTxInfo.txNUID=nuID;
      schToTxInfo.carrierFreqInMHz=carrierFreqInMHz;
      schToTxInfo.ttiCount=ttiCount;
      schToTxInfo.isTransmitting=true;
      schToTxInfo.txBeamID=txBeamID;
      schToTxInfo.txType=txType;
      schToTxIndx=schedulerToTransmitterInfo.length();
      append(schedulerToTransmitterInfo,schToTxInfo);
      append(txNodeIDs,txNodeID);
      append(txNuIDs,nuID);
      append(txBeamIDs,txBeamID);
    }
   
    append(schedulerToTransmitterInfo(schToTxIndx).rxNodeIDs,rxNodeID);
    append(schedulerToTransmitterInfo(schToTxIndx).dataBlkInfo,dataBlkInfo);
    if(txType==_TRANSMISSION_TYPE_DL_ || txType==_TRANSMISSION_TYPE_SL_)
      append(schedulerToTransmitterInfo(schToTxIndx).serviceNodesForSourceAndSink,rxNodeID);
    else
      append(schedulerToTransmitterInfo(schToTxIndx).serviceNodesForSourceAndSink,txNodeID);
    
    schedulerToTransmitterInfo(schToTxIndx).txSubcarrierPowerInDBmPerAntenna=txSubcarrierPowerInDBm;
    return;
  }
  
  void addRxDataBlk(int rxNodeID, int txNodeID, int nuID, double carrierFreqInMHz, int ttiCount, DataBlkInfo_S &dataBlkInfo,double txSubcarrierPowerInDBm,TransmissionType txType,int rxBeamID)
  {
    
    int schToRxIndx=getSchedulerToReceiverInfoIndx(rxNodeID,nuID,rxBeamID);
    
    if(schToRxIndx==-1)
    {
      SchedulerToReceiverInfo_S schToRxInfo;
      schToRxInfo.rxNodeID=rxNodeID;
      schToRxInfo.rxNUID=nuID;
      schToRxInfo.carrierFreqInMHz=carrierFreqInMHz;
      schToRxInfo.ttiCount=ttiCount;
      schToRxInfo.rxBeamID=rxBeamID;
      schToRxInfo.txType=txType;
      schToRxIndx=schedulerToReceiverInfo.length();
      append(schedulerToReceiverInfo,schToRxInfo);
      append(rxNodeIDs,rxNodeID);
      append(rxNuIDs,nuID);
      append(rxBeamIDs,rxBeamID);
      
    }
    
    append(schedulerToReceiverInfo(schToRxIndx).txNodeIDs,txNodeID);
    append(schedulerToReceiverInfo(schToRxIndx).dataBlkInfo,dataBlkInfo);
    if(txType==_TRANSMISSION_TYPE_DL_ || txType==_TRANSMISSION_TYPE_SL_)
      append(schedulerToReceiverInfo(schToRxIndx).serviceNodesForSourceAndSink,rxNodeID);
    else
      append(schedulerToReceiverInfo(schToRxIndx).serviceNodesForSourceAndSink,txNodeID);
    append(schedulerToReceiverInfo(schToRxIndx).txSubcarrierPowerInDBmPerAntenna,(double)txSubcarrierPowerInDBm);
    
    
  }
//   void setRBInfoForUnScheduledInTransmitter(int txNodeID, int nuID, RBInfo_S &rbInfo)
//   {
//     int indx=getSchedulerToTransmitterInfoIndx(txNodeID,nuID);
//     if(indx!=-1)
//     {
//       schedulerToTransmitterInfo(indx).rbInfoForUnScheduled=rbInfo;
//       return;
//     }
//       
//     cout<<"[both:] Undefined txNodeID to setRBInfoForUnScheduledInTransmitter()..."<<endl;  
//     abort();
//   }
  SchedulerToTransmitterInfo_S getSchedulerToTransmitterInfo(int txNodeID, int nuID, int txBeamID)
  {
    int indx=getSchedulerToTransmitterInfoIndx(txNodeID,nuID,txBeamID);
    
    if(indx!=-1)
      return(schedulerToTransmitterInfo(indx));
      
    cout<<"[both:] Unscheduled txNodeID to getSchedulerToTransmitterInfo()..."<<endl;
    abort();
  }
  SchedulerToReceiverInfo_S getSchedulerToReceiverInfo(int rxNodeID, int nuID, int rxBeamID)
  {
    int indx=getSchedulerToReceiverInfoIndx(rxNodeID,nuID,rxBeamID);
    
    if(indx!=-1)
      return(schedulerToReceiverInfo(indx));
      
    cout<<"[both:] Unscheduled rxNodeID to getSchedulerToReceiverInfo()..."<<endl;  
    abort();
  }
  void deleteSchedulerToTransmitterInfo(int txNodeID, int nuID, int txBeamID)
  {
    int indx=getSchedulerToTransmitterInfoIndx(txNodeID,nuID,txBeamID);
    
    if(indx!=-1)
    {
      deleteInArray(schedulerToTransmitterInfo,to_ivec(indx));
      txNodeIDs.del(indx);
      txNuIDs.del(indx);
      txBeamIDs.del(indx);
      return;
    }
    
    cout<<"[both:] Unscheduled txNodeID to deleteSchedulerToTransmitterInfo()..."<<endl;
    abort();
  }
  void deleteSchedulerToReceiverInfo(int rxNodeID, int nuID, int rxBeamID)
  {
    int indx=getSchedulerToReceiverInfoIndx(rxNodeID,nuID,rxBeamID);
    
    if(indx!=-1)
    {
      deleteInArray(schedulerToReceiverInfo,to_ivec(indx));
      rxNodeIDs.del(indx);
      rxNuIDs.del(indx);
      rxBeamIDs.del(indx);
      return;
    }
    
    cout<<"[both:] Unscheduled txNodeID to deleteSchedulerToTransmitterInfo()..."<<endl;
    abort();
  }
  int getSchedulerToTransmitterInfoIndx(int txNodeID, int nuID, int txBeamID)
  {
    ivec txIndices=find(txNodeIDs,txNodeID,true);
    int schToTxIndx=-1;
    if(txIndices.length())
    {
      ivec nuIndices=find(txNuIDs(txIndices),nuID,true); //Get the correct equation.
      if(nuIndices.length())
      {
        int beamIndex=find(txBeamIDs(nuIndices), txBeamID);
        if(beamIndex!=-1)
        {
            schToTxIndx=txIndices(nuIndices(beamIndex));
        }
      }
    }
    return(schToTxIndx);
  }
  int getSchedulerToReceiverInfoIndx(int rxNodeID, int nuID, int rxBeamID)
  {
    ivec rxIndices=find(rxNodeIDs,rxNodeID,true);
    int schToRxIndx=-1;
    if(rxIndices.length())
    {
      ivec nuIndices=find(rxNuIDs(rxIndices),nuID,true);
      if(nuIndices.length())
      {
        int beamIndex=find(rxBeamIDs(nuIndices), rxBeamID);
        if(beamIndex!=-1)
        {
            schToRxIndx=rxIndices(nuIndices(beamIndex));
        }
      }
    }
    return(schToRxIndx);
  }
  ivec getSchedulerToTransmitterInfoIndx(ivec txNodes, int nuID, ivec txBeamIDs)
  {
    ivec output=zeros_i(txNodes.length())-1;
    for(int tx_cnt=0;tx_cnt<txNodes.length();tx_cnt++)
      output(tx_cnt)=getSchedulerToTransmitterInfoIndx(txNodes(tx_cnt),nuID, txBeamIDs(tx_cnt)); 
    return(output);
  }
  ivec getSchedulerToReceiverInfoIndx(ivec rxNodes, int nuID,ivec rxBeamIDs)
  {
    ivec output=zeros_i(rxNodes.length())-1;
    for(int rx_cnt=0;rx_cnt<rxNodes.length();rx_cnt++)
      output(rx_cnt)=getSchedulerToReceiverInfoIndx(rxNodes(rx_cnt),nuID,rxBeamIDs(rx_cnt)); 
    return(output);
  }
 
};

struct RBMergeInfo_S{
  
  ivec rbs;
  Array<string> mergedRBName;
  
  RBMergeInfo_S(){
    
    rbs.set_length(0);
    mergedRBName.set_length(0);
  }
};




// void createDataBlkInfo(Array< DataBlkInfo_S >& dataBlkInfoPerServiceNode, SchedulerOutput_S& schedulerOutput, int srvrNodeIndx, ivec& activeServiceNodes, Array< RBGroupInfo_S >& rbGroups, Array< SchedulerInfo_S >& scheduledServiceNodesInfoPerRBGroup, RBInfo_S& rbInfoForUnscheduled, ivec defaultMCSIndices, bool isDownlink, ServiceNodesInfo_S &serviceNodesInfo,int subFrameNumber);
// void createDataBlkInfoNew(Array< DataBlkInfo_S >& dataBlkInfoPerServiceNode, SchedulerOutput_S& schedulerOutput, int srvrNode, ivec& activeServiceNodes, Array< RBGroupInfo_S >& rbGroups, Array< SchedulerInfo_S >& scheduledServiceNodesInfoPerRBGroup, RBInfo_S& rbInfoForUnscheduled, ivec defaultMCSIndices, int isDownlink, ServiceNodesInfo_S &serviceNodesInfo);
// void addDataBlksToSchedulerOutput(ServiceNodesInfo_S* serviceNodesInfo, ivec activeServiceNodes, SchedulerOutput_S& schedulerOutput, int nuID, Array< DataBlkInfo_S >& dataBlkInfoPerServiceNode, ivec serverNodes, int srvrNodeIndx, vec txPowersInDBm, double& txSubcarrierPowersInDBm, bool isDownlink, int subFrameNumber, bool isLinkSimulation = false);
// void updateUnscheduledFrame(SchedulerOutput_S &schedulerOutput, int srvrNode_cnt, int nRBs, ivec &txAntennaCount ,vec &txPowersInDBm, double &txSubcarrierPowersInDBm, RBInfo_S &rbInfoForUnscheduled,bool isDownlink);
// void updateUnscheduledFrameNew(SchedulerOutput_S &schedulerOutput, int srvrNode, int nRBs, ivec &txAntennaCount ,vec &txPowersInDBm, double &txSubcarrierPowersInDBm, RBInfo_S &rbInfoForUnscheduled,int isDownlink);
// zeroWarning case handled below
// void updateRBInfoForUnscheduled(SchedulerOutput_S &schedulerOutput, ivec serverNodes, int srvrNode_cnt, /*double txSubcarrierPowersInDBm,*/ RBInfo_S &rbInfoForUnscheduled, ivec serviceNodes, bool isDownlink);
// void scheduleFrames(SchedulerOutput_S &schedulerOutput,Array<RSInfo_S> &rsInfoForUnScheduledPerServerNode,MacScheduler &macScheduler, bool isDownlink, ivec serverNodes,bvec serverNodesActiveList, ivec txNodes, ivec txAntennaCount ,vec txPowersInDBm, int resourceBlockGroupSize, ivec idCellPerServerNode, CQIInfo_S *cqiInfo, int subFrameNumber,bool isRestrictionForReTx=false,int maximumServiceNodesPerSubframe=-1 , bool isRankAdaptationEnabled=false,bool isLinkSimulation=false,int defaultCQIIndex=1,ivec defaultMCSIndices="",bool isWithAbstraction=false,vec loadPerServerNode="");
// void scheduleFramesNew(SchedulerOutput_S &schedulerOutput, int nuID,PortInfo_S dlPortInfo,PortInfo_S ulPortInfo,Array<RSInfo_S> &dlRSInfoForUnScheduledPerServerNode,Array<RSInfo_S> &ulRSInfoForUnScheduledPerServerNode,MacScheduler &macScheduler,ivec serverNodes,bvec serverNodesActiveList,ivec dlTxNodes,ivec ulTxNodes,ivec dlTxAntennaCount,ivec ulTxAntennaCount,vec serverNodeTxPowersInDBm,vec serviceNodeTxPowersInDBm,int dlRBGroupSize,int ulRBGroupSize,ivec idCellPerServerNode,CQIInfo_S *cqiInfo, int subFrameNumber,bool isRestrictionForReTx,int maximumServiceNodesPerSubframe,bool isRankAdaptationEnabled=false,bool isLinkSimulation=false,int defaultCQIIndex=1,ivec defaultMCSIndices="",vec loadPerServerNode="",Array<ivec> reservedRBGs="");
// zeroWarning case handled here
// void scheduleRBGroups(Array< RBGroupInfo_S >& rbGroups, Array< SchedulerInfo_S >& scheduledServiceNodesInfoPerRBGroup, int serverNodeID, int srvrNodeAntennaCnt, int idCell, ivec serviceNodes, ivec srvcNodeAntennaCnt, int nRUs, int nRBGroups, /*int resourceBlockGroupSize*/ MacScheduler& macScheduler, bool isDownlink, FrameStructure &frameStructure, PortInfo_S &portInfo, ResourceMapper& resourceMapper, SourceAndSink& sourceAndSink, CQIInfo_S* cqiInfo, ivec& myServiceNodesFeedbackIndices, int subFrameNumber, Codebook* codeBook, bool isRestrictionForReTx = false,int maximumServiceNodesPerSubframe=-1, bool isRankAdaptationEnabled = false, bool isLinkSimulation = false, int defaultCQIIndex = 1, double loadInPercentage = -1,ivec reservedRBGs="");
TransmissionScheme_E getTransmissionScheme(TransmissionMode_E transmissionMode);
// Array< DataBlkInfo_S > mergeRBGroups(Array<RBGroupInfo_S> &allRbGroupsInfo, ivec serviceNodes, Array<SchedulerInfo_S> &scheduledServiceNodesInfoPerRBGroup,/*CqiToMaxMCSRate_S &cqiToMaxMCSRate,*/cLTETables *lteTables,Codebook &codebook, ServiceNodesInfo_S &serviceNodesInfo,/*int subFrameNumber,*/bool isDownlink,ivec defaultMCSIndices);
vec getTotalMCSRate(Array<vec> mcsRatesPerCodeword, Array<TransmissionConfig_S> &transmissionConfig,ivec txConfigIndices="");
RBMergeInfo_S generateRBStructure(FrameStructure* frameStructure, RSInfo_S& rsInfo, ivec vrbs, bvec useCurrentRBAsBase, bool isDownlink, bool isLocalised = 1, FrameReservationInfo_S* cchReservation = 0);
//SRSconfig_S getSRSBW(AssociationInfo_S associationInfo, int SRSbandwidthinRBs, int SRScomb, int SRSsoundingbandwidthinRBs, int SRSsoundingbandwidthFirstRB, int srvc_cnt, int SRSCount);
void loadAllResourceBlockStructures(FrameStructure& frameStructure, string rbPatternPath, int cellID, ivec rsPorts, bool isDownlink, int CSIRSconfig = 0);
void loadNRCSIRSPatterns(FrameStructure &frameStructure,string csirsPatternPath);
void loadNRSRSPatterns(FrameStructure &frameStructure,string csirsPatternPath);
void loadAllCCHPatterns(FrameStructure &frameStructure,string cchPatternPath, int cellID);
void loadPBCHPatterns(FrameStructure &frameStructure,string cchPatternPath, int cellID);
void loadShortPUCCHpattern(FrameStructure &frameStructure,string cchPatternPath);
void loadPssSssPatterns(FrameStructure &frameStructure,string cchPatternPath);
void loadSSBPatterns(FrameStructure &frameStructure,string cchPatternPath);
// void generateData(Array< SchedulerOutput_S >& schedulerOutput, Array< ServiceNodesInfo_S* > &serviceNodesInfoPerCU, int subframeNumber, bool isDownlink, bool isWithAbstraction, bool updateFrameStructure, Array< Array< Array<RSInfo_S> > > &rsInfoForUnscheduledPerServerNode);
// bool isServiceNodeEligibleForUplinkScheduling(int serviceNodeID,Array<SchedulerInfo_S> scheduledServiceNodesInfoPerRBGroup, Array<ivec> resourceUnits,int contentingRBGIndx);
// ivec getServiceNodeIDsEligibleForUplinkScheduling(ivec serviceNodeIDs,Array<SchedulerInfo_S> scheduledServiceNodesInfoPerRBGroup, Array<ivec> resourceUnits,int contentingRBGIndx);
void generateDLPowerMask(Array< DataBlkInfo_S >& dataBlkInfoPerServiceNode, FrameStructure& frameStructure, cTBTables* tbTables, double txSubcarrierPowerInDBmPerAntenna, double Pb);
int getTxSrvrNodeIndx(Array<SchedulerToTransmitterInfo_S> schedulerToTransmitterInfo ,int srvrNode);
int getRxSrvrNodeIndx(Array<SchedulerToReceiverInfo_S> schedulerToReceiverInfo ,int srvrNode);
#ifdef USING_CALIBRATION
ivec CQIToMCSForCQITesting(ivec cqiIndex);
ivec CQIToMCSForRITesting(ivec cqiIndex);
#endif

#endif
