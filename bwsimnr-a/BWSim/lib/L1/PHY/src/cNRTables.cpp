#include "../include/cNRTables.h"


double computeAvgSINRindB(vec SINRPerCWindB, int rank){
  
  double avgSINR = 1.0;
  vec SINRPerCW = inv_dB(SINRPerCWindB);
  ivec nLayersPerCW = getNumLayersPerCodeWord(SINRPerCW.length(),rank);
  
  for(int iCW = 0; iCW < SINRPerCW.length(); iCW++)
  {
    avgSINR *= pow((1.0 + SINRPerCW(iCW)),nLayersPerCW(iCW));
  }
  avgSINR = avgSINR - 1.0;
  
  return dB(avgSINR);
}


ivec cTBTables::getCodeBlockSizes(int tbSize, double tCodeRate)
{
  return getCodeBlockSizesForTBNR(tbSize,tCodeRate);
}

int cTBTables::getNumberOfCodeBlocks(int tbSize, double tCodeRate)
{
  return getCodeBlockSizes(tbSize,tCodeRate).length();
}

int cTBTables::getNumberOfCRCbits(int tbSize, double tCodeRate)
{
  int nBlocks = getNumberOfCodeBlocks(tbSize,tCodeRate), L = 24;
  int nCRC = (nBlocks>1) ? (nBlocks+1)*L : L;
 
  return nCRC;
}


int cTBTables::quantizeN_RE_Prime(int nREPrime)
{
  int nREPrimeDash=0;
    if(nREPrime<=9)
      nREPrimeDash = 6;
    else if(nREPrime> 9 && nREPrime<=15)
      nREPrimeDash = 12;
    else if(nREPrime> 15 && nREPrime<= 30)
      nREPrimeDash =18;
    else if(nREPrime> 30 && nREPrime<= 57)
      nREPrimeDash = 42;
    else if(nREPrime> 57 && nREPrime<= 90)
      nREPrimeDash = 72;
    else if(nREPrime> 90 && nREPrime<= 126)
      nREPrimeDash = 108;
    else if(nREPrime> 126 && nREPrime<= 150)
      nREPrimeDash =144;
    else if(nREPrime>=150)
      nREPrimeDash = 156;
    
    return nREPrimeDash;
}

int cTBTables::getTBSizeForSINR(vec SINR,int nRBs, int nREs, int nSymbols, int nLayersInCW, bool isDownlink, double sinrOffsetIndB, double targetBLER)
{

  int TBSize = 0;
  int nMCS;
  if(isDownlink)
    nMCS= transportBlockMCSMapping.modulationAlphabetPDSCH.length();
  else
    nMCS=transportBlockMCSMapping.modulationAlphabetPUSCH.length();
  
  ivec tbSizesPerMCS = zeros_i(nMCS);
  vec codeRatesPerMCS= zeros(nMCS);
  vec specEffPerMCS= zeros(nMCS);

  for(int mcs_cnt = 0; mcs_cnt < nMCS; mcs_cnt++)
  {
    int mcsIndex;
    if(isDownlink)
      mcsIndex = transportBlockMCSMapping.mcsIndex(mcs_cnt);
    else
      mcsIndex = transportBlockMCSMapping.nrMcsIndicesPUSCH(mcs_cnt);

    int modulationAlphabet;
    double tCodeRate;
    
    if(isDownlink)
    {
      modulationAlphabet = transportBlockMCSMapping.modulationAlphabetPDSCH (mcs_cnt);
      tCodeRate = transportBlockMCSMapping.nrMCSCodeRatePDSCH(mcs_cnt);
    }
    else
    {
      modulationAlphabet = transportBlockMCSMapping.modulationAlphabetPUSCH(mcs_cnt);
      tCodeRate = transportBlockMCSMapping.nrMCSCodeRatePUSCH(mcs_cnt);
    }
      
    tbSizesPerMCS(mcs_cnt) = getTBSizeForAllLayersCombined(mcsIndex,nREs,nRBs, nLayersInCW,nSymbols,isDownlink);
    
    int nCRC = getNumberOfCRCbits(tbSizesPerMCS(mcs_cnt),tCodeRate);
    
    codeRatesPerMCS(mcs_cnt) = (tbSizesPerMCS(mcs_cnt)+nCRC)/(double)(nREs*nLayersInCW * modulationAlphabet);
    specEffPerMCS(mcs_cnt) = (double)tbSizesPerMCS(mcs_cnt)/(double)nREs;
  }
  
  int MCS = 0;
  
  double rate=0.0;
  vec targetBLERPerMCS=zeros(codeRatesPerMCS.length())+targetBLER;
  MCS = awgnTablesConvCoder.mcsSelect(rate,SINR,tbSizesPerMCS+24,codeRatesPerMCS,specEffPerMCS,isDownlink,sinrOffsetIndB,targetBLERPerMCS);
  TBSize = tbSizesPerMCS(MCS);
  
  return TBSize;
}

int cTBTables::getModOrder(int mcsIndex, bool isDownlink)
{
  int modOrder = -1;
  if(isDownlink)
    modOrder = transportBlockMCSMapping.modulationAlphabetPDSCH(mcsIndex);
  else
    modOrder = transportBlockMCSMapping.modulationAlphabetPUSCH(mcsIndex);
  
  return modOrder;
}

int cTBTables::getCodeRate(int mcsIndex, bool isDownlink)
{
  double CR = 0.0;
  if(isDownlink)
    CR = transportBlockMCSMapping.nrMCSCodeRatePDSCH(mcsIndex);
  else
    CR = transportBlockMCSMapping.nrMCSCodeRatePUSCH(mcsIndex);
  
  return CR;
}

