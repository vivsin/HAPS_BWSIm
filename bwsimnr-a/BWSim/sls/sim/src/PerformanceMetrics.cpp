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

#include "../include/SupportingFunctions.h"

void PerformanceMetricsPerNU_S::init(ivec serverNodeIDs, ivec serviceNodeIDs, bool isDownlink, double TDDScaling){
  
  isDL=isDownlink;
  tddScaling=TDDScaling;
  if(isDL)
  {
    txNodes=serverNodeIDs;
    rxNodes=serviceNodeIDs;
  }
  else
  {
    txNodes=serviceNodeIDs;
    rxNodes=serverNodeIDs;
  }
  
  int txNodesLength=txNodes.length();
  int rxNodesLength=rxNodes.length();
  
  txNodesThroughput=zeros(txNodesLength)-1;
  rxNodesThroughput=zeros(rxNodesLength)-1;
  txSpectralEffeciency=zeros(txNodesLength)-1;
  rxSpectralEffeciency=zeros(rxNodesLength)-1;
  txBLER=zeros(txNodesLength)-1;
  rxBLER=zeros(rxNodesLength)-1;
  txBLERNew=zeros(txNodesLength)-1;
  rxBLERNew=zeros(rxNodesLength)-1;
  
  txBLER_RTx=zeros(txNodesLength)-1;
  rxBLER_RTx=zeros(rxNodesLength)-1;
  
  txBER=zeros(txNodesLength)-1;
  txNMSE=zeros(txNodesLength);
  txPPSINR=zeros(txNodesLength);
  txPrePSINR=zeros(txNodesLength);
  rxBER=zeros(rxNodesLength)-1;
  rxNMSE=zeros(rxNodesLength);
  rxPPSINR=zeros(rxNodesLength);
  rxPrePSINR=zeros(rxNodesLength);
  
  txPackets=zeros(txNodesLength);
  txCorrectPackets=zeros(txNodesLength);
  txNewPackets=zeros(txNodesLength);
  txNewCorrectPackets=zeros(txNodesLength);
  txBits=zeros(txNodesLength);
  txBTPs=zeros(txNodesLength);
  txCorrectBits=zeros(txNodesLength);
  txCorrectBitsBER=zeros(txNodesLength);
  txCorrectBitsBLER=zeros(txNodesLength);
  
  rxPackets=zeros(rxNodesLength);
  rxCorrectPackets=zeros(rxNodesLength);
  rxNewPackets=zeros(rxNodesLength);
  rxNewCorrectPackets=zeros(rxNodesLength);
  rxBits=zeros(rxNodesLength);
  rxBTPs=zeros(rxNodesLength);
  rxCorrectBits=zeros(rxNodesLength);
  rxCorrectBitsBER=zeros(rxNodesLength);
  rxCorrectBitsBLER=zeros(rxNodesLength);
  
  packetsWithinCQIPeriodicityPerServiceNode=zeros(serviceNodeIDs.length());
  acksWithinCQIPeriodicityPerServiceNode=zeros(serviceNodeIDs.length());
  sinroffsetPerServiceNode=zeros(serviceNodeIDs.length());
  
  WindowRxNewPacketsPerCW.set_length(rxNodesLength);
  WindowTxNewPacketsPerCW.set_length(txNodesLength);
  WindowRxCorrectNewPacketsPerCW.set_length(rxNodesLength);
  WindowTxCorrectNewPacketsPerCW.set_length(txNodesLength);
  
  for(int i=0;i<WindowRxNewPacketsPerCW.length();i++)
  {
    WindowRxNewPacketsPerCW(i)=zeros(2);
    WindowRxCorrectNewPacketsPerCW(i)=zeros(2);
  }
  
  for(int i=0;i<WindowTxNewPacketsPerCW.length();i++)
  {
    WindowTxNewPacketsPerCW(i)=zeros(2);
    WindowTxCorrectNewPacketsPerCW(i)=zeros(2);
  }
  
  isNewPrintDLeNBstats=ones_b(serverNodeIDs.length());
  isNewPrintULeNBstats=ones_b(serverNodeIDs.length());
//   NumUEsScheduledPerTTI=zeros_i(serverNodeIDs.length());
//   NumRBsScheduledPerTTI=zeros_i(serverNodeIDs.length());
  SuccessfullyRxBitsPerTTI=zeros_i(serverNodeIDs.length());
  currentTimeInSec=0.0;
  
  sumRankPerServiceNode = zeros(serviceNodeIDs.length());
  rankComputationInstant = zeros(serviceNodeIDs.length());
  
  avgCQIPerServiceNode = zeros(serviceNodeIDs.length());
  cqiComputationInstant = zeros(serviceNodeIDs.length());
  
  avgMCSPerServiceNode = zeros(serviceNodeIDs.length());
  mcsComputationInstant = zeros(serviceNodeIDs.length());
  
  startTimePerServiceNode = zeros(serviceNodeIDs.length())-1;
  endTimePerServiceNode = zeros(serviceNodeIDs.length())-1;
  
  activeTimePerServiceNode = zeros(serviceNodeIDs.length());
  nScheduledRBsPerServiceNode = zeros_i(serviceNodeIDs.length());
  
//   RBUtilizationPereNB.set_length(serverNodeIDs.length());
//   PercentageOfRBUtilization.set_length(serverNodeIDs.length());
//   for(int i=0;i<RBUtilizationPereNB.length();i++)
//   {
//     RBUtilizationPereNB(i)=zeros_i(numRBs);
//     PercentageOfRBUtilization(i)=zeros(numRBs);
//   }
}

void PerformanceMetricsPerNU_S::clear(){
  
  txNodesThroughput.clear();
  rxNodesThroughput.clear();
  txSpectralEffeciency.clear();
  rxSpectralEffeciency.clear();
  txBLER.clear();
  rxBLER.clear();
  txBLERNew.clear();
  rxBLERNew.clear();
  
  txBLER_RTx.clear();
  rxBLER_RTx.clear();
  
  txBER.clear();
  rxBER.clear();
  
  txPackets.clear();
  txCorrectPackets.clear();
  txNewPackets.clear();
  txNewCorrectPackets.clear();
  txBits.clear();
  txBTPs.clear();
  txCorrectBits.clear();
  txCorrectBitsBER.clear();
  txCorrectBitsBLER.clear();
  txNMSE.clear();
  txPPSINR.clear();
  txPrePSINR.clear();
  
  rxPackets.clear();
  rxCorrectPackets.clear();
  rxNewPackets.clear();
  rxNewCorrectPackets.clear();
  rxBits.clear();
  rxBTPs.clear();
  rxCorrectBits.clear();
  rxCorrectBitsBER.clear();
  rxCorrectBitsBLER.clear();
  rxNMSE.clear();
  rxPPSINR.clear();
  rxPrePSINR.clear();
  packetsWithinCQIPeriodicityPerServiceNode.clear();
  acksWithinCQIPeriodicityPerServiceNode.clear();
  sinroffsetPerServiceNode.clear();
  
  for(int i=0;i<WindowRxNewPacketsPerCW.length();i++)
  {
    WindowRxNewPacketsPerCW(i).clear();
    WindowRxCorrectNewPacketsPerCW(i).clear();
  }
  
  for(int i=0;i<WindowTxNewPacketsPerCW.length();i++)
  {
    WindowTxNewPacketsPerCW(i).clear();
    WindowTxCorrectNewPacketsPerCW(i).clear();
  }
  
//   NumUEsScheduledPerTTI.clear();
//   NumRBsScheduledPerTTI.clear();
  SuccessfullyRxBitsPerTTI.clear();
  currentTimeInSec = 0.0;
  
  sumRankPerServiceNode.clear();
  rankComputationInstant.clear();
  
  avgCQIPerServiceNode.clear();
  cqiComputationInstant.clear();
  
  avgMCSPerServiceNode.clear();
  mcsComputationInstant.clear();
  
  startTimePerServiceNode.clear();
  endTimePerServiceNode.clear();
  activeTimePerServiceNode.clear();
  nScheduledRBsPerServiceNode.clear();
  
//   for(int i=0;i<RBUtilizationPereNB.length();i++)
//   {
//     RBUtilizationPereNB(i).clear();
//     PercentageOfRBUtilization(i).clear();
//   }

}

// void PerformanceLog_S::printBSschedulingStats(int nodeID,int ruCount,int correctBitCount, double currentTime){
//   
//   int indx;
//   if(isDL)
//     indx=find(txNodes,nodeID);
//   else
//     indx=find(rxNodes,nodeID);
//   string filename;
//   if(isDL)
//     filename="Results/eNBstatsDL/eNB_ID_"+toString(nodeID)+".txt";
//   else
//     filename="Results/eNBstatsUL/eNB_ID_"+toString(nodeID)+".txt";
//   std::ofstream fout;
//   
//   if(isDL)
//   {
//     if(isNewPrintDLeNBstats(indx))
//     {
//       fout.open(filename.c_str(), std::ios::out);
//       fout<<"%CurrentTime,\tnScheduledUEs,\tnScheduledNumRBs,  RxBits\n";
//       isNewPrintDLeNBstats(indx)=0;
//     }
//     else
//       fout.open(filename.c_str(), std::ios::app);
//   }
//   else
//   {
//     if(isNewPrintULeNBstats(indx))
//     {
//       fout.open(filename.c_str(), std::ios::out);
//       fout<<"%CurrentTime,\tnScheduledUEs,\tnScheduledNumRBs,  RxBits\n";
//       isNewPrintULeNBstats(indx)=0;
//     }
//     else
//       fout.open(filename.c_str(), std::ios::app);
//   }
//   
//   if(currentTimeInSec(indx)!=currentTime)
//   {
//     for(int cnt=0;cnt<(subframeNumber-currentSubframe(indx));cnt++)
//     {
//       fout<<setw(10)<<currentTimeInSec(indx)+cnt<<","<<setw(19)<<NumUEsScheduledPerTTI(indx)<<","<<setw(15)<<NumRBsScheduledPerTTI(indx)<<","<<setw(15)<<SuccessfullyRxBitsPerTTI(indx)<<endl;
//       NumUEsScheduledPerTTI(indx)=0;
//       NumRBsScheduledPerTTI(indx)=0;
//       SuccessfullyRxBitsPerTTI(indx)=0;
//     }
//     currentTimeInSec(indx)=currentTime;
//     
//   }
//   
//   if(currentSubframe(indx)==subframeNumber)
//   {
//     NumUEsScheduledPerTTI(indx)+=1;
//     NumRBsScheduledPerTTI(indx)+=ruCount;
//     SuccessfullyRxBitsPerTTI(indx)+=correctBitCount;
//   }
//   
// }


