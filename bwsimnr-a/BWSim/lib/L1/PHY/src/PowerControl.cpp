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

#include "../include/PowerControl.h"
// extern ivec powerHeadroomNodeIDs; 
// extern vec powerHeadroomInDBPerNode;
// extern ivec powerHeadroomInDBPerNode_subframeCount;

/*!\brief NIValuesPerServerNode(power/subcarrier) is computed by BS & broadcast to UEs for power control.
 * NIValuesPerServerNode Values also used for computing CQI at BS
 * TODO calculate NIValuesPerServerNode values for different partitions */
void updateNIforUplink(double &NIValuePerServerNode ,double averageInterferencePerReInWatts, PowerControlInfo_S &powerControlInfo, int subframeNumber)
{
  //! Computing NIInstantaneous as noise+interference power in current subframe...
  double NIInstantaneous = averageInterferencePerReInWatts;
  double windowSize=powerControlInfo.windowSizeForNI;  
  
  if(NIValuePerServerNode==0 || subframeNumber==1)
    NIValuePerServerNode = NIInstantaneous;
  else                              //! For all other subframes, we r taking running average...
    NIValuePerServerNode = (1.0-1.0/windowSize)*NIValuePerServerNode + (1.0/windowSize)*NIInstantaneous;

}


// void doUplinkPowerControl(SchedulerToTransmitterInfo_S &schedulerToTransmitterInfo,SchedulerToReceiverInfo_S &schedulerToReceiverInfo, double NIValue , double downlinkSIRInDB, ivec servernodes, ivec serviceNodes, mat serviceToServerNodeLinkGainInDB, PowerControlInfo_S &powerControlInfo,bool isSRSEnabled)
// {
//   double maxTxPowerInWatts;
//   int txNodeID=-1, rxNodeID=-1;
//   double channelGainInDB;
//   int txNodeIndx, rxNodeIndx;
//   double uplinkPowerPerSubCarrierInWatts;
//   int txIndx;
//   //   double totalPowerOfUE;
//   double desiredPowerInWatts;
//   double desiredSRSPowerInWatts,uplinkSRSPowerInWatts ;
//   
//   int subFrm_cnt=schedulerToTransmitterInfo.ttiCount;
//   if(schedulerToTransmitterInfo.dataBlkInfo.length()!=0)		//! If UE is Scheduled
//   {
//     txNodeID = schedulerToTransmitterInfo.txNodeID;	
//     
//     txNodeIndx=find(serviceNodes,txNodeID);
//     int M=schedulerToTransmitterInfo.dataBlkInfo(0).resourceBlocks.length();
//     
//     //!Here only one Receiver is assumed (need to change for COMP case)
//     //cout<<"rxNodeIDs : "<<schedulerOutput(centralUnitIDs(cu_cnt)).schedulerToTransmitterInfo.rxNodeIDs.length()<<endl;
//     if(schedulerToTransmitterInfo.rxNodeIDs.length()!=1)
//     {cout<<"[both:] Please update power in all Sch-To-Rx info in uplink power control"<<endl;exit(0);}
//     
//     rxNodeID = schedulerToTransmitterInfo.rxNodeIDs(0);
//     rxNodeIndx=find(servernodes,rxNodeID);
//     channelGainInDB= serviceToServerNodeLinkGainInDB(rxNodeIndx,txNodeIndx);
//     //cout << "channelGainInDB: " << txNodeIndx << " " << rxNodeIndx << " " << channelGainInDB << endl;
//     //       uplinkPowerPerSubCarrierInWatts=inv_dB(downlinkSIRInDB(txNodeIndx) - channelGainInDB + dB(NIValuesPerServerNode(rxNodeIndx)));
//     if(powerControlInfo.powerControlMethod == _POW_CONTROL_LTE_) 
//     {
//       int fValueForPowerControl;
//       
//       if(schedulerToTransmitterInfo.dataBlkInfo(0).isAbsTPC)
// 	fValueForPowerControl=schedulerToTransmitterInfo.dataBlkInfo(0).uplinkPowerControlCorrectionValInDB;
//       else
// 	fValueForPowerControl=powerControlInfo.fValueForPowerControl+schedulerToTransmitterInfo.dataBlkInfo(0).uplinkPowerControlCorrectionValInDB;
//       
//       
//       //! deltaTF Computation //////////
//       // 	    int totalInfoBits=0;
//       // 	    for(int cwd_cnt=0;cwd_cnt<schedulerToTransmitterInfo.dataBlkInfo(0).dataGenInfo.nCodeWords;cwd_cnt++)
//       // 	    {
//       // 	      int tbindex=schedulerToTransmitterInfo.sourceAndSink->lteTables.transportBlockMCSMapping.transportBlockIndexPDSCH(schedulerToTransmitterInfo.dataBlkInfo(0).dataGenInfo.mcsIndices[cwd_cnt]);
//       // 	      int tbSize=schedulerToTransmitterInfo.sourceAndSink->lteTables.getTBSizeForAllLayersCombined(tbindex,schedulerToTransmitterInfo.dataBlkInfo(0).resourceBlocks.length(),getNumLayersPerCodeWord(schedulerToTransmitterInfo.dataBlkInfo(0).dataGenInfo.nCodeWords,schedulerToTransmitterInfo.dataBlkInfo(0).dataGenInfo.nLayers)(cwd_cnt));
//       // 	      
//       // 	      int tbSizeWithCRC=tbSize+24;
//       // 	      ivec codeBlocksSizes=schedulerToTransmitterInfo.sourceAndSink->lteTables.getCodeBlockSizes(tbSizeWithCRC);
//       // 	      totalInfoBits+=sum(codeBlocksSizes);
//       // 	    }
//       // 	    int nREs=0;
//       // 	    Array<imat> rbStructurePerRB=getMergedRBStructures(schedulerToTransmitterInfo.frameStructure,schedulerToTransmitterInfo.dataBlkInfo(0).dataGenInfo.rbInfo.rsInfo.rsPorts,schedulerToTransmitterInfo.dataBlkInfo(0).resourceBlocks,false,true,schedulerToTransmitterInfo.frameReservationInfo);
//       // 	    for(int rb_cnt=0;rb_cnt<rbStructurePerRB.length();rb_cnt++)
//       // 	      nREs+=count(rbStructurePerRB(rb_cnt),0);
//       // 	    
//       // 	    double BPRE=(double)totalInfoBits/(double)nREs;
//       // 	    double betaPUSCH=1;
//       // 	    double Ks=schedulerToTransmitterInfo.dataBlkInfo(0).KsForPowerControl;
//       // 	    if(schedulerToTransmitterInfo.dataBlkInfo(0).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_CL_SM_)
//       // 	      Ks=0;
//       // 	    
//       double deltaTF=0;
//       // 	    if(Ks==0) 
//       // 	      deltaTF=0;
//       // 	    else
//       // 	      deltaTF=10*log10((pow(2.0,BPRE*Ks)-1.0)*betaPUSCH);      	    
//       // 	    
//       //!/////////////////////////////////
//       maxTxPowerInWatts=inv_dB(schedulerToTransmitterInfo.txSubcarrierPowerInDBmPerAntenna-30)*M*12;
//       
//       desiredPowerInWatts=inv_dB(powerControlInfo.targetPowerForLTEPowerControlindBm-30 + 10*log10(M) - powerControlInfo.pathlossCompensation*channelGainInDB + deltaTF + fValueForPowerControl);
//       
//       if(desiredPowerInWatts <= maxTxPowerInWatts) // Updating f_PUSCH(i) only if the max power bound is satisfied
//       {
// 	powerControlInfo.fValueForPowerControl=fValueForPowerControl;
// 	uplinkPowerPerSubCarrierInWatts=desiredPowerInWatts/(M*12);
//       }
//       else //i.e. using f_PUSCH(i-1) for power control and not updating
//       {
// 	desiredPowerInWatts=inv_dB(powerControlInfo.targetPowerForLTEPowerControlindBm-30 + 10*log10(M) - powerControlInfo.pathlossCompensation*channelGainInDB + deltaTF + powerControlInfo.fValueForPowerControl);
// 	if(desiredPowerInWatts <= maxTxPowerInWatts)
// 	  uplinkPowerPerSubCarrierInWatts=desiredPowerInWatts/(M*12);
// 	else
// 	  uplinkPowerPerSubCarrierInWatts=maxTxPowerInWatts/(M*12);
//       }
//       
//       // 	  if(isSRSEnabled)
//       // 	  {
//       // 	      SRSSequence srsSequence=ueL3SRS.generateSequence(txNodeID,subFrm_cnt);//Assuming antenna=port 
//       // 	      
//       // 	      double pSRS_OffSet=0.0;
//       // 	      int pSRS_OffSet_Value = randi(0,15);
//       // 	      if(Ks==0) 
//       // 		pSRS_OffSet = -10.5 + 1.5*pSRS_OffSet_Value;
//       // 	      else if(Ks==1.25)
//       // 	      {
//       // 		pSRS_OffSet = 3; 
//       // 	      }
//       // 	      
//       // 	      if(srsSequence.sequencePerPort.length()!=0)
//       // 	      {
//       // 		int srsRBs=srsSequence.sequencePerPort(0).length()*2/12;//12 is no.of subcarriers per RB
//       // 		double maxSRSTxPowerInWatts=inv_dB(schedulerOutput(centralUnitIDs(cu_cnt)).schedulerToTransmitterInfo.txSubcarrierPowerInDBmPerAntenna-30)*srsRBs*12;
//       // 		desiredSRSPowerInWatts=inv_dB(powerControlInfo.targetPowerForLTEPowerControlindBm-30 + pSRS_OffSet + 10*log10(srsRBs) - powerControlInfo.pathlossCompensation*channelGainInDB + fValueForPowerControl);
//       // 		if(desiredSRSPowerInWatts <= maxSRSTxPowerInWatts)
//       // 		  uplinkSRSPowerInWatts=desiredSRSPowerInWatts/(srsRBs*12);
//       // 		else
//       // 		  uplinkSRSPowerInWatts=maxSRSTxPowerInWatts/(srsRBs*12);
//       // 		
//       // 		srsTxSubcarrierPowerInDBm(txNodeIndx) = dB(uplinkSRSPowerInWatts)+30; 	
//       // 		
//       // 		schedulerOutput(centralUnitIDs(cu_cnt)).schedulerToTransmitterInfo.srsTxSubcarrierPowerInDBmPerAntenna = dB(uplinkSRSPowerInWatts)+30; 
//       // 		schedulerToRxIndx=schedulerOutput(centralUnitIDs(cu_cnt)).getSchedulerToReceiverInfoIndx(rxNodeID);
//       // 		txIndx=find(schedulerOutput(centralUnitIDs(cu_cnt)).schedulerToReceiverInfo(schedulerToRxIndx).txNodeIDs,txNodeID);
//       // 		append(schedulerOutput(centralUnitIDs(cu_cnt)).schedulerToReceiverInfo(schedulerToRxIndx).srsTxSubcarrierPowerInDBmPerAntenna,dB(uplinkSRSPowerInWatts)+30);
//       // 	      }
//       // 	  }	  
//     }
//     else if(powerControlInfo.powerControlMethod == _POW_CONTROL_TargetSINR_)
//     {
//       maxTxPowerInWatts=inv_dB(schedulerToTransmitterInfo.txSubcarrierPowerInDBmPerAntenna-30)*M*12;
//       desiredPowerInWatts=inv_dB(max(powerControlInfo.minimumSINRInDB,downlinkSIRInDB) - powerControlInfo.pathlossCompensation*channelGainInDB + dB(NIValue) /*+ offsetForPowerControl*/);
//       if(desiredPowerInWatts <= maxTxPowerInWatts)
// 	uplinkPowerPerSubCarrierInWatts=desiredPowerInWatts/(M*12);
//       else
// 	uplinkPowerPerSubCarrierInWatts=maxTxPowerInWatts/(M*12);
//       // cout<<"downlinkSIRInDB ="<<downlinkSIRInDB(txNodeIndx)<<"\t channelGainInDB ="<<channelGainInDB<<"\t NIValuesPerServerNode ="<<dB(NIValuesPerServerNode(rxNodeIndx))<<endl;
//     }
//     else 
//     {
//       cout<<"[both:] Error: Unsupported powerControlMethod!! Currently powerControlMethod = "<<powerControlInfo.powerControlMethod<<endl;
//       abort();
//     }
//     
//     std::ofstream Txpowerused;// to print in a file
//     Txpowerused.open("./Results/print_ref_power.txt", std::ios::app);
//     Txpowerused<<"\t ueId= "<<txNodeID<<"\t ueIdpower= "<<uplinkPowerPerSubCarrierInWatts*M*12 /*<<"\t uesubframe_all= "<< subFrm_cnt*/ << "\t pathloss= "<< -channelGainInDB <<endl;
//     Txpowerused.close(); // THIS IS THE FILE WHERE USED TX POWER IS STORED
//     
//     schedulerToTransmitterInfo.txSubcarrierPowerInDBmPerAntenna = dB(uplinkPowerPerSubCarrierInWatts)+30;        
//     txIndx=find(schedulerToReceiverInfo.txNodeIDs,txNodeID);
//     schedulerToReceiverInfo.txSubcarrierPowerInDBmPerAntenna(txIndx)=dB(uplinkPowerPerSubCarrierInWatts)+30;
//     
//     if(traceValuePerTraceName["printUETxPower"]=="true") 
//     {
//       cout<<"UE Transmission power per SC(dB) = " << txNodeID << " " << (schedulerToTransmitterInfo.txSubcarrierPowerInDBmPerAntenna)+10*log10(M*12) << " " << dB(maxTxPowerInWatts)+30 << endl;
//     }
//     //! Computing Power Headroom (\assumed: i. 12 SC per RB  ii. for Uplink UE, only 1 dataBlkInfo present. )
//     if (schedulerToTransmitterInfo.dataBlkInfo(0).resourceBlocks.length() > 0)
//     {
//       //cout << "TxPow: " << schedulerOutput(centralUnitIDs(cu_cnt)).schedulerToTransmitterInfo.txSubcarrierPowerInDBmPerAntenna << endl;
//       int indx=find(powerHeadroomNodeIDs,schedulerToTransmitterInfo.txNodeID);
//       powerHeadroomInDBPerNode(indx) =dB(maxTxPowerInWatts) -dB(desiredPowerInWatts);
//       powerHeadroomInDBPerNode_subframeCount(indx) = subFrm_cnt;
//     }
//     //	totalPowerOfUE = 12*schedulerOutput(centralUnitIDs(cu_cnt)).schedulerToTransmitterInfo.dataBlkInfo(0).resourceBlocks.length()*txPowerInWatts;
//   }
// 
// }

