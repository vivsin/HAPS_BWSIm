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

#ifndef _SAE_BWSim_H_
#define _SAE_BWSim_H_

#include "../../Mcell/include/SupportingFunctions.h"

#include <lib/L1/PHY/include/FrameHandler.h>
#include <lib/L1/PHY/include/ChannelCoder.h>

#include <lib/L1/PHY/include/LayerMapper.h>
#include <lib/L1/PHY/include/ResourceMapper.h>
#include <lib/L1/PHY/include/Precoder.h>
#include <lib/L1/PHY/include/Equalizer.h>
#include <lib/L1/PHY/include/ReferenceSignalGenerator.h>
#include <lib/L1/PHY/include/PowerControl.h>
#include <lib/L1/PHY/include/SupportingFunctions.h>
#include <lib/L1/PHY/include/MultipleCSI.h>
#include <lib/L1/PHY/include/AWGNTables.h>
#include <lib/Frozen/mcell/include/System.h>
#include <lib/L1/PHY/include/SupportingFunctions.h>
#include <lib/L2/MAC/include/Old_Scheduler.h>
#include <lib/L2/MAC/include/HARQManager.h>
#include <lib/L2/MAC/include/Old_OuterLoopLA.h>
#include <lib/L2/MAC/include/Others/cRadioBearer.h>


#ifdef USING_MPI
#include "../../MPI/include/MpiSupport.h"
#endif


struct UlDlServerNodes_S{
  
public:
  ivec dlServerNodes;
  ivec ulServerNodes;
  UlDlServerNodes_S()
  {
    dlServerNodes.set_size(0);
    ulServerNodes.set_size(0);
  }
  void clear(){
    
    dlServerNodes.set_size(0);
    ulServerNodes.set_size(0);
  }
  
};



struct TxFrameInfo_S{

  int nodeID;
  int ttiCount;
  int idCell;
  bool isTransmitting;
  int nTxAntenna;
  NUParams_S nuParams;
  Array<cmat> txFrame;
  double txSubcarrierPowerInDBmPerAntenna;
  int txBeamID;
  
#ifdef USING_SRS
  bool isSRSEnabled;
  SRSSequence srsSequence;
#endif  
  
  bool isTimeDomainSubframeAvailable;
  Array <cvec> timeDomainSubframe;
  
  TxFrameInfo_S(){

    nodeID=-1;
    txFrame.set_size(0);
    isTransmitting=false;
    
#ifdef USING_SRS
    isSRSEnabled=false;
#endif
    
    isTimeDomainSubframeAvailable=false;
    timeDomainSubframe.set_size(0);
  }
  void clear(){
    
    txFrame.set_length(0);
    timeDomainSubframe.set_length(0);
  };
};


struct LinkSimulationInfo_S{

  bool isEnabled;
  vec snrRangeInDB;
  bool enableFEC;
  ModulationScheme_E modulationScheme;
  int cqiIndex;

  LinkSimulationInfo_S(){

    isEnabled=false;
    snrRangeInDB.set_length(0);
    enableFEC=true;
    modulationScheme=_MODULATION_SCHEME_UNINIT_;
    cqiIndex=1;
  }
};

struct MobilityConfig_S{

  MobilityType_E mobilityType;
  bool changeDirectionOnHandoff;
  double refreshDistanceInMetres;

  MobilityConfig_S(){

    mobilityType=_MOBILITY_TYPE_UNINIT_;
    changeDirectionOnHandoff=false;
    refreshDistanceInMetres=-1;
  }
};

struct RandomLoadInfo_S{

  bool enableRandomLoad;
  double loadPerServerNode;

  RandomLoadInfo_S(){

    enableRandomLoad=false;
    loadPerServerNode=-1;
  }
};

struct ReceptionModel_S{

  bool enableRestrictedRecepition;
  ivec serverNodesWithActualReception;

  ReceptionModel_S(){

    enableRestrictedRecepition=false;
    serverNodesWithActualReception.set_length(0);
  }
};


class SystemInfo_S{
public:
  Array<vec> FFR_RBGScaling;
  ivec nuIDs;
  Array<NUConfig_S> nuConfig;
 
