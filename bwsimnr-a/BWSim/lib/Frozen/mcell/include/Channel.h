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

#ifndef _CHN_H_
#define _CHN_H_

#include "SCMLink.h"

struct PhaseNoiseParams_S{
vec  freqVal;
vec  PSDValues;
double  fcBase;
PhaseNoiseParams_S()
{
  freqVal.set_length(6) ;
  PSDValues.set_length(6);
}
};

struct PTRSDensity_S{
int timeDensity;
int freqDensity;
};

/**
 * @brief ChannelFilter Class which manages input buffer for each transmitter and performs either a dot-product (freq-domain) or time-domain convolution of input data and channel modeling Inter-Symbol-Interference
 */
class ChannelFilter{
  
  private :
      bool isTimeDomain;
      vec currentTimePerBuffer;
      vec endTimePerBuffer;
      double sampleIntervalInSec;
      Array<cvec> inputBuffer;
      Array<cvec> filterBuffer;
  public:
    ChannelFilter(); 
    void init(double samplingFrequency,int antennaCount, bool timeDomain=false,int filterLength=0); ///< Initialize the channelFilter with the given parameters
    void load(Array<cvec> &input, double currentTimeInSec, int overwriteOrAdd=0); //overwriteOrAdd - 0->No Action; 1-> Overwrite; 2->Add	 ///< Load the input buffer with the data provided according to the given time-Stamp
    void load(Array< cvec >& input, ivec indices, double currentTimeInSec, int overwriteOrAdd);//overwriteOrAdd - 0->No Action; 1-> Overwrite; 2->Add	 ///< Load the input buffer with the data provided according to the given time-Stamp
    cvec getInputData(int currentAntenna,double startTimeInSec, int ofdmSymbolIndx,int dataSize);	///< Returns data from the input buffer from the given time if available
    bool isHavingData(double currentTimeInSec);
    bool isLoaded();	///< Returns load-status of the input buffer
    bool isLoaded(int Antenna);
    void clearBuffer(); ///< Clears the input buffer
    void clearBuffer(ivec indices); ///< Clears the input buffer
    cvec applyChannel(cvec& channel, int currentAntenna, int ofdmSymbolIndx, double currentTimeInSec, double endTimeInSeconds, double samplingFreqToUse=-1); ///< Apply the channel (either time domain convolution or frequency domain dot product) on the input buffer and returns output
    cvec applyTimeDomainChannel(cvec &channel,int currentAntenna, double currentTimeInSec, double endTimeInSeconds, double samplingFreqToUse); ///< Apply the channel (either time domain convolution or frequency domain dot product) on the input buffer and returns output
    cvec applyFrequencyDomainChannel(cvec &channel,int currentAntenna, int ofdmSymbolIndx); ///< Apply the channel (either time domain convolution or frequency domain dot product) on the input buffer and returns output
    double  getCurrentTime(int currentAntenna);	///< Returns the current time of the data
    cvec linearRateMatch(cvec input, double currentTime, double desiredSamplingFreq);
};

class LLSChannel
{
private:
    FadingChannelModel_E channelModel;
    FadingParameters_S fadingParams;
    Array<vec> subpathAOD,subpathAOA,subpathZOD,subpathZOA;
    Array<vec> txAntennaGainPerSubpath,rxAntennaGainPerSubpath;
    Array<vec> subpathPhase;
    Array<mat> txLocationVectorPerAntennaElement,rxLocationVectorPerAntennaElement;
    vec txPolarizationAngleInDegreePerAntennaElement,rxPolarizationAngleInDegreePerAntennaElement;
    sBeamParameters txBeamParameters,rxBeamParameters;
    double carrierFrequencyInHz;
    mat velocityVector;
    Array<vec> xprPerSubpath;
    int nClusters,nRaysPerCluster;
    vec clusterPow,clusterDelay;
    bool isLOS;
    double losTxAntennaGain,losRxAntennaGain;
    double losAOD,losAOA,losZOD,losZOA;
    double losTapPow;
    Array<Vec<unsigned int>> rndSeed;
public:
    
