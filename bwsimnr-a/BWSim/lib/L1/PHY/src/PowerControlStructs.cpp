#include "../include/PowerControlStructs.h"

// double getInitialPreviousTPC(double deltaPrampupRequested, double deltaMsg2, PoAlphaSet_S ptemp) // use this function for j==0 (i.e. msg3) as pcfValueForPowerControl=getInitialPreviousTPC as initial value outside frame count loopb4 using applypower control
// {
//   double calpower=calculateDesiredPowerInWattsInd(ptemp.poIndBm,ptemp.alpha, ptemp.pcReqParaByMeasure.nRB, (ptemp.pcReqParaByMeasure.RSP-ptemp.pcReqParaByMeasure.RSRP), ptemp.pcReqParaByMeasure.deltaTF_, deltaMsg2);
//   double deltaP_rampup=min( max(0, {dB(ptemp.pcReqParaByMeasure.txPower*600)-calpower}), deltaPrampupRequested);
//   double f0initial = deltaP_rampup+deltaMsg2; // initial value of pcfValueForPowerControl (i.e. previous values)
//   return f0initial;
// }

// // //! deltaTF Computation //////////  for PUCCH
// double getDeltaTFforPUCCH(PcControlReqInfo_S PCcontrolParamForPUCCH, PcReqParaByMeasure_S PUCCHParamByUE/*, SchedulerToTransmitterInfo_S schedulerToTransmitterInfo*/)
// {
//   double Npucch_ref, deltaTF, BPRE;
//   int nREs=0;
//   
//   nREs = PUCCHParamByUE.nRB*PCcontrolParamForPUCCH.Npucch_symb*12;
// 
// //   Array<imat> rbStructurePerRB= getMergedRBStructures(schedulerToTransmitterInfo.frameStructure,schedulerToTransmitterInfo.dataBlkInfo(0).dataGenInfo.rbInfo.rsInfo.rsPorts,schedulerToTransmitterInfo.dataBlkInfo(0).resourceBlocks,false,true,schedulerToTransmitterInfo.frameReservationInfo);
// //   for(int rb_cnt=0;rb_cnt<rbStructurePerRB.length();rb_cnt++)
// //     nREs+=count(rbStructurePerRB(rb_cnt),0);
//   
//   BPRE=(PUCCHParamByUE.nHarqAck+PUCCHParamByUE.Ocsi+PUCCHParamByUE.Osr+PUCCHParamByUE.Ocrc)/nREs;
//   
//   if(PUCCHParamByUE.formatIndex==0||1)
//   {
//      if(PUCCHParamByUE.formatIndex==0)
// 	Npucch_ref=1; 
//       else if(PUCCHParamByUE.formatIndex==2)
// 	Npucch_ref = PCcontrolParamForPUCCH.Nslot_symb;
//       
//       deltaTF= dB(Npucch_ref/PCcontrolParamForPUCCH.Npucch_symb);     
//   }
// 
//   else if(PUCCHParamByUE.formatIndex==2||3||4 && PUCCHParamByUE.UCI <= 11)
//       deltaTF=dB(PCcontrolParamForPUCCH.k1*(PUCCHParamByUE.nHarqAck+PUCCHParamByUE.Ocsi+PUCCHParamByUE.Osr)/nREs);
// 
//   else if(PUCCHParamByUE.formatIndex==2||3||4 && PUCCHParamByUE.UCI > 11)
//       deltaTF=dB(POW(2,PCcontrolParamForPUCCH.k2*BPRE)-1);
// 
//   else  
//     cout<< "Choice other than 1, 2 and 3 " <<endl;
// 
//   return deltaTF;
// }

//! deltaTF Computation //////////  for data
// double getDeltaTF(SchedulerToTransmitterInfo_S &schedulerToTransmitterInfo)
// {
//   int totalInfoBits=0;
//   for(int cwd_cnt=0;cwd_cnt<schedulerToTransmitterInfo.dataBlkInfo(0).dataGenInfo.nCodeWords;cwd_cnt++)
//   {
//     int tbindex=schedulerToTransmitterInfo.sourceAndSink->lteTables.transportBlockMCSMapping.transportBlockIndexPDSCH(schedulerToTransmitterInfo.dataBlkInfo(0).dataGenInfo.mcsIndices[cwd_cnt]);
//     int tbSize=schedulerToTransmitterInfo.sourceAndSink->lteTables.getTBSizeForAllLayersCombined(tbindex,schedulerToTransmitterInfo.dataBlkInfo(0).resourceBlocks.length(),getNumLayersPerCodeWord(schedulerToTransmitterInfo.dataBlkInfo(0).dataGenInfo.nCodeWords,schedulerToTransmitterInfo.dataBlkInfo(0).dataGenInfo.nLayers)(cwd_cnt));
//     int tbSizeWithCRC=tbSize+24;
//     ivec codeBlocksSizes=schedulerToTransmitterInfo.sourceAndSink->lteTables.getCodeBlockSizes(tbSizeWithCRC);
//     totalInfoBits+=sum(codeBlocksSizes);
//   }
//   int nREs=0;
//   Array<imat> rbStructurePerRB= getMergedRBStructures(schedulerToTransmitterInfo.frameStructure,schedulerToTransmitterInfo.dataBlkInfo(0).dataGenInfo.rbInfo.rsInfo.rsPorts,schedulerToTransmitterInfo.dataBlkInfo(0).resourceBlocks,false,true,schedulerToTransmitterInfo.frameReservationInfo);
//   for(int rb_cnt=0;rb_cnt<rbStructurePerRB.length();rb_cnt++)
//     nREs+=count(rbStructurePerRB(rb_cnt),0);
//   
//   double BPRE=(double)totalInfoBits/(double)nREs;
//   double betaPUSCH=1;
//   double Ks=schedulerToTransmitterInfo.dataBlkInfo(0).KsForPowerControl;
//   double deltaTF=0;
//   if(schedulerToTransmitterInfo.dataBlkInfo(0).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_CL_SM_)
//     	      Ks=0;
//   
//   if(Ks==0)
//     deltaTF=0;
//   else
//     deltaTF=10*log10((pow(2.0,BPRE*Ks)-1.0)*betaPUSCH);
//   return  deltaTF;
//   
// }

