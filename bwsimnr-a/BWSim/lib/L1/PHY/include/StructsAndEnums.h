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

#ifndef _SAE_LTE_H_
#define _SAE_LTE_H_

#include "../../../Frozen/simSupport/include/CommonStructsAndEnums.h"
#include <map>


enum RSType_E{
  
  _RSTYPE_UNINIT_=-1,
  _RSTYPE_CRS_,
  _RSTYPE_DMRS_,
  _RSTYPE_CSIRS_,
  _RSTYPE_MBSFNRS_,
  _RSTYPE_PRS_
};

enum AperiodicReportingMode_E {
  
  _APERIODIC_MODE_UNINIT_=-1,
  _APERIODIC_MODE_1_2_,
  _APERIODIC_MODE_2_0_,
  _APERIODIC_MODE_2_2_,
  _APERIODIC_MODE_3_0_,
  _APERIODIC_MODE_3_1_,
  _APERIODIC_MODE_3_2_
};

enum PeriodicReportingMode_E {
  
  _PERIODIC_MODE_UNINIT_=-1,
  _PERIODIC_MODE_1_0_,
  _PERIODIC_MODE_1_1_,
  _PERIODIC_MODE_2_0_,
  _PERIODIC_MODE_2_1_
};
enum ReportingModeFor_1_1_E {
  _REPORTING_MODE_UNIT_=-1,
  _SUBMODE_1_,
  _SUBMODE_2_
};
enum NRCodeBookMode {
  _NR_CODEBOOK_MODE_UNIT_ = -1,
  _NR_CODEBOOK_MODE_1_,
  _NR_CODEBOOK_MODE_2_
};
enum NRCodeBookType {
  _NR_CODEBOOK_TYPE_UNIT_ = -1,
  _NR_CODEBOOK_TYPE_I_,
  _NR_CODEBOOK_TYPE_I_MULTIPANEL_,
  _NR_CODEBOOK_TYPE_II_,
  _NR_CODEBOOK_TYPE_II_ENHANCED_,
  _NR_CODEBOOK_TYPE_II_ENHANCED_CJT_,
  _NR_CODEBOOK_PORT_SELECTION_,
  _NR_CODEBOOK_PORT_SELECTION_ENHANCED_,
  _NR_CODEBOOK_PORT_SELECTION_FURTHER_ENHANCED_
};
enum CQIFormatIndicator{
    _UNINIT_CQI_=-1,
    _WIDEBAND_CQI_,
    _SUBBAND_CQI_
};

enum PMIFormatIndicator{
    _UNINIT_PMI_=-1,
    _WIDEBAND_PMI_,
    _SUBBAND_PMI_
};
enum TypeIMethod{
    _UNINIT_TYPEI=-1,
    _SPECTRAL_EFFICIENCY_,
    _SEARCHFREE_
};
enum TypeIIMethod{
    _UNINIT_TYPEII=-1,
    _HALFBEAM_,
    _OMP_,
    _AVIK_
};

 struct TRSConfig_S
  {
  int TRSPeriodicityInms;
  int trssymbolno; 
  
  TRSConfig_S(){
    
    trssymbolno=-1;
    TRSPeriodicityInms=0;
  };
  
  struct SRSconfig_S{

  Array<ivec>  srsstartRB;
 int  srsnRB;
 ivec  symbolnoinsrsresource;
 int  srscomboffset;
 
 SRSconfig_S():srsnRB(-1),symbolnoinsrsresource(ivec(0)),srscomboffset(-1)
 {}
 
};

  SRSconfig_S SRSconfig;
  
  
  bool isToTransmitTRSPorts(int subframe)
  { 
    bool isTRSPortAvailable=false;
    
    if(TRSPeriodicityInms!=0)
    {
      if((subframe%TRSPeriodicityInms)==0 || (subframe%TRSPeriodicityInms)==1)
	isTRSPortAvailable=true;
      else
	isTRSPortAvailable=false;
    }
  
    return isTRSPortAvailable;
  }
 
  
  };
  
struct Size_S{

  int numberOfSymbols;
  int numberOfSubcarriers;

  Size_S(){

    numberOfSubcarriers=-1;
    numberOfSymbols=-1;
  }
  Size_S(int nSubc, int nSym){

    numberOfSubcarriers=nSubc;
    numberOfSymbols=nSym;
  }
};


struct LayerInfo_S{
  
  int nLayers; // Desired layer count
  bvec CDMMap; // Bitmap indicating desired layers
  int dmrstype;
  int CDM;
  int nCDM;
  int maxLength;
    
  LayerInfo_S(){
    
    nLayers=0;
    CDMMap.set_length(0);
    dmrstype=-1;
    maxLength=-1;
  }
  
  LayerInfo_S(int layers,int dmrsType,int MaxLength){
    
    dmrstype=dmrsType;
    maxLength=MaxLength;
    nLayers=layers;
    if(dmrsType==1)
    {   
        if(maxLength==1 && nLayers>4){cout<<"nLayers: "<<nLayers<<"\tDMRS Type 1 with MaxLength 1 supports upto 4 Layers only..."<<endl;abort();}
        CDM = (maxLength==1) ? 2 : (nLayers>4) ? 4 : 2;
        nCDM = (nLayers>2) ? 2 : 1;
    }
    else if(dmrsType==2)
    {
        if(maxLength==1 && nLayers>6){cout<<"nLayers: "<<nLayers<<"\tDMRS Type 2 with MaxLength 1 supports upto 6 Layers only..."<<endl;abort();}
        CDM = (maxLength==1) ? 2 : (nLayers>6) ? 4 :2;
        nCDM = (nLayers>4) ? 3 : (nLayers>2) ? 2 : 1;
    }
    CDMMap=ones_b(nCDM);
  }
  void print()
  {
      cout<<"\nDMRS Type: "<<dmrstype<<" MaxLength: "<<maxLength<<" nLayers: "<<nLayers;
      cout<<" CDM: "<<CDM<<" nCDM: "<<nCDM<<" CDMMap: "<<CDMMap<<endl;
  }
//   LayerInfo_S operator=(LayerInfo_S layerInfo)
//   {
//       LayerInfo_S this_(layerInfo.nLayers,layerInfo.totalLayers,layerInfo.dmrstype,layerInfo.maxLength);
//       this_.CDM = layerInfo.CDM;this_.nCDM = layerInfo.nCDM;this_.CDMMap = layerInfo.CDMMap;
//       return this_;
//   }
  
};

struct DlVarPowMask_S{
    bool isInitialized;
    ivec Pb_index;
    vec Tx1Ant;
    vec Tx_2_4Ant;
    ivec Rho_A_1_2Ant;
    ivec Rho_A_4Ant;
    ivec Rho_B_1_2Ant;
    ivec Rho_B_4Ant;
    DlVarPowMask_S(){
        isInitialized = false;
        Pb_index.set_size(4);
        Pb_index(0)= 0;Pb_index(1)= 1;Pb_index(2)= 2;Pb_index(3)= 3;
        Tx1Ant.set_size(4);
        Tx1Ant(0) =1.0 ;Tx1Ant(1) =4/5.0 ;Tx1Ant(2) =3/5.0 ;Tx1Ant(3) = 2/5.0;
        Tx_2_4Ant.set_size(4);
        Tx_2_4Ant(0)=5/4.0 ;Tx_2_4Ant(1)=1.0 ;Tx_2_4Ant(2)=3/4.0 ;Tx_2_4Ant(3)=1/2.0 ;
        Rho_A_1_2Ant.set_size(5);
        Rho_A_1_2Ant(0)=1;Rho_A_1_2Ant(1)=2;Rho_A_1_2Ant(2)=3;Rho_A_1_2Ant(3)=5;Rho_A_1_2Ant(4)=6;
        Rho_A_4Ant.set_size(4);
        Rho_A_4Ant(0)=2 ;Rho_A_4Ant(1)=3 ;Rho_A_4Ant(2)=5 ;Rho_A_4Ant(3)= 6;
        Rho_B_1_2Ant.set_size(2);
        Rho_B_1_2Ant(0)=0 ;Rho_B_1_2Ant(1)=4 ;
        Rho_B_4Ant.set_size(3);
        Rho_B_4Ant(0)= 0;Rho_B_4Ant(1)= 1;Rho_B_4Ant(2)=4;
    }
};

struct PortInfo_S{
  
  TransmissionMode_E configuredTxMode;
  TransmissionMode_E configuredULTxMode;
  int TM7DemodPort;
  int nPorts;
  int nCRSPorts;
  bool enableFallBackMode;
  imat NZPCSIConfigurations;
  bool enableZPcsirs;
  int  ZPcsirsPattern;
  ivec ZPcsirsSymPositions;
  ivec NZPcsirsSymPositions;
  bool multipleCSIProcessFlag;
  int SRSCount;
  bool enablePTRS;
  
  
  PortInfo_S();
  void load(string DLConfigFile,string ULConfigFile);
  ivec getCQIPorts();
  int getTRSPort();
  ivec getCQIPorts(int nAntennaPorts);
  ivec getDLDemodPorts(TransmissionScheme_E transmissionScheme, LayerInfo_S layerInfo, int nAntennaPorts);
  ivec getZPCsiPorts(TransmissionScheme_E transmissionScheme, PortInfo_S& portInfo);
  ivec getPTRSPorts(TransmissionScheme_E transmissionScheme,int higherlayerparam1);
  ivec getOtherDemodPorts(TransmissionScheme_E transmissionScheme, LayerInfo_S layerInfo);
  ivec getULCQIPorts(int nAntennaPorts);
  ivec getULDemodPorts(TransmissionScheme_E transmissionScheme, LayerInfo_S layerInfo, int nAntenna);
  
  };
  

struct CCHInfo_S{

  bool enableShortPUCCH = false;  
  bool enablelongPUCCH = false;
  int pdcchCount;
  int srsCount;
//   bool enablePBCH;
//   bool enablePssSss;
  int SRSPeriodicityInms;
  bool enableEPDCCH;
  
  bool enableSSB;
  bool isSSBPresent;
  ivec SSBSymbols;
  int SSBOffsetFromPRB0; // Defined in terms of Subcarriers
  int SSBSubcarrierCount;
  double SSBNuScaling;// ssb_numerology/BWP_numerology
  
  CCHInfo_S(){
    
    pdcchCount=0;
    srsCount=0;
    SRSPeriodicityInms=0;
    enableSSB= false;
    isSSBPresent= false;
//     enablePBCH=false;
//     enablePssSss=false;
    enableEPDCCH=false;
    SSBSymbols.set_length(0);
    SSBOffsetFromPRB0 = -1;
    SSBSubcarrierCount =-1;
    SSBNuScaling=-1;
  }
};


struct FrameReservationInfo_S{
  
  CCHInfo_S cchInfo;
  ivec prbs;
  Array<ivec> cchtonesPerRB; 
  Array<ivec> dlPrbMapping; 
  ivec reservedVRBs;
  ivec reservedSymbols;
  ivec reservedVRBsForEpdcch;
  Array < ivec > BSBWPLocations;
  
  FrameReservationInfo_S(){
    
    prbs.set_length(0);
    cchtonesPerRB.set_length(0);
    dlPrbMapping.set_length(0);
    reservedVRBs.set_length(0);
    reservedSymbols.set_length(0);
    
  }
  void clear(){
    
    prbs.set_length(0);
    cchtonesPerRB.set_length(0);
    dlPrbMapping.set_length(0);
    reservedVRBs.set_length(0);
    reservedSymbols.set_length(0);
  }
};

struct RSInfo_S{
  
  int cellid;
  double rsBoostInDB;
  RSType_E CQIrsType;
  RSType_E DeModrsType;
  ivec rsPorts;
  ivec portReferenceNumbers;
  ivec CQIports;
  ivec ZPcsiports;
  int nCQIports;
  int dmrsType;
  int CDM;
  int nCDM;
  ivec DeModPorts;
  ivec otherDemodPorts;
  ivec PtrsPorts;
  ivec crsPorts;
  bool fillPRS;
  int PRSperiodicity;
  int PRSoffset;
  Array<ivec> rsSymPosition;
  ivec rsScPosition;
  int csirsPattern;
  int srsPattern;
  bool multipleCSIProcessFlag;
  bool isTRSpresent;
  Array<ivec> rsrbpos;
  ivec PTRSassocDMRSPorts;
  
  RSInfo_S(){
    
    cellid=-1;
    rsBoostInDB=0;
    CQIrsType=_RSTYPE_UNINIT_;
    DeModrsType=_RSTYPE_UNINIT_;
    rsPorts.set_length(0);
    CQIports.set_length(0);
    ZPcsiports.set_length(0);
    crsPorts.set_length(0);
    DeModPorts.set_length(0);
    otherDemodPorts.set_size(0);
    PtrsPorts.set_length(0);
    portReferenceNumbers.set_length(0);
    fillPRS=false;
    PRSperiodicity=-1;
    PRSoffset=-1;
    rsSymPosition.set_length(0);
    rsScPosition.set_length(0);
    csirsPattern=-1;
    multipleCSIProcessFlag=false;
    isTRSpresent=false;
    rsrbpos.set_length(0);
    PTRSassocDMRSPorts.set_length(0);
    
  }
};

struct UplinkCQIModeInfo_S {

