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

#include "../include/FrameScheduler.h"


// void scheduleFramesNew(SchedulerOutput_S& schedulerOutput, int nuID, PortInfo_S dlPortInfo, PortInfo_S ulPortInfo, Array< RSInfo_S >& dlRSInfoForUnScheduledPerServerNode, Array< RSInfo_S >& ulRSInfoForUnScheduledPerServerNode, MacScheduler& macScheduler, ivec serverNodes, bvec serverNodesActiveList, ivec dlTxNodes, ivec ulTxNodes, ivec dlTxAntennaCount, ivec ulTxAntennaCount, vec serverNodeTxPowersInDBm, vec serviceNodeTxPowersInDBm, int dlRBGroupSize, int ulRBGroupSize, ivec idCellPerServerNode, CQIInfo_S* cqiInfo, int subFrameNumber, bool isRestrictionForReTx, int maximumServiceNodesPerSubframe, bool isRankAdaptationEnabled, bool isLinkSimulation, int defaultCQIIndex, ivec defaultMCSIndices, vec loadPerServerNode, Array< ivec > reservedRBGs)
// { 
//   //int idCellIndx;
//   ivec feedBackServiceNodeList(cqiInfo->serviceNodeIDs.length());
//   ivec myServiceNodesFeedbackIndices;
//   ivec activeServiceNodes, serviceNodes(0);
//   int nRBs,nRBGroups,resourceBlockGroupSize;
//   
//   RBInfo_S rbInfoForUnscheduled; //contains only cqi ports
//   
//   ResourceMapper *resourceMapper;
//   SourceAndSink *dataGenerator;
//   FrameStructure *frameStructure;
//   //FrameReservationInfo_S *frameReservationInfo;
//   Codebook *codebook;
//   
//   for(int fb_cnt=0;fb_cnt<cqiInfo->serviceNodeIDs.length();fb_cnt++)
//     feedBackServiceNodeList(fb_cnt)=cqiInfo->serviceNodeIDs(fb_cnt);
//   
//   vec txPowersInDBmPerTxNode;
//   ivec antennaCountPerTxNode;
//   ServiceNodesInfo_S *serviceNodesInfo=NULL;
//   PortInfo_S portInfo;
//   int srvrNodeAntCnt=0;
//   ivec srvcNodeAntCnt(0);
//   bool scheduleDownlink=false;
//   bool scheduleUplink=false;
//   
//   if(loadPerServerNode.length())
//   {
//     if(loadPerServerNode.length()!=serverNodes.length()){cout<<"[both:] Load info not available for all servernodes in scheduleFrames().."<<endl;abort();}
//   }
//   else
//     loadPerServerNode=zeros(serverNodes.length())-1.0;
// 
//   for(int srvrNode_cnt=0;srvrNode_cnt<serverNodes.length();srvrNode_cnt++)
//   {
// 
//     int schToTxIndx=schedulerOutput.getSchedulerToTransmitterInfoIndx(serverNodes(srvrNode_cnt),nuID);
//     if(schToTxIndx!=-1) //! Downlink
//     {
//       scheduleDownlink=true;
//       int myTxIndex=find(dlTxNodes,serverNodes(srvrNode_cnt));
//       append(txPowersInDBmPerTxNode,serverNodeTxPowersInDBm(myTxIndex));
//       append(antennaCountPerTxNode,dlTxAntennaCount(myTxIndex));
//       srvrNodeAntCnt=dlTxAntennaCount(myTxIndex);
//       serviceNodesInfo=macScheduler.dlServiceNodesInfo;
//       rbInfoForUnscheduled.rsInfo=dlRSInfoForUnScheduledPerServerNode(srvrNode_cnt);
//              
//       if(rbInfoForUnscheduled.rsInfo.fillPRS)
//       {
// 	if((subFrameNumber-rbInfoForUnscheduled.rsInfo.PRSoffset)%rbInfoForUnscheduled.rsInfo.PRSperiodicity==0)
// 	{
// 	  if(rbInfoForUnscheduled.rsInfo.nCQIports<=2)
// 	  {
// 	    append(rbInfoForUnscheduled.rsInfo.rsPorts,61);
// 	    append(rbInfoForUnscheduled.rsInfo.portReferenceNumbers,71);
// 	  }
// 	  else
// 	  {
// 	    append(rbInfoForUnscheduled.rsInfo.rsPorts,62);
// 	    append(rbInfoForUnscheduled.rsInfo.portReferenceNumbers,72);
// 	  }
// 	}
//       }
// 	  
//     }
//     
//     int schToRxIndx=schedulerOutput.getSchedulerToReceiverInfoIndx(serverNodes(srvrNode_cnt),nuID);
//     if(schToRxIndx!=-1) //! Uplink
//     {
//       scheduleUplink=true;
//       serviceNodesInfo=macScheduler.ulServiceNodesInfo;
//       rbInfoForUnscheduled.rsInfo=ulRSInfoForUnScheduledPerServerNode(srvrNode_cnt);
//     }
//     
//     if(serverNodesActiveList(srvrNode_cnt))
//     {
//       //cout<<"Scheduling for "<<serverNodes(schToTx_cnt)<<endl;
// 
//       Array<RBGroupInfo_S> rbGroups;
//       Array<SchedulerInfo_S> scheduledServiceNodesInfoPerRBGroup;
// 
// 
//       //picking only the service nodes associated with the servernode for scheduling in the frame - will be changed in advance scheduling...
//       serviceNodes=serviceNodesInfo->associatedServiceNodes(find(serviceNodesInfo->associatedServerNodesForEachServiceNode,serverNodes(srvrNode_cnt),true));
//       activeServiceNodes=serviceNodesInfo->activeServiceNodes(remove(find(serviceNodesInfo->activeServiceNodes,serviceNodes),-1));
//       myServiceNodesFeedbackIndices=find(feedBackServiceNodeList,activeServiceNodes);
//       if(scheduleUplink) //! Uplink
//       {
// 	ivec myTxIndices=find(ulTxNodes,activeServiceNodes);
// 	txPowersInDBmPerTxNode=serviceNodeTxPowersInDBm(myTxIndices);
// 	antennaCountPerTxNode=ulTxAntennaCount(myTxIndices);
// 	srvcNodeAntCnt=antennaCountPerTxNode;
// 	SchedulerToReceiverInfo_S temp=schedulerOutput.getSchedulerToReceiverInfo(serverNodes(srvrNode_cnt),nuID);
// 
// 	resourceMapper=temp.resourceMapper;
// 	frameStructure=temp.frameStructure;
// 	portInfo=ulPortInfo;
// 	dataGenerator=temp.sourceAndSink;
// 	codebook=temp.codebook;
// 	nRBs=temp.resourceMapper->ulResourceMapper.getResourceBlocksPerSubframe();
// 	resourceBlockGroupSize=ulRBGroupSize;
// 	
// 	double txSubcarrierPowersInDBm;
// 	if(activeServiceNodes.length()!=0)
// 	{
// 	  nRBGroups=nRBs/resourceBlockGroupSize;
// 	  if(nRBGroups*resourceBlockGroupSize<nRBs) nRBGroups+=1;
// 	  
// 	  ivec reservedRBGs_srvrcnt(0);
// 	  if(reservedRBGs.length() != 0)
// 	  {
// 	    reservedRBGs_srvrcnt=reservedRBGs(srvrNode_cnt);
// 	    scheduleRBGroups(rbGroups,scheduledServiceNodesInfoPerRBGroup,serverNodes(srvrNode_cnt),srvrNodeAntCnt,idCellPerServerNode(srvrNode_cnt),activeServiceNodes,srvcNodeAntCnt,nRBs,nRBGroups,/*resourceBlockGroupSize,*/macScheduler,false,*frameStructure,portInfo,*resourceMapper,*dataGenerator,cqiInfo,myServiceNodesFeedbackIndices,subFrameNumber,codebook,isRestrictionForReTx, maximumServiceNodesPerSubframe,isRankAdaptationEnabled,isLinkSimulation, defaultCQIIndex,loadPerServerNode(srvrNode_cnt),reservedRBGs_srvrcnt);
// 	  }
// 	  else
// 	    scheduleRBGroups(rbGroups,scheduledServiceNodesInfoPerRBGroup,serverNodes(srvrNode_cnt),srvrNodeAntCnt,idCellPerServerNode(srvrNode_cnt),activeServiceNodes,srvcNodeAntCnt,nRBs,nRBGroups,/*resourceBlockGroupSize,*/macScheduler,false,*frameStructure,portInfo,*resourceMapper,*dataGenerator,cqiInfo,myServiceNodesFeedbackIndices,subFrameNumber,codebook,isRestrictionForReTx, maximumServiceNodesPerSubframe,isRankAdaptationEnabled,isLinkSimulation, defaultCQIIndex,loadPerServerNode(srvrNode_cnt));
// 	  
// 	  //! Copying Scheduling Information to dataBlkInfoPerServiceNode...
// 	  Array<DataBlkInfo_S> dataBlkInfoPerServiceNode;
// 	  createDataBlkInfoNew(dataBlkInfoPerServiceNode,schedulerOutput,serverNodes(srvrNode_cnt),activeServiceNodes,rbGroups,scheduledServiceNodesInfoPerRBGroup,rbInfoForUnscheduled,defaultMCSIndices, false, *serviceNodesInfo);
// 	  //! Copying Informations to proper structures, to pass it to the Transmitter & Receiver...
// 	  addDataBlksToSchedulerOutput(serviceNodesInfo,activeServiceNodes,schedulerOutput,nuID,dataBlkInfoPerServiceNode,serverNodes,srvrNode_cnt,txPowersInDBmPerTxNode,txSubcarrierPowersInDBm,/*isWithAbstraction,*/false,subFrameNumber,isLinkSimulation);
// 	}
// 	else //unscheduled Frame (filled only with CQI Ports)
// 	  updateUnscheduledFrameNew(schedulerOutput,serverNodes(srvrNode_cnt),nRBs,antennaCountPerTxNode,txPowersInDBmPerTxNode,txSubcarrierPowersInDBm,rbInfoForUnscheduled,false);
// 	
// 	rbInfoForUnscheduled.rsInfo.cellid=idCellPerServerNode(srvrNode_cnt);
// 	updateRBInfoForUnscheduled(schedulerOutput,serverNodes,srvrNode_cnt,/*txSubcarrierPowersInDBm,*/rbInfoForUnscheduled,serviceNodes,false);//! ivec converted into bvec since it is paased as a bool
//       }
//       if(scheduleDownlink) //! Downlink
//       {
// 	SchedulerToTransmitterInfo_S temp=schedulerOutput.getSchedulerToTransmitterInfo(serverNodes(srvrNode_cnt),nuID);
// 	resourceMapper=temp.resourceMapper;
// 	frameStructure=temp.frameStructure;
// 	portInfo=dlPortInfo;
// 	dataGenerator=temp.sourceAndSink;
// 	codebook=temp.codebook;
// 	nRBs=temp.resourceMapper->dlResourceMapper.getResourceBlocksPerSubframe();
// 	resourceBlockGroupSize=dlRBGroupSize;
// 	
// 	 double txSubcarrierPowersInDBm;
// 	if(activeServiceNodes.length()!=0)
// 	{
// 	  nRBGroups=nRBs/resourceBlockGroupSize;
// 	  if(nRBGroups*resourceBlockGroupSize<nRBs) nRBGroups+=1;
// 
// 	  ivec reservedRBGs_srvrcnt(0);
// 	  if(reservedRBGs.length() != 0)
// 	  {
// 	    reservedRBGs_srvrcnt=reservedRBGs(srvrNode_cnt);
// 	    scheduleRBGroups(rbGroups,scheduledServiceNodesInfoPerRBGroup,serverNodes(srvrNode_cnt),srvrNodeAntCnt,idCellPerServerNode(srvrNode_cnt),activeServiceNodes,srvcNodeAntCnt,nRBs,nRBGroups,/*resourceBlockGroupSize,*/macScheduler,true,*frameStructure,portInfo,*resourceMapper,*dataGenerator,cqiInfo,myServiceNodesFeedbackIndices,subFrameNumber,codebook,isRestrictionForReTx, maximumServiceNodesPerSubframe,isRankAdaptationEnabled,isLinkSimulation, defaultCQIIndex,loadPerServerNode(srvrNode_cnt),reservedRBGs_srvrcnt);
// 	  }
// 	  else
// 	    scheduleRBGroups(rbGroups,scheduledServiceNodesInfoPerRBGroup,serverNodes(srvrNode_cnt),srvrNodeAntCnt,idCellPerServerNode(srvrNode_cnt),activeServiceNodes,srvcNodeAntCnt,nRBs,nRBGroups,/*resourceBlockGroupSize,*/macScheduler,true,*frameStructure,portInfo,*resourceMapper,*dataGenerator,cqiInfo,myServiceNodesFeedbackIndices,subFrameNumber,codebook,isRestrictionForReTx, maximumServiceNodesPerSubframe,isRankAdaptationEnabled,isLinkSimulation, defaultCQIIndex,loadPerServerNode(srvrNode_cnt));
// 
// 	  //! Copying Scheduling Information to dataBlkInfoPerServiceNode...
// 	  Array<DataBlkInfo_S> dataBlkInfoPerServiceNode;
// 	  createDataBlkInfoNew(dataBlkInfoPerServiceNode,schedulerOutput,serverNodes(srvrNode_cnt),activeServiceNodes,rbGroups,scheduledServiceNodesInfoPerRBGroup,rbInfoForUnscheduled,defaultMCSIndices, true, *serviceNodesInfo);
// 	  //! Copying Informations to proper structures, to pass it to the Transmitter & Receiver...
// 	  addDataBlksToSchedulerOutput(serviceNodesInfo,activeServiceNodes,schedulerOutput, nuID,dataBlkInfoPerServiceNode,serverNodes,srvrNode_cnt,txPowersInDBmPerTxNode,txSubcarrierPowersInDBm,/*isWithAbstraction,*/true,subFrameNumber,isLinkSimulation);
// 	}
// 	else //unscheduled Frame (filled only with CQI Ports)
// 	  updateUnscheduledFrameNew(schedulerOutput,serverNodes(srvrNode_cnt),nRBs,antennaCountPerTxNode,txPowersInDBmPerTxNode,txSubcarrierPowersInDBm,rbInfoForUnscheduled,true);
// 	
// 	rbInfoForUnscheduled.rsInfo.cellid=idCellPerServerNode(srvrNode_cnt);
// 	updateRBInfoForUnscheduled(schedulerOutput,serverNodes,srvrNode_cnt,/*txSubcarrierPowersInDBm,*/rbInfoForUnscheduled,serviceNodes,true);//! ivec converted into bvec since it is paased as a bool
//       }
//      
//     }
//     else //! serverNodesActiveList(srvrNode_cnt)
//     {
//       if(scheduleDownlink)
//       {
// 	int schToTxIndx=schedulerOutput.getSchedulerToTransmitterInfoIndx(serverNodes(srvrNode_cnt),nuID);
// 	schedulerOutput.schedulerToTransmitterInfo(schToTxIndx).isTransmitting=false;
//       }
//     }
//   }
// }// zeroWarning case handled below
// void scheduleRBGroups(Array< RBGroupInfo_S >& rbGroups, Array< SchedulerInfo_S >& scheduledServiceNodesInfoPerRBGroup, int serverNodeID, int srvrNodeAntennaCnt, int idCell, ivec serviceNodes, ivec srvcNodeAntennaCnt, int nRUs, int nRBGroups, /*int resourceBlockGroupSize*/ MacScheduler& macScheduler, bool isDownlink, FrameStructure &frameStructure, PortInfo_S &portInfo, ResourceMapper& resourceMapper, SourceAndSink& sourceAndSink, CQIInfo_S* cqiInfo, ivec& myServiceNodesFeedbackIndices, int subFrameNumber, Codebook* codeBook, bool isRestrictionForReTx,int maximumServiceNodesPerSubframe, bool isRankAdaptationEnabled,bool isLinkSimulation, int defaultCQIIndex, double loadInPercentage,ivec reservedRBGs){
//   
//   // LA for Link simulation
//   if(isLinkSimulation && defaultCQIIndex==-1)
//   {
//     isLinkSimulation=false;
//     defaultCQIIndex=1;
//   }
//   rbGroups.set_length(nRBGroups);
//   ivec selectedCQIIndicesPerRBGroup(nRBGroups);
//   scheduledServiceNodesInfoPerRBGroup.set_length(nRBGroups);
//   Array<ivec> resourceUnits = segmentVecToArrayOfVecs(getIntegers(0,nRUs-1),nRBGroups);
//   Array<ivec> cqiIndices;
//   Array<vec> effSINRs;
//   ivec pmiIndices;
//   ivec contendingServiceNodeList;
//   int rbLogicalIndex;
//   Array<vec> mcsRatesPerCodeword;
//   vec mcsRatesPerUser;
// //   bool isLocalised;
//     
//   
//   if(isDownlink)
//   {
//     ivec srvcNodeIndices=find(macScheduler.dlServiceNodesInfo->associatedServiceNodes,serviceNodes);// Useful only when multiple serverNodes are under a CU
//     for(int cnt=0;cnt<serviceNodes.length();cnt++)
//       if(macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).fallbackConfig.isEnabled)
//       {
// 	if(sourceAndSink.isRetransmitting(macScheduler.dlServiceNodesInfo->associatedServiceNodes(srvcNodeIndices(cnt)),subFrameNumber,macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).originalConfig.nCodeWords))
// 	{
// 	  cout<<"Falling back to SFBC for usr : "<<serviceNodes(cnt)<<endl;
// 	  macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).nLayers=macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).fallbackConfig.txModeConfig.nLayers;
// 	  macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).nCodeWords=macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).fallbackConfig.txModeConfig.nCodeWords;
// 	  macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).transmissionMode=macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).fallbackConfig.txModeConfig.transmissionMode;
// 	  macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).codeWordIndicesToConsider=sourceAndSink.getCodewordsToConsider(macScheduler.dlServiceNodesInfo->associatedServiceNodes(srvcNodeIndices(cnt)),subFrameNumber,macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).nCodeWords);
// 	  cout<<macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).codeWordIndicesToConsider<<endl;
// 	}
// 	else
// 	{
// 	  macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).nLayers=macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).originalConfig.nLayers;
// 	  macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).nCodeWords=macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).originalConfig.nCodeWords;
// 	  macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).transmissionMode=macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).originalConfig.transmissionMode;
// 	  macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).codeWordIndicesToConsider=getIntegers(0,macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).originalConfig.nCodeWords-1);
// 	}
//       }
//   }   
//   
//   //Rank Update
//   if(isDownlink && isRankAdaptationEnabled)
//   {
//     ivec srvcNodeIndices=find(macScheduler.dlServiceNodesInfo->associatedServiceNodes,serviceNodes);// Useful only when multiple serverNodes are under a CU
//     for(int cnt=0;cnt<serviceNodes.length();cnt++)
//     {
//       if(cqiInfo->getLatestDLCQIInfo(serviceNodes(cnt)).isValid && cqiInfo->getLatestNumCodewordsInDL(serviceNodes(cnt))>0)
//       {
// 	macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).nLayers=cqiInfo->getLatestNumofLayersInDL(serviceNodes(cnt));
// 	macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).nCodeWords=cqiInfo->getLatestNumCodewordsInDL(serviceNodes(cnt));
// 	macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).codeWordIndicesToConsider=sourceAndSink.getCodewordsToConsider(macScheduler.dlServiceNodesInfo->associatedServiceNodes(srvcNodeIndices(cnt)),subFrameNumber,cqiInfo->getLatestNumCodewordsInDL(serviceNodes(cnt)));
// 	if(macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).transmissionMode==_TRANSMISSION_MODE_3_ && macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).nLayers==1)
// 	{
// 	  macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).transmissionMode=_TRANSMISSION_MODE_2_;
// 	  macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).nLayers=macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).rsInfo.crsPorts.length();
// 	}
// 	else if(macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).transmissionMode==_TRANSMISSION_MODE_2_ && macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).nLayers>1)
// 	{
// 	  macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).transmissionMode=_TRANSMISSION_MODE_3_;
// 	}
// 	else if(macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).transmissionMode==_TRANSMISSION_MODE_2_)
// 	  macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).nLayers=macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).rsInfo.crsPorts.length();
// 
//       }
//       else
//       {
// 	if(macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).originalConfig.transmissionMode==_TRANSMISSION_MODE_3_ && macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).originalConfig.transmissionMode==_TRANSMISSION_MODE_4_)
// 	{
// 	  macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).nLayers=macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).rsInfo.crsPorts.length();
// 	  macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).nCodeWords=1;
// 	  macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).transmissionMode=_TRANSMISSION_MODE_2_;
// 	  macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndices(cnt)).codeWordIndicesToConsider=ivec("0");
// 	}
//       }
//     }
//   }
//   
//   ivec srvcNodeIndices;
//   if(isDownlink)
//     srvcNodeIndices=find(macScheduler.dlServiceNodesInfo->associatedServiceNodes,serviceNodes);
//   else
//     srvcNodeIndices=find(macScheduler.ulServiceNodesInfo->associatedServiceNodes,serviceNodes);
//   
// 
//   Array<TransmissionConfig_S> myTxConfigs;
//   
//   if(loadInPercentage!=-1 && maximumServiceNodesPerSubframe!=-1)
//   {
//     cout<<"[detl:]Error. Random load scheduling and maximumServiceNodesPerSubframe cannot be enabled simultaneously."<<endl;
//     abort();
//   }
//   int schedulerResolution=1; // Scheduler resolution in RBGs
//   if(maximumServiceNodesPerSubframe!=-1 && serviceNodes.length()>maximumServiceNodesPerSubframe)
//   {
//     schedulerResolution=ceil_i((double)nRBGroups/(double)maximumServiceNodesPerSubframe);
//   }
// #ifdef USING_CALIBRATION  
//   ivec selectedRBGs=zeros_i(2)-1;
//   if(cqiInfo->latestsubframeNumberForDLCQIUpdateInScheduler!=-1)
//   {
//     PerUserDLCQIFeedback_S cqifb=cqiInfo->dlCqiInfoPerServiceNodeForScheduler(0);
//     ivec sbCQI(0);
//     for(int sb_cnt=0;sb_cnt<cqifb.subbandCqiInfoPerGroupHigherLayer(0).cqiIndexPerCodeWord.length();sb_cnt++) 
//       append(sbCQI,cqifb.subbandCqiInfoPerGroupHigherLayer(0).cqiIndexPerCodeWord(sb_cnt)(0));
//     
//     
//     
//     if(traceValuePerTraceName["PUSCH_2_0subband"]=="true" || traceValuePerTraceName["PUSCH_2_0wideband"]=="true")
//     {
//       cqifb.subbandCqiInfoPerGroupUESelected(0).subbandIndex.set_size(5);
//       ivec subbandNumbers=cqifb.subbandCqiInfoPerGroupUESelected(0).subbandIndex;
//       int selectedSubbandNum=randi(0,4);
//       selectedRBGs(0)=subbandNumbers(selectedSubbandNum);
//       if (selectedRBGs(0)==16)
// 	selectedRBGs(0)=15;
//       selectedRBGs(1)=40;
//       cout << "selectedRBGs ::" << selectedRBGs(0) << endl;
//     }
//     
//     
//     int bestSB;
//     if(traceValuePerTraceName["PUSCH_3_0caseBsubband"]=="true")
//    {
//       bestSB=max_index(sbCQI(0,7));
//       selectedRBGs(0)=2*bestSB;
//       selectedRBGs(1)=2*bestSB+1;
//    }
//    if(traceValuePerTraceName["PUSCH_3_0caseBwideband"]=="true")
//    {
//      int bestSB;
//      bestSB=randi(0,7);
//      selectedRBGs(0)=2*bestSB;
//      selectedRBGs(1)=2*bestSB+1;
//    }
//    
//   }
// #endif  
// 
//   // Commented the UE shuffling
// //   if(serviceNodes.length()<=nRBGroups)
// //   {
// //     ivec shuffleindices=getIntegers(0,serviceNodes.length()-1);
// //     shuffleindices=shuffleVec(shuffleindices);
// //     serviceNodes=serviceNodes(shuffleindices);
// //     myServiceNodesFeedbackIndices=myServiceNodesFeedbackIndices(shuffleindices);
// //   }
// 
//   //For dividing UEs 
//   if(!isDownlink)
//   {
//     int uplinkSchResolution;
//     if(serviceNodes.length()<=nRBGroups)
//     {
//       uplinkSchResolution=floor_i((double)nRBGroups/(double)serviceNodes.length());
//       if(uplinkSchResolution<resourceMapper.ulResourceMapper.getResourceBlockGroupingSize()) //To avoid allocation becoming very small
// 	uplinkSchResolution=resourceMapper.ulResourceMapper.getResourceBlockGroupingSize();
//     }
//     else
//     {
//       uplinkSchResolution=resourceMapper.ulResourceMapper.getResourceBlockGroupingSize();
//     }
//     if(schedulerResolution!=1)
//     {
//       if(schedulerResolution<uplinkSchResolution)
// 	schedulerResolution=uplinkSchResolution;
//     }
//     else
//       schedulerResolution=uplinkSchResolution;
// 	
//   }
//   
//   int lastScheduledRBG=-1;
//   for(int rbGroup_cnt=0,schRBGs=0;rbGroup_cnt<nRBGroups;rbGroup_cnt++)
//   {
//     rbGroups(rbGroup_cnt).resourceBlocks=resourceUnits(rbGroup_cnt);
//     
//     if(find(reservedRBGs,rbGroup_cnt)==-1)
//     {
//       if((loadInPercentage!=-1 && (randu()<(loadInPercentage/100.0))) || loadInPercentage==-1)
//   #ifdef USING_CALIBRATION  
//       if(selectedRBGs(0)==rbGroup_cnt || selectedRBGs(1)==rbGroup_cnt || traceValuePerTraceName["schedule_in_aband"]!="true") 
//   #endif	
//       {  
// 	if(schRBGs%schedulerResolution==0)
// 	{
// 	  if(isDownlink) myTxConfigs=macScheduler.dlServiceNodesInfo->getTxConfig(serviceNodes);
// 	  else	myTxConfigs=macScheduler.ulServiceNodesInfo->getTxConfig(serviceNodes);
// 	  cqiIndices.set_length(0);
// 	  effSINRs.set_length(0);
// 	  pmiIndices.set_length(0);
// 	  contendingServiceNodeList.set_length(0);
// 	  rbLogicalIndex=rbGroups(rbGroup_cnt).resourceBlocks(0);
// 	  
// // 	  if(isDownlink)
// // 	    isLocalised=resourceMapper.dlResourceMapper.isLocalised(rbLogicalIndex);
// // 	  else
// // 	    isLocalised=true;
// 	  //! If CQI feedback is available...
// 	  if((isDownlink && cqiInfo->latestsubframeNumberForDLCQIUpdateInScheduler!=-1) || (!isDownlink && cqiInfo->latestsubframeNumberForULCQIUpdateInScheduler!=-1))
// 	  {
// 	    ivec serviceNodesWithCQI0(0);
// 	    
// 	    if(isDownlink)
// 	    {
// 	      for(int fb_cnt=0;fb_cnt<myServiceNodesFeedbackIndices.length();fb_cnt++)
// 	      {
// 		if(myServiceNodesFeedbackIndices(fb_cnt)!=-1)
// 		{
// 		  ivec cqiPerCodeword;int PMI;
// 		  int srvcNodeIndx=find(macScheduler.dlServiceNodesInfo->associatedServiceNodes,serviceNodes(fb_cnt));
// 		  if(getLatestDLCQI(cqiInfo->getLatestDLCQIInfo(serviceNodes(fb_cnt)),macScheduler.dlServiceNodesInfo->cqiComputationConfigForEachServiceNode(srvcNodeIndx)(0).isAPeriodicCQIConfigured,macScheduler.dlServiceNodesInfo->cqiComputationConfigForEachServiceNode(srvcNodeIndx)(0).dlCQIModeInfo.aperiodicMode,macScheduler.dlServiceNodesInfo->cqiComputationConfigForEachServiceNode(srvcNodeIndx)(0).dlCQIModeInfo.periodicMode,macScheduler.dlServiceNodesInfo->txConfigForEachServiceNode(srvcNodeIndx).codeWordIndicesToConsider,resourceMapper.dlResourceMapper.getResourceBlocksPerSubframe(),rbGroups(
// 		    rbGroup_cnt).resourceBlocks,cqiPerCodeword,PMI))
// 		  {
// 		    if(isLinkSimulation)
// 		    {
//   #ifdef USING_CALIBRATION		    
//   if(traceValuePerTraceName["RITesting"]!="true" && traceValuePerTraceName["cqiawgntesting"]!="true" && traceValuePerTraceName["PUSCH_3_0caseA"]!="true" && traceValuePerTraceName["PUSCH_3_0caseBsubband"]!="true" && traceValuePerTraceName["PUSCH_3_0caseBwideband"]!="true" && traceValuePerTraceName["PUSCH_2_0subband"]!="true" && traceValuePerTraceName["PUSCH_2_0wideband"]!="true")
//   #endif		      
// 			append(cqiIndices,ones_i(myTxConfigs(fb_cnt).nCodeWords)*defaultCQIIndex);
// 
//   #ifdef USING_CALIBRATION
// 			if(traceValuePerTraceName["RITesting"]=="true" || traceValuePerTraceName["cqiawgntesting"]=="true" || traceValuePerTraceName["PUSCH_3_0caseA"]=="true" || traceValuePerTraceName["PUSCH_3_0caseBsubband"]=="true" || traceValuePerTraceName["PUSCH_3_0caseBwideband"]=="true" || traceValuePerTraceName["PUSCH_2_0subband"]=="true" || traceValuePerTraceName["PUSCH_2_0wideband"]=="true")
// 			  append(cqiIndices,cqiPerCodeword);
//   #endif
// 		      append(pmiIndices,PMI);
// 		    }
// 		    
// 		    else
// 		    {
// 		      if(find(cqiPerCodeword,0)!=-1)
// 			append(serviceNodesWithCQI0,serviceNodes(fb_cnt));
// 		      else
// 		      {
// 			append(cqiIndices,cqiPerCodeword);
// 			append(pmiIndices,PMI);
// 		      }
// 		    }
// 		    
// 		  }
// 		  else
// 		  {
// 		    append(pmiIndices,0);
// 		    if(isLinkSimulation)
// 		      append(cqiIndices,ones_i(myTxConfigs(fb_cnt).nCodeWords)*defaultCQIIndex);
// 		    else
// 		      append(cqiIndices,ones_i(myTxConfigs(fb_cnt).nCodeWords));
// 		  }
// 		}
// 		else
// 		{
// 		  append(pmiIndices,0);
// 		  if(isLinkSimulation)
// 		    append(cqiIndices,ones_i(myTxConfigs(fb_cnt).nCodeWords)*defaultCQIIndex);
// 		  else
// 		    append(cqiIndices,ones_i(myTxConfigs(fb_cnt).nCodeWords));
// 		}
// 	      }
// 	      contendingServiceNodeList=serviceNodes;
// 	    }
// 	    else	//! Uplink
// 	    {
// 	      //! For uplink Scheduling, ensure that RB-Groups are contiguous...
// 	      for(int fb_cnt=0;fb_cnt<myServiceNodesFeedbackIndices.length();fb_cnt++)
// 	      {
// 		if(isServiceNodeEligibleForUplinkScheduling(cqiInfo->serviceNodeIDs(myServiceNodesFeedbackIndices(fb_cnt)),scheduledServiceNodesInfoPerRBGroup(0,rbGroup_cnt),resourceUnits(0,rbGroup_cnt),rbGroup_cnt))
// 		{
// 		  if(myServiceNodesFeedbackIndices(fb_cnt)!=-1)	//! CQI information is available...
// 		  {
// 		    if(cqiInfo->ulCqiInfoPerServiceNodeForScheduler(myServiceNodesFeedbackIndices(fb_cnt)).subbandCqiInfoPerGroup.length())
// 		    {
// 		      int rbIndx=find(cqiInfo->ulCqiInfoPerServiceNodeForScheduler(myServiceNodesFeedbackIndices(fb_cnt)).subbandCqiInfoPerGroup(0).vrbIndex,rbLogicalIndex);
// 		      if(rbIndx!=-1)
// 		      {
// 			int sbIndx=cqiInfo->ulCqiInfoPerServiceNodeForScheduler(myServiceNodesFeedbackIndices(fb_cnt)).subbandCqiInfoPerGroup(0).subbandIndexPerRB(rbIndx);
// 
// 			if(cqiInfo->ulCqiInfoPerServiceNodeForScheduler(myServiceNodesFeedbackIndices(fb_cnt)).isValid && sbIndx!=-1)
// 			{
//   // 			cout<<"Valid CQI available for RB : "<<rbIndx<<" for UE : "<<serviceNodes(fb_cnt)<<endl;
// 			  if(isLinkSimulation)
// 			  {
// 			    append(effSINRs,ones(myTxConfigs(fb_cnt).nCodeWords)*-5);
// 			    if(cqiInfo->ulCqiInfoPerServiceNodeForScheduler(myServiceNodesFeedbackIndices(fb_cnt)).subbandCqiInfoPerGroup(0).pmiIndex.length())
// 			      append(pmiIndices,cqiInfo->ulCqiInfoPerServiceNodeForScheduler(myServiceNodesFeedbackIndices(fb_cnt)).subbandCqiInfoPerGroup(0).pmiIndex(sbIndx));
// 			    
// 			  }
// 			  else
// 			  {
// 			    if(sum(to_vec(find(cqiInfo->ulCqiInfoPerServiceNodeForScheduler(myServiceNodesFeedbackIndices(fb_cnt)).subbandCqiInfoPerGroup(0).effSINRPerCodeWord(sbIndx) < -6.5)))!= 0)
// 			      append(serviceNodesWithCQI0,serviceNodes(fb_cnt));
// 			    else
// 			    {
// 			      append(effSINRs,cqiInfo->ulCqiInfoPerServiceNodeForScheduler(myServiceNodesFeedbackIndices(fb_cnt)).subbandCqiInfoPerGroup(0).effSINRPerCodeWord(sbIndx));
// 			      if(cqiInfo->ulCqiInfoPerServiceNodeForScheduler(myServiceNodesFeedbackIndices(fb_cnt)).subbandCqiInfoPerGroup(0).pmiIndex.length())
// 				append(pmiIndices,cqiInfo->ulCqiInfoPerServiceNodeForScheduler(myServiceNodesFeedbackIndices(fb_cnt)).subbandCqiInfoPerGroup(0).pmiIndex(sbIndx));
// 			      
// 			    }
// 			  }
// 			}
// 			else
// 			{
// 			  append(pmiIndices,0);
// 			  append(effSINRs,ones(myTxConfigs(fb_cnt).nCodeWords)*-5.0);
// 			}
// 			append(contendingServiceNodeList,serviceNodes(fb_cnt));
// 		      }
// 		    
// 		      else 
// 		      {
// 			append(pmiIndices,0);
// 			append(contendingServiceNodeList,serviceNodes(fb_cnt));
// 			append(effSINRs,ones(myTxConfigs(fb_cnt).nCodeWords)*-5.0);
// 		      }
// 		    }
// 		    else
// 		    {
// 		      append(pmiIndices,0);
// 		      append(contendingServiceNodeList,serviceNodes(fb_cnt));
// 		      append(effSINRs,ones(myTxConfigs(fb_cnt).nCodeWords)*-5.0);
// 		    }
// 		  }
// 		  else	//! CQI information is not available...
// 		  {
// 		    append(contendingServiceNodeList,serviceNodes(fb_cnt));
// 		    append(pmiIndices,0);
// 		    append(effSINRs,ones(myTxConfigs(fb_cnt).nCodeWords)*-5.0);
// 		  }
// 		}
// 		else //! Even if the user is ineligible, he is in the contendingServiceNodeList but with CQI 0
// 		{
// 		  append(pmiIndices,0);
// 		  append(contendingServiceNodeList,serviceNodes(fb_cnt));
// 		  append(effSINRs,ones(myTxConfigs(fb_cnt).nCodeWords)-9.0);
// 		}
// 	      }
// 	    }
// 	    
// 	    if(serviceNodesWithCQI0.length()>0)
// 	    {
// 	      cout<<"The following users are removed from the scheduling because the PPSINR is < -6.5 dB"<<endl;
// 	      cout<<serviceNodesWithCQI0<<endl;
// 	      contendingServiceNodeList = remove(contendingServiceNodeList,serviceNodesWithCQI0);
// 	    }
// 	    mcsRatesPerCodeword.set_length(contendingServiceNodeList.length());
// 	    for(int usr_cnt = 0; usr_cnt < contendingServiceNodeList.length(); usr_cnt++)
// 	    {
// 	      vec rateOffset;
// 	      if(isDownlink)
// 	      {
// 		rateOffset = zeros(cqiIndices(usr_cnt).length());
// 		if(macScheduler.isProportinallyFairScheduler && macScheduler.dlServiceNodesInfo->outerLoopLinkAdaptationInfo.isEnabled)
// 		  rateOffset = macScheduler.dlServiceNodesInfo->outerLoopLinkAdaptationInfo.getRateOffset(cqiIndices(usr_cnt),contendingServiceNodeList(usr_cnt));
// 	      }
// 	      if(isDownlink)
// 		mcsRatesPerCodeword(usr_cnt) = sourceAndSink.lteTables.cqiToMaxMCSRate.getMaximumMCSRate(cqiIndices(usr_cnt)) + rateOffset;
// 	      else
// 		mcsRatesPerCodeword(usr_cnt) = sourceAndSink.lteTables.cqiToMaxMCSRate.getMaximumMCSRate(sourceAndSink.lteTables.awgnTablesConvCoder.getCQIPerCWfromSINR(effSINRs(usr_cnt)));
// 	    }
// 	    
// 	    mcsRatesPerUser=getTotalMCSRate(mcsRatesPerCodeword,myTxConfigs);
// 	    scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt)=macScheduler.findBestUser(isDownlink,serverNodeID,mcsRatesPerUser,contendingServiceNodeList,pmiIndices,codeBook);
// 	  }
// 	  else	//! No CQI feedback case...
// 	  {
// 
// 	    ivec myTxConfigsIndices;
// 	    //!Given the RBG size and Scheduling Resolution are adjusted to accomodate users properly, filtering specific to uplink not required- 10010
// 	    if(isDownlink )
// 	    {
// 	      contendingServiceNodeList=serviceNodes;
// 	      myTxConfigsIndices=getIntegers(0,myTxConfigs.length()-1);
// 	      cqiIndices.set_length(contendingServiceNodeList.length());
// 	    }
// 	    else
// 	    {
// 	      contendingServiceNodeList = getServiceNodeIDsEligibleForUplinkScheduling(serviceNodes,scheduledServiceNodesInfoPerRBGroup(0,rbGroup_cnt),resourceUnits(0,rbGroup_cnt),rbGroup_cnt);
// 	      myTxConfigsIndices=find(serviceNodes,contendingServiceNodeList);
// 	      effSINRs.set_length(contendingServiceNodeList.length());
// 	    }
// 	    
// 	    
// 	    for(int srvcNode_cnt=0;srvcNode_cnt<contendingServiceNodeList.length();srvcNode_cnt++)
// 	    {
// 	      if(isDownlink)
// 		cqiIndices(srvcNode_cnt)=ones_i(myTxConfigs(myTxConfigsIndices(srvcNode_cnt)).nCodeWords)*defaultCQIIndex;
// 	      else
// 		effSINRs(srvcNode_cnt)=ones(myTxConfigs(myTxConfigsIndices(srvcNode_cnt)).nCodeWords)*-5.0;
// 	    }  
// 	    
// 	    pmiIndices=zeros_i(serviceNodes.length());
// 	    scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt)=macScheduler.findBestUser(isDownlink,serverNodeID,"",contendingServiceNodeList,pmiIndices,codeBook);
// 	  }
// 	  // Restricting to 1 RBG for the Retransmitting UEs
// 	  if(isRestrictionForReTx && sourceAndSink.isRetransmitting(scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).userIDs(0),subFrameNumber,scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).txConfig->nCodeWords))
// 	  {
// 	    if(traceValuePerTraceName["EnablePrints"]=="true")
// 	      cout<<"[detl:]Retransmitting User "<<scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).userIDs(0)<<endl<<endl;
// 	    int indxtodel=find(serviceNodes,scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).userIDs(0));
// 	    serviceNodes.del(indxtodel);
// 	  srvcNodeAntennaCnt.del(indxtodel);
// 	    myServiceNodesFeedbackIndices.del(indxtodel);
// 	  }
// 	  
// 	  if(isDownlink)
// 	    scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).cqiIndices.set_length(scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).userIDs.length());
// 	  else
// 	    scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).effSINRs.set_length(scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).userIDs.length());
// 	  
// 	  rbGroups(rbGroup_cnt).dataGenInfo.set_length(scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).userIDs.length());
// 	  rbGroups(rbGroup_cnt).pmiIndexPerUser.set_length(scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).userIDs.length());
// 	  for(int usr_cnt=0;usr_cnt<scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).userIDs.length();usr_cnt++)
// 	  {
// 	    rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).transmissionScheme=getTransmissionScheme(scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).txConfig->transmissionMode);
// 	    
// // 	  rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).rbInfo.rsInfo=scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).txConfig->rsInfo;
// 	    rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).rbInfo.rsInfo.cellid=idCell;
// 	    rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).cellID=idCell;
// 	    rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).nLayers=scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).txConfig->nLayers;
// 	    rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).nCodeWords=scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).txConfig->nCodeWords;
// 	    rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).rnti=scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).rnti(usr_cnt);
// 	    
// 	    if(isDownlink)
// 	  {  
// 	    setRsInfo(rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).rbInfo.rsInfo,portInfo,frameStructure,idCell,rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).transmissionScheme,rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).nLayers,srvrNodeAntennaCnt,scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).txConfig->rsInfo.rsBoostInDB,1);
// 	  scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).cqiIndices(usr_cnt)=cqiIndices(find(contendingServiceNodeList,scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).userIDs(usr_cnt)));
// 	  }
// 	    else
// 	  {
// 	    setRsInfo(rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).rbInfo.rsInfo,portInfo,frameStructure,idCell,rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).transmissionScheme,rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).nLayers,srvcNodeAntennaCnt(find(serviceNodes, rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).rnti)),scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).txConfig->rsInfo.rsBoostInDB,0);
// 	      scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).effSINRs(usr_cnt)=effSINRs(find(contendingServiceNodeList,scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).userIDs(usr_cnt)))(0,scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).txConfig->nCodeWords-1);
// 	  }
// 	    
// 	    
// 	    if(rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).transmissionScheme== _TRANSMISSION_SCHEME_CL_SM_||rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).transmissionScheme==_TRANSMISSION_SCHEME_CL_MU_MIMO_||rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).transmissionScheme== _TRANSMISSION_SCHEME_CL_BF_||rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).transmissionScheme==_TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_||rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).transmissionScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).transmissionScheme==_TRANSMISSION_SCHEME_NR_CL_)
// 	      rbGroups(rbGroup_cnt).pmiIndexPerUser(usr_cnt)=pmiIndices(find(contendingServiceNodeList,scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).userIDs(usr_cnt)));
// 	    rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).symbolRepetitionFactor=getSymbolRepetitionFactor(rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).transmissionScheme,rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).nLayers);
// 	    rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).mimoRate=(double)rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).nLayers/(double)(rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).symbolRepetitionFactor);
// 	    rbGroups(rbGroup_cnt).dataGenInfo(usr_cnt).scheduledPRBs=rbGroups(rbGroup_cnt).resourceBlocks.length();
// 	  }		
// 	  lastScheduledRBG=rbGroup_cnt;
// 	  schRBGs++;
// 	}
// 	else
// 	{
// 	  rbGroups(rbGroup_cnt)= rbGroups(lastScheduledRBG);
// 	  rbGroups(rbGroup_cnt).resourceBlocks=resourceUnits(rbGroup_cnt);
// 	  scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt)=scheduledServiceNodesInfoPerRBGroup(lastScheduledRBG);
// 	  schRBGs++;
// 	}
//       }
//     
//       
//     }
//   
//   }
// }