// double txPowerRACH(int PREAMBLE_POWER_RAMPING_COUNTER, double preambleInitialReceivedTargetPower, double  DELTA_PREAMBLE, double powerRampingStep) // RACH (re)transmission
// {
//   double txPowerForRach = preambleInitialReceivedTargetPower + DELTA_PREAMBLE + (PREAMBLE_POWER_RAMPING_COUNTER-1)*powerRampingStep;
//   return txPowerForRach;
// }

void setvalues(double p0,double PLCompensation, double pcfValueForPowerControl1,vector<PoAlphaSet_S> &tpoAlphaSet, int factor_cnt) //outside somewhere to have all values then one will be used as per index in do_ulpowcont, this is to set values
{  
  PoAlphaSet_S pa(p0,PLCompensation);// here p0 & PLCompensation are from config files
  pa.pcfValueForPowerControl=pcfValueForPowerControl1;
  
  if (tpoAlphaSet.size()<=factor_cnt)
    tpoAlphaSet.push_back(pa);    
}

/*! \fn getTPCbasedOnCmdFeild(...)
* \brief Generate the instantenous CLPC correction value at UE.
* \param [in,out] TPCCmdFeild ---> TPC command field by BS 
* \param [in,out] isAbsTPC --> to check for absolute and accumulated mode.
* \param [out] TPCInDB --->final instantenous adjustment of CLPC.
* \return returns the calculated value of TPCInDB
*/

// double PcControlReqInfo_S::getTPCbasedOnCmdFeild(bool isAbsTPC, int TPCCmdFeild) //select tpc based on command feils
//   {
//     double TPCInDB;
//       if(isAbsTPC)
// 	TPCInDB = this->AbsoluteTPC_ValuesForIndexinDB(TPCCmdFeild);
//       else
// 	TPCInDB = this->AccumulateTPC_ValuesForIndexinDB(TPCCmdFeild);
//     
//     return(TPCInDB);
//   }

/*! \fn setTPCbasedvalue()
* \brief Set the final TPC value in UE based on Absolute ans Accumulated mode.
* \param [in] TPCCmdFeild ---> TPC command field by BS 
* \param uplinkPowerControlCorrectionValInDB --> instant correction value based on TPC command.
* \param [in] getTPCbasedOnCmdFeild(...) ---> function to generate uplinkPowerControlCorrectionValInDB
* \param [in] isAbsTPC --> to check for absolute and accumulated mode.
* \param [in] pcfValueForPowerControl ---> previously accumulated TPC correction
* \param [in,out] fValueForPowerControl --->final adjustment of CLPC.
* \return returns the calculated value of fValueForPowerControl
*/

double PoAlphaSet_S::setTPCbasedvalue() 
{
  double fValueForPowerControl;
  if(this->powerControlReqInfo.TPCCmdFeild!=-1) 
  {
    this->powerControlReqInfo.uplinkPowerControlCorrectionValInDB=  this->powerControlReqInfo.getTPCbasedOnCmdFeild(this->powerControlReqInfo.isAbsTPC, this->powerControlReqInfo.TPCCmdFeild);

    if(this->powerControlReqInfo.isAbsTPC) //check for absolute
      fValueForPowerControl=this->powerControlReqInfo.uplinkPowerControlCorrectionValInDB; 
    else
    {       
      if(this->powerControlReqInfo.resetTPC)   // check for reset TPC command  
	pcfValueForPowerControl=0;
      
      fValueForPowerControl=pcfValueForPowerControl+this->powerControlReqInfo.uplinkPowerControlCorrectionValInDB;         
    }    
  }
  else
    fValueForPowerControl=pcfValueForPowerControl; //if no TPC command detected
    
    return fValueForPowerControl;   
}