  int CQIProcessingDelay;// the processing delay PHY eNB takes to decode the CQI received in the reported subframe
  int CQIPMIOffset;// the subframe offset where CQI/PMI has to be reported
  int CQIPMIPeriodicity;// the periodicity with which CQI/PMI has to be reported
  int ReciproMeasureOffset; //reciprocity
  void load(string configFile){
    
    if(!parse(configFile,"CQIPMIPeriodicityInms",CQIPMIPeriodicity))
    {
      cout<<"Loading CQIPMIPeriodicity as 5 ms by default.. "<<endl;
      CQIPMIPeriodicity=5;
    }
    if(!parse(configFile,"CQIPMIOffsetInms",CQIPMIOffset))
    {
      cout<<"Loading CQIPMIOffset as 1 ms by default.. "<<endl;
      CQIPMIOffset=1;
    }
    if(!parse(configFile,"CQIProcessingDelayInms",CQIProcessingDelay))
    {
      cout<<"Loading CQIProcessingDelay as 1 ms by default.. "<<endl;
      CQIProcessingDelay=1;
    }
    
      if(!parse(configFile,"ReciproMeasureOffsetInms",ReciproMeasureOffset))
    {
      cout<<"Loading CQIProcessingDelay as 1 ms by default.. "<<endl;
      ReciproMeasureOffset=0;
    }
    
  }
  bool isToTransmitCQIPorts(int slotNumber)
  {
    int cqiPMIOffset;
    
    if(CQIPMIOffset>=(CQIProcessingDelay))
      cqiPMIOffset=CQIPMIOffset;
    else  
      cqiPMIOffset=CQIPMIOffset+CQIPMIPeriodicity;
    
    if(slotNumber>=ReciproMeasureOffset) //reciprocity
    {
    if((slotNumber%CQIPMIPeriodicity)==(cqiPMIOffset-(CQIProcessingDelay)))
      return true;
    else
        return false;
    }
    else
      return false;
  }
  
  
};

struct DownlinkCQIModeInfo_S {
  
  bool isAperiodicCQICompEnabled;
  int CQIReportingDelay;
  
  int CQIProcessingDelay;// the processing delay PHY eNB takes to decode the CQI received in the reported subframe
  int CQIPMIOffset;// the subframe offset where CQI/PMI has to be reported
  int CQIPMIPeriodicity;// the periodicity with which CQI/PMI has to be reported
  int RIPeriodicity;// the (RIPeriodicity*widebandperiodicity) periodicity with which RI has to be reported
  int RIOffset;// the subframe offset(wrt wideband CQI) where RI has to be reported
  int BWPartCycles;// the number of cycles subbandCQI has to be reported between two widebandCQI reports
  int numOfCSIRSPorts;// the number of CSI-RS ports configured
  int CSIRSPeriodicity;// the periodicty and offset of CSI-RS ports (subframe config)
  int CSIRSConfig;// the RS position config
  int FirstWideBandPMIPeriod;//! Periodicity with which Wideband PMI index i1 to be reported (For 8 antenna ports case only)
  enum AperiodicReportingMode_E aperiodicMode;// configured aperiodicMode
  enum PeriodicReportingMode_E periodicMode;// configured periodicMode
  enum ReportingModeFor_1_1_E submode;//! To be configured for periodicMode 1_1 for the case of 8 ports
  enum CQIFormatIndicator cqiFormatIndicator;
  enum PMIFormatIndicator pmiFormatIndicator;
  enum TypeIMethod typeIMethod;
  enum TypeIIMethod typeIIMethod;
  int riRestriction;
  DownlinkCQIModeInfo_S(){
    
    isAperiodicCQICompEnabled=false;
    CQIProcessingDelay=-1;
    CQIPMIPeriodicity=-1;
    RIPeriodicity=-1;
    CQIPMIOffset=-1;
    RIOffset=-1;
    BWPartCycles=-1;
    numOfCSIRSPorts=-1;
    CSIRSPeriodicity=-1;
    CSIRSConfig=-1;
    FirstWideBandPMIPeriod=-1;
    CQIReportingDelay=-1;
    aperiodicMode=_APERIODIC_MODE_UNINIT_;
    periodicMode=_PERIODIC_MODE_UNINIT_;
    submode=_REPORTING_MODE_UNIT_;
    cqiFormatIndicator=_UNINIT_CQI_;
    pmiFormatIndicator=_UNINIT_PMI_;
    typeIMethod = _UNINIT_TYPEI;
    typeIIMethod = _UNINIT_TYPEII;
    riRestriction=-1;
  }
  
  void load(string configFile){
    
    if(!parse(configFile,"enablePerAperCqiComp",isAperiodicCQICompEnabled))
    {
      isAperiodicCQICompEnabled=false;
    }
    
    if(!parse(configFile,"CQIProcessingDelayInms",CQIProcessingDelay))
    {
      CQIProcessingDelay=1;
    }
    if(!parse(configFile,"CQIPMIPeriodicityInms",CQIPMIPeriodicity))
    {
      CQIPMIPeriodicity=5;
    }
    if(!parse(configFile,"RIPeriodicityInms",RIPeriodicity))
    {
      RIPeriodicity=1;
    }
    if(!parse(configFile,"CQIPMIOffsetInms",CQIPMIOffset))
    {
      CQIPMIOffset=1;
    }
    
    if(!parse(configFile,"CQIReportingDelayinms",CQIReportingDelay))
    {
      CQIReportingDelay=4;
    }
    
    if(!parse(configFile,"RIOffsetInms",RIOffset))
    {
      RIOffset=1;
    }
    if(!parse(configFile,"BWPartCycles",BWPartCycles))
    {
      BWPartCycles=1;
    }
  
    string aMode;
    if(isAperiodicCQICompEnabled && parse(configFile,"AperiodicMode",aMode))
    {
      if(aMode=="_APERIODIC_MODE_1_2_") aperiodicMode=_APERIODIC_MODE_1_2_;
      else if(aMode=="_APERIODIC_MODE_3_0_") aperiodicMode=_APERIODIC_MODE_3_0_;
      else if(aMode=="_APERIODIC_MODE_3_1_") aperiodicMode=_APERIODIC_MODE_3_1_;
      else if(aMode=="_APERIODIC_MODE_3_2_") aperiodicMode=_APERIODIC_MODE_3_2_;
      else if(aMode=="_APERIODIC_MODE_2_0_") aperiodicMode=_APERIODIC_MODE_2_0_;
      else if(aMode=="_APERIODIC_MODE_2_2_") aperiodicMode=_APERIODIC_MODE_2_2_;
      else
      {
	cout<<"[both:] Unknown Aperiodic mode for CQI Computation..Default value not possible "<<endl;abort();
      }
      
    }
    else
    {
      aperiodicMode=_APERIODIC_MODE_UNINIT_;
    }
    bool tModeAdaptation=false;
    
    parse(configFile,"DL_enableModeAdaptation",tModeAdaptation);
    
    string pMode;
    if(parse(configFile,"PeriodicMode",pMode) && !tModeAdaptation)
    {
      if(pMode=="_PERIODIC_MODE_1_0_") periodicMode=_PERIODIC_MODE_1_0_;
      else if(pMode=="_PERIODIC_MODE_1_1_") periodicMode=_PERIODIC_MODE_1_1_;
      else if(pMode=="_PERIODIC_MODE_2_0_") periodicMode=_PERIODIC_MODE_2_0_;
      else if(pMode=="_PERIODIC_MODE_2_1_") periodicMode=_PERIODIC_MODE_2_1_;
      else
      {
	cout<<"[both:] Unknown Periodic mode for CQI Computation..Default value not possible "<<endl;abort();
      }
      
    }
    else if(tModeAdaptation)
    {
      periodicMode=_PERIODIC_MODE_1_0_;
    }
    else
    {
      periodicMode=_PERIODIC_MODE_UNINIT_;
    }
    //! Now Verifying the parsed inputs
    assert(CQIPMIPeriodicity > CQIProcessingDelay && "Currently (CQIPMIPeriodicity<=CQIProcessingDelay) not supported !!!!!"); //CQI1 since if CQIPMIPeriodocity<=CQIProcessing Delay then buffer are to be maintained 
//     assert(CQIPMIPeriodicity > CQIPMIOffset && "Currently (CQIPMIPeriodicity<=CQIPMIOffset) not supported !!!!!"); 
    assert((CQIPMIPeriodicity==5 || CQIPMIPeriodicity==10 || CQIPMIPeriodicity==20 || CQIPMIPeriodicity==40) && "CQIPMIPeriodicity must be in {5,10,20,40} set !!!!!");
    string TM_Mode;
    if(parse(configFile,"DL_TransmissionMode",TM_Mode))
    {
      if((TM_Mode=="_TRANSMISSION_MODE_1_") || (TM_Mode=="_TRANSMISSION_MODE_2_") ||(TM_Mode=="_TRANSMISSION_MODE_3_") || (TM_Mode=="_TRANSMISSION_MODE_7_"))
      {
	if(!((pMode=="_PERIODIC_MODE_1_0_") || (pMode=="_PERIODIC_MODE_2_0_")))
	{cout<<"[both:] Invalid Periodic CQI mode for "<<TM_Mode<<endl;abort();}
	
      }    
      else if((TM_Mode=="_TRANSMISSION_MODE_4_") || (TM_Mode=="_TRANSMISSION_MODE_5_") || (TM_Mode=="_TRANSMISSION_MODE_6_"))
      {
	if(!((pMode=="_PERIODIC_MODE_1_1_") || (pMode=="_PERIODIC_MODE_2_1_")))
	{cout<<"[both:] Invalid Periodic CQI mode for "<<TM_Mode<<endl;abort(); }
      }
    }
    
    if((TM_Mode=="_TRANSMISSION_MODE_9_") || (TM_Mode=="_TRANSMISSION_MODE_NR_")  || (TM_Mode=="_TRANSMISSION_MODE_10_") || (TM_Mode=="_TRANSMISSION_MODE_11_"))
    {
      if(((pMode=="_PERIODIC_MODE_1_0_") || (aMode=="_APERIODIC_MODE_3_0_")))
      {cout<<"[both:] Transmission mode "<<TM_Mode<<" does not support reporting mode without pmi ..."<<endl;
	if(pMode=="_PERIODIC_MODE_1_0_")
	  cout<<"Reporitng mode is :"<<pMode<<endl;
	else
	  cout<<"Reporitng mode is :"<<aMode<<endl;
	abort(); 
	
      }
      
      string sMode;
      if(parse(configFile,"ReportingModeFor_1_1_",sMode))
      {
	if(sMode=="_SUBMODE_1_")
	  submode=_SUBMODE_1_;
	else if(sMode=="_SUBMODE_2_") 
	  submode=_SUBMODE_2_;
	else
	{
	  cout<<"[both:] Unknown Reporting Mode for 1_1 for CQI Computation..Default value not possible "<<endl;abort();
	}      
      }
      else
      {
	submode=_REPORTING_MODE_UNIT_;
      }
      
      
      if(!parse(configFile,"CSIRSPeriodicity",CSIRSPeriodicity))
      {
	CSIRSPeriodicity=5;
      }
      if(!parse(configFile,"CSIRSConfig",CSIRSConfig))
      {
	CSIRSConfig=1; 
      }
      if(!parse(configFile,"FirstWideBandPMIPeriod",FirstWideBandPMIPeriod))
      {
	FirstWideBandPMIPeriod=2;
      }
      if(currentTaskid==0)
      {
        cout<<"isAperiodicCQICompEnabled:	"<<isAperiodicCQICompEnabled<<endl;
        cout<<"CQIProcessingDelay: "<<CQIProcessingDelay<<endl;
        cout<<"CQIPMIPeriodicity: "<<CQIPMIPeriodicity<<endl;
        cout<<"RIPeriodicity: "<<RIPeriodicity<<endl;
        cout<<"CQIPMIOffset: "<<CQIPMIOffset<<endl;
        cout<<"CQIReportingDelayinms: "<<CQIReportingDelay<<endl;
        cout<<"RIOffset: "<<RIOffset<<endl;
        cout<<"BWPartCycles: "<<BWPartCycles<<" cycle  "<<endl;
        cout<<"CSIRSPeriodicity  "<<CSIRSPeriodicity<<" subframe  "<<endl;
        cout<<"CSIRSConfig  "<<CSIRSConfig<<" subframe  "<<endl;
        cout<<"FirstWideBandPMIPeriod  "<<FirstWideBandPMIPeriod<<" subframe  "<<endl;
      }
      
    }
    int maxRank;
    parse(configFile,"DL_riRestriction",maxRank);
    riRestriction = maxRank;
    string type1Method,CQIType,PMIType,type2Method;
    
    parse(configFile,"CQIFormatIndicator",CQIType);
    parse(configFile,"PMIFormatIndicator",PMIType);
    parse(configFile,"TypeIMethod",type1Method);
    parse(configFile,"TypeIIMethod",type2Method);
    
    if(type1Method == "_SPECTRAL_EFFICIENCY_")
    {
        typeIMethod = _SPECTRAL_EFFICIENCY_;
    }
    else if(type1Method == "_SEARCHFREE_")
    {
        typeIMethod = _SEARCHFREE_;
    }
    else
    {
        cout<<"Invalid TypeIMethod..."<<endl;abort();
    }
    
    if(type2Method == "_HALFBEAM_")
    {
        typeIIMethod = _HALFBEAM_;
    }
    else if(type2Method == "_OMP_")
    {
        typeIIMethod = _OMP_;
    }
    else if(type2Method == "_AVIK_")
    {
        typeIIMethod = _AVIK_;
    }
    else
    {
        cout<<"Warning....TypeIIMethod uninitialised...Loading OMP as default"<<endl;
        typeIIMethod = _OMP_;
    }
    
    if(CQIType == "_WIDEBAND_CQI_")
    {
        cqiFormatIndicator = _WIDEBAND_CQI_;
    }
    else if(CQIType == "_SUBBAND_CQI_")
    {
        cqiFormatIndicator = _SUBBAND_CQI_;
    }
    else{cout<<"Invalid CQIFormatIndicator ..."<<endl;abort();}
    
    if(PMIType == "_WIDEBAND_PMI_")
    {
        pmiFormatIndicator = _WIDEBAND_PMI_;
    }
    else if(PMIType == "_SUBBAND_PMI_")
    {
        pmiFormatIndicator = _SUBBAND_PMI_;
    }
    else{cout<<"Invalid PMIFormatIndicator ..."<<endl;abort();}
    
    if(currentTaskid==0)
    {
        cout<<"PMIFormatIndicator: "<<PMIType<<endl;
        cout<<"CQIFormatIndicator: "<<CQIType<<endl;
        cout<<"typeIMethod: "<<type1Method<<endl;
        cout<<"typeIIMethod: "<<type2Method<<endl;
    }
  }

  
  bool isToTransmitCQIPorts(int slotNumber)
  {
   int cqiPMIOffset;
    
    if(CQIPMIOffset>=(CQIReportingDelay+CQIProcessingDelay))
      cqiPMIOffset=CQIPMIOffset;
    else  
      cqiPMIOffset=CQIPMIOffset+CQIPMIPeriodicity;
    
    if((slotNumber%CQIPMIPeriodicity)==(cqiPMIOffset-(CQIReportingDelay+CQIProcessingDelay)))
      return true;
    else
      return false;
  }



  
  bool isToReportCQI(int slotNumber)
  {
    int cqiPMIOffset;
    
    if(CQIPMIOffset>=(CQIReportingDelay+CQIProcessingDelay))
      cqiPMIOffset=CQIPMIOffset;
    else  
      cqiPMIOffset=CQIPMIOffset+CQIPMIPeriodicity;
    
    if((slotNumber%CQIPMIPeriodicity)==(cqiPMIOffset))
      return true;
    else
      return false;
    }
};
/*
  void init(int nuID,int nCodeWords,int nLayers,double txSubcarrierPowerIndBm,bool enableRankAdaptation,TransmissionScheme_E transmissionscheme,Waveform_E waveform,int associatedServerNode, RSInfo_S &rsInfo,bool enableCQI0Feedback=false,DownlinkCQIModeInfo_S dlCQImodeInfo=DownlinkCQIModeInfo_S(),SRSconfig_S srsConfig=SRSconfig_S()){

    cqiMeasurementConfig(0).SRSconfig=srsConfig;

  }
*/
// struct NRCQIInfoAPeriodic_S{
//     
//     ivec wbCqiIndexPerCodeWord;
//     Array<ivec> layerIndicationPerCodeWord;
//     Array<ivec> sbCqiIndexPerCodeWord;
//     Array< Array<cmat> > channelPerSubBand;
//     int widebandPMI;
//     ivec subbandIndex;
//     ivec  subbandPMI;
//     ivec subbandPrecIndi2;
//     int wideBandPrecoderIndi1_1;
//     int wideBandPrecoderindi1_2;
//     int wideBandPrecoderindi1_3;
//     bool isValid; 
//     NRCQIInfoAPeriodic_S():wbCqiIndexPerCodeWord(ivec(0)),layerIndicationPerCodeWord(0),channelPerSubBand(0),sbCqiIndexPerCodeWord(0),widebandPMI(-1),subbandIndex(ivec(0)),subbandPMI(ivec(0)),subbandPrecIndi2(ivec(0)),wideBandPrecoderIndi1_1(-1),wideBandPrecoderindi1_2(-1),wideBandPrecoderindi1_3(-1),isValid(false)
//     {}
// };
// 
// struct NRTypeIICQIInfoAPeriodic_S{
//     
//     ivec wbCqiIndexPerCodeWord;
//     Array<ivec> sbCqiIndexPerCodeWord;
//     Array<cmat> sbPrecoder;
//     Array< Array<cmat> > channelPerSubBand;
//     bool isValid; 
//     NRTypeIICQIInfoAPeriodic_S():wbCqiIndexPerCodeWord(ivec(0)),sbCqiIndexPerCodeWord(0),channelPerSubBand(0),sbPrecoder(0),isValid(false)
//     {}
// };


