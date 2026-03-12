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

#include <lib/L1/PDCCH/include/DownlinkControlChannels.hpp>
#include "../include/Transmitter.h"
#include "../include/NodeDevice.h"
// #include "../../lib/PrachSupport/include/PRACH_CONST.h"

Array<cmat> mapLogicalToPhysicalSubFrame(Array<cmat> logicalSubFrame, int fftSize , Size_S rbSize, ResourceMapper *resourceMapper, bool isDownlink,/*int subframeNumber,bool enableUplinkFreqHopping,*/ivec uplinkScheduledRBs) {

    int scPerRU=rbSize.numberOfSubcarriers;
    //int symPerRU=rbSize.numberOfSymbols;
    int nRBs;
    if(isDownlink)
      nRBs=logicalSubFrame(0).rows()/scPerRU;
    else
      nRBs=uplinkScheduledRBs.length();

    Array<cmat> physicalFrame(logicalSubFrame.size());
    int nSymbols=logicalSubFrame(0).cols();
    for (int txant_cnt=0;txant_cnt<logicalSubFrame.size();txant_cnt++)
    {
        physicalFrame(txant_cnt).set_size(fftSize,logicalSubFrame(0).cols());
        physicalFrame(txant_cnt).clear();
    }
    Array<ivec> phySC(2);///2=> no of slots Per RB
    int slotIndx;

    int scStart;
    for (int ru_cnt=0;ru_cnt<nRBs;ru_cnt++)
    {
        if (isDownlink)
	{
	  phySC=resourceMapper->dlResourceMapper.getPRBSubcarrierStartLocationinEachSlot(ru_cnt);
	  scStart=ru_cnt*scPerRU;
	}
        else
	{
	  phySC=resourceMapper->ulResourceMapper.getPRBSubcarrierStartLocationinEachSlot(uplinkScheduledRBs(ru_cnt));
	  scStart=uplinkScheduledRBs(ru_cnt)*scPerRU;
	}

        for (int sym_cnt=0;sym_cnt<nSymbols;sym_cnt++)
        {
            slotIndx=(sym_cnt>=(nSymbols/2));
            for (int sc_cnt=0;sc_cnt<scPerRU;sc_cnt++)
            {
                for (int txant_cnt=0;txant_cnt<logicalSubFrame.size();txant_cnt++)
                    physicalFrame(txant_cnt).set(phySC(slotIndx)(sc_cnt),sym_cnt,logicalSubFrame(txant_cnt)(scStart+sc_cnt,sym_cnt));
            }
        }
    }
    return(physicalFrame);

}



// Transmitter::Transmitter(NodeDevice * devPointer) {
// 
//     nodeID=-1;
//     antennaCount=-1;
//     centralUnitIndx=-1;
//     myNodeDevice=devPointer; 
// }

void Transmitter::setNodeID(int nodeId) {

    nodeID=nodeId;
}

int Transmitter::getNodeID() {

    return(nodeID);
}

void Transmitter::setAntennaCount(int antCount) {
  
 antennaCount = antCount;
}

int Transmitter::getAntennaCount() {
  
  return(antennaCount);
}

void Transmitter::initTransmitter(int nodeid, int antCount, NUParams_S& nuparams) {
  
    nodeID=nodeid;
    antennaCount=antCount;
    frameHandler.setAntennaCount(antennaCount);
    nuParams=nuparams;
}
// Array<TxFrameInfo_S> generateTxFrame(Array<Transmitter *> &myTransmitters, Array<SchedulerOutput_S > &schedulerOutput, SystemInfo_S &mySysInfo, bool isDownlink,int subframeNumber){
// 
//   cout<<endl<<"Error...generateTxFrame() function is no longer used, please use generateTxFrameNew() function."<<endl;abort();
// }

/*!\brief This module computes the ACI,based upon second network is in DL or UL,corresponding line of code need to be enabled */
// void createAci(MultiOpInfo multiopInfo,Array<TxFrameInfo_S> &myTxFrameInfo,ivec txNodes)  
// {
//   
//   ivec secondNetworkNodeIndices;
//   secondNetworkNodeIndices.set_size(0);
//   secondNetworkNodeIndices=getSecondNetworkNodeIndices(multiopInfo.secondNetworkNodes,txNodes);
//   ivec InterferingNodes =txNodes(secondNetworkNodeIndices); 
//   
//   
//   double bandGapinHz=multiopInfo.bandGapMHz*1.0e6; 
//   double subCarrSpacing = 15.0e3;  // Subcarrier Spacing 
//   int fftSize=myTxFrameInfo(0).txFrame(0).rows();
//   int upSamplingFactor;
//   double bandwidth;
//   cvec filterRespone;
//   
//   switch(fftSize)
//   {
//     case 128:
//       
//       bandwidth=1.4e6; 
//       upSamplingFactor=4;
//       break;
//       
//     case 256:
//       
//       bandwidth=3.0e6; 
//       upSamplingFactor=4;		  
//       break;
//       
//     case 512:
//       
//       bandwidth=5.0e6;
//       upSamplingFactor=4;
//       break;
//       
//     case 1024:
//       
//       bandwidth=10.0e6;
//       upSamplingFactor=4;
//       break;
//       
//     case 1536:
//       
//       bandwidth=15.0e6; 
//       upSamplingFactor=4;
//       break;
//       
//     case 2048:
//       
//       bandwidth=20.0e6;
//       upSamplingFactor=4;
//       break;
//       
//     default:
//       cout<<"Error... FFT size does not match any LTE bandwidth..."<<endl;abort(); 
//   }
//   cvec filterOutput;
//   filterOutput.set_size(0);
//   cvec interferenceFrame;
//   interferenceFrame.clear();
//   cvec timeDomainFrame;
//   
//   for(int tx_cnt=0;tx_cnt<myTxFrameInfo.length();tx_cnt++)
//   {
//     cvec temp;     
//     temp.zeros();
//     if(find(InterferingNodes,myTxFrameInfo(tx_cnt).nodeID)!=-1)  
//     {
//       
//       if(myTxFrameInfo(tx_cnt).isTransmitting)
// 	for(int ant_cnt=0;ant_cnt<myTxFrameInfo(tx_cnt).txFrame.length();ant_cnt++) 	  
// 	  for(int symb_cnt=0;symb_cnt<myTxFrameInfo(tx_cnt).txFrame(ant_cnt).cols();symb_cnt++)// perform symbol-by-symbol process, first 3-symbols are reserved for PDCCH
// 	  {
// 	    
// 	    timeDomainFrame=getTimeDomainFramePerSymbol(myTxFrameInfo(tx_cnt).txFrame(ant_cnt).get_col(symb_cnt),fftSize,symb_cnt);     
// 	    //!  starts up-sampling 
// 	    cvec oversampledTDFrame;
// 	    oversampledTDFrame.clear();
// 	    oversampledTDFrame=concat(timeDomainFrame,zeros_c(fftSize*(upSamplingFactor-1)));//! ends up-sampling	      
// 	    
// 	    vec coefficients="6.09164e-21 6.33603e-07 6.35219e-22 -6.5813e-07 1.11749e-20 6.83939e-07 -4.09816e-21 -7.11115e-07 1.70562e-20 7.39751e-07 -9.61213e-21 -7.69945e-07 1.53727e-21 8.01805e-07 -1.60387e-20 -8.35447e-07 7.52082e-21 8.70998e-07 1.74654e-21 -9.08595e-07 1.45521e-20 9.48387e-07 -4.74603e-21 -9.90538e-07 -5.95738e-21 1.03522e-06 -1.24463e-20 -1.08264e-06 1.08194e-21 1.13299e-06 -2.15848e-20 -1.18652e-06 9.51903e-21 1.24347e-06 3.74754e-21 -1.30413e-06 1.96357e-20 1.3688e-06 -5.49507e-21 -1.43781e-06 3.17786e-20 1.51155e-06 -1.67091e-20 -1.59041e-06 -4.42329e-24 1.67486e-06 -3.03262e-20 -1.76539e-06 1.24339e-20 1.86257e-06 7.50762e-21 -1.96701e-06 2.77407e-20 2.07941e-06 -6.28325e-21 -2.20054e-06 -1.77614e-20 2.33127e-06 -2.35209e-20 -2.47256e-06 -2.50265e-21 2.6255e-06 -4.50904e-20 -2.79132e-06 1.69259e-20 2.9714e-06 1.50373e-20 -3.16732e-06 4.16777e-20 3.38085e-06 -6.84295e-21 -3.61401e-06 7.32745e-20 3.86912e-06 -3.53245e-20 -4.14881e-06 -8.43462e-21 4.45613e-06 -7.24563e-20 -4.79457e-06 2.4378e-20 5.16818e-06 -4.47217e-20 -5.58163e-06 6.82603e-20 6.04039e-06 -6.15244e-21 -6.55085e-06 3.01007e-20 7.12049e-06 -5.82441e-20 -7.75814e-06 -2.3845e-20 8.47428e-06 -4.33072e-21 -9.28136e-06 3.8104e-20 1.01943e-05 -7.86041e-20 -1.1231e-05 1.27221e-19 1.24132e-05 1.7709e-23 -1.37674e-05 4.97834e-20 1.53259e-05 -1.11077e-19 -1.7129e-05 -7.11246e-20 1.92264e-05 1.00226e-20 -2.16808e-05 6.77921e-20 2.45716e-05 -1.66948e-19 -2.80002e-05 2.93553e-19 3.20978e-05 3.3796e-20 -3.70359e-05 9.76953e-20 4.30418e-05 -2.7361e-19 -5.04203e-05 1.20682e-19 5.95877e-05 9.56284e-20 -7.11208e-05 1.52864e-19 8.58354e-05 -5.10586e-19 -0.00010491 1.999e-19 0.000130088 2.85768e-19 -0.000164024 2.7259e-19 0.000210889 -1.18155e-18 -0.000277485 3.93742e-19 0.000375421 -4.87255e-19 -0.000525589 6.18737e-19 0.000768169 -8.12092e-19 -0.00118717 1.11373e-18 0.00197862 -1.62418e-18 -0.00367457 2.5987e-18 0.00808406 -4.87255e-18 -0.0242522 1.29935e-17 0.169765 0.5 0.848826 1 0.848826 0.5 0.169765 1.29935e-17 -0.0242522 -4.87255e-18 0.00808406 2.5987e-18 -0.00367457 -1.62418e-18 0.00197862 1.11373e-18 -0.00118717 -8.12092e-19 0.000768169 6.18737e-19 -0.000525589 -4.87255e-19 0.000375421 3.93742e-19 -0.000277485 -1.18155e-18 0.000210889 2.7259e-19 -0.000164024 2.85768e-19 0.000130088 1.999e-19 -0.00010491 -5.10586e-19 8.58354e-05 1.52864e-19 -7.11208e-05 9.56284e-20 5.95877e-05 1.20682e-19 -5.04203e-05 -2.7361e-19 4.30418e-05 9.76953e-20 -3.70359e-05 3.3796e-20 3.20978e-05 2.93553e-19 -2.80002e-05 -1.66948e-19 2.45716e-05 6.77921e-20 -2.16808e-05 1.00226e-20 1.92264e-05 -7.11246e-20 -1.7129e-05 -1.11077e-19 1.53259e-05 4.97834e-20 -1.37674e-05 1.7709e-23 1.24132e-05 1.27221e-19 -1.1231e-05 -7.86041e-20 1.01943e-05 3.8104e-20 -9.28136e-06 -4.33072e-21 8.47428e-06 -2.3845e-20 -7.75814e-06 -5.82441e-20 7.12049e-06 3.01007e-20 -6.55085e-06 -6.15244e-21 6.04039e-06 6.82603e-20 -5.58163e-06 -4.47217e-20 5.16818e-06 2.4378e-20 -4.79457e-06 -7.24563e-20 4.45613e-06 -8.43462e-21 -4.14881e-06 -3.53245e-20 3.86912e-06 7.32745e-20 -3.61401e-06 -6.84295e-21 3.38085e-06 4.16777e-20 -3.16732e-06 1.50373e-20 2.9714e-06 1.69259e-20 -2.79132e-06 -4.50904e-20 2.6255e-06 -2.50265e-21 -2.47256e-06 -2.35209e-20 2.33127e-06 -1.77614e-20 -2.20054e-06 -6.28325e-21 2.07941e-06 2.77407e-20 -1.96701e-06 7.50762e-21 1.86257e-06 1.24339e-20 -1.76539e-06 -3.03262e-20 1.67486e-06 -4.42329e-24 -1.59041e-06 -1.67091e-20 1.51155e-06 3.17786e-20 -1.43781e-06 -5.49507e-21 1.3688e-06 1.96357e-20 -1.30413e-06 3.74754e-21 1.24347e-06 9.51903e-21 -1.18652e-06 -2.15848e-20 1.13299e-06 1.08194e-21 -1.08264e-06 -1.24463e-20 1.03522e-06 -5.95738e-21 -9.90538e-07 -4.74603e-21 9.48387e-07 1.45521e-20 -9.08595e-07 1.74654e-21 8.70998e-07 7.52082e-21 -8.35447e-07 -1.60387e-20 8.01805e-07 1.53727e-21 -7.69945e-07 -9.61213e-21 7.39751e-07 1.70562e-20 -7.11115e-07 -4.09816e-21 6.83939e-07 1.11749e-20 -6.5813e-07 6.35219e-22 6.33603e-07 6.09164e-21";
// 	    filterOutput=performLinearConvolution(oversampledTDFrame,coefficients);
// 	    
// 	    cvec temp1=deleteCPtdFrame(filterOutput,fftSize,upSamplingFactor);//! deleting CP from time domain frame 
// 	    cvec freqDomainFrame = fft(temp1)/sqrt((double)temp1.length());
// 	    
// 	    int beginSubCarrs=floor_i((bandwidth/2+bandGapinHz)/subCarrSpacing);//calculating  begining subcarrier point for all bandwidths
// 	    interferenceFrame = freqDomainFrame.get(beginSubCarrs,beginSubCarrs+fftSize-1);//taking interferenceFrame of fftsize
// 	    
// 	    myTxFrameInfo(tx_cnt).txFrame(ant_cnt).set_col(symb_cnt,interferenceFrame);
// 	    
// 	  }//! ends for symb_cnt
// 	  
//     }//! ends for tx_cnt    
//   }  
// }


