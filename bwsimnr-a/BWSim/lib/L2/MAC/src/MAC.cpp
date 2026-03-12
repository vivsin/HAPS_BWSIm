#include "../include/MAC.h"

using namespace std;

eTrafficType freturnTrafficType(string trafficTypeCommon)
{
    eTrafficType trafficType;
    if(trafficTypeCommon == "TRAFFIC_TYPE_VOIP")
    {
        trafficType = TRAFFIC_TYPE_VOIP;
    }
    else if(trafficTypeCommon==  "TRAFFIC_TYPE_CBR")
    {
        trafficType = TRAFFIC_TYPE_CBR;
    }
    else if(trafficTypeCommon ==  "TRAFFIC_TYPE_XR")
    {
        trafficType = TRAFFIC_TYPE_XR;
    }
    else if(trafficTypeCommon ==  "TRAFFIC_TYPE_VIDEO")
    {
        trafficType = TRAFFIC_TYPE_VIDEO;
    }
    else if(trafficTypeCommon == "TRAFFIC_TYPE_HTTP")
    {
        trafficType = TRAFFIC_TYPE_HTTP;
    }
    else if(trafficTypeCommon ==  "TRAFFIC_TYPE_FTP")
    {
        trafficType = TRAFFIC_TYPE_FTP;
    }
    else if(trafficTypeCommon == "TRAFFIC_TYPE_INFINITE_BUFFER")
    {
        trafficType = TRAFFIC_TYPE_INFINITE_BUFFER;
    }
    else
    {
        cout<<"Bad input \"mTrafficType\" "<<endl;
        trafficType = TRAFFIC_TYPE_UNINIT;
    }
    return trafficType;
}

Array<HarqInfo> collectHARQInfo(Array<HARQManager*> harqManagerPerUE, int slotIndx, ivec codeWordsRequired)
{
    Array<HarqInfo> harqInfo(harqManagerPerUE.length());
    for(int ue_cnt=0;ue_cnt<harqManagerPerUE.length();ue_cnt++)
    {
        harqInfo(ue_cnt) = harqManagerPerUE(ue_cnt)->getCurrentHarqInfo(slotIndx,codeWordsRequired(ue_cnt));
//         cout<<"isNewTransmission : "<<harqInfo(ue_cnt).isNewTransmission<<endl;
    }
    return harqInfo;
}


cUEcontext::cUEcontext(int UEid,int numSlots)
{
    mUEid = UEid;
    int arrTime = -1;
    // cout<<"1"<<endl; 
    // cFTP* ftpPtr = new cFTP(TRAFFIC_TYPE_FTP,"../sls/apps/QAS/myL2Support_QAS.txt",TOTAL_ITERS,arrTime);
    cFTP* ftpPtr = new cFTP(TRAFFIC_TYPE_FTP,"../sls/configFiles/NU1/myL2Support.txt",numSlots,arrTime);
    // cout<<"2"<<endl;  
    myFtpObj = ftpPtr;


    int tSrc = 2;
    int tDest = 3;
    int tAppId = 0;
    // double mSimulationTime = simDuration;
    double frameDuration = 1;
    // cVoip* voipPtr = new cVoip(TRAFFIC_TYPE_VOIP, tSrc, tDest, tAppId, mSimulationTime , frameDuration);
    cVoip* voipPtr = new cVoip(TRAFFIC_TYPE_VOIP, tSrc, tDest, tAppId, numSlots , frameDuration);
    myVoipObj = voipPtr;


    
    // cVideo* videoPtr = new cVideo(TRAFFIC_TYPE_VIDEO,"../sls/apps/QAS/myL2Support_QAS.txt", TOTAL_ITERS);
    cVideo* videoPtr = new cVideo(TRAFFIC_TYPE_VIDEO,"../sls/configFiles/NU1/myL2Support.txt", numSlots);
    myVideoObj = videoPtr;


    // cXR* xrPtr = new cXR(TRAFFIC_TYPE_XR,"../sls/configFiles/NU1/myL2Support.txt",numSlots);
    // cout<<xrPtr->mDataToTransmit<<endl;
    // myXrObj = xrPtr;
    
    printf("voip data to tx = %ld \n",myVoipObj->mDataToTransmit);
    printf("ftp data to tx = %ld \n",myFtpObj->mDataToTransmit);
    printf("video data to tx = %ld \n",myVideoObj->mDataToTransmit);
    // windowedLossCount.set_capacity(WINDOW_SIZE);
    // windowedTxnCount.set_capacity(WINDOW_SIZE);

    
}
void cUEcontext::fregenerateFTP(int startSF, int sfCnt)
{
    myFtpObj->fregen(sfCnt,startSF);
}
void cUEcontext::fregenerateVideo(int startSF, int sfCnt)
{
    myVideoObj->fregen(startSF,sfCnt);
}
void cUEcontext::fregenerateVoip(int startSF, int sfCnt)
{
    myVoipObj->genVoipTraffic(startSF,startSF+sfCnt);
}
// void cUEcontext::fregenerateXR(int startSF, int sfCnt)
// {
//     myXrObj->genXRTraffic(startSF,startSF+sfCnt);
// }


