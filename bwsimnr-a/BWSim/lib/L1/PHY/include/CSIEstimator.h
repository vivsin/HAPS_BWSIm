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


#ifndef _CSI_EST_H_
#define _CSI_EST_H_
#include "StructsAndEnums.h"
#include "SupportingFunctions.h"
#include "cNRTables.h"
struct CSI_Info_S{
  
  int nLayers;
  int nCodeWords;
  
  int PMI;
  
  ivec CQIindexPerCW;
  vec effSINRsPerCW;
  Array<vec> sinrPerCodeword;
  
  ivec nLayersPerCW;
  vec blerPerCW;
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
    blerPerCW.set_size(nCodeWords);
    specEff = 0.0;
  }
  
  ivec modOrderPerCW(){
    
    ivec modOrder(nCodeWords);
    for(int icw = 0; icw < nCodeWords; icw++)
      modOrder(icw) = (int)modSchemePerCW(icw);
    
    return modOrder;
  }
  
};

Array<vec> performLayerDeMapping(Array<vec> &ppSINRPerLayer, TransmissionScheme_E txScheme, int nCodewords, int nLayers);
vec getSINRwithMMSEperLayer(cmat precodedChannel, cmat Intrcov, int nLayers, double sigpowInWatts);
double computeSpecEff(CSI_Info_S& cqiInfo,Array<vec>& sinrPerCodeword,int Q,int nLayers,AWGNTablesConvCoder* awgnTables);
Array<vec> calculateSinrPerCodewordNR(int nLayers, Array<cmat>& rawChannel,cmat precoder, double sigpowInWatts, cmat estimatedIntrCovariance,DemodulationScheme_E desc);
vec getSINRwithMRCperLayer(cmat precodedChannel,cmat Intrcov, int nLayers, double sigpowInWatts);
vec getSINRperLayer(DemodulationScheme_E desc, cmat precodedChannel,cmat Intrcov, int nLayers, double sigpowInWatts);
double computeCQI(CSI_Info_S& cqiInfo, Array<vec>& sinrPerCodeword,int nLayers,cTBTables* tbTables,int nREsPerRB,int rbCount);
int getBestPMI(Array<cmat>& currentRawChannel,Array<cmat>& codeBook,AWGNTablesConvCoder *awgnTables,int Q,cmat intrCov,Array<vec>& sinrPerCW,double& specEff,DemodulationScheme_E desc);
CSI_Info_S computeBestPMIAndCQI(Array<cmat>& currentRawChannel,Array<cmat>& codeBook,cTBTables* tbTables,cmat intrCov,int subbandSize,int nSubbands,int nREsPerRB,DemodulationScheme_E desc);
double computeCQIAndBLER(CSI_Info_S& cqiInfo, Array<vec>& sinrPerCodeword,int nLayers,cTBTables* tbTables,int tbSize,double rate);
#endif