Array<TxFrameInfo_S> nodeGenerate(Array< Transmitter* >& myTransmitters, AssociationInfo_S& associationInfo, Array< SchedulerOutput_S >& schedulerOutput, int nuID, SystemInfo_S& mySysInfo, ivec txNodes, ivec txBeamIDs) {

  bool updateFrameStructure=true;
//   generateDataNew(schedulerOutput,myNodeDevices,nodeDeviceIDs,mySysInfo.isWithAbstraction,txNodes,nuID);
  
  ivec transmitterIDs(myTransmitters.length());
  for(int tx_cnt=0;tx_cnt<myTransmitters.length();tx_cnt++)
    transmitterIDs(tx_cnt)=myTransmitters(tx_cnt)->getNodeID();
  
  ivec txIndx;
  
  if(txNodes.length()!=0)
    txIndx=find(transmitterIDs,txNodes);
  else
  {
    txNodes=transmitterIDs;
    txIndx=getIntegers(0,transmitterIDs.length()-1);
  }
  
  int nodeID;
  bool isDownlink;
  Array<TxFrameInfo_S> txFrameInfo(txNodes.length());
  for(int tx_cnt=0;tx_cnt<txNodes.length();tx_cnt++)
  {
     nodeID = txNodes(tx_cnt);       
     int srvrIndx=find(mySysInfo.serverNodes,nodeID);
     if(srvrIndx!=-1)//!\brief to find out current TX in DL or UL
      isDownlink=1;
     else
     {
       isDownlink=0;
       int associatedNode=associationInfo.associatedServerNodesForEachServiceNode(find(associationInfo.serviceNodes,nodeID));
       srvrIndx=find(mySysInfo.serverNodes,associatedNode);
     }
     int nuIndx=myTransmitters(txIndx(tx_cnt))->myNodeDevice->getNUIndx(nuID);
     
     SchedulerToTransmitterInfo_S schToTxInfo;
     int schToTxInfoIndx= schedulerOutput(srvrIndx).getSchedulerToTransmitterInfoIndx(myTransmitters(txIndx(tx_cnt))->getNodeID(),nuID, txBeamIDs(tx_cnt));
     if(schToTxInfoIndx!=-1)
     {
      schToTxInfo = schedulerOutput(srvrIndx).schedulerToTransmitterInfo(schToTxInfoIndx);
      schedulerOutput(srvrIndx).deleteSchedulerToTransmitterInfo(myTransmitters(txIndx(tx_cnt))->getNodeID(),nuID,txBeamIDs(tx_cnt));
     }
     else
     {
       // This is for transmitting RS even when the node is not scheduled.
       schToTxInfo.txNodeID=nodeID;
       schToTxInfo.ttiCount=myTransmitters(txIndx(tx_cnt))->myNodeDevice->ttiCountPerNU(nuIndx);
       schToTxInfo.isTransmitting=true;
       schToTxInfo.txBeamID=txBeamIDs(tx_cnt);
       schToTxInfo.txNUID=nuID;
     }
  
   //This is the main function where Frame from L1 core is copied to transmit frame..
   
     txFrameInfo(tx_cnt)=myTransmitters(txIndx(tx_cnt))->transmitFrame(mySysInfo,isDownlink,schToTxInfo);    
  
//     if(isDownlink && mySysInfo.FFR_RBGScaling.length()) //FFR is enabled
//       applyRBGSCalingForFFR(txFrameInfo(tx_cnt).txFrame, mySysInfo.FFR_RBGScaling(txFrameInfo(tx_cnt).idCell%3),myNodeDevices(deviceIndices(tx_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getResourceBlockGroupingSize() , myNodeDevices(deviceIndices(tx_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getResourceBlocksPerSubframe(), myNodeDevices(deviceIndices(tx_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getGuardSubcarriersInOneSide(),myNodeDevices(deviceIndices(tx_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getCurrentFrameStructurePointer());
  }

//   if(mySysInfo.multiopInfo.isMultiOperator)
//     createAci(mySysInfo.multiopInfo,txFrameInfo,txNodes); 
  
  for(int tx_cnt=0;tx_cnt<txNodes.length();tx_cnt++)
  {
    txFrameInfo(tx_cnt).timeDomainSubframe = getTimeDomainFrame(txFrameInfo(tx_cnt).txFrame,txFrameInfo(tx_cnt).txFrame(0).rows());
    txFrameInfo(tx_cnt).isTimeDomainSubframeAvailable = 1;
  }
  
  return txFrameInfo;
  
//   if(myTxFrameInfo.length()==0)
//     myTxFrameInfo=txFrameInfo;
//   else
//   {
//     ivec txFrameInfoIndx=getTxframeIndx(txNodes,txBeamIDs,nuID,myTxFrameInfo);
//     for(int cnt=0;cnt<txFrameInfoIndx.length();cnt++)
//     {
//       if(txFrameInfoIndx(cnt)==-1)
// 	append(myTxFrameInfo,txFrameInfo(cnt));
//       else
// 	myTxFrameInfo(txFrameInfoIndx(cnt))=txFrameInfo(cnt);
//     }
//   }
  
}