/*std::ostream &operator<<(std::ostream &os, const PoAlphaSet_S &v)
 * { *
 * os << "PoAlphaSet:  P0:"<<v.poIndBm<<" alpha:"<<v.alpha<<endl ; 
 * os << "finalTxULPowerPerSubCarrierInWatts: "<<v.pcReqParaByMeasure.finalTxULPowerPerSubCarrierInWatts<<" in dBw "<<dB(v.pcReqParaByMeasure.finalTxULPowerPerSubCarrierInWatts)+30+dB(12)+dB(v.pcReqParaByMeasure.nRB)<<endl;
 * return os; 
 * }*/

double calculateDesiredPowerInWattsInd(double poIndBm,double alpha, int BWinRB, double PL, double MCSPowAdjust, double TPCcommad) // MCSPowAdjust is deltaTF component & "=0" for SRS Power Control
{
  double  desiredPowerInWatts_;
  desiredPowerInWatts_=inv_dB( poIndBm-30 + dB(BWinRB) + alpha*PL + MCSPowAdjust + TPCcommad);
  return  desiredPowerInWatts_;  
}

double PoAlphaSet_S::calculateDesiredPowerInWatts(int BWinRB, double PL, double MCSPowAdjust, double TPCcommad) 
{
  return calculateDesiredPowerInWattsInd(poIndBm,alpha,BWinRB,PL,MCSPowAdjust,TPCcommad);
}

void PoAlphaSet_S::uplinkPowerControl(/*PoAlphaSet_S &PoAlphaSetObject, PcReqParaByMeasure_S &pcReqParaByMeasure,*/ double time1)
{
  double maxTxPowerInWatts;
  double pathlossindB;
  double uplinkPowerPerSubCarrierInWatts;
  double desiredPowerInWatts;
  double txSubcarrierPowerInDBmPerAntenna;
  double fPUCCHAdjust;
  
  fPUCCHAdjust=this->powerControlReqInfo.fPUCCH_Adjust; //fPUCCHAdjust is for control channel & "=0" for data
  this->pcReqParaByMeasure.Mrb=pow(2,(this->pcReqParaByMeasure.mu/*-1*/))*this->pcReqParaByMeasure.nRB;
  int M1=  this->pcReqParaByMeasure.nRB;
  int M=  this->pcReqParaByMeasure.Mrb;

//     cout<<"BANDWIDTH "<< M << endl;
  
  txSubcarrierPowerInDBmPerAntenna=this->pcReqParaByMeasure.txPowerMax/*-10*log10(M*12)*/; 
  pathlossindB= (this->pcReqParaByMeasure.RSP - this->pcReqParaByMeasure.RSRP);
  this->pcReqParaByMeasure.pathlossindB=pathlossindB;
  //   cout<<"pathlossindB "<< pathlossindB << endl;
  
  //   if(this->pcReqParaByMeasure.pcMethod == 1) 
  //   { 
  double fValueForPowerControl = /*PoAlphaSetObject.*/setTPCbasedvalue();
  //   cout<<"fValueForPowerControl "<<fValueForPowerControl<<endl;
  
  maxTxPowerInWatts=inv_dB(txSubcarrierPowerInDBmPerAntenna-30)*M1*12;  
//   cout<<" maxTxPowerInDB"<< " " << dB(maxTxPowerInWatts) << endl;

  desiredPowerInWatts=/*PoAlphaSetObject.*/calculateDesiredPowerInWatts(M, pathlossindB, this->pcReqParaByMeasure.deltaTF_, fValueForPowerControl)*inv_dB(fPUCCHAdjust);
// //     cout<<" desiredPowerInDB"<< " " << dB(desiredPowerInWatts)<< " pathlossindB \t" << pathlossindB <<  "\t deltaTF_" <<  this->pcReqParaByMeasure.deltaTF_ << "tpc \t" <<fValueForPowerControl<<endl;
  
  if(desiredPowerInWatts <= maxTxPowerInWatts) // Updating f_PUSCH(i) only if the max power bound is satisfied
  {
    /*PoAlphaSetObject.*/pcfValueForPowerControl=fValueForPowerControl;
    uplinkPowerPerSubCarrierInWatts=desiredPowerInWatts/(M*12);       
  }
  else //using f_PUSCH(i-1) for power control and not updating
  {
    desiredPowerInWatts=/*PoAlphaSetObject.*/calculateDesiredPowerInWatts(M, pathlossindB, this->pcReqParaByMeasure.deltaTF_, /*PoAlphaSetObject.*/pcfValueForPowerControl)*inv_dB(fPUCCHAdjust);
    uplinkPowerPerSubCarrierInWatts =min(desiredPowerInWatts, maxTxPowerInWatts)/(M*12);		
  } 
  //   }
  this->pcReqParaByMeasure.finalTxULPowerPerSubCarrierInWatts=uplinkPowerPerSubCarrierInWatts;  
  this->pcReqParaByMeasure.desiredPowerInWattsForNR=desiredPowerInWatts;
  //     cout<<*this<<endl;
}