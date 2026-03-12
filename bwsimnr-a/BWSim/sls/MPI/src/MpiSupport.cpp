/*************************************************************************
 * 
 * CEWiT CONFIDENTIAL
 * __________________
 *
 * All Rights Reserved © 2014 CEWiT, India
 *
 * \ NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
 * and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
 * Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
 * express, printed and signed license for use is strictly forbidden.
 */


#include "../include/MpiSupport.h"
#include "../../sim/include/SupportingFunctions.h"

MPI_SchedulingInfo_S MPI_encode(DataBlkInfo_S &dataBlkInfo,int serverNodeID, double scTxPowIndBm, int nuID, double carrierFreqInMHz, int ttiCount, bool isDownlink)
{
  MPI_SchedulingInfo_S MPI_SchedulingInfo;
  
  // Filling isScheduled
  MPI_SchedulingInfo.isScheduled = true;
  
  MPI_SchedulingInfo.isDownlink = isDownlink;
  
  // Filling TxScheme
  MPI_SchedulingInfo.TxScheme = (unsigned int)dataBlkInfo.dataGenInfo.transmissionScheme;

  MPI_SchedulingInfo.waveform=dataBlkInfo.waveform;
  
  // Filling nPorts
  MPI_SchedulingInfo.nPorts = (unsigned int)dataBlkInfo.dataGenInfo.rbInfo.rsInfo.nCQIports;
  
  // Filling nLayers
  MPI_SchedulingInfo.nLayers = (unsigned int)dataBlkInfo.dataGenInfo.nLayers;
  
  MPI_SchedulingInfo.dmrstype = dataBlkInfo.dataGenInfo.layersInfo.dmrstype;
  
  MPI_SchedulingInfo.maxLength = dataBlkInfo.dataGenInfo.layersInfo.maxLength;
  
  //Filling LayerInfo
  MPI_SchedulingInfo.nCDM=dataBlkInfo.dataGenInfo.layersInfo.nCDM;
  MPI_SchedulingInfo.CDM=dataBlkInfo.dataGenInfo.layersInfo.CDM;
  MPI_SchedulingInfo.CDMMapLength=dataBlkInfo.dataGenInfo.layersInfo.CDMMap.length();
  
  for(int cnt=0;cnt<MPI_SchedulingInfo.CDMMapLength;cnt++)
  {
    MPI_SchedulingInfo.CDMMap[cnt]=dataBlkInfo.dataGenInfo.layersInfo.CDMMap(cnt);
  }
  
  // Filling nCodewords
  MPI_SchedulingInfo.nCodewords = (unsigned int)dataBlkInfo.dataGenInfo.nCodeWords;
  
  // Filling prbBundlingSize
  MPI_SchedulingInfo.prbBundlingSize = (unsigned int)dataBlkInfo.prbBundlingSize;
  
  // Filling MCS_1
  MPI_SchedulingInfo.MCS_1 = (unsigned int)dataBlkInfo.dataGenInfo.mcsIndices(0);
  
  // Filling MCS_2 if nCodeWords is 2
  MPI_SchedulingInfo.MCS_2 = 0;
  if(dataBlkInfo.dataGenInfo.nCodeWords==2)
    MPI_SchedulingInfo.MCS_2 = (unsigned int)dataBlkInfo.dataGenInfo.mcsIndices(1);
  
  MPI_SchedulingInfo.harqProcessID=dataBlkInfo.harqInfo.harqProcessID;
  
  MPI_SchedulingInfo.cwdIndxTB1=dataBlkInfo.harqInfo.codeWordIndicesToConsider(0);
  MPI_SchedulingInfo.rvIndxTB1=dataBlkInfo.harqInfo.rvIndicesPerCodeword(0);
  MPI_SchedulingInfo.ndiTB1=dataBlkInfo.harqInfo.newDataIndicator(0);            
  MPI_SchedulingInfo.isNewTB1=dataBlkInfo.harqInfo.isNewTransmission(0); 
  MPI_SchedulingInfo.sizeTB1=dataBlkInfo.harqInfo.TBSizePerCodewordInBits(0);
  
  if(dataBlkInfo.dataGenInfo.nCodeWords==2)
  {
    MPI_SchedulingInfo.cwdIndxTB2=dataBlkInfo.harqInfo.codeWordIndicesToConsider(1);
    MPI_SchedulingInfo.rvIndxTB2=dataBlkInfo.harqInfo.rvIndicesPerCodeword(1);
    MPI_SchedulingInfo.ndiTB2=dataBlkInfo.harqInfo.newDataIndicator(1);            
    MPI_SchedulingInfo.isNewTB2=dataBlkInfo.harqInfo.isNewTransmission(1); 
    MPI_SchedulingInfo.sizeTB2=dataBlkInfo.harqInfo.TBSizePerCodewordInBits(1);
  }   
  
  // Filling serverNodeID
  MPI_SchedulingInfo.serverNodeID = serverNodeID;
  
  // Filling Tx Pow
  MPI_SchedulingInfo.txPowIndBm = scTxPowIndBm;
  
  MPI_SchedulingInfo.nuID = nuID;
  
  MPI_SchedulingInfo.carrierFreqInMHz=carrierFreqInMHz;
  
  MPI_SchedulingInfo.ttiCount=ttiCount;
  
  if(dataBlkInfo.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_OLMUMIMO_)
    MPI_SchedulingInfo.isSemiOpenloopScheme = dataBlkInfo.isSemiOpenLoopScheme;
  
  for(int sym_cnt=0;sym_cnt<dataBlkInfo.symPos.length();sym_cnt++)
  {
    if(dataBlkInfo.symPos(sym_cnt)>=100)
    {
      cout<<"Unsupported SymPos for MPI_encode() "<<endl;
      abort();
    }
    MPI_SchedulingInfo.Symbols[dataBlkInfo.symPos(sym_cnt)] = 1;
  }
  
  // Filling PMIs
  for(int rb_cnt=0;rb_cnt<dataBlkInfo.resourceBlocks.length();rb_cnt++)
  {
    if(dataBlkInfo.resourceBlocks(rb_cnt)>=300)
    {
      cout<<"Unsupported RB for MPI_encode() "<<endl;
      abort();
    }
    MPI_SchedulingInfo.ResourceBlocks[dataBlkInfo.resourceBlocks(rb_cnt)] = 1;
    if(dataBlkInfo.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_CL_SM_ || dataBlkInfo.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_CL_MU_MIMO_ || dataBlkInfo.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_CL_BF_
      || dataBlkInfo.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ || dataBlkInfo.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || dataBlkInfo.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_NR_CL_  || dataBlkInfo.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_COMP_|| (dataBlkInfo.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_OLMUMIMO_ && dataBlkInfo.isSemiOpenLoopScheme))
    {
      for(int txPort_cnt = 0; txPort_cnt < dataBlkInfo.precoderPerRB(rb_cnt).rows(); txPort_cnt++)
	for(int layer_cnt = 0; layer_cnt < dataBlkInfo.precoderPerRB(rb_cnt).cols(); layer_cnt++)
	  MPI_SchedulingInfo.PrecoderPerRB[dataBlkInfo.resourceBlocks(rb_cnt)][txPort_cnt][layer_cnt]=dataBlkInfo.precoderPerRB(rb_cnt)(txPort_cnt,layer_cnt);
    }
  }
  
  return(MPI_SchedulingInfo);
}

