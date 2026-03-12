/*************************************************************************
 * 
 * CEWiT CONFIDENTIAL
 * __________________
 *
 * All Rights Reserved © 2014 CEWiT, India
 *
 \ NOTICE: All information* contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
 * and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
 * Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
 * express, printed and signed license for use is strictly forbidden.
 */


#ifndef BEAM_STRUCT_AND_ENUMS_H
#define BEAM_STRUCT_AND_ENUMS_H

#include "../../simSupport/include/CommonStructsAndEnums.h"

#ifdef ENABLE_COUT
#define BEAM_MGT_DEBUG
#endif

struct eTilt{
  
  double horizontalTilt;
  double verticalTilt;
  
//   double hTiltRad;
//   double vTiltRad;
  
  eTilt(){
    
    horizontalTilt = 0;
    verticalTilt = 90;
    
//     hTiltRad = 0;
//     vTiltRad = 0;
  }
};

struct BeamInfo_S{

  int Mq,Nq,P;
  imat TXRUsPerPanel;// panels x TXRUsPerPanel
  void set(int m,int n,int p)
  {
      Mq=m;Nq=n;P=p;
  }
};

struct sTXRUParameters{
  
  int numberOfTXRU;
  int numberOfRowsPerTXRU;
  int numberOfColsPerTXRU;
  int numberOfElementsPerTXRU;
  int Mg, Ng, M, N, P, Mp, Np, nTotalElements;
  int panelN1,panelN2;
  double dpv,dph,dv,dh;
  double lamda;
  int nPanels;
  Array <ivec> antennaElementsPerTXRU;
  imat TXRUsPerPanel;// panels x TXRUsPerPanel
  int nOrientations;
  ivec orientationIDPerPanel;
  ivec orientationIDperTXRU;
  
  sTXRUParameters();
  sTXRUParameters(double carrierFrequencyInHz, ivec antennaStructure, ivec txruMap, vec antennaSpacing,OrientationInfo_S orientationInfo);
  
  void initTXRUParameters(double carrierFrequencyInHz, ivec antennaStructure, ivec txruMap, vec antennaSpacing);
  void formTXRUmodel();
  void setTXRUParameters(int numOfTXRU, double tVerAntElementSpacing, double tHorzAntElementSpacing, int numRowsPerTXRU, int numElemPerTXRU);
  ivec getTXRUAntennaElements(int TXRUid);
  
  void printTXRUStructure(ostream& os);
  void setOrientations(OrientationInfo_S orientationInfo);
  void modifyTXRUs(imat TXRUsPerPanel);
};

struct sBeamParameters{

  bool isInitialized;
  sTXRUParameters TXRUparameters;
  int numberOfAzimuthAngles;
  int numberOfZenithAngles;
  int nTotalBeams;
  Array< eTilt > electricalTilts;
  Array< cvec > weightsPerTilts;

  // No txRU has a dedicated beam
  // Each beam has a defaultTxRU for initial beam sweeping - Dhiv
  
  ivec txRUPerBeamForAnalogBeamSweeping; // Used for analog beam sweeping. 
  Array<ivec> txRUsPerBeam;
  
  sBeamParameters();
  void configureBeamParameters(ivec& antStructure,ivec& txruMap, vec& antSpacing, Array<vec>& azimuthAngles, Array<vec>& zenithAngles, double carrierFrequencyInHz,OrientationInfo_S orientationInfo); 
  void setBeamParameters(Array<vec> azimuthAngles, Array<vec> zenithAngles);
  void setBeamTiltsAndWeights(vec azimuthAngles, vec zenithAngles);
  void setBeamTiltsAndWeights(Array<vec> &azimuthAngles, Array<vec> &zenithAngles);
  cvec getBeamformingVector(int beamID);
  void printOrientations(ostream& os);
    
};
vec getPhaseShiftPerElement(sTXRUParameters* txruParameters,double ZOD,double AOD,bool isInRadian=false);


#endif