// int cUEcontext::fReturnNumOfTimeoutPkts(int currSlot)
// {
//     int numTimeoutPkts = 0;
//     // cout<<"DRB_FTP size = "<<DRB_FTP.size()<<endl;
//     /********************checking FTP queue************************/
//     for(int PktSet=0; PktSet< DRB_FTP.size(); PktSet++)
//     {
//         sDRBpacket OldestPktSet = DRB_FTP.front();
//         //for(int pktCnt = 0; pktCnt < OldestPktSet.packetCount; pktCnt++)
//         //{
//             int numSlotsSpent = currSlot - OldestPktSet.arrivalInstant;
//             int currDelay = SLOT_IN_MS * numSlotsSpent; //delay so far (in ms)

//             // printf("FTP packet: curr delay: %d ----- admissibleDelay %d \n",currDelay,OldestPktSet.admissibleDelay);
//             if(currDelay > OldestPktSet.admissibleDelay)
//             {
//                 //printf("FTP PACKET TIMED OUT \n");
//                 numTimeoutPkts++;
//                 runtimelatencyOfEachPkt.push_back(currDelay);
                
//                 if(currSlot > SETTLING_TIME_SLOTS)
//                 {
//                     numBytesLost = numBytesLost + OldestPktSet.initialPktSize; //if a packet is lost, the entire packet size is considered to be lost
//                     // cout<< "UE " <<mUEid<< " -- numbytes lost = " << numBytesLost << endl;
//                     latencyOfEachPkt.push_back(currDelay);
//                     // if(mUEid == 8)
//                     // {
//                     //     cout<<"ueID: "<<mUEid<<" -- Arrival time: "<<DRB_FTP[pktCnt].arrivalInstant<<" -- Serving Time: "<<currSlot<<" -- Delay incurred : "<<(currSlot - DRB_FTP[pktCnt].arrivalInstant)<<" -- Packet Timed out = 1 ----- CurrentBufferSize = "<<DRB_FTP.size()<< " ---- currAvgDelay in the queue is now "<< fReturnAvgLatencyInAllPkts()<<endl;
//                     // }
//                 }
//                 DRB_FTP.erase(DRB_FTP.begin());
//             }
//         //}
//         if(numTimeoutPkts == 0)
//         {
//             // cout<<"FTP Zero timeouts"<<endl;
//             break;// because the oldestpkt itself has not timed out. Need not check the rest.
//         }
//     }

//     /********************checking VOIP queue************************/
//     // cout<<"DRB_VOIP size = "<<DRB_VOIP.size()<<endl;

//     for(int PktSet=0; PktSet< DRB_VOIP.size(); PktSet++)
//     {
//         sDRBpacket OldestPktSet = DRB_VOIP.front();
//         //for(int pktCnt = 0; pktCnt < OldestPktSet.packetCount; pktCnt++)
//         //{
//             int numSlotsSpent = currSlot - OldestPktSet.arrivalInstant;
//             int currDelay = SLOT_IN_MS * numSlotsSpent; //delay so far (in ms)
//             // printf("VOIP packet: curr delay: %d ----- admissibleDelay %d \n",currDelay,OldestPktSet.admissibleDelay);
//             if(currDelay > OldestPktSet.admissibleDelay)
//             {
//                 //printf("VOIP PACKET TIMED OUT \n");
//                 numTimeoutPkts++;
//                 runtimelatencyOfEachPkt.push_back(currDelay);
//                 if(currSlot > SETTLING_TIME_SLOTS)
//                 {
//                     numBytesLost = numBytesLost + OldestPktSet.initialPktSize; //if a packet is lost, the entire packet size is considered to be lost
//                     // cout<< "UE " <<mUEid<< " -- numbytes lost = " << numBytesLost << endl;
//                     latencyOfEachPkt.push_back(currDelay);
//                     // if(mUEid == 8)
//                     // {
//                     //     cout<<"ueID: "<<mUEid<<" -- Arrival time: "<<DRB_VOIP[pktCnt].arrivalInstant<<" -- Serving Time: "<<currSlot<<" -- Delay incurred : "<<(currSlot - DRB_VOIP[pktCnt].arrivalInstant)<<" -- Packet Timed out = 1 ----- CurrentBufferSize = "<<DRB_VOIP.size()<< " ---- currAvgDelay in the queue is now "<< fReturnAvgLatencyInAllPkts()<<endl;
//                     // }
//                 }
//                 DRB_VOIP.erase(DRB_VOIP.begin());
//             }
//         //}
//         if(numTimeoutPkts == 0)
//         {
//             // cout<<"VOIP Zero timeouts"<<endl;
//             break;// because the oldestpkt itself has not timed out. Need not check the rest.
//         }
//     }


//     /********************checking Video queue************************/
//     // cout<<"DRB_VIDEO size = "<<DRB_VIDEO.size()<<endl;

//     for(int PktSet=0; PktSet< DRB_VIDEO.size(); PktSet++)
//     {
//         sDRBpacket OldestPktSet = DRB_VIDEO.front();
//         int numSlotsSpent = currSlot - OldestPktSet.arrivalInstant;
//         int currDelay = SLOT_IN_MS * numSlotsSpent; //delay so far (in ms)
//         // printf("DRB pkt: %d in queue | UE %d | curr delay: %d ----- admissibleDelay %d \n",PktSet,mUEid,currDelay,OldestPktSet.admissibleDelay);

//         //for(int pktCnt = 0; pktCnt < OldestPktSet.packetCount; pktCnt++)
//         //{
            