int MPI_decode(MPI_SchedulingInfo_S &MPI_SchedulingInfo,DataBlkInfo_S &dataBlkInfo, double &scTxPowIndBm, int &nuID)
{
  ivec RBG_b_r;
  RBG_b_r.set_length(270);
  for(int ii=0;ii<RBG_b_r.length();ii++){
    RBG_b_r(ii) = MPI_SchedulingInfo.ResourceBlocks[ii];
  }
  dataBlkInfo.resourceBlocks= find(RBG_b_r,1,true);
  
  ivec Sym_b_r;
  Sym_b_r.set_length(14);
  for(int ii=0;ii<Sym_b_r.length();ii++){
    Sym_b_r(ii) = MPI_SchedulingInfo.Symbols[ii];
  }
  dataBlkInfo.symPos=find(Sym_b_r,1,true);
  
  if(dataBlkInfo.resourceBlocks.length()==0 || dataBlkInfo.symPos.length()==0)
  {
    cout<<"MPI Decode error.."<<endl;abort();
  }
  
  
  dataBlkInfo.prbBundlingSize = MPI_SchedulingInfo.prbBundlingSize;
  dataBlkInfo.dataGenInfo.transmissionScheme = TransmissionScheme_E(MPI_SchedulingInfo.TxScheme);
  
  dataBlkInfo.waveform=MPI_SchedulingInfo.waveform;
  
  dataBlkInfo.dataGenInfo.nLayers = MPI_SchedulingInfo.nLayers;
  dataBlkInfo.dataGenInfo.layersInfo.nLayers=MPI_SchedulingInfo.nLayers;
  dataBlkInfo.dataGenInfo.layersInfo.dmrstype=MPI_SchedulingInfo.dmrstype;
  dataBlkInfo.dataGenInfo.layersInfo.maxLength=MPI_SchedulingInfo.maxLength;
  dataBlkInfo.dataGenInfo.layersInfo.nCDM=MPI_SchedulingInfo.nCDM;
  dataBlkInfo.dataGenInfo.layersInfo.CDM=MPI_SchedulingInfo.CDM;
  dataBlkInfo.dataGenInfo.layersInfo.CDMMap.set_length(MPI_SchedulingInfo.CDMMapLength);
  for(int cnt=0;cnt<MPI_SchedulingInfo.CDMMapLength;cnt++)
  {
    dataBlkInfo.dataGenInfo.layersInfo.CDMMap(cnt)=MPI_SchedulingInfo.CDMMap[cnt];
  }
  
  dataBlkInfo.dataGenInfo.nCodeWords = MPI_SchedulingInfo.nCodewords;
  dataBlkInfo.dataGenInfo.mcsIndices.set_length(dataBlkInfo.dataGenInfo.nCodeWords);
  dataBlkInfo.dataGenInfo.mcsIndices(0) = MPI_SchedulingInfo.MCS_1;
  if(dataBlkInfo.dataGenInfo.nCodeWords==2)
    dataBlkInfo.dataGenInfo.mcsIndices(1) = MPI_SchedulingInfo.MCS_2;
  
  int serverNodeID = MPI_SchedulingInfo.serverNodeID;
  scTxPowIndBm=MPI_SchedulingInfo.txPowIndBm;
  nuID=MPI_SchedulingInfo.nuID;
  
  if(dataBlkInfo.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_OLMUMIMO_)
    dataBlkInfo.isSemiOpenLoopScheme=MPI_SchedulingInfo.isSemiOpenloopScheme;
  
  if(dataBlkInfo.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_CL_SM_ || dataBlkInfo.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_CL_MU_MIMO_ || dataBlkInfo.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_CL_BF_
    || dataBlkInfo.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ || dataBlkInfo.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || dataBlkInfo.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_NR_CL_ || dataBlkInfo.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_COMP_|| (dataBlkInfo.dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_OLMUMIMO_ && dataBlkInfo.isSemiOpenLoopScheme))
  {
    dataBlkInfo.precoderPerRB.set_length(dataBlkInfo.resourceBlocks.length());
    for(int rb_cnt=0;rb_cnt<dataBlkInfo.resourceBlocks.length();rb_cnt++)
    {
      dataBlkInfo.precoderPerRB(rb_cnt).set_size(MPI_SchedulingInfo.nPorts,MPI_SchedulingInfo.nLayers);
      for(int txPort_cnt = 0; txPort_cnt < MPI_SchedulingInfo.nPorts; txPort_cnt++)
	for(int layer_cnt = 0; layer_cnt < MPI_SchedulingInfo.nLayers; layer_cnt++)
	  dataBlkInfo.precoderPerRB(rb_cnt)(txPort_cnt,layer_cnt)=MPI_SchedulingInfo.PrecoderPerRB[dataBlkInfo.resourceBlocks(rb_cnt)][txPort_cnt][layer_cnt];
    }
  }
  
  
  return(serverNodeID);
  
}

