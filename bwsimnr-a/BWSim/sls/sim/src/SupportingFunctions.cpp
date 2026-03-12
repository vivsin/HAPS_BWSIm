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

#include "../include/SupportingFunctions.h"





TransmissionConfig_S parseTxConfig(string fileName,string l2fileName, bool isDownlink, bool isSSBEnabled){

  TransmissionConfig_S txConfig;

  string prefix;
  if(isDownlink) prefix="DL_";
  else	prefix="UL_";
  bool temp=false;
  
  if(!parse(fileName,prefix+"enableModeAdaptation",temp))
    cout<<"unable to parse mode adaptation variable"<<endl;
  if(!temp)
    txConfig.transmissionMode=parseTransmissionMode(fileName,isDownlink);
  else
    txConfig.transmissionMode=_TRANSMISSION_MODE_3_;

	
  if(isDownlink)
  {
    int PaIndx;
    vec pamap="-6,-4.77,-3,-1.77,0,1,2,3";//dB values
    parse(fileName,prefix+"Pa",PaIndx);
    txConfig.Pa=pamap(PaIndx);
  }  
  
  if(!parse(fileName,prefix+"nPorts",txConfig.nPorts))
  {
    cout<<"[both:]Unable to load nPorts from the Configuration File - myDLTxRxConfig / myULTxRxConfig."<<endl;
    abort();
  }
  
  if(!parse(fileName,prefix+"prbBundlingSize",txConfig.prbBundlingSize))
  {
    cout<<"[both:]Unable to load prbBundlingSize from the Configuration File - myDLTxRxConfig / myULTxRxConfig."<<endl;
    abort();
  }
  
  if(!parse(fileName,prefix+"nLayers",txConfig.nLayers))
  {
    cout<<"[both:]Unable to load nLayers from the Configuration File - myDLTxRxConfig / myULTxRxConfig."<<endl;
    abort();
  }
  
//   if(!parse(fileName,prefix+"enableRankAdaptation",txConfig.enableRankAdaptation))
//   {
//     cout<<"[both:]Unable to parse enableRankAdaptation from the Configuration File - myDLTxRxConfig / myULTxRxConfig. Disabling RankAdaptation by default.. "<<endl;
//     txConfig.enableRankAdaptation=false;
//   }
// 
//   if(temp)
//   {
//     txConfig.enableRankAdaptation=false;
//   }
  
  if(!parse(fileName,prefix+"nCodeWords",txConfig.nCodeWords))
  {
    cout<<"[both:]Unable to load nCodeWords from the Configuration File - myDLTxRxConfig / myULTxRxConfig."<<endl;
    abort();
  }
  
  if(!parse(l2fileName,"harqType",txConfig.harqType))
  {
    cout<<"[detl:]Unable to load harqType from the Configuration File - L2SimSupport. Loading IR as default for harqType."<<endl;
    txConfig.harqType="IR";
  }
  if(!parse(l2fileName,prefix+"mNumOfHarqProcess",txConfig.maximumHarqProcesses))
  {
    cout<<"[detl:]Unable to load maximumHarqProcesses from the Configuration File - L2SimSupport. Loading 8 as default for maximumHarqProcesses."<<endl; //(Refer: ETSI TS 136 104 V10.6.0 (2012-03) : Table 8.2.1-1 (page:74))..
    txConfig.maximumHarqProcesses=8;
  }
  if(!parse(l2fileName,prefix+"minDelayBetweenSuccessiveTransmissions",txConfig.minDelayBetweenSuccessiveTransmissions))
  {
    cout<<"[detl:]Unable to load minDelayBetweenSuccessiveTransmissions from the Configuration File - L2SimSupport. Loading 8 as default for minDelayBetweenSuccessiveTransmissions."<<endl; //Refer: ETSI TS 136 104 V10.6.0 (2012-03) : Table 8.2.4.1-1 (page:84)
    txConfig.minDelayBetweenSuccessiveTransmissions=8;
  }
  if(!parse(l2fileName,prefix+"mMaxNumOfHarqTransmissions",txConfig.maximumReTransmissions))
  {
    cout<<"[detl:]Unable to load maximumReTransmissions from the Configuration File - L2SimSupport. Loading 3 as default for maximumReTransmissions."<<endl;  //(Refer: ETSI TS 136 104 V10.6.0 (2012-03) : Table 8.2.1-1 (page:74))..
    txConfig.maximumReTransmissions=3;
  }

  if(!txConfig.isReady(false))
  {
    cout<<"[both:]Forced to abort. Transmission parameters not fully configured."<<endl;
    abort();
  }
  
  if(temp)
  {
    txConfig.nCodeWords=2;
    if(txConfig.nLayers<=2)
      txConfig.nLayers=2;
    else
      txConfig.nLayers=4;
  }
  
  if(!parse(fileName,"enableFallbackMode",txConfig.fallbackConfig.isEnabled))
  {
#ifdef USING_MPI
if(currentTaskid==0)
#endif    
    cout<<"[detl:]Disabling fallback mode by default ..."<<endl;
    txConfig.fallbackConfig.isEnabled=false;
  }
  if(txConfig.fallbackConfig.isEnabled){

    if(txConfig.transmissionMode!=_TRANSMISSION_MODE_3_ && txConfig.transmissionMode!=_TRANSMISSION_MODE_4_ && txConfig.transmissionMode!=_TRANSMISSION_MODE_8_ && txConfig.transmissionMode!=_TRANSMISSION_MODE_9_ && txConfig.transmissionMode!=_TRANSMISSION_MODE_NR_)
    {
     cout<<"[both:]Unsupported transmissionMode for Fallback.."<<endl;
     abort();
    }
    txConfig.fallbackConfig.txModeConfig.transmissionMode=_TRANSMISSION_MODE_2_;
    txConfig.fallbackConfig.txModeConfig.nLayers=txConfig.nLayers;
    txConfig.fallbackConfig.txModeConfig.nCodeWords=1;
  }
  txConfig.codeWordIndicesToConsider=getIntegers(0,txConfig.nCodeWords-1);

  txConfig.originalConfig.nCodeWords=txConfig.nCodeWords;
  txConfig.originalConfig.nLayers=txConfig.nLayers;
  txConfig.originalConfig.transmissionMode=txConfig.transmissionMode;

  if(isDownlink)
  {
    if(!parse(fileName,"globalAperiodicCQITrigger",txConfig.globalAperiodicCQITrigger))
    {
      cout<<"[detl:]Loading globalAperiodicCQITrigger as 1 subframe by default."<<endl;
      txConfig.globalAperiodicCQITrigger=1;
    }
    
    if(!parse(fileName,"pdcchCount",txConfig.cchInfo.pdcchCount))
    {
      cout<<"[detl:]Loading 0 as default value for PDCCH Count."<<endl;
      txConfig.cchInfo.pdcchCount=0;
    }
    
    if(!parse(fileName,"SRSCount",txConfig.cchInfo.srsCount))
    {
      cout<<"[detl:]Loading 0 as default value for SRS Count."<<endl;
      txConfig.cchInfo.srsCount=0;
      
    }
    
    if(!parse(fileName,"SRSPeriodicityInms",txConfig.cchInfo.SRSPeriodicityInms))
    {
      cout<<"[detl:]Loading 0 as default value for SRSPeriodicityInms."<<endl;
      txConfig.cchInfo.SRSPeriodicityInms=0;
    }
//  
//     if(!parse(fileName,"enablePBCH",txConfig.cchInfo.enablePBCH))
//     {
//       cout<<"[detl:]Disabling PBCH by default."<<endl;
//       txConfig.cchInfo.enablePBCH=false;
//     }
//     if(!parse(fileName,"enablePssSss",txConfig.cchInfo.enablePssSss))
//     {
//       cout<<"[detl:]Disabling PssSss by default."<<endl;
//       txConfig.cchInfo.enablePssSss=false;
//     }  
    txConfig.cchInfo.enableSSB=isSSBEnabled;
     
    if(!parse(fileName,"enableEPDCCH",txConfig.cchInfo.enableEPDCCH))
    {
      cout<<"[detl:]Disabling EPDCCH by default."<<endl;
      txConfig.cchInfo.enableEPDCCH=false;
    }  
    //! DL power-allocation   
    if(!parse(fileName,"enableVarPowInRB",txConfig.enableVarPowInRB))
    {
      cout<<"Disabling Variable Power transmission in RB by default..."<<endl;
      txConfig.enableVarPowInRB=false;
    }
    txConfig.dlCQIModeInfo.load(fileName);
    
    parse(l2fileName,"mIsNonIdealPDCCHEnabled",txConfig.pdcchInfo.NonIdealPDCCHEnabled);
    parse(l2fileName,"isPDCCHAbstractionEnabled",txConfig.pdcchInfo.isPDCCHAbstractionEnabled);
    parse(l2fileName,"mIsEPDCCHEnabled",txConfig.pdcchInfo.isEPDCCHEnabled);
    parse(l2fileName, "fractionOfUEsForEPDCCH", txConfig.pdcchInfo.ueFractionForEPDCCH);
    
    txConfig.cchInfo.enableEPDCCH=txConfig.pdcchInfo.isEPDCCHEnabled;
    
    parse(fileName,"TRSPeriodicityInms",txConfig.mTRSConf.TRSPeriodicityInms);
    parse(fileName,"firstTRSSymbol",txConfig.mTRSConf.trssymbolno);
    parse(fileName,"DL_isModeAdaptationEnabled",txConfig.isModeAdaptationEnabled);
    
    
    if(parse(fileName,"multipleCSIProcessFlag",txConfig.multipleCSIProcessFlag))
    {
      parse(fileName,"nCSIProcess",txConfig.nCSIProcess);    
      
      if(txConfig.nCSIProcess<1)
      {
	cout<<"[Error:] if multipleCSIProcessFlag flag is enabled, nCSIProcess should be greated than 1... Update myDLTxRXConfig.txt... "<<endl;
	abort();
      }
    }
    
  }
  if(!isDownlink)
  {
    txConfig.ulCQIModeInfo.load(fileName);
    if(!parse(fileName,"gammaIOT",txConfig.powerControlInfo.gammaIOT))
    {
      cout<<"[detl:]Loading 0.8 as default value for gammaIOT."<<endl;
      txConfig.powerControlInfo.gammaIOT = 0.8;
    }
    if(!parse(fileName,"minimumSINRInDB",txConfig.powerControlInfo.minimumSINRInDB))
    {
      cout<<"[detl:]Loading 0 as default value for minimumSINRInDB."<<endl;
      txConfig.powerControlInfo.minimumSINRInDB = 0;
    }
    if(!parse(fileName,"windowSizeForNI",txConfig.powerControlInfo.windowSizeForNI))
    {
      cout<<"[detl:]Loading 20 as default value for windowSizeForNI."<<endl;
      txConfig.powerControlInfo.windowSizeForNI = 20;
    }
    if(!parse(fileName,"enableClosedLoopPowerControl",txConfig.powerControlInfo.enableClosedLoopPowerControl))
    {
      cout<<"[detl:]Disabling CL power control by default."<<endl;
      txConfig.powerControlInfo.enableClosedLoopPowerControl = false;      
    }
    
     if(!parse(fileName,"windowSizeForTPCcmd",txConfig.powerControlInfo.windowSizeForTPCcmd))
    {
      cout<<"[detl:]Loading 5 as default value for windowSizeForTPCcmd."<<endl;
      txConfig.powerControlInfo.windowSizeForTPCcmd = 5;
    }
    
    if(!parse(fileName,"enablePowerBackoffModel",txConfig.powerControlInfo.enablePowerBackoffModel))
    {
        cout<<"[detl:]Disabling power back off model by default."<<endl;
        txConfig.powerControlInfo.enablePowerBackoffModel = false;
    }
    string powControlMethod;
    if(parse(fileName,"powerControlMethod",powControlMethod))
    {
      if(powControlMethod=="_POW_CONTROL_LTE_")
	txConfig.powerControlInfo.powerControlMethod = _POW_CONTROL_LTE_;
      else if(powControlMethod=="_POW_CONTROL_TargetSINR_")
	txConfig.powerControlInfo.powerControlMethod = _POW_CONTROL_TargetSINR_;
      else
      {
	cout<<"[detl:]Unknown powerControlMethod in the config file. Loading _POW_CONTROL_LTE_ as default."<<endl;
	txConfig.powerControlInfo.powerControlMethod = _POW_CONTROL_LTE_;
      }
      
    }
    else
    {
      cout<<"[detl:]Loading _POW_CONTROL_LTE_ as default for powerControlMethod."<<endl;
      txConfig.powerControlInfo.powerControlMethod = _POW_CONTROL_LTE_;
    }
    
    if(!parse(fileName,"pathlossCompensation",txConfig.powerControlInfo.pathlossCompensation))
    {
      cout<<"[detl:]Loading 1 as default value for pathlossCompensation."<<endl;
      txConfig.powerControlInfo.pathlossCompensation = 0.8;
    }
    
    if(!parse(fileName,"targetPowerForLTEPowerControlindBm",txConfig.powerControlInfo.targetPowerForLTEPowerControlindBm))
    {
      cout<<"[detl:]Loading -76 as default value for targetPowerForLTEPowerControlindBm."<<endl;
      txConfig.powerControlInfo.targetPowerForLTEPowerControlindBm = -76;
    }
    
  }
  
  
  
  return(txConfig);
}