void PerformanceMetricsPerNU_S::update(int nodeID, bvec isSuccess, ivec UsedRBs, double effBWPerRU, ivec bitCount, ivec errorBitCount, double prePSINR,double ppSINR, double nMSE, bvec isNewTransmission, double currentTime, double ttiDuration,string filename){
  
  int ruCount = UsedRBs.length();
  int packetCount=isSuccess.length();
  int correctBitCount=0;
  int indx=find(txNodes,nodeID);
  currentTimeInSec = currentTime;
  if(indx!=-1)
  {
    txPackets(indx)+=(double)packetCount;
    txCorrectPackets(indx)+=(double)sum(to_ivec(isSuccess));
    txBits(indx)+=(double)sum(bitCount);
    
    for(int icw=0;icw<isSuccess.length();icw++)
    {
      if(isSuccess(icw)==1)
	txCorrectBitsBLER(indx) += bitCount(icw);
    }
    
    if(!isDL)
      txBTPs(indx)+=(double)ruCount*effBWPerRU*ttiDuration;

    for(int icw=0;icw<errorBitCount.length();icw++)
    {
      if(errorBitCount(icw)==0)
      {
	txCorrectBits(indx)+=bitCount(icw);
	correctBitCount+=bitCount(icw);
      }
    }
    txCorrectBitsBER(indx)+=sum(bitCount)- sum(errorBitCount);
    txNMSE(indx)+=nMSE;
    txPPSINR(indx)+=ppSINR;
    txPrePSINR(indx)+=prePSINR;
    
    double tempNewpacketsCount=sum(to_ivec(isNewTransmission));
    ivec tempNewpackets=to_ivec(isNewTransmission);
    if(tempNewpacketsCount>0)
    {
      txNewPackets(indx)+=tempNewpacketsCount;
      if(!isDL)
	packetsWithinCQIPeriodicityPerServiceNode(indx)+=tempNewpacketsCount;
      
      if(tempNewpacketsCount==1)
      {
	txNewCorrectPackets(indx)+=(int)isSuccess(find(tempNewpackets,1));
	if(!isDL)
	  acksWithinCQIPeriodicityPerServiceNode(indx)+=(int)isSuccess(find(tempNewpackets,1));
      }
      else
      {
	txNewCorrectPackets(indx)+=sum(to_ivec(isSuccess));
	if(!isDL)
	  acksWithinCQIPeriodicityPerServiceNode(indx)+=sum(to_ivec(isSuccess));
      }
    }
    
    
    
    for(int icw=0;icw<tempNewpackets.length();icw++)
    {
      WindowTxNewPacketsPerCW(indx)(icw)+=(double)tempNewpackets(icw);
      if(tempNewpackets(icw) == 1 && isSuccess(icw) == 1)
	WindowTxCorrectNewPacketsPerCW(indx)(icw)+=1.0;

      //Printing WindowBLERstats
      if(!isDL && WindowTxNewPacketsPerCW(indx)(icw)==100)
      {
	printWindowBLERStats(nodeID,WindowTxCorrectNewPacketsPerCW(indx)(icw),currentTime,icw,filename);
	WindowTxNewPacketsPerCW(indx)(icw)=0;
	WindowTxCorrectNewPacketsPerCW(indx)(icw)=0;
      }
    }
    
    //Printing BSScheduling stats per TTI basis
//     if(isDL)
//     {
//       printBSschedulingStats(nodeID,ruCount,correctBitCount,subframeNumber);
//       for(int i=0;i<UsedRBs.length();i++)
// 	RBUtilizationPereNB(indx)(UsedRBs(i))+=1;
//     }
    
    if(!isDL)
    {
      if(startTimePerServiceNode(indx) == -1)
      {
        startTimePerServiceNode(indx) = currentTime;
      }
      endTimePerServiceNode(indx) = currentTime+ttiDuration;
      
      if(sum(bitCount)>0)   // If data is received in that TTI
      {
          activeTimePerServiceNode(indx) += ttiDuration;
      }
      nScheduledRBsPerServiceNode(indx) += UsedRBs.length();
    }
  
  }
  else
  {
    indx=find(rxNodes,nodeID);
    if(indx!=-1)
    {
      rxPackets(indx)+=(double)packetCount;
      rxCorrectPackets(indx)+=(double)sum(to_ivec(isSuccess));
      rxBits(indx)+=(double)sum(bitCount);
      
      for(int icw=0;icw<isSuccess.length();icw++)
      {
	if(isSuccess(icw)==1)
	  rxCorrectBitsBLER(indx) += bitCount(icw);
      }
      
      if(isDL)
	rxBTPs(indx)+=(double)ruCount*effBWPerRU*ttiDuration;
      
      for(int icw=0;icw<errorBitCount.length();icw++)
      {
	if(errorBitCount(icw)==0)
	{
	  rxCorrectBits(indx)+=bitCount(icw);
	  correctBitCount+=bitCount(icw);
	}
      }
      rxCorrectBitsBER(indx)+=sum(bitCount)- sum(errorBitCount);
      rxNMSE(indx)+=nMSE;
      rxPPSINR(indx)+=ppSINR;
      rxPrePSINR(indx)+=prePSINR;
      
      double tempNewpacketsCount=sum(to_ivec(isNewTransmission));
      ivec tempNewpackets=to_ivec(isNewTransmission);
      if(tempNewpacketsCount>0)
      {
	rxNewPackets(indx)+=tempNewpacketsCount;
	if(isDL)
	  packetsWithinCQIPeriodicityPerServiceNode(indx)+=tempNewpacketsCount;
	
	if(tempNewpacketsCount==1)
	{
	  rxNewCorrectPackets(indx)+=(int)isSuccess(find(tempNewpackets,1));
	  if(isDL)
	    acksWithinCQIPeriodicityPerServiceNode(indx)+=(int)isSuccess(find(tempNewpackets,1));
	}
	else
	{
	  rxNewCorrectPackets(indx)+=sum(to_ivec(isSuccess));
	  if(isDL)
	    acksWithinCQIPeriodicityPerServiceNode(indx)+=sum(to_ivec(isSuccess));
	}
      }
      
      
      for(int icw=0;icw<tempNewpackets.length();icw++)
      {
	WindowRxNewPacketsPerCW(indx)(icw)+=(double)tempNewpackets(icw);
	if(tempNewpackets(icw) == 1 && isSuccess(icw) == 1)
	  WindowRxCorrectNewPacketsPerCW(indx)(icw)+=1.0;

	//Printing WindowBLERstats
	if(isDL && WindowRxNewPacketsPerCW(indx)(icw)==100)
	{
	  printWindowBLERStats(nodeID,WindowRxCorrectNewPacketsPerCW(indx)(icw),currentTime,icw,filename);
	  WindowRxNewPacketsPerCW(indx)(icw)=0;
	  WindowRxCorrectNewPacketsPerCW(indx)(icw)=0;
	}
      }
      
//       if(!isDL)
//       {
// 	printBSschedulingStats(nodeID,ruCount,correctBitCount,currentTime);
// 	for(int i=0;i<UsedRBs.length();i++)
// 	  RBUtilizationPereNB(indx)(UsedRBs(i))+=1;
//       }
//       
      if(isDL)
      {
        if(startTimePerServiceNode(indx) == -1)
            startTimePerServiceNode(indx) = currentTime;
        endTimePerServiceNode(indx) = currentTime+ttiDuration;
    
        if(sum(bitCount)>0)   // If data is received in that TTI
        {
            activeTimePerServiceNode(indx) += ttiDuration;
        }
        nScheduledRBsPerServiceNode(indx) += UsedRBs.length();
      }
    }
    else
    {
      cout<<"[both:] Unknown nodeID to update() in PerformanceMetrics_S..."<<endl;
      abort();
    }
  }
}

bool PerformanceMetricsPerNU_S::computeMetric(double currentTime, CarrierInfo_S carrierInfo){
  
    bool printOutput=false;
    currentTimeInSec = currentTime;
    if(sum(txPackets)+sum(rxPackets)==0)
    {
        return false;
    }
    if(nuParams.nuID!=-1)
    {
        printOutput = true;
        for(int tx_cnt=0;tx_cnt<txNodes.length();tx_cnt++)
        {
            if(txPackets(tx_cnt)!=0)
            {
                txBLER(tx_cnt)=1.0-(txCorrectPackets(tx_cnt)/txPackets(tx_cnt));
                if(txNewPackets(tx_cnt)>0)
                    txBLERNew(tx_cnt)=1.0-(txNewCorrectPackets(tx_cnt)/txNewPackets(tx_cnt));
                if((txPackets(tx_cnt)-txNewPackets(tx_cnt))>0)
                    txBLER_RTx(tx_cnt)=1.0-((txCorrectPackets(tx_cnt)-txNewCorrectPackets(tx_cnt))/(txPackets(tx_cnt)-txNewPackets(tx_cnt)));
            }
            if(txBits(tx_cnt)!=0)
            {
                txBER(tx_cnt)=1.0-(txCorrectBitsBER(tx_cnt)/(txBits(tx_cnt)));
                txNodesThroughput(tx_cnt)=txCorrectBits(tx_cnt)/currentTime;

                if(isDL)
                {
                    double supportedBWInHz=carrierInfo.carrierBandwidthInMHz*1e6;
                    txSpectralEffeciency(tx_cnt)=txNodesThroughput(tx_cnt)/(supportedBWInHz)*tddScaling;
                }
                else
                    txSpectralEffeciency(tx_cnt)=txCorrectBits(tx_cnt)/(txBTPs(tx_cnt));
            }
            else
            {
                txNodesThroughput(tx_cnt)=0;
                txSpectralEffeciency(tx_cnt)=0;
            }
        }
        for(int rx_cnt=0;rx_cnt<rxNodes.length();rx_cnt++)
        {
            if(rxPackets(rx_cnt)!=0)
            {
                rxBLER(rx_cnt)=1.0-(rxCorrectPackets(rx_cnt)/rxPackets(rx_cnt));
                if(rxNewPackets(rx_cnt)>0)
                    rxBLERNew(rx_cnt)=1.0-(rxNewCorrectPackets(rx_cnt)/rxNewPackets(rx_cnt));
                if((rxPackets(rx_cnt)-rxNewPackets(rx_cnt))>0)
                    rxBLER_RTx(rx_cnt)=1.0-((rxCorrectPackets(rx_cnt)-rxNewCorrectPackets(rx_cnt))/(rxPackets(rx_cnt)-rxNewPackets(rx_cnt)));
            }
            if(rxBits(rx_cnt)!=0)
            {
                rxBER(rx_cnt)=1.0-(rxCorrectBitsBER(rx_cnt)/(rxBits(rx_cnt)));
                rxNodesThroughput(rx_cnt)=rxCorrectBits(rx_cnt)/currentTime;

                if(!isDL)
                {
                    double supportedBWInHz=carrierInfo.carrierBandwidthInMHz*1e6;
                    rxSpectralEffeciency(rx_cnt)=rxNodesThroughput(rx_cnt)/(supportedBWInHz)*tddScaling;
                }
                else
                    rxSpectralEffeciency(rx_cnt)=rxCorrectBits(rx_cnt)/(rxBTPs(rx_cnt));
            }
            else
            {
                rxNodesThroughput(rx_cnt)=0;
                rxSpectralEffeciency(rx_cnt)=0;
            }
        }
    }
  return printOutput;
}

void PerformanceMetricsPerNU_S::printBERPerformanceInfo(string filename, double snrIndB){
  
  std::ofstream fout;
  static bool isNewPrint1=1;
  if(isNewPrint1)
  {
    fout.open(filename.c_str(), std::ios::out);
    fout<<"%ServiceNodeID,\tSNR(dB),\t\tBER,\t\tBLER,\t\tBLERNewTx,\tThroughput(bps),\tSE(bps/Hz),\tAvgPPSINR(dB),\tAvgPrePSINR(dB)\n";
    isNewPrint1=false;
  }
  else
    fout.open(filename.c_str(), std::ios::app);
  
  if(isDL)
    for(int rx_cnt=0;rx_cnt<rxNodes.length();rx_cnt++)
    {
      if(rxPackets(rx_cnt)!=0)
	fout<<rxNodes(rx_cnt)<<",\t"<<setw(7)<<snrIndB<<",\t"<<setw(11)<<rxBER(rx_cnt)<<",\t"<<setw(12)<<rxBLER(rx_cnt)<<",\t"<<setw(17)<<rxBLERNew(rx_cnt)<<",\t"<<setw(15)<<rxNodesThroughput(rx_cnt)<<",\t"<<setw(10)<<rxSpectralEffeciency(rx_cnt)<<",\t"<<setw(10)<<dB(rxPPSINR(rx_cnt)/rxPackets(rx_cnt))<<",\t"<<setw(10)<<dB(rxPrePSINR(rx_cnt)/rxPackets(rx_cnt))<<endl;
    }
    
    else
      for(int tx_cnt=0;tx_cnt<txNodes.length();tx_cnt++)
      {
	if(txPackets(tx_cnt)!=0)
	  fout<<txNodes(tx_cnt)<<",\t\t"<<setw(7)<<snrIndB<<",\t"<<setw(11)<<txBER(tx_cnt)<<",\t"<<setw(12)<<txBLER(tx_cnt)<<",\t"<<setw(17)<<txBLERNew(tx_cnt)<<",\t"<<setw(15)<<txNodesThroughput(tx_cnt)<<",\t"<<setw(10)<<txSpectralEffeciency(tx_cnt)<<",\t"<<setw(10)<<dB(txPPSINR(tx_cnt)/txPackets(tx_cnt))<<",\t"<<setw(10)<<dB(txPrePSINR(tx_cnt)/txPackets(tx_cnt))<<endl;
      }
      fout.close();
}

// void PerformanceMetricsPerNU_S::printNMSEperformanceInfo(string filename, double snrIndB){
//   
//   std::ofstream fout;
//   static bool isNewPrint1=1;
//   if(isNewPrint1)
//   {
//     fout.open(filename.c_str(), std::ios::out);
//     fout<<"ServiceNodeID,\tSNR(dB),\t\tnMSE\n";
//     isNewPrint1=false;
//   }
//   else
//     fout.open(filename.c_str(), std::ios::app);
//   
//   if(isDL)
//     for(int rx_cnt=0;rx_cnt<rxNodes.length();rx_cnt++)
//       fout<<rxNodes(rx_cnt)<<",\t\t"<<snrIndB<<",\t\t"<<rxNMSE(rx_cnt)/rxRUs(rx_cnt)<<endl;
//     else
//       for(int tx_cnt=0;tx_cnt<txNodes.length();tx_cnt++)
// 	fout<<txNodes(tx_cnt)<<",\t\t"<<snrIndB<<",\t\t"<<txNMSE(tx_cnt)/txRUs(tx_cnt)<<endl;
//       
//       fout.close();
// }

