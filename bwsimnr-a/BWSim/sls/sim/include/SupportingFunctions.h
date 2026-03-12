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

#ifndef _SF_SLS_H_
#define _SF_SLS_H_

#include "StructsAndEnums.h"
#include <lib/L1/PHY/include/FrameHandler.h>
#include <lib/L1/PHY/include/FrameScheduler.h>
#include <lib/Frozen/mcell/include/Channel.h>

TransmissionConfig_S parseTxConfig(string fileName, string l2fileName, bool isDownlink, bool isSSBEnabled = false);
ReceptionConfig_S parseRxConfig(string fileName,bool isDownlink);

CodeBookConfig_S parseDlCodeBookConfig(string fileName1, string fileName2, TransmissionMode_E transmissionMode,string AASfileName, int nBeams);

// For AWGN curves
void parseSystemInfoAWGN(SystemInfo_S &mySysInfo);
ivec getFrameInfoIndx(Array<TxFrameInfo_S> &myTxFrameInfo, ivec txNodes);
int getFrameInfoIndx(Array<TxFrameInfo_S> &txFrameInfo, int txNode);
RSInfo_S parseRSInfo(ConfigFileNames_S &configFileNames);
void checkConfigFiles(TransmissionConfig_S txConfig, int nTxAntennas, bool isDlmode);
double getEndTime(double currentTime, int currentSymbol, int fftSize, double samplingFrequencyInHz);
double getCurrentTime(double currentTime,int currentSymbol,int fftSize ,double samplingFrequencyInHz);
void removeCyclicPrefix(Array<cvec> &timeDomainSignal,int fftSize);
void interruptCatch(int sigNumber);
void segmentationFaultCatch(int sigNumber);
void printScheduledUEInfo(string filename,Array<SchedulerOutput_S> &schedulerOutput,int subFrm_cnt);
void createFrameStructure(string fileName ,string frameName,int nRBs, double tslots);
void assignNodeVelocity(McellSystem &mySys,string sysConfigFileName);
void cropFilesForMaximumServiceNodes(McellSystem &mcellSystem, string linkTableFileName, string nodeLocationsFileName, ivec serverNodes="", ivec serviceNodes="");
void PrintMCSindices(int nodeID,int eNBid,int nLayers, ivec MCSindices, bvec isNewTrans, int subframenumber, vec ppSINRperCWindB, int nRB, bvec isSuccess, ivec tbSize, string MCSlogsFileName);
void printIoTvalues(int serverNodeID, int SubframeNumber,double NIvalueInCurrSubframe, double avgNIvalue, double instantIoT, double avgIoT,string dirName);
vec getDownLinkSIRInDBForUplinkPowerControl(ChannelCloud &myChannelCloud,AssociationInfo_S associationInfo);
void checkForSimulationCompatability(SystemInfo_S& mySysInfo);
double printPostProcessingSINRForCoExistence(/*int rxNodeID,*/Array<cvec> receivedDataSymbols,Array<cvec> transmittedDataSymbols,Array< Array<cvec> > myPrecodedChannelPerRB/*,ivec dataTones,float restOfInterferenceScRxPowerInWatts,ChannelCloud &myChannelCloud*/);
double computesubFrameDuration(int fftSize , double subcarrierSpacingInHz , int nOfdmSymbols);
sBWPartInfo initBWPartInfo(string dlTxRxConfigFileName, NUParams_S& nuParams);
Array < StrongAndWeakTransmittersInfo_S > getStrongAndWeakTransmittersInfo(ivec rxNodes, SystemInfo_S& mySysInfo, McellSystem& mySys, Array< SchedulerOutput_S >& schedulerOutput, AssociationInfo_S associationInfo, double currentTime);
// void updateSSBReservationInfo(ssBurstSetConfig ssbConfig,FrameReservationInfo_S *cchReservationInfo, double bwpSCS, double currentTimeinmilliseconds);
void updateCCHReservationInfo(CCHInfo_S cchInfo,FrameReservationInfo_S *cchReservationInfo, double bwpSCS, double currentTimeinmilliseconds,int nRBS, int subframeNumber, bool isDownlink=false, Array<ivec> dlPrbMapping="");
void setREIndxForTM3Precoding(Size_S rbSize, DataBlkInfo_S &dataBlkInfo);    
void getTDDScaling(SystemInfo_S& mySysInfo, vec& dlTDDScaling, vec& ulTDDScaling);
int getServerIndx(AssociationInfo_S& associationInfo,int nodeID);


ModulationScheme_E parseModulationScheme(string fileName,string variableName);

void initAllFiles(int argc, char** argv, ConfigFileNames_S &myConfigFiles , ResultFileNames_S &resultFiles);
    

#endif
