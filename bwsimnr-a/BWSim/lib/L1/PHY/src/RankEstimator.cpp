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

#include "../include/CQIEstimatorNR.h"


vec computeConditionNumber(vec SV,int maxRank)
{
  vec conditionNumberOverRanks = ones(maxRank);
  for(int i=1;i<maxRank;i++)
    conditionNumberOverRanks(i)=((double)SV(i-1)/SV(i));
  return conditionNumberOverRanks;
}
vec computeDiff(vec s)
{
    vec diff(s.length());diff(0)=0;
    for(int i=1;i<diff.length();i++)
    {
        diff(i) = s(i) - s(i-1);
    }
    return diff;
}

double CQIEstimatorNR::computePrePSINRandConditionNumberindB(int maxRank,vec& avgConditionNumber)
{
    cmat estimatedIntrCovariance = mean(perToneInterferenceCovariance);
    cmat avgChannelCovariance = avgCovariance(entireRawChannel);
    cmat V;vec S;
    eig_sym(avgChannelCovariance,S,V);
    S = S(reverse(sort_index(S))); //Arranging eigen values in descending order. 
    avgConditionNumber = dB(computeConditionNumber(S,maxRank));
    double IplusN = dB(abs(trace(estimatedIntrCovariance))/(double)estimatedIntrCovariance.cols());
  
    double sinr = dB(RSRP)-IplusN;
    if(isnan(sinr)) abort();
    return sinr;
}

