#include "../include/wrapper_funcs.h"


unsigned long int getTimeInNanoSec(double time)
{
  return(unsigned long int)(time/1e-9);
}

void printOutputFiles_event(BWSimSystem_S& bwsimSystem, double currentTime, int snrIndx){
  printOutputFiles(bwsimSystem,currentTime,snrIndx);
}

void nodePreUpdate_event(BWSimSystem_S& bwsimSystem, ivec serverNodes, int nuCaID){

  int nuIndx=find(bwsimSystem.mySysInfo.nuIDs,getNUID(nuCaID));
  long unsigned int nexttime = getnext_simtime()-getTimeInNanoSec(bwsimSystem.mySysInfo.nuConfig(nuIndx).ttiDurationInSec);
  std::function<void()> tempobj1 = std::bind(nodeUpdate_event,std::ref(bwsimSystem),serverNodes,nuCaID);
  add_event(nexttime,(int)UPDATE,UPDATE,tempobj1);
}

void nodeUpdate_event(BWSimSystem_S& bwsimSystem, ivec serverNodes, int nuCaID){
  
  nodeUpdate(bwsimSystem,serverNodes,nuCaID);
  int nuIndx=find(bwsimSystem.mySysInfo.nuIDs,getNUID(nuCaID));
  
  if(bwsimSystem.mySysInfo.ulDlMode == _ULDL_MODE_TDD_)
  {
    ivec srvrIndx=find(bwsimSystem.associationInfo.serverNodes,serverNodes);
    for(int k =0; k< srvrIndx.length();k++)
    {
      long unsigned int nexttime; 
      int nuCaIndx=bwsimSystem.myNodeDevices(srvrIndx(k)).getNUIndx(nuCaID);
      int frameConfig=bwsimSystem.myNodeDevices(srvrIndx(k)).L1CorePerNU(nuCaIndx).frameConfig.dlFrameIndx(bwsimSystem.myNodeDevices(srvrIndx(k)).actualLoopCountPerNU(nuCaIndx)%bwsimSystem.myNodeDevices(srvrIndx(k)).L1CorePerNU(nuCaIndx).frameConfig.dlFrameIndx.length());
      nexttime = getnext_simtime()+getTimeInNanoSec(bwsimSystem.mySysInfo.nuConfig(nuIndx).ttiDurationInSec);
      if(frameConfig == 21)
      {
	std::function<void()> tempobj1 = std::bind(nodePreUpdate_event,std::ref(bwsimSystem),serverNodes(k,k),nuCaID);
	add_event(nexttime,(int)PREUPDATE,PREUPDATE,tempobj1); 
      }
      else
      {
	std::function<void()> tempobj1 = std::bind(nodeUpdate_event,std::ref(bwsimSystem),serverNodes(k,k),nuCaID);
	add_event(nexttime,(int)UPDATE,UPDATE,tempobj1);
      }
    }
  }
  else if(bwsimSystem.mySysInfo.ulDlMode == _ULDL_MODE_FDD_)
  {
    ivec srvrIndx=find(bwsimSystem.associationInfo.serverNodes,serverNodes);
    long unsigned int nexttime; 
    int nuCaIndx=bwsimSystem.myNodeDevices(srvrIndx(0)).getNUIndx(nuCaID);
    int frameConfig=bwsimSystem.myNodeDevices(srvrIndx(0)).L1CorePerNU(nuCaIndx).frameConfig.dlFrameIndx(bwsimSystem.myNodeDevices(srvrIndx(0)).actualLoopCountPerNU(nuCaIndx)%bwsimSystem.myNodeDevices(srvrIndx(0)).L1CorePerNU(nuCaIndx).frameConfig.dlFrameIndx.length());
    nexttime = getnext_simtime()+getTimeInNanoSec(bwsimSystem.mySysInfo.nuConfig(nuIndx).ttiDurationInSec);
    if(frameConfig == 1)
    {
      std::function<void()> tempobj1 = std::bind(nodePreUpdate_event,std::ref(bwsimSystem),serverNodes,nuCaID);
      add_event(nexttime,(int)PREUPDATE,PREUPDATE,tempobj1); 
      
    }
    else
    {
      std::function<void()> tempobj1 = std::bind(nodeUpdate_event,std::ref(bwsimSystem),serverNodes,nuCaID);
      add_event(nexttime,(int)UPDATE,UPDATE,tempobj1);
    }
  }
  else
  {
    long unsigned int nexttime = getnext_simtime()+getTimeInNanoSec(bwsimSystem.mySysInfo.nuConfig(nuIndx).ttiDurationInSec);
    
    std::function<void()> tempobj1 = std::bind(nodeUpdate_event,std::ref(bwsimSystem),serverNodes,nuCaID);
    add_event(nexttime,(int)UPDATE,UPDATE,tempobj1);
  }
}