//             if(currDelay > OldestPktSet.admissibleDelay)
//             {
//                 //printf("VIDEO PACKET TIMED OUT \n");
//                 numTimeoutPkts++;
//                 runtimelatencyOfEachPkt.push_back(currDelay);
//                 if(currSlot > SETTLING_TIME_SLOTS)
//                 {
//                     numBytesLost = numBytesLost + OldestPktSet.initialPktSize; //if a packet is lost, the entire packet size is considered to be lost
//                     latencyOfEachPkt.push_back(currDelay);
//                     //cout<< "UE " <<mUEid<< " -- numbytes lost = " << numBytesLost << endl;
//                     // if(mUEid == 8)
//                     // {
//                     //     cout<<"ueID: "<<mUEid<<" -- Arrival time: "<<DRB_VIDEO[pktCnt].arrivalInstant<<" -- Serving Time: "<<currSlot<<" -- Delay incurred : "<<(currSlot - DRB_VIDEO[pktCnt].arrivalInstant)<<" --admissible delay = "<<OldestPktSet.admissibleDelay<<" -- Packet Timed out = 1 ----- CurrentBufferSize = "<<DRB_VIDEO.size()<< " ---- currAvgDelay in the queue is now "<< fReturnAvgLatencyInAllPkts()<<endl;
//                     // }
//                 }
//                 DRB_VIDEO.erase(DRB_VIDEO.begin());
//             }
//         //}
//         if(numTimeoutPkts == 0)
//         {
//             // cout<<"Video Zero timeouts"<<endl;
//             break;// because the oldestpkt itself has not timed out. Need not check the rest.
//         }
//         else
//         {
//             //printf("VIDEO PACKET TIMED OUT -- UE-ID [%d] \n",mUEid);
//         }
//     }



//     for(int PktSet=0; PktSet< DRB_XR.size(); PktSet++)
//     {
//         sDRBpacket OldestPktSet = DRB_XR.front();
//         int numSlotsSpent = currSlot - OldestPktSet.arrivalInstant;
//         int currDelay = SLOT_IN_MS * numSlotsSpent; //delay so far (in ms)
//         // printf("DRB pkt: %d in queue | UE %d | curr delay: %d ----- admissibleDelay %d \n",PktSet,mUEid,currDelay,OldestPktSet.admissibleDelay);

//         //for(int pktCnt = 0; pktCnt < OldestPktSet.packetCount; pktCnt++)
//         //{
            
//             if(currDelay > OldestPktSet.admissibleDelay)
//             {
//                 //printf("VIDEO PACKET TIMED OUT \n");
//                 numTimeoutPkts++;
//                 runtimelatencyOfEachPkt.push_back(currDelay);
//                 if(currSlot > SETTLING_TIME_SLOTS)
//                 {
//                     numBytesLost = numBytesLost + OldestPktSet.initialPktSize; //if a packet is lost, the entire packet size is considered to be lost
//                     latencyOfEachPkt.push_back(currDelay);
//                     //cout<< "UE " <<mUEid<< " -- numbytes lost = " << numBytesLost << endl;
//                     // if(mUEid == 8)
//                     // {
//                     //     cout<<"ueID: "<<mUEid<<" -- Arrival time: "<<DRB_XR[pktCnt].arrivalInstant<<" -- Serving Time: "<<currSlot<<" -- Delay incurred : "<<(currSlot - DRB_XR[pktCnt].arrivalInstant)<<" --admissible delay = "<<OldestPktSet.admissibleDelay<<" -- Packet Timed out = 1 ----- CurrentBufferSize = "<<DRB_XR.size()<< " ---- currAvgDelay in the queue is now "<< fReturnAvgLatencyInAllPkts()<<endl;
//                     // }
//                 }
//                 DRB_XR.erase(DRB_XR.begin());
//             }
//         //}
//         if(numTimeoutPkts == 0)
//         {
//             // cout<<"Video Zero timeouts"<<endl;
//             break;// because the oldestpkt itself has not timed out. Need not check the rest.
//         }
//         else
//         {
//             //printf("VIDEO PACKET TIMED OUT -- UE-ID [%d] \n",mUEid);
//         }
//     }
//     if(currSlot > SETTLING_TIME_SLOTS)
//     {
//         numTimeoutOverSimulationPeriod = numTimeoutOverSimulationPeriod + numTimeoutPkts;
//     }
//     windowedLossCount.push_back(numTimeoutPkts);
//     // cout<<"ue "<<mUEid<<" -- curdrSlot: "<<currSlot<<" -- loss: "<<numTimeoutPkts<<endl;
//     return numTimeoutPkts;
// }

// int cUEcontext::fReturnTotalWindowedTimeouts()
// {
//     int sumTimeouts = 0;
//     for(int lossIndx=0; lossIndx<windowedLossCount.size(); lossIndx++)
//     {
//         sumTimeouts += windowedLossCount[lossIndx];
//     }
//     return sumTimeouts;
// }


// int cUEcontext::fReturnTotalWindowedTxnCount()
// {
//     int sumTxns = 0;
//     for(int txnIndx=0; txnIndx<windowedTxnCount.size(); txnIndx++)
//     {
//         sumTxns += windowedTxnCount[txnIndx];
//     }
//     return sumTxns;
// }