// zeroWarning case handled below
// void createDataBlkInfo(Array< DataBlkInfo_S >& dataBlkInfoPerServiceNode, SchedulerOutput_S& schedulerOutput, int srvrNodeIndx, ivec& activeServiceNodes, Array< RBGroupInfo_S >& rbGroups, Array< SchedulerInfo_S >& scheduledServiceNodesInfoPerRBGroup, RBInfo_S& rbInfoForUnscheduled, ivec defaultMCSIndices, bool isDownlink, ServiceNodesInfo_S& serviceNodesInfo,int subFrameNumber){
//    
//   cout<<endl<<"Error... createDataBlkInfo() function is no longer used, please use createDataBlkInfoNew() function"<<endl;abort();  
// }

// void createDataBlkInfoNew(Array< DataBlkInfo_S >& dataBlkInfoPerServiceNode, SchedulerOutput_S& schedulerOutput, int srvrNode, ivec& activeServiceNodes, Array< RBGroupInfo_S >& rbGroups, Array< SchedulerInfo_S >& scheduledServiceNodesInfoPerRBGroup, RBInfo_S& rbInfoForUnscheduled,ivec defaultMCSIndices, int isDownlink, ServiceNodesInfo_S& serviceNodesInfo){
//   
//   int nRBGroups=rbGroups.length();
//   //Updating Framestructure;
//   if(isDownlink)
//   {
//     int srvrNodeIndx = getTxSrvrNodeIndx(schedulerOutput.schedulerToTransmitterInfo,srvrNode);
//     for(int rbg_cnt=0;rbg_cnt<nRBGroups;rbg_cnt++)
//       if(rbGroups(rbg_cnt).dataGenInfo.length())
//       {
// 	RBMergeInfo_S mergedRbInfo=generateRBStructure(schedulerOutput.schedulerToTransmitterInfo(srvrNodeIndx).frameStructure,rbGroups(rbg_cnt).dataGenInfo(0).rbInfo.rsInfo.rsPorts,rbGroups(rbg_cnt).resourceBlocks,isDownlink,schedulerOutput.schedulerToTransmitterInfo(srvrNodeIndx).resourceMapper->dlResourceMapper.isLocalised(rbGroups(rbg_cnt).resourceBlocks(0)),schedulerOutput.schedulerToTransmitterInfo(srvrNodeIndx).frameReservationInfo);
// 	for(int rb_cnt=0;rb_cnt<rbGroups(rbg_cnt).resourceBlocks.length();rb_cnt++)
// 	  schedulerOutput.schedulerToTransmitterInfo(srvrNodeIndx).frameStructure->updateRUType(rbGroups(rbg_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 	for(int usr_cnt=0;usr_cnt<rbGroups(rbg_cnt).dataGenInfo.length();usr_cnt++)
// 	  for(int rb_cnt=0;rb_cnt<rbGroups(rbg_cnt).resourceBlocks.length();rb_cnt++)
// 	    append(rbGroups(rbg_cnt).dataGenInfo(usr_cnt).resourceElementsPerRB,schedulerOutput.schedulerToTransmitterInfo(srvrNodeIndx).frameStructure->getToneCountPerRU(rbGroups(rbg_cnt).resourceBlocks(rb_cnt),"data"));
//       }
//       else //unscheduled RBGs (filled only with CQI Ports)
//       {
// 	RBMergeInfo_S mergedRbInfo=generateRBStructure(schedulerOutput.schedulerToTransmitterInfo(srvrNodeIndx).frameStructure,rbInfoForUnscheduled.rsInfo.rsPorts,rbGroups(rbg_cnt).resourceBlocks,isDownlink,schedulerOutput.schedulerToTransmitterInfo(srvrNodeIndx).resourceMapper->dlResourceMapper.isLocalised(rbGroups(rbg_cnt).resourceBlocks(0)),schedulerOutput.schedulerToTransmitterInfo(srvrNodeIndx).frameReservationInfo);
// 	for(int rb_cnt=0;rb_cnt<rbGroups(rbg_cnt).resourceBlocks.length();rb_cnt++)
// 	  schedulerOutput.schedulerToTransmitterInfo(srvrNodeIndx).frameStructure->updateRUType(rbGroups(rbg_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
//       }
//       dataBlkInfoPerServiceNode=mergeRBGroups(rbGroups,activeServiceNodes,scheduledServiceNodesInfoPerRBGroup/*,schedulerOutput.schedulerToTransmitterInfo(srvrNodeIndx).sourceAndSink->lteTables.cqiToMaxMCSRate*/,&(schedulerOutput.schedulerToTransmitterInfo(srvrNodeIndx).sourceAndSink->lteTables),*(schedulerOutput.schedulerToTransmitterInfo(srvrNodeIndx).codebook),serviceNodesInfo,/*subFrameNumber,*/isDownlink,defaultMCSIndices);
//   }
//   else //! uplink
//   {
//     int srvrNodeIndx=getRxSrvrNodeIndx(schedulerOutput.schedulerToReceiverInfo,srvrNode);
//     for(int rbg_cnt=0;rbg_cnt<nRBGroups;rbg_cnt++)
//       if(rbGroups(rbg_cnt).dataGenInfo.length())
//       { 
// 	RBMergeInfo_S mergedRbInfo=generateRBStructure(schedulerOutput.schedulerToReceiverInfo(srvrNodeIndx).frameStructure,rbGroups(rbg_cnt).dataGenInfo(0).rbInfo.rsInfo.rsPorts,rbGroups(rbg_cnt).resourceBlocks,isDownlink,true,schedulerOutput.schedulerToReceiverInfo(srvrNodeIndx).frameReservationInfo);
// 	for(int rb_cnt=0;rb_cnt<rbGroups(rbg_cnt).resourceBlocks.length();rb_cnt++){
// 	  schedulerOutput.schedulerToReceiverInfo(srvrNodeIndx).frameStructure->updateRUType(rbGroups(rbg_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 	}
// 	  for(int usr_cnt=0;usr_cnt<rbGroups(rbg_cnt).dataGenInfo.length();usr_cnt++)
// 	  for(int rb_cnt=0;rb_cnt<rbGroups(rbg_cnt).resourceBlocks.length();rb_cnt++)
// 	  {
// 	    append(rbGroups(rbg_cnt).dataGenInfo(usr_cnt).resourceElementsPerRB,schedulerOutput.schedulerToReceiverInfo(srvrNodeIndx).frameStructure->getToneCountPerRU(rbGroups(rbg_cnt).resourceBlocks(rb_cnt),"data"));
// 	  }
//       }
//       //unscheduled RBGs (filled only with CQI Ports)
//       else
//       {
// 	RBMergeInfo_S mergedRbInfo=generateRBStructure(schedulerOutput.schedulerToReceiverInfo(srvrNodeIndx).frameStructure,rbInfoForUnscheduled.rsInfo.rsPorts,rbGroups(rbg_cnt).resourceBlocks,isDownlink,true,schedulerOutput.schedulerToReceiverInfo(srvrNodeIndx).frameReservationInfo);
// 	for(int rb_cnt=0;rb_cnt<rbGroups(rbg_cnt).resourceBlocks.length();rb_cnt++)
// 	{
// 	  if(mergedRbInfo.mergedRBName(rb_cnt)=="") mergedRbInfo.mergedRBName(rb_cnt)="nullRB";
// 	  schedulerOutput.schedulerToReceiverInfo(srvrNodeIndx).frameStructure->updateRUType(rbGroups(rbg_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 	}
//       }
//       dataBlkInfoPerServiceNode=mergeRBGroups(rbGroups,activeServiceNodes,scheduledServiceNodesInfoPerRBGroup,/*schedulerOutput.schedulerToReceiverInfo(srvrNodeIndx).sourceAndSink->lteTables.cqiToMaxMCSRate,*/&(schedulerOutput.schedulerToReceiverInfo(srvrNodeIndx).sourceAndSink->lteTables),*(schedulerOutput.schedulerToReceiverInfo(srvrNodeIndx).codebook),serviceNodesInfo,/*subFrameNumber,*/isDownlink,defaultMCSIndices);
//       
//   }  
//   
// }