void nodeProcess_event(BWSimSystem_S& bwsimSystem, ivec rxNodeIDs, double currentTime, int nuID, ivec rxBeamIDs){
  
  nodeProcess(bwsimSystem,rxNodeIDs,currentTime/1e9,nuID,rxBeamIDs);
  ivec rxDeviceIndx=find(bwsimSystem.nodeDeviceIDs,rxNodeIDs); 
  
  bvec isDownlink=(find(bwsimSystem.mySysInfo.serverNodes,rxNodeIDs)==-1);
  int nuIndxInMySysInfo=find(bwsimSystem.mySysInfo.nuIDs,getNUID(nuID));
  long unsigned int startTime = getnext_simtime()-getTimeInNanoSec(bwsimSystem.mySysInfo.nuConfig(nuIndxInMySysInfo).ttiDurationInSec);
  
  for(int rx_cnt =0 ; rx_cnt < rxNodeIDs.length(); rx_cnt++)
  {
    if(isDownlink(rx_cnt))
    {
      int nuIndx=bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).getNUIndx(nuID);
      if(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx).isToComputeDLFeedback()) 
      {
	DownlinkCQIModeInfo_S dlCQIModeInfo=bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx).receiverConfig.dlCQIModeInfo;
	long unsigned int nexttime=startTime+getTimeInNanoSec((dlCQIModeInfo.CQIReportingDelay+dlCQIModeInfo.CQIProcessingDelay)/1e3); // Both reporting delay and processing delay handled in same place. Need to change in future - Dhiv
    cout<<"[detl:] NodeCQIReport_event scheduled at : "<<nexttime<<endl;
    std::function<void()> tempobj1 = std::bind(nodeCQIReport_event,std::ref(bwsimSystem),rxNodeIDs(rx_cnt),"",nexttime,nuID);
	add_event(nexttime,(int)CQIREPORT,CQIREPORT,tempobj1); 
      }
    }
    else
    {
      int nuIndx=bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).getNUIndx(nuID);
      
      ivec serviceNodesWithCQI=bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx).uplinkCQIInfo.serviceNodeIDs(find(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx).uplinkCQIInfo.isToReportCQI));
      
      if(serviceNodesWithCQI.length()) // Finding any UE CQI to be updated
      {
	long unsigned int nexttime; 
	UplinkCQIModeInfo_S ulCQIModeInfo=bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx).receiverConfig.ulCQIModeInfo;
	nexttime =startTime+getTimeInNanoSec((ulCQIModeInfo.CQIProcessingDelay)/1e3); // No reporting delay in UL - Dhiv
	
	cout<<"[detl:] NodeCQIReport_event scheduled at : "<<nexttime<<endl;
	std::function<void()> tempobj1 = std::bind(nodeCQIReport_event,std::ref(bwsimSystem),rxNodeIDs(rx_cnt),serviceNodesWithCQI,nexttime,nuID);
	add_event(nexttime,(int)CQIREPORT,CQIREPORT,tempobj1); 
	bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx).uplinkCQIInfo.isToReportCQI.zeros();
      }
    }
  }
}

void nodeReceive_event(BWSimSystem_S& bwsimSystem , ivec rxNodeIDs, int nuCaID, double currentTime, ivec rxBeamIDs){

  ivec rxNodesToProcess=nodeReceive(bwsimSystem,rxNodeIDs,nuCaID,currentTime/1e9,rxBeamIDs);
  std::function<void()> tempobj1 = std::bind(nodeProcess_event,std::ref(bwsimSystem),rxNodesToProcess,currentTime,nuCaID,rxBeamIDs);
  // Priority of PROCESS event changed to RECEIVE to clear channel faster. This can reduce
  // memory usage.
  // SecondLevelPriority set to high value for high priority execution
  add_event(getnext_simtime(),(int)RECEIVE,PROCESS,tempobj1, 10000);
}

void nodeTransmit_event(BWSimSystem_S& bwsimSystem , ivec txNodeIDs, int nuCaID, ivec txBeamIDs){
  
  if(traceValuePerTraceName["EnablePrints"]=="true")
    cout<<"[detl:]Transmitting from : "<<txNodeIDs<<" for nuID : "<<nuCaID<<" and beamID : "<<txBeamIDs<<endl;
  
  nodeTransmit(bwsimSystem,txNodeIDs,nuCaID,txBeamIDs);
}

void nodeGenerateData_event(BWSimSystem_S& bwsimSystem , ivec txNodeIDs, double currentTime, int nuCaID,ivec txBeamIDs){

  if(traceValuePerTraceName["EnablePrints"]=="true")
    cout<<"[detl:]Generating Data for : "<<txNodeIDs<<" , "<<nuCaID<<" , "<<txBeamIDs<<endl;
  
  // This line assumes transmit event executed immediately after generate data . 
  //If not there will be TxFrameInfo mismatch during transmit event  -Dhiv
  bwsimSystem.myTxFrameInfo=nodeGenerate(bwsimSystem,txNodeIDs,currentTime/1e9,nuCaID,txBeamIDs); 
  std::function<void()> tempobj = std::bind(nodeTransmit_event,std::ref(bwsimSystem),txNodeIDs,nuCaID,txBeamIDs);
  // Priority of transmit changes to generate data to clear TxFrameInfo faster. - Dhiv
  // SecondLevelPriority set to high value for high priority execution
  add_event(getnext_simtime(),(int)GENERATEDATA,TRANSMIT,tempobj,10000);
}

