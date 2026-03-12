#ifndef CLTETABLES_H
#define CLTETABLES_H

#include "StructsAndEnums.h"
#include "SupportingFunctions.h"
#include "AWGNTables.h"
// #include "cLTETables.h"

struct LinkAdaptation_S
{ 
  int UEid;
  ivec mcsIndxPerCodeword;
  ivec tbSizePerCodewordInBytes;
  double mcsRate;
  
  LinkAdaptation_S()
  { 
    UEid = -1;
    mcsIndxPerCodeword.set_length(0);
    tbSizePerCodewordInBytes.set_length(0);
    mcsRate = -1.0;
  }
  
  void printLinkAdaptation()
  {
    cout<<"======== Printing Link Adaptation per UE =========="<<endl;
    cout<<"UEid:                        "<<UEid<<endl;
    cout<<"mcsIndxPerCodeword:          "<<mcsIndxPerCodeword<<endl;
    cout<<"tbSizePerCodewordInBytes:    "<<tbSizePerCodewordInBytes<<endl;
    cout<<"mcsRate:                     "<<mcsRate<<endl;
  }
};


double computeAvgSINRindB(vec SINRPerCWindB, int rank);

struct nrTransportBlockMCSMappingInfo_S
{
  ivec mcsIndex;
  ivec modulationAlphabetPDSCH;
  ivec modulationAlphabetPUSCH;
  ivec transportBlockIndexPDSCH;
  ivec transportBlockIndexPUSCH;
  imat transportBlockSizeInBitsFor1Layer1Codeword;
  
  ivec nrMcsIndicesPUSCH;
  vec nrMCSCodeRatePDSCH;
  vec nrMCSCodeRatePUSCH;
  bool is256QAMEnabled, ispi2BPSKEnabled, isTransmissionModeNR;
  
  nrTransportBlockMCSMappingInfo_S()
  {
    mcsIndex.set_length(0);
    modulationAlphabetPDSCH.set_length(0);
    modulationAlphabetPUSCH.set_length(0);
    transportBlockIndexPDSCH.set_length(0);
    transportBlockIndexPUSCH.set_length(0);
    
    nrMcsIndicesPUSCH.set_length(0);
    nrMCSCodeRatePDSCH.set_length(0);
    nrMCSCodeRatePUSCH.set_length(0);
    is256QAMEnabled=false; ispi2BPSKEnabled=false; isTransmissionModeNR=false;
  }

