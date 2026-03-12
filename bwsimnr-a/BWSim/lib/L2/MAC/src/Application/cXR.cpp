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
#include "../../include/Application/cXR.h"


cXR::cXR() {
  fSetSubFrameIndx(0);
  fSetSourceID(0);
  fSetDestinationId(0);
}

cXR::cXR(eTrafficType trafficType, string l2SupportFileName, int numSlots) {
  fSetTrafficType(trafficType);
  fSetSubFrameIndx(0);
  parse(l2SupportFileName, "mAvgDataRate", mAvgDataRate);
  parse(l2SupportFileName, "mFPS", mFPS);
  parse(l2SupportFileName, "mMeanPktSize", mMeanPktSize);
  parse(l2SupportFileName, "mStdPktSize", mStdPktSize);
  parse(l2SupportFileName, "mMinPktSize", mMinPktSize);
  parse(l2SupportFileName, "mMaxPktSize", mMaxPktSize);
  parse(l2SupportFileName, "mMeanJitter", mMeanJitter);
  parse(l2SupportFileName, "mStdJitter", mStdJitter);
  parse(l2SupportFileName, "mMinJitter", mMinJitter);
  parse(l2SupportFileName, "mMaxJitter", mMaxJitter);

  genXRTraffic(startTime, numSlots);

}

cXR::cXR(eTrafficType trafficType, int tSrc, int tDest, int AppId,
         sL2SimConfig *sSimConfig)
    : cTrafficClass(trafficType, tSrc, tDest, AppId,
                    sSimConfig->mSimulationTime) {
  mFrameDuration = sSimConfig->frameDuration;

  mFPS           = sSimConfig->xrConfig.mFPS;
  mAvgDataRate   = sSimConfig->xrConfig.mAvgDataRate;

  mMeanPktSize = sSimConfig->xrConfig.mMeanPktSize;
  mStdPktSize  = sSimConfig->xrConfig.mStdPktSize;
  mMinPktSize  = sSimConfig->xrConfig.mMinPktSize;
  mMaxPktSize  = sSimConfig->xrConfig.mMaxPktSize;
  mMeanJitter  = sSimConfig->xrConfig.mMeanJitter;
  mStdJitter   = sSimConfig->xrConfig.mStdJitter;
  mMinJitter   = sSimConfig->xrConfig.mMinJitter;
  mMaxJitter   = sSimConfig->xrConfig.mMaxJitter;

  startTime = 0;

  genXRTraffic(startTime, sSimConfig->mSimulationTime);
}

cXR::cXR(cTrafficClass &tTrafficClass)
    : cTrafficClass(tTrafficClass.fGetSourceId(),
                    tTrafficClass.fGetDestinationId(),
                    tTrafficClass.fGetAppId(), tTrafficClass.fGetSimTime()) {
  fSetSubFrameIndx(tTrafficClass.fGetSubFrameIndx());

  fSetTrafficType(tTrafficClass.fGetTrafficType());
}

cXR&
cXR::operator=(const cXR &tXR) {
  this->mFPS         = tXR.mFPS;
  this->mAvgDataRate = tXR.mAvgDataRate;
  this->mMeanPktSize = tXR.mMeanPktSize;
  this->mStdPktSize  = tXR.mStdPktSize;
  this->mMinPktSize  = tXR.mMinPktSize;
  this->mMaxPktSize  = tXR.mMaxPktSize;
  this->mMeanJitter  = tXR.mMeanJitter;
  this->mStdJitter   = tXR.mStdJitter;
  this->mMinJitter   = tXR.mMinJitter;
  this->mMaxJitter   = tXR.mMaxJitter;
  this->fSetSubFrameIndx(tXR.fGetSubFrameIndx());
  this->fSetTrafficType(tXR.fGetTrafficType());
  this->fSetSourceID(tXR.fGetSourceId());
  this->fSetDestinationId(tXR.fGetDestinationId());
  this->fSetAppId(tXR.fGetAppId());
  return *this;
}

void
cXR::genXRTraffic(int tStartSubFrameNo, int tEndSubFrameNo) {
  // TODO: Add code to genenrate code for UL
  // TODO: Add code to generate code for multiple stream

  // Create a dir to store the XR traffic details
  string dir = "./Results/xrTrafficDetails/";
  string cmd = "mkdir -p " + dir;
  system_exec(cmd.c_str());
  string filename = dir + "ue_" + std::to_string(mDestinationId) + ".txt";
  std::ofstream fout(filename, std::ios::out);
  fout
      << R"(txNodeID,	rxNodeID,	startSFNo,	endSFNo,	curSFNo,	curTime, packetSize,	interPacketDelay
)";

  // Stagger the starting time. Otherwise all UE will have packet to transmit
  // at 0. TODO: Check if this is OK
  tStartSubFrameNo = 10;
  double tCurTime = tStartSubFrameNo + randi(0, 9);
  int tCurSubFrameNum = floor_i(tCurTime);

  // By taking the floor of the curTime, the curSubFrameNum reflects the
  // subframe in which the packet was generated. This is done so that even if
  // the packet arrives at the middle of the subframe, it can still be
  // scheduled within that subframe using mini-slot scheduling.
  // If we are doing subframe based scheduling, the existing code assumes that
  // the all the data to be transmitted in a given subframe has already arrived
  // before the starting time of current subframe. In this case, if the
  // curSubFrameNum has the subframe number in which the packet was generated
  // instead of the subframe number in which the packet is to be transmitted,
  // this will mean that we are scheduling the packet before it arrives.
  //
  // TODO: Adjust the curSubFrame number according to the scheduler. Or add a
  // new field to the packet to store both the subframe number in which packet
  // has arrived and the subframe number in which the packet is to be
  // transmitted.
  while (tCurSubFrameNum < tEndSubFrameNo) {
    // The packet size is truncated Gaussian with give parameters
    int tPktSize = ceil_i((mStdPktSize * randn()) + mMeanPktSize);
    while (tPktSize <= mMinPktSize || tPktSize >= mMaxPktSize) {
      tPktSize = ceil_i((mStdPktSize * randn()) + mMeanPktSize);
    }

    // Generate packet. XR packet have an additional field curTime.
    // This can be used to get the symbol number within the subframe for
    // mini-slot scheduling.
    generatePacket(tCurSubFrameNum, tPktSize, tCurTime);

    // Jitter is truncated Gaussian with given parameters
    double tJitter = ((mStdJitter * randn()) + mMeanJitter);
    while (tJitter <= mMinJitter || tJitter >= mMaxJitter) {
      tJitter = ((mStdJitter * randn()) + mMeanJitter);
    }

    // Increment the time
    // The packet inter-arrival time = (1 / fps) + jitter [ms]
    double tInterPktTime = ((1.0 / (double)mFPS) * 1000) + tJitter;

    // Print Details
    fout << mSourceId << ",\t" << mDestinationId << ",\t"
         << tStartSubFrameNo << ",\t" 
         << tEndSubFrameNo << ",\t"
         << tCurSubFrameNum << ",\t"
         << tCurTime << ",\t"
         << tPktSize << ",\t" << tInterPktTime << endl;

    tCurTime += tInterPktTime;
    tCurSubFrameNum = floor_i(tCurTime);
  }
  fout.close();
}