    void initChannel(FadingChannelModel_E channelModel,double carrierFrequency,double delaySpread,mat velocity,Antenna* txAntennaPtr,Antenna* rxAntennaPtr);
    cvec generateTapWeights(int txAntennaCount,int rxAntennaCount,double currentTimeInSeconds);
    vec getClusterDelays(){return clusterDelay;}
    void reInitChannel(Antenna* txAntennaPtr,Antenna* rxAntennaPtr,double delaySpread);
    FadingParameters_S getFadingParams(){return fadingParams;}
    dComplex getLOSPathGain(int currentTxAntennaElement,int currentRxAntennaElement,double currentTimeInSeconds);
    dComplex getPathGain(int clusterCnt,int pathCnt,int currentTxAntennaElement,int currentRxAntennaElement,double currentTimeInSeconds);
    cvec generateTDLChannelTaps(int nTaps,int nSinusoids,double velocity,double currentTimeInSeconds,double carrierFreq,unsigned int rnd_state);
    
};

bool isCDLModel(FadingChannelModel_E fadingModel);
bool isTDLModel(FadingChannelModel_E fadingModel);

struct ChannelModelDecider_S{
  
  Array<FadingChannelModel_E> channelModels;
//  double dopplerThreshold;
  double velocityThreshold;
  bool isTDLmodel;
  bool isCDLmodel;
  vec delaySpreadTDL;
  
  ChannelModelDecider_S(){
    
    channelModels.set_length(0);
 //   dopplerThreshold=-1;
    velocityThreshold=-1;
    isTDLmodel=false;
    isCDLmodel=false;
    delaySpreadTDL.set_size(0);
  }
  
  void load(string configFile){
    
    
    Array<string> channelModel(2);
    if(!parse(configFile,"pedestrianChannelModel",channelModel(0)))
    {
      cout<<"[both:]Unable to parse pedestrianChannelModel from config file."<<endl;abort();
    }
    if(!parse(configFile,"vehicularChannelModel",channelModel(1)))
    {
      cout<<"[both:]Unable to parse vehicularChannelModel from config file."<<endl;abort();
    }
    if(channelModel(1)=="_FADING_CHANNEL_MODEL_UNINIT_") channelModel(1)=channelModel(0);
    
    channelModels.set_length(2);
    for(int string_cnt=0;string_cnt<channelModel.length();string_cnt++)
    {
      channelModels(string_cnt)=parseChannelModel(channelModel(string_cnt));
      isTDLmodel = isTDLModel(channelModels(string_cnt));
      isCDLmodel = isCDLModel(channelModels(string_cnt));
    }

    if(!parse(configFile,"velocityThreshold",velocityThreshold))
    {
      cout<<"[detl:]Unable to parse velocityThreshold. Disabling velocityThreshold by default "<<endl;
      velocityThreshold=-1;
    }
    
    if(isTDLmodel || isCDLmodel)
    {
      if(!parse(configFile,"delaySpreadTDL",delaySpreadTDL))
      {
	cout<<"[detl:]Unable to parse delaySpreadTDL "<<endl;
	abort();
      }
      delaySpreadTDL = 1e-9 * delaySpreadTDL;
    }
    
//      double carrierFreq,carrierFreqInMHz;
//     if(!parse(configFile,"carrierFreqInMHz",carrierFreqInMHz))
//     {
//       cout<<"[both:] Unable to parse carrierFreqInMHz from config file..."<<endl;abort();
//     }
//     carrierFreq=carrierFreqInMHz*1e6;
//     if(velocityThreshold!=-1)
//       dopplerThreshold=velocityThreshold/lightSpeed*carrierFreq;
//     else
//       dopplerThreshold=-1;
  }
  FadingChannelModel_E getChannelModel(double velocity){
    
    if(velocityThreshold!=-1)
    {
      if(velocity>velocityThreshold) return(channelModels(1));
      else				return(channelModels(0));
    }
    else	return(channelModels(0));
   
  }
  
  double getDelaySpreadForTDL(double velocity){
    
    if(velocityThreshold!=-1)
    {
      if(velocity>velocityThreshold) return(delaySpreadTDL(1));
      else				return(delaySpreadTDL(0));
    }
    else	return(delaySpreadTDL(0));
  }
};


struct CorrMatrix_S{

    bool isInitialized; 
    AntennaCorrelationType_E antennaCorrelation;
    mat Tx1Ant,Tx2Ant,Tx4Ant;
    mat Rx1Ant,Rx2Ant,Rx4Ant;