// zeroWarning case handled below
// void addDataBlksToSchedulerOutput(ServiceNodesInfo_S *serviceNodesInfo, ivec activeServiceNodes,SchedulerOutput_S &schedulerOutput, int nuID,Array<DataBlkInfo_S> &dataBlkInfoPerServiceNode,ivec serverNodes, int srvrNodeIndx ,vec txPowersInDBm, double &txSubcarrierPowersInDBm,/* bool isWithAbstraction,*/ bool isDownlink, int subFrameNumber, bool isLinkSimulation){
//   
//   int subcarriersPerRB;
//   if(isDownlink)
//     subcarriersPerRB=schedulerOutput.schedulerToTransmitterInfo(srvrNodeIndx).resourceMapper->dlResourceMapper.getRBSize().numberOfSubcarriers;
//   else
//     subcarriersPerRB=schedulerOutput.schedulerToReceiverInfo(srvrNodeIndx).resourceMapper->ulResourceMapper.getRBSize().numberOfSubcarriers;
//   
//   
//   //! Copying Informations to proper structures, to pass it to the Transmitter & Receiver...
//     for(int srvcNode_cnt=0;srvcNode_cnt<dataBlkInfoPerServiceNode.length();srvcNode_cnt++)
//       if(dataBlkInfoPerServiceNode(srvcNode_cnt).resourceBlocks.length()>0)
//       {
// 	// Generating Merged Pattern ...
// 	
// 	if(serviceNodesInfo->getCQIComputationConfig(activeServiceNodes(srvcNode_cnt))(0).cqiMeasurementConfig(0).transmissionScheme!=dataBlkInfoPerServiceNode(srvcNode_cnt).dataGenInfo.transmissionScheme
// 	  ||
// 	  serviceNodesInfo->getCQIComputationConfig(activeServiceNodes(srvcNode_cnt))(0).cqiMeasurementConfig(0).numberOfLayers!=dataBlkInfoPerServiceNode(srvcNode_cnt).dataGenInfo.nLayers)
// 	  serviceNodesInfo->updateCQIComputationConfig(activeServiceNodes(srvcNode_cnt),dataBlkInfoPerServiceNode(srvcNode_cnt).dataGenInfo.transmissionScheme,dataBlkInfoPerServiceNode(srvcNode_cnt).dataGenInfo.rbInfo.rsInfo.CQIports,dataBlkInfoPerServiceNode(srvcNode_cnt).dataGenInfo.nLayers,dataBlkInfoPerServiceNode(srvcNode_cnt).dataGenInfo.nCodeWords); 
// 	
// 	  if(traceValuePerTraceName["pdschDemodPerfTesting"]=="true")
// 	  {
// 	    int indx=find(schedulerOutput.schedulerToTransmitterInfo(srvrNodeIndx).sourceAndSink->serviceNodeIDs,activeServiceNodes(srvcNode_cnt));
// 	    if(dataBlkInfoPerServiceNode(srvcNode_cnt).dataGenInfo.mcsIndices(0)<17)
// 		schedulerOutput.schedulerToTransmitterInfo(srvrNodeIndx).sourceAndSink->harqManagers(indx).updateRVSeqRateMatching("0 1 2 3");
// 	    else
// 		schedulerOutput.schedulerToTransmitterInfo(srvrNodeIndx).sourceAndSink->harqManagers(indx).updateRVSeqRateMatching("0 0 1 2");
// 	  }
// 	  if(isDownlink)
// 	    dataBlkInfoPerServiceNode(srvcNode_cnt).harqInfo = schedulerOutput.schedulerToTransmitterInfo(srvrNodeIndx).sourceAndSink->getCurrentHarqInfo(activeServiceNodes(srvcNode_cnt),dataBlkInfoPerServiceNode(srvcNode_cnt).dataGenInfo.nCodeWords,subFrameNumber);
// 	  else
// 	    dataBlkInfoPerServiceNode(srvcNode_cnt).harqInfo = schedulerOutput.schedulerToReceiverInfo(srvrNodeIndx).sourceAndSink->getCurrentHarqInfo(activeServiceNodes(srvcNode_cnt),dataBlkInfoPerServiceNode(srvcNode_cnt).dataGenInfo.nCodeWords,subFrameNumber);
// 	  
// 	  int txNodeID,rxNodeID;
// 	  if(isDownlink)
// 	  {
// 	    txNodeID=serverNodes(srvrNodeIndx);
// 	    rxNodeID=activeServiceNodes(srvcNode_cnt);
// 	    
// 	    int downlinkUsedSubcarriers=schedulerOutput.schedulerToTransmitterInfo(srvrNodeIndx).resourceMapper->dlResourceMapper.getUsedSubcarriers();
// 	    
// 	    if(isLinkSimulation)
// 	      txSubcarrierPowersInDBm=30;  /// Hard-coded to 0 dBW for link level simulations
// 	    else
// 	      txSubcarrierPowersInDBm=txPowersInDBm(srvrNodeIndx)-10*log10(downlinkUsedSubcarriers);
// 	  }
// 	  else
// 	  {
// 	    txNodeID=activeServiceNodes(srvcNode_cnt);
// 	    rxNodeID=serverNodes(srvrNodeIndx);
// 	    int uplinkUsedSubcarriers=subcarriersPerRB*dataBlkInfoPerServiceNode(srvcNode_cnt).resourceBlocks.length();
// 	    if(isLinkSimulation)
// 	      txSubcarrierPowersInDBm=30;/// Hard-coded to 0 dBW for link level simulations
// 	    else
// 	      txSubcarrierPowersInDBm=txPowersInDBm(srvcNode_cnt)-10*log10(uplinkUsedSubcarriers);
// 	  }
// 	  schedulerOutput.addTxDataBlk(txNodeID,rxNodeID,nuID,dataBlkInfoPerServiceNode(srvcNode_cnt),txSubcarrierPowersInDBm,isDownlink);
// 	    schedulerOutput.addRxDataBlk(rxNodeID,txNodeID,nuID,dataBlkInfoPerServiceNode(srvcNode_cnt),txSubcarrierPowersInDBm,isDownlink);
//       }
// }