  InitialAssociationMetric initialAssociationMetric;
  bool isWithPicos;
  double picoBias;
  bool enableBeamManagement;
  bool isWithRelays;
  double noiseFigure;
  double frequencySpacingInHz;
  int subFramesPerFrame;
  bool enableSidelink;
  bool enableMacInterface;
  bool enableAntCorrelation;
  double simulationTimeInSec;
  double settlingTimeInSec;
  ivec serverNodes;
  ivec serviceNodes;
  vec serverNodeTxPowersInDBm;
  vec serviceNodeTxPowersInDBm;
  ivec serverNodeAntenna;
  ivec serviceNodeAntenna;
  Array<ivec> serverNodeNumerologies;
  Array<ivec> serviceNodeNumerologies;
  ivec serverNodeIDCells;
  ivec serverNodeTDDConf;
  AntennaCorrelationType_E antenneCorrelation;
  UlDlMode_E ulDlMode;
  bool isX2Enabled;
  bool isWithAbstraction;
  ABSTRACTION_MODEL AbstractionModel;
  bool isLocalised;
  bool useFixedSeed;
  unsigned int simulationSeed;
  int strongInterferersToSimulate;
  int strongInterferersToSimulateForPUCCH;
  bool modelWeakInterferenceAsNoise;
  int maximumServiceNodesPerServerNode;
  ConfigFileNames_S myConfigFiles;  
  ResultFileNames_S resultFiles;
  
  //Enable the restriction in scheduling for ReTx
  bool enableRestrictionForReTx;
  
  // Maximum UEs scheduled per TTI
  int maximumServiceNodesPerSubframe;
  
  // Link Level Simulation
  LinkSimulationInfo_S linkSimulationInfo;
  
  //Fixed MCS
  ivec mcsIndices;

  // User Mobility Modelling
  bool enableUserMobility;
  MobilityConfig_S mobilityConfig;
  ReceptionModel_S receptionModel;
  RandomLoadInfo_S randomLoadInfo;

  bool isSRSEnabled;
  bool isPRACHEnabled;
  bool isSSBlockEnabled;
  //OutputFile Print Frequency
  double outputFilePrintFrequency;
  
  //CoMP
  CoMPInfo_S compInfo;
  
  //Node Deactivator
  bool enableServiceNodeDeactivator;
  ivec deactivatorServiceNodeList;

  CarrierInfo_S carrierInfo;
  
  //Reciprocity
  vec weakIntr;
  Array<Array<cmat>> intrCov;
  vec noiseVar;
  
#ifdef USING_MPI
  MPIInfo_S mpiInfo;
#endif

  bool isRRCEnabled;
  
  bool isRRMEnabled;
  
  bool isControlChannelEnabled;
/// Dynamic feature addition can be done by calling updateFeature(..)
  //hook for dynamic feature addition
   map<string,bool> feature;
 bool hasFeature(string featureName){
   map<string,bool>::iterator it=feature.find(featureName);
   if(it != feature.end())
     return it->second;
   else return false;
 }

 void disableFeature(string featureName){
   map<string,bool>::iterator it=feature.find(featureName);
   if(it != feature.end())
     it->second = false;
 }
 