TxFrameInfo_S Transmitter::transmitFrame(SystemInfo_S &mySysInfo, bool isDownlink,SchedulerToTransmitterInfo_S schedulerToTransmitterInfo) {
  
  if (schedulerToTransmitterInfo.txNodeID!=nodeID)
  {
    cout<<"[both:]Error. Unmatched SchedulerToTransmitterInfo_S passed to transmitFrame()."<<endl;
    abort();
  }
  
  int subframeNumber=schedulerToTransmitterInfo.ttiCount;
  TxFrameInfo_S myFrameInfo;
  int nBlks=schedulerToTransmitterInfo.dataBlkInfo.length();
  ivec resourceBlocks;
  Size_S rbSize;
  
  int nuIndx=myNodeDevice->getNUIndx(schedulerToTransmitterInfo.txNUID);
//   frameHandler.setDataBlkInfo(schedulerToTransmitterInfo.dataBlkInfo);
  if(isDownlink)
  {
    myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure.setCurrentFrameType("dlFrame");
//     cout<<"check the BW parts ::"<<myNodeDevice->L1CorePerNU(nuIndx).nuParameters.BSBWPLocations(0)<<endl;
    frameHandler.pointFrameStructure(&myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure);
  }
  else
  {
    myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure.setCurrentFrameType("ulFrame");
    frameHandler.pointFrameStructure(&myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure);
  }
  
 
  myFrameInfo.nodeID=nodeID;
  myFrameInfo.nuParams=nuParams;
  myFrameInfo.ttiCount=schedulerToTransmitterInfo.ttiCount;
  myFrameInfo.isTransmitting=schedulerToTransmitterInfo.isTransmitting; 
  myFrameInfo.nTxAntenna=antennaCount;
  myFrameInfo.txSubcarrierPowerInDBmPerAntenna=schedulerToTransmitterInfo.txSubcarrierPowerInDBmPerAntenna;
  myFrameInfo.idCell=myNodeDevice->L1CorePerNU(nuIndx).cellID;  /// All dataBlk Info to a particular transmitter should have same cellID
  myFrameInfo.txBeamID=schedulerToTransmitterInfo.txBeamID;
  
  if ((!mySysInfo.isWithAbstraction || (mySysInfo.isWithAbstraction && (mySysInfo.AbstractionModel == _ABSTRACTION_DECODER_ || mySysInfo.AbstractionModel == _ABSTRACTION_IDEAL_))) && schedulerToTransmitterInfo.isTransmitting)
  {
    int nSymbolsmappedPerDB,nSymbolsmappedPerLayerPerRB; //! Number of symbols transmitted in RB-pair
    Array< Array<cvec> > modulatedSymbolsPerLayer(nBlks);
    Array<cvec> precodedLayersPerRB;
    DataBlkInfo_S *myDataBlkInfo;
    Array<cvec> precodedLayersAcrossPhysicalAntennas;
    RBInfo_S *rbInfoForUnScheduled;
    if (isDownlink)
      rbInfoForUnScheduled=&myNodeDevice->L1CorePerNU(nuIndx).dlRbInfoForUnScheduled;
    else
      rbInfoForUnScheduled=&myNodeDevice->L1CorePerNU(nuIndx).ulRbInfoForUnScheduled;
    
    frameHandler.initCurrentFrame();
    
    ivec allRBs=getIntegers(0,frameHandler.getAllRuCount()-1);
    string toneType="";
    // nBlks is similar to number of Scheudled UEs....
    if (nBlks>0)  
    {
      rbSize.numberOfSubcarriers=schedulerToTransmitterInfo.dataBlkInfo(0).dataGenInfo.rbInfo.rsPositionMatrix.rows();
      rbSize.numberOfSymbols=schedulerToTransmitterInfo.dataBlkInfo(0).dataGenInfo.rbInfo.rsPositionMatrix.cols();
      
      ivec portsUsedInSubframe(0);
      for (int blk_cnt=0;blk_cnt<nBlks;blk_cnt++)
      {
	append(portsUsedInSubframe,schedulerToTransmitterInfo.dataBlkInfo(blk_cnt).dataGenInfo.rbInfo.rsInfo.rsPorts);
	if(schedulerToTransmitterInfo.dataBlkInfo(blk_cnt).dataGenInfo.rbInfo.rsInfo.otherDemodPorts.length())
	  remove(portsUsedInSubframe,schedulerToTransmitterInfo.dataBlkInfo(blk_cnt).dataGenInfo.rbInfo.rsInfo.otherDemodPorts);
      }      
      append(portsUsedInSubframe,rbInfoForUnScheduled->rsInfo.rsPorts);
      
     
      portsUsedInSubframe=findUniqueNumbers(portsUsedInSubframe);
//       if (isDownlink)
      generateRS(myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,frameHandler.getAllRuCount(),myNodeDevice->L1CorePerNU(nuIndx).cellID,subframeNumber%mySysInfo.subFramesPerFrame, portsUsedInSubframe,*rbInfoForUnScheduled,schedulerToTransmitterInfo.dataBlkInfo, schedulerToTransmitterInfo.serviceNodesForSourceAndSink,isDownlink);
    }
    else
    {
      rbSize.numberOfSubcarriers=rbInfoForUnScheduled->rsPositionMatrix.rows();
      rbSize.numberOfSymbols=rbInfoForUnScheduled->rsPositionMatrix.cols();
     
      if (isDownlink)
	generateRS(myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,frameHandler.getAllRuCount(),myNodeDevice->L1CorePerNU(nuIndx).cellID,subframeNumber%mySysInfo.subFramesPerFrame, rbInfoForUnScheduled->rsInfo.rsPorts,*rbInfoForUnScheduled,schedulerToTransmitterInfo.dataBlkInfo, schedulerToTransmitterInfo.rxNodeIDs,isDownlink);
    }
    Array<ivec> dataBlkInfoIndicesForallRBs(allRBs.length());
    for (int rb_cnt=0;rb_cnt<allRBs.length();rb_cnt++)
      dataBlkInfoIndicesForallRBs(rb_cnt).set_size(0);
    
    // Copying modulatedSymbols & other control informations for each DataBlockInfo(similar to each Receiver), to RB structure
//     cout<<"nBlks : "<<nBlks<<endl;
    for (int blk_cnt=0;blk_cnt<nBlks;blk_cnt++)
    {
      myDataBlkInfo=&schedulerToTransmitterInfo.dataBlkInfo(blk_cnt);
      
//       cout<<"Transmitting from : "<<nodeID<<" over "<<myDataBlkInfo->resourceBlocks<<" for UE : "<<schedulerToTransmitterInfo.rxNodeIDs(blk_cnt)<<endl;
      if (!myDataBlkInfo->dataGenInfo.isReady())
      {
        cout<<"[both:]Error. Datagen info not ready."<<endl;
        abort();
      }
      nSymbolsmappedPerDB = sum(myDataBlkInfo->dataGenInfo.resourceElementsPerRB)*myDataBlkInfo->dataGenInfo.nLayers/myDataBlkInfo->dataGenInfo.symbolRepetitionFactor;
      modulatedSymbolsPerLayer(blk_cnt)=performLayerMapping(myDataBlkInfo->modulatedSymbols,myDataBlkInfo->dataGenInfo.nCodeWords,myDataBlkInfo->dataGenInfo.nLayers);
      
      if(myDataBlkInfo->waveform == _Waveform_SCFDMA_)
        modulatedSymbolsPerLayer(blk_cnt)=performTransformPrecodingForUplink(modulatedSymbolsPerLayer(blk_cnt),true,myDataBlkInfo->resourceBlocks.length()*rbSize.numberOfSubcarriers/myDataBlkInfo->dataGenInfo.symbolRepetitionFactor);
      
      if (nSymbolsmappedPerDB!=myDataBlkInfo->dataGenInfo.nLayers*modulatedSymbolsPerLayer(blk_cnt)(0).length())
      {
          cout<<"struct : "<<frameHandler.getResourceUnitMatrix(myDataBlkInfo->resourceBlocks(0))<<endl;  
        cout<<nSymbolsmappedPerDB<<","<<myDataBlkInfo->dataGenInfo.nLayers*modulatedSymbolsPerLayer(blk_cnt)(0).length()<<endl;
        cout<<"[both:]Error. Length mismatch between data generated and data required."<<endl;
        abort();
      }
      //Assuming rbIndx=rbCnt (will create problem if rb indexing is changed)
	
      for (int rb_cnt=0;rb_cnt<myDataBlkInfo->resourceBlocks.length();rb_cnt++)
	append(dataBlkInfoIndicesForallRBs(myDataBlkInfo->resourceBlocks(rb_cnt)),blk_cnt);
//        if (!isDownlink )
//        fillMyUplinkReferenceSignal(myNodeDevice->L1CorePerNU(nuIndx).cellID,&frameHandler,&myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper,schedulerToTransmitterInfo.dataBlkInfo(blk_cnt).resourceBlocks,frameHandler.getRuCountPerLevel(),schedulerToTransmitterInfo.dataBlkInfo(blk_cnt).dataGenInfo.rbInfo.rsInfo.rsPorts,&myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,&schedulerToTransmitterInfo.dataBlkInfo(blk_cnt).dataGenInfo.rbInfo);
    }
    ivec dataPositionPointer=zeros_i(nBlks);
    Array<cvec> modulatedSymbolsPerLayerPerRB;
    Array<cvec> combinedLayersPerRB;
//     int rbIndxInDataBlk, nUsers=2;
    // Filling the frame, each RB by RB wise...
    
    ivec xAllocation=myNodeDevice->L1CorePerNU(nuIndx).nuParameters.BSBWPLocations(0);//Assuming no carrier aggregation or multiple BWP
    
    for(int rb_cnt=0;rb_cnt<allRBs.length();rb_cnt++)
    {
      if (dataBlkInfoIndicesForallRBs(rb_cnt).length()!=0)
      {
          myDataBlkInfo=&schedulerToTransmitterInfo.dataBlkInfo(dataBlkInfoIndicesForallRBs(rb_cnt)(0));
          combinedLayersPerRB.set_length(myDataBlkInfo->dataGenInfo.nLayers);
          
          for (int layer_cnt=0;layer_cnt<combinedLayersPerRB.length();layer_cnt++)
              combinedLayersPerRB(layer_cnt).set_length(0);
          
          // MU-MIMO
          
          for (int dataBlk_cnt=0;dataBlk_cnt<dataBlkInfoIndicesForallRBs(rb_cnt).length();dataBlk_cnt++) 
          {
              myDataBlkInfo=&schedulerToTransmitterInfo.dataBlkInfo(dataBlkInfoIndicesForallRBs(rb_cnt)(dataBlk_cnt));
              nSymbolsmappedPerLayerPerRB = myDataBlkInfo->dataGenInfo.resourceElementsPerRB(find(myDataBlkInfo->resourceBlocks,allRBs(rb_cnt)))/myDataBlkInfo->dataGenInfo.symbolRepetitionFactor;
              modulatedSymbolsPerLayerPerRB=getFromArrayOfVector(modulatedSymbolsPerLayer(dataBlkInfoIndicesForallRBs(rb_cnt)(dataBlk_cnt)),dataPositionPointer(dataBlkInfoIndicesForallRBs(rb_cnt)(dataBlk_cnt)),dataPositionPointer(dataBlkInfoIndicesForallRBs(rb_cnt)(dataBlk_cnt))+nSymbolsmappedPerLayerPerRB-1);
              dataPositionPointer(dataBlkInfoIndicesForallRBs(rb_cnt)(dataBlk_cnt))+=nSymbolsmappedPerLayerPerRB;
              // 	  rbIndxInDataBlk=find(myDataBlkInfo->resourceBlocks,allRBs(rb_cnt));
              combinedLayersPerRB=modulatedSymbolsPerLayerPerRB;
              // using the same myDataBlkInfo as info other than rsports and scheduledLayers are same for the rb;
              precodedLayersPerRB=precodeDataInRB(combinedLayersPerRB,*myDataBlkInfo,allRBs(rb_cnt));
              precodedLayersAcrossPhysicalAntennas= myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper.mapPortsToPhysicalAntennas(precodedLayersPerRB,frameHandler.frame.length(),myDataBlkInfo->dataGenInfo.transmissionScheme < 6);
              
              //! DL power-allocation 
              //====================================================================================================
              // Code to apply Downlink Power allocation (3gpp 36.213 - Section 5.2) difference between
              // OFDM symbols with and without CRS
              //====================================================================================================
//               if(isDownlink)
//                   if(myNodeDevice->L1CorePerNU(nuIndx).dlTxConfig.enableVarPowInRB==true)
//                   {                    
//                       ivec dataTonerLoc = frameHandler.getRow1stToneIndicesInRU(allRBs(rb_cnt),"data");                    
//                       cvec PowerVec; PowerVec.set_size(dataTonerLoc.length());PowerVec.zeros();
//                       int rbindex = find(myDataBlkInfo->resourceBlocks,allRBs(rb_cnt));
//                       for(int dataToneInd = 0 ; dataToneInd < dataTonerLoc.length();dataToneInd++)
//                           PowerVec(dataToneInd) = sqrt(myDataBlkInfo->dataGenInfo.rbInfo.dlPowMask(rbindex)(dataTonerLoc(dataToneInd)));	     
//                       
//                       for(int txAntcnt =0;txAntcnt < myFrameInfo.nTxAntenna;txAntcnt++)
//                           precodedLayersAcrossPhysicalAntennas(txAntcnt)=elem_mult(PowerVec,precodedLayersAcrossPhysicalAntennas(txAntcnt));
//                   }
                  //=====================================================================================================
            frameHandler.setTonesInRU(precodedLayersAcrossPhysicalAntennas,myDataBlkInfo->symPos,allRBs(rb_cnt),"data",true);
            cmat precoder;
            precoder=myDataBlkInfo->getMyPrecoder(allRBs(rb_cnt));
            fillRS(myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,allRBs(rb_cnt),myDataBlkInfo->dataGenInfo.rbInfo,rbSize,frameHandler,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,&myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper,isDownlink, precoder);
          }
          
          
      }
      // If Transmitter is not scheduled, then fill the Reference Signals only...
      else if (isDownlink)
      {
          if(allRBs(rb_cnt)>=xAllocation(0) && allRBs(rb_cnt)<=xAllocation(1)) // checking if the RB is in my BWP and transmit RS only then - dhiv
              fillRS(myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,allRBs(rb_cnt),myNodeDevice->L1CorePerNU(nuIndx).dlRbInfoForUnScheduled,rbSize,frameHandler,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,&myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper,true);
      }
    }
    
      if(nBlks)
      myFrameInfo.txFrame=mapLogicalToPhysicalSubFrame(frameHandler.frame,myNodeDevice->L1CorePerNU(nuIndx).nuParameters.fftSize,rbSize,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,isDownlink/*,!schedulerToTransmitterInfo.dataBlkInfo(0).isLocalised*/,schedulerToTransmitterInfo.dataBlkInfo(0).resourceBlocks);
    else
      myFrameInfo.txFrame=mapLogicalToPhysicalSubFrame(frameHandler.frame,myNodeDevice->L1CorePerNU(nuIndx).nuParameters.fftSize,rbSize,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,isDownlink);

    frameHandler.frame.set_length(0);
    // Fill control channel
     if(isDownlink && mySysInfo.isControlChannelEnabled)
       fillDLControlChannels(myFrameInfo.txFrame,schedulerToTransmitterInfo,mySysInfo,subframeNumber,allRBs.length());

    for (int ant_cnt=0;ant_cnt<myFrameInfo.txFrame.length();ant_cnt++)
      myFrameInfo.txFrame(ant_cnt)*=sqrt(inv_dB(schedulerToTransmitterInfo.txSubcarrierPowerInDBmPerAntenna-30));
    
    if(nBlks)
      if(traceValuePerTraceName["frameHandlerLevel01TraceInfo"]=="printInTx")
      {
	cout<<"////////////////////////////////////////////////////////////////"<<endl;
	cout<<"/////////////////Initiating FrameHandler Testing for TxNode : "<<nodeID<<" ////////"<<endl;
	cout<<"////////////////////////////////////////////////////////////////"<<endl;
	frameHandler.printAllTones(schedulerToTransmitterInfo.dataBlkInfo(0).resourceBlocks(0),"fhTraces.txt");
	traceValuePerTraceName["frameHandlerLevel01TraceInfo"]="onHold";
      }
  }

  return myFrameInfo;
  
}

 void Transmitter::fillDLControlChannels(Array<cmat>& txFrame,SchedulerToTransmitterInfo_S& schedulerToTransmitterInfo,SystemInfo_S& mySysInfo, int subframeNumber,int nDLRBs){
   
   
  int pdcchCount = 2;
  int nuIndx     = this->myNodeDevice->getNUIndx(schedulerToTransmitterInfo.txNUID);
  int fftSize    = this->myNodeDevice->L1CorePerNU(nuIndx).nuParameters.fftSize;
  int cellID     = this->myNodeDevice->L1CorePerNU(nuIndx).cellID;
  RBInfo_S rbInfoForUnScheduled  = this->myNodeDevice->L1CorePerNU(nuIndx).dlRbInfoForUnScheduled;

  GenerateAndFillCoreset(schedulerToTransmitterInfo, txFrame,
                         rbInfoForUnScheduled, subframeNumber, pdcchCount,
                         cellID, fftSize, nDLRBs);
  
 // int nCRSPorts=txFrame.length();
 // PHICH_Duration phichDuration = _NORMAL_;
 // PHICH_Resource phichResource = _ONE_;
        Array<cmat> txFramein;
     if(mod(subframeNumber,5)==0){
     txFramein=txFrame;
     }
//    PSS and SSS
//    if(myNodeDevice->L1CorePerNU(0).dlTxConfig.cchInfo.enableSSB)
//      GenerateAndFillPSSandSSS(txFrame,subframeNumber,myNodeDevice->L1CorePerNU(0).cellID);
//    
     if(mod(subframeNumber,5)==0){
     Array<cmat> txFrameout;
     txFrameout=txFrame;
    }

//    PBCH
//    if(mySysInfo.cchInfo.enablePBCH)
//      GenerateAndFillPBCH(schedulerToTransmitterInfo,txFrame,nDLRBs,nCRSPorts,subframeNumber,phichDuration,phichResource);
   
//    PCFICH. PHICH and PDCCH
//   if(mySysInfo.cchInfo.pdcchCount>0)
//   {
//     ivec PCFICHTonesLogical;
//     ivec PCFICHTonesFFT=GenerateAndFillPCFICH(txFrame,nDLRBs,nCRSPorts,subframeNumber,mySysInfo.cchInfo.pdcchCount,schedulerToTransmitterInfo.cellID,PCFICHTonesLogical);
//     ivec PHICHTonesFFT=GenerateAndFillPHICH(schedulerToTransmitterInfo,txFrame,subframeNumber,schedulerToTransmitterInfo.cellID,PCFICHTonesLogical,schedulerToTransmitterInfo.rxNodeIDs,nDLRBs,nCRSPorts);
//     if(mySysInfo.fapiInfo.isEnabled && find(mySysInfo.fapiInfo.serverNodesWithFAPIInterface, schedulerToTransmitterInfo.txNodeID)!=-1)
//       GenerateAndFillPDCCHFromL2(schedulerToTransmitterInfo,txFrame,nDLRBs,nCRSPorts,subframeNumber,phichResource,mySysInfo.cchInfo.pdcchCount,schedulerToTransmitterInfo.cellID,PCFICHTonesFFT,PHICHTonesFFT);
//   }
//    if(mySysInfo.cchInfo.enableEPDCCH)
//    {
//      int nuIndx=myNodeDevice->getNUIndx(schedulerToTransmitterInfo.txNUID);
//      GenerateAndFillEpdcch(schedulerToTransmitterInfo,myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure,myNodeDevice->L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper,myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper,txFrame,subframeNumber,mySysInfo.cchInfo.pdcchCount,schedulerToTransmitterInfo.cellID,schedulerToTransmitterInfo.rxNodeIDs,nDLRBs,nCRSPorts);
//    }
}