void nodePreSchedule_event(BWSimSystem_S& bwsimSystem, ivec serverNodes, double currentTime, int nuCaID,int beamID){
  
  int nuIndx=find(bwsimSystem.mySysInfo.nuIDs,getNUID(nuCaID));
  long unsigned int nexttime = getnext_simtime()-getTimeInNanoSec(bwsimSystem.mySysInfo.nuConfig(nuIndx).ttiDurationInSec);
  std::function<void()> tempobj1 = std::bind(nodeSchedule_event,std::ref(bwsimSystem),serverNodes,(double)(nexttime),nuCaID,beamID);
  add_event(nexttime,(int)SCHEDULE,SCHEDULE,tempobj1,serverNodes(0));
  
}

void nodePostSchedule_event(BWSimSystem_S& bwsimSystem, ivec serverNodes, double currentTime, int nuCaID,int beamID){
    
    if(traceValuePerTraceName["EnablePrints"]=="true")
        cout<<"[detl:]Post Scheduling processing for : "<<serverNodes<<" , "<<nuCaID<<" , "<<beamID<<endl;
    
    nodeScheduleRS(bwsimSystem,serverNodes,currentTime/1e9,nuCaID,beamID);
    
    ivec txNodeIDs(0);
    ivec rxNodeIDs(0);
    
    
    ivec srvrIndices=find(bwsimSystem.associationInfo.serverNodes,serverNodes);
    for(int srvr_cnt=0;srvr_cnt<serverNodes.length();srvr_cnt++)
    {
        for(int txLoop_cnt=0;txLoop_cnt<bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo.length();txLoop_cnt++)
        {
            if(bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(txLoop_cnt).txNUID==nuCaID)
            {
                if(bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(txLoop_cnt).txBeamID==beamID)
                {	
                    int srvrIndx=getServerIndx(bwsimSystem.associationInfo,bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(txLoop_cnt).txNodeID);
                    bool goFlag=true;
                    #ifdef USING_MPI
                    if(nTasks>1)
                        if(find(bwsimSystem.mySysInfo.mpiInfo.srvrIndxListPerTaskid(currentTaskid),srvrIndx)==-1)
                            goFlag=false;
                    #endif
                    if(bwsimSystem.mySysInfo.receptionModel.enableRestrictedRecepition)	
                        if(find(bwsimSystem.mySysInfo.receptionModel.serverNodesWithActualReception,bwsimSystem.associationInfo.serverNodes(srvrIndx))==-1)
                            goFlag=false;
                            
                    append(txNodeIDs,bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(txLoop_cnt).txNodeID);
                    if(goFlag)
                    {
                        ivec currentRxNodes=findUniqueNumbers(concat(bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(txLoop_cnt).rxNodeIDs,bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(txLoop_cnt).rsRxNodeIDs));
                        append(rxNodeIDs,currentRxNodes);
                    }
                        
                }	
            }
        }
    }
    txNodeIDs=findUniqueNumbers(txNodeIDs);
    rxNodeIDs=findUniqueNumbers(rxNodeIDs);
    ivec txBeamIDs=zeros_i(txNodeIDs.length())+beamID;
    ivec rxBeamIDs=zeros_i(rxNodeIDs.length())+beamID;
    
    // Uplink Nodes
    ivec srvcIndx=find(bwsimSystem.associationInfo.serviceNodes,txNodeIDs);
    for(int srvc_cnt=0;srvc_cnt<srvcIndx.length();srvc_cnt++)
    {
        if(srvcIndx(srvc_cnt)!=-1)
        {
            txBeamIDs(srvc_cnt)=bwsimSystem.associationInfo.associatedBeamPairLinkforEachServiceNode(srvcIndx(srvc_cnt)).srvcBeamID;
        }
    }
    
    // Downlink Nodes
    srvcIndx=find(bwsimSystem.associationInfo.serviceNodes,rxNodeIDs);
    for(int srvc_cnt=0;srvc_cnt<srvcIndx.length();srvc_cnt++)
    {
        if(srvcIndx(srvc_cnt)!=-1)
        {
            rxBeamIDs(srvc_cnt)=bwsimSystem.associationInfo.associatedBeamPairLinkforEachServiceNode(srvcIndx(srvc_cnt)).srvcBeamID;
        }
    }
    
        
    if(txNodeIDs.length())
    {
        for(int tx_cnt=0;tx_cnt<txNodeIDs.length();tx_cnt++)
        {
            std::function<void()> tempobj2 = std::bind(nodeGenerateData_event,std::ref(bwsimSystem),txNodeIDs(tx_cnt,tx_cnt),(double)(getnext_simtime()),nuCaID,txBeamIDs(tx_cnt,tx_cnt));
            add_event(getnext_simtime(),(int)GENERATEDATA,GENERATEDATA,tempobj2);
        }
    }
    
    if(rxNodeIDs.length())
    {
        //why not segragating receivers w.r.t numerology.
        int nuIndx=find(bwsimSystem.mySysInfo.nuIDs,getNUID(nuCaID));
        
        for(int rx_cnt=0;rx_cnt<rxNodeIDs.length();rx_cnt++)
        {
            std::function<void()> tempobj = std::bind(nodeReceive_event,std::ref(bwsimSystem),rxNodeIDs(rx_cnt,rx_cnt),nuCaID,(double)getnext_simtime(),rxBeamIDs(rx_cnt,rx_cnt));
            add_event(getnext_simtime()+getTimeInNanoSec(bwsimSystem.mySysInfo.nuConfig(nuIndx).ttiDurationInSec),(int)RECEIVE,RECEIVE,tempobj);
        }
    }

}

