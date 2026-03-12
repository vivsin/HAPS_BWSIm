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


#include "../include/MainFunctions.h"

// AssociationInfo_S associateNodes(SystemInfo_S &mySysInfo, Array<ivec> &strongServerNodesPerServiceNode,ivec NodeCount){
//   
//   string linkTableFileName=mySysInfo.resultFiles.linkTableFileName;
//   AssociationInfo_S associationInfo;
//   Array<string> filenames(5);
//   filenames(0) = mySysInfo.resultFiles.directory + "CouplingGain_Downlink.txt";
//   filenames(1) = mySysInfo.resultFiles.directory + "CouplingGain_Uplink.txt";
//   filenames(2) = mySysInfo.resultFiles.directory + "SINRDistribution.txt";
//   filenames(3) = mySysInfo.resultFiles.directory + "initialAssociationLogs.txt";
//   filenames(4) = mySysInfo.resultFiles.directory + "initialAssociationInfo.txt";
//   
//   InitialAssociationMetric associationMetric = (mySysInfo.initialAssociationMetric == _PATHLOSS_) ? _RSRP_ : mySysInfo.initialAssociationMetric;
//   
//   AssociationChannelInfo_S associationChannelInfo;
//   associateNodes(associationInfo,linkTableFileName,mySysInfo.carrierInfo,mySysInfo.strongInterferersToSimulate,strongServerNodesPerServiceNode,filenames,mySysInfo.maximumServiceNodesPerServerNode,associationMetric);
//   
//   return(associationInfo);
// }

// void transmitPrachSubframe(Array<TxFrameInfo_S> &myTxFrameInfo,Array<PrachTxFrameInfo_S> &prachTxFrameInfo,ivec txFrameInfoIndex)
// {
//   for(int tx_cnt=0;tx_cnt<prachTxFrameInfo.length();tx_cnt++)
//   {
//     // 		timeDomainFrame=getTimeDomainFrame(prachTxFrameInfo(tx_cnt).txFrame,mySysInfo.BandwidthInfo.fftSize);
//     myTxFrameInfo(txFrameInfoIndex(tx_cnt)).isTransmitting=true;
//     myTxFrameInfo(txFrameInfoIndex(tx_cnt)).isTimeDomainSubframeAvailable=true;
//     myTxFrameInfo(txFrameInfoIndex(tx_cnt)).timeDomainSubframe = prachTxFrameInfo(tx_cnt).txFrame;
//   }
//   
//   
// }



cvec hannWindow(int length){
  vec windowTaps(length);
  int l=floor_i(length/2);
  for(double tapcnt=-l;tapcnt<l;tapcnt=tapcnt+1.0){
    windowTaps[tapcnt+l] = pow(0.5*(1 + cos(2.0*pi*tapcnt/(length-1))),0.6);
  }
  return to_cvec(windowTaps);
}

cvec getSincFilter(double bandwidth, double centerFreq, double scSpacing, int Filterlen, int fftSize){
  double toneOffset=60e3;
  complex< double > j=complex< double >(0,1);
  cvec output(Filterlen);
  int Lb2=floor_i(Filterlen/2);
  for(double n=-Lb2;n<Lb2;n=n+1.0)
    output(n+Lb2)=((2.0*toneOffset+bandwidth)/(fftSize*scSpacing))*sinc((2.0*toneOffset+bandwidth)*(double)n/((double)fftSize*scSpacing))*exp(j*2.0*pi*(double)n*centerFreq/(double)fftSize);
  //     output(n+Lb2)=((2.0*toneOffset+bandwidth)/(fftSize*scSpacing))*sinc(bandwidth+2.0*toneOffset*(double)n/(double)fftSize)*exp(-j*2.0*pi*n*centerFreq/(fftSize*scSpacing));
  return output;
}


cvec getFilterTaps(int nFFT,ivec xAllocation,double scSpacingInHz,int Filterlen){
  double centerFreq;
  double bandwidthInHz = (xAllocation(1)-xAllocation(0)+1)*scSpacingInHz;
  //   if(xAllocation(0)>=nFFT/2)
  //   {
  //     xAllocation-=nFFT/2;
  //     centerFreq=(xAllocation(0)-xAllocation(xAllocation.length()-1))*scSpacing; // not sure of this negative sign!!
  //   }
  //   else
  //     centerFreq=(xAllocation(xAllocation.length()-1)-xAllocation(0))*scSpacing;
  xAllocation-=nFFT/2;
  centerFreq=(xAllocation(0)+xAllocation(1))/2;
  //   xAllocation-=centerFreq;
  //2.5*15 or 1.25*30
  
  cvec w=hannWindow(Filterlen);
  cvec pb=getSincFilter(bandwidthInHz,centerFreq,scSpacingInHz,Filterlen,nFFT);
  
  cvec filter=elem_mult(w,pb);
//   if(scSpacingInHz == 15e3){
//   ofstream fout("../../../../NRmatlab/PHYmatlab/BWSimFilter15Khz.txt");
//   for(int i=0;i<filter.length();i++){
//     fout<<filter(i)<<endl;
//   }
//   fout.close();
//   }
//   if(scSpacingInHz == 30e3){
//     ofstream fout("../../../../NRmatlab/PHYmatlab/BWSimFilter30Khz.txt");
//     for(int i=0;i<filter.length();i++){
//       fout<<filter(i)<<endl;
//     }
//     fout.close();
//   }
//   cout<<xAllocation(0)<<" "<<xAllocation(1)<<endl<<endl<<endl;
//   exit(0);
  //   complex< double > filtNorm=complex< double >(norm(filter),0);
  //   filter=filter/filtNorm;
  //   for(int i=0;i<Filterlen;i++){
  //     cout<<filter(i)<<endl;
  //   }
  //   exit(0);
  
  return (filter);
}

cvec convolve(cvec x,cvec y){
  cvec z=zeros_c((x.length()+y.length()-1));
  for(int i=0;i<z.length();i++){
    for(int j=0;j<x.length();j++){
      if(((i-j)>=0)&&((i-j)<y.length())){
	z(i)+=(x(j)*y(i-j));
      }
    }
  }
  //   append(x,zeros_c(y.length()-1));
  //   append(y,zeros_c(x.length()-1));
  //   z=ifft(elem_mult(fft(x),fft(y)));
  return z;
}


void passThroughFilter(Array<cvec> &timeDomainFrame,cvec filterTaps)
{
  for(int ant_cnt=0;ant_cnt<timeDomainFrame.length();ant_cnt++){
//     cvec filteredFrame=convolve(timeDomainFrame(ant_cnt),filterTaps);
    cvec filteredFrame=conv(timeDomainFrame(ant_cnt),filterTaps);
    cvec windowFrame=filteredFrame(filterTaps.length()/2,(filterTaps.length()/2)+timeDomainFrame(ant_cnt).length()-1);
    timeDomainFrame(ant_cnt)=windowFrame;
  }
}





void nodeTransmit(SystemInfo_S &mySysInfo,Array<NodeDevice> &myNodeDevices, ivec nodeDeviceIDs,McellSystem &mySys,Array<TxFrameInfo_S> &myTxFrameInfo, ivec txNodeIDs, int nuID,ivec txBeamIDs){
  
//   cout<<"entering to loop 1 ::"<<endl;
  if(txNodeIDs.length()==0)
  {
    for(int tx_cnt=0;tx_cnt<txNodeIDs.length();tx_cnt++)
      if(myTxFrameInfo(tx_cnt).nuParams.nuID==nuID)
	append(txNodeIDs,myTxFrameInfo(tx_cnt).nodeID);
  }
  ivec txFrameInfoIndex=getTxframeIndx(txNodeIDs,txBeamIDs,nuID,myTxFrameInfo);
//   for(int tx_cnt=0;tx_cnt<txNodeIDs.length();tx_cnt++)
//     mySys.aNodes(txNodeIDs(tx_cnt)).setCarrierFreq(mySysInfo.carrierAggregation.getUniqueCarrierFreqsInMHz()(carrierCount)*1e6);
  
  if(!mySysInfo.isWithAbstraction || (mySysInfo.isWithAbstraction && (mySysInfo.AbstractionModel == _ABSTRACTION_DECODER_ || mySysInfo.AbstractionModel == _ABSTRACTION_IDEAL_)))
  {
      if(mySys.channelCloud.isTimeDomain()) // Timedomain is common for all clouds. Hence using '0'
      {
          Array<cvec> timeDomainFrame;
          for(int tx_cnt=0;tx_cnt<txNodeIDs.length();tx_cnt++)
          {
              int txIndx=txFrameInfoIndex(tx_cnt);  
              
              if(txIndx==-1)
              {
                  cout<<"Error .. Missing txFrameInfo during nodeTransmit.. "<<endl;abort();
              }
              
              double currentTime=myTxFrameInfo(txIndx).nuParams.ttiDurationInSec*myTxFrameInfo(txIndx).ttiCount;
              if(myTxFrameInfo(txIndx).isTransmitting)
              {
                  
                  int nodeIndx=find(nodeDeviceIDs,txNodeIDs(tx_cnt));
                  int nuIndx=myNodeDevices(nodeIndx).getNUIndx(nuID);
                  ivec xAllocation=myNodeDevices(nodeIndx).L1CorePerNU(nuIndx).nuParameters.BSBWPLocations(0);// considering only 1 BWP
                  int nuFactor=myNodeDevices(nodeIndx).L1CorePerNU(nuIndx).nuParameters.subcarrierSpacingInHz/15000;
                  bool isDownlink=false;
                  if(find(mySysInfo.serverNodes,txNodeIDs(tx_cnt))!=-1) isDownlink=true;
                  
                  ivec subcarrierIndx(2);
                  int fftSize;
                  if(isDownlink)
                  {
                      subcarrierIndx(0)=myNodeDevices(nodeIndx).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getPRBSubcarrierStartLocationinEachSlot(xAllocation(0))(0)(0);
                      subcarrierIndx(1)=myNodeDevices(nodeIndx).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getPRBSubcarrierStartLocationinEachSlot(xAllocation(1))(0)(11);
                      fftSize=myNodeDevices(nodeIndx).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getFFTSize();
                  }
                  else
                  {
                      subcarrierIndx(0)=myNodeDevices(nodeIndx).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getPRBSubcarrierStartLocationinEachSlot(xAllocation(0))(0)(0);
                      subcarrierIndx(1)=myNodeDevices(nodeIndx).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getPRBSubcarrierStartLocationinEachSlot(xAllocation(1))(0)(11);
                      fftSize=myNodeDevices(nodeIndx).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getFFTSize();
                  } 
                  
                  // 	  if(myTxFrameInfo(txIndx).isTimeDomainSubframeAvailable)
                  timeDomainFrame=myTxFrameInfo(txIndx).timeDomainSubframe;
                  // 	    if(nuID==1){
                  // 	      ofstream fout("../../../../NRmatlab/PHYmatlab/BWSimSubFrame_15Khz.txt");
                  // 	      for(int i=0;i<timeDomainFrame(0).length();i++){
                  // 		fout<<timeDomainFrame(0)(i)<<endl;
                  // 	      }
                  // 	      fout.close();
                  // 	    }
                  // 	    if(nuID==2){
                  // 	      ofstream fout("../../../../NRmatlab/PHYmatlab/BWSimSubFrame_30Khz.txt");
                  // 	      for(int i=0;i<timeDomainFrame(0).length();i++){
                  // 		fout<<timeDomainFrame(0)(i)<<endl;
                  // 	      }
                  // 	      fout.close();
                  // 	    }
                  // 	    cout<<subcarrierIndx<<endl;
                  if(myNodeDevices(nodeIndx).L1CorePerNU(nuIndx).nuParameters.applyFilter)
                      passThroughFilter(timeDomainFrame,getFilterTaps(fftSize,subcarrierIndx,myNodeDevices(nodeIndx).L1CorePerNU(nuIndx).nuParameters.subcarrierSpacingInHz,fftSize/2));
                  // 	    if(nuID==1){
                  // 	      ofstream fout("../../../../NRmatlab/PHYmatlab/BWSimFilteredSubFrame_15Khz.txt");
                  // 	      for(int i=0;i<timeDomainFrame(0).length();i++){
                  // 		fout<<timeDomainFrame(0)(i)<<endl;
                  // 	      }
                  // 	      fout.close();
                  // 	    }
                  // 	    if(nuID==2){
                  // 	      ofstream fout("../../../../NRmatlab/PHYmatlab/BWSimFilteredSubFrame_30Khz.txt");
                  // 	      for(int i=0;i<timeDomainFrame(0).length();i++){
                  // 		fout<<timeDomainFrame(0)(i)<<endl;
                  // 	      }
                  // 	      fout.close();
                  // 	      exit(0);
                  // 	    }
                  // 	  else
                  // 	    timeDomainFrame=getTimeDomainFrame(myTxFrameInfo(txIndx).txFrame,myTxFrameInfo(txIndx).txFrame(0).rows());
                  mySys.transmitData(myTxFrameInfo(txIndx).nodeID,myTxFrameInfo(txIndx).txBeamID,myTxFrameInfo(txIndx).txFrame(0).rows(),timeDomainFrame,currentTime,2);
                  myTxFrameInfo(txIndx).clear();
              }
          }
      }
      else
      {
          Array<cvec> freqDomainFrame;
          for(int tx_cnt=0;tx_cnt<txNodeIDs.length();tx_cnt++)
          {
              int txIndx=txFrameInfoIndex(tx_cnt);  
              mySys.channelCloud.clearChannelBuffer(txNodeIDs(tx_cnt,tx_cnt),to_ivec(myTxFrameInfo(txIndx).txBeamID));
	double currentTime=myTxFrameInfo(txIndx).nuParams.ttiDurationInSec*myTxFrameInfo(txIndx).ttiCount;
	if(myTxFrameInfo(txIndx).isTransmitting)
	{
	  freqDomainFrame=getFreqDomainFrame(myTxFrameInfo(txIndx).txFrame);
      mySys.transmitData(myTxFrameInfo(txIndx).nodeID,myTxFrameInfo(txIndx).txBeamID,myTxFrameInfo(txIndx).txFrame(0).rows(),freqDomainFrame,currentTime);
	  myTxFrameInfo(txIndx).clear();	  
	}
      }
    }
  }
}


bool isToEnterReceiveLoop(Receiver *myReceiver,bool isScheduled,SystemInfo_S &mySysInfo,AssociationInfo_S &associationInfo,bool isDownlink){
  
  bool goFlag;
  if(isDownlink)
  {
    //if((isScheduled && (mySysInfo.linkSimulationInfo.isEnabled || myReceiver->isLinkAdaptationEnabled()==false || subframeNumber>=(myReceiver->cqiEstimator.downlinkCQIInfo->cqiComputationInfo.CQIPMIOffset+mySysInfo.dlCQIModeInfo.CQIProcessingDelay))) || myReceiver->cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger||myReceiver->cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger ||myReceiver->cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQITriggered|| myReceiver->cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isrankTrigger/*mySchedulerToReceiverInfo.serviceNodesInfo->getFeedbackFlag(rxNodeID)*/)
    if(isScheduled || myReceiver->isToComputeDLFeedback())
      goFlag=true;
    else
      goFlag=false;
  }
  else
  {
    if(isScheduled)      goFlag=true;
    else      goFlag=false;
  }

  int srvrIndx=getServerIndx(associationInfo,myReceiver->getRxNodeID()); 
  int associatedServerNode=associationInfo.serverNodes(srvrIndx);
  #ifdef USING_MPI
  if(goFlag)
  {
      
    if(find(mySysInfo.mpiInfo.srvrIndxListPerTaskid(currentTaskid),srvrIndx)==-1)
        goFlag=false;
    
  }
  #endif
  if(goFlag)
    if(mySysInfo.receptionModel.enableRestrictedRecepition)
    {
        if(find(mySysInfo.receptionModel.serverNodesWithActualReception,associatedServerNode)==-1)
	  goFlag=false;
	}
    
    return(goFlag);
}