 ///updateFeature adds or update a feature named "featureName" available or accessible from any  where in simulator through hasFeature function: status (true,false). updateFeature checks first the configuration file present(abort if file not present). then check the featureName present in file, if it is there parse it, else set the defaultFeatureValue and write the configuration entry to temporary configFile used.  
 bool updateFeature(string featureName,string configFileName,bool defaultFeatureValue=false)
 {
   bool featureStatus=defaultFeatureValue;
    if(!parse(configFileName,featureName,featureStatus))
    {
      cout<<"[detl:]Warning...  unable to parse "<<featureName<<" from file "<<configFileName<<". Loading given default value "<< defaultFeatureValue<<endl;
      string trueFalseString=(defaultFeatureValue==true)? "true":"false";
      {
	ifstream file(configFileName.c_str());
	if(!file){
	cout<<"[detl:] unable to open configuration file "<<configFileName<<" to read feature " << featureName<<" value for updateFeature function, fileName could be wrong or not present in given location :  exiting ....."<<endl;
	abort();
	}
      }
      ofstream fout(configFileName.c_str(),ofstream::app);
      
      fout<<"//////// Begin of auto generated config Entry (by updatFeature function ////"<<endl;
      fout<<featureName<<"="<<trueFalseString<<endl;
      fout<<"//////// End of auto generated config Entry (by updatFeature function) ////"<<endl;
      cout<<"[detl:]Warning...  adding entry in config file "<<configFileName<<" '"<<featureName<<"="<<trueFalseString<<"'"<<endl;
      feature[featureName] = defaultFeatureValue;return false;//returning false as unable to parse from file
    }
    feature[featureName] = featureStatus;return true;
 }
//end of Dynamic feature addition hook
  SystemInfo_S(){
    
    map<string,bool> feature; 
    
    
    FFR_RBGScaling.set_length(0);
    nuIDs.set_length(0);
    nuConfig.set_length(0);
    initialAssociationMetric=_UNINIT_;
    isWithPicos=false;
    picoBias=-1;
    isWithRelays=false;
    noiseFigure=-1;
    subFramesPerFrame=-1;
    frequencySpacingInHz=-1;
    enableMacInterface =false;
    enableAntCorrelation =false;
    simulationTimeInSec=-1;
    settlingTimeInSec=-1;
    serverNodes.set_size(0);
    serviceNodes.set_size(0);
    serverNodeTxPowersInDBm.set_size(0);
    serviceNodeTxPowersInDBm.set_size(0);
    serverNodeAntenna.set_size(0);
    serviceNodeAntenna.set_size(0);
    serverNodeIDCells.set_length(0);
    serverNodeTDDConf.set_length(0);
    antenneCorrelation=_ANTENNA_CORRELATION_UNCORRELATED_;
    ulDlMode=_ULDL_MODE_UNINIT_;
    isX2Enabled = false;
    isWithAbstraction=false;
    AbstractionModel=_ABSTRACTION_UNINIT_;
    isX2Enabled = false;
    useFixedSeed=false;
    isLocalised = false;
    simulationSeed=0;
    strongInterferersToSimulate=-1;
    strongInterferersToSimulateForPUCCH=-1;
    modelWeakInterferenceAsNoise=true;
    maximumServiceNodesPerServerNode=-1;
//     compScheme=_COMP_SCHEME_UNINIT_;
    mcsIndices.set_length(0);
    enableUserMobility=false;
    
    outputFilePrintFrequency=0.1;
    
    enableRestrictionForReTx=false;
    maximumServiceNodesPerSubframe=-1;
    isPRACHEnabled=false;
    isRRCEnabled = false;
    
    isRRMEnabled=false;
    
    isControlChannelEnabled=false;
    
    isSRSEnabled=false;
    isSSBlockEnabled=false;
    
    enableServiceNodeDeactivator=false;
    deactivatorServiceNodeList.set_size(0);
    weakIntr.set_length(0);
    intrCov.set_length(0);
    noiseVar.set_length(0);
  
    
    enableSidelink=false;
    
  }

  void defaults(){
    
    initialAssociationMetric=_UNINIT_;
    isWithPicos=false;
    isWithRelays=false;
    noiseFigure=-1;
    subFramesPerFrame=-1;
    frequencySpacingInHz=-1;
    simulationTimeInSec=-1;
    settlingTimeInSec=-1;
    serverNodes.set_size(0);
    serviceNodes.set_size(0);
    serverNodeTxPowersInDBm.set_size(0);
    serviceNodeTxPowersInDBm.set_size(0);
    serverNodeAntenna.set_size(0);
    serviceNodeAntenna.set_size(0);
    serverNodeIDCells.set_length(0);
    serverNodeTDDConf.set_length(0);
    antenneCorrelation=_ANTENNA_CORRELATION_UNCORRELATED_;
    ulDlMode=_ULDL_MODE_UNINIT_;
    isWithAbstraction=false;
    AbstractionModel=_ABSTRACTION_UNINIT_;
    useFixedSeed=false;
    simulationSeed=0;
    modelWeakInterferenceAsNoise=true;
    maximumServiceNodesPerServerNode=-1;
    //     compScheme=_COMP_SCHEME_UNINIT_;
    mcsIndices.set_length(0);
    enableUserMobility=false;
    
    outputFilePrintFrequency=0.1;
    
    enableRestrictionForReTx=false;
    maximumServiceNodesPerSubframe=-1;
    
    isRRCEnabled = false;
    
    isRRMEnabled=false;
    
    isControlChannelEnabled=false;
    
    isSRSEnabled=false;
    isSSBlockEnabled=false;
    
    enableServiceNodeDeactivator=false;
    deactivatorServiceNodeList.set_size(0);
    weakIntr.set_length(0);
    intrCov.set_length(0);
    noiseVar.set_length(0);
    
  }
  void calcOthers(){

    subFramesPerFrame=10;
    frequencySpacingInHz=15e3;
 
  }

  void init(int argc, char** argv);
  void checkForSimulationCompatability();
  void loadNUConfig();
  void parseNodesInfo();
};


struct ChannelHandler_S{

  ivec txNodeIDs;
  Array< Array< Array<cmat> > > channel;