    CorrMatrix_S(){
      
        isInitialized=false;      
	antennaCorrelation=_ANTENNA_CORRELATION_UNCORRELATED_;
        Tx1Ant.set_size(1,1);Tx1Ant=eye(1);
        Tx2Ant.set_size(2,2);Tx2Ant=eye(2);
        Tx4Ant.set_size(4,4);Tx4Ant=eye(4);
        Rx1Ant.set_size(1,1);Rx1Ant=eye(1);
        Rx2Ant.set_size(2,2);Rx2Ant=eye(2);
        Rx4Ant.set_size(4,4);Rx4Ant=eye(4);
    }

    void load(AntennaCorrelationType_E antCorrelation){
      
      isInitialized=true;
      double alpha =0.0;
      double beta =0.0;
      antennaCorrelation=antCorrelation;
      if(antennaCorrelation==_ANTENNA_CORRELATION_UNCORRELATED_)
      { alpha =0.0; beta =0.0;}
      else if(antennaCorrelation==_ANTENNA_CORRELATION_MEDIUM_)
      { alpha =0.9; beta =0.3;}
      else if(antennaCorrelation==_ANTENNA_CORRELATION_HIGH_)
      { alpha =0.9; beta =0.9;}
      else
      {
         cout<<"Antenna Correlation is _ANTENNA_CORRELATION_UNCORRELATED_ by default..."<<endl;
         alpha =0.0; beta =0.0;
       }
       
       Tx1Ant(0,0)=1.0;
       Rx1Ant(0,0)=1.0;
       Tx2Ant(0,0)=1.0;Tx2Ant(0,1)=alpha;Tx2Ant(1,0)=alpha;Tx2Ant(1,1)=1.0;
       Rx2Ant(0,0)=1.0;Rx2Ant(0,1)=beta;Rx2Ant(1,0)=beta;Rx2Ant(1,1)=1.0;
       Tx4Ant(0,0)=1.0;Tx4Ant(0,1)=std::pow(alpha,1/9.0);Tx4Ant(0,2)=std::pow(alpha,4.0/9.0);Tx4Ant(0,3)=alpha;
       Tx4Ant(1,0)=std::pow(alpha,1/9.0);Tx4Ant(1,1)=1.0;Tx4Ant(1,2)=std::pow(alpha,1/9.0);Tx4Ant(1,3)=std::pow(alpha,4.0/9.0);
       Tx4Ant(2,0)=std::pow(alpha,4.0/9.0);Tx4Ant(2,1)=std::pow(alpha,1/9.0); Tx4Ant(2,2)=1.0; Tx4Ant(2,3)=std::pow(alpha,1/9.0);
       Tx4Ant(3,0)=alpha;Tx4Ant(3,1)=std::pow(alpha,4.0/9.0);Tx4Ant(3,2)=std::pow(alpha,1/9.0);Tx4Ant(3,3)=1.0;
       Rx4Ant(0,0)=1.0;Rx4Ant(0,1)=std::pow(alpha,1/9.0);Rx4Ant(0,2)=std::pow(alpha,4.0/9.0);Rx4Ant(0,3)=alpha;
       Rx4Ant(1,0)=std::pow(alpha,1/9.0);Rx4Ant(1,1)=1.0;Rx4Ant(1,2)=std::pow(alpha,1/9.0);Rx4Ant(1,3)=std::pow(alpha,4.0/9.0);
       Rx4Ant(2,0)=std::pow(alpha,4.0/9.0);Rx4Ant(2,1)=std::pow(alpha,1/9.0); Rx4Ant(2,2)=1.0; Rx4Ant(2,3)=std::pow(alpha,1/9.0);
       Rx4Ant(3,0)=alpha;Rx4Ant(3,1)=std::pow(alpha,4.0/9.0);Rx4Ant(3,2)=std::pow(alpha,1/9.0);Rx4Ant(3,3)=1.0;

    }
};

struct QuasiStaticInfo_S{
  
  bool isEnabled;
  bool isTimeDomain;
  vec quasiStaticDurationPerRxNode;
  Array<vec> channelGenTimePerLink;
  Array< Array< Array< Array <cvec> > > > oldChannelPerRxNode;
  
  Array<Array <vec>> channelTapDelays;
  double samplingFreqToUse;
  QuasiStaticInfo_S(){
    
    isEnabled=false;
    isTimeDomain=false;
    quasiStaticDurationPerRxNode.set_length(0);
    channelGenTimePerLink.set_length(0);
    oldChannelPerRxNode.set_length(0);

    channelTapDelays.set_length(0);
    samplingFreqToUse = -1.0;
  }
  void clear(){
    
    for(int d1=0;d1<oldChannelPerRxNode.length();d1++)
      for(int d2=0;d2<oldChannelPerRxNode(d1).length();d2++)
      {
	oldChannelPerRxNode(d1)(d2).set_length(0);
	channelGenTimePerLink(d1)(d2)=-1;
      }
  }
  