    void load(string fileName, bool isNR, bool isDownlink)
    {
      isTransmissionModeNR=isNR;
      
      if(isTransmissionModeNR)
      {
	if(isDownlink)
	{
	  if(!parse(fileName,"is256QAMEnabled", is256QAMEnabled))
	  {
	    cout<<"[both:] Unable to load is256QAMEnabled from the given file in nrTransportBlockMCSMappingInfo_S..."<<endl;
	    abort();
	  }
	}
	else
	{
	  if(!parse(fileName,"ispi2BPSKEnabled", ispi2BPSKEnabled))
	  {
	    cout<<"[both:] Unable to load ispi2BPSKEnabled from the given file in nrTransportBlockMCSMappingInfo_S..."<<endl;
	    abort();
	  }
	}
	string mcsTableNum = (is256QAMEnabled) ? "nrMcsIndexTable2":"nrMcsIndexTable1";
	string modulationTableNum = (is256QAMEnabled) ? "nrModulationIndexPDSCHTable2":"nrModulationIndexPDSCHTable1";
	string codeRateTableNum = (is256QAMEnabled) ? "nrMCSCodeRateTable2" : "nrMCSCodeRateTable1";
    
	if(!parse(fileName,mcsTableNum, mcsIndex))
	{
	  cout<<"[both:] Unable to load mcsIndex from the given file in nrTransportBlockMCSMappingInfo_S..."<<endl;
	  abort();
	}
	if(!parse(fileName,modulationTableNum, modulationAlphabetPDSCH))
	{
	  cout<<"[both:] Unable to load modulationIndexPDSCH from the given file in nrTransportBlockMCSMappingInfo_S..."<<endl;
	  abort();
	}
	if(!parse(fileName,codeRateTableNum, nrMCSCodeRatePDSCH))
	{
	  cout<<"[both:] Unable to load modulationIndexPDSCH from the given file in nrTransportBlockMCSMappingInfo_S..."<<endl;
	  abort();
	}
	
	if(!parse(fileName,"nrMcsIndexPUSCH", nrMcsIndicesPUSCH))
	{
	  cout<<"[both:] Unable to load nrMcsIndicesPUSCH from the given file in nrTransportBlockMCSMappingInfo_S..."<<endl;
	  abort();
	}
	if(!parse(fileName,"nrMcsIndexPUSCH", transportBlockIndexPUSCH))
	{
	  cout<<"[both:] Unable to load transportBlockIndexPUSCH from the given file in TransportBlockMappingInfo_S..."<<endl;
	  abort();
	}
	if(!parse(fileName,mcsTableNum, transportBlockIndexPDSCH))
	{
	  cout<<"[both:] Unable to load transportBlockIndexPDSCH from the given file in TransportBlockMappingInfo_S..."<<endl;
	  abort();
	}
	if(!parse(fileName,"nrModulationIndexPUSCH", modulationAlphabetPUSCH))
	{
	  cout<<"[both:] Unable to load modulationIndexPUSCH from the given file in nrTransportBlockMCSMappingInfo_S..."<<endl;
	  abort();
	}
	if(!parse(fileName,"nrMCSCodeRatePUSCH", nrMCSCodeRatePUSCH))
	{
	  cout<<"[both:] Unable to load nrMCSCodeRatePUSCH from the given file in nrTransportBlockMCSMappingInfo_S..."<<endl;
	  abort();
	}
	if(ispi2BPSKEnabled)
	{
	  modulationAlphabetPUSCH(0)=1; modulationAlphabetPUSCH(1)=1;
	  nrMCSCodeRatePUSCH(0) = 2*nrMCSCodeRatePUSCH(0); nrMCSCodeRatePUSCH(1) = 2*nrMCSCodeRatePUSCH(1);
	}
      }
      else
      {
	if(!parse(fileName,"mcsIndex", mcsIndex))
	  {
	    cout<<"[both:] Unable to load mcsIndex from the given file in TransportBlockMappingInfo_S..."<<endl;
	    abort();
	  }
	  if(!parse(fileName,"modulationIndexPDSCH", modulationAlphabetPDSCH))
	  {
	    cout<<"[both:] Unable to load modulationIndexPDSCH from the given file in TransportBlockMappingInfo_S..."<<endl;
	    abort();
	  }
	  if(!parse(fileName,"modulationIndexPUSCH", modulationAlphabetPUSCH))
	  {
	    cout<<"[both:] Unable to load modulationIndexPUSCH from the given file in TransportBlockMappingInfo_S..."<<endl;
	    abort();
	  }
	  if(!parse(fileName,"transportBlockIndexPUSCH", transportBlockIndexPUSCH))
	  {
	    cout<<"[both:] Unable to load transportBlockIndexPUSCH from the given file in TransportBlockMappingInfo_S..."<<endl;
	    abort();
	  }
	  if(!parse(fileName,"transportBlockIndexPDSCH", transportBlockIndexPDSCH))
	  {
	    cout<<"[both:] Unable to load transportBlockIndexPDSCH from the given file in TransportBlockMappingInfo_S..."<<endl;
	    abort();
	  }
	  
      }
      // following are loaded always
    
      if(!parse(fileName,"transportBlockSizeInBitsFor1Layer1Codeword", transportBlockSizeInBitsFor1Layer1Codeword))
      {
	cout<<"[both:] Unable to load transportBlockSizeInBitsFor1Layer1Codeword from the given file in TransportBlockMappingInfo_S..."<<endl;
	abort();
      }
    }
};

struct nrCqiToMaxMCSRate_S
{
  ivec cqiIndices;
  ivec modulationIndex;
  vec codeRate;
  bool is256QAMEnabled, ispi2BPSKEnabled,isTransmissionModeNR;
  
