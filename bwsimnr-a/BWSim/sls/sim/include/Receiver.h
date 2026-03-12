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

#ifndef _RX_H_
#define _RX_H_

#include "StructsAndEnums.h"
#include "../../Mcell/include/FadingChannel.h"
#include <lib/L1/PHY/include/CQIEstimatorNR.h>
#include <lib/L1/PHY/include/ChannelandIntrCovEstimator.h>
// #include <lib/ControlChannelSupport/include/DownlinkControlChannels.h>
#include <lib/L1/SSB/include/DLSyncDetector.h>
// #include <lib/ControlChannelSupport/include/PBCHRxChain.h>
// #include <lib/ControlChannelSupport/include/PCFICHRxChain.h>
// #include <lib/ControlChannelSupport/include/PDCCHRxChain.h>
// #include <lib/ControlChannelSupport/include/PHICHReceiver.h>
// #include <lib/ControlChannelSupport/include/UplinkControlChannels.h>


class NodeDevice;//defined in NodeDevice.h
class Receiver{

  protected:
	int nodeID;
	int antennaCount;
  public:
	ReceptionConfig_S receiverConfig;
	Framehandler frameHandler;
	ChannelEstimator channelEstimator;
	CQIEstimatorNR cqiEstimator;
	NUParams_S nuParams; 
	Array<DownlinkCQIInfo_S> downlinkCQIInfo; // Array size 2 for ABS/Non-ABS
	UplinkCQIInfo_S uplinkCQIInfo;
	ivec HARQModelServiceNodes;
// 	PHICH_Duration phichDuration;
// 	PHICH_Resource phichResource;
	NodeDevice *myNodeDevice;
	bool isAPIForCRSBasedChannelEstimation;
	bool isAPIForDMRSBasedChannelEstimation;
	
	RxFrameInfo_S rxFrameInfo;
	bool enterReceiveLoop;
	
  public:
// 	Receiver(NodeDevice * devPointer=NULL);
	Receiver();
	void enableLinkAdaptation();
	void disableLinkAdaptation();
	void enableEstimateInterference();
	void disableEstimateInterference();
	void idealChannelMode(bool istrue);
	bool isIdealChannel();
	int getCQIComputationPeriodicity();
	bool isLinkAdaptationEnabled();
	bool isModeAdaptationEnabled();
	bool isSubbandUser();
	void loadConfig(ReceptionConfig_S &rxConfig);
	void setTxNodesToReceiveSignalFrom(ivec txNodes);
	void setCentralUnitIndx(int centralUnitIndx);
	void setFrameHandler(Framehandler &frameHandler);
    void initReceiver(int rxID, int antCount, L1Core_S& coreModules, bool isServerNode, UlDlMode_E ulDlMode, bool isWithAbstraction);
	int getRxNodeID();
	void setRxAntennaCount(int count);
	int getRxAntennaCount();
	ivec getTxNodesToReceiveSignalFrom();
	void selectDownlinkCQIInfo(int carrierInd,bool isABSInfo=false);
	void estimateChannelOverFrequencyPartition1(Array<cmat> &rxFrame, SchedulerToReceiverInfo_S &schedulerToReceiverInfo,Array< Array<cmat> > &estimatedMyChannel, Array< Array< Array<cmat> > > &estimatedInterferenceChannelOverFrequencyPartition ,Array<cmat> &estimatedInterferenceCovariance ,ChannelCloud &myChannelCloud, Array<TxFrameInfo_S> &txFrameInfo, ivec subFrames ,bool estimateInterference );
	EqualizerOutput_S processReceivedRB(RxFrameInfo_S& myRxFrameInfo, bool isDownlink, SchedulerToReceiverInfo_S& schedulerToReceiverInfo, ChannelCloud& myChannelCloud, Array< Array< cmat > >& wideBandChannel, int myChannelIndx, int blkNumber, Size_S rbSize, double restOfInterferenceScRxPowerInWatts);
	Array<TransmissionStatus_S> processReceivedSubFrame(RxFrameInfo_S& myRxFrameInfo, SystemInfo_S& mySysInfo, bool isDownlink, PerformanceMetrics_S* performanceMetrics, SchedulerToReceiverInfo_S& schedulerToReceiverInfo, ChannelCloud& myChannelCloud, string MCSlogsFileName);
// 	Array< Array<cmat> > computeWideBandChannelOverUplinkDMRS(RxFrameInfo_S& myRxFrameInfo, SystemInfo_S& mySysInfo, bool isDownlink, PerformanceMetrics_S* performanceMetrics, Array< TxFrameInfo_S >& txFrameInfo, SchedulerToReceiverInfo_S& schedulerToReceiverInfo, ChannelCloud& myChannelCloud, string MCSlogsFileName);
	