// ivec cTBTables::getTBSizeForAllLayersCombined(const ivec& mcsIndices,int nREsAllocated, int allocatedPRBs, int numLayers, int allocatedSymbols, int cqiIndex)
// {
//   
//   //38.214 v 15.1.0 section 5.1.3.2 
//   int N_RE=0,N_RE_Prime=0, N_RE_Prime_dash=0;
//   double  N_info =0, RQm_maxRate=0;
//   RQm_maxRate = cqiToMaxMCSRate.getCQICodeRate(cqiIndex);
//   if(RQm_maxRate<=0) 
//   {
//     cout << "maxCQIRate not set ... aborting" <<endl;abort();
//   }
//   vec tempRate;
//   ivec TBSizes, mcsIndicesToConsider;
//   tempRate.set_length(mcsIndices.length());
// //   int nPRB=0,N_RB_SC=12, N_sh_symb=0, N_PRB_DMRS=0, N_PRB_oh=0;   
// //   double  N_info =0,TBS=0;
// //   int R=0,Qm=0,v=0;
// //   int Xoh_PDSCH=0; //(if not configured to either 0,6,12,or 18 assume to be zero) 
// //   N_PRB_oh = Xoh_PDSCH;
// //   N_RE_Prime = N_RB_SC*N_sh_symb - N_PRB_DMRS - N_PRB_oh;
//   for(int i=0;i<mcsIndices.length();i++)
//   {
//     tempRate(i) = transportBlockMCSMapping.modulationAlphabetPDSCH(i)*transportBlockMCSMapping.nrMCSCodeRatePDSCH(i);
//   }
//   mcsIndicesToConsider = mcsIndices(find(tempRate<=RQm_maxRate));
//   TBSizes.set_length(mcsIndicesToConsider.length());
//   N_RE_Prime = nREsAllocated;
//   if(N_RE_Prime<=0) 
//   {
//     cout << "nRE not set ... aborting" <<endl;abort();
//   }
//   N_RE_Prime_dash = quantizeN_RE_Prime(N_RE_Prime);
//   N_RE = N_RE_Prime_dash*allocatedPRBs;
//   for(int k=0; k<mcsIndicesToConsider.length();k++)
//   {
//     N_info = N_RE*tempRate(k)*numLayers; //(N_RE*R*Qm*v)
//     if(N_info<=3824)
//     {
//       vector<double> TBS_Table = {24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128, 136, 144, 152, 160, 168, 176, 184, 192, 208, 224, 240, 256, 272, 288, 304, 320, 336, 352, 368, 384, 408, 432, 456, 480, 504, 528, 552, 576, 608, 640, 672, 704, 736, 768, 808, 848, 888, 928, 984, 1032, 1064, 1128, 1160, 1192, 1224, 1256, 1288, 1320, 1352, 1416, 1480, 1544, 1608, 1672, 1736, 1800, 1864, 1928, 2024, 2088, 2152, 2216, 2280, 2408, 2472, 2536, 2600, 2664, 2728, 2792, 2856, 2976, 3104, 3240, 3368, 3496, 3624, 3752, 3824}; 
//       //TBStable = vec("24 24 32 40 48 56 64 72 80 88 96 104 112 120 128 136 144 152 160 168 176 184 192 208 224 240 256 272 288 304 320 336 352 368 384 408 432 456 480 504 528 552 576 608 640 672 704 736 768 808 848 888 928 984 1032 1064 1128 1160 1192 1224 1256 1288 1320 1352 1416 1480 1544 1608 1672 1736 1800 1864 1928 2024 2088 2152 2216 2280 2408 2472 2536 2600 2664 2728 2792 2856 2976 3104 3240 3368 3496 3624 3752 3824");
//       double N_info_prime=0,n=0,tmp=0;
//       tmp = floor(log2(N_info))-6.0;
//       n = (3.0>=tmp)? 3.0:tmp;
//       tmp = pow(2,n)*floor(N_info/pow(2,n));
//       N_info_prime = (24.0>=tmp)? 24.0:tmp;
//       TBSizes(k) = closest(TBS_Table,N_info_prime); //selects A TBS from Table 5.1.3.2-2 which is closest or equal to N_info_prime but is not less than N_info_prime
//     }
//     else
//     {
//       double N_info_prime=0,n=0;
//       n = floor(log2(N_info-24.0))-5.0; 
//       N_info_prime = pow(2,n)*((((int)N_info-24.0)/(int)pow(2,n))+1.0);  // round functions in the formula takes next largest integer. Son made the division in int and added 1 to make sure it always takes the next largest value.
//       
//       double C=0,div=0;
//       div = (tempRate(k)<=0.25)?3816.0:(N_info_prime>8424)?8424.0 : 1.0;
//       C = ceil((N_info_prime+24)/div);
//       TBSizes(k) = 8*C*ceil((N_info_prime+24.0)/(8*C))-24;
//     }
//   }
//   return TBSizes;
// }


ivec cTBTables::getTBSizeForAllLayersCombined(const ivec& mcsIndices,int nREsAllocated, int allocatedPRBs, int numLayers, int allocatedSymbols, int cqiIndex)
{
  
  //38.214 v 15.1.0 section 5.1.3.2 
  int N_RE=0,N_RE_Prime=0, N_RE_Prime_dash=0;
  double  N_info =0, RQm_maxRate=0;
  RQm_maxRate = cqiToMaxMCSRate.getCQICodeRate(cqiIndex)*cqiToMaxMCSRate.getCQIModulationAlphabet(cqiIndex);
  if(RQm_maxRate<=0) 
  {
    cout << "maxCQIRate not set ... aborting" <<endl;abort();
  }
  
  vec tempRate;
  ivec TBSizes, mcsIndicesToConsider;
  tempRate.set_length(mcsIndices.length());
  //   int nPRB=0,N_RB_SC=12, N_sh_symb=0, N_PRB_DMRS=0, N_PRB_oh=0;   
  //   double  N_info =0,TBS=0;
  //   int R=0,Qm=0,v=0;
  //   int Xoh_PDSCH=0; //(if not configured to either 0,6,12,or 18 assume to be zero) 
  //   N_PRB_oh = Xoh_PDSCH;
  //   N_RE_Prime = N_RB_SC*N_sh_symb - N_PRB_DMRS - N_PRB_oh;
  for(int i=0;i<mcsIndices.length();i++)
  {
    tempRate(i) = transportBlockMCSMapping.modulationAlphabetPDSCH(i)*transportBlockMCSMapping.nrMCSCodeRatePDSCH(i);
  }
  ivec desiredIndices=find(tempRate<=RQm_maxRate);
  if(desiredIndices.length() == 0) // If desired MCS index is not found, zeroth index is considered.
  {
    desiredIndices = ivec("0");
  }
  mcsIndicesToConsider = mcsIndices(desiredIndices);
  tempRate=tempRate(desiredIndices);
  
  TBSizes.set_length(mcsIndicesToConsider.length());
  for(int k=0; k<mcsIndicesToConsider.length();k++)
  {
    TBSizes(k)=getTBSizeForAllLayersCombined(mcsIndicesToConsider(k), nREsAllocated, allocatedPRBs, numLayers, allocatedSymbols, true);
  }
  return TBSizes;
}