  nrCqiToMaxMCSRate_S(){
    
    cqiIndices.set_length(0);
    modulationIndex.set_length(0);
    codeRate.set_length(0);
    is256QAMEnabled=false;
    isTransmissionModeNR=false;
    ispi2BPSKEnabled=false;
  }
    void load(string mcsFileName,string fileName, bool isNR,bool isDownlink)
    {
      isTransmissionModeNR=isNR;
      
      if(isTransmissionModeNR)
      {
        if(!isDownlink)
        {
            if(!parse(mcsFileName,"ispi2BPSKEnabled", ispi2BPSKEnabled))
            {
                cout<<"[both:] Unable to load ispi2BPSKEnabled from the given file in nrTransportBlockMCSMappingInfo_S..."<<endl;
                abort();
            }
        }
        else
        {  
            if(!parse(mcsFileName,"is256QAMEnabled", is256QAMEnabled))
            {
                cout<<"[both:] Unable to load is256QAMEnabled from the given file in nrTransportBlockMCSMappingInfo_S..."<<endl;
                abort();
            }
        }
    if(currentTaskid==0)
    {
        cout<<"is256QAMEnabled : "<<is256QAMEnabled<<endl;
        cout<<"ispi2BPSKEnabled : "<<ispi2BPSKEnabled<<endl;
    }
    string suffix;
    if(is256QAMEnabled)
        suffix="2";
    else if(ispi2BPSKEnabled)
        suffix="3";
    else
        suffix="1";
            
    
	string cqiTableNum = "nrCqiIndexTable";
	string cqiModulationTableNum = "nrModulationIndexTable"+suffix;
	string codeRateTableNum = "nrCodeRateTable"+suffix;
	if(!parse(fileName,cqiTableNum, cqiIndices))
	{
	  cout<<"[both:] Unable to load cqiIndices from the given file in nrCqiToMaxMCSRate_S..."<<endl;
	  abort();
	}
	if(!parse(fileName,cqiModulationTableNum, modulationIndex))
	{
	  cout<<"[both:] Unable to load modulationIndex from the given file in nrCqiToMaxMCSRate_S..."<<endl;
	  abort();
	}
	if(!parse(fileName,codeRateTableNum, codeRate))
	{
	  cout<<"[both:] Unable to load codeRate from the given file in nrCqiToMaxMCSRate_S..."<<endl;
	  abort();
	}
      }
      else
      {
	if(!parse(fileName,"cqiIndex", cqiIndices))
	{
	  cout<<"[both:] Unable to load cqiIndex from the given file in CqiToMCSRate_S..."<<endl;
	  abort();
	}
	if(!parse(fileName,"modulationIndex", modulationIndex))
	{
	  cout<<"[both:] Unable to load modulationIndex from the given file in CqiToMCSRate_S..."<<endl;
	  abort();
	}
	if(!parse(fileName,"codeRate", codeRate))
	{
	  cout<<"[both:] Unable to load codeRate from the given file in CqiToMCSRate_S..."<<endl;
	  abort();
	}
      }
    }
    
    double getCQICodeRate(int cqiIndex)
    {
      
      if(cqiIndices.length()==0)
      {
	cout<<"[both:] Please initialize nrCqiToMaxMCSRate_S before calling getMaximumMCSRate()..."<<endl;
	abort();
      }
      int indx=find(cqiIndices,cqiIndex);
      if(indx==-1)
      {
	cout<<"[both:] Error...Unknown cqiIndex to getMaximumMCSRate()..."<<endl;
	abort();
      }
      return(codeRate(indx));
    }
    int getCQIModulationAlphabet(int cqiIndex)
    {
      
      if(cqiIndices.length()==0)
      {
	cout<<"[both:] Please initialize nrCqiToMaxMCSRate_S before calling getMaximumMCSRate()..."<<endl;
	abort();
      }
      int indx=find(cqiIndices,cqiIndex);
      if(indx==-1)
      {
	cout<<"[both:] Error...Unknown cqiIndex to getMaximumMCSRate()..."<<endl;
	abort();
      }
      return(modulationIndex(indx));
    }
  
    double getMaximumMCSRate(int cqiIndex)
    {
      
      if(cqiIndices.length()==0)
      {
	cout<<"[both:] Please initialize nrCqiToMaxMCSRate_S before calling getMaximumMCSRate()..."<<endl;
	abort();
      }
      int indx=find(cqiIndices,cqiIndex);
      if(indx==-1)
      {
	cout<<"[both:] Error...Unknown cqiIndex to getMaximumMCSRate()..."<<endl;
	abort();
      }
      return(((double)modulationIndex(indx))*codeRate(indx));
    }
    
    vec getMaximumMCSRate(ivec cqiIndex)
    {
      if(cqiIndices.length()==0)
      {
	cout<<"[both:] Please initialize nrCqiToMaxMCSRate_S before calling getMaximumMCSRate()..."<<endl;
	abort();
      }
      ivec indx=find(cqiIndices,cqiIndex);
      
      vec mcsRates(indx.length());
      for(int cwd_cnt=0;cwd_cnt<indx.length();cwd_cnt++)
      {
	if(indx(cwd_cnt)>=0)
	  mcsRates(cwd_cnt)=(double)(modulationIndex(indx(cwd_cnt)))*codeRate(indx(cwd_cnt));
	else
	  mcsRates(cwd_cnt)=0; 
      }
      
      return mcsRates;
    }
    
