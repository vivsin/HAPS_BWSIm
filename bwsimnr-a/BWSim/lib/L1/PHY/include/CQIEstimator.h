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

#ifndef _CQI_EST_H_
#define _CQI_EST_H_

#include "Precoder.h"
#include "LayerMapper.h"
#include "Equalizer.h"
#include "AWGNTables.h"
#include "ResourceMapper.h"
#include "FrameHandler.h"
#include "FrameScheduler.h"
#include "ChannelandIntrCovEstimator.h"
#include "SupportingFunctions.h"

class CQIEstimator{
  
public:
  cTBTables* tbTables;
  Codebook* mCodebook;
  bool estimateInterference;
  ChannelEstimator *channelEstimator;
  DownlinkCQIInfo_S *downlinkCQIInfo;
  UplinkCQIInfo_S *uplinkCQIInfo;
  double geometrySINRindB;
  double sinrOffsetForOLLA;
  int ackCount;
  int nackCount;
  //bool isWithAbstraction;
  Array<Array<vec> > sinrOverWideband;
  double crsPowBoostInDB;
  
  cmat uplinkAvgIntrCov;
  
  
  #ifdef USING_SRS
  bool isSRSEnabled;
  ivec srsRBs;
  bool isOddSCs;
  #endif
  
  CQIEstimator();
  int computeRIForDownlinkPeriodicRIReport(CQIMeasurementConfig_S& cqiMeasurementConfig, FrameStructure& frameStructure, DemodulationScheme_E& demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, Array< cmat >& rxFrame, Array< Array< Array< cmat > > >& allChannels, double myTxSCPowerInDB, ivec txNodeIDSForChannel, double noiseVariance, double restOfInterferenceInDB, int ServiceNodeID, int subframeNumber, string WideBandCQIlogsFileName, string RIlogsFileName, bool isAperiodicComputation);
  static void getPhysicalChannel(Array< Array<cmat> >& outputChannel ,ResourceMapper *resourceMapper,Array< Array<cmat> > &channel, int vrbNumber,bool isDownlink);
  void getPrecodedPhysicalChannelAcrossRE(Array< cmat >& signalChannel, ResourceMapper* resourceMapper, PortToAntennaMapper_S* portToAntennaMapper, Array< Array< cmat > >& channel, int vrbNumber, TransmissionScheme_E txScheme, int nLayers, int nPorts, bool isCloseLoop, int pmi_i1, bool isDownlink, int reCountPerRb, int numberOfRbs, ivec positionsToConsider = " ", cmat precoder = " ");
  void getRawPhysicalChannelAcrossRE(Array< cmat >& rawChannel, ResourceMapper* resourceMapper, Array< Array< cmat > >& channel, int vrbNumber, bool isDownlink, int reCountPerRb, int numberOfRbs, ivec positionsToConsider);
  // 	static void getPrecodedIntrfrChannelAcrossRE(Array< Array<cmat> >& intrfrChannel,ResourceMapper* resourceMapper, Array< Framehandler* > interferingTxFrameHandlers, ivec interfererChannelIndices, PortToAntennaMapper_S* portToAntennaMapper, Array< Array< Array< cmat > > >& allChannels, int vrbNumber, bool isDownlink, int reCountPerRb, int numOfRbs, int nRx, int nLayers, ivec positionsToConsider = "");
  static Array< cmat > getIntrPrecodedPhysicalChannelOverRS(ResourceMapper* resourceMapper, PortToAntennaMapper_S* portToAntennaMapper, Array< Array< cmat > >& channel, int vrbNumber, DataBlkInfo_S& dataBlkInfo, bool isDownlink, ivec positionsToConsider = "");
  Array< cmat > getPrecodedPhysicalChannel(ResourceMapper* resourceMapper, PortToAntennaMapper_S* portToAntennaMapper, Array< Array< cmat > >& channel, int vrbNumber, TransmissionScheme_E txScheme, int nLayers, int nPorts, bool isCloseLoop,int pmi_i1, bool isDownlink, ivec positionsToConsider = "", cmat precoder = "");
  Array< cmat > getRawPhysicalChannel(ResourceMapper *resourceMapper ,Array< Array<cmat> > &channel, int vrbNumber,bool isDownlink,ivec positionsToConsider);
  Array<cmat > getPerToneInterferenceCovariance(Array<cmat> &rxFrame, FrameStructure& frameStructure, ResourceMapper *resourceMapper, bool isDownlink,Array< Array<cmat > > &myChannel,PortToAntennaMapper_S *portToAntennaMapper,ReferenceSignal_S *refSignal, imat rbRSPattern,ivec portReferenceNumbers,TransmissionScheme_E myTransmissionScheme,int reCountPerRb,int rbNumber,int NumOfRbs,ivec cqiPorts,double sigpowInWatts, double& rsrp);
  ivec getCQIPerCodewords(double& exp_spec_eff, Array<vec>& sinrPerCodeword, vec& MMIBperModSchemes,ivec& modOrdersPerCW, vec& effSINRsPerCW,int rbCount,int nREsPerRB,TransmissionScheme_E txScheme,int nLayers, bool enableCQI0=false);
  Array<vec> computeSinrPerCW(FrameStructure& frameStructure, double myTxSCPowerInDB, Array< Array< cmat > > myChannel, Array< cmat >& rxFrame, Array< Array< Array< cmat > > >& allChannels, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, double noiseVariance, double restOfInterferenceInDB, int rbNumber, TransmissionScheme_E txScheme, int nLayers, ivec cqiPorts, int nCodewords, int NumOfRbs, bool isCloseLoop, int pmi_i1, bool isDownlink, cmat precoder = "");
  Array < Array<vec > > computeSinrPerCWforI1indices(FrameStructure& frameStructure,double myTxSCPowerInDB, Array< Array<cmat> > myChannel,Array<cmat> &rxFrame, Array< Array< Array< cmat > > >& allChannels, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper,ReferenceSignal_S *refSignal, ResourceMapper* resourceMapper, double noiseVariance, double restOfInterferenceInDB,int rbNumber,TransmissionScheme_E txScheme, int nLayers, ivec cqiPorts, int nCodewords,int NumOfRbs,bool isCloseLoop,bool isDownlink, ivec wb_I1indices);
  Array < Array<vec > > computeSinrPerCWForCodeBook (FrameStructure& frameStructure, double myTxSCPowerInDB, Array< Array< cmat > > myChannel, Array< cmat >& rxFrame, Array< Array< Array< cmat > > >& allChannels, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, double noiseVariance, double restOfInterferenceInDB, int rbNumber, TransmissionScheme_E txScheme, int nLayers, ivec cqiPorts, int nCodewords, int NumOfRbs, bool isCloseLoop, bool isDownlink, Array< cmat >& tCodeBook);
  Array < Array<vec > > computeSinrPerPrecodersPerCW(FrameStructure& frameStructure,double myTxSCPowerInDB, Array< Array<cmat> > myChannel,Array<cmat> &rxFrame, Array< Array< Array< cmat > > >& allChannels, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, double noiseVariance, double restOfInterferenceInDB, int rbNumber, TransmissionScheme_E txScheme, int nLayers, ivec cqiPorts, int nCodewords, int NumOfRbs, bool isCloseLoop, bool isDownlink);
  void computeCQIFromIdealChannelForDownlinkAperiodicCQIReport(DLCQIMeasurement_S& perUserFeedback, FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, Array< cmat >& rxFrame, Array< Array< Array< cmat > > >& allChannels, double myTxSCPowerInDB, int desiredTxNode, ivec txNodeIDSForChannel, CQIMeasurementConfig_S& cqiMeasurementConfig, double noiseVariance, double restOfInterferenceInDB, int nodeID, int subframeNumber, string AperiodicCQILogsFileName);
  void computeCQIFromIdealChannelForDownlinkPeriodicCQIReport( DLCQIMeasurement_S& perUserFeedback, FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, Array< cmat >& rxFrame, Array< Array< Array< cmat > > >& allChannels, double myTxSCPowerInDB, int desiredTxNode, ivec txNodeIDSForChannel, CQIMeasurementConfig_S& cqiMeasurementConfig, double noiseVariance, double restOfInterferenceInDB, int ServiceNodeID, int subframeNumber, string WideBandCQIlogsFileName, string SubbandCQIlogsFileName);
  SubbandULCQIInfo_S computeRankAndCQIFromIdealChannelForUplink(FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ResourceMapper* resourceMapper, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, TransmissionScheme_E txScheme, Waveform_E waveform, int maxLayers, ivec cqiPorts, int maxCodewords, double noiseVariance, double NIValueForTheServerNode, bool enableCQI0 = 0);
  SubbandULCQIInfo_S computeRankAndCQIFromIdealChannelForUplinkForDLReci(FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ResourceMapper* resourceMapper, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, TransmissionScheme_E txScheme, Waveform_E waveform, int maxLayers, ivec cqiPorts, int maxCodewords, double noiseVariance, double NIValueForTheServerNode,  ReciprocityPrecodersInfo_S &reciprocityPrecoders, double weakIntr, Array<cmat> intrCov, double noiseVar, bool enableCQI0 = 0);//reciprocity
  SubbandULCQIInfo_S computeCQIFromIdealChannelForUplink(FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ResourceMapper* resourceMapper, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, TransmissionScheme_E txScheme, Waveform_E waveform, int nLayers, ivec cqiPorts, int nCodewords, double noiseVariance, double NIValueForTheServerNode, bool enableCQI0 = 0); // ask later
  