  bool isChannelAvailable(int txNodeIndx, int rxNodeIndx, double currentTime){
    
    if(oldChannelPerRxNode(rxNodeIndx)(txNodeIndx).length()==0)
      return(false);
    
    if(abs(currentTime-channelGenTimePerLink(rxNodeIndx)(txNodeIndx))>=quasiStaticDurationPerRxNode(rxNodeIndx))
      return(false);
    else
      return(true);
    
  }
  Array< Array< cvec > > getOldChannel(int txNodeIndx, int rxNodeIndx, double currentTime){
    
    if(isChannelAvailable(txNodeIndx,rxNodeIndx,currentTime))
    {
      if(isTimeDomain)
	return(oldChannelPerRxNode(rxNodeIndx)(txNodeIndx));
      else
      {
	Array< Array<cvec> > temp(oldChannelPerRxNode(rxNodeIndx)(txNodeIndx).length());
	for(int d1=0;d1<oldChannelPerRxNode(rxNodeIndx)(txNodeIndx).length();d1++)
	{
	  temp(d1).set_length(oldChannelPerRxNode(rxNodeIndx)(txNodeIndx)(d1).length());
	  for(int d2=0;d2<oldChannelPerRxNode(rxNodeIndx)(txNodeIndx)(d1).length();d2++)
	  {
	    temp(d1)(d2)=oldChannelPerRxNode(rxNodeIndx)(txNodeIndx)(d1)(d2);
	    
	    }
	}   
	return(temp);
      }
    }
    else
    { cout<<"[both:] Error .. Trying to get channel which is not available in getOldChannel()..."<<endl;abort();}
  }

  cvec getOldChannel(int txNodeIndx, int rxNodeIndx, int txAntennaCount, int rxAntennaCount,
                     double currentTime, int fftSize) {
    cvec channel;

    getOldChannel(txNodeIndx, rxNodeIndx, txAntennaCount, rxAntennaCount, currentTime, fftSize,
                  channel);

    return (channel);
  }

  void getOldChannel(int txNodeIndx, int rxNodeIndx, int txAntennaCount, int rxAntennaCount,
                     double currentTime, int fftSize, cvec &channel) {
    if (!isChannelAvailable(txNodeIndx, rxNodeIndx, currentTime)) {
      cout << "[both:] Error .. Trying to get channel which is not available in getOldChannel()..."
           << endl;
      abort();
    }

    if (txAntennaCount >= oldChannelPerRxNode(rxNodeIndx)(txNodeIndx).length()) {
      cout << "[both:] Invalid txAntennaCount to getOldChannel()..." << endl;
      abort();
    }

    if (rxAntennaCount >= oldChannelPerRxNode(rxNodeIndx)(txNodeIndx)(0).length()) {
      cout << "[both:] Invalid rxAntennaCount to getOldChannel()..." << endl;
      abort();
    }

    vec tapDel     = channelTapDelays(rxNodeIndx)(txNodeIndx);
    int lenTaps    = tapDel.length();
    int lenChannel = floor_i(tapDel(lenTaps - 1) * samplingFreqToUse);
    lenChannel += 1;
    channel.set_size(lenChannel);
    channel.clear();

    cvec tapWeights = oldChannelPerRxNode(rxNodeIndx)(txNodeIndx)(txAntennaCount)(rxAntennaCount);

    int channelIndx = 0;
    for (int tap_cnt = 0; tap_cnt < lenTaps; tap_cnt++) {
      channelIndx = floor_i(tapDel(tap_cnt) * samplingFreqToUse);
      if (channelIndx >= channel.length()) {
        cout << "[both:] Insufficient Channel length to represent the Channel Model at the given "
                "sampling Frequency."
             << endl;
        abort();
      }
      channel(channelIndx) = channel(channelIndx) + tapWeights(tap_cnt);
    }

    if (not isTimeDomain) {
      // Expand or truncate channel len as per FFT size.
      if (channel.length() < fftSize)
        append(channel, zeros_c(fftSize - channel.length()));
      else
        channel = channel(0, fftSize - 1);

      channel = fft(channel);
      channel = fftShift(channel);
    }
  }