int cTBTables::cqiToMCSIndex(int cqiIndex, int rbCount, int nSymbols, int nREs, int nLayers, bool isDownlink, bool isOLLAenabled, double rateOffset)
{
    double maxCQIRate;
      // Compute the rate reported by UE through cqiIndex (rateOffset is added when OLLA is enabled)
    if(isOLLAenabled)
      maxCQIRate=cqiToMaxMCSRate.getMaximumMCSRate(cqiIndex) + rateOffset;
    else
      maxCQIRate=cqiToMaxMCSRate.getMaximumMCSRate(cqiIndex);
    
    int modulationIndex = cqiToMaxMCSRate.getCQIModulationAlphabet(cqiIndex);
    double tCodeRate = cqiToMaxMCSRate.getCQICodeRate(cqiIndex);
    
    ivec mcsIndicesWithMatchedQ;
    if(isOLLAenabled)
    {
      if(isDownlink)
	mcsIndicesWithMatchedQ = transportBlockMCSMapping.mcsIndex;
      else
	mcsIndicesWithMatchedQ = transportBlockMCSMapping.nrMcsIndicesPUSCH;   
    }
    else
    {
      // Get tbIndices for corresponding modulationIndex
      if(isDownlink)
	mcsIndicesWithMatchedQ = transportBlockMCSMapping.mcsIndex(find(transportBlockMCSMapping.modulationAlphabetPDSCH,modulationIndex,true));
      else
	mcsIndicesWithMatchedQ = transportBlockMCSMapping.nrMcsIndicesPUSCH(find(transportBlockMCSMapping.modulationAlphabetPUSCH,modulationIndex,true));
    
    }
    ivec tbSizesToConsider = getTBSizeForAllLayersCombined(mcsIndicesWithMatchedQ,nREs,rbCount,nLayers,nSymbols,cqiIndex);
    vec ratesForCondideredTBSizes(tbSizesToConsider.length());
    for(int cnt=0;cnt<tbSizesToConsider.length();cnt++)
    {
      int nCRC = getNumberOfCRCbits(tbSizesToConsider(cnt),tCodeRate);
      ratesForCondideredTBSizes(cnt)= (tbSizesToConsider(cnt)+ nCRC )/(double)(nREs*nLayers);
    }
      // Pick the tbIndex having maximum rate which is <= maxCQIRate ( \note Assumption: Rate always increase with TBIndex for a given mdulation-scheme)
    vec temp=ratesForCondideredTBSizes(find(ratesForCondideredTBSizes<=maxCQIRate));
    int bestMcsIndex;
    if(temp.length()!=0)
      bestMcsIndex = mcsIndicesWithMatchedQ(max_index(temp));
    else
      bestMcsIndex = mcsIndicesWithMatchedQ(0);
    
    if(!isOLLAenabled)
    {
	if(isDownlink)
	{
	  if(transportBlockMCSMapping.modulationAlphabetPDSCH(bestMcsIndex) == modulationIndex)
	    return bestMcsIndex;
	  else
	  {
	    cout << "Modulation of MCS index and modulation of CQI index not matching....aborting"<<endl;
	    abort();
	  }
	}
	else
	{
	  if(transportBlockMCSMapping.modulationAlphabetPUSCH(bestMcsIndex) == modulationIndex)
	    return bestMcsIndex;
	  else
	  {
	    cout << "Modulation of MCS index and modulation of CQI index not matching....aborting"<<endl;
	    abort();
	  }
	}
    }
    else
      return bestMcsIndex;
  
    cout<<"[both:] Error: Not able to find out MCS..."<<endl;abort();// This error won't come normaly. But kept as a precaution..
}
ivec cTBTables::findBestMCSIndex(ivec cqiIndices, int numberOfRBs, int allocatedSymbols, int nREs, int mimoRate, bool isDownlink, bool isOLLAenabled, vec rateOffset)
{
  ivec mcsIndices;
  mcsIndices.set_length(cqiIndices.length());
  for(int cwd_cnt=0;cwd_cnt<cqiIndices.length();cwd_cnt++)
    mcsIndices(cwd_cnt)=cqiToMCSIndex(cqiIndices(cwd_cnt),numberOfRBs,allocatedSymbols,nREs,getNumLayersPerCodeWord(cqiIndices.length(),mimoRate)(cwd_cnt),isDownlink,isOLLAenabled,rateOffset(cwd_cnt));
  
  return(mcsIndices);
}


int cTBTables::findOptimalCQIIndex(ivec cqiIndices)
{
  double AvgRate=0.0;int cqiIndex=0;
  for(int cnt=0;cnt<cqiIndices.length();cnt++)
    AvgRate=AvgRate+cqiToMaxMCSRate.getMaximumMCSRate(cqiIndices(cnt));
  AvgRate=AvgRate/cqiIndices.length();
  for(int cnt=0;cnt<cqiToMaxMCSRate.cqiIndices.length();cnt++)
    if(cqiToMaxMCSRate.getMaximumMCSRate(cqiToMaxMCSRate.cqiIndices(cnt))<=AvgRate )
    {
      cqiIndex=cqiToMaxMCSRate.cqiIndices(cnt);
    }
    else
      break;
    return(cqiIndex);
}
// ivec getNumLayersPerCodeWord(int nCodeWords, int mimoRate)
// {
//    
//   if(mimoRate < nCodeWords)
//   {
//     cout<<"nCodeWords :"<<nCodeWords<<" mimoRate :"<<mimoRate<<endl;
//     cout<<"Unsupported nCodeWords and mimoRate"<<endl;
//     abort();
//   }
//     
//   ivec numLayersPerCodeWord;
//   numLayersPerCodeWord.set_size(nCodeWords);
//   
//   switch(nCodeWords) {
//     case 1:
//       numLayersPerCodeWord(0) = mimoRate;
//       break;
//     case 2:
//       numLayersPerCodeWord(0) = floor_i(mimoRate/2.0);
//       numLayersPerCodeWord(1) = ceil_i(mimoRate/2.0);
//       break;
//     default:
//       cout<<"[both:]Error: Number of LayersPerCodeWord is not defined for nCodeWords ="<<nCodeWords<<", not in the set {1,2}...\n";
//       abort();
//   }
//   
//   return(numLayersPerCodeWord);
// }

int findTempTBSize(double N_info_prime,double mCodeRate)
{
  double C=0;
  if(mCodeRate<=0.25)
  {
    C = ceil((N_info_prime+24.0)/3816);
  }
  else
  {
    if(N_info_prime>8424)
      C = ceil((N_info_prime+24.0)/8424);
    else
      C=1;
  }
  return (8*C*ceil((N_info_prime+24.0)/(8*C))-24);
}