ReceptionConfig_S parseRxConfig(string fileName,bool isDownlink){

  ReceptionConfig_S rxConfig;

  string prefix;
  if(isDownlink) prefix="DL_";
  else	prefix="UL_";

  if (!parse(fileName,prefix+"enableLinkAdaptation",rxConfig.enableLinkAdaptation))
    cout<<"[detl:]Disabling Link Adaptation in the receiver by default."<<endl;
  
  if (!parse(fileName,prefix+"enableRankAdaptation",rxConfig.enableRankAdaptation))
    cout<<"[detl:]Disabling Link Adaptation in the receiver by default."<<endl;

  
  if(!parse(fileName,prefix+"enableOuterLoopLinkAdaptation",rxConfig.isOuterLoopLinkAdaptationEnabled))
  {
    cout<<"[detl:]Warning. Cannot parse enableOuterLoopLinkAdaptation, Disabling Outer-Loop LA by default."<<endl;
    rxConfig.isOuterLoopLinkAdaptationEnabled = false;
  }
  
  if(rxConfig.isOuterLoopLinkAdaptationEnabled)
  {
    if(!parse(fileName,prefix+"targetBLER",rxConfig.targetBLER))
    {
      if(currentTaskid==0)
	cout<<"[detl:]Warning. Cannot parse TargetBLER, Setting TargetBLER=0.1 by default."<<endl;
      rxConfig.targetBLER = 0.1;
    }
    if(currentTaskid==0)
      cout<<"[detl:]Outer-Loop LA at UE is enabled and Target BLER = "<<rxConfig.targetBLER<<endl;
  }
  
  if (!parse(fileName,prefix+"idealChannel",rxConfig.idealChannel))
    cout<<"[detl:]Loading ideal channel in the receiver as default."<<endl;
  
  if (!parse(fileName,prefix+"errModelFlag",rxConfig.errModelFlag))
    cout<<"[detl:]Disabling errModelFlag in the receiver as default."<<endl;
  
  rxConfig.estimateInterference=true;
  
  if (!parse(fileName,prefix+"UEcategory",rxConfig.UEcategory))
    cout<<"[detl:]Loading UEcategory as 8 by default."<<endl;
   
  if(!parse(fileName,prefix+"prbBundlingSize",rxConfig.prbBundlingSize))
  {
    cout<<"[both:]Unable to load prbBundlingSize from the Configuration File - myDLTxRxConfig / myULTxRxConfig."<<endl;
    abort();
  }
  
  
  string desc;
  if (parse(fileName,prefix+"demodulationScheme",desc))
  {
    if(desc=="_DEMODULATION_SCHEME_MMSE_")rxConfig.demodulationScheme=_DEMODULATION_SCHEME_MMSE_;
    else if(desc=="_DEMODULATION_SCHEME_MRC_")rxConfig.demodulationScheme=_DEMODULATION_SCHEME_MRC_;
    else if(desc=="_DEMODULATION_SCHEME_WHITE_MRC_")rxConfig.demodulationScheme=_DEMODULATION_SCHEME_WHITE_MRC_;
    else
    {
      cout<<"[both:]Unknown Demodulation Scheme in the Configuration File - myDLTxRxConfig / myULTxRxConfig."<<endl;
      abort();
    }
    
    TransmissionMode_E transmissionMode;
    bool isModeAdaptationEnabled=false;
    parse(fileName,prefix+"enableModeAdaptation",isModeAdaptationEnabled);
    
    if(isModeAdaptationEnabled)
      transmissionMode = _TRANSMISSION_MODE_2_;
    else
      transmissionMode=parseTransmissionMode(fileName,isDownlink);
    
    if((transmissionMode==_TRANSMISSION_MODE_3_ || transmissionMode==_TRANSMISSION_MODE_4_) && rxConfig.demodulationScheme==_DEMODULATION_SCHEME_MRC_)
    {
      if(currentTaskid==0)
	cout<<"[detl:]Overriding Demodulation scheme to MMSE for TM3 and TM4."<<endl;
      rxConfig.demodulationScheme=_DEMODULATION_SCHEME_MMSE_;
      desc="_DEMODULATION_SCHEME_MMSE_";
    }
    
    if(currentTaskid==0)
      cout<<"[detl:]Receiver type is "<<desc<<endl<<endl;;
  }
  else
  {
    cout<<"[both:]Unable to load Demodulation Scheme from the Configuration File - myDLTxRxConfig / myULTxRxConfig."<<endl;
    abort();
  }
  if(isDownlink)
    rxConfig.dlCQIModeInfo.load(fileName);
  else
  {
    rxConfig.ulCQIModeInfo.load(fileName);
    
    if(!parse(fileName,"gammaIOT",rxConfig.powerControlInfo.gammaIOT))
    {
      cout<<"[detl:]Loading 0.8 as default value for gammaIOT."<<endl;
      rxConfig.powerControlInfo.gammaIOT = 0.8;
    }
    if(!parse(fileName,"minimumSINRInDB",rxConfig.powerControlInfo.minimumSINRInDB))
    {
      cout<<"[detl:]Loading 0 as default value for minimumSINRInDB."<<endl;
      rxConfig.powerControlInfo.minimumSINRInDB = 0;
    }
    if(!parse(fileName,"windowSizeForNI",rxConfig.powerControlInfo.windowSizeForNI))
    {
      cout<<"[detl:]Loading 20 as default value for windowSizeForNI."<<endl;
      rxConfig.powerControlInfo.windowSizeForNI = 20;
    }
    if(!parse(fileName,"enablePowerBackoffModel",rxConfig.powerControlInfo.enablePowerBackoffModel))
    {
        cout<<"[detl:]Disabling power back off model by default."<<endl;
        rxConfig.powerControlInfo.enablePowerBackoffModel = false;
    }
    
    string powControlMethod;
    if(parse(fileName,"powerControlMethod",powControlMethod))
    {
      if(powControlMethod=="_POW_CONTROL_LTE_")
	rxConfig.powerControlInfo.powerControlMethod = _POW_CONTROL_LTE_;
      else if(powControlMethod=="_POW_CONTROL_TargetSINR_")
	rxConfig.powerControlInfo.powerControlMethod = _POW_CONTROL_TargetSINR_;
      else
      {
	cout<<"[detl:]Unknown powerControlMethod in the config file. Loading _POW_CONTROL_LTE_ as default."<<endl;
	rxConfig.powerControlInfo.powerControlMethod = _POW_CONTROL_LTE_;
      }
      
    }
    else
    {
      cout<<"[detl:]Loading _POW_CONTROL_LTE_ as default for powerControlMethod."<<endl;
      rxConfig.powerControlInfo.powerControlMethod = _POW_CONTROL_LTE_;
    }
    
    if(!parse(fileName,"pathlossCompensation",rxConfig.powerControlInfo.pathlossCompensation))
    {
      cout<<"[detl:]Loading 1 as default value for pathlossCompensation."<<endl;
      rxConfig.powerControlInfo.pathlossCompensation = 0.8;
    }
    
    if(!parse(fileName,"targetPowerForLTEPowerControlindBm",rxConfig.powerControlInfo.targetPowerForLTEPowerControlindBm))
    {
      cout<<"[detl:]Loading -76 as default value for targetPowerForLTEPowerControlindBm."<<endl;
      rxConfig.powerControlInfo.targetPowerForLTEPowerControlindBm = -76;
    }
    
  }
  if(!parse(fileName,prefix+"enableRankAdaptation",rxConfig.enableRankAdaptation))
  {
    cout<<"[both:]Unable to parse enableRankAdaptation from the Configuration File - myDLTxRxConfig / myULTxRxConfig. Disabling RankAdaptation by default.. "<<endl;
    rxConfig.enableRankAdaptation=false;
  }
  
  if(!parse(fileName,"enableCodebookRestriction",rxConfig.enableCodebookRestriction))
  {
      cout<<"[both:]Unable to parse enableCodebookRestriction from the Configuration File - myDLTxRxConfig / myULTxRxConfig. Disabling CodebookRestriction by default.. "<<endl;
      rxConfig.enableCodebookRestriction=false;
  }
  if(rxConfig.enableCodebookRestriction)
  if(!parse(fileName,"trainingDurationInSlots",rxConfig.trainingDurationInSlots))
  {
      cout<<"[both:]Unable to parse trainingDurationInSlots from the Configuration File - myDLTxRxConfig / myULTxRxConfig. Using 50ms by default.. "<<endl;
      rxConfig.trainingDurationInSlots=50;
  }
  
  cout<<"trainingDurationInSlots : "<<rxConfig.trainingDurationInSlots<<endl;
  
  return(rxConfig);
}

CodeBookConfig_S parseDlCodeBookConfig(string fileName1, string fileName2, TransmissionMode_E transmissionMode,string AASfileName, int nBeams)
{
  CodeBookConfig_S codeBookConfig;
  
  if(transmissionMode == _TRANSMISSION_MODE_NR_)
  {
    codeBookConfig.isNRCodeBook = true;
    string codeBookmode,codeBookType;
    if(parse(fileName1,"NRCodebookMode",codeBookmode))
    {
      if(codeBookmode == "_NR_CODEBOOK_MODE_1_")
	codeBookConfig.codeBookMode = _NR_CODEBOOK_MODE_1_;
      else if(codeBookmode == "_NR_CODEBOOK_MODE_2_")
	codeBookConfig.codeBookMode = _NR_CODEBOOK_MODE_2_;
      else
	codeBookConfig.codeBookMode = _NR_CODEBOOK_MODE_UNIT_;
    }
    else
      codeBookConfig.codeBookMode = _NR_CODEBOOK_MODE_UNIT_;
    
    if(parse(fileName1,"NRCodebookType",codeBookType))
    {
      if(codeBookType == "_NR_CODEBOOK_TYPE_I_")
	codeBookConfig.codeBookType = _NR_CODEBOOK_TYPE_I_;
      else if(codeBookType == "_NR_CODEBOOK_TYPE_II_")
      {
        codeBookConfig.codeBookType = _NR_CODEBOOK_TYPE_II_;
        parse(fileName1,"Lbeams",codeBookConfig.LBeams);
        parse(fileName1,"nPSK",codeBookConfig.nPSK);
        parse(fileName1,"isType2IdealCoeff",codeBookConfig.isType2IdealCoeff);
        parse(fileName1,"isType2IdealPrecoder",codeBookConfig.isType2IdealPrecoder);
      }
      else if(codeBookType == "_NR_CODEBOOK_TYPE_I_MULTIPANEL_")
          codeBookConfig.codeBookType = _NR_CODEBOOK_TYPE_I_MULTIPANEL_;
      else
       codeBookConfig.codeBookType = _NR_CODEBOOK_TYPE_UNIT_;
    }
    else
      codeBookConfig.codeBookType = _NR_CODEBOOK_TYPE_UNIT_;
    
    
    Array<ivec> AntStructure, TXRUMap;ivec NgN1N2P(4);
    if(parse(fileName2,"AntStructure",AntStructure) && (AntStructure.length() == 4) && parse(fileName2,"TXRUMap",TXRUMap) && (TXRUMap.length() == 4))
    {
        NgN1N2P(0) = AntStructure(0)(0)*AntStructure(0)(1);
        NgN1N2P(3) = AntStructure(0)(0)*AntStructure(0)(4);
        NgN1N2P(1) = TXRUMap(0)(0);
        NgN1N2P(2) = TXRUMap(0)(1);
        int nPanels = AntStructure(0)(0)*AntStructure(0)(1);
        int nTXRUs = prod(TXRUMap(0))*nPanels*AntStructure(0)(4);
        int Ng=NgN1N2P(0),N1=NgN1N2P(1),N2=NgN1N2P(2),P=NgN1N2P(3),nPorts = prod(NgN1N2P);
        
        if(nPorts!=nTXRUs/nBeams) //check ports with respect to physical AntStructure
        {
            cout<<"Mismatch in BS nTXRUsPerBeam:"<<nTXRUs/nBeams<<" and nCSIRSPorts :"<<nPorts<<"..."<<endl;abort();
        }
        if((Ng!=(nPanels*TXRUMap(0)(2))/nBeams && Ng>1) || (nPanels==1 && Ng!=nPanels))   //check codebook panel match.  
        {
            cout<<"[both:] Invalid Panel Configuration for Codebook."<<endl;abort();
        }
        else if(Ng>1 && (N1!=TXRUMap(0)(1) || N2!=TXRUMap(0)(0)))
        {
            cout<<"Invalid N1 N2 with respect to Antenna Structure.."<<endl;abort();
        }
        else if(N1<N2) 
        {
            cout<<"[both:] N1 cannot be less than N2"<<endl;abort();
        }
        
        if(codeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_I_MULTIPANEL_)
        {
            if(Ng!=2 && Ng!=4)
            {
                cout<<"[both:] Ng can be {2,4} for Type-I Multipanel Codebook"<<endl;abort();
            }
            else if(Ng!=2 && codeBookmode == "_NR_CODEBOOK_MODE_2_")
            {
                cout<<"[both:] Ng cannot be "<<NgN1N2P(0)<<" for Codebook mode 2"<<endl;abort();
            }
            if(N2>2)
            {
                cout<<"[both:] N2 cannot be greater than 2"<<endl;abort();
            }
        }
        else if(Ng>1)
        {
            cout<<"Ng cannot be greater than 1 for Single Panel Codebook..."<<endl;abort();
        }
        codeBookConfig.Ng = Ng; 
        codeBookConfig.N1 = N1;
        codeBookConfig.N2 = N2;
        codeBookConfig.P = P;
    }
    else
    {
        cout<<"[both:]Configuration Error in ActiveAntennaSystemConfig.txt"<<endl;
      abort();
    }
    if(prod(NgN1N2P)==1 && codeBookConfig.codeBookType!=_NR_CODEBOOK_TYPE_I_)
    {
        cout<<codeBookType<<" doesnot support single port CSIRS...setting _NR_CODEBOOK_TYPE_I_ by default.."<<endl;
        codeBookConfig.codeBookType = _NR_CODEBOOK_TYPE_I_;
    }
  }
  
  return codeBookConfig;
  
}

void SystemInfo_S::parseNodesInfo(){

  if(parse(resultFiles.linkTableFileName,"serverNodeList",serverNodes)==false)  {	cout<<"[both:]Unable to load Server Nodes from the file - myLinkTable."<<endl;	abort();  }
  if(parse(resultFiles.linkTableFileName,"serviceNodeList",serviceNodes)==false)  {	cout<<"[both:]Unable to load Service Nodes from the file - myLinkTable."<<endl;	abort();  }
  if(parse(resultFiles.linkTableFileName,"serverNodeAntennaCount",serverNodeAntenna)==false)  {	cout<<"[both:]Unable to load Rx Node Antenna Count from the file - myLinkTable."<<endl;	abort();  }
  if(parse(resultFiles.linkTableFileName,"serviceNodeAntennaCount",serviceNodeAntenna)==false)  {	cout<<"[both:]Unable to load Tx Node Antenna Count from the file - myLinkTable."<<endl;	abort();  }
  parse(resultFiles.linkTableFileName,"serviceNodeTransmitPowerInDBm",serviceNodeTxPowersInDBm);
  parse(resultFiles.linkTableFileName,"serverNodeNumerologies",serverNodeNumerologies);
  parse(resultFiles.linkTableFileName,"serviceNodeNumerologies",serviceNodeNumerologies);
  parse(resultFiles.linkTableFileName,"serverNodeTransmitPowerInDBm",serverNodeTxPowersInDBm);
  parse(resultFiles.linkTableFileName,"serverNodeIDCell",serverNodeIDCells);
  parse(resultFiles.linkTableFileName,"TDDConfigPerServerNode",serverNodeTDDConf); // used only if TDD is enabled
  
}

void parseSystemInfoAWGN(SystemInfo_S &mySysInfo)
{
    parse(mySysInfo.myConfigFiles.simulationConfigFileName,"serviceNodeTransmitPowerInDBm",mySysInfo.serviceNodeTxPowersInDBm);
}



string getConfigFilesPath(int argc, char** argv){
  
  string toReturn="";
  if(argc==3)
  {
    if(strcmp(argv[1],"-c")==0)
      toReturn=string(argv[2]);
  }
  else if(argc==4)
  {
    if(strcmp(argv[1],"-c")==0) 
    {
      toReturn=string(argv[2]);
      if(strcmp(argv[3],"mcell")==1) // ./LTESim.run -c cpath1 cpath2
	toReturn=toReturn+"/* "+string(argv[3]);
    }
    else if(strcmp(argv[2],"-c")==0) // ./LTESim.run mcell -c cpath
      toReturn=string(argv[3]);
  }
  else if(argc==5) 
  {
    if(strcmp(argv[1],"-c")==0) // ./LTESim.run -c cpath1 cpath2 mcell 
    {
      toReturn=string(argv[2]);
      toReturn=toReturn+"/* "+string(argv[3]);
    }
    else if(strcmp(argv[2],"-c")==0) // ./LTESim.run mcell -c cpath1 cpath2
    {
      toReturn=string(argv[3]);
      toReturn=toReturn+"/* "+string(argv[4]);
    }
  }
  return(toReturn);
}

