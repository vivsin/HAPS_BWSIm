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

#ifndef _NODE_DEVICE_H_
#define _NODE_DEVICE_H_

#include "Transmitter.h"
#include "Receiver.h"
// #include "../../lib/L1/include/PowerControlStructs.h"
#include <lib/L1/PHY/include/PowerControl.h>

struct DownlinkHARQInfo_S
{
  bvec HARQPerCodeword;
  int ULTxsubFrameIndex;
  
  DownlinkHARQInfo_S()
  {
    HARQPerCodeword.set_size(0);
//     CCEIndex=-1;
    ULTxsubFrameIndex=-1;
  }
};

class NodeDevice{
 
  
  public:
    std::vector<PoAlphaSet_S> poAlphaSet;//[length];//outside somewhere to have all values then one will be used as per index in do_ulpowcont, this is to set values
    
    int nodeID;
    ivec nuIDs;
    ivec beamIDs;
    
    CQIMeasurementConfig_S tCQIMeasurementConfig;
    Array<Receiver> receiverPerNU;
    Array<Transmitter> transmitterPerNU;
    ivec tranmissionBeamID;
    
    Array<L1Core_S> L1CorePerNU;
    Array<L2Core_S> L2CorePerNU;
    
    ivec actualLoopCountPerNU;
    ivec ttiCountPerNU;
  
    ssBurstSetConfig SSBurstSetConfig;
    
    map<int,DownlinkHARQInfo_S> HARQHistory;
    map<int,double> pucchPower;
    
//     map<int,DownlinkHARQInfo_S> HARQHistory;
//     map<int,double> pucchPower;
//     PUCCHCommonParam_S pucchCommonParams;
//     PUCCHUESpecificSeq_S pucchseq;
  
    
    NodeDevice();
    //fills the HARQHistory buffer for the given subframe and it will be valid for next twenty subframes
    void setDLHarqStatus(int sfNumber, bvec HARQPerCodeword)
    {
      int sfNumberIndx=sfNumber%20;//to limit HARQHistory size to 20
      DownlinkHARQInfo_S ulharqInfo;
      ulharqInfo.HARQPerCodeword=HARQPerCodeword;
      HARQHistory[sfNumberIndx]=ulharqInfo;
    }
    //it reads HARQHistory buffer for the given subframe
    DownlinkHARQInfo_S getDLHarqStatus(int reportingSubFrameNumber)
    {  
      int HARQSubFrame=reportingSubFrameNumber - 4;//considering only FDD, Hard-coded since it is fixed for FDD
      
      int HARQSubFrameIndx=HARQSubFrame%20;//to limit HARQHistory size to 20
      std::map<int,DownlinkHARQInfo_S>::iterator it=HARQHistory.find(HARQSubFrameIndx);
      if(it!=HARQHistory.end())
	return it->second;
      DownlinkHARQInfo_S harqStatus; 
      return harqStatus;
    }
    // erase the HARQ buffer after 20 subframes 
    void eraseDLHARQStatus(int reportingSubFrameNumber)
    {
      
      int HARQSubFrame=reportingSubFrameNumber - 4;//considering only FDD, Hard-coded since it is fixed for FDD
      
      int HARQSubFrameIndx=HARQSubFrame%20;//to limit HARQHistory size to 20
      std::map<int,DownlinkHARQInfo_S>::iterator it=HARQHistory.find(HARQSubFrameIndx);
      if(it!=HARQHistory.end())
	HARQHistory.erase(it);
      
    }
    int getNUIndx(int nuID)
    {
      return(find(nuIDs,nuID));
    }
    // updates the pucch power for each UE
    void setPucchPower(int subFrameNumber,double powerPower)
    {
      int sfNumberIndx=subFrameNumber%20;
      pucchPower[sfNumberIndx]=powerPower;
    }
    // reads pucch power for the given subframe
    double getPucchPower(int subFrameNumber)
    {
      int SubFrameIndx=subFrameNumber%20;//to limit HARQHistory size to 20
      std::map<int,double>::iterator it=pucchPower.find(SubFrameIndx);
      if(it!=pucchPower.end())
	return it->second;
      double pucchPowerDefault=-1000;//ue should not transmit pucch  
      return pucchPowerDefault;
    }
    // collects the CSI bits(CQI,PMI,RI) for the given subframe
//     bvec getCurrentCSIBits(int subframeNumber, int nuID=0)//Function is not using for only uplink scenario
//     {  
//       int nuIndx=getNUIndx(nuID);
//       bvec CSIBits(0);
//       if(receiverPerNU(nuIndx).cqiEstimator.downlinkCQIInfo->perUserFeedback.isValid && receiverPerNU(nuIndx).isToReportDLCQI(subframeNumber))//CQI to update the CQI at the reporting subframe
//       {
// 	if(receiverPerNU(nuIndx).cqiEstimator.downlinkCQIInfo->perUserFeedback.isRankFeedback)
// 	{
// 	  CSIBits=dec2bin(receiverPerNU(nuIndx).cqiEstimator.downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(0).rankPeriodic);
// 	}
// 	else //assuming CQI is Reported in adifferent subframe than rank
// 	{
// 	  if(receiverPerNU(nuIndx).cqiEstimator.downlinkCQIInfo->perUserFeedback.isWideBandFeedback)
// 	    CSIBits=receiverPerNU(nuIndx).cqiEstimator.downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(0).widebandCQIPDUPerGroupPeriodic;
// 	  else
// 	    CSIBits=receiverPerNU(nuIndx).cqiEstimator.downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(0).subbandCQIPDUPerGroupBWPart;
// 	}
//       }
//       return(CSIBits);
//     }