struct CoMPInfo_S{
  
  bool isWithCoMP;
  CompSchemes compScheme;
  int maximumCoMPSetSize;
  int maximumCoMPSets;
  
  CoMPInfo_S(){
    
    isWithCoMP=false;
    compScheme=_COMP_SCHEME_UNINIT_;
    maximumCoMPSets=-1;
    maximumCoMPSetSize=-1;
    
  }
};

void computeCDMsizeAndcount(ivec layersPerUE,int& CDMSize,int& CDMCount,int dmrstype,int maxLength);


typedef std::complex<double> dComplex;

enum UlDlMode_E{
  
  _ULDL_MODE_UNINIT_=-1,
  _ULDL_MODE_DOWNLINK_,
  _ULDL_MODE_UPLINK_,
  _ULDL_MODE_TDD_,
  _ULDL_MODE_FDD_
};

enum TDDConfiguration_E{
  
  _TDD_CONF_UNINIT_=-1,
  _TDD_CONF_0_,
  _TDD_CONF_1_,
  _TDD_CONF_2_,
  _TDD_CONF_3_,
  _TDD_CONF_4_,
  _TDD_CONF_5_,
  _TDD_CONF_6_
  
};

struct FrameConfigInfo_S{
  
  ivec dlFrameIndx;
  int framePeriodicityInMilliSec;
};


enum SpecialSubFrameConfiguration_E{
  
  _SSF_CONF_UNINIT_=-1,
  _SSF_CONF_0_,
  _SSF_CONF_1_,
  _SSF_CONF_2_,
  _SSF_CONF_3_,
  _SSF_CONF_4_,
  _SSF_CONF_5_,
  _SSF_CONF_6_,
  _SSF_CONF_7_,
  _SSF_CONF_8_,
  _SSF_CONF_9_
};

struct SpecialSubFrameConfig_S
  { 
    SpecialSubFrameConfiguration_E SSFConfiguration;
    int nDwPts,nUpPts;
  };

struct SymbolLocation
{
  //Normally it both the params has to be over array, which represents the multislot scheduling.
  ivec indexSymbol;
  int  slotnumber;
  SymbolLocation()
  {
    indexSymbol.set_length(2);
  }
};

struct RBInfo_S{
  
  RSInfo_S rsInfo;
  SymbolLocation symbolLocation;
  imat rsPositionMatrix;
  imat logicalSubFrameStructure;
  Array<cmat> dlPowMask; // To apply the power difference between symbols with and without CRS per UE
  double crsPowBoostInDB; // used to apply power boost in CRS for dlPowMask variable
  
  RBInfo_S(){
    crsPowBoostInDB = 0;
    dlPowMask.set_size(0);
    rsPositionMatrix.set_size(0,0);
    logicalSubFrameStructure.set_size(0,0);
  }
  int  getPortReferenceNumber(int portNumber){
    int indx=find(rsInfo.rsPorts,portNumber);
    if(indx!=-1)
      return(rsInfo.portReferenceNumbers(indx));
    else
    { cout<<"[both:] Unknown port number to getPortReferenceNumber()..."<<endl;abort();}
  }
  ivec  getPortReferenceNumbers(ivec portnumbers){
     ivec indices=find(rsInfo.rsPorts,portnumbers);
     if(find(indices,-1,true).length()==0)
      {
       return(rsInfo.portReferenceNumbers(indices));
     }
       else
    { cout<<"[both:] Unknown port number to getPortReferenceNumbers()..."<<endl;abort();}
  }
  
};

struct TxModeConfig_S{
  
  int nLayers;
  int nCodeWords;
  TransmissionMode_E transmissionMode;
  
  TxModeConfig_S(){
    
    nLayers=-1;
    nCodeWords=-1;
    transmissionMode=_TRANSMISSION_MODE_UNINIT_;
  }
};

struct FallbackConfig_S{
  
  bool isEnabled=false;
  TxModeConfig_S txModeConfig;
};


struct PDCCHInfo_S{
  
  bool NonIdealPDCCHEnabled=false;
  bool isPDCCHAbstractionEnabled=false;
  bool isEPDCCHEnabled=false;
  int ueFractionForEPDCCH=-1; /*! fraction of Users selected for EPDCCH among RRC connected users*/
  
};



enum PowerControlMethod_E {
  
  _POW_CONTROL_UNINIT_=-1,
  _POW_CONTROL_LTE_,
  _POW_CONTROL_TargetSINR_
  
};


struct PowerControlInfo_S{
  
  bool enablePowerBackoffModel; 
  bool enableClosedLoopPowerControl;
  int windowSizeForTPCcmd;
  double fValueForPowerControl;
  PowerControlMethod_E powerControlMethod;
  double targetPowerForLTEPowerControlindBm;
  double pathlossCompensation;
  double windowSizeForNI;	
  double gammaIOT;
  double minimumSINRInDB;
  
  PowerControlInfo_S()
  {
    enablePowerBackoffModel=false;
    enableClosedLoopPowerControl=false;
    windowSizeForTPCcmd=-1.0;
    fValueForPowerControl=-1.0;
    powerControlMethod=_POW_CONTROL_UNINIT_;
    targetPowerForLTEPowerControlindBm=-1.0;
    pathlossCompensation=-1.0;
    windowSizeForNI=-1.0;	
    gammaIOT=-1.0;
    minimumSINRInDB=-1.0;
    
  }
  
};


struct TransmissionConfig_S{
  
  TransmissionMode_E transmissionMode;
  int nPorts;
  int nLayers;
  int nCodeWords;
  int prbBundlingSize;
  string harqType;
  int maximumHarqProcesses;
  int minDelayBetweenSuccessiveTransmissions;
  int maximumReTransmissions;
  RSInfo_S rsInfo;
  FallbackConfig_S fallbackConfig;
  TxModeConfig_S originalConfig;
  ivec codeWordIndicesToConsider;
  double Pa; // Value in dB provided by higher layers to compute downlink power allocated to UE.

  //CCH Info
  CCHInfo_S cchInfo;
  PDCCHInfo_S pdcchInfo;
  
  bool enableVarPowInRB;
  int globalAperiodicCQITrigger;
  DownlinkCQIModeInfo_S dlCQIModeInfo;
  UplinkCQIModeInfo_S ulCQIModeInfo;
   
  PowerControlInfo_S powerControlInfo;
  
  bool isModeAdaptationEnabled;
 
  bool multipleCSIProcessFlag;
  int nCSIProcess;
  TRSConfig_S mTRSConf; 
 
  TransmissionConfig_S(){
    transmissionMode=_TRANSMISSION_MODE_UNINIT_;
    nPorts=-1;
    nLayers=-1;
    nCodeWords=-1;
    harqType="";
    maximumHarqProcesses=-1;
    minDelayBetweenSuccessiveTransmissions=-1;
    maximumReTransmissions=-1;
    codeWordIndicesToConsider.set_length(0);
    Pa = 0;
    enableVarPowInRB = 0;
    globalAperiodicCQITrigger = -1;
    isModeAdaptationEnabled=false;
    multipleCSIProcessFlag = false;
    nCSIProcess=-1;
   }
  
  bool isReady(bool checkRsInfo=true){
    
    bool ready=true;
    if(transmissionMode==_TRANSMISSION_MODE_UNINIT_){ cout<<"transmissionMode uninitialized..."<<endl;ready=false;}
    if(nLayers==-1){ cout<<"nLayers uninitialized..."<<endl;ready=false;}
    if(nCodeWords==-1){ cout<<"nCodeWords uninitialized..."<<endl;ready=false;}
    if(harqType==""){ cout<<"harqType uninitialized..."<<endl;ready=false;}
    if(maximumHarqProcesses==-1){ cout<<"maximumHarqProcesses uninitialized..."<<endl;ready=false;}
    if(minDelayBetweenSuccessiveTransmissions==-1){ cout<<"nackDelay uninitialized..."<<endl;ready=false;}
    if(maximumReTransmissions==-1){ cout<<"maximumReTransmissions uninitialized..."<<endl;ready=false;}
    if(checkRsInfo) if(rsInfo.DeModPorts.length()==0){ cout<<"rsInfo uninitialized..."<<endl;ready=false;}
    return(ready);
  }
  
};

enum DemodulationScheme_E{
  
  _DEMODULATION_SCHEME_UNINIT_=-1,
  _DEMODULATION_SCHEME_ZEROFORCING_,
  _DEMODULATION_SCHEME_MMSE_,
  _DEMODULATION_SCHEME_VBLAST_ZEROFORCING_,
  _DEMODULATION_SCHEME_VBLAST_MMSE_,
  _DEMODULATION_SCHEME_ML_,
  _DEMODULATION_SCHEME_MMSE_MLD_,
  _DEMODULATION_SCHEME_WHITE_MRC_,
  _DEMODULATION_SCHEME_MRC_,
  _DEMODULATION_SCHEME_whitend_MLD_
  
};

struct ReceptionConfig_S{
  
  bool enableLinkAdaptation;
  bool idealChannel;
  bool errModelFlag;
  bool jointDetection;
  bool estimateInterference;
  DemodulationScheme_E demodulationScheme;
  int UEcategory;
  
  //OLLA
  bool isOuterLoopLinkAdaptationEnabled;
  double targetBLER;
  
  bool isModeAdaptationEnabled;
  //Rank
  bool enableRankAdaptation;
  int prbBundlingSize;
  TRSConfig_S mTRSConf; 
  
  PowerControlInfo_S powerControlInfo;
    
  DownlinkCQIModeInfo_S dlCQIModeInfo;
  UplinkCQIModeInfo_S ulCQIModeInfo;
  
  
  bool enableCodebookRestriction=false;
  double trainingDurationInSlots=0;
  