//computes the RI as per the trigger as per the RI Periodic configuration
int CQIEstimator::computeRIForDownlinkPeriodicRIReport(CQIMeasurementConfig_S &cqiMeasurementConfig,FrameStructure& frameStructure, DemodulationScheme_E& demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper,ReferenceSignal_S *refSignal, ResourceMapper* resourceMapper,Array<cmat> &rxFrame, Array< Array< Array< cmat > > >& allChannels, double myTxSCPowerInDB,ivec txNodeIDSForChannel,double noiseVariance, double restOfInterferenceInDB, int ServiceNodeID, int subframeNumber,string WideBandCQIlogsFileName,string RIlogsFileName,bool isAperiodicComputation){
  sinrOverWideband.set_length(0);
  Array<cmat> codeBook; //CQI1 array of precoders
  int nLayers=cqiMeasurementConfig.numberOfLayers;
  int nCodewords=cqiMeasurementConfig.numberOfCodeWords;
  ivec cqiPorts=cqiMeasurementConfig.cqiPorts;
  int nPorts=cqiPorts.length();
  int desiredTxNode=cqiMeasurementConfig.serverNodeGroup(0);
  double exp_spec_eff=0;
  int desiredTxNodeIndx=  find(txNodeIDSForChannel,desiredTxNode);
  // txNodeIDSForChannel contains the IDs of the node under consideration and the strong 8 interfering nodes
  //Framehandler* myTxFrameHandler=TxFrameHandlers(desiredTxNodeIndx);
//   Array<Framehandler*> interferingTxFrameHandlers;
//   vec interfererTxSCPowerInDB;
  //interferingTxFrameHandlers.set_length(txNodeIDSForChannel.length()-1);
//   interfererTxSCPowerInDB.set_length(txNodeIDSForChannel.length()-1);
//   ivec interfererIndices=findNot(txNodeIDSForChannel,desiredTxNode);
//   for(int intr_cnt=0;intr_cnt<txNodeIDSForChannel.length()-1;intr_cnt++)
//   {
//     interfererTxSCPowerInDB(intr_cnt)=txSubcarrierPowerInDB(interfererIndices(intr_cnt));
    //interferingTxFrameHandlers(intr_cnt)=TxFrameHandlers(interfererIndices(intr_cnt));
//   }
  
  //Obtaining the channel matrix for a node under consideration
  Array< Array<cmat> > myChannel=allChannels(desiredTxNodeIndx);

  int nRxAntenna=myChannel(0).length();
  int maxRank = min(nPorts,nRxAntenna);
  
  Array<ivec> CQIperCodewords(maxRank);
  Array<ivec> modOrderPerCW(maxRank);
  Array<vec> effSINRPerCW(maxRank);
  vec spectralEfficiencyOverRanks=zeros(maxRank);
  ivec tempPMI=zeros_i(maxRank)-1;
  ivec tempPMI_I1=zeros_i(maxRank)-1;
  int Rank=-1;
  vec avgPPSINR=zeros(maxRank)-1;
  
  /// Temp Variables
  int DLRBs=cqiMeasurementConfig.nRBs;
  int rbNumber=cqiMeasurementConfig.startRB;
  
  
  // Need to pass all RS ports, no problem for CRS tx modes
  ivec portLocations=getPortLocationsInRB(frameStructure, cqiPorts,0,true);
  
  Array<string> rbName(cqiPorts.length());
  rbName.set_length(cqiPorts.length());
  for(int port_cnt=0;port_cnt<cqiPorts.length();port_cnt++)
    rbName(port_cnt)="port"+toString(cqiPorts(port_cnt));
  imat rsRBStructure;
  rsRBStructure=frameStructure.getMergedResourceUnit("nullRB",rbName,"data",cqiMeasurementConfig.rsSymPositions,cqiMeasurementConfig.rsScPositions);
  
  rsRBStructure = rsRBStructure.get_cols(3,rsRBStructure.cols()-1);
  ivec dataTones = find(rsRBStructure,0,true);
  int nREsPerRB = dataTones.length();
  Array<Array< Array<vec> > > sinrPerCodewordForWidebandCQItemp(maxRank);

  switch(downlinkCQIInfo->cqiComputationInfo.periodicMode)
  {
    case _PERIODIC_MODE_1_0_:
    case _PERIODIC_MODE_2_0_:
    {
      if(cqiMeasurementConfig.transmissionScheme == _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_ || cqiMeasurementConfig.transmissionScheme == _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_)
      {
	for(int rank_cnt=0;rank_cnt<maxRank;rank_cnt++)
	{
	  sinrPerCodewordForWidebandCQItemp(rank_cnt).set_length(1);
	  if(rank_cnt==0)
	  {
	    cqiMeasurementConfig.transmissionScheme=_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_;
	    nLayers=cqiMeasurementConfig.numberOfLayers=cqiPorts.length();
	    nCodewords=cqiMeasurementConfig.numberOfCodeWords=1;
	  }
	  else
	  {
	    cqiMeasurementConfig.transmissionScheme=_TRANSMISSION_SCHEME_LARGE_DELAY_CDD_;
	    nLayers=cqiMeasurementConfig.numberOfLayers=rank_cnt+1;
	    nCodewords=cqiMeasurementConfig.numberOfCodeWords=2;
	  }

	  sinrPerCodewordForWidebandCQItemp(rank_cnt)(0)=computeSinrPerCW(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,cqiMeasurementConfig.transmissionScheme,nLayers,cqiMeasurementConfig.cqiPorts,nCodewords,DLRBs,0,-1,1);

	  vec MMIBvaluesPerModScheme;
	  
	  CQIperCodewords(rank_cnt)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForWidebandCQItemp(rank_cnt)(0),MMIBvaluesPerModScheme,modOrderPerCW(rank_cnt),effSINRPerCW(rank_cnt),DLRBs,nREsPerRB,cqiMeasurementConfig.transmissionScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	  
	  spectralEfficiencyOverRanks(rank_cnt)=exp_spec_eff;
	  
	  avgPPSINR(rank_cnt) = computeAvgSINRindB(effSINRPerCW(rank_cnt),rank_cnt+1);
	  
	  if(rank_cnt > 0 && spectralEfficiencyOverRanks(rank_cnt) < spectralEfficiencyOverRanks(rank_cnt - 1))
	    break;
	  
	}
      }
      else
      {
	cout<<"Not supported transmissionScheme "<<cqiMeasurementConfig.transmissionScheme<<endl;abort();
      }
      
      break;
    }
    
    case _PERIODIC_MODE_1_1_:
    case _PERIODIC_MODE_2_1_:
    {  
      if(cqiMeasurementConfig.transmissionScheme == _TRANSMISSION_SCHEME_OLMUMIMO_ && nPorts == 8)
      {
	for(int rank_cnt=0;rank_cnt<maxRank;rank_cnt++)
	{
	  if(rank_cnt==0)
	  {
	    nLayers=cqiMeasurementConfig.numberOfLayers=1;
	    nCodewords=cqiMeasurementConfig.numberOfCodeWords=1;
	  }
	  else
	  {
	    nLayers=cqiMeasurementConfig.numberOfLayers=rank_cnt+1;
	    nCodewords=cqiMeasurementConfig.numberOfCodeWords=2;
	  }
	  ivec wb_I1indices = getValidi1Indices(nLayers);
	  int I1Length = wb_I1indices.size();
	  
	  sinrPerCodewordForWidebandCQItemp(rank_cnt)=computeSinrPerCWforI1indices(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,cqiMeasurementConfig.transmissionScheme,nLayers,cqiMeasurementConfig.cqiPorts,nCodewords,DLRBs,1,1,wb_I1indices);
	  vec spectralEfficiencyOverPMIs=zeros(I1Length)-1;
	  Array<ivec> tempCQIperCodewords(I1Length);
	  Array<vec> MMIBvaluesPermodscheme(I1Length);
	  Array<ivec> modOrderPerCWTemp(I1Length);
	  
	  Array<vec> effSINRPerCWTemp(I1Length);
	  for(int precIndex=0; precIndex<I1Length; precIndex++)
	  {
	    tempCQIperCodewords(precIndex)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForWidebandCQItemp(rank_cnt)(precIndex),MMIBvaluesPermodscheme(precIndex),modOrderPerCWTemp(precIndex),effSINRPerCWTemp(precIndex),DLRBs,nREsPerRB,cqiMeasurementConfig.transmissionScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	    spectralEfficiencyOverPMIs(precIndex)=exp_spec_eff;
	  }
	  
	  int tempI1 = getBestPMIindex(spectralEfficiencyOverPMIs,MMIBvaluesPermodscheme);
	  
	  tempPMI(rank_cnt) = -1;
	  tempPMI_I1(rank_cnt)=wb_I1indices(tempI1);

	  CQIperCodewords(rank_cnt)=tempCQIperCodewords(tempI1);
	  modOrderPerCW(rank_cnt)=modOrderPerCWTemp(tempI1);
	  effSINRPerCW(rank_cnt)=effSINRPerCWTemp(tempI1);
	  
	  // Rank restriction
	  if(rank_cnt > 0)
	  {
	    if(CQIperCodewords(rank_cnt)(0) < 4 || CQIperCodewords(rank_cnt)(1) < 4 || abs(CQIperCodewords(rank_cnt)(0)-CQIperCodewords(rank_cnt)(1)) > 4 )
	      spectralEfficiencyOverRanks(rank_cnt)=exp_spec_eff/((double)rank_cnt+1.0);
	    else
	      spectralEfficiencyOverRanks(rank_cnt)=exp_spec_eff;
	  }
	  else
	    spectralEfficiencyOverRanks(rank_cnt)=exp_spec_eff;
	  
	  avgPPSINR(rank_cnt) = computeAvgSINRindB(effSINRPerCW(rank_cnt),rank_cnt+1);
	  if(rank_cnt > 0 && spectralEfficiencyOverRanks(rank_cnt) < spectralEfficiencyOverRanks(rank_cnt - 1))
	    break;
	}
	
      }
      else if(cqiMeasurementConfig.transmissionScheme == _TRANSMISSION_SCHEME_CL_SM_ || cqiMeasurementConfig.transmissionScheme == _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ || cqiMeasurementConfig.transmissionScheme == _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || cqiMeasurementConfig.transmissionScheme == _TRANSMISSION_SCHEME_NR_CL_ || cqiMeasurementConfig.transmissionScheme == _TRANSMISSION_SCHEME_COMP_)
      {
	for(int rank_cnt=0;rank_cnt<maxRank;rank_cnt++)
	{
	  
	  if(rank_cnt==0)
	  {
	    nLayers=cqiMeasurementConfig.numberOfLayers=1;
	    nCodewords=cqiMeasurementConfig.numberOfCodeWords=1;
	  }
	  else
	  {
	    nLayers=cqiMeasurementConfig.numberOfLayers=rank_cnt+1;
	    nCodewords=cqiMeasurementConfig.numberOfCodeWords=2;
	  }
	  
	  if(nPorts==8)
	    codeBook= mCodebook->getSubSampledCodeBookForSubmode1(nLayers);
	  else
	    codeBook=mCodebook->getCodebook(nPorts,nLayers,1);
	  
	  int codebookLength=codeBook.length();
	  sinrPerCodewordForWidebandCQItemp(rank_cnt)=computeSinrPerCWForCodeBook(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,cqiMeasurementConfig.transmissionScheme,nLayers,cqiMeasurementConfig.cqiPorts,nCodewords,DLRBs,1,1,codeBook);
	  vec spectralEfficiencyOverPMIs=zeros(codebookLength)-1;
	  Array<ivec> tempCQIperCodewords(codebookLength);
	  Array<vec> MMIBvaluesPermodscheme(codebookLength);
	  Array<ivec> modOrderPerCWTemp(codebookLength);
	  Array<vec> effSINRPerCWTemp(codebookLength);
	  
	  for(int precIndex=0; precIndex<codebookLength; precIndex++)
	  {
	    //! identity matrix precoder is restricted from the selection..
	    if(precIndex==0 && nPorts==2 && nLayers==2)
	      continue;
	    
	    tempCQIperCodewords(precIndex)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForWidebandCQItemp(rank_cnt)(precIndex),MMIBvaluesPermodscheme(precIndex),modOrderPerCWTemp(precIndex),effSINRPerCWTemp(precIndex),DLRBs,nREsPerRB,cqiMeasurementConfig.transmissionScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	  	    // Rank restriction
	    if((nCodewords==1 && tempCQIperCodewords(precIndex)(0) < 4) || (nCodewords==2 && ( tempCQIperCodewords(precIndex)(0) < 4 || tempCQIperCodewords(precIndex)(1) < 4 || abs(tempCQIperCodewords(precIndex)(0) - tempCQIperCodewords(precIndex)(1)) > 4 )))
	      spectralEfficiencyOverPMIs(precIndex)=exp_spec_eff/((double)rank_cnt+1.0);
	    else
	      spectralEfficiencyOverPMIs(precIndex)=exp_spec_eff;
	  }
	  
	  int tPMI = getBestPMIindex(spectralEfficiencyOverPMIs,MMIBvaluesPermodscheme);
	  
	  if(nPorts==8)
	  {
	    tempPMI(rank_cnt) = mCodebook->CodeBookIndicesToSelecti1ForSubmode1(rank_cnt)(tPMI);
	    tempPMI_I1(rank_cnt)=getLongTermPMIInd(nLayers,tempPMI(rank_cnt));
	  }
	  else
	    tempPMI(rank_cnt) = tPMI;
	  
	  CQIperCodewords(rank_cnt)=tempCQIperCodewords(tPMI);
	  modOrderPerCW(rank_cnt)=modOrderPerCWTemp(tPMI);
	  effSINRPerCW(rank_cnt)=effSINRPerCWTemp(tPMI);

	  spectralEfficiencyOverRanks(rank_cnt)=max(spectralEfficiencyOverPMIs);
	  
	  avgPPSINR(rank_cnt) = computeAvgSINRindB(effSINRPerCW(rank_cnt),rank_cnt+1);
	  if(rank_cnt > 0 && spectralEfficiencyOverRanks(rank_cnt) < spectralEfficiencyOverRanks(rank_cnt - 1))
	    break;
	}
      }
      else
      {
	cout<<"Not supported transmissionScheme "<<cqiMeasurementConfig.transmissionScheme<<endl;abort();
      }
      break;
    }
    
    default :
    {
      cout<<" [both:] RI is not ready for other cases"<<endl;abort();
    }
  }
  

  Rank = max_index(spectralEfficiencyOverRanks)+1;

  sinrOverWideband=sinrPerCodewordForWidebandCQItemp(Rank-1);
#ifdef CSI_DEBUG
  cout<<"PPSINRs : "<<avgPPSINR<<"\tspectralEfficiencyOverRanks : "<<spectralEfficiencyOverRanks <<"\t CQI : "<<CQIperCodewords<<"\tRank : "<<Rank<<endl;
#endif
  
//   downlinkCQIInfo->cqiComputationInfo.isRankComputed=true;
  cqiMeasurementConfig.numberOfLayers=Rank;
  
  if(traceValuePerTraceName["EnableUElogs"]=="true")
    PrintRI(ServiceNodeID,Rank,avgPPSINR(Rank-1),spectralEfficiencyOverRanks,subframeNumber,RIlogsFileName);
  
  if(Rank<2)
  {
    if(cqiMeasurementConfig.transmissionScheme==_TRANSMISSION_SCHEME_LARGE_DELAY_CDD_)
    {
      cqiMeasurementConfig.transmissionScheme=_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_;
      cqiMeasurementConfig.numberOfLayers=cqiMeasurementConfig.cqiPorts.length();
    }
    else if(cqiMeasurementConfig.transmissionScheme==_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_)
    {
      cqiMeasurementConfig.numberOfLayers=cqiMeasurementConfig.cqiPorts.length();
    }
    cqiMeasurementConfig.numberOfCodeWords=1;
  }
  else
  {
    if(cqiMeasurementConfig.transmissionScheme==_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_)
      cqiMeasurementConfig.transmissionScheme=_TRANSMISSION_SCHEME_LARGE_DELAY_CDD_;
    cqiMeasurementConfig.numberOfCodeWords=2;
  }
  int measurementIndx=downlinkCQIInfo->perUserFeedback.getMeasurementIndx(cqiMeasurementConfig.requestID);

  if(isAperiodicComputation)
  {
    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).rankAperiodic=Rank;
    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).rankPeriodic=-1;
    if(traceValuePerTraceName["EnablePrints"]=="true")
    {
      if(cqiMeasurementConfig.numberOfCodeWords==1)
	cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" RI : "<<setw(1)<<Rank<<" , effSINRsPerCW : "<<setw(11)<<effSINRPerCW(Rank-1)(0)<<" in subframe "<<setw(5)<<subframeNumber<<endl;
      else
	cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" RI : "<<setw(1)<<Rank<<" , effSINRsPerCW : "<<setw(5)<<effSINRPerCW(Rank-1)(0)<<" "<<setw(5)<<effSINRPerCW(Rank-1)(1)<<" in subframe "<<setw(5)<<subframeNumber<<endl;
    }
  }
  else      
  {
    if(traceValuePerTraceName["EnablePrints"]=="true")
    {
      if(cqiMeasurementConfig.numberOfCodeWords==1)
	cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" RI : "<<setw(1)<<Rank<<" , effSINRsPerCW : "<<setw(11)<<effSINRPerCW(Rank-1)(0)<<" in subframe "<<setw(5)<<subframeNumber<<endl;
      else
	cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" RI : "<<setw(1)<<Rank<<" , effSINRsPerCW : "<<setw(5)<<effSINRPerCW(Rank-1)(0)<<" "<<setw(5)<<effSINRPerCW(Rank-1)(1)<<" in subframe "<<setw(5)<<subframeNumber<<endl;
    }
    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).rankPeriodic=Rank;
    
  }
  if(!isAperiodicComputation)
  {
    if(downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger)
    {
      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.pmiIndex=tempPMI(Rank-1);
      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1=tempPMI_I1(Rank-1);
      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord=CQIperCodewords(Rank-1);
      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.isValid=true;
      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).isValid=true;
      downlinkCQIInfo->perUserFeedback.isValid=true;

      if(traceValuePerTraceName["EnablePrints"]=="true")
      {
	if(cqiMeasurementConfig.numberOfCodeWords==1)
	  cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" , Wideband PMI : "<<setw(2)<<tempPMI(Rank-1)<<" , Wideband CQI : "<<setw(5)<<downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0)<< " , effSINRsPerCW : "<<setw(11)<<effSINRPerCW(Rank-1)(0)<<" in subframe "<<setw(5)<<subframeNumber<<endl;
	else
	  cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" , Wideband PMI : "<<setw(2)<<tempPMI(Rank-1)<<" , Wideband CQI : "<<setw(2)<<downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0)<<" "<<setw(2)<< downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(1)<< " , effSINRsPerCW : "<<setw(5)<<effSINRPerCW(Rank-1)(0)<<" "<<setw(5)<<effSINRPerCW(Rank-1)(1)<<" in subframe "<<setw(5)<<subframeNumber<<endl;
      }
      if(traceValuePerTraceName["EnableUElogs"]=="true")
	PrintCQIindices(ServiceNodeID,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord,modOrderPerCW(Rank-1),effSINRPerCW(Rank-1), subframeNumber,WideBandCQIlogsFileName,tempPMI(Rank-1));
    }  
    
  }
  downlinkCQIInfo->perUserFeedback.isValid=true;
  return Rank;
}

