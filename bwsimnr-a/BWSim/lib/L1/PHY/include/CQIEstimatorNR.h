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

#ifndef _CQI_EST_NR_H_
#define _CQI_EST_NR_H_

#include "CQIEstimator.h"
#ifdef ENABLE_COUT
#define CSI_DEBUG
#endif

struct CSI_Info_S{
  
  int nLayers;
  int nCodeWords;
  
  int PMI;
  type1PMI_S type1PMI;
  
  ivec CQIindexPerCW;
  vec effSINRsPerCW;
  Array<vec> sinrPerCodeword;
  
  ivec nLayersPerCW;
  Array<ModulationScheme_E> modSchemePerCW;
  Array< map<ModulationScheme_E,double> > MMIBperCW;
  int layerIndicator;
  
  double specEff;
  
  CSI_Info_S(){
    
    nLayers = -1;
    nCodeWords = -1;
    PMI = -1;
    
    CQIindexPerCW.set_size(0);
    effSINRsPerCW.set_size(0);
    sinrPerCodeword.set_size(0);
    
    nLayersPerCW.set_size(0);
    modSchemePerCW.set_size(0);
    MMIBperCW.set_size(0);
    layerIndicator=-1;
    
    specEff = 0.0;
    
  }
  void print()
  {
        cout<<"nLayers: "<<nLayers<<" nCodewords: "<<nCodeWords<<" PMI: "<<PMI<<" CQIindexPerCW: "<<CQIindexPerCW;
        cout<<" specEff: "<<specEff<<endl;
  }
  
  void initCSI_Info(int nCodeWordsTemp, int nLayersTemp){
    
    nLayers = nLayersTemp;
    nCodeWords = nCodeWordsTemp;
    
    CQIindexPerCW.set_size(nCodeWords);
    effSINRsPerCW.set_size(nCodeWords);
    
    nLayersPerCW.set_size(nCodeWords);
    modSchemePerCW.set_size(nCodeWords);
    MMIBperCW.set_size(nCodeWords);
    sinrPerCodeword.set_size(nCodeWords);
  }
  
  ivec modOrderPerCW(){
    
    ivec modOrder(nCodeWords);
    for(int icw = 0; icw < nCodeWords; icw++)
      modOrder(icw) = (int)modSchemePerCW(icw);
    
    return modOrder;
  }
  
};

struct CQI_Info
{
    bool isCQIComputed;
    int subframeNumber;
    CSI_Info_S csiInfo;
    double computationTime;
    CQI_Info()
    {
        subframeNumber=-1;isCQIComputed=false;
    }
    CQI_Info(CSI_Info_S CSIInfo,int SFN,bool p)
    {
        csiInfo = CSIInfo;subframeNumber=SFN;isCQIComputed=p;
    }
    void clear()
    {
        isCQIComputed=false;subframeNumber=-1;computationTime=0.0;csiInfo = CSI_Info_S();
    }
};

struct Link_Param_S
{
    TransmissionScheme_E txScheme;
    DemodulationScheme_E demodulationScheme;
    PortToAntennaMapper_S* portToAntennaMapper;
    double myTxSCPowerInDB;
    int rbNumber;
    int nRBs;
    int nREsPerRB;
    bool enableCQI0;
    bool enableLayerIndication;
    TypeIMethod typeIMethod;
    Link_Param_S()
    {
        txScheme=_TRANSMISSION_SCHEME_NR_CL_;demodulationScheme=_DEMODULATION_SCHEME_MMSE_;portToAntennaMapper=new PortToAntennaMapper_S();
        typeIMethod =_SPECTRAL_EFFICIENCY_;enableCQI0=false;myTxSCPowerInDB=0;
        rbNumber=-1;nRBs=-1;nREsPerRB=-1;enableLayerIndication = false;
    }
    Link_Param_S(TransmissionScheme_E tx,DemodulationScheme_E demod,PortToAntennaMapper_S* port,double txP,int rb,int RBs,int REs,bool CQI0,TypeIMethod pmiMethod)
    {
        txScheme = tx;demodulationScheme = demod;portToAntennaMapper=port;myTxSCPowerInDB=txP;
        rbNumber=rb;nRBs=RBs;nREsPerRB=REs;enableCQI0=CQI0;typeIMethod= pmiMethod;enableLayerIndication = false;
    }
};