  ReceptionConfig_S(){
    
    enableLinkAdaptation=false;
    idealChannel=true;
    errModelFlag=false;
    jointDetection=false;
    estimateInterference=true;
    demodulationScheme=_DEMODULATION_SCHEME_UNINIT_;
    UEcategory=8;
    isOuterLoopLinkAdaptationEnabled=false;
    targetBLER=0.1;
    isModeAdaptationEnabled =false;
    enableRankAdaptation = false;
    
  }
  
};

struct EqualizerOutput_S{
  
  Array<cvec> equalizedSymbols;
  Array<cvec> equalizedChannel;
  Array<vec> equalizedNoiseVariance;
  Array<vec> postProcessingSinr;
  vec nMSEPerRB;
  double preProcessingSinr;
  cmat avgIntrCov;
  
  EqualizerOutput_S(){
    
    equalizedSymbols.set_size(0);
    equalizedChannel.set_size(0);
    equalizedNoiseVariance.set_length(0);
    postProcessingSinr.set_size(0);
    nMSEPerRB.set_size(0);
    preProcessingSinr=-1.0;
  }
  
  void concatTo(EqualizerOutput_S equOutput){
    
    if(equalizedSymbols.length()==0)
      equalizedSymbols=equOutput.equalizedSymbols;
    else
      concatInArray(equalizedSymbols,equOutput.equalizedSymbols);
    
    if(equalizedChannel.length()==0)
      equalizedChannel=equOutput.equalizedChannel;
    else
      concatInArray(equalizedChannel,equOutput.equalizedChannel);
    if(equalizedNoiseVariance.length()==0)
      equalizedNoiseVariance=equOutput.equalizedNoiseVariance;
    else
      concatInArray(equalizedNoiseVariance,equOutput.equalizedNoiseVariance);
    if(postProcessingSinr.length()==0)
      postProcessingSinr=equOutput.postProcessingSinr;
    else
      concatInArray(postProcessingSinr,equOutput.postProcessingSinr);

    if(nMSEPerRB.length()==0)
      nMSEPerRB=equOutput.nMSEPerRB;
    else
      append(nMSEPerRB,equOutput.nMSEPerRB);
  }
  
  void clear(){
    
    equalizedSymbols.set_size(0);
    equalizedChannel.set_size(0);
    postProcessingSinr.set_size(0);
    equalizedNoiseVariance.set_size(0);
    nMSEPerRB.set_size(0);
  }
  
  EqualizerOutput_S get(ivec indices){
    
    EqualizerOutput_S equOutput;
    equOutput.equalizedChannel=getFromArrayOfVector(equalizedChannel,indices);
    equOutput.equalizedSymbols=getFromArrayOfVector(equalizedSymbols,indices);
    equOutput.postProcessingSinr=getFromArrayOfVector(postProcessingSinr,indices);
    equOutput.equalizedNoiseVariance=getFromArrayOfVector(equalizedNoiseVariance,indices);
    equOutput.nMSEPerRB=nMSEPerRB(indices);
    return(equOutput);
  }
};

struct SubbandCQIInfoHigherLayer_S
{
  Array< ivec > cqiIndexPerCodeWord;	
  ivec subbandIndex;
  ivec subbandPrecoderIndi2;
  SubbandCQIInfoHigherLayer_S():cqiIndexPerCodeWord(Array<ivec>(0)),subbandIndex(ivec(0)),subbandPrecoderIndi2(ivec(0))
  {}
};

struct SubbandCQIInfoUESelected_S{
  
  ivec cqiIndexPerCodeWord;
  ivec subbandIndex; 
  int PMI;
  int subbandPrecIndi2;
  SubbandCQIInfoUESelected_S():cqiIndexPerCodeWord(ivec(0)),subbandIndex(ivec(0)),PMI(-1),subbandPrecIndi2(-1)
  {}
};

struct SubbandCQIInfoBWPart_S{
  Array<ivec> cqiIndexPerCodeWord;	
  ivec subbandIndexPerBWPart;
  ivec PMI;
  ivec subbandPrecoderIndi2;
  bvec isValidPerBWPart; 
  SubbandCQIInfoBWPart_S():cqiIndexPerCodeWord(Array<ivec>(0)),subbandIndexPerBWPart(ivec(0)),PMI(ivec(0)),subbandPrecoderIndi2(ivec(0)),isValidPerBWPart(bvec(0))
  {}
};

struct WidebandCQIInfoPeriodic_S{
  
  bool isValid = false;
  ivec cqiIndexPerCodeWord;
  int pmiIndex;
  int WideBandPrecoderIndi1;
  int WideBandPrecoderIndi2;
  Array<ivec> layerIndicator; //CW<layerIndices>
  
  WidebandCQIInfoPeriodic_S():cqiIndexPerCodeWord(ivec(0)),pmiIndex(-1),WideBandPrecoderIndi1(-1),WideBandPrecoderIndi2(-1), layerIndicator(0)
  {}
};

struct WidebandCQIInfoAPeriodic_S{
  
  ivec cqiIndexPerCodeWord;
  int widebandPMI;
  ivec  subbandPMI;
  ivec subbandPrecIndi2;
  int WidebandPrecoderIndi1;
  int WideBandPrecoderindi2;
  bool isValid; 
  WidebandCQIInfoAPeriodic_S():cqiIndexPerCodeWord(ivec(0)),widebandPMI(-1),subbandPMI(ivec(0)),subbandPrecIndi2(ivec(0)),WidebandPrecoderIndi1(-1),WideBandPrecoderindi2(-1),isValid(false)
  {}
};

struct NRMultipanelCQIInfoAPeriodic_S
{
    ivec wbCqiIndexPerCodeWord;
    Array<ivec> layerIndicationPerCodeWord;
    Array<ivec> sbCqiIndexPerCodeWord;
    Array< Array<cmat> > channelPerSubBand;
    int widebandPMI;
    ivec subbandIndex;
    ivec  subbandPMI;
    ivec subbandPrecIndi2;
    int wideBandPrecoderIndi1_1;
    int wideBandPrecoderIndi1_2;
    int wideBandPrecoderIndi1_3;
    int wideBandPrecoderIndi1_4;
    bool isValid; 
    NRMultipanelCQIInfoAPeriodic_S():wbCqiIndexPerCodeWord(ivec(0)),layerIndicationPerCodeWord(0),channelPerSubBand(0),sbCqiIndexPerCodeWord(0),widebandPMI(-1),subbandIndex(ivec(0)),subbandPMI(ivec(0)),subbandPrecIndi2(ivec(0)),wideBandPrecoderIndi1_1(-1),wideBandPrecoderIndi1_2(-1),wideBandPrecoderIndi1_3(-1),wideBandPrecoderIndi1_4(-1),isValid(false)
    {}
};
struct NRCQIInfoAPeriodic_S{
  
  ivec wbCqiIndexPerCodeWord;
  Array<ivec> layerIndicationPerCodeWord;
  Array<ivec> sbCqiIndexPerCodeWord;
  Array< Array<cmat> > channelPerSubBand;
  int widebandPMI;
  ivec subbandIndex;
  ivec  subbandPMI;
  ivec subbandPrecIndi2;
  int wideBandPrecoderIndi1_1;
  int wideBandPrecoderIndi1_2;
  int wideBandPrecoderIndi1_3;
  bool isValid; 
  NRCQIInfoAPeriodic_S():wbCqiIndexPerCodeWord(ivec(0)),layerIndicationPerCodeWord(0),channelPerSubBand(0),sbCqiIndexPerCodeWord(0),widebandPMI(-1),subbandIndex(ivec(0)),subbandPMI(ivec(0)),subbandPrecIndi2(ivec(0)),wideBandPrecoderIndi1_1(-1),wideBandPrecoderIndi1_2(-1),wideBandPrecoderIndi1_3(-1),isValid(false)
  {}
};


struct Subband_Type2coefficients_OMP_S
{
    cmat orthBeams; 
    Array<Array<cmat>> sortedBeams; //  <layers<polarization<cmat>>
    Array<Array<vec>> amplitudes; // <layers<polarization<Lbeams>>
    Array<Array<vec>> phases; // <layers<polarization<Lbeams>> 
    Array<Array<cvec>> coefficients; // <layers<polarization<Lbeams>>
    ivec i11Indices;  // Over Lbeams
    ivec i12Indices;  // Over Lbeams
    Subband_Type2coefficients_OMP_S()
    {
        orthBeams.set_size(0,0);
        sortedBeams.set_length(0);
        amplitudes.set_length(0);
        phases.set_length(0);
        coefficients.set_length(0);
        i11Indices.set_length(0);
        i12Indices.set_length(0);
    }
};

struct TypeIIPMIIndices
{
    int i11,i12,nLayers;
    ivec i13;
    Array<ivec> i14;
    Array<imat> i21,i22;
    Array<ivec> i23;
    Array<imat> i24,i25;
    int i15;
    ivec i16,i18;
    void init(int rank,NRCodeBookType cbType)
    {
        nLayers = rank;
        if(cbType == _NR_CODEBOOK_TYPE_II_ || cbType == _NR_CODEBOOK_PORT_SELECTION_)
        {
            i13.set_length(nLayers);
            i14.set_length(nLayers);
            i21.set_length(nLayers);
            i22.set_length(nLayers);
             i18.set_length(nLayers);
            i16.set_length(nLayers);
            i23.set_length(nLayers);
            i24.set_length(nLayers);
            i25.set_length(nLayers);
            i15=-1;
        }
        else
        {
            i18.set_length(nLayers);
            i16.set_length(nLayers);
            i23.set_length(nLayers);
            i24.set_length(nLayers);
            i25.set_length(nLayers);
            i15=-1;
        }
    }
    void print(NRCodeBookType cbType,ostream& out=cout)
    {
        out<<"[i11 i12]: ["<<i11<<" "<<i12<<"]"<<endl;
        if(cbType == _NR_CODEBOOK_TYPE_II_ || cbType == _NR_CODEBOOK_PORT_SELECTION_)
        {
            out<<"i13: "<<i13<<endl;
            out<<"i14: "<<i14<<endl;
            out<<"i21: "<<i21<<endl;
            out<<"i22: "<<i22<<endl;
        }
        else
        {
            out<<"i15: "<<i15<<endl;
            out<<"i16: "<<i16<<endl;
            out<<"i18: "<<i18<<endl;
            out<<"i23: "<<i23<<endl;
            out<<"i24: "<<i24<<endl;
            out<<"i25: "<<i25<<endl;
        }
    }
};
struct Type2coefficients_OMP_S
{
    cmat orthBeams;
    Array<Array<Array<cmat>>> sortedBeamsOverSubbands; //  <subbands<layers<polarization<cmat>>>
    Array<Array<Array<vec>>> amplitudeOverSubbands; //  <subbands<layers<polarization<Lbeams>>>
    Array<Array<Array<vec>>> phasesOverSubbands; // <subbands<layers<polarization<Lbeams>>> 
    Array<Array<Array<cvec>>> coefficientsOverSubbands; // <subbands<layers<polarization<Lbeams>>>
    Array<cmat> idealPrecoderperSubband;
    ivec i11Indices;  // Over Lbeams
    ivec i12Indices;  // Over Lbeams
    Type2coefficients_OMP_S()
    {
        orthBeams.set_size(0,0);
        sortedBeamsOverSubbands.set_length(0);
        amplitudeOverSubbands.set_length(0);
        phasesOverSubbands.set_length(0);
        coefficientsOverSubbands.set_length(0);
        i11Indices.set_length(0);
        i12Indices.set_length(0);
        idealPrecoderperSubband.set_length(0);
    }
    Subband_Type2coefficients_OMP_S operator()(const int subBandCnt)
    {
        Subband_Type2coefficients_OMP_S subBandCoef;
        subBandCoef.orthBeams = orthBeams;
        subBandCoef.sortedBeams = sortedBeamsOverSubbands(subBandCnt);   
        subBandCoef.amplitudes = amplitudeOverSubbands(subBandCnt);   
        subBandCoef.phases = phasesOverSubbands(subBandCnt); 
        subBandCoef.coefficients = coefficientsOverSubbands(subBandCnt); 
        subBandCoef.i11Indices = i11Indices;
        subBandCoef.i12Indices = i12Indices;
        return subBandCoef;
    }
    
    void printFirstBeami1value()
    {
        cout << "i11: "<< i11Indices(0)<< " i12: "<< i12Indices(0)<<endl;
    }
};

struct Type2coefficients_N_OMP_S
{
    cmat orthBeams;
    Array<cmat> idealPrecoderperSubband;
    Array<cmat> precoderPerSubband;
    ivec i11Indices;  // Over Lbeams
    ivec i12Indices;  // Over Lbeams
    Array<cmat> WfMatrixPerLayer;
    TypeIIPMIIndices pmiIndices;
    Array<cmat> W2PerLayer;
    Array<mat> sbAmplitudesPerLayer;
    Array<mat> phasePerLayer;
    Array<vec> wbAmplitudesPerLayer;
    Type2coefficients_N_OMP_S()
    {
        orthBeams.set_size(0,0);
        i11Indices.set_length(0);
        i12Indices.set_length(0);
        idealPrecoderperSubband.set_length(0);
        precoderPerSubband.set_length(0);
        WfMatrixPerLayer.set_length(0);
    }
    
    void printFirstBeami1value()
    {
        cout << "i11: "<< i11Indices<< " i12: "<< i12Indices<<endl;
    }
};

struct type1PMI_S
{
    int i11, i12, i13, i14, i2;
    ivec i2Indices;
    type1PMI_S()
    {
        i11 = -1; i12 = -1; i13 = -1; i2 = -1,i14=-1;
    }
    type1PMI_S(const int A, const int B, const int C, const int D,const int E):
    i11(A),i12(B),i13(C),i14(D),i2(E){}
    