// This is new function writen for NR. Periodic mode 1_1 code from LTE is kept as base for rank calculation
int CQIEstimatorNR::computeRIForDownlinkRankReportNR(CQIMeasurementConfig_S &cqiMeasurementConfig, FrameStructure& frameStructure, DemodulationScheme_E& demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper,ReferenceSignal_S *refSignal, ResourceMapper* resourceMapper,Array<cmat> &rxFrame, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB,double noiseVariance, double restOfInterferenceInDB, int serviceNodeID,int eNBID, int subframeNumber,string WideBandCQIlogsFileName,string RIlogsFileName,bool isAperiodicComputation){

  TransmissionScheme_E txScheme=cqiMeasurementConfig.transmissionScheme;
  checkNRTransmissionScheme(txScheme);
 
  configureCQIEstimatorNR(cqiMeasurementConfig.cqiPorts,frameStructure);
  
  sinrOverWideband.set_length(0);

  int nRxAntenna=myChannel(0).length();
  int maxRankUE = min(nCSIRSPorts,8);
  
  CodeBookConfig_S codebookConfig = mCodebook->getCodeBookConfig();
  
  static bool skipRIrestriction = false;
  if(!skipRIrestriction)
  {
    vec maxRanksPerCBtype=pow2(to_vec(4-getIntegers(1,3)));
    int maxRankPerUE = (int)maxRanksPerCBtype((int)codebookConfig.codeBookType);
    if(cqiMeasurementConfig.riRestriction > maxRankPerUE)
    {
        cout<<"MaxRankPerUE greater than "<<maxRankPerUE<<" is not supported for "<<NRCodebookType_Str[codebookConfig.codeBookType]<<endl;
        cout<<"Setting RI Restriction to "<<maxRankPerUE<<endl;
        cqiMeasurementConfig.riRestriction = maxRankPerUE;
    }
    skipRIrestriction = true;
  }
  
  int maxRank = min(cqiMeasurementConfig.riRestriction,min(nRxAntenna,maxRankUE));
  int rank = 1;
  
  Array<ivec> CQIperCodewords(maxRank);
  Array<ivec> modOrderPerCW(maxRank);
  Array<vec> effSINRPerCW(maxRank);
  vec spectralEfficiencyOverRanks=zeros(maxRank);
  vec avgPostPSINR=zeros(maxRank)-1;
  ivec tempPMI=zeros_i(maxRank)-1;
  ivec tempPMI_I1=zeros_i(maxRank)-1;
  vec avgConditionNumberOverRanks=zeros(maxRank);
  
  /// Temp Variables
  int DLRBs=cqiMeasurementConfig.nRBs;
  int rbNumber=cqiMeasurementConfig.startRB;

  int nREsPerRB = getNumOfREsPerRB(CSIRSPorts,frameStructure,cqiMeasurementConfig.rsSymPositions,cqiMeasurementConfig.rsScPositions);
  
  CQIInfo.clear();
  
  //CQI extracting the channel over all the DLRBS on positions corresponding to CRS Ports 
  getRawPhysicalChannelAcrossRE(entireRawChannel,resourceMapper,myChannel,rbNumber,true,reCountPerRb,DLRBs,portLocations);
  perToneInterferenceCovariance=getPerToneInterferenceCovariance(rxFrame,frameStructure,resourceMapper,true,myChannel,portToAntennaMapper,refSignal,rbRSPattern, portReferenceNumbers,txScheme,reCountPerRb,rbNumber,DLRBs,CSIRSPorts,inv_dB(myTxSCPowerInDB),RSRP);
  avgWidebandChannel = mean(entireRawChannel);  //--Add if needed.
  CQIInfo.subframeNumber = subframeNumber;
  
  double prePSINR = computePrePSINRandConditionNumberindB(maxRank,avgConditionNumberOverRanks);
  vec cumCN = cumsum(avgConditionNumberOverRanks);
  vec diff = computeDiff(avgConditionNumberOverRanks);
  
  vec computationTimeOverRanks=zeros(maxRank);
  
  std::stringstream ss;
  
  if(currentTaskid==0 && subframeNumber==0)
      ss<<"nTx: "<<entireRawChannel(0).cols()<<"\tnRx: "<<entireRawChannel(0).rows()<<"\tSub-Carrier Power: "<<myTxSCPowerInDB+30<<" dBm\tNoiseVariance: "<<dB(noiseVariance)+30<<" dBm\tnRBs: "<<DLRBs<<"\tnREsPerRB: "<<nREsPerRB<<endl;
  
  double SINRTh = (codebookConfig.Ng==1) ? -3 : -10;
  
  clock_t t;t = clock();
  
//   if(cqiMeasurementConfig.typeIMethod == _SEARCHFREE_)
//     goto label;
   
//   if(cumCN(maxRank-1) <= 2*max(avgConditionNumberOverRanks) && cumCN(maxRank-1)>10 && maxRank>2)
//       rank = max_index(avgConditionNumberOverRanks);
//   else if(max(diff) > 10 && find(diff>10)(0)<=4)
//       rank = find(diff>10)(0);
//   else if(prePSINR<SINRTh)  
//       rank = 1;
//   else
  {
      label:
        Link_Param_S* linkParam = new Link_Param_S(txScheme,demodulationScheme,portToAntennaMapper,myTxSCPowerInDB,rbNumber,DLRBs,nREsPerRB,downlinkCQIInfo->cqiComputationInfo.enableCQI0,cqiMeasurementConfig.typeIMethod);
        linkParam->enableLayerIndication = (cqiMeasurementConfig.cqiFormatIndicator == _WIDEBAND_CQI_ && cqiMeasurementConfig.pmiFormatIndicator == _WIDEBAND_PMI_); //should be enabled only if both PMI and CQI are Wideband reported.
        Array<CSI_Info_S> csiInfoPerRank(maxRank);
      
        for(int rank_cnt=0;rank_cnt<maxRank;rank_cnt++)
        {
            t=clock();
            int nLayers=cqiMeasurementConfig.numberOfLayers=rank_cnt + 1 ;
            
            CSI_Info_S csiInfo = getBestPMIandCQIOverWideband(linkParam,nLayers);
                
            tempPMI(rank_cnt) = csiInfo.PMI;
            
            if(!isAperiodicComputation)
            {
                tempPMI_I1(rank_cnt) = tempPMI(rank_cnt);
            }
                
            CQIperCodewords(rank_cnt)=csiInfo.CQIindexPerCW;
            modOrderPerCW(rank_cnt)=csiInfo.modOrderPerCW();
            effSINRPerCW(rank_cnt)=csiInfo.effSINRsPerCW;
            csiInfoPerRank(rank_cnt) = csiInfo;
                
            spectralEfficiencyOverRanks(rank_cnt)=csiInfo.specEff;
    
            avgPostPSINR(rank_cnt) = computeAvgSINRindB(effSINRPerCW(rank_cnt),rank_cnt+1);
            computationTimeOverRanks(rank_cnt) = (double)(clock()-t)/CLOCKS_PER_SEC;
                
            if(rank_cnt > 0 && spectralEfficiencyOverRanks(rank_cnt) < spectralEfficiencyOverRanks(rank_cnt - 1))
                break;
        }
        if(maxRank > 1)
            rank = max_index(spectralEfficiencyOverRanks)+1;
        CQIInfo = CQI_Info(csiInfoPerRank(rank-1),subframeNumber,true);
        
        delete linkParam;
  }
  
  double rankComputationTime = sum(computationTimeOverRanks);
  CQIInfo.computationTime=computationTimeOverRanks(rank-1);

  #ifdef CSI_DEBUG
  cout<<"\nGeometry SINR : "<<geometrySINRindB<<"\t PrePSINR : "<<prePSINR;
  if(spectralEfficiencyOverRanks!=zeros(maxRank))
      cout<<"\t PostPSINRs : "<<avgPostPSINR<<"\tspectralEfficiencyOverRanks : "<<spectralEfficiencyOverRanks <<"\t CQI : "<<CQIperCodewords<<" EffSINRPerCW: "<<effSINRPerCW;
  else
      cout<<" ConditionNumberOverRanks: "<<avgConditionNumberOverRanks;
  cout<<" Rank : "<<rank<<endl;
  #endif
  
//   avgConditionNumberOverRanks = concat(avgConditionNumberOverRanks,rankComputationTime);
  
  //   downlinkCQIInfo->cqiComputationInfo.isRankComputed=true;
  cqiMeasurementConfig.numberOfLayers=rank;
  cqiMeasurementConfig.numberOfCodeWords=(rank > 4) ? 2 : 1;
  
  if(traceValuePerTraceName["EnableUElogs"]=="true")
      printRI(serviceNodeID,eNBID,rank,prePSINR,avgPostPSINR(rank-1),spectralEfficiencyOverRanks,subframeNumber,avgConditionNumberOverRanks,RIlogsFileName,ss.str());
  
  int measurementIndx=downlinkCQIInfo->perUserFeedback.getMeasurementIndx(cqiMeasurementConfig.requestID);
  
  if(isAperiodicComputation)
  {
    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).rankAperiodic=rank;
    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).rankPeriodic=-1;
  }
  else      
  {
    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).rankPeriodic=rank;
    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).rankAperiodic=-1;
    
  }
  
  // FIXME Modify the report for periodic mode in NR 
  // This code is  to avoid WB CSI computation again in CQI Estimator. 
  // Need to modify appropriate in case of NR
  if(!isAperiodicComputation)
  {
    if(downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger)
    {
      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.pmiIndex=tempPMI(rank-1);
      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1=tempPMI_I1(rank-1);
      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord=CQIperCodewords(rank-1);
      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.isValid=true;
      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).isValid=true;
      downlinkCQIInfo->perUserFeedback.isValid=true;
      
      if(traceValuePerTraceName["EnablePrints"]=="true")
      {
	if(cqiMeasurementConfig.numberOfCodeWords==1)
	  cout<<"[detl:]Feedback for UE "<<setw(3)<<serviceNodeID<<" , Wideband PMI : "<<setw(2)<<tempPMI(rank-1)<<" , Wideband CQI : "<<setw(5)<<downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0)<< " , effSINRsPerCW : "<<setw(11)<<effSINRPerCW(rank-1)(0)<<" in subframe "<<setw(5)<<subframeNumber<<endl;
	else
	  cout<<"[detl:]Feedback for UE "<<setw(3)<<serviceNodeID<<" , Wideband PMI : "<<setw(2)<<tempPMI(rank-1)<<" , Wideband CQI : "<<setw(2)<<downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0)<<" "<<setw(2)<< downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(1)<< " , effSINRsPerCW : "<<setw(5)<<effSINRPerCW(rank-1)(0)<<" "<<setw(5)<<effSINRPerCW(rank-1)(1)<<" in subframe "<<setw(5)<<subframeNumber<<endl;
      }
      if(traceValuePerTraceName["EnableUElogs"]=="true")
	PrintCQIindices(serviceNodeID,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord,modOrderPerCW(rank-1),effSINRPerCW(rank-1), subframeNumber,WideBandCQIlogsFileName,tempPMI(rank-1));
    }      
  }
  downlinkCQIInfo->perUserFeedback.isValid=true;
  return rank;
}