  ChannelHandler_S(){

    txNodeIDs.set_length(0);
    channel.set_length(0);
  }

  void initChannelHandler(ivec nodeIDS, ivec txAntennaCount, int rxAntennaCount, int rowCount, int columnCount);
  void clear();
  void loadChannel(Array< Array<cvec> > channelAlongColumn,int txNodeID , int columnNumber);
  
};

struct BWPconfig
{
  ivec rbRange;
  int dedicatedInx;
  
  BWPconfig()
  {
    rbRange.set_length(2);
    dedicatedInx=0;
  }
  
};

struct RxFrameInfo_S{

  int rxNodeID;
  ChannelHandler_S channelHandler;
  Array<cmat> rxFrame;
  Array<cvec> rxFrameInTimeDomain;
  vec summedUpPowerOverREs;
  ivec signalNodeIDs;
  double ROIpowerInWatts;
    //Reciprocity
  double weakIntr;
  Array<cmat> intrCov;
  double noiseVar;

  RxFrameInfo_S(){

    rxNodeID=-1;
    rxFrame.set_length(0);
    summedUpPowerOverREs.set_length(0);
    signalNodeIDs.set_length(0);
    ROIpowerInWatts=0;
    weakIntr=0;
    intrCov.set_length(0);
    noiseVar=0;

  }

  void clear(){

    rxNodeID=-1;
    rxFrame.set_length(0);
    rxFrameInTimeDomain.set_length(0);
    channelHandler.clear();
//     summedUpPowerOverREs.set_length(0);
    signalNodeIDs.set_length(0);
  }

};


struct PerformanceMetricsPerNU_S{

  NUParams_S nuParams;
  bool isDL;
  
  ivec txNodes;
  ivec rxNodes;
  
  vec txNodesThroughput;
  vec rxNodesThroughput;
  
  vec txSpectralEffeciency;
  vec rxSpectralEffeciency;
  
  vec txBLER;
  vec rxBLER;
  
  vec txBLERNew;
  vec rxBLERNew;
  
  vec txBLER_RTx;
  vec rxBLER_RTx;
  
  vec txBER;
  vec rxBER;
  
  vec txPackets;
  vec txCorrectPackets;
  vec txNewPackets;
  vec txNewCorrectPackets;
  vec txBits;
  vec txBTPs;
  vec txCorrectBits;
  vec txCorrectBitsBER;
  vec txCorrectBitsBLER;
  vec txPPSINR;
  vec txPrePSINR;
  vec txNMSE;
  
  vec rxPackets;
  vec rxCorrectPackets;
  vec rxNewPackets;
  vec rxNewCorrectPackets;
  vec rxBits;
  vec rxBTPs;
  vec rxCorrectBits;
  vec rxCorrectBitsBER;
  vec rxCorrectBitsBLER;
  vec rxPPSINR;
  vec rxPrePSINR;
  vec rxNMSE;
  
  vec packetsWithinCQIPeriodicityPerServiceNode;
  vec acksWithinCQIPeriodicityPerServiceNode;
  vec sinroffsetPerServiceNode;
  
  Array<vec> WindowRxNewPacketsPerCW;
  Array<vec> WindowTxNewPacketsPerCW;
  Array<vec> WindowRxCorrectNewPacketsPerCW;
  Array<vec> WindowTxCorrectNewPacketsPerCW;
  
  bvec isNewPrintDLeNBstats; 
  bvec isNewPrintULeNBstats;
  double currentTimeInSec;
//   ivec NumUEsScheduledPerTTI;
//   ivec NumRBsScheduledPerTTI;
  ivec SuccessfullyRxBitsPerTTI;
  
  vec sumRankPerServiceNode;
  vec rankComputationInstant;
  
  vec avgCQIPerServiceNode;
  vec cqiComputationInstant;
  
  vec avgMCSPerServiceNode;
  vec mcsComputationInstant;
  
  vec startTimePerServiceNode;
  vec endTimePerServiceNode;
  
  vec activeTimePerServiceNode;
  ivec nScheduledRBsPerServiceNode;
  
  
  double tddScaling=1;
  
  
//   int numRBs;
//   Array<ivec> RBUtilizationPereNB;
//   Array<vec> PercentageOfRBUtilization;
  
// #ifdef USING_MPI
//   MPIResultInfo_S MPIResultInfo;
// #endif