bool receiveSubframe(RxFrameInfo_S &rxFrameInfo,Receiver *myReceiver,SystemInfo_S &mySysInfo ,McellSystem &mySys, AssociationInfo_S &associationInfo, StrongAndWeakTransmittersInfo_S &myStrongAndWeakTransmittersInfo, TransmissionType txType, bool isScheduled, double currentTimeInSec,SchedulerToReceiverInfo_S mySchedulerToReceiverInfo,int rxBeamID,int forceReception,ivec forceScheduledIDs){
  
  int rxNodeID=myReceiver->getRxNodeID();
  int nuID=myReceiver->nuParams.nuID;
  
  int rxAntenna=getAntennaCountForBeam(mySys,rxNodeID,nuID,rxBeamID);
  myReceiver->setRxAntennaCount(rxAntenna);
  
  rxFrameInfo.clear();
  rxFrameInfo.rxNodeID=rxNodeID;
  
  bool goFlag1=isToEnterReceiveLoop(myReceiver,isScheduled,mySysInfo,associationInfo,txType==_TRANSMISSION_TYPE_DL_);
   
  bool GenerateChannel=false;
  int srvrIndx=getServerIndx(associationInfo,myReceiver->getRxNodeID()); 
  int associatedServerNode=associationInfo.serverNodes(srvrIndx);
  //Forcing receiption in co-operating nodes even if they are not scheduled
  if(mySysInfo.compInfo.isWithCoMP && mySysInfo.compInfo.compScheme==_COMP_SCHEME_JOINT_RX_)
    #ifdef USING_MPI
    if(find(mySysInfo.mpiInfo.srvrIndxListPerTaskid(currentTaskid),srvrIndx)!=-1)
        forceReception=1;
    
    #endif
    
    
    
  if(forceReception==1) goFlag1=true;
  else if(forceReception==-1) goFlag1=false;
  
  if(goFlag1)
  {
    if(mySysInfo.isWithAbstraction && mySysInfo.AbstractionModel==_ABSTRACTION_CQI_)
    {
      if(myReceiver->cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger || myReceiver->cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger || myReceiver->cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isrankTrigger)
	GenerateChannel=true;
    }
    else
      GenerateChannel=true;
    
    int nuIndx=myReceiver->myNodeDevice->getNUIndx(mySchedulerToReceiverInfo.rxNUID);
    
    if(txType==_TRANSMISSION_TYPE_DL_)
    {
      if(isScheduled && myReceiver->myNodeDevice->ttiCountPerNU(nuIndx)>(myReceiver->receiverConfig.dlCQIModeInfo.CQIPMIOffset))
      {
	if(traceValuePerTraceName["EnablePrints"]=="true") 
	{
	  cout<<"[detl:]Received data in node "<<setw(5)<<rxNodeID<<" from txNode "<<setw(5)<<mySchedulerToReceiverInfo.txNodeIDs(0)<<"  scheduled in "<<setw(3)<<mySchedulerToReceiverInfo.dataBlkInfo(0).resourceBlocks.length()<<" RBs  and "<<mySchedulerToReceiverInfo.dataBlkInfo(0).symPos.length()<<" Symbols  ";
	  if(mySchedulerToReceiverInfo.dataBlkInfo(0).dataGenInfo.mcsIndices.length()==1)
	    cout<<"  With mcs : "<<setw(6)<<mySchedulerToReceiverInfo.dataBlkInfo(0).dataGenInfo.mcsIndices(0)<<"  isNewTransmission : "<<setw(4)<<(bool)mySchedulerToReceiverInfo.dataBlkInfo(0).harqInfo.isNewTransmission(0)<<endl;
	  else
	    cout<<"  With mcs : "<<setw(3)<<mySchedulerToReceiverInfo.dataBlkInfo(0).dataGenInfo.mcsIndices(0)<<setw(3)<<mySchedulerToReceiverInfo.dataBlkInfo(0).dataGenInfo.mcsIndices(1)<<"  isNewTransmission : "<<setw(2)<<(bool)mySchedulerToReceiverInfo.dataBlkInfo(0).harqInfo.isNewTransmission(0)<<setw(2)<<(bool)mySchedulerToReceiverInfo.dataBlkInfo(0).harqInfo.isNewTransmission(1)<<endl;
	}
      }
    }
    else
    {
      if(isScheduled && myReceiver->myNodeDevice->ttiCountPerNU(nuIndx)>(myReceiver->receiverConfig.ulCQIModeInfo.CQIPMIOffset))
      {
	ivec rbs(0);
	Array<ivec> mcs(0);
	for(int blk_cnt=0;blk_cnt<mySchedulerToReceiverInfo.dataBlkInfo.length();blk_cnt++)
	{
	  append(rbs,mySchedulerToReceiverInfo.dataBlkInfo(blk_cnt).resourceBlocks.length());
	  append(mcs,mySchedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.mcsIndices);
	}
	if(traceValuePerTraceName["EnablePrints"]=="true") 
	{
	    cout<<"[detl:]Received data in node "<<setw(5)<<rxNodeID<<" from txNodes "<<setw(5)<<mySchedulerToReceiverInfo.txNodeIDs<<"  scheduled in  "<<setw(2)<<rbs<<"  RBs ";
	    cout<<" with mcs : "<<mcs<<" respectively ..."<<endl;
	  }
	}
      } 
    }
    if(GenerateChannel)
    {
      if(isScheduled)
      {
	rxFrameInfo.signalNodeIDs=mySchedulerToReceiverInfo.txNodeIDs;
	append(rxFrameInfo.signalNodeIDs,mySchedulerToReceiverInfo.rsTxNodeIDs);
      }
      else
      {
     if(txType==_TRANSMISSION_TYPE_DL_)
        rxFrameInfo.signalNodeIDs=to_ivec(associatedServerNode);
	else
	{
	  rxFrameInfo.signalNodeIDs=mySchedulerToReceiverInfo.rsTxNodeIDs;
	  if(forceReception)
	    append(rxFrameInfo.signalNodeIDs,forceScheduledIDs);
	  
	  if(rxFrameInfo.signalNodeIDs.length()==0)
	    return(false);
	}
      }
      rxFrameInfo.signalNodeIDs=findUniqueNumbers(rxFrameInfo.signalNodeIDs);
      Size_S physicalFrameSize;
      int nuIndx=myReceiver->myNodeDevice->getNUIndx(nuID);
      physicalFrameSize.numberOfSubcarriers=myReceiver->myNodeDevice->L1CorePerNU(nuIndx).nuParameters.fftSize;
    if(txType==_TRANSMISSION_TYPE_DL_)
	physicalFrameSize.numberOfSymbols=myReceiver->myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitSize("nullRB").numberOfSymbols*myReceiver->myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure.getRuLevels();
      else
	physicalFrameSize.numberOfSymbols=myReceiver->myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure.getResourceUnitSize("nullRB").numberOfSymbols*myReceiver->myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure.getRuLevels();
      
      if(!mySysInfo.isWithAbstraction || (mySysInfo.isWithAbstraction && (mySysInfo.AbstractionModel == _ABSTRACTION_DECODER_ || mySysInfo.AbstractionModel == _ABSTRACTION_IDEAL_)))
      {
	rxFrameInfo.rxFrame.set_length(rxAntenna);
	for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntenna;rxAntenna_cnt++)
	  rxFrameInfo.rxFrame(rxAntenna_cnt).set_size(physicalFrameSize.numberOfSubcarriers,physicalFrameSize.numberOfSymbols);
      }
      
      Array<ivec> signalNodeAntenna(rxFrameInfo.signalNodeIDs.length());
      ivec signalNodeAntennaCount(rxFrameInfo.signalNodeIDs.length());
    if(txType==_TRANSMISSION_TYPE_DL_)
      {
	if(rxFrameInfo.signalNodeIDs.length()>1)
	{
	  cout<<"Currently JT not supported in Downlink due to lack of BeamPairLink for other nodes."<<endl;
	  abort();
	}
      signalNodeAntenna(0)=getTXRUsForBeam(mySys,rxFrameInfo.signalNodeIDs(0),nuID,associationInfo.associatedBeamPairLinkforEachServiceNode(find(associationInfo.serviceNodes,rxNodeID)).srvrBeamID);
	signalNodeAntennaCount(0)=signalNodeAntenna(0).length();
	
      }
      else //! Up-link
      {
	ivec srvcIndices=find(associationInfo.serviceNodes,rxFrameInfo.signalNodeIDs);
	for(int indx=0;indx<rxFrameInfo.signalNodeIDs.length();indx++)
	{
          signalNodeAntenna(indx)=getTXRUsForBeam(mySys,rxFrameInfo.signalNodeIDs(indx),nuID,associationInfo.associatedBeamPairLinkforEachServiceNode(srvcIndices(indx)).srvcBeamID);
	  signalNodeAntennaCount(indx)=signalNodeAntenna(indx).length();
	}
      }
      rxFrameInfo.channelHandler.initChannelHandler(rxFrameInfo.signalNodeIDs,signalNodeAntennaCount,myReceiver->getRxAntennaCount(),physicalFrameSize.numberOfSubcarriers,physicalFrameSize.numberOfSymbols);
      
      Array<cvec> temp;
      ChannelOutput_S channelOutput;
      double endTime;
      for(int sym_cnt=0;sym_cnt<physicalFrameSize.numberOfSymbols;sym_cnt++)
      {
	double currentTime=getCurrentTime(currentTimeInSec,sym_cnt,myReceiver->myNodeDevice->L1CorePerNU(nuIndx).nuParameters.fftSize,myReceiver->myNodeDevice->L1CorePerNU(nuIndx).nuParameters.samplingFrequencyInHz);
	// Endtime used only in case of timeDomainFrame
	endTime=getEndTime(currentTimeInSec,sym_cnt,myReceiver->myNodeDevice->L1CorePerNU(nuIndx).nuParameters.fftSize,myReceiver->myNodeDevice->L1CorePerNU(nuIndx).nuParameters.samplingFrequencyInHz);
	if(!mySysInfo.isWithAbstraction || (mySysInfo.isWithAbstraction && (mySysInfo.AbstractionModel == _ABSTRACTION_DECODER_ || mySysInfo.AbstractionModel == _ABSTRACTION_IDEAL_)))
	{
	  bool printChannel=false;
	  if(mySysInfo.linkSimulationInfo.isEnabled && traceValuePerTraceName["EnableChannelLogs"]=="true") printChannel=true;
	  ivec interfererNodeIDs=remove(myStrongAndWeakTransmittersInfo.strongTransmitters,rxFrameInfo.signalNodeIDs);
	  
	channelOutput=mySys.receiveData(rxNodeID,rxBeamID,mySysInfo.antenneCorrelation,sym_cnt,currentTime,endTime,printChannel,myStrongAndWeakTransmittersInfo.strongTransmitters,interfererNodeIDs);
	  
	}
	else
	channelOutput=mySys.generateChannel(rxNodeID,rxBeamID,mySysInfo.antenneCorrelation,currentTime,myReceiver->myNodeDevice->L1CorePerNU(nuIndx).nuParameters.fftSize,myStrongAndWeakTransmittersInfo.strongTransmitters);
      
     /* if(mySysInfo.modelWeakInterferenceAsNoise && myStrongAndWeakTransmittersInfo.weakTransmitters.length())
      {
	for(int tx_cnt=0;tx_cnt<txNodeIndicesToModelAsNoise.length();tx_cnt++)
	  rxFrameInfo.txNodePowerInDBmToModelAsNoise(tx_cnt)=myTxFrameInfo(txNodeIndicesToModelAsNoise(tx_cnt)).txSubcarrierPowerInDBmPerAntenna;
	if(!mySysInfo.isWithAbstraction)
	{
	  double correctionFactorInDB=0;
	  if(!isDownlink)
	    if(mySysInfo.serverNodes.length()>(mySysInfo.strongInterferersToSimulate+1)) //Assuming only 1 desired signal...Need to reconsider for Comp scenarios...
	      correctionFactorInDB=dB((double)(mySysInfo.serverNodes.length()-(mySysInfo.strongInterferersToSimulate+1))/(double)rxFrameInfo.txNodesToModelAsNoise.length());
	    
	    rxFrameInfo.txNodePowerInDBmToModelAsNoise+=correctionFactorInDB;
	
	  rxFrameInfo.ROIpowerInWatts=mySys.channelCloud.addSignalsModelledAsNoise(rxNodeID,channelOutput,currentTime,rxFrameInfo.txNodesToModelAsNoise,rxFrameInfo.txNodePowerInDBmToModelAsNoise);
	}
	else
	{
	  rxFrameInfo.ROIpowerInWatts=mySys.channelCloud.getInstantNoiseModellingPower(rxNodeID,currentTime,rxFrameInfo.txNodesToModelAsNoise,rxFrameInfo.txNodePowerInDBmToModelAsNoise);
	}
      }
      
      */ 
	
	if(mySys.channelCloud.isTimeDomain())
	{
	  if(rxFrameInfo.rxFrameInTimeDomain.length()==0)
	  {rxFrameInfo.rxFrameInTimeDomain=channelOutput.data;
	    //cout<<"channelOutput.data"<<channelOutput.data(0)(0,10)<<endl;
	    //abort();	  
	  }
	  else
	    for(int ant_cnt=0;ant_cnt<channelOutput.data.length();ant_cnt++)
	    { append(rxFrameInfo.rxFrameInTimeDomain(ant_cnt),channelOutput.data(ant_cnt));
	      //cout<<"rxFrameInfo.rxFrameInTimeDomain(ant_cnt).length"<<rxFrameInfo.rxFrameInTimeDomain(ant_cnt).length()<<endl;	    
	    }
	    removeCyclicPrefix(channelOutput.data,myReceiver->myNodeDevice->L1CorePerNU(nuIndx).nuParameters.fftSize);
	  removeCyclicPrefix(channelOutput.summedUpSignalPerAntennaPerTone,myReceiver->myNodeDevice->L1CorePerNU(nuIndx).nuParameters.fftSize);
	  channelOutput.takeFFT(myReceiver->myNodeDevice->L1CorePerNU(nuIndx).nuParameters.fftSize);
	}
	
	#ifdef USING_CALIBRATION
	if(traceValuePerTraceName["pdschDemodPerfTesting"]=="true")
	{
	  //if(myReceiver->isIdealChannel())
	  for(int node_cnt=0;node_cnt<rxFrameInfo.txNodesToReceiveSignalFrom.length();node_cnt++)
	    rxFrameInfo.channelHandler.loadChannel(channelOutput.channels(node_cnt),rxFrameInfo.txNodesToReceiveSignalFrom(node_cnt),sym_cnt);
	}
	else
	  #endif
	{
	  // 	  if(myReceiver->isIdealChannel())
	  ivec signalNodeIndices=find(channelOutput.txNodeIDs,rxFrameInfo.signalNodeIDs);
	  for(int node_cnt=0;node_cnt<rxFrameInfo.signalNodeIDs.length();node_cnt++)
	  {
	    // channelOutput has channel from all antennas(TXRUs) of transmitter. Truncating the channel onyl over desired TXRUs - Dhiv
	    Array< Array<cvec> > channelForBeam=getFromArray(channelOutput.channels(signalNodeIndices(node_cnt)),signalNodeAntenna(node_cnt));
	    rxFrameInfo.channelHandler.loadChannel(channelForBeam,rxFrameInfo.signalNodeIDs(node_cnt),sym_cnt);
	  }
	}
	if(!mySysInfo.isWithAbstraction || (mySysInfo.isWithAbstraction && (mySysInfo.AbstractionModel == _ABSTRACTION_DECODER_ || mySysInfo.AbstractionModel == _ABSTRACTION_IDEAL_)))
	  setInArrayOfMatrix(rxFrameInfo.rxFrame,channelOutput.data,sym_cnt);
      if(txType!=_TRANSMISSION_TYPE_DL_)
	{
	  if(!sym_cnt)
	    rxFrameInfo.summedUpPowerOverREs=zeros(channelOutput.summedUpSignalPerAntennaPerTone(0).length());
	  for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntenna;rxAntenna_cnt++)
	    rxFrameInfo.summedUpPowerOverREs+=sqr(abs(channelOutput.summedUpSignalPerAntennaPerTone(rxAntenna_cnt)))/rxAntenna;
	}
      }
    }
    
    return(goFlag1);
    
}

//CQI "manageDLCQIReporting" funtcion manages reporting of the CQI by updating the CQI computed, also the triggering of 
//CQI periodic widebandCQI/PMI or BWPART CQI/PMI computation
// void manageDLCQIReporting(Receiver *myReceiver,int subframeNumber,CQIInfo_S &dlCQIInfo)
// {
//   static int sfn=-1;
//     
//   if((myReceiver->cqiEstimator.downlinkCQIInfo->perUserFeedback.isValid) && (myReceiver->isToReportDLCQI(subframeNumber) || (myReceiver->isToReportRI(subframeNumber) && (myReceiver->cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isRankComputed))))//CQI to update the CQI at the reporting subframe
//   {
//     if(traceValuePerTraceName["EnablePrints"]=="true")
//     {
//       if(sfn!=subframeNumber && myReceiver->isToReportDLCQI(subframeNumber))
//       {
// 	cout<<"[detl:]Downlink CQI is Reported "<<endl;
// 	sfn=subframeNumber;
//       }
//       else if(sfn!=subframeNumber && myReceiver->isToReportRI(subframeNumber))
//       {
// 	cout<<"[detl:]Downlink RI is Reported ##################### "<<endl;
// 	sfn=subframeNumber;
//       }
//     }
//     bool isRInCQIreport=(myReceiver->isToReportRI(subframeNumber) && myReceiver->isToReportDLCQI(subframeNumber));
//     bool isAperiodicReporting=(subframeNumber==myReceiver->cqiEstimator.downlinkCQIInfo->perUserFeedback.APeriodicReportedSubframe);
//     dlCQIInfo.update(myReceiver->getRxNodeID(),myReceiver->cqiEstimator.downlinkCQIInfo->perUserFeedback,subframeNumber,myReceiver->isToReportRI(subframeNumber),myReceiver->cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isRankAdaptationEnabled,isAperiodicReporting,isRInCQIreport);
//   }
//  
// }

void initLinkSimulation(SystemInfo_S &mySysInfo, ChannelCloud &myChannelCloud, Array<NodeDevice> &myNodeDevices, PerformanceMetrics_S &dlPerformanceMetric,PerformanceMetrics_S &ulPerformanceMetric,int snrLoop_cnt){
  
  if(mySysInfo.linkSimulationInfo.isEnabled)
  {
    cout<<"[both:]Running SNR "<<mySysInfo.linkSimulationInfo.snrRangeInDB(snrLoop_cnt)<<endl;
    
    double noiseVarianceInDB=-mySysInfo.linkSimulationInfo.snrRangeInDB(snrLoop_cnt); 
    
    bool isDownlink=false;
    if(mySysInfo.ulDlMode==_ULDL_MODE_DOWNLINK_) isDownlink=true;
    else if(mySysInfo.ulDlMode==_ULDL_MODE_UPLINK_) isDownlink=false;
    else	{cout<<"[both:]Error. Unknown UlDLMode to initLinkSimulation()."<<endl;abort();}
    
    myChannelCloud.clearChannelBuffer();
    myChannelCloud.clearQuasistaticChannelInfo();
    if(isDownlink)
    {
      myChannelCloud.setNoiseVariancePerDimension(mySysInfo.serviceNodes,inv_dB(noiseVarianceInDB)/2);
      dlPerformanceMetric.clear();
    }
    else
    {
      myChannelCloud.setNoiseVariancePerDimension(mySysInfo.serverNodes,inv_dB(noiseVarianceInDB)/2);
      ulPerformanceMetric.clear();
    }
  }
}

