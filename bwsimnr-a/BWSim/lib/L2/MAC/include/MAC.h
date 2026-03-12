#ifndef _MAC_H
#define _MAC_H

#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <ctime>
// #include <boost/circular_buffer.hpp>
#include "scheduler.h"
#include "HARQManager.h"

//#include <Common/include/common.h>


#include "Application/cFTP.h"
#include "Application/cVoip.h"
#include "Application/cVideo.h"
#include "Application/cXR.h"
// #include "../../SchedulerCode/include/scheduler.h"
// #include "../../SchedulerCode_new/include/scheduler.h"


#define FTP_PDB 300 //in ms -- 5QI = 6
#define VOIP_PDB 100 //in ms -- 5QI = 20
#define VIDEO_PDB 100 //in ms -- 5QI = 67
#define XR_PDB 10 //in ms -- 5QI = 67

#define NUM_REMAINING_TIME_CONSIDERED 100
#define MAX_UE_PER_CELL 500
#define FRACTION_DELAY_N3_IF 0.1
// #define WINDOW_SIZE 300
#define XR_MEAN_PKT_SIZE 62500 //bytes
using namespace std;






struct sDRBpacket
{
    int packetCount = 0;
    long int initialPktSize = 0;
    long int totalSize = 0; //in bytes
    int arrivalInstant = 0; //instant
    //int perPktSize = 0; //depends on the traffic
    int admissibleDelay = 0; //in ms -- depends on the QoS
    //int txInstant = -1; // should be positive only in TXD queues
    vector<cPacket> vPkts;
};
struct sUeDelayMapArray
{
    int numBytes_RemTime[NUM_REMAINING_TIME_CONSIDERED+1] = {0};
};


//class cUEcontext : public scheduler
class cUEcontext
{
    public:
    //DRB for each user
    eTrafficType myTrafficType;
    cFTP* myFtpObj;
    cVoip* myVoipObj;
    cVideo* myVideoObj;
    // cXR* myXrObj;
    double myMcs;

    int bytesWaiting_currentInstant = 0;
    vector<sDRBpacket> DRB_FTP;
    vector<sDRBpacket> DRB_VOIP;
    vector<sDRBpacket> DRB_VIDEO;
    vector<sDRBpacket> DRB_XR;

    // int bytesSoFar = 0; //temporary: for test
    vector<int> latencyOfEachPkt;
    // vector<int> runtimelatencyOfEachPkt;

    // long int numBytesLost = 0;
    // long int numBytesArrived = 0;
    // int TotalNumPkts = 0;
    // double fReturnRemainingTimeForOldestPkt(int currSlot);
    // int numTimeoutOverSimulationPeriod = 0;//keep incrementing it
    // double fReturnAvgLatencyInAllPkts();
    // double fReturnRuntimeAvgLatencyInAllPkts();
    // int fReturnNumOfTimeoutPkts(int CurrSlot);
    long int fReturnTotalBytesWaiting();
    // sUeDelayMapArray fReturnDelayMap(int currInstant);
    void fClearAllEmptyPkts();
    int mUEid;
    cUEcontext(int UEid,int numSlots);
    //vector<boost::circular_buffer<double>> windowedLoss;//loss in the last 100 slots
    //vector<boost::circular_buffer<double>> windowedArrival;//arrivals in the last 100 slots
    // boost::circular_buffer<double> windowedLossCount;
    // boost::circular_buffer<double> windowedTxnCount;
    // int fReturnTotalWindowedTimeouts();
    // int fReturnTotalWindowedTxnCount();
    // double avgLossArrivalRatio;  
    //to generate traffic packets every 5000 slots
    void fregenerateFTP(int startSF, int endSF);
    void fregenerateVideo(int startSF, int endSF);
    void fregenerateVoip(int startSF, int endSF);
    void fregenerateXR(int startSF, int endSF);

};
class cMAC
{
public:
    // cUEcontext *pUE[MAX_UE_PER_CELL]; // Max assumed UEs per Cell
    int myGnbId = 0;
    int numUEattached = 0;
    int myTotalNumRBG = 0; // to be calculated by freturnNumRBGforGivenLoad
    double mcsRate[16] = {0, 0.152, 0.234, 0.376, 0.6, 0.876, 1.174, 1.476, 1.912, 2.404, 2.73, 3.318, 3.9, 4.518, 5.112, 5.52};
    ivec attachedUEids;
    Scheduler_C myScheduler;
    Array<cUEcontext*> uePerGnb;
    Array<HARQManager*> harqManager;
    void fInsertCurrentSlotPackets(int slotNum);
    ivec fBufferSizesPerAttachedUE();
    vec myUEmcs;
    cMAC(int gNBIndx, int numSlots, ivec attachedUEs,eTrafficType trafficType)
    {
        myGnbId = gNBIndx;
        attachedUEids = attachedUEs;
        myScheduler = Scheduler_C(0.0005,attachedUEs);
        numUEattached = attachedUEs.length();
        
        uePerGnb.set_length(numUEattached);
        harqManager.set_length(numUEattached);
       
        for(int ue_cnt=0; ue_cnt < attachedUEs.length(); ue_cnt++)
        {
            cout<<"Creating UE for : "<<attachedUEs(ue_cnt)<<endl;
            cUEcontext *ue = new cUEcontext(attachedUEs(ue_cnt),numSlots);
            ue->myTrafficType = trafficType;
            uePerGnb(ue_cnt) = ue;
        
            HARQManager *harq = new HARQManager();
            harq->initializeHarqManager("IR",8,1,4);
            harqManager(ue_cnt) = harq;
        }
        
    }
    cMAC()
    {
        //
    }
};

eTrafficType freturnTrafficType(string trafficTypeCommon);
Array<HarqInfo> collectHARQInfo(Array<HARQManager*> harqManagerPerUE, int slotIndx, ivec codeWordsRequired);


#endif
