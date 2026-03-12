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


#ifndef _SAE_MC_H_
#define _SAE_MC_H_

#include "../../simSupport/include/CommonStructsAndEnums.h"
#include "BeamStructAndEnums.h"

using namespace std;

using std::cout;
using std::endl;

//Enabling this will generate the channel according to R1-1907968 for InF and 3GPP TR 38.900 v14.0.0 for other scenerio. 
// #define _USING_CALIBRATION_

#define earthRadius 6371000  //Earth radius in m 

///------------------Enums-------------------
enum ACF_type {un_init=-1, COMB150, COMB300, COMB500, COMB1000, EXP150, EXP300, EXP500, EXP1000, GAUSS150, GAUSS300, GAUSS500, GAUSS1000};

enum Distribution {Normal, Uniform, Uniform_Forced, Uniform_Int, Exponential, Laplace};

enum DropMethod_E{
  _DROPMETHOD_RANDOM_,
  _DROPMETHOD_HEXAGONAL_,
  _DROPMETHOD_RANDOMHEX_,
  _DROPMETHOD_DENSE_PICO_,
  _DROPMETHOD_SINGLE_CELL_,
  _DROPMETHOD_INBUILDING_,
  _DROPMETHOD_INBUILDING3SEC_,
  _DROPMETHOD_SATELLITE_
};

enum NodeCategory_E{
  _NODECATEGORY_UNINIT_=-1,
  _NODECATEGORY_SERVERNODE_,
  _NODECATEGORY_SERVICENODE_
};

enum AntennaType_E{
  _ANTENNA_TYPE_OMNI_,
  _ANTENNA_TYPE_PARABOLIC_,
  _ANTENNA_TYPE_CUSTOM_,
  _ANTENNA_TYPE_QUASI_ISOTROPIC_,
  _ANTENNA_TYPE_CIRCULAR_,
  _ANTENNA_TYPE_HAPS_    // HAPS: 3GPP 38.901 element pattern + planar array + GCS-to-LCS mech tilt
};

enum CellLayout_E{
  _CELLLAYOUT_UNINIT_=-1,
  _CELLLAYOUT_HEXAGONAL_,
  _CELLLAYOUT_RANDOM_,
  _CELLLAYOUT_NTN_
  
  
};

enum ShadowType_E{
  _SHADOWTYPE_UNINIT_=-1,
  _SHADOWTYPE_CORRELATED_,
  _SHADOWTYPE_UNCORRELATED_
};

enum FadingChannelModel_E{
  _FADING_CHANNEL_MODEL_UNINIT_=-1,
  _FADING_CHANNEL_MODEL_AWGN_,
  _FADING_CHANNEL_MODEL_SUI_3_,
  _FADING_CHANNEL_MODEL_PED_A_,
  _FADING_CHANNEL_MODEL_PED_B_,
  _FADING_CHANNEL_MODEL_VEH_A_,
  _FADING_CHANNEL_MODEL_EPA_,
  _FADING_CHANNEL_MODEL_EVA_,
  _FADING_CHANNEL_MODEL_ETU_,
  _FADING_CHANNEL_MODEL_MODIFIED_PED_B_,
  _FADING_CHANNEL_MODEL_MODIFIED_VEH_A_,
  _FADING_CHANNEL_MODEL_URBAN_MACRO_,
  _FADING_CHANNEL_MODEL_TDL_A_,
  _FADING_CHANNEL_MODEL_TDL_B_,
  _FADING_CHANNEL_MODEL_TDL_C_,
  _FADING_CHANNEL_MODEL_TDL_D_,
  _FADING_CHANNEL_MODEL_TDL_E_,
  _FADING_CHANNEL_MODEL_CDL_A_,
  _FADING_CHANNEL_MODEL_CDL_B_,
  _FADING_CHANNEL_MODEL_CDL_C_,
  _FADING_CHANNEL_MODEL_CDL_D_,
  _FADING_CHANNEL_MODEL_CDL_E_,
  _FADING_CHANNEL_MODEL_FLAT_FADING_,
  _FADING_CHANNEL_MODEL_CUSTOM_

};