ivec getTxframeIndx(ivec NodeIds, ivec beamIDs, int nuID, Array<TxFrameInfo_S> &myTxFrameInfo)
{
  ivec myTxFrameIndxs=zeros_i(NodeIds.length())-1;
  //captureTime(124,RERUN);
  for(int myTxFrameIndx =0;myTxFrameIndx<myTxFrameInfo.length();myTxFrameIndx++)   
  { 
    for(int NodeIdIndx =0;NodeIdIndx<NodeIds.length();NodeIdIndx++)
    {
        if(myTxFrameInfo(myTxFrameIndx).nodeID==NodeIds(NodeIdIndx) && myTxFrameInfo(myTxFrameIndx).nuParams.nuID==nuID && myTxFrameInfo(myTxFrameIndx).txBeamID==beamIDs(NodeIdIndx)) 	{	 
	//  captureTime(124,PAUSE);
	myTxFrameIndxs(NodeIdIndx)=myTxFrameIndx;
      }
    }
  }
/*    if(find(myTxFrameIndxs,-1)!=-1) 
    {
      cout<<"Undefined NodeID for getting transmit frame index ..."<<endl;  
      abort();
    }  */ 	     
    return(myTxFrameIndxs);
}

// ivec getTxframeIndx(ivec NodeIds,Array<TxFrameInfo_S> myTxFrameInfo)
// {
//   ivec myTxFrameIndxs=zeros_i(NodeIds.length())-1;
//   //captureTime(124,RERUN);
//   for(int myTxFrameIndx =0;myTxFrameIndx<myTxFrameInfo.length();myTxFrameIndx++)   
//   { 
//     for(int NodeIdIndx =0;NodeIdIndx<NodeIds.length();NodeIdIndx++)
//     {
//       if(myTxFrameInfo(myTxFrameIndx).nodeID==NodeIds(NodeIdIndx)) 	{	 
// 	//  captureTime(124,PAUSE);
// 	myTxFrameIndxs(NodeIdIndx)=myTxFrameIndx;
//       }
//     }
//   }
//   if(find(myTxFrameIndxs,-1)!=-1) 
//   {
//    cout<<"Undefined NodeID for getting transmit frame index ..."<<endl;  
//    abort();
//   }   	     
//   return(myTxFrameIndxs);
// }