  void init(ivec serverNodeIDs, ivec serviceNodeIDs, bool isDownlink, double TDDScaling=1);
  void clear();
//   void printBSschedulingStats(int nodeID, int ruCount, int correctBitCount, double currentTime);
  void update(int nodeID, bvec isSuccess, ivec UsedRBs, double effBWPerRU, ivec bitCount, ivec errorBitCount, double prePSINR, double ppSINR, double nMSE, bvec isNewTransmission, double currentTime, double ttiDuration, string filename);
  bool computeMetric(double currentTime, CarrierInfo_S carrierInfo);
  void printBERPerformanceInfo(string filename, double snrIndB);
//   void printNMSEperformanceInfo(string filename, double snrIndB);
  void printSystemPerformanceInfo(McellSystem *mySysPtr,string uePerformanceFileName,string sysPerformanceFileName,string cumPerformanceFileName,AssociationInfo_S *associationInfoPtr);
  void printUEPerformanceInfo(string uePerformanceFileName,AssociationInfo_S* associationInfoPtr,double& cellEdgeThroughput,double& AvgThroughput);
  void printBSPerformanceInfo(string sysPerformanceFileName,McellSystem* mySysPtr,AssociationInfo_S* associationInfoPtr,PerformanceInfo_S* performanceInfoPtr,double cellEdgeThroughput,double AvgThroughput);
  void printSystemPerformanceInfo(string cumPerformanceFileName,McellSystem* mySysPtr,PerformanceInfo_S* performanceInfoPtr,double cellEdgeThroughput,double AvgThroughput);
  PerformanceInfo_S getPerformanceInfo(McellSystem* mySysPtr,Array<ivec>& associatedServiceNodesPerServerNode);
      //   void printRBUtilization(int nodeID);
  
};

struct PerformanceMetrics_S{
  
  ivec nuIDs;
  Array<PerformanceMetricsPerNU_S> performanceLogPerNU;
  
  void init(ivec serverNodeIDs, ivec serviceNodeIDs, ivec nuids, bool isDownlink, vec TDDScaling="")
  {
    nuIDs=nuids;
    if(TDDScaling.length()==0)
       TDDScaling=ones(nuIDs.length()); 
    performanceLogPerNU.set_length(nuIDs.length());
    for(int nu_cnt=0;nu_cnt<nuIDs.length();nu_cnt++)
      performanceLogPerNU(nu_cnt).init(serverNodeIDs,serviceNodeIDs,isDownlink,TDDScaling(nu_cnt));
  }
  void clear()
  {
    for(int nu_cnt=0;nu_cnt<nuIDs.length();nu_cnt++)
      performanceLogPerNU(nu_cnt).clear();
  }
  //   void printBSschedulingStats(int nodeID, int ruCount, int correctBitCount, double currentTime);
  void update(int nodeID, int nuID, bvec isSuccess, ivec UsedRBs, double effBWPerRU, ivec bitCount, ivec errorBitCount, double prePSINR, double ppSINR, double nMSE, bvec isNewTransmission, double currentTime, double ttiDuration,Array<string> filename)
  {
    int nuIndx=find(nuIDs,nuID);
    performanceLogPerNU(nuIndx).update(nodeID,isSuccess,UsedRBs, effBWPerRU,bitCount, errorBitCount,prePSINR,ppSINR, nMSE, isNewTransmission, currentTime, ttiDuration,filename(nuIndx));
  }
  
  bool computeMetric(int nuID, double currentTime, CarrierInfo_S carrierInfo)
  {
    int nuIndx=find(nuIDs,nuID);
    return(performanceLogPerNU(nuIndx).computeMetric(currentTime,carrierInfo));
  }
  
  void printBERPerformanceInfo(string filename, int nuID, double snrIndB)
  {
    int nuIndx=find(nuIDs,nuID);
    performanceLogPerNU(nuIndx).printBERPerformanceInfo(filename,snrIndB);
  }
  
//   void printNMSEperformanceInfo(string filename, int nuID, double snrIndB)
//   {
//     int nuIndx=find(nuIDs,nuID);
//     performanceLogPerNU(nuIndx).printNMSEperformanceInfo(filename,snrIndB);
//   }
  void printSystemPerformanceInfo(McellSystem *mySysPtr, int nuID,string filename1,string filename2,string filename3,AssociationInfo_S *associationInfoPtr)
  {
    int nuIndx=find(nuIDs,nuID);
    performanceLogPerNU(nuIndx).printSystemPerformanceInfo(mySysPtr,filename1,filename2,filename3,associationInfoPtr);
  }
//   void printRBUtilization(int nodeID, int nuID)
//   {
//     int nuIndx=find(nuIDs,nuID);
//     performanceLogPerNU(nuIndx).printRBUtilization(nodeID);
//   }
  void setNUParams(NUParams_S& nuParams)
  {
      performanceLogPerNU(find(nuIDs,nuParams.nuID)).nuParams = nuParams;
  }
};