enum ChannelGenerationMethod_E{
  _CHANNEL_GENERATION_METHOD_UNINIT_=-1,
  _CHANNEL_GENERATION_METHOD_JAKES_,
  _CHANNEL_GENERATION_METHOD_MOD_JAKES_,
  _CHANNEL_GENERATION_METHOD_SMITH_,
  _CHANNEL_GENERATION_METHOD_SCM_,
  _CHANNEL_GENERATION_METHOD_CDL_
};

enum FadingType_E{
  _FADING_TYPE_UNINIT_=-1,
  _FADING_TYPE_CORRELATED_,
  _FADING_TYPE_UNCORRELATED_,
  _FADING_TYPE_AWG_
};

enum AntennaCorrelationType_E{
  _ANTENNA_CORRELATION_UNCORRELATED_, // UNCORRELATED=>LOW as per 36.101
  _ANTENNA_CORRELATION_MEDIUM_,
  _ANTENNA_CORRELATION_HIGH_
};

enum LSPParameter{
  _UNINIT_LSP_ = -1,_DS_,_ASD_,_ASA_,_SF_,_K_,_ZSA_,_ZSD_,_DELTA_TAU_
};

enum SCMLinkCondition{
  _UNINIT_SCMLC_ = -1,_LOS_,_NLOS_,_O2I_
};

enum SCMPropagationScenario{

  _SCM_UNINIT_=-1 ,
  
  //Related to 2D channel 3GPP TR 36.814 V9.0.0(2010-03)
  _SCM2D_URBAN_MACRO_ ,
  _SCM2D_URBAN_MICRO_ ,
  _SCM2D_RURAL_MACRO_ ,
  _SCM2D_INDOOR_HOTSPOT_,
  
  //Related to 3D channel 3GPP TR 36.873 V2.0.0(2014-03)
  _SCM3D_URBAN_MICRO_ ,
  _SCM3D_URBAN_MACRO_ ,
  
  // Related to 5G Channel _FADING_CHANNEL_MODEL_2_TAP_
  _SCM5G_URBAN_MICRO_ ,
  _SCM5G_URBAN_MACRO_,
  _SCM5G_RURAL_MACRO_,
  _SCM5G_INDOOR_OFFICE_,
  _SCM5G_INDOOR_FACTORY_,
  
  //Related to NTN
  _SCMNTN_DENSE_URBAN_ ,
  _SCMNTN_URBAN_,
  _SCMNTN_SUB_URBAN_,
  _SCMNTN_RURAL_
};

// For IMT2020 Simulations
enum ChannelModelType{
  
  _MODEL_UNINIT_ = -1,
  _MODEL_A_,
  _MODEL_B_
  
};

enum InFScenario_E {_INF_UNDEF_=-1,_INF_SL_,_INF_DL_,_INF_SH_,_INF_DH_,_INF_HH_};


enum MobilityType_E{

  _MOBILITY_TYPE_UNINIT_=-1,
  _MOBILITY_TYPE_RND_WALK_,
  _MOBILITY_TYPE_RND_DIR_,

};

/// ------------------Structs ----------------

struct SCMLargeScaleParameters_S{
  
  double ds , asd , asa , sf, k, zsa, zsd, muOffSetZOD, muZSD, ueHeight, bsHeight,deltaTau;
  
