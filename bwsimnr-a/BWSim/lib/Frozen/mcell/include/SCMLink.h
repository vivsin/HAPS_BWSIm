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

#ifndef _SCM_H_
#define _SCM_H_

#include "Antenna.h"
#include "SupportingFunctions.h"

enum SCMType
{
    _SCMTYPE_UNINIT_=-1,
    _SCMTYPE_2D_,
    _SCMTYPE_3D_,
    _SCMTYPE_5G_,
    _SCMTYPE_NTN_
};

/**
 * @brief 2DSCMLink Class which manages input buffer for each transmitter and performs either a dot-product (freq-domain) or time-domain convolution of input data and channel modeling Inter-Symbol-Interference
 */
class SCMSpatialConsistency
{
public:
    Array< Array<imat> > scmGridsForSpConsist;
    mat dcorrForSPConsistency;
    Area_S coverageArea;
    SCMSpatialConsistency()
    {
        scmGridsForSpConsist.set_length(0);
        dcorrForSPConsistency.set_size(0,0);
        coverageArea = Area_S();
    }
};

class MobilityModeling
{
    
public:
    Location_S t0TxLoc;
    Location_S t0RxLoc;
    
    vec tau_n_tilde_kminus1;
    vec tau_n0;
    
    int k;
    mat XnMat; 
    
    Array<mat> v_bar_dash_rx;    
    Array<mat> v_bar_dash_tx; 
    
    MobilityModeling()
    {
        k=0;
        XnMat=eye(3); // Need to be generated as a uniform random variable (as per 38.901-h00-page 54) - Dhiv
    }
};