void initSimulation(string configFilesPath,string configFileDir,string resultFilesPath){
  
  #ifdef USING_MPI  
  if (currentTaskid==0) {
    #endif   
    
    if(configFilesPath=="")
    {
      string toExec =  "rm -rf "+configFileDir+"; mkdir "+configFileDir+"; cp -r ../sls/configFiles/* "+configFileDir+";"; 
      system_exec(toExec.c_str());
    }
    else if(configFilesPath != "./configFiles" && configFilesPath != "configFiles")
    {
      string toExec= "rm -rf "+configFileDir+"; mkdir "+configFileDir+"; cp -r "+configFilesPath+"/* "+configFileDir+";";
      system_exec(toExec.c_str());
    }
    
    //loadConfigFileNames(configFiles);
    
//     ivec nu ;
//     parse(configFiles.nuConfigFileName,"nuIDs",nu);
//     Array<ivec> carrierFrequenciesinMHz;
//     parse(configFiles.carrierAggregationFileName,"carrierFreqInMHzPerServerNodeType",carrierFrequenciesinMHz);
//     
//     ivec carriersInMHz(0);
//     for(int srvr_cnt=0;srvr_cnt<carrierFrequenciesinMHz.length();srvr_cnt++)
//       append(carriersInMHz,carrierFrequenciesinMHz(srvr_cnt));
//     carriersInMHz=findUniqueNumbers(carriersInMHz);
//     
//     ivec nuCaIDs=getNUCaIDs(nu,carriersInMHz);
   
//     system_exec("rm -rf Results; mkdir Results; mkdir Results/eNBstatsDL; mkdir Results/eNBstatsUL; mkdir Results/delayStatsDL; mkdir Results/delayStatsUL; mkdir Results/IoTLogs;");
    string cmd = "rm -rf "+resultFilesPath+"; mkdir "+resultFilesPath;
    system_exec(cmd.c_str());
    
//     for(int i =0;i < nuCaIDs.length(); i++ )
//     {
//       string input= "mkdir Results/NU_"+toString(nuCaIDs(i));
//       system_exec(input.c_str());
//     }
    signal(SIGINT,interruptCatch);
    signal(SIGSEGV,segmentationFaultCatch);
    captureTime(1,START);
//     string simDetails = briefSimulationDetails();
//     cout<<simDetails;
    
    #ifdef USING_MPI
  }
  MPI_Barrier(MPI_COMM_WORLD);
  #endif
}

void initAllFiles(int argc, char** argv, ConfigFileNames_S &myConfigFiles , ResultFileNames_S &resultFiles){
    
    string configFilePath=getConfigFilesPath(argc, argv);
    resultFiles.directory = "./Results/";
    myConfigFiles.directory = "./configFiles/";
    initSimulation(configFilePath,myConfigFiles.directory,resultFiles.directory);    //Initializing the folder for simulation
    
    myConfigFiles.loadConfigFileNames(myConfigFiles.directory);
    
    double carrierFreqInMHz;
    parse(myConfigFiles.sysConfigFileName,"carrierFreqInMHz",carrierFreqInMHz);
    ivec nu=myConfigFiles.nuIDs;
    ivec nuCaIDs=getNUCaIDs(nu,to_ivec(carrierFreqInMHz));
    resultFiles.loadResultFileNames(resultFiles.directory,nuCaIDs);
}


