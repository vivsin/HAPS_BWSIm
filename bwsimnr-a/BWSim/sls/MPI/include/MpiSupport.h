/*************************************************************************
*
* CEWiT CONFIDENTIAL
* __________________
*
* All Rights Reserved © 2014 CEWiT, India
*
\ NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
* and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
* Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
* express, printed and signed license for use is strictly forbidden.
*/


#ifndef MPI_SUPPORT_H
#define MPI_SUPPORT_H

#include <lib/Frozen/mcell/include/System.h>
#include <lib/L1/PHY/include/FrameScheduler.h>


#ifdef USING_MPI

#include <mpi.h>

#endif


struct MPI_SchedulingInfo_S
{
  bool isScheduled;
  bool ResourceBlocks[270];		// Bitmap over RBs (Max size 270 is hardcoded)
  bool Symbols[14];// Bitmap over Symbols (Max size 100 is hardcoded)
  unsigned TxScheme 		: 4;	// Transmission Scheme
  unsigned nPorts		: 8;	// Number of Ports
  unsigned nLayers 		: 4;	// Number of Layers
  unsigned nCodewords 		: 2;	// Number of Codewords
  unsigned MCS_1 		: 5;	// MCS for CW1
  unsigned MCS_2 		: 5;	// MCS for CW2
  int serverNodeID;
  int nuID;
  bool isSemiOpenloopScheme;
  complex< double > PrecoderPerRB[270][32][12]; // Precoder max size is fixed as 32x12
  double txPowIndBm;
  double carrierFreqInMHz;
  int nSlots;
  int prbBundlingSize;
  int ttiCount;
  bool isDownlink;
    
  //HarqInfo
  int harqProcessID;
  int cwdIndxTB1;
  int cwdIndxTB2;
  int rvIndxTB1;
  int rvIndxTB2;
  bin ndiTB1;            
  bin ndiTB2;            
  bin isNewTB1; 
  bin isNewTB2; 
  int sizeTB1;
  int sizeTB2;
  
  //layerInfo
  int CDMMapLength;
  bin CDMMap[6];
  int nCDM;
  int CDM;
  int dmrstype;
  int maxLength;
  
  Waveform_E waveform;

  MPI_SchedulingInfo_S(){

    isScheduled=false;
    isSemiOpenloopScheme=false;
    harqProcessID=-1;
    cwdIndxTB1=-1;
    cwdIndxTB2=-1;
    rvIndxTB1=-1;
    rvIndxTB2=-1;
    sizeTB1=-1;
    sizeTB2=-1;
    
    for(int i=0;i<270;i++)
    {
      ResourceBlocks[i]=false;
      complex< double > temp=complex< double >(0,0);
      for(int j = 0; j < 32; j++)
	for(int k = 0; k < 12; k++)
	  PrecoderPerRB[i][j][k] = temp;
    }
    nLayers=0;
    dmrstype=-1;
    maxLength=-1;
    CDM=-1;nCDM=-1;
  }

};



struct MPIInfo_S
{
  ivec offsetsPerSrvr;
  ivec countsPerSrvr;
  int *offsetsPerTask;
  ivec indexShiftPerTask;
  int *countsPerTask;
  MPI_SchedulingInfo_S *schInfoField;
//   AssociationInfo_S *associationInfo;
  Array<ivec> srvrIndxListPerTaskid;

  void init(int srvrCount,int srvcNodesCount){

    offsetsPerTask=new int[nTasks];
    countsPerTask=new int[nTasks];
    indexShiftPerTask.set_length(nTasks);
    offsetsPerSrvr.set_length(srvrCount);
    countsPerSrvr.set_length(srvrCount);
    schInfoField=new MPI_SchedulingInfo_S[srvcNodesCount];
   
    srvrIndxListPerTaskid.set_length(nTasks);
    
    ivec srvrCountPerTaskid=zeros_i(nTasks);
    for(int cu_cnt=0;cu_cnt<srvrCount;cu_cnt++)
      srvrCountPerTaskid(mod(cu_cnt,nTasks))++;

    int cu_num=0;
    for(int task=0;task<nTasks;task++)
    {
      srvrIndxListPerTaskid(task).set_size(srvrCountPerTaskid(task));
      for(int srvr_indx=0;srvr_indx<srvrCountPerTaskid(task);srvr_indx++)
	srvrIndxListPerTaskid(task).set(srvr_indx,cu_num++);
    }
  }
  void free(){

    delete[] offsetsPerTask;
    delete[] countsPerTask;
    delete[] schInfoField;
  }

};

MPI_SchedulingInfo_S MPI_encode(DataBlkInfo_S& dataBlkInfo, int serverNodeID, double scTxPowIndBm, int nuID, double carrierFreqInMHz, int ttiCount, bool isDownlink);
int MPI_decode(MPI_SchedulingInfo_S& MPI_SchedulingInfo, DataBlkInfo_S& dataBlkInfo, double& scTxPowIndBm, int& nuID);
void initMPI(int *nTasks, int *taskid, int argc, char** argv);
void loadMPIInfo(MPIInfo_S &mpiInfo,AssociationInfo_S &associationInfo);
void uploadMPIDataNew(MPIInfo_S& mpiInfo, ivec serverNodes, AssociationInfo_S& associationInfo, Array< SchedulerOutput_S >& schedulerOutput);
void clearMPIData(MPIInfo_S &mpiInfo);