	// 	Array<TransmissionStatus_S> abstractReceiver(RxFrameInfo_S& myRxFrameInfo, SystemInfo_S& mySysInfo, bool isDownlink, PerformanceMetrics_S* performanceMetrics, Array< TxFrameInfo_S >& txFrameInfo, SchedulerToReceiverInfo_S& schedulerToReceiverInfo, ChannelCloud& myChannelCloud, string MCSlogsFileName);
	Array<vec> ComputeSINROverRBs(bool isDownlink,ChannelHandler_S *myChannelHandler,int myChannelIndex,ivec interfererChannelIndices,ivec txNodesToReceiveSignalsFrom,double roiScRxPowInWatts,Array<TxFrameInfo_S> &txFrameInfo, SchedulerToReceiverInfo_S &schedulerToReceiverInfo ,ChannelCloud &myChannelCloud, DataBlkInfo_S& myDataBlkInfo );
	void generateFeedback(int nuID, int txBeamID, int subframeNumber, RxFrameInfo_S& myRxFrameInfo, ChannelCloud& myChannelCloud, bool isDownlink, PerformanceMetrics_S& performanceMetrics, SystemInfo_S mySysInfo, string WideBandCQIlogsFileName, string RIlogsFileName, string SubbandCQIlogsFileName, string AperiodicCQILogsFileName, double NIValueForTheServerNode = -1);
	void generateDLFeedback(CQIMeasurementConfig_S& cqiMeasurementConfig, int subframeNumber, RxFrameInfo_S& myRxFrameInfo, ChannelCloud& myChannelCloud, PerformanceMetrics_S& performanceMetrics, SystemInfo_S& mySysInfo, string WideBandCQIlogsFileName, string RIlogsFileName, string SubbandCQIlogsFileName, string AperiodicCQILogsFileName);
    ReciprocityPrecodersInfo_S generateReciprocityPrecodersForUL(CQIMeasurementConfig_S &cqiMeasurementConfig, int subframeNumber,RxFrameInfo_S &myRxFrameInfo,ChannelCloud &myChannelCloud, PerformanceMetrics_S &performanceMetrics,SystemInfo_S &mySysInfo,string WideBandCQIlogsFileName, string RIlogsFileName,string SubbadCQIlogsFileName, string AperiodicCQILogsFileName);
	