// class SCM2DLink{
//     
// public:
//     double txLosAntGain,rxLosAntGain,txAvgAntGain,rxAvgAntGain;
//     bool addPathSpecificAntennaGain2D;
//     SCMPropagationScenario propagationScenario2DChannel;
//     bool isReady;
//     int isLOS;
//     int isOutdoorToIndoor;
//     double dopplerSpreadInHz;
//     double carrierFrequencyInHz;
//     double mobileVelocity;
//     int nClusters;
//     vec txAntennaSpacing, rxAntennaSpacing;
//     vec tapPow;
//     vec tapDelays;
//     vec pathAoD;
//     vec pathAoA;
//     Array<ivec> subpathAoA_association_Index,subpathPhase_fixedpt;
//     Array<vec> subpath_Boresight_AoD_angle,subpath_Boresight_AoA_angle;
//     Array<vec> subpathAoDAntennaGain,subpathAoAAntennaGain;
//     ivec SubPathtypeinCluster,aMap;
//     double XPR_lin;
//     double theta_Tx,theta_Rx,phi_Tx,phi_Rx,K_rice;
//     double v_phase;
//     
//     bool txCrossPolarizationFlag;
//     vec txPolarizationAngleInDegree;
//     bool rxCrossPolarizationFlag;
//     vec rxPolarizationAngleInDegree;
//     
// public:
//     
//     SCM2DLink();
//     //SCM2DLink(int dummy);
//     bool isInitialized(); ///< Returns whether the SCMLink is initialized
//     void diablePathSpecificAntennaGain(); 	///< Diables having different antenna gain for each multipath
//     void enablePathSpecificAntennaGain();	///< Enables having different antenna gain for each multipath
//     double initialize2DFadingParams(int  isOutdoortoIndoor,bool isLineOfSight,double dopplerSpreadInHz,double carrierFrequencyInHz,SCMPropagationScenario propScenario,SCMLargeScaleParameters_S &scmParameters,Location_S nodeLocationTx, Location_S nodeLocationRx,Antenna &txAntenna, Antenna &rxAntenna,double txAntennaSpacing,double rxAntennaSpacing) ; ///< Intialize the 2DSCM link with the parameters
//     cvec generate2DSCMTapGains(int currentTxAntenna, int currentRxAntenna,double currentTimeInSeconds); ///< Generate the SCM channel in the cirrent time for the Tx-Rx antenna pair
//     double get2DSCMLOSAntennaGainInDB(); ///< Returns the LOS angle antennaGain (need not be a LOS link)
//     vec get2DSCMTapDelays();///< Returns tap delays of the link
//     void computeAntennaGains(Antenna &txAntenna, Antenna &rxAntenna);	///< Computes antenna gains for all AoD and AoA
//     
// private:
//     
//     void initializeChannelModel(vec v2_LSP,Location_S nodeLocationTx, Location_S nodeLocationRx,Antenna &txAntenna, Antenna &rxAntenna,double txAntSpacing, double rxAntSpacing);   ///< Initialize the link
//     void generatePowerDelayProfiles(int N,double sigma_DS,vec& tapPow,vec& tapdelays); ///< Generates the power-delay profile
//     double getClusterShadow();
//     double getDelayScaling();
//     vec generatePathAOAs(vec tpow,double sigma_AOA); 	///< Generates AoAs
//     vec generatePathAODs(vec tpow,double sigma_AOD);	///< Generates AoDs
//     vec generateClusterAngles(vec tpow, double sigma_A);	///< Generates cluster distribution angles
//     int getClusterCount();	///< Returns cluster count
//     Array<ivec> associateBSMSpaths(int N,int M);	
//     void getBoresight_LOS_angle(double& thetaTx, double& thetaRx, double& phiTx, double& phiRx, Location_S nodeLocationTx, Location_S nodeLocationRx); ///< Returns the LOS angle of the two locations
//     void generateSubClusters(int M);	///< Generates subclusters
//     ivec getSubPathIndicesofClusters(int setid);	///< Return cluster sub-path Indices
//     Array<ivec> generateSubpathPhases(int N);	///< Returns sub-path phases
//     double getCrossPolarizationRatio();
//     void computeBoresight2subpath_angles(int nTaps, int nRays);
//     vec generateSubpathAngleOffsets(int M,int  isAoA); // isAoA: 0=> AoD, 1=> AoA
//     double getClusterRMSAzimuthSpread(int isAoA);
//     double measureDelaySpread(vec p, vec t, double K, int isLOS);
//     double measureCircularAngularSpread(Array<vec> a,vec p,ivec SubPathtypeinCluster,double K,int isLOS,double thetaLOS);
//     
// };
// 
// class SCM3DLink{
//     
// public:
//     bool addPathSpecificAntennaGain3D;
//     double txLos3DAntGain,rxLos3DAntGain,txAvg3DAntGain,rxAvg3DAntGain;
//     SCMPropagationScenario propagationScenario3DChannel;
//     bool isReadyFor3DChannel;
//     int isLOS3DChannel;
//     int isOutdoorToIndoor3DChannel;
//     //     int antElementsPerCol;
//     double antennaSpacing;
//     double dopplerSpreadInHz3DChannel;
//     double carrierFrequencyInHz3DChannel;
//     double mobileVelocity3DChannel;
//     int nClusters3DChannel;
//     vec tapPow3DChannel;
//     vec tapDelays3DChannel;
//     vec pathAoD3DChannel;
//     vec pathAoA3DChannel;
//     vec pathZoD3DChannel;
//     vec pathZoA3DChannel;
//     double muOffSetZOD;
//     double muZSD;
//     double sectorTilt;
//     double txHeight, rxHeight;
//     
//     //AAS
//     ivec txAntStructure; //[Mg Ng M N P]
//     ivec rxAntStructure;
//     int txNumberOfTxRUs;
//     int rxNumberOfTxRUs;
//     int totalTxAntElem;
//     int totalRxAntElem;
//     
//     cvec antWeights;
//     cvec rxAntWeights; //AAS
//     
//     int txNumOfTxRUs; // Transmitter TXRUs
//     int rxNumOfTxRUs; // Receiver TXRUs    
//     double txVerAntElementSpacing, txHorzAntElementSpacing, rxVerAntElementSpacing, rxHorzAntElementSpacing;
//     double txVerAntPanalSpacing, txHorzAntPanalSpacing, rxVerAntPanalSpacing, rxHorzAntPanalSpacing;
//     
//     
//     Array<ivec> subpath3DAoA_association_Index,subpath3DZoA_association_Index,subpath3DZoD_association_Index,subpathPhase_3Dfixedpt;
//     Array<vec> subpath_Boresight_3DAoD_angle,subpath_Boresight_3DAoA_angle,subpath_Boresight_3DZoD_angle,subpath_Boresight_3DZoA_angle;
//     Array<vec> subpathAoDZoDAntennaGain,subpathAoAZoAAntennaGain;
//     ivec SubPathtypeinCluster3D,aMap3D;
//     double XPR_lin_3D;
//     double theta_Tx_3D,theta_Rx_3D,phi_Tx_3D,phi_Rx_3D,K_rice_3D;
//     double v_phase_3D;   
//     double h_phase_3D;
//     bool txCrossPolarizationFlag3D;
//     vec txPolarizationAngleInDegree3D;
//     bool rxCrossPolarizationFlag3D;
//     vec rxPolarizationAngleInDegree3D;
//     Array<ivec> txAntElementIndxFor3D;
//     Array<ivec> rxAntElementIndxFor3D;
//     //     cmat elevationAntWeight;
//     //     Array<cvec> elevationWeights;
//     
// public:
//     
//     SCM3DLink();
//     //     SCM3DLink(int dummy);
//     bool is3DChannelInitialized(); ///< Returns whether the 3DChannelLink is initialized
//     void diable3DChannelPathSpecificAntennaGain(); 	///< Diables having different antenna gain for each multipath
//     void enable3DChannelPathSpecificAntennaGain();	///< Enables having different antenna gain for each multipath
//     double initialize3DChannelFadingParams(int  isOutdoortoIndoor,bool isLineOfSight,double dopplerSpread,double carrierFrequency,SCMPropagationScenario propScenario,SCMLargeScaleParameters_S &channel3DParameters,Location_S nodeLocationTx, Location_S nodeLocationRx,Antenna &txAntenna, Antenna &rxAntenna ,double txAntSpacing,double rxAntSpacing); ///< Intialize the 3D Channel link with the parameters
//     //cvec generate3DChannelTapGains(int currentTxAntenna,int currentTxAntennaElement, int phyAntNum, int currentRxAntenna,double currentTimeInSeconds,cvec elevationWeights_temp); ///< Generate the 3D channel in the cirrent time for the Tx-Rx antenna pair
//     cvec generate3DChannelTapGains(int currentTxAntenna,int currentTxAntennaElement, /*int physicalAntNum*/ int currentRxAntenna,int currentRxAntennaElement,double currentTimeInSeconds,cvec elevationWeights_temp);///< Generate the 3D channel in the cirrent time for the Tx-Rx antenna pair
//     double get3DChannelLOSAntennaGainInDB(); ///< Returns the LOS angle antennaGain (need not be a LOS link)
//     vec get3DChannelTapDelays();///< Returns tap delays of the link
//     cvec generateAASChannelTapGains(int currentTxAntenna, int currentRxAntenna,double currentTimeInSeconds); ///< Generate the 3D channel in the cirrent time for the Tx-Rx antenna pair
//     void compute3DAntennaGains(Antenna &txAntenna, Antenna &rxAntenna);	///< Computes antenna gains for all AoD and AoA
//     
// private:
//     
//     void initialize3DChannelModel(vec v2_LSP,Location_S nodeLocationTx, Location_S nodeLocationRx,Antenna &txAntenna, Antenna &rxAntenna,double txAntSpacing, double rxAntSpacing);   ///< Initialize the link
//     void generate3DChannelPowerDelayProfiles(int NosOfClusters,double sigma_DS,vec& tapPow,vec& tapdelays); ///< Generates the power-delay profile
//     double get3DChannelClusterShadow();
//     double get3DChannelDelayScaling();
//     
//     vec generate3DChannelPathAOAs(int NosOfClusters,vec tpow,double sigma_AOA); 	///< Generates AoAs
//     vec generate3DChannelPathAODs(int NosOfClusters,vec tpow,double sigma_AOD);	///< Generates AoDs
//     vec generate3DChannelPathZOAs(int NosOfClusters,vec tpow,double sigma_ZOA); 	///< Generates ZoAs
//     vec generate3DChannelPathZODs(int NosOfClusters,vec tpow,double sigma_ZOD);	///< Generates ZoDs
//     
//     vec generateAzimuth3DChannelClusterAngles(vec tpow, double sigma_A);	///< Generates cluster distribution angles
//     vec generateZenith3DChannelClusterAngles(vec tpow, double sigma_Z,bool isZOD);	///< Generates cluster distribution angles
//     
//     int get3DChannelClusterCount();	///< Returns cluster count
//     Array<ivec> associate3DChannelBSMSpaths(int nTaps,int NosOfRays);	
//     void getBoresight_LOS_3Dangle(double& thetaTx, double& thetaRx, double& phiTx, double& phiRx, Location_S nodeLocationTx, Location_S nodeLocationRx, double sectorTilt); ///< Returns the LOS angle of the two locations
//     void generate3DChannelSubClusters(int M);	///< Generates subclusters
//     ivec get3DSubPathIndicesofClusters(int setid);	///< Return cluster sub-path Indices
//     Array<ivec> generate3DSubpathPhases(int N);	///< Returns sub-path phases
//     double get3DCrossPolarizationRatio();
//     void computeBoresight2subpath_3Dangles(int nTaps, int nRays);
//     vec generate3DSubpathAngleOffsets(int M,int  isAzimuth_Zenith); // isAzimuth_Zenith: 0=>AoD, 1=>AoA, 2=>ZoA, 3=>ZoD
//     double getCluster3DRMSAzimuthSpread(int isAzimuth_Zenith);
//     //  double measureDelaySpread3D(vec p, vec t, double K, int isLOS);
//     //  double measureCircularAngularSpread(Array<vec> a,vec p,ivec SubPathtypeinCluster,double K,int isLOS,double thetaLOS);
//     
// };