struct CSI_Param_S
{
    vec spectralEfficiencyOverPMIs;
    Array<CSI_Info_S> csiInfoPerPMI;
    Array<Array<vec>> sinrPerPMI;
    Array<cmat> Codebook;
    Link_Param_S* linkParam;
    int maxIndex;
    ivec layerIndicatorPerPrecoder;
    Array<ivec> cqiIndexPerCWPerPrecoder;
    CSI_Param_S(Link_Param_S* LinkParam)
    {
        spectralEfficiencyOverPMIs.set_length(0);
        csiInfoPerPMI.set_length(0);
        sinrPerPMI.set_length(0);
        Codebook.set_length(0);
        layerIndicatorPerPrecoder.set_length(0);
        cqiIndexPerCWPerPrecoder.set_length(0);
        linkParam=LinkParam;
        maxIndex=-1;
    }
};


class CQIEstimatorNR : public CQIEstimator
{
  
private:
  
  ivec CSIRSPorts;
  int nCSIRSPorts;
  ivec portReferenceNumbers;
  ivec portLocations;
  int reCountPerRb;
  imat rbRSPattern;
  
  Array<cmat> entireRawChannel;
  Array<cmat> perToneInterferenceCovariance;
  
  cmat avgWidebandChannel;
  double RSRP;
  CQI_Info CQIInfo;
  
  int LoopsforWideband;
  
public:
  CQIEstimatorNR();
    
  void configureCQIEstimatorNR(ivec cqiPorts,FrameStructure& frameStructure);
  CSI_Info_S computeCQIPerCodewords(Array<vec>& sinrPerCodeword,int rbCount,int nREsPerRB,TransmissionScheme_E txScheme,int nLayers, bool enableCQI0);
  CSI_Info_S getBestPMIandCQI(Array<Array<vec> > sinrPerPMI,int rbCount,int nREsPerRB,TransmissionScheme_E txScheme,int nLayers, bool enableCQI0 );
  CSI_Info_S getBestPMIandCQI(Array<Array<vec> > sinrPerPMI,int rbCount,int nREsPerRB,TransmissionScheme_E txScheme,int nLayers, bool enableCQI0,vec& spectralEfficiencyOverPMIs,Array<ivec>& cqiIndexPerCWPerPrecoder,Array<CSI_Info_S>& csiInfoPerPMI);
  
  int computeRIForDownlinkRankReportNR(CQIMeasurementConfig_S &cqiMeasurementConfig,FrameStructure& frameStructure, DemodulationScheme_E& demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper,ReferenceSignal_S *refSignal, ResourceMapper* resourceMapper,Array<cmat> &rxFrame, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB,double noiseVariance, double restOfInterferenceInDB, int serviceNodeID,int eNBID, int subframeNumber,string WideBandCQIlogsFileName,string RIlogsFileName,bool isAperiodicComputation);
  int computeDLRankForTypeII(CQIMeasurementConfig_S &cqiMeasurementConfig,FrameStructure& frameStructure, DemodulationScheme_E& demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, Array< cmat >& rxFrame, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, double noiseVariance, double restOfInterferenceInDB, int ServiceNodeID, int subframeNumber, string WideBandCQIlogsFileName, string RIlogsFileName, bool isAperiodicComputation);
  void computeDLAperiodicCSIForTypeI(DLCQIMeasurement_S& perUserFeedback, FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, Array< cmat >& rxFrame, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, CQIMeasurementConfig_S& cqiMeasurementConfig, double noiseVariance, double restOfInterferenceInDB, int nodeID,int eNBID, int subframeNumber, string AperiodicCQILogsFileName);
    void computeDLAperidicCSIForTypeIForReci(DLCQIMeasurement_S& perUserFeedback, FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, Array< cmat >& rxFrame, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, CQIMeasurementConfig_S& cqiMeasurementConfig, double noiseVariance, double restOfInterferenceInDB, int nodeID, int subframeNumber, string AperiodicCQILogsFileName, ReciprocityPrecodersInfo_S reci);
  void computeDLAperiodicCSIForTypeIMultiPanel(DLCQIMeasurement_S& perUserFeedback, FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, Array< cmat >& rxFrame, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, CQIMeasurementConfig_S& cqiMeasurementConfig, double noiseVariance, double restOfInterferenceInDB, int nodeID,int eNBID, int subframeNumber, string AperiodicCQILogsFileName);
  void computeDLAperiodicCSIForTypeII(DLCQIMeasurement_S& perUserFeedback, FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, Array< cmat >& rxFrame, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, CQIMeasurementConfig_S& cqiMeasurementConfig, double noiseVariance, double restOfInterferenceInDB, int nodeID,int eNBID, int subframeNumber, string AperiodicCQILogsFileName);
  void computeDLAperiodicCSIForTypeII_OMP(DLCQIMeasurement_S& perUserFeedback, FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, Array< cmat >& rxFrame, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, CQIMeasurementConfig_S& cqiMeasurementConfig, double noiseVariance, double restOfInterferenceInDB, int nodeID,int eNBID, int subframeNumber, string AperiodicCQILogsFileName);
  void computeDLAperidicCSIForTypeII_Avik(DLCQIMeasurement_S& perUserFeedback, FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, Array< cmat >& rxFrame, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, CQIMeasurementConfig_S& cqiMeasurementConfig, double noiseVariance, double restOfInterferenceInDB, int nodeID,int eNBID, int subframeNumber, string AperiodicCQILogsFileName);
  