// sUeDelayMapArray cUEcontext::fReturnDelayMap(int currInstant)
// {
//     sUeDelayMapArray mapArray;
//     int lastRemainingTimeConsidered = 1;
//     int numBytesInTheCurrArrivalInstant = 0;
//     switch(myTrafficType)
//     {
//         case TRAFFIC_TYPE_FTP:
//         {
//             lastRemainingTimeConsidered = FTP_PDB - (currInstant - DRB_FTP.front().arrivalInstant);//oldestpacket will have smallest rt
//             //cout<<"DRB_FTP.size() = "<<DRB_FTP.size()<< " lastRemainingTimeConsidered = "<<lastRemainingTimeConsidered<<endl;
//             for(int i=0; i<DRB_FTP.size() && lastRemainingTimeConsidered <= NUM_REMAINING_TIME_CONSIDERED ;i++)
//             {
//                 auto itr = DRB_FTP.begin() + i;
//                 int remainingTime = FTP_PDB - (currInstant - itr->arrivalInstant);
//                 //cout<<"Remaining time for drbpkt "<<i<< " is --- " << remainingTime<<endl;
//                 //cout<<"last remaining time considered = "<<lastRemainingTimeConsidered<<endl;
//                 if(remainingTime == lastRemainingTimeConsidered)
//                 {
//                     numBytesInTheCurrArrivalInstant = numBytesInTheCurrArrivalInstant + itr->totalSize;
//                     mapArray.numBytes_RemTime[lastRemainingTimeConsidered] = numBytesInTheCurrArrivalInstant; //store the previous remainingtime-bytes 
//                     //cout<<"FTP: For UE-ID "<<mUEid<<": setting - numBytes_RemTime["<<lastRemainingTimeConsidered <<"] = "<<numBytesInTheCurrArrivalInstant<<endl;
//                 }
//                 else
//                 {
//                     //cout<<"FTP: last rem time = "<<lastRemainingTimeConsidered << " ... numBytes = "<<numBytesInTheCurrArrivalInstant << endl;
//                     mapArray.numBytes_RemTime[lastRemainingTimeConsidered] = numBytesInTheCurrArrivalInstant; //store the previous remainingtime-bytes 
//                     lastRemainingTimeConsidered = FTP_PDB - (currInstant - itr->arrivalInstant);
//                     numBytesInTheCurrArrivalInstant = itr->totalSize;
//                 }
//             }
//             lastRemainingTimeConsidered++;
//             break;
//         }
//         case TRAFFIC_TYPE_VOIP:
//         {
//             lastRemainingTimeConsidered = VOIP_PDB - (currInstant - DRB_VOIP.front().arrivalInstant);//oldestpacket will have smallest rt
//             //cout<<"DRB_VOIP.size() = "<<DRB_FTP.size()<< " lastRemainingTimeConsidered = "<<lastRemainingTimeConsidered<<endl;
//             for(int i=0; i<DRB_VOIP.size() && lastRemainingTimeConsidered <= NUM_REMAINING_TIME_CONSIDERED ;i++)
//             {
//                 auto itr = DRB_VOIP.begin() + i;
//                 int remainingTime = VOIP_PDB - (currInstant - itr->arrivalInstant);
//                 //cout<<"Remaining time for drbpkt "<<i<< " is --- " << remainingTime<<endl;
//                 //cout<<"last remaining time considered = "<<lastRemainingTimeConsidered<<endl;
//                 if(remainingTime == lastRemainingTimeConsidered)
//                 {
//                     numBytesInTheCurrArrivalInstant = numBytesInTheCurrArrivalInstant + itr->totalSize;
//                     mapArray.numBytes_RemTime[lastRemainingTimeConsidered] = numBytesInTheCurrArrivalInstant; //store the previous remainingtime-bytes 
//                     //cout<<"VoIP: For UE-ID "<<mUEid<<": setting - numBytes_RemTime["<<lastRemainingTimeConsidered <<"] = "<<numBytesInTheCurrArrivalInstant<<endl;

//                 }
//                 else
//                 {
//                     //cout<<"VoIP: last rem time = "<<lastRemainingTimeConsidered << " ... numBytes = "<<numBytesInTheCurrArrivalInstant << endl;
//                     mapArray.numBytes_RemTime[lastRemainingTimeConsidered] = numBytesInTheCurrArrivalInstant; //store the previous remainingtime-bytes 
//                     lastRemainingTimeConsidered = VOIP_PDB - (currInstant - itr->arrivalInstant);
//                     numBytesInTheCurrArrivalInstant = itr->totalSize;
//                 }
//             }
//             lastRemainingTimeConsidered++;
//             break;
//         }
//         case TRAFFIC_TYPE_VIDEO:
//         {
//             lastRemainingTimeConsidered = VIDEO_PDB - (currInstant - DRB_VIDEO.front().arrivalInstant);//oldestpacket will have smallest rt
//             for(int i=0; i<DRB_VIDEO.size() && lastRemainingTimeConsidered <= NUM_REMAINING_TIME_CONSIDERED ;i++)
//             {
//                 auto itr = DRB_VIDEO.begin() + i;
//                 int remainingTime = VIDEO_PDB - (currInstant - itr->arrivalInstant);
//                 //cout<<"Remaining time for drbpkt "<<i<< " is --- " << remainingTime<<endl;
//                 //cout<<"last remaining time considered = "<<lastRemainingTimeConsidered<<endl;
//                 if(remainingTime == lastRemainingTimeConsidered)
//                 {
//                     numBytesInTheCurrArrivalInstant = numBytesInTheCurrArrivalInstant + itr->totalSize;
//                     mapArray.numBytes_RemTime[lastRemainingTimeConsidered] = numBytesInTheCurrArrivalInstant; //store the previous remainingtime-bytes 
//                     //cout<<"Video: For UE-ID "<<mUEid<<": setting - numBytes_RemTime["<<lastRemainingTimeConsidered <<"] = "<<numBytesInTheCurrArrivalInstant<<endl;