  SCMLargeScaleParameters_S(){
    
    ds=-1;
    asd=-1;
    asa=-1;
    sf=-1;
    k=-1;
    zsa=-1;
    zsd=-1;
    deltaTau=-1;
  }
//   SCMLargeScaleParameters_S(int dummy){
//     
//     ds=-1;
//     asd=-1;
//     asa=-1;
//     sf=-1;
//     k=-1;
//     zsa=-1;
//     zsd=-1;
//   }
  void load(vec largeScaleParameters)
  {
    if(largeScaleParameters.length()==5)
    {
     ds=largeScaleParameters(0);
     asd=largeScaleParameters(1);
     asa=largeScaleParameters(2);
     sf=largeScaleParameters(3);
     k=largeScaleParameters(4);    
    }
    else if(largeScaleParameters.length()==7)
    {
     ds=largeScaleParameters(0);
     asd=largeScaleParameters(1);
     asa=largeScaleParameters(2);
     sf=largeScaleParameters(3);
     k=largeScaleParameters(4); 
     zsa=largeScaleParameters(5);
     zsd=largeScaleParameters(6);
    }
    else if(largeScaleParameters.length()==9)
    {
        ds=largeScaleParameters(_DS_);
        asd=largeScaleParameters(_ASD_);
        asa=largeScaleParameters(_ASA_);
        sf=largeScaleParameters(_SF_);
        k=largeScaleParameters(_K_); 
        zsa=largeScaleParameters(_ZSA_);
        zsd=largeScaleParameters(_ZSD_);
        muOffSetZOD=largeScaleParameters(7);
        muZSD=largeScaleParameters(8);
    }
    else if(largeScaleParameters.length()==10)
    {
        ds=largeScaleParameters(_DS_);
        asd=largeScaleParameters(_ASD_);
        asa=largeScaleParameters(_ASA_);
        sf=largeScaleParameters(_SF_);
        k=largeScaleParameters(_K_); 
        zsa=largeScaleParameters(_ZSA_);
        zsd=largeScaleParameters(_ZSD_);
        muOffSetZOD=largeScaleParameters(7);
        muZSD=largeScaleParameters(8);
        deltaTau = largeScaleParameters(9);
    }
    else 
    {
      cout<<"[both:] Error .. load() in SCMLargeScaleParameters_S can be used only with vector of length 5 or length 7 ..."<<endl;
      exit(0);
    }
    
  }
  vec getAsVector(){
    
    if(zsa == -1 && zsd == -1)//2D SCM
    {
      vec output(5);
      output(0)=ds;
      output(1)=asd;
      output(2)=asa;
      output(3)=sf;
      output(4)=k;
      return(output);
    }
    else//3D Channel Model
    {
      vec output(7);
      output(0)=ds;
      output(1)=asd;
      output(2)=asa;
      output(3)=sf;
      output(4)=k;
      output(5)=zsa;
      output(6)=zsd;
      return(output);
    }
  }
    vec getVector()
    {
            vec output(10);
            output(_DS_)=ds;
            output(_ASD_)=asd;
            output(_ASA_)=asa;
            output(_SF_)=sf;
            output(_K_)=k;
            output(_ZSA_)=zsa;
            output(_ZSD_)=zsd;
            output(7)=muOffSetZOD;
            output(8)=muZSD;
            output(9)=deltaTau;
            return(output);
    }
    string print()
    {
        stringstream ss;
        //ss<<ds<<",\t"<<asd<<",\t"<<asa<<",\t"<<sf<<",\t"<<k<<",\t"<<zsa<<",\t"<<zsd<<",\t"<<muOffSetZOD<<",\t"<<muZSD;
        ss<<getVector();
        return ss.str();
    }
};

struct Location_S{