// void nodeTransmitSSB_event(BWSimSystem_S& bwsimSystem , ivec txNodeIDs,ivec txBeamIDs,ivec txPanelIDs, double currentTime){
//     
//     cout<<"Transmit SSB from : "<<txNodeIDs<<" at time "<<currentTime<<endl;
//     nodeTransmitSSB(bwsimSystem,txNodeIDs,txBeamIDs,txPanelIDs,currentTime/1e9);
//     
//     ivec rxNodeIDs(0);
//     ivec srvrIndices=find(bwsimSystem.associationInfo.serverNodes,txNodeIDs);
//     ivec nodeIndices=find(bwsimSystem.nodeDeviceIDs,txNodeIDs);
//     
//     for(int tx_cnt=0;tx_cnt<txNodeIDs.length();tx_cnt++)
//     {
//         rxNodeIDs=bwsimSystem.associationInfo.associatedServiceNodesPerServerNode(srvrIndices(tx_cnt));
//         for(int rx_cnt=0;rx_cnt<rxNodeIDs.length();rx_cnt++)
//         {
//             std::function<void()> tempobj = std::bind(nodeReceiveSSB_event,std::ref(bwsimSystem),to_ivec(rxNodeIDs(rx_cnt)),(double)getnext_simtime());
//             add_event(getnext_simtime()+getTimeInNanoSec(get_nextSSBlockTime(bwsimSystem.myNodeDevices(nodeIndices(tx_cnt)).SSBurstSetConfig,1)),(int)RECEIVE,RECEIVE,tempobj);
//         }
//         long unsigned int nextSSBTime=getTimeInNanoSec(get_nextSSBlockTime(bwsimSystem.myNodeDevices(nodeIndices(tx_cnt)).SSBurstSetConfig,0));
//         std::function<void()> tempobj = std::bind(nodeTransmitSSB_event,std::ref(bwsimSystem),to_ivec(txNodeIDs(tx_cnt)),to_ivec(txBeamIDs(tx_cnt)),to_ivec(txPanelIDs(tx_cnt)),(double)getnext_simtime()+nextSSBTime);
// 
//         add_event(getnext_simtime()+nextSSBTime,(int)TRANSMIT,TRANSMIT,tempobj);
//     }
// }
// 
// void nodeReceiveSSB_event(BWSimSystem_S& bwsimSystem , ivec rxNodeIDs, double currentTime){
//     
//     nodeReceiveSSB(bwsimSystem,rxNodeIDs,currentTime);
// }

    
void nodeExchangeMPIData_event(BWSimSystem_S& bwsimSystem, int nuCaID,int beamID){
  
  #ifdef USING_MPI
  if(nTasks>1)
  {
    if(traceValuePerTraceName["EnablePrints"]=="true")
      cout<<"Exchanging MPI Data ..."<<endl;
    exchangeMPIDataNew(bwsimSystem.myNodeDevices,bwsimSystem.nodeDeviceIDs,bwsimSystem.mySysInfo.serverNodes,bwsimSystem.associationInfo,bwsimSystem.mySysInfo,bwsimSystem.schedulerOutput,nuCaID,beamID);
  
    int nuIndx=find(bwsimSystem.mySysInfo.nuIDs,getNUID(nuCaID));

    std::function<void()> tempobj = std::bind(nodeExchangeMPIData_event,std::ref(bwsimSystem),nuCaID,beamID);
    add_event(getnext_simtime()+getTimeInNanoSec(bwsimSystem.mySysInfo.nuConfig(nuIndx).ttiDurationInSec),(int)MPIEXCHANGE,MPIEXCHANGE,tempobj);
  }
  #endif
}