    string printValues(){
        stringstream ss;
        ss<<"[i11 i12 i13";
        if(i14!=-1){cout<<" i14";}
        ss<<" i2"<<"] : "<<"["<<i11<<" "<<i12<<" "<<i13;
        if(i14!=-1){ss<<" "<<i14;}
        ss<<" "<<i2<<"]";
        return ss.str();
    }
    
};
struct i1index_S
{
  int i11, i12, i13, i14;
  
  i1index_S()
  {
      i11 = -1; i12 = -1; i13 = -1, i14=-1;
  }
  
  i1index_S(const int A, const int B, const int C,const int D):
  i11(A),i12(B),i13(C),i14(D){}
  
  void printValues(){
      
      cout<<"[i11 i12 i13 ";
      if(i14!=-1){cout<<"i14";}
      cout<<"]"<<"["<<i11<<" "<<i12<<" "<<i13;
      if(i14!=-1){cout<<" "<<i14;}
      cout<<"]";
  }
};

struct Type2PMIIndices
{
  int q1, q2;
  ivec n1, n2;
  Array<ivec> P;
  Array<ivec> C;
  //   ivec Ml;
  Type2PMIIndices()
  {
    q1=-1;q2=-1;
    n1.set_length(0); n2.set_length(0);
    P.set_length(0);
    C.set_length(0);
    //     Ml.set_length(0);
  }
};

//HalfBeams
struct Type1HalfBeamWithi1
{
  int i11,i12,i13; //i1 indices
  int i2Size;
  cmat Halfbeam;
  Type1HalfBeamWithi1()
  {
    i11=-1;i12=-1;i13=-1,i2Size=-1;
    Halfbeam.set_size(0,0);
  }
};

struct Type2coefficients
{
  Array<Array<vec>> finalAmplitude; //[subband][layer][2L]
  Array<Array<ivec>> finalSubBandPhasePerOrthBeamPerPolarization; 
  ivec orthBeamIndices;
  Type2coefficients()
  {
    finalAmplitude.set_size(0);
    finalSubBandPhasePerOrthBeamPerPolarization.set_length(0);
    orthBeamIndices.set_size(0);
  }
};




struct NRTypeIICQIInfoAPeriodic_S{
  
  ivec wbCqiIndexPerCodeWord;
  Array<ivec> sbCqiIndexPerCodeWord;
  Array<cmat> sbPrecoder;
  Array<Subband_Type2coefficients_OMP_S> subBandType2Coeff_OMP;
  Array< Array<cmat> > channelPerSubBand;
  bool isValid; 
  NRTypeIICQIInfoAPeriodic_S():wbCqiIndexPerCodeWord(ivec(0)),sbCqiIndexPerCodeWord(0),sbPrecoder(0),subBandType2Coeff_OMP(0),channelPerSubBand(0),isValid(false)
  {}
};

struct CQIMeasurementConfig_S{
  
  int requestID;
  ivec serverNodeGroup;
  ivec crsPorts;
  ivec cqiPorts;
  ivec cqiPortCountPerServerNode;
  double txSubcarrierPowerIndBm;
  PMIFormatIndicator pmiFormatIndicator;
  CQIFormatIndicator cqiFormatIndicator;
  TypeIMethod typeIMethod;
  TypeIIMethod typeIIMethod;
  Array<ivec> rsSymPositions;
  ivec rsScPositions;
  // Associated with UL for UL precoder extraction
  bool isAssociatedWithUL;
   //reciprocity
  double noiseVar;
  Array<cmat> intrCov;
  
  int nuID;
  int numberOfLayers;// RANK
  int numberOfCodeWords;// generally number of codewords will be 2 if RANK>1 else 1
  int riRestriction;
  TransmissionScheme_E transmissionScheme;
  Waveform_E waveform; //used only in uplink (downlink assumes only OFDMA)
  
  // Measurement Region
  int startRB;
  int nRBs;
  
  CompSchemes compScheme;
  int CSIProcessId;
  int CSIConfiguration;
  
  bool isCodebookRestrictionEnabled=false;
  int trainingDurationInSlots;
  
  bool isFeedbackAvailable=false;
  bool isPMIHistoryCollected=false;
  
  NRCQIInfoAPeriodic_S previousFeedback;
  Array<type1PMI_S> wbPMIHistory;
  int i11Range=-1;
  int i12Range=-1;
  int i13Range=-1;
  
  
  CQIMeasurementConfig_S():requestID(-1),serverNodeGroup(ivec(0)),crsPorts(ivec(0)),cqiPorts(ivec(0)),cqiPortCountPerServerNode(ivec(0)), isAssociatedWithUL(0),
  nuID(-1),startRB(-1),nRBs(-1),numberOfLayers(-1),numberOfCodeWords(-1),riRestriction(-1),transmissionScheme(_TRANSMISSION_SCHEME_UNINIT_),waveform(_Waveform_SCFDMA_),compScheme(_COMP_SCHEME_UNINIT_),
  CSIProcessId(-1), CSIConfiguration(-1), rsScPositions(ivec(0)),txSubcarrierPowerIndBm(-500),pmiFormatIndicator(_UNINIT_PMI_),cqiFormatIndicator(_UNINIT_CQI_),typeIMethod(_UNINIT_TYPEI),typeIIMethod(_UNINIT_TYPEII)
  {}
  
};


struct CQIComputationConfig_S{
  
  bool isInitialized;
  bool isChanged; 
  Array<CQIMeasurementConfig_S> cqiMeasurementConfig;
  DownlinkCQIModeInfo_S dlCQIModeInfo;
  bool enableCQI0;
  bool isRankAdaptationEnabled;
  bool isCodebookRestrictionEnabled;
  int trainingDurationInSlots;
  
   /// Will be set by Higher Layers
  bool isAPeriodicCQIConfigured;
  bool releaseAPeriodicCQIConfiguration; 
  bool isAPeriodicCQITriggered;
  bool isPeriodicConfigured;
  bool releasePeriodicCQIConfiguration;
  
  
  CQIComputationConfig_S():isInitialized(false)
  ,isChanged(false),enableCQI0(false),isRankAdaptationEnabled(false),isAPeriodicCQIConfigured(false),releaseAPeriodicCQIConfiguration(false),isAPeriodicCQITriggered(false),isPeriodicConfigured(false),
  releasePeriodicCQIConfiguration(false),isCodebookRestrictionEnabled(false)
  { }
  
  int getMeasurementIndx(int requestID){
    
    int desiredIndx=-1;
    for(int cnt=0;cnt<cqiMeasurementConfig.length();cnt++)
      if(cqiMeasurementConfig(cnt).requestID==requestID)
	desiredIndx=cnt;
      
      if(desiredIndx!=-1)
	return(desiredIndx);
      else
      {
	cout<<"Measurement-Config not available for the requestID. Please create one before calling getMeasurementIndx()."<<endl;
	return(-1);
      }
  }
  
  void init(int nuID,int nCodeWords,int nLayers,double txSubcarrierPowerIndBm,bool enableRankAdaptation,bool enableCodebookRestriction, int trainingTimeInSlots, TransmissionScheme_E transmissionscheme,Waveform_E waveform,int associatedServerNode, RSInfo_S &rsInfo,bool enableCQI0Feedback=false,DownlinkCQIModeInfo_S dlCQImodeInfo=DownlinkCQIModeInfo_S()){
    
    isInitialized=true;
    cqiMeasurementConfig.set_length(1);
    cqiMeasurementConfig(0).nuID=nuID;
    cqiMeasurementConfig(0).requestID=randi(1,10000); // assigning a random request ID
    cqiMeasurementConfig(0).serverNodeGroup=to_ivec(associatedServerNode);
    cqiMeasurementConfig(0).crsPorts=rsInfo.crsPorts;
    cqiMeasurementConfig(0).cqiPorts=rsInfo.CQIports;
    
    cqiMeasurementConfig(0).rsScPositions=rsInfo.rsScPosition(find(rsInfo.rsPorts,rsInfo.CQIports));
    cqiMeasurementConfig(0).rsSymPositions=getFromArray(rsInfo.rsSymPosition,find(rsInfo.rsPorts,rsInfo.CQIports));
    
    cqiMeasurementConfig(0).txSubcarrierPowerIndBm=txSubcarrierPowerIndBm;
    
    cqiMeasurementConfig(0).cqiPortCountPerServerNode.set_size(1);
    cqiMeasurementConfig(0).cqiPortCountPerServerNode=rsInfo.CQIports.length();
    cqiMeasurementConfig(0).numberOfCodeWords=nCodeWords;
    cqiMeasurementConfig(0).numberOfLayers=nLayers;
    cqiMeasurementConfig(0).transmissionScheme=transmissionscheme;
    cqiMeasurementConfig(0).waveform=waveform;
    cqiMeasurementConfig(0).pmiFormatIndicator = dlCQImodeInfo.pmiFormatIndicator;
    cqiMeasurementConfig(0).cqiFormatIndicator = dlCQImodeInfo.cqiFormatIndicator;
    cqiMeasurementConfig(0).typeIMethod= dlCQImodeInfo.typeIMethod;
    cqiMeasurementConfig(0).typeIIMethod= dlCQImodeInfo.typeIIMethod;
    cqiMeasurementConfig(0).riRestriction = dlCQImodeInfo.riRestriction;
    
    cqiMeasurementConfig(0).trainingDurationInSlots=trainingTimeInSlots;
    cqiMeasurementConfig(0).isCodebookRestrictionEnabled=enableCodebookRestriction;
    
    dlCQIModeInfo=dlCQImodeInfo;
    enableCQI0=enableCQI0Feedback;
    isRankAdaptationEnabled=enableRankAdaptation;
    
    if(dlCQImodeInfo.aperiodicMode==_APERIODIC_MODE_UNINIT_)
    {
      isAPeriodicCQIConfigured=false;
      releaseAPeriodicCQIConfiguration=false;
      isAPeriodicCQITriggered=false;
    }
    else
    {
      isAPeriodicCQIConfigured=true;
      releaseAPeriodicCQIConfiguration=false;
      isAPeriodicCQITriggered=false;
    }
    if(dlCQImodeInfo.periodicMode==_PERIODIC_MODE_UNINIT_)
    {
      isPeriodicConfigured=false;
      releasePeriodicCQIConfiguration=false;
    }
    else
    {
      isPeriodicConfigured=true;
      releasePeriodicCQIConfiguration=false;
    }
  }
  
};

struct CQIComputationInfo_S{
  
  Array<CQIMeasurementConfig_S> cqiMeasurementConfig;
  bool enableCQI0;
  bool isRankAdaptationEnabled;
  
  /// Will be set by Higher Layers
  bool isAPeriodicCQIConfigured;// true if aperiodic CQI configured
  bool releaseAPeriodicCQIConfiguration;// if true Aperiodic mode has to be reset to unitialized
  bool isAPeriodicCQITriggered;// true if aperiodic CQI is triggered in current subframe
  bool isPeriodicConfigured;// true if periodic CQI configured
  bool releasePeriodicCQIConfiguration;// if true periodic mode has to be reset to unitialized
  int CQIPMIPeriodicity;// the periodicity with which CQI/PMI has to be reported
  int CQIReportingDelay;
  int RIPeriodicity;// the (RIPeriodicity*widebandperiodicity) periodicity with which RI has to be reported
  int CQIPMIOffset;// the subframe offset where CQI/PMI has to be reported
  int CQIProcessingDelay;
  int RIOffset;// the subframe offset(wrt wideband CQI) where RI has to be reported
  int BWPartCycles;// the number of cycles subbandCQI has to be reported between two widebandCQI reports
  int numOfCSIRSPorts;// the number of CSI-RS ports configured
  int CSIRSPeriodicity;// the periodicty and offset of CSI-RS ports (subframe config)
  int CSIRSConfig;// the RS position config
  int FirstWideBandPMIPeriod;//! Periodicity with which Wideband PMI index i1 to be reported (For 8 antenna ports case only)
  enum AperiodicReportingMode_E aperiodicMode;// configured aperiodicMode
  enum PeriodicReportingMode_E periodicMode;// configured periodicMode
  enum ReportingModeFor_1_1_E submode;//! To be configured for periodicMode 1_1 for the case of 8 ports
  
  /// For CQI computation management
  bool isRankComputed;
  int latestAperiodicCQITriggeredSubframe;
  ivec subbandSizeAperiodic;// indicates the number of subands and subdband size in _APERIODIC_MODE_3_0_/_3_1_
  ivec subbandSizeAperiodicUESelected;// indicates the number of subands and subdband size in _APERIODIC_MODE_2_2_
  Array<ivec> subbandSizePeriodic;// indicates the number of subands per BWPart and their sizes in _PERIODIC_MODE_2_0_/_2_1 (Array over BWParts)
  int numberOfUESelectedSubbands;// indicates the number of preferred subands in _APERIODIC_MODE_2_2_
  int numberOfBWParts;// indicates the number of BWParts for the current Bandwidth configured
  bool isWidebandTrigger;// if true wideband CQI has to be computed in current subframe
  bool isWidebandi1Trigger;// if true wideband PMI index i1 to be determined in current subframe
  int i1diffcounter;
  bool isrankTrigger;// if true RANK has to be computed in current subframe
  bool isBWPartTrigger;// if true BWPart CQI has to be computed in current subframe
  int BWPartNumberTrigger;// tells about which BWPart CQI has to be computed in current subframe
  