  double x,y,z;
  bool isIndoor;
  bool isPedestrian;
  bool isLowPenetrationLoss;
  bool isInBuilding;
  int LocId;			// Node specific identifier
  int floorNumber;
  double indoorDistance;
  double carLoss;
  double o2iLoss;
  Location_S():LocId ( -1)
  {
    x=NAN;y=NAN;z=NAN;floorNumber=0;isIndoor=0;isPedestrian=0;isLowPenetrationLoss=0;isInBuilding=false;
    indoorDistance=0.0;carLoss=0.0;o2iLoss = 0.0; 
  }
  Location_S(vec a):LocId ( -1)
  {
    if(a.length()!=3){cout<<"[detl:]Invalid vector to Location_S(vec)."<<endl;abort();}
    x=a(0);y=a(1);z=a(2);
    
  }
  Location_S(double a, double b, double c):LocId ( -1)
  {
    x=a;y=b;z=c;
    floorNumber=0;isIndoor=0;isInBuilding=false;
  }
  Location_S operator+(const Location_S &A)
  {
    Location_S B;
    B.x=x+A.x;
    B.y=y+A.y;	
    return B;
  }
  Location_S& operator=(const Location_S& A)
  {
    x=A.x;
    y=A.y;
    z=A.z;
    isIndoor=A.isIndoor;
    isPedestrian = A.isPedestrian;
    isLowPenetrationLoss = A.isLowPenetrationLoss;
    floorNumber=A.floorNumber;
    isInBuilding=A.isInBuilding;
    indoorDistance = A.indoorDistance;
    carLoss = A.carLoss;
    o2iLoss = A.o2iLoss;
    if(LocId == -1)
      LocId = A.LocId;
    
    return *this;
  }
  Location_S operator-(const Location_S &A)
  {
    Location_S B;
    B.x=x-A.x;
    B.y=y-A.y;
    return B;
  }
  bool operator==(const Location_S &A)
  {
    if(x==A.x && y==A.y && z==A.z && isIndoor==A.isIndoor && isInBuilding==A.isInBuilding && isPedestrian == A.isPedestrian && isLowPenetrationLoss == A.isLowPenetrationLoss )
      return true;
    else
      return false;
  }
  bool operator!=(const Location_S &A)
  {
      if(x!=A.x || y!=A.y || z!=A.z)
          return true;
      else
          return false;    
  }
  friend ostream& operator<<(ostream& os , const Location_S A)
  {
    complex<double> a(A.x,A.y );
    os<<a<<" "<<A.z;
    return os;
  }
  
  void setLocId(int id)
  {
    LocId = id;
  }
  int getLocId()
  {
    return LocId;
  }
  vec getAsVector(int dim)
  {
      vec loc(dim);
      loc(0)=x;loc(1)=y;
      if(dim==3)loc(2)=z;
      return loc;
  }
  mat getAsMatrix(int dim)
  {
      mat loc(dim,1);
      loc.set_col(0,getAsVector(dim));
      return loc;
  }
  string print()
  {
      stringstream ss;
      ss<<LocId<<" "<<x<<" "<<y<<" "<<z;
      return ss.str();
  }
  double getDistFromOrigin()
  {
      return sqrt(x*x+y*y+z*z);
  }
  void setLoc(vec loc)
  {
      x = loc(0),y = loc(1),z = loc(2);
  }
  void setLoc(mat loc)
  {
      setLoc(loc.get_col(0));
  }
};

struct Correlatedshadow_S{
  double corrDist;
  double siteToSiteCorr;
  int lGridX,lGridY;
  mat commnShdw;
};

struct NodeLinkInfo_S{