int cTBTables::getTBSizeForAllLayersCombined(int mcsIndex, int nREsAllocated, int allocatedPRBs, int numLayers, int allocatedSymbols, bool isDownlink)
{
//   cout<<"nREsAllocated : "<<nREsAllocated<<endl;
  //38.214 v 15.1.0 section 5.1.3.2 
  int N_RE=0, N_RE_Prime_dash=0;
  double  N_info =0;
  double mCodeRate;
  int TBSize=0;
  int modulationAlphabet;
  double N_info_prime=0,n=0;
  if(isDownlink)
  {
    modulationAlphabet = transportBlockMCSMapping.modulationAlphabetPDSCH(mcsIndex);
    mCodeRate= transportBlockMCSMapping.nrMCSCodeRatePDSCH(mcsIndex);
  }
  else
  {
    modulationAlphabet = transportBlockMCSMapping.modulationAlphabetPUSCH(mcsIndex);
    mCodeRate= transportBlockMCSMapping.nrMCSCodeRatePUSCH(mcsIndex);
  }
  if(nREsAllocated<=0) 
  {
    cout << "nRE not set ... aborting" <<endl;abort();
  }
//   N_RE_Prime_dash = quantizeN_RE_Prime(N_RE_Prime);
  N_RE_Prime_dash = 156;
  N_RE = min(nREsAllocated,N_RE_Prime_dash*allocatedPRBs);
  
  N_info = N_RE*mCodeRate*modulationAlphabet*numLayers; //(N_RE*R*Qm*v)
  if(N_info<=3824)
  {
    vector<double> TBS_Table = {24, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128, 136, 144, 152, 160, 168, 176, 184, 192, 208, 224, 240, 256, 272, 288, 304, 320, 336, 352, 368, 384, 408, 432, 456, 480, 504, 528, 552, 576, 608, 640, 672, 704, 736, 768, 808, 848, 888, 928, 984, 1032, 1064, 1128, 1160, 1192, 1224, 1256, 1288, 1320, 1352, 1416, 1480, 1544, 1608, 1672, 1736, 1800, 1864, 1928, 2024, 2088, 2152, 2216, 2280, 2408, 2472, 2536, 2600, 2664, 2728, 2792, 2856, 2976, 3104, 3240, 3368, 3496, 3624, 3752, 3824}; 
    //TBStable = vec("24 24 32 40 48 56 64 72 80 88 96 104 112 120 128 136 144 152 160 168 176 184 192 208 224 240 256 272 288 304 320 336 352 368 384 408 432 456 480 504 528 552 576 608 640 672 704 736 768 808 848 888 928 984 1032 1064 1128 1160 1192 1224 1256 1288 1320 1352 1416 1480 1544 1608 1672 1736 1800 1864 1928 2024 2088 2152 2216 2280 2408 2472 2536 2600 2664 2728 2792 2856 2976 3104 3240 3368 3496 3624 3752 3824");
    double tmp=0;
    n = max(3.0,floor(log2(N_info))-6.0);
    tmp = pow(2,n)*floor(N_info/pow(2,n));
    N_info_prime = max(24.0,tmp);
    TBSize = closest(TBS_Table,N_info_prime); //selects A TBS from Table 5.1.3.2-2 which is closest or equal to N_info_prime but is not less than N_info_prime
  }
  else
  {
    
    n = floor(log2(N_info-24.0))-5.0; 
    N_info_prime = max(3840.0,pow(2,n)*round_i((N_info-24.0)/pow(2,n)));  // round functions in the formula takes next largest integer. Son made the division in int and added 1 to make sure it always takes the next largest value.
    
    TBSize = findTempTBSize(N_info_prime,mCodeRate);
    int Evec = nREsAllocated* numLayers*modulationAlphabet;
    
    double codeRate = (double)TBSize/(double)Evec;
    while(codeRate>0.92578125)
    {
      N_info_prime--;  // round functions in the formula takes next largest integer. Son made the division in int and added 1 to make sure it always takes the next largest value.
      TBSize = findTempTBSize(N_info_prime,mCodeRate);
      codeRate = (double)TBSize/(double)Evec;
    }
  }

  int Evec = nREsAllocated* numLayers*modulationAlphabet;
  //cout<< "nREsAllocated 1 : " << nREsAllocated << "\t Evec : " << Evec <<endl;
  double codeRate = (double)TBSize/(double)Evec;
  if(codeRate>0.92578125)
  {
    cout << "CodeRate: "<< codeRate<<"Abnormal codeRate!! .. aborting !!"<<endl;abort();
  }
  return TBSize;
}