struct MPI_UEAssociationInfo_S
{
  int bsID;
  int ueID;
  int bsBeamID;
  int ueBeamID;
  double rspIndBm;
  double rsrpIndBm;
  double geometrySinrInDB;
  
  MPI_UEAssociationInfo_S()
  {
    bsID=-1;ueID=-1;bsBeamID=-1;ueBeamID=-1;
    rspIndBm=0.0;rsrpIndBm=0.0;geometrySinrInDB=0.0;
  }
};

struct MPI_AssociationInfo_S
{
  int *offsetsPerTask;
  int *countsPerTask;
  ivec indexShiftPerTask;
  
  MPI_UEAssociationInfo_S *ueAssociationInfo;
  
  void init(Array<ivec> srvcNodesPerTask, int srvcNodesCount){
    
    offsetsPerTask=new int[nTasks];
    countsPerTask=new int[nTasks];
    indexShiftPerTask.set_length(nTasks);
    ueAssociationInfo=new MPI_UEAssociationInfo_S[srvcNodesCount];
    
    int offset=srvcNodesPerTask(0).length();

    countsPerTask[0]=0;offsetsPerTask[0]=0;indexShiftPerTask(0)=0;
    for(int task=1;task<nTasks;task++)
    {
      countsPerTask[task]=srvcNodesPerTask(task).length()*sizeof(MPI_UEAssociationInfo_S);
      offsetsPerTask[task]=offset*sizeof(MPI_UEAssociationInfo_S);
      indexShiftPerTask(task)=offset;
      offset+=srvcNodesPerTask(task).length();
    }
    
  }
  void free(){
    
    delete[] offsetsPerTask;
    delete[] countsPerTask;
    delete[] ueAssociationInfo;
  }
  
};

struct MPI_LinkParam_S
{
  int txNodeID;
  int rxNodeID;
  NodeLinkInfo_S linkInfo;
  
  MPI_LinkParam_S()
  {
    txNodeID=-1;rxNodeID=-1;
  }
  void load(NodeLinkInfo_S& templinkInfo)
  {
      linkInfo.load(templinkInfo);
      linkInfo.txAntennaGain = templinkInfo.txAntennaGain;
      linkInfo.rxAntennaGain = templinkInfo.rxAntennaGain;
  }
};

struct MPI_LinkInfo_S
{
  int *offsetsPerTask;
  int *countsPerTask;
  ivec indexShiftPerTask;
  
  MPI_LinkParam_S *linkParam;
  
  void init(Array<ivec> srvcNodesPerTask,int serverNodes,int linkCount){
     
    offsetsPerTask=new int[nTasks];
    countsPerTask=new int[nTasks];
    indexShiftPerTask.set_length(nTasks);
    linkParam=new MPI_LinkParam_S[linkCount];
    
    int offset=srvcNodesPerTask(0).length()*serverNodes*2;

    countsPerTask[0]=0;offsetsPerTask[0]=0;indexShiftPerTask(0)=0;
    for(int task=1;task<nTasks;task++)
    {
      int nLinksPerTask = srvcNodesPerTask(task).length()*serverNodes*2;
      countsPerTask[task]=nLinksPerTask*sizeof(MPI_LinkParam_S);
      offsetsPerTask[task]=offset*sizeof(MPI_LinkParam_S);
      indexShiftPerTask(task)=offset;
      offset+=nLinksPerTask;
    }
    
  }
  void free(){
    
    delete[] offsetsPerTask;
    delete[] countsPerTask;
    delete[] linkParam;
  }
  
};

void MPIBCastAssociationInfo(AssociationInfo_S &associationInfo);
void MPIExchangeAssociationInfo(AssociationInfo_S &associationInfo);
void MPIExchangeLinkInfo(McellSystem& mySys);
void MPIBCastLinkInfo(McellSystem& mySys);

struct AvgResults_S
{
  double avgThroughputPerServerNodeType[10];
  double avgSEPerServerNodeType[10];
  double avgThroughputPerServiceNodeUnderServerNodeType[10];
  int numberofServerNodes[10];
  int nScheduledRBsPerServerNodeType[10];
};

struct MPIResultInfo_S
{
  int *offsetsPerTask;
  int *countsPerTask;
  ivec indexShiftPerTask;
  AvgResults_S *avgResults;
  
  void init(int nTasks){
    
    offsetsPerTask=new int[nTasks];
    countsPerTask=new int[nTasks];
    indexShiftPerTask.set_length(nTasks);
    avgResults=new AvgResults_S[nTasks];
    
    int offset=1;
    
    countsPerTask[0]=0;offsetsPerTask[0]=0;indexShiftPerTask(0)=0;
    for(int task=1;task<nTasks;task++)
    {
      countsPerTask[task]=sizeof(AvgResults_S);
      offsetsPerTask[task]=offset*sizeof(AvgResults_S);
      indexShiftPerTask(task)=offset;
      offset+=1;
    }
    
  }
  void free(){
    delete[] offsetsPerTask;
    delete[] countsPerTask;
    delete[] avgResults;
  }
  
};

void MPIGatherPerformanceInfo(PerformanceInfo_S& performanceInfo,McellSystem* mySysPtr,AssociationInfo_S* associationInfoPtr);


#endif