void initMPI(int *nTasks, int *currentTaskid_ptr, int argc, char** argv){
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, nTasks);
  MPI_Comm_rank(MPI_COMM_WORLD, currentTaskid_ptr);
  
  if(currentTaskid==0) {
    
    cout<<"\n\n#################################################################"<<endl;
    cout<<"[both:]Initializing Message Passing Interface with nTasks as "<<*nTasks<<endl;
    cout<<"#################################################################\n\n"<<endl;
    
  }
  
  
}


void loadMPIInfo(MPIInfo_S &mpiInfo,AssociationInfo_S &associationInfo){
  
  MPI_Barrier(MPI_COMM_WORLD);
  cout<<"Initializing MPI for nTasks : "<<nTasks<<" in task : "<<currentTaskid<<endl;
  mpiInfo.init(associationInfo.serverNodes.length(),associationInfo.serviceNodes.length());
  int offset=0;
  for(int srvr_cnt=0;srvr_cnt<associationInfo.serverNodes.length();srvr_cnt++)
  {
    mpiInfo.countsPerSrvr(srvr_cnt)=associationInfo.associatedServiceNodesPerServerNode(srvr_cnt).length();
    mpiInfo.offsetsPerSrvr(srvr_cnt)=offset;
    offset+=mpiInfo.countsPerSrvr(srvr_cnt);
  }
  offset=sum(mpiInfo.countsPerSrvr(mpiInfo.srvrIndxListPerTaskid(0)));
  mpiInfo.countsPerTask[0]=0;mpiInfo.offsetsPerTask[0]=0;mpiInfo.indexShiftPerTask(0)=0;
  for(int task=1;task<nTasks;task++)
  {
    mpiInfo.countsPerTask[task]=sum(mpiInfo.countsPerSrvr(mpiInfo.srvrIndxListPerTaskid(task)))*sizeof(MPI_SchedulingInfo_S);
    mpiInfo.offsetsPerTask[task]=offset*sizeof(MPI_SchedulingInfo_S);
    mpiInfo.indexShiftPerTask(task)=offset;
    offset+=sum(mpiInfo.countsPerSrvr(mpiInfo.srvrIndxListPerTaskid(task)));
  }
  
}