class SCM5GLink{
    
public:
    SCMSpatialConsistency *spConsistPtr;
    MobilityModeling mobiltyModeling;

    ChannelModelType channelModelType;
    SCMLinkCondition scmLinkCondition;
    vec txLos5GFieldAmplitude,rxLos5GFieldAmplitude,txAvg5GAntGain,rxAvg5GAntGain;    //Antenna Gains across orientations.
    bool isFlatFadedChannel;
    bool addOxygenLoss;
    double dopplerSpreadInHz5GChannel,mobileVelocity5GChannel;
    double carrierFrequencyInHz5GChannel;
    vec txVelocityVector,rxVelocityVector;
    bool useGroundReflection;
    int groundReflectedTap;
    
    Antenna txAntenna;
    Antenna rxAntenna;
    ivec zodAssociation , aoaAssociation , zoaAssociation;
    
    
private:
    SCMPropagationScenario propagationScenario5GChannel;
    int scmGridIndx;
    bool isSpConsistEnable;
    bool isReadyFor5GChannel;
    int isLOS5GChannel;
//     int isOutdoorToIndoor5GChannel;
//     double antennaSpacing;
    int nClusters5GChannel, nTaps5GChannel,nRaysPerCluster;
    vec tapPow5GChannel,tapDelays5GChannel;
    vec pathAoD5GChannel,pathAoA5GChannel,pathZoD5GChannel,pathZoA5GChannel;
    double muOffSetZOD;
    double muZSD;
    double txHeight, rxHeight;
    double d3D;
//     double sectorTilt;
    ivec strong2;
    //SP module
    Location_S linkStartLoc;
    Location_S linkEndLoc;
    //double UE_sigma_SF;
    