// zeroWarning case handled below
// void updateUnscheduledFrame(SchedulerOutput_S &schedulerOutput, int srvrNode_cnt, int nRBs, ivec &txAntennaCount ,vec &txPowersInDBm, double &txSubcarrierPowersInDBm, RBInfo_S &rbInfoForUnscheduled,bool isDownlink){
//   
//   cout<<endl<<"Error... updateUnscheduledFrame() function is no longer used, please use updateUnscheduledFrameNew() function"<<endl;abort();;
// }

// void updateUnscheduledFrameNew(SchedulerOutput_S &schedulerOutput, int srvrNode, int nRBs, ivec &txAntennaCount ,vec &txPowersInDBm, double &txSubcarrierPowersInDBm, RBInfo_S &rbInfoForUnscheduled,int isDownlink){
//   if(isDownlink)
//   {
//     int srvrNode_cnt=getTxSrvrNodeIndx(schedulerOutput.schedulerToTransmitterInfo,srvrNode);
//     int downlinkUsedSubcarriers=schedulerOutput.schedulerToTransmitterInfo(srvrNode_cnt).resourceMapper->dlResourceMapper.getUsedSubcarriers();
//     
//     txSubcarrierPowersInDBm=txPowersInDBm(srvrNode_cnt)-10*log10(downlinkUsedSubcarriers)-10*log10(txAntennaCount(srvrNode_cnt));
//     ivec vrbs=getIntegers(0,nRBs-1);
//     RBMergeInfo_S mergedRbInfo=generateRBStructure(schedulerOutput.schedulerToTransmitterInfo(srvrNode_cnt).frameStructure,rbInfoForUnscheduled.rsInfo.rsPorts,vrbs,true,schedulerOutput.schedulerToTransmitterInfo(srvrNode_cnt).resourceMapper->dlResourceMapper.isLocalised(0),schedulerOutput.schedulerToTransmitterInfo(srvrNode_cnt).frameReservationInfo);
//     for(int rb_cnt=0;rb_cnt<nRBs;rb_cnt++)
//       schedulerOutput.schedulerToTransmitterInfo(srvrNode_cnt).frameStructure->updateRUType(rb_cnt,mergedRbInfo.mergedRBName(rb_cnt));
//   }
//   else //! Uplink
//   {
//     int srvrNode_cnt=getRxSrvrNodeIndx(schedulerOutput.schedulerToReceiverInfo,srvrNode);
//     txSubcarrierPowersInDBm=0; //Assuming UE won't transmit anything when un-scheduled 
//     ivec vrbs=getIntegers(0,nRBs-1);
//     RBMergeInfo_S mergedRbInfo=generateRBStructure(schedulerOutput.schedulerToReceiverInfo(srvrNode_cnt).frameStructure,rbInfoForUnscheduled.rsInfo.rsPorts,vrbs,false,true,schedulerOutput.schedulerToReceiverInfo(srvrNode_cnt).frameReservationInfo);
//     for(int rb_cnt=0;rb_cnt<nRBs;rb_cnt++)
//     {
//       if(mergedRbInfo.mergedRBName(rb_cnt)=="") mergedRbInfo.mergedRBName(rb_cnt)="nullRB";
//       schedulerOutput.schedulerToReceiverInfo(srvrNode_cnt).frameStructure->updateRUType(rb_cnt,mergedRbInfo.mergedRBName(rb_cnt));
//     }
//   }
//   
// }
// zeroWarning case handled below
// void updateRBInfoForUnscheduled(SchedulerOutput_S &schedulerOutput, ivec serverNodes, int srvrNode_cnt, /*double txSubcarrierPowersInDBm,*/ RBInfo_S &rbInfoForUnscheduled, ivec serviceNodes, bool isDownlink){
//   
//   if(isDownlink)
//     schedulerOutput.setRBInfoForUnScheduledInTransmitter(serverNodes(srvrNode_cnt),rbInfoForUnscheduled); // may vary for each txNodeID based on the cell configuration in the future
//     else
//     {
//       for(int srvcNode_cnt=0;srvcNode_cnt<serviceNodes.length();srvcNode_cnt++)
// 	schedulerOutput.setRBInfoForUnScheduledInTransmitter(serviceNodes(srvcNode_cnt),rbInfoForUnscheduled); // Assuming TxPower is not used in unscheduled UEs in Uplink
//     }
// }




