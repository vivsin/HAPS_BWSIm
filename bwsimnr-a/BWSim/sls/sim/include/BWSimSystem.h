/*************************************************************************
 * 
 * CEWiT CONFIDENTIAL
 * __________________
 *
 * All Rights Reserved © 2014 CEWiT, India
 *
 * \ NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
 * and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
 * Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
 * express, printed and signed license for use is strictly forbidden.
 */

#ifndef MAINFUNC5G_H
#define MAINFUNC5G_H


#include "MainFunctions.h"
// #include "../4G/src/DLCCHsupport/dlcchsupport.h"
// #include "../4G/src/UCISupport/PUCCHSupportingmodules.h"
// #include "../sim/src/CompSupport/CompSupport.h"


#ifdef USING_L3
#include <lib/L3Support/include/L3interface.h>
#endif

#ifdef USING_X2
#include <lib/X2Support/include/X2Interface.h>
#endif
/*
#include <lib/PrachSupport/include/Cmplex.h>
#include <lib/PrachSupport/include/PRACH_CONST.h>
#include <lib/PrachSupport/include/Zc_functions.h>
#include <lib/PrachSupport/include/PRACH_LUT.h>*/


#define WRAP2(X) #X
#define WRAP1(X) WRAP2(X)
#define WRAP(X) WRAP1(X)

class BWSimSystem_S{

public:
  //Top Modules
  SystemInfo_S mySysInfo;
  McellSystem mySys;
  AssociationInfo_S associationInfo;
  Array<NodeDevice> myNodeDevices;
  PerformanceMetrics_S dlPerformanceMetric,ulPerformanceMetric,slPerformanceMetric;
  ivec nodeDeviceIDs;
  
  int snrLoopCount;
 
  // Loop Variables
  int snrLoop_cnt;
   
  
  //Temp Variables 1
//   vec downlinkSIRInDBPerServiceNode;
  Array< SchedulerOutput_S > schedulerOutput;
//   Array<CQIInfo_S> cqiInfoForScheduler;
//   Array<ABSparameters_S> ABSparameters;
  mat linkGainInDB;
  ivec srvrNodesToConsiderForScheduling;
  
  
  //Temp Variables 2 
  Array<TxFrameInfo_S> myTxFrameInfo; 
 
  BWSimSystem_S(){
    
    snrLoop_cnt=0;
//     myTxFrameInfo.set_length(0);
  }
  void initializeSystem(int argc, char** argv);
  void initializeMcell(int argc, char** argv);
private:
  void initAllNodeDevice();
  void initCoreModules(int nuCaID);
  void loadCarrierAssociationInfo(string linkTableFileName);
};

// void performLinkSpecificAntGainAssociation(McellSystem& mySys, AssociationInfo_S& associationInfo, string linkTableFileName, Array <ivec >  &strongServerNodesPerServiceNode);

// void getIntialMeasurements(ivec cellIds, ChannelMatrix_S& channelMat, int subFrameNum, ivec& portLocations, AssociationInfo_S& associationInfo, vec txSubcarrierPowersindBm, Array< PerCellL1Measurements >& measurements);
// 
// double getCurrentTime(int currentFrame,int currentSymbol,double subframeDuration,int fftSize ,double samplingFrequencyInHz);

void nodeUpdate(BWSimSystem_S& bwsimSystem, ivec serverNodes, int nuID);

ivec nodeScheduleData(BWSimSystem_S& bwsimSystem, ivec serverNodes, double currentTime, int nuID, int beamID);

void scheduleSidelink(BWSimSystem_S& bwsimSystem, ivec serverNodes, double currentTime, int nuID, int beamID);

void nodeScheduleRS(BWSimSystem_S& bwsimSystem, ivec serverNodes, double currentTime, int nuID, int beamID);

void nodeTransmit(BWSimSystem_S& bwsimSystem, ivec txNodeIDs, int nuID, ivec txBeamIDs);

Array<TxFrameInfo_S> nodeGenerate(BWSimSystem_S &bwsimSystem, ivec txNodeIDs, double currentTime, int nuID, ivec txBeamIDs);

Array<StrongAndWeakTransmittersInfo_S> getStrongAndWeakTransmittersInfo(BWSimSystem_S& bwsimSystem, int carrier_cnt, ivec rxNodeIDs ,double currentTime);

ivec nodeReceive(BWSimSystem_S& bwsimSystem, ivec rxNodeIDs, int nuID, double currentTime, ivec& rxBeamIDs);

void nodeProcess(BWSimSystem_S& bwsimSystem, ivec rxNodeIDs, double currentTime, int nuID, ivec rxBeamIDs);

void nodeReportHARQ(BWSimSystem_S& bwsimSystem, int srvcNode, int srvrNode, int ttiCount, TransmissionStatus_S &txStatus, TransmissionType txType, int nuID);

void nodeReportDLCQI(BWSimSystem_S& bwsimSystem, int srvcNode, int srvrNode, int ttiCount, PerUserDLCQIFeedback_S dlCQI, int nuID);

void nodeReportULCQI(BWSimSystem_S& bwsimSystem, int srvcNode, int srvrNode, int ttiCount, PerUserULCQIFeedback_S ulCQI, int nuID);

void printOutputFiles(BWSimSystem_S& bwsimSystem, double currentTime, int snrIndx=0);

void associateNodesBasedOnRSRPFormula(McellSystem& mySys, AssociationInfo_S& associationInfo, string linkTableFileName, Array <ivec > strongServerNodesPerServiceNode);

// void printAngleSpread(McellSystem& mySys, AssociationInfo_S associationInfo);
void performLinkGainBasedAssociation(ChannelCloud* channelCloud, AssociationInfo_S& associationInfo, SystemInfo_S& mySysInfo);
Array<vec> computeSingularValuesPerRB(Array <Array <cvec>>& channel, vec tapDelay,NUParams_S* nuParams,int txAntennaCount,int rxAntennaCount);

void nodeTransmitSSB(BWSimSystem_S &bwsimSystem, ivec txNodeIDs,ivec txBeamIDs,double currentTime);
void nodeReceiveSSB(BWSimSystem_S &bwsimSystem, ivec rxNodeIDs,double currentTime);
double get_nextSSBlockTime(ssBurstSetConfig& ssbConfig,bool isSSBlockTime);


#endif