    int txNumOfTxRUs,rxNumOfTxRUs;
    ivec txAntStructure,rxAntStructure; //[Mg Ng M N P]
    ivec txTXRUMap,rxTXRUMap;
    vec txAntSpacing,rxAntSpacing;
    bool txCrossPolarizationFlag5G,rxCrossPolarizationFlag5G;
    
    //Array<ivec> subpath5GAoA_association_Index,subpath5GZoA_association_Index,subpath5GZoD_association_Index;
    Array<Array<vec>> subpath_Boresight_5GAoD_angle,subpath_Boresight_5GAoA_angle,subpath_Boresight_5GZoD_angle,subpath_Boresight_5GZoA_angle;
    Array<vec> subpathAOD,subpathZOD,subpathAOA,subpathZOA;
    Array<vec> subpathPhase_5Gfixedpt;
    
    Array<Array<vec> > subpath5GAoDZoDAntennaFieldAmplitude,subpath5GAoAZoAAntennaFieldAmplitude;
    
    ivec SubPathtypeinCluster5G,aMap5G;
    mat XPR_lin_5G;
    double theta_Tx_5G,theta_Rx_5G,phi_Tx_5G,phi_Rx_5G,K_rice_5G;
    vec thetaTxLos5G,thetaRxLos5G,phiTxLos5G,phiRxLos5G;      //LOS angles for all orientations.
    double losPhase5G; 
    //double ue_phase_5G
    
    //   Array<Array<mat > > scmGridsForSpConsist; // Spacial Consistency grid <floor <parameters <grid> > >
    //   mat dcorrForSPConsistency;
    //   Area_S coverageArea;
    