void getUplinkPower(PoAlphaSet_S &paobj,int nRBs, double maxTxPowerIndbmPerSC, double RSPIndBm, double RSRPIndBm, double TPCCmdFeild, double currentTime, int mu,bool isAbsTPC, bool resetTPC, double timeByTpc)
{
 
   
  PcControlReqInfo_S pcc1;
  
  //       for(int ij=0;ij<closedLoopProcess;ij++) // closed loops
  //       {
  pcc1.setPCreqParameters(isAbsTPC, resetTPC, TPCCmdFeild, timeByTpc); 
  paobj.setPCcontrolInfo(pcc1); // stored in a list
  //       }
  
  double delta_TF=0;
  //       delta_TF=getDeltaTF(schedulerToTransmitterInfo);
  paobj.pcReqParaByMeasure.setPCreqMeasParameters(maxTxPowerIndbmPerSC, nRBs, RSPIndBm, RSRPIndBm, mu, delta_TF); // measurement values can also be multiple in no., hence for that also we can code like control values
  
  //       paobj.powerControlReqInfo.setParamByControlForPUCCH(??);
  //       paobj.pcReqParaByMeasure.setPUCCHparamByUE(??); // wher to get values or there can we have some config files for the i/p to parse
  //       double delta_TF=getDeltaTFforPUCCH(paobj.powerControlReqInfo, paobj.pcReqParaByMeasure, schedulerToTransmitterInfo);
  
  
  paobj.applyPCcontrol(currentTime);

  
  
}