	SubbandULCQIInfo_S generateULFeedback(int serviceNode, int nuID, int subframeNumber, RxFrameInfo_S& myRxFrameInfo, ChannelCloud& myChannelCloud, double NIValueForTheServerNode, int requestID = -1);
    SubbandULCQIInfo_S updateDLCQIForReci(int serviceNode,int nuID, int subframeNumber,RxFrameInfo_S &myRxFrameInfo,ChannelCloud &myChannelCloud,double NIValueForTheServerNode, SystemInfo_S &mySysInfo, ReciprocityPrecodersInfo_S &reciprocityPrecoders, int requestID = -1); //reciprocity
	ReciprocityPrecodersInfo_S generateReciprocityPrecoders(int serviceNode, int nuID, int subframeNumber, RxFrameInfo_S& myRxFrameInfo, ChannelCloud& myChannelCloud, double NIValueForTheServerNode, int requestID = -1, bool isDownlink = 1);

// 	bool isToReportRI(int subframeNumber);
// 	bool isToReportDLCQI(int subframeNumber);
// 	Array<cvec> computeInterference(Array<cmat> &receivedFrame,Array< Array<cmat> > &channel, double txPowerInWatts,SchedulerToReceiverInfo_S &schToRxInfo);
// 	double getWidebandSINR(RxFrameInfo_S &myRxFrameInfo, SchedulerToReceiverInfo_S &schedulerToReceiverInfo,Array<TxFrameInfo_S> &txFrameInfo,ChannelCloud &myChannelCloud);
// 	bool abstractPDCCH(SystemInfo_S &mySysInfo, ChannelHandler_S *myChannelHandler, Array<TxFrameInfo_S> &txFrameInfo, SchedulerToReceiverInfo_S &schedulerToReceiverInfo ,ChannelCloud &myChannelCloud, ivec txNodesToReceiveSignalsFrom, double roiScRxPowInWatts);
	vec ComputeSINROverPDCCHsymbols(SystemInfo_S &mySysInfo,ChannelHandler_S *myChannelHandler,int myChannelIndex,ivec interfererChannelIndices,ivec txNodesToReceiveSignalsFrom,double roiScRxPowInWatts,Array<TxFrameInfo_S> &txFrameInfo, SchedulerToReceiverInfo_S &schedulerToReceiverInfo ,ChannelCloud &myChannelCloud, DataBlkInfo_S myDataBlkInfo );
// 	MIB_S processPBCH(RxFrameInfo_S &myRxFrameInfo, SystemInfo_S &mySysInfo, Array<TxFrameInfo_S> &txFrameInfo, SchedulerToReceiverInfo_S &schedulerToReceiverInfo ,ChannelCloud &myChannelCloud);
// 	int processPCFICH(RxFrameInfo_S& myRxFrameInfo, SystemInfo_S& mySysInfo, Array< TxFrameInfo_S >& txFrameInfo, SchedulerToReceiverInfo_S& schedulerToReceiverInfo, ChannelCloud& myChannelCloud, int subframeNumber,ivec& PCFICHIndicesLogical, ivec& PCFICHIndicesFFT);
// 	void processPHICH(RxFrameInfo_S& myRxFrameInfo, SystemInfo_S& mySysInfo, Array< TxFrameInfo_S >& txFrameInfo, SchedulerToReceiverInfo_S& schedulerToReceiverInfo, ChannelCloud& myChannelCloud, int subframeNumber, PHICH_Duration phichDuration, PHICH_Resource phichResource, ivec PCFICHIndicesLogical, ivec& PHICHIndices);
// 	PDCCHReception_S processPDCCH(RxFrameInfo_S& myRxFrameInfo, SystemInfo_S& mySysInfo, Array< TxFrameInfo_S >& txFrameInfo, SchedulerToReceiverInfo_S& schedulerToReceiverInfo, ChannelCloud& myChannelCloud, int subframeNumber, int ReceivedpdcchCount, ivec PCFICHIndices, ivec PHICHIndices, double PHICHNg);
// 	bool processEPDCCH(RxFrameInfo_S& myRxFrameInfo, SystemInfo_S& mySysInfo, Array< TxFrameInfo_S >& txFrameInfo, SchedulerToReceiverInfo_S& schedulerToReceiverInfo, ChannelCloud& myChannelCloud, int subframeNumber);
// 	void pucchDecoding(RxFrameInfo_S &myRxFrameInfo,SchedulerToReceiverInfo_S &schedulerToReceiverInfo,PUCCHParam_S pucchparams,vec uplinkPUCCHPowerPerSubCarrierIndBm,bool isLinklevel,vec &pucch_sfn_ber);
	//void pucchDecoding(RxFrameInfo_S &myRxFrameInfo,SchedulerToReceiverInfo_S &schedulerToReceiverInfo,PUCCHParam_S pucchparams,int rxNodeID,int subframeNum,bool isDownlink,vec uplinkPUCCHPowerPerSubCarrierIndBm,bool isLinklevel,vec &pucch_sfn_ber,vec pucch_snr_ber);
// 	Array<cmat> EstimateInterferenceCovariance(RxFrameInfo_S &myRxFrameInfo,SchedulerToReceiverInfo_S &schedulerToReceiverInfo,PUCCHParam_S pucchparams,int tx_cnt,vec uplinkPUCCHPowerPerSubCarrierIndBm);
	//Comp Receivers
// 	bvec ReedMullerDecoding(PUCCHParam_S pucchparams,vec Rx_LLRs,int N_Tx_bits);
	Array<TransmissionStatus_S> processReceivedSubFrame(Array< RxFrameInfo_S >& rxFrameInfoUnderMyCU, SystemInfo_S& mySysInfo, bool isDownlink, PerformanceMetrics_S* performanceMetrics, Array< TxFrameInfo_S >& txFrameInfo, SchedulerToReceiverInfo_S& schedulerToReceiverInfo, ChannelCloud& myChannelCloud, string MCSlogsFileName);
// 	Array<TransmissionStatus_S> abstractReceiver(Array<RxFrameInfo_S> & rxFrameInfoUnderMyCU, SystemInfo_S& mySysInfo, bool isDownlink, PerformanceMetrics_S* performanceMetrics, Array< TxFrameInfo_S >& txFrameInfo, SchedulerToReceiverInfo_S& schedulerToReceiverInfo, ChannelCloud& myChannelCloud, string MCSlogsFileName);
// 	void generateFeedback(CQIInfo_S& cqiInfo, int subframeNumber, Array< RxFrameInfo_S >& rxFrameInfoUnderMyCU, Array< TxFrameInfo_S >& txFrameInfo, ChannelCloud& myChannelCloud, bool isDownlink, PerformanceMetrics_S& performanceMetrics, SystemInfo_S mySysInfo, string WideBandCQIlogsFileName, string SubbandCQIlogsFileName, string AperiodicCQILogsFileName, double NIValueForTheServerNode = -1);
	bool isToComputeDLFeedback();

  bool
  processCoreset(RxFrameInfo_S& myRxFrameInfo,
               SchedulerToReceiverInfo_S& schedulerToReceiverInfo,
               ChannelCloud& myChannelCloud,
               int blkNumber, Size_S rbSize);
};



void checkFrameHandler(Framehandler &frameHandler, int rbNumber, Array<cmat> & receivedRB);



#endif

