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


#include "../include/Receiver.h"
#include "../include/NodeDevice.h"

// #define ABSTRACTION_DEBUG
/*
Array<TransmissionStatus_S> Receiver::abstractReceiver(RxFrameInfo_S& myRxFrameInfo, SystemInfo_S& mySysInfo, bool isDownlink, PerformanceMetrics_S* performanceMetrics, Array< TxFrameInfo_S >& txFrameInfo, SchedulerToReceiverInfo_S& schedulerToReceiverInfo, ChannelCloud& myChannelCloud, string MCSlogsFileName){

  int subframeNumber=schedulerToReceiverInfo.ttiCount;
  ChannelHandler_S *myChannelHandler = &myRxFrameInfo.channelHandler;
  ivec txNodesToReceiveSignalsFrom = myRxFrameInfo.txNodesToReceiveSignalFrom;
  double roiScRxPowInWatts = myRxFrameInfo.ROIpowerInWatts;

  Array<TransmissionStatus_S> txStatusPerServiceNode(schedulerToReceiverInfo.dataBlkInfo.length());
  bool decodeAndUpdateMetric=false;
  if(isDownlink && (mySysInfo.linkSimulationInfo.isEnabled || receiverConfig.enableLinkAdaptation==false || subframeNumber>=receiverConfig.dlCQIModeInfo.CQIPMIOffset+receiverConfig.dlCQIModeInfo.CQIProcessingDelay))
    decodeAndUpdateMetric=true;
  else if(!isDownlink && (mySysInfo.linkSimulationInfo.isEnabled || receiverConfig.enableLinkAdaptation==false || subframeNumber>=receiverConfig.uplinkCQIProcessingDelay))
    decodeAndUpdateMetric=true;
  
  //mySysInfo.AbstractionModel=2;
  
  ivec myChannelIndices, interfererChannelIndices;
  if(mySysInfo.AbstractionModel==2)
  {
    myChannelIndices=find(myChannelHandler->txNodeIDs,schedulerToReceiverInfo.txNodeIDs);
    interfererChannelIndices=findNot(myChannelHandler->txNodeIDs,schedulerToReceiverInfo.txNodeIDs);
  }
  if(mySysInfo.AbstractionModel==1 && !isDownlink)
  {
    cout<<"[both:]Abstraction Model 1 is not supported for Uplink."<<endl;
    abort();
  }
  int nuIndx=myNodeDevice->getNUIndx(schedulerToReceiverInfo.rxNUID);
  if(isDownlink)
    frameHandler.pointFrameStructure(&myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure);
  else
    frameHandler.pointFrameStructure(&myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure);
  
  frameHandler.setDataBlkInfo(schedulerToReceiverInfo.dataBlkInfo);
  
  SourceAndSink *sourceAndSink;
  if(isDownlink)
    sourceAndSink=&myNodeDevice->L1CorePerNU(nuIndx).downlinkSourceAndSink;
  else
    sourceAndSink=&myNodeDevice->L1CorePerNU(nuIndx).uplinkSourceAndSink;
  
  if(schedulerToReceiverInfo.dataBlkInfo.length()!=0)
  {
    for (int blk_cnt=0;blk_cnt<schedulerToReceiverInfo.dataBlkInfo.length();blk_cnt++)
    {
      if(decodeAndUpdateMetric)
      {
	int nCodeWords=schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.nCodeWords;
    ivec TBsize = schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.TBSizePerCodewordInBits;
	ivec mcsIndices=schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.mcsIndices;
	vec BLERperCodewords(nCodeWords);
	
	txStatusPerServiceNode(blk_cnt).serviceNodeID=schedulerToReceiverInfo.serviceNodesForSourceAndSink(blk_cnt);
	txStatusPerServiceNode(blk_cnt).harqProcessID=schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID;
	txStatusPerServiceNode(blk_cnt).codeWordIndicesToConsider=schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.codeWordIndicesToConsider;
	txStatusPerServiceNode(blk_cnt).isSuccess.set_length(nCodeWords);
	txStatusPerServiceNode(blk_cnt).txInfoBitsPerCodeWord=schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.infoBitsperCodeWord;//! NOTE Available only when L2 scheduler runs 
	txStatusPerServiceNode(blk_cnt).rxInfoBitsPerCodeWord.set_length(nCodeWords);
	txStatusPerServiceNode(blk_cnt).nBitsInError=zeros_i(nCodeWords);
	
	ivec nLayersPerCodeword = getNumLayersPerCodeWord(nCodeWords,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.mimoRate);
	
	double nMSE=0.0;
	vec ppSINRperCW(nCodeWords);
	Array<vec> SINRoverRBs;

	if(mySysInfo.AbstractionModel==2)
	{
	  DataBlkInfo_S myDataBlkInfo=schedulerToReceiverInfo.dataBlkInfo(blk_cnt);
	  SINRoverRBs = ComputeSINROverRBs(isDownlink,myChannelHandler,myChannelIndices(blk_cnt),interfererChannelIndices,txNodesToReceiveSignalsFrom,roiScRxPowInWatts,txFrameInfo,schedulerToReceiverInfo,myChannelCloud,myDataBlkInfo);
	}
	
	for(int icw=0; icw<nCodeWords; icw++)
	{
	  int modulation_index=getModulationOrderAsLinearIndex(mcsIndices(icw));
	  
	  // effSINR for QPSK and 16 QAM will saturate soon, Taking effSINR for 64 QAM will be better
	  modulation_index = 2;
	  
	  vec observedMIsOverRBs(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).resourceBlocks.length());
	  if(mySysInfo.AbstractionModel==1)
	  {
	    for(int rb_cnt=0;rb_cnt<schedulerToReceiverInfo.dataBlkInfo(blk_cnt).resourceBlocks.length();rb_cnt++)
	    {
	      if(nCodeWords==1)
	      {
		if(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_CL_SM_  || schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_CL_BF_ || schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_CL_MU_MIMO_)
		{
		  cout<<"[both:]Abstraction Model 1 is not supported for closed loop schemes "<<endl;abort();
		}
		else
		  observedMIsOverRBs(rb_cnt)=cqiEstimator.mutualInfo.MIsPerRBforSingleCWD(modulation_index)(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).resourceBlocks(rb_cnt))(0);
	      }
	      else
	      {
		if(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_CL_SM_ )
		{
		  cout<<"[both:]Abstraction Model 1 is not supported for closed loop schemes "<<endl;abort();
		}
		else
		  observedMIsOverRBs(rb_cnt)=cqiEstimator.mutualInfo.MIsPerRBforTwoCWD(modulation_index)(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).resourceBlocks(rb_cnt))(0)(icw);
	      }
	    }
	  }
	  int modOrder;
	  if(isDownlink)
	    modOrder=sourceAndSink->lteTables.transportBlockMCSMapping.modulationIndexPDSCH(mcsIndices(icw));
	  else
	    modOrder=sourceAndSink->lteTables.transportBlockMCSMapping.modulationIndexPUSCH(mcsIndices(icw));
	  // effSINR for QPSK and 16 QAM will saturate soon, Taking effSINR for 64 QAM will be better
	  int effModOrder = 6;
	  
	  double effMMIB;
	  if(mySysInfo.AbstractionModel==2)
	    effMMIB = cqiEstimator.lteTables->awgnTablesConvCoder.sinrToEffMMIB(SINRoverRBs(icw),ModulationScheme_E(effModOrder));
	  else //if(mySysInfo.AbstractionModel==1)
	    effMMIB = mean(observedMIsOverRBs);

	  int nCRC,nBlocks=cqiEstimator.lteTables->getCodeBlockSizes(TBsize(icw)).length();
	  if(nBlocks>1)
	    nCRC=(nBlocks+1)*24;
	  else
	    nCRC=24;
	  
	  #ifdef ABSTRACTION_DEBUG
	  cout<<"effSINR : "<<effSINR<<endl;
	  cout<<"effSINR 2 : "<<cqiEstimator.lteTables->awgnTablesConvCoder.mmibToSinr(ModulationScheme_E(2), cqiEstimator.lteTables->awgnTablesConvCoder.sinrToEffMMIB(SINRoverRBs(icw),ModulationScheme_E(2)))<<endl;
	  cout<<"effSINR 4 : "<<cqiEstimator.lteTables->awgnTablesConvCoder.mmibToSinr(ModulationScheme_E(4), cqiEstimator.lteTables->awgnTablesConvCoder.sinrToEffMMIB(SINRoverRBs(icw),ModulationScheme_E(4)))<<endl;
	  cout<<"effSINR 6 : "<<cqiEstimator.lteTables->awgnTablesConvCoder.mmibToSinr(ModulationScheme_E(6), cqiEstimator.lteTables->awgnTablesConvCoder.sinrToEffMMIB(SINRoverRBs(icw),ModulationScheme_E(6)))<<endl;
	  #endif
	  
	  double nCodedBits = (double)(sum(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.resourceElementsPerRB)*nLayersPerCodeword(icw)*modOrder);
	
	  int indx=-1;
	  for(int i=0;i<sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID).length();i++)
	  {
	    if(sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(i).ueID==schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.rnti)
	    {
	      indx=i;
	      break;
	    }
	  }
	  
	  if(indx!=-1)
	  {
	    if(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.isNewTransmission(icw) && sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(indx).TransmissionCount(icw)!=0)
	    {
	      sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(indx).TransmissionCount(icw)=0;
	      sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(indx).effMMIBperReceptionPerCW(icw).set_length(0);
	      sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(indx).nCodedBitsPerReceptionPerCW(icw).set_length(0);
	    }
	    
	    sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(indx).TransmissionCount(icw)+=1;
	    append(sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(indx).effMMIBperReceptionPerCW(icw),effMMIB);
	    append(sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(indx).nCodedBitsPerReceptionPerCW(icw),nCodedBits);
	  }
	  else
	  {
	    indx=sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID).length();
	    sHARQAbstractionModel newEntry; 
	    append(sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID),newEntry);
	    sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(indx).ueID=schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.rnti;
	    sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(indx).HarqProcessID=schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID;
	    sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(indx).TransmissionCount(icw)+=1;
	    append(sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(indx).effMMIBperReceptionPerCW(icw),effMMIB);
	    append(sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(indx).nCodedBitsPerReceptionPerCW(icw),nCodedBits);
	    
	  }
	  
	  double effCodeRatewithReTx=(double)(TBsize(icw) + nCRC )/sum(sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(indx).nCodedBitsPerReceptionPerCW(icw));
	  double effMMIBwithReTx=mean(sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(indx).effMMIBperReceptionPerCW(icw));
	  double effSINRwithReTx=cqiEstimator.lteTables->awgnTablesConvCoder.mmibToSinr(ModulationScheme_E(effModOrder),effMMIBwithReTx);
	  BLERperCodewords(icw) = cqiEstimator.lteTables->awgnTablesConvCoder.getBlerFromMmibForAbstraction(effCodeRatewithReTx,effSINRwithReTx,ModulationScheme_E(modOrder),TBsize(icw)+nCRC);

	  #ifdef ABSTRACTION_DEBUG	  
	  cout<<"nCodedBits : "<<sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(indx).nCodedBitsPerReceptionPerCW(icw)<<endl;
	  cout<<"effCodeRatewithReTx : "<<effCodeRatewithReTx<<endl;
	  cout<<"MMIB : "<<sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(indx).effMMIBperReceptionPerCW(icw)<<endl;
	  cout<<"isNewTransmission : "<<schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.isNewTransmission<<endl;
	  cout<<"BLERperCodewords : "<<BLERperCodewords<<endl;
	  #endif

	  // PostProcessing SINR
	  ppSINRperCW(icw)=0;
	  if(mySysInfo.AbstractionModel==1)
	  {
	    for(int cnt=0;cnt<observedMIsOverRBs.length();cnt++)
	    {
	      ppSINRperCW(icw)+=inv_dB(cqiEstimator.lteTables->awgnTablesConvCoder.mmibToSinr(ModulationScheme_E(effModOrder),observedMIsOverRBs(cnt)));
	    }
	    ppSINRperCW(icw)/=observedMIsOverRBs.length();
	  }
	  else if(mySysInfo.AbstractionModel==2)
	  {
	    ppSINRperCW(icw)=mean(SINRoverRBs(icw));
	  }

	  if(randu()>BLERperCodewords(icw))
	  {
	    txStatusPerServiceNode(blk_cnt).isSuccess(icw)=1;
	    if(txStatusPerServiceNode(blk_cnt).txInfoBitsPerCodeWord.length())
	      txStatusPerServiceNode(blk_cnt).rxInfoBitsPerCodeWord(icw)=txStatusPerServiceNode(blk_cnt).txInfoBitsPerCodeWord(icw);
	    
	    for(int i=0;i<sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID).length();i++)
	    {
	      if(sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(i).ueID==schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.rnti)
	      {
		sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(i).TransmissionCount(icw)=0;
		sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(i).effMMIBperReceptionPerCW(icw).set_length(0);
		sourceAndSink->mHarqAbstractionModel(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(i).nCodedBitsPerReceptionPerCW(icw).set_length(0);
		break;
	      } 
	    }
	      
	  }
	  else
	  {
	    txStatusPerServiceNode(blk_cnt).isSuccess(icw)=0;
	    txStatusPerServiceNode(blk_cnt).rxInfoBitsPerCodeWord(icw)=zeros_b(TBsize(icw));
	    txStatusPerServiceNode(blk_cnt).nBitsInError(icw)=(TBsize(icw)/2.0)*(BLERperCodewords(icw)/2.0);
	  }
	  if(traceValuePerTraceName["EnablePrints"]=="true")
	    cout<<"[detl:] Abstraction Rx nodeID : "<<txStatusPerServiceNode(blk_cnt).serviceNodeID<<" effSINR : "<<effSINRwithReTx<<" BLER : "<<BLERperCodewords(icw)<<" isSuccess : "<<(int)txStatusPerServiceNode(blk_cnt).isSuccess(icw)<<endl;
	  
	}
	txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword = TBsize;
	
	bvec isNewTransmission=schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.isNewTransmission;
	
	sourceAndSink->setReceptionStatusInHarqManager(schedulerToReceiverInfo.serviceNodesForSourceAndSink(blk_cnt),txStatusPerServiceNode(blk_cnt).isSuccess,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID);
	
	if(traceValuePerTraceName["EnablePrints"]=="true")
	{
	  for(int icw=0;icw<isNewTransmission.length();icw++)
	    if(!isNewTransmission(icw))
	      cout<<"[detl:]RxAbstraction: CodeWord "<<icw<<", with BER  "<<setw(6)<<txStatusPerServiceNode(blk_cnt).nBitsInError(icw)<<" out of "<<setw(6)<<TBsize(icw)<<" bits.. \tRetransmission with RV : "<<schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.rvIndicesPerCodeword(icw)<<endl;
	    else
	      cout<<"[detl:]RxAbstraction: CodeWord "<<icw<<", with BER  "<<setw(6)<<txStatusPerServiceNode(blk_cnt).nBitsInError(icw)<<" out of "<<setw(6)<<TBsize(icw)<<" bits.. "<<endl;
	}

	//AvgSINR
	double ppSINR = inv_dB(computeAvgSINRindB(dB(ppSINRperCW),schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.mimoRate));
	
	//cout<<"Abs ppSINR : "<<dB(ppSINR)<<endl;
	
	int eNBid;
	if(isDownlink)
	  eNBid = schedulerToReceiverInfo.txNodeIDs(blk_cnt);
	else
	  eNBid = nodeID;
	MCSlogsFileName = "Results/MCSlogs/eNB_ID_"+toString(eNBid)+".txt";
	
	PrintMCSindices(schedulerToReceiverInfo.serviceNodesForSourceAndSink(blk_cnt),mcsIndices,isNewTransmission,subframeNumber,ppSINR,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).resourceBlocks.length(),txStatusPerServiceNode(blk_cnt).isSuccess,TBsize,MCSlogsFileName);
	
	if(isDownlink && myNodeDevice->L1CorePerNU(nuIndx).outerLoopLinkAdaptationInfoForDL.isEnabled)
	  myNodeDevice->L1CorePerNU(nuIndx).outerLoopLinkAdaptationInfoForDL.update(schedulerToReceiverInfo.serviceNodesForSourceAndSink(blk_cnt),txStatusPerServiceNode(blk_cnt).isSuccess,isNewTransmission,subframeNumber);
	
	else if(!isDownlink && myNodeDevice->L1CorePerNU(nuIndx).outerLoopLinkAdaptationInfoForUL.isEnabled)
	  myNodeDevice->L1CorePerNU(nuIndx).outerLoopLinkAdaptationInfoForUL.update(schedulerToReceiverInfo.serviceNodesForSourceAndSink(blk_cnt),txStatusPerServiceNode(blk_cnt).isSuccess,isNewTransmission,subframeNumber);
	

	if((schedulerToReceiverInfo.ttiCount*myNodeDevice->L1CorePerNU(nuIndx).nuParameters.ttiDurationInSec)>=mySysInfo.settlingTimeInSec)
	{
	  if(isDownlink)
	  {
	    performanceMetrics->update(schedulerToReceiverInfo.serviceNodesForSourceAndSink(blk_cnt),schedulerToReceiverInfo.rxNUID,txStatusPerServiceNode(blk_cnt).isSuccess,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).resourceBlocks,myNodeDevice->L1CorePerNU(nuIndx).nuParameters.BWPerCarrierInHz(find(myNodeDevice->L1CorePerNU(nuIndx).nuParameters.carrierFrequenciesInMHz,schedulerToReceiverInfo.carrierFreqInMHz))/myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure.getAllRuCount(),txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword,txStatusPerServiceNode(blk_cnt).nBitsInError,ppSINR,nMSE,isNewTransmission,schedulerToReceiverInfo.ttiCount*myNodeDevice->L1CorePerNU(nuIndx).nuParameters.ttiDurationInSec,myNodeDevice->L1CorePerNU(nuIndx).nuParameters.ttiDurationInSec);
	    performanceMetrics->update(schedulerToReceiverInfo.txNodeIDs(blk_cnt),schedulerToReceiverInfo.rxNUID,txStatusPerServiceNode(blk_cnt).isSuccess,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).resourceBlocks,myNodeDevice->L1CorePerNU(nuIndx).nuParameters.BWPerCarrierInHz(find(myNodeDevice->L1CorePerNU(nuIndx).nuParameters.carrierFrequenciesInMHz,schedulerToReceiverInfo.carrierFreqInMHz))/myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure.getAllRuCount(),txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword,txStatusPerServiceNode(blk_cnt).nBitsInError,ppSINR,nMSE,isNewTransmission,schedulerToReceiverInfo.ttiCount*myNodeDevice->L1CorePerNU(nuIndx).nuParameters.ttiDurationInSec,myNodeDevice->L1CorePerNU(nuIndx).nuParameters.ttiDurationInSec);
	  } 
	  else
	  {
	    performanceMetrics->update(schedulerToReceiverInfo.serviceNodesForSourceAndSink(blk_cnt),schedulerToReceiverInfo.rxNUID,txStatusPerServiceNode(blk_cnt).isSuccess,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).resourceBlocks,myNodeDevice->L1CorePerNU(nuIndx).nuParameters.BWPerCarrierInHz(find(myNodeDevice->L1CorePerNU(nuIndx).nuParameters.carrierFrequenciesInMHz,schedulerToReceiverInfo.carrierFreqInMHz))/myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure.getAllRuCount(),txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword,txStatusPerServiceNode(blk_cnt).nBitsInError,ppSINR,nMSE,isNewTransmission,schedulerToReceiverInfo.ttiCount*myNodeDevice->L1CorePerNU(nuIndx).nuParameters.ttiDurationInSec,myNodeDevice->L1CorePerNU(nuIndx).nuParameters.ttiDurationInSec);
	    performanceMetrics->update(nodeID,schedulerToReceiverInfo.rxNUID,txStatusPerServiceNode(blk_cnt).isSuccess,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).resourceBlocks,myNodeDevice->L1CorePerNU(nuIndx).nuParameters.BWPerCarrierInHz(find(myNodeDevice->L1CorePerNU(nuIndx).nuParameters.carrierFrequenciesInMHz,schedulerToReceiverInfo.carrierFreqInMHz))/myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure.getAllRuCount(),txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword,txStatusPerServiceNode(blk_cnt).nBitsInError,ppSINR,nMSE,isNewTransmission,schedulerToReceiverInfo.ttiCount*myNodeDevice->L1CorePerNU(nuIndx).nuParameters.ttiDurationInSec,myNodeDevice->L1CorePerNU(nuIndx).nuParameters.ttiDurationInSec);
	  }
	}
      }
      else
      {
	txStatusPerServiceNode(blk_cnt).serviceNodeID=schedulerToReceiverInfo.serviceNodesForSourceAndSink(blk_cnt);
	txStatusPerServiceNode(blk_cnt).isSuccess=ones_b(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.nCodeWords);
	txStatusPerServiceNode(blk_cnt).txInfoBitsPerCodeWord=schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.infoBitsperCodeWord;
	txStatusPerServiceNode(blk_cnt).rxInfoBitsPerCodeWord=schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.infoBitsperCodeWord;
	txStatusPerServiceNode(blk_cnt).nBitsInError=zeros_i(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.nCodeWords);
	txStatusPerServiceNode(blk_cnt).harqProcessID=schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID;
	txStatusPerServiceNode(blk_cnt).codeWordIndicesToConsider=schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.codeWordIndicesToConsider;
	txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword.set_size(txStatusPerServiceNode(blk_cnt).txInfoBitsPerCodeWord.length());
	for(int cwd_cnt=0;cwd_cnt<txStatusPerServiceNode(blk_cnt).txInfoBitsPerCodeWord.length();cwd_cnt++)
	  txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword(cwd_cnt)=txStatusPerServiceNode(blk_cnt).txInfoBitsPerCodeWord(cwd_cnt).length();
	
      }
    }
  }

  return txStatusPerServiceNode;
}


Array<vec> Receiver::ComputeSINROverRBs(bool isDownlink,ChannelHandler_S *myChannelHandler,int myChannelIndex,ivec interfererChannelIndices,ivec txNodesToReceiveSignalsFrom,double roiScRxPowInWatts,Array<TxFrameInfo_S> &txFrameInfo, SchedulerToReceiverInfo_S &schedulerToReceiverInfo ,ChannelCloud &myChannelCloud, DataBlkInfo_S& myDataBlkInfo ){
  
  ivec signalNodeIndices=find(txNodesToReceiveSignalsFrom,schedulerToReceiverInfo.txNodeIDs);
  ivec interfererNodeIndices=findNot(txNodesToReceiveSignalsFrom,schedulerToReceiverInfo.txNodeIDs);
  ivec txFrameInfoIndx=getFrameInfoIndx(txFrameInfo,txNodesToReceiveSignalsFrom);
  
  vec txSubcarrierPowerInDBmPerAntenna(txFrameInfo.length());
  
  for(int tx_cnt=0;tx_cnt<txFrameInfo.length();tx_cnt++)
    txSubcarrierPowerInDBmPerAntenna(tx_cnt)=txFrameInfo(tx_cnt).txSubcarrierPowerInDBmPerAntenna;
  
  Array<vec> sinrPerCodeword(myDataBlkInfo.dataGenInfo.nCodeWords);
  for(int icw=0;icw<sinrPerCodeword.length();icw++)
    sinrPerCodeword(icw).set_length(0);
  
  int nRxAntenna=myChannelHandler->channel(myChannelIndex)(0).length();
  
  L1Core_S *coreModules=&myNodeDevice->L1CorePerNU(myNodeDevice->getNUIndx(schedulerToReceiverInfo.rxNUID));
  
  for(int rb_cnt=0;rb_cnt<myDataBlkInfo.resourceBlocks.length();rb_cnt++)
  {
    imat rbStructure=myDataBlkInfo.dataGenInfo.rbInfo.logicalSubFrameStructure.get_rows(myDataBlkInfo.resourceBlocks(rb_cnt)*12,myDataBlkInfo.resourceBlocks(rb_cnt)*12+11);
    ivec positionsToConsiderperRB=find(rbStructure,0,true);
    
    Array< Array<cmat> > myChannelPerRB=extractFromRBArea(myChannelHandler->channel(myChannelIndex),&frameHandler,&coreModules->resourceMapper,myDataBlkInfo.resourceBlocks(rb_cnt),isDownlink);
    Array<cmat> signalChannel=precodeMyRBChannel(myChannelPerRB,&coreModules->portToAntennaMapper,myDataBlkInfo.resourceBlocks(rb_cnt),myDataBlkInfo,positionsToConsiderperRB);
    
    vec intrScTxPowerInWatts = inv_dB(txSubcarrierPowerInDBmPerAntenna(txFrameInfoIndx(interfererNodeIndices))-30);
    double sigpowInWatts = inv_dB(txSubcarrierPowerInDBmPerAntenna(txFrameInfoIndx(signalNodeIndices(0)))-30);
    ivec intrSFBCLayers(0);
    Array< Array<cmat> > intrfrChannel(interfererNodeIndices.length());
    for(int intr_cnt=0;intr_cnt<interfererNodeIndices.length();intr_cnt++)
    {
      ivec intrDataBlkNumbers=txFrameInfo(txFrameInfoIndx(interfererNodeIndices(intr_cnt))).frameHandler->getDataBlkNumbers(myDataBlkInfo.resourceBlocks(rb_cnt));
      Array< Array<cmat> > interferenceChannelPerRB=extractFromRBArea(myChannelHandler->channel(interfererChannelIndices(intr_cnt)),&frameHandler,&coreModules->resourceMapper,myDataBlkInfo.resourceBlocks(rb_cnt),isDownlink);
      
      if(intrDataBlkNumbers.length()==1)
      {
	DataBlkInfo_S dataBlkInfoInInterferingTx=txFrameInfo(txFrameInfoIndx(interfererNodeIndices(intr_cnt))).frameHandler->getDataBlkInfo(intrDataBlkNumbers(0));
	intrfrChannel(intr_cnt)=precodeIntrRBChannel(interferenceChannelPerRB,&coreModules->portToAntennaMapper,myDataBlkInfo.resourceBlocks(rb_cnt),dataBlkInfoInInterferingTx,positionsToConsiderperRB);
	append(intrSFBCLayers,dataBlkInfoInInterferingTx.dataGenInfo.nLayers*(dataBlkInfoInInterferingTx.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_));
      }
      else if(intrDataBlkNumbers.length()>1) // MU-MIMO
      {
	DataBlkInfo_S intrDataBlkInfo1=txFrameInfo(txFrameInfoIndx(interfererNodeIndices(intr_cnt))).frameHandler->getDataBlkInfo(intrDataBlkNumbers(0));
	for(int cnt = 1; cnt < intrDataBlkNumbers.length(); cnt++)
	{
	  DataBlkInfo_S intrDataBlkInfo2=txFrameInfo(txFrameInfoIndx(interfererNodeIndices(intr_cnt))).frameHandler->getDataBlkInfo(intrDataBlkNumbers(cnt));
	  intrDataBlkInfo1.dataGenInfo.nLayers += intrDataBlkInfo2.dataGenInfo.nLayers;
	  int rbIndx1=find(intrDataBlkInfo1.resourceBlocks,myDataBlkInfo.resourceBlocks(rb_cnt));
	  int rbIndx2=find(intrDataBlkInfo2.resourceBlocks,myDataBlkInfo.resourceBlocks(rb_cnt));
	  appendHorizontal(intrDataBlkInfo1.precoderPerRB(rbIndx1),intrDataBlkInfo2.precoderPerRB(rbIndx2));
	}

	intrfrChannel(intr_cnt)=precodeIntrRBChannel(interferenceChannelPerRB,coreModules->portToAntennaMapper ,myDataBlkInfo.resourceBlocks(rb_cnt),intrDataBlkInfo1,positionsToConsiderperRB);
	append(intrSFBCLayers,0);
      }
      else
      {
	intrfrChannel(intr_cnt).set_length(positionsToConsiderperRB.length());
	append(intrSFBCLayers,0);
	for(int re_cnt=0;re_cnt <positionsToConsiderperRB.length();re_cnt++)
	  intrfrChannel(intr_cnt)(re_cnt)=zeros_c(nRxAntenna,myDataBlkInfo.dataGenInfo.nLayers);
      }
    }
    
    ivec dbNumbers=txFrameInfo(txFrameInfoIndx(signalNodeIndices(0))).frameHandler->getDataBlkNumbers(myDataBlkInfo.resourceBlocks(rb_cnt));
    if(dbNumbers.length() > 1) // MU-MIMO
    {
      for(int co_usr_cnt = 0; co_usr_cnt < dbNumbers.length(); co_usr_cnt++)
      {
	DataBlkInfo_S coUserDatablkInfo=txFrameInfo(txFrameInfoIndx(signalNodeIndices(0))).frameHandler->getDataBlkInfo(dbNumbers(co_usr_cnt));
	if(nodeID != coUserDatablkInfo.dataGenInfo.rnti)
	{
	  Array<cmat> coUserPrecodedChannelPerRB = precodeIntrRBChannel(myChannelPerRB, &coreModules->portToAntennaMapper, myDataBlkInfo.resourceBlocks(rb_cnt), coUserDatablkInfo, positionsToConsiderperRB);
	  append(intrfrChannel,coUserPrecodedChannelPerRB);
	  append(intrScTxPowerInWatts,sigpowInWatts);
	  append(intrSFBCLayers,0);
	}
      }
    }
    
    Array<cmat> dummy;
    Array<vec> sinrPerCodewordPerRB;
    if(isDownlink)
      sinrPerCodewordPerRB=calculateSinrWithIdealChannelForAbstraction(myDataBlkInfo.dataGenInfo.transmissionScheme,myDataBlkInfo.dataGenInfo.nCodeWords,myDataBlkInfo.dataGenInfo.nLayers,receiverConfig.demodulationScheme,intrSFBCLayers,signalChannel,intrfrChannel,2*myChannelCloud.getNoiseVariancePerDimension(nodeID)+roiScRxPowInWatts, sigpowInWatts, intrScTxPowerInWatts,false,dummy);
    else
      sinrPerCodewordPerRB=calculateSinrWithIdealChannelperSubbandForUplink(myDataBlkInfo.dataGenInfo.transmissionScheme,myDataBlkInfo.dataGenInfo.nCodeWords,myDataBlkInfo.dataGenInfo.nLayers,receiverConfig.demodulationScheme,sum(myDataBlkInfo.dataGenInfo.resourceElementsPerRB),signalChannel,intrfrChannel,2*myChannelCloud.getNoiseVariancePerDimension(nodeID)+roiScRxPowInWatts, sigpowInWatts, intrScTxPowerInWatts);
    
    concatInArray(sinrPerCodeword,sinrPerCodewordPerRB);
  }
  
  return sinrPerCodeword;
  
}*/