  // NR Functions
  ReciprocityPrecodersInfo_S computeReciprocitybasedPrecoderFromIdealChannelForUplink(FrameStructure& frameStructure,DemodulationScheme_E demodulationScheme,PortToAntennaMapper_S *portToAntennaMapper,ResourceMapper *resourceMapper, Array< Array<cmat> > &myChannel, double myTxSCPowerInDB,TransmissionScheme_E txScheme, int nLayers,ivec cqiPorts , int nCodewords,double noiseVariance,double NIValueForTheServerNode,bool isDownlink,bool enableCQI0);
  ReciprocityPrecodersInfo_S computeReciprocitybasedPrecoderFromIdealChannelForUplinkPerRBG(Framehandler* myTxFrameHandler,DemodulationScheme_E demodulationScheme,PortToAntennaMapper_S *portToAntennaMapper,ResourceMapper *resourceMapper, Array< Array<cmat> > &myChannel, double myTxSCPowerInDB,TransmissionScheme_E txScheme, int nLayers,ivec cqiPorts , int nCodewords,double noiseVariance,double NIValueForTheServerNode,bool isDownlink,bool enableCQI0);
  ReciprocityPrecodersInfo_S computeReciprocitybasedPrecoderFromIdealChannelForDownlink(DLCQIMeasurement_S& perUserFeedback, FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, Array< cmat >& rxFrame, Array< Array< Array< cmat > > >& allChannels, int desiredTxNode, ivec txNodeIDSForChannel, CQIMeasurementConfig_S& cqiMeasurementConfig, double noiseVariance, double restOfInterferenceInDB, int nodeID, int subframeNumber, string AperiodicCQILogsFileName);
  