    Array<vec> getMaximumMCSRate(Array<ivec> cqiIndexPerCodeWord)
    {
    
      Array<vec> mcsRates(cqiIndexPerCodeWord.length());
      if(cqiIndices.length()==0)
      {
	cout<<"[both:] Please initialize nrCqiToMaxMCSRate_S before calling getMaximumMCSRate()..."<<endl;
	abort();
      }
      ivec indx;
      for(int cnt=0;cnt<mcsRates.length();cnt++)
      {
	indx=find(cqiIndices,cqiIndexPerCodeWord(cnt));
	mcsRates(cnt).set_size(indx.length());
	for(int cwd_cnt=0;cwd_cnt<indx.length();cwd_cnt++)
	{
	  if(indx(cwd_cnt)>=0)
	    mcsRates(cnt)(cwd_cnt)=(double)(modulationIndex(indx(cwd_cnt)))*codeRate(indx(cwd_cnt));
	  else
	    mcsRates(cnt)(cwd_cnt)=0; 
	}
      }
      return(mcsRates);
    }
    
    double getLeastMCSRate()
    {
      if(cqiIndices.length()==0)
      {
	cout<<"[both:] Please initialize nrCqiToMaxMCSRate_S before calling getLeastMCSRate()..."<<endl;
	abort();
      }
      return(((double)modulationIndex(0))*codeRate(0));
    }
    
};





// struct TransportBlockMCSMappingInfo_S{
//   
//   ivec mcsIndex;
//   ivec modulationIndexPDSCH;
//   ivec modulationIndexPUSCH;
//   ivec transportBlockIndexPDSCH;
//   ivec transportBlockIndexPUSCH;
//   imat transportBlockSizeInBitsFor1Layer1Codeword;
//   
//   void load(string fileName){
//     
//     if(!parse(fileName,"mcsIndex", mcsIndex))
//     {
//       cout<<"[both:] Unable to load mcsIndex from the given file in TransportBlockMappingInfo_S..."<<endl;
//       abort();
//     }
//     if(!parse(fileName,"modulationIndexPDSCH", modulationIndexPDSCH))
//     {
//       cout<<"[both:] Unable to load modulationIndexPDSCH from the given file in TransportBlockMappingInfo_S..."<<endl;
//       abort();
//     }
//     if(!parse(fileName,"modulationIndexPUSCH", modulationIndexPUSCH))
//     {
//       cout<<"[both:] Unable to load modulationIndexPUSCH from the given file in TransportBlockMappingInfo_S..."<<endl;
//       abort();
//     }
//     if(!parse(fileName,"transportBlockIndexPDSCH", transportBlockIndexPDSCH))
//     {
//       cout<<"[both:] Unable to load transportBlockIndexPDSCH from the given file in TransportBlockMappingInfo_S..."<<endl;
//       abort();
//     }
//     if(!parse(fileName,"transportBlockIndexPUSCH", transportBlockIndexPUSCH))
//     {
//       cout<<"[both:] Unable to load transportBlockIndexPUSCH from the given file in TransportBlockMappingInfo_S..."<<endl;
//       abort();
//     }
//     if(!parse(fileName,"transportBlockSizeInBitsFor1Layer1Codeword", transportBlockSizeInBitsFor1Layer1Codeword))
//     {
//       cout<<"[both:] Unable to load transportBlockSizeInBitsFor1Layer1Codeword from the given file in TransportBlockMappingInfo_S..."<<endl;
//       abort();
//     }
//   }
//   
// };

struct InterleaverParameters_S{
  
  ivec K;
  ivec f1;
  ivec f2;
  
  void load(string fileName){
    
    if(!parse(fileName,"K", K))
    {
      cout<<"[both:] Unable to load K from the given file in InterleaverParameters_S..."<<endl;
      abort();
    }
    if(!parse(fileName,"f1", f1))
    {
      cout<<"[both:] Unable to load f1 from the given file in InterleaverParameters_S..."<<endl;
      abort();
    }
    if(!parse(fileName,"f2", f2))
    {
      cout<<"[both:] Unable to load f2 from the given file in InterleaverParameters_S..."<<endl;
      abort();
    }
  }
  
};

struct LayerMappingTBSTranslationInfo_S{
  