    void setPrePSINRForLinkLevel(double prePSINRindB, int nuID=0)
    {
      //int nuIndx=getNUIndx(nuID);
//       receiverPerNU(0).cqiEstimator.geometrySINRindB = prePSINRindB;
    }
    Size_S getTTISize(bool isDownlink, int nuID){
      
      int nuIndx=getNUIndx(nuID);
      Size_S physicalFrameSize;
      if(isDownlink)
      {
	physicalFrameSize.numberOfSymbols=L1CorePerNU(nuIndx).dlFrameStructure.getFrameSize().numberOfSymbols;// Assuming all transmitters have same frame size..
	physicalFrameSize.numberOfSubcarriers=L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getFFTSize();
      }
      else
      {
	physicalFrameSize.numberOfSymbols=L1CorePerNU(nuIndx).ulFrameStructure.getFrameSize().numberOfSymbols;// Assuming all transmitters have same frame size..
	physicalFrameSize.numberOfSubcarriers=L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getFFTSize();
      }
      return(physicalFrameSize);
    }
};



Array< Array< Transmitter* > > getTransmitterPointer(Array< NodeDevice >& ServerNodeDevices, Array< NodeDevice >& ServiceNodeDevices, UlDlServerNodes_S TDDserverNodes, Array< ivec > associatedServiceNodesPerServerNode, ivec serverNodes, ivec serviceNodes);
Array< Array< Receiver* > > getReceiverPointer(Array< NodeDevice >& ServerNodeDevices, Array< NodeDevice >& ServiceNodeDevices, UlDlServerNodes_S TDDserverNodes, Array< ivec > associatedServiceNodesPerServerNode, ivec serverNodes, ivec serviceNodes);
Array< Array< Transmitter* > > getTransmitterPointer(Array< NodeDevice >& nodeDevices);
Array< Array< Receiver* > > getReceiverPointer(Array< NodeDevice >& nodeDevices);
Array< Transmitter* > getTransmitterPointer(Array<NodeDevice> &nodeDevices, ivec nodeIDs, ivec txNodes, int nuID);
Array< Receiver* > getReceiverPointer(Array< NodeDevice >& nodeDevices, ivec nodeIDs, ivec rxNodes, int nuID);
void getCurrentTxRxNodes(ivec &txNodes, ivec &rxNodes,UlDlServerNodes_S TDDserverNodes,Array< ivec >associatedServiceNodesPerNU,ivec serverNodes,ivec serviceNodes);
void moveUserNodes(McellSystem& mySys, SystemInfo_S& mySysInfo, AssociationInfo_S& associationInfo, Array< NodeDevice >& myNodeDevices, ivec nodeDeviceIDs, double currentTime, ivec serviceNodes="");
void reassociateNode(int serviceNodeID, int newServerNodeID, int oldServerNodeID, SystemInfo_S& mySysInfo, McellSystem& mySys, AssociationInfo_S& associationInfo, Array< NodeDevice >& myNodeDevices, ivec nodeDeviceIDs);
// void moveUserNodeByGivenDistance(int serviceNodeID, McellSystem& mySys, SystemInfo_S& mySysInfo, AssociationInfo_S& associationInfo, Array< CentralUnit >& myCentralUnits, Array< NodeDevice >& myNodeDevices, ivec nodeDeviceIDs, Array< CQIInfo_S >& cqiInfo, double currentTime, double distance);
ivec getLoopCount(ivec nodeDeviceIDs, Array<NodeDevice> &nodeDevices, ivec nodeIDs, int nuID);
ivec getTTICount(ivec nodeDeviceIDs, Array<NodeDevice> &nodeDevices, ivec nodeIDs, int nuID);
void updateSpecialSubFrameInfo_DwPts(Array< NodeDevice >& nodeDevices, ivec nodeDeviceIDs, int nuID, int serverNode, int actualLoopCount, int nDwpts);
void updateSpecialSubFrameInfo_UpPts(Array< NodeDevice >& nodeDevices, ivec nodeDeviceIDs, int nuID, int serverNode, int actualLoopCount, int nUpPts);
void updateFlexibleSubFrameInfo_DL(Array< NodeDevice >& nodeDevices, ivec nodeDeviceIDs, int nuID, int nodeID, int actualLoopCount, int nDwpts, ivec SFI);
void updateFlexibleSubFrameInfo_UL(Array< NodeDevice >& nodeDevices, ivec nodeDeviceIDs, int nuID, int nodeID, int actualLoopCount, int nUpPts, ivec SFI);
void splitBWP(NodeDevice &nodeDevice, SystemInfo_S &mySysInfo);

// void scheduleUsersNew(Array< SchedulerOutput_S >& schedulerOutput, SystemInfo_S mySysInfo, AssociationInfo_S& associationInfo, Array< NodeDevice >& nodeDevices, ivec nodeDeviceIDs, CQIInfo_S* cqiInfo, ivec dlServerNodes, ivec ulServerNodes, ivec serverNodesToSchedule, ivec ttiPerServerNode, int nuID);

#endif
