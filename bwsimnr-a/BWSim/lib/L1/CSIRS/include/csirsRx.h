
#ifndef _CSIRSRX_H_
#define _CSIRSRX_H_

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

#include "../../PHY/include/Codebook.h"
#include "../../PHY/include/ReferenceSignalGenerator.h"
#include "../../PHY/include/CQIEstimatorNR.h"

struct CSI_Feedback_Info_S
{
    Array<ivec> cqiPerSubbandPerCW;
    vec specEffPerSubband;
    double specEffWideband;
    int nLayers;
    Array<cmat> precoderPerSubband;
};

Array<cmat> retrieveCSIRSFromResourceGrid(int nRBsAllocated,Array<cmat>& rxGrid, Array<ivec> allocatedRBs, imat rbRSPattern);
cmat computeRxIntrCov(int nRBs,Array<cmat>& rxGrid, ivec allocatedRBs, imat rbRSPattern, double boostForCDM=1.0);
cmat computeRxIntrCov(Array<cmat>& rxGrid);
Array<cmat> estimateChannelFromCSIRS(Array<cmat>& rxRSPerRB, Array<cvec> csirsPerPort,cmat intCov);
double computeCQI(CSI_Info_S& cqiInfo, Array<vec>& sinrPerCodeword,int nLayers,cTBTables* tbTables,int nREsPerRB,int rbCount);
Array<vec> calculateSinrPerCodewordNR(int nLayers, Array<cmat>& rawChannel,cmat precoder, double sigpowInWatts, cmat estimatedIntrCovariance,DemodulationScheme_E desc);
CSI_Info_S computeBestPMIAndCQI(Array<cmat>& currentRawChannel,Array<cmat>& codeBook,cTBTables* tbTables,cmat intrCov,int subbandSize,int nSubbands,int nREsPerRB,DemodulationScheme_E desc);

double doLinkAdaptationforTypeI(Array<cmat>& estimatedChannelPerRB, int dlNRBs, int prgSize, int maxRank, int nCodeWords, cmat intCov,cTBTables tbTables,Codebook codebook,int& nLayers,type1PMI_S& pmiIndices,ivec &cqi);
vec doLinkAdaptationforTypeI(Array<cmat>& estimatedChannelPerRB, int dlNRBs, int prgSize, int maxRank, int nCodeWords, cmat intCov,cTBTables tbTables,Codebook codebook,int& nLayers,Array<cmat> &precoderPerSubband,Array<ivec> &cqi);

CSI_Feedback_Info_S doLinkAdaptationforTypeII(Array<cmat>& estimatedChannelPerRB,Type2coefficients_N_OMP_S& finalCoefficients, int dlNRBs, int prgSize, int maxRank, int nCodeWords, cmat intCov,cTBTables tbTables,Codebook codebook,int& nLayers,ivec &cqi);
vec doLinkAdaptationforTypeII(Array<cmat>& estimatedChannelPerRB,Array<cmat> &precoderPerSubband, int dlNRBs, int prgSize, int maxRank, int nCodeWords, cmat intCov,cTBTables tbTables,Codebook codebook,int& nLayers,Array<ivec> &cqi);

double doLinkAdaptationforTypeIMultiTRPNCJT(Array<Array<cmat>>& estimatedChannelPerTRP, int dlNRBs, int prgSize, int maxRank, int nCodeWords, cmat intCov,cTBTables tbTables,Codebook codebook,Array<type1PMI_S>& finalPMIIndices,ivec& nLayersPerTRP,ivec &cqi);
double doLinkAdaptationforTypeIIMultiTRPNCJT(Array<Array<cmat>>& estimatedChannelPerTRP,Array<Type2coefficients_N_OMP_S>& finalCoefficients, int dlNRBs, int prgSize, int maxRank, int nCodeWords, cmat intCov,cTBTables tbTables,Codebook codebook,ivec& nLayersPerTRP,ivec &cqi);
double doLinkAdaptationforMultiTRPCJT(Array<Array<cmat>>& estimatedChannelPerTRP,Array<Type2coefficients_N_OMP_S>& finalCoefficients, int dlNRBs, int prgSize, int maxRank, int nCodeWords, cmat intCov,cTBTables tbTables,Codebook codebook,int& nLayers,ivec& coPhasingFactors,ivec &cqi,double& BLER);


double computeSEforRank1Type1BruteForceWideband(Array<cmat> idealChannel, Array<cmat> orthBeamsTotalSets, int bestBeamSet, ivec bestBeams, cmat coPhase, cmat IntrCov, cTBTables tbTables, int dlRBs);
double computeSEforRank2Type1BruteForceWideband(Array<cmat> idealChannel, Array<cmat> orthBeamsTotalSets, int bestBeamSet, ivec bestBeams, cmat coPhase, cmat IntrCov, cTBTables tbTables, int dlRBs, int N1, int N2);

double computeSEforRank1Type1R15(Array<cmat> idealChannel, Array<cmat> orthBeamsTotalSets, int bestBeamSet, ivec bestBeams, cmat coPhase, cmat IntrCov, cTBTables tbTables, int dlRBs, int subbandSize);
double computeSEforRank2to8Type1R15(Array<cmat> idealChannel, Array<cmat> orthBeamsTotalSets, int bestBeamSet, ivec bestBeams, cmat coPhase, cmat IntrCov, cTBTables tbTables, int dlRBs, int subbandSize, int N1, int N2, int rank);
double computeSEforRank2to8Type1R19Scheme2(Array<cmat> idealChannel, Array<cmat> orthBeamsTotalSets, int bestBeamSet, ivec bestBeams, cmat coPhase, cmat IntrCov, cTBTables tbTables, int dlRBs, int subbandSize, int N1, int N2, int rank);
cmat getR15Type1SPPrecoder(cmat beams, cmat coPhase, int rank);
cmat getBeamsForR15Type1SP(CodeBookConfig_S codebookConfig, int rank, cmat avgChannelCovar);
CSI_Feedback_Info_S computeSEforType1Simplifed(CodeBookConfig_S codebookConfig, Array<cmat> idealChannel, cmat IntrCov, cTBTables tbTables, int dlRBs, int subbandSize, bool isR15, int maxRank);
CSI_Feedback_Info_S computeSEforType1Rel19(CodeBookConfig_S codebookConfig, Array<cmat> idealChannel, cmat IntrCov, cTBTables tbTables, int dlRBs, int subbandSize, bool isR15, int maxRank);
CSI_Feedback_Info_S computeSEforType1Rel19_5_8(CodeBookConfig_S codebookConfig, Array<cmat> idealChannel, cmat IntrCov, cTBTables tbTables, int dlRBs, int subbandSize, bool isR15, int maxRank);
CSI_Feedback_Info_S computeSEforSinglePort(Array<cmat> idealChannel, cmat IntrCov, cTBTables tbTables, int dlRBs, int subbandSize);


CSI_Feedback_Info_S doLinkAdaptationforUL(Array<cmat>& estimatedChannelPerRB, int ulNRBs, int prgSize, int nPorts, int maxRank, int nCodeWords, cmat intCov,cTBTables tbTables,Codebook codebook,int& nLayers);

#endif