  ivec L1toL2TBSL1,L1toL2TBSL2;
  ivec L1toL3TBSL1,L1toL3TBSL3;
  ivec L1toL4TBSL1,L1toL4TBSL4;
  
  void load(string fileName){
    //! Loading 1-Layer to 2-Layer Mapping....
    if(!parse(fileName,"L1toL2TBSL1", L1toL2TBSL1))
    {
      cout<<"[both:] Unable to load L1toL2TBSL1 from the given file in LayerMappingTBSTranslationInfo_S..."<<endl;
      abort();
    }
    if(!parse(fileName,"L1toL2TBSL2", L1toL2TBSL2))
    {
      cout<<"[both:] Unable to load L1toL2TBSL2 from the given file in LayerMappingTBSTranslationInfo_S..."<<endl;
      abort();
    }
    
    //! Loading 1-Layer to 3-Layer Mapping....
    if(!parse(fileName,"L1toL3TBSL1", L1toL3TBSL1))
    {
      cout<<"[both:] Unable to load L1toL3TBSL1 from the given file in LayerMappingTBSTranslationInfo_S..."<<endl;
      abort();
    }
    if(!parse(fileName,"L1toL3TBSL3", L1toL3TBSL3))
    {
      cout<<"[both:] Unable to load L1toL3TBSL3 from the given file in LayerMappingTBSTranslationInfo_S..."<<endl;
      abort();
    }
    
    //! Loading 1-Layer to 4-Layer Mapping....
    if(!parse(fileName,"L1toL4TBSL1", L1toL4TBSL1))
    {
      cout<<"[both:] Unable to load L1toL4TBSL1 from the given file in LayerMappingTBSTranslationInfo_S..."<<endl;
      abort();
    }
    if(!parse(fileName,"L1toL4TBSL4", L1toL4TBSL4))
    {
      cout<<"[both:] Unable to load L1toL4TBSL4 from the given file in LayerMappingTBSTranslationInfo_S..."<<endl;
      abort();
    }
  }
  
};

// struct CqiToMaxMCSRate_S{
//   
//   ivec cqiIndices;
//   ivec modulationIndex;
//   vec codeRate;
//   CqiToMaxMCSRate_S(){
//     
//     cqiIndices.set_length(0);
//     modulationIndex.set_length(0);
//     codeRate.set_length(0);
//   }
//   void load(string fileName){
//     
//     if(!parse(fileName,"cqiIndex", cqiIndices))
//     {
//       cout<<"[both:] Unable to load cqiIndex from the given file in CqiToMCSRate_S..."<<endl;
//       abort();
//     }
//     if(!parse(fileName,"modulationIndex", modulationIndex))
//     {
//       cout<<"[both:] Unable to load modulationIndex from the given file in CqiToMCSRate_S..."<<endl;
//       abort();
//     }
//     if(!parse(fileName,"codeRate", codeRate))
//     {
//       cout<<"[both:] Unable to load codeRate from the given file in CqiToMCSRate_S..."<<endl;
//       abort();
//     }
//   }
//   double getMaximumMCSRate(int cqiIndex){
//     
//     if(cqiIndices.length()==0)
//     {
//       cout<<"[both:] Please initialize CqiToMaxMCSRate_S before calling getMaximumMCSRate()..."<<endl;
//       abort();
//     }
//     int indx=find(cqiIndices,cqiIndex);
//     if(indx==-1)
//     {
//       cout<<"[both:] Error...Unknown cqiIndex to getMaximumMCSRate()..."<<endl;
//       abort();
//     }
//     return(((double)modulationIndex(indx))*codeRate(indx));
//   }
//   vec getMaximumMCSRate(ivec cqiIndex){
//     
//     if(cqiIndices.length()==0)
//     {
//       cout<<"[both:] Please initialize CqiToMaxMCSRate_S before calling getMaximumMCSRate()..."<<endl;
//       abort();
//     }
//     ivec indx=find(cqiIndices,cqiIndex);
//     
//     vec mcsRates(indx.length());
//     for(int cwd_cnt=0;cwd_cnt<indx.length();cwd_cnt++)
//     {
//       if(indx(cwd_cnt)>=0)
// 	mcsRates(cwd_cnt)=(double)(modulationIndex(indx(cwd_cnt)))*codeRate(indx(cwd_cnt));
//       else
// 	mcsRates(cwd_cnt)=0; 
//     }
//     
//     return mcsRates;
//   }
//   Array<vec> getMaximumMCSRate(Array<ivec> cqiIndexPerCodeWord){
//     
//     Array<vec> mcsRates(cqiIndexPerCodeWord.length());
//     if(cqiIndices.length()==0)
//     {
//       cout<<"[both:] Please initialize CqiToMaxMCSRate_S before calling getMaximumMCSRate()..."<<endl;
//       abort();
//     }
//     ivec indx;
//     for(int cnt=0;cnt<mcsRates.length();cnt++)
//     {
//       indx=find(cqiIndices,cqiIndexPerCodeWord(cnt));
//       mcsRates(cnt).set_size(indx.length());
//       for(int cwd_cnt=0;cwd_cnt<indx.length();cwd_cnt++)
//       {
// 	if(indx(cwd_cnt)>=0)
// 	  mcsRates(cnt)(cwd_cnt)=(double)(modulationIndex(indx(cwd_cnt)))*codeRate(indx(cwd_cnt));
// 	else
// 	  mcsRates(cnt)(cwd_cnt)=0; 
//       }
//     }
//     return(mcsRates);
//   }
//   double getLeastMCSRate(){
//     
//     if(cqiIndices.length()==0)
//     {
//       cout<<"[both:] Please initialize CqiToMaxMCSRate_S before calling getLeastMCSRate()..."<<endl;
//       abort();
//     }
//     return(((double)modulationIndex(0))*codeRate(0));
//   }
// };