    Array<mat> txLocationVectorPerAntennaElement,rxLocationVectorPerAntennaElement;
    vec txPolarizationAngleInDegreePerAntennaElement,rxPolarizationAngleInDegreePerAntennaElement;
    vec oxygenLossPerClusterIndB;double minTau,delTau;
    
    vec txGround5GFieldAmplitude,rxGround5GFieldAmplitude;
    vec thetaTxGround5G,thetaRxGround5G,phiTxGround5G,phiRxGround5G;

    
    
public:
    
    SCM5GLink();
    bool is5GChannelInitialized(); ///< Returns whether the 5GChannelLink is initialized
    void disable5GChannelPathSpecificAntennaGain(); 	///< Diables having different antenna gain for each multipath
    void enable5GChannelPathSpecificAntennaGain();	///< Enables having different antenna gain for each multipath
    double initialize5GChannelFadingParams(int isOutdoortoIndoor, bool isLineOfSight, double dopplerSpread, double carrierFrequency, SCMPropagationScenario propScenario, SCMLargeScaleParameters_S& channel5GParameters, Location_S nodeLocationTx, Location_S nodeLocationRx, Antenna* txAntennaPtr, Antenna* rxAntennaPtr); ///< Intialize the 5G Channel link with the parameters
    void initialize5GChannelModel(SCMLargeScaleParameters_S& channel5GParameters,Location_S nodeLocationTx, Location_S nodeLocationRx,Antenna *txAntennaPtr, Antenna *rxAntennaPtr);   ///< Initialize the link
    void initialize5GChannelModelFromStep9();
    cvec generate5GChannelTapGainsTxEtoRxE(int currentTxAntennaElement,int currentRxAntennaElement, double currentTimeInSeconds,bool isChannelGenForRSRP ,int txOrientationId , int rxOrientationId);
    
    int getNumberOfTaps();
    vec get5GChannelTapDelays();   ///< Returns tap delays of the link
    vec get5GChannelTapPowers();
    vec get5GChannelClusterPowers();
    vec get5GChannelClusterDelays();
    double getRMSDelaySpread(double& mu);
    string getClusterSpreads();
    string getAngleSpreads();
    string getLOSAnglesAndLinkLocations();
    
    //  double measureDelaySpread5G(vec p, vec t, double K, int isLOS);
    //  double measureCircularAngularSpread(Array<vec> a,vec p,ivec SubPathtypeinCluster,double K,int isLOS,double thetaLOS);
    
    //SP module
    void getSpatiallyConsistentClusterDelayAndPower(vec &Xn, vec &Zn);
    void getSpatiallyConsistentClusterSignAndOffset(int length, vec& Xn, vec& Yn, unsigned int seedOffsetForSpatialConsistency);
    void getSpatiallyConsistentRayCoupling(ivec idx , ivec &zodAssociation, ivec &aoaAssociation, ivec &zoaAssociation);
    void getSpatiallyConsistentXPR(int N, int nRaysPerCluster, mat &XPR);
    void getSpatiallyConsistentPhase(int nRaysPerCluster, vec &subpathPhase , double &losPhase);
    
    void compute5GAntennaGains();	///< Computes antenna gains for all AoD and AoA
    void generate5GSubpathPhases();	///< Returns sub-path phases
    int getNumberOfClusters();
    void set(SCM5GLink* link);
    double getDopplerPhase(mat r_tx,mat r_rx,double currentTimeInSeconds,bool isLOS);
    double getDopplerSpreadInHz();
    void reInitChannelForFDDReciprocity(double carrierFrequency);
    
    
    //Mobility Functions  - 38.901 - 7.6.3
    void update5GChannelPowerDelayProfiles(double sigma_DS, double deltaT);
    void update5GChannelArrivalAndDepartureAngles(double deltaT);
    void update5GChannelForMobility(Location_S nodeLocationTx, Location_S nodeLocationRx, double sigma_DS, double deltaT);
    
    
private:
    void generate5GCrossPolarizationRatios();
    void generate5GChannelPowerDelayProfiles(double sigma_DS); ///< Generates the power-delay profile
    