// void PerformanceMetricsPerNU_S::printSystemPerformanceInfo(McellSystem &mySys,string filename1,string filename2,AssociationInfo_S &associationInfo,int tNUID){
//     
//     Array<string> nodeTypes=mySys.getNodeTypes();
//     #ifdef USING_MPI
//     if(nodeTypes.length()>10)
//         cout<<"[both:] Error with MPI : nodeTypes.length() cannot be more than 10 "<<endl;
//     #endif
//     ivec nodeCounts=mySys.getNodeCount(), temp(0);
//     Array<NodeCategory_E> nodeCategories(nodeTypes.length());
//     for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
//         if(nodeCounts(nodeType_cnt)>0)
//             nodeCategories(nodeType_cnt)=mySys.getNodeCategory(nodeTypes(nodeType_cnt));
//         else
//             nodeCategories(nodeType_cnt)=_NODECATEGORY_UNINIT_;
//         
//         std::ofstream fout(filename1.c_str(), std::ios::out);
//     
//     #ifdef USING_MPI
//     if(currentTaskid==0)
//         #endif
//         fout<<"%"<<setw(12)<<"ServiceNodeID"<<setw(12)<<"ServerNodeID"<<setw(13)<<"BLER"<<setw(16)<<"BLERNewTx"<<setw(16)<<"BTP"<<setw(24)<<"Throughput(bps)"<<setw(17)<<"SE(bps/Hz)"<<setw(20)<<"AvgPPSINR(dB)"<<setw(14)<<"BLER_ReTx"<<setw(14)<<"PrePSINR (dB)"<<setw(14)<<"AvgRank"<<setw(14)<<setw(14)<<"Start Time"<<setw(14)<<"End Time"<<"UPT (bps)"<<"\nUEPerf=\"...\n";
//     
//     if(isDL) 
//     {
//         for(int rx_cnt=0;rx_cnt<rxNodes.length();rx_cnt++)
//         {
//             if(rxBits(rx_cnt)!=0)
//             {
//                 double avgRank=1.0;
//                 if(rankComputationInstant(rx_cnt)!=0)
//                     avgRank=sumRankPerServiceNode(rx_cnt)/rankComputationInstant(rx_cnt);
//                 
//                 double userPacketThroughput = 0.0;
//                 double activeTime = endTimePerServiceNode(rx_cnt) - startTimePerServiceNode(rx_cnt);
//                 if(activeTime != 0)
//                     userPacketThroughput = rxCorrectBitsBLER(rx_cnt)/activeTime;
//                 
//                 
//                 fout<<setw(12)<<rxNodes(rx_cnt)<<setw(12)<<associationInfo.associatedServerNodesForEachServiceNode(rx_cnt)<<setw(12)<<rxBLER(rx_cnt)<<setw(16)<<rxBLERNew(rx_cnt)<<setw(16)<<rxBTPs(rx_cnt)<<setw(24)<<rxNodesThroughput(rx_cnt)<<setw(20)<<rxSpectralEffeciency(rx_cnt)<<setw(16)<<dB(rxPPSINR(rx_cnt)/rxPackets(rx_cnt))<<setw(16)<<rxBLER_RTx(rx_cnt)<<setw(16)<<dB(rxPrePSINR(rx_cnt)/rxPackets(rx_cnt))<<setw(16)<<avgRank<<setw(16)<<startTimePerServiceNode(rx_cnt)<<setw(16)<<endTimePerServiceNode(rx_cnt)<<setw(16)<<userPacketThroughput<<";..."<<endl;
//             }
//         }
//     }
//     else
//     {
//         for(int tx_cnt=0;tx_cnt<txNodes.length();tx_cnt++)
//         {
//             if(txBits(tx_cnt)!=0)
//             {
//                 double avgRank=1.0;
//                 if(rankComputationInstant(tx_cnt)!=0)
//                     avgRank=sumRankPerServiceNode(tx_cnt)/rankComputationInstant(tx_cnt);
//                 
//                 double userPacketThroughput = 0.0;
//                 double activeTime = endTimePerServiceNode(tx_cnt) - startTimePerServiceNode(tx_cnt);
//                 if(activeTime != 0)
//                     userPacketThroughput = txCorrectBitsBLER(tx_cnt)/activeTime;
//                 fout<<setw(12)<<txNodes(tx_cnt)<<setw(12)<<associationInfo.associatedServerNodesForEachServiceNode(tx_cnt)<<setw(12)<<txBLER(tx_cnt)<<setw(16)<<txBLERNew(tx_cnt)<<setw(16)<<txBTPs(tx_cnt)<<setw(24)<<txNodesThroughput(tx_cnt)<<setw(20)<<txSpectralEffeciency(tx_cnt)<<setw(16)<<dB(txPPSINR(tx_cnt)/txPackets(tx_cnt))<<setw(16)<<txBLER_RTx(tx_cnt)<<setw(16)<<dB(txPrePSINR(tx_cnt)/txPackets(tx_cnt))<<setw(16)<<avgRank<<setw(16)<<startTimePerServiceNode(tx_cnt)<<setw(16)<<endTimePerServiceNode(tx_cnt)<<setw(16)<<userPacketThroughput<<";..."<<endl;
//             }
//         }
//     }
//     fout.close();
//     
//     ivec nodesPerType, nodeIndices;
//     vec avgThroughputPerServerNodeType=zeros(nodeTypes.length())-1;
//     vec avgSEPerServerNodeType=zeros(nodeTypes.length())-1;
//     vec avgThroughputPerServiceNodeUnderServerNodeType=zeros(nodeTypes.length())-1;
//     vec avgSEPerServiceNodeUnderServerNodeType=zeros(nodeTypes.length())-1;
//     
//     
//     ivec numberofServerNodes(nodeTypes.length());
//     std::ofstream fout1(filename2.c_str(), std::ios::out);
//     
//     for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
//     {
//         if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
//         {
//             #ifdef USING_MPI
//             if(currentTaskid==0) {
//                 #endif
//                 fout1<<"%#################################################################\n";
//                 fout1<<"%\t"<<nodeTypes(nodeType_cnt)<<" Details:";
//                 fout1<<"\n%#################################################################\n";
//                 fout1<<"\n%BS_ID,\t\tThroughput Per "<<nodeTypes(nodeType_cnt)<<" (bps),\tSE per "<<nodeTypes(nodeType_cnt)<<" (bps/Hz)\n";
//                 #ifdef USING_MPI
//             }
//             #endif
//             nodesPerType=mySys.getActiveNodes(nodeTypes(nodeType_cnt));
//             
//             if(isDL)
//             {
//                 nodeIndices=find(txNodes,nodesPerType);
//                 for(int node_type=0;node_type<nodesPerType.length();node_type++) {
//                     if(txNodesThroughput(nodeIndices(node_type))!=0)
//                         fout1<<setw(6)<<txNodes(nodeIndices(node_type))<<",\t\t"<< setw(16)<<txNodesThroughput(nodeIndices(node_type))<<",\t\t"<< setw(12)<<txSpectralEffeciency(nodeIndices(node_type))<<endl;
//                 }
//                 numberofServerNodes(nodeType_cnt)=findNot(ceil_i(txNodesThroughput(nodeIndices)),0).length();
//                 avgThroughputPerServerNodeType(nodeType_cnt) = sum(txNodesThroughput(nodeIndices))/numberofServerNodes(nodeType_cnt);
//                 avgSEPerServerNodeType(nodeType_cnt)= sum(txSpectralEffeciency(nodeIndices))/numberofServerNodes(nodeType_cnt);
//                 avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt)=sum(txNodesThroughput(nodeIndices));
//             }
//             else
//             {
//                 nodeIndices=find(rxNodes,nodesPerType);
//                 for(int node_type=0;node_type<nodesPerType.length();node_type++) {
//                     if(rxNodesThroughput(nodeIndices(node_type))!=0)
//                         fout1<<setw(6)<<rxNodes(nodeIndices(node_type))<<",\t\t"<< setw(16)<<rxNodesThroughput(nodeIndices(node_type))<<",\t\t"<< setw(12)<<rxSpectralEffeciency(nodeIndices(node_type))<<endl;
//                 }
//                 numberofServerNodes(nodeType_cnt)=findNot(ceil_i(rxNodesThroughput(nodeIndices)),0).length();
//                 avgThroughputPerServerNodeType(nodeType_cnt) = sum(rxNodesThroughput(nodeIndices))/numberofServerNodes(nodeType_cnt);
//                 avgSEPerServerNodeType(nodeType_cnt)= sum(rxSpectralEffeciency(nodeIndices))/numberofServerNodes(nodeType_cnt);
//                 avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt)=sum(rxNodesThroughput(nodeIndices));
//             }
//             
//             #ifndef USING_MPI
//             double srvcNodeUnder=0;
//             for(int node_cnt=0;node_cnt<nodeIndices.length();node_cnt++)
//                 srvcNodeUnder+=(double)associationInfo.associatedServiceNodesPerServerNode(nodeIndices(node_cnt)).length();
//             avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt)/=srvcNodeUnder;
//             #endif
//         }
//     }
//     
//     #ifdef USING_MPI
//     fout1.close();
//     
//     MPI_Barrier(MPI_COMM_WORLD);
//     MPIResultInfo.init(nTasks);
//     
//     for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
//     {
//         if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
//         {
//             MPIResultInfo.avgResults[currentTaskid].avgSEPerServerNodeType[nodeType_cnt]=avgSEPerServerNodeType(nodeType_cnt);
//             MPIResultInfo.avgResults[currentTaskid].avgThroughputPerServerNodeType[nodeType_cnt]=avgThroughputPerServerNodeType(nodeType_cnt);
//             MPIResultInfo.avgResults[currentTaskid].numberofServerNodes[nodeType_cnt]=numberofServerNodes(nodeType_cnt);
//             MPIResultInfo.avgResults[currentTaskid].avgThroughputPerServiceNodeUnderServerNodeType[nodeType_cnt]=avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt);
//         }
//     }
//     
//     int offset=0;
//     for(int cu_cnt=0;cu_cnt<nTasks;cu_cnt++)
//     {
//         MPIResultInfo.countsPerCU(cu_cnt)=1;
//         MPIResultInfo.offsetsPerCU(cu_cnt)=offset;
//         offset+=MPIResultInfo.countsPerCU(cu_cnt);
//     }
//     
//     offset=1;
//     MPIResultInfo.countsPerTask[0]=0;MPIResultInfo.offsetsPerTask[0]=0;
//     for(int task=1;task<nTasks;task++)
//     {
//         MPIResultInfo.countsPerTask[task]=sizeof(AvgResults_S);
//         MPIResultInfo.offsetsPerTask[task]=offset*sizeof(AvgResults_S);
//         offset+=1;
//     }
//     
//     MPI_Gatherv(MPIResultInfo.avgResults+MPIResultInfo.offsetsPerCU[currentTaskid],MPIResultInfo.countsPerTask[currentTaskid],MPI_BYTE,MPIResultInfo.avgResults,MPIResultInfo.countsPerTask,MPIResultInfo.offsetsPerTask,MPI_BYTE,0,MPI_COMM_WORLD);
//     MPI_Barrier(MPI_COMM_WORLD);
//     if(currentTaskid==0) {
//         if(isDL)
//         {
//             string input1="cat Results/NU_"+toString(tNUID)+"/dlSystemPerformanceInfo.txt.temp*  > Results/NU_"+toString(tNUID)+"/dlSystemPerformanceInfo.txt.tt1";
//             string input2="rm  Results/NU_"+toString(tNUID)+"/dlSystemPerformanceInfo.txt.temp*";
//             system_exec(input1.c_str());
//             system_exec(input2.c_str());
//         }
//         else
//         {
//             string input1="cat Results/NU_"+toString(tNUID)+"/ulSystemPerformanceInfo.txt.temp*  > Results/NU_"+toString(tNUID)+"/ulSystemPerformanceInfo.txt.tt1";
//             string input2="rm  Results/NU_"+toString(tNUID)+"/ulSystemPerformanceInfo.txt.temp*";
//             system_exec(input1.c_str());
//             system_exec(input2.c_str());
//         }
//         vec avgSEPerServerNodeType_temp=zeros(nTasks),avgThroughputPerServerNodeType_temp=zeros(nTasks),avgThroughputPerServiceNodeUnderServerNodeType_temp=zeros(nTasks);
//         ivec numberofServerNodes_temp=zeros_i(nTasks);
//         for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
//         {
//             if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
//             {
//                 nodesPerType=mySys.getActiveNodes(nodeTypes(nodeType_cnt));
//                 for(int task=0;task<nTasks;task++)
//                 {
//                     numberofServerNodes_temp(task)=MPIResultInfo.avgResults[task].numberofServerNodes[nodeType_cnt];
//                     if(MPIResultInfo.avgResults[task].avgSEPerServerNodeType[nodeType_cnt]!=MPIResultInfo.avgResults[task].avgSEPerServerNodeType[nodeType_cnt])
//                         avgSEPerServerNodeType_temp(task) = 0;
//                     else
//                         avgSEPerServerNodeType_temp(task)=(MPIResultInfo.avgResults[task].avgSEPerServerNodeType[nodeType_cnt])*numberofServerNodes_temp(task);
//                     if(MPIResultInfo.avgResults[task].avgThroughputPerServerNodeType[nodeType_cnt]!=MPIResultInfo.avgResults[task].avgThroughputPerServerNodeType[nodeType_cnt])
//                         avgThroughputPerServerNodeType_temp(task) = 0;
//                     else
//                         avgThroughputPerServerNodeType_temp(task)=MPIResultInfo.avgResults[task].avgThroughputPerServerNodeType[nodeType_cnt]*numberofServerNodes_temp(task);
//                     avgThroughputPerServiceNodeUnderServerNodeType_temp(task) = MPIResultInfo.avgResults[task].avgThroughputPerServiceNodeUnderServerNodeType[nodeType_cnt];
//                     
//                 }
//                 if(isDL)
//                     nodeIndices=find(txNodes,nodesPerType);
//                 else
//                     nodeIndices=find(rxNodes,nodesPerType);
//                 int srvcNodeUnder=0;
//                 for(int node_cnt=0;node_cnt<nodeIndices.length();node_cnt++)
//                     srvcNodeUnder+=associationInfo.associatedServiceNodesPerServerNode(nodeIndices(node_cnt)).length();
//                 
//                 avgSEPerServerNodeType(nodeType_cnt)=sum(avgSEPerServerNodeType_temp)/sum(numberofServerNodes_temp);
//                 avgThroughputPerServerNodeType(nodeType_cnt)=sum(avgThroughputPerServerNodeType_temp)/sum(numberofServerNodes_temp);
//                 avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt)=sum(avgThroughputPerServiceNodeUnderServerNodeType_temp)/srvcNodeUnder;
//             }
//         }
//         
//         if(isDL)
//             filename2 = "./Results/NU_"+toString(tNUID)+ "/dlSystemPerformanceInfo.txt.tt2";
//         else
//             filename2 = "./Results/NU_"+toString(tNUID)+ "/ulSystemPerformanceInfo.txt.tt2";
//         std::ofstream fout1(filename2.c_str(), std::ios::out);
//         #endif
//         fout1<<"\n\n%#################################################################\n%\tPerformance Metrics:\n%#################################################################\n"<<"\n%  Throughput:\n%  ~~~~~~~~~~~\n"<<endl;
//         for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
//             if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
//                 fout1<<"\tAverage UE Throughput Per "<<nodeTypes(nodeType_cnt)<<" (bps)\t= "<<avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt)<<"\n\n";
//             for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
//                 if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
//                     fout1<<"\tAverage Throughput Per "<<nodeTypes(nodeType_cnt)<<" (bps)\t\t= "<<avgThroughputPerServerNodeType(nodeType_cnt)<<"\n\n";
//                 fout1<<"\n\n%  Spectral Efficiency:\n%  ~~~~~~~~~~~~~~~~~~~~\n"<<endl;
//             
//             for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
//                 if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
//                     fout1<<"\tAverge SE per "<<nodeTypes(nodeType_cnt)<<" (bps/Hz)\t\t= "<<avgSEPerServerNodeType(nodeType_cnt)<<"\n";
//                 
//                 fout1<<"\n%#################################################################\n"<<endl;
//             fout1.close();
//             #ifdef USING_MPI
//     }
//     MPI_Barrier(MPI_COMM_WORLD);
//     if(currentTaskid==0){
//         if(isDL)
//         {
//             string input1="cat Results/NU_"+toString(tNUID)+"/dlUEPerformanceInfo.txt.temp*  > Results/NU_"+toString(tNUID)+"/dlUEPerformanceInfo.txt";
//             string input2="rm  Results/NU_"+toString(tNUID)+"/dlUEPerformanceInfo.txt.temp*";
//             string input3="cat Results/NU_"+toString(tNUID)+"/dlSystemPerformanceInfo.txt.tt*  > Results/NU_"+toString(tNUID)+"/dlSystemPerformanceInfo.txt";
//             string input4="rm  Results/NU_"+toString(tNUID)+"/dlSystemPerformanceInfo.txt.tt*";
//             
//             system_exec(input1.c_str());
//             system_exec(input2.c_str());
//             system_exec(input3.c_str());
//             system_exec(input4.c_str());
//         }
//         else
//         {	
//             string input1="cat Results/NU_"+toString(tNUID)+"/ulUEPerformanceInfo.txt.temp*  > Results/NU_"+toString(tNUID)+"/ulUEPerformanceInfo.txt";
//             string input2="rm  Results/NU_"+toString(tNUID)+"/ulUEPerformanceInfo.txt.temp*";
//             string input3="cat Results/NU_"+toString(tNUID)+"/ulSystemPerformanceInfo.txt.tt*  > Results/NU_"+toString(tNUID)+"/ulSystemPerformanceInfo.txt";
//             string input4="rm  Results/NU_"+toString(tNUID)+"/ulSystemPerformanceInfo.txt.tt*";
//             
//             system_exec(input1.c_str());
//             system_exec(input2.c_str());
//             system_exec(input3.c_str());
//             system_exec(input4.c_str());
//         }
//         #endif
//         vec preprocessingSINRData;
//         ivec serviceNodes;
//         if(!parse("./Results/SINRDistribution.txtTemp","preprocessingSINRData",preprocessingSINRData))
//             cout<<"Unable to Parse preprocessingSINRData..."<<endl;
//         if(!parse("./Results/SINRDistribution.txtTemp","serviceNodes",serviceNodes))
//             cout<<"Unable to Parse serviceNodes..."<<endl;
//         
//         if(isDL)
//             filename1= "./Results/NU_"+toString(tNUID)+ "/dlUEPerformanceInfo.txt";
//         else
//             filename1= "./Results/NU_"+toString(tNUID)+ "/ulUEPerformanceInfo.txt";
//         
//         std::ofstream fout3;
//         fout3.open(filename1.c_str(), std::ios::app);
//         fout3<<"\"";
//         fout3.close();
//         
//         cout<<"currentTaskid : "<<currentTaskid<<endl;
//         
//         mat UEPerf;
//         if(parse(filename1,"UEPerf",UEPerf)==false)
//             cout<<"Unable to Parse UEPerf..."<<endl;
//         
//         if(UEPerf.size()!=0){
//             vec throughput=UEPerf.get_col(5);
//             ivec throughputsortedindx=sort_index(throughput);
//             mat UEPerformance;
//             UEPerformance.set_size(UEPerf.rows(),UEPerf.cols()+1);
//             for(int row_indx=0;row_indx<UEPerformance.rows();row_indx++){
//                 for(int col_indx=0;col_indx<UEPerformance.cols();col_indx++){
//                     if(col_indx!=UEPerformance.cols()-1)
//                         UEPerformance(row_indx,col_indx)=UEPerf(throughputsortedindx(row_indx),col_indx);
//                     else
//                         UEPerformance(row_indx,col_indx)=preprocessingSINRData(find(serviceNodes,(int)UEPerformance(row_indx,0)));
//                 }
//             }
//             
//             if(isDL)
//             {
//                 string input6="rm  Results/NU_"+toString(tNUID)+"/dlUEPerformanceInfo.txt";
//                 system_exec(input6.c_str());
//             }
//             else
//             {
//                 string input6="rm  Results/NU_"+toString(tNUID)+"/ulUEPerformanceInfo.txt";
//                 system_exec(input6.c_str());
//             }
//             
//             string dlSysPerf="Results/NU_"+toString(tNUID)+"/dlSystemPerformanceInfo.txt";
//             string ulSysPerf="Results/NU_"+toString(tNUID)+"/ulSystemPerformanceInfo.txt";
//             
//             double cellEdgeThroughput=UEPerformance.get_col(5)(ceil_i(0.05*UEPerformance.rows()));
//             double AvgThroughput=mean(UEPerformance.get_col(5));
//             
//             std::ofstream UEperf(filename1.c_str(), std::ios::out);
//             #ifdef USING_CLUSTER
//             UEperf<<"%"<<setw(6)<<"UE-ID,"<<setw(10)<<"eNB-ID,"<<setw(12)<<"BLER,"<<setw(12)<<"RBcount,"<<setw(20)<<"Throughput [bps],"<<setw(16)<<"SE [bps/Hz],"<<setw(14)<<"BLERNewTx,"<<setw(14)<<"BLERReTx,"<<setw(18)<<"PostPSINR [dB],"<<setw(17)<<"GeometrySINR [dB],"<<setw(17)<<"PrePSINR [dB],"<<setw(17)<<"Avg Rank,"<<setw(17)<<"UPT (bps)\n";
//             #else
//             UEperf<<"%"<<setw(6)<<"ServiceNodeID,"<<setw(10)<<"ServerNodeID,"<<setw(12)<<"BLER,"<<setw(12)<<"RBcount,"<<setw(20)<<"Throughput [bps],"<<setw(16)<<"SE [bps/Hz],"<<setw(14)<<"BLERNewTx,"<<setw(14)<<"BLERReTx,"<<setw(18)<<"PostPSINR [dB],"<<setw(17)<<"GeometrySINR [dB],"<<setw(17)<<"PrePSINR [dB],"<<setw(17)<<"Avg Rank,"<<setw(17)<<"Start sf,"<<setw(17)<<"End sf,"<<setw(17)<<"UPT (bps)\n";
//             #endif	  
//             
//             for(int row_indx=0;row_indx<UEPerformance.rows();row_indx++)
//                 UEperf<<setw(6)<<UEPerformance(row_indx,0)<<","<<setw(9)<<UEPerformance(row_indx,1)<<","<<setw(11)<<UEPerformance(row_indx,2)<<","<<setw(11)<<UEPerformance(row_indx,4)<<","<<setw(19)<<UEPerformance(row_indx,5)<<","<<setw(15)<<UEPerformance(row_indx,6)<<","<<setw(13)<<UEPerformance(row_indx,3)<<","<<setw(13)<<UEPerformance(row_indx,8)<<","<<setw(17)<<UEPerformance(row_indx,7)<<","<<setw(16)<<UEPerformance(row_indx,14)<<","<<setw(16)<<UEPerformance(row_indx,9)<<","<<setw(16)<<UEPerformance(row_indx,10)<<","<<setw(16)<<UEPerformance(row_indx,11)<<setw(16)<<UEPerformance(row_indx,12)<<setw(16)<<UEPerformance(row_indx,13)<<endl;
//             UEperf.close();
//             
//             std::ofstream fout4;
//             if(isDL)
//                 fout4.open(dlSysPerf.c_str(), std::ios::app);
//             else
//                 fout4.open(ulSysPerf.c_str(), std::ios::app);
//             fout4<<"\n Average UE Throughput = "<<AvgThroughput<<"\n\n Cell-Edge UE Throughput = "<<cellEdgeThroughput<<endl;
//             
//             fout4<<"\n\n"<<briefSimulationDetails()<<endl;
//             fout4.close();
//             
//         }
//         #ifdef USING_MPI    
//     }
//     MPI_Barrier(MPI_COMM_WORLD);
//     #endif    
// }