  void ManageDLCQIReportTrigger(int DLRBs,int subframeNumber,bool isAperiodicCQICompEnabled=false);
  void FormPeriodicCQIPDU();
  void FormPeriodicCQIPDU(int requestID);
  void FormAPeriodicCQIPDU();// CQI Aperiodic CSI (CQI and PMI) represented in bits and PDU formed
  void FormAPeriodicCQIPDU(int requestID);
  void ManageDLCQIReportTriggerForABS(int DLRBs,int subframeNumber,bool isAperiodicCQICompEnabled, int offset);
  //Array< cmat > getRawPhysicalChannel(Array< Array<cmat> > &channel,Size_S rbSize,Array<ivec> prbSCStartLocInEachSlot,bool isDownlink,ivec positionsToConsider);
  
};


int PMIselect(ivec possiblePMIs, Array<vec>& MMIBvaluesperModScheme);
int getBestPMIindex(vec SpectralEfficiencyOverPMIs,Array<vec> MMIBvaluesPermodscheme);
 
Array<vec> calculateSinrWithIdealChannelperSubband(TransmissionScheme_E transmissionScheme, int nCodeWords, int nLayers, DemodulationScheme_E demodulationScheme,int nTones, Array<cmat> myPrecodedChannel, double sigpowInWatts, Array<cmat> &estimatedIntrCovariance);
// Array<cmat> calculateIdealInterferenceCoVariance(ResourceMapper *resourceMapper,Array<Framehandler*> interferingTxFrameHandlers,ivec interfererChannelIndices,PortToAntennaMapper_S *portToAntennaMapper ,Array< Array< Array<cmat> > > &allChannels, int vrbNumber,bool isDownlink,int reCountPerRb,int numOfRbs,int nRxAntenna,int nLayers,ivec positionsToConsider,TransmissionScheme_E transmissionScheme, double noiseVarWithROI, vec interfererTxScPowInWatts);
Array<vec> calculateSinrWithIdealChannelForAbstraction(TransmissionScheme_E transmissionScheme, int nCodeWords, int nLayers, DemodulationScheme_E demodulationScheme,ivec intrSFBCLayers, Array<cmat> myPrecodedChannel, Array< Array<cmat> > IntfPrecodedChannel, double noiseVarWithROI, double sigpowInWatts, vec interfererTxScPowInWatts,bool isEstimatedCovariance, Array<cmat> &estimatedIntrCovariance);
Array<vec> calculateSinrWithIdealChannelperSubbandForSCFDMA(TransmissionScheme_E transmissionScheme, int nCodeWords, int nLayers, DemodulationScheme_E demodulationScheme, int nTones, Array< cmat > myPrecodedChannel, Array< Array< cmat > > IntfPrecodedChannel, cmat ROICov, double sigpowInWatts, vec interfererTxScPowInWatts);
cmat computePostProcessingChannelCoeffForUplink(int nLayers, DemodulationScheme_E demodulationScheme,int nTones, Array<cmat> myPrecodedChannel, Array< Array<cmat> > IntfPrecodedChannel, cmat ROICov, double sigpowInWatts, vec interfererTxScPowInWatts,cmat& ppIntrCovChannel);
ivec getPortLocationsInRB(FrameStructure& frameStructure, ivec requiredPorts, int RBNumber = 0, bool excludeCCHCRS = 0);