void completeSimulation(string configFileDir,string resultDir,double currentTimeInSec){
  
  cout<<"[detl:]Dumping configFiles..."<<endl;
  
  string toExec = "tar cvzf "+resultDir+"configFilesDump.tar.gz "+configFileDir+";rm -rf "+configFileDir;
  system_exec(toExec.c_str());
  
  toExec = "mv *.txt "+resultDir;
  system_exec(toExec.c_str());
   if(IsFileExists(resultDir+"log.txt"))
   {
     string toExec = "mv "+resultDir+"log.txt .";
     system_exec(toExec.c_str());
   }
  //system_exec("tar cvzf Results/configFilesDump.tar.gz configFiles/;rm -rf configFiles");
  
  string fileName = resultDir+"briefSimulationDetails.txt";
  std::ofstream op(fileName.c_str(),std::ios::out);
  op<<briefSimulationDetails(currentTimeInSec);
  op.close();
  
  #ifdef USING_CLUSTER  
  system_exec("cd Results; for f in *txt ; do mv $f `basename $f txt`dat; done");
  #endif
  
  system_exec("if [ ! -d ../Results ]; then mkdir ../Results;  fi");
    
  toExec = "echo \"../Results/$(date +\"%y-%m-%d-%T\")_Result\" |xargs -I{} -i sh -c 'tar cvzf {}.tar.gz "+resultDir+"*;'";
  system_exec(toExec.c_str()); //! To backup the results

  cout<<briefSimulationDetails(currentTimeInSec);
  captureTime(1,STOP);
  cout<<"[both:]Simulation Completed."<<endl;
}

void completeSimulation(double currentTimeInSec){
  
  cout<<"[detl:]Dumping configFiles..."<<endl;
  
  system_exec("tar cvzf Results/configFilesDump.tar.gz configFiles/;rm -rf configFiles");
  
  std::ofstream op("./Results/briefSimulationDetails.txt",std::ios::out);
  op<<briefSimulationDetails(currentTimeInSec);
  op.close();
  
  #ifdef USING_CLUSTER  
  system_exec("cd Results; for f in *txt ; do mv $f `basename $f txt`dat; done");
  #endif
  
  if(IsFileExists("simulationOutput.out"))
    system_exec("cp -r simulationOutput.out ./Results"); //! Copying log file into the Results folder
    
    system_exec("if [ ! -d ../Results ]; then mkdir ../Results;  fi");  
  system_exec("echo \"../Results/$(date +\"%y-%m-%d-%T\")_Result\" |xargs -I{} -i sh -c 'tar cvzf {}.tar.gz Results/*;'"); //! To backup the results

  cout<<briefSimulationDetails(currentTimeInSec);
  captureTime(1,STOP);
  cout<<"[both:]Simulation Completed."<<endl;
}



// void printOutputFiles(double currentTime,PerformanceMetrics_S &dlPerformanceMetric, PerformanceMetrics_S &ulPerformanceMetric, SystemInfo_S &mySysInfo, McellSystem &mySys, AssociationInfo_S &associationInfo, int snrLoop_cnt)
// {
//   if(currentTaskid==0) 
//   {
//     if(mySysInfo.simulationTimeInSec!=currentTime)
//       cout<<"[detl:]Generating Intermediate Output Files."<<endl;
//     else
//       cout<<"[detl:]Generating Final Output Files."<<endl;
//   }
//   
//   if(mySysInfo.ulDlMode!=_ULDL_MODE_UPLINK_)
//   {
//     for(int nu_cnt=0;nu_cnt<dlPerformanceMetric.nuIDs.length();nu_cnt++)
//     {
//       bool printOutput=dlPerformanceMetric.computeMetric(dlPerformanceMetric.nuIDs(nu_cnt),currentTime-mySysInfo.settlingTimeInSec,mySysInfo.carrierAggregation);
//       cout<<"printOutput : "<<printOutput<<endl;
//       if(printOutput)
//       {
//         if(mySysInfo.linkSimulationInfo.isEnabled) dlPerformanceMetric.printBERPerformanceInfo(mySysInfo.resultFiles.dlLinkPerformanceInfoFileName(nu_cnt),dlPerformanceMetric.nuIDs(nu_cnt),mySysInfo.linkSimulationInfo.snrRangeInDB(snrLoop_cnt));
//         else dlPerformanceMetric.printSystemPerformanceInfo(mySys,dlPerformanceMetric.nuIDs(nu_cnt),mySysInfo.resultFiles.dlUEPerformanceInfoFileName(nu_cnt),mySysInfo.resultFiles.dlSystemPerformanceInfoFileName(nu_cnt),associationInfo);
//       }
//     }
//   }
//   if(mySysInfo.ulDlMode!=_ULDL_MODE_DOWNLINK_)
//   {
//     for(int nu_cnt=0;nu_cnt<ulPerformanceMetric.nuIDs.length();nu_cnt++)
//     {
//       bool printOutput=ulPerformanceMetric.computeMetric(ulPerformanceMetric.nuIDs(nu_cnt),currentTime-mySysInfo.settlingTimeInSec,mySysInfo.carrierAggregation);
//       if(printOutput)
//       {
//         if(mySysInfo.linkSimulationInfo.isEnabled) ulPerformanceMetric.printBERPerformanceInfo(mySysInfo.resultFiles.ulLinkPerformanceInfoFileName(nu_cnt),ulPerformanceMetric.nuIDs(nu_cnt), mySysInfo.linkSimulationInfo.snrRangeInDB(snrLoop_cnt));
//         else ulPerformanceMetric.printSystemPerformanceInfo(mySys,ulPerformanceMetric.nuIDs(nu_cnt),mySysInfo.resultFiles.ulUEPerformanceInfoFileName(nu_cnt),mySysInfo.resultFiles.ulSystemPerformanceInfoFileName(nu_cnt),associationInfo);
//       }
//     }
//   }
// //   if(traceValuePerTraceName["channelEstimator_01_00_traceFlag"]=="true" && mySysInfo.linkSimulationInfo.isEnabled)
// //   {
// //     if(mySysInfo.ulDlMode!=_ULDL_MODE_UPLINK_)
// //       dlPerformanceMetric.printNMSEperformanceInfo("./Results/dlNMSEperformanceInfo.txt",mySysInfo.linkSimulationInfo.snrRangeInDB(snrLoop_cnt));
// //     else
// //       ulPerformanceMetric.printNMSEperformanceInfo("./Results/ulNMSEperformanceInfo.txt",mySysInfo.linkSimulationInfo.snrRangeInDB(snrLoop_cnt));
// //     exit(0);
// //   }
//   #ifdef USING_MPI
//   MPI_Barrier(MPI_COMM_WORLD);
//   #endif
//   
//   if(currentTaskid==0 && mySysInfo.simulationTimeInSec<=currentTime)
//   {
//     system_exec("rm -rf ./Results/SINRDistribution.txtTemp");
//     system_exec("rm -rf ./Results/SINRDistribution_RSRP.txtTemp");
//     
//     if(!mySysInfo.linkSimulationInfo.isEnabled) 
//     {
//       if(mySysInfo.ulDlMode!=_ULDL_MODE_UPLINK_)
//       { 
// 	for(int nu_cnt=0;nu_cnt<dlPerformanceMetric.nuIDs.length();nu_cnt++)
// 	{ 
// 	  string filename1="Results/NU_"+toString(dlPerformanceMetric.nuIDs(nu_cnt));
// 	  string filename2 =filename1+"/dlWindowBLERstats.txt.temp0";
// 	  
// 	  if(IsFileExists(filename2.c_str()))
// 	  {
// 	    string filename3="cat Results/NU_"+toString(dlPerformanceMetric.nuIDs(nu_cnt))+"/dlWindowBLERstats.txt.temp*  > Results/NU_"+toString(dlPerformanceMetric.nuIDs(nu_cnt))+"/dlWindowBLERstats.txt; rm -rf Results/NU_"+toString(dlPerformanceMetric.nuIDs(nu_cnt))+"/dlWindowBLERstats.txt.temp*";
// 	    system_exec(filename3.c_str());
// 	  }
// 	  
// 	  if(traceValuePerTraceName["EnableUElogs"]=="true")
// 	  {  
// 	    string filename4= filename1+"/WideBandCQIlogs.txt.temp0";
// 	    if(IsFileExists(filename4.c_str()))
// 	    {
// 	      string filename3="cat Results/NU_"+toString(dlPerformanceMetric.nuIDs(nu_cnt))+"/WideBandCQIlogs.txt.temp*  > Results/NU_"+toString(dlPerformanceMetric.nuIDs(nu_cnt))+"/WideBandCQIlogs.txt; rm -rf Results/NU_"+toString(dlPerformanceMetric.nuIDs(nu_cnt))+"/WideBandCQIlogs.txt.temp*";
// 	      system_exec(filename3.c_str());
// 	    }
// 	    string filename5=filename1+"/SubbandCQIlogs.txt.temp0";
// 	    if(IsFileExists(filename5.c_str()))
// 	    {
// 	      string filename3="cat Results/NU_"+toString(dlPerformanceMetric.nuIDs(nu_cnt))+"/SubbandCQIlogs.txt.temp*  > Results/NU_"+toString(dlPerformanceMetric.nuIDs(nu_cnt))+"/SubbandCQIlogs.txt; rm -rf Results/NU_"+toString(dlPerformanceMetric.nuIDs(nu_cnt))+"/SubbandCQIlogs.txt.temp*";
// 	      system_exec(filename3.c_str());
// 	    }
// 	    string filename6= filename1+"/RIlogs.txt.temp0";
// 	    if(IsFileExists(filename6.c_str()))
// 	    {
// 	      string filename3="cat Results/NU_"+toString(dlPerformanceMetric.nuIDs(nu_cnt))+"/RIlogs.txt.temp*  > Results/NU_"+toString(dlPerformanceMetric.nuIDs(nu_cnt))+"/RIlogs.csv; rm -rf Results/NU_"+toString(dlPerformanceMetric.nuIDs(nu_cnt))+"/RIlogs.txt.temp*";
// 	      system_exec(filename3.c_str());
// 	    }
// 	    string filename7=filename1+"/AperiodicCQIlogs.txt.temp0";
//         if(IsFileExists(filename7.c_str()))
//         {
//             string filename3="cat Results/NU_"+toString(dlPerformanceMetric.nuIDs(nu_cnt))+"/AperiodicCQIlogs.txt.temp*  > Results/NU_"+toString(dlPerformanceMetric.nuIDs(nu_cnt))+"/AperiodicCQIlogs.csv; rm -rf Results/NU_"+toString(dlPerformanceMetric.nuIDs(nu_cnt))+"/AperiodicCQIlogs.txt.temp*";
//             system_exec(filename3.c_str());
//         }
// 	  }      
// 	}
//       }
//       
//       if(mySysInfo.ulDlMode!=_ULDL_MODE_DOWNLINK_)
//       {
// 	for(int nu_cnt=0;nu_cnt<ulPerformanceMetric.nuIDs.length();nu_cnt++)
// 	{
// 	  string filename1="Results/NU_"+toString(ulPerformanceMetric.nuIDs(nu_cnt));
// 	  string filename2 =filename1+"/ulWindowBLERstats.txt.temp0";
// 	  
// 	  if(IsFileExists(filename2.c_str()))
// 	  {
// 	    string filename3="cat Results/NU_"+toString(ulPerformanceMetric.nuIDs(nu_cnt))+"/ulWindowBLERstats.txt.temp*  > Results/NU_"+toString(ulPerformanceMetric.nuIDs(nu_cnt))+"/ulWindowBLERstats.txt; rm -rf Results/NU_"+toString(ulPerformanceMetric.nuIDs(nu_cnt))+"/ulWindowBLERstats.txt.temp*";
// 	    system_exec(filename3.c_str());
// 	  }
// 	}
//       }
//     }
// 
//   } 
// 
//   #ifdef USING_MPI
//   MPI_Barrier(MPI_COMM_WORLD);
//   #endif
// 
// }
void printOutputFiles(double currentTime, PerformanceMetrics_S& dlPerformanceMetric, PerformanceMetrics_S& ulPerformanceMetric, PerformanceMetrics_S& slPerformanceMetric, SystemInfo_S& mySysInfo, McellSystem& mySys, AssociationInfo_S& associationInfo, int snrLoop_cnt)
{
    if(currentTaskid==0) 
    {
        if(mySysInfo.simulationTimeInSec!=currentTime)
            cout<<"[detl:]Generating Intermediate Output Files."<<endl;
        else
            cout<<"[detl:]Generating Final Output Files."<<endl;
    }
    
    if(mySysInfo.ulDlMode!=_ULDL_MODE_UPLINK_)
    {
        for(int nu_cnt=0;nu_cnt<dlPerformanceMetric.nuIDs.length();nu_cnt++)
        {
            bool printOutput=dlPerformanceMetric.computeMetric(dlPerformanceMetric.nuIDs(nu_cnt),currentTime-mySysInfo.settlingTimeInSec,mySysInfo.carrierInfo);
            cout<<"printOutput : "<<printOutput<<endl;
            if(printOutput)
            {
                if(mySysInfo.linkSimulationInfo.isEnabled) dlPerformanceMetric.printBERPerformanceInfo(mySysInfo.resultFiles.dlLinkPerformanceInfoFileName(nu_cnt),dlPerformanceMetric.nuIDs(nu_cnt),mySysInfo.linkSimulationInfo.snrRangeInDB(snrLoop_cnt));
                else dlPerformanceMetric.printSystemPerformanceInfo(&mySys,dlPerformanceMetric.nuIDs(nu_cnt),mySysInfo.resultFiles.dlUEPerformanceInfoFileName(nu_cnt),mySysInfo.resultFiles.dlSystemPerformanceInfoFileName(nu_cnt),mySysInfo.resultFiles.dlCumulativePerformanceFileName(nu_cnt),&associationInfo);
            }
        }
    }
    if(mySysInfo.ulDlMode!=_ULDL_MODE_DOWNLINK_)
    {
        for(int nu_cnt=0;nu_cnt<ulPerformanceMetric.nuIDs.length();nu_cnt++)
        {
            bool printOutput=ulPerformanceMetric.computeMetric(ulPerformanceMetric.nuIDs(nu_cnt),currentTime-mySysInfo.settlingTimeInSec,mySysInfo.carrierInfo);
            if(printOutput)
            {
                if(mySysInfo.linkSimulationInfo.isEnabled) ulPerformanceMetric.printBERPerformanceInfo(mySysInfo.resultFiles.ulLinkPerformanceInfoFileName(nu_cnt),ulPerformanceMetric.nuIDs(nu_cnt), mySysInfo.linkSimulationInfo.snrRangeInDB(snrLoop_cnt));
                else ulPerformanceMetric.printSystemPerformanceInfo(&mySys,ulPerformanceMetric.nuIDs(nu_cnt),mySysInfo.resultFiles.ulUEPerformanceInfoFileName(nu_cnt),mySysInfo.resultFiles.ulSystemPerformanceInfoFileName(nu_cnt),mySysInfo.resultFiles.ulCumulativePerformanceFileName(nu_cnt),&associationInfo);
            }
        }
    
    //Sidelink
    for(int nu_cnt=0;nu_cnt<slPerformanceMetric.nuIDs.length();nu_cnt++)
    {
        bool printOutput=slPerformanceMetric.computeMetric(slPerformanceMetric.nuIDs(nu_cnt),currentTime-mySysInfo.settlingTimeInSec,mySysInfo.carrierInfo);
        if(printOutput)
        {
           slPerformanceMetric.printSystemPerformanceInfo(&mySys,slPerformanceMetric.nuIDs(nu_cnt),mySysInfo.resultFiles.slUEPerformanceInfoFileName(nu_cnt),mySysInfo.resultFiles.slSystemPerformanceInfoFileName(nu_cnt),"./Results/"+toString(slPerformanceMetric.nuIDs(nu_cnt))+"/sideLinkInfo.txt",&associationInfo);
        }
    }
    }
  //   if(traceValuePerTraceName["channelEstimator_01_00_traceFlag"]=="true" && mySysInfo.linkSimulationInfo.isEnabled)
  //   {
  //     if(mySysInfo.ulDlMode!=_ULDL_MODE_UPLINK_)
  //       dlPerformanceMetric.printNMSEperformanceInfo("./Results/dlNMSEperformanceInfo.txt",mySysInfo.linkSimulationInfo.snrRangeInDB(snrLoop_cnt));
  //     else
  //       ulPerformanceMetric.printNMSEperformanceInfo("./Results/ulNMSEperformanceInfo.txt",mySysInfo.linkSimulationInfo.snrRangeInDB(snrLoop_cnt));
  //     exit(0);
  //   }
    #ifdef USING_MPI
    MPI_Barrier(MPI_COMM_WORLD);
    #endif
    
    if(currentTaskid==0 && mySysInfo.simulationTimeInSec==currentTime)
    {
        if(!mySysInfo.linkSimulationInfo.isEnabled) 
        {
            if(mySysInfo.ulDlMode!=_ULDL_MODE_UPLINK_)
            { 
                for(int nu_cnt=0;nu_cnt<dlPerformanceMetric.nuIDs.length();nu_cnt++)
                { 
                    mergeFiles(mySysInfo.resultFiles.dlWindowBLERstatsFileName(nu_cnt));
                    if(traceValuePerTraceName["EnableUElogs"]=="true")
                    {
                        mergeFiles(mySysInfo.resultFiles.WideBandCQIlogsFileName(nu_cnt));
                        mergeFiles(mySysInfo.resultFiles.SubbandCQIlogsFileName(nu_cnt));
                    }
                }
            }
            
            if(mySysInfo.ulDlMode!=_ULDL_MODE_DOWNLINK_)
            {
                for(int nu_cnt=0;nu_cnt<ulPerformanceMetric.nuIDs.length();nu_cnt++)
                {
                    mergeFiles(mySysInfo.resultFiles.ulWindowBLERstatsFileName(nu_cnt));
                }
            }
        }
    } 
  
    #ifdef USING_MPI
    MPI_Barrier(MPI_COMM_WORLD);
    #endif
  
}


#ifdef USING_MPI

void filterServerNodesForMPI(ivec &centralUnitsToConsiderForScheduling,AssociationInfo_S &associationInfo,SystemInfo_S &mySysInfo)
{
  ivec centralUnitsForCurrentTask=associationInfo.serverNodes(mySysInfo.mpiInfo.srvrIndxListPerTaskid(currentTaskid));
  if(centralUnitsToConsiderForScheduling.length()==0)
    centralUnitsToConsiderForScheduling=centralUnitsForCurrentTask;
  else
  {
    ivec srvrIndx=find(centralUnitsToConsiderForScheduling,centralUnitsForCurrentTask);
    srvrIndx=remove(srvrIndx,-1);
    centralUnitsToConsiderForScheduling=centralUnitsToConsiderForScheduling(srvrIndx);
  }
}

