/*************************************************************************
 * 
 * CEWiT CONFIDENTIAL
 * __________________
 *
 * All Rights Reserved © 2014 CEWiT, India
 *
 * NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
 * and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
 * Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
 * express, printed and signed license for use is strictly forbidden.
 */


#ifndef DLSYNC_DETECTOR_H
#define DLSYNC_DETECTOR_H

#include "Generate_PSS_and_SSS.h"

struct CellSearchOutput_S{
  
  int UE_id;
  int Nid1;
  int Nid2;
  int cellID;
  bool isFDD;
  bool isNormalCP;
  
  Array<cvec> TimeDomainSamples;
  
  void PSSdetection();
  void SSSdetection();
  
  CellSearchOutput_S(){
    UE_id=-1;
    Nid1=-1;
    Nid2=-1;
    cellID=-1;
    isFDD=false;
    isNormalCP=false;
    
    TimeDomainSamples.set_length(0);
 
  }
};

CellSearchOutput_S cellSearchUsingPSSandSSS(Array<cvec> timeDomainSamples);

struct IvecAndInt {
    ivec start_loc_vec;
    int freq_offset;
};

IvecAndInt find_coarse_time_and_freq_offset(cvec, int);

struct PSS_struct {
  ivec fine_start;
  int N_id_2,pss_symb;
  double pss_thresh;
};

PSS_struct find_pss_and_fine_timing(cvec ,ivec);
cvec samps_to_symbs(cvec, int, int, int, int);


struct SSS_struct {
  int N_id_1;
  int f_start_idx;
};

SSS_struct find_sss(cvec,int,ivec,double);
void DownlinkSyncDetector(cvec input_samps);


#endif // DLSYNC_DETECTOR_H