int getMode2_1EncodedPrecoderInd(int RI,int codeBookIndexi2);
void getEncodedPrecoderIndForSubmode2(int &IPMI1,int &IPMI2,int RI,int codeBookIndexi1,int CodeBookIndexi2);
int getDifferentialCQIvalue(ivec CQIindexPerCW);
int getCombinatorialindex(ivec subbandIndex,int noOfSubbands);
int getModulationOrderFromCQIIndex(int CQI, int& linearIndex);
void getMaxNumberOfBitsFori1Andi2(int nLayers, int &noOfbitsForPrecoderi1,int &noOfbitsForPrecoderi2);

void generateReciprocityBasedPrecoderUsingSVD(ReciprocityPrecodersInfo_S & reciprocityPrecodersInfo,Array< Array <cmat > > &channel,ivec vrbs,Size_S rbSize,Array<Array<ivec> > & prbSCStartLocInEachSlot,ivec portLocations,bool isDownlink);
Array< cmat > getRawPhysicalChannel(Array< Array<cmat> > &channel,Size_S rbSize,Array<ivec> prbSCStartLocInEachSlot,bool isDownlink,ivec positionsToConsider);
void getPhysicalChannel(Array< Array<cmat> >& outputChannel,Array< Array<cmat> > &channel, Size_S rbSize,Array<ivec> prbSCStartLocInEachSlot,bool isDownlink);

  
#endif

