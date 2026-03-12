#ifndef _SRSTX_H_
#define _SRSTX_H_


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

#include "../../../Frozen/simSupport/include/CommonStructsAndEnums.h"



struct srsConfig{
  int UEid;
  int nSRSports;
  int seqLength;
  int cyclicShift;
  int u,v,K_TC;
  int rbOffset;
  int nRBs;
  ivec symbols;
  int combOffset;
  int periodicity,offset;
  double p0dBm;
  double srsTXpowerInWatts;
};


double getSRSUplinkPower(double PCmax, float alpha, double pathloss, srsConfig srsconfig);
Array<srsConfig> generateSRSconfig(AssociationInfo_S associationInfo, int nSRSPorts);
srsConfig getUEsrsConfig(Array<srsConfig> srsconfig, int UE);
cmat generateZCseq(srsConfig srs);



#endif