struct cTBTables
{
  
  /********* Member Variables *****************/
  
  Array<bvec> crcPolynomials;
  nrTransportBlockMCSMappingInfo_S transportBlockMCSMapping;
  nrCqiToMaxMCSRate_S cqiToMaxMCSRate;

  AWGNTablesConvCoder awgnTablesConvCoder;  //TODO need to change once AWGN curves for LDPC are ready
  DlVarPowMask_S dlVarPowMask;
  
  
  /********* Member Variables *****************/
  
  
  
  cTBTables()
  {
    crcPolynomials.set_length(6);
    crcPolynomials(0) = bvec("1 1 0 0 1 1 0 1 1");
    crcPolynomials(1) = bvec("1 1 1 0 0 0 1 0 0 0 0 1");
    crcPolynomials(2) = bvec("1 0 0 0 1 0 0 0 0 0 0 1 0 0 0 0 1");
    crcPolynomials(3) = bvec("1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 0 0 0 1 1");
    crcPolynomials(4) = bvec("1 1 0 0 0 0 1 1 0 0 1 0 0 1 1 0 0 1 1 1 1 1 0 1 1");
    crcPolynomials(5) = bvec("1 1 0 1 1 0 0 1 0 1 0 1 1 0 0 0 1 0 0 0 1 0 1 1 1");
  }

  /********************* Member Functions *********************/
  
  
  double closest(vector<double> const& vec, double value)  // returns an element from vec which is equal to 'value' or nearest element but not less than 'value'. Assumes 'vec' in ascending order.
  {
    vector<double>::const_iterator it = std::lower_bound(vec.begin(), vec.end(), value);
    return *(it-1);
  }

  int    quantizeN_RE_Prime(int nREPrime);
  int 	 getTBSizeForSINR(vec SINR, int nRBs, int nREs, int nSymbols, int nLayersInCW, bool isDownlink, double sinrOffsetIndB = 0.0, double targetBLER = 0.1);
  int    getModOrder(int mcsIndex, bool isDownlink);
  int    getCodeRate(int mcsIndex, bool isDownlink);
  
  /*****  Common Fuctions for both LTE and NR *************/
 