//                 }
//                 else
//                 {
//                     //cout<<"Video: last rem time = "<<lastRemainingTimeConsidered << " ... numBytes = "<<numBytesInTheCurrArrivalInstant << endl;
//                     mapArray.numBytes_RemTime[lastRemainingTimeConsidered] = numBytesInTheCurrArrivalInstant; //store the previous remainingtime-bytes 
//                     lastRemainingTimeConsidered = VIDEO_PDB - (currInstant - itr->arrivalInstant);
//                     numBytesInTheCurrArrivalInstant = itr->totalSize;
//                 }
//             }
//             lastRemainingTimeConsidered++;
//             break;
//         }
//         case TRAFFIC_TYPE_XR:
//         {
//             lastRemainingTimeConsidered = XR_PDB - (currInstant - DRB_XR.front().arrivalInstant);//oldestpacket will have smallest rt
//             // cout<<"DRB_XR.front().arrivalInstant = "<<DRB_XR.front().arrivalInstant<<"| currInstant = "<<currInstant<<endl;
//             // cout<<"DRB_XR size = "<<DRB_XR.size()<<" --- lastRemainingTimeConsidered = "<<lastRemainingTimeConsidered<<endl;
//             for(int i=0; i<DRB_XR.size() && lastRemainingTimeConsidered <= NUM_REMAINING_TIME_CONSIDERED ;i++)
//             {
//                 auto itr = DRB_XR.begin() + i;
//                 int remainingTime = XR_PDB - (currInstant - itr->arrivalInstant);
//                 //cout<<"Remaining time for drbpkt "<<i<< " is --- " << remainingTime<<endl;
//                 //cout<<"last remaining time considered = "<<lastRemainingTimeConsidered<<endl;
//                 if(remainingTime == lastRemainingTimeConsidered)
//                 {
//                     numBytesInTheCurrArrivalInstant = numBytesInTheCurrArrivalInstant + itr->totalSize;
//                     mapArray.numBytes_RemTime[lastRemainingTimeConsidered] = numBytesInTheCurrArrivalInstant; //store the previous remainingtime-bytes 
//                     //cout<<"Video: For UE-ID "<<mUEid<<": setting - numBytes_RemTime["<<lastRemainingTimeConsidered <<"] = "<<numBytesInTheCurrArrivalInstant<<endl;

//                 }
//                 else
//                 {
//                     //cout<<"Video: last rem time = "<<lastRemainingTimeConsidered << " ... numBytes = "<<numBytesInTheCurrArrivalInstant << endl;
//                     mapArray.numBytes_RemTime[lastRemainingTimeConsidered] = numBytesInTheCurrArrivalInstant; //store the previous remainingtime-bytes 
//                     lastRemainingTimeConsidered = XR_PDB - (currInstant - itr->arrivalInstant);
//                     numBytesInTheCurrArrivalInstant = itr->totalSize;
//                 }
//             }
//             lastRemainingTimeConsidered++;
//             break;        
//         }
//         default:
//         {
//             printf("***************ERROR: Invalid traffic type ************* \n");
//             break;
//         }
//     };
    

//     //for(int j=NUM_REMAINING_TIME_CONSIDERED; j>=NUM_REMAINING_TIME_CONSIDERED - 10; j--)
//     //{
//         //cout<<"ue "<<mUEid<<" --  ("<<j<<","<<mapArray.numBytes_RemTime[j]<<") ******* ";
//     //}
//     //cout<<endl;
//     return mapArray;    
// }




// double cUEcontext::fReturnRemainingTimeForOldestPkt(int currSlot)
// {
//     int remainingTime = 0;
//     int oldestPktDelay = 0;
//     double delay_PDB_ratio = 0;


//     switch(myTrafficType)
//     {
//         case TRAFFIC_TYPE_FTP:
//         {
//             oldestPktDelay = currSlot - (DRB_FTP.front().arrivalInstant);
//             remainingTime = FTP_PDB - oldestPktDelay;
//             delay_PDB_ratio = (double)oldestPktDelay/FTP_PDB;
//             break;
//         }
//         case TRAFFIC_TYPE_VOIP:
//         {
//             oldestPktDelay = currSlot - (DRB_VOIP.front().arrivalInstant);
//             remainingTime = VOIP_PDB - oldestPktDelay;
//             delay_PDB_ratio = (double)oldestPktDelay/VOIP_PDB;
//             break;
//         }
//         case TRAFFIC_TYPE_VIDEO:
//         {
//             oldestPktDelay = currSlot - (DRB_VIDEO.front().arrivalInstant);
//             remainingTime = VIDEO_PDB - oldestPktDelay;
//             delay_PDB_ratio = (double)oldestPktDelay/VIDEO_PDB;
//             break;
//         }
//         case TRAFFIC_TYPE_XR:
//         {
//             oldestPktDelay = currSlot - (DRB_XR.front().arrivalInstant);
//             remainingTime = XR_PDB - oldestPktDelay;
//             delay_PDB_ratio = (double)oldestPktDelay/XR_PDB;
//             break;
//         }
//         default:
//         {
//             printf("***************ERROR: Invalid traffic type ************* \n");
//             break;
//         }
//     }
//     if(remainingTime == 0)
//     {
//         remainingTime = 1;
//     }
//     //printf("UEID %d -- remaining time %d \n",mUEid,(SLOT_IN_MS * remainingTime));
//     //printf("UEID %d -- currentDelay (n slots) = %d \n",mUEid,oldestPktDelay);
//     //cout<< "the ratio (weight) = " << delay_PDB_ratio<<endl; 
//     //return  (SLOT_IN_MS * remainingTime);
//     return  delay_PDB_ratio;
// }