void exchangeMPIDataNew(Array< NodeDevice >& nodeDevices, ivec nodeDeviceIDs, ivec serverNodes, AssociationInfo_S& associationInfo, SystemInfo_S& mySysInfo, Array< SchedulerOutput_S >& schedulerOutput, int nuID, int beamID){
  
  ivec serverNodeForCurrentTask=mySysInfo.mpiInfo.srvrIndxListPerTaskid(currentTaskid); // Assuming CUs as ServerNodes
  ivec validIndices=find(serverNodeForCurrentTask,serverNodes);
  validIndices=remove(validIndices,-1);
  
  if(validIndices.length())
    serverNodeForCurrentTask=serverNodeForCurrentTask(validIndices);
  else
    serverNodeForCurrentTask.set_size(0);
  
  ivec serverNodesForOtherTasks=remove(serverNodes,serverNodeForCurrentTask);
  
  
  if(serverNodeForCurrentTask.length())
  {
    if(traceValuePerTraceName["EnablePrints"]=="true")
    {
      if(currentTaskid==0)
      {
	cout<<"///////////////////////////////////////////////////"<<endl;
	cout<<"Uploading MPI Data ..."<<endl;
      }
    }
    uploadMPIDataNew(mySysInfo.mpiInfo,serverNodeForCurrentTask,associationInfo,schedulerOutput);
  }
  
  MPI_Gatherv(mySysInfo.mpiInfo.schInfoField+mySysInfo.mpiInfo.indexShiftPerTask(currentTaskid),mySysInfo.mpiInfo.countsPerTask[currentTaskid],MPI_BYTE,mySysInfo.mpiInfo.schInfoField,mySysInfo.mpiInfo.countsPerTask,mySysInfo.mpiInfo.offsetsPerTask,MPI_BYTE,0,MPI_COMM_WORLD);
  MPI_Bcast(mySysInfo.mpiInfo.schInfoField,sum(mySysInfo.mpiInfo.countsPerSrvr)*sizeof(MPI_SchedulingInfo_S),MPI_BYTE,0,MPI_COMM_WORLD);
  
  if(serverNodesForOtherTasks.length())
  {
    if(traceValuePerTraceName["EnablePrints"]=="true") 
    {
      //       if(currentTaskid==0)
      {
	cout<<"Downloading MPI Data ..."<<endl;
	cout<<"///////////////////////////////////////////////////"<<endl;
      }
    }
    downloadMPIDataNew(nodeDevices,nodeDeviceIDs,serverNodesForOtherTasks,schedulerOutput,associationInfo,mySysInfo.mpiInfo,nuID,beamID);
  }
  clearMPIData(mySysInfo.mpiInfo);
}

void downloadMPIDataNew(Array< NodeDevice >& nodeDevices, ivec nodeDeviceIDs, ivec serverNodes, Array< SchedulerOutput_S >& schedulerOutput, AssociationInfo_S& associationInfo, MPIInfo_S& mpiInfo, int nuID,int beamID){
  
  for(int task_cnt=0;task_cnt<nTasks;task_cnt++)
    if(task_cnt!=currentTaskid)
    {
      ivec srvrIndices=mpiInfo.srvrIndxListPerTaskid(task_cnt);
      ivec validIndices=find(srvrIndices,find(associationInfo.serverNodes,serverNodes)); 
      validIndices=remove(validIndices,-1);
      srvrIndices=srvrIndices(validIndices);
      for(int srvr_cnt=0;srvr_cnt<srvrIndices.length();srvr_cnt++)
      {
	int srvrIndx=srvrIndices(srvr_cnt);

	double txSubcarrierPowerInDBm;
	ivec srvcNodeDeviceIndx=find(nodeDeviceIDs,associationInfo.associatedServiceNodesPerServerNode(srvrIndx));
	ivec srvcIndx=find(associationInfo.serviceNodes,associationInfo.associatedServiceNodesPerServerNode(srvrIndx));
	for (int srvcNode_cnt=0;srvcNode_cnt<associationInfo.associatedServiceNodesPerServerNode(srvrIndx).length();srvcNode_cnt++)
	{
	  MPI_SchedulingInfo_S schInfo=mpiInfo.schInfoField[mpiInfo.offsetsPerSrvr(srvrIndx)+srvcNode_cnt];
	  if(schInfo.isScheduled)
	  {
	    DataBlkInfo_S myDatablkInfo;
	    int nuID;
	    int currentServerNode=MPI_decode(schInfo,myDatablkInfo,txSubcarrierPowerInDBm,nuID);
	    bool isDownlink=schInfo.isDownlink;
	    int nPorts;
	    int nuIndx=nodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).getNUIndx(nuID);
	    int cellID=nodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).L1CorePerNU(nuIndx).cellID;
	    
	    myDatablkInfo.dataGenInfo.scheduledPRBs=myDatablkInfo.resourceBlocks.length();
	    if(myDatablkInfo.dataGenInfo.scheduledPRBs)
	      myDatablkInfo.dataGenInfo.scheduledSymbols=myDatablkInfo.symPos.length();
	    myDatablkInfo.dataGenInfo.rnti=associationInfo.associatedServiceNodesPerServerNode(srvrIndx)(srvcNode_cnt);
	    
	    sBeamPairLink beamPair=associationInfo.associatedBeamPairLinkforEachServiceNode(srvcIndx(srvcNode_cnt));
	    
	    int nCodeWords=myDatablkInfo.dataGenInfo.nCodeWords;
	    if(nCodeWords>2)
	    {cout<<"Invalid Codewords for MPI"<<endl;abort();}
	    
	    
	    myDatablkInfo.harqInfo.harqProcessID=schInfo.harqProcessID;
	    
	    myDatablkInfo.harqInfo.codeWordIndicesToConsider.set_size(nCodeWords);
	    myDatablkInfo.harqInfo.newDataIndicator.set_size(nCodeWords);
	    myDatablkInfo.harqInfo.rvIndicesPerCodeword.set_size(nCodeWords);
	    myDatablkInfo.harqInfo.isNewTransmission.set_size(nCodeWords);
        myDatablkInfo.harqInfo.TBSizePerCodewordInBits.set_size(nCodeWords);
	    
	    
	    myDatablkInfo.harqInfo.codeWordIndicesToConsider(0)=schInfo.cwdIndxTB1;
	    myDatablkInfo.harqInfo.newDataIndicator(0)=schInfo.ndiTB1;
	    myDatablkInfo.harqInfo.rvIndicesPerCodeword(0)=schInfo.rvIndxTB1;
	    myDatablkInfo.harqInfo.isNewTransmission(0)=schInfo.isNewTB1;
        myDatablkInfo.harqInfo.TBSizePerCodewordInBits(0)=schInfo.sizeTB1;
	    
	    if(nCodeWords==2)
	    {
	      myDatablkInfo.harqInfo.codeWordIndicesToConsider(1)=schInfo.cwdIndxTB2;
	      myDatablkInfo.harqInfo.newDataIndicator(1)=schInfo.ndiTB2;
	      myDatablkInfo.harqInfo.rvIndicesPerCodeword(1)=schInfo.rvIndxTB2;
	      myDatablkInfo.harqInfo.isNewTransmission(1)=schInfo.isNewTB2;
          myDatablkInfo.harqInfo.TBSizePerCodewordInBits(1)=schInfo.sizeTB2;
	    }
	    
	    if(isDownlink)
	    {
	      //int srvrNodeInfoIndx=schedulerOutput(srvrIndx).getSchedulerToTransmitterInfoIndx(currentServerNode,nuID,0,0); //MPI beams 
	      nPorts=nodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).L1CorePerNU(nuIndx).dlTxConfig.nPorts;
	      bool isToTransmitCQIPorts=nodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).L1CorePerNU(nuIndx).dlTxConfig.dlCQIModeInfo.isToTransmitCQIPorts(nodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).ttiCountPerNU(nuIndx));
	      bool isToTransmitTRSPorts=nodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).L1CorePerNU(nuIndx).dlTxConfig.mTRSConf.isToTransmitTRSPorts(nodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).ttiCountPerNU(nuIndx));
	      
          setRsInfo(myDatablkInfo.dataGenInfo.rbInfo.rsInfo,nodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).L1CorePerNU(nuIndx).dlPortInfo,nodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).L1CorePerNU(nuIndx).dlFrameStructure,cellID,myDatablkInfo.dataGenInfo.transmissionScheme,myDatablkInfo.dataGenInfo.layersInfo,nPorts,0,isDownlink,isToTransmitCQIPorts,myDatablkInfo.symPos(0),isToTransmitTRSPorts,TRSConfig_S(),myDatablkInfo.waveform);
	      
	    }
	    else
	    {
	      //int srvrNodeInfoIndx=schedulerOutput(srvrIndx).getSchedulerToReceiverInfoIndx(currentServerNode,nuID,0,0);
	      nPorts=nodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).L1CorePerNU(nuIndx).ulTxConfig.nPorts;
	      bool isToTransmitCQIPorts=nodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).L1CorePerNU(nuIndx).ulTxConfig.ulCQIModeInfo.isToTransmitCQIPorts(nodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).ttiCountPerNU(nuIndx));
	      setRsInfo(myDatablkInfo.dataGenInfo.rbInfo.rsInfo,nodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).L1CorePerNU(nuIndx).ulPortInfo,nodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).L1CorePerNU(nuIndx).ulFrameStructure,cellID,myDatablkInfo.dataGenInfo.transmissionScheme,myDatablkInfo.dataGenInfo.layersInfo,nPorts,0,isDownlink,isToTransmitCQIPorts,myDatablkInfo.symPos(0));
	    }
	    
	    myDatablkInfo.dataGenInfo.symbolRepetitionFactor=getSymbolRepetitionFactor(myDatablkInfo.dataGenInfo.transmissionScheme,myDatablkInfo.dataGenInfo.nLayers);
	    myDatablkInfo.dataGenInfo.mimoRate=(double)myDatablkInfo.dataGenInfo.nLayers/(double)(myDatablkInfo.dataGenInfo.symbolRepetitionFactor); 
	    myDatablkInfo.dataGenInfo.cellID=cellID;
	    
	    if(isDownlink)
	    {
	      schedulerOutput(srvrIndx).addTxDataBlk(currentServerNode,myDatablkInfo.dataGenInfo.rnti,nuID,schInfo.carrierFreqInMHz,schInfo.ttiCount,myDatablkInfo,txSubcarrierPowerInDBm, _TRANSMISSION_TYPE_DL_,beamPair.srvrBeamID);
	      schedulerOutput(srvrIndx).addRxDataBlk(myDatablkInfo.dataGenInfo.rnti,currentServerNode,nuID,schInfo.carrierFreqInMHz,schInfo.ttiCount,myDatablkInfo,txSubcarrierPowerInDBm,_TRANSMISSION_TYPE_DL_,beamPair.srvcBeamID);
	    }
	    else
	    {
	      schedulerOutput(srvrIndx).addTxDataBlk(myDatablkInfo.dataGenInfo.rnti,currentServerNode,nuID,schInfo.carrierFreqInMHz,schInfo.ttiCount,myDatablkInfo,txSubcarrierPowerInDBm, _TRANSMISSION_TYPE_UL_,beamPair.srvcBeamID);
	      schedulerOutput(srvrIndx).addRxDataBlk(currentServerNode,myDatablkInfo.dataGenInfo.rnti,nuID,schInfo.carrierFreqInMHz,schInfo.ttiCount,myDatablkInfo,txSubcarrierPowerInDBm, _TRANSMISSION_TYPE_UL_,beamPair.srvrBeamID);
	    }
	    
	  }
	}
	
	
      }
    }
}
#endif



void preInitializeSCMLinks(McellSystem &mySys,ivec rxNodes, int carrierCount,Array<StrongAndWeakTransmittersInfo_S> &strongAndWeakTransmittersInfo){
  
  Array<ivec> txNodesPerRxNode(rxNodes.length());
  for(int rx_cnt=0;rx_cnt<rxNodes.length();rx_cnt++)
    txNodesPerRxNode(rx_cnt)=strongAndWeakTransmittersInfo(rx_cnt).strongTransmitters;
  
  ivec indicesToDelete(0);
  for(int rx_cnt=rxNodes.length()-1;rx_cnt>=0;rx_cnt--)
  {
    for(int tx_cnt=txNodesPerRxNode(rx_cnt).length()-1;tx_cnt>=0;tx_cnt--)
      if(mySys.channelCloud.isSCMLinkInitialized(txNodesPerRxNode(rx_cnt)(tx_cnt),rxNodes(rx_cnt)))
	txNodesPerRxNode(rx_cnt).del(tx_cnt);
      
      if(txNodesPerRxNode(rx_cnt).length()==0) 
	append(indicesToDelete,rx_cnt);
      
  }
  if(indicesToDelete.length())  
  {
    deleteInVec(rxNodes,indicesToDelete);
    deleteInArray(txNodesPerRxNode,indicesToDelete);
  } 
  
  // Reciprocity is not taken care for TDD
  if(rxNodes.length()!=0 )
  {
    if(traceValuePerTraceName["EnablePrints"]=="true")
      if(currentTaskid==0)
	cout<<"[detl:]Initializing SCM Links."<<endl;
      
      mySys.channelCloud.preAllocateSCMLink(rxNodes,txNodesPerRxNode);
    for(int rx_cnt=0;rx_cnt<rxNodes.length();rx_cnt++)
      mySys.initSCMLink(to_ivec(rxNodes(rx_cnt)),txNodesPerRxNode(rx_cnt),false,true);
  }
}

void ChannelHandler_S::initChannelHandler(ivec nodeIDS, ivec txAntennaCount, int rxAntennaCount, int rowCount, int columnCount){
  
  if(nodeIDS.length()!=txAntennaCount.length())
  { cout<<"[both:] Error ... nodeIDs and txAntennaCount are of different length in initChannelHandler().. ...";abort();}
  
  txNodeIDs=nodeIDS;
  if(channel.length()!=txNodeIDs.length())
    channel.set_length(txNodeIDs.length());
  
  for(int tx_cnt=0;tx_cnt<txNodeIDs.length();tx_cnt++)
  {
    if(channel(tx_cnt).length()!=txAntennaCount(tx_cnt))
      channel(tx_cnt).set_length(txAntennaCount(tx_cnt));
    for(int txAntenna_cnt=0;txAntenna_cnt<txAntennaCount(tx_cnt);txAntenna_cnt++)
    {
      if(channel(tx_cnt)(txAntenna_cnt).length()!=rxAntennaCount)
	channel(tx_cnt)(txAntenna_cnt).set_length(rxAntennaCount);
      for(int rx_cnt=0;rx_cnt<rxAntennaCount;rx_cnt++)
      {
	if(channel(tx_cnt)(txAntenna_cnt)(rx_cnt).rows()!=rowCount || channel(tx_cnt)(txAntenna_cnt)(rx_cnt).cols()!=columnCount)
	  channel(tx_cnt)(txAntenna_cnt)(rx_cnt).set_size(rowCount,columnCount);
	channel(tx_cnt)(txAntenna_cnt)(rx_cnt).clear();
      }
    }
  }
}

void ChannelHandler_S::clear(){
  
  txNodeIDs.set_size(0);
  channel.set_size(0);
  // for(int tx_cnt=0;tx_cnt<channel.length();tx_cnt++)
  //   for(int txAnt_cnt=0;txAnt_cnt<channel(tx_cnt).length();txAnt_cnt++)
  //     for(int rxAnt_cnt=0;rxAnt_cnt<channel(tx_cnt)(txAnt_cnt).length();rxAnt_cnt++)
	// channel(tx_cnt)(txAnt_cnt)(rxAnt_cnt).clear();
      
}

void ChannelHandler_S::loadChannel(Array< Array<cvec> > channelAlongColumn,int txNodeID , int columnNumber){
  
  int txIndx=find(txNodeIDs,txNodeID);
  if(txIndx==-1)
  {
    cout<<"[both:] Error.. undefined txNodeID to loadMyChannel()..."<<endl;
    abort();
  }
  if(channelAlongColumn(0)(0).length()!=channel(txIndx)(0)(0).rows())
  {
    cout<<"[both:] Error length mismatch in loadMyChannel()..."<<channelAlongColumn(0)(0).length()<<","<<channel(txIndx)(0)(0).rows()<<endl;
    abort();
  }
  
  for(int txAnt_cnt=0;txAnt_cnt<channelAlongColumn.length();txAnt_cnt++)
    for(int rxAnt_cnt=0;rxAnt_cnt<channelAlongColumn(txAnt_cnt).length();rxAnt_cnt++)
      channel(txIndx)(txAnt_cnt)(rxAnt_cnt).set_col(columnNumber,channelAlongColumn(txAnt_cnt)(rxAnt_cnt));
}
//#endif