  virtual void load(bool isDownlink,string turboInterleaverTableFileName ,string transportBlockMCSMappingFileName, string layerMappingTBSTranslationFileName, string cqiToMCSRateFileName, AWGNTablesConvCoder& tawgnTablesConvCoder)
  {
    bool isNR=true;
    transportBlockMCSMapping.load(transportBlockMCSMappingFileName,isNR,isDownlink);
    cqiToMaxMCSRate.load(transportBlockMCSMappingFileName,cqiToMCSRateFileName,isNR,isDownlink);
    awgnTablesConvCoder = tawgnTablesConvCoder;
  }
  virtual nrCqiToMaxMCSRate_S fGetCqiToMaxMCSRate() { return cqiToMaxMCSRate;}
  virtual nrTransportBlockMCSMappingInfo_S fGetTransportBlockMCSMapping() { return transportBlockMCSMapping; };
  virtual ivec getCodeBlockSizes(int tbSize, double tCodeRate=0);
  virtual int getNumberOfCodeBlocks(int TBSize, double tCodeRate=0);
  virtual int getNumberOfCRCbits(int TBSize, double tCodeRate=0);
  virtual ivec getTBSizeForAllLayersCombined(const ivec& mcsIndices,int nREsAllocated, int allocatedPRBs, int numLayers, int allocatedSymbols, int cqiIndex);
  virtual int  cqiToMCSIndex(int cqiIndex, int rbCount, int nSymbols, int nREs, int nLayers, bool isDownlink, bool isOLLAenabled = false, double rateOffset = 0.0);
  virtual ivec findBestMCSIndex(ivec cqiIndices, int numberOfRBs, int allocatedSymbols,int nREs,int mimoRate, bool isDownlink = true, bool isOLLAenabled=false, vec rateOffset = zeros(2));
  virtual int  findOptimalCQIIndex(ivec cqiIndices);
  virtual LinkAdaptation_S getMCSPerCodeWord(Array< vec > SINRIndBPerCodeWord, int nRBs, int nSymbols, int nREs, int nLayers, bool isDownlink, vec sinrOffsetIndB , vec rateThresholds , vec targetBLER);
  virtual int getTBSizeForAllLayersCombined(int mcsIndex, int nREsAllocated, int allocatedPRBs, int numLayers, int allocatedSymbols, bool isDownlink=true);
  
  
  /*****  Common Fuctions for both LTE and NR *************/

  
  /********************* Member Functions *********************/

  
};


struct cLTETables : public cTBTables                          // Inherited from cNRTables 
{ 
  /********************* Member variables *********************/
  
  //   Array<bvec> crcPolynomials;    // two times
  
  LayerMappingTBSTranslationInfo_S layerMappingTBSTranslation;
  InterleaverParameters_S interleaverParameters;
  //   AWGNTablesConvCoder awgnTablesConvCoder;  // two times
  //   DlVarPowMask_S dlVarPowMask;//! DL power-allocation  // two times
  
  /********************* Member variables *********************/
  
  
  cLTETables()
  {
    crcPolynomials.set_length(4);
    crcPolynomials(0) = bvec("1 1 0 0 1 1 0 1 1");
    crcPolynomials(1) = bvec("1 0 0 0 1 0 0 0 0 0 0 1 0 0 0 0 1");
    crcPolynomials(2) = bvec("1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 0 0 0 1 1");
    crcPolynomials(3) = bvec( "1 1 0 0 0 0 1 1 0 0 1 0 0 1 1 0 0 1 1 1 1 1 0 1 1");
  }

  
  
  /********************* Member Functions *********************/
  
  
  
  /*****  Common Fuctions for both LTE and NR *************/
  
  
  void load(bool isDownlink,string turboInterleaverTableFileName ,string transportBlockMCSMappingFileName, string layerMappingTBSTranslationFileName, string cqiToMCSRateFileName, AWGNTablesConvCoder& tawgnTablesConvCoder)
  {
    bool isNR=false;
    interleaverParameters.load(turboInterleaverTableFileName);
    transportBlockMCSMapping.load(transportBlockMCSMappingFileName,isNR,isDownlink);
    layerMappingTBSTranslation.load(layerMappingTBSTranslationFileName);
    cqiToMaxMCSRate.load(transportBlockMCSMappingFileName,cqiToMCSRateFileName,isNR,isDownlink);
    awgnTablesConvCoder = tawgnTablesConvCoder;
  }
  nrCqiToMaxMCSRate_S fGetCqiToMaxMCSRate() { return cqiToMaxMCSRate;}
  nrTransportBlockMCSMappingInfo_S fGetTransportBlockMCSMapping() { return transportBlockMCSMapping; };
  ivec getCodeBlockSizes(int tbSize, double tCodeRate);
  int getNumberOfCodeBlocks(int TBSiz, double tCodeRate);
  int getNumberOfCRCbits(int tbSize, double tCodeRate);
  ivec getTBSizeForAllLayersCombined(const ivec& tbIndices,int nREsAllocated,int allocatedPRBs,int numLayers, int allocatedSymbols,int cqiIndex);
  int  cqiToMCSIndex(int cqiIndex, int rbCount, int nSymbols, int nREs, int nLayers, bool isDownlink, bool isOLLAenabled = false, double rateOffset = 0.0);
  ivec findBestMCSIndex(ivec cqiIndices, int numberOfRBs, int allocatedSymbols,int nREs,int mimoRate, bool isDownlink = true, bool isOLLAenabled=false, vec rateOffset=zeros(2));
  int  findOptimalCQIIndex(ivec cqiIndices);
  LinkAdaptation_S getMCSPerCodeWord(Array< vec > SINRIndBPerCodeWord, int nRBs, int nSymbols, int nREs, int nLayers, bool isDownlink, vec sinrOffsetIndB, vec rateThresholds, vec targetBLER);
  int  getTBSizeForAllLayersCombined(int tbIndex, int nREsAllocated, int allocatedPRBs, int numLayers, int allocatedSymbols, bool isDownlink=true);
  
  
  /*****  Common Fuctions for both LTE and NR *************/

};