    void getBoresight_LOS_3Dangle(Location_S nodeLocationTx, Location_S nodeLocationRx); ///< Returns the LOS angle of the two locations
    vec generate5GSubpathAngleOffsets(int M,int  isAzimuth_Zenith); // isAzimuth_Zenith: 0=>AoD, 1=>AoA, 2=>ZoA, 3=>ZoD
    vec generateClusterAngles(vec& tapPow,double sigma,double C,bool isAzimuth,bool isLOS, unsigned int seedOffsetForSpatialConsistency=0);
    vec generateAzimuth5GChannelClusterAngles(double sigma_A, unsigned int seedOffsetForSpatialConsistency=0);	///< Generates cluster distribution angles
    vec generateZenith5GChannelClusterAngles(double sigma_Z, unsigned int seedOffsetForSpatialConsistency=0);	///< Generates cluster distribution angles
    void computeBoresight2subpath_5Gangles();
    cmat generateRotationMatrix(int clusterIndx, int rayIndx);
    void generate5GChannelSubClusters();	///< Generates subclusters
    ivec get5GSubPathIndicesofClusters(int setid);	///< Return cluster sub-path Indices
    
    inline dComplex getPathGain(double txFieldAmplitude,double rxFieldAmplitude,cmat& Rot,mat& r_tx,mat& r_rx,int currentTxAntennaElement,int currentRxAntennaElement,double currentTimeInSeconds,bool isChannelGenForRSRP,bool isForLOSPath=false);
    dComplex generate5GChannelTapGainPerClusterTxEtoRxE(int tapCnt,int currentTxAntennaElement,int currentRxAntennaElement, double currentTimeInSeconds, bool isChannelGenForRSRP ,int txOrientationId , int rxOrientationId);
    dComplex generate5GChannelTapGainForLOSClusterTxEtoRxE(int tapCnt, int currentTxAntennaElement, int currentRxAntennaElement, double currentTimeInSeconds, bool isChannelGenForRSRP ,int txOrientationId , int rxOrientationId);
    
    void computeOxygenLoss(vec tapDelays);
    double get5GRMSClusterAngleSpread(int isAzimuth_Zenith,int scmGridIndx,double muZSD,double fc);
    dComplex computeChannelTapGainForGroundReflection(int currentTxAntennaElement, int currentRxAntennaElement, double currentTimeInSeconds, bool isChannelGenForRSRP,int txOrientationId , int rxOrientationId);

    

};

class NTNSCMLink
{
public:
    bool isLOSChannel;
    bool addPathSpecificAntennaGain;
    vec txLosFieldAmplitude,rxLosFieldAmplitude,txAvgAntGain,rxAvgAntGain;    //Antenna Gains across orientations.
    bool isFlatFadedChannel;
    bool addOxygenLoss;
    double carrierFrequencyInHz;
    vec txVelocityVector,rxVelocityVector;
    SCMLinkCondition scmLinkCondition;
    bool applyFaradayRotation;
    int cnt=1;
    
    Antenna txAntenna;
    Antenna rxAntenna;
    
    vec clusterAOD,clusterZOD,clusterAOA,clusterZOA;
    Array<Array<vec>> subpath_Boresight_AoD_angle,subpath_Boresight_AoA_angle,subpath_Boresight_ZoD_angle,subpath_Boresight_ZoA_angle;
    Array<vec> subpathAOD,subpathZOD,subpathAOA,subpathZOA;
    double theta_Tx,theta_Rx,phi_Tx,phi_Rx,K_rice;
    vec tapPow,tapDelays;
    Array<Array<vec> > subpathAoDZoDAntennaFieldAmplitude,subpathAoAZoAAntennaFieldAmplitude;
    SCMLargeScaleParameters_S scmLSPs;
    vec getTapPowers();
    
private:
    SCMPropagationScenario propagationScenario;
    Location_S linkStartLoc,linkEndLoc;
    double txHeight,rxHeight;
    bool isReady;
    int nClusters,nTaps,nRaysPerCluster;
    double d3D,muZSD,sectorTilt,elevationAngle,minTau;
    int txNumOfTxRUs,rxNumOfTxRUs;
    ivec txAntStructure,rxAntStructure; //[Mg Ng M N P]
    ivec txTXRUMap,rxTXRUMap;
    vec txAntSpacing,rxAntSpacing;
    bool txCrossPolarizationFlag,rxCrossPolarizationFlag;
    Array<vec> subpathPhase_fixedpt,subpathFaradayPhase;
    ivec subPathtypeinCluster,aMap;
    mat XPR_lin;
    vec thetaTxLos,thetaRxLos,phiTxLos,phiRxLos;      //LOS angles for all orientations.
    