// double cUEcontext::fReturnRuntimeAvgLatencyInAllPkts()
// {
//     double avgLatency = 0;
//     int totalLatencyAcrossAllPkts = 0;
//     // if(mUEid == 8)
//     // {
//     //     cout<<"Latency vec: "<<endl;
//     // }
//     for(int pktLatencyIndx = 0; pktLatencyIndx < runtimelatencyOfEachPkt.size(); pktLatencyIndx++)
//     {
//         // if(mUEid == 8)
//         // {
//         //     cout<<latencyOfEachPkt[pktLatencyIndx]<<" \t";
//         // }
//         totalLatencyAcrossAllPkts = totalLatencyAcrossAllPkts + runtimelatencyOfEachPkt[pktLatencyIndx];   
//     }
//     //cout<<endl;
//     if(runtimelatencyOfEachPkt.size() > 0)
//     {

//         avgLatency = (double) totalLatencyAcrossAllPkts / runtimelatencyOfEachPkt.size();
//         // if(mUEid == 8)
//         // {
//         //     cout<<"totalLatencyAcrossAllPkts = "<<totalLatencyAcrossAllPkts<<" --- avg latency = "<<avgLatency<<endl;
//         // }
//     }
//     // else
//     // {
//     //     cout<<"Avg Latency is 0"<< endl;
//     // }
//     //cout<<"For ueID "<<mUEid<<" -- current avgLatency = "<<avgLatency<<endl;
//     return avgLatency;
// }


// double cUEcontext::fReturnAvgLatencyInAllPkts()
// {
//     double avgLatency = 0;
//     int totalLatencyAcrossAllPkts = 0;
//     // if(mUEid == 8)
//     // {
//     //     cout<<"Latency vec: "<<endl;
//     // }
//     for(int pktLatencyIndx = 0; pktLatencyIndx < latencyOfEachPkt.size(); pktLatencyIndx++)
//     {
//         // if(mUEid == 8)
//         // {
//         //     cout<<latencyOfEachPkt[pktLatencyIndx]<<" \t";
//         // }
//         totalLatencyAcrossAllPkts = totalLatencyAcrossAllPkts + latencyOfEachPkt[pktLatencyIndx];   
//     }
//     //cout<<endl;
//     if(latencyOfEachPkt.size() > 0)
//     {
        
//         avgLatency = (double) totalLatencyAcrossAllPkts / latencyOfEachPkt.size();
//         // if(mUEid == 8)
//         // {
//         //     cout<<"totalLatencyAcrossAllPkts = "<<totalLatencyAcrossAllPkts<<" --- avg latency = "<<avgLatency<<endl;
//         // }
//     }
//     // else
//     // {
//     //     cout<<"Avg Latency is 0"<< endl;
//     // }
//     //cout<<"For ueID "<<mUEid<<" -- current avgLatency = "<<avgLatency<<endl;
//     return avgLatency;
// }

void cUEcontext::fClearAllEmptyPkts()
{
    int numDrbErased = 0;
    for(int PktSet=0; PktSet < DRB_FTP.size(); PktSet++)
    {
        int numErased = 0;
        int initPktsVecSize = DRB_FTP[PktSet-numDrbErased].vPkts.size();
        for(int pktCnt = 0; pktCnt < initPktsVecSize; pktCnt++)
        {
            if(DRB_FTP[PktSet-numDrbErased].vPkts[pktCnt - numErased].fGetPacketSizeInBytes() <= 0)
            {
                //printf("deleting FTP packets for UE ID %d \n",mUEid);
                DRB_FTP[PktSet-numDrbErased].vPkts.erase(DRB_FTP[PktSet-numDrbErased].vPkts.begin() + pktCnt - numErased);
            }
        }
        if(DRB_FTP[PktSet-numDrbErased].vPkts.size() == 0)
        {
            DRB_FTP.erase(DRB_FTP.begin()+PktSet-numDrbErased);
            numDrbErased++;
            //printf("drb erased for ue id %d \n",mUEid);
        }
    }

    numDrbErased = 0;
    for(int PktSet=0; PktSet < DRB_VOIP.size(); PktSet++)
    {
        int numErased = 0;
        int initPktsVecSize = DRB_VOIP[PktSet-numDrbErased].vPkts.size();
        for(int pktCnt = 0; pktCnt < initPktsVecSize; pktCnt++)
        {
            if(DRB_VOIP[PktSet-numDrbErased].vPkts[pktCnt - numErased].fGetPacketSizeInBytes() <= 0)
            {
                //printf("deleting VOIP");
                DRB_VOIP[PktSet-numDrbErased].vPkts.erase(DRB_VOIP[PktSet-numDrbErased].vPkts.begin() + pktCnt - numErased);
            }
        }
        if(DRB_VOIP[PktSet-numDrbErased].vPkts.size() == 0)
        {
            DRB_VOIP.erase(DRB_VOIP.begin()+PktSet-numDrbErased);
            numDrbErased++;
        }
    }

    numDrbErased = 0;
    for(int PktSet=0; PktSet < DRB_VIDEO.size(); PktSet++)
    {
        int numErased = 0;
        int initPktsVecSize = DRB_VIDEO[PktSet-numDrbErased].vPkts.size();
        for(int pktCnt = 0; pktCnt < initPktsVecSize; pktCnt++)
        {
            if(DRB_VIDEO[PktSet-numDrbErased].vPkts[pktCnt - numErased].fGetPacketSizeInBytes() <= 0)
            {
                //printf("deleting VIDEO empty packet");
                DRB_VIDEO[PktSet-numDrbErased].vPkts.erase(DRB_VIDEO[PktSet-numDrbErased].vPkts.begin() + pktCnt - numErased);
            }
        }
        if(DRB_VIDEO[PktSet-numDrbErased].vPkts.size() == 0)
        {
            DRB_VIDEO.erase(DRB_VIDEO.begin()+PktSet-numDrbErased);
            numDrbErased++;
        }
    }

    numDrbErased = 0;
    for(int PktSet=0; PktSet < DRB_XR.size(); PktSet++)
    {
        int numErased = 0;
        int initPktsVecSize = DRB_XR[PktSet-numDrbErased].vPkts.size();
        for(int pktCnt = 0; pktCnt < initPktsVecSize; pktCnt++)
        {
            if(DRB_XR[PktSet-numDrbErased].vPkts[pktCnt - numErased].fGetPacketSizeInBytes() <= 0)
            {
                //printf("deleting XR empty packet");
                DRB_XR[PktSet-numDrbErased].vPkts.erase(DRB_XR[PktSet-numDrbErased].vPkts.begin() + pktCnt - numErased);
            }
        }
        if(DRB_XR[PktSet-numDrbErased].vPkts.size() == 0)
        {
            DRB_XR.erase(DRB_XR.begin()+PktSet-numDrbErased);
            numDrbErased++;
        }
    }

    
    //for(int PktSet=0; PktSet< DRB_VOIP.size(); PktSet++)
    //for(int PktSet=0; PktSet< DRB_VIDEO.size(); PktSet++)
    //for(int PktSet=0; PktSet< DRB_FTP.size(); PktSet++)
}