void SystemInfo_S::init(int argc, char** argv){

  defaults();
  
  #ifdef USING_MPI
  initMPI(&(nTasks),&(currentTaskid),argc,argv);
  if(currentTaskid==0){
    #endif
    
    
    #ifdef USING_MPI
  }
  MPI_Barrier(MPI_COMM_WORLD);
  #endif  
  
  initAllFiles(argc,argv,myConfigFiles,resultFiles);
  
  
  #ifdef USING_MPI
  if(currentTaskid==0) {
  #endif

    cout<<"\n\n#################################################"<<endl;
    cout<<"[both:]Initializing Simulation Seed"<<endl;
    cout<<"#################################################\n\n"<<endl;

  #ifdef USING_MPI
  }
  #endif
  
  if(!parse(myConfigFiles.simulationConfigFileName,"useFixedSeed",useFixedSeed))
    useFixedSeed=false;
  else if(useFixedSeed)
    if(!parse(myConfigFiles.simulationConfigFileName,"seed",simulationSeed))
    {
      simulationSeed=12345;//(unsigned int)randi(0,99999);
      cout<<"[detl:]Loading default Fixed simulation seed as "<<simulationSeed<<endl;
    }
    
    if(useFixedSeed)
  {
  #ifdef USING_MPI
  unsigned int seedToBcast;
  if(currentTaskid==0) {
  #endif
      if(simulationSeed==0)
	simulationSeed=(unsigned int)randi(1,99999);
      cout<<"[detl:]Using "<<simulationSeed<<" as Simulation Seed."<<endl;
  #ifdef USING_MPI
      seedToBcast=simulationSeed;    
  }
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(&seedToBcast, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
  simulationSeed=seedToBcast;
  //cout<<"[detl:]Using "<<simulationSeed<<" as Simulation Seed."<<endl;
  #endif
      setSeed(simulationSeed);
    }
    else
    {
  #ifdef USING_MPI
  if(currentTaskid==0) {
  #endif
      cout<<"[detl:]Using random Simulation Seed."<<endl;
  #ifdef USING_MPI
  }
  #endif
      setSeed();
    }
    if(currentTaskid==0)
        cout<<briefSimulationDetails(0)<<endl;
  
//    cout<<"isRRMEnabled : : "<<isRRMEnabled<<endl;

  //Loading Trace variables
  traceValuePerTraceName = parseAll(myConfigFiles.tracesFileName);
  #ifdef USING_MPI
  MPI_Barrier(MPI_COMM_WORLD);
  #endif 
  
  // Loading Config file paths
  
  // Parsing simulation Information from config files
  
  // Loading Result file paths
  
//   Array<ivec> carrierFrequenciesinMHz;
//   parse(myConfigFiles.carrierAggregationFileName,"carrierFreqInMHzPerServerNodeType",carrierFrequenciesinMHz);
  ivec nodeCounts;
  parse(myConfigFiles.sysConfigFileName,"nodeCounts",nodeCounts);
  Array<string> nodeCategory;
  parse(myConfigFiles.sysConfigFileName,"nodeCategory",nodeCategory);
  
  carrierInfo=CarrierInfo_S(myConfigFiles.sysConfigFileName);
  
  parse(myConfigFiles.simulationConfigFileName,"simulationTimeInSec",simulationTimeInSec);
  
  if(currentTaskid==0)
    cout << "Simulation running for " << simulationTimeInSec << " seconds" << endl;
  
  if(!parse(myConfigFiles.simulationConfigFileName,"settlingTimeInSec",settlingTimeInSec))
  {
    cout<<"[detl:]Loading 0 as default value for settlingTimeInSec."<<endl;
    settlingTimeInSec = 0;
  }
  
  if(simulationTimeInSec <= settlingTimeInSec)
  {
    cout<<"[detl:]simulationTimeInSec cannot be less than or equal to settlingTimeInSec,... Overriding 0 for settlingTimeInSec. "<<endl;
    settlingTimeInSec = 0;
  }
  
//   double bandwidthInMHz;
//   parse(myConfigFiles.simulationConfigFileName,"bandwidthInMHz",bandwidthInMHz);
//   BandwidthInfo.bandwidth=bandwidthInMHz*1e6;
//   if (parse(myConfigFiles.simulationConfigFileName,"uplinkResourceBlocks",BandwidthInfo.uplinkResourceBlocks) == false)
//   {
//     cout<<"[both:]Unable to load number of uplink resource blocks from the Configuration File - mySimulationConfig."<<endl;
//     abort();
//   }
// 
//   if (parse(myConfigFiles.simulationConfigFileName,"downlinkResourceBlocks",BandwidthInfo.downlinkResourceBlocks) == false)
//   {
//     cout<<"[both:]Unable to load number of downlink resource blocks from the Configuration File - mySimulationConfig."<<endl;
//     abort();
//   }

  
  parse(myConfigFiles.simulationConfigFileName,"maximumServiceNodesPerServerNode",maximumServiceNodesPerServerNode);

  if(!parse(myConfigFiles.simulationConfigFileName,"enableLinkLevelSimulation",linkSimulationInfo.isEnabled)){

    cout<<"[detl:]Disabling Link Level Simulation by default."<<endl;
    linkSimulationInfo.isEnabled=false;
  }
  else
    if(linkSimulationInfo.isEnabled)
    {
      vec snrRangeInDB(3);
      if(!parse(myConfigFiles.simulationConfigFileName,"minimumSNR",snrRangeInDB(0))){

	cout<<"[both:]Unable to parse minimumSNR for Link Level Simulation. Default value not possible."<<endl;abort();
      }
      if(!parse(myConfigFiles.simulationConfigFileName,"maximumSNR",snrRangeInDB(1))){

	cout<<"[both:]Unable to parse maximumSNR for Link Level Simulation. Default value not possible."<<endl;abort();
      }
      if(!parse(myConfigFiles.simulationConfigFileName,"stepSizeForSNR",snrRangeInDB(2))){

	cout<<"[detl:]Assuming default value for stepSizeForSNR as 1.0 . "<<endl;
	snrRangeInDB(2)=1.0;
      }
      linkSimulationInfo.snrRangeInDB=getVector(snrRangeInDB(0),snrRangeInDB(1),snrRangeInDB(2));

      if(!parse(myConfigFiles.simulationConfigFileName,"enableFEC",linkSimulationInfo.enableFEC)){

	cout<<"[detl:]Enabling FEC by default in link Simulation setup."<<endl;
	linkSimulationInfo.enableFEC=true;
      }
      if(linkSimulationInfo.enableFEC)
      {
	if(!parse(myConfigFiles.simulationConfigFileName,"cqiIndex",linkSimulationInfo.cqiIndex)){

	  cout<<"[both:]Unable to parse cqiIndex for Link Level Simulation. Default value not possible."<<endl;abort();
	}
      }
      else
      {
	linkSimulationInfo.modulationScheme=parseModulationScheme(myConfigFiles.simulationConfigFileName,"modulationScheme");
	if(linkSimulationInfo.modulationScheme==_MODULATION_SCHEME_UNINIT_){

	  cout<<"[both:]Unable to parse Modulation Scheme for Link Level Simulation. Default value not possible."<<endl;abort();
	}
      }
    }
    if(!parse(myConfigFiles.simulationConfigFileName,"picoBias",picoBias))
    {
      cout<<"[detl:]Loading 0 as default value for picoBias."<<endl;
      picoBias = 0;
    }

  if(!parse(myConfigFiles.simulationConfigFileName,"isWithCoMP",compInfo.isWithCoMP))
  {
    cout<<"[detl:]Loading non CoMP system as default."<<endl;
    compInfo.isWithCoMP=false;
  }
  if(compInfo.isWithCoMP)
  {
    string compScheme;
    if(parse(myConfigFiles.simulationConfigFileName,"compScheme",compScheme))
    {
      if(compScheme=="_COMP_SCHEME_JOINT_TX_") compInfo.compScheme=_COMP_SCHEME_JOINT_TX_;
      else if(compScheme=="_COMP_SCHEME_JOINT_RX_") compInfo.compScheme=_COMP_SCHEME_JOINT_RX_;
      else if(compScheme=="_COMP_SCHEME_DPS_") compInfo.compScheme=_COMP_SCHEME_DPS_;
      else if(compScheme=="_COMP_SCHEME_DB_") compInfo.compScheme=_COMP_SCHEME_DB_;
      else
      {
	cout<<"[detl:]Unknow CoMP Scheme."<<endl;
	abort();
      }
    }
    parse(myConfigFiles.simulationConfigFileName,"maximumCoMPSetSize",compInfo.maximumCoMPSetSize);
    parse(myConfigFiles.simulationConfigFileName,"maximumCoMPSets",compInfo.maximumCoMPSets);
    
  }

  if(!parse(myConfigFiles.simulationConfigFileName,"enableMacInterface",enableMacInterface))
  {
    cout<<"[detl:]Disabling MAC Interface by default."<<endl;
    enableMacInterface=false;
  }

  
  
  if(!parse(myConfigFiles.simulationConfigFileName,"enableServiceNodeDeactivator",enableServiceNodeDeactivator))
  {
    cout<<"Disabling Node deactivator by default."<<endl;
    enableServiceNodeDeactivator = false;
  }
  if(enableServiceNodeDeactivator)
    parse(myConfigFiles.simulationConfigFileName,"deactivatorServiceNodeList",deactivatorServiceNodeList);
  
  string uldlMode;
  if(!parse(myConfigFiles.simulationConfigFileName,"ulDlMode",uldlMode))
  {
    cout<<"[both:]Please specify whether the simulation is uplink, downlink or duplex. Default value not possible."<<endl;
    abort();
  }
  else
  {
    if(uldlMode=="_ULDL_MODE_DOWNLINK_")
    {
      ulDlMode=_ULDL_MODE_DOWNLINK_;
    }
    else if(uldlMode=="_ULDL_MODE_UPLINK_")
    {
      cout<<"[detl:]Loading all Uplink mode as ULDL Mode for simulation.."<<endl;
      ulDlMode=_ULDL_MODE_UPLINK_;
    }
    else if(uldlMode=="_ULDL_MODE_TDD_")
    {
      ulDlMode=_ULDL_MODE_TDD_;
    }
    else if(uldlMode=="_ULDL_MODE_FDD_")
    {
      ulDlMode=_ULDL_MODE_FDD_;
    }
    else
    {
      cout<<"[both:]Unknown UL-DL Mode in the config file."<<endl;
      abort();
    }
    if(traceValuePerTraceName["printDLULMode"]=="true")
      cout<<"[detl:] ulDlMode = "<<ulDlMode<<endl;
  }

  if(!parse(myConfigFiles.sysConfigFileName,"enableAntCorrelation",enableAntCorrelation))
  {
    cout<<"Disabling Antenna Correlation by default..."<<endl;
    enableAntCorrelation=false;
  }
  else if(enableAntCorrelation)
  {
    string antennaCorrelation;
    if(parse(myConfigFiles.sysConfigFileName,"antenneCorrelation",antennaCorrelation))
    {
      if(antennaCorrelation=="_ANTENNA_CORRELATION_UNCORRELATED_")
	antenneCorrelation=_ANTENNA_CORRELATION_UNCORRELATED_;
      else if(antennaCorrelation=="_ANTENNA_CORRELATION_MEDIUM_")
	antenneCorrelation=_ANTENNA_CORRELATION_MEDIUM_;
      else if(antennaCorrelation=="_ANTENNA_CORRELATION_HIGH_")
	antenneCorrelation=_ANTENNA_CORRELATION_HIGH_;
      else
      {
	cout<<"Setting Antenna Correlationas _ANTENNA_CORRELATION_UNCORRELATED_ by default..."<<antenneCorrelation<<endl;
	antenneCorrelation=_ANTENNA_CORRELATION_UNCORRELATED_;
      }
    }
    else
    {
      cout<<"Setting Antenna Correlationas _ANTENNA_CORRELATION_UNCORRELATED_ by default..."<<antenneCorrelation<<endl;
      antenneCorrelation=_ANTENNA_CORRELATION_UNCORRELATED_;
    }
  }
  
  if(!parse(myConfigFiles.simulationConfigFileName,"strongInterferersToSimulate",strongInterferersToSimulate))
  {
    cout<<"[detl:]Loading 8 as default strongInterferersToSimulate."<<endl;
    strongInterferersToSimulate=8;
  }

  if(!parse(myConfigFiles.simulationConfigFileName,"modelWeakInterferenceAsNoise",modelWeakInterferenceAsNoise))
  {
    cout<<"[detl:]Enabling modelWeakInterferenceAsNoise by default."<<endl;
    modelWeakInterferenceAsNoise=true;
  }

  if(!parse(myConfigFiles.simulationConfigFileName,"outputFilePrintFrequency",outputFilePrintFrequency))
  {
    cout<<"[detl:]Loading 0.1sec for outputFilePrintFrequency by default."<<endl;
    outputFilePrintFrequency=0.1;
  }
  
  if(!parse(myConfigFiles.simulationConfigFileName,"isWithAbstraction",isWithAbstraction))
  {
    cout<<"[detl:]Disabling Abstraction mode by default."<<endl;
  }
  
  if(isWithAbstraction)
  {
    string abstractionModel;
    if(parse(myConfigFiles.simulationConfigFileName,"AbstractionModel",abstractionModel))
    {
      if(abstractionModel == "_ABSTRACTION_DECODER_")
	AbstractionModel=_ABSTRACTION_DECODER_;
      else if(abstractionModel == "_ABSTRACTION_CQI_")
	AbstractionModel=_ABSTRACTION_CQI_;
      else if(abstractionModel == "_ABSTRACTION_IDEAL_")
	AbstractionModel=_ABSTRACTION_IDEAL_;
      else
      {
	AbstractionModel=_ABSTRACTION_DECODER_;
	cout<<"[detl:]Abstraction Model is not configured correctly, setting _ABSTRACTION_DECODER_ by default."<<endl;
	sleep(5);
      }
    }
    else
    {
      AbstractionModel=_ABSTRACTION_DECODER_;
      cout<<"[detl:]Abstraction Model is _ABSTRACTION_DECODER_ by default."<<endl;
    }
    
//     if(linkSimulationInfo.isEnabled && linkSimulationInfo.enableFEC)
//     {
//       cout<<"[detl:]Warning: Only Channel Decoding can be abstracted for Link Level simulations."<<endl;
//       sleep(5);
//       AbstractionModel=_ABSTRACTION_DECODER_;
//     }
    
    if(ulDlMode!=_ULDL_MODE_DOWNLINK_)
    {
      cout<<"[detl:]Warning: Only Channel Decoding can be abstracted for Uplink/TDD simulations."<<endl;
      sleep(5);
      AbstractionModel=_ABSTRACTION_DECODER_;
    }
    if(currentTaskid==0)
      cout<<"[detl:]Abstraction mode is enabled and Model is "<<(int)AbstractionModel<<endl;
  }
  
  if(!parse(myConfigFiles.simulationConfigFileName,"isLocalised",isLocalised))
  {
    cout<<"[detl:]Loading localised permutation as default."<<endl;
    isLocalised=true;
  }
    
#ifdef USING_RRC
if(!parse(myConfigFiles.simulationConfigFileName,"isRRCEnabled",isRRCEnabled))
{
    cout<<"[detl:]Disabling isRRCEnabled by default."<<endl;
    isRRCEnabled= false;
}
#else
  isRRCEnabled= false;
#endif

#ifdef USING_RRM
if(!parse(myConfigFiles.simulationConfigFileName,"isRRMEnabled",isRRMEnabled))
{

  cout<<"[detl:]Disabling isRRMEnabled by default."<<endl;
  isRRMEnabled= false;
}
#endif

if(!parse(myConfigFiles.simulationConfigFileName,"isPRACHEnabled",isPRACHEnabled))
{
  cout<<"[detl:]Disabling PRACH by default."<<endl;
  isPRACHEnabled= false;
}
if(!parse(myConfigFiles.simulationConfigFileName,"enableSidelink",enableSidelink))
{
    cout<<"[detl:]Disabling sidelink by default."<<endl;
    enableSidelink= false;
}
if(!parse(myConfigFiles.simulationConfigFileName,"isSRSEnabled",isSRSEnabled))
{
  cout<<"[detl:]Disabling SRS by default."<<endl;
  isSRSEnabled= false;
}
if(isSRSEnabled && ulDlMode==_ULDL_MODE_DOWNLINK_)
{
  cout<<"[detl:]Warning. SRS can't be enabled in downlink simulation. Disabling SRS."<<endl;
  isSRSEnabled= false;
}
if(!parse(myConfigFiles.simulationConfigFileName,"enableControlChannels",isControlChannelEnabled))
{
  cout<<"[detl:]Disabling ControlChannels by default."<<endl;
  isControlChannelEnabled=false;
}

if(ulDlMode!=_ULDL_MODE_UPLINK_)
  parse(myConfigFiles.ssbConfigFileName,"isSSBEnabled",isSSBlockEnabled);

enableBeamManagement = true;

  if(linkSimulationInfo.isEnabled)
  {
      cout<<"[detl:]Disabling BeamManagement for link simulation."<<endl;
      enableBeamManagement=false;
  }
  
  
  if(!parse(myConfigFiles.simulationConfigFileName,"enableUserMobility",enableUserMobility))
  {
    cout<<"[detl:]Disabling enableUserMobility by default."<<endl;
    enableUserMobility=false;
  }
  else
    if(enableUserMobility)
    {
      string mobilityType;
      if(!parse(myConfigFiles.simulationConfigFileName,"mobilityType",mobilityType))
      {
	cout<<"[detl:]Loading _MOBILITY_TYPE_RND_DIR_ as default value for mobilityType."<<endl;
	mobilityConfig.mobilityType=_MOBILITY_TYPE_RND_DIR_;
      }
      else
      {
	if(mobilityType=="_MOBILITY_TYPE_RND_DIR_") mobilityConfig.mobilityType=_MOBILITY_TYPE_RND_DIR_;
	else if(mobilityType=="_MOBILITY_TYPE_RND_WALK_") mobilityConfig.mobilityType=_MOBILITY_TYPE_RND_WALK_;
	else
	  {cout<<"[both:]Unknown mobilityType in config file."<<endl;abort();}
      }
      if(mobilityConfig.mobilityType==_MOBILITY_TYPE_RND_WALK_)
	if(!parse(myConfigFiles.simulationConfigFileName,"changeDirectionOnHandoff",mobilityConfig.changeDirectionOnHandoff))
	{
	  cout<<"[detl:]Disabling changeDirectionOnHandoff by default."<<endl;
	  mobilityConfig.changeDirectionOnHandoff=false;
	}

	if(!parse(myConfigFiles.simulationConfigFileName,"refreshDistanceInMetres",mobilityConfig.refreshDistanceInMetres))
      {
	cout<<"[detl:]Loading 10 as refreshDistanceInMetres by default."<<endl;
	mobilityConfig.refreshDistanceInMetres=10;
      }
    }

  if(!parse(myConfigFiles.simulationConfigFileName,"enableRestrictedReception",receptionModel.enableRestrictedRecepition)) {

    cout<<"[detl:]Disabling restrictied Reception option by default."<<endl;
    receptionModel.enableRestrictedRecepition=false;
  }
  else{

    if(receptionModel.enableRestrictedRecepition==true)
      parse(myConfigFiles.simulationConfigFileName,"serverNodesWithActualReception",receptionModel.serverNodesWithActualReception);

  }
  if(!parse(myConfigFiles.simulationConfigFileName,"enableRandomLoad",randomLoadInfo.enableRandomLoad)) {

    cout<<"[both:]Disabling random load option by default."<<endl;
    randomLoadInfo.enableRandomLoad=false;
  }
  else{

    if(randomLoadInfo.enableRandomLoad==true)
      parse(myConfigFiles.simulationConfigFileName,"loadPerServerNode",randomLoadInfo.loadPerServerNode);

  }

  updateFeature("isWithABS",myConfigFiles.simulationConfigFileName,false);
  cout<<" ABS feature support "<< hasFeature("isWithABS")<<endl;
  
  updateFeature("isMultiOperator",myConfigFiles.simulationConfigFileName,false);
  cout<<" MultiOperator feature support "<< hasFeature("isMultiOperator")<<endl;  
  
  updateFeature("isDiffAntDiffUEs",myConfigFiles.simulationConfigFileName,false);
  cout<<" Different No.of Antennas to Different UEs feature support "<< hasFeature("isDiffAntDiffUEs")<<endl;  
 
  updateFeature("isPUCCHEnabled",myConfigFiles.simulationConfigFileName,false);
 
//    updateFeature("CarrierAggregationEnabled", myConfigFiles.carrierAggregationFileName,false);
  
//   if(hasFeature("CarrierAggregationEnabled")) 
//   {
//     if(compInfo.isWithCoMP || hasFeature("isMultiOperator"))
//     {
//       cout<<"[both:] CoMP or MultiOperator cannot be enabled with CarrierAggregation.. Aborting"<<endl;
//       abort();
//     }
//   }
   
//   multiopInfo.isMultiOperator=hasFeature("isMultiOperator");
  //     BandwidthInfo.samplingFrequencyInHz=BandwidthInfo.fftSize*frequencySpacingInHz;
  
  //Initializing carrierAggregation 
//   carrierAggregation.loadAdditionalCarrierFreqsFromConfigFile(myConfigFiles.sysConfigFileName,myConfigFiles.simulationConfigFileName,myConfigFiles.carrierAggregationFileName);
//   int nCarriersForCA = carrierAggregation.getNumberOfCarriersForCA();
//   //carrierinfo.loadAdditionalCarrierFreqsFromConfigFile();
//   
//   
//   if(hasFeature("CarrierAggregationEnabled") && nCarriersForCA == 1)
//   {
//     cout<<"[both:] nCarriersForCA is 1 and hence CarrierAggregation is disabled"<<endl;
//     disableFeature("CarrierAggregationEnabled");
//   }
//   
//   if(!hasFeature("CarrierAggregationEnabled") && nCarriersForCA != 1)
//   {
//     cout<<"[both:] Making nCarriersForCA for CarrierAggregation disabled case. "<<endl;
//     nCarriersForCA=1;
//   }
  
  if(!parse(myConfigFiles.simulationConfigFileName,"enableRestrictionForReTx",enableRestrictionForReTx))
  {
    cout<<"[detl:]Warning. Unable to parse enableRestrictionForReTx, Disabling Restriction in scheduling for ReTx by default."<<endl;
    enableRestrictionForReTx = false;
  }
  
  if(!parse(myConfigFiles.simulationConfigFileName,"maximumServiceNodesPerSubframe",maximumServiceNodesPerSubframe))
  {
    cout<<"[detl:]Warning. Unable to parse maximumServiceNodesPerSubframe, Disabling number of UE Restriction in scheduling by default."<<endl;
    maximumServiceNodesPerSubframe = -1;
  }

  string associationMetric;
  if(parse(myConfigFiles.simulationConfigFileName,"InitialAssociationMetric",associationMetric))
  {
    if(associationMetric=="_RSRP_")
      initialAssociationMetric = _RSRP_;
    else if(associationMetric=="_RSRQ_")
      initialAssociationMetric= _RSRQ_;
    else if(associationMetric=="_PATHLOSS_")
        initialAssociationMetric= _PATHLOSS_;
    else
    {
      cout<<"[detl:]Unknown associationMetric in the config file. Loading _RSRP_ as default."<<endl;
      initialAssociationMetric = _RSRP_;
    }

  }
  else
  {
    cout<<"[detl:]Loading _RSRP_ as default for associationMetric."<<endl;
    initialAssociationMetric = _RSRP_;
  }
  
  loadNUConfig();
  
  checkForSimulationCompatability();
  calcOthers();
    
  if(currentTaskid==0)
  {
    //! Priniting the 'pid' to file ./Results/pid.txt
    string fileName = resultFiles.directory + "briefSimulationDetails.txt";
    std::ofstream foutPID(fileName.c_str(), std::ios::out);
    foutPID<<briefSimulationDetails(0)<<endl;
    foutPID.close();
  }
}

void SystemInfo_S::loadNUConfig()
{  
  vec dlUsableBW,ulUsableBW;
  vec subcarrierSpacingInKHz,ttiDurationInSec;
//   Array<vec>  CPDurationInSec;
  Array<string> configFileFolder;
  vec  CellConfigPeriodicityInSec;
  parse(myConfigFiles.nuConfigFileName,"nuIDs",nuIDs);
  parse(myConfigFiles.nuConfigFileName,"subcarrierSpacingInKHz",subcarrierSpacingInKHz);
  parse(myConfigFiles.nuConfigFileName,"dlUsableBW",dlUsableBW);
  parse(myConfigFiles.nuConfigFileName,"ulUsableBW",ulUsableBW);
  parse(myConfigFiles.nuConfigFileName,"ttiDurationInSec",ttiDurationInSec);
//   parse(myConfigFiles.nuConfigFileName,"CPDurationInSec",CPDurationInSec);
  parse(myConfigFiles.nuConfigFileName,"configFileFolder",configFileFolder);
  parse(myConfigFiles.nuConfigFileName,"CellConfigPeriodicityInSec",CellConfigPeriodicityInSec);
  nuConfig.set_length(nuIDs.length());
  for(int nu_cnt=0;nu_cnt<nuIDs.length();nu_cnt++)
  {
    nuConfig(nu_cnt).nuID=nuIDs(nu_cnt);
    nuConfig(nu_cnt).subcarrierSpacingInHz=subcarrierSpacingInKHz(nu_cnt)*1000.0;
    nuConfig(nu_cnt).ttiDurationInSec=ttiDurationInSec(nu_cnt);
//     nuConfig(nu_cnt).CPDurationInSec=CPDurationInSec(nu_cnt);
    nuConfig(nu_cnt).dlUsableBW=dlUsableBW(nu_cnt);
    nuConfig(nu_cnt).ulUsableBW=ulUsableBW(nu_cnt);
    nuConfig(nu_cnt).configFileFolder=configFileFolder(nu_cnt);
    nuConfig(nu_cnt).CellConfigPeriodicityInSec=CellConfigPeriodicityInSec(nu_cnt);
    nuConfig(nu_cnt).init();
  }
}

void ConfigFileNames_S::loadConfigFileNames(string dirPrefix){

    directory = dirPrefix;
  simulationConfigFileName=dirPrefix+"mySimulationConfig.txt";
  sysConfigFileName=dirPrefix+"mySysConfig.txt";
  AASConfigFileName=dirPrefix+"ActiveAntennaSystemConfig.txt";
  advancedSysConfigFileName=dirPrefix+"mySysAdvancedConfig.txt";
 // linkTableFileName=dirPrefix+"myLinkTable.txt";
  nuConfigFileName=dirPrefix+"myNUConfig.txt";
  ssbConfigFileName=dirPrefix+"mySSBConfig.txt";
  ssbPatternPath=dirPrefix+"SSBPatterns/";
  mobilitySupportFileName=dirPrefix+"myMobilitySupport.txt";
  buildingInfoFileName=dirPrefix+"buildingInfo.txt";
//   carrierAggregationFileName=dirPrefix+"CarrierAggregationConfig.txt";
  admissionControlFileName=dirPrefix+"AdmissionControlConfig.txt";
  portToPhysicalAntennaMapperFileName=dirPrefix+"PortToAntennaMapper.txt";
  tracesFileName =dirPrefix+"myTraces.txt";
  
  parse(nuConfigFileName,"nuIDs",nuIDs);
  Array<string> nuFolderNames;
  parse(nuConfigFileName,"configFileFolder",nuFolderNames);
  nuConfigFiles.set_length(nuIDs.length());
  for(int nu_cnt=0;nu_cnt<nuIDs.length();nu_cnt++)
  {
    string prefix=dirPrefix+nuFolderNames(nu_cnt)+"/";
    nuConfigFiles(nu_cnt).dlTxRxConfigFileName=prefix+"myDLTxRxConfig.txt";
    nuConfigFiles(nu_cnt).ulTxRxConfigFileName=prefix+"myULTxRxConfig.txt";
    nuConfigFiles(nu_cnt).rbPatternPathForDownlink=prefix+"LTERbPattern/LTERbPatternDL/";
    nuConfigFiles(nu_cnt).rbPatternPathForUplink=prefix+"LTERbPattern/LTERbPatternUL/";
    nuConfigFiles(nu_cnt).cchPatternPath=prefix+"LTERbPattern/CCHPattern/";
    nuConfigFiles(nu_cnt).layerMappingTBSTranslationFileName=prefix+"LTETables/tbsTranslation.txt";
    nuConfigFiles(nu_cnt).transportBlockMCSMappingFileName=prefix+"LTETables/transportBlockMCSMapping.txt";
    nuConfigFiles(nu_cnt).turboInterleaverTableFileName=prefix+"LTETables/interleaverParameters.txt";
    nuConfigFiles(nu_cnt).cqiToMaxMCSRateFileName=prefix+"LTETables/cqiToMaxMCSRate.txt";
    nuConfigFiles(nu_cnt).L2SupportFileName=prefix+"myL2Support.txt";
    nuConfigFiles(nu_cnt).TCPSupportFilesName=prefix+"myTCPSupport.txt";
    nuConfigFiles(nu_cnt).NRrbPatternPath=prefix+"NRRbPattern/";
    
//     nuConfigFiles(nu_cnt).NRrbPatternPathUL=prefix+"NRRbPatternUL/";
  }
}

void ResultFileNames_S::loadResultFileNames(string dirPrefix, ivec tNUIDs){

  fSetSize(tNUIDs.length());
  
  directory = dirPrefix;
  nodeLocationsFileName=dirPrefix+"nodeLocations.txt";
  associationInfoFileName=dirPrefix+"associationInfo.txt";
  beamAssociationInfoFileName=dirPrefix+"beamAssociationInfo.txt";
  beamManagementInfoFileName=dirPrefix+"beamManagementInfo.txt";
  linkGainFileName=dirPrefix+"linkGainLogs.txt";
  beamRSRPfileName=dirPrefix+"beamRSRPdistribution.txt";
  associationLogsFileName = dirPrefix + "beamAssociationLogs.txt";
  antennaGainsFileName = dirPrefix + "antennaGains.txt";
  antennaInfoFileName = dirPrefix+"AntennaLocations/";
  linkTableFileName = dirPrefix+"myLinkTable.txt";
  for(int i=0; i < tNUIDs.length(); i++)
  { 
      string nuPrefix = dirPrefix+"NU_"+toString(tNUIDs(i));
  dlLinkPerformanceInfoFileName(i)=nuPrefix+"/dlLinkPerformanceInfo.txt";
  ulLinkPerformanceInfoFileName(i)=nuPrefix+"/ulLinkPerformanceInfo.txt";
  dlWindowBLERstatsFileName(i)=nuPrefix+"/dlWindowBLERstats.txt"; //Taskid's will be appended while printing. 
  ulWindowBLERstatsFileName(i)=nuPrefix+"/ulWindowBLERstats.txt";
  WideBandCQIlogsFileName(i)=nuPrefix+"/WideBandCQIlogs.txt";
  SubbandCQIlogsFileName(i)=nuPrefix+"/SubbandCQIlogs.txt";
  AperiodicCQIlogsFileName(i)=nuPrefix+"/AperiodicCQIlogs";
  RIlogsFileName(i)=nuPrefix+"/RIlogs";
  slUEPerformanceInfoFileName(i)=nuPrefix+"/slUEPerformanceInfo.txt.temp"+toString(currentTaskid);
  slSystemPerformanceInfoFileName(i)=nuPrefix+"/slSystemPerformanceInfo.txt.temp"+toString(currentTaskid);
  
  dlCumulativePerformanceFileName(i) = nuPrefix+"/dlSystemPerformanceLogs.txt";
  ulCumulativePerformanceFileName(i) = nuPrefix+"/ulSystemPerformanceLogs.txt";
  dlUEPerformanceInfoFileName(i)=nuPrefix+"/dlUEPerformanceInfo.txt";
  dlSystemPerformanceInfoFileName(i)=nuPrefix+"/dlSystemPerformanceInfo.txt";
  ulUEPerformanceInfoFileName(i)=nuPrefix+"/ulUEPerformanceInfo.txt";
  ulSystemPerformanceInfoFileName(i)=nuPrefix+"/ulSystemPerformanceInfo.txt";
  SchInfoFileName(i)=nuPrefix+"/SchedulingInfoLogs";
  MUMIMOpairingFileName(i)=nuPrefix+"/MUMIMOlogs";
  MCSlogsFileName(i)=nuPrefix+"/MCSlogs";  //As of now providing directories and file names will get appended later.
  
  }
}

ivec getFrameInfoIndx(Array<TxFrameInfo_S> &txFrameInfo, ivec txNodes){

  int indx;
  ivec indices=zeros_i(txNodes.length())-1;
  for(int tx_cnt=0;tx_cnt<txFrameInfo.length();tx_cnt++)
  {
    indx=find(txNodes,txFrameInfo(tx_cnt).nodeID);
    if(indx!=-1)
      indices(indx)=tx_cnt;
  }
  return(indices);
}

int getFrameInfoIndx(Array<TxFrameInfo_S> &txFrameInfo, int txNode){
  
  for(int tx_cnt=0;tx_cnt<txFrameInfo.length();tx_cnt++)
    if(txFrameInfo(tx_cnt).nodeID==txNode)
      return(tx_cnt);
  return(-1);
}

/*!\brief Compute the Downlink SIR from Strong-Interferers for all Service-nodes.
* This is a one-time process after large-scale channel generation.. Computed values are used later for computing \b subcarrierSignalPowerInWatts for UE Uplink-Transmission.. */

// vec getDownLinkSIRInDBForUplinkPowerControl(ChannelCloud &myChannelCloud,AssociationInfo_S associationInfo)
// {
//   vec downlinkSIRInDBPerServiceNode(associationInfo.serviceNodes.length());
//   ivec interferers;
//   double linkedChannelGainInDB;
//   vec interferingChannelGainInDB;
//   ivec signalNodeIndx=find(associationInfo.serverNodes,associationInfo.associatedServerNodesForEachServiceNode);
//   for(int srvc_cnt =0; srvc_cnt < associationInfo.serviceNodes.length(); srvc_cnt++)
//   {
//     interferers=associationInfo.serverNodes;
//     interferers.del(signalNodeIndx(srvc_cnt));
//     linkedChannelGainInDB = myChannelCloud.getLinkSpecificChannelGainInDB(associationInfo.serviceNodes(srvc_cnt),associationInfo.serverNodes(signalNodeIndx(srvc_cnt)),associationInfo.associatedBeamPairLinkforEachServiceNode(srvc_cnt).rxPanelID,associationInfo.associatedBeamPairLinkforEachServiceNode(srvc_cnt).txPanelID);
//     interferingChannelGainInDB = myChannelCloud.getLinkSpecificChannelGainInDB(associationInfo.serviceNodes(srvc_cnt),interferers,associationInfo.associatedBeamPairLinkforEachServiceNode(srvc_cnt).rxPanelID);
//     downlinkSIRInDBPerServiceNode(srvc_cnt) =linkedChannelGainInDB- dB(sum(inv_dB(interferingChannelGainInDB)));
//   }
//   return(downlinkSIRInDBPerServiceNode);
// }


void checkConfigFiles(TransmissionConfig_S txConfig, int nTxAntennas,bool isDlmode = true)
{
  RSInfo_S rsInfo=txConfig.rsInfo;
  int nCQIPorts=rsInfo.nCQIports;
  int nDeModPorts=rsInfo.DeModPorts.length();
  int nCRSPorts; 
  if(isDlmode)
    nCRSPorts = rsInfo.crsPorts.length();
  else
    nCRSPorts = nCQIPorts; // This is to satisfy the check in UPlink
  
  RSType_E CQIrsType=rsInfo.CQIrsType, DeModrsType=rsInfo.DeModrsType;

  if( (nTxAntennas<=32) && (nTxAntennas>=nCQIPorts) )
  {
//     cout<<"nCQIPorts:"<<nCQIPorts<<endl<<"nCRSPorts:"<<nCRSPorts<<endl;
    switch(getTransmissionScheme(txConfig.transmissionMode))
    {
      case _TRANSMISSION_SCHEME_SINGLE_PORT_CRS_:
	if((nCQIPorts != nDeModPorts)||(nCQIPorts != nCRSPorts) || (CQIrsType != DeModrsType) || (nCQIPorts != 1) || (txConfig.nLayers != 1) || (txConfig.nCodeWords != 1))
	  {cout<<"[both:]Invalid simulation setup for Single Port CRS Transmission Mode. Check myDLTxRxConfig / myULTxRxConfig."<<"Antennnas ="<<nTxAntennas<<endl;abort();}
	break;
      case _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_:
	if((nCQIPorts != nDeModPorts)||(nCQIPorts != nCRSPorts) || (CQIrsType != DeModrsType) || (nCQIPorts != 2 && nCQIPorts != 4) || (txConfig.nLayers != nCQIPorts) || (txConfig.nCodeWords != 1))
	  {cout<<"[both:]Invalid Simulation setup for Transmit Diversity Transmission Mode. Check myDLTxRxConfig / myULTxRxConfig."<<endl;abort();}
	break;
      case _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_:
	if((nCQIPorts != nDeModPorts)||(nCQIPorts != nCRSPorts) || (CQIrsType != DeModrsType) || (nCQIPorts != 2 && nCQIPorts != 4) || !(txConfig.nLayers>=1 && txConfig.nLayers <= nCQIPorts) || (txConfig.nCodeWords != 1 && txConfig.nCodeWords != 2))
	  {cout<<"[both:]Invalid Simulation setup for Large Delay CDD Transmission Mode. Check myDLTxRxConfig / myULTxRxConfig."<<endl;abort();}
	break;
      case _TRANSMISSION_SCHEME_CL_SM_:
	if((nCQIPorts != nDeModPorts) ||(nCQIPorts != nCRSPorts)|| (CQIrsType != DeModrsType) || (nCQIPorts != 2 && nCQIPorts != 4) || !(txConfig.nLayers>=1 && txConfig.nLayers <= nCQIPorts) || (txConfig.nCodeWords != 1 && txConfig.nCodeWords != 2))
	  {cout<<"[both:]Invalid Simulation setup for Closed Loop SM Transmission Mode. Check myDLTxRxConfig / myULTxRxConfig."<<endl;abort();}
	break;
      case _TRANSMISSION_SCHEME_CL_MU_MIMO_:
	if((nCQIPorts != nDeModPorts) ||(nCQIPorts != nCRSPorts)|| (CQIrsType != DeModrsType) || (nCQIPorts != 2 && nCQIPorts != 4) || (txConfig.nLayers!=1) || (txConfig.nCodeWords != 1))
	  {cout<<"[both:]Invalid Simulation setup for Closed Loop Mu MIMO Transmission Mode. Check myDLTxRxConfig / myULTxRxConfig."<<endl;abort();}
	break;
      case _TRANSMISSION_SCHEME_CL_BF_:
	if((nCQIPorts != nDeModPorts) ||(nCQIPorts != nCRSPorts)|| (CQIrsType != DeModrsType) || (nCQIPorts != 2 && nCQIPorts != 4) || (txConfig.nLayers != 1) || (txConfig.nCodeWords != 1))
	  {cout<<"[both:]Invalid Simulation setup for Closed Loop Beamforming Transmission Mode. Check myDLTxRxConfig / myULTxRxConfig."<<endl;abort();}
	break;
      case _TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_:
	if((nCQIPorts != 1) ||(nCRSPorts != 1)|| (nDeModPorts != 1) || (CQIrsType != _RSTYPE_CRS_) || (DeModrsType != _RSTYPE_DMRS_) || (txConfig.nLayers != 1) || (txConfig.nCodeWords != 1))
	  {cout<<"[both:]Invalid Simulation setup for Single Port DMRS Transmission Mode. Check myDLTxRxConfig / myULTxRxConfig."<<endl;abort();}
	break;
      case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_:
	if((nCQIPorts != 2) ||(nCQIPorts != nCRSPorts)|| !(nDeModPorts >= 1 && nDeModPorts <= nCQIPorts) || (CQIrsType != _RSTYPE_CRS_) || (DeModrsType != _RSTYPE_DMRS_) || !(txConfig.nLayers>= 1 && txConfig.nLayers <= nCQIPorts) || (txConfig.nLayers != nDeModPorts) || (txConfig.nCodeWords != 1 && txConfig.nCodeWords != 2))
	  {cout<<"[both:]Invalid Simulation setup for Closed Loop 2 Layer DMRS Transmission Mode. Check myDLTxRxConfig / myULTxRxConfig."<<endl;abort();}
	break;
      case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
      case _TRANSMISSION_SCHEME_COMP_:
      case _TRANSMISSION_SCHEME_OLMUMIMO_:
      if((nCQIPorts != 2 && nCQIPorts != 4 && nCQIPorts != 8) ||((nCRSPorts != 1 && nCRSPorts != 2 && nCRSPorts != 4))|| !(nDeModPorts >= 1 && nDeModPorts <= nCQIPorts) || (CQIrsType != _RSTYPE_CSIRS_) || (DeModrsType != _RSTYPE_DMRS_) || !(txConfig.nLayers>= 1 && txConfig.nLayers <= nCQIPorts) || (txConfig.nLayers != nDeModPorts) || (txConfig.nCodeWords != 1 && txConfig.nCodeWords != 2))
	  {
	    cout<<"nCQIPorts : "<<nCQIPorts<<" nDeModPorts : "<<nDeModPorts<<" CQIrsType != _RSTYPE_CSIRS_ "<<(CQIrsType != _RSTYPE_CSIRS_)<<" (DeModrsType != _RSTYPE_DMRS_) : "<<(DeModrsType != _RSTYPE_DMRS_)<<" txConfig.nLayers : "<<txConfig.nLayers<< " txConfig.nCodeWords"<<txConfig.nCodeWords<<endl;
	    cout<<"[both:]Invalid Simulation setup for Closed Loop Max 8 Layer DMRS Transmission Mode. Check myDLTxRxConfig / myULTxRxConfig."<<endl;
	    abort();
	  }
	  break;
      case _TRANSMISSION_SCHEME_NR_CL_:
	if((nCQIPorts != 1 && nCQIPorts != 2 && nCQIPorts != 4 && nCQIPorts != 8 && nCQIPorts != 12 && nCQIPorts != 16 && nCQIPorts != 24 && nCQIPorts != 32) /*||((nCRSPorts != 1 && nCRSPorts != 2 && nCRSPorts != 4))*/|| !(nDeModPorts >= 1 && nDeModPorts <= nCQIPorts) || (CQIrsType != _RSTYPE_CSIRS_) || (DeModrsType != _RSTYPE_DMRS_) || !(txConfig.nLayers>= 1 && txConfig.nLayers <= nCQIPorts) || (txConfig.nLayers != nDeModPorts) || (txConfig.nCodeWords != 1 && txConfig.nCodeWords != 2))
	  {
	    cout<<"nCQIPorts : "<<nCQIPorts<<" nDeModPorts : "<<nDeModPorts<<" CQIrsType != _RSTYPE_CSIRS_ "<<(CQIrsType != _RSTYPE_CSIRS_)<<" (DeModrsType != _RSTYPE_DMRS_) : "<<(DeModrsType != _RSTYPE_DMRS_)<<" txConfig.nLayers : "<<txConfig.nLayers<< " txConfig.nCodeWords"<<txConfig.nCodeWords<<endl;
	    cout<<"[both:]Invalid Simulation setup for Closed Loop NR DMRS Transmission Mode. Check myDLTxRxConfig / myULTxRxConfig."<<endl;
	    abort();
	  }
	break;
      default:
      {cout<<"[both:]Undefined Transmission Mode defined in the Configuration File - myDLTxRxConfig / myULTxRxConfig."<<endl;abort();}
	break;
    }
  }
  else
  {
    cout<<"[both:]Number of Transmit Antennas invalid. Check node antenna count in mySysConfig."<<endl;abort();
  }

}


double getCurrentTime(double currentTime,int currentSymbol,int fftSize ,double samplingFrequencyInHz)
{
  int currentSymbolCP;
  for(int sym_cnt=0;sym_cnt<currentSymbol;sym_cnt++)
  {
    currentSymbolCP=getCPLength(fftSize,sym_cnt);
    currentTime+=(fftSize+currentSymbolCP)*(1/samplingFrequencyInHz);
  }
  return(currentTime);
}

double getEndTime(double currentTime,int currentSymbol,int fftSize ,double samplingFrequencyInHz)
{
  int currentSymbolCP;
  double endTime=currentTime;
  for(int sym_cnt=0;sym_cnt<=currentSymbol;sym_cnt++)
  {
    currentSymbolCP=getCPLength(fftSize,sym_cnt);
    endTime+=(fftSize+currentSymbolCP)*(1/samplingFrequencyInHz);
  }
  return(endTime);
}

void removeCyclicPrefix(Array<cvec> &timeDomainSignal,int fftSize){

  for(int ant_cnt=0;ant_cnt<timeDomainSignal.length();ant_cnt++)
    timeDomainSignal(ant_cnt)=timeDomainSignal(ant_cnt).right(fftSize);
}

void interruptCatch(int /*sigNumber*/){
  char userInput;
  cout << "\nEnter Choice - Exit : Y / N : ";
  userInput = getchar();
  // printPerformance(); //Here we can print the current performance...
  if ((userInput == 'Y') || (userInput == 'y'))
    exit(1);
}

/*! Segmentation Fault Catch Function
 *     */
void segmentationFaultCatch(int /*sigNumber*/){
  
  cout << "[both:] Exception Handling Error (SIGSEGV) !!!"; cout.flush();
  abort();
}

void printScheduledUEInfo(string filename,Array<SchedulerOutput_S> &schedulerOutput,int subFrm_cnt){

  std::ofstream fout(filename.c_str(), std::ios::app);

  fout<<"Subframe : "<<subFrm_cnt<<endl;
  fout<<"Scheduled UEs : "<<schedulerOutput(0).schedulerToTransmitterInfo(0).rxNodeIDs<<endl;
  ivec mcsIndices(schedulerOutput(0).schedulerToTransmitterInfo(0).dataBlkInfo.length());
  ivec rbcount(schedulerOutput(0).schedulerToTransmitterInfo(0).dataBlkInfo.length());
  for(int cnt=0;cnt<schedulerOutput(0).schedulerToTransmitterInfo(0).dataBlkInfo.length();cnt++)
  {
    mcsIndices(cnt)=schedulerOutput(0).schedulerToTransmitterInfo(0).dataBlkInfo(cnt).dataGenInfo.mcsIndices(0);
    rbcount(cnt)=schedulerOutput(0).schedulerToTransmitterInfo(0).dataBlkInfo(cnt).resourceBlocks.length();
  }
  fout<<"RB counts     : "<<rbcount<<endl;
  fout<<"MCS Indices   : "<<mcsIndices<<endl;

  fout.close();
}

/*! \note
 * 1. Post-Processing SINR for Co-Existence Calibration */
double printPostProcessingSINRForCoExistence(/*int rxNodeID,*/Array<cvec> receivedDataSymbols,Array<cvec> transmittedDataSymbols,Array< Array<cvec> > myPrecodedChannelPerRB/*,ivec dataTones,float restOfInterferenceScRxPowerInWatts,ChannelCloud &myChannelCloud*/)
{
  double signalPowerPerSymbol=0; 
  vec sinrPerSymbol;sinrPerSymbol.clear();
//   double receivedPerSymbol=0;
  double interferencePowerPerSymbol=0;
  double sinr=0;
  
  cmat H,X(transmittedDataSymbols.length(),1),Y(receivedDataSymbols.length(),1),temp(receivedDataSymbols.length(),1);
  
  
  for(int symb_cnt=0;symb_cnt<transmittedDataSymbols(0).length();symb_cnt++)
  {
    H=getAcrossArray(myPrecodedChannelPerRB,symb_cnt);
    X.set_col(0,getAcrossArray(transmittedDataSymbols,symb_cnt));
    Y.set_col(0,getAcrossArray(receivedDataSymbols,symb_cnt));
    
    temp=operator*(H,X);
    
    signalPowerPerSymbol=sumsum(sqr(abs(operator*(H,X))));
    interferencePowerPerSymbol=sumsum(sqr(abs(Y-temp)));
    append(sinrPerSymbol,signalPowerPerSymbol/interferencePowerPerSymbol);
  }
  
  sinr=dB(mean(sinrPerSymbol));
  return(sinr);    
  
}




void createFrameStructure(string fileName ,string frameName,int nRBs, double tslots){
   if(currentTaskid==0)
  {
    cout<<"[detl:]Creating subframe structure for "<<nRBs<<" rbs "<<endl;
    
    std::ofstream fout(fileName.c_str(), std::ios::out);
       
    fout<<"frameName=\""<<frameName<<"\""<<endl;
    fout<<"frameStructure=\"{ ";
    fout.close();
    
    for(int rb_cnt=0;rb_cnt<nRBs;rb_cnt++)
    {   
     fout.open(fileName.c_str(),std::ios::app);
     fout<<"{"; 
     for(int slot=0; slot < (int)tslots; slot++)
     {
     
       if(slot == ((int)tslots -1))
       {
	fout<<" nullRB } ";
	break;
       } 
       fout<<" nullRB";
     }

     fout.close();
    }
    fout.open(fileName.c_str(),std::ios::app);
    fout<<"}\" "<<endl;
    fout.close();
  }
  
  #ifdef USING_MPI
  MPI_Barrier(MPI_COMM_WORLD);
  #endif
  
}


void PrintMCSindices(int nodeID,int eNBid,int nLayers, ivec MCSindices, bvec isNewTrans, int subframenumber, vec ppSINRperCWindB, int nRB, bvec isSuccess, ivec tbSize, string MCSlogsFileName)
{
    string dir = MCSlogsFileName;
    MCSlogsFileName = dir +"/eNB_ID_"+ toString(eNBid)+".txt";
    
  std::ofstream MCSlogs;
  
  if(!IsFileExists(MCSlogsFileName))
  {
    createDirectory(dir);
    MCSlogs.open(MCSlogsFileName.c_str(),ios::out);
    MCSlogs<<"% SubframeNumber,\tUE_ID,\t\tnLayers,\tppSINRperCW [dB],\tMCS_Indices,\t\tRBcount,\tTBSize,\t\tAckPerCW \t\t (NOTE: -1 => MCS is not available, -2 => ReTx)\n";
  }
  else
    MCSlogs.open(MCSlogsFileName.c_str(),ios::app);
  
  MCSlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(5)<<nLayers<<",\t\t";
  for(int Cnt=0;Cnt<isNewTrans.length();Cnt++)
  {
    if(isNewTrans(Cnt)==false)
      MCSindices(Cnt)=-2;//Indicates Retransmission
  }
  if(MCSindices.length()==1)
  {
    MCSlogs<<setw(8)<<ppSINRperCWindB(0)<<",-100"<<",\t\t"<<setw(6)<<MCSindices(0)<<",-1";
  }
  else if(MCSindices.length()==2)
  {
    MCSlogs<<setw(8)<<ppSINRperCWindB(0)<<","<< ppSINRperCWindB(1)<<setw(6)<<MCSindices(0)<<","<<MCSindices(1);
  }
  
  if(isSuccess.length()==1)
    MCSlogs<<",\t\t"<<setw(4)<<nRB<<",\t\t"<<setw(6)<<tbSize(0)<<",-1"<<",\t"<<setw(8)<<(int)isSuccess(0)<<",-1";
  else
    MCSlogs<<",\t\t"<<setw(4)<<nRB<<",\t\t"<<setw(6)<<tbSize(0)<<","<<tbSize(1)<<",\t"<<setw(8)<<(int)isSuccess(0)<<","<<(int)isSuccess(1);

  MCSlogs<<endl;
  
}

void printIoTvalues(int serverNodeID, int SubframeNumber,double NIvalueInCurrSubframe, double avgNIvalue, double instantIoT, double avgIoT,string dirName){

  std::ofstream IoTlogs;
  string fileName;
  createDirectory(dirName);
  fileName = dirName+"eNB_ID_"+toString(serverNodeID)+".txt";
  if(!IsFileExists(fileName))
  {
    IoTlogs.open(fileName.c_str(),ios::out);
    IoTlogs<<"% SubframeNumber,\teNB_ID,\t\tInstantNI [dB],\tAvgNI [dB],\tInstantIoT [dB],\tAvgIoT [dB]\n";
  }
  else
    IoTlogs.open(fileName.c_str(),ios::app);

  IoTlogs<<SubframeNumber<<",\t\t\t"<<serverNodeID<<",\t\t"<<dB(NIvalueInCurrSubframe)<<",\t"<<dB(avgNIvalue)<<",\t"<<dB(instantIoT)<<",\t\t"<<dB(avgIoT)<<endl;

}

ModulationScheme_E parseModulationScheme(string fileName,string variableName)
{
  string temp;
  if(!parse(fileName,variableName,temp))
    return(_MODULATION_SCHEME_UNINIT_);
  else
  {
    if(temp=="_MODULATION_SCHEME_BPSK_") return(_MODULATION_SCHEME_BPSK_);
    if(temp=="_MODULATION_SCHEME_QPSK_") return(_MODULATION_SCHEME_QPSK_);
    if(temp=="_MODULATION_SCHEME_16_QAM_") return(_MODULATION_SCHEME_16_QAM_);
    if(temp=="_MODULATION_SCHEME_64_QAM_") return(_MODULATION_SCHEME_64_QAM_);
    
    else
      return(_MODULATION_SCHEME_UNINIT_);
  }
  
}

void SystemInfo_S::checkForSimulationCompatability()
{
 if((enableUserMobility || isRRMEnabled) && isWithAbstraction)
 {
  cout<<"Abstraction mode is enabled"<<endl; 
  cout<<"User mobility and RRM is not supported with Abstraction mode.."<<endl;
//  abort();
 }
 cout<<"isRRMEnabled : "<<isRRMEnabled<<endl;
 if((ulDlMode != _ULDL_MODE_DOWNLINK_) && isRRMEnabled)
 {
   cout<<"Uplink mode is not supported with RRM"<<endl;
   abort();
  }
  
  
  if(( hasFeature("isWithABS") &&  compInfo.isWithCoMP) || (hasFeature("isWithABS") && hasFeature("isMultiOperator")))
  {
    cout<<"This combination of feature is not supported"<<endl;
    abort();
  }
  
// #ifdef USING_MPI
// int np;
// parse("configFiles/myMPI.txt","np",np);
// if(np > 1)
// if(enableUserMobility || isRRMEnabled)
//  {
//   cout<<"MPI is enabled"<<endl;
//   cout<<"User mobility and RRM is not supported with MPI"<<endl;
//   abort(); 
//  }
// #endif
}


double computesubFrameDuration(int fftSize , double subcarrierSpacingInHz , int nOfdmSymbols){
  
  double subFrameDuration;
  int lCP = 160;int sCP = 144;
  int largeCPCount = nOfdmSymbols/7; //assuming large cp is added for every 7th symbol
  int nSamples = fftSize*nOfdmSymbols+largeCPCount*(lCP*fftSize/2048)+(nOfdmSymbols-largeCPCount)*(sCP*fftSize/2048);
  subFrameDuration = (double)nSamples/(fftSize*subcarrierSpacingInHz);
  return subFrameDuration;
}


sBWPartInfo initBWPartInfo(string dlTxRxConfigFileName, NUParams_S &nuParams ){
  
  string apmode;
  AperiodicReportingMode_E apMode;
  parse(dlTxRxConfigFileName, "AperiodicMode",apmode);
  if(apmode=="_APERIODIC_MODE_1_2_") apMode =_APERIODIC_MODE_1_2_;
  else if(apmode=="_APERIODIC_MODE_2_0_") apMode=_APERIODIC_MODE_2_0_;
  else if(apmode=="_APERIODIC_MODE_2_2_") apMode=_APERIODIC_MODE_2_2_;
  else if(apmode=="_APERIODIC_MODE_3_0_") apMode=_APERIODIC_MODE_3_0_;
  else if(apmode=="_APERIODIC_MODE_3_1_") apMode=_APERIODIC_MODE_3_1_;
  else if(apmode=="_APERIODIC_MODE_3_2_") apMode=_APERIODIC_MODE_3_2_;
  else
  {
    cout<<"[both:] Unknown Aperiodic mode for CQI Computation..Default value not possible "<<endl;
    exit(-1);
  }
  
  sBWPartInfo bwPartInfo;
  
  int nRBs=nuParams.dlNRBs;
  bwPartInfo.bwPartPerRB.set_size(nRBs);
  bwPartInfo.subbandIndxPerRB.set_size(nRBs);
  bwPartInfo.subbandIndxPerRB_higherLayerConfigured.set_size(nRBs); // Added for Aperiodic Mode 3_0 and 3_1
  
  Array<ivec> SubbandSizePerBWPart;
  ivec subbandSizeAperiodic;
  
  int numOfBWParts;
  
  if(AperiodicReportingMode_E(apMode) == _APERIODIC_MODE_2_0_ || AperiodicReportingMode_E(apMode) == _APERIODIC_MODE_2_2_)
  { 
    getSubbandsAndBWPartsUESelected(SubbandSizePerBWPart,numOfBWParts,nRBs);
  }
  else
    getSubbandsAndBWParts(SubbandSizePerBWPart,numOfBWParts,nRBs);
  
  int rbIndx=0;
  for(int bp_cnt=0;bp_cnt<numOfBWParts;bp_cnt++)
    for(int sb_cnt=0;sb_cnt<SubbandSizePerBWPart(bp_cnt).length();sb_cnt++)
      for(int rb_cnt=0;rb_cnt<SubbandSizePerBWPart(bp_cnt)(sb_cnt);rb_cnt++,rbIndx++)
      {
	bwPartInfo.bwPartPerRB(rbIndx)=bp_cnt;
	bwPartInfo.subbandIndxPerRB(rbIndx)=sb_cnt;
      }
      //cout<<"bwParts :"<<bwPartInfo.bwPartPerRB<<"subband Index"<<bwPartInfo.subbandIndxPerRB<<endl;
      
      
      // Added for Aperiodic Reporting modes 3_0 , 3_1 , 3_2, 1_2
      if(AperiodicReportingMode_E(apMode) == _APERIODIC_MODE_3_0_ || AperiodicReportingMode_E(apMode) == _APERIODIC_MODE_3_1_ || AperiodicReportingMode_E(apMode) == _APERIODIC_MODE_3_2_ || AperiodicReportingMode_E(apMode) == _APERIODIC_MODE_1_2_)
      {
	subbandSizeAperiodic = getSubbandSizeAperiodic(nRBs);
    if(subbandSizeAperiodic.length()==0)
        bwPartInfo.subbandIndxPerRB_higherLayerConfigured=-1*ones_i(nRBs);
	//cout<<"subbandSizeAperiodic :"<<subbandSizeAperiodic<<endl;
	int rbCount=0;
	int sbIndex=0;
	for(int i=0; i<subbandSizeAperiodic.length(); i++)
	{
	  for(int j=0; j<subbandSizeAperiodic(i); j++)
	  {
	    bwPartInfo.subbandIndxPerRB_higherLayerConfigured(rbCount)=sbIndex;
	    rbCount++;
	  }
	  sbIndex++;
	}
	//cout<<"bwPartInfo.subbandIndxPerRB_higherLayerConfigured :"<<bwPartInfo.subbandIndxPerRB_higherLayerConfigured<<endl;
      }	    
      return bwPartInfo;
}


Array < StrongAndWeakTransmittersInfo_S > getStrongAndWeakTransmittersInfo(ivec rxNodes,SystemInfo_S &mySysInfo, McellSystem &mySys, Array<SchedulerOutput_S> &schedulerOutput, AssociationInfo_S associationInfo,double currentTime){
  
  
  if(mySysInfo.strongInterferersToSimulate<0)
  {
    cout<<"[both:]Invalid value "<<mySysInfo.strongInterferersToSimulate<<" in strongInterferersToSimulate."<<endl;abort();
  }
  
  Array< StrongAndWeakTransmittersInfo_S > strongAndWeakTransmittersInfo;
  
  strongAndWeakTransmittersInfo.set_length(rxNodes.length());
  
  vec rxPowInEachRB;
  ivec txNodesInEachRB;
  ivec strongTxNodes,sortIndx;
  ivec weakTxNodes;
  ivec strongInterfererIndx;
  double rxPowerInDBm;
  
  for(int rxNode_cnt=0;rxNode_cnt<rxNodes.length();rxNode_cnt++)
  {
    strongTxNodes.set_size(0);
    weakTxNodes.set_size(0);
    int rxNodeSrvcIndx=find(mySysInfo.serviceNodes,rxNodes(rxNode_cnt));
    
    int srvrIndx=getServerIndx(associationInfo,rxNodes(rxNode_cnt));
    ivec schToRxInfoIndices=find(schedulerOutput(srvrIndx).rxNodeIDs,rxNodes(rxNode_cnt),true);
    
    //! NOTE : Desired Tx need not be associated Tx
    for(int rx_cnt=0;rx_cnt<schToRxInfoIndices.length();rx_cnt++)
    {
      append(strongAndWeakTransmittersInfo(rxNode_cnt).strongTransmitters, schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoIndices(rx_cnt)).txNodeIDs); 
      append(strongAndWeakTransmittersInfo(rxNode_cnt).strongTransmitters, schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoIndices(rx_cnt)).rsTxNodeIDs); 
    }
    
    if(mySysInfo.strongInterferersToSimulate>0)
    {
    
    //! Collecting transmitters per RB (for both UL/DL)
    //! NOTE : The list will also include desired transmitter
    ivec txNodes=mySys.channelCloud.getTxNodesWithData(currentTime);
    if(!mySys.isCrossLinksEnabled || mySysInfo.ulDlMode==_ULDL_MODE_FDD_)
    {
      if(rxNodeSrvcIndx!=-1)
        txNodes=remove(txNodes,mySysInfo.serviceNodes);
      else
        txNodes=remove(txNodes,mySysInfo.serverNodes);
    }
    
    ivec txNodeIndices=find(mySysInfo.serverNodes,txNodes);
    ivec serverNodesIndicesInList=findNot(txNodeIndices,-1);
    ivec serviceNodesIndicesInList=find(txNodeIndices,-1,true);
        
    vec txPowersInDBm(txNodes.length());
    if(serverNodesIndicesInList.length())
      setInVec(txPowersInDBm,serverNodesIndicesInList,mySysInfo.serverNodeTxPowersInDBm(find(mySysInfo.serverNodes,txNodes(serverNodesIndicesInList))));
    if(serviceNodesIndicesInList.length())
      setInVec(txPowersInDBm,serviceNodesIndicesInList,mySysInfo.serviceNodeTxPowersInDBm(find(mySysInfo.serviceNodes,txNodes(serviceNodesIndicesInList))));
    
    
    for(int tx_cnt=0;tx_cnt<txNodes.length();tx_cnt++)
    {
      if(mySys.isValidLink(txNodes(tx_cnt),rxNodes(rxNode_cnt)))
      {
            bool isDownlink=(find(mySysInfo.serviceNodes,rxNodes(rxNode_cnt))!=-1);

          int srvcNodeID = (isDownlink) ? rxNodes(rxNode_cnt) : txNodes(tx_cnt);
          int srvcNodeIndx = find(associationInfo.serviceNodes,srvcNodeID);
          double linkGainIndB = associationInfo.associatedBeamPairLinkforEachServiceNode(srvcNodeIndx).rsrpIndBm-associationInfo.associatedBeamPairLinkforEachServiceNode(srvcNodeIndx).rspIndBm;
          rxPowerInDBm=txPowersInDBm(tx_cnt)+linkGainIndB;
          append(rxPowInEachRB,rxPowerInDBm);
          append(txNodesInEachRB,txNodes(tx_cnt));
      }
    }
    
    sortIndx=sort_index(rxPowInEachRB);
    
      if(txNodesInEachRB.length()>(mySysInfo.strongInterferersToSimulate+1))
      {
	append(strongTxNodes,txNodesInEachRB(sortIndx.right(mySysInfo.strongInterferersToSimulate+1)));
	append(weakTxNodes,txNodesInEachRB(sortIndx.get(0,sortIndx.length()-mySysInfo.strongInterferersToSimulate-2)));
      }
      else
	append(strongTxNodes,txNodesInEachRB);
      
      append(strongAndWeakTransmittersInfo(rxNode_cnt).strongTransmitters,strongTxNodes);
      append(strongAndWeakTransmittersInfo(rxNode_cnt).weakTransmitters,weakTxNodes);
    }
    
    strongAndWeakTransmittersInfo(rxNode_cnt).strongTransmitters=findUniqueNumbers(strongAndWeakTransmittersInfo(rxNode_cnt).strongTransmitters);
    strongAndWeakTransmittersInfo(rxNode_cnt).weakTransmitters=findUniqueNumbers(strongAndWeakTransmittersInfo(rxNode_cnt).weakTransmitters);
    
    //! Empty the weakTransmitters list, if mySysInfo.modelWeakInterferenceAsNoise is false...
    if(!mySysInfo.modelWeakInterferenceAsNoise)
      strongAndWeakTransmittersInfo(rxNode_cnt).weakTransmitters.set_length(0);
    
    if(strongAndWeakTransmittersInfo(rxNode_cnt).weakTransmitters.length())
      strongAndWeakTransmittersInfo(rxNode_cnt).weakTransmitters=strongAndWeakTransmittersInfo(rxNode_cnt).weakTransmitters(findNot(strongAndWeakTransmittersInfo(rxNode_cnt).weakTransmitters,strongAndWeakTransmittersInfo(rxNode_cnt).strongTransmitters));
    
  }
  
  return(strongAndWeakTransmittersInfo);
}

