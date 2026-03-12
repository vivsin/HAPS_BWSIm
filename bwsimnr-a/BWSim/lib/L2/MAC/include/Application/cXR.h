/*************************************************************************
 *
 * CEWiT CONFIDENTIAL
 * __________________
 *
 * All Rights Reserved © 2012 CEWiT, India
 *
 * NOTICE: All information contained herein is, and remains the property of
 * Centre of Excellence in Wireless Technology (CEWiT) and its suppliers, if
 * any. The intellectual and technical concepts contained herein may be
 * proprietary to CEWiT.  Unauthorized use, duplication, reverse engineering,
 * any form of redistribution, or use in part or in whole other than by prior,
 * express, printed and signed license for use is strictly forbidden.
 */
#ifndef CXR_H
#define CXR_H

#include "../Others/L2SimConfig.h"
#include "cTrafficClass.h"

class cXR : public cTrafficClass {
private:
  int mFPS;          /* Frames per Second value, unit: per sec, type: int */
  int mAvgDataRate;  /* Average Date rate, unit: Mbits per sec, type: int */

  double mMeanPktSize; /* Mean Packet Size,   unit: Bytes, type: double */
  double mStdPktSize;  /* STD of Packet Size, unit: Bytes, type: double */
  double mMinPktSize;  /* Min of Packet Size, unit: Bytes, type: double */
  double mMaxPktSize;  /* Max of Packet Size, unit: Bytes, type: double */

  double mMeanJitter;  /* Mean Jitter,   unit: ms, type: double */
  double mStdJitter;   /* STD of Jitter, unit: ms, type: double */
  double mMinJitter;   /* Min of Jitter, unit: ms, type: double */
  double mMaxJitter;   /* Max of Jitter, unit: ms, type: double */

public:
  cXR();
  ~cXR() {};

  cXR(eTrafficType trafficType, int tSrc, int tDest, int tAppId,
      sL2SimConfig *sSimConfig);
  cXR(eTrafficType trafficType, string l2SupportFileName, int numSlots);
  cXR(cTrafficClass &tTrafficClass);
  cXR&
  operator=(cXR const &tXR);

  void
  genXRTraffic(int tStartSubFrameNo, int tEndSubFrameNo);
};
#endif
