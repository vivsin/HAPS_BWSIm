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

#ifndef _AWGN_TABLES_H_
#define _AWGN_TABLES_H_

#include "StructsAndEnums.h"
#include "SupportingFunctions.h"

enum ModulationScheme_E{
  _MODULATION_SCHEME_UNINIT_=-1,
  _MODULATION_SCHEME_BPSK_=1,
  _MODULATION_SCHEME_QPSK_=2,
  _MODULATION_SCHEME_16_QAM_=4,
  _MODULATION_SCHEME_64_QAM_=6,
  _MODULATION_SCHEME_256_QAM_=8,
  _MODULATION_SCHEME_1024_QAM_=10,
  _MODULATION_SCHEME_4096_QAM_=12,
};

enum CodeRate_E{
  _CODE_RATE_UNINIT_=-1,
  _CODE_RATE_78b1024_,
  _CODE_RATE_120b1024_,
  _CODE_RATE_193b1024_,
  _CODE_RATE_308b1024_,
  _CODE_RATE_449b1024_,
  _CODE_RATE_602b1024_,
  _CODE_RATE_378b1024_,
  _CODE_RATE_490b1024_,
  _CODE_RATE_616b1024_,
  _CODE_RATE_466b1024_,
  _CODE_RATE_567b1024_,
  _CODE_RATE_666b1024_,
  _CODE_RATE_772b1024_,
  _CODE_RATE_873b1024_,
  _CODE_RATE_948b1024_
  
};

class AWGNTables
{
 
public:
  bool is256QAMenabled = false;
  bool isPi2BPSKenabled = false;
private:
  
  //required for NR
  bool isDL;
  TransmissionMode_E txMode = _TRANSMISSION_MODE_NR_ ;
  
  ivec modulationSchemePerCQI;
  vec codeRatePerCQI;
  map<ModulationScheme_E,int> moscToIndex;
  Array<vec>mmibTable;
  ivec TBSizeWithCrc;
  Array< mat > bler;
  double snrBlerStartIndex;
  double snrBlerResolution;
  vec mmibBlerStartIndex;
  vec mmibBlerResolution;
  ivec interleaveParametersK;
  
    
public:
    AWGNTables();
    double sinrToEffMMIB(vec& sinr, ModulationScheme_E mosc); 
    double sinrToEffMMIBusingLookUp(vec& sinr, ModulationScheme_E mosc); 
    const vec& getBlerTable(string filename);
    const vec& getMMIBTable(ModulationScheme_E mosc);
//     double sinr_to_mmib(const vec &sinr, const vec &sinr_mi_table,double stsinr,double sinrresolution)const;
//     vec JFunction(const vec& sinr);
//     double JinvFunction(double effmmib);
    double getBlerFromMmib(int cqi, double mmib, ModulationScheme_E mosc, int tbsize);
    double getBlerFromMmibForAbstraction(double codeRate, double effSinr, ModulationScheme_E mosc, int tbsizeWithCRC,vec &blerPerCB, double tCodeRate = 0.0);
    double getBlerForIdealAbstraction(int nTransmissions, double targetBLER);
    double mmibToSinr(ModulationScheme_E mosc,double mmib);
    int CQISelect(double &exp_spec_eff,  double mmib_256, double mmib_sxfr,double mmib_sxtn,double mmib_qpsk,ivec TBsizePerCQI, bool enableCQI0=false);
    int CQISelect(double &exp_spec_eff,  map<ModulationScheme_E,double>& MMIB,ivec TBsizePerCQI, bool enableCQI0);
    int mcsSelect(double& exp_spec_eff, vec& sinr, ivec tbsizeWithCRC, vec codeRate, vec spec_eff, bool isDownlink, double sinrOffsetIndB , vec targetBLER );
    double getSINRIndBfromCQI(int CQI);
    double getRateOffset(double rate, double powerOffsetIndB);
    vec getSINRIndBfromCQI(ivec CQI);
    Array<vec> getSINRIndBfromCQI(Array<ivec> CQI);
    int getCQIfromSINR(double SINRIndB);
    ivec getCQIPerCWfromSINR(vec SINRperCW);
    void init_AWGNTables(bool isDownlink, string TBMCSmappingFile, string cqiToMCSFile);
    double sinrToEffSINRindB(vec& sinr, int modOrder);
};


class AWGNTablesConvCoder : public AWGNTables
{
  ivec PDUSizeWithCrc;
  ivec aggLevels;
  Array< mat > berCC;
  Array< mat > blerCC;
  double snrBlerStartIndexCC;
  double snrBlerResolutionCC;
  
public:
  
  AWGNTablesConvCoder();
  double getErrorProbabilityForPDCCH(int aggLevel, double SinrIndB, int tPDUSizeWithCrc);
  int aggLevelSelect(ivec CQIs, int tPDUSizeWithCrc);
  
};

void mcsidxMapping(bool is256QAMEnabled, bool isPiby2BPSKEnabled, int MCSidx,ModulationScheme_E &mosc,bool isDownlink);
#endif


