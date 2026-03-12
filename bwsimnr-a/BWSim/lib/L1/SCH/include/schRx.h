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

#ifndef _PDSCHRX_H_
#define _PDSCHRX_H_

#include "../../PHY/include/Codebook.h"
#include "../../PHY/include/ReferenceSignalGenerator.h"
#include "../../PHY/include/ChannelandIntrCovEstimator.h"
#include "../../../Frozen/mcell/include/StructsAndEnums.h"
#include "../../PHY/include/Equalizer.h"

struct DecoderOutput_S
{
    Array<bvec> outputBits;
    bvec isSuccess;
    Array<bvec> isSuccessPerCB;
    
    DecoderOutput_S()
    {
        outputBits.set_length(0);
        isSuccess.set_length(0);
        isSuccessPerCB.set_length(0);
    }
    
};

DecoderOutput_S processSCH(Array<cmat> & resourceGridPerRxAntenna, Array<vec> &harqBuffer, cmat interferenceCovariance , int slotIndex, CodingParameters &codingParam, FadingParameters_S fadingParam, double dopplerSpreadInHz, int nRBs, ivec allocatedRBs , int dmrsType, Array<Array<Array<cvec>>> channelPerAntennaPerSymbol="" , Array<cmat> precoderPerRB="");
vec demodulate(int Q,cvec ip,cvec channel="",double N0=1.0);

bvec decode(vec LLR, int rvid,vector<double> &hip,int E,int Q,double codeRate,CoderType_E coderType,CRCTYPE crcType,bool& isSuccess, bvec &isSuccessPerCB);


#endif