long int cUEcontext::fReturnTotalBytesWaiting()
{
    long int totalBytes=0;

    switch(myTrafficType)
    {
        case TRAFFIC_TYPE_FTP:
        {
            for(int drbPktSet=0; drbPktSet< DRB_FTP.size(); drbPktSet++)
            {
                for(int pktCnt=0; pktCnt < DRB_FTP[drbPktSet].vPkts.size(); pktCnt++)
                {
                    totalBytes = totalBytes + DRB_FTP[drbPktSet].vPkts[pktCnt].fGetPacketSizeInBytes();
                    // totalBytes = totalBytes + DRB_FTP[drbPktSet].totalSize;
                }
            }
            //printf("FTP queue size for UE ID %d = %ld \n",mUEid,totalBytes);
            break;
        }
        case TRAFFIC_TYPE_VOIP:
        {
            for(int drbPktSet=0; drbPktSet< DRB_VOIP.size(); drbPktSet++)
            {
                for(int pktCnt=0; pktCnt < DRB_VOIP[drbPktSet].vPkts.size(); pktCnt++)
                {
                    totalBytes = totalBytes + DRB_VOIP[drbPktSet].vPkts[pktCnt].fGetPacketSizeInBytes();
                }
            }
            //printf("VOIP queue size for UE ID %d = %ld \n",mUEid,totalBytes);
            break;
        }
        case TRAFFIC_TYPE_VIDEO:
        {
            for(int drbPktSet=0; drbPktSet< DRB_VIDEO.size(); drbPktSet++)
            {
                for(int pktCnt=0; pktCnt < DRB_VIDEO[drbPktSet].vPkts.size(); pktCnt++)
                {
                    totalBytes = totalBytes + DRB_VIDEO[drbPktSet].vPkts[pktCnt].fGetPacketSizeInBytes();
                }
            }
            //printf("VIDEO queue size for UE ID %d = %ld \n",mUEid,totalBytes);
            break;
        }
        case TRAFFIC_TYPE_XR:
        {
            for(int drbPktSet=0; drbPktSet< DRB_XR.size(); drbPktSet++)
            {
                for(int pktCnt=0; pktCnt < DRB_XR[drbPktSet].vPkts.size(); pktCnt++)
                {
                    totalBytes = totalBytes + DRB_XR[drbPktSet].vPkts[pktCnt].fGetPacketSizeInBytes();
                }
            }
            //printf("VIDEO queue size for UE ID %d = %ld \n",mUEid,totalBytes);
            break;
        }
    }
    
    return totalBytes;
}