void uploadMPIDataNew(MPIInfo_S &mpiInfo,ivec serverNodes,AssociationInfo_S &associationInfo,Array<SchedulerOutput_S> &schedulerOutput){
  
  ivec srvcNodeIndx;
  ivec srvrIndices=mpiInfo.srvrIndxListPerTaskid(currentTaskid);
  for(int srvr_cnt=0;srvr_cnt<srvrIndices.length();srvr_cnt++)
  {
      for(int schToTx_cnt=0;schToTx_cnt<schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo.length();schToTx_cnt++)
    {
        if(schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTx_cnt).txType==_TRANSMISSION_TYPE_DL_)//! Downlink
      {
	if(find(serverNodes,schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTx_cnt).txNodeID)!=-1)
	{
	  srvcNodeIndx=find(associationInfo.associatedServiceNodesPerServerNode(srvrIndices(srvr_cnt)),schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTx_cnt).rxNodeIDs);
	  for(int blk_cnt=0;blk_cnt<srvcNodeIndx.length();blk_cnt++)
	  {
	    mpiInfo.schInfoField[mpiInfo.offsetsPerSrvr(srvrIndices(srvr_cnt))+srvcNodeIndx(blk_cnt)]=MPI_encode(schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTx_cnt).dataBlkInfo(blk_cnt),schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTx_cnt).txNodeID,schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTx_cnt).txSubcarrierPowerInDBmPerAntenna,schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTx_cnt).txNUID,schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTx_cnt).carrierFreqInMHz,schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTx_cnt).ttiCount,true);
      }
	}
      }
    }
    // else Uplink
    for(int schToRx_cnt=0;schToRx_cnt<schedulerOutput(srvrIndices(srvr_cnt)).schedulerToReceiverInfo.length();schToRx_cnt++)
    {
        if(schedulerOutput(srvrIndices(srvr_cnt)).schedulerToReceiverInfo(schToRx_cnt).txType==_TRANSMISSION_TYPE_UL_)//! Uplink
      {
	if(find(serverNodes,schedulerOutput(srvrIndices(srvr_cnt)).schedulerToReceiverInfo(schToRx_cnt).rxNodeID)!=-1)
	{
	  srvcNodeIndx=find(associationInfo.associatedServiceNodesPerServerNode(srvrIndices(srvr_cnt)),schedulerOutput(srvrIndices(srvr_cnt)).schedulerToReceiverInfo(schToRx_cnt).txNodeIDs);
	  for(int blk_cnt=0;blk_cnt<srvcNodeIndx.length();blk_cnt++)
	  {
	    mpiInfo.schInfoField[mpiInfo.offsetsPerSrvr(srvrIndices(srvr_cnt))+srvcNodeIndx(blk_cnt)]=MPI_encode(schedulerOutput(srvrIndices(srvr_cnt)).schedulerToReceiverInfo(schToRx_cnt).dataBlkInfo(blk_cnt),schedulerOutput(srvrIndices(srvr_cnt)).schedulerToReceiverInfo(schToRx_cnt).rxNodeID,schedulerOutput(srvrIndices(srvr_cnt)).schedulerToReceiverInfo(schToRx_cnt).txSubcarrierPowerInDBmPerAntenna(blk_cnt),schedulerOutput(srvrIndices(srvr_cnt)).schedulerToReceiverInfo(schToRx_cnt).rxNUID,schedulerOutput(srvrIndices(srvr_cnt)).schedulerToReceiverInfo(schToRx_cnt).carrierFreqInMHz,schedulerOutput(srvrIndices(srvr_cnt)).schedulerToReceiverInfo(schToRx_cnt).ttiCount,false);
	  }
	}
      }
    }
    
  }
}


void clearMPIData(MPIInfo_S &mpiInfo){
  
  int totatCount=sum(mpiInfo.countsPerSrvr);
  MPI_SchedulingInfo_S emptyInfo;
  for(int field_cnt=0;field_cnt<totatCount;field_cnt++)
    mpiInfo.schInfoField[field_cnt]=emptyInfo;
}

