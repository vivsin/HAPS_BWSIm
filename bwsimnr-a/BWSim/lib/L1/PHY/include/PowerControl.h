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
#ifndef _POW_CTRL_H
#define _POW_CTRL_H

#include "FrameScheduler.h"
#include "PowerControlStructs.h"
#include "AWGNTables.h"


struct powerBackoff_S
{
  
  vec edgeRB_MPR;
  vec outerRB_MPR;
  vec innerRB_MPR;
  powerBackoff_S()
  {
    edgeRB_MPR = "6.5,3.5,3.5,3.5,4.5,3.5,3.5,3.5,3.5";  // MPR values format {CP_OFDM_256QAM, CP_OFDM_64QAM, CP_OFDM_16QAM, CP_OFDM_QPSK, DFT_S_OFDM_256QAM, DFT_S_OFDM_64QAM, DFT_S_OFDM_16QAM, DFT_S_OFDM_QPSK, DFT_S_OFDM_pi/2BPSK}
    outerRB_MPR = "6.5,3.5,3,3,4.5,2.5,2,1,0.5";
    innerRB_MPR = "6.5,3.5,2,1.5,4.5,2.5,1,0,0";
  }
  double getMPR(ModulationScheme_E modulationScheme,Waveform_E waveform, ivec rballoc, int NRB, int UEclass=3);
};
  
int generateTPCcmd(PowerControlInfo_S &powerControlInfo, double NIValue, double receivedppSINR, int &currentTimeForSINR, int indx, vec &SINRdiffForK_Slots) ;// code to generate TPC command in BS
void updateNIforUplink(double &NIValuePerServerNode ,double averageInterferencePerReInWatts, PowerControlInfo_S &powerControlInfo, int subframeNumber);
// void doUplinkPowerControl(Array< SchedulerOutput_S >& schedulerOutput, vec NIValuesPerServerNode, vec downlinkSIRInDB, ivec& serverNodesForScheduling, ivec cuIDPerServerNode, int nuID, ivec ttiCountPerServerNode, ivec servernodes, ivec serviceNodes, mat serviceToServerNodeLinkGainInDB, PowerControlInfo_S& powerControlInfo, vec srsTxSubcarrierPowerInDBm, bool isSRSEnabled);

// void doUplinkPowerControl(SchedulerToTransmitterInfo_S &schedulerToTransmitterInfo,SchedulerToReceiverInfo_S &schedulerToReceiverInfo, double NIValue , double downlinkSIRInDB, ivec servernodes, ivec serviceNodes, mat serviceToServerNodeLinkGainInDB, PowerControlInfo_S &powerControlInfo,bool isSRSEnabled);
void getUplinkPower(PoAlphaSet_S& paobj, int nRBs, double maxTxPowerIndbmPerSC, double RSPIndBm, double RSRPIndBm, double TPCCmdFeild, double currentTime=0, int mu=0, bool isAbsTPC=false, bool resetTPC=false, double timeByTpc=0);
double doUplinkPowerControlNR(double maxTxPowerIndbmPerSC, ivec rbs, int totalRBs, int mcsIndx, Waveform_E waveform, int TPCcmdFieldIndividualUE, double RSPIndBm, double RSRPIndBm, PowerControlInfo_S& powerControlInfo, bool is256QAMEnabled, bool isPiby2BPSKEnabled, bool isSRSEnabled, double currentTime, int mu, vector< PoAlphaSet_S >& tpoAlphaSet);
// #ifdef USING_SRS
// void doUplinkPowerControl(Array< SchedulerOutput_S > &schedulerOutput, vec NIValuesPerServerNode , vec downlinkSIRInDB,ivec &serverNodes, ivec &serviceNodes,mat serviceToServerNodeLinkGainInDB, PowerControlInfo_S &powerControlInfo, int subFrm_cnt, UEL3SRS &ueL3SRS, vec &srsTxSubcarrierPowerInDBm,bool isSRSEnabled);
// #endif
// Array<Array<vec> > doUplinkPowerControlForPUCCH(AssociationInfo_S &associationInfo,Array< SchedulerOutput_S > &schedulerOutput,Array<Array< PUCCHParam_S > > &pucchparams,mat serviceToServerNodeLinkGainInDB, PowerControlInfo_S &powerControlInfo/*,int subFrm_cnt*/);

#endif
