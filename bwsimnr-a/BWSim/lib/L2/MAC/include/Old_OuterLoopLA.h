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


#ifndef _OLD_OUTER_LOOP_LA_H
#define _OLD_OUTER_LOOP_LA_H

#include "Others/L2SimConfig.h"

struct OuterLoopLinkAdaptation_S{
  
  bool isEnabled;
  int WindowSize;
  ivec ServiceNodes;
  Array<ivec> WindowCount;
  Array<ivec> SuccessWindowCount;
  Array<vec> sinrOffset;
  Array< std::queue <vec> > sinrOffsetWithDelay;
  Array<std::queue <unsigned int> > subframeNumberOffset;
  Array<ivec> txNewPackets;
  Array<ivec> txNewCorrectPackets;
  Array<ivec> LastUpdatedsubframe;
  
  Array< std::queue <ivec> > CQIHistory; 
  
  vec RateForCQI;					
  vec SinrForCQI;					
  vec sinrPointsForBLER_10_Percent;			
  vec ratesPersinrPointsForBLER_10_Percent;		
  vec TargetBLER;
  vec rateThresholds;
  
  OuterLoopLinkAdaptation_S();
  void clear(int serviceNodeID);
  void concatTo(OuterLoopLinkAdaptation_S& outerLoopLinkAdaptationInfo);
  void AddTo(ivec ServiceNodeIDs, vec tTargetBLER, vec rateThresholds, int tWindowSize = 2);
  void AddTo(ivec ServiceNodeIDs);
  void RemoveTo(int removeIndx);
  void clear();
  void update(int nodeID, bvec isSuccess, bvec isNewTransmission, int subframeNumber, double mcsRate);
  double getTargetBLER(double mcsRate);
  vec getRateOffset(ivec CQIperCW, int nodeID);
  vec getSINROffsetPerCWIndB(int nodeID);
  
};

#endif