  CQIComputationInfo_S():cqiMeasurementConfig(Array<CQIMeasurementConfig_S>(0))
  ,enableCQI0(false),isRankAdaptationEnabled(false),isAPeriodicCQIConfigured(false),releaseAPeriodicCQIConfiguration(false),isAPeriodicCQITriggered(false),isPeriodicConfigured(false),
  releasePeriodicCQIConfiguration(false),CQIPMIPeriodicity(-1),RIPeriodicity(-1),CQIPMIOffset(-1),RIOffset(-1),BWPartCycles(-1),numOfCSIRSPorts(-1),
  CSIRSPeriodicity(-1),CSIRSConfig(-1),aperiodicMode(_APERIODIC_MODE_UNINIT_),periodicMode(_PERIODIC_MODE_UNINIT_),submode(_REPORTING_MODE_UNIT_),
  isRankComputed(false), CQIProcessingDelay(-1),FirstWideBandPMIPeriod(-1),
  latestAperiodicCQITriggeredSubframe(-1),subbandSizeAperiodic(ivec(0)),subbandSizeAperiodicUESelected(ivec(0)),subbandSizePeriodic(Array<ivec>(0)),
  numberOfUESelectedSubbands(-1),numberOfBWParts(-1),isWidebandTrigger(false),isWidebandi1Trigger(false),i1diffcounter(-1),isrankTrigger(false),isBWPartTrigger(false),BWPartNumberTrigger(0)
  { }
  
  void init(CQIComputationConfig_S cqiConfig)
  {
    CQIComputationInfo_S();
    cqiMeasurementConfig=cqiConfig.cqiMeasurementConfig;
    enableCQI0=cqiConfig.enableCQI0;
    isRankAdaptationEnabled=cqiConfig.isRankAdaptationEnabled;
    
    isAPeriodicCQIConfigured=cqiConfig.isAPeriodicCQIConfigured;
    releaseAPeriodicCQIConfiguration=cqiConfig.releaseAPeriodicCQIConfiguration;
    isAPeriodicCQITriggered=cqiConfig.isAPeriodicCQITriggered;
    isPeriodicConfigured=cqiConfig.isPeriodicConfigured;
    releasePeriodicCQIConfiguration=cqiConfig.releasePeriodicCQIConfiguration;
    CQIPMIPeriodicity=cqiConfig.dlCQIModeInfo.CQIPMIPeriodicity;
    CQIReportingDelay=cqiConfig.dlCQIModeInfo.CQIReportingDelay;
    RIPeriodicity=cqiConfig.dlCQIModeInfo.RIPeriodicity;
    CQIPMIOffset=cqiConfig.dlCQIModeInfo.CQIPMIOffset;
    CQIProcessingDelay=cqiConfig.dlCQIModeInfo.CQIProcessingDelay;
    RIOffset=cqiConfig.dlCQIModeInfo.RIOffset;
    BWPartCycles=cqiConfig.dlCQIModeInfo.BWPartCycles;
    numOfCSIRSPorts=cqiConfig.dlCQIModeInfo.numOfCSIRSPorts;
    CSIRSPeriodicity=cqiConfig.dlCQIModeInfo.CSIRSPeriodicity;
    CSIRSConfig=cqiConfig.dlCQIModeInfo.CSIRSConfig;
    FirstWideBandPMIPeriod=cqiConfig.dlCQIModeInfo.FirstWideBandPMIPeriod;
    aperiodicMode=cqiConfig.dlCQIModeInfo.aperiodicMode;
    periodicMode=cqiConfig.dlCQIModeInfo.periodicMode;
    submode=cqiConfig.dlCQIModeInfo.submode;
  }
  
  
  int getMeasurementIndx(int requestID){
    
    int desiredIndx=-1;
    for(int cnt=0;cnt<cqiMeasurementConfig.length();cnt++)
      if(cqiMeasurementConfig(cnt).requestID==requestID)
	desiredIndx=cnt;
      
      if(desiredIndx!=-1)
	return(desiredIndx);
      else
      {
	cout<<"Measurement-Config not available for the requestID. Please create one before calling getMeasurementIndx()."<<endl;
	return(-1);
      }
  }
  
  bool isToTransmitCQIPorts(int slotNumber)
  {
    int cqiPMIOffset;
    
    if(CQIPMIOffset>=(CQIReportingDelay+CQIProcessingDelay))
      cqiPMIOffset=CQIPMIOffset;
    else  
      cqiPMIOffset=CQIPMIOffset+CQIPMIPeriodicity;
    
    if((slotNumber%CQIPMIPeriodicity)==(cqiPMIOffset-(CQIReportingDelay+CQIProcessingDelay)))
      return true;
    else
      return false;
  }
};

struct DLCQIMeasurement_S{
  
  bool isValid;
  int requestID;
  int rankAperiodic;// latest RANK reported as part of aperiodic trigger
  int rankPeriodic;// latest RANK reported as part of periodic configuartion
  WidebandCQIInfoAPeriodic_S widebandCqiInfoPerGroupAperiodic;// wideband CQI corresponding to _APERIODIC_MODE_1_2_/_3_0_/_3_1/_2_2_
  bvec widebandCQIPDUPerGroupPeriodic;
  WidebandCQIInfoPeriodic_S widebandCqiInfoPerGroupPeriodic;// wideband CQI corresponding to _PERIODIC_MODE_1_0_/_1_1_
  SubbandCQIInfoHigherLayer_S subbandCqiInfoPerGroupHigherLayer;// subband CQI corresponding to _APERIODIC_MODE_3_0_/_3_1_
  SubbandCQIInfoUESelected_S subbandCqiInfoPerGroupUESelected;// subband CQI corresponding to _APERIODIC_MODE_2_2_
  bvec subbandCQIPDUPerGroupBWPart;
  SubbandCQIInfoBWPart_S subbandCQIInfoPerGroupBWPart;// subband CQI corresponding to _PERIODIC_MODE_2_0_/_2_1_
  bvec CQIPDUPerGroupAperiodic;

  // NR Parameters
  NRCQIInfoAPeriodic_S nrCQIInfoAPeriodic;
  NRMultipanelCQIInfoAPeriodic_S nrMPCQIInfoAPeriodic;
  NRTypeIICQIInfoAPeriodic_S nrTypeIICQIInfoAPeriodic;
 

  DLCQIMeasurement_S(): isValid(false), requestID(-1), rankAperiodic(-1), rankPeriodic(-1){}
};

struct PerUserDLCQIFeedback_S{
  
  bool isValid;// indicates if atleast wideband CQI is reported
  int serviceNodeID;
  int APeriodicReportedSubframe;// indicates which subframe the latest aperiodic CQI was reported
  bool isWideBandFeedback;// does current report is wideband/subband
  bool isRankFeedback;//CQI1 whether current feedback  is rank feedback
  int currentBWPart;// which BWPart the current report has
  bool isWideBandi1Feedback;
  bool isAperiodicFeedback;
  int PTI;


  enum AperiodicReportingMode_E aPeriodicReportingMode;// configured aperiodicMode
  enum PeriodicReportingMode_E periodicReportingMode;// configured periodicMode
  enum ReportingModeFor_1_1_E submode;
  
  //Array over different CoMP sets
  Array<CQIMeasurementConfig_S> cqiMeasurementConfig;
  Array<DLCQIMeasurement_S> dlCQIMeasurement;

 /// all the below arrays are becoz of CoMP they correspond to diff servernode Groups
  
  PerUserDLCQIFeedback_S():isValid(false),isAperiodicFeedback(false),serviceNodeID(-1),APeriodicReportedSubframe(-1),isRankFeedback(false),isWideBandi1Feedback(false),PTI(-1), 
  aPeriodicReportingMode(AperiodicReportingMode_E(-1)),periodicReportingMode(PeriodicReportingMode_E(-1)),submode(ReportingModeFor_1_1_E(-1)),cqiMeasurementConfig(Array<CQIMeasurementConfig_S>(0)),dlCQIMeasurement( Array<DLCQIMeasurement_S>(0))
  { }
  
  void init(CQIComputationConfig_S feedbackConfig,int servicenodeID)
  {
    serviceNodeID=servicenodeID;
    cqiMeasurementConfig=feedbackConfig.cqiMeasurementConfig;
    aPeriodicReportingMode=feedbackConfig.dlCQIModeInfo.aperiodicMode;
    periodicReportingMode=feedbackConfig.dlCQIModeInfo.periodicMode;
    dlCQIMeasurement.set_length(cqiMeasurementConfig.length());
   
    for(int cqi_cnt=0;cqi_cnt<cqiMeasurementConfig.length();cqi_cnt++)
      dlCQIMeasurement(cqi_cnt).requestID=cqiMeasurementConfig(cqi_cnt).requestID;
    
  }
  void addMeasurement(CQIMeasurementConfig_S &measurementConfig, DLCQIMeasurement_S cqiMeasurement=DLCQIMeasurement_S()){

    for(int cnt=0;cnt<cqiMeasurementConfig.length();cnt++)
      if(cqiMeasurementConfig(cnt).requestID==measurementConfig.requestID)
      {
	cout<<"MeasurementConfig with the same reques ID exists. Skipping addMeasurement()."<<endl;
	return;
      }

      append(cqiMeasurementConfig,measurementConfig);
    cqiMeasurement.requestID=measurementConfig.requestID;
    append(dlCQIMeasurement,cqiMeasurement);

  }
  int getMeasurementIndx(int requestID){
    
    int desiredIndx=-1;
    for(int cnt=0;cnt<cqiMeasurementConfig.length();cnt++)
      if(cqiMeasurementConfig(cnt).requestID==requestID)
	desiredIndx=cnt;
      
      if(desiredIndx!=-1)
      {
	if(dlCQIMeasurement(desiredIndx).requestID==requestID)
	{
	  return(desiredIndx);
	}
	else
	{
	  cout<<"Measurement not available for the requestID. Please create one before calling getMeasurementIndx()."<<endl;
	  return(-1);
	}
      }
      else
      {
	cout<<"Measurement-Config not available for the requestID. Please create one before calling getMeasurementIndx()."<<endl;
	return(-1);
      }
  }
};



struct SubbandULCQIInfo_S{
  
  int requestID;
  Array< vec > effSINRPerCodeWord;	
  ivec subbandIndexPerRB;
  ivec subbandLogicalRBIndex;
  ivec vrbIndex;
  ivec pmiIndex;
  Array< Array<cmat> > channelPerSubBand;
  vec specEff;
  Array<ivec> layerIndices;
  Array<cmat> reciproPrecoderPerRB;
  int rank;
  Array<Array<ivec>> cqiIndxPerRBForDLReci;//reciprocity
  Array<Array<ivec>> cqiIndxPerRB;//reciprocity
  int rankForDLReci;//reciprocity
  
  SubbandULCQIInfo_S(){
    
    requestID=-1;
    effSINRPerCodeWord.set_length(0);
    subbandIndexPerRB.set_length(0);
    subbandLogicalRBIndex.set_length(0);
    vrbIndex.set_length(0);
    pmiIndex.set_length(0);
    layerIndices.set_length(0);
    reciproPrecoderPerRB.set_length(0);
    rank=-1;
    cqiIndxPerRBForDLReci.set_length(0);
    cqiIndxPerRB.set_length(0);
    rankForDLReci=-1;
    channelPerSubBand.set_length(0);
  }
};

struct ReciprocityPrecodersInfo_S{
  
  int requestID;
  ivec vrbIndex;
  Array<cmat> precoderPerRBG; // size is determined by the rank
  Array<cmat> precoderPerRB;
    
  ReciprocityPrecodersInfo_S(){
    
    requestID=-1;
    vrbIndex.set_length(0);
    precoderPerRBG.set_length(0);
    precoderPerRB.set_length(0);
  }
};

struct WidebandULCQIInfo_S{
  
  vec effSINRPerCodeWord;
  int pmiIndex;
  bool isValid;
  
  WidebandULCQIInfo_S(){
    
    effSINRPerCodeWord.set_length(0);
    pmiIndex = -1;
    isValid = false;
  }
  
};

struct PerUserULCQIFeedback_S{
  
  int serviceNodeID;
  bool isValid;
  
  //Array over different CoMP sets
  Array<CQIMeasurementConfig_S> cqiMeasurementConfig;
  Array<SubbandULCQIInfo_S> subbandCqiInfoPerGroup;	//! Group corresponds to Comp-Set. If there is no Comp, then this is of size '0'.
  
  PerUserULCQIFeedback_S(){
    
    serviceNodeID = -1;
    isValid = false;
    cqiMeasurementConfig.set_length(0);
    subbandCqiInfoPerGroup.set_length(0);
  }
  void init(CQIComputationConfig_S feedbackConfig,int servicenodeID)
  {
    serviceNodeID=servicenodeID;
    cqiMeasurementConfig=feedbackConfig.cqiMeasurementConfig;
    subbandCqiInfoPerGroup.set_length(cqiMeasurementConfig.length());
    for(int cqi_cnt=0;cqi_cnt<cqiMeasurementConfig.length();cqi_cnt++)
      subbandCqiInfoPerGroup(cqi_cnt).requestID=cqiMeasurementConfig(cqi_cnt).requestID;
  }
  void update(PerUserULCQIFeedback_S &cqiInfo);

  int getMeasurementIndx(int requestID){
    
    int desiredIndx=-1;
    for(int cnt=0;cnt<cqiMeasurementConfig.length();cnt++)
      if(cqiMeasurementConfig(cnt).requestID==requestID)
	desiredIndx=cnt;
      
      if(desiredIndx!=-1)
      {
	if(subbandCqiInfoPerGroup(desiredIndx).requestID==requestID)
	{
	  return(desiredIndx);
	}
	else
	{
	  cout<<"Measurement not available for the requestID. Please create one before calling getMeasurementIndx()."<<endl;
	  return(-1);
	}
      }
      else
      {
	cout<<"Measurement-Config not available for the requestID. Please create one before calling getMeasurementIndx()."<<endl;
	return(-1);
      }
  }
  
};