  bool isLOS;
  int id;
  int txID;
  int rxID;
  int txType;
  int rxType;
//   double o2iLoss;
//   double carLoss;
  double distance;
  bool isWrapped;
  double pathDelay;
  double linkCarrierFrequencyInHz;
  double pathLoss;
  double shadowLoss;
  // new -------
  double freeSpacePathLoss;
  double buildingLoss;
  double atmosphericLoss;
  double scintillationLoss;
  //-----------
  double shadowStdDev;
  double txPowerInDBm;
  double txAntennaGain;
  double rxAntennaGain;
  bool isForSCM;
  Location_S txLocation;
  Location_S rxLocation;
  SCMLargeScaleParameters_S scmLargeScaleParams;
  double elevationAngle;
  NodeLinkInfo_S(){

    id=-1;
    txType=-1;
    rxType=-1;
    distance=-1;
    linkCarrierFrequencyInHz = -1;
//     o2iLoss = 0.0;
    shadowStdDev = NAN;
    elevationAngle=0;
    txID=-1,rxID=-1;
  }
  bool isO2ILink()
  {
      if((txLocation.isIndoor && !rxLocation.isIndoor) || (!txLocation.isIndoor && rxLocation.isIndoor))
          return true;
      else 
          return false;
  }
  friend ostream& operator<<(ostream& os ,NodeLinkInfo_S A)
  {
      os<<A.id<<" "<<A.distance<<" "<<A.isLOS<<" "<<A.pathLoss<<" "<<A.shadowLoss<<" "<<A.freeSpacePathLoss<<" "<<A.buildingLoss<<" "<<A.atmosphericLoss<<" "<<A.scintillationLoss<<" "<<A.txAntennaGain<<" "<<A.rxAntennaGain<<" "<<A.scmLargeScaleParams.print()<<endl;return os;
  }
  void load(NodeLinkInfo_S linkInfo)
  {
      isLOS = linkInfo.isLOS;
      isWrapped = linkInfo.isWrapped;
      isForSCM = linkInfo.isForSCM;
      distance = linkInfo.distance;
      pathDelay = linkInfo.pathDelay;
      linkCarrierFrequencyInHz = linkInfo.linkCarrierFrequencyInHz;
      pathLoss = linkInfo.pathLoss;
      shadowLoss = linkInfo.shadowLoss;
      buildingLoss = linkInfo.buildingLoss;
      atmosphericLoss = linkInfo.atmosphericLoss;
      scintillationLoss = linkInfo.scintillationLoss;
      shadowStdDev = linkInfo.shadowStdDev;
      txPowerInDBm = linkInfo.txPowerInDBm;
      txLocation = linkInfo.txLocation;
      rxLocation = linkInfo.rxLocation;
//       o2iLoss = linkInfo.o2iLoss;
//       carLoss = linkInfo.carLoss;
      scmLargeScaleParams.load(linkInfo.scmLargeScaleParams.getVector());
      elevationAngle = linkInfo.elevationAngle;
  }  
};

struct Area_S{
  double xLen;
  double yLen;
  double zLen;

  Area_S(){

    xLen=-1;
    yLen=-1;
    zLen=-1;
  }
  double getVolume()
  {
        return xLen*yLen*zLen;
  }
  double getSurfaceArea(bool isTotal)
  {
        return 2*zLen*(xLen+yLen) + double(isTotal)*2*xLen*yLen;
  }
};

struct NodeGroup_S{

  ivec nodeIDs;
  string groupName;
  vec property;
  string propertyName;
  NodeGroup_S() : nodeIDs(0),property(0) {}
};

struct Neighbour_S{

  int cellID;
  Location_S cellXYLocation;
};

struct RoomSize_S{
  int length;
  int width;
  int height;

  RoomSize_S(){

	length=-1;
	width = -1;
	height=-1;
  }
};

struct SCMInfo_S{

  mat lspMuSigma;
  mat correlationDistance;
  mat dcorrForSPConsistency; // For 5G channel model spatial Consistency
  Area_S coverageArea;
  vec normGridDist;
//   bool useSOSForSpCorr;
  ACF_type acfType;

  SCMInfo_S(){

    lspMuSigma.set_size(0,0);
    correlationDistance.set_size(0,0);
    normGridDist.set_length(0);
//     useSOSForSpCorr = false;
    acfType = un_init;
  }
};

struct NodeInfo_S{

  ivec nodeList;
  bvec nodeType; //required to differentiate BS and UEs.
  ivec antennaCount;
  vec carrierFrequency;
  vec samplingFrequency;
  vec noiseVariancePerDimension;
  
  Array<sBeamParameters> beamParametersPerNode;
  Array<ivec> beamIDPerTxRU; //Beam ID to be used for each TxRU. Can be overwritten on requirement. No association of beam to TxRU assumed - Dhiv
  