TransmissionStatus_S SourceAndSink::getAbstractedTransmissionStatus(int serviceNodeID, DataGenInfo_S& dataGenInfo, HarqInfo& harqInfo, Array< vec >& postProcessingSinr, bvec isNewTransmission, bool isDownlink){
  
  TransmissionStatus_S txStatus;
  
  ivec nLayersPerCodeword = getNumLayersPerCodeWord(dataGenInfo.nCodeWords,dataGenInfo.mimoRate);
  ivec mcsIndices=dataGenInfo.mcsIndices;
  ivec TBsize = harqInfo.TBSizePerCodewordInBits;
  vec BLERperCodewords(mcsIndices.length());

  txStatus.serviceNodeID=serviceNodeID;
  txStatus.harqProcessID=harqInfo.harqProcessID;
  txStatus.codeWordIndicesToConsider=harqInfo.codeWordIndicesToConsider;
  txStatus.isSuccess.set_length(dataGenInfo.nCodeWords);
  txStatus.isSuccessPerCB.set_length(dataGenInfo.nCodeWords);
  txStatus.abstractionBLER.set_length(dataGenInfo.nCodeWords);
  txStatus.txInfoBitsPerCodeWord=harqInfo.infoBitsperCodeWord;//! NOTE Available only when L2 scheduler runs 
  txStatus.rxInfoBitsPerCodeWord.set_length(dataGenInfo.nCodeWords);
  txStatus.nBitsInError=zeros_i(dataGenInfo.nCodeWords);
  txStatus.isNewTransmission=harqInfo.isNewTransmission;
  
  for(int icw=0;icw<mcsIndices.length();icw++)
  {
    int modOrder = tbTables->getModOrder(mcsIndices(icw),isDownlink);
    double tCodeRate = tbTables->getCodeRate(mcsIndices(icw),isDownlink);
    
    double nCodedBits = (double)(sum(dataGenInfo.resourceElementsPerRB)*nLayersPerCodeword(icw)*modOrder);
    
    int nCRC=tbTables->getNumberOfCRCbits(TBsize(icw),tCodeRate);
    
    double effMMIB=tbTables->awgnTablesConvCoder.sinrToEffMMIB(postProcessingSinr(icw),ModulationScheme_E(modOrder));
    
    double effCodeRatewithReTx, effSINRwithReTx;
    updateHarqAbstractionModel(serviceNodeID,harqInfo.harqProcessID,isNewTransmission(icw),icw,effMMIB,modOrder,nCodedBits,TBsize(icw)+nCRC,effCodeRatewithReTx,effSINRwithReTx);

    //TBsize(icw)+24 is sufficient
     vec blerPerCB;
     BLERperCodewords(icw) = tbTables->awgnTablesConvCoder.getBlerFromMmibForAbstraction(effCodeRatewithReTx,effSINRwithReTx,ModulationScheme_E(modOrder),TBsize(icw)+24,blerPerCB,tCodeRate);
    txStatus.abstractionBLER(icw)=BLERperCodewords(icw);
    
  
    
    int nBlocks = tbTables->getNumberOfCodeBlocks(TBsize(icw),tCodeRate);
    txStatus.isSuccessPerCB(icw).set_length(nBlocks);
    
    for( int cb=0;cb< blerPerCB.size();cb++)
    { 
      if(randu()>blerPerCB(cb))
	txStatus.isSuccessPerCB(icw)(cb)= 1;
      else
	txStatus.isSuccessPerCB(icw)(cb)= 0;
    };
    
    
    
    if( txStatus.isSuccessPerCB(icw) == ones_b(blerPerCB.size()) ) // changed condition
    {
      txStatus.isSuccess(icw)=1;
      if(txStatus.txInfoBitsPerCodeWord.length())
	txStatus.rxInfoBitsPerCodeWord(icw)=txStatus.txInfoBitsPerCodeWord(icw);

      clearHarqAbstractionModel(serviceNodeID,harqInfo.harqProcessID,icw);
    }
    else
    {
      txStatus.isSuccess(icw)=0;
      txStatus.rxInfoBitsPerCodeWord(icw)=zeros_b(TBsize(icw));
      txStatus.nBitsInError(icw)=TBsize(icw)*(BLERperCodewords(icw)/2.0);
    }

    if(traceValuePerTraceName["EnablePrints"]=="true")
      cout<<"[detl:] Abstraction Rx nodeID : "<<serviceNodeID<<" effSINR : "<<effSINRwithReTx<<" BLER : "<<BLERperCodewords(icw)<<" isSuccess : "<<(int)txStatus.isSuccess(icw)<<endl;
    
  }
  txStatus.mcsRatePerCW=elem_div(to_vec(harqInfo.TBSizePerCodewordInBits),to_vec(sum(dataGenInfo.resourceElementsPerRB)*getNumLayersPerCodeWord(dataGenInfo.nCodeWords,dataGenInfo.nLayers)));
  
  txStatus.infoBitsBlockSizePerCodeword = TBsize;
  
  return txStatus;
}

