#ifndef PowerControlStructs_H
#define PowerControlStructs_H

#include "FrameScheduler.h"
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
// #include "itpp/itcomm.h"
// #include "../newcheck/bwsimnr/BWSim/lib/PrachSupport/include/PRACH_CONST.h"
#include <list>

// using namespace itpp;
using namespace std;

struct PcControlReqInfo_S // contains info from control channel (that may change or come suddenly with time) like TPC, Abs or Acc. mode etc
{
  double time=-1;
  bool isAbsTPC; //bool for TPC accumulation enabing
  bool resetTPC; //bool for when to reset PCOL paras
  double uplinkPowerControlCorrectionValInDB;
  int TPCCmdFeild;
  
  vec AbsoluteTPC_ValuesForIndexinDB;
  vec AccumulateTPC_ValuesForIndexinDB;
 
  // parameters related to PUCCH power control only
  double Nslot_symb, Npucch_symb, k1, k2, fPUCCH_Adjust; //fPUCCH_Adjust is for control channel & "=0" for data
  
  PcControlReqInfo_S():isAbsTPC(0), resetTPC(0), uplinkPowerControlCorrectionValInDB(0), fPUCCH_Adjust(0), TPCCmdFeild(-1)
  {
    AbsoluteTPC_ValuesForIndexinDB = "-4,-1,1,4";  // TPC values Index {0, 1, 2, 3}
    AccumulateTPC_ValuesForIndexinDB = "-1,0,1,3";  
  }
  
  void setPCreqParameters(bool isAbsTPC_, bool resetTPC_,double TPCCmdFeild_, double timeT) //  run func with obj of vector of this struct like v.pusback to store these values even if no doULpc/UL
  { // this is temp func, need to change to get i/ps from schtoTxInfo
    isAbsTPC = isAbsTPC_;
    resetTPC = resetTPC_;
    TPCCmdFeild= TPCCmdFeild_;
    time = timeT;
  }
  
  void setParamByControlForPUCCH(double tNslot_symb, double tNpucch_symb, double tk1, double tk2, double tfPUCCH_Adjust)
  {
    Nslot_symb=tNslot_symb;
    Npucch_symb=tNpucch_symb;
    k1=tk1;
    k2=tk2;
    fPUCCH_Adjust=tfPUCCH_Adjust;
  }
  
  double getTPCbasedOnCmdFeild(bool isAbsTPC, int TPCCmdFeild) //select tpc based on command feild
  {
    double TPCInDB;
      if(isAbsTPC)
	TPCInDB = AbsoluteTPC_ValuesForIndexinDB(TPCCmdFeild);
      else
	TPCInDB = AccumulateTPC_ValuesForIndexinDB(TPCCmdFeild);
    
    return(TPCInDB);
  }

};

struct PcReqParaByMeasure_S 
{
  int nRB; // no. of RBs allocated
  double RSP;
  double RSRP;
  double pathlossindB;
  int mu;
  double txPowerMax;
  //   int pcMethod=1;
  double finalTxULPowerPerSubCarrierInWatts, deltaTF_, desiredPowerInWattsForNR, desiredSRSpowerPerRB;
  int Mrb; // BW in terms of RBs w.r.t. 15 KHz SCS
  
  // parameters related to PUCCH power control only
  double nHarqAck; //number of HARQ-ACK information bits that the UE determines
  double Ocsi, Osr, Ocrc; // number of CSI,SR, CRC information bits that the UE determines 
  int formatIndex, UCI;
    
  
  PcReqParaByMeasure_S():nRB(1), mu(1), txPowerMax(23-dB(600)), deltaTF_(0), /*after this, param are for pucch only */nHarqAck(1), Ocsi(1), Osr(1), Ocrc(1), formatIndex(2){}
  // below constructor when almost fixed values(i.e. once constructed then not needed to change frequently) otherwise use below func
//   PcReqParaByMeasure_S(double txMaxPower,int n0RB, double RefSigPow,double RefSigRxPow,int numID):txPower(txMaxPower),nRB(n0RB),RSP(RefSigPow), RSRP(RefSigRxPow), nuID(numID){}  
  void setPCreqMeasParameters(double txMaxPower,int n0RB, double RefSigPow,double RefSigRxPow,int numerologyMu, double deltaTF)
  {
    txPowerMax= txMaxPower;
    mu=numerologyMu;
    nRB= n0RB;
    RSP=RefSigPow;
    RSRP=RefSigRxPow;        
    deltaTF_=deltaTF;
  }  
  