void assignRandomAntennas(McellSystem &mySys,AssociationInfo_S &associationInfo,SystemInfo_S &mySysInfo){
  
  ConfigFileNames_S configFileNames=mySysInfo.myConfigFiles;
  ivec antennasToBeConfigured="2 4 8";
  ivec percentageToBeGiven;
  //   TransmissionMode_E transmissionMode;
  //   
  //   if(mySysInfo.ulDlMode == _ULDL_MODE_DOWNLINK_)
  //     transmissionMode=parseTransmissionMode(configFileNames.dlTxRxConfigFileName,1);
  //   else
  //     transmissionMode=parseTransmissionMode(configFileNames.ulTxRxConfigFileName,0);  
  //   
  //   if(transmissionMode == _TRANSMISSION_MODE_1_)
  //   {cout<<"[both:] Error... \"Different Antennas To Different UEs Feature\" shall not be enabled for TM1..."<<endl;abort();}
  
  if(parse(configFileNames.simulationConfigFileName,"percentageToBeGiven",percentageToBeGiven)==false)
  {cout<<"[both:] Error... Unable to load percentageToBeGiven from the file in mySimulationConfig.txt..."<<endl;abort();}  
  
  cout<<"[both:] \"Different Antennas To Different UEs Feature\" enabled... So, overwriting existing antennas"<<endl;
  
  if(sum(percentageToBeGiven) > 100)
  {cout<<"[both:] Error... Total percentage to be given to antennas can not be greater than 100% ..."<<endl;abort();}
  
  ivec firstAntUsers(0);
  ivec secondAntUsers(0);
  ivec thirdAntUsers(0);
  int temp_falg=sum(percentageToBeGiven)%100;
  for(int srvrIndx=0;srvrIndx<associationInfo.associatedServiceNodesPerServerNode.length();srvrIndx++)
  {
    if(temp_falg || sum(percentageToBeGiven) == 0)
    {
      firstAntUsers=randPick(associationInfo.associatedServiceNodesPerServerNode(srvrIndx),round_i((percentageToBeGiven(0)/100.0)*associationInfo.associatedServiceNodesPerServerNode(srvrIndx).length()));
      ivec secondAntUsers_Temp=associationInfo.associatedServiceNodesPerServerNode(srvrIndx).get(findNot(associationInfo.associatedServiceNodesPerServerNode(srvrIndx),firstAntUsers));
      secondAntUsers=randPick(secondAntUsers_Temp,round_i((percentageToBeGiven(1)/100.0)*associationInfo.associatedServiceNodesPerServerNode(srvrIndx).length()));
      ivec thirdAntUsers_Temp1=associationInfo.associatedServiceNodesPerServerNode(srvrIndx).get(findNot(associationInfo.associatedServiceNodesPerServerNode(srvrIndx),firstAntUsers));
      thirdAntUsers=thirdAntUsers_Temp1.get(findNot(thirdAntUsers_Temp1,secondAntUsers));
    }
    else
    {
      
      firstAntUsers=randPick(associationInfo.associatedServiceNodesPerServerNode(srvrIndx),round_i((percentageToBeGiven(0)/100.0)*associationInfo.associatedServiceNodesPerServerNode(srvrIndx).length()));
      secondAntUsers=associationInfo.associatedServiceNodesPerServerNode(srvrIndx).get(findNot(associationInfo.associatedServiceNodesPerServerNode(srvrIndx),firstAntUsers));
    }
    if(mySysInfo.ulDlMode == _ULDL_MODE_DOWNLINK_)
    {
      for(int userIndx=0;userIndx<firstAntUsers.length();userIndx++)  
	mySys.aNodes(firstAntUsers(userIndx)).rxAntenna.setNumberOfPhysicalAntenna(antennasToBeConfigured(0));    
      for(int userIndx=0;userIndx<secondAntUsers.length();userIndx++)  
	mySys.aNodes(secondAntUsers(userIndx)).rxAntenna.setNumberOfPhysicalAntenna(antennasToBeConfigured(1));    
      for(int userIndx=0;userIndx<thirdAntUsers.length();userIndx++)    
	mySys.aNodes(thirdAntUsers(userIndx)).rxAntenna.setNumberOfPhysicalAntenna(antennasToBeConfigured(2));
    }
    else // uplink
    {
      for(int userIndx=0;userIndx<firstAntUsers.length();userIndx++)  
	mySys.aNodes(firstAntUsers(userIndx)).txAntenna.setNumberOfPhysicalAntenna(antennasToBeConfigured(0));    
      for(int userIndx=0;userIndx<secondAntUsers.length();userIndx++)  
	mySys.aNodes(secondAntUsers(userIndx)).txAntenna.setNumberOfPhysicalAntenna(antennasToBeConfigured(1));    
      for(int userIndx=0;userIndx<thirdAntUsers.length();userIndx++)    
	mySys.aNodes(thirdAntUsers(userIndx)).txAntenna.setNumberOfPhysicalAntenna(antennasToBeConfigured(2));
    }
  }
  
  //For Testing
  int twoAnt=0,fourAnt=0,eightAnt=0;
  if(mySysInfo.ulDlMode == _ULDL_MODE_DOWNLINK_)
  {
    for(int indx=associationInfo.serverNodes.length();indx<mySys.aNodes.length();indx++)
      if(mySys.aNodes(indx).rxAntenna.getNumberOfPhysicalAntenna()==2) twoAnt++;
      else if(mySys.aNodes(indx).rxAntenna.getNumberOfPhysicalAntenna()==4) fourAnt++;
      else eightAnt++;	
  }
  else
  {
    for(int indx=associationInfo.serverNodes.length();indx<mySys.aNodes.length();indx++)
      if(mySys.aNodes(indx).txAntenna.getNumberOfPhysicalAntenna()==2) twoAnt++;
      else if(mySys.aNodes(indx).txAntenna.getNumberOfPhysicalAntenna()==4) fourAnt++;
      else eightAnt++;	
  }
  //print the percentage of Antennas Configured
  cout<<"Percentage allocated to [2 4 8] antennas is "; 
  
  cout<<(twoAnt/double( associationInfo.serviceNodes.length()))*100;    
  cout<<","<<(fourAnt/double( associationInfo.serviceNodes.length()))*100;    
  cout<<","<<(eightAnt/double( associationInfo.serviceNodes.length()))*100<<endl; 
  
}

