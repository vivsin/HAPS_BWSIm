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


#ifndef _RES_MAPPER_H_
#define _RES_MAPPER_H_

#include "StructsAndEnums.h"

/*3GPP TS 36.211 version 10.1.0 Release 10
 Refer section 6.2.3.2 Virtual resource blocks of distributed type for
 equations used here.

-------------Ngap1 and Ngap2 are both considered------------------------ 
------------3GPP TS 36.211 version 10.1.0 Release 10-------------
------------Table 6.2.3.2-1: RBG gap values-------------------
*/

class DownlinkResourceMapper
{
  private:
    ivec fftSizePerBandwidth;
    ivec rbgSizePerBandwidth;
    
    int nDLRB;
    int RBG_Size;
    Array<ivec> nPRB;
    Size_S rbSize;
    int fftSize;
    int guardSubcarriersInOneSide;
    int usedSubcarriers;
    int dcSubcarrier;
    ivec isLocalisedRB;
    
  public:
    DownlinkResourceMapper();
    void init(int fft_size, int nRB, Size_S resourceBlockSize, bool isLocalised = true, bool gap1 = true);
    Size_S getRBSize();
    int getResourceBlockGroupingSize();
    int getResourceBlocksPerSubframe();
    int getFFTSize();
    int getUsedSubcarriers();
    ivec getPRBIndex(int nVRB);
    Array<ivec> getPRBMapping();
    bool isLocalised(int VRBNumber);
    Array<ivec> getPRBSubcarrierStartLocationinEachSlot(int nVRB);
    Array<ivec> getLocalisedSubcarrierStartLocationinEachSlot(int PRBNumber);
    int getGuardSubcarriersInOneSide();
    
    
};

enum HoppingMode_E
{
  _HOPPING_MODE_INTER_,
  _HOPPING_MODE_INTRA_INTER_
};

enum HoppingType_E
{
  _HOPPING_TYPE_00_, 
  _HOPPING_TYPE_01_, 
  _HOPPING_TYPE_10_, 
  _HOPPING_TYPE_11_  
};



class UplinkResourceMapper
{
  private:
    ivec fftSizePerBandwidth;
    ivec rbgSizePerBandwidth;
    
    int nULRBs;
    int RBG_Size;
    Size_S rbSize;
    int fftSize;
    int guardSubcarriersInOneSide;
    int usedSubcarriers;
    int dcSubcarrier;
    HoppingMode_E hoppingMode;
    HoppingType_E hoppingType;
    
    ivec vrbNumbers;
    Array<ivec> nPRB;
    
  public:
    UplinkResourceMapper();
    void init(int fft_size, int nRB, Size_S resourceBlockSize, HoppingMode_E hoppingMode=_HOPPING_MODE_INTRA_INTER_, HoppingType_E hoppingType=_HOPPING_TYPE_00_);
    Size_S getRBSize();
    int getResourceBlockGroupingSize();
    int getResourceBlocksPerSubframe();
    int getFFTSize();
    int getUsedSubcarriers();
    ivec getPRBIndex(int nVRB);
    Array<ivec> getPRBSubcarrierStartLocationinEachSlot(int rbNumber);
    Array<ivec> getLocalisedSubcarrierStartLocationinEachSlot(int prbNumber);
    int getGuardSubcarriersInOneSide();
    Array<ivec> performType2hopping(int N_SB , int N_HO_RB, int L_CRBS, int RB_START, int Subframe_number,int CURRENT_TX_NB, int NCellID, int UL_N_RB);
    void perform_UL_Freq_hopping(int Subframe_No,int RB_START,int L_CRBS,int N_SB, int NCellID, bool isWithHopping, int N_HO_RB=0);
    
};

class ResourceMapper
{
  public:
    DownlinkResourceMapper dlResourceMapper;
    UplinkResourceMapper ulResourceMapper;
};



#endif
