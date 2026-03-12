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

#include "../include/Generate_PSS_and_SSS.h"

void ssBurstSetConfig::init(string fileName, string ssbPatternsPath, double samplingfrequency, double carrierFrequencyInGHz)
{ 
    #ifdef USING_SECURITY 
    BWSIM_Password_function();
    #endif
    
    double periodicityInms;
    string ssbPattern;
    ssbPatterns.set_length(4);
    parse(fileName,"periodicityInms",periodicityInms);
    for(int count=0;count< 4;count++)
    {   ssbPattern=ssbPatternsPath+"SSBlock"+toString(count)+".txt";
        parse(ssbPattern,"rbStructure",ssbPatterns(count));  
        if(count==0)
        {
            parse(ssbPattern,"pss",ssbRefNumbers.pssRefNumber);
            parse(ssbPattern,"sss",ssbRefNumbers.sssRefNumber);
            parse(ssbPattern,"pbch",ssbRefNumbers.pbchRefNumber);
            parse(ssbPattern,"pbchdmrs",ssbRefNumbers.dmrsRefNumber);  
        }
    }
    periodicity= ceil_i(periodicityInms/5);  //since ssb_periodicity is a multiple of 5msec and takes the integer values starting from 1.
    parse(fileName,"subcarrierSpacingInKHz",nu);
    parse(fileName,"offsetFromPRB0",offsetFromPRB0);
    parse(fileName,"halfFrameIndex",halfFrameIndex);
    parse(fileName,"actuallyTransmittedSSB",actuallyTransmittedSSB);
    setssBurstSetConfig(nu, samplingfrequency,carrierFrequencyInGHz,periodicity,halfFrameIndex);
    
}

void ssBurstSetConfig:: setssBurstSetConfig(int nu,double samplingfrequency, double carrierFrequencyInGHz, int periodicity, bool halfFrameIndex)
{
    this->carrierFrequencyInGHz=carrierFrequencyInGHz;
    this->samplingFrequency=samplingfrequency;
    this->nu=nu;
    this->periodicity=periodicity;
    this->halfFrameIndex=halfFrameIndex;
    if(carrierFrequencyInGHz==-1 || nu==-1)
    {
        cout<<"Invalid ssBurstSetConfiguration"<<endl;
        exit(0);
    }
    //reference: 3GPP TS 38.213
    if(carrierFrequencyInGHz<=3)
    {
        ssbStartSymbols.set_length(4);
        if     (nu==15)	          ssbStartSymbols="2 8 16 22";
        else if(nu==30)           ssbStartSymbols="2 8 16 22";
    }
    else if(carrierFrequencyInGHz<=6)
    {             
        ssbStartSymbols.set_length(8);
        if     (nu==15)	 ssbStartSymbols="2 8 16 22 30 36 44 50";
        else if(nu==30)  ssbStartSymbols="2 8 16 22 30 36 44 50";
    }
    else
    {
        ssbStartSymbols.set_length(64);
        if     (nu==120)	
        {
            ssbStartSymbols="4,8,16,20,32,36,44,48,74,78,86,90,102,106,114, 118,144,148,156,160,172,176,184,188,214,218,226,230,242,246,254,258,284,288,296,300,312, 316,324,328,354,358,366,370,382,386,394,398,424,428,436,440,452,456,464,468,494,498,506, 510,522,526,534,538";
        }
        else if     (nu==240)	
        {
            ssbStartSymbols="8,12,16,20,32,36,40,44,64,68,72,76,88,92,96,100, 148,152,156,160,172,176,180,184,204,208,212,216,228,232,236,240,288,292,296,300,312,316, 320,324,344,348,352,356,368,372,376,380,428,432,436,440,452,456,460,464,484,488,492,496, 508,512,516,520";
        }
        
    }

    
    if(actuallyTransmittedSSB.length())
        ssbStartSymbols=ssbStartSymbols(actuallyTransmittedSSB);
    
}
PSS_SSS_S generate_pss_and_sss(ivec serverNodes)
{
  PSS_SSS_S pss_sss;
  pss_sss.PSS.set_length (serverNodes.length());
  pss_sss.SSS.set_length (serverNodes.length());
  
  for(int i=0;i<serverNodes.length();i++) {
    int N_id_cell = serverNodes(i);
    int N_id_2 = mod(N_id_cell,3);
    int N_id_1 = (N_id_cell - N_id_2)/3;
    pss_sss.PSS(i)=zeros_c(127);
    pss_sss.PSS(i)=generate_pss(N_id_2);
    pss_sss.SSS(i)=zeros_c(127);
    pss_sss.SSS(i)=generate_sss(N_id_1, N_id_2);
  }
  return pss_sss;
}

cvec generate_pss(int N_id_2)
{
    
  // Validate N_id_2
  if(!(N_id_2 >= 0 && N_id_2 <= 2)) {
    cout << "ERROR: Invalid N_id_2 " << N_id_2 << endl;
    abort();
  }
    
  ivec d_pss = zeros_i(127);
  ivec n,m,nplusk,x;
  int k;
  
  /// Generate PSS for n=0,1,...,126
  n = getIntegers(0,126);
  k = 43*N_id_2;
  nplusk=n+k;
  m = mod (nplusk, 127);
  
  x=zeros_i(127);
  // Generate x
  x(0) = 0;
  x(1) = 1;
  x(2) = 1;
  x(3) = 0;
  x(4) = 1;
  x(5) = 1;
  x(6) = 1;
  
  for(int i_hat=0;i_hat<=119;i_hat++) {
    x(i_hat+7) = mod((x(i_hat) + x(i_hat+4)), 2);
  }
  
  for(int idx=0;idx<=126;idx++) {
    d_pss(idx) = 1 - 2*x(m(idx));
  } 
  
  return to_cvec(d_pss) ;
}