  void setPUCCHparamByUE(double tnHarqAck,double tOcsi, double tOsr, double tOcrc, int tformatIndex, int tUCI)
    {
      nHarqAck=tnHarqAck;
      Ocsi=tOcsi;
      Osr=tOsr;
      Ocrc= tOcrc;
      formatIndex=tformatIndex;
      UCI=tUCI;
    }
};

struct PoAlphaSet_S // contains OL parameters for any beam
{
  double poIndBm;
  double alpha;
  double pcfValueForPowerControl; //needs to set somewhere outside, i.e. it should be initialized outside frame_cnt loop, since first value may be by RAR.. Po_pre+delta_ramping
  
  std::list<PcControlReqInfo_S> pcControlReqInfo; //extra; store these values in every subframe 
  PcControlReqInfo_S powerControlReqInfo;
  PcReqParaByMeasure_S pcReqParaByMeasure;
  
  PoAlphaSet_S():poIndBm(0),alpha(1){}
  PoAlphaSet_S(double p0, double alpha_i):poIndBm(p0),alpha(alpha_i){}
  
//   void setPoAlphaParameters(double targetIOT_Po, double pLCompensationAlpha/*, double subFrameNo, double Kpusch*/)//call outside somewhere (i think b4 doUlpc)to have all values then one will be used as per index in do_ulpowcont, this is to set values
//   {
//     poIndBm = targetIOT_Po;
//     alpha = pLCompensationAlpha;
//   }  
  
  void setPCcontrolInfo(PcControlReqInfo_S pcControl)
  {
    pcControlReqInfo.push_back(pcControl);
  }
  
  bool applyPCcontrol(double time1)
  {
    for(auto it=pcControlReqInfo.begin(); it!=pcControlReqInfo.end(); ++it)
    {
      
      if(it->time!=-1) // check for alocation, if allocated then
      {
	if(it->time <= time1)
	{
	  // 	   cout<<"entering time "<<it->time<<" time1 "<<time1<<endl;
	  powerControlReqInfo=*it;
	  uplinkPowerControl(time1);
	  pcControlReqInfo.erase(it--);
	}
      }    
      else // no allocation case,  i.e. time=-1 (no time mentioned for execution) for this case
      {
	powerControlReqInfo=*it;
	pcfValueForPowerControl=setTPCbasedvalue(); // accumulating given TPC but no allocation i.e. no PC to do but got TPC in any SF
      }
      
    }
    return true;
  }
  void uplinkPowerControl(double time1);
  double setTPCbasedvalue(); //extra
  double calculateDesiredPowerInWatts(int BWinRB, double PL, double MCSPowAdjust, double TPCcommad);
  
};

// double PoAlphaSet_S::setTPCbasedvalue();
double calculateDesiredPowerInWattsInd(double poIndBm,double alpha, int BWinRB, double PL, double MCSPowAdjust, double TPCcommad);
// double PoAlphaSet_S::calculateDesiredPowerInWatts(int BWinRB, double PL, double MCSPowAdjust, double TPCcommad);
// void PoAlphaSet_S::uplinkPowerControl(/*PoAlphaSet_S &PoAlphaSetObject, PcReqParaByMeasure_S &pcReqParaByMeasure,*/ double time1);

void setvalues(double p0,double PLCompensation, double pcfValueForPowerControl,vector<PoAlphaSet_S> &tpoAlphaSet, int factor_cnt); //outside somewhere to have all values then one will be used as per index in do_ulpowcont, this is to set values

double getInitialPreviousTPC(double deltaP_rampup, double deltaMsg2); // for initial RACH

double getDeltaTF(SchedulerToTransmitterInfo_S &schedulerToTransmitterInfo);
double getDeltaTFforPUCCH(PcControlReqInfo_S PCcontrolParamForPUCCH, PcReqParaByMeasure_S PUCCHParamByUE, SchedulerToTransmitterInfo_S schedulerToTransmitterInfo);

#endif