/*!\brief doUplinkPowerControl(Tx power/subcarrier) is computed by UE based on power control.
 * for RACH preamble initial Transmission, func. calculateDesiredPowerInWattsInd can be used but only Po & PL should be there Alpha==1 and other parameters(TPC, deltaTF etc) should be made zero
 * for RACH msg3 Transmission, use current module but call getInitialPreviousTPC b4 calling applyPCcontrol and keep deltaMsg2=uplinkPowerControlCorrectionValInDB, then uplinkPowerControlCorrectionValInDB=0(since already used)
 */
double doUplinkPowerControlNR(double maxTxPowerIndbmPerSC,ivec rbs, int totalRBs, int mcsIndx, Waveform_E waveform, int TPCcmdFieldIndividualUE , double RSPIndBm, double RSRPIndBm, PowerControlInfo_S &powerControlInfo,bool is256QAMEnabled, bool isPiby2BPSKEnabled,bool isSRSEnabled, double currentTime, int mu, vector<PoAlphaSet_S>& tpoAlphaSet)
{
  
  int NRBs = totalRBs;
  int txNodeID=-1, rxNodeID=-1, MrbinBW;
  int txNodeIndx, rxNodeIndx;
  double uplinkPowerPerSubCarrierInWatts;
  int txIndx, closedLoopProcess=2;
  double fPUCCH=0, delta_TF;
//   int subFrm_cnt=ttiCount;
  
  {
    if(powerControlInfo.enablePowerBackoffModel)
    {
        powerBackoff_S powerBackoff;
        ModulationScheme_E mosc;
        mcsidxMapping(is256QAMEnabled, isPiby2BPSKEnabled,mcsIndx,mosc,false);
	if(mosc!=1)
	{
	  double maxPowerReduction=powerBackoff.getMPR(mosc,waveform,rbs,NRBs);
	  maxTxPowerIndbmPerSC-=maxPowerReduction;
	}
	else
	{
	  maxTxPowerIndbmPerSC+=3;
	}
    }  
    
    for(int factor_cnt=0; factor_cnt < 4/*length*/; factor_cnt++) // factor like per beams etc 32/8 for PUSCH/PUCCH
    {
      setvalues(powerControlInfo.targetPowerForLTEPowerControlindBm,powerControlInfo.pathlossCompensation,powerControlInfo.fValueForPowerControl,tpoAlphaSet, factor_cnt);
    }
    
//     PoAlphaSet_S paobj=tpoAlphaSet[schedulerToTransmitterInfo.dataBlkInfo(0).indexOfFactor];
    
    PoAlphaSet_S po(powerControlInfo.targetPowerForLTEPowerControlindBm,powerControlInfo.pathlossCompensation);
    po.pcfValueForPowerControl=powerControlInfo.fValueForPowerControl;	
    
    
//     cout<<"Info : "<<schedulerToTransmitterInfo.dataBlkInfo(0).resourceBlocks.length()<<" , "<<maxTxPowerIndbmPerSC<<" , "<<RSPIndBm<<" , "<<RSRPIndBm<<endl;
    getUplinkPower(po,rbs.length(),maxTxPowerIndbmPerSC,RSPIndBm,RSRPIndBm,TPCcmdFieldIndividualUE/*,currentTime,mu,schedulerToTransmitterInfo.dataBlkInfo(0).isAbsTPC,schedulerToTransmitterInfo.dataBlkInfo(0).resetTPC,schedulerToTransmitterInfo.dataBlkInfo(0),schedulerToTransmitterInfo.dataBlkInfo(0).timeByTpc*/);
//     cout << "Calculated given TPC BY BS \t" << schedulerToTransmitterInfo.dataBlkInfo(0).TPCcmdFieldIndividualUE << endl;
    double uplinkPowerPerSubCarrierInWatts=po.pcReqParaByMeasure.finalTxULPowerPerSubCarrierInWatts;
    int MrbinBW= rbs.length();//po.pcReqParaByMeasure.Mrb;
    
      std::ofstream Txpowerused;// to print in a file
      Txpowerused.open("./Results/print_ref_power.txt", std::ios::app);
      Txpowerused<<"\t ueId= "<<txNodeID<<"\t ueIdpower= "<<uplinkPowerPerSubCarrierInWatts*MrbinBW*12 /*<<"\t uesubframe_all= "<< subFrm_cnt*/ << "\t pathloss= "<< RSPIndBm-RSRPIndBm <<endl;
      Txpowerused.close(); // THIS IS THE FILE WHERE USED TX POWER IS STORED
    
    
    double txSubcarrierPowerInDBmPerAntenna = dB(uplinkPowerPerSubCarrierInWatts)+30;       
//     cout<<"schedulerToTransmitterInfo.txSubcarrierPowerInDBmPerAntenna"<<schedulerToTransmitterInfo.txSubcarrierPowerInDBmPerAntenna<<endl; // %%%% CHECK purpose
    //! Computing Power Headroom (\assumed: i. 12 SC per RB  ii. for Uplink UE, only 1 dataBlkInfo present. )
    {
//       int indx=find(powerHeadroomNodeIDs,txNodeID);
// //       cout<<"\t MrbinBW  "<< MrbinBW << "\t maxTxPowerIndb  " << (maxTxPowerIndbmPerSC-30)+dB(MrbinBW*12) << "\t (dB(po.pcReqParaByMeasure.desiredPowerInWattsForNR)) " << (dB(po.pcReqParaByMeasure.desiredPowerInWattsForNR))<< endl;
//       powerHeadroomInDBPerNode(indx) =(maxTxPowerIndbmPerSC-30)+dB(MrbinBW*12) - dB(po.pcReqParaByMeasure.desiredPowerInWattsForNR);
//       powerHeadroomInDBPerNode_subframeCount(indx) = subFrm_cnt;
//       cout<<"\t powerHeadroomInDBPerNode for index " << indx << "\t is " << powerHeadroomInDBPerNode(indx) << "\t &  powerHeadroomInDBPerNode_subframeCount(indx) is " <<  powerHeadroomInDBPerNode_subframeCount(indx) << endl;
    }
    
    po.pcReqParaByMeasure.desiredSRSpowerPerRB=po.calculateDesiredPowerInWatts(1,po.pcReqParaByMeasure.pathlossindB,0,0);
    if(traceValuePerTraceName["printUETxPower"]=="true") 
    {
      cout<<"UE Transmission power(dB) = " << txNodeID << " " << (txSubcarrierPowerInDBmPerAntenna)/*+10*log10(MrbinBW*12) */<< " " << maxTxPowerIndbmPerSC/*+dB(MrbinBW*12)*/ << endl;
    }
    return txSubcarrierPowerInDBmPerAntenna;
   
  }

}