TransmissionStatus_S SourceAndSink::getIdealAbstractedTransmissionStatus(int serviceNodeID, DataGenInfo_S& dataGenInfo, HarqInfo& harqInfo, Array< vec >& postProcessingSinr, bvec isNewTransmission, bool isDownlink){
  
  TransmissionStatus_S txStatus;
  
  ivec nLayersPerCodeword = getNumLayersPerCodeWord(dataGenInfo.nCodeWords,dataGenInfo.mimoRate);
  ivec mcsIndices=dataGenInfo.mcsIndices;
  ivec TBsize = harqInfo.TBSizePerCodewordInBits;
  vec BLERperCodewords(mcsIndices.length());
  double targetBLER = 0.1;
  
  txStatus.serviceNodeID=serviceNodeID;
  txStatus.harqProcessID=harqInfo.harqProcessID;
  txStatus.codeWordIndicesToConsider=harqInfo.codeWordIndicesToConsider;
  txStatus.isSuccess.set_length(dataGenInfo.nCodeWords);
  txStatus.isSuccessPerCB.set_length(dataGenInfo.nCodeWords);
  txStatus.abstractionBLER.set_length(dataGenInfo.nCodeWords);
  txStatus.txInfoBitsPerCodeWord=harqInfo.infoBitsperCodeWord;//! NOTE Available only when L2 scheduler runs 
  txStatus.rxInfoBitsPerCodeWord.set_length(dataGenInfo.nCodeWords);
  txStatus.nBitsInError=zeros_i(dataGenInfo.nCodeWords);
  txStatus.isNewTransmission=harqInfo.isNewTransmission;
  
  for(int icw=0;icw<mcsIndices.length();icw++)
  {
    int modOrder = tbTables->getModOrder(mcsIndices(icw),isDownlink);
    double tCodeRate = tbTables->getCodeRate(mcsIndices(icw),isDownlink);

    double nCodedBits = (double)(sum(dataGenInfo.resourceElementsPerRB)*nLayersPerCodeword(icw)*modOrder);
    
    int nCRC=tbTables->getNumberOfCRCbits(TBsize(icw),tCodeRate);
    
    double effMMIB=tbTables->awgnTablesConvCoder.sinrToEffMMIB(postProcessingSinr(icw),ModulationScheme_E(modOrder));
    
    double effCodeRatewithReTx, effSINRwithReTx;
    updateHarqAbstractionModel(serviceNodeID,harqInfo.harqProcessID,isNewTransmission(icw),icw,effMMIB,modOrder,nCodedBits,TBsize(icw)+nCRC,effCodeRatewithReTx,effSINRwithReTx);
    
    TBsize(icw) = tbTables->getTBSizeForSINR(postProcessingSinr(icw),dataGenInfo.scheduledPRBs,sum(dataGenInfo.resourceElementsPerRB),dataGenInfo.scheduledSymbols,nLayersPerCodeword(icw),isDownlink);

    int nTransmissions = getTransmissionCount(serviceNodeID,harqInfo.harqProcessID,icw);
    BLERperCodewords(icw) = tbTables->awgnTablesConvCoder.getBlerForIdealAbstraction(nTransmissions,targetBLER);
    txStatus.abstractionBLER(icw)=BLERperCodewords(icw);
    
    int nBlocks = tbTables->getNumberOfCodeBlocks(TBsize(icw),tCodeRate);
    if(randu()>BLERperCodewords(icw))
    {
      txStatus.isSuccess(icw)=1;
      txStatus.isSuccessPerCB(icw)= ones_b(nBlocks);
      if(txStatus.txInfoBitsPerCodeWord.length())					
	txStatus.rxInfoBitsPerCodeWord(icw)=txStatus.txInfoBitsPerCodeWord(icw);
      
      clearHarqAbstractionModel(serviceNodeID,harqInfo.harqProcessID,icw);
    }
    else
    {
      txStatus.isSuccess(icw)=0;
      txStatus.isSuccessPerCB(icw)= zeros_b(nBlocks);
      txStatus.rxInfoBitsPerCodeWord(icw)=zeros_b(TBsize(icw));
      txStatus.nBitsInError(icw)=TBsize(icw)*(BLERperCodewords(icw)/2.0);
    }
    
    if(traceValuePerTraceName["EnablePrints"]=="true")
      cout<<"[detl:] Abstraction Rx nodeID : "<<serviceNodeID<<" effSINR : "<<effSINRwithReTx<<" BLER : "<<BLERperCodewords(icw)<<" isSuccess : "<<(int)txStatus.isSuccess(icw)<<endl;
    
  }
  txStatus.mcsRatePerCW=elem_div(to_vec(harqInfo.TBSizePerCodewordInBits),to_vec(sum(dataGenInfo.resourceElementsPerRB)*getNumLayersPerCodeWord(dataGenInfo.nCodeWords,dataGenInfo.nLayers)));
  
  txStatus.infoBitsBlockSizePerCodeword = TBsize;
  
  return txStatus;
}