void MPIBCastAssociationInfo(AssociationInfo_S &associationInfo){
  
    MPI_Barrier(MPI_COMM_WORLD);
  MPI_UEAssociationInfo_S *ueAssocationInfo;
  ueAssocationInfo=new MPI_UEAssociationInfo_S[associationInfo.serviceNodes.length()];
  
  if(currentTaskid==0)
  {
      cout<<"Broadcasting associationInfo to all processes..."<<endl;  
    for(int srvc_cnt=0;srvc_cnt<associationInfo.serviceNodes.length();srvc_cnt++)
    {
	ueAssocationInfo[srvc_cnt].bsID=associationInfo.associatedServerNodesForEachServiceNode(srvc_cnt);
	ueAssocationInfo[srvc_cnt].bsBeamID=associationInfo.associatedBeamPairLinkforEachServiceNode(srvc_cnt).srvrBeamID;
	ueAssocationInfo[srvc_cnt].ueBeamID=associationInfo.associatedBeamPairLinkforEachServiceNode(srvc_cnt).srvcBeamID;
    ueAssocationInfo[srvc_cnt].rspIndBm=associationInfo.associatedBeamPairLinkforEachServiceNode(srvc_cnt).rspIndBm;
	ueAssocationInfo[srvc_cnt].rsrpIndBm=associationInfo.associatedBeamPairLinkforEachServiceNode(srvc_cnt).rsrpIndBm;
	ueAssocationInfo[srvc_cnt].geometrySinrInDB = associationInfo.geometrySINRindBperServiceNode(srvc_cnt);
    }
  }
  MPI_Bcast(ueAssocationInfo,sizeof(MPI_UEAssociationInfo_S)*associationInfo.serviceNodes.length(),MPI_BYTE,0,MPI_COMM_WORLD);
  if(currentTaskid!=0)
  {
    associationInfo.associatedBeamPairLinkforEachServiceNode.set_length(associationInfo.serviceNodes.length());
    associationInfo.geometrySINRindBperServiceNode.set_length(associationInfo.serviceNodes.length());
    for(int srvc_cnt=0;srvc_cnt<associationInfo.serviceNodes.length();srvc_cnt++)
    {
      associationInfo.associatedServerNodesForEachServiceNode(srvc_cnt)=ueAssocationInfo[srvc_cnt].bsID;
      associationInfo.associatedBeamPairLinkforEachServiceNode(srvc_cnt).srvrBeamID=ueAssocationInfo[srvc_cnt].bsBeamID;
      associationInfo.associatedBeamPairLinkforEachServiceNode(srvc_cnt).srvcBeamID=ueAssocationInfo[srvc_cnt].ueBeamID;
      associationInfo.associatedBeamPairLinkforEachServiceNode(srvc_cnt).rspIndBm=ueAssocationInfo[srvc_cnt].rspIndBm;
      associationInfo.associatedBeamPairLinkforEachServiceNode(srvc_cnt).rsrpIndBm=ueAssocationInfo[srvc_cnt].rsrpIndBm;
      associationInfo.geometrySINRindBperServiceNode(srvc_cnt) = ueAssocationInfo[srvc_cnt].geometrySinrInDB;
    }
  }
}

