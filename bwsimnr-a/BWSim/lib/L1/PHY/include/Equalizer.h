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

#ifndef _EQUA_H_
#define _EQUA_H_

#include "FrameScheduler.h"


EqualizerOutput_S equalizeMyDataInRB(Array< cvec >& rxDataPerRB, Array< cmat >& precodedChannelPerRB, Array< Array< cmat > > intrPrecodedChannelPerRB,ivec IntrSFBCLayers, double txScPowInWatts, vec intrTxPowInWatts, Array< cmat >& estIntrCovPerRB, double restOfInterferenceWithNoiseVariance, DemodulationScheme_E demodulationScheme, bool isEstimatedIntrcov, Waveform_E waveform);
EqualizerOutput_S equlizeOfdma(Array<cvec> &rxDataPerRB, Array< cmat > &precodedChannelPerRB, Array< Array< cmat >  > &intrPrecodedChannelPerRB,ivec IntrSFBCLayers, double txScPowInWatts, vec &intrTxPowInWatts, Array<cmat> &estIntrCovPerRB, double restOfInterferenceWithNoiseVariance, DemodulationScheme_E demodulationScheme, bool isEstimatedIntrcov);
EqualizerOutput_S txDivDecoder(Array<cvec> &rxDataPerRB, Array< cmat > &precodedChannelPerRB, Array< Array< cmat >  > &intrPrecodedChannelPerRB,ivec IntrSFBCLayers, double txScPowInWatts, vec &intrTxPowInWatts, Array<cmat> &estIntrCovPerRB, double restOfInterferenceWithNoiseVariance, DemodulationScheme_E demodulationScheme,bool isEstimatedIntrcov);
//EqualizerOutput_S CLMUMIMOdecoder(DataBlkInfo_S dataBlkInfo, int rbNumber, Array<cvec> &rxDataPerRB, Array< cmat > &precodedChannelPerRB, Array< Array< cmat >  > &intrPrecodedChannelPerRB,ivec IntrSFBCLayers, double txScPowInWatts,vec &intrTxPowInWatts, Array<cmat> &estIntrCovPerRB, double restOfInterferenceWithNoiseVariance, DemodulationScheme_E demodulationScheme, bool isEstimatedIntrcov);
void MRCrx(cmat precodedChannel, cmat rxsym, cmat& EquNoise, cmat& estsym, cmat& EqChn,vec &ppSINR, cmat IntrCov, int nLayers, double txScPowInWatts);
void whiteMRCrx(cmat precodedChannel, cmat rxsym, cmat& EquNoise, cmat& estsym, cmat& EqChn,vec &ppSINR, cmat IntrCov, int nLayers, double txScPowInWatts);
void MMSErx(cmat precodedChannel, cmat rxsym, cmat& EquNoise, cmat& estsym, cmat& EqChn,vec &ppSINR, cmat IntrCov, int nLayers, double txScPowInWatts);

EqualizerOutput_S equalizeControlChannel(int nLayers, Array< cvec >& rxData, Array< cmat >& myChannel, Array< Array< cmat > > intrChannelPerRB, double txScPowInWatts, vec intrTxPowInWatts, Array< cmat >& estIntrCovPerRB, double restOfInterferenceWithNoiseVariance, DemodulationScheme_E demodulationScheme, bool isEstimatedIntrcov);

vec getSINRperLayer(DemodulationScheme_E desc, cmat precodedChannel,cmat Intrcov, int nLayers, double sigpowInWatts);


#endif