/*! \brief In uplink UEs must be scheduled with contiguous RBs. This function check if a given UE can be scheduled in RBs specified in resourceUnits(rbGroup_cnt)
 *  \note Assumption1: Number of RB-Groups already got scheduled is length(scheduledServiceNodesInfoPerRBGroup)-1.
 * \param[in] nRBGroups --> total number of RB-Groups
 *    */
// bool isServiceNodeEligibleForUplinkScheduling(int serviceNodeID,Array<SchedulerInfo_S> scheduledServiceNodesInfoPerRBGroup, Array<ivec> resourceUnits,int contentingRBGIndx)
// {
//   ivec ScheduledRBs;
//   for(int rbGroup_cnt=0; rbGroup_cnt<scheduledServiceNodesInfoPerRBGroup.length(); rbGroup_cnt++) {
//     if(find(scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).userIDs,serviceNodeID)!=-1)
//       append(ScheduledRBs,resourceUnits(rbGroup_cnt));
//   }
//   if(!ScheduledRBs.length()) {return true;}
//   ivec RBsToBeScheduled = resourceUnits(contentingRBGIndx);
//   
//   for(int rb_cnt=length(ScheduledRBs)-1; rb_cnt>=0; rb_cnt--) 
//     if(abs(ScheduledRBs(rb_cnt)-RBsToBeScheduled(0))==1){
//       return true;
//     
//   }
//   return false;
// }

// ivec getServiceNodeIDsEligibleForUplinkScheduling(ivec serviceNodeIDs, Array< SchedulerInfo_S > scheduledServiceNodesInfoPerRBGroup, Array< ivec > resourceUnits, int contentingRBGIndx)
// {
//   ivec eligibleServiceNodeIDs;
//   for(int iServiceNodeIDs=0; iServiceNodeIDs<serviceNodeIDs.length(); iServiceNodeIDs++)
//     if(isServiceNodeEligibleForUplinkScheduling(serviceNodeIDs(iServiceNodeIDs),scheduledServiceNodesInfoPerRBGroup,resourceUnits,contentingRBGIndx))
//       append(eligibleServiceNodeIDs,serviceNodeIDs(iServiceNodeIDs));
//     return eligibleServiceNodeIDs;
// }

TransmissionScheme_E getTransmissionScheme(TransmissionMode_E transmissionMode)
{
  TransmissionScheme_E transmissionScheme;
  switch(transmissionMode)
  {
    case (_TRANSMISSION_MODE_1_) : {transmissionScheme=_TRANSMISSION_SCHEME_SINGLE_PORT_CRS_; break;}
    case (_TRANSMISSION_MODE_2_) :  {transmissionScheme=_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_; break;}
    case (_TRANSMISSION_MODE_3_) :  {transmissionScheme= _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_; break;}
    case (_TRANSMISSION_MODE_4_)  : {transmissionScheme= _TRANSMISSION_SCHEME_CL_SM_; break;}
    case (_TRANSMISSION_MODE_5_)  : {transmissionScheme= _TRANSMISSION_SCHEME_CL_MU_MIMO_; break;}
    case (_TRANSMISSION_MODE_6_)  :  {transmissionScheme=_TRANSMISSION_SCHEME_CL_BF_ ; break;}
    case (_TRANSMISSION_MODE_7_)  :  {transmissionScheme=_TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_ ; break;}
    case (_TRANSMISSION_MODE_8_)  :  {transmissionScheme=_TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ ; break;}
    case (_TRANSMISSION_MODE_9_)  :  {transmissionScheme=_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ ; break;}
    case (_TRANSMISSION_MODE_NR_)  :  {transmissionScheme=_TRANSMISSION_SCHEME_NR_CL_; break;}
    case (_TRANSMISSION_MODE_10_)  :  {transmissionScheme=_TRANSMISSION_SCHEME_COMP_ ; break;}
    case (_TRANSMISSION_MODE_11_)  :  {transmissionScheme=_TRANSMISSION_SCHEME_OLMUMIMO_ ; break;}
    default : {	cout<<"[both:] undefined TransmissionMode_E in getTransmissionScheme()..."<<endl;abort();}
  }
  return(transmissionScheme);
}

// zeroWarning case handled below
// Array< DataBlkInfo_S > mergeRBGroups(Array<RBGroupInfo_S> &allRbGroupsInfo, ivec serviceNodes, Array<SchedulerInfo_S> &scheduledServiceNodesInfoPerRBGroup,/*CqiToMaxMCSRate_S &cqiToMaxMCSRate,*/cLTETables *lteTables,Codebook &codebook, ServiceNodesInfo_S &serviceNodesInfo,/*int subFrameNumber,*/bool isDownlink,ivec defaultMCSIndices){
// 
//   
//   int serviceNodeIndx;
//   
//   Array< Array<ivec> > cqiIndices(serviceNodes.length());
//   Array< Array<vec> > effSINRs(serviceNodes.length());
//   
//   Array< DataBlkInfo_S > dataBlkInfoPerServiceNode(serviceNodes.length());
//   for(int rbGroup_cnt=0;rbGroup_cnt<scheduledServiceNodesInfoPerRBGroup.length();rbGroup_cnt++)
//     for(int usr_cnt=0;usr_cnt<scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).userIDs.length();usr_cnt++)
//     {
//       serviceNodeIndx=find(serviceNodes,scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).userIDs(usr_cnt));
//       append(dataBlkInfoPerServiceNode(serviceNodeIndx).resourceBlockGroups,rbGroup_cnt);
//       if(dataBlkInfoPerServiceNode(serviceNodeIndx).resourceBlocks.length()==0)
//       {
// 	dataBlkInfoPerServiceNode(serviceNodeIndx).resourceBlocks=allRbGroupsInfo(rbGroup_cnt).resourceBlocks;
// 	dataBlkInfoPerServiceNode(serviceNodeIndx).dataGenInfo=allRbGroupsInfo(rbGroup_cnt).dataGenInfo(usr_cnt);
// 	
// 	if(isDownlink)
// 	{
// 	  cout<<"check for RBGs in the bandwidth ::"<<dataBlkInfoPerServiceNode(serviceNodeIndx).resourceBlockGroups.size()<<endl;
// 	  cqiIndices(serviceNodeIndx).set_length(scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).cqiIndices(usr_cnt).length());
// 	  for(int cwd_cnt=0;cwd_cnt<cqiIndices(serviceNodeIndx).length();cwd_cnt++)
// 	    cqiIndices(serviceNodeIndx)(cwd_cnt)=to_ivec(scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).cqiIndices(usr_cnt)(cwd_cnt));
// 	}
// 	else
// 	{
// 	  effSINRs(serviceNodeIndx).set_length(scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).effSINRs(usr_cnt).length());
// 	  for(int cwd_cnt=0;cwd_cnt<effSINRs(serviceNodeIndx).length();cwd_cnt++)
// 	    effSINRs(serviceNodeIndx)(cwd_cnt)=to_vec(scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).effSINRs(usr_cnt)(cwd_cnt));
// 	}
//       }
//       else
//       {
// 	append(dataBlkInfoPerServiceNode(serviceNodeIndx).resourceBlocks,allRbGroupsInfo(rbGroup_cnt).resourceBlocks);
// 	append(dataBlkInfoPerServiceNode(serviceNodeIndx).dataGenInfo.resourceElementsPerRB,allRbGroupsInfo(rbGroup_cnt).dataGenInfo(usr_cnt).resourceElementsPerRB);
// 	
// 	if(isDownlink)
// 	{
// 	  for(int cwd_cnt=0;cwd_cnt<cqiIndices(serviceNodeIndx).length();cwd_cnt++)
// 	    append(cqiIndices(serviceNodeIndx)(cwd_cnt),scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).cqiIndices(usr_cnt)(cwd_cnt));
// 	}
// 	else
// 	{
// 	  for(int cwd_cnt=0;cwd_cnt<effSINRs(serviceNodeIndx).length();cwd_cnt++)
// 	    append(effSINRs(serviceNodeIndx)(cwd_cnt),scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).effSINRs(usr_cnt)(cwd_cnt));
// 	} 
//       }
//       
//       if(dataBlkInfoPerServiceNode(serviceNodeIndx).dataGenInfo.transmissionScheme==  _TRANSMISSION_SCHEME_CL_SM_||dataBlkInfoPerServiceNode(serviceNodeIndx).dataGenInfo.transmissionScheme== _TRANSMISSION_SCHEME_CL_BF_
// 	||dataBlkInfoPerServiceNode(serviceNodeIndx).dataGenInfo.transmissionScheme== _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_||dataBlkInfoPerServiceNode(serviceNodeIndx).dataGenInfo.transmissionScheme== _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || dataBlkInfoPerServiceNode(serviceNodeIndx).dataGenInfo.transmissionScheme== _TRANSMISSION_SCHEME_NR_CL_)
//       {
// 	dataBlkInfoPerServiceNode(serviceNodeIndx).isClosedLoopScheme=true;
// 		
// 	for(int rb_cnt=0;rb_cnt<allRbGroupsInfo(rbGroup_cnt).resourceBlocks.length();rb_cnt++)
// 	{
// 	  append(dataBlkInfoPerServiceNode(serviceNodeIndx).precoderPerRB,codebook.getPrecoder(dataBlkInfoPerServiceNode(serviceNodeIndx).dataGenInfo.rbInfo.rsInfo.nCQIports,dataBlkInfoPerServiceNode(serviceNodeIndx).dataGenInfo.nLayers,allRbGroupsInfo(rbGroup_cnt).pmiIndexPerUser(usr_cnt)));/// For MU MIMO change is needed
// 	}
// 	
//       }
//       else if(dataBlkInfoPerServiceNode(serviceNodeIndx).dataGenInfo.transmissionScheme== _TRANSMISSION_SCHEME_CL_MU_MIMO_)
//       {
// 	dataBlkInfoPerServiceNode(serviceNodeIndx).isClosedLoopScheme=true;
// 	for(int rb_cnt=0;rb_cnt<allRbGroupsInfo(rbGroup_cnt).resourceBlocks.length();rb_cnt++)
// 	{
// 	  append(dataBlkInfoPerServiceNode(serviceNodeIndx).precoderPerRB,scheduledServiceNodesInfoPerRBGroup(rbGroup_cnt).precoder);
// 	}
//       }
//     }
//     for(int dataBlk_cnt=0;dataBlk_cnt<dataBlkInfoPerServiceNode.length();dataBlk_cnt++)
//       if(dataBlkInfoPerServiceNode(dataBlk_cnt).resourceBlocks.length()!=0)
//       {
// 	ivec cqiIndicesPerCodeWord(cqiIndices(dataBlk_cnt).length());
// 	
// 	if(isDownlink)
// 	{
// 	  for(int Cwd_cnt=0;Cwd_cnt<cqiIndices(dataBlk_cnt).length();Cwd_cnt++)
// 	    cqiIndicesPerCodeWord(Cwd_cnt)=lteTables->findOptimalCQIIndex(cqiIndices(dataBlk_cnt)(Cwd_cnt));
// 	}
// 
// 	vec rateOffset=zeros(cqiIndicesPerCodeWord.length());
// 	if(serviceNodesInfo.outerLoopLinkAdaptationInfo.isEnabled)
// 	{
// 	  rateOffset=serviceNodesInfo.outerLoopLinkAdaptationInfo.getRateOffset(cqiIndicesPerCodeWord,serviceNodes(dataBlk_cnt));
// 	}
// 	
// 	if(defaultMCSIndices.length()!=0)
// 	  dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.mcsIndices=defaultMCSIndices;
// 	else
// 	{
// 	  if(isDownlink)
// 	    dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.mcsIndices=lteTables->findBestMCSIndex(cqiIndicesPerCodeWord,dataBlkInfoPerServiceNode(dataBlk_cnt).resourceBlocks.length(),sum(dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.resourceElementsPerRB),dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.mimoRate,isDownlink,serviceNodesInfo.outerLoopLinkAdaptationInfo.isEnabled,rateOffset);
// 	  else
// 	  {
// 	    Array<ivec> tbSizesPerMCS(effSINRs(dataBlk_cnt).length());
// 	    Array<vec> codeRatesPerMCS(effSINRs(dataBlk_cnt).length());
// 	    for(int Cwd_cnt=0;Cwd_cnt<effSINRs(dataBlk_cnt).length();Cwd_cnt++)
// 	    {
// 	      tbSizesPerMCS(Cwd_cnt).set_length(29);
// 	      codeRatesPerMCS(Cwd_cnt).set_length(29);
// 	    }
// 	    
// 	    for(int mcs_cnt = 0; mcs_cnt < 29; mcs_cnt++)
// 	    {
// 	      int tbIndex = lteTables->transportBlockMCSMapping.transportBlockIndexPUSCH(mcs_cnt);
// 	      for(int Cwd_cnt=0;Cwd_cnt<effSINRs(dataBlk_cnt).length();Cwd_cnt++)
// 	      {
// 		int modulationIndex = lteTables->transportBlockMCSMapping.modulationIndexPUSCH(mcs_cnt);
// 		tbSizesPerMCS(Cwd_cnt)(mcs_cnt) = lteTables->getTBSizeForAllLayersCombined(tbIndex,dataBlkInfoPerServiceNode(dataBlk_cnt).resourceBlocks.length(),getNumLayersPerCodeWord(dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.nCodeWords,dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.mimoRate)(Cwd_cnt));
// 		int nCRC,nBlocks=lteTables->getCodeBlockSizes(tbSizesPerMCS(Cwd_cnt)(mcs_cnt)).length();
// 		if(nBlocks>1)
// 		  nCRC=(nBlocks+1)*24;
// 		else
// 		  nCRC=24;
// 		codeRatesPerMCS(Cwd_cnt)(mcs_cnt) = (tbSizesPerMCS(Cwd_cnt)(mcs_cnt)+nCRC)/(double)(sum(dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.resourceElementsPerRB)*getNumLayersPerCodeWord(dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.nCodeWords,dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.mimoRate)(Cwd_cnt) * modulationIndex);
// 	      }
// 	    }
// 
// 	    dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.mcsIndices.set_length(effSINRs(dataBlk_cnt).length());
// 	    for(int Cwd_cnt=0;Cwd_cnt<effSINRs(dataBlk_cnt).length();Cwd_cnt++)
// 	    {
// 	      vec SINRlinear = inv_dB(effSINRs(dataBlk_cnt)(Cwd_cnt));
// 	      dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.mcsIndices(Cwd_cnt) = lteTables->awgnTablesConvCoder.mcsSelect(SINRlinear,tbSizesPerMCS(Cwd_cnt),codeRatesPerMCS(Cwd_cnt),isDownlink);
// 	    }
// 	  }
// 	}
// 	
// #ifdef USING_CALIBRATION
// 	/// following lines are added for CQI module testing
// 	ivec mcsIndicesForCQITesting;
// 	if(traceValuePerTraceName["RITesting"]=="true")
// 	{
// 	  mcsIndicesForCQITesting=CQIToMCSForRITesting(getAcrossArray(cqiIndices(dataBlk_cnt),0));
// 	  dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.mcsIndices=mcsIndicesForCQITesting;
// 	}
// 	
// 	if(traceValuePerTraceName["PUSCH_3_0caseBsubband"]=="true")
//         {
// 	  mcsIndicesForCQITesting=CQIToMCSForCQITesting(getAcrossArray(cqiIndices(dataBlk_cnt),0));
// 	  dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.mcsIndices=mcsIndicesForCQITesting;
// 	}
//   //! For testing traces
//   ivec temp(0);
//   std::istringstream ss(traceValuePerTraceName["mcsValuesPerSubframePerCodeword"]);
//   ss >>temp;
//   if(traceValuePerTraceName["pdschDemodPerfTesting"]=="true" || traceValuePerTraceName["PUSCH_3_0caseBwideband"]=="true")
//           {
//             if( (subFrameNumber%10) ==0)
//               dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.mcsIndices = temp(0,1);
//             else if( (subFrameNumber%10) ==5)
// 	      dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.mcsIndices = temp(2,3);
//             else
// 	      dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.mcsIndices = temp(4,5);
//            }
// #endif	
// 
// 	sort(dataBlkInfoPerServiceNode(dataBlk_cnt).resourceBlocks);
// 	sort(dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.rbInfo.rsInfo.CQIports);
// 	sort(dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.rbInfo.rsInfo.DeModPorts);
// 	sort(dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.rbInfo.rsInfo.rsPorts);
// 	dataBlkInfoPerServiceNode(dataBlk_cnt).dataGenInfo.scheduledPRBs=dataBlkInfoPerServiceNode(dataBlk_cnt).resourceBlocks.length();
//       }
//       
//       return(dataBlkInfoPerServiceNode);
// }