// void generateDataNew(Array< SchedulerOutput_S >& schedulerOutput, Array<NodeDevice> &myNodeDevices, ivec nodeDeviceIDs ,bool isWithAbstraction, ivec txNodeIDs, int nuID){
//   
//   cout<<"tx node IDS ::"<<txNodeIDs<<endl;
//   
//   ivec deviceIndices=find(nodeDeviceIDs,txNodeIDs);
//   for(int sch_cnt=0;sch_cnt<schedulerOutput.length();sch_cnt++)
//   {
//     for(int schToTx_cnt=0;schToTx_cnt<schedulerOutput(sch_cnt).schedulerToTransmitterInfo.length();schToTx_cnt++)
//     {
//       int txNodeIdx=find(txNodeIDs,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).txNodeID);
//       if(txNodeIdx!=-1 && schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).txNUID==nuID)  
//       {
// 	int ttiCount = schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).ttiCount;
// 	int nuIndx=myNodeDevices(deviceIndices(txNodeIdx)).getNUIndx(nuID);
// 	if(schedulerOutput(sch_cnt).isSchToTxInfoForDownlink(schToTx_cnt)==1)//! Downlink
// 	{
// 	  bool isDownlink=true;
// 	  ivec rxNodeDeviceIndices=find(nodeDeviceIDs,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).rxNodeIDs);
// 	  int carrier_cnt=find(myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).nuParameters.carrierFrequenciesInMHz,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).carrierFreqInMHz);
// 	  myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.setCurrentFrameType("dlFrame");
// 	  myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.loadDLResourceMapperForBandwidth(myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.getRuCountPerLevel());
// 	  if(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo.length())
// 	  {
// 	    ivec srvcNodeIndices=schedulerOutput(sch_cnt).getSchedulerToReceiverInfoIndx(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).rxNodeIDs,nuID);
// 	    bvec isTheRBScheduled=zeros_b(myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.getAllRuCount());
// 	    for(int db_cnt=0;db_cnt<schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo.length();db_cnt++)
// 	    {
// 	      setInVec(isTheRBScheduled,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).resourceBlocks,ones_b(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).resourceBlocks.length()));
// 	      RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo.rsPorts,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).resourceBlocks,true,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).isLocalised,&myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameReservationInfo);
// 	      schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB.set_length(0);
// 	      for(int rb_cnt=0;rb_cnt<schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).resourceBlocks.length();rb_cnt++)
// 	      {
// 		myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.updateRUType(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 		append(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB,myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.getToneCountPerRU(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),"data"));
// 	      }
// 	    }
// 	    
// 	    ivec unScheduledRBs=find(to_ivec(isTheRBScheduled),0,true);
// 	    {
// 	      myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo=myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRSInfoForUnScheduled;
// 	      
// 	      cout<<"myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.isCQIComputed"<<myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.isCQIComputed<<endl;
// 	      if(!myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.isCQIComputed && schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).dataGenInfo.transmissionScheme > 7)
// 	      {
// 		// 		myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts=remove(myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIports);
// 	      }
// 	      
// 	      cout<<"check RS ports at receiver ::"<<myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts<<endl;
// 	      if(myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameReservationInfo.cchInfo.enableEPDCCH)
// 	      {
// 		ivec epdcchRBs=myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameReservationInfo.reservedVRBsForEpdcch;
// 		if(epdcchRBs.length())
// 		{
// 		  unScheduledRBs=remove(unScheduledRBs,epdcchRBs);
// 		  ivec DMRSPorts;
// 		  if(myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.CQIports.length()>2)
// 		    DMRSPorts=ivec("7 8 9 10");
// 		  else
// 		    DMRSPorts=ivec("7 8");
// 		  
// 		  RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure,concat(myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,DMRSPorts),myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameReservationInfo.reservedVRBsForEpdcch,true,myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.isLocalised(epdcchRBs(0)),&myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameReservationInfo);
// 		  for(int rb_cnt=0;rb_cnt<epdcchRBs.length();rb_cnt++)
// 		    myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.updateRUType(epdcchRBs(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 		  
// 		}
// 	      }
// 	      
// 	      
// 	      if(unScheduledRBs.length())
// 	      {
// 		RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure,myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,unScheduledRBs,true,myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.isLocalised(unScheduledRBs(0)),&myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameReservationInfo);
// 		for(int rb_cnt=0;rb_cnt<unScheduledRBs.length();rb_cnt++)
// 		  myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.updateRUType(unScheduledRBs(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 	      }
// 	    }
// 	    myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.updateCurrentFrameStructure();
// 	    for(int db_cnt=0;db_cnt<schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo.length();db_cnt++)
// 	    {
// 	      int rxNUIndx=myNodeDevices(rxNodeDeviceIndices(db_cnt)).getNUIndx(nuID);
// 	      schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix=myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).resourceBlocks(0));
// 	      schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure=*myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.getCurrentFrameStructurePointer();
// 	      schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).reIndxForPrecoding.set_length(0);
// 	      
// 	      //!To get the RE indx while precoding for TM3 (May not be useful for other modes)
// 	      if(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_LARGE_DELAY_CDD_)
// 	      {
// 		ivec myRBs=schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).resourceBlocks;
// 		Size_S rbSize=myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitSize("nullRB");
// 		imat allocationStructure(0,0);
// 		allocationStructure=schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure.get_rows(myRBs(0)*rbSize.numberOfSubcarriers,myRBs(0)*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1);
// 		for(int rb_cnt=1;rb_cnt<myRBs.length();rb_cnt++)
// 		  appendVertical(allocationStructure,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure.get_rows(myRBs(rb_cnt)*rbSize.numberOfSubcarriers,myRBs(rb_cnt)*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1));
// 		
// 		imat dataIndxingStructure=allocationStructure;
// 		
// 		ivec dataREs=find(allocationStructure,0,true);
// 		ivec reIndx=getIntegers(0,dataREs.length()-1);
// 		setInMatrix(dataIndxingStructure,dataREs,reIndx);
// 		
// 		for(int rb_cnt=0;rb_cnt<myRBs.length();rb_cnt++)
// 		{
// 		  imat rbStructure=allocationStructure.get_rows(rb_cnt*rbSize.numberOfSubcarriers,rb_cnt*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1);
// 		  ivec localDataREs=find(rbStructure,0,true);
// 		  rbStructure=dataIndxingStructure.get_rows(rb_cnt*rbSize.numberOfSubcarriers,rb_cnt*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1);
// 		  ivec dataREIndx=getFromMatrix(rbStructure,localDataREs);
// 		  append(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).reIndxForPrecoding,dataREIndx);
// 		}
// 	      }
//       
// 	      schedulerOutput(sch_cnt).schedulerToReceiverInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).dataGenInfo.rbInfo.rsPositionMatrix=schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix;
// 	      schedulerOutput(sch_cnt).schedulerToReceiverInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).dataGenInfo.resourceElementsPerRB=schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB;
// 	      schedulerOutput(sch_cnt).schedulerToReceiverInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).dataGenInfo.rbInfo.logicalSubFrameStructure=*myNodeDevices(rxNodeDeviceIndices(db_cnt)).L1CorePerNU(rxNUIndx).dlFrameStructure.getCurrentFrameStructurePointer();
// 	      schedulerOutput(sch_cnt).schedulerToReceiverInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).reIndxForPrecoding=schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).reIndxForPrecoding;
// 	      
// 	      myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).downlinkSourceAndSink.setTransmissionStatusInHarqManager(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).rxNodeIDs(db_cnt),schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).harqInfo,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).ttiCount);
// 	      myNodeDevices(rxNodeDeviceIndices(db_cnt)).L1CorePerNU(rxNUIndx).downlinkSourceAndSink.setTransmissionStatusInHarqManager(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).rxNodeIDs(db_cnt),schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).harqInfo,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).ttiCount);
// 	     
// 	      if(!isWithAbstraction)
// 	      {
// 		schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).modulatedSymbols=myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).downlinkSourceAndSink.getModulatedSymbols(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).rxNodeIDs(db_cnt),&schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).dataGenInfo,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).harqInfo,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).ttiCount,isDownlink);
// 		schedulerOutput(sch_cnt).schedulerToReceiverInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).modulatedSymbols=schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).modulatedSymbols;
// 	      }
// 	      else
// 	      {
// 		myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).downlinkSourceAndSink.initializeHarqProcess(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).rxNodeIDs(db_cnt),&schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).dataGenInfo,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).harqInfo,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).ttiCount,isDownlink);
// 		myNodeDevices(rxNodeDeviceIndices(db_cnt)).L1CorePerNU(rxNUIndx).downlinkSourceAndSink.initializeHarqProcess(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).rxNodeIDs(db_cnt),&schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).dataGenInfo,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).harqInfo,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).ttiCount,isDownlink);
// 	      }
// 	      schedulerOutput(sch_cnt).schedulerToReceiverInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).harqInfo=schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).harqInfo;
// 	      
// 	    }
// 	    
// 	    //! DL power-allocation 
// 	    
// 	    //====================================================================================================
// 	    // Code to apply Downlink Power allocation (3gpp 36.213 - Section 5.2) difference between
// 	    // OFDM symbols with and without CRS
// 	    // Following code section creates Power mask to apply on PDSCH RE
// 	    //====================================================================================================
// 	    if(myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).downlinkSourceAndSink.lteTables.dlVarPowMask.isInitialized)
// 	    {
// 	      
// 	      if(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo.length())
// 	      {
// 		if(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).dataGenInfo.rbInfo.rsInfo.DeModrsType != _RSTYPE_CRS_)
// 		{cout<<"[both: ] Error Power Mask support provided only for CRS reference signals. Support not provided for CSI-RS and DMRS reference signals";exit(1);}
// 		int totalScheduledRBPerSrvrNode = 0;
// 		for(int serviceNode_cnt =0; serviceNode_cnt<schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo.length();serviceNode_cnt++)
// 		  for(int rbcnt = 0; rbcnt < schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(serviceNode_cnt).resourceBlocks.length();rbcnt++)
// 		    totalScheduledRBPerSrvrNode++;
// 		  imat logicalSubFrameStructure = schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).dataGenInfo.rbInfo.logicalSubFrameStructure;
// 		if(totalScheduledRBPerSrvrNode != (logicalSubFrameStructure.rows() / myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitInfo("nullRB").resourceUnitSize.numberOfSubcarriers))
// 		{cout<<"[both: ] Error No unscheduled RB in a Enode-B supported while applying power mask";exit(1);}
// 		
// 		generateDLPowerMask(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo,schedulerOutput(sch_cnt),myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure,myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).downlinkSourceAndSink.lteTables, schToTx_cnt,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).Pb);
// 	      }
// 	      for(int db_cnt=0;db_cnt<schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo.length();db_cnt++)
// 	      {
// 		schedulerOutput(sch_cnt).schedulerToReceiverInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).dataGenInfo.rbInfo.dlPowMask=schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.dlPowMask;
// 		schedulerOutput(sch_cnt).schedulerToReceiverInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).dataGenInfo.rbInfo.crsPowBoostInDB=schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.crsPowBoostInDB;
// 	      }
// 	    }
// 	    //===================================================================================================  
// 	    if(unScheduledRBs.length())
// 	      myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsPositionMatrix=myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(unScheduledRBs(0)); 
// 	    else
// 	      myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsPositionMatrix=myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(0); // All RBs contains CRS.
// 	      
// 	      if(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo.length())
// 		myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.crsPowBoostInDB = schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).dataGenInfo.rbInfo.crsPowBoostInDB;
// 	      
// 	      myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.logicalSubFrameStructure=*myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.getCurrentFrameStructurePointer();
// 	  }
// 	  else // Entire frame is not scheduled
// 	  {
// 	    ivec unScheduledRBs=getIntegers(0,myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getResourceBlocksPerSubframe()-1);
// 	    {
// 	      myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo=myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRSInfoForUnScheduled;
// 	      if(myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.fillPRS)
// 	      {
// 		if((ttiCount-myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.PRSoffset)%myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.PRSperiodicity==0)
// 		{
// 		  if(myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.nCQIports<=2)
// 		  {
// 		    append(myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,61);
// 		    append(myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.portReferenceNumbers,71);
// 		  }
// 		  else
// 		  {
// 		    append(myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,62);
// 		    append(myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.portReferenceNumbers,72);
// 		  }
// 		}
// 	      }
// 	      RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure,myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts,unScheduledRBs,true, myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.isLocalised(0), &myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameReservationInfo);
// 	      for(int rb_cnt=0;rb_cnt<unScheduledRBs.length();rb_cnt++)
// 		myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.updateRUType(unScheduledRBs(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 	    }
// 	    myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.updateCurrentFrameStructure();
// 	    myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsPositionMatrix=myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(unScheduledRBs(0));
// 	    myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.logicalSubFrameStructure=*myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure.getCurrentFrameStructurePointer();  
// 	  }
// 	  ivec srvcNodeIndices=schedulerOutput(sch_cnt).getSchedulerToReceiverInfoIndx(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).rxNodeIDs,nuID);
// 	  
// 	  //Copying core modules from TxNode to RxNodes
// 	  for(int rx_cnt=0;rx_cnt<rxNodeDeviceIndices.length();rx_cnt++)
// 	  {
// 	    int rxNUIndx=myNodeDevices(rxNodeDeviceIndices(rx_cnt)).getNUIndx(nuID);
// 	    myNodeDevices(rxNodeDeviceIndices(rx_cnt)).L1CorePerNU(rxNUIndx).dlFrameStructure=myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameStructure;
// 	    myNodeDevices(rxNodeDeviceIndices(rx_cnt)).L1CorePerNU(rxNUIndx).dlFrameReservationInfo=myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlFrameReservationInfo;
// 	    myNodeDevices(rxNodeDeviceIndices(rx_cnt)).L1CorePerNU(rxNUIndx).dlRbInfoForUnScheduled=myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).dlRbInfoForUnScheduled;
// 	  }
// 	  
// 	}
//       } 
//       
//     }
//   }
//   
// //   //else //!Up-link
// //   for(int sch_cnt=0;sch_cnt<schedulerOutput.length();sch_cnt++)
// //   {
// //     for(int schToRx_cnt=0;schToRx_cnt<schedulerOutput(sch_cnt).isSchToRxInfoForDownlink.length();schToRx_cnt++)
// //     {
// //       if(schedulerOutput(sch_cnt).isSchToRxInfoForDownlink(schToRx_cnt)==0 && schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).rxNUID==nuID)//! Uplink
// //       {
// // 	ivec srvcNodeTxIndx=find(txNodeIDs,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).txNodeIDs); 
// // 	
// // 	bool isDownlink=false;
// // 	if(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo.length())
// // 	{
// // 	  ivec srvcNodeIndices=schedulerOutput(sch_cnt).getSchedulerToTransmitterInfoIndx(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).txNodeIDs,nuID);
// // 	  bvec isTheRBScheduled=zeros_b(myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe());
// // 	  for(int db_cnt=0;db_cnt<schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo.length();db_cnt++)
// // 	    if(srvcNodeTxIndx(db_cnt)!=-1)  
// // 	    {
// // 	      int subframeNumber=schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).subFrameNumber;
// // 	      setInVec(isTheRBScheduled,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks,ones_b(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks.length()));
// // 	      // 	        isTheRBScheduled(schedulerOutput(sch_cnt).schedulerToReceiverInfo(srvrNode_cnt).dataBlkInfo(db_cnt).resourceBlocks)=ones_b(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks.length());
// // 	      int nRBs = myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe();
// // 	      int nSb=nRBs/myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.getResourceBlockGroupingSize();
// // 	      myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.perform_UL_Freq_hopping(subframeNumber,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks(0),schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks.length(),nSb,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.cellID,!schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).isLocalised);
// // 	      if(updateFrameStructure)
// // 	      {
// // 		RBMergeInfo_S mergedRbInfo=generateRBStructure(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo.rsPorts,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks,false,true,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameReservationInfo);
// // 		schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB.set_length(0);
// // 		for(int rb_cnt=0;rb_cnt<schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks.length();rb_cnt++)
// // 		{
// // 		  schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure->updateRUType(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// // 		  append(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure->getToneCountPerRU(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),"data"));
// // 		}
// // 	      }
// // 	    }
// // 	    
// // 	    //! Below code commented for node level processing .. May need to be moved to another location to uncomment - Dhiv
// // 	    
// // 	    // 	  ivec unScheduledRBs=find(to_ivec(isTheRBScheduled),0,true);
// // 	    // 	  ivec rbChunkStart(0),rbChunkEnd(0);
// // 	    // 	  isTheRBScheduled=concat((bin)1,isTheRBScheduled);
// // 	    // 	  append(isTheRBScheduled,1);
// // 	    // 	  for(int k=1;k<isTheRBScheduled.length();k++)
// // 	    // 	  {
// // 	    // 	    if(isTheRBScheduled(k-1)==1 && isTheRBScheduled(k)==0)
// // 	    // 	      append(rbChunkStart,k-1);
// // 	    // 	    
// // 	    // 	    if(isTheRBScheduled(k-1)==0 && isTheRBScheduled(k)==1)
// // 	    // 	      append(rbChunkEnd,k-2);
// // 	    // 	  }
// // 	    // 	  if(rbChunkStart.length())
// // 	    // 	  {
// // 	    // 	    int nRBs = myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe();
// // 	    // 	    int nSb=nRBs/myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.getResourceBlockGroupingSize();
// // 	    // 	    for(int cnt=0;cnt<rbChunkStart.length();cnt++)
// // 	    // 	      myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.perform_UL_Freq_hopping(subframeNumber,rbChunkStart(cnt),rbChunkEnd(cnt)-rbChunkStart(cnt)+1,nSb,0,false);
// // 	    // 	  
// // 	    // 	    RBMergeInfo_S mergedRbInfo=generateRBStructure(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure,to_ivec(""),unScheduledRBs,false,true,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameReservationInfo);
// // 	    // 	    for(int rb_cnt=0;rb_cnt<unScheduledRBs.length();rb_cnt++)
// // 	    // 	    {
// // 	    // 	      if(mergedRbInfo.mergedRBName(rb_cnt)=="") mergedRbInfo.mergedRBName(rb_cnt)="nullRB";
// // 	    // 	      schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure->updateRUType(unScheduledRBs(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// // 	    // 	    }
// // 	    // 	  }
// // 	    
// // 	    schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure->updateCurrentFrameStructure();
// // 	    for(int db_cnt=0;db_cnt<schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo.length();db_cnt++)
// // 	      if(srvcNodeTxIndx(db_cnt)!=-1)  
// // 	      {
// // 		int subframeNumber=schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).subFrameNumber;
// // 		schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix=schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure->getResourceUnitMatrix(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks(0));
// // 		schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure=schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure->getCurrentFrameStructurePointer();
// // 		schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).dataGenInfo.rbInfo.rsPositionMatrix=schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix;
// // 		schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).dataGenInfo.resourceElementsPerRB=schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB;
// // 		schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).dataGenInfo.rbInfo.logicalSubFrameStructure=schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure;
// // 		schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).sourceAndSink->setTransmissionStatusInHarqManager(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).txNodeID,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).harqInfo,subframeNumber);
// // 		if(!isWithAbstraction)
// // 		{
// // 		  schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).modulatedSymbols=schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).sourceAndSink->getModulatedSymbols(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).txNodeID,&schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).dataGenInfo,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).harqInfo,subframeNumber,isDownlink);
// // 		  schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).modulatedSymbols=schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).modulatedSymbols;
// // 		}
// // 		else
// // 		{
// // 		  schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).sourceAndSink->initializeHarqProcess(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).txNodeID,&schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).dataGenInfo,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).harqInfo,subframeNumber,isDownlink);
// // 		  schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).sourceAndSink->initializeRxHarqProcess(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).txNodeID,&schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).dataGenInfo,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).harqInfo,subframeNumber,isDownlink);
// // 		}
// // 		schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).harqInfo=schedulerOutput(sch_cnt).schedulerToTransmitterInfo(srvcNodeIndices(db_cnt)).dataBlkInfo(0).harqInfo;
// // 	      } 
// // 	}
// // 	
// // 	else // Entire frame is not scheduled
// // 	{
// // 	  
// // 	  ivec unScheduledRBs=getIntegers(0,myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe()-1);
// // 	  if(updateFrameStructure)
// // 	  {
// // 	    RBMergeInfo_S mergedRbInfo=generateRBStructure(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure,"",unScheduledRBs,false,true,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameReservationInfo);
// // 	    for(int rb_cnt=0;rb_cnt<unScheduledRBs.length();rb_cnt++)
// // 	      schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure->updateRUType(unScheduledRBs(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// // 	  }
// // 	  schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure->updateCurrentFrameStructure();
// // 	  schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).rbInfoForUnScheduled.rsPositionMatrix=schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure->getResourceUnitMatrix(unScheduledRBs(0));
// // 	  schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).rbInfoForUnScheduled.logicalSubFrameStructure=schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure->getCurrentFrameStructurePointer();  
// // 	  
// // 	  int nSb=myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe()/myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.getResourceBlockGroupingSize();
// // 	  myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.perform_UL_Freq_hopping(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).subFrameNumber,0,myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe(),nSb,0,false);
// // 	}
// // 	
// //       }
// //     }
// //   }
// 
//   
//   //else //!Up-link
//   for(int sch_cnt=0;sch_cnt<schedulerOutput.length();sch_cnt++)
//   {
//     for(int schToTx_cnt=0;schToTx_cnt<schedulerOutput(sch_cnt).isSchToTxInfoForDownlink.length();schToTx_cnt++)
//     {
//       int txNodeIdx=find(txNodeIDs,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).txNodeID);
//       if(txNodeIdx!=-1 && schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).txNUID==nuID)  
//       {
// 	int nuIndx=myNodeDevices(deviceIndices(txNodeIdx)).getNUIndx(nuID);
// 	if(schedulerOutput(sch_cnt).isSchToTxInfoForDownlink(schToTx_cnt)==0)//! Uplink
// 	{
// 	  bool isDownlink=false;
// 	  if(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo.length())
// 	  {
// 	    int rxNodeID=schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).rxNodeIDs(0);
// 	    int schToRx_cnt=schedulerOutput(sch_cnt).getSchedulerToReceiverInfoIndx(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).rxNodeIDs(0),nuID);
// 	    int db_cnt=find(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).txNodeIDs,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).txNodeID);
// 	    int rxDeviceIndx=find(nodeDeviceIDs,rxNodeID);
// 	    int rxNuIndx=myNodeDevices(rxDeviceIndx).getNUIndx(nuID);
// 	    
// 	    // both carrier count will be same as of now -  Dhiv
// 	    int txCarrier_cnt=find(myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).nuParameters.carrierFrequenciesInMHz,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).carrierFreqInMHz);
// 	    int rxCarrier_cnt=find(myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).nuParameters.carrierFrequenciesInMHz,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).carrierFreqInMHz);
// 	    
// 	    myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).ulFrameStructure.setCurrentFrameType("ulFrame");
// 	    myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.loadULResourceMapperForBandwidth(myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).ulFrameStructure.getRuCountPerLevel());
// 	    myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).ulFrameStructure.setCurrentFrameType("ulFrame");
// 	    myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.loadULResourceMapperForBandwidth(myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).ulFrameStructure.getRuCountPerLevel());
// 	    
// // 	    bvec isTheRBScheduled=zeros_b(myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe());
// 	    int ttiCount=schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).ttiCount;
// // 	    setInVec(isTheRBScheduled,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks,ones_b(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks.length()));
// 	    // 	        isTheRBScheduled(schedulerOutput(sch_cnt).schedulerToReceiverInfo(srvrNode_cnt).dataBlkInfo(db_cnt).resourceBlocks)=ones_b(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks.length());
// 	    int nRBs = myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe();
// 	    int nSb=nRBs/myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.getResourceBlockGroupingSize();
// 	    myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.perform_UL_Freq_hopping(ttiCount,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks(0),schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks.length(),nSb,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.cellID,!schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).isLocalised);
// 	    myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.perform_UL_Freq_hopping(ttiCount,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks(0),schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks.length(),nSb,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.cellID,!schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).isLocalised);
// 	    {
// 	      RBMergeInfo_S mergedRbInfo=generateRBStructure(&myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).ulFrameStructure,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo.rsPorts,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks,false,true,&myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).ulFrameReservationInfo);
// 	      // called again to update Tx framestructure
// 	      generateRBStructure(&myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).ulFrameStructure,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsInfo.rsPorts,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks,false,true,&myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).ulFrameReservationInfo);
// 	      schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB.set_length(0);
// 	      for(int rb_cnt=0;rb_cnt<schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks.length();rb_cnt++)
// 	      {
// 		myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).ulFrameStructure.updateRUType(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 		myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).ulFrameStructure.updateRUType(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 		append(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB,myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).ulFrameStructure.getToneCountPerRU(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks(rb_cnt),"data"));
// 	      }
// 	    }
// 	    
// 	      //! Below code commented for node level processing .. May need to be moved to another location to uncomment - Dhiv
// 	      
// 	      // 	  ivec unScheduledRBs=find(to_ivec(isTheRBScheduled),0,true);
// 	      // 	  ivec rbChunkStart(0),rbChunkEnd(0);
// 	      // 	  isTheRBScheduled=concat((bin)1,isTheRBScheduled);
// 	      // 	  append(isTheRBScheduled,1);
// 	      // 	  for(int k=1;k<isTheRBScheduled.length();k++)
// 	      // 	  {
// 	      // 	    if(isTheRBScheduled(k-1)==1 && isTheRBScheduled(k)==0)
// 	      // 	      append(rbChunkStart,k-1);
// 	      // 	    
// 	      // 	    if(isTheRBScheduled(k-1)==0 && isTheRBScheduled(k)==1)
// 	      // 	      append(rbChunkEnd,k-2);
// 	      // 	  }
// 	      // 	  if(rbChunkStart.length())
// 	      // 	  {
// 	      // 	    int nRBs = myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe();
// 	      // 	    int nSb=nRBs/myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.getResourceBlockGroupingSize();
// 	      // 	    for(int cnt=0;cnt<rbChunkStart.length();cnt++)
// 	      // 	      myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.perform_UL_Freq_hopping(subframeNumber,rbChunkStart(cnt),rbChunkEnd(cnt)-rbChunkStart(cnt)+1,nSb,0,false);
// 	      // 	  
// 	      // 	    RBMergeInfo_S mergedRbInfo=generateRBStructure(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure,to_ivec(""),unScheduledRBs,false,true,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameReservationInfo);
// 	      // 	    for(int rb_cnt=0;rb_cnt<unScheduledRBs.length();rb_cnt++)
// 	      // 	    {
// 	      // 	      if(mergedRbInfo.mergedRBName(rb_cnt)=="") mergedRbInfo.mergedRBName(rb_cnt)="nullRB";
// 	      // 	      schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure->updateRUType(unScheduledRBs(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 	      // 	    }
// 	      // 	  }
// 	      
// 	      myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).ulFrameStructure.updateCurrentFrameStructure();
// 	      myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).ulFrameStructure.updateCurrentFrameStructure();
// 	      schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix=myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).ulFrameStructure.getResourceUnitMatrix(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).resourceBlocks(0));
// 	      schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure=*myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).ulFrameStructure.getCurrentFrameStructurePointer();
// 	      schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).dataGenInfo.rbInfo.rsPositionMatrix=schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.rbInfo.rsPositionMatrix;
// 	      schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).dataGenInfo.resourceElementsPerRB=schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).dataGenInfo.resourceElementsPerRB;
// 	      schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).dataGenInfo.rbInfo.logicalSubFrameStructure=*myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).ulFrameStructure.getCurrentFrameStructurePointer();
// 	      myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).uplinkSourceAndSink.setTransmissionStatusInHarqManager(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).txNodeID,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).harqInfo,ttiCount);
// 	      myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).uplinkSourceAndSink.setTransmissionStatusInHarqManager(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).txNodeID,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).harqInfo,ttiCount);
// 	      if(!isWithAbstraction)
// 	      {
// 		schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).modulatedSymbols=myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).uplinkSourceAndSink.getModulatedSymbols(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).txNodeID,&schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).dataGenInfo,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).harqInfo,ttiCount,isDownlink);
// 		schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).modulatedSymbols=schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).modulatedSymbols;
// 	      }
// 	      else
// 	      {
// 		myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).uplinkSourceAndSink.initializeHarqProcess(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).txNodeID,&schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).dataGenInfo,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).harqInfo,ttiCount,isDownlink);
// 		myNodeDevices(deviceIndices(txNodeIdx)).L1CorePerNU(nuIndx).uplinkSourceAndSink.initializeRxHarqProcess(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).txNodeID,&schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).dataGenInfo,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).harqInfo,ttiCount,isDownlink);
// 	      
// 		myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).uplinkSourceAndSink.initializeHarqProcess(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).txNodeID,&schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).dataGenInfo,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).harqInfo,ttiCount,isDownlink);
// 		myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).uplinkSourceAndSink.initializeRxHarqProcess(schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).txNodeID,&schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).dataGenInfo,schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).harqInfo,ttiCount,isDownlink);
// 	      }
// 	      schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(db_cnt).harqInfo=schedulerOutput(sch_cnt).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(0).harqInfo;
// 	  }
// 	  
// 	  else // Entire frame is not scheduled
// 	  {
// 	    
// // 	    ivec unScheduledRBs=getIntegers(0,myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe()-1);
// // 	    if(updateFrameStructure)
// // 	    {
// // 	      RBMergeInfo_S mergedRbInfo=generateRBStructure(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure,"",unScheduledRBs,false,true,schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameReservationInfo);
// // 	      for(int rb_cnt=0;rb_cnt<unScheduledRBs.length();rb_cnt++)
// // 		schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure->updateRUType(unScheduledRBs(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// // 	    }
// // 	    schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure->updateCurrentFrameStructure();
// // 	    schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).rbInfoForUnScheduled.rsPositionMatrix=schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure->getResourceUnitMatrix(unScheduledRBs(0));
// // 	    schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).rbInfoForUnScheduled.logicalSubFrameStructure=schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).frameStructure->getCurrentFrameStructurePointer();  
// // 	    
// // 	    int nSb=myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe()/myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.getResourceBlockGroupingSize();
// // 	    myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.perform_UL_Freq_hopping(schedulerOutput(sch_cnt).schedulerToReceiverInfo(schToRx_cnt).subFrameNumber,0,myNodeDevices(rxDeviceIndx).L1CorePerNU(rxNuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe(),nSb,0,false);
// 	  }
// 	}
//       }
//     }
//   }
//   
//   
// }