void MPIExchangeAssociationInfo(AssociationInfo_S &associationInfo)
{
  MPI_Barrier(MPI_COMM_WORLD);
  if(currentTaskid==0)
    cout<<"Uploading Association Info for MPI.."<<endl;
  
  MPI_AssociationInfo_S mpiAssociationInfo;
  mpiAssociationInfo.init(associationInfo.serviceNodesPerTask,associationInfo.serviceNodes.length());
  
  for(int srvc_cnt=0;srvc_cnt<associationInfo.serviceNodesPerTask(currentTaskid).length();srvc_cnt++)
  {
    int srvcIndx = find(associationInfo.serviceNodes,associationInfo.serviceNodesPerTask(currentTaskid)(srvc_cnt));
    mpiAssociationInfo.ueAssociationInfo[srvcIndx].ueID = associationInfo.serviceNodes(srvcIndx);
    mpiAssociationInfo.ueAssociationInfo[srvcIndx].bsID=associationInfo.associatedServerNodesForEachServiceNode(srvcIndx);
    mpiAssociationInfo.ueAssociationInfo[srvcIndx].bsBeamID=associationInfo.associatedBeamPairLinkforEachServiceNode(srvcIndx).srvrBeamID;
    mpiAssociationInfo.ueAssociationInfo[srvcIndx].ueBeamID=associationInfo.associatedBeamPairLinkforEachServiceNode(srvcIndx).srvcBeamID;
    mpiAssociationInfo.ueAssociationInfo[srvcIndx].rspIndBm=associationInfo.associatedBeamPairLinkforEachServiceNode(srvcIndx).rspIndBm;
    mpiAssociationInfo.ueAssociationInfo[srvcIndx].rsrpIndBm=associationInfo.associatedBeamPairLinkforEachServiceNode(srvcIndx).rsrpIndBm;
    mpiAssociationInfo.ueAssociationInfo[srvcIndx].geometrySinrInDB=associationInfo.geometrySINRindBperServiceNode(srvcIndx);
  }
  
  MPI_Gatherv(mpiAssociationInfo.ueAssociationInfo+mpiAssociationInfo.indexShiftPerTask(currentTaskid),mpiAssociationInfo.countsPerTask[currentTaskid],MPI_BYTE,mpiAssociationInfo.ueAssociationInfo,mpiAssociationInfo.countsPerTask,mpiAssociationInfo.offsetsPerTask,MPI_BYTE,0,MPI_COMM_WORLD);
  MPI_Bcast(mpiAssociationInfo.ueAssociationInfo,associationInfo.serviceNodes.length()*sizeof(MPI_UEAssociationInfo_S),MPI_BYTE,0,MPI_COMM_WORLD);
  
    
  //if(currentTaskid==0){
  cout<<"Updating Association Info For Process..."<<currentTaskid<<endl;
  associationInfo.associatedBeamPairLinkforEachServiceNode.set_size(associationInfo.serviceNodes.length());
  associationInfo.associatedServerNodesForEachServiceNode.set_size(associationInfo.serviceNodes.length());
  associationInfo.geometrySINRindBperServiceNode.set_size(associationInfo.serviceNodes.length());
  
  for(int cnt=0;cnt<associationInfo.serviceNodes.length();cnt++)
  {
    int srvcNodeIndx = find(associationInfo.serviceNodes,mpiAssociationInfo.ueAssociationInfo[cnt].ueID);
    if(srvcNodeIndx==-1)
    {
      cout<<"Unknown UE id.."<<mpiAssociationInfo.ueAssociationInfo[cnt].ueID<<endl;abort();
    }
    associationInfo.associatedServerNodesForEachServiceNode(srvcNodeIndx)=mpiAssociationInfo.ueAssociationInfo[cnt].bsID;
    associationInfo.associatedBeamPairLinkforEachServiceNode(srvcNodeIndx).srvrBeamID=mpiAssociationInfo.ueAssociationInfo[cnt].bsBeamID;
    associationInfo.associatedBeamPairLinkforEachServiceNode(srvcNodeIndx).srvcBeamID=mpiAssociationInfo.ueAssociationInfo[cnt].ueBeamID;
    associationInfo.associatedBeamPairLinkforEachServiceNode(srvcNodeIndx).rspIndBm=mpiAssociationInfo.ueAssociationInfo[cnt].rspIndBm;
    associationInfo.associatedBeamPairLinkforEachServiceNode(srvcNodeIndx).rsrpIndBm=mpiAssociationInfo.ueAssociationInfo[cnt].rsrpIndBm;
    associationInfo.geometrySINRindBperServiceNode(srvcNodeIndx) = mpiAssociationInfo.ueAssociationInfo[cnt].geometrySinrInDB;
  }
  mpiAssociationInfo.free();
}

void MPIExchangeLinkInfo(McellSystem& mySys)
{
  if(currentTaskid==0)
      cout<<"Exchanging LinkInfo across Processes..."<<endl;
  
  ivec serviceNodes = mySys.getNodes(_NODECATEGORY_SERVICENODE_);
  ivec serverNodes = mySys.getNodes(_NODECATEGORY_SERVERNODE_);
  
  Array<ivec> serviceNodesPerTask = mySys.serviceNodesPerTask;
  for(int i=serviceNodesPerTask.length();i<nTasks;i++)
      append(serviceNodesPerTask,zeros_i(0));
  
  MPI_LinkInfo_S mpiLinkInfo;
  mpiLinkInfo.init(serviceNodesPerTask,serverNodes.length(),mySys.getLinkCount());
  
//   if(mySys.serviceNodesPerTask.length()>currentTaskid)  //Collecting data from all processes.
  {
      for(int srvc_cnt=0;srvc_cnt<serviceNodesPerTask(currentTaskid).length();srvc_cnt++)
      {
            for(int srvr_cnt=0;srvr_cnt<serverNodes.length();srvr_cnt++)
            {
                int srvcIndx = find(serviceNodes,serviceNodesPerTask(currentTaskid)(srvc_cnt));
                int srvrIndx = srvr_cnt;
                int linkIndx = 2*(srvcIndx*serverNodes.length() + srvrIndx);
                
                NodeLinkInfo_S tempLinkInfo = mySys.getLinkInfo(serverNodes(srvrIndx),serviceNodes(srvcIndx));
                
                mpiLinkInfo.linkParam[linkIndx].txNodeID = serverNodes(srvrIndx);
                mpiLinkInfo.linkParam[linkIndx].rxNodeID = serviceNodes(srvcIndx);
                mpiLinkInfo.linkParam[linkIndx].load(tempLinkInfo);
                
                linkIndx++;
                
                tempLinkInfo = mySys.getLinkInfo(serviceNodes(srvcIndx),serverNodes(srvrIndx));
                
                mpiLinkInfo.linkParam[linkIndx].rxNodeID = serverNodes(srvrIndx);
                mpiLinkInfo.linkParam[linkIndx].txNodeID = serviceNodes(srvcIndx);
                mpiLinkInfo.linkParam[linkIndx].load(tempLinkInfo);
            }
      }
  }
  
  MPI_Gatherv(mpiLinkInfo.linkParam+mpiLinkInfo.indexShiftPerTask(currentTaskid),mpiLinkInfo.countsPerTask[currentTaskid],MPI_BYTE,mpiLinkInfo.linkParam,mpiLinkInfo.countsPerTask,mpiLinkInfo.offsetsPerTask,MPI_BYTE,0,MPI_COMM_WORLD);
  MPI_Bcast(mpiLinkInfo.linkParam,mySys.getLinkCount()*sizeof(MPI_LinkParam_S),MPI_BYTE,0,MPI_COMM_WORLD);
  
//   if(currentTaskid==0)
  {
      for(int link_cnt=0;link_cnt<mySys.getLinkCount();link_cnt++)
      {
          MPI_LinkParam_S linkParam = mpiLinkInfo.linkParam[link_cnt];
          
          NodeLinkInfo_S tempLinkInfo;
          tempLinkInfo.load(linkParam.linkInfo);
        
          tempLinkInfo.id = linkParam.txNodeID;
          tempLinkInfo.txAntennaGain = linkParam.linkInfo.txAntennaGain;
          tempLinkInfo.rxAntennaGain = linkParam.linkInfo.rxAntennaGain;
          mySys.aNodes(linkParam.rxNodeID).setLinkInfo(tempLinkInfo);
      }
  }
  mpiLinkInfo.free();
}