void updateFrameReservationInfo(Array< NodeDevice >& nodeDevices, ivec nodeDeviceIDs, ivec nodeIDs, int nuID, bool isDownlink)
{
  ivec nodeDeviceIndx=find(nodeDeviceIDs,nodeIDs);
  for(int node_cnt=0;node_cnt<nodeIDs.length();node_cnt++)
  {
    int nuIndx=nodeDevices(nodeDeviceIndx(node_cnt)).getNUIndx(nuID);
    int deviceIndx=nodeDeviceIndx(node_cnt);
    int ttiCount=nodeDevices(nodeDeviceIndx(node_cnt)).ttiCountPerNU(nuIndx);
    
    CCHInfo_S cchInfo;
    if(isDownlink)
    {
      cchInfo=nodeDevices(nodeDeviceIndx(node_cnt)).L1CorePerNU(nuIndx).dlTxConfig.cchInfo;
      updateCCHReservationInfo(cchInfo,&nodeDevices(nodeDeviceIndx(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo,nodeDevices(nodeDeviceIndx(node_cnt)).L1CorePerNU(nuIndx).nuParameters.subcarrierSpacingInHz/1e3,(double)ttiCount,nodeDevices(nodeDeviceIndx(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getRuCountPerLevel(),ttiCount,true,nodeDevices(nodeDeviceIndx(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getPRBMapping());
    }
    else
    {
      cchInfo=nodeDevices(nodeDeviceIndx(node_cnt)).L1CorePerNU(nuIndx).ulTxConfig.cchInfo;
      
      // Below code commented for now. May need for PUCCH reservation - Dhiv
//       if(nodeDevices(nodeDeviceIndx(node_cnt)).L1CorePerNU(nuIndx).ulRxConfig.ulCQIModeInfo.isToReportCQI(ttiCount))
//         cchInfo.enablelongPUCCH=true;
//       cchInfo.enableShortPUCCH=true; //Assuming full buffer
//       
      updateCCHReservationInfo(cchInfo,&nodeDevices(nodeDeviceIndx(node_cnt)).L1CorePerNU(nuIndx).ulFrameReservationInfo,nodeDevices(nodeDeviceIndx(node_cnt)).L1CorePerNU(nuIndx).nuParameters.subcarrierSpacingInHz/1e3,(double)ttiCount,nodeDevices(nodeDeviceIndx(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getRuCountPerLevel(),ttiCount);
    }
    
    // For TDD simulations
    ivec CellConfig=nodeDevices(nodeDeviceIndx(node_cnt)).L1CorePerNU(nuIndx).CellConfig;
    if(CellConfig.length()==0)
    {
      updateSpecialSubFrameInfo_DwPts(nodeDevices,nodeDeviceIDs,nuID,nodeIDs(node_cnt),nodeDevices(deviceIndx).actualLoopCountPerNU(nuIndx),nodeDevices(deviceIndx).L1CorePerNU(nuIndx).ssfConfig.nDwPts);
      updateSpecialSubFrameInfo_UpPts(nodeDevices,nodeDeviceIDs,nuID,nodeIDs(node_cnt),nodeDevices(deviceIndx).actualLoopCountPerNU(nuIndx),nodeDevices(deviceIndx).L1CorePerNU(nuIndx).ssfConfig.nDwPts);
    }
    else
    {
      ivec SFI=nodeDevices(nodeDeviceIndx(node_cnt)).L1CorePerNU(nuIndx).SFI;
      int CellConfigPeriodicityInSlots=nodeDevices(nodeDeviceIndx(node_cnt)).L1CorePerNU(nuIndx).CellConfigPeriodicityInSlots;
      ivec DLULSymbs=getDLULSymbsForCellSpecificConfig(ttiCount,CellConfig,CellConfigPeriodicityInSlots);
      int DLSymbs,ULSymbs;
      DLSymbs=DLULSymbs(0);
      ULSymbs=DLULSymbs(1);
      updateFlexibleSubFrameInfo_DL(nodeDevices,nodeDeviceIDs,nuID,nodeIDs(node_cnt),nodeDevices(deviceIndx).actualLoopCountPerNU(nuIndx),DLSymbs,SFI);
      updateFlexibleSubFrameInfo_UL(nodeDevices,nodeDeviceIDs,nuID,nodeIDs(node_cnt),nodeDevices(deviceIndx).actualLoopCountPerNU(nuIndx),ULSymbs,SFI);
    }
  }
}




ivec getAntennaCountForBeam(McellSystem &mySys, ivec nodeIDs, int nuID, ivec beamIDs)
{
  ivec antennaCount=zeros_i(nodeIDs.length());
  ivec nodeIndx=find(mySys.channelCloud.nodesInfo.nodeList,nodeIDs);
  for(int cnt=0;cnt<nodeIDs.length();cnt++)
  {
//     if(mySys.channelCloud.nodesInfo.beamParametersPerNode(nodeIndx(cnt)).isInitialized)
//     {
//       ivec txRUsForCurrentBeam=find(mySys.channelCloud.nodesInfo.beamParametersPerNode(nodeIndx(cnt)).beamIDsPerTXRU,beamIDs(cnt),true);
//       antennaCount(cnt)=txRUsForCurrentBeam.length();
//     }
//     else
//       antennaCount(cnt) = mySys.channelCloud.nodesInfo.antennaCount(nodeIndx(cnt));
    antennaCount(cnt) =  mySys.channelCloud.nodesInfo.getTXRUCount(nodeIDs(cnt),beamIDs(cnt)); 
    if(antennaCount(cnt)==0)
    {
      cout<<"No TXRU configured with the given beam in getAntennaCountForBeam()..."<<endl;
      abort();
    }
  }
  return(antennaCount);
}

int getAntennaCountForBeam(McellSystem &mySys, int nodeID, int nuID, int beamID)
{
  int antennaCount=mySys.channelCloud.nodesInfo.getTXRUCount(nodeID,beamID);
  if(antennaCount==0)
  {
    cout<<"No TXRU configured with the given beam in getAntennaCountForBeam()..."<<endl;
    abort();
  }
  
  return(antennaCount);
}

ivec getTXRUsForBeam(McellSystem &mySys, int nodeID, int nuID, int beamID)
{
  ivec txRUsForCurrentBeam=mySys.channelCloud.nodesInfo.getTXRUs(nodeID,beamID);
  int antennaCount=txRUsForCurrentBeam.length();
  if(antennaCount==0)
  {
    cout<<"No TXRU configured with the given beam in getAntennaCountForBeam()..."<<endl;
    abort();
  }
  return(txRUsForCurrentBeam);
}

// void nodePrepare(Array< SchedulerOutput_S >& schedulerOutput, AssociationInfo_S associationInfo, Array<NodeDevice> &myNodeDevices, ivec nodeDeviceIDs, ivec nodeIDs, bool isWithAbstraction, ABSTRACTION_MODEL AbstractionModel, int nuID,double currentTime,ivec BeamIDs, bool isForTransmission){
//   
//   ivec deviceIndices=find(nodeDeviceIDs,nodeIDs);
//   ivec ttiCount=getTTICount(nodeDeviceIDs,myNodeDevices,nodeIDs,nuID);
//   ivec loopCountPerServerNode=getLoopCount(nodeDeviceIDs,myNodeDevices,nodeIDs,nuID);
//   for(int node_cnt=0;node_cnt<nodeIDs.length();node_cnt++)
//   {
//     int nuIndx=myNodeDevices(deviceIndices(node_cnt)).getNUIndx(nuID);
//     ivec CellConfig=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).CellConfig;
//     ivec SFI=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).SFI;
//     int CellConfigPeriodicityInSlots=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).CellConfigPeriodicityInSlots;
//     int DLSymbs,ULSymbs;
//     if(CellConfig.length()!=0)
//     {
//       ivec DLULSymbs=getDLULSymbsForCellSpecificConfig(ttiCount(node_cnt),CellConfig,CellConfigPeriodicityInSlots);
//       DLSymbs=DLULSymbs(0);
//       ULSymbs=DLULSymbs(1);
//     }
//     if(nuIndx!=-1)
//     { 
//       int indx=find(associationInfo.serverNodes,nodeIDs(node_cnt));
//       
//       if(indx!=-1)
//       {
// 	int srvrIndx=associationInfo.centralUnitIDsPerServerNode(indx);
// 	int schToTxInfoindx=schedulerOutput(srvrIndx).getSchedulerToTransmitterInfoIndx(nodeIDs(node_cnt),nuID,BeamIDs(node_cnt)); //Do I need check for tx beam count 
// 	if(schToTxInfoindx!=-1) // DL Scheduled
// 	{
// 	  if(CellConfig.length()==0)
// 	  {
// 	    SpecialSubFrameConfig_S SSFConfig;
// 	    getSpecialSubFrameConfig(&SSFConfig);
// 	    updateSpecialSubFrameInfo_DwPts(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs,loopCountPerServerNode,SSFConfig.nDwPts);
// 	    updateSpecialSubFrameInfo_UpPts(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs,loopCountPerServerNode,SSFConfig.nUpPts);
// 	  }
// 	  else
// 	  {
// 	    int nodeIndices=find(nodeDeviceIDs,nodeIDs(node_cnt));
// 	    updateFlexibleSubFrameInfo_DL(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs(node_cnt),myNodeDevices(nodeIndices).actualLoopCountPerNU(nuIndx),DLSymbs,SFI);
// 	    updateFlexibleSubFrameInfo_UL(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs(node_cnt),myNodeDevices(nodeIndices).actualLoopCountPerNU(nuIndx),ULSymbs,SFI);
// 	    
// 	  }
// 	  bool isDownlink=true;
// 	  int ttiCount = schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).ttiCount;
// 	  myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.setCurrentFrameType("dlFrame");
// 	  myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.loadDLResourceMapperForBandwidth(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getRuCountPerLevel());
// 	  bvec isTheRBScheduled=zeros_b(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getAllRuCount());
// 	  for(int db_cnt=0;db_cnt<schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo.length();db_cnt++)
// 	  {
// 	    bvec isAlreadyScheduled=isTheRBScheduled(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks);
// 	    setInVec(isTheRBScheduled,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks,ones_b(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length()));
// 	    RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks,isAlreadyScheduled,true,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).isLocalised,&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo);
// 	    schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB.set_length(0);
// 	    for(int rb_cnt=0;rb_cnt<schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length();rb_cnt++)
// 	    {
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.updateRUType(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 	      append(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getToneCountPerRU(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),"data",schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).symPos));
// 	    }
// 	  }
// 	  
// 	  ivec unScheduledRBs=find(to_ivec(isTheRBScheduled),0,true);
// 	  
// 	  {
// 	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRSInfoForUnScheduled;
// 	    
// 	    if(!myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRxConfig.dlCQIModeInfo.isToTransmitCQIPorts(ttiCount) && myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIrsType==_RSTYPE_CSIRS_)
// 	    {
// 	      ivec cqiPortIndices=find(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIports);
// 	      deleteInArray(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsSymPosition,cqiPortIndices);
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts=remove(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIports);
// 	    }
// 	    // Need to check DMRS for ePDCCH- Dhiv
// 	    
// 	    // 	    if(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo.cchInfo.enableEPDCCH)
// 	    // 	    {
// 	    // 	      ivec epdcchRBs=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo.reservedVRBsForEpdcch;
// 	    // 	      if(epdcchRBs.length())
// 	    // 	      {
// 	    // 		unScheduledRBs=remove(unScheduledRBs,epdcchRBs);
// 	    // 		ivec DMRSPorts;
// 	    // 		if(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIports.length()>2)
// 	    // 		  DMRSPorts=ivec("7 8 9 10");
// 	    // 		else
// 	    // 		  DMRSPorts=ivec("7 8");
// 	    // 		
// 	    // 		RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure,concat(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,DMRSPorts),myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo.reservedVRBsForEpdcch,true,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.isLocalised(epdcchRBs(0)),&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo);
// 	    // 		for(int rb_cnt=0;rb_cnt<epdcchRBs.length();rb_cnt++)
// 	    // 		  myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.updateRUType(epdcchRBs(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 	    // 		
// 	    // 	      }
// 	    // 	    }
// 	    
// 	    
// 	    if(unScheduledRBs.length())
// 	    {
// 	      //Array<ivec> dummy(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts.length());
// 	      RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo,unScheduledRBs,zeros_b(unScheduledRBs.length()),true,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.isLocalised(unScheduledRBs(0)),&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo);
// 	      for(int rb_cnt=0;rb_cnt<unScheduledRBs.length();rb_cnt++)
// 		myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.updateRUType(unScheduledRBs(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 	    }
// 	  }
// 	  myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.updateCurrentFrameStructure();
// 	  for(int db_cnt=0;db_cnt<schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo.length();db_cnt++)
// 	  {
// 	    schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(0));
// 	    schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure=*myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getCurrentFrameStructurePointer();
// 	    schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).reIndxForPrecoding.set_length(0);
// 	    
// 	    //!To get the RE indx while precoding for TM3 (May not be useful for other modes)
// 	    if(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_LARGE_DELAY_CDD_)
// 	    {
// 	      ivec myRBs=schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks;
// 	      Size_S rbSize=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitSize("nullRB");
// 	      imat allocationStructure(0,0);
// 	      allocationStructure=schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure.get_rows(myRBs(0)*rbSize.numberOfSubcarriers,myRBs(0)*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1);
// 	      for(int rb_cnt=1;rb_cnt<myRBs.length();rb_cnt++)
// 		appendVertical(allocationStructure,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure.get_rows(myRBs(rb_cnt)*rbSize.numberOfSubcarriers,myRBs(rb_cnt)*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1));
// 	      
// 	      imat dataIndxingStructure=allocationStructure;
// 	      
// 	      ivec dataREs=find(allocationStructure,0,true);
// 	      ivec reIndx=getIntegers(0,dataREs.length()-1);
// 	      setInMatrix(dataIndxingStructure,dataREs,reIndx);
// 	      
// 	      for(int rb_cnt=0;rb_cnt<myRBs.length();rb_cnt++)
// 	      {
// 		imat rbStructure=allocationStructure.get_rows(rb_cnt*rbSize.numberOfSubcarriers,rb_cnt*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1);
// 		ivec localDataREs=find(rbStructure,0,true);
// 		rbStructure=dataIndxingStructure.get_rows(rb_cnt*rbSize.numberOfSubcarriers,rb_cnt*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1);
// 		ivec dataREIndx=getFromMatrix(rbStructure,localDataREs);
// 		append(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).reIndxForPrecoding,dataREIndx);
// 	      }
// 	    }
// 	    if(!isWithAbstraction || (isWithAbstraction && (AbstractionModel == _ABSTRACTION_DECODER_ || AbstractionModel == _ABSTRACTION_IDEAL_)))
// 	      schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).modulatedSymbols=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).downlinkSourceAndSink.getModulatedSymbols(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).rxNodeIDs(db_cnt),&schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).harqInfo,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).ttiCount,isDownlink);
// 	    else
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).downlinkSourceAndSink.initializeHarqProcess(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).rxNodeIDs(db_cnt),&schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).harqInfo,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).ttiCount,isDownlink);
// 	    
// 	  }
// 	  
// 	  
// 	  //! DL power-allocation 
// 	  
// 	  //====================================================================================================
// 	  // Code to apply Downlink Power allocation (3gpp 36.213 - Section 5.2) difference between
// 	  // OFDM symbols with and without CRS
// 	  // Following code section creates Power mask to apply on PDSCH RE
// 	  //====================================================================================================
// 	  if(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).downlinkSourceAndSink.tbTables->dlVarPowMask.isInitialized)
// 	  {
// 	    
// 	    if(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo.length())
// 	    {
// 	      if(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(0).dataGenInfo.rbInfo.rsInfo.DeModrsType != _RSTYPE_CRS_)
// 	      {cout<<"[both: ] Error Power Mask support provided only for CRS reference signals. Support not provided for CSI-RS and DMRS reference signals";exit(1);}
// 	      int totalScheduledRBPerSrvrNode = 0;
// 	      for(int serviceNode_cnt =0; serviceNode_cnt<schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo.length();serviceNode_cnt++)
// 		for(int rbcnt = 0; rbcnt < schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(serviceNode_cnt).resourceBlocks.length();rbcnt++)
// 		  totalScheduledRBPerSrvrNode++;
// 		imat logicalSubFrameStructure = schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(0).dataGenInfo.rbInfo.logicalSubFrameStructure;
// 	      if(totalScheduledRBPerSrvrNode != (logicalSubFrameStructure.rows() / myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitInfo("nullRB").resourceUnitSize.numberOfSubcarriers))
// 	      {cout<<"[both: ] Error No unscheduled RB in a Enode-B supported while applying power mask";exit(1);}
// 	      
// 	      generateDLPowerMask(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).downlinkSourceAndSink.tbTables, schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).txSubcarrierPowerInDBmPerAntenna,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).Pb);
// 	    }
// 	  }
// 	  //===================================================================================================  
// 	  if(unScheduledRBs.length())
// 	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(unScheduledRBs(0)); 
// 	  else
// 	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(0); // All RBs contains CRS.
// 	    
// 	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.crsPowBoostInDB = schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(0).dataGenInfo.rbInfo.crsPowBoostInDB;
// 	  myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.logicalSubFrameStructure=*myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getCurrentFrameStructurePointer();
// 	}
// 	else 
// 	{
// 	  int schToRxInfoindx=schedulerOutput(srvrIndx).getSchedulerToReceiverInfoIndx(nodeIDs(node_cnt),nuID,BeamIDs(node_cnt));
// 	  if(schToRxInfoindx!=-1) // UL Scheduled
// 	  {
// 	    if(isForTransmission) {cout<<"Invalid condition of isForTransmission when schToRxInfoindx!=-1"<<endl;abort();}
// 	    
// 	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.setCurrentFrameType("ulFrame");
// 	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.loadULResourceMapperForBandwidth(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getRuCountPerLevel());
// 	    
// 	    bvec isTheRBScheduled=zeros_b(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe());
// 	    int ttiCount=schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).ttiCount;
// 	    int nRBs = myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe();
// 	    int nSb=nRBs/myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getResourceBlockGroupingSize();
// 	    ivec loopCountPerServerNode=getLoopCount(nodeDeviceIDs,myNodeDevices,nodeIDs,nuID);
// 	    if(CellConfig.length()==0)
// 	    {
// 	      SpecialSubFrameConfig_S SSFConfig;
// 	      getSpecialSubFrameConfig(&SSFConfig);
// 	      updateSpecialSubFrameInfo_DwPts(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs,loopCountPerServerNode,SSFConfig.nDwPts);
// 	      updateSpecialSubFrameInfo_UpPts(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs,loopCountPerServerNode,SSFConfig.nUpPts);
// 	    }
// 	    else
// 	    {
// 	      int nodeIndices=find(nodeDeviceIDs,nodeIDs(node_cnt));
// 	      updateFlexibleSubFrameInfo_DL(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs(node_cnt),myNodeDevices(nodeIndices).actualLoopCountPerNU(nuIndx),DLSymbs,SFI);
// 	      updateFlexibleSubFrameInfo_UL(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs(node_cnt),myNodeDevices(nodeIndices).actualLoopCountPerNU(nuIndx),ULSymbs,SFI);
// 	    }
// 	    for(int db_cnt=0;db_cnt<schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo.length();db_cnt++)
// 	    {
// 	      
// 	      bvec isAlreadyScheduled=isTheRBScheduled(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks);
// 	      setInVec(isTheRBScheduled,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks,ones_b(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length()));
// // 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.perform_UL_Freq_hopping(ttiCount,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(0),schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length(),nSb,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.cellID,!schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).isLocalised);
// 	      RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks,isAlreadyScheduled,false,true,&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameReservationInfo);
// 	      schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB.set_length(0);
// 	      for(int rb_cnt=0;rb_cnt<schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length();rb_cnt++)
// 	      {
// 		myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.updateRUType(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 		append(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getToneCountPerRU(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),"data",schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).symPos));
// 	      }
// 	      schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getResourceUnitMatrix(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(0));
// 	      schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure=*myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getCurrentFrameStructurePointer();
// 	    }
// 	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.updateCurrentFrameStructure();
// 	    
// 	  }
// 	  else
// 	  {
// 	    if(isForTransmission) // DL UnScheduled
// 	    {
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.setCurrentFrameType("dlFrame");
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.loadDLResourceMapperForBandwidth(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getRuCountPerLevel());
// 	      ivec loopCountPerServerNode=getLoopCount(nodeDeviceIDs,myNodeDevices,nodeIDs,nuID); 
// 	      if(CellConfig.length()==0)
// 	      {
// 		SpecialSubFrameConfig_S SSFConfig;
// 		getSpecialSubFrameConfig(&SSFConfig);
// 		updateSpecialSubFrameInfo_DwPts(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs,loopCountPerServerNode,SSFConfig.nDwPts);
// 		updateSpecialSubFrameInfo_UpPts(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs,loopCountPerServerNode,SSFConfig.nUpPts);
// 	      }
// 	      else
// 	      {
// 		int nodeIndices=find(nodeDeviceIDs,nodeIDs(node_cnt));
// 		updateFlexibleSubFrameInfo_DL(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs(node_cnt),myNodeDevices(nodeIndices).actualLoopCountPerNU(nuIndx),DLSymbs,SFI);
// 		updateFlexibleSubFrameInfo_UL(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs(node_cnt),myNodeDevices(nodeIndices).actualLoopCountPerNU(nuIndx),ULSymbs,SFI);
// 	      }
// 	      ivec unScheduledRBs=getIntegers(0,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getResourceBlocksPerSubframe()-1);
// 	      {
// 		myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRSInfoForUnScheduled;
// 		if(!myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRxConfig.dlCQIModeInfo.isToTransmitCQIPorts(ttiCount(node_cnt)) && myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIrsType==_RSTYPE_CSIRS_)
// 		{
// 		  ivec cqiPortIndices=find(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIports);
// 		  deleteInArray(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsSymPosition,cqiPortIndices);
// 		  myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts=remove(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIports);
// 		}
// 		
// 		/*    if(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.fillPRS)
// 		 *	      {
// 		 *		if((ttiCount-myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.PRSoffset)%myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.PRSperiodicity==0)
// 		 *		{
// 		 *		  if(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.nCQIports<=2)
// 		 *		  {
// 		 *		    append(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,61);
// 		 *		    append(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.portReferenceNumbers,71);
// 	      }
// 	      else
// 	      {
// 	      append(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,62);
// 	      append(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.portReferenceNumbers,72);
// 	      }
// 	      }
// 	      }
// 	      */     
// 		Array<ivec> dummy(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts.length());
// 		RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo,unScheduledRBs,zeros_b(unScheduledRBs.length()),true, myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.isLocalised(0), &myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo);
// 		
// 		for(int rb_cnt=0;rb_cnt<unScheduledRBs.length();rb_cnt++)
// 		  myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.updateRUType(unScheduledRBs(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 	      }
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.updateCurrentFrameStructure();
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(unScheduledRBs(0));
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.logicalSubFrameStructure=*myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getCurrentFrameStructurePointer();  
// 	    }
// 	    else // UL UnScheduled 
// 	    {
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.setCurrentFrameType("ulFrame");
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.loadULResourceMapperForBandwidth(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getRuCountPerLevel());
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.updateCurrentFrameStructure();
// 	    }
// 	  }
// 	  
// 	}
//       }
//       else
//       {
// 	indx=find(associationInfo.serviceNodes,nodeIDs(node_cnt));
// 	int associatedServerNode=associationInfo.associatedServerNodesForEachServiceNode(indx);
// 	int srvrNodeIndx=find(associationInfo.serverNodes,associatedServerNode);
// 	int srvrIndx=associationInfo.centralUnitIDsPerServerNode(srvrNodeIndx);
// 	
// 	ivec nodeIDuser;
// 	nodeIDuser.set_size(0);
// 	for(int k =0;k<nodeIDs.size();k++)
// 	{
// 	  int ind=find(associationInfo.serviceNodes,nodeIDs(k));
// 	  int associatedServerNod=associationInfo.associatedServerNodesForEachServiceNode(ind);
// 	  append(nodeIDuser,associatedServerNod);
// 	}
// 	
// 	int schToRxInfoindx=schedulerOutput(srvrIndx).getSchedulerToReceiverInfoIndx(nodeIDs(node_cnt),nuID,BeamIDs(node_cnt));
// 	if(schToRxInfoindx!=-1) // DL Scheduled
// 	{
// 
// 	  bool isDownlink=true;
// 	  int ttiCount = schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).ttiCount;
// 	  myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.setCurrentFrameType("dlFrame");
// 	  myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.loadDLResourceMapperForBandwidth(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getRuCountPerLevel());
// 	  
// 	  CCHInfo_S cchInfo=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlTxConfig.cchInfo;
// 	  updateCCHReservationInfo(cchInfo,&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).nuParameters.subcarrierSpacingInHz/1e3,(double)ttiCount,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getRuCountPerLevel(),ttiCount,myNodeDevices(deviceIndices(node_cnt)).SSBurstSetConfig,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getPRBMapping());
// 
// 	  ivec loopCountPerServerNode=getLoopCount(nodeDeviceIDs,myNodeDevices,nodeIDuser,nuID);
// 	  if(CellConfig.length()==0)
// 	  {
// 	    SpecialSubFrameConfig_S SSFConfig;
// 	    getSpecialSubFrameConfig(&SSFConfig);
// 	    updateSpecialSubFrameInfo_DwPts(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs,loopCountPerServerNode,SSFConfig.nDwPts);
// 	    updateSpecialSubFrameInfo_UpPts(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs,loopCountPerServerNode,SSFConfig.nUpPts);
// 	  }
// 	  else
// 	  {
// 	    int nodeIndices=find(nodeDeviceIDs,nodeIDs(node_cnt));
// 	    updateFlexibleSubFrameInfo_DL(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs(node_cnt),myNodeDevices(nodeIndices).actualLoopCountPerNU(nuIndx),DLSymbs,SFI);
// 	    updateFlexibleSubFrameInfo_UL(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs(node_cnt),myNodeDevices(nodeIndices).actualLoopCountPerNU(nuIndx),ULSymbs,SFI);
// 	  }
// 	  bvec isTheRBScheduled=zeros_b(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getAllRuCount());
// 	  for(int db_cnt=0;db_cnt<schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo.length();db_cnt++)
// 	  {
// 	    bvec isAlreadyScheduled=isTheRBScheduled(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks);
// 	    setInVec(isTheRBScheduled,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks,ones_b(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length()));
// 	    RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks,isAlreadyScheduled,true,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).isLocalised,&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo);
// 	    schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB.set_length(0);
// 	    for(int rb_cnt=0;rb_cnt<schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length();rb_cnt++)
// 	    {
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.updateRUType(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 	      append(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getToneCountPerRU(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),"data",schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).symPos));
// 	    }
// 	  }
// 	  
// 	  ivec unScheduledRBs=find(to_ivec(isTheRBScheduled),0,true);
// 	  
// 	  {
// 	    
// 	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRSInfoForUnScheduled;
// 	    
// 	    if(!myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRxConfig.dlCQIModeInfo.isToTransmitCQIPorts(ttiCount) && myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIrsType==_RSTYPE_CSIRS_)
// 	    {
// 	      ivec cqiPortIndices=find(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIports);
// 	      deleteInArray(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsSymPosition,cqiPortIndices);
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts=remove(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIports);
// 	    }
// 	    // 	    if(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo.cchInfo.enableEPDCCH)
// 	    // 	    {
// 	    // 	      ivec epdcchRBs=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo.reservedVRBsForEpdcch;
// 	    // 	      if(epdcchRBs.length())
// 	    // 	      {
// 	    // 		unScheduledRBs=remove(unScheduledRBs,epdcchRBs);
// 	    // 		ivec DMRSPorts;
// 	    // 		if(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIports.length()>2)
// 	    // 		  DMRSPorts=ivec("7 8 9 10");
// 	    // 		else
// 	    // 		  DMRSPorts=ivec("7 8");
// 	    // 		
// 	    // 		RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure,concat(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,DMRSPorts),myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo.reservedVRBsForEpdcch,true,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.isLocalised(epdcchRBs(0)),&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo);
// 	    // 		for(int rb_cnt=0;rb_cnt<epdcchRBs.length();rb_cnt++)
// 	    // 		  myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.updateRUType(epdcchRBs(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 	    // 		
// 	    // 	      }
// 	    // 	    }
// 	    // 	    
// 	    if(unScheduledRBs.length())
// 	    { 
// 	      ivec loopCountPerServerNode=getLoopCount(nodeDeviceIDs,myNodeDevices,nodeIDuser,nuID);
// 	      if(CellConfig.length()==0)
// 	      {
// 		SpecialSubFrameConfig_S SSFConfig;
// 		getSpecialSubFrameConfig(&SSFConfig);
// 		updateSpecialSubFrameInfo_DwPts(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs,loopCountPerServerNode,SSFConfig.nDwPts);
// 		updateSpecialSubFrameInfo_UpPts(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs,loopCountPerServerNode,SSFConfig.nUpPts);
// 	      }
// 	      else
// 	      {
// 		int nodeIndices=find(nodeDeviceIDs,nodeIDs(node_cnt));
// 		updateFlexibleSubFrameInfo_DL(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs(node_cnt),myNodeDevices(nodeIndices).actualLoopCountPerNU(nuIndx),DLSymbs,SFI);
// 		updateFlexibleSubFrameInfo_UL(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs(node_cnt),myNodeDevices(nodeIndices).actualLoopCountPerNU(nuIndx),ULSymbs,SFI);
// 	      }
// 	      Array<ivec> dummy(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts.length());
// 	      RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo,unScheduledRBs,zeros_b(unScheduledRBs.length()),true,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.isLocalised(unScheduledRBs(0)),&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo);
// 	      
// 	      for(int rb_cnt=0;rb_cnt<unScheduledRBs.length();rb_cnt++)
// 		myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.updateRUType(unScheduledRBs(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 	    }
// 	  }
// 	  myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.updateCurrentFrameStructure();
// 	  for(int db_cnt=0;db_cnt<schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo.length();db_cnt++)
// 	  {
// 	    schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(0));
// 	    schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure=*myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getCurrentFrameStructurePointer();
// 	    schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).reIndxForPrecoding.set_length(0);
// 	    
// 	    //!To get the RE indx while precoding for TM3 (May not be useful for other modes)
// 	    if(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_LARGE_DELAY_CDD_)
// 	    {
// 	      ivec myRBs=schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks;
// 	      Size_S rbSize=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitSize("nullRB");
// 	      imat allocationStructure(0,0);
// 	      allocationStructure=schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure.get_rows(myRBs(0)*rbSize.numberOfSubcarriers,myRBs(0)*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1);
// 	      for(int rb_cnt=1;rb_cnt<myRBs.length();rb_cnt++)
// 		appendVertical(allocationStructure,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure.get_rows(myRBs(rb_cnt)*rbSize.numberOfSubcarriers,myRBs(rb_cnt)*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1));
// 	      
// 	      imat dataIndxingStructure=allocationStructure;
// 	      
// 	      ivec dataREs=find(allocationStructure,0,true);
// 	      ivec reIndx=getIntegers(0,dataREs.length()-1);
// 	      setInMatrix(dataIndxingStructure,dataREs,reIndx);
// 	      
// 	      for(int rb_cnt=0;rb_cnt<myRBs.length();rb_cnt++)
// 	      {
// 		imat rbStructure=allocationStructure.get_rows(rb_cnt*rbSize.numberOfSubcarriers,rb_cnt*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1);
// 		ivec localDataREs=find(rbStructure,0,true);
// 		rbStructure=dataIndxingStructure.get_rows(rb_cnt*rbSize.numberOfSubcarriers,rb_cnt*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1);
// 		ivec dataREIndx=getFromMatrix(rbStructure,localDataREs);
// 		append(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).reIndxForPrecoding,dataREIndx);
// 	      }
// 	    }
// 	  }
// 	  
// 	  //! DL power-allocation 
// 	  
// 	  //====================================================================================================
// 	  // Code to apply Downlink Power allocation (3gpp 36.213 - Section 5.2) difference between
// 	  // OFDM symbols with and without CRS
// 	  // Following code section creates Power mask to apply on PDSCH RE
// 	  //====================================================================================================
// 	  if(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).downlinkSourceAndSink.tbTables->dlVarPowMask.isInitialized)
// 	  {
// 	    
// 	    if(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo.length())
// 	    {
// 	      if(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(0).dataGenInfo.rbInfo.rsInfo.DeModrsType != _RSTYPE_CRS_)
// 	      {cout<<"[both: ] Error Power Mask support provided only for CRS reference signals. Support not provided for CSI-RS and DMRS reference signals";exit(1);}
// 	      int totalScheduledRBPerSrvrNode = 0;
// 	      for(int serviceNode_cnt =0; serviceNode_cnt<schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo.length();serviceNode_cnt++)
// 		for(int rbcnt = 0; rbcnt < schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(serviceNode_cnt).resourceBlocks.length();rbcnt++)
// 		  totalScheduledRBPerSrvrNode++;
// 		imat logicalSubFrameStructure = schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(0).dataGenInfo.rbInfo.logicalSubFrameStructure;
// 	      if(totalScheduledRBPerSrvrNode != (logicalSubFrameStructure.rows() / myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitInfo("nullRB").resourceUnitSize.numberOfSubcarriers))
// 	      {cout<<"[both: ] Error No unscheduled RB in a Enode-B supported while applying power mask";exit(1);}
// 	      
// 	      generateDLPowerMask(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).downlinkSourceAndSink.tbTables, schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).txSubcarrierPowerInDBmPerAntenna(0),schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).Pb);
// 	    }
// 	  }
// 	  //===================================================================================================  
// 	  if(unScheduledRBs.length())
// 	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(unScheduledRBs(0)); 
// 	  else
// 	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(0); // All RBs contains CRS.
// 	    
// 	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.crsPowBoostInDB = schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(0).dataGenInfo.rbInfo.crsPowBoostInDB;
// 	  myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.logicalSubFrameStructure=*myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getCurrentFrameStructurePointer();
// 	}
// 	else 
// 	{
// 	  int schToTxInfoindx=schedulerOutput(srvrIndx).getSchedulerToTransmitterInfoIndx(nodeIDs(node_cnt),nuID,BeamIDs(node_cnt));
// 	  if(schToTxInfoindx!=-1) // UL Scheduled
// 	  {
// 	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.setCurrentFrameType("ulFrame");
// 	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.loadULResourceMapperForBandwidth(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getRuCountPerLevel());
// 	    
// 	    ivec loopCountPerServerNode=getLoopCount(nodeDeviceIDs,myNodeDevices,nodeIDuser,nuID);
// 	    if(CellConfig.length()==0)
// 	    {
// 	      SpecialSubFrameConfig_S SSFConfig;
// 	      getSpecialSubFrameConfig(&SSFConfig);
// 	      updateSpecialSubFrameInfo_DwPts(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs,loopCountPerServerNode,SSFConfig.nDwPts);
// 	      updateSpecialSubFrameInfo_UpPts(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs,loopCountPerServerNode,SSFConfig.nUpPts);
// 	    }
// 	    else
// 	    {
// 	      int nodeIndices=find(nodeDeviceIDs,nodeIDs(node_cnt));
// 	      updateFlexibleSubFrameInfo_DL(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs(node_cnt),myNodeDevices(nodeIndices).actualLoopCountPerNU(nuIndx),DLSymbs,SFI);
// 	      updateFlexibleSubFrameInfo_UL(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs(node_cnt),myNodeDevices(nodeIndices).actualLoopCountPerNU(nuIndx),ULSymbs,SFI);
// 	    }
// 	    
// 	    bool isDownlink=false;
// 	    bvec isTheRBScheduled=zeros_b(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe());
// 	    int ttiCount=schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).ttiCount;
// 	    int nRBs = myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe();
// 	    int nSb=nRBs/myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getResourceBlockGroupingSize();
// 	    for(int db_cnt=0;db_cnt<schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo.length();db_cnt++)
// 	    { 
// 	      bvec isAlreadyScheduled=isTheRBScheduled(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks);
// 	      setInVec(isTheRBScheduled,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks,ones_b(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length()));
// 	      // 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.perform_UL_Freq_hopping(ttiCount,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(0),schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length(),nSb,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.cellID,!schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).isLocalised);
// 	      RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks,isAlreadyScheduled,false,true,&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameReservationInfo);
// 	      schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB.set_length(0);
// 	      for(int rb_cnt=0;rb_cnt<schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length();rb_cnt++)
// 	      {
// 		myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.updateRUType(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 		append(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getToneCountPerRU(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),"data",schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).symPos));
// 	      }
// 	      schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getResourceUnitMatrix(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(0));
// 	      schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure=*myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getCurrentFrameStructurePointer();
// 	      if(!isWithAbstraction || (isWithAbstraction && (AbstractionModel == _ABSTRACTION_DECODER_ || AbstractionModel == _ABSTRACTION_IDEAL_)))
// 		schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).modulatedSymbols=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).uplinkSourceAndSink.getModulatedSymbols(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).txNodeID,&schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).harqInfo,ttiCount,isDownlink);
// 	      else
// 	      {
// 		myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).uplinkSourceAndSink.initializeHarqProcess(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).txNodeID,&schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).harqInfo,ttiCount,isDownlink);
// 		myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).uplinkSourceAndSink.initializeRxHarqProcess(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).txNodeID,&schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).harqInfo,ttiCount,isDownlink);
// 	      }
// 	    }
// 	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.updateCurrentFrameStructure();
// 	    
// 	  }
// 	  else 
// 	  {
// 	    if(isForTransmission) // UL UnScheduled
// 	    {
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.setCurrentFrameType("ulFrame");
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.loadULResourceMapperForBandwidth(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getRuCountPerLevel());
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.updateCurrentFrameStructure();
// 	      
// 	      // Need to write code to update unscheduled RBs for SRS transmission - Dhiv
// 	    }
// 	    else // DL UnScheduled
// 	    {
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.setCurrentFrameType("dlFrame");
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.loadDLResourceMapperForBandwidth(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getRuCountPerLevel());
// 	      CCHInfo_S cchInfo=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlTxConfig.cchInfo;
// 	      // 	    updateCCHReservationInfo(cchInfo,&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getRuCountPerLevel(),myNodeDevices(deviceIndices(node_cnt)).ttiCountPerNU(nuIndx),myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getPRBMapping());
// 	      updateCCHReservationInfo(cchInfo,myNodeDevices(deviceIndices(node_cnt)).SSBurstSetConfig,&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).nuParameters.subcarrierSpacingInHz/1e3,currentTime,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getRuCountPerLevel(),(int)(currentTime*1e3),myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getPRBMapping());
// 	      ivec loopCountPerServerNode=getLoopCount(nodeDeviceIDs,myNodeDevices,nodeIDuser,nuID);
// 	      if(CellConfig.length()==0)
// 	      {
// 		SpecialSubFrameConfig_S SSFConfig;
// 		getSpecialSubFrameConfig(&SSFConfig);
// 		updateSpecialSubFrameInfo_DwPts(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs,loopCountPerServerNode,SSFConfig.nDwPts);
// 		updateSpecialSubFrameInfo_UpPts(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs,loopCountPerServerNode,SSFConfig.nUpPts);
// 	      }
// 	      else
// 	      {
// 		int nodeIndices=find(nodeDeviceIDs,nodeIDs(node_cnt));
// 		updateFlexibleSubFrameInfo_DL(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs(node_cnt),myNodeDevices(nodeIndices).actualLoopCountPerNU(nuIndx),DLSymbs,SFI);
// 		updateFlexibleSubFrameInfo_UL(myNodeDevices,nodeDeviceIDs,nuID,nodeIDs(node_cnt),myNodeDevices(nodeIndices).actualLoopCountPerNU(nuIndx),ULSymbs,SFI);
// 	      }
// 	      ivec unScheduledRBs=getIntegers(0,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getResourceBlocksPerSubframe()-1);
// 	      {
// 		myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRSInfoForUnScheduled;
// 		if(!myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRxConfig.dlCQIModeInfo.isToTransmitCQIPorts(ttiCount(node_cnt)) && myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIrsType==_RSTYPE_CSIRS_)
// 		{
// 		  ivec cqiPortIndices=find(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIports);
// 		  deleteInArray(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsSymPosition,cqiPortIndices);
// 		  myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts=remove(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIports);
// 		}
// 		/*    
// 		 *	      if(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.fillPRS)
// 		 *	      {
// 		 *		if((ttiCount-myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.PRSoffset)%myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.PRSperiodicity==0)
// 		 *		{
// 		 *		  if(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.nCQIports<=2)
// 		 *		  {
// 		 *		    append(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,61);
// 		 *		    append(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.portReferenceNumbers,71);
// 	      }
// 	      else
// 	      {
// 	      append(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,62);
// 	      append(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.portReferenceNumbers,72);
// 	      }
// 	      }
// 	      }
// 	      */   
// 		//  Array<ivec> dummy(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts.length());
// 		RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo,unScheduledRBs,zeros_b(unScheduledRBs.length()),true, myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.isLocalised(0), &myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo);
// 		for(int rb_cnt=0;rb_cnt<unScheduledRBs.length();rb_cnt++)
// 		  myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.updateRUType(unScheduledRBs(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 	      }
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.updateCurrentFrameStructure();
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(unScheduledRBs(0));
// 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.logicalSubFrameStructure=*myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getCurrentFrameStructurePointer();  
// 	    updateFrameReservationInfo(myNodeDevices,nodeDeviceIDs,nodeIDs(node_cnt,node_cnt),nuID,false);
// 	      
// 	    }
// 	    
// 	  }
// 	}
//       }
//     }
//     
//   }
// 
// }



void nodePrepare(Array< SchedulerOutput_S >& schedulerOutput, AssociationInfo_S &associationInfo, Array<NodeDevice> &myNodeDevices, ivec nodeDeviceIDs, ivec nodeIDs, bool isWithAbstraction, ABSTRACTION_MODEL AbstractionModel, int nuID,double currentTime,ivec beamIDs, bool isForTransmission){
  
  ivec deviceIndices=find(nodeDeviceIDs,nodeIDs);
  ivec ttiCountPerNode=getTTICount(nodeDeviceIDs,myNodeDevices,nodeIDs,nuID);
  ivec loopCountPerNode=getLoopCount(nodeDeviceIDs,myNodeDevices,nodeIDs,nuID);
  int schToTxInfoindx,schToRxInfoindx;
  for(int node_cnt=0;node_cnt<nodeIDs.length();node_cnt++)
  {
    int nuIndx=myNodeDevices(deviceIndices(node_cnt)).getNUIndx(nuID);
    int ttiCount=ttiCountPerNode(node_cnt);
    int srvrIndx=getServerIndx(associationInfo,nodeIDs(node_cnt));
    if(nuIndx!=-1)
    {
      TransmissionType txType;
      if(isForTransmission)
      {
          schToTxInfoindx=schedulerOutput(srvrIndx).getSchedulerToTransmitterInfoIndx(nodeIDs(node_cnt),nuID,beamIDs(node_cnt)); //Do I need check for tx beam count 
          txType=schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).txType;
      }
      else
      {
          schToRxInfoindx=schedulerOutput(srvrIndx).getSchedulerToReceiverInfoIndx(nodeIDs(node_cnt),nuID,beamIDs(node_cnt));
          txType=schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).txType;
          
      }
      bool isDownlink=false;
      if(txType==_TRANSMISSION_TYPE_DL_)
          isDownlink=true;
      
      if(txType==_TRANSMISSION_TYPE_DL_)
      {
	myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.setCurrentFrameType("dlFrame");
	updateFrameReservationInfo(myNodeDevices,nodeDeviceIDs,nodeIDs(node_cnt,node_cnt),nuID,isDownlink);
	ivec unScheduledRBs=getIntegers(0,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getAllRuCount()-1);
	if(isForTransmission) //serverNode
	{
	  int schToTxInfoindx=schedulerOutput(srvrIndx).getSchedulerToTransmitterInfoIndx(nodeIDs(node_cnt),nuID,beamIDs(node_cnt)); //Do I need check for tx beam count 
	  if(schToTxInfoindx!=-1)
	  {
	    bvec isTheRBScheduled=zeros_b(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getAllRuCount());
	    for(int db_cnt=0;db_cnt<schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo.length();db_cnt++)
	    {
	      bvec isAlreadyScheduled=isTheRBScheduled(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks);
	      setInVec(isTheRBScheduled,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks,ones_b(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length()));
	      RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks,isAlreadyScheduled,true,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).isLocalised,&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo);
	      
	     schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB.set_length(0);
	      for(int rb_cnt=0;rb_cnt<schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length();rb_cnt++)
	      {
		myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.updateRUType(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
		append(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getToneCountPerRU(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),"data",schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).symPos));
	      }
	      
	      //FIXME Check
	      int ptrsIndx=find(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo.rsPorts,1200);
#ifdef SCHEDULER_DEBUG
          cout<<"ptrsIndx : "<<ptrsIndx<<endl;
#endif
	      if(ptrsIndx != -1)
		schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo.rsrbpos(ptrsIndx)(0));
	      else
		schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(0));
#ifdef SCHEDULER_DEBUG	     
	      cout<<"\n\nrsPositionMatrix : \n"<<schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix<<endl;
#endif      
	      schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure=*myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getCurrentFrameStructurePointer();
	      
	      //!To get the RE indx while precoding for TM3 (May not be useful for other modes)
	      if(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_LARGE_DELAY_CDD_)
		setREIndxForTM3Precoding( myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitSize("nullRB"),schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt));
	      
	      if(!isWithAbstraction || (isWithAbstraction && (AbstractionModel == _ABSTRACTION_DECODER_ || AbstractionModel == _ABSTRACTION_IDEAL_)))
		schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).modulatedSymbols=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).downlinkSourceAndSink.getModulatedSymbols(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).rxNodeIDs(db_cnt),&schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).harqInfo,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).ttiCount,isDownlink);
	      else
		myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).downlinkSourceAndSink.initializeHarqProcess(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).rxNodeIDs(db_cnt),&schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).harqInfo,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).ttiCount,isDownlink);
	    }
	    
	    if(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).downlinkSourceAndSink.tbTables->dlVarPowMask.isInitialized)
	    {
	      if(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo.length())
	      {
		if(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(0).dataGenInfo.rbInfo.rsInfo.DeModrsType != _RSTYPE_CRS_)
		{cout<<"[both: ] Error Power Mask support provided only for CRS reference signals. Support not provided for CSI-RS and DMRS reference signals";exit(1);}
		int totalScheduledRBPerSrvrNode = 0;
		for(int serviceNode_cnt =0; serviceNode_cnt<schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo.length();serviceNode_cnt++)
		  for(int rbcnt = 0; rbcnt < schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(serviceNode_cnt).resourceBlocks.length();rbcnt++)
		    totalScheduledRBPerSrvrNode++;
		  imat logicalSubFrameStructure = schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(0).dataGenInfo.rbInfo.logicalSubFrameStructure;
		if(totalScheduledRBPerSrvrNode != (logicalSubFrameStructure.rows() / myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitInfo("nullRB").resourceUnitSize.numberOfSubcarriers))
		{cout<<"[both: ] Error No unscheduled RB in a Enode-B supported while applying power mask";exit(1);}
		
		generateDLPowerMask(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).downlinkSourceAndSink.tbTables, schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).txSubcarrierPowerInDBmPerAntenna,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).Pb);
	      }
	    }
	    if(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo.length())
	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.crsPowBoostInDB = schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(0).dataGenInfo.rbInfo.crsPowBoostInDB;
	    
	    unScheduledRBs=find(to_ivec(isTheRBScheduled),0,true);
	    
	  }
	}
	else //serviceNode
	{
	  int schToRxInfoindx=schedulerOutput(srvrIndx).getSchedulerToReceiverInfoIndx(nodeIDs(node_cnt),nuID,beamIDs(node_cnt));
	  if(schToRxInfoindx!=-1)
	  {
	    bvec isTheRBScheduled=zeros_b(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getAllRuCount());
	    for(int db_cnt=0;db_cnt<schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo.length();db_cnt++)
	    {
	      bvec isAlreadyScheduled=isTheRBScheduled(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks);
	      setInVec(isTheRBScheduled,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks,ones_b(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length()));
	      RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks,isAlreadyScheduled,true,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).isLocalised,&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo);
	      schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB.set_length(0);
	      for(int rb_cnt=0;rb_cnt<schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length();rb_cnt++)
	      {
		myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.updateRUType(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
		append(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getToneCountPerRU(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),"data",schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).symPos));
	      }
	      
	      int ptrsIndx=find(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo.rsPorts,1200); 
	      if(ptrsIndx != -1) 
		schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo.rsrbpos(ptrsIndx)(0));
	      else
		schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(0));