void nodeSchedule_event(BWSimSystem_S& bwsimSystem, ivec serverNodes, double currentTime, int nuCaID,int beamID){

  int nuIndx=find(bwsimSystem.mySysInfo.nuIDs,getNUID(nuCaID));
  int currentTTI = currentTime/getTimeInNanoSec(bwsimSystem.mySysInfo.nuConfig(nuIndx).ttiDurationInSec);
  
  if(traceValuePerTraceName["EnablePrints"]=="true")
    cout<<"[detl:]Scheduling for : "<<serverNodes<<" , "<<nuCaID<<" , "<<beamID<<" for TTI : "<<currentTTI<<endl;
  
  if(currentTTI==0)
  {
      createDirectory(bwsimSystem.mySysInfo.resultFiles.directory+"NU_"+toString(nuCaID));
  }
  if(currentTaskid==0)
  {
    ofstream op(bwsimSystem.mySysInfo.resultFiles.directory+"briefSimulationDetails.txt",std::ios::out);
    op<<briefSimulationDetails(currentTime*1e-9)<<endl;op.close();
  }
  ivec dlServerNodes=nodeScheduleData(bwsimSystem,serverNodes,currentTime/1e9,nuCaID,beamID);
  
  if(bwsimSystem.mySysInfo.ulDlMode == _ULDL_MODE_TDD_)
  {
    ivec srvrIndx=find(bwsimSystem.associationInfo.serverNodes,serverNodes);
    for(int k =0; k< srvrIndx.length();k++)
    {
      long unsigned int nexttime; 
      int nuCaIndx=bwsimSystem.myNodeDevices(srvrIndx(k)).getNUIndx(nuCaID);
      int frameConfig=bwsimSystem.myNodeDevices(srvrIndx(k)).L1CorePerNU(nuCaIndx).frameConfig.dlFrameIndx(bwsimSystem.myNodeDevices(srvrIndx(k)).actualLoopCountPerNU(nuCaIndx)%bwsimSystem.myNodeDevices(srvrIndx(k)).L1CorePerNU(nuCaIndx).frameConfig.dlFrameIndx.length());
      nexttime = getnext_simtime()+getTimeInNanoSec(bwsimSystem.mySysInfo.nuConfig(nuIndx).ttiDurationInSec);
      if(frameConfig == 21)
      {
	std::function<void()> tempobj1 = std::bind(nodePreSchedule_event,std::ref(bwsimSystem),serverNodes(k,k),(double)(nexttime),nuCaID,beamID);
	add_event(nexttime,(int)PRESCHEDULE,PRESCHEDULE,tempobj1,serverNodes(k));
      }
      else
      {
	std::function<void()> tempobj1 = std::bind(nodeSchedule_event,std::ref(bwsimSystem),serverNodes(k,k),(double)(nexttime),nuCaID,beamID);
	add_event(nexttime,(int)SCHEDULE,SCHEDULE,tempobj1,serverNodes(k));
      }
    }
  }
  else if(bwsimSystem.mySysInfo.ulDlMode == _ULDL_MODE_FDD_)
  {
    ivec srvrIndx=find(bwsimSystem.associationInfo.serverNodes,serverNodes);
    long unsigned int nexttime; 
    int nuCaIndx=bwsimSystem.myNodeDevices(srvrIndx(0)).getNUIndx(nuCaID);
    int frameConfig=bwsimSystem.myNodeDevices(srvrIndx(0)).L1CorePerNU(nuCaIndx).frameConfig.dlFrameIndx(bwsimSystem.myNodeDevices(srvrIndx(0)).actualLoopCountPerNU(nuCaIndx)%bwsimSystem.myNodeDevices(srvrIndx(0)).L1CorePerNU(nuCaIndx).frameConfig.dlFrameIndx.length());
    nexttime = getnext_simtime()+getTimeInNanoSec(bwsimSystem.mySysInfo.nuConfig(nuIndx).ttiDurationInSec);
    if(frameConfig == 1)
    {
      std::function<void()> tempobj1 = std::bind(nodePreSchedule_event,std::ref(bwsimSystem),serverNodes,(double)(nexttime),nuCaID,beamID);
      add_event(nexttime,(int)PRESCHEDULE,PRESCHEDULE,tempobj1); 
    }
    else
    {
      std::function<void()> tempobj1 = std::bind(nodeSchedule_event,std::ref(bwsimSystem),serverNodes,(double)(nexttime),nuCaID,beamID);
      add_event(nexttime,(int)SCHEDULE,SCHEDULE,tempobj1,serverNodes(0));
    }
  }
  else
  {
    long unsigned int nexttime = getnext_simtime()+getTimeInNanoSec(bwsimSystem.mySysInfo.nuConfig(nuIndx).ttiDurationInSec);
    
    std::function<void()> tempobj1 = std::bind(nodeSchedule_event,std::ref(bwsimSystem),serverNodes,(double)(nexttime),nuCaID,beamID);
    add_event(nexttime,(int)SCHEDULE,SCHEDULE,tempobj1,serverNodes(0));
  }

  std::function<void()> tempobj1 = std::bind(nodePostSchedule_event,std::ref(bwsimSystem),serverNodes,(double)getnext_simtime(),nuCaID,beamID);
  add_event(getnext_simtime(),(int)POSTSCHEDULE,POSTSCHEDULE,tempobj1);
}