void SourceAndSink::updateHarqAbstractionModel(int serviceNodeID,int harqProcessID, bool isNewTransmission, int codewordCnt, double effMMIB, int modOrder, int nCodedBits, int tbSizeWithCRC, double& effCodeRatewithReTx,double& effSINRwithReTx){
  
  int indx=-1;
  for(int i=0;i<mHarqAbstractionModel(harqProcessID).length();i++)
  {
    if(mHarqAbstractionModel(harqProcessID)(i).ueID==serviceNodeID)
    {
      indx=i;
      break;
    }
  }

  if(indx!=-1)
  {
    if(isNewTransmission || (isNewTransmission && mHarqAbstractionModel(harqProcessID)(indx).TransmissionCount(codewordCnt)!=0))
    {
      mHarqAbstractionModel(harqProcessID)(indx).TransmissionCount(codewordCnt)=0;
      mHarqAbstractionModel(harqProcessID)(indx).modulationOrderPerCW(codewordCnt).set_length(0);
      mHarqAbstractionModel(harqProcessID)(indx).effMMIBperReceptionPerCW(codewordCnt).set_length(0);
      mHarqAbstractionModel(harqProcessID)(indx).nCodedBitsPerReceptionPerCW(codewordCnt).set_length(0);
    }
    
    mHarqAbstractionModel(harqProcessID)(indx).TransmissionCount(codewordCnt)+=1;
    append(mHarqAbstractionModel(harqProcessID)(indx).modulationOrderPerCW(codewordCnt),modOrder);
    append(mHarqAbstractionModel(harqProcessID)(indx).effMMIBperReceptionPerCW(codewordCnt),effMMIB);
    append(mHarqAbstractionModel(harqProcessID)(indx).nCodedBitsPerReceptionPerCW(codewordCnt),(double)nCodedBits);
  }
  else
  {
    indx=mHarqAbstractionModel(harqProcessID).length();
    sHARQAbstractionModel newEntry; 
    append(mHarqAbstractionModel(harqProcessID),newEntry);
    
    mHarqAbstractionModel(harqProcessID)(indx).ueID=serviceNodeID;
    mHarqAbstractionModel(harqProcessID)(indx).HarqProcessID=harqProcessID;
    mHarqAbstractionModel(harqProcessID)(indx).TransmissionCount(codewordCnt)+=1;
    append(mHarqAbstractionModel(harqProcessID)(indx).modulationOrderPerCW(codewordCnt),modOrder);
    append(mHarqAbstractionModel(harqProcessID)(indx).effMMIBperReceptionPerCW(codewordCnt),effMMIB);
    append(mHarqAbstractionModel(harqProcessID)(indx).nCodedBitsPerReceptionPerCW(codewordCnt),(double)nCodedBits);
  }
  
  effCodeRatewithReTx = (double)(tbSizeWithCRC)/sum(mHarqAbstractionModel(harqProcessID)(indx).nCodedBitsPerReceptionPerCW(codewordCnt));
  double effMMIBwithReTx = mean(mHarqAbstractionModel(harqProcessID)(indx).effMMIBperReceptionPerCW(codewordCnt));
  effSINRwithReTx=tbTables->awgnTablesConvCoder.mmibToSinr(ModulationScheme_E(mHarqAbstractionModel(harqProcessID)(indx).modulationOrderPerCW(codewordCnt)(0)),effMMIBwithReTx);
}