double powerBackoff_S::getMPR(ModulationScheme_E modulationScheme,Waveform_E waveform, ivec rballoc, int NRB, int UEclass)
{
    double MPR;
    bool continuous=((max(rballoc)-min(rballoc)+1)==rballoc.length());
    int rbStartLow = max(1,(int)floor(rballoc.length()/2));
    int rbStarthigh = NRB-rbStartLow-rballoc.length();
    if(UEclass==2)
    {
      if(continuous)
      {
	if((rballoc == "0")||(rballoc == getIntegers(NRB-1,NRB-1))||(rballoc == "0 1")||((rballoc == getIntegers(NRB-2,NRB-1))&&(UEclass==2))) 
	{
	  MPR=edgeRB_MPR((waveform*4)+(4-modulationScheme/2)); 
	}
	else if((rbStartLow <= min(rballoc))&&(rbStarthigh >= min(rballoc))&&(rballoc.length() <= ceil(NRB/2)))
	{
	  MPR=innerRB_MPR((waveform*4)+(4-modulationScheme/2));
	}
	else
	{
	  MPR=outerRB_MPR((waveform*4)+(4-modulationScheme/2));
	}
      }
      else
      {
	if((rballoc == "0")||(rballoc == getIntegers(NRB-1,NRB-1))||(rballoc == "0 1")||(rballoc == getIntegers(NRB-2,NRB-1))) 
	{
	  MPR=edgeRB_MPR((waveform*4)+(4-modulationScheme/2))+2;
        }
        else if((rbStartLow <= min(rballoc))&&(rbStarthigh >= min(rballoc))&&(rballoc.length() <= ceil(NRB/2)))
        {
	  MPR=innerRB_MPR((waveform*4)+(4-modulationScheme/2))+2;
        }
        else
        {
	  MPR=outerRB_MPR((waveform*4)+(4-modulationScheme/2))+2;
        }
      }
      return(MPR);
    }
    else if(UEclass==3)
    {
      if(continuous)
      {
	if((rbStartLow <= min(rballoc))&&(rbStarthigh >= min(rballoc))&&(rballoc.length() <= ceil(NRB/2)))
	{
	  MPR=innerRB_MPR((waveform*4)+(4-modulationScheme/2));
	}
	else
	{
	  MPR=outerRB_MPR((waveform*4)+(4-modulationScheme/2));
	}
      }
      else
      {
        if((rbStartLow <= min(rballoc))&&(rbStarthigh >= min(rballoc))&&(rballoc.length() <= ceil(NRB/2)))
        {
	  MPR=innerRB_MPR((waveform*4)+(4-modulationScheme/2))+2;
        }
        else
        {
	  MPR=outerRB_MPR((waveform*4)+(4-modulationScheme/2))+2;
        }
      }
      return(MPR);
    }
    return 0;
}