  void updateChannel(int txNodeIndx, int rxNodeIndx, int txAntennaCount, int rxAntennaCount, double currentTime, cvec &channel)
  {
//     if(channelGenTimePerLink(rxNodeIndx)(txNodeIndx)>currentTime)
//     {
//       cout<<"Cannot update channel using an old time ..."<<endl;abort();return;
//     }
//     else
    {
      channelGenTimePerLink(rxNodeIndx)(txNodeIndx)=currentTime;
      oldChannelPerRxNode(rxNodeIndx)(txNodeIndx)(txAntennaCount)(rxAntennaCount)=channel;
    }
  }
  void updateChannel(int txNodeIndx, int rxNodeIndx, double currentTime, Array< Array < cvec > > &channel)
  {
//     if(channelGenTimePerLink(rxNodeIndx)(txNodeIndx)>currentTime)
//     {
//         cout<<"Cannot update channel using an old time ..."<<endl;abort();return;
//     }
//     else
    {
      channelGenTimePerLink(rxNodeIndx)(txNodeIndx)=currentTime;
      oldChannelPerRxNode(rxNodeIndx)(txNodeIndx)=channel;
    }
  }

  void updateChannelTapDelay(int txNodeIndx, int rxNodeIndx, vec &channelTapDelay) {
    channelTapDelays(rxNodeIndx)(txNodeIndx) = channelTapDelay;
  }

  void updatesamplingFreqToUse(double samplingFreq) {
    samplingFreqToUse = samplingFreq;
  }
}; 

class ChannelCloud{
  
  public:
    NodeInfo_S nodesInfo;
    ChannelInfo_S sysWideChannelInfo;
    imat scmLinkIndx;
    Array< SCMLink > scmLinks;
    bool channelReciprocity;
    bool enableFlatFading;
    bool addOxygenLoss;
    bool useGroundReflection;
    LLSChannel llsChannel;
    Array< Array< Vec<unsigned int> > > rndSeed;
    bool applyFaradayRotation;
    int numb=0;

  private:
    bool isChannelInitialized;
    bool parameterStatus;
    int timeDomain;
    int dataSize;
    int addMultiPathFading;
    int addNoise;
    double commonSamplingFrequency;
    double simulationDuration;
   // ChannelInfo_S sysWideChannelInfo;
    ChannelGenerationMethod_E channelGenerationMethod;
    ChannelModelType channelModelType;
    Array<FadingParameters_S> customFadingParameters;
    Array<string> customFadingParametersName;
    Array<ChannelInfo_S> linkSpecificChannelInfo;
    imat channelInfoIndx;
    Array<ChannelFilter> channelFilterPerNode;
    
    mat velocityMatrix;
    mat linkGainInDB;
    mat pathDelayInSec;
    mat instantPathDelayInSec;
    bmat scmReciprocityFlag;

    
    ivec currentTxNodes;  ///< Nodes currently in Tx mode.
    
    ModifiedJakesInfo_S modifiedJakesInfo;
    bool addPathDelay;
    ivec nodeSyncList;
    ChannelModelDecider_S channelModelDecider;
    CorrMatrix_S CorrMatrix;
    QuasiStaticInfo_S quasiStaticChannelInfo;
//     ostream h_avg;
    
