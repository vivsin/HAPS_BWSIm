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
#ifndef _SUPPORTING_FUNCTIONS_LTE_
#define _SUPPORTING_FUNCTIONS_LTE_

#include "StructsAndEnums.h"
#include "FrameHandler.h"
#include "ResourceMapper.h"

#include <stdint.h>



ivec getDMRSports(LayerInfo_S layerInfo,bool returnMyPorts);
ivec getAllPorts(int nFrontLoadedSymbols,int nCDMgroupsWithoutData,int dmrsType,Array<ivec>& layersPerCDM);

void getSubbandsAndBWParts(Array<ivec> &subbandSizes,int &numOfBWParts,int DLRBs);
void getSubbandsAndBWPartsUESelected(Array<ivec> &subbandSizes,int &numOfBWParts,int DLRBs);
ivec getSubbandSizeAperiodic(int DLRBs);
ivec getSubbandSizeAperiodicNR(int DLRBs);
ivec getSubbandSizeAperiodicUESelected(int DLRBs);
int getNumberOfUESelectedSubbands(int DLRBs);
TransmissionMode_E getTransmissionMode(string transmissionMode);
void setRsInfo(RSInfo_S& rsInfo, PortInfo_S& portInfo, FrameStructure& frameStructure, int cellID, TransmissionScheme_E txScheme, LayerInfo_S layerInfo, int nAntennaPorts, double rsBoostInDB, bool isDownlink, bool isToTransmitCQIPorts, int dataStartSymbol, bool isToTransmitTRSPorts = false, TRSConfig_S mTRSConf = TRSConfig_S(), Waveform_E waveform=_Waveform_OFDMA_);
void addptrs(RSInfo_S& rsInfo, PortInfo_S& portInfo, FrameStructure& frameStructure, int cellID, TransmissionScheme_E txScheme, int rnti, ivec Scheduledrbs, double rsBoostInDB, bool isDownlink);
void getNoOfSamplesAndPTRSGroupsTPenabled(int scheduledBW, int &nSamplesPerGroup, int &nPTRSGroups);
ivec getPTRSindexmTPenabled(int nSamplesPerGroup, int nPTRSGroups, int MpuschSC);
ivec getOrthoCoverCodeForPTRSTPenabled(int nRNTI, int nSamplesPerGroup);
// Array<imat> getMergedRBStructures(FrameStructure* frameStructure, ivec rsPorts, ivec vrbs, bool isDownlink, bool isLocalised = true, FrameReservationInfo_S* cchReservation = 0);
void formCSIRSPortStructures(RSInfo_S &rsInfo, FrameStructure &frameStructure);
void formSRSPortStructures(RSInfo_S &rsInfo, FrameStructure &frameStructure);
void ptrsfrequencyassignment(RSInfo_S& rsInfo, PortInfo_S& portInfo, FrameStructure& frameStructure, int rnti, int ptrsfrequencydensity, ivec Scheduledrbs, int ptrsreoffset);
void formTRSPortStructures(RSInfo_S &rsInfo, FrameStructure &frameStructure);
void ptrsfrequencyassignment(RSInfo_S& rsInfo, PortInfo_S& portInfo, FrameStructure& frameStructure, int rnti, int ptrsfrequencydensity, ivec Scheduledrbs, int ptrsreoffset);
Array<imat> getMergedRBStructures(FrameStructure *frameStructure, RSInfo_S& rsInfo, ivec vrbs, bool isDownlink, bool isLocalised = true, FrameReservationInfo_S* cchReservation = 0);
void getRStypes(ivec CQIports, ivec DeModPorts, RSType_E &CQIrsType, RSType_E &DeModrsType);
ivec getPortReferenceNumbers(FrameStructure &frameStructure, ivec rsPorts);
double getPowerHeadroomInDB(int nodeID, int& subframeNumber);
vec getPowerHeadroomInDB(ivec nodeIDs, ivec& subframeNumbers);
ivec getCodeBlockSizesForTB(int tbSize, ivec& interleaverParameters_K);
ivec getCodeBlockSizesForTBNR(int tbSize, double tCodeRate);
TransmissionMode_E parseTransmissionMode(string fileName, bool isDownlink);
int getSymbolRepetitionFactor(TransmissionScheme_E txnScheme, int nLayers);
vec getSINRwithMRCperLayer(cmat precodedChannel,cmat Intrcov, int nLayers, double sigpowInWatts);
vec getSINRwithMMSEperLayer(cmat precodedChannel, cmat Intrcov,  int nLayers, double sigpowInWatts);
vec getSINRwithWhiteMRCperLayer(cmat precodedChannel, cmat Intrcov,  int nLayers, double sigpowInWatts);
vec getSINROffsetInDBwithMMSEperCodeword(vec oldPPSinr, cmat precodedChannel, cmat additionalIntrcov, dComplex additionalScaling);
ivec getNumLayersPerCodeWord(int nCodeWords, int mimoRate);

// Implemented from IEEE paper titled "MU-MIMO and CSI Feedback Performance of NR/LTE"
Array<vec> getModifiedSINRInDBperCodeword_SMMSE(cmat reportedPrecoder, Array<cmat>& precodersOfComb, Array<vec> oldPPSinrOfComb, int UEIndxInComb, cmat &modifiedPrecoder);
cmat modifiedPrecoder(cmat reportedPrecoder, cmat matrixJkl);
cmat MatrixJkl(cmat reportedPrecoder, int layerNum, Array<cmat>& precodersOfComb, vec oldPPSinrOfComb, int UEIndxInComb);