// void PerformanceMetricsPerNU_S::printSystemPerformanceInfo(McellSystem &mySys,string uePerformanceFileName,string sysPerformanceFileName,string cumPerformanceFileName,AssociationInfo_S &associationInfo){
//   
//   Array<string> nodeTypes=mySys.getNodeTypes();
// #ifdef USING_MPI
//   if(nodeTypes.length()>10)
//     cout<<"[both:] Error with MPI : nodeTypes.length() cannot be more than 10 "<<endl;
// #endif
//     ivec nodeCounts=mySys.getNodeCount(), temp(0);
//     Array<NodeCategory_E> nodeCategories(nodeTypes.length());
//     for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
//       if(nodeCounts(nodeType_cnt)>0)
// 	nodeCategories(nodeType_cnt)=mySys.getNodeCategory(nodeTypes(nodeType_cnt));
//       else
// 	nodeCategories(nodeType_cnt)=_NODECATEGORY_UNINIT_;
//     
//     string tempUEFileName = uePerformanceFileName+".temp"+toString(currentTaskid);
//     string tempSysFileName = sysPerformanceFileName+".temp"+toString(currentTaskid);
//    
//     std::ofstream fout(tempUEFileName.c_str(), std::ios::out);
//     
// #ifdef USING_MPI
//       if(currentTaskid==0)
// #endif
//       fout<<"%"<<setw(12)<<"ServiceNodeID"<<setw(12)<<"ServerNodeID"<<setw(13)<<"BLER"<<setw(16)<<"BLERNewTx"<<setw(16)<<"BTP"<<setw(24)<<"Throughput(bps)"<<setw(17)<<"SE(bps/Hz)"<<setw(20)<<"AvgPPSINR(dB)"<<setw(14)<<"BLER_ReTx"<<setw(14)<<"PrePSINR (dB)"<<setw(14)<<"AvgRank"<<setw(14)<<setw(14)<<"Start Time"<<setw(14)<<"End Time"<<"UPT (bps)"<<"\nUEPerf=\"...\n";
//      
//       if(isDL) 
//       {
// 	for(int rx_cnt=0;rx_cnt<rxNodes.length();rx_cnt++)
// 	{
// 	  if(rxBits(rx_cnt)!=0)
// 	  {
// 	    double avgRank=1.0;
// 	    if(rankComputationInstant(rx_cnt)!=0)
// 	      avgRank=sumRankPerServiceNode(rx_cnt)/rankComputationInstant(rx_cnt);
// 	    
// 	    double userPacketThroughput = 0.0;
// 	    double activeTime = activeTimePerServiceNode(rx_cnt); //endTimePerServiceNode(rx_cnt) - startTimePerServiceNode(rx_cnt);
// 	    if(activeTime != 0)
//         {
// 	      userPacketThroughput = rxCorrectBitsBLER(rx_cnt)/activeTime;
//         }
//         double RBCount = nScheduledRBsPerServiceNode(rx_cnt);
// 	    fout<<setw(12)<<rxNodes(rx_cnt)<<setw(12)<<associationInfo.associatedServerNodesForEachServiceNode(rx_cnt)<<setw(12)<<rxBLER(rx_cnt)<<setw(16)<<rxBLERNew(rx_cnt)<<setw(16)<<rxBTPs(rx_cnt)<<setw(24)<<rxNodesThroughput(rx_cnt)<<setw(20)<<rxSpectralEffeciency(rx_cnt)<<setw(16)<<dB(rxPPSINR(rx_cnt)/rxPackets(rx_cnt))<<setw(16)<<rxBLER_RTx(rx_cnt)<<setw(16)<<dB(rxPrePSINR(rx_cnt)/rxPackets(rx_cnt))<<setw(16)<<avgRank<<setw(16)<<startTimePerServiceNode(rx_cnt)<<setw(16)<<endTimePerServiceNode(rx_cnt)<<setw(16)<<userPacketThroughput<<setw(16)<<RBCount<<";..."<<endl;
// 	  }
// 	}
//       }
//       else
//       {
// 	for(int tx_cnt=0;tx_cnt<txNodes.length();tx_cnt++)
// 	{
// 	  if(txBits(tx_cnt)!=0)
// 	  {
// 	    double avgRank=1.0;
// 	    if(rankComputationInstant(tx_cnt)!=0)
// 	      avgRank=sumRankPerServiceNode(tx_cnt)/rankComputationInstant(tx_cnt);
// 	    
// 	    double userPacketThroughput = 0.0;
// 	    double activeTime = activeTimePerServiceNode(tx_cnt);//endTimePerServiceNode(tx_cnt) - startTimePerServiceNode(tx_cnt);
// 	    if(activeTime != 0)
// 	      userPacketThroughput = txCorrectBitsBLER(tx_cnt)/activeTime;
//         double RBCount = nScheduledRBsPerServiceNode(tx_cnt);
// 	    fout<<setw(12)<<txNodes(tx_cnt)<<setw(12)<<associationInfo.associatedServerNodesForEachServiceNode(tx_cnt)<<setw(12)<<txBLER(tx_cnt)<<setw(16)<<txBLERNew(tx_cnt)<<setw(16)<<txBTPs(tx_cnt)<<setw(24)<<txNodesThroughput(tx_cnt)<<setw(20)<<txSpectralEffeciency(tx_cnt)<<setw(16)<<dB(txPPSINR(tx_cnt)/txPackets(tx_cnt))<<setw(16)<<txBLER_RTx(tx_cnt)<<setw(16)<<dB(txPrePSINR(tx_cnt)/txPackets(tx_cnt))<<setw(16)<<avgRank<<setw(16)<<startTimePerServiceNode(tx_cnt)<<setw(16)<<endTimePerServiceNode(tx_cnt)<<setw(16)<<userPacketThroughput<<setw(16)<<RBCount<<";..."<<endl;
// 	  }
// 	}
//       }
//       fout.close();
//       
//       ivec nodesPerType, nodeIndices;
//       vec avgThroughputPerServerNodeType=zeros(nodeTypes.length())-1;
//       vec avgSEPerServerNodeType=zeros(nodeTypes.length())-1;
//       vec avgThroughputPerServiceNodeUnderServerNodeType=zeros(nodeTypes.length())-1;
//       vec avgSEPerServiceNodeUnderServerNodeType=zeros(nodeTypes.length())-1;
//       ivec totalScheduledRBsPerServerNodeType = zeros_i(nodeTypes.length());
//       ivec totalScheduledRBsForSystemPerServerNodeType = zeros_i(nodeTypes.length());
//       double cellEdgeThroughput = 0,AvgThroughput = 0;
//       
//       ivec numberofServerNodes(nodeTypes.length());
//       std::ofstream fout1(tempSysFileName.c_str(), std::ios::out);
//       
//       for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
//       {
// 	if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
// 	{
// #ifdef USING_MPI
// 	  if(currentTaskid==0) {
// #endif
// 	    fout1<<"%#################################################################\n";
// 	    fout1<<"%\t"<<nodeTypes(nodeType_cnt)<<" Details:";
// 	    fout1<<"\n%#################################################################\n";
// 	    fout1<<"\n%BS_ID,\t\tThroughput Per "<<nodeTypes(nodeType_cnt)<<" (bps),\tSE per "<<nodeTypes(nodeType_cnt)<<" (bps/Hz),\tRB Utilisation per "<<nodeTypes(nodeType_cnt)<<"\n";
// #ifdef USING_MPI
// 	  }
// #endif
// 	  nodesPerType=mySys.getActiveNodes(nodeTypes(nodeType_cnt));
// 	  
// 	  if(isDL)
// 	  {
// 	    nodeIndices=find(txNodes,nodesPerType);
// 	    for(int node_cnt=0;node_cnt<nodesPerType.length();node_cnt++) 
//         {
// 	      if(txNodesThroughput(nodeIndices(node_cnt))!=0)
//           {
//               ivec assocSrvcNodes = associationInfo.associatedServiceNodesPerServerNode(txNodes(nodeIndices(node_cnt)));
//               ivec srvcIndices = find(rxNodes,assocSrvcNodes);
//               int totalScheduledRBs = sum(nScheduledRBsPerServiceNode(srvcIndices));
//               int nRBsPerTTI = (isDL) ? nuParams.dlNRBs : nuParams.ulNRBs;
//               int currentTTI = currentTimeInSec/nuParams.ttiDurationInSec;
//               totalScheduledRBsPerServerNodeType(nodeType_cnt) += totalScheduledRBs;
//             fout1<<setw(6)<<txNodes(nodeIndices(node_cnt))<<",\t\t"<< setw(16)<<txNodesThroughput(nodeIndices(node_cnt))<<",\t\t"<< setw(12)<<txSpectralEffeciency(nodeIndices(node_cnt))<<",\t\t"<< setw(12)<<(double)totalScheduledRBs/(nRBsPerTTI*currentTTI)<<endl;
//           }
// 	    }
// 	    numberofServerNodes(nodeType_cnt)=findNot(ceil_i(txNodesThroughput(nodeIndices)),0).length();
// 	    avgThroughputPerServerNodeType(nodeType_cnt) = sum(txNodesThroughput(nodeIndices))/numberofServerNodes(nodeType_cnt);
// 	    avgSEPerServerNodeType(nodeType_cnt)= sum(txSpectralEffeciency(nodeIndices))/numberofServerNodes(nodeType_cnt);
// 	    avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt)=sum(txNodesThroughput(nodeIndices));
// 	  }
// 	  else
// 	  {
// 	    nodeIndices=find(rxNodes,nodesPerType);
// 	    for(int node_cnt=0;node_cnt<nodesPerType.length();node_cnt++) 
//         {
// 	      if(rxNodesThroughput(nodeIndices(node_cnt))!=0)
//           {
//               ivec assocSrvcNodes = associationInfo.associatedServiceNodesPerServerNode(rxNodes(nodeIndices(node_cnt)));
//               ivec srvcIndices = find(txNodes,assocSrvcNodes);
//               int totalScheduledRBs = sum(nScheduledRBsPerServiceNode(srvcIndices));
//               int nRBsPerTTI = (isDL) ? nuParams.dlNRBs : nuParams.ulNRBs;
//               int currentTTI = currentTimeInSec/nuParams.ttiDurationInSec;
//               totalScheduledRBsPerServerNodeType(nodeType_cnt) += totalScheduledRBs;
//             fout1<<setw(6)<<rxNodes(nodeIndices(node_cnt))<<",\t\t"<< setw(16)<<rxNodesThroughput(nodeIndices(node_cnt))<<",\t\t"<< setw(12)<<rxSpectralEffeciency(nodeIndices(node_cnt))<<",\t\t"<< setw(12)<<(double)totalScheduledRBs/(nRBsPerTTI*currentTTI)<<endl;
//           }
// 	    }
// 	    numberofServerNodes(nodeType_cnt)=findNot(ceil_i(rxNodesThroughput(nodeIndices)),0).length();
// 	    avgThroughputPerServerNodeType(nodeType_cnt) = sum(rxNodesThroughput(nodeIndices))/numberofServerNodes(nodeType_cnt);
// 	    avgSEPerServerNodeType(nodeType_cnt)= sum(rxSpectralEffeciency(nodeIndices))/numberofServerNodes(nodeType_cnt);
// 	    avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt)=sum(rxNodesThroughput(nodeIndices));
// 	  }
// 	  
// #ifndef USING_MPI
// 	  double srvcNodeUnder=0;
// 	  for(int node_cnt=0;node_cnt<nodeIndices.length();node_cnt++)
// 	    srvcNodeUnder+=(double)associationInfo.associatedServiceNodesPerServerNode(nodeIndices(node_cnt)).length();
// 	  avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt)/=srvcNodeUnder;
// #endif
// 	}
//       }
//       
//       #ifdef USING_MPI
//       fout1.close();
//       
//       MPI_Barrier(MPI_COMM_WORLD);
//       MPIResultInfo.init(nTasks);
//       
//       for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
//       {
//         if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
//         {
//           MPIResultInfo.avgResults[currentTaskid].avgSEPerServerNodeType[nodeType_cnt]=avgSEPerServerNodeType(nodeType_cnt);
//           MPIResultInfo.avgResults[currentTaskid].avgThroughputPerServerNodeType[nodeType_cnt]=avgThroughputPerServerNodeType(nodeType_cnt);
//           MPIResultInfo.avgResults[currentTaskid].numberofServerNodes[nodeType_cnt]=numberofServerNodes(nodeType_cnt);
//           MPIResultInfo.avgResults[currentTaskid].avgThroughputPerServiceNodeUnderServerNodeType[nodeType_cnt]=avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt);
//           MPIResultInfo.avgResults[currentTaskid].nScheduledRBsPerServerNodeType[nodeType_cnt] = totalScheduledRBsPerServerNodeType(nodeType_cnt);
//         }
//       }
//       
//       cout<<"Loading Result Info For MPI in taskID: "<<currentTaskid<<endl;
//       
//       MPI_Gatherv(MPIResultInfo.avgResults+MPIResultInfo.indexShiftPerTask[currentTaskid],MPIResultInfo.countsPerTask[currentTaskid],MPI_BYTE,MPIResultInfo.avgResults,MPIResultInfo.countsPerTask,MPIResultInfo.offsetsPerTask,MPI_BYTE,0,MPI_COMM_WORLD);
//       MPI_Barrier(MPI_COMM_WORLD);
//       
//       if(currentTaskid==0) 
//       {
//           if(!mergeFiles(sysPerformanceFileName))
//           {
//                 cout<<"Unable to find the file "<<sysPerformanceFileName<<".temp*"<<endl;abort();
//           }
//         
//         vec avgSEPerServerNodeType_temp=zeros(nTasks),avgThroughputPerServerNodeType_temp=zeros(nTasks),avgThroughputPerServiceNodeUnderServerNodeType_temp=zeros(nTasks);
//         ivec numberofServerNodes_temp=zeros_i(nTasks);
//         for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
//         {
//           if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
//           {
//             nodesPerType=mySys.getActiveNodes(nodeTypes(nodeType_cnt));
//             for(int task=0;task<nTasks;task++)
//             {
//               numberofServerNodes_temp(task)=MPIResultInfo.avgResults[task].numberofServerNodes[nodeType_cnt];
//               if(MPIResultInfo.avgResults[task].avgSEPerServerNodeType[nodeType_cnt]!=MPIResultInfo.avgResults[task].avgSEPerServerNodeType[nodeType_cnt])
//                 avgSEPerServerNodeType_temp(task) = 0;
//               else
//                 avgSEPerServerNodeType_temp(task)=(MPIResultInfo.avgResults[task].avgSEPerServerNodeType[nodeType_cnt])*numberofServerNodes_temp(task);
//               if(MPIResultInfo.avgResults[task].avgThroughputPerServerNodeType[nodeType_cnt]!=MPIResultInfo.avgResults[task].avgThroughputPerServerNodeType[nodeType_cnt])
//                 avgThroughputPerServerNodeType_temp(task) = 0;
//               else
//                 avgThroughputPerServerNodeType_temp(task)=MPIResultInfo.avgResults[task].avgThroughputPerServerNodeType[nodeType_cnt]*numberofServerNodes_temp(task);
//               avgThroughputPerServiceNodeUnderServerNodeType_temp(task) = MPIResultInfo.avgResults[task].avgThroughputPerServiceNodeUnderServerNodeType[nodeType_cnt];
//               totalScheduledRBsForSystemPerServerNodeType(nodeType_cnt) += MPIResultInfo.avgResults[task].nScheduledRBsPerServerNodeType[nodeType_cnt];
//             }
//             if(isDL)
//               nodeIndices=find(txNodes,nodesPerType);
//             else
//               nodeIndices=find(rxNodes,nodesPerType);
//             int srvcNodeUnder=0;
//             for(int node_cnt=0;node_cnt<nodeIndices.length();node_cnt++)
//               srvcNodeUnder+=associationInfo.associatedServiceNodesPerServerNode(nodeIndices(node_cnt)).length();
//             
//             avgSEPerServerNodeType(nodeType_cnt)=sum(avgSEPerServerNodeType_temp)/sum(numberofServerNodes_temp);
//             avgThroughputPerServerNodeType(nodeType_cnt)=sum(avgThroughputPerServerNodeType_temp)/sum(numberofServerNodes_temp);
//             avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt)=sum(avgThroughputPerServiceNodeUnderServerNodeType_temp)/srvcNodeUnder;
//           }
//         }
//         
//         std::ofstream fout1(sysPerformanceFileName.c_str(), std::ios::app);
//         #endif
//         
//         fout1<<"\n\n%#################################################################\n%\tPerformance Metrics:\n%#################################################################\n"<<"\n%  Throughput:\n%  ~~~~~~~~~~~\n"<<endl;
//         for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
//           if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
//             fout1<<"\tAverage UE Throughput Per "<<nodeTypes(nodeType_cnt)<<" (bps)\t= "<<avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt)<<"\n\n";
//           for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
//             if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
//               fout1<<"\tAverage Throughput Per "<<nodeTypes(nodeType_cnt)<<" (bps)\t\t= "<<avgThroughputPerServerNodeType(nodeType_cnt)<<"\n\n";
//             fout1<<"\n\n%  Spectral Efficiency:\n%  ~~~~~~~~~~~~~~~~~~~~\n"<<endl;
//           
//           for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
//             if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
//               fout1<<"\tAverage SE per "<<nodeTypes(nodeType_cnt)<<" (bps/Hz)\t\t= "<<avgSEPerServerNodeType(nodeType_cnt)<<"\n";
//             
//             fout1<<"\n%#################################################################\n"<<endl;
//           fout1.close();
//           #ifdef USING_MPI
//       }
//       
//       MPIResultInfo.free();
//       MPI_Barrier(MPI_COMM_WORLD);
//       if(currentTaskid==0)
//       {
//           if(!mergeFiles(uePerformanceFileName))
//           {
//                 cout<<"Unable to find the file "<<uePerformanceFileName<<".temp*"<<endl;abort();
//           }
// #endif
// 	vec preprocessingSINRData = associationInfo.geometrySINRindBperServiceNode;
// 	ivec serviceNodes = associationInfo.serviceNodes;
// 	
// 	std::ofstream fout3;
// 	fout3.open(uePerformanceFileName.c_str(), std::ios::app);
// 	fout3<<"\"";
// 	fout3.close();
// 	
// 	cout<<"currentTaskid : "<<currentTaskid<<endl;
// 	
// 	mat UEPerf;
// 	if(parse(uePerformanceFileName,"UEPerf",UEPerf)==false)
// 	  cout<<"Unable to Parse UEPerf..."<<endl;
//     
// 	if(UEPerf.size()!=0){
// 	  vec throughput=UEPerf.get_col(5);
// 	  ivec throughputsortedindx=sort_index(throughput);
// 	  mat UEPerformance;
// 	  UEPerformance.set_size(UEPerf.rows(),UEPerf.cols()+1);
// 	  for(int row_indx=0;row_indx<UEPerformance.rows();row_indx++){
// 	    for(int col_indx=0;col_indx<UEPerformance.cols();col_indx++){
// 	      if(col_indx!=UEPerformance.cols()-1)
// 		UEPerformance(row_indx,col_indx)=UEPerf(throughputsortedindx(row_indx),col_indx);
// 	      else
// 		UEPerformance(row_indx,col_indx)=preprocessingSINRData(find(serviceNodes,(int)UEPerformance(row_indx,0)));
// 	    }
// 	  }
// 	  
// 	  string input6 = "rm "+uePerformanceFileName;system_exec(input6.c_str());
//       
// 	  cellEdgeThroughput=UEPerformance.get_col(5)(floor_i(0.05*UEPerformance.rows()));
// 	  AvgThroughput=mean(UEPerformance.get_col(5));
// 	  
// 	  std::ofstream UEperf(uePerformanceFileName.c_str(), std::ios::out);
// 	  #ifdef USING_CLUSTER
// 	  UEperf<<"%"<<setw(6)<<"UE-ID,"<<setw(10)<<"eNB-ID,"<<setw(12)<<"BLER,"<<setw(12)<<"RBcount,"<<setw(20)<<"Throughput [bps],"<<setw(16)<<"SE [bps/Hz],"<<setw(14)<<"BLERNewTx,"<<setw(14)<<"BLERReTx,"<<setw(18)<<"PostPSINR [dB],"<<setw(17)<<"GeometrySINR [dB],"<<setw(17)<<"PrePSINR [dB],"<<setw(17)<<"Avg Rank,"<<setw(17)<<"UPT (bps)\n";
// 	  #else
// 	  UEperf<<"%"<<setw(6)<<"ServiceNodeID,"<<setw(10)<<"ServerNodeID,"<<setw(12)<<"BLER,"<<setw(12)<<"RBcount,"<<setw(20)<<"Throughput [bps],"<<setw(16)<<"SE [bps/Hz],"<<setw(14)<<"BLERNewTx,"<<setw(14)<<"BLERReTx,"<<setw(18)<<"PostPSINR [dB],"<<setw(17)<<"GeometrySINR [dB],"<<setw(17)<<"PrePSINR [dB],"<<setw(17)<<"Avg Rank,"<<setw(17)<<"Start sf,"<<setw(17)<<"End sf,"<<setw(17)<<"UPT (bps),"<<setw(17)<<"RB Utilisation\n";
// 	  #endif	  
// 	  
// 	  for(int row_indx=0;row_indx<UEPerformance.rows();row_indx++)
// 	    UEperf<<setw(6)<<UEPerformance(row_indx,0)<<","<<setw(9)<<UEPerformance(row_indx,1)<<","<<setw(11)<<UEPerformance(row_indx,2)<<","<<setw(11)<<UEPerformance(row_indx,4)<<","<<setw(19)<<UEPerformance(row_indx,5)<<","<<setw(15)<<UEPerformance(row_indx,6)<<","<<setw(13)<<UEPerformance(row_indx,3)<<","<<setw(13)<<UEPerformance(row_indx,8)<<","<<setw(17)<<UEPerformance(row_indx,7)<<","<<setw(16)<<UEPerformance(row_indx,14)<<","<<setw(16)<<UEPerformance(row_indx,9)<<","<<setw(16)<<UEPerformance(row_indx,10)<<","<<setw(16)<<UEPerformance(row_indx,11)<<setw(16)<<UEPerformance(row_indx,12)<<setw(16)<<UEPerformance(row_indx,13)<<setw(16)<<(double)UEPerformance(row_indx,14)/(nuParams.dlNRBs*double(currentTimeInSec/nuParams.ttiDurationInSec))<<endl;
// 	  UEperf.close();
// 	  
// 	  std::ofstream fout4;
//       fout4.open(sysPerformanceFileName.c_str(),std::ios::app);
//       
// 	  fout4<<"\n Average UE Throughput = "<<AvgThroughput<<"\n\n Cell-Edge UE Throughput = "<<cellEdgeThroughput<<endl;
// 	  
// 	  fout4<<"\n\n"<<briefSimulationDetails(currentTimeInSec)<<endl;
// 	  fout4.close();
// 	  
// 	}
// #ifdef USING_MPI    
//       }
//       MPI_Barrier(MPI_COMM_WORLD);
// #endif
//         if(currentTaskid==0)
//         {
//             string fileName = cumPerformanceFileName;
//             double ttiDurationInSec = nuParams.ttiDurationInSec;
//             int currentTTI = currentTimeInSec/ttiDurationInSec;
//             int nRBsPerTTI = (isDL) ? nuParams.dlNRBs : nuParams.ulNRBs;
//             ofstream op;
//             if(!IsFileExists(fileName.c_str()))
//             {
//                 op.open(fileName.c_str(),std::ios::out);
//                 op<<"%currentTTI,\tavgUEThroughputPerBS (bps),\tavgBSThroughput (bps),\tBSSpectralEfficiency,\tavgResourceUtilisationPerBS,\tavgUEThroughput (bps),\tcellEdgeUEThroughput (bps)"<<endl;
//             }
//             else
//                 op.open(fileName.c_str(),std::ios::app);
//             stringstream ss;
//             ss<<currentTTI<<",\t";
//             for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
//             {
//                 if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
//                 {
//                     ss<<avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt)<<",\t"<<avgThroughputPerServerNodeType(nodeType_cnt)<<",\t"<<avgSEPerServerNodeType(nodeType_cnt)<<",\t"<<(double)totalScheduledRBsForSystemPerServerNodeType(nodeType_cnt)/(nRBsPerTTI*currentTTI*nodeCounts(nodeType_cnt))<<",\t";
//                 }
//             }
//             ss<<AvgThroughput<<",\t"<<cellEdgeThroughput;
//             op<<ss.str()<<endl;
//             op.close();
//         }
// }