// void updateSSBReservationInfo(ssBurstSetConfig ssbConfig,FrameReservationInfo_S *cchReservationInfo, double bwpSCS, double currentTimeinmilliseconds)
// {   
//     int ssBurstSetPeriodicity=ssbConfig.getSSBPeriodicity();
//     bool halfFrameIndex= ssbConfig.getHalfFrameIndex();
//     int cnt=floor_i(currentTimeinmilliseconds)%(ssBurstSetPeriodicity*5);
//     cchReservationInfo->cchInfo.isSSBPresent=0;
//     
//     ivec ssbStartSymbols= 5*14*((int)halfFrameIndex)+ssbConfig.getssBurstSetConfig();
// 	double ssbSCS=ssbConfig.getSSBscs();
// 	double nuScaling = bwpSCS/ssbSCS; // posible values = 4 , 2 , 1 , 0.5 
// 	ivec ssbSymbolsWithinPeriodicity(0);
// 	ssbStartSymbols=to_ivec(to_vec(ssbStartSymbols*nuScaling));
// 	ivec ssbEndSymbols=to_ivec(to_vec((ssbStartSymbols+4*nuScaling)-1));
// 	
//     for(int cnt=0;cnt<ssbStartSymbols.length();cnt++)
// 	  append(ssbSymbolsWithinPeriodicity,getIntegers(ssbStartSymbols(cnt),ssbEndSymbols(cnt)));
//     
//     double slotDuration;
// 	if (bwpSCS==15) slotDuration=1;
//     else if (bwpSCS==30) slotDuration=0.5;
//     else if (bwpSCS==60) slotDuration=0.25;
//     else if (bwpSCS==120) slotDuration=0.125;
//     else if (bwpSCS==240) slotDuration=0.0625;
//     
//     double cnt3=cnt+(currentTimeinmilliseconds-floor(currentTimeinmilliseconds));
//     int slotIndx=ceil((double)cnt3/slotDuration);
// 	
// 	ivec slotSymbols=getIntegers(0,13)+slotIndx*14; // assuming the function is called for each slot. Need to change this - Dhiv
// 	ivec temp=find(ssbSymbolsWithinPeriodicity,slotSymbols);
// 	ivec ssbSymbolsInTheSlot=find(temp!=-1);
// 	if (ssbSymbolsInTheSlot.length()!=0)
//         {
// 	    cchReservationInfo->cchInfo.isSSBPresent=1;
//             cchReservationInfo->cchInfo.SSBNuScaling=ssbSCS/bwpSCS;
//             cchReservationInfo->cchInfo.SSBOffsetFromPRB0=ssbConfig.getSSBoffsetFromPRB0inKHz()/bwpSCS;
//             cchReservationInfo->cchInfo.SSBSubcarrierCount=240.0*cchReservationInfo->cchInfo.SSBNuScaling;
//             cchReservationInfo->cchInfo.SSBSymbols=ssbSymbolsInTheSlot;
// 	
//             cout<<"Reserving for SSB in symbols : "<<ssbSymbolsInTheSlot<<endl;
//             
// 	    int ssbStartRB=floor_i((ssbConfig.getSSBoffsetFromPRB0inKHz()/bwpSCS)/12.0);
// 	    int ssbEndRB=ceil_i((ssbConfig.getSSBoffsetFromPRB0inKHz()/bwpSCS)/12.0+(20.0*cchReservationInfo->cchInfo.SSBNuScaling)-1.0);
// 	    
// // 	    append(cchReservationInfo->reservedVRBs,getIntegers(ssbStartRB,ssbEndRB));
// 	}
//       
//     /*if(ssbConfig.enablePBCH && subframeNumber%10) cchReservationInfo->ssbConfig.enablePBCH=false;
//      *      if(ssbConfig.enablePssSss && subframeNumber%5) cchReservationInfo->ssbConfig.enablePssSss=false;
//      *      
//      *      if(cchReservationInfo->ssbConfig.enablePBCH)
//      *      {
//      *	if(nRBS%2==0)
//      *	{
//      *	  for(int rb_cnt=nRBS/2-3;rb_cnt<nRBS/2+3;rb_cnt++)
//      *	    append(cchReservationInfo->cchtonesPerRB(rb_cnt),54);
// }
// else
// {
// append(cchReservationInfo->cchtonesPerRB((nRBS-1)/2-3),55);
// for(int rb_cnt=(nRBS-1)/2-2;rb_cnt<(nRBS-1)/2+3;rb_cnt++)
//     append(cchReservationInfo->cchtonesPerRB(rb_cnt),54);
// append(cchReservationInfo->cchtonesPerRB((nRBS-1)/2+3),56);
// }
// 
// }
// if(cchReservationInfo->ssbConfig.enablePssSss)
// {
// if(nRBS%2==0)
// {
// for(int rb_cnt=nRBS/2-6;rb_cnt<nRBS/2+6;rb_cnt++)
//     append(cchReservationInfo->cchtonesPerRB(rb_cnt),57);
// }
// else
// {
// append(cchReservationInfo->cchtonesPerRB((nRBS-1)/2-6),58);
// for(int rb_cnt=(nRBS-1)/2-2;rb_cnt<(nRBS-1)/2+6;rb_cnt++)
//     append(cchReservationInfo->cchtonesPerRB(rb_cnt),57);
// append(cchReservationInfo->cchtonesPerRB((nRBS-1)/2+6),59);
// }
// 
// }
// */
//     
// }