  NodeInfo_S(){
    
    nodeList.set_size(0);
    nodeType.set_size(0);
    antennaCount.set_size(0);
    carrierFrequency.set_size(0);
    samplingFrequency.set_size(0);
    noiseVariancePerDimension.set_size(0);
    beamParametersPerNode.set_size(0);
    beamIDPerTxRU.set_size(0);
    
  }
  
  void initBeamParameters(int nodeID, double carrierFrequencyInHz, ivec antStructure, ivec txruMap, vec antSpacing, Array<vec> azimuthAngles, Array<vec> zenithAngles, OrientationInfo_S orientationInfo,bool nodetype ,bool reInitialize = false);
  int getNumberOfTxRU(int nodeID);
  int getNumberOfElementsPerTxRU(int nodeID);
  ivec getTXRUAntennaElements(int nodeID,int TXRUindx);
  int getTXRUCount(int nodeID, int beamID);
  ivec getTXRUCountForAllBeams(int nodeID);
  ivec getTXRUs(int nodeID, int beamID);
  int getNumberOfBeams(int nodeID);
  int getNumberOfPanels(int nodeID);
  ivec getOrientationPerPanel(int nodeID);
  int getOrientationID(int nodeID,int panelID);
  Array<ivec> getPanelIDsPerBeam(int nodeID);
  int getNumberOfOrientations(int nodeID);
  sBeamParameters getBeamParameters(int nodeID)
  {
      int nodeIndx = find(nodeList,nodeID);
      //cout << "beamParametersPerNode(nodeIndx) : "<< beamParametersPerNode(nodeIndx)<<endl;
      return beamParametersPerNode(nodeIndx);
  }
  bool isBeamParametersInitialised(int nodeID)
  {
      int nodeIndx = find(nodeList,nodeID);
      return beamParametersPerNode(nodeIndx).isInitialized;
  }
  void resetBeamParameters(ivec nodeIDs);
  
};

struct FadingParameters_S{
  
  int nTaps;
  int nSpecSamples;
  int nSinusoids;
  vec tapDel;
  vec tapGain;
  
  FadingParameters_S()
  {
    nTaps=-1;
    nSpecSamples=-1;
    nSinusoids=-1;
    tapDel.set_size(0);
    tapGain.set_size(0);
  }
};

struct ChannelInfo_S{

 // double dopplerSpread;

// Non SCM
  FadingChannelModel_E channelModel;
  string customFadParametersName;
  FadingParameters_S fadingParameters;
  FadingType_E fadingType;
  double delaySpreadTDL;

//SCM

  SCMPropagationScenario scmPropagationScenario;

 
  ChannelInfo_S()
  {
  //  dopplerSpread=-1;
    channelModel=_FADING_CHANNEL_MODEL_UNINIT_;
    customFadParametersName="";
    fadingType=_FADING_TYPE_UNINIT_;
    delaySpreadTDL=-1.0;
    scmPropagationScenario=_SCM_UNINIT_;
  }
};

struct ChannelMatrix_S{
  
  int rxNodeID;
  Array<ivec> txNodeIDs;
  vec timeStamp;
  Array< Array <Array <cmat> > > channels;
  Array<Array <Array <cvec> > > channels_forRSRP; // TXNode< <txAnt <rxAnt <taps > > > > this is for RSRP
  ChannelMatrix_S(){
    
    rxNodeID = -1;
    txNodeIDs.set_length(0);
    timeStamp.set_length(0);
    channels.set_size(0);
    channels_forRSRP.set_size(0);
  }
  
};

struct ChannelOutput_S{

  int rxNodeID;
  ivec txNodeIDs;
  Array<cvec> data;
  Array< Array <Array <cvec> > > channels;
  Array< Array <Array <cvec> > >channels_forRSRP; // <txAnt <rxAnt <taps>>> this is for RSRP
  Array<cvec> summedUpSignalPerAntennaPerTone;