ivec generateGoldSequence(int Cinit,int seqLength);
Array<cvec> performTransformPrecodingForUplink(const  Array<cvec> &dataCarriersPerLayer,bool isDFT, int transformSize); //
int getCPLength(int fftSize,int symb_cnt,int scsInkHz=15);
Array<cvec> getFreqDomainFrame(Array<cmat> &subframe);
Array<cmat> getFreqDomainFrame(Array<cvec> &timeDomainFrame, int fftSize, int startSym, int nSymbols);
Array<cmat> getFreqDomainFrame(Array<cmat> &timeDomainFrame, int fftSize);
Array<cvec> getTimeDomainFrame(Array<cmat> &frequencyDomainFrame, int fftSize, int startSym=0);
Array<cmat> getTimeDomainFrameMat(Array<cmat> &frequencyDomainFrame, int fftSize, int startSym=0);
FrameConfigInfo_S getFrameConfigurationInfo(UlDlMode_E uldlMode, TDDConfiguration_E tddConf, ivec CellConfig, int CellConfigPeriodicityInSlots);
void getSpecialSubFrameConfig(SpecialSubFrameConfig_S *SSFConfig,string simFileName);
ivec getSubbandSizeAperiodic(int DLRBs);

bool getLatestDLCQI(PerUserDLCQIFeedback_S perUserCQIReport,bool isAPeriodicCQIConfigured, AperiodicReportingMode_E aperiodicMode, PeriodicReportingMode_E periodicMode , ivec codewordIndicesToConsider,int DLRBs,ivec rbs,ivec &CQIPerCodeword,int &PMI, int requestID=-1);
int getNumOfBitsForPrecoder(int nPorts,int nCodewords);
int getHigherLayerSubbandDiffCQIvalue(int WidebandCQIIndex, int SubbandCQIIndex);
uint8_t getCQIIndexForHigherLayerSubband(unsigned int WidebandCQIIndex, unsigned int SubbandCQIIndexOffset);
int getUESelectedSubbandDiffCQIvalue(int widebandCQIIndex,int SelectedSubbandsCQiIndex);
int getUESelectedSubbandCQIvalue(int widebandCQIIndex,int SelectedSubbandsCQiIndexOffset);
Array<int> getBestMSubbands(int CombinatorialIndex,int nDLRBs);

Array<cmat> extractFromRBArea(Array<cmat> &physicalFrame, FrameStructure& frameStructure, ResourceMapper *resourceMapper, int rbNumber, bool isDownlink);
Array< Array<cmat> > extractFromRBArea(Array< Array<cmat> > &physicalFrame, FrameStructure& frameStructure, ResourceMapper *resourceMapper, int rbNumber, bool isDownlink);
Array< Array< Array<cmat> > > extractFromRBArea(Array< Array< Array<cmat> > > &physicalFrame, FrameStructure& frameStructure, ResourceMapper *resourceMapper, int rbNumber, bool isDownlink, ivec indices="");
void applyIDFT(EqualizerOutput_S &equalizerOutput, int dftSize);
CQIComputationInfo_S createBasicCQIComputationInfo(CQIComputationConfig_S &cqiComputationConfig);
RSInfo_S getRSInfoForUnScheduled(RSInfo_S defaultRSInfo,FrameStructure &frameStructure);
int getModulationOrderAsLinearIndex(int mcs);
std::ostream& operator<<(std::ostream &o,UlDlMode_E n);
void PrintRI(int nodeID, int RI,double avgPPSINRindB, vec SpecEffPerRI, int subframenumber,string fileName);
void printRI(int nodeID,int eNBID, int RI,double prepSINRindB,double avgPPSINRindB, vec specEffPerRI, int subframenumber,vec conditionNumber,string fileName,string parameters);
void PrintCQIindices(int nodeID, ivec cqiIndexPerCodeWord, ivec modOrders, vec effSINRs, int subframenumber, string WideBandCQIlogsFileName, int PMI = -1);
void PrintCQIindices(int nodeID, Array<ivec> cqiIndexPerCodeWord, int subframenumber,string SubbandCQIlogsFileName,int BWPartNumber=-1);
void printCQIindices(AperiodicReportingMode_E aperiodicMode,int nodeID,int eNBID, int nLayers,ivec cqiIndexPerCodeWord,ivec modOrders, vec effSINRs, int subframenumber,string aperiodicCQIlogsFileName,int WB_PMI,Array<ivec> SBcqiIndexPerCodeword, ivec SB_PMI,double computationTime,ivec pmiIndices);
void PrintCQIindices(AperiodicReportingMode_E AperiodicMode, int nodeID, ivec cqiIndexPerCodeWord, ivec modOrders, vec effSINRs, int subframenumber, string AperiodicCQIlogsFileName, int WB_PMI, Array< ivec > SBcqiIndexPerCodeword, ivec SB_PMI);
//void printReAssociation(AssociationInfo_S& associationInfo, string linkTableFileName, Array< Array< vec > >& RSRP,string rsrpDistributionFileName,string dlRSRPcouplingFileName);
void PrintPMILog(AperiodicReportingMode_E AperiodicMode,int nodeID, int subframenumber,string AperiodicPMIlogsFileName,NRCQIInfoAPeriodic_S nrCQIInfoAPeriodic);
void PrintPMILog(AperiodicReportingMode_E AperiodicMode,int nodeID, int subframenumber,string AperiodicPMIlogsFileName,NRMultipanelCQIInfoAPeriodic_S nrMPCQIInfoAPeriodic);

_UE_CATEGORY_ findUEcategory(int ueCategory);

int getNumOfREsPerRB(ivec CSIRSPorts, FrameStructure& frameStructure, Array< ivec >& rsSymPositions, ivec& rsScPositions);
void checkNRTransmissionScheme(TransmissionScheme_E txScheme);

cmat avgCovariance(Array<cmat>& Channel);
ivec getNearestIndices(vec Phase,ivec N);
#endif