// int cTBTables::getTBSizeForAllLayersCombined(int mcsIndex,int nREsAllocatedinTotal, int allocatedPRBs, int NsymPerRB, int NdmrsPerRB, int NohPerRB, int allocatedPRBs, int numLayers, int allocatedSymbols, bool isDownlink)
// {
//   //   cout<<"nREsAllocated : "<<nREsAllocated<<endl;
//   //38.214 v 15.1.0 section 5.1.3.2 
//   int N_RE=0, N_RE_Prime=0, SCperRB=12;
//   double  N_info =0;
//   double mCodeRate;
//   int TBSize=0;
//   int modulationAlphabet;
//   double N_info_prime=0,n=0;
//   N_RE_Prime = SCperRB*NsymPerRB-NdmrsPerRB-NohPerRB;
//   N_RE = min(156,N_RE_Prime)*allocatedPRBs;
// 
//   if(isDownlink)
//   {
//     modulationAlphabet = transportBlockMCSMapping.modulationAlphabetPDSCH(mcsIndex);
//     mCodeRate= transportBlockMCSMapping.nrMCSCodeRatePDSCH(mcsIndex);
//   }
//   else
//   {
//     modulationAlphabet = transportBlockMCSMapping.modulationAlphabetPUSCH(mcsIndex);
//     mCodeRate= transportBlockMCSMapping.nrMCSCodeRatePUSCH(mcsIndex);
//   }
//   if(N_RE<=0) 
//   {
//     cout << "nRE not set ... aborting" <<endl;abort();
//   }
//   N_info = N_RE*mCodeRate*modulationAlphabet*numLayers; //(N_RE*R*Qm*v)
//   int Evec = nREsAllocatedinTotal* numLayers*modulationAlphabet;
//   if(N_info<=3824)
//   {
//     vector<double> TBS_Table = {24, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128, 136, 144, 152, 160, 168, 176, 184, 192, 208, 224, 240, 256, 272, 288, 304, 320, 336, 352, 368, 384, 408, 432, 456, 480, 504, 528, 552, 576, 608, 640, 672, 704, 736, 768, 808, 848, 888, 928, 984, 1032, 1064, 1128, 1160, 1192, 1224, 1256, 1288, 1320, 1352, 1416, 1480, 1544, 1608, 1672, 1736, 1800, 1864, 1928, 2024, 2088, 2152, 2216, 2280, 2408, 2472, 2536, 2600, 2664, 2728, 2792, 2856, 2976, 3104, 3240, 3368, 3496, 3624, 3752, 3824}; 
//     //TBStable = vec("24 24 32 40 48 56 64 72 80 88 96 104 112 120 128 136 144 152 160 168 176 184 192 208 224 240 256 272 288 304 320 336 352 368 384 408 432 456 480 504 528 552 576 608 640 672 704 736 768 808 848 888 928 984 1032 1064 1128 1160 1192 1224 1256 1288 1320 1352 1416 1480 1544 1608 1672 1736 1800 1864 1928 2024 2088 2152 2216 2280 2408 2472 2536 2600 2664 2728 2792 2856 2976 3104 3240 3368 3496 3624 3752 3824");
//     double tmp=0;
//     n = max(3.0,floor(log2(N_info))-6.0);
//     tmp = pow(2,n)*floor(N_info/pow(2,n));
//     N_info_prime = max(24.0,tmp);
//     TBSize = closest(TBS_Table,N_info_prime); //selects A TBS from Table 5.1.3.2-2 which is closest or equal to N_info_prime but is not less than N_info_prime
//   }
//   else
//   {
//     
//     n = floor(log2(N_info-24.0))-5.0; 
//     N_info_prime = max(3840.0,pow(2,n)*round_i((N_info-24.0)/pow(2,n)));  // round functions in the formula takes next largest integer. Son made the division in int and added 1 to make sure it always takes the next largest value.
//     
//     TBSize = findTempTBSize(N_info_prime,mCodeRate);
//     double codeRate = (double)TBSize/(double)Evec;
//     while(codeRate>0.92578125)
//     {
//       N_info_prime--;  // round functions in the formula takes next largest integer. Son made the division in int and added 1 to make sure it always takes the next largest value.
//       TBSize = findTempTBSize(N_info_prime,mCodeRate);
//       codeRate = (double)TBSize/(double)Evec;
//     }
//   }
//   
//   double codeRate = (double)TBSize/(double)Evec;
//   if(codeRate>0.92578125)
//   {
//     cout << "CodeRate: "<< codeRate<<"Abnormal codeRate!! .. aborting !!"<<endl;abort();
//   }
//   return TBSize;
// }


LinkAdaptation_S cTBTables::getMCSPerCodeWord(Array< vec > SINRIndBPerCodeWord, int nRBs, int nSymbols, int nREs, int nLayers, bool isDownlink, vec sinrOffsetIndB, vec rateThresholds, vec targetBLER)
{
  
  LinkAdaptation_S tLinkAdaptation;
  
  int nCodeWords = SINRIndBPerCodeWord.length();
  
  Array<ivec> tbSizesPerMCS(nCodeWords);
  Array<vec> codeRatesPerMCS(nCodeWords);
  Array<vec> specEffPerMCS(nCodeWords);
  
  int nMCS;
  if(isDownlink)
    nMCS= transportBlockMCSMapping.modulationAlphabetPDSCH.length();
  else
    nMCS= transportBlockMCSMapping.modulationAlphabetPUSCH.length();
  
  for(int Cwd_cnt=0;Cwd_cnt<nCodeWords;Cwd_cnt++)
  {
    tbSizesPerMCS(Cwd_cnt).set_length(nMCS);
    codeRatesPerMCS(Cwd_cnt).set_length(nMCS);
    specEffPerMCS(Cwd_cnt).set_length(nMCS);
  }
  
  for(int mcs_cnt = 0; mcs_cnt < nMCS; mcs_cnt++)
  {
    int mcsIndex;
    if(isDownlink)
      mcsIndex = transportBlockMCSMapping.mcsIndex(mcs_cnt);
    else
      mcsIndex = transportBlockMCSMapping.nrMcsIndicesPUSCH(mcs_cnt);
    for(int Cwd_cnt=0;Cwd_cnt < nCodeWords;Cwd_cnt++)
    {
      int modulationAlphabet;
      double tCodeRate=0;
      if(isDownlink)
      {
        modulationAlphabet = transportBlockMCSMapping.modulationAlphabetPDSCH (mcs_cnt);
        tCodeRate = transportBlockMCSMapping.nrMCSCodeRatePDSCH(mcs_cnt);
      }
      else
      {
        modulationAlphabet = transportBlockMCSMapping.modulationAlphabetPUSCH(mcs_cnt);
        tCodeRate = transportBlockMCSMapping.nrMCSCodeRatePUSCH(mcs_cnt);
      }
      
      tbSizesPerMCS(Cwd_cnt)(mcs_cnt) = getTBSizeForAllLayersCombined(mcsIndex,nREs,nRBs,getNumLayersPerCodeWord(nCodeWords,nLayers)(Cwd_cnt),nSymbols,isDownlink);
      int nCRC = getNumberOfCRCbits(tbSizesPerMCS(Cwd_cnt)(mcs_cnt),tCodeRate);
      codeRatesPerMCS(Cwd_cnt)(mcs_cnt) = (tbSizesPerMCS(Cwd_cnt)(mcs_cnt)+nCRC)/(double)(nREs*getNumLayersPerCodeWord(nCodeWords,nLayers)(Cwd_cnt) * modulationAlphabet);
      specEffPerMCS(Cwd_cnt)(mcs_cnt) = tbSizesPerMCS(Cwd_cnt)(mcs_cnt)/(double)nREs; // spec eff formula changed to bits/sym - Dhiv
    }
  }
  
  tLinkAdaptation.mcsIndxPerCodeword = zeros_i(nCodeWords);
  tLinkAdaptation.tbSizePerCodewordInBytes = zeros_i(nCodeWords);
  tLinkAdaptation.mcsRate = 0.0;
  
  if(sinrOffsetIndB.length()==0) sinrOffsetIndB=zeros(nCodeWords);
  
  vec targetBLERPerMCS(tbSizesPerMCS(0).length());
  targetBLERPerMCS=targetBLER(targetBLER.length()-1);
  
  for(int cnt=(rateThresholds.length()-1);cnt>=0;cnt--)
  {
    ivec indices=find(specEffPerMCS(0)<=rateThresholds(cnt));
    setInVec(targetBLERPerMCS,indices,targetBLER(cnt));
  }
  
  for(int Cwd_cnt=0;Cwd_cnt<nCodeWords;Cwd_cnt++)
  {
    vec SINRlinear = inv_dB(SINRIndBPerCodeWord(Cwd_cnt));
    double rate=0.0;
    tLinkAdaptation.mcsIndxPerCodeword(Cwd_cnt) = awgnTablesConvCoder.mcsSelect(rate,SINRlinear,tbSizesPerMCS(Cwd_cnt)+24,codeRatesPerMCS(Cwd_cnt),specEffPerMCS(Cwd_cnt),isDownlink,sinrOffsetIndB(Cwd_cnt),targetBLERPerMCS);
    tLinkAdaptation.tbSizePerCodewordInBytes(Cwd_cnt) = tbSizesPerMCS(Cwd_cnt)(tLinkAdaptation.mcsIndxPerCodeword(Cwd_cnt))/8;
    tLinkAdaptation.mcsRate += rate;
  }
  
  return tLinkAdaptation;
  
}

