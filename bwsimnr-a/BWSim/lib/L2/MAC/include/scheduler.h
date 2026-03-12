
#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "../../../Frozen/simSupport/include/simSupport.h"
#include "../../../L1/SCH/include/schTx.h"
#include "../../../L1/CSIRS/include/csirsRx.h"
#include "powerControl.h"



struct schInfo_S
{
    ivec selectedUEs;
    vec rate;
    vec powerOffsetInDB;

    schInfo_S()
    {
        selectedUEs.set_length(0);
        rate.set_length(0);
        powerOffsetInDB.set_length(0);
    }
    schInfo_S(int nRBGs)
    {
        selectedUEs=zeros_i(nRBGs)-1;
        rate=zeros(nRBGs);
        powerOffsetInDB=zeros(nRBGs);
            }
    void add(int rbgIndx , int ue , double r, double powOffsetInDB=0)
    {
        selectedUEs(rbgIndx)=ue;
        ivec scheduledRBGs = find(selectedUEs,ue,true);
        setInVec(rate,scheduledRBGs,r);
        setInVec(powerOffsetInDB,scheduledRBGs,powOffsetInDB);
    }
    int getScheduledRBGs(int ue)
    {
      return(count(selectedUEs,ue));  
    }
    
};


class Scheduler_C
{
    public:    
    ivec associatedUEs;
    int nRBGs;
    vec R;
    double invTau;
    int roundRobinIndex;
    
    //UL Power control
    bool isDownlink;
    int rbgSize;
    PowerControl_S powerControl;
    
    Array<CSI_Feedback_Info_S> latestCSIFeedbackInfoPerAssociatedUE;
    
    Scheduler_C();
    Scheduler_C(double invTau,ivec associatedUES);
    void setParams(bool isDL, int rbgsize){isDownlink=isDL;rbgSize=rbgsize;}
    schInfo_S schedule(int nRBGs, bool isPF, ivec contendingUEs, Array<vec> ratePerRBGPerUE, Array<HarqInfo> harqInfoPerAssociatedUE, ivec bufferSizes="", vec srsTxSCPowerInWatts="");
    schInfo_S schedule(int nRBGs , bool isPF , ivec contendingUEs, vec ratePerUE, Array<HarqInfo> harqInfoPerAssociatedUE, ivec bufferSizes="", vec srsTxSCPowerInWatts="");
    void updateRForAll(ivec contendingUEs, int winnerUE, float r);
    
    private:
    void performRateOffsetForUplink(ivec contendingUEs, vec &r, schInfo_S schInfo, vec srsTxSCPowerInWatts);
    void performRateOffsetForOLLA(ivec contendingUEs, vec &r, vec powerOffsetInDB);
    void updateContention(schInfo_S schInfo, ivec &contendingUEs,ivec &contendingUEIndices,ivec &bufferSizes);
    
        
};


Array<Array<SCHGrant_S>> generateGrantInfo(int nRBs, int rbgSize, int nSymbols, Array<schInfo_S> &schInfo, ivec &uesForCSI, Array<CSI_Feedback_Info_S> &csiFeedbackInfoPerUE, Array<ivec> &ueIDsForHARQ, Array<Array<HarqInfo>> &harqInfoPerAssociatedUE);


#endif