vec getTotalMCSRate(Array<vec> mcsRatesPerCodeword, Array<TransmissionConfig_S> &transmissionConfig,ivec txConfigIndices){
  
  /// For MIMO mode adaptation transmissionScheme is to be changed
  vec totalMCSRate=zeros(mcsRatesPerCodeword.length());
  if(txConfigIndices.length()==0) txConfigIndices=getIntegers(0,transmissionConfig.length()-1);
  for(int cnt=0;cnt<totalMCSRate.length();cnt++)
  {
    if(transmissionConfig(txConfigIndices(cnt)).transmissionMode!=_TRANSMISSION_MODE_2_)
      for(int cwd_cnt=0;cwd_cnt<mcsRatesPerCodeword(cnt).length();cwd_cnt++)
      {
	if(cwd_cnt==0)
	  totalMCSRate(cnt)=mcsRatesPerCodeword(cnt)(cwd_cnt)*getNumLayersPerCodeWord(mcsRatesPerCodeword(cnt).length(),transmissionConfig(txConfigIndices(cnt)).nLayers)(cwd_cnt);
	else
	  totalMCSRate(cnt)+=mcsRatesPerCodeword(cnt)(cwd_cnt)*getNumLayersPerCodeWord(mcsRatesPerCodeword(cnt).length(),transmissionConfig(txConfigIndices(cnt)).nLayers)(cwd_cnt);
      }
      else
	totalMCSRate(cnt)=mcsRatesPerCodeword(cnt)(0);
      
  }
  
  return(totalMCSRate);
}

RBMergeInfo_S generateRBStructure(FrameStructure *frameStructure, RSInfo_S &rsInfo,ivec vrbs,bvec useCurrentRBAsBase, bool isDownlink,bool isLocalised,FrameReservationInfo_S *cchReservation)
{ RBMergeInfo_S rbMergeInfo;
  string mergedRbName="";
  string mergedRbName2="";
  ivec rsPorts=rsInfo.rsPorts;
  Array<ivec> rsSymPositions=rsInfo.rsSymPosition;
  ivec rsScPositions=rsInfo.rsScPosition;
  
  Array<string> rbName(rsPorts.length());
  for(int port_cnt=0;port_cnt<rsPorts.length();port_cnt++)
  {
    mergedRbName+=toString(rsPorts(port_cnt));
    for(int sym_cnt=0;sym_cnt<rsSymPositions(port_cnt).length();sym_cnt++)
      mergedRbName+=("sp"+to_string(rsSymPositions(port_cnt)(sym_cnt)));
  }
  for(int port_cnt=0;port_cnt<rsPorts.length();port_cnt++)
  {
    if(find(rsInfo.PtrsPorts,rsPorts(port_cnt))==-1)
    {
      mergedRbName2+=toString(rsPorts(port_cnt));
      for(int sym_cnt=0;sym_cnt<rsSymPositions(port_cnt).length();sym_cnt++)
	mergedRbName2+=("sp"+to_string(rsSymPositions(port_cnt)(sym_cnt)));
    }
  }
   if(isDownlink)
  {
    if(cchReservation->cchInfo.pdcchCount>0)
    {
      mergedRbName+="pdcch"+toString(cchReservation->cchInfo.pdcchCount);
      mergedRbName2+="pdcch"+toString(cchReservation->cchInfo.pdcchCount);
  
    }
  }
  
  rbMergeInfo.rbs=vrbs;
  rbMergeInfo.mergedRBName.set_length(vrbs.length());
  int ptrsindex=-1;
  for(int port_cnt=0;port_cnt<rsPorts.length();port_cnt++)  
  {
    if((find(rsInfo.PtrsPorts,rsPorts(port_cnt)))!=-1)
    {ptrsindex=port_cnt;
    }
  }
  int flag;
  for(int rb_cnt=0;rb_cnt<vrbs.length();rb_cnt++)
  { string localString;
    if(ptrsindex!=-1 && (find(rsInfo.rsrbpos(ptrsindex),vrbs(rb_cnt))!=-1) )
    {
      localString=mergedRbName;
	flag=0;     
    }
    else
    {
      localString=mergedRbName2;
      flag=1;
    }
int pbch=0,pss=0,shortPUCCH=0;
    bool srsPresent=false;
    bool epdcch_rb=false;
    ivec prbs;prbs.set_length(0);
    
    if(isDownlink)
    {
      prbs=cchReservation->dlPrbMapping(vrbs(rb_cnt)%cchReservation->dlPrbMapping.length());
//       if(cchReservation->cchInfo.enablePBCH)
//       {
// 	int prb;
// 	if(isLocalised)
// 	  prb=vrbs(rb_cnt);
// 	else
// 	  prb=prbs(1); // considering only the second slot for prbs
// 	if(find(cchReservation->cchtonesPerRB(prb),54)!=-1)
// 	{localString+="pbch";pbch=54;}
// 	else if(find(cchReservation->cchtonesPerRB(prb),55)!=-1)
// 	{localString+="pbchUp";pbch=55;}
// 	else if(find(cchReservation->cchtonesPerRB(prb),56)!=-1)
// 	{localString+="pbchDown";pbch=56;}
//     }
	
      /*if(cchReservation->cchInfo.enablePssSss)
      {
	int prb;
	if(isLocalised)
	  prb=vrbs(rb_cnt);
	else
	  prb=prbs(0); // considering only the first slot for prbs
	if(find(cchReservation->cchtonesPerRB(prb),57)!=-1)
	{localString+="pssSss";pss=57;}
	else if(find(cchReservation->cchtonesPerRB(prb),58)!=-1)
	{localString+="pssSssUp";pss=58;}
	else if(find(cchReservation->cchtonesPerRB(prb),59)!=-1)
	{localString+="pssSssDown";pss=59;}
      }
      */
      if(isLocalised)
      {
	if(find(cchReservation->cchtonesPerRB(vrbs(rb_cnt)%cchReservation->dlPrbMapping.length()),100)!=-1)
	{
	  epdcch_rb=true;
	  localString+="epdcch";
	}
	
      }
    }
    else //!Uplink
    {
      if(find(cchReservation->cchtonesPerRB(vrbs(rb_cnt)),60)!=-1)
 	{localString+="shortPUCCH";shortPUCCH=60;}
      //!SRS Reservation 
      #ifdef USING_SRS          
      if(cchReservation!=NULL)
	if(cchReservation->cchtonesPerRB.length())
	{
	  int prb;
	  prb=vrbs(rb_cnt); 
	  if(find(cchReservation->cchtonesPerRB(prb),60)!=-1)
	    {localString+="srs";srsPresent=true;}
	}
      #endif
    }

    for(int sym_cnt=0;sym_cnt<cchReservation->reservedSymbols.length();sym_cnt++)
        localString+="b"+toString(cchReservation->reservedSymbols(sym_cnt));

    
    if(localString=="")
      localString="nullRB";
    
    ivec reservedSymbols=cchReservation->reservedSymbols;
    int len1=reservedSymbols.length();
    Array<ivec> reservedSCPerSym(reservedSymbols.length());
    for(int cnt=0;cnt<reservedSymbols.length();cnt++)
        reservedSCPerSym(cnt).set_length(0);
    
    if(isDownlink && cchReservation->cchInfo.enableSSB && cchReservation->cchInfo.isSSBPresent)
    {
        int prb;
        if(isLocalised)
            prb=vrbs(rb_cnt);
        else
            prb=prbs(0); // considering only the first slot for prbs
        
        int prbStart=prb*12;
        int prbEnd=prb*12+11;
        int ssbStart=floor_i(cchReservation->cchInfo.SSBOffsetFromPRB0/12.0)*12;//cchReservation->cchInfo.SSBOffsetFromPRB0;
        int ssbEnd=ceil_i((cchReservation->cchInfo.SSBOffsetFromPRB0+cchReservation->cchInfo.SSBSubcarrierCount)/12.0)*12-1;//cchReservation->cchInfo.SSBOffsetFromPRB0+cchReservation->cchInfo.SSBSubcarrierCount-1;

        ivec blockedSymbols=cchReservation->cchInfo.SSBSymbols(findNot(cchReservation->cchInfo.SSBSymbols,cchReservation->reservedSymbols));  
        
        if((ssbStart<=prbStart && prbStart<=ssbEnd) || (ssbStart<=prbEnd && prbEnd<=ssbEnd)) // PRB is within SSB region
        {
            for(int sym_cnt=0;sym_cnt<blockedSymbols.length();sym_cnt++)
                localString+="b"+toString(blockedSymbols(sym_cnt));
            
            append(reservedSymbols,blockedSymbols);
            if(ssbStart<=prbStart && prbStart<=ssbEnd && ssbStart<=prbEnd && prbEnd<=ssbEnd) // Full PRB is within region
            {
                for(int cnt=0;cnt<blockedSymbols.length();cnt++)
                   append(reservedSCPerSym,ivec(""));
            }
            else if(ssbStart<=prbStart && prbStart<=ssbEnd) // End PRB
            { 
              for(int cnt=0;cnt<blockedSymbols.length();cnt++)
              {
                    append(reservedSCPerSym,zeros_i(2));
                    reservedSCPerSym(cnt+len1)(0)=0;
                    reservedSCPerSym(cnt+len1)(1)=ssbEnd-floor(ssbEnd/12)*12;
                    
                    localString+="bsc0_"+toString(reservedSCPerSym(cnt+len1)(1));
                    
              }
            }
            else // Start PRB
            {
                for(int cnt=0;cnt<blockedSymbols.length();cnt++)
                {
                    append(reservedSCPerSym,zeros_i(2));
                    reservedSCPerSym(cnt+len1)(0)=ssbStart-floor(ssbStart/12)*12;
                    reservedSCPerSym(cnt+len1)(1)=11;
              
                    localString+="bsc"+toString(reservedSCPerSym(cnt+len1)(0))+"_11";
                }
            }
        }
    }
    
    if(useCurrentRBAsBase(rb_cnt))
      localString=frameStructure->getResourceUnitType(vrbs(rb_cnt))+localString;
    
    rbMergeInfo.mergedRBName(rb_cnt)=localString;
    if(!frameStructure->isResourceUnitAvailable(localString))
    {
      if(rsPorts.length())
      {
	
	for(int port_cnt=0;port_cnt<rsPorts.length();port_cnt++)
	{ if(flag==0)
	  {rbName.set_length(rsPorts.length());
	  rbName(port_cnt)="port"+toString(rsPorts(port_cnt));
	  }
	  else if(flag==1)
	  {rbName.set_length(rsPorts.length()-rsInfo.PtrsPorts.length());
	    if(find(rsInfo.PtrsPorts,rsPorts(port_cnt))==-1)
	    rbName(port_cnt)="port"+toString(rsPorts(port_cnt));
	  }
	  }
	}
      else
      {
	rbName.set_length(1);
	rbName(0)="nullRB";
      }
      if(isDownlink)
      {
	if(cchReservation->cchInfo.pdcchCount>0)
	  append(rbName,"pdcch"+toString(cchReservation->cchInfo.pdcchCount));
	
// 	if(pbch!=0)
// 	{
// 	  if(pbch==54)	  	{string temp="pbch";append(rbName,temp);}
// 	  else if(pbch==55)	{string temp="pbchUp";append(rbName,temp);}
// 	  else if(pbch==56)	{string temp="pbchDown";append(rbName,temp);}
// 	}
// 	if(pss!=0)
// 	{
// 	  if(pss==57)	  	{string temp="pssSss";append(rbName,temp);}
// 	  else if(pss==58)	{string temp="pssSssUp";append(rbName,temp);}
// 	  else if(pss==59)	{string temp="pssSssDown";append(rbName,temp);}
// 	}
      }
     else
      {
	if(srsPresent)
	{
	  string temp="srs";
	  append(rbName,temp);
	}
		  if(shortPUCCH==60)	  	{string temp="shortPUCCH";append(rbName,temp);}
      }
      //for epdcch
      if(epdcch_rb)
      {	
	string epdcch1;
	if(cchReservation->cchInfo.pdcchCount)
	 epdcch1="epdcchdata_wpdcch";
	else
	  epdcch1="epdcchdata_wopdcch";
	append(rbName,epdcch1);
      }     
      
      Array<ivec> rsPos=rsSymPositions;
      if(rbName.length()>rsSymPositions.length())
       {
	 Array<ivec> emptySet(rbName.length()-rsSymPositions.length());
	 append(rsPos,emptySet);
       }
       if(useCurrentRBAsBase(rb_cnt))
	 frameStructure->mergeResourceUnits(frameStructure->getResourceUnitType(vrbs(rb_cnt)),rbName, rsPos,rsScPositions,rbMergeInfo.mergedRBName(rb_cnt),"data",reservedSymbols,reservedSCPerSym); 
       else
	   frameStructure->mergeResourceUnits("nullRB",rbName, rsPos,rsScPositions,rbMergeInfo.mergedRBName(rb_cnt),"data",reservedSymbols,reservedSCPerSym); 
    }
  }
  return(rbMergeInfo);
}

