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


#ifndef GEN_PSS_SSS_H
#define GEN_PSS_SSS_H

#include <lib/Frozen/simSupport/include/simSupport.h>
#include <lib/L1/PHY/include/SupportingFunctions.h>


struct ssbRefNumbers_S{
    
    int pssRefNumber;
    int sssRefNumber;
    int pbchRefNumber;
    int dmrsRefNumber;
};

   
    struct ssBurstSet{
    ivec ssbStartSymbols;
    ivec ssbIndices;
    ssBurstSet(){ssbStartSymbols.set_length(0);ssbIndices.set_length(0);}
    void clear(){ssbStartSymbols.set_length(0);ssbIndices.set_length(0);}
    void init(ivec ssbStartSymbols)
    {
     this->ssbStartSymbols.set_length(length(ssbStartSymbols));
     this->ssbIndices.set_length(length(ssbStartSymbols));
     this->ssbStartSymbols=ssbStartSymbols;
     this->ssbIndices=getIntegers(0,length(ssbStartSymbols)-1);
    }
    };
    
    class ssBurstSetConfig
    {
    int nu;// subcarrier spacing in KHz
    double carrierFrequencyInGHz;//in GHz
    double samplingFrequency;
    ivec ssbStartSymbols;
    int periodicity;//periodicity is a multiple of 5msec
    bool halfFrameIndex;//{0,1}={first half-frame,second half-frame}
    int offsetFromPRB0;// multiple of ssb nu 
    ivec actuallyTransmittedSSB;
    ssbRefNumbers_S ssbRefNumbers;
    Array<imat> ssbPatterns; 
    
    public:
    bool isInitialized(){return (nu!=-1);}  
    int ssbIndx=0;
    ssBurstSetConfig(){ nu=-1;carrierFrequencyInGHz=-1;periodicity=-1;}
    ssBurstSetConfig(int nu,double carrierFrequencyInGHz){setssBurstSetConfig(nu,carrierFrequencyInGHz,4,0);}/*default periodicity assumed by initial access UE=20msec*/
    ssBurstSetConfig(int nu,double carrierFrequencyInGHz, int periodicity,bool halfFrameIndex){setssBurstSetConfig(nu,carrierFrequencyInGHz,periodicity,halfFrameIndex);}
    void clear(){nu=-1;carrierFrequencyInGHz=-1;ssbStartSymbols.set_length(0);periodicity=-1;} 
    void init(string configFile, string ssbPatternPath,double samplingfrequency, double carrierFrequencyInGHzinGHz);
    void setssBurstSetConfig(int nu,double samplingfrequency,double carrierFrequencyInGHz,int periodicity,bool halfFrameIndex=0);
    ivec getssBurstSetConfig(){return ssbStartSymbols;}
    bool getHalfFrameIndex(){return halfFrameIndex;}
    int  getSSBscs(){return nu;}
    int  getSSBPeriodicity(){return periodicity;}
    int  getSSBoffsetFromPRB0inKHz(){return offsetFromPRB0;}
    double getSamplingFrequency(){return samplingFrequency;}
    imat getSSBPattern(int n){return ssbPatterns(n);}
    ssbRefNumbers_S getSSBRefNumbers(){return ssbRefNumbers;}
    };
  
cvec generate_pss(int N_id_2);
cvec generate_sss(int N_id_1,int N_id_2);
cvec generate_PbchDMRS(int cellID, int iSSB, int nHf=0,int seqLength=144);

struct PSS_SSS_S {
  Array<cvec> PSS;
  Array<cvec> SSS;
};

PSS_SSS_S generate_pss_and_sss(ivec serverNodes);
cmat GenerateAndFillPSSandSSS(imat &frameStructure, ssbRefNumbers_S &ssbRefNumbers, int cellID);
cmat GenerateAndFillPBCH(imat &frameStructure, ssbRefNumbers_S &ssbRefNumbers, int cellID,int L, int iSSB, bool nHf=0);
cvec getDummyPBCHsymbols(int seqLength=432);

#endif