/*****
int Transmitter::generatePTRSLocations(Array<ivec> PTRSLocations,int numRB, int MCS, double subCarrSpacing, int numDMRSSym, Array <ivec> DMRSSym){
  // Require PDSCH region, DMRS symbol locations, C-RNTI, DMRS port, RRC param: PTRS_RE_Offset 
  // To fetch symbols of asociated DMRS port
  
  PTRSDensity_S tempPTRSDensity;
  int numPTRS,,numSC,SCIdx(numRB),numSym,symIdx(14);
  int symArr=[0,1,2,3,4,5,6,7,8,9,10,11,12,13],idx,idx2;
  
  tempPTRSDensity = getPTRSDensity(int numRB, int MCS, int subCarrSpacing);
  
  // create SC indices for PTRS
  
  switch tempPTRSDensity.freqDensity
  {
    case 0:
    {
     numSC = 0; 
    }

    case 1:
    {
     numSC = numRB;
     for(idx = 0; idx < numSC; idx++)
       SCIdx(idx) = REOffset + idx*12;
      
    }
     case 2:
    {
     numSC = numRB/2;
     for(idx = 0; idx < numSC; idx++)
       SCIdx(idx) = (RBOffset * 12) + REOffset + idx*24 ;
      
    }
    case 4:
    {
     numSC = numRB/4;
     for(idx = 0; idx < numSC; idx++)
       SCIdx(idx) = (RBOffset * 12) + REOffset + idx*48 ;
      
    }
  }
  
  switch tempPTRSDensity.timeDensity
  {
    case 0:
    {
     numSym = 0; 
    }
    
    case 1:
    {
      numSym = 14-numDMRSSym;
      symIdx = setDiff(symArr,DMRSSym);      
       
    }
    case 2:
    {
      
    }
    
    case 4:
    {
    }
    
  }
  
  numPTRS = numSC * numSym;
  for(idx = 0;idx<numSym;idx++)
    for(idx2 = 0;idx2<numSC;idx++)
    {
          PTRSLocations(idx*numSC+idx2,1) = symIdx(idx);
	  PTRSLocations(idx*numSC+idx2,2) = SCIdx(idx2);
    }
  return(PTRSLocations);
}


PTRSDensity_S Transmitter::getPTRSDensity(int numRB, int MCS, double subCarrSpacing) {
 
  PTRSDensity_S tempPTRSDensity;
  
  // create a file and fill all threshold values
  
if ((subCarrSpacing == 60e3) || (subCarrSpacing == 120e3))
{
  if (MCS <= MCSThesh11) tempPTRSDensity.timeDensity = 0;
  if (MCS <= MCSThesh12) tempPTRSDensity.timeDensity = 4;
  if (MCS <= MCSThesh13) tempPTRSDensity.timeDensity = 2;
  if (MCS <= MCSThesh14) tempPTRSDensity.timeDensity = 1;
  
  
  if (numRB <= RBThesh11) tempPTRSDensity.freqDensity = 0;
  if (numRB <= RBThesh12) tempPTRSDensity.freqDensity = 1;
  if (numRB <= RBThesh13) tempPTRSDensity.freqDensity = 2;
  if (numRB <= RBThesh14) tempPTRSDensity.freqDensity = 4;
}
 
 if ((subCarrSpacing == 240e3) || (subCarrSpacing == 480e3))
{
  if (MCS <= MCSThesh21) tempPTRSDensity.timeDensity = 0;
  if (MCS <= MCSThesh22) tempPTRSDensity.timeDensity = 4;
  if (MCS <= MCSThesh23) tempPTRSDensity.timeDensity = 2;
  if (MCS <= MCSThesh24) tempPTRSDensity.timeDensity = 1;
  
  
  if (numRB <= RBThesh21) tempPTRSDensity.freqDensity = 0;
  if (numRB <= RBThesh22) tempPTRSDensity.freqDensity = 1;
  if (numRB <= RBThesh23) tempPTRSDensity.freqDensity = 2;
  if (numRB <= RBThesh24) tempPTRSDensity.freqDensity = 4;
}
 return(tempPTRSDensity)
}
***/