  public:
    ChannelCloud();
    void loadChannel(string fileName); ///< Initialize channel cloud parameters from the file
    void loadCorrMatrix(AntennaCorrelationType_E antennaCorrelation); // Load correlation matrix and compute its Cholesky transfomation to apply on Tx and Rx
    Array<cmat> getAntennaCorrelationMatrices(int txAntenna, int rxAntenna);// Load antenna correlation matrix for multiplication with Tx and Rx
    void initChannel(bool channelReciprocity,bool enableQuasiStatic,double quasiStaticDurationinSec);	///< Initialize channel cloud with required Random values for channel generation of each link
    bool isChannelReady();	///< Returns whether the channel is initialized fully for channel generation
    bool isTimeDomain();	///< Returns whether the channel is initialized to Time-Domain mode
    void disableMultiPathChannel(); ///< Disables multipath fading
    void enableMultiPathChannel();	///< Enables multipath fading if disabled
    void disableNoise();	///< Diables AWGN addition
    void enableNoise();	///<	Enables AWGN addition if disabled
    void disablePathDelay();	///< Disables path delay addition from Transmitter to Receiver
    void enablePathDelay();	///< Disables path delay addition from Transmitter to Receiver
    void copyChannelSeeds(int fromNodeID, int toNodeID); ///< Copy channel seeds from a node to another. This makes the channel exactly same in both the nodes .
    double getNoiseVariancePerDimension(int rxNodeID);	///< Returns noise variance per dimension of the rxNodeID
    double getNodeVelocity(int txNodeID,int rxNodeID);	///< Returns relative-velocity of the rxNodeID w.r.t txNodeID   
    double getNodeCarrierFrequency(int rxNodeID);	///< Returns node carrierFrequencyInHz of the rxNodeID
    int getDataSize();	///< Returns size of data loaded into the channel
    double getCommonSamplingFrequencyInHz();	///< Returns channel sampling frequency
    int getScmLinkIndx(int rxNodeId, int txNodeId);
    void setSCMLinkIndx(int rxNodeId, int txNodeId,int index);
    double getSimulationDurationInSeconds();	///< Returns simulation duration in seconds (set only for Smith generation method)
    ChannelGenerationMethod_E getChannelGenerationMethod();	///< Returns channel generation method
    FadingChannelModel_E getSysWideChannelModel();	///< Returns System-wide channel model
    FadingType_E getSysWideFadingType();	///< Returns System-wide Fading type
    SCMPropagationScenario getSysWideSCMPropagationScenario();	///< Returns System-wide SCMPropagationScenario
   // double getSysWideDopplerSpread();	///< Returns System-wide Doppler Spread
    ivec getNodes();	///< Returns the Nodes avaiable in the channel cloud
    ivec getNodesInTxMode();	///< Returns the Nodes currently in Tx mode in the channel cloud 
    ChannelInfo_S getLinkSpecificChannelInfo(int txNode, int rxNode);	///< Returns Link specific channel information between the Tx-Rx pair
    double getLinkSpecificChannelGainInDB(int txNode, int rxNode,int txPanelID,int rxPanelID,bool isWithSCMSpecificAntGain=false,bool isLOS=false);	///< Returns Link-Specific channel gain in dB between the Tx-Rx pair
    vec getLinkSpecificChannelGainInDB(ivec txNodes, int rxNode,ivec txPanelIDs,int rxPanelID, bool isWithSCMSpecificAntGain=false); 	///< Returns Link-Specific channel gain in dB between the Tx-Rx pairs
    vec getLinkSpecificChannelGainInDB(int txNode, ivec rxNodes,int txPanelID,ivec rxPanelIDs="", bool isWithSCMSpecificAntGain=false); ///< Returns Link-Specific channel gain in dB between the Tx-Rx pairs
    SCMLink getSCMLink(int rxNode);
    mat getLinkSpecificChannelGainInDB(ivec txNodes, ivec rxNodes, bool isWithSCMSpecificAntGain=false,ivec txPanelIDs="",ivec rxPanelIDs="");	///< Returns Link-Specific channel gain in dB between the Tx-Rx pairs
    FadingParameters_S getLinkSpecificFadingParameters(int txNode, int rxNode);	///< Returns FadingParameters between the Tx-Rx pair
    
    void clearChannelBuffer();	///< Clears channel buffer of all Tx-Rx pair
    void clearChannelBuffer(ivec txNodes, ivec txBeamIDs="");
    void clearQuasistaticChannelInfo();
    void clearSCMFixedTapGains();
    ivec getTxNodesWithData(double currentTimeInSec);
    