void updateCCHReservationInfo(CCHInfo_S cchInfo, FrameReservationInfo_S* cchReservationInfo, double bwpSCS, double currentTimeinmilliseconds, int nRBS, int subframeNumber, bool isDownlink, Array< ivec > dlPrbMapping){
    
    cchReservationInfo->clear();
    cchReservationInfo->cchInfo=cchInfo;
    cchReservationInfo->prbs=getIntegers(0,nRBS-1);
    cchReservationInfo->cchtonesPerRB.set_length(nRBS);
    cchReservationInfo->dlPrbMapping = dlPrbMapping;
    
    
    if(isDownlink)
    {
        if(cchInfo.pdcchCount>0)
        {
            for(int rb_cnt=0;rb_cnt<nRBS;rb_cnt++)
                append(cchReservationInfo->cchtonesPerRB(rb_cnt),50+cchInfo.pdcchCount-1); // Assuming pdcchCount tones are 50,51,52,53 as in the rbStructure file
                
                if(cchInfo.pdcchCount>=4 && nRBS>10) {cout<<"[both:] \n Error : pdcchCount "<<cchInfo.pdcchCount<<" is not valid "<<endl; abort();}
                else if(cchInfo.pdcchCount>4 && nRBS<10) {cout<<"[both:] \n Error : pdcchCount "<<cchInfo.pdcchCount<<" is not valid "<<endl; abort();}
        }      
        
        if(cchReservationInfo->cchInfo.enableEPDCCH)
            for(int rb_cnt=0;rb_cnt<cchReservationInfo->reservedVRBsForEpdcch.length();rb_cnt++)
                append(cchReservationInfo->cchtonesPerRB(cchReservationInfo->reservedVRBsForEpdcch(rb_cnt)),100);
            
//             if(ssbConfig.isInitialized())	    
//                 updateSSBReservationInfo( ssbConfig,cchReservationInfo,bwpSCS,currentTimeinmilliseconds);
    }
	if(cchReservationInfo->cchInfo.enableShortPUCCH)
	  append(cchReservationInfo->cchtonesPerRB(nRBS-1),60);
	if(cchReservationInfo->cchInfo.enablelongPUCCH)
	  append(cchReservationInfo->reservedVRBs,getIntegers(0,2));
	
	    
	    if(cchReservationInfo->cchInfo.srsCount>0)
	    {
	      if(cchReservationInfo->cchInfo.pdcchCount>0) {cout<<"CCh Reservation Error.."<<endl;abort();}
		
	      cchReservationInfo->reservedSymbols=getIntegers(14-cchReservationInfo->cchInfo.srsCount+1,14);
	    }
            
	    

}