void endSimulation_event(){

  std::cout<<"\n\n\n\n\n\n\n\n\n\nEnd of Simulation\n\n\n\n\n\n\n\n\n\n";
}

void initLinkSimulation_event(BWSimSystem_S& bwsimSystem){
 
  initLinkSimulation(bwsimSystem.mySysInfo,bwsimSystem.mySys.channelCloud,bwsimSystem.myNodeDevices,bwsimSystem.dlPerformanceMetric,bwsimSystem.ulPerformanceMetric,bwsimSystem.snrLoop_cnt);
  
  if(isCDLModel(bwsimSystem.mySys.channelCloud.sysWideChannelInfo.channelModel) && bwsimSystem.snrLoop_cnt!=0)
    bwsimSystem.mySys.channelCloud.llsChannel.reInitChannel(&bwsimSystem.mySys.aNodes(0).txAntenna,&bwsimSystem.mySys.aNodes(1).rxAntenna,bwsimSystem.mySys.channelCloud.sysWideChannelInfo.delaySpreadTDL);
  
  ivec serverNodes = bwsimSystem.associationInfo.serverNodes;
  
  int carrierInMHz=(int)bwsimSystem.mySysInfo.carrierInfo.carrierFreqInMHz; 
  int nuCaID=carrierInMHz*100+bwsimSystem.mySysInfo.nuIDs(0);
  std::function<void()> tempobj1 = std::bind(nodeUpdate_event,std::ref(bwsimSystem),serverNodes,nuCaID);
  add_event(0,(int)UPDATE,UPDATE,tempobj1);
  std::function<void()> tempobj2 = std::bind(nodeSchedule_event,std::ref(bwsimSystem),serverNodes,0,nuCaID,0);
  add_event(0,(int)SCHEDULE,SCHEDULE,tempobj2);
  std::function<void()> tempobj3 = std::bind(printOutputFiles_event,std::ref(bwsimSystem),bwsimSystem.mySysInfo.simulationTimeInSec,bwsimSystem.snrLoop_cnt);
  add_event(getTimeInNanoSec(bwsimSystem.mySysInfo.simulationTimeInSec),(int)PRINTOUTPUT,PRINTOUTPUT,tempobj3);
  
  
  bwsimSystem.snrLoop_cnt++;
  for(int node_cnt=0;node_cnt<bwsimSystem.myNodeDevices.length();node_cnt++)
  {
    bwsimSystem.myNodeDevices(node_cnt).actualLoopCountPerNU=-1;
    bwsimSystem.myNodeDevices(node_cnt).ttiCountPerNU=-1;
  }
  
  
  if(bwsimSystem.snrLoop_cnt<bwsimSystem.snrLoopCount){
    
    std::function<void()> tempobj1 = std::bind(initLinkSimulation_event,std::ref(bwsimSystem));
    add_event(getTimeInNanoSec(bwsimSystem.mySysInfo.simulationTimeInSec),(int)LINKINIT,LINKINIT,tempobj1);
  }
  else
    bwsimSystem.snrLoop_cnt--;
  
}