int cLTETables::cqiToMCSIndex(int cqiIndex, int rbCount, int nSymbols, int nREs, int nLayers, bool isDownlink, bool isOLLAenabled, double rateOffset){
  
  double maxCQIRate;
  // Compute the rate reported by UE through cqiIndex (rateOffset is added when OLLA is enabled)
  if(isOLLAenabled)
    maxCQIRate=cqiToMaxMCSRate.getMaximumMCSRate(cqiIndex) + rateOffset;
  else
    maxCQIRate=cqiToMaxMCSRate.getMaximumMCSRate(cqiIndex);
  
  // Get modulatioIndex for reported CQI
  int modulationIndex = cqiToMaxMCSRate.modulationIndex(cqiIndex-1);
    
  ivec tbIndices;
    
  if(isOLLAenabled)
  {
    if(isDownlink)
      tbIndices = transportBlockMCSMapping.transportBlockIndexPDSCH;
    else
      tbIndices = transportBlockMCSMapping.transportBlockIndexPUSCH;
  }
  else
  {
    // Get tbIndices for corresponding modulationIndex
    if(isDownlink)
      tbIndices = transportBlockMCSMapping.transportBlockIndexPDSCH(find(transportBlockMCSMapping.modulationAlphabetPDSCH==modulationIndex));
    else
      tbIndices = transportBlockMCSMapping.transportBlockIndexPUSCH(find(transportBlockMCSMapping.modulationAlphabetPUSCH==modulationIndex));
  }
  // Get TB-Size for each TBIndex.
  ivec tbSizeForRbCountPerTBIndex = getTBSizeForAllLayersCombined(tbIndices,nREs,rbCount,nLayers,nSymbols,cqiIndex);
  
  // Get rate(infoBits+CRC over all layers/(nLayers*REs)) for each TBIndex. crcPolynomials(3) will give polynomial of CRC_24B
  vec ratePerTBIndex(tbSizeForRbCountPerTBIndex.length());
  for(int cnt=0;cnt<tbSizeForRbCountPerTBIndex.length();cnt++)
  {
    int nCRC = getNumberOfCRCbits(tbSizeForRbCountPerTBIndex(cnt),0.0);
    ratePerTBIndex(cnt)= (tbSizeForRbCountPerTBIndex(cnt)+ nCRC )/(double)(nREs*nLayers);
  }
  // Pick the tbIndex having maximum rate which is <= maxCQIRate ( \note Assumption: Rate always increase with TBIndex for a given mdulation-scheme)
  vec temp=ratePerTBIndex(find(ratePerTBIndex<=maxCQIRate));
  int maxTBIndex;
  if(temp.length()!=0)
    maxTBIndex= tbIndices(max_index(temp));
  else
    maxTBIndex= tbIndices(0);
     
  // Get the corresponding MCS-Index(Actually Index of MCS-Index) for maxTBIndex
  ivec mcsIndices;
  if(isDownlink)
    mcsIndices = find(transportBlockMCSMapping.transportBlockIndexPDSCH==maxTBIndex);
  else
    mcsIndices = find(transportBlockMCSMapping.transportBlockIndexPUSCH==maxTBIndex);
  
  if(isOLLAenabled)
    return mcsIndices(0);  
  
  // Pick the only MCS which is having the same modulation-scheme as specified by CQI
  for(int ind=0; ind<mcsIndices.length(); ind++)
  {
    if(isDownlink)
    {
      if(transportBlockMCSMapping.modulationAlphabetPDSCH(mcsIndices(ind)) == modulationIndex)
	return transportBlockMCSMapping.mcsIndex(mcsIndices(ind));
    }
    else
    {
      if(transportBlockMCSMapping.modulationAlphabetPUSCH(mcsIndices(ind)) == modulationIndex)
	return transportBlockMCSMapping.mcsIndex(mcsIndices(ind));
    }
  }
      
  // This error won't come normaly. But kept as a precaution..
  cout<<"[both:] Error: Not able to find out MCS..."<<endl;abort();
}


ivec cLTETables::getCodeBlockSizes(int tbSize, double tCodeRate){

  return getCodeBlockSizesForTB(tbSize,interleaverParameters.K);
  
}

int cLTETables::getNumberOfCodeBlocks(int TBSize, double tCodeRate){
  
  return getCodeBlockSizes(TBSize,tCodeRate).length();
}

int cLTETables::getNumberOfCRCbits(int tbSize, double tCodeRate){
  
  int L = 24, nBlocks = getCodeBlockSizes(tbSize,tCodeRate).length();
  int nCRC = (nBlocks>1) ? (nBlocks+1)*L : L;
  return nCRC;
}

int cLTETables::findOptimalCQIIndex(ivec cqiIndices){
  
  double AvgRate=0.0;int cqiIndex=0;
  for(int cnt=0;cnt<cqiIndices.length();cnt++)
    AvgRate=AvgRate+cqiToMaxMCSRate.getMaximumMCSRate(cqiIndices(cnt));
  AvgRate=AvgRate/cqiIndices.length();
  for(int cnt=0;cnt<cqiToMaxMCSRate.cqiIndices.length();cnt++)
    if(cqiToMaxMCSRate.getMaximumMCSRate(cqiToMaxMCSRate.cqiIndices(cnt))<=AvgRate )
    {
      cqiIndex=cqiToMaxMCSRate.cqiIndices(cnt);
    }
    else
      break;
    
    return(cqiIndex);
}