void PerformanceMetricsPerNU_S::printUEPerformanceInfo(string uePerformanceFileName,AssociationInfo_S* associationInfoPtr,double& cellEdgeThroughput,double& AvgThroughput)
{
    string tempUEFileName = uePerformanceFileName+".temp"+toString(currentTaskid);
    std::ofstream fout(tempUEFileName.c_str(), std::ios::out);
    
    #ifdef USING_MPI
      if(currentTaskid==0)
    #endif
      fout<<"%"<<setw(12)<<"ServiceNodeID"<<setw(12)<<"ServerNodeID"<<setw(13)<<"BLER"<<setw(16)<<"BLERNewTx"<<setw(16)<<"BTP"<<setw(24)<<"Throughput(bps)"<<setw(17)<<"SE(bps/Hz)"<<setw(20)<<"AvgPPSINR(dB)"<<setw(14)<<"BLER_ReTx"<<setw(14)<<"PrePSINR (dB)"<<setw(14)<<"AvgRank"<<setw(14)<<setw(14)<<"Start Time"<<setw(14)<<"End Time"<<"UPT (bps)"<<"\nUEPerf=\"...\n";
     
      if(isDL) 
      {
        for(int rx_cnt=0;rx_cnt<rxNodes.length();rx_cnt++)
        {
            if(rxBits(rx_cnt)!=0)
            {
                double avgRank=1.0;
                if(rankComputationInstant(rx_cnt)!=0)
                    avgRank=sumRankPerServiceNode(rx_cnt)/rankComputationInstant(rx_cnt);
	    
                double userPacketThroughput = 0.0;
                double activeTime = activeTimePerServiceNode(rx_cnt); //endTimePerServiceNode(rx_cnt) - startTimePerServiceNode(rx_cnt);
                if(activeTime != 0)
                {
                    userPacketThroughput = rxCorrectBitsBLER(rx_cnt)/activeTime;
                }
                double RBCount = nScheduledRBsPerServiceNode(rx_cnt);
                fout<<setw(12)<<rxNodes(rx_cnt)<<setw(12)<<associationInfoPtr->associatedServerNodesForEachServiceNode(rx_cnt)<<setw(12)<<rxBLER(rx_cnt)<<setw(16)<<rxBLERNew(rx_cnt)<<setw(16)<<rxBTPs(rx_cnt)<<setw(24)<<rxNodesThroughput(rx_cnt)<<setw(20)<<rxSpectralEffeciency(rx_cnt)<<setw(16)<<dB(rxPPSINR(rx_cnt)/rxPackets(rx_cnt))<<setw(16)<<rxBLER_RTx(rx_cnt)<<setw(16)<<dB(rxPrePSINR(rx_cnt)/rxPackets(rx_cnt))<<setw(16)<<avgRank<<setw(16)<<startTimePerServiceNode(rx_cnt)<<setw(16)<<endTimePerServiceNode(rx_cnt)<<setw(16)<<userPacketThroughput<<setw(16)<<RBCount<<";..."<<endl;
            }
        }
      }
      else
      {
        for(int tx_cnt=0;tx_cnt<txNodes.length();tx_cnt++)
        {
            if(txBits(tx_cnt)!=0)
            {
                double avgRank=1.0;
                if(rankComputationInstant(tx_cnt)!=0)
                    avgRank=sumRankPerServiceNode(tx_cnt)/rankComputationInstant(tx_cnt);
	    
                double userPacketThroughput = 0.0;
                double activeTime = activeTimePerServiceNode(tx_cnt);//endTimePerServiceNode(tx_cnt) - startTimePerServiceNode(tx_cnt);
                if(activeTime != 0)
                    userPacketThroughput = txCorrectBitsBLER(tx_cnt)/activeTime;
                double RBCount = nScheduledRBsPerServiceNode(tx_cnt);
                fout<<setw(12)<<txNodes(tx_cnt)<<setw(12)<<associationInfoPtr->associatedServerNodesForEachServiceNode(tx_cnt)<<setw(12)<<txBLER(tx_cnt)<<setw(16)<<txBLERNew(tx_cnt)<<setw(16)<<txBTPs(tx_cnt)<<setw(24)<<txNodesThroughput(tx_cnt)<<setw(20)<<txSpectralEffeciency(tx_cnt)<<setw(16)<<dB(txPPSINR(tx_cnt)/txPackets(tx_cnt))<<setw(16)<<txBLER_RTx(tx_cnt)<<setw(16)<<dB(txPrePSINR(tx_cnt)/txPackets(tx_cnt))<<setw(16)<<avgRank<<setw(16)<<startTimePerServiceNode(tx_cnt)<<setw(16)<<endTimePerServiceNode(tx_cnt)<<setw(16)<<userPacketThroughput<<setw(16)<<RBCount<<";..."<<endl;
            }
        }
      }
      fout.close();
#ifdef USING_MPI
      MPI_Barrier(MPI_COMM_WORLD);      
      if(currentTaskid==0)
      {
        if(!mergeFiles(uePerformanceFileName))
        {
            cout<<"Unable to find the file "<<uePerformanceFileName<<".temp*"<<endl;abort();
        }
#endif
//         vec preprocessingSINRData = associationInfoPtr->geometrySINRindBperServiceNode;
        ivec serviceNodes = associationInfoPtr->serviceNodes;
	
        std::ofstream fout1;
        fout1.open(uePerformanceFileName.c_str(), std::ios::app);
        fout1<<"\"";
        fout1.close();
	
        cout<<"currentTaskid : "<<currentTaskid<<endl;
	
        mat UEPerf;
        if(parse(uePerformanceFileName,"UEPerf",UEPerf)==false)
            cout<<"Unable to Parse UEPerf..."<<endl;
    
        if(UEPerf.size()!=0)
        {
            vec throughput=UEPerf.get_col(5);
            ivec throughputsortedindx=sort_index(throughput);
            mat UEPerformance;
            UEPerformance.set_size(UEPerf.rows(),UEPerf.cols()+1);
            for(int row_indx=0;row_indx<UEPerformance.rows();row_indx++){
            for(int col_indx=0;col_indx<UEPerformance.cols();col_indx++){
                
                if(col_indx!=UEPerformance.cols()-1)
                    UEPerformance(row_indx,col_indx)=UEPerf(throughputsortedindx(row_indx),col_indx);
                else
                {
                    UEPerformance(row_indx,col_indx)=-500;
                }
            }
            }
	  
            string input6 = "rm "+uePerformanceFileName;system_exec(input6.c_str());
      
            cellEdgeThroughput=UEPerformance.get_col(5)(floor_i(0.05*UEPerformance.rows()));
            AvgThroughput=mean(UEPerformance.get_col(5));
	  
            std::ofstream UEperf(uePerformanceFileName.c_str(), std::ios::out);
            #ifdef USING_CLUSTER
            UEperf<<"%"<<setw(6)<<"UE-ID,"<<setw(10)<<"eNB-ID,"<<setw(12)<<"BLER,"<<setw(12)<<"RBcount,"<<setw(20)<<"Throughput [bps],"<<setw(16)<<"SE [bps/Hz],"<<setw(14)<<"BLERNewTx,"<<setw(14)<<"BLERReTx,"<<setw(18)<<"PostPSINR [dB],"<<setw(17)<<"GeometrySINR [dB],"<<setw(17)<<"PrePSINR [dB],"<<setw(17)<<"Avg Rank,"<<setw(17)<<"UPT (bps)\n";
            #else
            UEperf<<"%"<<setw(6)<<"ServiceNodeID,"<<setw(10)<<"ServerNodeID,"<<setw(12)<<"BLER,"<<setw(12)<<"RBcount,"<<setw(20)<<"Throughput [bps],"<<setw(16)<<"SE [bps/Hz],"<<setw(14)<<"BLERNewTx,"<<setw(14)<<"BLERReTx,"<<setw(18)<<"PostPSINR [dB],"<<setw(17)<<"GeometrySINR [dB],"<<setw(17)<<"PrePSINR [dB],"<<setw(17)<<"Avg Rank,"<<setw(17)<<"Start sf,"<<setw(17)<<"End sf,"<<setw(17)<<"UPT (bps),"<<setw(17)<<"RB Utilisation\n";
            #endif	  
	  
            for(int row_indx=0;row_indx<UEPerformance.rows();row_indx++)
                UEperf<<setw(6)<<UEPerformance(row_indx,0)<<","<<setw(9)<<UEPerformance(row_indx,1)<<","<<setw(11)<<UEPerformance(row_indx,2)<<","<<setw(11)<<UEPerformance(row_indx,4)<<","<<setw(19)<<UEPerformance(row_indx,5)<<","<<setw(15)<<UEPerformance(row_indx,6)<<","<<setw(13)<<UEPerformance(row_indx,3)<<","<<setw(13)<<UEPerformance(row_indx,8)<<","<<setw(17)<<UEPerformance(row_indx,7)<<","<<setw(16)<<UEPerformance(row_indx,14)<<","<<setw(16)<<UEPerformance(row_indx,9)<<","<<setw(16)<<UEPerformance(row_indx,10)<<","<<setw(16)<<UEPerformance(row_indx,11)<<setw(16)<<UEPerformance(row_indx,12)<<setw(16)<<UEPerformance(row_indx,13)<<setw(16)<<(double)UEPerformance(row_indx,14)/(nuParams.dlNRBs*double(currentTimeInSec/nuParams.ttiDurationInSec))<<endl;
            UEperf.close();
        }
    #ifdef USING_MPI
      }
    #endif
}