void cMAC::fInsertCurrentSlotPackets(int currInstant)
{
    for (int ueIndx = 0; ueIndx < numUEattached; ueIndx++)
        {
            int ueID = attachedUEids(ueIndx);
            if (uePerGnb(ueIndx)->myTrafficType == TRAFFIC_TYPE_FTP)
            {
                cPacketQueue pktQueue = uePerGnb(ueIndx)->myFtpObj->fCallTraffic(currInstant);
                if (pktQueue.fGetPackets().size() > 0)
                {
                    sDRBpacket mydrbPkt;
                    mydrbPkt.totalSize = pktQueue.fGetSize();
                    mydrbPkt.initialPktSize = mydrbPkt.totalSize;
                    if (mydrbPkt.totalSize > 0)
                    {
                        mydrbPkt.packetCount = pktQueue.fGetPackets().size();
                        mydrbPkt.arrivalInstant = currInstant;
                        list<cPacket> pktQ = pktQueue.fGetPackets();
                        auto itr = pktQ.begin();
                        for (int pktCnt = 0; pktCnt < pktQ.size(); pktCnt++)
                        {
                            mydrbPkt.vPkts.push_back(*itr);
                            itr++;
                            break; // assuming the packet size is same across all the packets
                        }
                        mydrbPkt.admissibleDelay = (1 - FRACTION_DELAY_N3_IF) * FTP_PDB; // 0.9*300
                        uePerGnb(ueIndx)->DRB_FTP.push_back(mydrbPkt);
                        // cout<<"Received one FTP packet of size "<<mydrbPkt.totalSize << " in instant "<<currInstant<<endl;
                    }
                }
            }
            else if (uePerGnb(ueIndx)->myTrafficType == TRAFFIC_TYPE_VOIP)
            {
                cPacketQueue pktQueue = uePerGnb(ueIndx)->myVoipObj->fCallTraffic(currInstant);
                if (pktQueue.fGetPackets().size() > 0)
                {
                    sDRBpacket mydrbPkt;
                    mydrbPkt.totalSize = pktQueue.fGetSize();
                    mydrbPkt.initialPktSize = mydrbPkt.totalSize;
                    uePerGnb(ueIndx)->DRB_VOIP.clear();
                    if (mydrbPkt.totalSize > 0)
                    {
                        mydrbPkt.packetCount = pktQueue.fGetPackets().size();
                        mydrbPkt.arrivalInstant = currInstant;
                        list<cPacket> pktQ = pktQueue.fGetPackets();
                        auto itr = pktQ.begin();
                        for (int pktCnt = 0; pktCnt < pktQ.size(); pktCnt++)
                        {
                            mydrbPkt.vPkts.push_back(*itr);
                            itr++;
                            break; // assuming the packet size is same across all the packets
                        }
                        mydrbPkt.admissibleDelay = (1 - FRACTION_DELAY_N3_IF) * VOIP_PDB; // 0.9*100
                        uePerGnb(ueIndx)->DRB_VOIP.push_back(mydrbPkt);
                    }
                }
            }
            else if (uePerGnb(ueIndx)->myTrafficType == TRAFFIC_TYPE_VIDEO)
            {
                // generate Video
                cPacketQueue pktQueue = uePerGnb(ueIndx)->myVideoObj->fCallTraffic(currInstant);
                if (pktQueue.fGetPackets().size() > 0)
                {
                    sDRBpacket mydrbPkt;
                    mydrbPkt.totalSize = pktQueue.fGetSize();
                    mydrbPkt.initialPktSize = mydrbPkt.totalSize;
                    
                    if (mydrbPkt.totalSize > 0)
                    {
                        mydrbPkt.packetCount = pktQueue.fGetPackets().size();
                        mydrbPkt.arrivalInstant = currInstant;
                        list<cPacket> pktQ = pktQueue.fGetPackets();

                        auto itr = pktQ.begin();
                        for (int pktCnt = 0; pktCnt < pktQ.size(); pktCnt++)
                        {
                            mydrbPkt.vPkts.push_back(*itr);
                            itr++;
                            break; // assuming the packet size is same across all the packets
                        }
                        mydrbPkt.admissibleDelay = (1 - FRACTION_DELAY_N3_IF) * VIDEO_PDB; // 0.9*100
                        uePerGnb(ueIndx)->DRB_VIDEO.push_back(mydrbPkt);
                    }
                }
            }
            // else if (uePerGnb(ueIndx)->myTrafficType == TRAFFIC_TYPE_XR)
            // {
            //     // generate Video
            //     cPacketQueue pktQueue = uePerGnb(ueIndx)->myXrObj->fCallTraffic(currInstant);
            //     if (pktQueue.fGetPackets().size() > 0)
            //     {
            //         sDRBpacket mydrbPkt;
            //         mydrbPkt.totalSize = pktQueue.fGetSize();
            //         mydrbPkt.initialPktSize = mydrbPkt.totalSize;
            //         if (mydrbPkt.totalSize > 0)
            //         {
            //             mydrbPkt.packetCount = pktQueue.fGetPackets().size();
            //             mydrbPkt.arrivalInstant = currInstant;
            //             list<cPacket> pktQ = pktQueue.fGetPackets();

            //             auto itr = pktQ.begin();
            //             for (int pktCnt = 0; pktCnt < pktQ.size(); pktCnt++)
            //             {
            //                 mydrbPkt.vPkts.push_back(*itr);
            //                 itr++;
            //                 break; // assuming the packet size is same across all the packets
            //             }
            //             mydrbPkt.admissibleDelay = (1 - FRACTION_DELAY_N3_IF) * XR_PDB; // 0.9*100
            //             uePerGnb(ueIndx)->DRB_XR.push_back(mydrbPkt);
            //         }
            //     }
            // }
            else
            {
                // generate V2X
            }
        }
}

ivec cMAC::fBufferSizesPerAttachedUE()
{
    ivec fCurrentBufferSizes;
    for (int ueIndx = 0; ueIndx < numUEattached; ueIndx++)
    {
        uePerGnb(ueIndx)->bytesWaiting_currentInstant = 0; //setting
        uePerGnb(ueIndx)->bytesWaiting_currentInstant = uePerGnb(ueIndx)->fReturnTotalBytesWaiting();
        fCurrentBufferSizes.ins(fCurrentBufferSizes.length(),uePerGnb(ueIndx)->bytesWaiting_currentInstant);
        // cout<<"buffer size of ue in indxPosition "<<ueIndx<< " is "<<uePerGnb(ueIndx)->bytesWaiting_currentInstant<<endl;
    }
    // cout<<fCurrentBufferSizes<<endl;
    return fCurrentBufferSizes;
}