void MPIBCastLinkInfo(McellSystem& mySys)
{
  if(currentTaskid==0)
      cout<<"Broadcasting LinkInfo to all Processes..."<<endl;
  
  ivec serviceNodes = mySys.getNodes(_NODECATEGORY_SERVICENODE_);
  ivec serverNodes = mySys.getNodes(_NODECATEGORY_SERVERNODE_);
  
  MPI_LinkInfo_S mpiLinkInfo;
  mpiLinkInfo.linkParam = new MPI_LinkParam_S[mySys.getLinkCount()];
  
  if(currentTaskid==0)  //Collecting data from all processes.
  {
      for(int srvc_cnt=0;srvc_cnt<serviceNodes.length();srvc_cnt++)
      {
            for(int srvr_cnt=0;srvr_cnt<serverNodes.length();srvr_cnt++)
            {
                int linkIndx = 2*(srvc_cnt*serverNodes.length() + srvr_cnt);
                
                NodeLinkInfo_S tempLinkInfo = mySys.getLinkInfo(serverNodes(srvr_cnt),serviceNodes(srvc_cnt));
                
                mpiLinkInfo.linkParam[linkIndx].txNodeID = serverNodes(srvr_cnt);
                mpiLinkInfo.linkParam[linkIndx].rxNodeID = serviceNodes(srvc_cnt);
                mpiLinkInfo.linkParam[linkIndx].load(tempLinkInfo);
                
                linkIndx++;
                
                tempLinkInfo = mySys.getLinkInfo(serviceNodes(srvc_cnt),serverNodes(srvr_cnt));
                
                mpiLinkInfo.linkParam[linkIndx].rxNodeID = serverNodes(srvr_cnt);
                mpiLinkInfo.linkParam[linkIndx].txNodeID = serviceNodes(srvc_cnt);
                mpiLinkInfo.linkParam[linkIndx].load(tempLinkInfo);
            }
      }
  }
  
  MPI_Bcast(mpiLinkInfo.linkParam,mySys.getLinkCount()*sizeof(MPI_LinkParam_S),MPI_BYTE,0,MPI_COMM_WORLD);
  
  if(currentTaskid!=0)
  {
      for(int link_cnt=0;link_cnt<mySys.getLinkCount();link_cnt++)
      {
          MPI_LinkParam_S linkParam = mpiLinkInfo.linkParam[link_cnt];
          
          NodeLinkInfo_S tempLinkInfo;
          tempLinkInfo.load(linkParam.linkInfo);
        
          tempLinkInfo.id = linkParam.txNodeID;
          tempLinkInfo.txAntennaGain = linkParam.linkInfo.txAntennaGain;
          tempLinkInfo.rxAntennaGain = linkParam.linkInfo.rxAntennaGain;
          mySys.aNodes(linkParam.rxNodeID).setLinkInfo(tempLinkInfo);
      }
  }
  delete[] mpiLinkInfo.linkParam;
}