  Array < Array<vec > > computeSinrPerCWForCodeBookNR(TransmissionScheme_E txScheme, double myTxSCPowerInDB, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, int rbNumber, int nLayers, int nCodewords, int NumOfRbs, bool isDownlink, Array< cmat >& tCodeBook);
  
  Array<vec> calculateSinrPerCodewordNR(int nLayers, int nCodeWords, TransmissionScheme_E txScheme, DemodulationScheme_E demodulationScheme, Array< cmat >& myPrecodedChannel, double sigpowInWatts, cmat estimatedIntrCovariance);
  
  Type2coefficients getBestAmplitudeAndPhase(Array< Type1HalfBeamWithi1 >& type1halfBeamswithi1, int &Lbeams, int nPSK, DemodulationScheme_E demodulationScheme, double sigpowInWatts, int nLayers, int rbNumber, int DLRBs);
  Type2coefficients_OMP_S getType2OMPCoefficients(const int Lbeams, const int nLayers, const int nPSK); 
  type1PMI_S computeWideBandPMI(cmat channel, int nLayers);
  Type2coefficients_OMP_S getType2Avik(const int Lbeams, const int nLayers, const int nPSK); 
  
  //Functions used for SearchFree
  int computei13(int k1,int k2,int nLayers);
  
  CSI_Info_S computei2forSubband(int nLayers,type1PMI_S precIndex,Link_Param_S* linkParam);
  CSI_Info_S computeCSIinfoForMultipanelMode2(vec Thetas,int nLayers,type1PMI_S precIndex,Link_Param_S* linkParam);
  CSI_Info_S computeWBCSIinfoUsingSearchFree(int nLayers,Link_Param_S* linkParam);
  CSI_Info_S computeCSIinfoOfPrecoderFromThetas(vec Thetas,int nLayers,int CBmode,CBParam_S CBparam,Link_Param_S* linkParam,bool enable=false);
  
  vec getThetasFromPMIIndices(type1PMI_S precIndex,bool special,int CBtype,int CBmode,CBParam_S CBparam,int nLayers);
  
  CSI_Info_S getBestPMIandCQIOverWideband(Link_Param_S* linkParam,int nLayers);
  double computePrePSINRandConditionNumberindB(int maxRank,vec& avgConditionNumber);
  CSI_Info_S getCSIinfoOfBestPrecoder(int nLayers,Array<cmat>& codebook,Link_Param_S* linkParam);
  CSI_Info_S getCSIinfoOfBestPrecoderUsingAvgChannel(int nLayers,Array<cmat> codebook,Link_Param_S* linkParam);
  CSI_Param_S* computeCSIparamsForCodebook(int nLayers,Array<cmat> codebook,Link_Param_S* linkParam);
  
  //Functions Used for Computing i1
  type1PMI_S getBestI1();
  type1PMI_S getBestI1forSinglePanel();
  type1PMI_S getBestI1forMultiPanel();
  
  void clear()
  {
    entireRawChannel.set_length(0);
    perToneInterferenceCovariance.set_length(0);
  }  
};

vec getAmplitude(Array< Type1HalfBeamWithi1 >& type1halfBeamswithi1, Array< cmat > halfChannel);
Array< ivec > getLayerIndication(Array< vec >& ppSINR, int nCodewords, int nLayers, bool isPerCodeword = false);
int getLayerIndicatorNR(Array<vec>& sinrPerCodeword,ivec& CQIindexPerCW,int nCodewords,int nLayers);
ivec getLbestPMIIndices(Array< Type1HalfBeamWithi1 > type1halfBeamswithi1, vec amplitudesOverBeams, int L);
ivec getBestBeams(Array< Type1HalfBeamWithi1 > type1halfBeamswithi1, vec amplitudesOverBeams, int &L);
Array< Array< ivec > > getLayerIndicationPerCWPerPrecoder(Array<Array<vec>>& sinrPerCWPerPrecoder,int nCodewords,int nLayers);

vec computeCoarseThetas(cmat V,int nLayers,ivec N);
vec computeThetas(cmat V,int nLoops,int nLayers,ivec N);
vec computeThetaN(cmat V,int nLayers,type1PMI_S precIndex,CBParam_S CBparam,vec Theta,ivec N);
Array<cmat> getSubVectors(cmat V,int nLayers,ivec N);
Array<mat> getBetas(vec Thetas,int nLayers,ivec N);

#endif