ivec cLTETables::findBestMCSIndex(ivec cqiIndices, int numberOfRBs, int allocatedSymbols,int nREs,int mimoRate, bool isDownlink, bool isOLLAenabled, vec rateOffset){
  
  ivec mcsIndices;
  mcsIndices.set_length(cqiIndices.length());
  for(int cwd_cnt=0;cwd_cnt<cqiIndices.length();cwd_cnt++)
    mcsIndices(cwd_cnt)=cqiToMCSIndex(cqiIndices(cwd_cnt),numberOfRBs,allocatedSymbols,nREs,getNumLayersPerCodeWord(cqiIndices.length(),mimoRate)(cwd_cnt),isDownlink,isOLLAenabled,rateOffset(cwd_cnt));
  
  return(mcsIndices);
}

/*! \brief Given a tbIndices, allocatedPRBs and numLayers, compute the TBSize for each codeWord using the look-up table
 *        & put it in  tbConfig->transportBlockSize[iCodeWord] for each layer
 * \ref 36.213-v10.1.0 {Table 7.1.7.2.1.1}         */
ivec cLTETables::getTBSizeForAllLayersCombined(const ivec& tbIndices,int nREsAllocated, int allocatedPRBs, int numLayers, int allocatedSymbols, int cqiIndex)
{
  //! Single-Layer to Multiple-Layer Translation Table is not needed if nPRBs is less...
  if((numLayers==1) || (numLayers==2 && allocatedPRBs<=55) || (numLayers==3 && allocatedPRBs<=36) || (numLayers==4 && allocatedPRBs<=27))
    return(numLayers*transportBlockMCSMapping.transportBlockSizeInBitsFor1Layer1Codeword.get_col(allocatedPRBs-1)(tbIndices));
  
  //! Else calculate TB-Size for each tbIndex....
    ivec toReturn(tbIndices.length());
    for(int itbIndex=0; itbIndex<tbIndices.length(); itbIndex++)
      toReturn(itbIndex) = getTBSizeForAllLayersCombined(tbIndices(itbIndex),nREsAllocated,allocatedPRBs,numLayers,allocatedSymbols);
    
    return toReturn;
}

int cLTETables::getTBSizeForAllLayersCombined(int tbIndex,int nREsAllocated,int allocatedPRBs,int numLayers, int allocatedSymbols, bool isDownlink)
{
  
  allocatedPRBs=floor_i((double)allocatedPRBs*(double)allocatedSymbols/14.0);
  if(!allocatedPRBs)
  {
    allocatedPRBs =1;
  }
  
  int layerOneSize = transportBlockMCSMapping.transportBlockSizeInBitsFor1Layer1Codeword(tbIndex,allocatedPRBs-1);
  
  //! Single-Layer to Multiple-Layer Translation Table is not needed if nPRBs is less...
  if((numLayers==1) || (numLayers==2 && allocatedPRBs<=55) || (numLayers==3 && allocatedPRBs<=36) || (numLayers==4 && allocatedPRBs<=27))
    return transportBlockMCSMapping.transportBlockSizeInBitsFor1Layer1Codeword(tbIndex,numLayers*allocatedPRBs-1);
  
  int runIndex, TBS1,TBS2;
  
  switch(numLayers) {
    case 2:
      runIndex = find(layerMappingTBSTranslation.L1toL2TBSL1,layerOneSize);
      if(runIndex!=-1)  return (layerMappingTBSTranslation.L1toL2TBSL2(runIndex));
      break;
    case 3:
      runIndex = find(layerMappingTBSTranslation.L1toL3TBSL1,layerOneSize);
      if(runIndex!=-1)  return (layerMappingTBSTranslation.L1toL3TBSL3(runIndex));
      break;
    case 4:
      runIndex = find(layerMappingTBSTranslation.L1toL4TBSL1,layerOneSize);
      if(runIndex!=-1)  return (layerMappingTBSTranslation.L1toL4TBSL4(runIndex));
      break;
    case 5:
      if(allocatedPRBs<=55)
	TBS1=transportBlockMCSMapping.transportBlockSizeInBitsFor1Layer1Codeword(tbIndex,floor_i(numLayers/2)*allocatedPRBs-1);
      else
      {
	runIndex = find(layerMappingTBSTranslation.L1toL2TBSL1,layerOneSize);
	TBS1=layerMappingTBSTranslation.L1toL2TBSL2(runIndex);
      }
      if(allocatedPRBs<=36)
	TBS2=transportBlockMCSMapping.transportBlockSizeInBitsFor1Layer1Codeword(tbIndex,ceil_i(numLayers/2)*allocatedPRBs-1);
      else
      {
	runIndex = find(layerMappingTBSTranslation.L1toL2TBSL1,layerOneSize);
	TBS2=layerMappingTBSTranslation.L1toL3TBSL3(runIndex);
      }
      return (TBS1+TBS2);
      break;
    case 6:
      if(allocatedPRBs<=36)
	TBS1=transportBlockMCSMapping.transportBlockSizeInBitsFor1Layer1Codeword(tbIndex,floor_i(numLayers/2)*allocatedPRBs-1);
      else
      {
	runIndex = find(layerMappingTBSTranslation.L1toL3TBSL1,layerOneSize);
	TBS1=layerMappingTBSTranslation.L1toL3TBSL3(runIndex);
      }
      if(allocatedPRBs<=36)
	TBS2=transportBlockMCSMapping.transportBlockSizeInBitsFor1Layer1Codeword(tbIndex,ceil_i(numLayers/2)*allocatedPRBs-1);
      else
      {
	runIndex = find(layerMappingTBSTranslation.L1toL3TBSL1,layerOneSize);
	TBS2=layerMappingTBSTranslation.L1toL3TBSL3(runIndex);
      }
      return (TBS1+TBS2);
      break;
    case 7:
      if(allocatedPRBs<=36)
	TBS1=transportBlockMCSMapping.transportBlockSizeInBitsFor1Layer1Codeword(tbIndex,floor_i(numLayers/2)*allocatedPRBs-1);
      else
      {
	runIndex = find(layerMappingTBSTranslation.L1toL3TBSL1,layerOneSize);
	TBS1=layerMappingTBSTranslation.L1toL3TBSL3(runIndex);
      }
      if(allocatedPRBs<=27)
	TBS2=transportBlockMCSMapping.transportBlockSizeInBitsFor1Layer1Codeword(tbIndex,ceil_i(numLayers/2)*allocatedPRBs-1);
      else
      {
	runIndex = find(layerMappingTBSTranslation.L1toL3TBSL1,layerOneSize);
	TBS2=layerMappingTBSTranslation.L1toL4TBSL4(runIndex);
      }
      return (TBS1+TBS2);
      break;
    case 8:
      if(allocatedPRBs<=27)
	TBS1=transportBlockMCSMapping.transportBlockSizeInBitsFor1Layer1Codeword(tbIndex,floor_i(numLayers/2)*allocatedPRBs-1);
      else
      {
	runIndex = find(layerMappingTBSTranslation.L1toL4TBSL1,layerOneSize);
	TBS1=layerMappingTBSTranslation.L1toL4TBSL4(runIndex);
      }
      if(allocatedPRBs<=27)
	TBS2=transportBlockMCSMapping.transportBlockSizeInBitsFor1Layer1Codeword(tbIndex,ceil_i(numLayers/2)*allocatedPRBs-1);
      else
      {
	runIndex = find(layerMappingTBSTranslation.L1toL4TBSL1,layerOneSize);
	TBS2=layerMappingTBSTranslation.L1toL4TBSL4(runIndex);
      }
      return (TBS1+TBS2);
      break;
    default:
    {cout<<"[both:]Error: numLayers ="<<numLayers<<" is not in the set {1,2,3,4}...\n"; abort();}
  }
  
  
  cout << "[both:]Error: Unable to find the Layer - "<<numLayers<<" Size for Layer One Size ="<<layerOneSize<<"....\n";
  abort();
  
}