void setREIndxForTM3Precoding(Size_S rbSize, DataBlkInfo_S &dataBlkInfo)
{
  ivec myRBs=dataBlkInfo.resourceBlocks;
  imat allocationStructure(0,0);
  
  dataBlkInfo.reIndxForPrecoding.set_length(0);
  allocationStructure=dataBlkInfo.dataGenInfo.rbInfo.logicalSubFrameStructure.get_rows(myRBs(0)*rbSize.numberOfSubcarriers,myRBs(0)*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1);
  for(int rb_cnt=1;rb_cnt<myRBs.length();rb_cnt++)
    appendVertical(allocationStructure,dataBlkInfo.dataGenInfo.rbInfo.logicalSubFrameStructure.get_rows(myRBs(rb_cnt)*rbSize.numberOfSubcarriers,myRBs(rb_cnt)*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1));
  
  imat dataIndxingStructure=allocationStructure;
  
  ivec dataREs=find(allocationStructure,0,true);
  ivec reIndx=getIntegers(0,dataREs.length()-1);
  setInMatrix(dataIndxingStructure,dataREs,reIndx);
  
  for(int rb_cnt=0;rb_cnt<myRBs.length();rb_cnt++)
  {
    imat rbStructure=allocationStructure.get_rows(rb_cnt*rbSize.numberOfSubcarriers,rb_cnt*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1);
    ivec localDataREs=find(rbStructure,0,true);
    rbStructure=dataIndxingStructure.get_rows(rb_cnt*rbSize.numberOfSubcarriers,rb_cnt*rbSize.numberOfSubcarriers+rbSize.numberOfSubcarriers-1);
    ivec dataREIndx=getFromMatrix(rbStructure,localDataREs);
    append(dataBlkInfo.reIndxForPrecoding,dataREIndx);
  }
}