    void setDataSize(int size, bool overwrite=false);	///< Sets size of data to be loaded into the channel
    void setNoiseVariancePerDimension(int rxNodeID,double noiseVar);	///< Sets noisev ariance of the given RxNodeID
    void setNoiseVariancePerDimension(ivec rxNodeIDs, double noiseVar);	///< Sets noisev ariance of the given RxNodeIDs
    void setSamplingFrequencyInHz(double samplingFreq);
    void setSamplingFrequencyInHz(ivec nodeIDs, double samplingFreq);
    void setSimulationDurationInSeconds(double simulationDur);
    void setChannelToFrequencyDomain();
    void setChannelToTimeDomain();
    void setCustomFadingParameters(string fadingParmsName, FadingParameters_S customFadingParams);
    void setChannelGenerationMethod(ChannelGenerationMethod_E method);
    void setSysWideChannelInfo(FadingChannelModel_E sysChannelModel,FadingType_E fadingType=_FADING_TYPE_UNINIT_,  string customFadParamsName="");
    void setSysWideChannelInfo(SCMPropagationScenario scmScenario);
    //void setSysWideChannelInfo(double dopplerSpread,FadingType_E fadingType=_FADING_TYPE_UNINIT_);
    //void setSysWideChannelInfo(FadingChannelModel_E sysChannelModel,double dopplerSpread,FadingType_E fadingType=_FADING_TYPE_UNINIT_,  string customFadParamsName="");
    //void setSysWideChannelInfo(SCMPropagationScenario scmScenario,double dopplerSpread);
    void setSysLinkGainInfo(string filename);
    void setSysLinkGainInfo(ivec nodeList,mat pathDelayInSec, mat &linkGainTableInDB);
    void updateLinkGainInfo(int refNode, ivec nodeList,Array<vec> &linkGainTableInDB);
    void setSCMLargeScaleParametersInfo(string fileName);
    /*void setLinkSpecificChannelInfo(ivec txNodeList, ivec rxNodeList, double linkDopplerSpread,  FadingType_E linkFadingType=_FADING_TYPE_UNINIT_, bool overwrite=false);
    void setLinkSpecificChannelInfo(ivec txNodeList, ivec rxNodeList, FadingChannelModel_E linkChannelModel,double linkDopplerSpread,  FadingType_E linkFadingType=_FADING_TYPE_UNINIT_, bool overwrite=false, string customFadParamsName="");
    void setLinkSpecificChannelInfo(ivec txNodeList, ivec rxNodeList, SCMPropagationScenario scmScenario,double linkDopplerSpread,  bool overwrite=false);
    */
    void setLinkSpecificChannelInfo(ivec txNodeList, ivec rxNodeList, double linkDopplerSpread,  FadingType_E linkFadingType=_FADING_TYPE_UNINIT_, bool overwrite=false);
    void setLinkSpecificChannelInfo(ivec txNodeList, ivec rxNodeList, FadingChannelModel_E linkChannelModel,double linkDopplerSpread,  FadingType_E linkFadingType=_FADING_TYPE_UNINIT_, bool overwrite=false, string customFadParamsName="");
    void setLinkSpecificChannelInfo(ivec txNodeList, ivec rxNodeList, SCMPropagationScenario scmScenario,double linkDopplerSpread,  bool overwrite=false);
    void setLinkGain(int nodeX, int nodeY, double xToyLinkGain,double yToxLinkGain, bool overwtrite=false);
    void setNodesAntennaCount(ivec nodeList,int count);
    void setNodesAntennaCount(ivec nodeList,ivec countList);
    void setNodesCarrierFrequency(ivec nodeList,double carrierFreqnHz);
    void setNodesCarrierFrequency(ivec nodeList,vec carrierFreqInHz);
    void setNodesNoiseVariancePerDimension(ivec nodeList,double noiseVar);
    void setNodesNoiseVariancePerDimension(ivec nodeList,vec noiseVars);
    void syncLink(int serverNode, int serviceNode, bool overwrite=false);
    FadingParameters_S generateFadingParameters(FadingChannelModel_E channelModel, double delaySpreadTDL = -1, double dopplerSpread = -1);   
    
    cvec generateFadingChannel(int txNodeID, int rxNodeID, double currentTimeInSeconds, bool printChannel=false, int currentTxAntenna=-1, int currentRxAntenna=-1, unsigned int rndSeed=0,int fftSize=-1);
    
    cvec generateFadingChannelForInitialAssociation(int txNodeID, int rxNodeID, int currentTxAntenna, int currentRxAntenna,int txBeamID=-1, int rxBeamID=-1);
    
    cvec generate5GAASChannelTapGains(int txNodeID, int rxNodeID, int currentTxAntenna, int currentRxAntenna,double currentTimeInSeconds, bool isChannelGenForRSRP , int txBeamID, int rxBeamID);
    