#ifdef SCHEDULER_DEBUG
	      cout<<"\n\nrsPositionMatrix check at rx : \n"<<schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix<<endl;
#endif    
	      schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure=*myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getCurrentFrameStructurePointer();
	      schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).reIndxForPrecoding.set_length(0);
	      
	      //!To get the RE indx while precoding for TM3 (May not be useful for other modes)
	      if(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_LARGE_DELAY_CDD_)
	      {
		ivec myRBs=schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks;
		Size_S rbSize=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitSize("nullRB");
		imat allocationStructure(0,0);
		allocationStructure=schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure.get_rows(myRBs(0)*rbSize.numberOfSubcarriers,myRBs(0)*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1);
		for(int rb_cnt=1;rb_cnt<myRBs.length();rb_cnt++)
		  appendVertical(allocationStructure,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure.get_rows(myRBs(rb_cnt)*rbSize.numberOfSubcarriers,myRBs(rb_cnt)*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1));
		
		imat dataIndxingStructure=allocationStructure;
		
		ivec dataREs=find(allocationStructure,0,true);
		ivec reIndx=getIntegers(0,dataREs.length()-1);
		setInMatrix(dataIndxingStructure,dataREs,reIndx);
		
		for(int rb_cnt=0;rb_cnt<myRBs.length();rb_cnt++)
		{
		  imat rbStructure=allocationStructure.get_rows(rb_cnt*rbSize.numberOfSubcarriers,rb_cnt*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1);
		  ivec localDataREs=find(rbStructure,0,true);
		  rbStructure=dataIndxingStructure.get_rows(rb_cnt*rbSize.numberOfSubcarriers,rb_cnt*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1);
		  ivec dataREIndx=getFromMatrix(rbStructure,localDataREs);
		  append(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).reIndxForPrecoding,dataREIndx);
		}
	      }
	    }
	    
	    //! DL power-allocation 
	    
	    //====================================================================================================
	    // Code to apply Downlink Power allocation (3gpp 36.213 - Section 5.2) difference between
	    // OFDM symbols with and without CRS
	    // Following code section creates Power mask to apply on PDSCH RE
	    //====================================================================================================
	    if(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).downlinkSourceAndSink.tbTables->dlVarPowMask.isInitialized)
	    {
	      
	      if(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo.length())
	      {
		if(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(0).dataGenInfo.rbInfo.rsInfo.DeModrsType != _RSTYPE_CRS_)
		{cout<<"[both: ] Error Power Mask support provided only for CRS reference signals. Support not provided for CSI-RS and DMRS reference signals";exit(1);}
		int totalScheduledRBPerSrvrNode = 0;
		for(int serviceNode_cnt =0; serviceNode_cnt<schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo.length();serviceNode_cnt++)
		  for(int rbcnt = 0; rbcnt < schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(serviceNode_cnt).resourceBlocks.length();rbcnt++)
		    totalScheduledRBPerSrvrNode++;
		  imat logicalSubFrameStructure = schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(0).dataGenInfo.rbInfo.logicalSubFrameStructure;
		if(totalScheduledRBPerSrvrNode != (logicalSubFrameStructure.rows() / myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitInfo("nullRB").resourceUnitSize.numberOfSubcarriers))
		{cout<<"[both: ] Error No unscheduled RB in a Enode-B supported while applying power mask";exit(1);}
		
		generateDLPowerMask(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).downlinkSourceAndSink.tbTables, schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).txSubcarrierPowerInDBmPerAntenna(0),schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).Pb);
	      }
	    }
	    
	    unScheduledRBs=find(to_ivec(isTheRBScheduled),0,true);
	    