void getTDDScaling(SystemInfo_S& mySysInfo, vec& dlTDDScaling, vec& ulTDDScaling)
{
    Array <ivec> CellConfig(mySysInfo.nuIDs.length());
    Array <ivec> SFI;
    parse(mySysInfo.myConfigFiles.nuConfigFileName,"CellConfig",CellConfig);
    parse(mySysInfo.myConfigFiles.nuConfigFileName,"SFI",SFI);
    
    dlTDDScaling=ones(mySysInfo.nuIDs.length());
    ulTDDScaling=ones(mySysInfo.nuIDs.length());
    
    
    for(int nuIndx=0;nuIndx<mySysInfo.nuIDs.length();nuIndx++)
    {
        if (CellConfig(nuIndx).length()!=0)
        {
        double nSymbols=mySysInfo.nuConfig(nuIndx).CellConfigPeriodicityInSlots*14.0;
        double nDlSymbols=CellConfig(nuIndx)(0)*14.0;
        double nUlSymbols=CellConfig(nuIndx)(2)*14.0;
        if(SFI(nuIndx).length()!=0)
        {
            int nSFISlots=mySysInfo.nuConfig(nuIndx).CellConfigPeriodicityInSlots-CellConfig(nuIndx)(0)-CellConfig(nuIndx)(2);
            nDlSymbols+=(double)(nSFISlots*count(SFI(nuIndx),1));  
            nUlSymbols+=(double)(nSFISlots*count(SFI(nuIndx),0));  
        }
        else
        {
            nDlSymbols+=(double)CellConfig(nuIndx)(1);
            nUlSymbols+=(double)CellConfig(nuIndx)(3);
        }
        
        dlTDDScaling(nuIndx) = nSymbols/nDlSymbols;
        ulTDDScaling(nuIndx) = nSymbols/nUlSymbols;
        
        }
    }
}


// ReAssociationInfo_S performBeamBasedAssociation(int rxNodeID, ivec txNodelist, McellSystem& mySys, AssociationInfo_S& associationInfo)
// {
//     ivec nTxBeamsPerServerNode(0);
//     int nRxBeams = -1;
//     ChannelMatrix_S channelMatrix = mySys.generateChannelForAnalogBeams(rxNodeID,0,txNodelist,nTxBeamsPerServerNode,nRxBeams);
//     ReAssociationInfo_S reassociationInfo;
//     reassociationInfo.RSRPPerTxNode.set_length(txNodelist.length());
//     
//     // RSRP calculation for Port 0 -> assuption is Port 0 is Tx antenna 0
//     vec RSRP = zeros(channelMatrix.txNodeIDs.length());
//     
//     for(int srvrCnt = 0; srvrCnt <txNodelist.length();srvrCnt++)
//         reassociationInfo.RSRPPerTxNode(srvrCnt) = zeros(nRxBeams,nTxBeamsPerServerNode(srvrCnt));
//     
//     for(int txCnt = 0; txCnt <channelMatrix.txNodeIDs.length();txCnt++)
//     {
//         int nodeID = channelMatrix.txNodeIDs(txCnt)/10000;
//         int txBeamID = mod(channelMatrix.txNodeIDs(txCnt),10000)/100;
//         int rxBeamID = mod(channelMatrix.txNodeIDs(txCnt),100);
//         
//         int txAntNo = 0;
//         for(int rxAntCnt = 0;rxAntCnt <channelMatrix.channels_forRSRP(txCnt)(txAntNo).length();rxAntCnt ++)
//         {
//             double tempAvgTap = 0.0;
//             for(int tapCnt = 0;tapCnt <channelMatrix.channels_forRSRP(txCnt)(txAntNo)(rxAntCnt).length();tapCnt ++)
//             {
//                 tempAvgTap += sqr(abs(channelMatrix.channels_forRSRP(txCnt)(txAntNo)(rxAntCnt)(tapCnt)));
//             }
//             RSRP(txCnt) += tempAvgTap;
//         }
//         RSRP(txCnt) /= channelMatrix.channels_forRSRP(txCnt)(txAntNo).length();
//         // converting into dB
//         RSRP(txCnt) = 10*log10(abs(RSRP(txCnt)));
//         reassociationInfo.RSRPPerTxNode(find(txNodelist,nodeID))(rxBeamID,txBeamID) = RSRP(txCnt);
//     }
//     int maxIndx = max_index(RSRP);
//     reassociationInfo.updatedCellID = channelMatrix.txNodeIDs(maxIndx)/10000;
//     reassociationInfo.maxRSRP = RSRP(maxIndx);
//     associationInfo.reassociate(rxNodeID,reassociationInfo.updatedCellID);
//     int rx_cnt=find(associationInfo.serviceNodes,rxNodeID);
//     associationInfo.associatedBeamPairLinkforEachServiceNode(rx_cnt).srvrBeamID = mod(channelMatrix.txNodeIDs(maxIndx),10000)/100;
//     associationInfo.associatedBeamPairLinkforEachServiceNode(rx_cnt).srvcBeamID = mod(channelMatrix.txNodeIDs(maxIndx),100);
//     associationInfo.associatedBeamPairLinkforEachServiceNode(rx_cnt).rsrpIndBm=RSRP(maxIndx);
//     associationInfo.associatedBeamPairLinkforEachServiceNode(rx_cnt).rspIndBm=0; // need to set app value - Dhiv
//     return reassociationInfo;
// }
// 
// void performBeamBasedAssociation(McellSystem& mySys, AssociationInfo_S& associationInfo, int nRBs, string linkTableFileName)
// {  
//     vec serverNodeTransmitPowerInDBm; 
//     if(parse(linkTableFileName,"serverNodeTransmitPowerInDBm",serverNodeTransmitPowerInDBm)==false)
//     {
//         cout<<"[both:]Error... Unable to load serverNodeTransmitPowerInDBm from the file in performBeamBasedAssociation()..."<<endl;	abort();
//     }
//     
//     // FIXME txSubcarrierPowerindBm is not used for SINR computation, it is not correct for pico simulations where the TxPower is different
//     //   int nSubcarriers =  nRBs * 12;
//     //   vec txSubcarrierPowerindBm = serverNodeTransmitPowerInDBm - 10*log10(nSubcarriers);
//     
//     ivec updatedCellIds(associationInfo.serviceNodes.length());
//     ivec initCellIds = associationInfo.associatedServerNodesForEachServiceNode;
//     Array< Array< vec> > RSRPdesired;
//     RSRPdesired.set_length(associationInfo.serviceNodes.length());
//     
//     
//     mat linkGain = mySys.channelCloud(0).getLinkSpecificChannelGainInDB(associationInfo.serverNodes,associationInfo.serviceNodes,true);
//     string tempFileName="./Results/linkGainFile.txt";
//     std::ofstream fout(tempFileName.c_str(), std::ios::out);
//     fout<<linkGain;
//     fout.close();
//     
//     ivec txNodelist = associationInfo.serverNodes;
//     
//     Array< Array< mat > > RSRPwithRandomInterfererBeams(associationInfo.serviceNodes.length());
//     int nTrials=1;
//     
//     for(int rx_cnt=0; rx_cnt<associationInfo.serviceNodes.length(); rx_cnt++)
//     {
//         RSRPdesired(rx_cnt).set_length(txNodelist.length());
//         for(int tx_cnt=0; tx_cnt<txNodelist.length(); tx_cnt++)
//             RSRPdesired(rx_cnt)(tx_cnt)=zeros(nTrials);
//     }
//     for(int rx_cnt=0; rx_cnt<associationInfo.serviceNodes.length(); rx_cnt++)
//     {     
//         ReAssociationInfo_S reassociationInfo=performBeamBasedAssociation(associationInfo.serviceNodes(rx_cnt),txNodelist,mySys,associationInfo);
//         updatedCellIds(rx_cnt) = reassociationInfo.updatedCellID;
//         int associatedTxIndx = find(txNodelist,updatedCellIds(rx_cnt));
//         RSRPdesired(rx_cnt)(associatedTxIndx) = reassociationInfo.maxRSRP;
//         RSRPwithRandomInterfererBeams(rx_cnt) = reassociationInfo.RSRPPerTxNode;
//     }
//     
//     for(int rx_cnt=0; rx_cnt<associationInfo.serviceNodes.length(); rx_cnt++)
//     {
//         
//         ivec interfererIndices = findNot(txNodelist,updatedCellIds(rx_cnt));
//         
//         for(int interfererCnt = 0; interfererCnt <interfererIndices.length();interfererCnt++)
//         {
//             int nodeIndx = interfererIndices(interfererCnt);
//             int rxBeamID = associationInfo.associatedBeamPairLinkforEachServiceNode(rx_cnt).srvcBeamID;
//             
//             for(int trial_cnt=0;trial_cnt<nTrials;trial_cnt++)
//             {
//                 //Picking a random UE and his corresponding beam
//                 // 	int randomScheduleUE = randPick(associationInfo.associatedServiceNodesPerServerNode(interfererIndices(interfererCnt)),1)(0);
//                 // 	int rndSchUEIndx=find(associationInfo.serviceNodes,randomScheduleUE);
//                 // 	int randomTxBeamID=associationInfo.associatedBeamPairLinkforEachServiceNode(rndSchUEIndx).txBeamID;
//                 int randomTxBeamID=randi(0,RSRPwithRandomInterfererBeams(rx_cnt)(nodeIndx).cols()-1);
//                 
//                 RSRPdesired(rx_cnt)(interfererIndices(interfererCnt))(trial_cnt) = RSRPwithRandomInterfererBeams(rx_cnt)(nodeIndx)(rxBeamID,randomTxBeamID);
//             }
//         }
//     }  
//     //   cout<<"RSRP in beam sweeping : "<<RSRPdesired<<endl;  
//     //   cout<<"initial servernodes = "<<initCellIds<<endl;
//     //   cout<<"Reassoc servernodes = "<<updatedCellIds<<endl;
//     //   cout<<"Changed num = "<<findNot((initCellIds-updatedCellIds),0).length()<<endl;
//     
//     printReAssociation(associationInfo,linkTableFileName,RSRPdesired);
//     //   cout<<"RSRPdesired:"<<RSRPdesired<<endl;
//     
//     //   string tempFileRSRP="./Results/RSRPFile.txt";
//     //   std::ofstream fout1(tempFileRSRP.c_str(), std::ios::out);
//     //   mat varRSRPmat(RSRPdesired.length(),RSRPdesired(0).length());  
//     //   for(int i=0;i<RSRPdesired.length();i++)
//     //   {
//     //     varRSRPmat.set_row(i,RSRPdesired(i));
//     //   }
//     //   fout1<<varRSRPmat;
//     //   fout1.close();
// }

int getServerIndx(AssociationInfo_S& associationInfo,int nodeID)
{
    int srvrIndx=find(associationInfo.serverNodes,nodeID);
    if(srvrIndx==-1)
    {
        int associatedNode=associationInfo.associatedServerNodesForEachServiceNode(find(associationInfo.serviceNodes,nodeID));
        srvrIndx=find(associationInfo.serverNodes,associatedNode);
    }
    return srvrIndx;
}