void initializeEvents(BWSimSystem_S& bwsimSystem){
  
  ivec serverNodes = bwsimSystem.associationInfo.serverNodes;
  if(bwsimSystem.mySysInfo.linkSimulationInfo.isEnabled)
  {
    std::function<void()> tempobj1 = std::bind(initLinkSimulation_event,std::ref(bwsimSystem));
    add_event(0,(int)LINKINIT,LINKINIT,tempobj1);
  }
  else
  {
    ivec deviceIndices=find(bwsimSystem.nodeDeviceIDs,serverNodes);
    for(int srvr_cnt=0;srvr_cnt<serverNodes.length();srvr_cnt++)
    {
        for(int nu_cnt=0;nu_cnt<bwsimSystem.myNodeDevices(deviceIndices(srvr_cnt)).nuIDs.length();nu_cnt++)
        {
//          if(find(bwsimSystem.mySysInfo.carrierInfo.carrierFreqInMHz,getCarrierFreqInMHz(bwsimSystem.myNodeDevices(deviceIndices(srvr_cnt)).nuIDs(nu_cnt)))!=-1)
            {
                std::function<void()> tempobj1 = std::bind(nodeUpdate_event,std::ref(bwsimSystem),to_ivec(serverNodes(srvr_cnt)),bwsimSystem.myNodeDevices(deviceIndices(srvr_cnt)).nuIDs(nu_cnt));
                add_event(0,(int)UPDATE,UPDATE,tempobj1);
                for(int bm_cnt=0;bm_cnt<bwsimSystem.myNodeDevices(deviceIndices(srvr_cnt)).beamIDs.length();bm_cnt++)
                {
                    std::function<void()> tempobj2 = std::bind(nodeSchedule_event,std::ref(bwsimSystem),to_ivec(serverNodes(srvr_cnt)),0.0,bwsimSystem.myNodeDevices(deviceIndices(srvr_cnt)).nuIDs(nu_cnt),bwsimSystem.myNodeDevices(deviceIndices(srvr_cnt)).beamIDs(bm_cnt));
                    add_event(0,(int)SCHEDULE,SCHEDULE,tempobj2,serverNodes(srvr_cnt)); //using serverNodeID as SecondLevelPriority for MPI purposes- Dhiv
                }
            }
        }
        //!commented temporarily - Dhiv
        
//       if(bwsimSystem.mySysInfo.isSSBlockEnabled==1)
//       {
//         double Tsa = 1/bwsimSystem.myNodeDevices(deviceIndices(srvr_cnt)).SSBurstSetConfig.getSamplingFrequency();
//       int nfftSize=1/(Tsa*bwsimSystem.myNodeDevices(deviceIndices(srvr_cnt)).SSBurstSetConfig.getSSBscs()*1e3);
//         int ssb0StartSym=bwsimSystem.myNodeDevices(deviceIndices(srvr_cnt)).SSBurstSetConfig.getssBurstSetConfig()(0);
//       int totalCPLength=ssb0StartSym*144 + 16.0*(bwsimSystem.myNodeDevices(deviceIndices(srvr_cnt)).SSBurstSetConfig.getSSBscs()/15);
//         // PanelID and BeamID hardcoded for SSB.. Need to fix - Dhiv
//       double ssb0StartTiming=(double)(ssb0StartSym + totalCPLength/2048.0)*(double)(nfftSize)*Tsa;
//       std::function<void()> tempobj2 = std::bind(nodeTransmitSSB_event,std::ref(bwsimSystem),to_ivec(serverNodes(srvr_cnt)),"0","0",getTimeInNanoSec(ssb0StartTiming));
//       add_event(getTimeInNanoSec(ssb0StartTiming),(int)TRANSMIT,TRANSMIT,tempobj2);
//       }
    }
      
    // Needs to be done for each NU and Carrier  - Dhiv
    std::function<void()> tempobj1 = std::bind(nodeExchangeMPIData_event,std::ref(bwsimSystem),bwsimSystem.myNodeDevices(0).nuIDs(0),0);
    add_event(0,(int)MPIEXCHANGE,MPIEXCHANGE,tempobj1);
        
    if((int)(bwsimSystem.mySysInfo.simulationTimeInSec/bwsimSystem.mySysInfo.outputFilePrintFrequency)>1)
	for(int print_cnt=1;print_cnt<(int)(bwsimSystem.mySysInfo.simulationTimeInSec/bwsimSystem.mySysInfo.outputFilePrintFrequency);print_cnt++)
	{
	  std::function<void()> tempobj3 = std::bind(printOutputFiles_event,std::ref(bwsimSystem),(double)print_cnt*bwsimSystem.mySysInfo.outputFilePrintFrequency,0);
	  add_event(getTimeInNanoSec((double)print_cnt*bwsimSystem.mySysInfo.outputFilePrintFrequency),(int)PRINTOUTPUT,PRINTOUTPUT,tempobj3);
	}
	
	std::function<void()> tempobj3 = std::bind(printOutputFiles_event,std::ref(bwsimSystem),bwsimSystem.mySysInfo.simulationTimeInSec,0);
      add_event(getTimeInNanoSec(bwsimSystem.mySysInfo.simulationTimeInSec),(int)PRINTOUTPUT,PRINTOUTPUT,tempobj3);
      
      
  }  


  std::function<void()> tempobj4 = std::bind(endSimulation_event);
  add_event(getTimeInNanoSec(bwsimSystem.mySysInfo.simulationTimeInSec),(int)END_SIMULATION,END_SIMULATION,tempobj4);
}