void printWindowBLERStats(int nodeID, int correctPacketCount, double currentTime, int codeword, string fileName);

struct StrongAndWeakTransmittersInfo_S{

  ivec strongTransmitters;
  ivec weakTransmitters;
  ivec strongTransmittersForPUCCH;
  ivec weakTransmittersForPUCCH;

  StrongAndWeakTransmittersInfo_S(){

    strongTransmitters.set_length(0);
    weakTransmitters.set_length(0);
    strongTransmittersForPUCCH.set_length(0);
    weakTransmittersForPUCCH.set_length(0);
  }
};

struct ReTxInfo_S{
  
  int ueID;
  bvec isNewTransmission;
  ivec tbSizeInBitsForReTx; 
  ivec codewordIndices;
};

struct L2Core_S{
  
  int arrivalTimeforFTP;
  ivec associatedUEs;
  ivec TPCcmdFieldPerUE;
  ivec activeUEs;
  Array<sBeamPairLink> beamPairLinkperUE;
  Array<BWPconfig > bwpAssocaitedUsers; /* Which runs over number of users.*/
  Scheduler dlScheduler;
  Scheduler ulScheduler;
  Array<RadioBearer_S> dlRadioBearer;
  Array<RadioBearer_S> ulRadioBearer;
  Array<HARQManager> dlHarqManager;
  Array<HARQManager> ulHarqManager;
  OuterLoopLinkAdaptation_S outerLoopLinkAdaptationInfoForDL;
  OuterLoopLinkAdaptation_S outerLoopLinkAdaptationInfoForUL;
  
  Array<UEBufferInfo_S> getUEBufferInfo(int subframeNumber, TransmissionType txType, int beamID);
  void updateUEBufferInfo(int ueID, int tbSizeInBytes,int rbID, TransmissionType txType);
  L2Core_S()
  {
  arrivalTimeforFTP =-1;  
  }
  
};

struct L1Core_S{
  
  int cellID;
  NUParams_S nuParameters;
  TDDConfiguration_E tddConfig;
  FrameConfigInfo_S frameConfig;
  RSInfo_S dlRSInfoForUnScheduled;
  RSInfo_S ulRSInfoForUnScheduled;
  RBInfo_S dlRbInfoForUnScheduled;
  RBInfo_S ulRbInfoForUnScheduled;
  CQIComputationConfig_S dlCQIComputationConfig;
  CQIComputationConfig_S ulCQIComputationConfig;
  FrameStructure nativeDlFrameStructure;  ///Native copy of the frame structure to be used when freshly required
  FrameStructure nativeUlFrameStructure;  ///Native copy of the frame structure to be used when freshly required
  FrameStructure dlFrameStructure; ///Current copy of frame structure used by txNodes and rxNodes
  FrameStructure ulFrameStructure;  ///Current copy of frame structure used by txNodes and rxNodes
  ReferenceSignal_S referenceSignal;
  PortInfo_S dlPortInfo;
  PortInfo_S ulPortInfo;
  ResourceMapper resourceMapper; // Need separate resourceMapper for Uplink and Downlink
  PortToAntennaMapper_S portToAntennaMapper;
  Array< Codebook* > codebookPerBeam;
  SourceAndSink downlinkSourceAndSink;
  SourceAndSink uplinkSourceAndSink;
  SourceAndSink sidelinkSourceAndSink;
  FrameReservationInfo_S dlFrameReservationInfo;
  FrameReservationInfo_S ulFrameReservationInfo;
  TransmissionConfig_S dlTxConfig,ulTxConfig;
  ReceptionConfig_S dlRxConfig,ulRxConfig;
  int Pb;
  sBWPartInfo  dlBWPartInfo;
  SpecialSubFrameConfig_S ssfConfig;
  ivec CellConfig;
  int CellConfigPeriodicityInSlots;
  ivec SFI;
  double NI; // Need to take care of BWP and carrier later
  vec SINRdiffForK_Slots; //TPC
  int currentTimeForSINR=4; //TPC note: avg time is 5 here but given as 4 since subframe index is from 0 (i.e. 0 1 2 3 4)
  L1Core_S()
  {
  SINRdiffForK_Slots.set_length(5);  //TPC
  }
};

#endif