    Array<mat> txLocationVectorPerAntennaElement,rxLocationVectorPerAntennaElement;
    vec txPolarizationAngleInDegreePerAntennaElement,rxPolarizationAngleInDegreePerAntennaElement;
    bool isKaBandCarrier;
    int scmRowIndx,scmColIndx;
    
private:
    void initAntennaParams(Antenna* txAntennaPtr,Antenna* rxAntennaPtr);
    void initLocationParams(Location_S txLoc,Location_S rxLoc);
    
    int getClusterCount();
    double getClusterShadow();
    double getDelayScaling();
    vec getXPRMuSigma();
    double getRMSClusterDelaySpread();
    double getRMSClusterAngleSpread(int isAzimuth_Zenith);
    ivec getSubPathIndicesofClusters(int setid);
    
    void generatePowerDelayProfiles(double sigma_DS);
    vec generateAzimuthClusterAngles(double sigma_A);
    vec generateClusterAngles(vec& tapPow,double sigma,double C,bool isAzimuth,bool isLOS);
    vec generateZenithClusterAngles(double sigma_Z);
    void generateCrossPolarizationRatios();
    void generateSubpathPhases();
    void generateChannelSubClusters();
    vec generateSubpathAngleOffsets(int M,int  isAzimuth_Zenith);
    void computeBoresight2subpath_5Gangles();
    
    cmat generateRotationMatrix(int clusterIndx, int rayIndx);
    double getDopplerPhase(mat r_tx,mat r_rx,double currentTimeInSeconds,bool isLOS);
    dComplex getPathGain(double txFieldAmplitude,double rxFieldAmplitude,cmat& Rot,mat& r_tx,mat& r_rx,int currentTxAntennaElement,int currentRxAntennaElement,double currentTimeInSeconds,bool isChannelGenForRSRP,bool isForLOSPath=false);
    dComplex generateTapGainPerClusterTxEtoRxE(int tapCnt,int currentTxAntennaElement,int currentRxAntennaElement, double currentTimeInSeconds, bool isChannelGenForRSRP,int txOrientationId, int rxOrientationId);
    dComplex generateTapGainForLOSClusterTxEtoRxE(int tapCnt, int currentTxAntennaElement, int currentRxAntennaElement, double currentTimeInSeconds, bool isChannelGenForRSRP, int txOrientationId, int rxOrientationId);
public:
    NTNSCMLink();
    void set(NTNSCMLink* link);

    double getRMSDelaySpread(double& mu);
    string getClusterSpreads();
    string getAngleSpreads();

    void computeAntennaGains();
    double computeAntennaGains(double& theta); //-deep
    double initialize5GChannelFadingParams(int isOutdoortoIndoor, bool isLineOfSight, double dopplerSpread, double carrierFrequency, SCMPropagationScenario propScenario, SCMLargeScaleParameters_S& scmParams, Location_S nodeLocationTx, Location_S nodeLocationRx, Antenna* txAntennaPtr, Antenna* rxAntennaPtr); ///< Intialize the 5G Channel link with the parameters
    cvec generateTapGainsTxEtoRxE(int currentTxAntennaElement,int currentRxAntennaElement, double currentTimeInSeconds,bool isChannelGenForRSRP,int txOrientationId , int rxOrientationId);
    bool isInitialized(){return isReady;}
    vec getTapDelays();
};


class SCMLink
{
    
public:
    SCM5GLink* scm5GLink;
    //SCM2DLink* scm2DLink;
    //SCM3DLink* scm3DLink;
    NTNSCMLink* ntnSCMLink;
    SCMSpatialConsistency* spConsist;
    SCMType scmType;
public:
    //bool isLOS;// added --santhosh 
    SCMPropagationScenario propagationScenario;
    bool addPathSpecificAntennaGain;
    bool enableFlatFading;
    bool addOxygenLoss;
    bool applyFaradayRotation;
    bool useGroundReflection;
    Array<Array<Array<Array<cvec>>>> fixedTapGainsForInitialBeam; //RxAntenna<TxAntenna < Rx elements < Tx elements < taps > > >
    Location_S txLoc,rxLoc;
    
    SCMLink();
    bool isSCMChannelInitialized();
//     void setElementGains(double txGain,double rxGain){txElementGainInDB=txGain,rxElementGainInDB=rxGain;}
    