// 	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.crsPowBoostInDB = schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(0).dataGenInfo.rbInfo.crsPowBoostInDB;
	    
	  }
	}
	
	myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRSInfoForUnScheduled;
	if(!myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRxConfig.dlCQIModeInfo.isToTransmitCQIPorts(ttiCount) && myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIrsType==_RSTYPE_CSIRS_)
	{
	  ivec cqiPortIndices=find(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIports);
	  deleteInArray(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsSymPosition,cqiPortIndices);
      deleteInVec(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsScPosition,cqiPortIndices);
      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts=remove(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIports);
      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIports.set_length(0);
	}
	if(unScheduledRBs.length())
	{
	  RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo,unScheduledRBs,zeros_b(unScheduledRBs.length()),true,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.isLocalised(unScheduledRBs(0)),&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo);
	  for(int rb_cnt=0;rb_cnt<unScheduledRBs.length();rb_cnt++)
	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.updateRUType(unScheduledRBs(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
	  myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(unScheduledRBs(0)); 
	}
	else
	  myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(0); 
	
	myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.updateCurrentFrameStructure();
	myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.logicalSubFrameStructure=*myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getCurrentFrameStructurePointer();
      }
      else //  if(txType==_TRANSMISSION_TYPE_UL_ || txType==_TRANSMISSION_TYPE_SL_)
      {
         
	myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.setCurrentFrameType("ulFrame");
	updateFrameReservationInfo(myNodeDevices,nodeDeviceIDs,nodeIDs(node_cnt,node_cnt),nuID,isDownlink);
	ivec unScheduledRBs=getIntegers(0,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getAllRuCount()-1);
	
	if(isForTransmission) //serviceNode
	{
	  int schToTxInfoindx=schedulerOutput(srvrIndx).getSchedulerToTransmitterInfoIndx(nodeIDs(node_cnt),nuID,beamIDs(node_cnt));
	  if(schToTxInfoindx!=-1)
	  {
	    bvec isTheRBScheduled=zeros_b(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe());
	    int ttiCount=schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).ttiCount;
	    int nRBs = myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe();
	    int nSb=nRBs/myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getResourceBlockGroupingSize();
	    for(int db_cnt=0;db_cnt<schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo.length();db_cnt++)
	    { 
	      bvec isAlreadyScheduled=isTheRBScheduled(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks);
	      setInVec(isTheRBScheduled,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks,ones_b(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length()));
	      // 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.perform_UL_Freq_hopping(ttiCount,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(0),schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length(),nSb,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.cellID,!schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).isLocalised);
	      RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks,isAlreadyScheduled,false,true,&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameReservationInfo);
	      schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB.set_length(0);
	      for(int rb_cnt=0;rb_cnt<schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length();rb_cnt++)
	      {
		myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.updateRUType(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
		append(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getToneCountPerRU(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),"data",schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).symPos));
	      }
	      
          SourceAndSink *sourceAndSink;
          int serviceNodeIDForSourceAndSink;
          if(txType==_TRANSMISSION_TYPE_UL_)
          {
              sourceAndSink = &myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).uplinkSourceAndSink;
              serviceNodeIDForSourceAndSink=schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).txNodeID;
          }
          else if(txType==_TRANSMISSION_TYPE_SL_)
          {
              sourceAndSink = &myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).sidelinkSourceAndSink;
              serviceNodeIDForSourceAndSink=schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).rxNodeIDs(db_cnt); 
          }
	      int ptrsIndx=find(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo.rsPorts,1200); 
	      if(ptrsIndx != -1)
		schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo.rsrbpos(ptrsIndx)(0));
	      else
		schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getResourceUnitMatrix(schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(0));
	schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure=*myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getCurrentFrameStructurePointer();
          if(!isWithAbstraction || (isWithAbstraction && (AbstractionModel == _ABSTRACTION_DECODER_ || AbstractionModel == _ABSTRACTION_IDEAL_)))
            schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).modulatedSymbols=sourceAndSink->getModulatedSymbols(serviceNodeIDForSourceAndSink,&schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).harqInfo,ttiCount,isDownlink);
	      else
	      {
              sourceAndSink->initializeHarqProcess(serviceNodeIDForSourceAndSink,&schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).harqInfo,ttiCount,isDownlink);
              sourceAndSink->initializeRxHarqProcess(serviceNodeIDForSourceAndSink,&schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).dataGenInfo,schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoindx).dataBlkInfo(db_cnt).harqInfo,ttiCount,isDownlink);
	      }
	    }
	    unScheduledRBs=find(to_ivec(isTheRBScheduled),0,true);
	    
	  }
	}
	else //serverNode
	{
	  int schToRxInfoindx=schedulerOutput(srvrIndx).getSchedulerToReceiverInfoIndx(nodeIDs(node_cnt),nuID,beamIDs(node_cnt));
	  if(schToRxInfoindx!=-1)
	  {
	    bvec isTheRBScheduled=zeros_b(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe());
	    for(int db_cnt=0;db_cnt<schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo.length();db_cnt++)
	    {
	      
	      bvec isAlreadyScheduled=isTheRBScheduled(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks);
	      setInVec(isTheRBScheduled,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks,ones_b(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length()));
	      // 	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.perform_UL_Freq_hopping(ttiCount,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(0),schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length(),nSb,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.cellID,!schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).isLocalised);
	      RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo,schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks,isAlreadyScheduled,false,true,&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameReservationInfo);
	      schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB.set_length(0);
	      for(int rb_cnt=0;rb_cnt<schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks.length();rb_cnt++)
	      {
		myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.updateRUType(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
		append(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getToneCountPerRU(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),"data",schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).symPos));
	      }
	      int ptrsIndx=find(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo.rsPorts,1200); 
	      if(ptrsIndx != -1)
		schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo.rsrbpos(ptrsIndx)(0));
	      else
		schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getResourceUnitMatrix(schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).resourceBlocks(0));
#ifdef SCHEDULER_DEBUG
	      cout<<"\n\nrsPositionMatrix 2 check at rx : \n"<<schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix<<endl;
#endif  
          schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoindx).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure=*myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getCurrentFrameStructurePointer();
	    }
	    unScheduledRBs=find(to_ivec(isTheRBScheduled),0,true);
	    
	  }
	}
	
	myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulRbInfoForUnScheduled.rsInfo=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulRSInfoForUnScheduled;
	
	if(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulRbInfoForUnScheduled.rsInfo.CQIports.length()) // FIXME need to find a way to set this for servernode - Dhiv
	  if(!myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulRxConfig.ulCQIModeInfo.isToTransmitCQIPorts(ttiCount))
	  {
	    ivec cqiPortIndices=find(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulRbInfoForUnScheduled.rsInfo.rsPorts,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulRbInfoForUnScheduled.rsInfo.CQIports);
	    deleteInArray(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulRbInfoForUnScheduled.rsInfo.rsSymPosition,cqiPortIndices);
	    deleteInVec(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulRbInfoForUnScheduled.rsInfo.rsScPosition,cqiPortIndices);
	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulRbInfoForUnScheduled.rsInfo.rsPorts=remove(myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulRbInfoForUnScheduled.rsInfo.rsPorts,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulRbInfoForUnScheduled.rsInfo.CQIports);
	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulRbInfoForUnScheduled.rsInfo.CQIports.set_length(0);
	  }
	  if(unScheduledRBs.length())
	  {
	    RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure,myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulRbInfoForUnScheduled.rsInfo,unScheduledRBs,zeros_b(unScheduledRBs.length()),false,true,&myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameReservationInfo);
	    for(int rb_cnt=0;rb_cnt<unScheduledRBs.length();rb_cnt++)
	      myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.updateRUType(unScheduledRBs(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulRbInfoForUnScheduled.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getResourceUnitMatrix(unScheduledRBs(0)); 
	  }
	  else	// This else is not requried for UL - Dhiv
	    myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulRbInfoForUnScheduled.rsPositionMatrix=myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getResourceUnitMatrix(0); 
	  
	  myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.updateCurrentFrameStructure();
	  myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulRbInfoForUnScheduled.logicalSubFrameStructure=*myNodeDevices(deviceIndices(node_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getCurrentFrameStructurePointer();
	  
      }
      
    }    
    
  }
}


Array<UEBufferInfo_S> L2Core_S::getUEBufferInfo(int subframeNumber, TransmissionType txType, int beamID)
{
  ivec beamAssociatedUsers;
  beamAssociatedUsers.set_length(0);  
  ivec index;
  index.set_length(0);
  for(int i=0; i<beamPairLinkperUE.length(); i++)
  {
      if(beamPairLinkperUE(i).srvrBeamID == beamID)
      if(find(activeUEs,associatedUEs(i))!=-1)    
      {
	append(beamAssociatedUsers,associatedUEs(i));
	append(index, i);
      }
  }
  
  Array<UEBufferInfo_S> bufferInfo(beamAssociatedUsers.length());
  for(int ue_cnt=0;ue_cnt<beamAssociatedUsers.length();ue_cnt++)
  {
    bufferInfo(ue_cnt).mUEId=beamAssociatedUsers(ue_cnt);
    if(txType == _TRANSMISSION_TYPE_DL_)
    {
        bufferInfo(ue_cnt).mDataInBufferInBytes=dlRadioBearer(index(ue_cnt)).getBufferSize(subframeNumber,bufferInfo(ue_cnt).mRBId);
        if(bufferInfo(ue_cnt).mDataInBufferInBytes!=0)
            bufferInfo(ue_cnt).isInfiniteBuffer=dlRadioBearer(index(ue_cnt)).isInfiniteBuffer(bufferInfo(ue_cnt).mRBId);
    }
    else
    {
        bufferInfo(ue_cnt).mDataInBufferInBytes=ulRadioBearer(index(ue_cnt)).getBufferSize(subframeNumber,bufferInfo(ue_cnt).mRBId);	
        if(bufferInfo(ue_cnt).mDataInBufferInBytes!=0)
            bufferInfo(ue_cnt).isInfiniteBuffer=ulRadioBearer(index(ue_cnt)).isInfiniteBuffer(bufferInfo(ue_cnt).mRBId);
    }
  }
  return bufferInfo;
}

void L2Core_S::updateUEBufferInfo(int ueID, int tbSizeInBytes,int rbID, TransmissionType txType)
{
  int ueIndx=find(associatedUEs,ueID);
  if(txType == _TRANSMISSION_TYPE_DL_)
    dlRadioBearer(ueIndx).reduceBufferSize(tbSizeInBytes,rbID);	
  else
    ulRadioBearer(ueIndx).reduceBufferSize(tbSizeInBytes,rbID);	
}

Array< ReTxInfo_S > getUEReTxInfo(NodeDevice &srvrNodeDevice, int nuID, int subframeNumber, TransmissionType txType,int beamID)
{
  ivec beamAssociatedUsers;
  beamAssociatedUsers.set_length(0);
  int nuIndx=srvrNodeDevice.getNUIndx(nuID);	 
  ivec index;
  index.set_length(0);
  
  for(int i=0; i<srvrNodeDevice.L2CorePerNU(nuIndx).beamPairLinkperUE.length(); i++)
  {
    if(srvrNodeDevice.L2CorePerNU(nuIndx).beamPairLinkperUE(i).srvrBeamID == beamID)
        if(find(srvrNodeDevice.L2CorePerNU(nuIndx).activeUEs,srvrNodeDevice.L2CorePerNU(nuIndx).associatedUEs(i))!=-1)    
      {
      append(beamAssociatedUsers,srvrNodeDevice.L2CorePerNU(nuIndx).associatedUEs(i));
	append(index, i); 
      }
  }
  
  Array<ReTxInfo_S> retxInfo(beamAssociatedUsers.length());
  for(int ue_cnt=0;ue_cnt<beamAssociatedUsers.length();ue_cnt++)
  {
    retxInfo(ue_cnt).ueID=beamAssociatedUsers(ue_cnt);
    // Here nCodewords is based on previous transmission.. 
    HarqInfo harqInfo;
    if(txType==_TRANSMISSION_TYPE_DL_)
        harqInfo=srvrNodeDevice.L2CorePerNU(nuIndx).dlHarqManager(index(ue_cnt)).getCurrentHarqInfo(subframeNumber,srvrNodeDevice.L1CorePerNU(nuIndx).dlTxConfig.nCodeWords); // using codewords from config file - need to clarify
    else if(txType==_TRANSMISSION_TYPE_UL_ || txType==_TRANSMISSION_TYPE_SL_)
        harqInfo=srvrNodeDevice.L2CorePerNU(nuIndx).ulHarqManager(index(ue_cnt)).getCurrentHarqInfo(subframeNumber,srvrNodeDevice.L1CorePerNU(nuIndx).ulTxConfig.nCodeWords);
    
      retxInfo(ue_cnt).isNewTransmission=harqInfo.isNewTransmission;
    // nCodewords for retx and the one in configfile has to be handled properly
    retxInfo(ue_cnt).tbSizeInBitsForReTx=harqInfo.TBSizePerCodewordInBits;
    retxInfo(ue_cnt).codewordIndices=harqInfo.codeWordIndicesToConsider;
    
  }
  return retxInfo;
}

ivec getDLULSymbsForCellSpecificConfig(int ttiCount,ivec CellConfig, int CellConfigPeriodicityInSlots)//Gives the DL/UL symbols for flexible subframe 
{
  
  int DLSymbs,ULSymbs;
  ivec DLULSymbs;
  
  if(mod(ttiCount,CellConfigPeriodicityInSlots)==CellConfig(0) && mod(ttiCount,CellConfigPeriodicityInSlots)==(CellConfigPeriodicityInSlots-CellConfig(2)-1))
  {
    DLSymbs=CellConfig(1);
    ULSymbs=CellConfig(3);
  }
  else if (mod(ttiCount,CellConfigPeriodicityInSlots)==(CellConfigPeriodicityInSlots-CellConfig(2)-1))
  {
    DLSymbs=0;
    ULSymbs=CellConfig(3);
  }
  else if(mod(ttiCount,CellConfigPeriodicityInSlots)==CellConfig(0))
  {
    DLSymbs=CellConfig(1);
    ULSymbs=0;
  }
  else
  {
    DLSymbs=0;
    ULSymbs=0;
  }
  append(DLULSymbs,DLSymbs);
  append(DLULSymbs,ULSymbs);
  return DLULSymbs;
}