LinkAdaptation_S cLTETables::getMCSPerCodeWord(Array< vec > SINRIndBPerCodeWord, int nRBs, int nSymbols, int nREs, int nLayers, bool isDownlink, vec sinrOffsetIndB, vec rateThresholds, vec targetBLER)
{
  
  LinkAdaptation_S tLinkAdaptation;
  
  int nCodeWords = SINRIndBPerCodeWord.length();
  
  Array<ivec> tbSizesPerMCS(nCodeWords);
  Array<vec> codeRatesPerMCS(nCodeWords);
  Array<vec> specEffPerMCS(nCodeWords);
  
  int nMCS;
  if(isDownlink)
    nMCS= transportBlockMCSMapping.modulationAlphabetPDSCH.length();
  else
    nMCS= transportBlockMCSMapping.modulationAlphabetPUSCH.length();
  
  for(int Cwd_cnt=0;Cwd_cnt<nCodeWords;Cwd_cnt++)
  {
    tbSizesPerMCS(Cwd_cnt).set_length(nMCS);
    codeRatesPerMCS(Cwd_cnt).set_length(nMCS);
    specEffPerMCS(Cwd_cnt).set_length(nMCS);
  }
  
  for(int mcs_cnt = 0; mcs_cnt < nMCS; mcs_cnt++)
  {
    int tbIndex;
    if(isDownlink)
      tbIndex = transportBlockMCSMapping.transportBlockIndexPDSCH(mcs_cnt);
    else
      tbIndex = transportBlockMCSMapping.transportBlockIndexPUSCH(mcs_cnt);
    for(int Cwd_cnt=0;Cwd_cnt < nCodeWords;Cwd_cnt++)
    {
      int modulationAlphabet;
      if(isDownlink)
	modulationAlphabet = transportBlockMCSMapping.modulationAlphabetPDSCH(mcs_cnt);
      else
	modulationAlphabet = transportBlockMCSMapping.modulationAlphabetPUSCH(mcs_cnt);
      tbSizesPerMCS(Cwd_cnt)(mcs_cnt) = getTBSizeForAllLayersCombined(tbIndex,nREs, nRBs, getNumLayersPerCodeWord(nCodeWords,nLayers)(Cwd_cnt),nSymbols);
      
      int nCRC = getNumberOfCRCbits(tbSizesPerMCS(Cwd_cnt)(mcs_cnt),0.0);
      codeRatesPerMCS(Cwd_cnt)(mcs_cnt) = (tbSizesPerMCS(Cwd_cnt)(mcs_cnt)+nCRC)/(double)(nREs*getNumLayersPerCodeWord(nCodeWords,nLayers)(Cwd_cnt) * modulationAlphabet);
      specEffPerMCS(Cwd_cnt)(mcs_cnt) = tbSizesPerMCS(Cwd_cnt)(mcs_cnt)/(double)nREs; // spec eff formula changed to bits/sym - Dhiv
    }
  }
  tLinkAdaptation.mcsIndxPerCodeword = zeros_i(nCodeWords);
  tLinkAdaptation.tbSizePerCodewordInBytes = zeros_i(nCodeWords);
  tLinkAdaptation.mcsRate = 0.0;
  
  if(sinrOffsetIndB.length()==0) sinrOffsetIndB=zeros(nCodeWords);
  
  vec targetBLERPerMCS(tbSizesPerMCS(0).length());
  targetBLERPerMCS=targetBLER(targetBLER.length()-1);
  for(int cnt=(rateThresholds.length()-1);cnt>=0;cnt--)
  {
      ivec indices=find(specEffPerMCS(0)<=rateThresholds(cnt));
      setInVec(targetBLERPerMCS,indices,targetBLER(cnt));
  }
  for(int Cwd_cnt=0;Cwd_cnt<nCodeWords;Cwd_cnt++)
  {
    vec SINRlinear = inv_dB(SINRIndBPerCodeWord(Cwd_cnt));
    double rate=0.0;
    tLinkAdaptation.mcsIndxPerCodeword(Cwd_cnt) = awgnTablesConvCoder.mcsSelect(rate,SINRlinear,tbSizesPerMCS(Cwd_cnt)+24,codeRatesPerMCS(Cwd_cnt),specEffPerMCS(Cwd_cnt),isDownlink,sinrOffsetIndB(Cwd_cnt),targetBLERPerMCS);
    tLinkAdaptation.tbSizePerCodewordInBytes(Cwd_cnt) = tbSizesPerMCS(Cwd_cnt)(tLinkAdaptation.mcsIndxPerCodeword(Cwd_cnt))/8;
    tLinkAdaptation.mcsRate += rate;
  }
  
  return tLinkAdaptation;
  
}