    double initializeFadingParams(int  isOutdoortoIndoor,bool isLineOfSight,double dopplerSpread,double carrierFrequency,SCMPropagationScenario propScenario,SCMLargeScaleParameters_S &scmParameters,Location_S nodeLocationTx, Location_S nodeLocationRx,Antenna *txAntenna, Antenna *rxAntenna ,double txAntSpacing,double rxAntSpacing, ChannelModelType tChannelModelType); 
    double getLOSAntennaGainInDB(int txOrientationID,int rxOrientationID); ///< Returns the LOS angle antennaGain (need not be a LOS link)
    double getAvgAntennaGainInDB(int txOrientationID,int rxOrientationID); ///< Returns the avg antennaGain
//     double getElementGainInDB(){return txElementGainInDB+rxElementGainInDB;}
    vec getTapDelays();///< Returns tap delays of the link
    cvec generateTapGains(int currentTxAntenna, int currentRxAntenna,double currentTimeInSeconds);  
    void acquireSCMGridsForSpConsist(Array< Array<imat> > GridsForSpConsist, mat dcorrMat,Area_S coverageAreaLoc);
    void printAntennaGains(ostream &os);
    void clearTapGains()
    {
        fixedTapGainsForInitialBeam.set_length(0);
    }
    void generate5GChannelTapGainsForInitialBeams (sTXRUParameters* txTXRUParameters, sTXRUParameters* rxTXRUParameters);
//     void set(SCMLink* scmLink);
    cvec generate5GChannelTapGainsForInitialBeams(int currentTxAntennaElement,int currentRxAntennaElement, double currentTimeInSeconds,bool isChannelGenForRSRP,int txOrientationId , int rxOrientationId);
    cvec generate5GChannelTapGains(int currentTxAntennaElement,int currentRxAntennaElement, double currentTimeInSeconds,bool isChannelGenForRSRP,int txOrientationId , int rxOrientationId);
    void setDoppler(vec rxVelocityVector,vec txVelocityVector);
    void computeAntennaGains();
    string getAngleAndDelaySpread();
    string getClusterSpread();
    SCMLinkCondition getSCMLinkCondition();
    
};

void calculateLocationVectorsAndPolAngles(Antenna* antennaPtr,double carrierFrequencyInHz5GChannel,Array<mat>& locationVectorPerAntennaElement, vec& polarizationAngleInDegreePerAntennaElement);
mat getStructureCentre(ivec antStructure,vec antSpacing);
vec compute5GPathSpecificAntennaGains(Antenna* antennaPtr,Array<Array<vec>>& fieldAmplitudes,Array<Array<vec>>& boreSightAzimuthAngles,Array<Array<vec>>& boreSightZenithAngles,vec& tapPow,double carrierFrequencyInHz=-1);
vec compute5GLOSAntennaGains(Antenna* antennaPtr,vec& losTheta,vec& losPhi,double theta,double phi,double carrierFrequencyInHz=-1);

vec compute5GPathSpecificAntennaGain(Antenna* antennaPtr,Array<Array<vec>>& fieldAmplitudes,Array<Array<vec>>& boreSightAzimuthAngles,Array<Array<vec>>& boreSightZenithAngles,vec& tapPow,double& thetaAngle,double carrierFrequencyInHz=-1);
vec compute5GLOSAntennaGain(Antenna* antennaPtr,vec& losTheta,vec& losPhi,double theta,double phi,double& thetaAngle,double carrierFrequencyInHz=-1);

vec getAvgAntennaGains(Array<vec>& subpathAoDZoDAntennaGain,Array<vec>& subpathAoAZoAAntennaGain,vec tapPow,int isLOS,double K_rice);
double getRMSSpread(vec param,vec tapPow,double &mu);
double getAngleSpread(vec tapPow,Array<vec> angle,double& mu);
double getAngleSpreadForInF(vec tapPow,Array<vec>& angle,double& mu);
int get5GChannelClusterCount(int scmGridIndx);
double get5GChannelDelayScaling(int scmGridIndx);
double get5GChannelClusterShadow(int scmGridIndx);
vec getXPRMuSigma(int scmGridIndx);
// double get5GRMSClusterAngleSpread(int isAzimuth_Zenith,int scmGridIndx,double muZSD,double fc);
double get5GRMSClusterDelaySpread(double fc,SCMPropagationScenario propagationScenario,ChannelModelType channelModelType,SCMLinkCondition scmLinkCondition);
SCMType getSCMType(SCMPropagationScenario scmScenario);
#endif