#endif







// struct cNRTables_S
// {
//   double maxCQIRateNR=0; int nRE=0;
//   Array<bvec> crcPolynomials;
//   nrTransportBlockMCSMappingInfo_S transportBlockMCSMapping;
//   nrCqiToMaxMCSRate_S cqiToMaxMCSRate;
// 
//   AWGNTablesConvCoder awgnTablesConvCoder;  //TODO
//   DlVarPowMask_S dlVarPowMask;//! DL power-allocation  //TODO 
// 
//   cNRTables_S(){
//     
//     crcPolynomials.set_length(6);
//     crcPolynomials(0) = bvec("1 1 0 0 1 1 0 1 1");
//     crcPolynomials(1) = bvec("1 1 1 0 0 0 1 0 0 0 0 1");
//     crcPolynomials(2) = bvec("1 0 0 0 1 0 0 0 0 0 0 1 0 0 0 0 1");
//     crcPolynomials(3) = bvec("1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 0 0 0 1 1");
//     crcPolynomials(4) = bvec("1 1 0 0 0 0 1 1 0 0 1 0 0 1 1 0 0 1 1 1 1 1 0 1 1");
//     crcPolynomials(5) = bvec("1 1 0 1 1 0 0 1 0 1 0 1 1 0 0 0 1 0 0 0 1 0 1 1 1");
//   }
//   void load(string transportBlockMCSMappingFileName, string cqiToMCSRateFileName, AWGNTablesConvCoder& tawgnTablesConvCoder){
//     
//     transportBlockMCSMapping.load(transportBlockMCSMappingFileName);
//     cqiToMaxMCSRate.load(transportBlockMCSMappingFileName,cqiToMCSRateFileName);
//     
//     awgnTablesConvCoder = tawgnTablesConvCoder;
//   }
//   double closest(vector<double> const& vec, int value)  // returns an element from vec which is equal to 'value' or nearest element but not less than 'value'. Assumes 'vec' in ascending order.
//   {
//     vector<double>::const_iterator it = std::lower_bound(vec.begin(), vec.end(), value);
//     return *(it);
//   }
//   nrTransportBlockMCSMappingInfo_S fGetTransportBlockMCSMapping() { return transportBlockMCSMapping; }
//   void   setMaxCQIRateNR(double val){maxCQIRateNR =val;}
//   double getMaxCQIRateNR(){return maxCQIRateNR;}
//   void   setN_RE(int val){nRE = val;}
//   int    getN_RE(){return nRE;}
//   ivec   getCodeBlockSizes(double codeRate, int TBSize);
//   int    quantizeN_RE_Prime(int nREPrime);
//   ivec   getTBSizeForAllLayersCombined(const ivec& tbIndices, int allocatedPRBs, int numLayers, int allocatedSymbols);
//   int    cqiToMCSIndex(int cqiIndex, int rbCount, int nSymbols, int nREs, int nLayers, bool isDownlink, bool isOLLAenabled = false, double rateOffset = 0.0);
//   ivec   findBestMCSIndex(ivec cqiIndices, int numberOfRBs, int allocatedSymbols,int nREs,int mimoRate, bool isDownlink = true, bool isOLLAenabled=false, vec rateOffset=vec("0.0 0.0"));
//   int    findOptimalCQIIndex(ivec cqiIndices);
//   ivec   getMCSPerCodeWord(vec& RatePerCW, Array< ivec > CQIPerCW, int nRBs, int nSymbols, int nREs, int nLayers, bool isDownlink, vec sinrOffsetIndB = zeros(2), double targetBLER = 0.1);
//   ivec   getTBSizeForAllLayersCombined(const ivec& tbIndices, int allocatedPRBs, int numLayers, int allocatedSymbols);
// };