int CQIEstimatorNR::computeDLRankForTypeII(CQIMeasurementConfig_S &cqiMeasurementConfig, FrameStructure& frameStructure, DemodulationScheme_E& demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, Array< cmat >& rxFrame, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, double noiseVariance, double restOfInterferenceInDB, int ServiceNodeID, int subframeNumber, string WideBandCQIlogsFileName,string RIlogsFileName,bool isAperiodicComputation)
{
  TransmissionScheme_E txScheme = cqiMeasurementConfig.transmissionScheme;
  checkNRTransmissionScheme(txScheme);
  
  configureCQIEstimatorNR(cqiMeasurementConfig.cqiPorts,frameStructure);
  
  int nSubbands = downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic.length();
  
  CodeBookConfig_S codebookConfig = mCodebook->getCodeBookConfig();
  
  int Lbeams = codebookConfig.LBeams;
  int nPSK = codebookConfig.nPSK;
  
  int DLRBs=cqiMeasurementConfig.nRBs;
  int rbNumber=cqiMeasurementConfig.startRB;
  
  //CQI extracting the channel over all the DLRBS on positions corresponding to CRS Ports 
  getRawPhysicalChannelAcrossRE(entireRawChannel,resourceMapper,myChannel,rbNumber,true,reCountPerRb,DLRBs,portLocations);
  perToneInterferenceCovariance=getPerToneInterferenceCovariance(rxFrame,frameStructure,resourceMapper,true,myChannel,portToAntennaMapper,refSignal,rbRSPattern, portReferenceNumbers,txScheme,reCountPerRb,rbNumber,DLRBs,CSIRSPorts,inv_dB(myTxSCPowerInDB),RSRP);
  //avgWidebandChannel = average(entireRawChannel);  --Add if needed.
  
  int nRxAntenna = myChannel(0).length();
  int maxRankUE = (nCSIRSPorts > 8) ? 8 : nCSIRSPorts;
  int maxRank = (codebookConfig.codeBookType == _NR_CODEBOOK_TYPE_I_) ? min(maxRankUE,nRxAntenna) : min(2,nRxAntenna);
  
  int Rank = 1;
  
  Array<ivec> CQIperCodewords(maxRank);
  Array<ivec> modOrderPerCW(maxRank);
  Array<vec> effSINRPerCW(maxRank);
  vec spectralEfficiencyOverRanks=zeros(maxRank);
  vec avgPPSINR=zeros(maxRank)-1;
  ivec tempPMI=zeros_i(maxRank)-1;
  ivec tempPMI_I1=zeros_i(maxRank)-1;
  
  Array < Array<vec> > sinrPerCodewordPerRank(maxRank);
  
  int nREsPerRB = getNumOfREsPerRB(CSIRSPorts,frameStructure,cqiMeasurementConfig.rsSymPositions,cqiMeasurementConfig.rsScPositions);
  
  Array<Type1HalfBeamWithi1> type1halfBeamswithi1 = mCodebook->getType1HalfBeam();
  
  CSI_Info_S csiInfo;
  
  for(int rank_cnt=0;rank_cnt<maxRank;rank_cnt++)
  {
    int nLayers = cqiMeasurementConfig.numberOfLayers = rank_cnt + 1 ;
    int nCodewords = (nLayers > 4) ? 2 : 1;
    
    Type2coefficients ampAndPhaseCooeff = getBestAmplitudeAndPhase(type1halfBeamswithi1,Lbeams,nPSK,demodulationScheme,inv_dB(myTxSCPowerInDB),nLayers,rbNumber,DLRBs);
    
    Array<cmat> orthBeams(Lbeams);
    for(int i=0;i<Lbeams;i++)
      orthBeams(i) = type1halfBeamswithi1(ampAndPhaseCooeff.orthBeamIndices(i)).Halfbeam;
    
    Array<vec> sinrPerCodewordtemp;// <codewords<REs>>
    sinrPerCodewordPerRank(rank_cnt).set_length(nCodewords);
    Array<ivec> layerIndicationPerCW;
    
    int currentRB=0, localRB=0;
    for(int subbandCnt=0;subbandCnt<nSubbands;subbandCnt++)
    {
      int nRBs = downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(subbandCnt);
      
      if(currentRB>=rbNumber && currentRB<(rbNumber+DLRBs)) // Valid subband for the measurement config
      {
        Array<cmat> type2Precoder = mCodebook->getType2PrecoderbyCombineLbeams(orthBeams,ampAndPhaseCooeff.finalAmplitude,ampAndPhaseCooeff.finalSubBandPhasePerOrthBeamPerPolarization,nLayers,subbandCnt,nPSK);
        
        Array< Array<vec> > sinrPerPMI = computeSinrPerCWForCodeBookNR(txScheme, myTxSCPowerInDB,demodulationScheme,portToAntennaMapper,currentRB,nLayers,nCodewords,nRBs,1,type2Precoder);   // <pmi<codewords<REs>>>>
        csiInfo = getBestPMIandCQI(sinrPerPMI,nRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
        
        // concatinating subband sinr based on chosen PMI for respective subband
        concatInArray(sinrPerCodewordPerRank(rank_cnt),sinrPerPMI(csiInfo.PMI));
        localRB+=nRBs;
      }
      currentRB += nRBs;
    }
    
    // Calculation of wide band CQI
    csiInfo = computeCQIPerCodewords(sinrPerCodewordPerRank(rank_cnt),DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
    CQIperCodewords(rank_cnt) = csiInfo.CQIindexPerCW;
    
    spectralEfficiencyOverRanks(rank_cnt) = csiInfo.specEff;
    
    effSINRPerCW(rank_cnt) = csiInfo.effSINRsPerCW;
    avgPPSINR(rank_cnt) = computeAvgSINRindB(effSINRPerCW(rank_cnt),rank_cnt+1);
    
    if(rank_cnt > 0 && spectralEfficiencyOverRanks(rank_cnt) < spectralEfficiencyOverRanks(rank_cnt - 1))
      break;
  }
  
  if(maxRank > 1)
    Rank = max_index(spectralEfficiencyOverRanks)+1;
  
  
  #ifdef RANK_DEBUG
  cout<<"PPSINRs : "<<avgPPSINR<<"\tspectralEfficiencyOverRanks : "<<spectralEfficiencyOverRanks <<"\t CQI : "<<CQIperCodewords<<"\tRank : "<<Rank<<endl;
  #endif
  
  //   downlinkCQIInfo->cqiComputationInfo.isRankComputed=true;
  cqiMeasurementConfig.numberOfLayers = Rank;
  cqiMeasurementConfig.numberOfCodeWords=(Rank > 4) ? 2 : 1;
  
  if(traceValuePerTraceName["EnableUElogs"]=="true")
    PrintRI(ServiceNodeID,Rank,avgPPSINR(Rank-1),spectralEfficiencyOverRanks,subframeNumber,RIlogsFileName);
  
  int measurementIndx=downlinkCQIInfo->perUserFeedback.getMeasurementIndx(cqiMeasurementConfig.requestID);
  
  if(isAperiodicComputation)
  {
    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).rankAperiodic=Rank;
    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).rankPeriodic=-1;
    if(traceValuePerTraceName["EnablePrints"]=="true")
    {
      if(cqiMeasurementConfig.numberOfCodeWords==1)
        cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" RI : "<<setw(1)<<Rank<<" , effSINRsPerCW : "<<setw(11)<<effSINRPerCW(Rank-1)(0)<<" in subframe "<<setw(5)<<subframeNumber<<endl;
      else
        cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" RI : "<<setw(1)<<Rank<<" , effSINRsPerCW : "<<setw(5)<<effSINRPerCW(Rank-1)(0)<<" "<<setw(5)<<effSINRPerCW(Rank-1)(1)<<" in subframe "<<setw(5)<<subframeNumber<<endl;
    }
  }
  else      
  {
    if(traceValuePerTraceName["EnablePrints"]=="true")
    {
      if(cqiMeasurementConfig.numberOfCodeWords==1)
        cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" RI : "<<setw(1)<<Rank<<" , effSINRsPerCW : "<<setw(11)<<effSINRPerCW(Rank-1)(0)<<" in subframe "<<setw(5)<<subframeNumber<<endl;
      else
        cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" RI : "<<setw(1)<<Rank<<" , effSINRsPerCW : "<<setw(5)<<effSINRPerCW(Rank-1)(0)<<" "<<setw(5)<<effSINRPerCW(Rank-1)(1)<<" in subframe "<<setw(5)<<subframeNumber<<endl;
    }
    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).rankPeriodic=Rank;
    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).rankAperiodic=-1;
  }
  
  downlinkCQIInfo->perUserFeedback.isValid=true;
  return Rank;
}