/*! \fn generateTPCcmd(...)
* \brief generate TPC command field in BS.
* \param [in,out] powerControlInfo ---> it contains the targetPowerForLTEPowerControlindBm 
* \param [in] NIValue --> average interference over noise value.
* \param [in] receivedppSINR ---> Averaged Recieved ppSINR
* \param [in,out] avgTimeForSINR --> Initial Time index (N slots) for computing average SINR difference at BS, Updated after every mean.
* \param [in] indx ---> current slot/subframe index
* \param [in,out] SINRdiffForK_Slots --->vector to store SINR difference values for N slots for moving averageof SINR difference.
* \return returns the calculated value of TPC command field
*/

int generateTPCcmd(PowerControlInfo_S &powerControlInfo, double NIValue, double receivedppSINR, int &currentTimeForSINR, int indx, vec &SINRdiffForK_Slots)
{
  double SINRdifference, avgSINRdiff=0;
  int TPCcmd_feild=1;
  double targetSINR=(powerControlInfo.targetPowerForLTEPowerControlindBm-30)-dB(NIValue);
  SINRdifference = targetSINR-receivedppSINR;
  append(SINRdiffForK_Slots,SINRdifference); //SINRdiffForK_Slots is inside datablkinfo

  if(indx==currentTimeForSINR) // this func should be called for avgTimeForSINR no. of slots, before getting final sinr difference
  {
    avgSINRdiff= mean(SINRdiffForK_Slots); // update value after each round of average (moving avg)
    if(avgSINRdiff <=-1)
      TPCcmd_feild=0;
    else if(avgSINRdiff <=1)
      TPCcmd_feild=1;
    else if(avgSINRdiff <= 5)
      TPCcmd_feild =2;
    else
    {
      TPCcmd_feild =3;   
      cout<<"targetSINR\t"<<targetSINR<<"\treceivedppSINR\t"<<receivedppSINR<<"\tcurrentTimeForSINR\t"<<currentTimeForSINR<<"\tindx\t"<<indx<<"\tavgSINRdiff\t"<<avgSINRdiff<<"\tTPCcmd_feild 456\t"<<TPCcmd_feild<<endl;
    } 
//     cout<<"targetSINR\t"<<targetSINR<<"receivedppSINR\t"<<receivedppSINR<<"indx\t"<<indx<<"currentTimeForSINR\t"<<currentTimeForSINR<<"avgSINRdiff\t"<<avgSINRdiff<<"\tTPCcmd_feild 456\t"<<TPCcmd_feild<<endl;

    currentTimeForSINR=currentTimeForSINR+1;
    SINRdiffForK_Slots = SINRdiffForK_Slots(findNot(SINRdiffForK_Slots,SINRdiffForK_Slots(0)));
  }
  return TPCcmd_feild;
}