    void transmitData(int txNodeID, int txBeamID ,int datasize, Array< cvec >& dataFromAllAntennas, double currentTimeInSeconds, int overwriteOrAdd=0); //overwriteOrAdd - 0->No Action; 1-> Overwrite; 2->Add
    cvec applyGaussianNoiseAndReturnNoise(cvec &signal, double noiseVariancePerDimension);
    void setNodesToTxMode(ivec txNodes);
    ChannelOutput_S receiveData(int rxNodeID, int rxBeamID, int txNodeID, int ofdmSymbolIndx, double currentTimeInSeconds, double endTimeInSeconds = -1, bool printChannel = false, bool computeSumPower = true, cmat txAntennaCorrelationMatrix = "", cmat rxAntennaCorrelationMatrix = "");
    ChannelOutput_S receiveData(int rxNodeID, int rxBeamID, AntennaCorrelationType_E antCorrelationType, int ofdmSymbolIndx, double currentTimeInSeconds, double endTimeInSeconds, bool printChannel = 0, ivec txNodeList = "", ivec txNodesToSumPowerFrom = "");
    ChannelOutput_S generateChannel(int rxNodeID, int rxBeamID, int txNodeID, double currentTimeInSeconds, int fftSize = -1, cmat txAntennaCorrelationMatrix = "", cmat rxAntennaCorrelationMatrix = "");
    ChannelOutput_S generateChannel(int rxNodeID, int rxBeamID, AntennaCorrelationType_E antCorrelationType, double currentTimeInSeconds, int fftSize = -1, ivec txNodeList = "");
    ChannelMatrix_S generateChannel(int rxNodeID, int rxBeamID, int txNodeID, vec currentTimeInSeconds, int fftSize=-1,cmat txAntennaCorrelationMatrix="", cmat rxAntennaCorrelationMatrix="");
    ChannelMatrix_S generateChannel(int rxNodeID, int rxBeamID, AntennaCorrelationType_E antCorrelationType, vec currentTimeInSeconds, int fftSize = -1, ivec txNodeList = "");
    
    ChannelMatrix_S generateChannelForAnalogBeams(int rxNodeID, ivec txNodeList, ivec& nTxBeamsPerTxNode, int& nRxBeams, double currentTimeInSec=0.0, bool isChannelGenForRSRP=true);
    ChannelMatrix_S generateChannelForAnalogBeams(int rxNodeID, int txNodeID, double currentTimeInSec=0.0, bool isChannelGenForRSRP=true);
    
    double addSignalsModelledAsNoise(int rxNodeID, ChannelOutput_S &channelOutput,double currentTimeInSeconds ,ivec xNodeListToModelAsNoise, vec noiseModellingTxSubcarrierPowerInDBm);
    double getInstantNoiseModellingPower(int rxNodeID, double currentTimeInSeconds ,ivec xNodeListToModelAsNoise, vec noiseModellingTxSubcarrierPowerInDBm);
    RestOfTransmittersInfo_S getRestOfTransmittersInfo(int rxNodeID, ivec txNodesToOmit);
     cvec generateChannelTaps(ChannelInfo_S channelInfo,double velocity, double currentTimeInSeconds, FadingParameters_S channelParams,double carrierFreq, unsigned int rndSeed, double samplingFrequency=-1); // sampling freq needed only for smith method
    //cvec generateChannelTaps(ChannelInfo_S channelInfo, double currentTimeInSeconds, FadingParameters_S channelParams, unsigned int rndSeed);
    void generateTapWeightsForModifiedJakes(int rxNodeID, ivec txNodeIDs, double currentTimeInSeconds);
    
    double getDopplerSpreadInHz(int txNodeID, int rxNodeID);
    double initializeSCMLink(int txNodeID, int rxNodeID, double carrierFrequencyInHz, SCMLargeScaleParameters_S scmLargeScaleParameters, Location_S txNodeLocation, Location_S rxNodeLocation, Antenna& txAntenna, Antenna& rxAntenna, bool isLOS, bool maintainReciprocity, bool addPathSpecificAntennaGain = 1, bool reInitialize = 0);
    void preAllocateSCMLink(ivec rxNodeIDs, Array< ivec > txNodeIDsPerRxNode, bool maintainReciprocity = false);
    void preAllocateSCMLink(Array< ivec > rxNodeIDsPerTxNode, ivec txNodeIDs, bool maintainReciprocity = false);
    bool isSCMLinkInitialized(int txNodeID, int rxNodeID);
    
    //PhaseNoise
    cvec generatePNSamples(int rxNodeID,int pnPSDID,int numPNSamples);
    void printAntennaGains(ivec serverNodes,ivec serviceNodes,string fileName);
    double getChannelGain(int rxNodeID,int txNodeID);
    SCMLink getSCMLink(int rxNode,int txNode){return scmLinks(getScmLinkIndx(rxNode,txNode));}
    double getLinkGainInDB(int rxNodeID,int txNodeID);
    void preAllocateSCMLink(int rxNodeID,int txNodeID,bool maintainReciprocity = false);
  private:
    void loadDefault();
    
};

#endif