void PerformanceMetricsPerNU_S::printBSPerformanceInfo(string sysPerformanceFileName,McellSystem* mySysPtr,AssociationInfo_S* associationInfoPtr,PerformanceInfo_S* performanceInfoPtr,double cellEdgeThroughput,double AvgThroughput)
{
    Array<string> nodeTypes=mySysPtr->getNodeTypes();
    
    ivec nodeCounts=mySysPtr->getNodeCount();
    Array<NodeCategory_E> nodeCategories = mySysPtr->getNodeCategory();
    
    string tempSysFileName = sysPerformanceFileName+".temp"+toString(currentTaskid);
    std::ofstream fout1(tempSysFileName.c_str(), std::ios::out);
    
    ivec nodesPerType, nodeIndices;
    
    for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
    {
        if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
        {
            #ifdef USING_MPI
            if(currentTaskid==0) {
            #endif
                fout1<<"%#################################################################\n";
                fout1<<"%\t"<<nodeTypes(nodeType_cnt)<<" Details:";
                fout1<<"\n%#################################################################\n";
                fout1<<"\n%BS_ID,\t\tThroughput Per "<<nodeTypes(nodeType_cnt)<<" (bps),\tSE per "<<nodeTypes(nodeType_cnt)<<" (bps/Hz),\tRB Utilisation per "<<nodeTypes(nodeType_cnt)<<"\n";
            #ifdef USING_MPI
            }
            #endif
            nodesPerType=mySysPtr->getActiveNodes(nodeTypes(nodeType_cnt));
	  
            if(isDL)
            {
                nodeIndices=find(txNodes,nodesPerType);
                for(int node_cnt=0;node_cnt<nodesPerType.length();node_cnt++) 
                {
                    if(txNodesThroughput(nodeIndices(node_cnt))!=0)
                    {
                        ivec assocSrvcNodes = associationInfoPtr->associatedServiceNodesPerServerNode(txNodes(nodeIndices(node_cnt)));
                        ivec srvcIndices = find(rxNodes,assocSrvcNodes);
                        int totalScheduledRBs = sum(nScheduledRBsPerServiceNode(srvcIndices));
                        int nRBsPerTTI = (isDL) ? nuParams.dlNRBs : nuParams.ulNRBs;
                        int currentTTI = currentTimeInSec/nuParams.ttiDurationInSec;
                        fout1<<setw(6)<<txNodes(nodeIndices(node_cnt))<<",\t\t"<< setw(16)<<txNodesThroughput(nodeIndices(node_cnt))<<",\t\t"<< setw(12)<<txSpectralEffeciency(nodeIndices(node_cnt))<<",\t\t"<< setw(12)<<(double)totalScheduledRBs/(nRBsPerTTI*currentTTI)<<endl;
                    }
                }
            }
            else
            {
                nodeIndices=find(rxNodes,nodesPerType);
                for(int node_cnt=0;node_cnt<nodesPerType.length();node_cnt++) 
                {
                    if(rxNodesThroughput(nodeIndices(node_cnt))!=0)
                    {
                        ivec assocSrvcNodes = associationInfoPtr->associatedServiceNodesPerServerNode(rxNodes(nodeIndices(node_cnt)));
                        ivec srvcIndices = find(txNodes,assocSrvcNodes);
                        int totalScheduledRBs = sum(nScheduledRBsPerServiceNode(srvcIndices));
                        int nRBsPerTTI = (isDL) ? nuParams.dlNRBs : nuParams.ulNRBs;
                        int currentTTI = currentTimeInSec/nuParams.ttiDurationInSec;
                        fout1<<setw(6)<<rxNodes(nodeIndices(node_cnt))<<",\t\t"<< setw(16)<<rxNodesThroughput(nodeIndices(node_cnt))<<",\t\t"<< setw(12)<<rxSpectralEffeciency(nodeIndices(node_cnt))<<",\t\t"<< setw(12)<<(double)totalScheduledRBs/(nRBsPerTTI*currentTTI)<<endl;
                    }
                }
            }
	  
#ifndef USING_MPI
	  double srvcNodeUnder=0;
	  for(int node_cnt=0;node_cnt<nodeIndices.length();node_cnt++)
          srvcNodeUnder+=(double)associationInfoPtr->associatedServiceNodesPerServerNode(nodeIndices(node_cnt)).length();
      performanceInfoPtr->avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt)/=srvcNodeUnder;
#endif
	}
      }
      
      #ifdef USING_MPI
      fout1.close();
      MPI_Barrier(MPI_COMM_WORLD);
      if(currentTaskid==0) 
      {
          if(!mergeFiles(sysPerformanceFileName))
          {
                cout<<"Unable to find the file "<<sysPerformanceFileName<<".temp*"<<endl;abort();
          }
        
        std::ofstream fout1(sysPerformanceFileName.c_str(), std::ios::app);
     #endif
        
        fout1<<"\n\n%#################################################################\n%\tPerformance Metrics:\n%#################################################################\n"<<"\n%  Throughput:\n%  ~~~~~~~~~~~\n"<<endl;
        for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
          if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
            fout1<<"\tAverage UE Throughput Per "<<nodeTypes(nodeType_cnt)<<" (bps)\t= "<<performanceInfoPtr->avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt)<<"\n\n";
          for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
            if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
              fout1<<"\tAverage Throughput Per "<<nodeTypes(nodeType_cnt)<<" (bps)\t\t= "<<performanceInfoPtr->avgThroughputPerServerNodeType(nodeType_cnt)<<"\n\n";
            fout1<<"\n\n%  Spectral Efficiency:\n%  ~~~~~~~~~~~~~~~~~~~~\n"<<endl;
          
          for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
            if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
              fout1<<"\tAverage SE per "<<nodeTypes(nodeType_cnt)<<" (bps/Hz)\t\t= "<<performanceInfoPtr->avgSEPerServerNodeType(nodeType_cnt)<<"\n";
            
            fout1<<"\n%#################################################################\n"<<endl;
          
	  fout1<<"\n Average UE Throughput = "<<AvgThroughput<<"\n\n Cell-Edge UE Throughput = "<<cellEdgeThroughput<<endl;
      
      if(isnan(sum(performanceInfoPtr->avgThroughputPerServiceNodeUnderServerNodeType)) || isnan(sum(performanceInfoPtr->avgThroughputPerServerNodeType)) || isnan(sum(performanceInfoPtr->avgSEPerServerNodeType)))
      {
          cout<<"Avg UE ThroughputPerBS,\tAvg ThroughputPerBS,\tAvg SEPerBS"<<endl;
          cout<<performanceInfoPtr->avgThroughputPerServiceNodeUnderServerNodeType<<",\t"<<performanceInfoPtr->avgThroughputPerServerNodeType<<",\t"<<performanceInfoPtr->avgSEPerServerNodeType<<endl;
          cout<<"Throughput cannot be NAN...";
          abort();
      }
	  
	  fout1<<"\n\n"<<briefSimulationDetails(currentTimeInSec)<<endl;
	  fout1.close();
    #ifdef USING_MPI
      }
    #endif
}