cvec generate_sss(int N_id_1,int N_id_2)
{
   // Validate N_id_1
  if(!(N_id_1 >= 0 && N_id_1 <= 335)) {
    cout << "ERROR: Invalid N_id_1 " << N_id_1 << endl;
    abort();
  }
  
  // Validate N_id_2
  if(!(N_id_2 >= 0 && N_id_2 <= 2)) {
    cout << "ERROR: Invalid N_id_2 " << N_id_2 << endl;
    abort();
  }
  
  ivec d_sss=zeros_i(127);
  
  ivec n,m,nplusm0,nplusm1,x_0,x_1;
  int k,m0,m1;
  // Generate m0 and m1
  m0      = 15*floor_i(N_id_1/112)+5*N_id_2;
  m1      = mod(N_id_1, 112);
  
  
  /// Generate PSS for n=0,1,...,126
  n = getIntegers(0,126);
  k = 43*N_id_2;
  nplusm0=n+m0;
  nplusm0=mod(nplusm0,127);
  nplusm1=n+m1;
  nplusm1=mod(nplusm1,127);
  
  x_0=zeros_i(127);  
  // Generate x_0 
  x_0(0) = 1;
  x_0(1) = 0;
  x_0(2) = 0;
  x_0(3) = 0;
  x_0(4) = 0;
  x_0(5) = 0;
  x_0(6) = 0;
  
  for(int i_hat=0;i_hat<=119;i_hat++) {
    x_0(i_hat+7) = mod((x_0(i_hat+4) + x_0(i_hat)), 2);
  }
  
  x_1=zeros_i(127);
  // Generate x_1
  x_1(0) = 1;
  x_1(1) = 0;
  x_1(2) = 0;
  x_1(3) = 0;
  x_1(4) = 0;
  x_1(5) = 0;
  x_1(6) = 0;
  for(int i_hat=0;i_hat<=119;i_hat++) {
    x_1(i_hat+7) = mod((x_1(i_hat+1) + x_1(i_hat)), 2);
  }
  
  for(int idx=0;idx<127;idx++) {
    d_sss(idx) = (1 - 2*x_0(nplusm0(idx)))*(1 - 2*x_1(nplusm1(idx)));
  }
  
 return to_cvec(d_sss);
}

cvec getDummyPBCHsymbols(int seqLength)
{
    cvec QPSK_symbols(4);
    QPSK_symbols(0)=dComplex(0.7071,0.7071); 
    QPSK_symbols(1)=dComplex(0.7071,-0.7071); 
    QPSK_symbols(2)=dComplex(-0.7071,0.7071); 
    QPSK_symbols(3)=dComplex(-0.7071,-0.7071); 
    cvec dummySymbols(seqLength);
    ivec dummyData=randi(seqLength,0,3);
    for (int i=0;i<seqLength;i++)
     dummySymbols(i)=QPSK_symbols(dummyData(i)); 
    return dummySymbols;
};

cvec generate_PbchDMRS(int cellID, int iSSB, int nHf, int seqLength)
{
    ivec goldSeq;
    cvec pbch_dmrs;
    int Cinit;
    
    int jSSB=iSSB+4*nHf;
    pbch_dmrs=zeros_c(seqLength);
    Cinit = pow(2.0,11.0)*(jSSB+1)*(floor_i(cellID/4)+1)+ pow(2.0,6.0)*(jSSB+1)+ mod(cellID,4);
    
    goldSeq = generateGoldSequence(Cinit,2*seqLength);
    
    for (int iSeq = 0;iSeq < (seqLength);iSeq ++)
    {
    pbch_dmrs(iSeq) = dComplex((1.0 - 2.0 * goldSeq(2 * iSeq)),(1.0 - 2.0 * goldSeq(2 * iSeq + 1))) / sqrt(2.0);
    }
    return pbch_dmrs;
}


cmat GenerateAndFillPBCH(imat &frameStructure, ssbRefNumbers_S &ssbRefNumbers, int cellID, int L, int iSSB, bool nHf)
{ 
    //frameStructure is (fftx14) imat giving ssb resources  
    cmat txFrame=zeros_c(frameStructure.rows(),frameStructure.cols());  
    int nSSB=mod(iSSB,8);
    cvec pbch_dmrs;
    //nHf is applicable only for L=4 case; L= max number of SSB's in SS burst set.
    if(L==4) pbch_dmrs=generate_PbchDMRS(cellID,nSSB,(int)nHf);
    else pbch_dmrs=generate_PbchDMRS(cellID,nSSB);
    
    cvec pbch=getDummyPBCHsymbols();// generate dummy QPSK modulated symbols to fill into PBCH
    setInMatrix(txFrame,find(frameStructure,ssbRefNumbers.dmrsRefNumber,true),pbch_dmrs);
    setInMatrix(txFrame,find(frameStructure,ssbRefNumbers.pbchRefNumber,true),pbch);
    return txFrame;
}


cmat GenerateAndFillPSSandSSS(imat &frameStructure, ssbRefNumbers_S &ssbRefNumbers, int cellID)
{ 
  //frameStructure is (fftx4) imat giving ssb resources   
  cmat txFrame=zeros_c(frameStructure.rows(),frameStructure.cols());  
  int N_id_2 = mod(cellID,3);
  int N_id_1 = (cellID - N_id_2)/3;
    cvec pssSymbol=generate_pss(N_id_2);
    cvec sssSymbol=generate_sss(N_id_1,N_id_2);
    setInMatrix(txFrame,find(frameStructure,ssbRefNumbers.pssRefNumber,true),pssSymbol);
    setInMatrix(txFrame,find(frameStructure,ssbRefNumbers.sssRefNumber,true),sssSymbol);
    return txFrame;
}



