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

#ifndef _LYR_MPR_H_
#define _LYR_MPR_H_

#include "ChannelCoder.h"

Array<ivec> getLayerToCWMapping(int nLayers,int nCodeWords);
Array<cvec> performLayerMapping(Array<cvec> &modulatedSymbolsPerCodeWord , int nCodewords, int nLayers);
Array<cvec> performLayerDeMapping(Array<cvec> &equalizedSymbolsPerLayer, int nCodewords, int nLayers);
Array<vec> performLayerDeMapping(Array<vec> &ppSINRPerLayer, int nCodewords, int nLayers);
Array<vec> performLayerMapping(Array<vec>& sinrPerCodeword,int nCodewords,int nLayers);
Array<vec> performLayerDeMapping(vec& sinrPerLayer, int nCodewords, int nLayers);

#endif