void PerformanceMetricsPerNU_S::printSystemPerformanceInfo(string cumPerformanceFileName,McellSystem* mySysPtr,PerformanceInfo_S* performanceInfoPtr,double cellEdgeThroughput,double AvgThroughput)
{
    Array<string> nodeTypes=mySysPtr->getNodeTypes();
  
    ivec nodeCounts=mySysPtr->getNodeCount(), temp(0);
    Array<NodeCategory_E> nodeCategories = mySysPtr->getNodeCategory();
    
#ifdef USING_MPI
    if(currentTaskid==0)
    {
#endif
            string fileName = cumPerformanceFileName;
            double ttiDurationInSec = nuParams.ttiDurationInSec;
            int currentTTI = currentTimeInSec/ttiDurationInSec;
            int nRBsPerTTI = (isDL) ? nuParams.dlNRBs : nuParams.ulNRBs;
            ofstream op;
            if(!IsFileExists(fileName.c_str()))
            {
                op.open(fileName.c_str(),std::ios::out);
                op<<"%currentTTI,\tavgUEThroughputPerBS (bps),\tavgBSThroughput (bps),\tBSSpectralEfficiency,\tavgResourceUtilisationPerBS,\tavgUEThroughput (bps),\tcellEdgeUEThroughput (bps)"<<endl;
            }
            else
                op.open(fileName.c_str(),std::ios::app);
            stringstream ss;
            ss<<currentTTI<<",\t";
            for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
            {
                if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
                {
                    ss<<performanceInfoPtr->avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt)<<",\t"<<performanceInfoPtr->avgThroughputPerServerNodeType(nodeType_cnt)<<",\t"<<performanceInfoPtr->avgSEPerServerNodeType(nodeType_cnt)<<",\t"<<(double)performanceInfoPtr->totalScheduledRBsForSystemPerServerNodeType(nodeType_cnt)/(nRBsPerTTI*currentTTI*nodeCounts(nodeType_cnt))<<",\t";
                }
            }
            ss<<AvgThroughput<<",\t"<<cellEdgeThroughput;
            op<<ss.str()<<endl;
            op.close();
    #ifdef USING_MPI
    }
    #endif
}