/*
struct CQIInfo_S{
  
  ivec serviceNodeIDs;
  
  int dlCqiProcessingDelay;
  int latestsubframeNumberForDLCQIUpdateInScheduler;
  ivec dlSubframeNumberForCQIPerServiceNode;
  ivec dlSubframeNumberForRIPerServiceNode;
  Array<PerUserDLCQIFeedback_S> dlCqiInfoPerServiceNode;
  Array<PerUserDLCQIFeedback_S> dlCqiInfoPerServiceNodeForScheduler;
  
  int ulCqiProcessingDelay;
  int latestsubframeNumberForULCQIUpdateInScheduler;
  Array< queue<int> > ulSubframeNumberPerServiceNode;
  Array< queue<PerUserULCQIFeedback_S> > ulCqiInfoPerServiceNode;
  Array<PerUserULCQIFeedback_S> ulCqiInfoPerServiceNodeForScheduler;
  
  
  CQIInfo_S(){
    
    dlCqiProcessingDelay=-1;
    ulCqiProcessingDelay=-1;
    latestsubframeNumberForDLCQIUpdateInScheduler=-1;
    latestsubframeNumberForULCQIUpdateInScheduler=-1;
    serviceNodeIDs.set_length(0);
    dlCqiInfoPerServiceNode.set_length(0);
    dlCqiInfoPerServiceNodeForScheduler.set_length(0);
    ulCqiInfoPerServiceNode.set_length(0);
    ulCqiInfoPerServiceNodeForScheduler.set_length(0);
  }
  void init(ivec &serviceNodes, int dlCQIDelay, int ulCQIDelay){
    
    dlCqiProcessingDelay=dlCQIDelay;
    ulCqiProcessingDelay=ulCQIDelay;
    serviceNodeIDs=serviceNodes;
    latestsubframeNumberForDLCQIUpdateInScheduler=-1;
    latestsubframeNumberForULCQIUpdateInScheduler=-1;
    dlSubframeNumberForCQIPerServiceNode=zeros_i(serviceNodes.length())-1;
    dlSubframeNumberForRIPerServiceNode=zeros_i(serviceNodes.length())-1;
    ulSubframeNumberPerServiceNode.set_length(serviceNodes.length());
    dlCqiInfoPerServiceNode.set_length(serviceNodes.length());
    dlCqiInfoPerServiceNodeForScheduler.set_length(serviceNodes.length());
    ulCqiInfoPerServiceNode.set_length(serviceNodes.length());
    ulCqiInfoPerServiceNodeForScheduler.set_length(serviceNodes.length());
    
  }
  void update(int serviceNodeID,PerUserDLCQIFeedback_S cqiInfo,int subframeNumber,bool isRIfeedback,bool rankAdaptation,bool isAperiodicConfigured,bool bothReport)
  {
    int measurementIndx=0;
    if(traceValuePerTraceName["printCQIReported"]=="true")
    {
      if(!isAperiodicConfigured)
      {
	if(!isRIfeedback)
	{      
	  cout<<"cqiInfo.isValid = "<<cqiInfo.isValid <<"  cqiInfo.serviceNodeID = "<<cqiInfo.serviceNodeID <<"  cqiInfo.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord = "<<cqiInfo.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord
	  <<"  cqiInfo.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord = "<<cqiInfo.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord<<"  cqiInfo_subframeNumber = "<<subframeNumber<<endl;
	}
	else
	{     
	  cout<<"cqiInfo.isValid = "<<cqiInfo.isValid <<"  cqiInfo.serviceNodeID = "<<cqiInfo.serviceNodeID <<"  cqiInfo_subframeNumber = "<<subframeNumber<<" cqiInfo_PeriodicRank = "<<cqiInfo.dlCQIMeasurement(measurementIndx).rankPeriodic<<endl;
	}
      }
      
    }
    
    if(rankAdaptation && (!isAperiodicConfigured) && (!bothReport))
    {
      //cout<<"RA on"<<endl;
      int indx=find(serviceNodeIDs,serviceNodeID);
      if(indx!=-1)
      {  
	if(dlCqiInfoPerServiceNode(indx).dlCQIMeasurement.length()==0)
	{
	  dlCqiInfoPerServiceNode(indx).dlCQIMeasurement.set_length(1);
	  dlCqiInfoPerServiceNode(indx).cqiMeasurementConfig=cqiInfo.cqiMeasurementConfig;
	}
	if(isRIfeedback)	
	{
	  
	  dlCqiInfoPerServiceNode(indx).dlCQIMeasurement(measurementIndx).rankPeriodic=cqiInfo.dlCQIMeasurement(measurementIndx).rankPeriodic;
	  dlCqiInfoPerServiceNode(indx).isRankFeedback=cqiInfo.isRankFeedback;
	  dlCqiInfoPerServiceNode(indx).serviceNodeID=cqiInfo.serviceNodeID;
	  dlCqiInfoPerServiceNode(indx).periodicReportingMode=cqiInfo.periodicReportingMode;
	  dlSubframeNumberForRIPerServiceNode(indx)=subframeNumber;	  	
	}
	else
	{
	  int rankPeriodic=dlCqiInfoPerServiceNode(indx).dlCQIMeasurement(measurementIndx).rankPeriodic;
	  dlCqiInfoPerServiceNode(indx)=cqiInfo;
	  dlCqiInfoPerServiceNode(indx).dlCQIMeasurement(measurementIndx).rankPeriodic=rankPeriodic;
	  dlSubframeNumberForCQIPerServiceNode(indx)=subframeNumber;
	}
	
	
      }
      else
      {	
	cout<<"first add"<<endl;
	append(serviceNodeIDs,serviceNodeID);
	append(dlCqiInfoPerServiceNode,cqiInfo);
	if(isRIfeedback)
	  append(dlSubframeNumberForRIPerServiceNode,subframeNumber);
	else	
	  append(dlSubframeNumberForCQIPerServiceNode,subframeNumber);	
      }
    }
    else
    {
      int indx=find(serviceNodeIDs,serviceNodeID);
      if(indx!=-1)
      {     
	dlCqiInfoPerServiceNode(indx)=cqiInfo;
	dlSubframeNumberForCQIPerServiceNode(indx)=subframeNumber;
      }
      
      else
      {	
	append(serviceNodeIDs,serviceNodeID);
	append(dlCqiInfoPerServiceNode,cqiInfo);
	append(dlSubframeNumberForCQIPerServiceNode,subframeNumber);
	
      }
      if(!isAperiodicConfigured && bothReport)
      {
	if(indx!=-1)
	  dlSubframeNumberForRIPerServiceNode(indx)=subframeNumber;
	else	    
	  append(dlSubframeNumberForRIPerServiceNode,subframeNumber);
      }
    }
    
  }
  void update(int serviceNodeID,PerUserULCQIFeedback_S cqiInfo,int subframeNumber){
    
    if(traceValuePerTraceName["printCQIReported"]=="true")
      cout<<"cqiInfo.isValid = "<<cqiInfo.isValid <<"  cqiInfo.serviceNodeID = "<<cqiInfo.serviceNodeID
      <<"  cqiInfo.subbandCqiInfoPerGroup(0).effSINRPerCodeWord = "<<cqiInfo.subbandCqiInfoPerGroup(0).effSINRPerCodeWord<<"  cqiInfo_subframeNumber = "<<subframeNumber<<endl;
    
    int indx=find(serviceNodeIDs,serviceNodeID);
    if(indx!=-1)
    {
      PerUserULCQIFeedback_S ulCqiInfoPerServiceNodetemp;
      if(!ulCqiInfoPerServiceNode(indx).empty())
	ulCqiInfoPerServiceNodetemp = ulCqiInfoPerServiceNode(indx).front();
      ulCqiInfoPerServiceNodetemp.update(cqiInfo);
      ulCqiInfoPerServiceNode(indx).push(ulCqiInfoPerServiceNodetemp);
      ulSubframeNumberPerServiceNode(indx).push(subframeNumber);
    }
    else
    {
      queue<PerUserULCQIFeedback_S> ulCqiInfoPerServiceNodetemp;
      ulCqiInfoPerServiceNodetemp.push(cqiInfo);
      append(serviceNodeIDs,serviceNodeID);
      append(ulCqiInfoPerServiceNode,ulCqiInfoPerServiceNodetemp);
      queue<int> ulSubframeNumberPerServiceNodetemp;
      ulSubframeNumberPerServiceNodetemp.push(subframeNumber);
      append(ulSubframeNumberPerServiceNode,ulSubframeNumberPerServiceNodetemp);
    }
  }
  void updateCQIForScheduler(int subframeNumber, ivec serviceNodes, bool isDownlink){
    
    if(isDownlink)
    {
      if(dlCqiProcessingDelay==-1)
      {
	cout<<"Downlink CQI Processing delay not initialized in CQIInfo_S...skipping updateCQIForScheduler()..."<<endl;return;
      }
      bool printMe=true;
      ivec srvcIndx=find(serviceNodeIDs,serviceNodes);
      for(int srvcNode_cnt=0;srvcNode_cnt<serviceNodes.length();srvcNode_cnt++)
      {
	if(dlSubframeNumberForCQIPerServiceNode(srvcIndx(srvcNode_cnt))!=-1 && subframeNumber==(dlSubframeNumberForCQIPerServiceNode(srvcIndx(srvcNode_cnt))+dlCqiProcessingDelay))
	{
	  if(printMe)
	  {
	    if(traceValuePerTraceName["EnablePrints"]=="true")
	      cout<<"Updating Downlink CQI to Scheduler..."<<endl;
	    printMe=false;
	  }
	  
	  dlCqiInfoPerServiceNodeForScheduler(srvcIndx(srvcNode_cnt))=dlCqiInfoPerServiceNode(srvcIndx(srvcNode_cnt));
	  
	  latestsubframeNumberForDLCQIUpdateInScheduler=subframeNumber;
	}
	if(dlSubframeNumberForRIPerServiceNode(srvcIndx(srvcNode_cnt))!=-1 && subframeNumber==(dlSubframeNumberForRIPerServiceNode(srvcIndx(srvcNode_cnt))+dlCqiProcessingDelay))
	{
	  if(printMe)
	  {
	    if(traceValuePerTraceName["EnablePrints"]=="true")
	      cout<<"Updating Downlink RI to Scheduler..."<<endl;
	    printMe=false;
	  }
	  dlCqiInfoPerServiceNodeForScheduler(srvcIndx(srvcNode_cnt))=dlCqiInfoPerServiceNode(srvcIndx(srvcNode_cnt));
	  
	  latestsubframeNumberForDLCQIUpdateInScheduler=subframeNumber;
	}
      }
    }
    else
    {
      if(ulCqiProcessingDelay==-1)
      {
	cout<<"Uplink CQI Processing delay not initialized in CQIInfo_S...skipping updateCQIForScheduler()..."<<endl;return;
      }
      bool printMe=true;
      ivec srvcIndx=find(serviceNodeIDs,serviceNodes);
      for(int srvcNode_cnt=0;srvcNode_cnt<serviceNodes.length();srvcNode_cnt++){
	
	if( !ulSubframeNumberPerServiceNode(srvcIndx(srvcNode_cnt)).empty() && subframeNumber==(ulSubframeNumberPerServiceNode(srvcIndx(srvcNode_cnt)).front() +ulCqiProcessingDelay))
	{
	  if(printMe)
	  {
	    if(traceValuePerTraceName["EnablePrints"]=="true")
	      cout<<"Updating Uplink CQI to Scheduler..."<<endl;
	    printMe=false;
	  }
	  ulCqiInfoPerServiceNodeForScheduler(srvcNode_cnt)=ulCqiInfoPerServiceNode(srvcNode_cnt).front();
	  ulCqiInfoPerServiceNode(srvcNode_cnt).pop();
	  ulSubframeNumberPerServiceNode(srvcNode_cnt).pop();
	  latestsubframeNumberForULCQIUpdateInScheduler=subframeNumber;
	}
      }
    }
  }
  int getLatestNumofLayersInDL(int serviceNodeID, int requestID=-1){
    
    int indx=find(serviceNodeIDs,serviceNodeID);
    if(indx!=-1)
    {
      if(requestID==-1)
      {
	if(dlCqiInfoPerServiceNodeForScheduler(indx).dlCQIMeasurement(0).rankPeriodic!=-1)
	  return(dlCqiInfoPerServiceNodeForScheduler(indx).dlCQIMeasurement(0).rankPeriodic);
	else
	  return(dlCqiInfoPerServiceNodeForScheduler(indx).dlCQIMeasurement(0).rankAperiodic); //! FIXME: need to change when both periodic cqi and aperiodic cqi are computed and reported
      }
      else
      {
	int measurementIndx=dlCqiInfoPerServiceNodeForScheduler(indx).getMeasurementIndx(requestID);
	
	if(dlCqiInfoPerServiceNodeForScheduler(indx).dlCQIMeasurement(measurementIndx).rankPeriodic!=-1)
	  return(dlCqiInfoPerServiceNodeForScheduler(indx).dlCQIMeasurement(measurementIndx).rankPeriodic);
	else
	  return(dlCqiInfoPerServiceNodeForScheduler(indx).dlCQIMeasurement(measurementIndx).rankAperiodic); //! FIXME: need to change when both periodic cqi and aperiodic cqi are computed and reported
	  
      }
    }
    else
    {
      cout<<"[both:] Error ...CQIInfo not initialized for the serviceNodeID "<<serviceNodeID<< "  in getLatestNumofLayers()..."<<endl;abort();
    }
  }
  int getLatestNumCodewordsInDL(int serviceNodeID, int requestID=-1){
    
    int nCodewords = -1;
    int indx=find(serviceNodeIDs,serviceNodeID);
    int measurementIndx=0;
    if(requestID!=-1)
      measurementIndx=dlCqiInfoPerServiceNodeForScheduler(indx).getMeasurementIndx(requestID);
    
    if(indx!=-1)
    {
      if(dlCqiInfoPerServiceNodeForScheduler(indx).dlCQIMeasurement(measurementIndx).rankPeriodic!=-1)
      {
	if(dlCqiInfoPerServiceNodeForScheduler(indx).dlCQIMeasurement(measurementIndx).rankPeriodic>1)
	  nCodewords = 2;
	else if(dlCqiInfoPerServiceNodeForScheduler(indx).dlCQIMeasurement(measurementIndx).rankPeriodic==1)
	  nCodewords = 1;
      }
      else if(dlCqiInfoPerServiceNodeForScheduler(indx).dlCQIMeasurement(measurementIndx).rankAperiodic!=-1)
      {
	if(dlCqiInfoPerServiceNodeForScheduler(indx).dlCQIMeasurement(measurementIndx).rankAperiodic>1)
	  nCodewords = 2;
	else if(dlCqiInfoPerServiceNodeForScheduler(indx).dlCQIMeasurement(measurementIndx).rankAperiodic==1)
	  nCodewords = 1;
      }
      else
      {
	cout<<"[both:] Error ... Rank is not set  for the serviceNodeID "<<serviceNodeID<< "  in getLatestNumCodewords()..."<<endl;abort();
      }
    }
    else
    {
      cout<<"[both:] Error ...CQIInfo not initialized for the serviceNodeID "<<serviceNodeID<< "  in getLatestNumCodewords()..."<<endl;abort();
    }
    
    if(nCodewords == -1)
    {
      cout<<"[both:] nCodewords = -1 for the serviceNodeID "<<serviceNodeID<< "  in getLatestNumCodewords()..."<<endl;abort();
    }
    
    return nCodewords;
  }
  PerUserDLCQIFeedback_S getLatestDLCQIInfo(int serviceNodeID){
    
    int indx=find(serviceNodeIDs,serviceNodeID);
    
    if(indx!=-1)
    {
      return(dlCqiInfoPerServiceNodeForScheduler(indx));
    }
    else
    {
      cout<<"[both:] Error ...CQIInfo not initialized for the serviceNodeID "<<serviceNodeID<< "  in getLatestDLCQIInfo()..."<<endl;abort();
    }
  }
  PerUserULCQIFeedback_S getLatestULCQIInfo(int serviceNodeID){
    
    int indx=find(serviceNodeIDs,serviceNodeID);
    if(indx!=-1)
    {
      return(ulCqiInfoPerServiceNodeForScheduler(indx));
    }
    else
    {
      cout<<"[both:] Error ...CQIInfo not initialized for the serviceNodeID "<<serviceNodeID<< "  in getLatestULCQIInfo()..."<<endl;abort();
    }
  }
  void clearPerUserCQI(int serviceNodeID)
  {
    int indx=find(serviceNodeIDs,serviceNodeID);
    if(indx!=-1)
    {
      dlCqiInfoPerServiceNode(indx)=PerUserDLCQIFeedback_S();
      dlCqiInfoPerServiceNodeForScheduler(indx)=PerUserDLCQIFeedback_S();
      queue<PerUserULCQIFeedback_S> ulCqiInfoPerServiceNodetemp;
      ulCqiInfoPerServiceNode(indx)=ulCqiInfoPerServiceNodetemp;
      ulCqiInfoPerServiceNodeForScheduler(indx)=PerUserULCQIFeedback_S();
      
    }
    else
    {
      cout<<"[both:] Error ...unknown ServiceNode "<<serviceNodeID<< "  to clearPerUserCQI()..."<<endl;abort();
    }
  }
};*/