  ChannelOutput_S(){
    
    txNodeIDs.set_size(0);
    data.set_length(0);
    channels.set_length(0);
    summedUpSignalPerAntennaPerTone.set_length(0);
  }
  
  void takeFFT(int fftSize){

    for(int data_cnt=0;data_cnt<data.length();data_cnt++)
    {
      data(data_cnt)=fft(data(data_cnt),fftSize)/(double)fftSize;
      data(data_cnt)=fftShift(data(data_cnt));
    }

    for(int channel_cnt=0;channel_cnt<channels.length();channel_cnt++)
      for(int d1_cnt=0;d1_cnt<channels(channel_cnt).length();d1_cnt++)
	for(int d2_cnt=0;d2_cnt<channels(channel_cnt)(d1_cnt).length();d2_cnt++)
	{
	  //! Channel won't have any scaling - 10010
	  channels(channel_cnt)(d1_cnt)(d2_cnt)=fft(channels(channel_cnt)(d1_cnt)(d2_cnt),fftSize);
	  channels(channel_cnt)(d1_cnt)(d2_cnt)=fftShift(channels(channel_cnt)(d1_cnt)(d2_cnt));
	}
  }
};

struct RestOfTransmittersInfo_S{

  ivec txNodeID;
  vec  channelGainInDB;
};

struct ModifiedJakesInfo_S{
  
  int maximumNo;
  int rxNodeID;
  ivec txNodeIDs;
  cmat walshMatrix;
  Array<cvec> tapWeights;
  ivec currentIndx;
  cvec ebetan;
  vec alphan;
  
  ModifiedJakesInfo_S(){
    
    rxNodeID=-1;
    txNodeIDs.set_length(0);
    maximumNo=-1;
    walshMatrix.set_size(0,0);
    currentIndx.set_size(0);
  }
  ModifiedJakesInfo_S(int maxNo){
    
    if((maxNo%(int)pow(2,floor(log2(maxNo)))) !=0) {cout<<"[both:] Invalid maxNo to ModifiedJakesInfo_S()..."<<endl;exit(0);}
    maximumNo=maxNo;
    walshMatrix.set_size(0,0);
    currentIndx.set_size(0);
  }
  ModifiedJakesInfo_S(int maxNo, int No){
    const double PI=3.142857143;
    if((maxNo%(int)pow(2,floor(log2(maxNo)))) !=0) {cout<<"[both:] Invalid maxNo to ModifiedJakesInfo_S()..."<<endl;exit(0);}
    if((No%(int)pow(2,floor(log2(No)))) !=0) {cout<<"[both:] Invalid No to ModifiedJakesInfo_S()..."<<endl;exit(0);}
    if(No>maxNo) {cout<<"[both:] No can't be greater than maximum No in ModifiedJakesInfo_S()..."<<endl;exit(0);}
    maximumNo=maxNo;
    walshMatrix=to_cmat(hadamard(No));
    ivec indices=getIntegers(1,No);
    ebetan=exp((std::complex<double>(0.0,1.0)* (PI/(double)No))*indices);//calculating ebetan instead of betan as this may result in some performance improvement
    alphan=((2.0*PI/((double)No))*(indices-0.5));
    currentIndx.set_size(0);
  }
  bool isLinked(int txNodeId, int rxNodeId){
    
    int txIndx=find(txNodeIDs,txNodeId);
    if(txIndx==-1 || rxNodeID!=rxNodeId)
      return(false);
    
    if(currentIndx(txIndx)>=tapWeights(txIndx).length())
      return(false);
    return(true);
  }
  cvec getTapWeights(int txNodeId, int rxNodeId ,int tapCount){
    
    int txIndx=find(txNodeIDs,txNodeId);
    if(txIndx==-1 || rxNodeID!=rxNodeId)
    {
      cout<<"[both:] Tap weights not available for the given txNode-rxNode pair in getTapWeights()..."<<endl;
      exit(0);
    }
    
    if(currentIndx(txIndx)>=tapWeights(txIndx).length())
    {
      cout<<"[both:] Trying to access expired tapWeights in Modified Jakes..."<<endl;
      exit(0);
    }
    if((currentIndx(txIndx)+tapCount-1)>=tapWeights(txIndx).length())
    {
      cout<<"[both:] Tap weights not available for the given tapCount "<<tapCount<< "from the current tap index "<<currentIndx(txIndx)<<" in tapWeights of length "<<tapWeights.length()<<endl;
      exit(0);
    }
    cvec toReturn=tapWeights(txIndx)(currentIndx(txIndx),currentIndx(txIndx)+tapCount-1);
    currentIndx(txIndx)+=tapCount;
    return(toReturn);
  }
  void refreshWalshMatrix(int No=-1){
    
    if(No==-1) No=maximumNo;
    if(No>maximumNo || (No%(int)pow(2,floor(log2(No)))) !=0)
    {
      cout<<"[both:] Unsupported No for refreshWalshMatrix()..."<<endl;
      exit(0);
    }
    if(No>walshMatrix.rows())
    {
      const double PI=3.142857143;
      walshMatrix=to_cmat(hadamard(No));
      ivec indices=getIntegers(1,No);
      ebetan=exp((std::complex<double>(0.0,1.0)* (PI/(double)No))*indices);
      alphan=((2.0*PI/((double)No))*(indices-0.5));
      
    }
  }
  void loadTapweights(ivec txNodeIds, int rxNodeId ,Array<cvec> &newTapWeights){
    
    txNodeIDs=txNodeIds;
    rxNodeID=rxNodeId;
    tapWeights=newTapWeights;
    currentIndx=zeros_i(txNodeIds.length());
  }
  
};