//Code commented
/*
SRSconfig_S getSRSBW(AssociationInfo_S associationInfo, int SRSbandwidthinRBs, int SRScomb, int SRSsoundingbandwidthinRBs, int SRSsoundingbandwidthFirstRB, int srvc_cnt, int SRSCount)
{ 
  int slotsforcompletehopping=SRSsoundingbandwidthinRBs/(SRSbandwidthinRBs*SRSCount);
    SRSconfig_S srsConfig;
  //int AssociatedServiceNodes=associationInfo.getMyAssociatedServiceNodes(srvr_cnt);
    srsConfig.srsstartRB.set_length(slotsforcompletehopping);
    srsConfig.srscomboffset=srvc_cnt%SRScomb;
    srsConfig.srsnRB=SRSbandwidthinRBs;
    srsConfig.symbolnoinsrsresource=ivec("12 13");
    for(int loop=0;loop<slotsforcompletehopping;loop++)
    { srsConfig.srsstartRB(loop).set_length(SRSCount);
      for(int loop2=0;loop2<SRSCount;loop2++)
      {
	srsConfig.srsstartRB(loop)(loop2)=SRSsoundingbandwidthFirstRB+(SRSbandwidthinRBs*(srvc_cnt/SRScomb))+(loop2*SRSbandwidthinRBs)+(loop*SRSbandwidthinRBs*SRSCount);
//	if((srsConfig.srsstartRB(loop)(loop2))>=SRSsoundingbandwidthinRBs)
//	srsConfig.srsstartRB(loop)(loop2)=(srsConfig.srsstartRB(loop)(loop2))%SRSsoundingbandwidthinRBs;
       }
     }
    
    return srsConfig;
}

*/


void loadAllResourceBlockStructures(FrameStructure &frameStructure, string rbPatternPath, int cellID ,ivec rsPorts,bool isDownlink, int CSIRSconfig)
{
  string rbFileName;
  Array<string> rbName;
  rbName.set_size(rsPorts.length());
  string rbPath;
  ivec cellSpecificSubcarrierUpShift(rsPorts.length()), cellSpecificSymbolRightShift(rsPorts.length());
  ivec CSIRSPorts = getCSIRSPortIndices(rsPorts);// extracted CSIRS ports from rsPorts
  ivec NRCSIRSPorts = getNRCSIRSPortIndices(rsPorts);// extracted CSIRS ports from rsPorts
  int CSIRSConfigSpecificSubcarrierUpShift;
  int CSIRSConfigSpecificSymbolRightShift;
  rbPath=rbPatternPath;
  if(isDownlink)
    getRSportShifts(cellSpecificSubcarrierUpShift, cellSpecificSymbolRightShift, rsPorts, cellID, CSIRSconfig);
  ivec CSIRSsymbolsToswap="", tempSym(2);
  int totalCSIRSConfigurations = 0;
  
  for(int port_cnt=0;port_cnt<rsPorts.length();port_cnt++)
  {
    rbFileName=rbPath+"Port"+toString(rsPorts(port_cnt))+".txt";
    rbName(port_cnt)="port"+toString(rsPorts(port_cnt));
    if(isDownlink)
    {
//       if(rsPorts((port_cnt) >= 3000 && rsPorts(port_cnt) <= 3032) || rsPorts(port_cnt) == 3100)
// 	frameStructure.loadResourceUnit(rbName(port_cnt),rbFileName,cellSpecificSubcarrierUpShift(port_cnt), cellSpecificSymbolRightShift(port_cnt), CSIRSsymbolsToswap);
// 	else
      frameStructure.loadResourceUnit(rbName(port_cnt),rbFileName,cellSpecificSubcarrierUpShift(port_cnt), cellSpecificSymbolRightShift(port_cnt), CSIRSsymbolsToswap);
       //...........
      if((rsPorts(port_cnt) > 14 && rsPorts(port_cnt) < 23) || (rsPorts(port_cnt) >=3000 && rsPorts(port_cnt)<=3100) ){
	
	//int nCSIRSPorts = CSIRSPorts.length();
	int nCSIRSPorts=0 ;
	// Below switch supports only FDD (Frame Type1)
	// modification needed for TDD considerations
	if((rsPorts(port_cnt) >=15) & (rsPorts(port_cnt)<= 16)){totalCSIRSConfigurations = 20; nCSIRSPorts = 2;}
	else if((rsPorts(port_cnt) >=17) & (rsPorts(port_cnt)<= 18)){totalCSIRSConfigurations = 10;nCSIRSPorts =4;}
	else if((rsPorts(port_cnt) >=19) & (rsPorts(port_cnt)<= 22)){totalCSIRSConfigurations = 5;nCSIRSPorts =8;}
	/*else if(rsPorts(port_cnt) ==3000 ){totalCSIRSConfigurations = 12;nNRCSIRSPorts =1;}
	else if(rsPorts(port_cnt) ==3100 ){totalCSIRSConfigurations = 4;nNRCSIRSPorts =1;}
	*/else{
	  //do nothing 
	}
	for(int csiConfig=0; csiConfig < totalCSIRSConfigurations; csiConfig++)
	{
	  getCSIRSportShifts(CSIRSConfigSpecificSubcarrierUpShift, CSIRSConfigSpecificSymbolRightShift, nCSIRSPorts, csiConfig );
	  if(CSIRSconfig>=20 && CSIRSconfig<=31)
	  {
	    tempSym(0)=CSIRSConfigSpecificSymbolRightShift;
	    tempSym(1)=CSIRSConfigSpecificSymbolRightShift+2;
	    CSIRSsymbolsToswap=tempSym;
	  }
	  int toneNumberOffset = ((rsPorts(port_cnt)+(rsPorts(port_cnt)%2))*100+csiConfig-(rsPorts(port_cnt)+(rsPorts(port_cnt)%2)));
	  string newRuType = rbName(port_cnt)+toString(csiConfig/10)+toString(csiConfig%10);
	  //frameStructure.modifyResourceUnit(rbName(port_cnt),newRuType,"data",toneNumberOffset,toString(csiConfig),CSIRSConfigSpecificSubcarrierUpShift, CSIRSConfigSpecificSymbolRightShift, CSIRSsymbolsToswap);
	  frameStructure.modifyResourceUnit(rbName(port_cnt),newRuType,"data",toneNumberOffset,toString(csiConfig/10)+toString(csiConfig%10),CSIRSConfigSpecificSubcarrierUpShift, CSIRSConfigSpecificSymbolRightShift, CSIRSsymbolsToswap);

	}
	//...................
	
      }
    }
    else
      frameStructure.loadResourceUnit(rbName(port_cnt),rbFileName);
   
    
  }
  
  string nullRBName="nullRB";
  if(!frameStructure.isResourceUnitAvailable(nullRBName))
  {
    rbFileName=rbPatternPath+"nullRB.txt";
    frameStructure.loadResourceUnit(nullRBName,rbFileName);
  }

}


void loadNRCSIRSPatterns(FrameStructure &frameStructure,string csirsPatternPath){
  
  string fileName;
  string ruType;
  ivec csiPatterns=getIntegers(1,4);
  for(int pattern_cnt=0;pattern_cnt<csiPatterns.length();pattern_cnt++)
  {
      ruType="csipattern"+toString(csiPatterns(pattern_cnt));
    fileName=csirsPatternPath+"CSIPattern"+toString(csiPatterns(pattern_cnt))+".txt";
    frameStructure.loadResourceUnit(ruType,fileName);
  }
}


void loadNRSRSPatterns(FrameStructure &frameStructure,string srsPatternPath){
    
    string fileName;
    string ruType;
    ivec srsPatterns=getIntegers(1,2);
    for(int pattern_cnt=0;pattern_cnt<srsPatterns.length();pattern_cnt++)
    {
        ruType="srspattern"+toString(srsPatterns(pattern_cnt));
        fileName=srsPatternPath+"SRSPattern"+toString(srsPatterns(pattern_cnt))+".txt";
        frameStructure.loadResourceUnit(ruType,fileName);
    }
}

void loadAllCCHPatterns(FrameStructure &frameStructure,string cchPatternPath, int cellID){
  
  string fileName;
  string ruType;
  ivec crsPorts=to_ivec(0);
  ivec cellSpecificSubcarrierUpShift(crsPorts.length()), cellSpecificSymbolRightShift(crsPorts.length());
  getRSportShifts(cellSpecificSubcarrierUpShift, cellSpecificSymbolRightShift, crsPorts, cellID);
  for(int cch_cnt=1;cch_cnt<=4;cch_cnt++)
  {
    ruType="pdcch"+toString(cch_cnt);
    fileName=cchPatternPath+ruType+".txt";
    frameStructure.loadResourceUnit(ruType,fileName,cellSpecificSubcarrierUpShift(0),cellSpecificSymbolRightShift(0));
  }
  //for epdcch
  fileName=cchPatternPath+"epdcch_data_wpdcch.txt";
  ruType="epdcchdata_wpdcch";
  frameStructure.loadResourceUnit(ruType,fileName);
  fileName=cchPatternPath+"epdcch_data_wopdcch.txt";
  ruType="epdcchdata_wopdcch";
  frameStructure.loadResourceUnit(ruType,fileName);
}

void loadPBCHPatterns(FrameStructure &frameStructure,string cchPatternPath, int cellID){
  
  string fileName;
  string ruType;
  ivec crsPorts=to_ivec(0);
  ivec cellSpecificSubcarrierUpShift(crsPorts.length()), cellSpecificSymbolRightShift(crsPorts.length());
  getRSportShifts(cellSpecificSubcarrierUpShift, cellSpecificSymbolRightShift, crsPorts, cellID);
  ruType="pbch";
  fileName=cchPatternPath+ruType+".txt";
  frameStructure.loadResourceUnit(ruType,fileName,cellSpecificSubcarrierUpShift(0),cellSpecificSymbolRightShift(0));
  
  ivec rowsToSwapForpbchUp(4);
  rowsToSwapForpbchUp(0)=8;rowsToSwapForpbchUp(1)=8-cellSpecificSubcarrierUpShift(0)%3;
  rowsToSwapForpbchUp(2)=11;rowsToSwapForpbchUp(3)=11-cellSpecificSubcarrierUpShift(0)%3;
  
  ruType="pbchUp";
  fileName=cchPatternPath+ruType+".txt";
  frameStructure.loadResourceUnit(ruType,fileName,0,0,"",rowsToSwapForpbchUp,true);
  
  
  ivec rowsToSwapForpbchDown(4);
  rowsToSwapForpbchDown(0)=2;rowsToSwapForpbchDown(1)=2-cellSpecificSubcarrierUpShift(0)%3;
  rowsToSwapForpbchDown(2)=5;rowsToSwapForpbchDown(3)=5-cellSpecificSubcarrierUpShift(0)%3;
  ruType="pbchDown";
  fileName=cchPatternPath+ruType+".txt";
  frameStructure.loadResourceUnit(ruType,fileName,0,0,"",rowsToSwapForpbchDown,true);
  
}

