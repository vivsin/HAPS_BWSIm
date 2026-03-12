/*************************************************************************
*
* CEWiT CONFIDENTIAL
* __________________
*
* All Rights Reserved © 2012 CEWiT, India
*
* NOTICE: All information contained herein is, and remains the property of Center of Excellence in Wireless Technology (CEWiT) and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT. Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior, express, printed and signed license for use is strictly forbidden.
*/

#ifndef MAINFUNC_H
#define MAINFUNC_H


#include "../../Mcell/include/MCell.h"
#include "../../Mcell/include/FadingChannel.h"
#include "NodeDevice.h"
#include <lib/L1/PHY/include/PowerControl.h>


/** File included for L2 Support */
#include <lib/L2/MAC/include/Others/L2SimConfig.h>


// AssociationInfo_S associateNodes(SystemInfo_S &mySysInfo, Array<ivec> &strongServerNodesPerServiceNode,ivec NodeCount);
AssociationInfo_S associateNodesBasedOnRSRPFormula(SystemInfo_S &mySysInfo, Array<ivec> &strongServerNodesPerServiceNode,ivec NodeCount);

// void transmitPrachSubframe(Array< TxFrameInfo_S >& myTxFrameInfo, Array< PrachTxFrameInfo_S >& prachTxFrameInfo, ivec txFrameInfoIndex);
//bool receivePrachSubframe(RxFrameInfo_S &rxFrameInfo,Receiver *myReceiver,SystemInfo_S &mySysInfo ,McellSystem &mySys, AssociationInfo_S &associationInfo,Array<TxFrameInfo_S> &myTxFrameInfo, StrongAndWeakTransmittersInfo_S &myStrongAndWeakTransmittersInfo, bool isDownlink, SchedulerToReceiverInfo_S &mySchedulerToReceiverInfo,Size_S physicalFrameSize, int subframeNumber,bool forceReception=false,ivec forceScheduledIDs="");


void nodeTransmit(SystemInfo_S& mySysInfo, Array< NodeDevice >& myNodeDevices, ivec nodeDeviceIDs, McellSystem& mySys, Array< TxFrameInfo_S >& myTxFrameInfo, ivec txNodeIDs, int nuID, ivec txBeamIDs);
cvec convolve(cvec x,cvec y);
void passThroughFilter(Array<cvec> &timeDomainFrame,cvec filterTaps);
cvec getFilterTaps(int nFFT,ivec xAllocation, double scSpacingInHz ,int Filterlen);
cvec hannWindow(int length);
cvec getSincFilter(double bandwidth, double centreFreq, double scSpacing, int Filterlen, int fftSize);
bool receiveSubframe(RxFrameInfo_S& rxFrameInfo, Receiver* myReceiver, SystemInfo_S& mySysInfo, McellSystem& mySys, AssociationInfo_S& associationInfo, StrongAndWeakTransmittersInfo_S& myStrongAndWeakTransmittersInfo, TransmissionType txType, bool isScheduled, double currentTimeInSec, SchedulerToReceiverInfo_S mySchedulerToReceiverInfo, int rxBeamID, int forceReception, ivec forceScheduledIDs = "");

bool isToEnterReceiveLoop(Receiver* myReceiver, bool isScheduled, SystemInfo_S& mySysInfo, AssociationInfo_S& associationInfo, bool isDownlink);
// void manageDLCQIReporting(Receiver *myReceiver,int subframeNumber,CQIInfo_S &dlCQIInfo);
// void manageDLCQIReportingForABS(Receiver *myReceiver,int subframeNumber,CQIInfo_S &dlCQIInfo,int carrierInd);

void initLinkSimulation(SystemInfo_S& mySysInfo, ChannelCloud& myChannelCloud, Array< NodeDevice >& myNodeDevices, PerformanceMetrics_S& dlPerformanceMetric, PerformanceMetrics_S& ulPerformanceMetric, int snrLoop_cnt);
void completeSimulation(double currentTimeInSec=0);
void completeSimulation(string configFileDir,string resultDir,double currentTimeInSec);
void printOutputFiles(double currentTime, PerformanceMetrics_S& dlPerformanceMetric, PerformanceMetrics_S& ulPerformanceMetric, PerformanceMetrics_S& slPerformanceMetric, SystemInfo_S& mySysInfo, McellSystem& mySys, AssociationInfo_S& associationInfo, int snrLoop_cnt);

#ifdef USING_MPI
void filterServerNodesForMPI(ivec &centralUnitsToConsiderForScheduling,AssociationInfo_S &associationInfo,SystemInfo_S &mySysInfo);
void exchangeMPIDataNew(Array< NodeDevice >& nodeDevices, ivec nodeDeviceIDs, ivec serverNodes, AssociationInfo_S& associationInfo, SystemInfo_S& mySysInfo, Array< SchedulerOutput_S >& schedulerOutput, int nuID, int beamID);
void downloadMPIDataNew(Array< NodeDevice >& nodeDevices, ivec nodeDeviceIDs, ivec serverNodes, Array< SchedulerOutput_S >& schedulerOutput, AssociationInfo_S& associationInfo, MPIInfo_S& mpiInfo, int nuID, int beamID);
#endif


void preInitializeSCMLinks(McellSystem& mySys, ivec rxNodes, int carrierCount, Array< StrongAndWeakTransmittersInfo_S >& strongAndWeakTransmittersInfo);

void assignRandomAntennas(McellSystem& mySys,AssociationInfo_S &associationInfo,SystemInfo_S &mySysInfo);

//void updateSSBReservationInfo(ssBurstSetConfig ssbConfig,FrameReservationInfo_S *cchReservationInfo,int nRBS, int subframeNumber, Array<ivec> dlPrbMapping);
    
void updateFrameReservationInfo(Array< NodeDevice >& nodeDevices, ivec nodeDeviceIDs, ivec serverNodes, int nuID, bool isDownlink);

bool isActiveUE(PerformanceMetrics_S* performanceMetric, int UEid, int nuID, int startSubframeNumber, int subframeNumber);

Array< RSInfo_S > getRSInfoForUnscheduled(Array< NodeDevice >& nodeDevices, ivec nodeDeviceIDs, ivec requiredNodes, int nuID, bool isDownlink);

void nodePrepare(Array< SchedulerOutput_S >& schedulerOutput, AssociationInfo_S& associationInfo, Array< NodeDevice >& myNodeDevices, ivec nodeDeviceIDs, ivec nodeIDs, bool isWithAbstraction, ABSTRACTION_MODEL AbstractionModel, int nuID, double currentTime, ivec beamIDs, bool isForTransmission);

Array< ReTxInfo_S > getUEReTxInfo(NodeDevice& nodeDevice, int nuID, int subframeNumber, TransmissionType txType, int beamID);

ivec getDLULSymbsForCellSpecificConfig(int ttiCount,ivec CellConfig, int CellConfigPeriodicityInSlots);
ivec getAntennaCountForBeam(McellSystem& mySys, ivec nodeIDs, int nuID, ivec beamIDs);
int getAntennaCountForBeam(McellSystem &mySys, int nodeID, int nuID, int beamID);
ivec getTXRUsForBeam(McellSystem &mySys, int nodeID, int nuID, int beamID);
#endif