struct MobilityInfo_S{

  MobilityType_E mobilityType;
  double refreshDistanceForRandomWalk;
  ivec serviceNodes;
  vec directionPerServiceNode;
  vec pastTimePerServiceNode;
  vec distanceMovedPerServiceNode;
};

struct ChannelGainInfo_S{

  ivec linkedNodeIDs;
  vec uplinkGainsInDB;
  vec downlinkGainsInDB;
  bvec isLOS;

  ChannelGainInfo_S(){

    linkedNodeIDs.set_length(0);
    uplinkGainsInDB.set_length(0);
    downlinkGainsInDB.set_length(0);
    isLOS.set_length(0);
    
  }

};

struct BuildingInfo_S{
  
  bool isBuildingsAvailable;
  int nBuildings;
  bool isBuildingsPlanned;
  bool isPlannedBuildingLocationsforASector;
  bool isFixedFloors;
  bool enableWallCountForPathloss;
  Array<Location_S> plannedLocations;
  int indoorWallSpacing;
  int maximumFloors;
  RoomSize_S roomSize;
  double ceilingLossInDB;
  double thickWallLossInDB;
  double thinWallLossInDB;
  Array<mat> floor3DModel;
  Area_S buildingArea;
  
  // Buildings
  ivec floorPerbuilding;
  Array<Location_S> buildingLocations;
  
  
  BuildingInfo_S(){
    
    isBuildingsAvailable=false;
    nBuildings=0;
    isBuildingsPlanned=false;
    isPlannedBuildingLocationsforASector=false;
    isFixedFloors=false;
    plannedLocations.set_length(0);
    indoorWallSpacing=-1;
    maximumFloors=-1;
    
    floorPerbuilding.set_length(0);
    buildingLocations.set_length(0);
    
    ceilingLossInDB=12;
    thickWallLossInDB=12;
    thinWallLossInDB=5;
    
  }
  void init(string configfile, bool createbuildings=true);
};

ACF_type getACF(string acf,int nCoeff);
#endif