PerformanceInfo_S PerformanceMetricsPerNU_S::getPerformanceInfo(McellSystem* mySysPtr,Array<ivec>& associatedServiceNodesPerServerNode)
{
    Array<string> nodeTypes=mySysPtr->getNodeTypes();
    ivec nodeCounts=mySysPtr->getNodeCount();
    Array<NodeCategory_E> nodeCategories = mySysPtr->getNodeCategory();
    
    PerformanceInfo_S performanceInfo(nodeTypes.length());
    
    ivec nodesPerType, nodeIndices;
    for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
    {
        if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
        {
            nodesPerType=mySysPtr->getActiveNodes(nodeTypes(nodeType_cnt));
            if(isDL)
            {
                nodeIndices=find(txNodes,nodesPerType);
                for(int node_cnt=0;node_cnt<nodesPerType.length();node_cnt++) 
                {
                    if(txNodesThroughput(nodeIndices(node_cnt))!=0)
                    {
                        ivec assocSrvcNodes = associatedServiceNodesPerServerNode(txNodes(nodeIndices(node_cnt)));
                        ivec srvcIndices = find(rxNodes,assocSrvcNodes);
                        int totalScheduledRBs = sum(nScheduledRBsPerServiceNode(srvcIndices));
                        performanceInfo.totalScheduledRBsPerServerNodeType(nodeType_cnt) += totalScheduledRBs;
                        
                    }
                }
                performanceInfo.numberofServerNodes(nodeType_cnt)=findNot(ceil_i(txNodesThroughput(nodeIndices)),0).length();
                if(performanceInfo.numberofServerNodes(nodeType_cnt))
                {
                    performanceInfo.avgThroughputPerServerNodeType(nodeType_cnt) = sum(txNodesThroughput(nodeIndices))/performanceInfo.numberofServerNodes(nodeType_cnt);
                    performanceInfo.avgSEPerServerNodeType(nodeType_cnt)= sum(txSpectralEffeciency(nodeIndices))/performanceInfo.numberofServerNodes(nodeType_cnt);
                    performanceInfo.avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt)=sum(txNodesThroughput(nodeIndices));
                }
            }
            else
            {
                nodeIndices=find(rxNodes,nodesPerType);
                for(int node_cnt=0;node_cnt<nodesPerType.length();node_cnt++) 
                {
                    if(rxNodesThroughput(nodeIndices(node_cnt))!=0)
                    {
                        ivec assocSrvcNodes = associatedServiceNodesPerServerNode(rxNodes(nodeIndices(node_cnt)));
                        ivec srvcIndices = find(txNodes,assocSrvcNodes);
                        int totalScheduledRBs = sum(nScheduledRBsPerServiceNode(srvcIndices));
                        performanceInfo.totalScheduledRBsPerServerNodeType(nodeType_cnt) += totalScheduledRBs;
                        
                    }
                }
                performanceInfo.numberofServerNodes(nodeType_cnt)=findNot(ceil_i(rxNodesThroughput(nodeIndices)),0).length();
                if(performanceInfo.numberofServerNodes(nodeType_cnt))
                {
                    performanceInfo.avgThroughputPerServerNodeType(nodeType_cnt) = sum(rxNodesThroughput(nodeIndices))/performanceInfo.numberofServerNodes(nodeType_cnt);
                    performanceInfo.avgSEPerServerNodeType(nodeType_cnt)= sum(rxSpectralEffeciency(nodeIndices))/performanceInfo.numberofServerNodes(nodeType_cnt);
                    performanceInfo.avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt)=sum(rxNodesThroughput(nodeIndices));
                }
            }
        }
    }
    return performanceInfo;
}

void PerformanceMetricsPerNU_S::printSystemPerformanceInfo(McellSystem *mySysPtr,string uePerformanceFileName,string sysPerformanceFileName,string cumPerformanceFileName,AssociationInfo_S *associationInfoPtr){
  
  Array<string> nodeTypes=mySysPtr->getNodeTypes();
#ifdef USING_MPI
  if(nodeTypes.length()>10)
    cout<<"[both:] Error with MPI : nodeTypes.length() cannot be more than 10 "<<endl;
#endif
    
    double cellEdgeThroughput = 0,AvgThroughput = 0;
      
    printUEPerformanceInfo(uePerformanceFileName,associationInfoPtr,cellEdgeThroughput,AvgThroughput);
      
    PerformanceInfo_S performanceInfo = getPerformanceInfo(mySysPtr,associationInfoPtr->associatedServiceNodesPerServerNode);
     
#ifdef USING_MPI
    if(nTasks>1)
        MPIGatherPerformanceInfo(performanceInfo,mySysPtr,associationInfoPtr);
#endif
    printBSPerformanceInfo(sysPerformanceFileName,mySysPtr,associationInfoPtr,&performanceInfo,cellEdgeThroughput,AvgThroughput);
      
    printSystemPerformanceInfo(cumPerformanceFileName,mySysPtr,&performanceInfo,cellEdgeThroughput,AvgThroughput);
}

void printWindowBLERStats(int nodeID, int correctPacketCount, double currentTime, int codeword,string fileName){
  
//   #ifdef USING_MPI
//   if(DL)
//   {
//   string fileName="./Results/dlWindowBLERstats.txt.temp"+toString(currentTaskid);
//   }
//   else
//   {
//    string fileName="./Results/ULWindowBLERstats.txt.temp"+toString(currentTaskid); 
//   }
//   #else
//   string fileName="./Results/DLWindowBLERstats.txt";
//   #endif
    #ifdef USING_MPI
    fileName = fileName+".temp"+toString(currentTaskid);
    #endif
  std::ofstream fout;

  static bool isNewPrint1=1;
  if(isNewPrint1)
  {
    fout.open(fileName.c_str(),ios::out);
    if(currentTaskid==0)
      fout<<"%CurrentTime,\tUE_ID,\t\tWindowBLER,\t\tcodewordIndx \t\t\t //Note : BLER per 100 new Blocks\n";
    isNewPrint1=false;
  }
  else
    fout.open(fileName.c_str(),ios::app);

  fout<<setw(6)<<currentTime<<",\t\t\t"<<setw(6)<<nodeID<<",\t\t"<<setw(10)<<1.0-(correctPacketCount/100.0)<<",\t\t"<<setw(4)<<codeword<<endl;
    
}

// void PerformanceMetricsPerNU_S::printRBUtilization(int nodeID){
//   
//   #ifdef USING_MPI
//   string filename="./Results/RBUtilization.txt.temp"+toString(currentTaskid);
//   #else
//   string filename="./Results/RBUtilization.txt";
//   #endif
//   
//   int indx;
//   if(isDL)
//     indx=find(txNodes,nodeID);
//   else
//     indx=find(rxNodes,nodeID);
// 
//   std::ofstream fout;
//   
//   static bool isNewPrint1=1;
//   if(isNewPrint1)
//   {
//     fout.open(filename.c_str(), std::ios::out);
//     if(currentTaskid==0)
//       fout<<"% eNBid,\t\tPercentageOfRBUtilization\n";
//     isNewPrint1=0;
//   }
//   else
//     fout.open(filename.c_str(), std::ios::app);
//   
//   fout<<setw(4)<<nodeID<<"\t";
//   for(int cnt=0;cnt<PercentageOfRBUtilization(indx).length();cnt++)
//   {
//     fout<<","<<PercentageOfRBUtilization(indx)(cnt);
//   }
//   fout<<endl;
// }