void loadShortPUCCHpattern(FrameStructure &frameStructure,string cchPatternPath){
  
  string fileName;
  string ruType;
  
  ruType="shortPUCCH";
  fileName=cchPatternPath+ruType+".txt";
  frameStructure.loadResourceUnit(ruType,fileName);
 }

void loadPssSssPatterns(FrameStructure &frameStructure,string cchPatternPath){
  
  string fileName;
  string ruType;
  
  ruType="pssSss";
  fileName=cchPatternPath+ruType+".txt";
  frameStructure.loadResourceUnit(ruType,fileName);
  
  ruType="pssSssUp";
  fileName=cchPatternPath+ruType+".txt";
  frameStructure.loadResourceUnit(ruType,fileName);
  
  ruType="pssSssDown";
  fileName=cchPatternPath+ruType+".txt";
  frameStructure.loadResourceUnit(ruType,fileName);
  
 }

void loadSSBPatterns(FrameStructure &frameStructure,string cchPatternPath){

  string fileName;
  string ruType;
  
  ruType="SSBlock0";
  fileName=cchPatternPath+ruType+".txt";
  frameStructure.loadResourceUnit(ruType,fileName);
  
  ruType="SSBlock1";
  fileName=cchPatternPath+ruType+".txt";
  frameStructure.loadResourceUnit(ruType,fileName);
  
  ruType="SSBlock2";
  fileName=cchPatternPath+ruType+".txt";
  frameStructure.loadResourceUnit(ruType,fileName);
  
  ruType="SSBlock3";
  fileName=cchPatternPath+ruType+".txt";
  frameStructure.loadResourceUnit(ruType,fileName);
  
}


// void generateData(Array< SchedulerOutput_S >& schedulerOutput, Array< ServiceNodesInfo_S* > &serviceNodesInfoPerCU, int subframeNumber, bool isDownlink, bool isWithAbstraction, bool updateFrameStructure,Array< Array< Array<RSInfo_S> > > &rsInfoForUnscheduledPerServerNode){
// 
//   cout<<endl<<"Error... generateData() function is no longer used, please use generateDataNew function."<<endl;abort();
// }


int getTxSrvrNodeIndx(Array<SchedulerToTransmitterInfo_S> schedulerToTransmitterInfo ,int srvrNode)
{
  for(int srvrIndx=0;srvrIndx<schedulerToTransmitterInfo.length();srvrIndx++)
    if(srvrNode==schedulerToTransmitterInfo(srvrIndx).txNodeID)
      return(srvrIndx);
  return -1;
}

int getRxSrvrNodeIndx(Array<SchedulerToReceiverInfo_S> schedulerToReceiverInfo ,int srvrNode)
{
  for(int srvrIndx=0;srvrIndx<schedulerToReceiverInfo.length();srvrIndx++)
    if(srvrNode==schedulerToReceiverInfo(srvrIndx).rxNodeID)
      return(srvrIndx);
    
  return -1;
}


//====================================================================================================
// Code to apply Downlink Power allocation (3gpp 36.213 - Section 5.2) difference between
// OFDM symbols with and without CRS
// Function generates the Power Mask to be applied on 
//====================================================================================================
void generateDLPowerMask(Array< DataBlkInfo_S >& dataBlkInfoPerServiceNode, FrameStructure& frameStructure, cTBTables* tbTables, double txSubcarrierPowerInDBmPerAntenna, double Pb)
{   
    cmat powMask;
    double RhoBrhoAratio,RhoA,RhoB;
    ivec RhoAantLoc, RhoBantLoc;    
    imat logicalSubFrameStructure = dataBlkInfoPerServiceNode(0).dataGenInfo.rbInfo.logicalSubFrameStructure;
    powMask.set_size(logicalSubFrameStructure.rows(),logicalSubFrameStructure.cols());
    powMask.zeros();
    double powInPowMask=0.0;
    int nPilots=0;int ndataCRS =0; int ndataNonCRS=0;
    double PowPerSubCarrInWatt = inv_dB(txSubcarrierPowerInDBmPerAntenna-30);


   for(int index = 0 ; index  < dataBlkInfoPerServiceNode(0).dataGenInfo.rbInfo.rsInfo.rsPorts.length();index++ )
   {
       ivec rsPos = find(logicalSubFrameStructure,dataBlkInfoPerServiceNode(0).dataGenInfo.rbInfo.rsInfo.rsPorts(index)+1,true);       
       for(int rsPos_cnt = 0 ; rsPos_cnt<rsPos.length();rsPos_cnt++)
       {
          powMask(rsPos(rsPos_cnt))=1.0; nPilots++; powInPowMask +=PowPerSubCarrInWatt;
       }
   }

   ivec TxAntenna_Cnt = dataBlkInfoPerServiceNode(0).dataGenInfo.rbInfo.rsInfo.rsPorts;
   powInPowMask = powInPowMask/TxAntenna_Cnt.length();

   for(int serviceNode_cnt =0; serviceNode_cnt<dataBlkInfoPerServiceNode.length();serviceNode_cnt++)
    {      
       ivec resourceBlock = dataBlkInfoPerServiceNode(serviceNode_cnt).resourceBlocks;
       
       double Pa =dataBlkInfoPerServiceNode(serviceNode_cnt).pa;
	
	
       int nSymbolsPerRB = frameStructure.getResourceUnitInfo("nullRB").resourceUnitSize.numberOfSymbols;
       int numOfSubCarrriersPerRB = frameStructure.getResourceUnitInfo("nullRB").resourceUnitSize.numberOfSubcarriers;              
       ivec TxAntennaCnt = dataBlkInfoPerServiceNode(serviceNode_cnt).dataGenInfo.rbInfo.rsInfo.rsPorts;

        if(TxAntennaCnt.length()==1)
        {
            RhoBrhoAratio = tbTables->dlVarPowMask.Tx1Ant(Pb);
            RhoAantLoc =tbTables->dlVarPowMask.Rho_A_1_2Ant;
            RhoBantLoc =tbTables->dlVarPowMask.Rho_B_1_2Ant;
        }
        else if(TxAntennaCnt.length()==2)
        {
            RhoBrhoAratio = tbTables->dlVarPowMask.Tx_2_4Ant(Pb);
            RhoAantLoc =tbTables->dlVarPowMask.Rho_A_1_2Ant;
            RhoBantLoc =tbTables->dlVarPowMask.Rho_B_1_2Ant;
        }
        else if(TxAntennaCnt.length()==4)
        {
            RhoBrhoAratio = tbTables->dlVarPowMask.Tx_2_4Ant(Pb);
            RhoAantLoc =tbTables->dlVarPowMask.Rho_A_4Ant;
            RhoBantLoc =tbTables->dlVarPowMask.Rho_B_4Ant;
        }
        else
        {
            cout<<"[both:]Error. Downlink Power Allocation not defined for Tx ports other than 1 or 2 or 4"<<endl;
            abort();
        }
        
        vec dlpoweroffset="0,-3";
	RhoA=inv_dB(Pa+dlpoweroffset((int)dataBlkInfoPerServiceNode(serviceNode_cnt).downlinkPowerOffset)); // Add Pa  here
	RhoB=RhoA*RhoBrhoAratio;
	
        for(int rbcnt = 0; rbcnt < dataBlkInfoPerServiceNode(serviceNode_cnt).resourceBlocks.length();rbcnt++)
       {
            int rbNum = dataBlkInfoPerServiceNode(serviceNode_cnt).resourceBlocks(rbcnt);
            for(int nSymPerRB = 0 ; nSymPerRB < nSymbolsPerRB ; nSymPerRB++)
            {
                if( find(RhoAantLoc,(nSymPerRB%7)) != -1 )
                    for(int nSubCarr= 0; nSubCarr<numOfSubCarrriersPerRB;nSubCarr++)
                    {
                        if(logicalSubFrameStructure(rbNum*numOfSubCarrriersPerRB+nSubCarr,nSymPerRB)==0)
                        {
                            powMask(rbNum*numOfSubCarrriersPerRB+nSubCarr,nSymPerRB)=RhoA;
                            ndataNonCRS++; powInPowMask += (RhoA*PowPerSubCarrInWatt);
                        }
                    }
                else if( find(RhoBantLoc,(nSymPerRB%7)) != -1 )
                    for(int nSubCarr= 0; nSubCarr<numOfSubCarrriersPerRB;nSubCarr++)
                    {
                        if(logicalSubFrameStructure(rbNum*numOfSubCarrriersPerRB+nSubCarr,nSymPerRB)==0)
                        {
                            powMask(rbNum*numOfSubCarrriersPerRB+nSubCarr,nSymPerRB)=RhoB;
                            ndataCRS++; powInPowMask +=(RhoB*PowPerSubCarrInWatt);
                        }
                    }
            }
     }
   }
   double powAdjust = PowPerSubCarrInWatt*(double)((nPilots/TxAntenna_Cnt.length())+ndataNonCRS+ndataCRS)/(double)powInPowMask;
   powMask = complex<double>(powAdjust,0) * powMask;
   for(int serviceNode_cnt =0; serviceNode_cnt<dataBlkInfoPerServiceNode.length();serviceNode_cnt++)
   {
       dataBlkInfoPerServiceNode(serviceNode_cnt).dataGenInfo.rbInfo.crsPowBoostInDB = dB(powAdjust);
       int nSymbolsPerRB = frameStructure.getResourceUnitInfo("nullRB").resourceUnitSize.numberOfSymbols;
       int numOfSubCarrriersPerRB = frameStructure.getResourceUnitInfo("nullRB").resourceUnitSize.numberOfSubcarriers;
       dataBlkInfoPerServiceNode(serviceNode_cnt).dataGenInfo.rbInfo.dlPowMask.set_size(dataBlkInfoPerServiceNode(serviceNode_cnt).resourceBlocks.length());
       for(int rbcnt = 0; rbcnt < dataBlkInfoPerServiceNode(serviceNode_cnt).resourceBlocks.length();rbcnt++)
       {
           dataBlkInfoPerServiceNode(serviceNode_cnt).dataGenInfo.rbInfo.dlPowMask(rbcnt).set_size(numOfSubCarrriersPerRB,nSymbolsPerRB);
           dataBlkInfoPerServiceNode(serviceNode_cnt).dataGenInfo.rbInfo.dlPowMask(rbcnt).zeros();
           int rbNum = dataBlkInfoPerServiceNode(serviceNode_cnt).resourceBlocks(rbcnt);
           for(int nSymPerRB = 0 ; nSymPerRB < nSymbolsPerRB ; nSymPerRB++)
           {
               for(int nSubCarr= 0; nSubCarr<numOfSubCarrriersPerRB;nSubCarr++)
               {
                  int REmask = logicalSubFrameStructure(rbNum*numOfSubCarrriersPerRB+nSubCarr,nSymPerRB);
                  if(REmask==0||REmask==1 || REmask==2 || REmask==3 || REmask==4 )
                      dataBlkInfoPerServiceNode(serviceNode_cnt).dataGenInfo.rbInfo.dlPowMask(rbcnt)(nSubCarr,nSymPerRB)=powMask(rbNum*numOfSubCarrriersPerRB+nSubCarr,nSymPerRB);
                  else if( (REmask>=50 && REmask<60) || REmask==-1)		  
		      dataBlkInfoPerServiceNode(serviceNode_cnt).dataGenInfo.rbInfo.dlPowMask(rbcnt)(nSubCarr,nSymPerRB)=1.0;
                }
           }
           
       }
       
   }
}




#ifdef USING_CALIBRATION
  ivec CQIToMCSForRITesting(ivec cqiIndex)
  {
    ivec mcsIndex;
    mcsIndex.set_length(cqiIndex.length());
    for (int k=0;k<cqiIndex.length();k++)
    {    
      if (cqiIndex(k) == 1)
	mcsIndex(k)=0;
      else if (cqiIndex(k) == 2)
	mcsIndex(k)=0;
      else if (cqiIndex(k) == 3)
	mcsIndex(k)=2;
      else if (cqiIndex(k) == 4)
	mcsIndex(k)=4;
      else if (cqiIndex(k) == 5)
	mcsIndex(k)=6;
      else if (cqiIndex(k) == 6)
	mcsIndex(k)=8;
      else if (cqiIndex(k) == 7)
	mcsIndex(k)=11;
      else if (cqiIndex(k) == 8)
	mcsIndex(k)=13;
      else if (cqiIndex(k) == 9)
	mcsIndex(k)=15;
      else if (cqiIndex(k) == 10)
	mcsIndex(k)=18;
      else if (cqiIndex(k) == 11)
	mcsIndex(k)=20;
      else if (cqiIndex(k) == 12)
	mcsIndex(k)=22;
      else if (cqiIndex(k) == 13)
	mcsIndex(k)=24;
      else if (cqiIndex(k) == 14)
	mcsIndex(k)=26;
      else
	mcsIndex(k)=27;
    }
    return(mcsIndex);
  }

/// Function CQIToMCSForCQITesting is used for cqi module testing
ivec CQIToMCSForCQITesting(ivec cqiIndex)
{
  ivec mcsIndex;
  mcsIndex.set_length(cqiIndex.length());
  for (int k=0;k<cqiIndex.length();k++)
  {    
    if (cqiIndex(k) == 1)
      mcsIndex(k)=0;
    else if (cqiIndex(k) == 2)
      mcsIndex(k)=0;
    else if (cqiIndex(k) == 3)
      mcsIndex(k)=2;
    else if (cqiIndex(k) == 4)
      mcsIndex(k)=4;
    else if (cqiIndex(k) == 5)
      mcsIndex(k)=6;
    else if (cqiIndex(k) == 6)
      mcsIndex(k)=8;
    else if (cqiIndex(k) == 7)
      mcsIndex(k)=11;
    else if (cqiIndex(k) == 8)
      mcsIndex(k)=13;
    else if (cqiIndex(k) == 9)
      mcsIndex(k)=16;
    else if (cqiIndex(k) == 10)
      mcsIndex(k)=19;
    else if (cqiIndex(k) == 11)
      mcsIndex(k)=21;
    else if (cqiIndex(k) == 12)
      mcsIndex(k)=23;
    else if (cqiIndex(k) == 13)
      mcsIndex(k)=25;
    else if (cqiIndex(k) == 14)
      mcsIndex(k)=27;
    else
      mcsIndex(k)=27;
  }
  return(mcsIndex);
}
#endif