// void nodeHARQReport_event(BWSimSystem_S& bwsimSystem, ivec rxNodeIDs, double currentTime, int nuID,ivec rxBeamIDs)
// {
//   ivec rxDeviceIndx=find(bwsimSystem.nodeDeviceIDs,rxNodeIDs);
//   
//   for(int rx_cnt1=0;rx_cnt1<rxNodeIDs.length();rx_cnt1++)
//   {
//     
//     int rxNodeID=rxNodeIDs(rx_cnt1);
//     int centralUnitID= getCentralUnitID(bwsimSystem.associationInfo,rxNodeID);
//     int rxSrvcIndx=find(bwsimSystem.mySysInfo.serviceNodes,rxNodeID);
//     bool isDownlink;
//     
//     int schToRxInfoIndx=bwsimSystem.schedulerOutput(centralUnitID).getSchedulerToReceiverInfoIndx(rxNodeID,nuID,rxBeamIDs(rx_cnt1));
//     bool isScheduled=true;
//     
//     if(schToRxInfoIndx==-1)
//       isScheduled=false;
//     
//     if(rxSrvcIndx!=-1)
//     {
//       isDownlink=1; //! Down-link
//     }
//     else
//     {
//       isDownlink=0;//! Up-link
//     }
//     if(isScheduled)  
//     {  
//   SchedulerToReceiverInfo_S mySchedulerToReceiverInfo=bwsimSystem.schedulerOutput(centralUnitID).schedulerToReceiverInfo(schToRxInfoIndx);
//   for(int blk_cnt=0;blk_cnt<mySchedulerToReceiverInfo.dataBlkInfo.length();blk_cnt++)
//   {
//     if(isDownlink)
//       nodeReportHARQ(bwsimSystem,mySchedulerToReceiverInfo.rxNodeID,mySchedulerToReceiverInfo.txNodeIDs(blk_cnt),bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).ttiCountPerNU(nuIndx),txStatus(blk_cnt),isDownlink,nuID);
//     else
//       nodeReportHARQ(bwsimSystem,mySchedulerToReceiverInfo.txNodeIDs(blk_cnt),mySchedulerToReceiverInfo.rxNodeID,bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).ttiCountPerNU(nuIndx),txStatus(blk_cnt),isDownlink,nuID);
//   }
//   }
//  }
// }

void  nodeCQIReport_event(BWSimSystem_S& bwsimSystem, int rxNodeID, ivec validServiceNodes , double currentTime, int nuID)
{
  int rxDeviceIndx=find(bwsimSystem.nodeDeviceIDs,rxNodeID);
  
  int rxSrvcIndx=find(bwsimSystem.mySysInfo.serviceNodes,rxNodeID);
  bool isDownlink;
  int nuIndx=bwsimSystem.myNodeDevices(rxDeviceIndx).getNUIndx(nuID);
  if(rxSrvcIndx!=-1)
  {
    isDownlink=1; //! Down-link
  }
  else
  {
    isDownlink=0;//! Up-link
  }
  if(!isDownlink)
  {
    if(bwsimSystem.myNodeDevices(rxDeviceIndx).receiverPerNU(nuIndx).isLinkAdaptationEnabled())
    {   
      
      ivec srvcIndx = find(bwsimSystem.myNodeDevices(rxDeviceIndx).receiverPerNU(nuIndx).uplinkCQIInfo.serviceNodeIDs,validServiceNodes);
      for(int srvcNode_cnt = 0; srvcNode_cnt < validServiceNodes.length(); srvcNode_cnt++)
      {
	int indxInULCQIInfo = srvcIndx(srvcNode_cnt);
	if(indxInULCQIInfo!=-1)
	{
	  cout<<"[detl:]Updating UL CQI from Node : "<<validServiceNodes(srvcNode_cnt)<<endl;
	  PerUserULCQIFeedback_S ulCQI = bwsimSystem.myNodeDevices(rxDeviceIndx).receiverPerNU(nuIndx).uplinkCQIInfo.perUserULCQIFeedback(indxInULCQIInfo);
	  nodeReportULCQI(bwsimSystem,validServiceNodes(srvcNode_cnt),rxNodeID,bwsimSystem.myNodeDevices(rxDeviceIndx).ttiCountPerNU(nuIndx),ulCQI,nuID);
	}
      }
      
    } 
    
  }
  else
  {
    //CQI- The computation of periodic CQI/PMI or Aperiodic CQI/PMI if triggered is done in generateFeedback
    static ivec ontimeflag=zeros_i(bwsimSystem.mySysInfo.serviceNodes.length());
    if(bwsimSystem.myNodeDevices(rxDeviceIndx).receiverPerNU(nuIndx).isLinkAdaptationEnabled() && ontimeflag(rxSrvcIndx) == 0)
    {
      ontimeflag(rxSrvcIndx) =  1;
      //cout<<"ontimeflag:"<<ontimeflag(rxSrvcIndx)<<endl;
      cout<<"[detl:]Updating DL CQI from Node: "<<rxNodeID<<endl;
      PerUserDLCQIFeedback_S dlCQI = bwsimSystem.myNodeDevices(rxDeviceIndx).receiverPerNU(nuIndx).cqiEstimator.downlinkCQIInfo->perUserFeedback;
      nodeReportDLCQI(bwsimSystem,rxNodeID,bwsimSystem.associationInfo.getMyAssociatedServerNode(rxNodeID),bwsimSystem.myNodeDevices(rxDeviceIndx).ttiCountPerNU(nuIndx),dlCQI,nuID);
    }
    if(!ontimeflag(rxSrvcIndx))
    {
      cout<<"Error in flag"<<endl;  
      abort();
    }
  }
  
}
