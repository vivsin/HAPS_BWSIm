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


#ifndef RMSI_CONFIGURATIONS_H
#define RMSI_CONFIGURATIONS_H

#include <lib/Frozen/simSupport/include/simSupport.h>
#include <lib/L1/PHY/include/SupportingFunctions.h>

int ssbSubCarrierOffset;
int rmsiConfiguration;

enum SSB_CORESET_MUX_PATTERN_E{
    
    SSB_CORESET_MUX_PATTERN_UNINIT_=-1,
    SSB_CORESET_MUX_PATTERN_1_=1,
    SSB_CORESET_MUX_PATTERN_2_,
    SSB_CORESET_MUX_PATTERN_3_
};

enum prbOFFSET_E{
    
    PRB_OFFSET_IDX_UNINIT_=-1,
    PRB_OFFSET_IDX_0_,
    PRB_OFFSET_IDX_1_,
    PRB_OFFSET_IDX_2_,
    PRB_OFFSET_IDX_3_,
    PRB_OFFSET_IDX_4_,
    PRB_OFFSET_IDX_5_,
    PRB_OFFSET_IDX_6_,
    PRB_OFFSET_IDX_7_,
    PRB_OFFSET_IDX_8_,
    PRB_OFFSET_IDX_9_,
    PRB_OFFSET_IDX_10_,
    PRB_OFFSET_IDX_11_,
    PRB_OFFSET_IDX_12_,
    PRB_OFFSET_IDX_13_,
    PRB_OFFSET_IDX_14_,
    PRB_OFFSET_IDX_15_
};

enum TMO_E{    // timingAndMonitoringOccasions : TMO
    
    TMO_IDX_UNINIT_=-1,
    TMO_IDX_0_,
    TMO_IDX_1_,
    TMO_IDX_2_,
    TMO_IDX_3_,
    TMO_IDX_4_,
    TMO_IDX_5_,
    TMO_IDX_6_,
    TMO_IDX_7_,
    TMO_IDX_8_,
    TMO_IDX_9_,
    TMO_IDX_10_,
    TMO_IDX_11_,
    TMO_IDX_12_,
    TMO_IDX_13_,
    TMO_IDX_14_,
    TMO_IDX_15_,
};


struct prbOffsetConfig_S
{
    prbOFFSET_E prbOffsetIndx;
    SSB_CORESET_MUX_PATTERN_E Pattern;
    int nCORESET0RBs;//{24,48,96}
    int nCORESET0Symbols;//{1,2,3}
    int RBOffset;
    prbOffsetConfig_S():prbOffsetIndx(PRB_OFFSET_IDX_UNINIT_),Pattern(SSB_CORESET_MUX_PATTERN_UNINIT_),nCORESET0RBs(-1),nCORESET0Symbols(-1),RBOffset(-1){};
    void clear()
    {
        prbOffsetIndx=PRB_OFFSET_IDX_UNINIT_;
        Pattern=SSB_CORESET_MUX_PATTERN_UNINIT_;
        nCORESET0RBs=-1;
        nCORESET0Symbols=-1;
        RBOffset=-1;
    }
};

struct TMO_Config_S// TMO:timingAndMonitoringOccasions 
{
    TMO_E tmoIndx;
    double O;//O:GroupOffset, applicable only for SSB_CORESET_MUX_PATTERN_1_
    int N;//N:searchSpaceSetsPerSlot{1,2}, applicable only for SSB_CORESET_MUX_PATTERN_1_
    double M;//Depends on N & O, applicable only for SSB_CORESET_MUX_PATTERN_1_
    int firstSymbolIndxOfCoreset0;
    int sfnC0;//systemfameNumber to monitor for Coreset0
    ivec slotC0;//slots to monitor for Coreset0
    TMO_Config_S():tmoIndx(TMO_IDX_UNINIT_),O(-1),N(-1),M(-1),firstSymbolIndxOfCoreset0(-1),sfnC0(-1),slotC0(-1){};
    void clear()
    {
        tmoIndx=TMO_IDX_UNINIT_;
        O=-1.0;
        N=-1;
        M=-1.0;
        firstSymbolIndxOfCoreset0=-1; 
        sfnC0=-1;
        slotC0.set_length(0);
    }
};

struct rmsiConfig_S
{ 
    prbOFFSET_E prbOffsetIndx; 
    TMO_E tmoIndx;
    rmsiConfig_S():prbOffsetIndx(PRB_OFFSET_IDX_UNINIT_),tmoIndx(TMO_IDX_UNINIT_){};
};

struct pbchContents // needed to append other pbchContents later
{
    int kSSB;//if(ssbSubCarrierOffset = (0...11)for FR1,(0...23) for FR2) valid SSB; Else invalid SSB; 
    int SubCarrierSpacingCommon;// (15,30)kHz for FR1 ; (60,120)kHz for FR2
    rmsiConfig_S rmsiConfiguration;
    void init(string configFile);
    void update(int kSSB,int rmsiSCS,prbOFFSET_E prbOffsetIndx,TMO_E tmoIndx)
    {
        this->kSSB= kSSB;
        SubCarrierSpacingCommon=rmsiSCS;
        rmsiConfiguration.prbOffsetIndx=prbOffsetIndx;
        rmsiConfiguration.tmoIndx=tmoIndx;       
    }
};

struct Coreset0Tables
{
    Array<imat>  RbSymOffConfigTables;
    Array<mat>  TMOConfigTables;
    Coreset0Tables()
    {
        RbSymOffConfigTables.set_length(0);
        TMOConfigTables.set_length(0);
    }
};


class ssbCoreset0Configuration
{
    prbOffsetConfig_S prbOffsetConfiguration;
    TMO_Config_S timingAndMonitoringOccasionsConfiguration;
    int ssbSubCarrierSpacing;
    int ueTxBandwidth;
public:   
    void init(int ssbSCS,pbchContents& pbchContents);
    void init(int ssbSCS,pbchContents& pbchContents,int ueTxBandwidth);
    void clear()
    {
        prbOffsetConfiguration.clear();
        timingAndMonitoringOccasionsConfiguration.clear();
        ssbSubCarrierSpacing=-1;
        ueTxBandwidth=-1;
    }
    prbOffsetConfig_S getprbOffsetConfiguration(){return prbOffsetConfiguration;}
    TMO_Config_S getTimingAndMonitoringOccasions(){return timingAndMonitoringOccasionsConfiguration;}
};

void Coreset0Configuration(prbOffsetConfig_S *RbSymOffConfig,TMO_Config_S *tmoConfig,int ssbSCS,int rmsiSCS,int ssbIndx,int sfnSSB,int slotSSB, int kSSB, int ueTxBandwidth,rmsiConfig_S rmsiConfig, Coreset0Tables coreset0Tables);//sets prbOffset&TMO configurations based on rmsi Configuration,rmsiSCS&ssbSCS from Coreset0Tables
int nextValidSSB(int kSSB,int RMSIPDCCHConfiguration/*0...255*/,bool isFR1);//returns GSCN offset from current SSB GSCN
void LoadCoreset0Tables(string FileName,Coreset0Tables& Coreset0Tables);
int getSlotsPerFrame(int mu,bool isnormalCP);

#endif