void MPIGatherPerformanceInfo(PerformanceInfo_S& performanceInfo,McellSystem* mySysPtr,AssociationInfo_S* associationInfoPtr)
{
    if(currentTaskid==0)
        cout<<"Gathering Performance Info For MPI"<<endl;
    
    MPI_Barrier(MPI_COMM_WORLD);
    Array<string> nodeTypes=mySysPtr->getNodeTypes();
    ivec nodeCounts=mySysPtr->getNodeCount();
    Array<NodeCategory_E> nodeCategories = mySysPtr->getNodeCategory();
    
    ivec nodesPerType, nodeIndices,numberofServerNodes = zeros_i(nodeTypes.length());    
    
    MPIResultInfo_S MPIResultInfo;
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPIResultInfo.init(nTasks);
    
    for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
    {
        if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
        {
          MPIResultInfo.avgResults[currentTaskid].avgSEPerServerNodeType[nodeType_cnt]=performanceInfo.avgSEPerServerNodeType(nodeType_cnt);
          MPIResultInfo.avgResults[currentTaskid].avgThroughputPerServerNodeType[nodeType_cnt]=performanceInfo.avgThroughputPerServerNodeType(nodeType_cnt);
          MPIResultInfo.avgResults[currentTaskid].numberofServerNodes[nodeType_cnt]=performanceInfo.numberofServerNodes(nodeType_cnt);
          MPIResultInfo.avgResults[currentTaskid].avgThroughputPerServiceNodeUnderServerNodeType[nodeType_cnt]=performanceInfo.avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt);
          MPIResultInfo.avgResults[currentTaskid].nScheduledRBsPerServerNodeType[nodeType_cnt] = performanceInfo.totalScheduledRBsPerServerNodeType(nodeType_cnt);
        }
    }
      
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gatherv(MPIResultInfo.avgResults+MPIResultInfo.indexShiftPerTask[currentTaskid],MPIResultInfo.countsPerTask[currentTaskid],MPI_BYTE,MPIResultInfo.avgResults,MPIResultInfo.countsPerTask,MPIResultInfo.offsetsPerTask,MPI_BYTE,0,MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    
    if(currentTaskid==0)
    {
        vec avgSEPerServerNodeType_temp=zeros(nTasks),avgThroughputPerServerNodeType_temp=zeros(nTasks),avgThroughputPerServiceNodeUnderServerNodeType_temp=zeros(nTasks);
        ivec numberofServerNodes_temp=zeros_i(nTasks);
        for(int nodeType_cnt=0;nodeType_cnt<nodeTypes.length();nodeType_cnt++)
        {
          if(nodeCategories(nodeType_cnt)==_NODECATEGORY_SERVERNODE_ && nodeCounts(nodeType_cnt)>0)
          {
            nodesPerType=mySysPtr->getActiveNodes(nodeTypes(nodeType_cnt));
            for(int task=0;task<nTasks;task++)
            {
              numberofServerNodes_temp(task)=MPIResultInfo.avgResults[task].numberofServerNodes[nodeType_cnt];
              avgSEPerServerNodeType_temp(task)=(MPIResultInfo.avgResults[task].avgSEPerServerNodeType[nodeType_cnt])*numberofServerNodes_temp(task);
              avgThroughputPerServerNodeType_temp(task)=MPIResultInfo.avgResults[task].avgThroughputPerServerNodeType[nodeType_cnt]*numberofServerNodes_temp(task);
              avgThroughputPerServiceNodeUnderServerNodeType_temp(task) = MPIResultInfo.avgResults[task].avgThroughputPerServiceNodeUnderServerNodeType[nodeType_cnt];
              performanceInfo.totalScheduledRBsForSystemPerServerNodeType(nodeType_cnt) += MPIResultInfo.avgResults[task].nScheduledRBsPerServerNodeType[nodeType_cnt];
            }
            nodeIndices = associationInfoPtr->serverNodes;
            int srvcNodeUnder=0;
            for(int node_cnt=0;node_cnt<nodeIndices.length();node_cnt++)
              srvcNodeUnder+=associationInfoPtr->associatedServiceNodesPerServerNode(nodeIndices(node_cnt)).length();
            if(sum(numberofServerNodes_temp))
            {
                performanceInfo.avgSEPerServerNodeType(nodeType_cnt)=sum(avgSEPerServerNodeType_temp)/sum(numberofServerNodes_temp);
                performanceInfo.avgThroughputPerServerNodeType(nodeType_cnt)=sum(avgThroughputPerServerNodeType_temp)/sum(numberofServerNodes_temp);
            }
            performanceInfo.avgThroughputPerServiceNodeUnderServerNodeType(nodeType_cnt)=sum(avgThroughputPerServiceNodeUnderServerNodeType_temp)/srvcNodeUnder;
          }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