int SourceAndSink::getTransmissionCount(int serviceNodeID,int harqProcessID,int codewordCnt){
  
  int indx=-1, transmissionCount = 0;
  for(int i=0;i<mHarqAbstractionModel(harqProcessID).length();i++)
  {
    if(mHarqAbstractionModel(harqProcessID)(i).ueID==serviceNodeID)
    {
      indx=i;
      break;
    }
  }
  
  if(indx!=-1)
  {
    transmissionCount = mHarqAbstractionModel(harqProcessID)(indx).TransmissionCount(codewordCnt);
  }
  else
  {
    cout<<"[both:]serviceNodeID is not found "<<serviceNodeID<<endl;
    abort();
  }
  return transmissionCount;
}

void SourceAndSink::clearHarqAbstractionModel(int serviceNodeID,int harqProcessID, int codewordCnt){
  
  for(int i=0;i<mHarqAbstractionModel(harqProcessID).length();i++)
  {
    if(mHarqAbstractionModel(harqProcessID)(i).ueID==serviceNodeID)
    {
      mHarqAbstractionModel(harqProcessID)(i).TransmissionCount(codewordCnt)=0;
      mHarqAbstractionModel(harqProcessID)(i).modulationOrderPerCW(codewordCnt).set_length(0);
      mHarqAbstractionModel(harqProcessID)(i).effMMIBperReceptionPerCW(codewordCnt).set_length(0);
      mHarqAbstractionModel(harqProcessID)(i).nCodedBitsPerReceptionPerCW(codewordCnt).set_length(0);
      break;
    } 
  }
}


