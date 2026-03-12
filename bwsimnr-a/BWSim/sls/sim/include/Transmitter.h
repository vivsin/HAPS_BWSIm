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

#ifndef _TX_H_
#define _TX_H_

#include "SupportingFunctions.h"
// #include <lib/FFRSupport/FFRSupport.h>

class NodeDevice;


class Transmitter{

  protected:
	int nodeID;
	int antennaCount;

  public:
	NUParams_S nuParams;
	Framehandler frameHandler;
	NodeDevice *myNodeDevice;
	
  public:
// 	Transmitter(NodeDevice * devPointer=NULL);
	void setNodeID(int nodeId);
	void setAntennaCount(int antCount);	
	int getAntennaCount(); 

	Size_S getPhysicalFrameSize();
	int getNodeID();
	void initTransmitter(int nodeid, int antCount, NUParams_S& nuparams);
	TxFrameInfo_S transmitFrame(SystemInfo_S& mySysInfo, bool isDownlink, SchedulerToTransmitterInfo_S schedulerToTransmitterInfo);
 	void fillDLControlChannels(Array<cmat>& txFrame,SchedulerToTransmitterInfo_S& schedulerToTransmitterInfo,SystemInfo_S& mySysInfo, int subframeNumber,int nDLRBs);
};


Array<cmat> mapLogicalToPhysicalSubFrame(Array<cmat> logicalSubFrame, int fftSize , Size_S rbSize, ResourceMapper *resourceMapper,bool isDownlink/*,int subframeNumber,bool enableUplinkFreqHopping=false*/,ivec uplinkScheduledRBs="");
// void createAci(MultiOpInfo multiopInfo,Array<TxFrameInfo_S> &myTxFrameInfo,ivec txNodes);
ivec getTxframeIndx(ivec NodeIds, ivec beamIDs, int nuID, Array< TxFrameInfo_S >& myTxFrameInfo);
// ivec getTxframeIndx(ivec NodeIds,Array<TxFrameInfo_S> myTxFrameInfo);
// void generateDataNew(Array< SchedulerOutput_S >& schedulerOutput, Array< NodeDevice >& myNodeDevices, ivec nodeDeviceIDs, bool isWithAbstraction, ivec txNodeIDs, int nuID);

// Array<TxFrameInfo_S> generateTxFrame(Array<Transmitter *> &myTransmitters, Array<SchedulerOutput_S > &schedulerOutput, SystemInfo_S &mySysInfo, bool isDownlink,int subframeNumber);
Array<TxFrameInfo_S> nodeGenerate(Array< Transmitter* >& myTransmitters, AssociationInfo_S& associationInfo, Array< SchedulerOutput_S >& schedulerOutput, int nuID, SystemInfo_S &mySysInfo, ivec txNodes, ivec txBeamIDs);
// #include <lib/ControlChannelSupport/include/DownlinkControlChannels.h>
// #include <lib/ControlChannelSupport/include/RBStartPositionsForPHICH.h>

#endif


