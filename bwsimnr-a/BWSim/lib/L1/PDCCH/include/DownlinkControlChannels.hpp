#pragma once
#ifndef _DOWNLINKCONTROLCHANNELS_HPP_
#define _DOWNLINKCONTROLCHANNELS_HPP_

#include "../../../Frozen/simSupport/include/simSupport.h"
#include "../../PHY/include/FrameScheduler.h"
#include "../../PHY/include/Precoder.h"
#include "../../PHY/include/SupportingFunctions.h"

void
GenerateAndFillCoreset(SchedulerToTransmitterInfo_S schedulerToTransmitterInfo,
                       Array<cmat> &txFrame, RBInfo_S &dlRbInfoForUnScheduled,
                       int subframeNumber, int pdcchCount,
                       int cellID, int fftSize,int nDLRBs);

#endif // _DOWNLINKCONTROLCHANNELS_HPP_
