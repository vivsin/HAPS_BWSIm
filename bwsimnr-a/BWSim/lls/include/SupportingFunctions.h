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

#ifndef LLS_SUPP_H
#define LLS_SUPP_H

#include "StructsAndEnums.h"

cvec generateNoiseSamples(double N0,int nSamples);
void parseAntennaParams(AntennaParameters& antParam,string fileName);
void loadAntennaParam(Antenna& antenna,AntennaParameters& antParam,int index,double carrierFrequency,bool enableAAS = true);
#endif // MAIN_SUPP_H