struct UplinkCQIInfo_S{
  
  ivec serviceNodeIDs;
  Array<CQIComputationInfo_S> cqiComputationInfoPerServiceNode;
  Array<PerUserULCQIFeedback_S> perUserULCQIFeedback;
  bvec isToReportCQI;
  
  
  void addServiceNode(CQIComputationConfig_S &cqiConfig,int serviceNodeID){
    
    int indx=find(serviceNodeIDs,serviceNodeID);
    if(indx==-1)
    {
      append(serviceNodeIDs,serviceNodeID);
      
      CQIComputationInfo_S cqiComputationInfo;
      cqiComputationInfo.init(cqiConfig);
      append(cqiComputationInfoPerServiceNode,cqiComputationInfo);
      
      PerUserULCQIFeedback_S perUserFeedback;
      perUserFeedback.init(cqiConfig,serviceNodeID);
      append(perUserULCQIFeedback,perUserFeedback);
      append(isToReportCQI,(bin)0);
      
    }
    
  }
  void updateCQIComputationInfoFromCQIConfig(CQIComputationConfig_S &cqiConfig,int serviceNodeID){
    
    if(cqiConfig.isChanged==true)
    {
      int indx=find(serviceNodeIDs,serviceNodeID);
      if(indx!=-1)
      {
	cqiComputationInfoPerServiceNode(indx).init(cqiConfig);
	cqiConfig.isChanged=false;
      }
    }
  };
};


struct DownlinkCQIInfo_S{
  
  CQIComputationInfo_S cqiComputationInfo;
  PerUserDLCQIFeedback_S perUserFeedback;
  int subframeNumberToReportPeriodicCQI;
  int subframeNumberToReportAperiodicCQI;
  int subframeNumberToReportPeriodicRI;
  int subframeNumberToReportAperiodicRI;
  
  DownlinkCQIInfo_S():subframeNumberToReportPeriodicCQI(-1),
  subframeNumberToReportAperiodicCQI(-1),subframeNumberToReportPeriodicRI(-1),subframeNumberToReportAperiodicRI(-1)
  {}
  
  void updateCQIComputationInfoFromCQIConfig(CQIComputationConfig_S &cqiConfig,int serviceNodeID){
    
    if(cqiConfig.isChanged==true){
      cqiComputationInfo.init(cqiConfig);
      perUserFeedback.init(cqiConfig,serviceNodeID);
       
    }
  };
  void addMeasurement(CQIMeasurementConfig_S &measurementConfig, DLCQIMeasurement_S cqiMeasurement=DLCQIMeasurement_S()){

    for(int cnt=0;cnt<cqiComputationInfo.cqiMeasurementConfig.length();cnt++)
      if(cqiComputationInfo.cqiMeasurementConfig(cnt).requestID==measurementConfig.requestID)
      {
	cout<<"MeasurementConfig with the same reques ID exists. Skipping addMeasurement()."<<endl;
	return;
      }

      append(cqiComputationInfo.cqiMeasurementConfig,measurementConfig);
    perUserFeedback.addMeasurement(measurementConfig,cqiMeasurement);
  }
  void updateServerNodeGroup(int nodeID){
    
    cqiComputationInfo.cqiMeasurementConfig(0).serverNodeGroup=to_ivec(nodeID);
    perUserFeedback.cqiMeasurementConfig(0).serverNodeGroup=to_ivec(nodeID);
  }
  
};

// struct DCIInfo_S{
//   
//   DCI_FORMATS dciFormat;
//   bvec DCIPDU;
//   int cRNTI;
//   int aggLevel;
//   int startCCEIndex;
//   
//   bool isEPDCCH;
//   int 	nEPDCCHSets;
//   ivec 	ecceAllocated;
//   Array<ivec> prbPairsAllocated;
//   ivec epdcchCCEs;
//   ivec mapCCEtoRB;
//   
//   DCIInfo_S(){
//     
//     dciFormat=DCI_FORMAT_UNINIT;
//     DCIPDU.set_length(0);
//     cRNTI=-1;
//     aggLevel=-1;
//     startCCEIndex=-1;
//     epdcchCCEs.set_length(0);
//     mapCCEtoRB.set_length(0);
//     isEPDCCH=false;
//   }
//   
// };

struct DataGenInfo_S{
  
  int nCodeWords; 
  int nLayers;
  LayerInfo_S layersInfo;
  double mimoRate;
  int symbolRepetitionFactor;
  int scheduledPRBs;
  int scheduledSymbols;
  ivec resourceElementsPerRB;
  ivec mcsIndices;
  int cellID;
  TransmissionScheme_E transmissionScheme;
  RBInfo_S rbInfo;
  int rnti;
  int channelCoderID;
//   DCIInfo_S dciInfo;
  
  DataGenInfo_S(){
    
    nCodeWords=-1; //SCW or MCW
    nLayers=-1;
    mimoRate=-1;
    symbolRepetitionFactor=-1;
    scheduledPRBs=-1;
    resourceElementsPerRB.set_length(0);
    mcsIndices.set_length(0);
    cellID=-1;
    transmissionScheme=_TRANSMISSION_SCHEME_UNINIT_;
    rnti=-1;
  }
  bool isReady() const {
    
    bool ready=true;
    
    if(nCodeWords==-1) {cout<<"nCodeWords not assigned..."<<endl; ready=false;} 
    if(nLayers==-1) {cout<<"nLayers not assigned..."<<endl; ready=false;}
    if(mimoRate==-1) {cout<<"mimoRate not assigned..."<<endl; ready=false;}
    if(rnti==-1) {cout<<"rnti not assigned..."<<endl; ready=false;}
    if(symbolRepetitionFactor==-1) {cout<<"symbolRepetitionFactor not assigned..."<<endl; ready=false;}
    if(scheduledPRBs==-1) {cout<<"scheduledPRBs not assigned..."<<endl; ready=false;}
    if(resourceElementsPerRB.length()==0) {cout<<"resourceElementsPerRB not assigned..."<<endl; ready=false;}
    if(cellID==-1) {cout<<"cellID not assigned..."<<endl; ready=false;}
    if(transmissionScheme==_TRANSMISSION_SCHEME_UNINIT_) {cout<<"transmissionScheme not assigned..."<<endl; ready=false;}
    
    return(ready);
  }
};

/*!\brief Following structure is the output of harqManager.
 * \note length of newDataIndicator is also used as number of codeWords in ChannelCoder */
struct HarqInfo
{
  int harqProcessID;
  ivec rvIndicesPerCodeword;
  bvec newDataIndicator;                //! specify NDI per codeWord (toggling of this variable indicate new data transmission.. NDI always start from '0').
  ivec codeWordIndicesToConsider;       //! Indicate the which codeWord has to be transmitted.. (length of this variable is taken as nCodeWords)
  Array<ivec> codeBlockIndicesToConsider; //! Indicate the which codeBlock within codeWord has to be transmitted.
  bvec isNewTransmission; 
  ivec TBSizePerCodewordInBits;
  Array<bvec> infoBitsperCodeWord;
  Array<vec> harqBuffer;
  
  double powerOffsetInDBFromOLLA;
  
  HarqInfo(){
    
    harqProcessID=-1;
    rvIndicesPerCodeword.set_length(0);
    newDataIndicator.set_length(0);
    codeWordIndicesToConsider.set_length(0);
    codeBlockIndicesToConsider.set_length(0);
    isNewTransmission.set_length(0);
    TBSizePerCodewordInBits.set_length(0);
    infoBitsperCodeWord.set_length(0);
    harqBuffer.set_length(0);
    powerOffsetInDBFromOLLA=0;
  }
};


struct DataBlkInfo_S{
 
  Waveform_E waveform;
  
  ivec resourceBlocks;
  ivec resourceBlockGroups;
  ivec symPos;
    
  bool isLocalised;
  bool isGap1; 
  int prbBundlingSize;
  Array<cmat> precoderPerRB;
  DataGenInfo_S dataGenInfo;
  HarqInfo harqInfo;
  Array<cvec> modulatedSymbols;
  Array<ivec> reIndxForPrecoding;
  bool downlinkPowerOffset;	
  bool isClosedLoopScheme;
  bool isSemiOpenLoopScheme;
  
  bool isAbsTPC;
  int uplinkPowerControlCorrectionValInDB;
  double KsForPowerControl;
  int indexOfFactor;
  bool resetTPC;
  int timeByTpc;
  vec SINRdiffForK_Slots; // k is to be set
  int TPCcmdFieldIndividualUE;

  
  CompSchemes compScheme;
  ivec coOperatingNodes;
  bool isDummyDataBlk;
  
  double pa;
  DataBlkInfo_S(){
    
    waveform=_Waveform_UNINIT_;
    
    resourceBlocks.set_size(0);
    resourceBlockGroups.set_length(0);
    symPos.set_length(0);
    
    modulatedSymbols.set_length(0);
    precoderPerRB.set_size(0);
    downlinkPowerOffset=true; 
    isLocalised=true;
    isClosedLoopScheme=false;
    isSemiOpenLoopScheme=false;
    isAbsTPC=true;
    uplinkPowerControlCorrectionValInDB=0;
    KsForPowerControl=0;
    indexOfFactor=1;
    timeByTpc=1;
    
    compScheme=_COMP_SCHEME_UNINIT_;
    coOperatingNodes.set_length(0);
    isDummyDataBlk=false;
    pa=-500;//in dB
    downlinkPowerOffset=0;//default value 0 corresponds to 0dB offset
  }
  cmat getMyPrecoder(int rbNumber){
    
    int rbIndx=find(resourceBlocks,rbNumber);
    if(rbIndx!=-1)
      return(precoderPerRB(rbIndx));
    else
    {
      cout<<"[both:] Error ...Unknown rb to getMyScheduledLayers() in DataBlkInfo_S..."<<endl;
      abort();
    }
  }
};

struct TransmissionModeInfo_S{
  
  TransmissionMode_E transmissionMode;
  int nLayers;
  int nCodewords;
};


enum ABSTRACTION_MODEL
{
  _ABSTRACTION_UNINIT_=-1,
  _ABSTRACTION_DECODER_,
  _ABSTRACTION_CQI_,
  _ABSTRACTION_IDEAL_
  
};


#endif
