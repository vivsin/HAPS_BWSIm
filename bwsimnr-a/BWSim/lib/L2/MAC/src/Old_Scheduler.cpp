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
//#define PFSCHEDULER_DEBUG
#include "../include/Old_Scheduler.h"
#ifdef ENABLE_COUT
#define MUMIMO_DEBUG
#define SCH_DEBUG
#endif
template<class T>Vec<T> && stdVectorToVec(std::vector<T> a)
{
    Vec<T> toReturn(a.size());
    for(int i=0;i<a.size();++i)
    {
        toReturn[i]=a[i];
        cout<<"a[i]'"<<a[i]<<endl;
    }
    return std::move(toReturn);  

}

void Scheduler::initScheduler(string configFilename, string l2ConfigFilename, int cellid, ivec associatedUEIDs, Array< sBeamPairLink > beamPairLink, ivec antennaCountPerBeam, ivec srvcNodeAntenna, double serverNodeTxSCPowerIndBm, vec serviceNodeTxSCPowerIndBm, cTBTables* tbTables, FrameStructure* frameStructure, FrameReservationInfo_S* frameReservationInfo, PortInfo_S* portsInfo, Array< Codebook* > codebook, sBWPartInfo bwPartInfo, int RBGSize, int prbBundlingSize, bool isDownlink, PowerControlInfo_S powControlInfo){

    #ifdef USING_SECURITY 
    BWSIM_Password_function();
    #endif
    
    mUEIds=associatedUEIDs;
    beamPair=beamPairLink;
    mUEAvgRate=ones(associatedUEIDs.length())*0.0001; // 0.001 is dummy minimum rate
    nScheduledRBsPerUE = zeros_i(mUEIds.length());
    //mUEAvgRate=ones(associatedUEIDs.length())*1e6;

    if(!parse(l2ConfigFilename,"isPFScheduler",mIsPF))
    {
        cout<<"[detl:] Unable to parse isPFScheduler from L2Support file.."<<endl;
        abort();
    }

    if(mIsPF)
    {
        parse(l2ConfigFilename,"gFactor",mPFTc);
        parse(l2ConfigFilename,"updateThroughputEachRBG",updateThroughputEachRBG);
    }

    nSymbolsForMinimumTimeAllocation=14;
    mRoundRobinCounter=0;
    mPFCounter=0;
    pairedUEsInCurrentSubframe.set_length(0);
    mIsDownlink=isDownlink;

    mTBTables=tbTables;
    mFrameStructure=frameStructure;
    mFrameReservationInfo=frameReservationInfo;
    mPortsInfo=portsInfo;
    mRBGSize=RBGSize;
    mRBGCount=ceil_i((double)mFrameStructure->getRuCountPerLevel()/(double)RBGSize);
    mPRBBundlingSize=prbBundlingSize;
    mBWPartInfo=bwPartInfo;
    mCodebookPerBeam=codebook;
    mCodebook=mCodebookPerBeam(0);
    powerControlInfo=powControlInfo;

    mIsEPDCCHEnabled=false;
    isSemiOpenloopScheme=false;
    mIsSubbandCQIEnabled=false;
    mIsMUMIMOEnabled=false;
    mIsModeAdaptationEnabled=false;

    nUserPairing=-1;
    nCRSPorts=-1;
    dlTransmissionMode.set_length(0);
    ulTransmissionMode.set_length(0);

    eNodeBAntennaCountPerBeam=antennaCountPerBeam;
    eNodeBAntennaCount=eNodeBAntennaCountPerBeam(0);
    ueAntennaCount=srvcNodeAntenna;

    eNodeBTxSCPowerIndBm=serverNodeTxSCPowerIndBm;
    UETxSCPowerIndBm=serviceNodeTxSCPowerIndBm;

    cellID=cellid;
    cqiList.set_length(associatedUEIDs.length());
    for(int cnt=0;cnt<cqiList.length();cnt++)
        cqiList(cnt).rnti=associatedUEIDs(cnt);


    //Non-slot scheduling
    int mMaxSlotsPerTTI=frameStructure->getRuLevels();
    symbolsPerTTI.set_length(0);
    parse(l2ConfigFilename,"nSymbolsForMinimumTimeAllocation",nSymbolsForMinimumTimeAllocation);
    if(nSymbolsForMinimumTimeAllocation<(14*mMaxSlotsPerTTI))
    {
        int nTimePartitions=floor_i((14.0*(double)mMaxSlotsPerTTI)/(double)nSymbolsForMinimumTimeAllocation);
        symbolsPerTTI.set_length(nTimePartitions);
        int startSym=0,endSym=0;
        for(int cnt=0;cnt<nTimePartitions;cnt++)
        {
            endSym=(startSym+nSymbolsForMinimumTimeAllocation-1);
            if((cnt+1)==nTimePartitions)
                endSym=(14*mMaxSlotsPerTTI)-1;

            symbolsPerTTI(cnt)=getIntegers(startSym,endSym);
            startSym+=nSymbolsForMinimumTimeAllocation;
        }
    }
    else
    {
        symbolsPerTTI.set_length(1);
        symbolsPerTTI(0)=getIntegers(0,(14*mMaxSlotsPerTTI)-1);
    }
    if(isDownlink)
    {
        parse(l2ConfigFilename,"mIsEPDCCHEnabled",mIsEPDCCHEnabled);
        parse(configFilename, "DL_isSemiOpenloopScheme", isSemiOpenloopScheme);
        // Reciprocity precoder
        if(!parse(configFilename, "useReciproPrecode", useReciproPrecode))
        {
            cout<<"[both:] Variable useReciproPrecode is nor found in myDLTxRxConfig.txt.. Aborting ..."<<endl;
        }

        bool mIsAperiodicCQIEnabled;
        parse(configFilename, "enablePerAperCqiComp", mIsAperiodicCQIEnabled);
        if(mIsAperiodicCQIEnabled)
            mIsSubbandCQIEnabled=true;
        else
        { 
            string pMode;
            parse(configFilename, "PeriodicMode", pMode);
            if(pMode=="_PERIODIC_MODE_2_0_" || pMode=="_PERIODIC_MODE_2_1_")
                mIsSubbandCQIEnabled = true;
        }

        parse(configFilename, "DL_enableMUMIMO", mIsMUMIMOEnabled);
    if(useReciproPrecode && mIsMUMIMOEnabled)
    {
        cout<<"MUMIMO should not be enabled for reciprocity. Aborting ..."<<endl;abort();
    }
        if(mIsMUMIMOEnabled)
        {
            string pairingAlgo;
            parse(configFilename, "DL_forceMUMIMO", mForceMUMIMO);  
            parse(configFilename, "DL_nUserPairing", nUserPairing);
            parse(configFilename, "DL_MUMIMOpairing",pairingAlgo);
            pairingMethod = getPairingMethod(pairingAlgo);

            if(prbBundlingSize==0 || prbBundlingSize>RBGSize)
            {
                cout<<"prbBundlingSize : "<<prbBundlingSize<<endl;
                cout<<"Error. PRB Bundling size can't be larger than RBGSize for MU-MIMO Scheduling. Recommended value : "<<RBGSize<<endl;
                abort();
            }
        }
        
        parse(configFilename, "DL_enableModeAdaptation", mIsModeAdaptationEnabled);
        parse(configFilename, "DL_nCRSPorts", nCRSPorts);
        parse(configFilename, "DL_nLayers", nDlLayers);
        parse(configFilename,"DL_defaultCQI", defaultCQI);
        TransmissionMode_E dlTxMode= parseTransmissionMode(configFilename, isDownlink);
        dlTransmissionMode.set_length(associatedUEIDs.length());
        for(int cnt=0;cnt<associatedUEIDs.length();cnt++)
            dlTransmissionMode(cnt)=dlTxMode;

        string dmrstype;
        parse(configFilename,"DL_DMRSType",dmrstype);
        if(dmrstype=="_DMRSTYPE_1_")
        {
            dmrsType=1;
        }
        else if(dmrstype=="_DMRSTYPE_2_")
        {
            dmrsType=2;
        }
        else
        {
            cout<<"Invalid DL_DMRS Type...Aborting.."<<endl;abort();
        }
        parse(configFilename,"DL_DMRSMaxLength",dmrsLength);
        if(!(dmrsLength==1 || dmrsLength==2))
        {
            cout<<"Invalid DL_DMRS MaxLength...Aborting..."<<endl;abort();
        }
    }
    else
    {
        // What to do for UL
        cout<<"Warning.. Some variables may not be set for UL..."<<endl;
        // Reciprocity precoder
        if(!parse(configFilename, "useReciproPrecode", useReciproPrecode))
        {
            cout<<"[both:] Variable useReciproPrecode is nor found in myDLTxRxConfig.txt.. Aborting ..."<<endl;
        }
        parse(configFilename, "UL_nLayers", nUlLayers);
        parse(configFilename, "UL_enableMUMIMO", mIsMUMIMOEnabled);
      if(useReciproPrecode && mIsMUMIMOEnabled)
    {
        cout<<"MUMIMO should not be enabled for reciprocity. Aborting ..."<<endl;abort();
    }
        if(mIsMUMIMOEnabled)
        {
            parse(configFilename, "UL_forceMUMIMO", mForceMUMIMO);  
            parse(configFilename, "UL_nUserPairing", nUserPairing);
            string pairingAlgo;
            parse(configFilename, "UL_MUMIMOpairing",pairingAlgo);
            pairingMethod = getPairingMethod(pairingAlgo);
        }

        TransmissionMode_E ulTxMode= parseTransmissionMode(configFilename, isDownlink);
        ulTransmissionMode.set_length(associatedUEIDs.length());
        for(int cnt=0;cnt<associatedUEIDs.length();cnt++)
            ulTransmissionMode(cnt)=ulTxMode;

        string dmrstype;
        parse(configFilename,"UL_DMRSType",dmrstype);
        if(dmrstype=="_DMRSTYPE_1_")
        {
            dmrsType=1;
        }
        else if(dmrstype=="_DMRSTYPE_2_")
        {
            dmrsType=2;
        }
        else
        {
            cout<<"Invalid UL_DMRS Type...Aborting.."<<endl;abort();
        }
        parse(configFilename,"UL_DMRSMaxLength",dmrsLength);
        if(!(dmrsLength==1 || dmrsLength==2))
        {
            cout<<"Invalid UL_DMRS MaxLength...Aborting..."<<endl;abort();
        }
    }
    if(nUserPairing > dmrsType+1 && mIsMUMIMOEnabled)
    {
        if(currentTaskid==0 && cellID==0)
            cout<<"nUserPairing cannot be greater than "<<dmrsType+1<<" for dmrsType "<<dmrsType<<endl;
        nUserPairing = dmrsType+1;
    }
}

SchedulingInfo_S Scheduler::schedule(int subframe, int beamID, Array< UEBufferInfo_S >& tUEDataInfo, OuterLoopLinkAdaptation_S& tOuterLoopLinkAdaptation, int eNBID, bool isCQIportAvailable, bool forceContiguousAllocation, ivec TPCcmdField_, TRSConfig_S mTRSConf,string muMIMOInfoFileName,string schInfoFilename)
{
    SchedulingInfo_S schedulingInfo;
    sScheduleReturn schReturn;
  TPCcmdFieldperUE = TPCcmdField_;
    isCQIPortAvailable=isCQIportAvailable;
    mTrsConf=mTRSConf; 
    isTRSPortAvailable=mTrsConf.isToTransmitTRSPorts(subframe); // assuming 1ms subfame -- need to change
    ivec contendingUEs= fGetContendingUEList(tUEDataInfo);
    for(int ue_cnt=0;ue_cnt<tUEDataInfo.length();ue_cnt++)
        if(!tUEDataInfo(ue_cnt).isInfiniteBuffer && !mIsPF)
        {
            cout<<"Error.. RoundRobin can't run for finite buffer simulation..."<<endl;
            abort();
        }

    int nRBs = mFrameStructure->getRuCountPerLevel();

    if(mIsDownlink)
    {
        mCodebook=mCodebookPerBeam(beamID);
        if(mCodebook->getCodeBookConfig().nCSIRSPorts==1 && mIsMUMIMOEnabled)
        {
            if(currentTaskid==0)
                cout<<"MU-MIMO is not possible with one CSIRS port...disabling by default..."<<endl;
            mIsMUMIMOEnabled = false;
        }
            
    }

    eNodeBAntennaCount=eNodeBAntennaCountPerBeam(beamID);

    schReturn.mResourceBlockMapping.resize(contendingUEs.size());
    schReturn.PrecoderPerRB.resize(contendingUEs.size());
    schReturn.scheduledVRBs.resize(contendingUEs.size());
    schReturn.tsympos.set_length(contendingUEs.size());
    Array<ivec> resourceUnits = segmentVecToArrayOfVecs(getIntegers(0,nRBs-1),mRBGCount);
    ivec rbsList=shuffleVec(getIntegers(0,mRBGCount-1));
    resourceUnits=getFromArray(resourceUnits,rbsList);

    bool mIsSubbandCQIEnabled_Org=mIsSubbandCQIEnabled;
    if(!forceContiguousAllocation)
    {
        ivec rbsList=shuffleVec(getIntegers(0,mRBGCount-1));
        resourceUnits=getFromArray(resourceUnits,rbsList);
    }
    else
        mIsSubbandCQIEnabled=false; // Disabled to avoid best band scheduling, which gives non contiguous allocation;

    for (int i = 0; i < (int)schReturn.mResourceBlockMapping.size(); i++)
    {
        schReturn.mResourceBlockMapping[i].resize(nRBs);
        schReturn.scheduledVRBs[i].set_length(0);
        schReturn.PrecoderPerRB[i].resize(nRBs);
        for(int j=0; j<nRBs; j++)
            schReturn.PrecoderPerRB[i][j]=zeros_c(1,1);
    }

    if(mIsEPDCCHEnabled)
    {
        ivec rbgReservedForEPDCCH;
        ivec rbsReservedForEPDCCH=mFrameReservationInfo->reservedVRBsForEpdcch;
        cout<<"rbReservedForEPDCCH= "<<rbsReservedForEPDCCH<<endl;
        for(int j=0; j< rbsReservedForEPDCCH.length(); j++)
            for (int i=0; i< resourceUnits.length(); i++)
            {
                if(find(resourceUnits(i),rbsReservedForEPDCCH(j))!=-1)
                    append(rbgReservedForEPDCCH, i);
            }

        rbgReservedForEPDCCH=findUniqueNumbers(rbgReservedForEPDCCH);
        cout<<"rbgReservedForEPDCCH= "<<rbgReservedForEPDCCH<<endl;
        deleteInArray(resourceUnits,rbgReservedForEPDCCH);    
    }


    if(mFrameReservationInfo->reservedVRBs.length())
    {
    
    cout<<"Reserved RBs : "<<mFrameReservationInfo->reservedVRBs<<endl;
    
        ivec rbgsReserved;
        ivec rbsReserved=mFrameReservationInfo->reservedVRBs;
        for(int j=0; j< rbsReserved.length(); j++)
            for (int i=0; i< resourceUnits.length(); i++)
            {
                if(find(resourceUnits(i),rbsReserved(j))!=-1)
                    append(rbgsReserved, i);
            }

        rbgsReserved=findUniqueNumbers(rbgsReserved);
        deleteInArray(resourceUnits,rbgsReserved);    
    }

    //Do the same job for numerologies. 

    Array<ivec > tBWPLoc=mFrameReservationInfo->BSBWPLocations;
    ivec nonreservedRBforNU;
    ivec rbgReservedforOtherNU;
    for(int i =0 ; i <tBWPLoc.length(); i++ )
    {
        ivec rbPos =tBWPLoc(i);
        int diff = rbPos(1)-rbPos(0);
        nonreservedRBforNU=getIntegers(0,diff);
        int app= ceil_i((double)(rbPos(0)));
        nonreservedRBforNU = nonreservedRBforNU+app;
        ivec actualRBs= getIntegers(0,nRBs-1);
        for(int i1 =0 ; i1 < nonreservedRBforNU.length(); i1++)
        {
            int index = find(actualRBs,nonreservedRBforNU(i1));
            if(index != -1)
            {
                actualRBs.del(index);
            }
        }
        for(int k=0; k< actualRBs.length(); k++)
            for (int rg=0; rg< resourceUnits.length(); rg++)
            {
                if(find(resourceUnits(rg),actualRBs(k))!=-1)
                    append(rbgReservedforOtherNU, rg);
            }
        rbgReservedforOtherNU=findUniqueNumbers(rbgReservedforOtherNU);

        //        resourceUnits=segmentVecToArrayOfVecs(getIntegers(0,nonreservedRBforNU),mRBGCount);
        deleteInArray(resourceUnits,rbgReservedforOtherNU);
    }  

    //int RBIndex = 0;

    //   for(int l=0; l < resourceUnits.length() ; l++)
    //   {
    //     cout<<"checking the contending RBS: "<<resourceUnits(l)<<endl;
    //   }

    int rbgIndx = 0;
    for (int i = 0; i < (int)contendingUEs.size(); i++)
    {
        schReturn.UEIndex.push_back(contendingUEs[i]);
        schReturn.isRetx1.push_back(sScheduleReturn::NO_TRANSMISSION);
        schReturn.isRetx2.push_back(sScheduleReturn::NO_TRANSMISSION);
        schReturn.mcs_1.push_back(-1);
        schReturn.mcs_2.push_back(-1);
        schReturn.transmissionMode.push_back(-1);
        schReturn.nLayers.push_back(-1);
        schReturn.pmi.push_back(-1);
        schReturn.isPaired.push_back(false);

        schReturn.tbSize_1.push_back(0);
        schReturn.tbSize_2.push_back(0);
        schReturn.isSemiOpenloopScheme.push_back(isSemiOpenloopScheme);
    }

    int freshAllocRBGStartIndx = rbgIndx;

    ScheduledRBsInfo_S scheduledRBsInfo;

    ivec scheduledRBGIndices(0);
    Array<ivec> availableResourceUnits = resourceUnits;

    Array<ivec> subbands = availableResourceUnits;

#ifdef PFSCHEDULER_DEBUG
    cout<<"nSubbands: "<<subbands.length()<<endl;
    cout<<"subband: "<<subbands<<endl;
#endif

    //Start CoMP Support
    vector <bool> RBGsReserveMap;
    RBGsReserveMap.assign(mRBGCount,false);  
    // Restrict number of UEs
    //     if(contendingUEs.size() > 4)
    //     {
    //       random_shuffle ( contendingUEs.begin(), contendingUEs.end() );
    //       contendingUEs.set_size(4);
    //     }
    mScheduledUEsInCurrentSubframe.set_size(0);
    mMcsRatesPerScheduledUEs.set_size(0);


    int prevUEId=-1;

    // For each RBG, the contending users list should change according to the RBG sizes of particular numerology users.
    // Should get the RBG sizes for all the contending users.
    // subbands and RBGs are same here. Number of subbands = Number of RBGs

    ivec miniSlotIndxPerUE=zeros_i(contendingUEs.size())-1;
    ivec ueIDsForMiniSlot=contendingUEs;

    ivec selectedUEs(0);
    vec instantMcsRateForSelectedUEs(0);
    Array<ivec> pairedUEs(0);
    Array<vec> SINRInDBOfPairedUEs(0);

    //   if(isToRetainMUMIMOPairs) 
    //   {
    //     retainedMUMIMOPairs.set_length(0);
    //     isToUseMUMIMOPairs=false;
    //   }

    //   contendingUEs = fGetContendingUEList(tUEData, contendingUEs);     /// Return UEs with non-zero buffer size

    if (contendingUEs.size() == 0) 
    {
        cout<<"No Contending UEs found in current TTI...Skipping Schedule.."<<endl;  
        return schedulingInfo;
    }
    double avgRankPerSB = 0.0;int nMUMIMO=0,nSUMIMO=0;

    for(int sb_cnt = 0; sb_cnt < subbands.length() && (contendingUEs.size() > 0); sb_cnt++)
    {
        if(RBGsReserveMap[sb_cnt]) 
        {
            // Skip the current RBG loop iteration since this RBG is reserved for CoMP Dynamic Blanking
            continue;
        }

        // Restrict number of UEs
        //     if(mScheduledUEsInCurrentSubframe.size() > 6)
        //     {
        //       contendingUEs = fGetContendingUEList(tUEData, mScheduledUEsInCurrentSubframe); 
        //     }

        ScheduledUEInfo_S schUEInfo;
        //     cout<<"getting the CQI list ::"<<cqiList.length()<<endl;

        int selectedSB = -1;

        if((mIsPF && !mIsSubbandCQIEnabled) || !mIsPF || isCQIAvailable==false)
            selectedSB = sb_cnt;

        ivec symbolsToSchedule=symbolsPerTTI(0);
        symbolsToSchedule=remove(symbolsToSchedule,mFrameReservationInfo->reservedSymbols);
#ifdef SCHEDULER_DEBUG
    cout<<"symbolsToSchedule  1 : "<<symbolsToSchedule<<endl;
    cout<<"mFrameReservationInfo->cchInfo.srsCount : "<<mFrameReservationInfo->cchInfo.srsCount<<endl;
    cout<<"symbolsToSchedule.right(mFrameReservationInfo->cchInfo.srsCount) : "<<symbolsToSchedule.right(mFrameReservationInfo->cchInfo.srsCount)<<endl;
    cout<<"symbolsToSchedule.left(mFrameReservationInfo->cchInfo.srsCount) : "<<symbolsToSchedule.left(mFrameReservationInfo->cchInfo.srsCount)<<endl;
#endif    
    symbolsToSchedule=remove(symbolsToSchedule,symbolsToSchedule.left(mFrameReservationInfo->cchInfo.srsCount));
#ifdef SCHEDULER_DEBUG
    cout<<"symbolsToSchedule  2 : "<<symbolsToSchedule<<endl;
#endif
        if(mIsDownlink)
            if(mFrameReservationInfo->cchInfo.pdcchCount)
                symbolsToSchedule=symbolsToSchedule.right(symbolsToSchedule.length()-mFrameReservationInfo->cchInfo.pdcchCount);

        {
            //Non slot is common for all forms, semi, MIMO, SIMO

            if(symbolsPerTTI.length()>1 && !mIsPF) // non-slot
            {
                cout<<"Non-slot scheduling available only with PF !!.. "<<endl;
                abort();
            }

            if(symbolsPerTTI.length()>1 && mIsMUMIMOEnabled) // non-slot
            {
                cout<<"Non-slot scheduling available only for SU-MIMO !!.. "<<endl;
                abort();
            }


            if(!mIsMUMIMOEnabled)
            {
                ivec dummy(0);

                if(mIsPF)
                {
                    // 	  if(isSemiOpenloopScheme)
                    // 	    schUEInfo = SemiOLSUMIMOScheduler(contendingUEs,symbolsToSchedule,cqiList,scheduledUEsPerRB,cqiForScheduledUEsPerRB,rbStructureForScheduledUEsPerRB,schReturn, eNodeBAntennaCount,subbands,tOuterLoopLinkAdaptation,scheduledRBGIndices,selectedSB,dummy,eNBID);
                    // 	  else
                    {
                        if(symbolsPerTTI.length()>1) // non-slot
                        {
                            ivec indx=find(ueIDsForMiniSlot,contendingUEs);
                            ivec temp=miniSlotIndxPerUE(indx);
                            schUEInfo=fFindBestUEsFornonSlotusingSubbandCSI(subframe, contendingUEs,temp, cqiList, scheduledRBsInfo, schReturn, eNodeBAntennaCount, subbands, tOuterLoopLinkAdaptation, scheduledRBGIndices, selectedSB, dummy, eNBID);///Id of UE being considered for scheduling   
                            setInVec(miniSlotIndxPerUE,indx,temp);
                        }
                        else
                            schUEInfo=fFindBestUEusingSubbandCSI(subframe, contendingUEs,symbolsToSchedule, cqiList, scheduledRBsInfo, schReturn, eNodeBAntennaCount, subbands, tOuterLoopLinkAdaptation, scheduledRBGIndices, selectedSB, dummy, eNBID);///Id of UE being considered for scheduling   
                    }
                }
                else
                    schUEInfo=fFindBestUEusingSubbandCSI(subframe, contendingUEs,symbolsToSchedule, cqiList, scheduledRBsInfo, schReturn, eNodeBAntennaCount, subbands, tOuterLoopLinkAdaptation, scheduledRBGIndices, selectedSB, dummy, eNBID);///Id of UE being considered for scheduling   
            }
            else
            {
                if(mIsPF)
                {
                    // TB size coming out of the schUEInfo for MU-MIMO will be wrong due to improper RB structure. May have to recalculate - Dhiv
                    if(isCQIAvailable==true) // CQI Reported..  - Need to change for the case where periodicity is different for UEs - Dhiv
                        schUEInfo=fFindBestUEsForMUMIMOusingSubbandCSI(subframe,contendingUEs,symbolsToSchedule,cqiList, scheduledRBsInfo,schReturn, eNodeBAntennaCount,subbands,tOuterLoopLinkAdaptation,scheduledRBGIndices,selectedSB,eNBID,pairedUEs,SINRInDBOfPairedUEs,beamID,muMIMOInfoFileName);
                    else
                        schUEInfo=fFindBestUEusingSubbandCSI(subframe, contendingUEs,symbolsToSchedule, cqiList, scheduledRBsInfo, schReturn, eNodeBAntennaCount, subbands, tOuterLoopLinkAdaptation, scheduledRBGIndices, selectedSB, "", eNBID);///Id of UE being considered for scheduling   	  
                }
                else
                {
                    cout<<"Error.. RR scheduler not available for MU-MIMO."<<endl;
                    abort();
                }
            }
        }

        // Skipping the current band
        //I am leaving out an RBs since the PF matrix is 0
        if(schUEInfo.ueid.length() == 0)
            continue;

        //If the contendingUEs become zero.
        //Hoping the length of schUEInfo.ueid is x for mimo case.

        if(0)  
            if(forceContiguousAllocation)
            {
                if(schUEInfo.ueid.length()>1)
                {
                    cout<<"MU-MIMO with contiguous allocation not written yet."<<endl;abort();
                }
                // Assuming non-MUMIMO and accessing ueid(0)
                if(prevUEId == -1)
                    prevUEId = schUEInfo.ueid(0);

                if(prevUEId != schUEInfo.ueid(0)) ///To ensure contiguous allocation
                {
                    contendingUEs = remove(contendingUEs, prevUEId); 
                    prevUEId = schUEInfo.ueid(0);
                    // 	sb_cnt--;
                    // 	continue;
                }
            }

        fUpdateUEBufferInfo(tUEDataInfo, schUEInfo);
        fUpdateSchReturn(schReturn, schUEInfo);
        append(scheduledRBGIndices,selectedSB);
        for(int iUE = 0; iUE < schUEInfo.ueid.length(); iUE++)
        {
            scheduledRBsInfo.setScheduledRBsInfo(schUEInfo.ueid(iUE),schUEInfo.nLayers(iUE),schUEInfo.vrbs,schUEInfo.effSINRs(iUE),schUEInfo.rbStructurePerRB);
            int indx = -1;
            for(int jj = 0; jj < mScheduledUEsInCurrentSubframe.size(); jj++)
            {
                if(mScheduledUEsInCurrentSubframe[jj] == schUEInfo.ueid(iUE))
                    indx = jj;
            }

            if(indx != -1)
            {
                mMcsRatesPerScheduledUEs(indx) = schUEInfo.mcsRate(iUE);
            }
            else
            {
                append(mScheduledUEsInCurrentSubframe,schUEInfo.ueid(iUE));
                append(mMcsRatesPerScheduledUEs,schUEInfo.mcsRate(iUE));
#ifdef PFSCHEDULER_DEBUG
                //cout<<"mScheduledUEsInCurrentSubframe    :"<<mScheduledUEsInCurrentSubframe<<endl;
                cout<<"mMcsRatesPerScheduledUEs          :"<<mMcsRatesPerScheduledUEs<<endl;
#endif
            }
            schReturn.fUpdateMCS(schUEInfo.ueid(iUE),schUEInfo.mcsIndxPerCodeword(iUE));
            schReturn.fUpdateTxMode(schUEInfo.ueid(iUE), (int)schUEInfo.txMode);
            schReturn.fUpdateResourceMapping(schUEInfo.ueid(iUE), schUEInfo.vrbs);
            schReturn.fUpdateScheduledVRBs(schUEInfo.ueid(iUE),schUEInfo.vrbs);
            schReturn.fUpdateSymbolPosition(schUEInfo.ueid(iUE),schUEInfo.symPos(iUE));     

            schReturn.fUpdatePrecoderPerRB(schUEInfo.ueid(iUE), schUEInfo.vrbs,schUEInfo.nLayers(iUE),schUEInfo.precoderPerRB[iUE]);
        }

        if(schUEInfo.MUMIMOPair.pairingInfo.ueIDs.length())
        {
            ivec ueIndices=find(schUEInfo.MUMIMOPair.pairingInfo.ueIDs,scheduledRBsInfo.ueIDs);
            scheduledRBsInfo.pairingPositionPerUE=schUEInfo.MUMIMOPair.pairingInfo.pairingPositionPerUE(ueIndices); //overwriting pairing info
        }

        //       if(isToRetainMUMIMOPairs)
        //         append(retainedMUMIMOPairs,schUEInfo.MUMIMOPair);

        // Updating Average Rate after every RBG allocation
        if(updateThroughputEachRBG)
        {
            if(isCQIAvailable==true)
                fUpdateAvgThroughputV2(contendingUEs, schUEInfo.ueid, schUEInfo.instantMcsRate);
        }
        else
        {
            append(selectedUEs,schUEInfo.ueid);
            append(instantMcsRateForSelectedUEs,schUEInfo.instantMcsRate);
        }

        if(schUEInfo.ueid.length()>1)
            nMUMIMO += schUEInfo.vrbs.length();
        else
            nSUMIMO += schUEInfo.vrbs.length();
        avgRankPerSB +=sum(schUEInfo.nLayers);

    } //End of SB (or RBG) loop

    avgRankPerSB /= (double) subbands.length();

    //   if(isToRetainMUMIMOPairs)
    //   {
    //     isToRetainMUMIMOPairs=false;
    //     isToUseMUMIMOPairs=true;
    //   }

    if(!updateThroughputEachRBG)
        if(isCQIAvailable==true)
            fUpdateAvgThroughputV2(contendingUEs, selectedUEs,instantMcsRateForSelectedUEs);

    scheduledRBsInfo.updateLayersMapInfo(dmrsType,dmrsLength);

    if(forceContiguousAllocation)
        mIsSubbandCQIEnabled=mIsSubbandCQIEnabled_Org;

#ifdef PFSCHEDULER_DEBUG
    cout<<"scheduledRBGIndices :             "<<scheduledRBGIndices<<endl;
#endif    

    int totalPacketSize=0;
    ivec tbSizes(0);
    ivec mcsIndices(0);

    for(int i=0;i<(int)schReturn.UEIndex.size();i++)
    {
        if (schReturn.nLayers[i] != -1)
        {
            append(schedulingInfo.ueIDs,schReturn.UEIndex[i]);
            DataBlkInfo_S myDataBlkInfo;
            myDataBlkInfo.waveform=mWaveform;
            myDataBlkInfo.dataGenInfo.rnti=schReturn.UEIndex[i];
            myDataBlkInfo.resourceBlocks=schReturn.scheduledVRBs[i];
            sort(myDataBlkInfo.resourceBlocks);
            myDataBlkInfo.dataGenInfo.scheduledPRBs=myDataBlkInfo.resourceBlocks.length();
            myDataBlkInfo.dataGenInfo.transmissionScheme=TransmissionScheme_E(schReturn.transmissionMode[i]);
            myDataBlkInfo.dataGenInfo.nLayers=schReturn.nLayers[i];
            myDataBlkInfo.symPos= schReturn.tsympos(i);
            myDataBlkInfo.dataGenInfo.scheduledSymbols=myDataBlkInfo.symPos.length();
            myDataBlkInfo.dataGenInfo.nCodeWords = (myDataBlkInfo.dataGenInfo.nLayers > 4) ? 2 : 1;
            if(schReturn.mcs_1[i]!=-1)
            {
                // myDataBlkInfo.dataGenInfo.nCodeWords++;  
                append(myDataBlkInfo.dataGenInfo.mcsIndices,schReturn.mcs_1[i]);
                append(mcsIndices,schReturn.mcs_1[i]);
                append(tbSizes,schReturn.tbSize_1[i]*8);
            }
            if(schReturn.mcs_2[i]!=-1)
            {
                //myDataBlkInfo.dataGenInfo.nCodeWords++;  
                append(myDataBlkInfo.dataGenInfo.mcsIndices,schReturn.mcs_2[i]);
                append(mcsIndices,schReturn.mcs_2[i]);
                append(tbSizes,schReturn.tbSize_2[i]*8);
            }

            for(int ue_cnt=0;ue_cnt<tUEDataInfo.length();ue_cnt++)
                if(tUEDataInfo(ue_cnt).mUEId == schReturn.UEIndex[i])
                {
                    append(schedulingInfo.rbIDs,tUEDataInfo(ue_cnt).mRBId);
                    break;
                }

            if(!mIsDownlink && mTBTables->transportBlockMCSMapping.ispi2BPSKEnabled)
                if(myDataBlkInfo.dataGenInfo.mcsIndices(0)<2 && myDataBlkInfo.dataGenInfo.nLayers==1) // First two MCS indices correspond to PiBy2BPSK
                    myDataBlkInfo.waveform=_Waveform_SCFDMA_;

            LayerInfo_S layerInfo=scheduledRBsInfo.layersInfoPerUE(find(scheduledRBsInfo.ueIDs,schReturn.UEIndex[i]));
            myDataBlkInfo.dataGenInfo.layersInfo=layerInfo;

            complex< double > additionalScaling=complex< double >(sqrt(1.0/(double)layerInfo.nCDM),0.0);	  
            //       complex< double > additionalScaling=complex< double >(sqrt((double)layerInfo.nLayers/(double)layerInfo.totalLayers),0.0);	  

            //MU-MIMO scaling done here.. The scaling changes every RBG based on the current pairing, to normalize power. - Dhiv
            myDataBlkInfo.precoderPerRB.set_length(myDataBlkInfo.dataGenInfo.scheduledPRBs);
            for(int rb_cnt=0;rb_cnt<myDataBlkInfo.dataGenInfo.scheduledPRBs;rb_cnt++)
                myDataBlkInfo.precoderPerRB(rb_cnt)=additionalScaling*schReturn.PrecoderPerRB[i][myDataBlkInfo.resourceBlocks(rb_cnt)];


            myDataBlkInfo.prbBundlingSize=mPRBBundlingSize;

            //Averaging precoders over PRB bunldling
            if(mPRBBundlingSize!=0)
            {
                Array<ivec> contiguousRBs=splitContiguousNumbers(myDataBlkInfo.resourceBlocks,mPRBBundlingSize);
                for(int cnt1=0,rb_cnt1=0,rb_cnt2=0;cnt1<contiguousRBs.length();cnt1++)
                {
                    cmat temp;
                    for(int cnt2=0;cnt2<contiguousRBs(cnt1).length();cnt2++,rb_cnt1++)
                    {
                        if(cnt2==0)
                            temp=myDataBlkInfo.precoderPerRB(rb_cnt1);
                        else
                            temp+=myDataBlkInfo.precoderPerRB(rb_cnt1);
                    }
                    temp/=(double)contiguousRBs(cnt1).length();
                    for(int cnt2=0;cnt2<contiguousRBs(cnt1).length();cnt2++,rb_cnt2++)
                        myDataBlkInfo.precoderPerRB(rb_cnt2)=temp;
                }
            }

            if(mIsDownlink)
                setRsInfo(myDataBlkInfo.dataGenInfo.rbInfo.rsInfo,*mPortsInfo,*mFrameStructure,cellID,myDataBlkInfo.dataGenInfo.transmissionScheme,layerInfo,eNodeBAntennaCount,0,1,isCQIPortAvailable,myDataBlkInfo.symPos(0),isTRSPortAvailable,mTrsConf);
            else
                setRsInfo(myDataBlkInfo.dataGenInfo.rbInfo.rsInfo,*mPortsInfo,*mFrameStructure,cellID,myDataBlkInfo.dataGenInfo.transmissionScheme,layerInfo,ueAntennaCount(find(mUEIds,schReturn.UEIndex[i])),0,0,isCQIPortAvailable,myDataBlkInfo.symPos(0),false,TRSConfig_S(),myDataBlkInfo.waveform);

      if(mPortsInfo->enablePTRS == true)
      addptrs(myDataBlkInfo.dataGenInfo.rbInfo.rsInfo,*mPortsInfo,*mFrameStructure,cellID,myDataBlkInfo.dataGenInfo.transmissionScheme,myDataBlkInfo.dataGenInfo.rnti,myDataBlkInfo.resourceBlocks,0,1);
#ifdef SCHEDULER_DEBUG
cout<<"RS ports in schedule() : \n"<<myDataBlkInfo.dataGenInfo.rbInfo.rsInfo.rsPorts<<endl;
#endif
            myDataBlkInfo.dataGenInfo.symbolRepetitionFactor=getSymbolRepetitionFactor(myDataBlkInfo.dataGenInfo.transmissionScheme,myDataBlkInfo.dataGenInfo.nLayers);
            myDataBlkInfo.dataGenInfo.mimoRate=(double)myDataBlkInfo.dataGenInfo.nLayers/(double)(myDataBlkInfo.dataGenInfo.symbolRepetitionFactor); //! \bug fix, divided by symbolRepetitionFactor

            myDataBlkInfo.dataGenInfo.cellID=cellID;

      if(!mIsDownlink)
            myDataBlkInfo.TPCcmdFieldIndividualUE= TPCcmdFieldperUE(find(mUEIds,schReturn.UEIndex[i]));
            append(schedulingInfo.dataBlkInfo,myDataBlkInfo); 
        }
    }

#ifdef SCH_DEBUG
    cout<<"MCS Indices: "<<mcsIndices<<endl;
    cout<<"tbSizes : "<<tbSizes<<endl;
    cout<<"totalPacketSize : "<<sum(tbSizes)<<endl;
    cout<<"Expected SE : "<<(double)sum(tbSizes)/(1e-3*1e7)<<endl;
#endif

    if(traceValuePerTraceName["EnableBSlogs"]=="true")
        printSchInfoLogs(schedulingInfo,subframe,eNBID,beamID,nRBs,nMUMIMO,nSUMIMO,avgRankPerSB,schInfoFilename);

#ifdef SCHEDULER_DEBUG
    cout << "schReturn: " << (int)schReturn.UEIndex.size() << endl;

    for (int i = 0; i < (int)schReturn.UEIndex.size(); i++)
    {
        cout <<endl<< "UE Id: " << schReturn.UEIndex[i] << " TM: " << schReturn.transmissionMode[i] << " nLayer: "
            << schReturn.nLayers[i] << " pmi: " << schReturn.pmi[i] << " MCS1: " << schReturn.mcs_1[i] << " MCS2: " << schReturn.mcs_2[i] << endl;

        cout<<"============mResourceBlockMapping================"<<endl;
        for(int j = 0; j < (int)schReturn.mResourceBlockMapping[i].size(); j++)
        {
            cout<<schReturn.mResourceBlockMapping[i][j]<<"\t";
        }
    }
    cout<<endl;
#endif
    updateRBInfo(schedulingInfo);

    return schedulingInfo;
}

void Scheduler::updateRBInfo(SchedulingInfo_S& schInfo)
{
    for(int ue_cnt=0;ue_cnt<schInfo.ueIDs.length();ue_cnt++)
    {
        int ueIndx = find(mUEIds,schInfo.ueIDs(ue_cnt));
        nScheduledRBsPerUE(ueIndx) += schInfo.dataBlkInfo(ue_cnt).dataGenInfo.scheduledPRBs;
    }
}

ScheduledUEInfo_S Scheduler::fFindBestUEusingSubbandCSI(int subframe,ivec &contendingUEs,ivec symbolsToSchedule, Array<sCqiList> &cqiList,ScheduledRBsInfo_S &scheduledRBsInfo,sScheduleReturn &schReturn, int eNBAntennaCount, Array<ivec> vrbNumbers,OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, ivec& scheduledRBGIndices, int& selectedRBG, ivec invalidUEsForSUMIMO, int eNBID)
{ 
    if(symbolsToSchedule.length()==0) symbolsToSchedule=getIntegers(0,13);
    int totalSubbands=vrbNumbers.length();
    if(selectedRBG != -1)
    {
        ivec rbNumbers = vrbNumbers(selectedRBG);
        vrbNumbers.set_length(1);
        vrbNumbers(0) = rbNumbers;
    }

    int nSubbands = vrbNumbers.length();
    int nUEs = (int)contendingUEs.size();

    //! Collecting CQI Indices of the contending UEs
    Array< Array< UEInfo_S > > tUEInfoPerSubbandsPerUEs(nSubbands);

    for(int iSB = 0; iSB < nSubbands; iSB++)
    {
        // Skipping already scheduled RBG
        if(nSubbands > 1 && find(scheduledRBGIndices,iSB) !=-1)
        {
            continue;
        }
        tUEInfoPerSubbandsPerUEs(iSB).set_length(nUEs);
        for(int iUE=0;iUE<(int)cqiList.size();iUE++)
        {
            // Skipping UE which is already scheduled in MUMIMO mode
            if(find(invalidUEsForSUMIMO,cqiList(iUE).rnti)!=-1)
            {
                continue;
            }
            int ueIndx=find(contendingUEs, cqiList(iUE).rnti);
            if(ueIndx!=-1)
            {
                if(mIsDownlink)
                    tUEInfoPerSubbandsPerUEs(iSB)(ueIndx) = fCollectUEInfo(cqiList(iUE),dlTransmissionMode(iUE),vrbNumbers(iSB));
                else
                    tUEInfoPerSubbandsPerUEs(iSB)(ueIndx) = fCollectUEInfo(cqiList(iUE),ulTransmissionMode(iUE),vrbNumbers(iSB));

            }
        }

    }

    //! Collecting average Rate so far of the contending UEs
    vec avgRatePerUE = fCollectAvgRate(contendingUEs);

#ifdef PFSCHEDULER_DEBUG
    cout<<endl<<"avgRatePerUE "<<avgRatePerUE<<endl;
#endif

    Array<Array<ivec> > possbileMCSPerCodeword(nSubbands);
    Array<Array<ivec> > tbSizePerCodewordInBytes(nSubbands);
    Array<vec> finalMCSRatePerUE(nSubbands);
    Array<vec> instantaneousMCSRatePerUE(nSubbands);
    Array<Array< Array<imat> > > rbStructurePerRB(nSubbands);
    Array<vec> pfMetric(nSubbands);

    double maxPFMetric = -1.0;
    int selectedUEIndx=-1;

    //We can write a subloop for the split of non slot.
    //Get the contending users slot structure. Depends on that decide what to do. 
    //If the contending users have mix of non slot based users of different length. 
    //Collect those seperately for all the numerologies and form a list of contending users of length numerology
    //If scheduler decide the number of symbols to go for a user.
    //So before getting to this loop , scheduler has to decide. how many symbols.
    //Why for a normal buffer, a user go for the non slot based. 
    //So depends on the traffic, non slot based is decided. So scheduler should know that. 
    //slot based and non slot based cant co-exist.


    for(int iSB = 0; iSB < nSubbands; iSB++)
    {
        possbileMCSPerCodeword(iSB).set_length(contendingUEs.size());
        tbSizePerCodewordInBytes(iSB).set_length(contendingUEs.size());
        finalMCSRatePerUE(iSB)=zeros(contendingUEs.size());
        instantaneousMCSRatePerUE(iSB)=zeros(contendingUEs.size());
        pfMetric(iSB)=zeros(contendingUEs.size())-1;
        rbStructurePerRB(iSB).set_length(contendingUEs.size());

        // Skipping already scheduled RBG
        if(nSubbands > 1 && find(scheduledRBGIndices,iSB) !=-1)
        {
            continue;
        }


        for(int iUE=0;iUE<(int)contendingUEs.size();iUE++)
        {
            // Skipping UE which is already scheduled in MUMIMO mode 
            if(find(invalidUEsForSUMIMO,contendingUEs[iUE])!=-1)
            {
                continue;
            }

            int txAntennaCount=eNBAntennaCount;

            if(!mIsDownlink)
                txAntennaCount=fGetUEAntennaCount(contendingUEs[iUE]);

            LinkAdaptation_S tLinkAdaptation;
            ivec tbSizePerCodewordOldInBytes(0);
            Array<vec> possibleEffSINRs;
            Array<imat> possibleRBStructures;

            scheduledRBsInfo.getScheduledRBsInfo(contendingUEs[iUE],possibleEffSINRs,possibleRBStructures);

            if(possibleEffSINRs.length()>0)
            {
                tLinkAdaptation = fDoLinkAdaptation(contendingUEs[iUE],tUEInfoPerSubbandsPerUEs(iSB)(iUE).nLayers,tUEInfoPerSubbandsPerUEs(iSB)(iUE).txMode,scheduledRBsInfo.getScheduledRBs(contendingUEs[iUE]),symbolsToSchedule,possibleRBStructures,possibleEffSINRs,tOuterLoopLinkAdaptation,tUEInfoPerSubbandsPerUEs(iSB)(iUE).isCQIReported);
                tbSizePerCodewordOldInBytes = tLinkAdaptation.tbSizePerCodewordInBytes;
            }

            LayerInfo_S layerInfo(tUEInfoPerSubbandsPerUEs(iSB)(iUE).nLayers,dmrsType,dmrsLength);
            computeCDMsizeAndcount(to_ivec(tUEInfoPerSubbandsPerUEs(iSB)(iUE).nLayers),layerInfo.CDM,layerInfo.nCDM,dmrsType,dmrsLength);
            layerInfo.CDMMap = zeros_b(1);
            layerInfo.CDMMap(0)=1;

            RSInfo_S rsInfo;
            if(mIsDownlink)
                setRsInfo(rsInfo,*mPortsInfo,*mFrameStructure,cellID,TransmissionScheme_E((int)tUEInfoPerSubbandsPerUEs(iSB)(iUE).txMode),layerInfo,txAntennaCount,0,mIsDownlink,isCQIPortAvailable,symbolsToSchedule(0),isTRSPortAvailable,mTrsConf);
            else
                setRsInfo(rsInfo,*mPortsInfo,*mFrameStructure,cellID,TransmissionScheme_E((int)tUEInfoPerSubbandsPerUEs(iSB)(iUE).txMode),layerInfo,txAntennaCount,0,mIsDownlink,isCQIPortAvailable,symbolsToSchedule(0));
      
      if(mPortsInfo->enablePTRS == true)
	addptrs(rsInfo,*mPortsInfo,*mFrameStructure,cellID,TransmissionScheme_E((int)tUEInfoPerSubbandsPerUEs(iSB)(iUE).txMode),cqiList(iUE).rnti,scheduledRBsInfo.getScheduledRBs(contendingUEs[iUE]),0,1);
      
            rbStructurePerRB(iSB)(iUE)=getMergedRBStructures(mFrameStructure,rsInfo,vrbNumbers(iSB),mIsDownlink,true,mFrameReservationInfo);
            //It gives the possible RE for the schedule. It will run for all the contending users. 
            //Before giving it to it change the location of RS info.

            for(int k =0; k < rbStructurePerRB(iSB)(iUE).length(); k++)
                rbStructurePerRB(iSB)(iUE)(k)=rbStructurePerRB(iSB)(iUE)(k).get_cols(symbolsToSchedule);

            //Write the condition.
            //Why there are two link adaptaions.
            //Half the TB size = vrbNumbers(iSB).length().
            tLinkAdaptation = fDoLinkAdaptation(contendingUEs[iUE],tUEInfoPerSubbandsPerUEs(iSB)(iUE).nLayers,tUEInfoPerSubbandsPerUEs(iSB)(iUE).txMode,vrbNumbers(iSB),symbolsToSchedule,rbStructurePerRB(iSB)(iUE),tUEInfoPerSubbandsPerUEs(iSB)(iUE).sinrInDB,tOuterLoopLinkAdaptation,tUEInfoPerSubbandsPerUEs(iSB)(iUE).isCQIReported);

            instantaneousMCSRatePerUE(iSB)(iUE) = tLinkAdaptation.mcsRate;

            if(tLinkAdaptation.UEid == -1)
                continue;   // Skipping UE which has CQI 0;

            append(possibleEffSINRs,tUEInfoPerSubbandsPerUEs(iSB)(iUE).sinrInDB);
            append(possibleRBStructures,rbStructurePerRB(iSB)(iUE));

            int nNumOfRbs = vrbNumbers(iSB).length() + fFindNumOfRBsScheduled(schReturn.mResourceBlockMapping,schReturn.UEIndex,contendingUEs[iUE]);
            ivec rbsToConsider=concat(vrbNumbers(iSB),fgetRBsScheduled(schReturn.mResourceBlockMapping,schReturn.UEIndex,contendingUEs[iUE]));
            tLinkAdaptation = fDoLinkAdaptation(contendingUEs[iUE],tUEInfoPerSubbandsPerUEs(iSB)(iUE).nLayers,tUEInfoPerSubbandsPerUEs(iSB)(iUE).txMode,rbsToConsider,symbolsToSchedule,possibleRBStructures,possibleEffSINRs,tOuterLoopLinkAdaptation,tUEInfoPerSubbandsPerUEs(iSB)(iUE).isCQIReported);

            if(tbSizePerCodewordOldInBytes.length() > 0 && sum(tbSizePerCodewordOldInBytes) >= sum(tLinkAdaptation.tbSizePerCodewordInBytes))
            {
                instantaneousMCSRatePerUE(iSB)(iUE) = 0.001;
            }

#ifdef PFSCHEDULER_DEBUG
            tLinkAdaptation.printLinkAdaptation();
#endif

            possbileMCSPerCodeword(iSB)(iUE) = tLinkAdaptation.mcsIndxPerCodeword;
            tbSizePerCodewordInBytes(iSB)(iUE) = tLinkAdaptation.tbSizePerCodewordInBytes;
            finalMCSRatePerUE(iSB)(iUE) = tLinkAdaptation.mcsRate;

        }
        pfMetric(iSB)=elem_div(instantaneousMCSRatePerUE(iSB),avgRatePerUE);

#ifdef PFSCHEDULER_DEBUG
        cout<<endl<<"=======Printing pfMetric============"<<endl;
        cout<<"instantaneousMCSRatePerUE : "<<instantaneousMCSRatePerUE<<endl;
        cout<<"pfMetric :	"<<pfMetric<<endl<<endl;
#endif	
    }

    //! Selecting UE based on PF Metric
    int selectedRBGIndx=0;

    if(mIsPF) 
    { 
        vec maxPFMetricOverSBs = zeros(pfMetric.length());
        ivec maxIndexOverSBs = zeros_i(pfMetric.length()) - 1;
        //get the small r
        for(int sb_cnt = 0; sb_cnt < pfMetric.length(); sb_cnt++)
        {
            if(pfMetric(sb_cnt).length()>0)
            {

                maxPFMetricOverSBs(sb_cnt) = max(pfMetric(sb_cnt));
                ivec possibleselectedUEIndices = find(pfMetric(sb_cnt),max(pfMetric(sb_cnt)),true);
                if(possibleselectedUEIndices.length()==1)
                    maxIndexOverSBs(sb_cnt)=possibleselectedUEIndices(0);
                else
                    maxIndexOverSBs(sb_cnt)=randPick(possibleselectedUEIndices,1)(0);

#ifdef PFSCHEDULER_DEBUG
                cout<<"SB : "<<sb_cnt<<" r : "<<instantaneousMCSRatePerUE(sb_cnt)(maxIndexOverSBs(sb_cnt))<<" R : "<<avgRatePerUE(maxIndexOverSBs(sb_cnt))<<" PF : "<<max(pfMetric(sb_cnt))<<" UE : "<<contendingUEs[maxIndexOverSBs(sb_cnt)]<<endl;
#endif
            }
        }
        maxPFMetric = max(maxPFMetricOverSBs);
        ivec possibleselectedSBs = find(maxPFMetricOverSBs,max(maxPFMetricOverSBs),true);
        if(possibleselectedSBs.length()==1)
            selectedRBGIndx=possibleselectedSBs(0);
        else
            selectedRBGIndx=randPick(possibleselectedSBs,1)(0);

        selectedUEIndx = maxIndexOverSBs(selectedRBGIndx);

        if(selectedRBG==-1)
            selectedRBG=selectedRBGIndx;

        // Skipping the scheduling
        //     if(maxPFMetric == 0)
        //     {
        //         cout<<"Error in Scheduling.."<<endl;abort();
        //     }

    }

    else
    {
        if(selectedRBG==-1)
        {
            cout<<"Roundrobin scheduling cant perform band selection.."<<endl;
            abort();
        }

        int nSubbandsPerUE=1;
        int roundRobinIndx;

        if(totalSubbands>nUEs)
        {
            nSubbandsPerUE=ceil_i((double)totalSubbands/(double)nUEs);
            roundRobinIndx= mRoundRobinCounter +selectedRBG/nSubbandsPerUE;
            if(selectedRBG==(totalSubbands-1))
            {
                if(nUEs>1)
                    mRoundRobinCounter+=randi(1,nUEs-1); // Incrementing to change the start indx randomly;
                mRoundRobinCounter=mRoundRobinCounter%nUEs;
            }
        }
        else
        {
            roundRobinIndx=mRoundRobinCounter+selectedRBG/nSubbandsPerUE;
            if(selectedRBG==(totalSubbands-1))
            {
                mRoundRobinCounter+=selectedRBG/nSubbandsPerUE;
                mRoundRobinCounter=mRoundRobinCounter%nUEs;
            }
        }
        selectedUEIndx=roundRobinIndx%nUEs;
    }

    //TESTING

    //       cout<<"##################### START ########################"<<endl;
    //       cout<<"pfMetric :	"<<pfMetric<<endl<<endl;
    //       cout<<"maxPFMetricOverSBs : "<<maxPFMetricOverSBs<<endl;
    //       cout<<"maxIndexOverSBs : "<<maxIndexOverSBs<<endl;
    //       cout<<endl<<"selectedRBGIndx        : "<<selectedRBGIndx<<endl;
    //       cout<<"maxPFMetric              : "<<maxPFMetric<<endl;
    //       cout<<"UE                       : "<<contendingUEs[selectedUEIndx]<<endl;
    //       cout<<"##################### END ########################"<<endl;

#ifdef PFSCHEDULER_DEBUG
    cout<<endl<<"selectedRBGIndx        : "<<selectedRBGIndx<<endl;
    cout<<"maxPFMetric              : "<<maxPFMetric<<endl;
    cout<<"UE                       : "<<contendingUEs[selectedUEIndx]<<endl;
#endif

    ScheduledUEInfo_S schUEInfo;


    schUEInfo.pfMetric = maxPFMetric;
    schUEInfo.txMode = tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).txMode;
    schUEInfo.ueid.set_size(1);
    schUEInfo.mcsRate.set_size(1);
    schUEInfo.instantMcsRate.set_size(1);
    schUEInfo.effSINRs.set_size(1);
    schUEInfo.mcsIndxPerCodeword.set_size(1);
    schUEInfo.tbSizeInBytesPerCodeword.set_size(1);
    schUEInfo.precoderPerRB.resize(1);
    schUEInfo.precoderPerRB[0].resize(vrbNumbers(selectedRBGIndx).length());
    schUEInfo.nLayers.set_size(1);
    schUEInfo.ueid(0)=contendingUEs[selectedUEIndx]; 
    schUEInfo.mcsRate(0)=finalMCSRatePerUE(selectedRBGIndx)(selectedUEIndx);
    schUEInfo.instantMcsRate(0)=instantaneousMCSRatePerUE(selectedRBGIndx)(selectedUEIndx);
    schUEInfo.effSINRs(0)=tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).sinrInDB; 
    schUEInfo.cqiType=tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).cqiType;
    schUEInfo.mcsIndxPerCodeword(0)=possbileMCSPerCodeword(selectedRBGIndx)(selectedUEIndx);
    schUEInfo.tbSizeInBytesPerCodeword(0)=tbSizePerCodewordInBytes(selectedRBGIndx)(selectedUEIndx);
    schUEInfo.symPos.set_length(1);schUEInfo.symPos(0)=symbolsToSchedule;

    // reciprocity based precoder 
  if(useReciproPrecode && tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).reciproPrecoder.size() !=0) // if reciprocity based precoder present
    {
        cout<<"Entered in DL reciprocity based in scheduler..."<<endl;
        for(int rb_cnt = 0; rb_cnt < vrbNumbers(selectedRBGIndx).length(); rb_cnt++)
    {
        //normalising the precoder based on number of layers
      cmat precoder=tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).reciproPrecoder(rb_cnt);
      int nLayers= tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).nLayers;
      precoder=precoder.get_cols(0,nLayers-1);
       double sum=0.0;
          for(int prec_rows=0;prec_rows<precoder.rows();prec_rows++)
          {
              for(int prec_cols=0;prec_cols<precoder.cols();prec_cols++)
              {
                  sum=sum+(abs(precoder(prec_rows,prec_cols))* abs(precoder(prec_rows,prec_cols)));
              }
          }
          precoder=precoder/(sqrt(sum));   
      
      schUEInfo.precoderPerRB[0][rb_cnt] =precoder;

    } 
        cout<<"[both:]Reciprocity based precoder is found in downlink for UE "<<selectedUEIndx<<". Updating in scheduler for same..."<<endl;
        
    }
    else
    {
        int txAntennaCount=eNBAntennaCount;

        if(!mIsDownlink)
            txAntennaCount=fGetUEAntennaCount(contendingUEs[selectedUEIndx]);


        if(schUEInfo.txMode == _TRANSMISSION_MODE_NR_)
        {
            for(int rb_cnt = 0; rb_cnt < vrbNumbers(selectedRBGIndx).length(); rb_cnt++)
            {
                if(mIsDownlink)
                {
                    if(tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).isPrecoderAvailable)
                    {
                        schUEInfo.precoderPerRB[0][rb_cnt] = tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).precoder(rb_cnt);
                    }
                    else
                    {
                        type1PMI_S wbPMIIndStruct;
                        wbPMIIndStruct.i11 = tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).i1_1;
                        wbPMIIndStruct.i12 = tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).i1_2;
                        wbPMIIndStruct.i13 = tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).i1_3;
                        wbPMIIndStruct.i14 = tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).i1_4;
                        wbPMIIndStruct.i2 = tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).i2(rb_cnt);
                        schUEInfo.precoderPerRB[0][rb_cnt] = mCodebook->getPrecoderNR(tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).nLayers,wbPMIIndStruct);
                    }
                }
                else
                {
                    if(txAntennaCount>1)
                    {
                        Array<cmat> nCodebookMatrix=mCodebook->getCodebook(txAntennaCount,tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).nLayers,mIsDownlink);

                        for(int rb_cnt = 0; rb_cnt < vrbNumbers(selectedRBGIndx).length(); rb_cnt++)
                        {	   
                            schUEInfo.precoderPerRB[0][rb_cnt] = nCodebookMatrix(tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).pmi(rb_cnt));
                        }
                    }
                    else
                        for(int rb_cnt = 0; rb_cnt < vrbNumbers(selectedRBGIndx).length(); rb_cnt++)
                            schUEInfo.precoderPerRB[0][rb_cnt]=ones_c(1,1);
                }

            }
        }
        else
        {
            Array<cmat> nCodebookMatrix=mCodebook->getCodebook(txAntennaCount,tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).nLayers,mIsDownlink);

            for(int rb_cnt = 0; rb_cnt < vrbNumbers(selectedRBGIndx).length(); rb_cnt++)
            {	   
                schUEInfo.precoderPerRB[0][rb_cnt] = nCodebookMatrix(tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).pmi(rb_cnt));
            }
        }
    }

    schUEInfo.nLayers(0)=tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).nLayers;
    schUEInfo.txMode=tUEInfoPerSubbandsPerUEs(selectedRBGIndx)(selectedUEIndx).txMode;
    schUEInfo.vrbs=vrbNumbers(selectedRBGIndx);
    schUEInfo.rbStructurePerRB=rbStructurePerRB(selectedRBGIndx)(selectedUEIndx);

    return(schUEInfo);
}

ScheduledUEInfo_S Scheduler::fFindBestUEusingRR(int subframe, ivec contendingUEs,ivec symbolsToSchedule, Array< sCqiList >& cqiList, ScheduledRBsInfo_S &scheduledRBsInfo, sScheduleReturn& schReturn, int eNBAntennaCount, Array<ivec> subbands, int selectedSB, OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, int eNBID)
{
  if(symbolsToSchedule.length()==0) symbolsToSchedule=getIntegers(0,13);
  
  ScheduledUEInfo_S schUEInfo;
  int nUEs=contendingUEs.length();
  int nSubbands=subbands.length();
  int nSubbandsPerUE=1;
  int roundRobinIndx;
  
  if(nSubbands>nUEs)
  {
    nSubbandsPerUE=ceil_i((double)nSubbands/(double)nUEs);
    roundRobinIndx= mRoundRobinCounter +selectedSB/nSubbandsPerUE;
    if(selectedSB==(nSubbands-1))
    {
      if(nUEs>1)
	mRoundRobinCounter+=randi(1,nUEs-1); // Incrementing to change the start indx randomly;
      mRoundRobinCounter=mRoundRobinCounter%nUEs;
    }
  }
  else
  {
    roundRobinIndx=mRoundRobinCounter+selectedSB/nSubbandsPerUE;
    if(selectedSB==(nSubbands-1))
    {
      mRoundRobinCounter+=selectedSB/nSubbandsPerUE;
      mRoundRobinCounter=mRoundRobinCounter%nUEs;
    }
  }
  roundRobinIndx=roundRobinIndx%nUEs;
  cout<<"contendingUEs : "<<contendingUEs<<endl;
  if(roundRobinIndx<(int)contendingUEs.size())
  {
    schUEInfo.ueid.set_size(1);
    schUEInfo.ueid(0)=contendingUEs[roundRobinIndx];
    
    int txAntennaCount;
    if(mIsDownlink)
      txAntennaCount=eNBAntennaCount;
    else
      txAntennaCount=fGetUEAntennaCount(schUEInfo.ueid(0));
    
    schUEInfo.symPos.set_length(1);schUEInfo.symPos(0)=symbolsToSchedule;
    ivec vrbNumbers=subbands(selectedSB);
    int cqiListIndx = -1;
    for(int id = 0;id<(int)cqiList.size();id++)
      if(cqiList(id).rnti == schUEInfo.ueid(0))
      {
	cqiListIndx = id;
	break;
      }
      bool isCQIReported=false;  
      if(isCQIAvailable)
    {
      isCQIReported=true;
      CQIFeedback_S cqiInfo=fGetCQIInfo(cqiList(cqiListIndx),vrbNumbers);
      schUEInfo.precoderPerRB.resize(1);
      schUEInfo.precoderPerRB[0].resize(vrbNumbers.length());
      schUEInfo.nLayers.set_size(1);
      schUEInfo.effSINRs.set_length(1);
      if(mIsDownlink)
	schUEInfo.effSINRs(0)=mTBTables->awgnTablesConvCoder.getSINRIndBfromCQI(cqiInfo.cqi);
      else
	schUEInfo.effSINRs(0)=cqiInfo.effSINRs; // For UL
	
	if(mIsDownlink)
	  schUEInfo.txMode=dlTransmissionMode(cqiListIndx);
	else
	  schUEInfo.txMode=ulTransmissionMode(cqiListIndx);
	
	schUEInfo.isPaired=false;
      //cout<<"TM : "<<schUEInfo.txMode<<endl;
      if(mIsModeAdaptationEnabled)
	schUEInfo.txMode=fGetTMMode(cqiList(cqiListIndx));
      
      
      if(schUEInfo.txMode==_TRANSMISSION_MODE_3_ && cqiInfo.ri==1)
      {
	schUEInfo.nLayers(0)=nCRSPorts;
	schUEInfo.txMode=_TRANSMISSION_MODE_2_;
      }
      else
	schUEInfo.nLayers(0)=cqiInfo.ri;
      
      
      //       // Extracted reciprocity precoder per RBG if present
      //       if(cqiInfo.reciproPrecoder.length() != 0)
      //       {
      // 	cout<<"cqiInfo.reciproPrecoder(0).rows():"<<cqiInfo.reciproPrecoder(0).rows()<<endl;
      // 	cout<<"cqiInfo.reciproPrecoder(0).cols():"<<cqiInfo.reciproPrecoder(0).cols()<<endl;
      // 	cmat tempPrecoder = zeros_c(cqiInfo.reciproPrecoder(0).rows(),cqiInfo.reciproPrecoder(0).cols());
      // 	cout<<"tempPrecoder:"<<tempPrecoder<<endl;
      // 	for(int i=0;i<vrbNumbers.length();i++)
      // 	  tempPrecoder += cqiInfo.reciproPrecoder(i);    
      // 	schUEInfo.precoderPerRB=tempPrecoder/vrbNumbers.length();
      // 	cout<<"tUEInfo.precoder:"<<tUEInfo.precoder<<endl;
      //       }
      
      if(cqiInfo.pmi(0) != -1) // Valid PMI
      {
	Array<cmat> nCodebookMatrix=mCodebook->getCodebook(txAntennaCount,schUEInfo.nLayers(0),mIsDownlink);
	for(int rb_cnt = 0; rb_cnt < vrbNumbers.length(); rb_cnt++)
	{
	  schUEInfo.precoderPerRB[0][rb_cnt] = nCodebookMatrix(cqiInfo.pmi(rb_cnt));
	}
      }
      else
      {
          
          cout<<"CQI reported with Invalid PMI .. "<<endl;abort();
          
	for(int rb_cnt = 0; rb_cnt < vrbNumbers.length(); rb_cnt++)
	{
	  schUEInfo.precoderPerRB[0][rb_cnt] = zeros_c(1,1);
	}
      }
      
      
      //cout<<"TM : "<<schUEInfo.txMode<<endl;
    }
    else //CQI Not reported
    {
      isCQIReported=false;
      schUEInfo.precoderPerRB.resize(1);
      schUEInfo.precoderPerRB[0].resize(vrbNumbers.length());
      schUEInfo.nLayers.set_size(1);
      schUEInfo.effSINRs.set_length(1);
      schUEInfo.effSINRs(0).set_length(vrbNumbers.length());
      
      
      TransmissionMode_E txMode;
      if(mIsDownlink)
      {
	txMode=dlTransmissionMode(cqiListIndx);
	schUEInfo.nLayers(0) = nDlLayers;
      }
      else
      {
	txMode=ulTransmissionMode(cqiListIndx);
	schUEInfo.nLayers(0) = nUlLayers;
      }
      
      for(int rb_cnt=0;rb_cnt<vrbNumbers.length();rb_cnt++)
	schUEInfo.effSINRs(0)(rb_cnt)=(-5)*ones(1); //Using SINR=-5 when no report is available
	
	int pmi = -1;
      if(txMode == _TRANSMISSION_MODE_9_ || (txMode == _TRANSMISSION_MODE_NR_ && txAntennaCount>1))
	pmi=0;  
      else if(txMode!= _TRANSMISSION_MODE_1_ && txMode!= _TRANSMISSION_MODE_3_ && txMode != _TRANSMISSION_MODE_NR_ )
	pmi=1;
      
      if(pmi != -1) // Valid PMI
      {
	Array<cmat> nCodebookMatrix=mCodebook->getCodebook(txAntennaCount,schUEInfo.nLayers(0),mIsDownlink);
	
	for(int rb_cnt = 0; rb_cnt < vrbNumbers.length(); rb_cnt++)
	{
	  schUEInfo.precoderPerRB[0][rb_cnt] = nCodebookMatrix(pmi);
	}
      }
      else
      {
	for(int rb_cnt = 0; rb_cnt < vrbNumbers.length(); rb_cnt++)
	{
	  schUEInfo.precoderPerRB[0][rb_cnt] = ones_c(1,1);
	}
      }
      schUEInfo.txMode=txMode;
      //cout<<"TM : "<<schUEInfo.txMode<<endl;
    }
    
    schUEInfo.mcsRate.set_size(1);
    schUEInfo.mcsRate(0)=-1;
    
    LayerInfo_S layerInfo(schUEInfo.nLayers(0),dmrsType,dmrsLength);
    computeCDMsizeAndcount(to_ivec(schUEInfo.nLayers(0)),layerInfo.CDM,layerInfo.nCDM,dmrsType,dmrsLength);
    layerInfo.CDMMap = zeros_b(1);
    layerInfo.CDMMap(0)=1;
    
    RSInfo_S rsInfo;
    if(mIsDownlink)
     setRsInfo(rsInfo,*mPortsInfo,*mFrameStructure,cellID,TransmissionScheme_E((int)schUEInfo.txMode),layerInfo,txAntennaCount,0,mIsDownlink,isCQIPortAvailable,symbolsToSchedule(0),isTRSPortAvailable,mTrsConf);
    else
      setRsInfo(rsInfo,*mPortsInfo,*mFrameStructure,cellID,TransmissionScheme_E((int)schUEInfo.txMode),layerInfo,txAntennaCount,0,mIsDownlink,isCQIPortAvailable,symbolsToSchedule(0));
    
    schUEInfo.vrbs=vrbNumbers;
    
    schUEInfo.rbStructurePerRB=getMergedRBStructures(mFrameStructure,rsInfo,vrbNumbers,mIsDownlink,true,mFrameReservationInfo);
    for(int cnt=0;cnt<schUEInfo.rbStructurePerRB.length();cnt++)
    {
      schUEInfo.rbStructurePerRB(cnt)=schUEInfo.rbStructurePerRB(cnt).get_cols(symbolsToSchedule);
    }
    
    Array<vec> possibleEffSINRs;
    Array<imat> possibleRBStructures;

    scheduledRBsInfo.getScheduledRBsInfo(schUEInfo.ueid[0],possibleEffSINRs,possibleRBStructures);
    
    //! Appending the current CQI Indices
    append(possibleRBStructures,schUEInfo.rbStructurePerRB);
    
    append(possibleEffSINRs,schUEInfo.effSINRs(0));
    
    int nNumOfRbs = vrbNumbers.length() + fFindNumOfRBsScheduled(schReturn.mResourceBlockMapping,schReturn.UEIndex,schUEInfo.ueid(0));
    ivec rbsToConsider=concat(vrbNumbers,fgetRBsScheduled(schReturn.mResourceBlockMapping,schReturn.UEIndex,schUEInfo.ueid(0)));
    
    LinkAdaptation_S tLinkAdaptation;
    
    schUEInfo.mcsIndxPerCodeword.set_size(1);
    tLinkAdaptation = fDoLinkAdaptation(schUEInfo.ueid(0),schUEInfo.nLayers(0),schUEInfo.txMode,rbsToConsider,symbolsToSchedule,possibleRBStructures,possibleEffSINRs,tOuterLoopLinkAdaptation,isCQIReported);
    schUEInfo.mcsIndxPerCodeword(0) = tLinkAdaptation.mcsIndxPerCodeword; 
    
    schUEInfo.tbSizeInBytesPerCodeword.set_length(1);
    schUEInfo.tbSizeInBytesPerCodeword(0) = tLinkAdaptation.tbSizePerCodewordInBytes;
    
    return(schUEInfo);
  }
  else
  {
    cout<<roundRobinIndx<<endl;
    cout<<contendingUEs.size()<<endl;
    cout<<"[detl:]Error. Round Robin Indx out of bound in fFindBestUEusingRR()."<<endl;
    abort();
  }
}

ScheduledUEInfo_S Scheduler::fFindBestUEsForMUMIMOusingSubbandCSI(int subframe, ivec& contendingUEs,ivec symbolsToSchedule, Array< sCqiList >& cqiList, ScheduledRBsInfo_S &scheduledRBsInfo, sScheduleReturn& schReturn, int eNBAntennaCount, Array< ivec > vrbNumbers, OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, ivec& scheduledRBGIndices, int& selectedRBG, int eNBID, Array<ivec> &pairedUEs, Array<vec> &SINRInDBOfPairedUEs,int beamID,string PairingFileName)
{
    if(symbolsToSchedule.length()==0) symbolsToSchedule=getIntegers(0,13);
    ScheduledUEInfo_S schUEInfo;
    int selectedRBGtemp = selectedRBG;
    if(selectedRBGtemp != -1)
    {
        ivec rbNumbers = vrbNumbers(selectedRBGtemp);
        vrbNumbers.set_length(1);
        vrbNumbers(0) = rbNumbers;
    }

    int nSubbands = vrbNumbers.length();
    int nUEs = (int)contendingUEs.size();

    //! Collecting CQI Indices of the contending UEs
    Array< Array< UEInfo_S > > tUEInfoPerSubbandsPerUEs(nSubbands);

    for(int iSB = 0; iSB < nSubbands; iSB++)
    {
        // Skipping already scheduled RBG
        if(nSubbands > 1 && find(scheduledRBGIndices,iSB) !=-1)
        {
            continue;
        }
        tUEInfoPerSubbandsPerUEs(iSB).set_length(nUEs);
        for(int iUE=0;iUE<(int)cqiList.size();iUE++)
        {
            int ueIndx=find(contendingUEs, cqiList(iUE).rnti);
            if(ueIndx!=-1)
            {
                if(mIsDownlink)
                    tUEInfoPerSubbandsPerUEs(iSB)(ueIndx) = fCollectUEInfo(cqiList(iUE),dlTransmissionMode(iUE),vrbNumbers(iSB));
                else
                    tUEInfoPerSubbandsPerUEs(iSB)(ueIndx) = fCollectUEInfo(cqiList(iUE),ulTransmissionMode(iUE),vrbNumbers(iSB));

            }
        }

    }
    //! Collecting average Rate so far of the contenting UEs
    vec avgRatePerUE = fCollectAvgRate(contendingUEs);

    Array<Array<vec>> finalMCSRatePerUEpair(nSubbands);
    Array<Array<vec>> instantMCSRatePerUEpair(nSubbands);
    Array<vec> instantMCSRate(nSubbands);
    Array<Array< Array <ivec> > >tbSizePerCodewordInBytesPerUEpair(nSubbands);
    Array<Array< Array <ivec> > >possbileMCSPerCodewordPerUEpair(nSubbands);
    Array<Array < Array< Array<imat> > > > rbStructurePerRBPerUEpair(nSubbands);
    Array<vec> pfMetricPerUEPair(nSubbands);
    Array<Array<MUMIMOPairingInfo_S>> MUMIMOPairingUsers(nSubbands);

    double maxPFMetric = -1.0;
    int selectedUEIndx=-1;
    for(int sb_cnt = 0; sb_cnt < nSubbands; sb_cnt++)
    {
        // Skipping already scheduled RBG
        if(nSubbands > 1 && find(scheduledRBGIndices,sb_cnt) !=-1)
        {
            continue;
        }  

        //     if(isToUseMUMIMOPairs)
        //     {
        //       MUMIMOPairingUsers(sb_cnt)=retainedMUMIMOPairs;
        //     }
        //     else
        //     {
        //       if(mIsDownlink)
        //           fFindMUMIMOPairingOptions(MUMIMOPairingUsers(sb_cnt),tUEInfoPerSubbandsPerUEs(sb_cnt),mCodebook,eNBAntennaCount,dlTransmissionMode(0),pairedUEs, SINRInDBOfPairedUEs);
        //       else
        //           fFindMUMIMOPairingOptions(MUMIMOPairingUsers(sb_cnt),tUEInfoPerSubbandsPerUEs(sb_cnt),mCodebook,eNBAntennaCount,ulTransmissionMode(0),pairedUEs, SINRInDBOfPairedUEs);
        //     }

        TransmissionMode_E txMode = (mIsDownlink) ? dlTransmissionMode(0) : ulTransmissionMode(0);
        if(txMode == _TRANSMISSION_MODE_NR_)
        {
            fFindMUMIMOPairingOptionsForNR(MUMIMOPairingUsers(sb_cnt),tUEInfoPerSubbandsPerUEs(sb_cnt),pairedUEs);
        }
        else 
        {
            cout<<"MU-MIMO is enabled "<<endl;
            cout<<"Error in TX Mode. Please set TMNR for using MU-MIMO"<<endl;
            abort();
        }
        
        int nMUMIMOPairs = MUMIMOPairingUsers(sb_cnt).length();

        finalMCSRatePerUEpair(sb_cnt).set_length(nMUMIMOPairs);
        instantMCSRatePerUEpair(sb_cnt).set_length(nMUMIMOPairs);
        instantMCSRate(sb_cnt).set_length(nMUMIMOPairs);
        tbSizePerCodewordInBytesPerUEpair(sb_cnt).set_length(nMUMIMOPairs);
        possbileMCSPerCodewordPerUEpair(sb_cnt).set_length(nMUMIMOPairs);
        rbStructurePerRBPerUEpair(sb_cnt).set_length(nMUMIMOPairs);
        pfMetricPerUEPair(sb_cnt)=zeros(nMUMIMOPairs);

        //MU-MIMO
        for(int pair_cnt=0;pair_cnt<nMUMIMOPairs;pair_cnt++)
        {
            int nPairedUEs = MUMIMOPairingUsers(sb_cnt)(pair_cnt).mUEIds.size();
            
            finalMCSRatePerUEpair(sb_cnt)(pair_cnt) = zeros(nPairedUEs);
            instantMCSRatePerUEpair(sb_cnt)(pair_cnt) = zeros(nPairedUEs);
            tbSizePerCodewordInBytesPerUEpair(sb_cnt)(pair_cnt).set_length(nPairedUEs);
            possbileMCSPerCodewordPerUEpair(sb_cnt)(pair_cnt).set_length(nPairedUEs);
            rbStructurePerRBPerUEpair(sb_cnt)(pair_cnt).set_length(nPairedUEs);
            
            for(int ue_cnt=0;ue_cnt<nPairedUEs;ue_cnt++)
            {
                int UEid = -1, UEindx = -1;
                for(int i = 0; i < tUEInfoPerSubbandsPerUEs(sb_cnt).length(); i++)
                {
                    if(tUEInfoPerSubbandsPerUEs(sb_cnt)(i).UEid == MUMIMOPairingUsers(sb_cnt)(pair_cnt).mUEIds(ue_cnt))
                    {
                        UEid = tUEInfoPerSubbandsPerUEs(sb_cnt)(i).UEid;
                        UEindx = i;
                        break;
                    }
                }
                if(UEindx == -1)
                {
                    cerr<<"Error in MUMIMO scheduler "<<endl; abort();
                }

                Array<vec> possibleEffSINRs;
                Array<imat> possibleRBStructures;

                LinkAdaptation_S tLinkAdaptation;
                ivec tbSizePerCodewordOldInBytes(0);

                scheduledRBsInfo.getScheduledRBsInfo(UEid,possibleEffSINRs,possibleRBStructures);
                if(possibleEffSINRs.length()>0)
                {
                    tLinkAdaptation = fDoLinkAdaptation(UEid,tUEInfoPerSubbandsPerUEs(sb_cnt)(UEindx).nLayers,tUEInfoPerSubbandsPerUEs(sb_cnt)(UEindx).txMode,scheduledRBsInfo.getScheduledRBs(UEid),symbolsToSchedule,possibleRBStructures,possibleEffSINRs,tOuterLoopLinkAdaptation,tUEInfoPerSubbandsPerUEs(sb_cnt)(UEindx).isCQIReported);
                    tbSizePerCodewordOldInBytes = tLinkAdaptation.tbSizePerCodewordInBytes;
                }

                int txAntennaCount=eNBAntennaCount;
                if(!mIsDownlink)
                    txAntennaCount=fGetUEAntennaCount(UEid);

                LayerInfo_S layerInfo(tUEInfoPerSubbandsPerUEs(sb_cnt)(UEindx).nLayers,dmrsType,dmrsLength);
                ivec layersPerUE = MUMIMOPairingUsers(sb_cnt)(pair_cnt).nLayers;
                computeCDMsizeAndcount(layersPerUE,layerInfo.CDM,layerInfo.nCDM,dmrsType,dmrsLength);
                layerInfo.CDMMap = zeros_b(layersPerUE.length());
                int indx = find(MUMIMOPairingUsers(sb_cnt)(pair_cnt).mUEIds,UEid);
                layerInfo.CDMMap(indx)=1;

                RSInfo_S rsInfo;
                if(mIsDownlink)
                    //setRsInfo(rsInfo,*mPortsInfo,*mFrameStructure,cellID,TransmissionScheme_E((int)tUEInfoPerSubbandsPerUEs(sb_cnt)(UEindx).txMode),LayerInfo_S(tUEInfoPerSubbandsPerUEs(sb_cnt)(UEindx).nLayers,sum(MUMIMOPairingUsers(sb_cnt)(pair_cnt).nLayers)),txAntennaCount,0,mIsDownlink,isCQIPortAvailable,symbolsToSchedule(0),isTRSPortAvailable,mTrsConf);
                    setRsInfo(rsInfo,*mPortsInfo,*mFrameStructure,cellID,TransmissionScheme_E((int)tUEInfoPerSubbandsPerUEs(sb_cnt)(UEindx).txMode),layerInfo,txAntennaCount,0,mIsDownlink,isCQIPortAvailable,symbolsToSchedule(0),isTRSPortAvailable,mTrsConf);
                else
                    setRsInfo(rsInfo,*mPortsInfo,*mFrameStructure,cellID,TransmissionScheme_E((int)tUEInfoPerSubbandsPerUEs(sb_cnt)(UEindx).txMode),layerInfo,txAntennaCount,0,mIsDownlink,isCQIPortAvailable,symbolsToSchedule(0));

	  if(mPortsInfo->enablePTRS == true)
	    addptrs(rsInfo,*mPortsInfo,*mFrameStructure,cellID,TransmissionScheme_E((int)tUEInfoPerSubbandsPerUEs(sb_cnt)(UEindx).txMode),cqiList(UEindx).rnti,scheduledRBsInfo.getScheduledRBs(contendingUEs[UEindx]),0,1);
	  
rbStructurePerRBPerUEpair(sb_cnt)(pair_cnt)(ue_cnt)=getMergedRBStructures(mFrameStructure,rsInfo,vrbNumbers(sb_cnt),mIsDownlink,true,mFrameReservationInfo);
                for(int cnt=0;cnt<rbStructurePerRBPerUEpair(sb_cnt)(pair_cnt)(ue_cnt).length();cnt++)
                    rbStructurePerRBPerUEpair(sb_cnt)(pair_cnt)(ue_cnt)(cnt)=rbStructurePerRBPerUEpair(sb_cnt)(pair_cnt)(ue_cnt)(cnt).get_cols(symbolsToSchedule);

                Array<vec> currentSINRInDB=addAcrossArray(tUEInfoPerSubbandsPerUEs(sb_cnt)(UEindx).sinrInDB,MUMIMOPairingUsers(sb_cnt)(pair_cnt).SINRoffsetInDB(ue_cnt));

                tLinkAdaptation = fDoLinkAdaptation(UEid,tUEInfoPerSubbandsPerUEs(sb_cnt)(UEindx).nLayers,tUEInfoPerSubbandsPerUEs(sb_cnt)(UEindx).txMode,vrbNumbers(sb_cnt),symbolsToSchedule,rbStructurePerRBPerUEpair(sb_cnt)(pair_cnt)(ue_cnt),currentSINRInDB,tOuterLoopLinkAdaptation,tUEInfoPerSubbandsPerUEs(sb_cnt)(UEindx).isCQIReported);

                append(possibleEffSINRs,currentSINRInDB);
                append(possibleRBStructures,rbStructurePerRBPerUEpair(sb_cnt)(pair_cnt)(ue_cnt));

                int nNumOfRbs = vrbNumbers(sb_cnt).length() + fFindNumOfRBsScheduled(schReturn.mResourceBlockMapping,schReturn.UEIndex,UEid);
                ivec rbsToConsider=concat(vrbNumbers(sb_cnt),fgetRBsScheduled(schReturn.mResourceBlockMapping,schReturn.UEIndex,UEid));

                tLinkAdaptation = fDoLinkAdaptation(UEid,tUEInfoPerSubbandsPerUEs(sb_cnt)(UEindx).nLayers,tUEInfoPerSubbandsPerUEs(sb_cnt)(UEindx).txMode,rbsToConsider,symbolsToSchedule,possibleRBStructures,possibleEffSINRs,tOuterLoopLinkAdaptation,tUEInfoPerSubbandsPerUEs(sb_cnt)(UEindx).isCQIReported);

                instantMCSRatePerUEpair(sb_cnt)(pair_cnt)(ue_cnt) = tLinkAdaptation.mcsRate;

                if(tbSizePerCodewordOldInBytes.length() > 0 && sum(tbSizePerCodewordOldInBytes) >= sum(tLinkAdaptation.tbSizePerCodewordInBytes))
                {
                    instantMCSRatePerUEpair(sb_cnt)(pair_cnt)(ue_cnt) = 0.001;
                }
                instantMCSRate(sb_cnt)(pair_cnt)=sum(instantMCSRatePerUEpair(sb_cnt)(pair_cnt));
                possbileMCSPerCodewordPerUEpair(sb_cnt)(pair_cnt)(ue_cnt) = tLinkAdaptation.mcsIndxPerCodeword;
                tbSizePerCodewordInBytesPerUEpair(sb_cnt)(pair_cnt)(ue_cnt) = tLinkAdaptation.tbSizePerCodewordInBytes;
                finalMCSRatePerUEpair(sb_cnt)(pair_cnt)(ue_cnt) = tLinkAdaptation.mcsRate;

                pfMetricPerUEPair(sb_cnt)(pair_cnt) += (instantMCSRatePerUEpair(sb_cnt)(pair_cnt)(ue_cnt)/avgRatePerUE(find(contendingUEs,UEid)));

            }

            if(find(instantMCSRatePerUEpair(sb_cnt)(pair_cnt),0.0)!=-1)
                pfMetricPerUEPair(sb_cnt)(pair_cnt) = 0;
        }
    }

    //! Selecting UE based on PF Metric
    vec maxPFMetricOverSBs = zeros(pfMetricPerUEPair.length());
    ivec maxIndexOverSBs = zeros_i(pfMetricPerUEPair.length()) - 1;
    //get the small r
    for(int sb_cnt = 0; sb_cnt < pfMetricPerUEPair.length(); sb_cnt++)
    {
        if(pfMetricPerUEPair(sb_cnt).length()>0)
        {
            maxPFMetricOverSBs(sb_cnt) = max(pfMetricPerUEPair(sb_cnt));
            ivec possibleselectedUEIndices = find(pfMetricPerUEPair(sb_cnt),max(pfMetricPerUEPair(sb_cnt)),true);
            if(possibleselectedUEIndices.length()==1)
                maxIndexOverSBs(sb_cnt)=possibleselectedUEIndices(0);
            else
                maxIndexOverSBs(sb_cnt)=randPick(possibleselectedUEIndices,1)(0);
        }
    }
    maxPFMetric = max(maxPFMetricOverSBs);
    ivec possibleSelectedSBs = find(maxPFMetricOverSBs,max(maxPFMetricOverSBs),true);

    if(possibleSelectedSBs.length()==1)
        selectedRBGtemp=possibleSelectedSBs(0);
    else
        selectedRBGtemp=randPick(possibleSelectedSBs,1)(0);

    int selectedUEPairIndx = maxIndexOverSBs(selectedRBGtemp);

    append(pairedUEs,MUMIMOPairingUsers(selectedRBGtemp)(selectedUEPairIndx).mUEIds);
    append(SINRInDBOfPairedUEs,getAcrossArray(MUMIMOPairingUsers(selectedRBGtemp)(selectedUEPairIndx).expectedSINRInDB,0));

    int nUEsInMUMIMO = MUMIMOPairingUsers(selectedRBGtemp)(selectedUEPairIndx).mUEIds.size();

#ifdef MUMIMO_DEBUG
    if(nUEsInMUMIMO>1)
        cout<<"MU-MIMO pairing of "<<MUMIMOPairingUsers(selectedRBGtemp)(selectedUEPairIndx).mUEIds<<" in RBG : "<<selectedRBGtemp<<" with SINR offset : "<<MUMIMOPairingUsers(selectedRBGtemp)(selectedUEPairIndx).SINRoffsetInDB<<" and ranks "<<MUMIMOPairingUsers(selectedRBGtemp)(selectedUEPairIndx).nLayers<<endl;
    else
        cout<<"SU-MIMO of "<<MUMIMOPairingUsers(selectedRBGtemp)(selectedUEPairIndx).mUEIds<<" is used in RBG : "<<selectedRBGtemp<<" and rank "<<MUMIMOPairingUsers(selectedRBGtemp)(selectedUEPairIndx).nLayers<<endl;
#else
    cout<<"RBG: "<<((nSubbands>1) ? selectedRBGtemp : selectedRBG)<<"\tPairedUEs: "<<MUMIMOPairingUsers(selectedRBGtemp)(selectedUEPairIndx).mUEIds<<endl;
#endif

    schUEInfo.ueid.set_size(nUEsInMUMIMO);
    schUEInfo.mcsRate.set_size(nUEsInMUMIMO);
    schUEInfo.instantMcsRate.set_size(nUEsInMUMIMO);
    schUEInfo.mcsIndxPerCodeword.set_size(nUEsInMUMIMO);
    schUEInfo.tbSizeInBytesPerCodeword.set_size(nUEsInMUMIMO);
    schUEInfo.precoderPerRB.resize(nUEsInMUMIMO);
    //schUEInfo.pmi.set_size(nUEsInMUMIMO);
    schUEInfo.nLayers.set_size(nUEsInMUMIMO);
    schUEInfo.symPos.set_length(nUEsInMUMIMO);
    schUEInfo.effSINRs.set_length(nUEsInMUMIMO);
    schUEInfo.MUMIMOPair=MUMIMOPairingUsers(selectedRBGtemp)(selectedUEPairIndx);

    for(int ue_cnt = 0; ue_cnt < nUEsInMUMIMO; ue_cnt++)
    {
        schUEInfo.ueid(ue_cnt) = MUMIMOPairingUsers(selectedRBGtemp)(selectedUEPairIndx).mUEIds(ue_cnt);
        schUEInfo.mcsRate(ue_cnt)=finalMCSRatePerUEpair(selectedRBGtemp)(selectedUEPairIndx)(ue_cnt);
        schUEInfo.instantMcsRate(ue_cnt)=instantMCSRatePerUEpair(selectedRBGtemp)(selectedUEPairIndx)(ue_cnt);

        int indx = -1;
        for(int i = 0; i < tUEInfoPerSubbandsPerUEs(selectedRBGtemp).length(); i++)
        {
            if(tUEInfoPerSubbandsPerUEs(selectedRBGtemp)(i).UEid == schUEInfo.ueid(ue_cnt))
                indx = i;
        }
        if(indx == -1)
        {
            cerr<<"Error in MUMIMO scheduler "<<endl;
        }
        schUEInfo.cqiType=tUEInfoPerSubbandsPerUEs(selectedRBGtemp)(indx).cqiType;
        //schUEInfo.pmi(ue_cnt)=tUEInfoPerUEs(indx).pmi(0);
        schUEInfo.mcsIndxPerCodeword(ue_cnt)=possbileMCSPerCodewordPerUEpair(selectedRBGtemp)(selectedUEPairIndx)(ue_cnt);
        schUEInfo.tbSizeInBytesPerCodeword(ue_cnt)=tbSizePerCodewordInBytesPerUEpair(selectedRBGtemp)(selectedUEPairIndx)(ue_cnt);

        schUEInfo.precoderPerRB[ue_cnt].resize(tUEInfoPerSubbandsPerUEs(selectedRBGtemp)(indx).vrbNumbers.length());

        // Primary check to verify all precoders of RBG are same
        if(pairingMethod == _SUCCESSIVE_MMSE_ || pairingMethod == _ZERO_FORCING_)
        {
            for(int rb_cnt=0;rb_cnt<tUEInfoPerSubbandsPerUEs(selectedRBGtemp)(indx).vrbNumbers.length();rb_cnt++)
            {
                if(tUEInfoPerSubbandsPerUEs(selectedRBGtemp)(indx).precoder(0)!=tUEInfoPerSubbandsPerUEs(selectedRBGtemp)(indx).precoder(rb_cnt))
                {
                    cout<<" precoders in same RBG are not matching ... aborting"<<endl;abort();
                }
                schUEInfo.precoderPerRB[ue_cnt][rb_cnt]=MUMIMOPairingUsers(selectedRBGtemp)(selectedUEPairIndx).modifiedMuMimoPrecoders(ue_cnt);

                //             cout<<"selected offset: "<< MUMIMOPairingUsers(selectedRBG)(selectedUEPairIndx).SINRoffsetInDB(ue_cnt)<<endl;abort();
            }

            //          cout<< "\n selected pair index: "<< selectedUEPairIndx<< "\n UE indx in pair: "<< tUEInfoPerSubbandsPerUEs(selectedRBG)(indx).UEid <<endl;
            //         cout<< "\n modified precoder at the end: "<< tUEInfoPerSubbandsPerUEs(selectedRBG)(indx).MuMimoPrecoder(selectedUEPairIndx) <<endl;
        }
        else
        {
            for(int rb_cnt=0;rb_cnt<tUEInfoPerSubbandsPerUEs(selectedRBGtemp)(indx).vrbNumbers.length();rb_cnt++)
                schUEInfo.precoderPerRB[ue_cnt][rb_cnt]=tUEInfoPerSubbandsPerUEs(selectedRBGtemp)(indx).precoder(rb_cnt);
        }

        schUEInfo.symPos(ue_cnt)=symbolsToSchedule;

        int txAntennaCount=eNBAntennaCount;
        if(!mIsDownlink)
            txAntennaCount=fGetUEAntennaCount(schUEInfo.ueid(ue_cnt));

        schUEInfo.nLayers(ue_cnt)=MUMIMOPairingUsers(selectedRBGtemp)(selectedUEPairIndx).nLayers(ue_cnt);

        schUEInfo.rbStructurePerRB=rbStructurePerRBPerUEpair(selectedRBGtemp)(selectedUEPairIndx)(ue_cnt);
        schUEInfo.txMode=tUEInfoPerSubbandsPerUEs(selectedRBGtemp)(indx).txMode;
        schUEInfo.effSINRs(ue_cnt)=addAcrossArray(tUEInfoPerSubbandsPerUEs(selectedRBGtemp)(indx).sinrInDB,MUMIMOPairingUsers(selectedRBGtemp)(selectedUEPairIndx).SINRoffsetInDB(ue_cnt));
        schUEInfo.vrbs=vrbNumbers(selectedRBGtemp);
    }

    if(nUEsInMUMIMO>0 && traceValuePerTraceName["EnableBSlogs"]=="true")
        printMUMIMOpairingInfo(schUEInfo,subframe,eNBID,beamID,selectedRBGtemp,PairingFileName);

    if(nSubbands>1) selectedRBG = selectedRBGtemp;

    return(schUEInfo);
}

ScheduledUEInfo_S Scheduler::fFindBestUEsFornonSlotusingSubbandCSI(int subframe, ivec& contendingUEs, ivec& miniSlotIndxPerUE, Array< sCqiList >& cqiList,ScheduledRBsInfo_S &scheduledRBsInfo, sScheduleReturn& schReturn, int eNBAntennaCount, Array< ivec > vrbNumbers, OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, ivec& scheduledRBGIndices, int& selectedRBG, ivec invalidUEsForSUMIMO, int eNBID)
{
    ScheduledUEInfo_S schUEInfo;

    if(symbolsPerTTI.length() ==0)
    {
        cout<<"forgot to mention the symbol length ::"<<endl;
    }
    for(int minislotIndx =0; minislotIndx < symbolsPerTTI.length();minislotIndx++)
    {
        ScheduledUEInfo_S tempScheduledUEinfoForSUMIMO;

        bvec selectedUE=zeros_b(contendingUEs.length());
        for(int cnt=0;cnt<contendingUEs.length();cnt++)
            if(miniSlotIndxPerUE(cnt)==minislotIndx || miniSlotIndxPerUE(cnt)==-1)
                selectedUE(cnt)=(bin)1;  

        ivec shortlistedUsers =contendingUEs(find(selectedUE));

        tempScheduledUEinfoForSUMIMO = fFindBestUEusingSubbandCSI(subframe,shortlistedUsers,symbolsPerTTI(minislotIndx),cqiList,scheduledRBsInfo,schReturn,eNBAntennaCount,vrbNumbers,tOuterLoopLinkAdaptation,scheduledRBGIndices,selectedRBG,invalidUEsForSUMIMO,eNBID);
        if(tempScheduledUEinfoForSUMIMO.ueid.length() == 0)
        {
            continue;
        }
        miniSlotIndxPerUE(find(contendingUEs,tempScheduledUEinfoForSUMIMO.ueid(0)))=minislotIndx;
        append(schUEInfo.ueid,tempScheduledUEinfoForSUMIMO.ueid);
        append(schUEInfo.mcsRate,tempScheduledUEinfoForSUMIMO.mcsRate);
        append(schUEInfo.instantMcsRate,tempScheduledUEinfoForSUMIMO.instantMcsRate); 
        append(schUEInfo.mcsIndxPerCodeword,tempScheduledUEinfoForSUMIMO.mcsIndxPerCodeword);
        append( schUEInfo.tbSizeInBytesPerCodeword,tempScheduledUEinfoForSUMIMO.mcsIndxPerCodeword);
        //What is this precoder, I cant run for other modes.
        schUEInfo.precoderPerRB.push_back(tempScheduledUEinfoForSUMIMO.precoderPerRB[0]);
        //append(schUEInfo.pmi,tempScheduledUEinfoForSUMIMO.pmi);
        append(schUEInfo.nLayers,tempScheduledUEinfoForSUMIMO.nLayers); 
        //I didnt change the frame structure length while passing on.
        //     cout<<"check the structure ::"<<tempScheduledUEinfoForSUMIMO.rbStructurePerRB.length()<<endl;
        append(schUEInfo.rbStructurePerRB,tempScheduledUEinfoForSUMIMO.rbStructurePerRB); 
        schUEInfo.txMode=tempScheduledUEinfoForSUMIMO.txMode;
        //We have to pass the length of symbols also ::
        schUEInfo.vrbs=tempScheduledUEinfoForSUMIMO.vrbs;
        append(schUEInfo.symPos,symbolsPerTTI(minislotIndx));

    }
    return(schUEInfo);
}


///Modified version of fFindBestUEforUL to use SINR instead of CQI
//  For Uplink
// ScheduledUEInfo_S Scheduler::fFindBestUEforULNew(ivec& contendingUEs, Array< sCqiList >& cqiList, ivec& scheduledUEsPerRB, Array< ivec >& cqiForScheduledUEsPerRB, Array< vec >& effSINRsForScheduledUEsPerRB, Array< imat >& rbStructureForScheduledUEsPerRB, sScheduleReturn& schReturn, ivec vrbNumbers,ivec symbolsToSchedule,OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, int prevUEId){
//   
//   if(symbolsToSchedule.length()==0) symbolsToSchedule=getIntegers(0,13);
//   ScheduledUEInfo_S schUEInfo;
//   
//   if(mIsPF)
//   {
//     //! Collecting CQI Indices of the contending UEs
//     Array< Array<ivec> > cqiIndicesPerUE(contendingUEs.size());
//     Array< Array<vec> > effSINRsPerUE(contendingUEs.size());
//     ivec pmi(contendingUEs.size());
//     Array<Array< cmat> > reciproPrecoder;
//     reciproPrecoder.set_length(contendingUEs.size());
//     ivec nLayersPerUE(contendingUEs.size());
//     Array<TransmissionMode_E> txModePerUE(contendingUEs.size());
//     Array<eCQIType> cqiTypePerUE(contendingUEs.size());
//     for(int ue_cnt=0;ue_cnt<(int)cqiList.size();ue_cnt++)
//     {
//       int ueIndx=find(contendingUEs,cqiList(ue_cnt).rnti);
//       
//       if(ueIndx!=-1)
//       {
// 	//Assuming vrb=prb
// 	cqiTypePerUE(ueIndx)=cqiList(ue_cnt).cqiType;
// 	txModePerUE(ueIndx)=ulTransmissionMode(ue_cnt);
// 	//if(cqiList(ue_cnt).wb_cqi[0]!=NOT_DEFINED_8BIT)
// 	reciproPrecoder(ueIndx).set_length(1); // dummy assignment of size
// 	if(cqiList(ue_cnt).ulCqiReportList.size()!=0)
// 	{
// 	  CQIFeedback_S cqiInfo=fGetULCQIInfo(cqiList(ue_cnt),vrbNumbers);
// 	  cqiIndicesPerUE(ueIndx)=cqiInfo.cqi;
// 	  effSINRsPerUE(ueIndx)=cqiInfo.effSINRs; // This SINR assumes 0dB transmit power
// 	  pmi(ueIndx)=cqiInfo.pmi;
// 	  //Reciprocity precoder
// 	  reciproPrecoder(ueIndx) = cqiInfo.reciproPrecoder;
// 	  nLayersPerUE(ueIndx)=cqiInfo.ri;
// 	}
// 	else//! CQI not yet reported
// 	{
// 	  cqiIndicesPerUE(ueIndx).set_length(vrbNumbers.length());
// 	  for(int rb_cnt=0;rb_cnt<vrbNumbers.length();rb_cnt++)
// 	    cqiIndicesPerUE(ueIndx)(rb_cnt)=ones_i(1); //Using 1 cwd with cqi 1 when no report is available
// 	    
// 	    effSINRsPerUE(ueIndx).set_length(vrbNumbers.length());
// 	  for(int rb_cnt=0;rb_cnt<vrbNumbers.length();rb_cnt++)
// 	    effSINRsPerUE(ueIndx)(rb_cnt)=(-5)*ones(1); //Using SINR=-5 when no report is available
// 	    
// 	    pmi(ueIndx)=0;
// 	  nLayersPerUE(ueIndx)=nUlLayers;
// 	}
// 	
//       }
//     }
//     
//     //! Collecting average Rate so far of the contending UEs
//     vec avgRatePerUE(contendingUEs.size());
//     ivec ueIndicesInScheduler(0);
//     for(int ue_cnt=0;ue_cnt<(int)contendingUEs.size();ue_cnt++)
//     {
//       int ueIndx=find(mUEIds,contendingUEs[ue_cnt]);
//       if(ueIndx!=-1)
//       {
// 	avgRatePerUE(ue_cnt)=mUEAvgRate(ueIndx);
// 	append(ueIndicesInScheduler,ueIndx);
//       }
//       else
//       {
// 	cout<<"[detl:]Error. Unknown UEid to fFindBestUE()."<<endl;
// 	abort();
//       }
//     }
//     
//     Array<ivec> cqiPerCodeword(contendingUEs.size());
//     Array<vec> effSINRsPerCodewod(contendingUEs.size());
//     Array<ivec> possbileMCSPerCodeword(contendingUEs.size());
//     Array<ivec> tbSizePerCodewordInBytes(contendingUEs.size());
//     vec finalMCSRatePerUE=zeros(contendingUEs.size());
//     Array< Array<imat> > rbStructurePerRB(contendingUEs.size());
//     
//     for(int ue_cnt=0;ue_cnt<(int)contendingUEs.size();ue_cnt++)
//     {
//       Array<ivec> possibleCQIIndices;
//       Array<vec> possibleEffSINRs;
//       Array<imat> possibleRBStructures;
//       //! Collecting scheduled CQI Indices for the selected UE
//       if(scheduledUEsPerRB.length()!=0)
//       {
// 	ivec scheduledRBIndices=find(scheduledUEsPerRB,contendingUEs[ue_cnt],true);
// 	possibleCQIIndices=getFromArray(cqiForScheduledUEsPerRB,scheduledRBIndices);
// 	possibleEffSINRs=getFromArray(effSINRsForScheduledUEsPerRB,scheduledRBIndices);
// 	possibleRBStructures=getFromArray(rbStructureForScheduledUEsPerRB,scheduledRBIndices);	
//       }
//       
//       RSInfo_S rsInfo;
//       //setRsInfo(rsInfo,*mPortsInfo,*mFrameStructure,0,TransmissionScheme_E((int)txModePerUE(ue_cnt)),nLayersPerUE(ue_cnt),txAntennaCount[ue_cnt],0);
//       int tUEAntennaCount = fGetUEAntennaCount(contendingUEs[ue_cnt]);
//       setRsInfo(rsInfo,*mPortsInfo,*mFrameStructure,0,TransmissionScheme_E((int)txModePerUE(ue_cnt)),nLayersPerUE(ue_cnt),tUEAntennaCount,0,0,isCQIPortAvailable,symbolsToSchedule(0));
//       rbStructurePerRB(ue_cnt)=getMergedRBStructures(mFrameStructure,rsInfo,vrbNumbers,false,true,mFrameReservationInfo);
//       for(int cnt=0;cnt<rbStructurePerRB(ue_cnt).length();cnt++)
// 	rbStructurePerRB(ue_cnt)(cnt)=rbStructurePerRB(ue_cnt)(cnt).get_cols(symbolsToSchedule);
//       
//       //! Appending the current CQI Indices
//       append(possibleCQIIndices,cqiIndicesPerUE(ue_cnt));
//       append(possibleEffSINRs,effSINRsPerUE(ue_cnt));
//       append(possibleRBStructures,rbStructurePerRB(ue_cnt));
//       
//       int nNumOfRbs = vrbNumbers.length() + fFindNumOfRBsScheduled(schReturn.mResourceBlockMapping,schReturn.UEIndex,contendingUEs[ue_cnt]);
//       
//       ivec REsPerRB(possibleRBStructures.length());
//       for(int rb_cnt=0;rb_cnt<possibleRBStructures.length();rb_cnt++)
// 	REsPerRB(rb_cnt)=count(possibleRBStructures(rb_cnt),0);
//       
//       //! Determining the possible TBSize
//       #if 0
//       Array<ivec> possibleCQIIndicesOverCodeword= swapDimensions(possibleCQIIndices);
//       ivec cqiIndicesPerCodeWord(possibleCQIIndicesOverCodeword.length());
//       for(int Cwd_cnt=0;Cwd_cnt<possibleCQIIndicesOverCodeword.length();Cwd_cnt++)
// 	cqiIndicesPerCodeWord(Cwd_cnt)=mLTETables->findOptimalCQIIndex(possibleCQIIndicesOverCodeword(Cwd_cnt));
//       tbSizePerCodewordInBytes(ue_cnt).set_length(0);
//       possbileMCSPerCodeword(ue_cnt) = mLTETables->findBestMCSIndex(cqiIndicesPerCodeWord,nNumOfRbs,sum(REsPerRB),nLayersPerUE(ue_cnt),false);
//       ivec tbIndexPerCodeword = mLTETables->fGetTransportBlockMCSMapping().transportBlockIndexPUSCH(possbileMCSPerCodeword(ue_cnt)); ///Find the MCS
//       
//       for(int cwd_cnt=0;cwd_cnt<tbIndexPerCodeword.length();cwd_cnt++)
// 	append(tbSizePerCodewordInBytes(ue_cnt), mLTETables->getTBSizeForAllLayersCombined(tbIndexPerCodeword(cwd_cnt), nNumOfRbs, getNumLayersPerCodeWord(tbIndexPerCodeword.size(),nLayersPerUE(ue_cnt))(cwd_cnt))/8); /// Get TB Size in Bytes
// 	#endif
// 	//Array<ivec> tbSizesPerMCS(possibleEffSINRs.length());
// 	
//       Array<vec> possibleEffSINRsOverCodeword= swapDimensions(possibleEffSINRs);
//       
//       int nCodewords=effSINRsPerUE(ue_cnt)(0).length();
//       
//       vec sinrOffsetIndB=zeros(nCodewords);
//       
//       if(tOuterLoopLinkAdaptation.isEnabled)
// 	sinrOffsetIndB = tOuterLoopLinkAdaptation.getSINROffsetPerCWIndB(contendingUEs[ue_cnt]);
//       
//       PoAlphaSet_S po(powerControlInfo.targetPowerForLTEPowerControlindBm,powerControlInfo.pathlossCompensation);
//       po.pcfValueForPowerControl=powerControlInfo.fValueForPowerControl;	
//       double maxSCPowInDBm=23-dB(nNumOfRbs*12*tUEAntennaCount);
//       sBeamPairLink myBeamPair=beamPair(find(mUEIds,contendingUEs(ue_cnt)));
//       getUplinkPower(po,nNumOfRbs,maxSCPowInDBm,myBeamPair.rspIndBm,myBeamPair.rsrpIndBm);
//       double srsTransmitPowerInDB=(23-30)-dB(300);
//       
//       for(int Cwd_cnt=0;Cwd_cnt<nCodewords;Cwd_cnt++)
// 	possibleEffSINRsOverCodeword(Cwd_cnt)+=(dB(po.pcReqParaByMeasure.finalTxULPowerPerSubCarrierInWatts)-srsTransmitPowerInDB);	
//       
//       int nMCS=mTBTables->transportBlockMCSMapping.modulationAlphabetPUSCH.length();
//       Array<ivec> tbSizesPerMCS(nCodewords);
//       Array<vec> codeRatesPerMCS(nCodewords);
//       
//       
// //       if(cellID==0)
// //       {
// // 	cout<<"schUEInfo.ueid(0) : "<<contendingUEs[ue_cnt]<<endl;
// // 	cout<<"nNumOfRbs,maxSCPowInDBm,myBeamPair.rspIndBm,myBeamPair.rsrpIndBm : "<<nNumOfRbs<<" , "<<maxSCPowInDBm<<" , "<<myBeamPair.rspIndBm<<" , "<<myBeamPair.rsrpIndBm<<endl;
// // 	cout<<"dB(po.pcReqParaByMeasure.finalTxULPowerPerSubCarrierInWatts) : "<<dB(po.pcReqParaByMeasure.finalTxULPowerPerSubCarrierInWatts)<<endl;
// // 	cout<<"sinrOffsetIndB : "<<sinrOffsetIndB<<endl;
// // 	cout<<"Expected SINR : "<<possibleEffSINRsOverCodeword(0)<<endl;
// //       }
//       
//       for(int Cwd_cnt=0;Cwd_cnt<nCodewords;Cwd_cnt++)
//       {
// 	tbSizesPerMCS(Cwd_cnt).set_length(nMCS);
// 	codeRatesPerMCS(Cwd_cnt).set_length(nMCS);
//       }
//       
//       int nSymbols=symbolsToSchedule.length();
//       //int nCW = sSimConfig.nCodeWordsUL; ///hard-coded here, needs to be changed later
//       for(int mcs_cnt = 0; mcs_cnt < nMCS; mcs_cnt++)
//       {
// 	int tbIndex = mTBTables->transportBlockMCSMapping.transportBlockIndexPUSCH(mcs_cnt);
// 	for(int Cwd_cnt=0;Cwd_cnt<nCodewords;Cwd_cnt++)
// 	{
// 	  int modulationAlphabet = mTBTables->transportBlockMCSMapping.modulationAlphabetPUSCH(mcs_cnt);
// 	  tbSizesPerMCS(Cwd_cnt)(mcs_cnt) = mTBTables->getTBSizeForAllLayersCombined(tbIndex,sum(REsPerRB),nNumOfRbs, getNumLayersPerCodeWord(nCodewords,nLayersPerUE(ue_cnt))(Cwd_cnt),nSymbols,mIsDownlink); /// Get TB Size in Bytes
// 	  
// 	  int nCRC = mTBTables->getNumberOfCRCbits(tbSizesPerMCS(Cwd_cnt)(mcs_cnt),mTBTables->transportBlockMCSMapping.nrMCSCodeRatePUSCH(mcs_cnt));
// 
// 	  codeRatesPerMCS(Cwd_cnt)(mcs_cnt) = (tbSizesPerMCS(Cwd_cnt)(mcs_cnt)+nCRC)/(double)(sum(REsPerRB)*getNumLayersPerCodeWord(nCodewords,nLayersPerUE(ue_cnt))(Cwd_cnt) * modulationAlphabet);
// 	}
//       }
//       
//       for(int Cwd_cnt=0;Cwd_cnt<nCodewords;Cwd_cnt++)
//       {
// 	vec SINRlinear = inv_dB(possibleEffSINRs(Cwd_cnt));
// 	append(possbileMCSPerCodeword(ue_cnt), mTBTables->awgnTablesConvCoder.mcsSelect(SINRlinear,tbSizesPerMCS(Cwd_cnt),codeRatesPerMCS(Cwd_cnt),mIsDownlink,sinrOffsetIndB(Cwd_cnt)));
//       }
//       ivec tbIndexPerCodeword = mTBTables->fGetTransportBlockMCSMapping().transportBlockIndexPUSCH(possbileMCSPerCodeword(ue_cnt)); ///Find the MCS
//       
//       tbSizePerCodewordInBytes(ue_cnt).set_length(0);
//       for(int cwd_cnt=0;cwd_cnt<tbIndexPerCodeword.length();cwd_cnt++)
// 	append(tbSizePerCodewordInBytes(ue_cnt), mTBTables->getTBSizeForAllLayersCombined(tbIndexPerCodeword(cwd_cnt),sum(REsPerRB), nNumOfRbs, getNumLayersPerCodeWord(tbIndexPerCodeword.size(),nLayersPerUE(ue_cnt))(cwd_cnt),nSymbols,mIsDownlink)/8); /// Get TB Size in Bytes
// 	
// 	//! Calculating possible MCS Rates of the contending UEs
// 	finalMCSRatePerUE(ue_cnt)=(double)sum(tbSizePerCodewordInBytes(ue_cnt))/(double)sum(REsPerRB);
//     }
//     
//     int selectedUEIndx;
//     
//     ///Only UE left in the list of contending UEs
//     if(contendingUEs.size() == 1)
//       selectedUEIndx = 0;
//     else
//     {
//       vec pfMetric=elem_div(finalMCSRatePerUE,avgRatePerUE);
//       selectedUEIndx=max_index(pfMetric);
//       if(prevUEId!=contendingUEs[selectedUEIndx] && prevUEId!=-2 && prevUEId!=-1)
//       {
// 	int nNumOfRbs = fFindNumOfRBsScheduled(schReturn.mResourceBlockMapping,schReturn.UEIndex,prevUEId);
// 	if(!fCheckForFFTSizes(nNumOfRbs))
// 	{
// 	  for(int Indx=0;Indx<(int)contendingUEs.size();Indx++)
// 	    if(contendingUEs[Indx]==prevUEId)
// 	    {
// 	      selectedUEIndx=Indx;
// 	      break;
// 	    }
// 	}
//       }
//     }
//     
//     append(schUEInfo.ueid,contendingUEs[selectedUEIndx]);
//     append(schUEInfo.mcsRate,finalMCSRatePerUE(selectedUEIndx));
//     append(schUEInfo.instantMcsRate,finalMCSRatePerUE(selectedUEIndx));
//     append(schUEInfo.cqiIndices,cqiIndicesPerUE(selectedUEIndx));
//     append(schUEInfo.effSINRs,effSINRsPerUE(selectedUEIndx));
//     schUEInfo.cqiType=cqiTypePerUE(selectedUEIndx);
//     schUEInfo.symPos.set_length(1);schUEInfo.symPos(0)=symbolsToSchedule;
//     append(schUEInfo.mcsIndxPerCodeword,possbileMCSPerCodeword(selectedUEIndx));
//     append(schUEInfo.tbSizePerCodeword,tbSizePerCodewordInBytes(selectedUEIndx));
//     append(schUEInfo.pmi,pmi(selectedUEIndx));
//     append(schUEInfo.nLayers,nLayersPerUE(selectedUEIndx));
//     schUEInfo.precoderPerRB.resize(1);
//     schUEInfo.precoderPerRB[0].resize(vrbNumbers.length());
//     
// // PMI Selection for Selected UE
//     if(useReciproPrecode && reciproPrecoder(selectedUEIndx)(0).size())
//     {
//       int tUEAntennaCount = fGetUEAntennaCount(schUEInfo.ueid(0));
//        if(tUEAntennaCount == reciproPrecoder(0)(0).rows())
//       {
// 	for(int rb_cnt = 0; rb_cnt < vrbNumbers.length(); rb_cnt++)
// 	{
// 	  schUEInfo.precoderPerRB[0][rb_cnt] = reciproPrecoder(selectedUEIndx)(rb_cnt);
// 	}
// 	cout<<"[both:]Reciprocity based precoder found... Updating in scheduler ..."<<endl;
//       }
//       else{
// 	cout<<"tUEAntennaCount is not equal to reciproPrecoder cmat rows in fFindBestUEforULNew() function.. Aborting "<<endl;
// 	abort();
//       }
//     }
//     else
//     {
//       if(schUEInfo.pmi(0) != -1) // Valid PMI
//       {
// 	int tUEAntennaCount = fGetUEAntennaCount(schUEInfo.ueid(0));
// 	Array<cmat> nCodebookMatrix;
// 	if(tUEAntennaCount !=1)
// 	{
// 	  nCodebookMatrix=mCodebook->getCodebook(tUEAntennaCount,schUEInfo.nLayers(0),mIsDownlink);
// 	  for(int rb_cnt = 0; rb_cnt < vrbNumbers.length(); rb_cnt++)
// 	  {
// 	    schUEInfo.precoderPerRB[0][rb_cnt] = nCodebookMatrix(schUEInfo.pmi(0));
// 	  }
// 	}
// 	else
// 	{
// 	  for(int rb_cnt = 0; rb_cnt < vrbNumbers.length(); rb_cnt++)
// 	  {
// 	    schUEInfo.precoderPerRB[0][rb_cnt] = eye_c(1);
// 	  }
// 	}
//       }
//       else
//       {
// 	for(int rb_cnt = 0; rb_cnt < vrbNumbers.length(); rb_cnt++)
// 	{
// 	  schUEInfo.precoderPerRB[0][rb_cnt] = zeros_c(1,1);
// 	}
//       }
//     }    
//     schUEInfo.txMode=txModePerUE(selectedUEIndx);
//     schUEInfo.vrbs=vrbNumbers;
//     schUEInfo.rbStructurePerRB=rbStructurePerRB(selectedUEIndx);
//     
//     return(schUEInfo);
//   }
//   
// }



bool Scheduler::fCheckForFFTSizes(int arg1)
{
    ivec mValidSizes= ("1 2 3 4 5 6 8 9 10 12 15 16 18 20 24 25 27 30 32 36 40 45 48 50 54 60 64 72 75 80 81 90 96 100");

    if(find(mValidSizes,arg1)!=-1)
        return true;
    else
        return false;
}

void Scheduler::fUpdateAvgThroughput(ivec contentedUEs,int selectedUE, double possibleMCSRate){

    //! Updating PF Metric for the selected and all the contented UEs
    int selectedUEIndxInScheduler=find(mUEIds,selectedUE);

    mUEAvgRate(selectedUEIndxInScheduler)=(1-mPFTc)*mUEAvgRate(selectedUEIndxInScheduler)+mPFTc*possibleMCSRate;
    for(int user_cnt=0;user_cnt<(int)contentedUEs.size();user_cnt++)
        if(contentedUEs[user_cnt]!=selectedUE)
            mUEAvgRate(find(mUEIds,contentedUEs[user_cnt]))*=(1-mPFTc);
}


void Scheduler::fUpdateAvgThroughputV2(ivec contentedUEs ,ivec selectedUEs, vec possibleMCSRate){

    ivec othersUEs = remove(contentedUEs,selectedUEs);

    if (!mIsPF) return;

    //! Updating PF Metric for the selected and all the contented UEs
    for(int ue_cnt = 0; ue_cnt < selectedUEs.length(); ue_cnt++ )
    {
        int selectedUEIndxInScheduler=find(mUEIds,selectedUEs(ue_cnt));
        if (selectedUEIndxInScheduler!=-1 && possibleMCSRate(ue_cnt)>0) /// very unlikely to receive -1 though
        {
            if(mUEAvgRate(selectedUEIndxInScheduler)<=0.0001)
                mUEAvgRate(selectedUEIndxInScheduler)= possibleMCSRate(ue_cnt);
            else
                mUEAvgRate(selectedUEIndxInScheduler)=(1.0 -mPFTc)*mUEAvgRate(selectedUEIndxInScheduler) + mPFTc*possibleMCSRate(ue_cnt);
        }
    }



    for(int user_cnt=0;user_cnt<(int)othersUEs.size();user_cnt++)
        mUEAvgRate(find(mUEIds,othersUEs[user_cnt]))*=(1.0 - mPFTc);

}


int Scheduler::fFindDataAllocated(Array< ivec > arg1, int arg2)
{
    int retVal =0 ;
    for(int i=0;i<(int)arg1.size();i++)
    {
        if(arg2 == arg1(i)[0])
        {
            retVal =  retVal +  arg1(i)[1];
        }
    }
    return retVal;
}

int Scheduler::fFindDataAllocated(Array< ivec > arg1, int arg2, int arg3)
{
    int retVal =0 ;
    for(int i=0;i<(int)arg1.size();i++)
    {
        if((arg2 == arg1(i)[0]) && (arg3 == arg1(i)[2]))
        {
            retVal =  retVal +  arg1(i)[1];
        }
    }
    return retVal;
}

ivec Scheduler::fgetRBsScheduled(vector< vector< bool > > arg1, vector<int> arg2, int arg3)
{
    int indx = -1;
    for(int i=0;i<(int)arg2.size() ;i++)
    {
        if(arg2[i] == arg3)
        {
            indx = i;
        }
    }
    ivec retVal(0);
    for(int i=0;i<(int)arg1[indx].size();i++)
    {
        if(arg1[indx][i] == true)
        {
            append(retVal,i);
        }
    }
    return retVal;
}


int Scheduler::fFindNumOfRBsScheduled(vector< vector< bool > > arg1, vector<int> arg2, int arg3)
{
    int indx = -1;
    for(int i=0;i<(int)arg2.size() ;i++)
    {
        if(arg2[i] == arg3)
        {
            indx = i;
        }
    }
    int retVal = 0;
    for(int i=0;i<(int)arg1[indx].size();i++)
    {
        if(arg1[indx][i] == true)
        {
            retVal ++;
        }
    }
    return retVal;
}


int Scheduler::fFindIndex(Array<UEBufferInfo_S>& tVec, int tVal)
{
    for(int i = 0; i < (int)tVec.size(); i++)
    {
        if(tVec(i).mUEId == tVal)
        {
            return i;
        }
    }

    cout << "Error!!! Unable to find UE: " << tVal << endl;
    return -1;
}

int Scheduler::fGetBufferSize(Array<UEBufferInfo_S>& tVec, int tUEId)
{
    for(int i = 0; i < (int)tVec.size(); i++)
    {
        if(tVec(i).mUEId == tUEId)
        {
            return tVec(i).mDataInBufferInBytes;
        }
    }

    cout << "Error!!! Unable to find UE: " << tUEId << endl;
    return -1;
}

ivec Scheduler::fGetContendingUEList(Array<UEBufferInfo_S>& tUEBufferInfoList)
{
    ivec retValues(0);

    //   cout << "tUEBufferInfoList.ssssssssss: " << tUEBufferInfoList.size() << endl;
    for (int i = 0; i < (int)tUEBufferInfoList.size(); i++)
    {
        //     cout << "HUFF: " << tUEBufferInfoList(i).mUEId << " " << tUEBufferInfoList(i).mDataInBufferInBytes << endl;
        if (tUEBufferInfoList(i).mDataInBufferInBytes > 0)
        {
            append(retValues,tUEBufferInfoList(i).mUEId);
        }
    }

    return retValues;
}

ivec Scheduler::fGetContendingUEList(Array<UEBufferInfo_S>& tUEBufferInfoList, ivec &tUEs)
{
    ivec retValues(0);

    //cout << "tUEBufferInfoList.ssssssssss: " << tUEs.size() << endl;
    for (int i = 0; i < (int)tUEs.size(); i++)
    {
        for (int j = 0; j < (int)tUEBufferInfoList.size(); j++)
        {
            if (tUEs[i] == tUEBufferInfoList(j).mUEId)
            {
                if (tUEBufferInfoList(j).isInfiniteBuffer || tUEBufferInfoList(j).mDataInBufferInBytes > 0)
                {
                    append(retValues,tUEBufferInfoList(j).mUEId);
                }
            }
        }
    }

    return retValues;
}

/// rewrite this function for mu-mimo if need
// void Scheduler::fUpdateUEBufferInfo(Array<UEBufferInfo_S>& tVec, sScheduleReturn& tSch, ScheduledUEInfo_S tSchInfo)
// {
//   for(int i = 0; i < (int)tVec.size(); i++)
//   {
//     if(tVec(i).mUEId == tSchInfo.ueid(0))
//     {
//       int tAlloc = 0;
//       if (tSchInfo.tbSizePerCodeword(0).size() == 2)
//       {
// 	tAlloc = tSchInfo.tbSizePerCodeword(0)(0) + tSchInfo.tbSizePerCodeword(0)(1);
//       }
//       else
//       {
// 	tAlloc = tSchInfo.tbSizePerCodeword(0)(0);
//       }
//       
//       if (tVec(i).mDataInBufferInBytes <= tAlloc)
//       {
// 	tVec(i).mDataInBufferInBytes = 0;
//       }
//       //else
//       //{
//       //   tVec(i).mDataInBuffer -= tAlloc;
//       //}
//     }
//   }
//   
//   for (int i = 0; i < (int)tSch.UEIndex.size(); i++)
//   {
//     if (tSch.UEIndex[i] == tSchInfo.ueid(0))
//     {
//       tSch.tbSize_1[i] = tSchInfo.tbSizePerCodeword(0)(0);
//       
//       if (tSchInfo.tbSizePerCodeword(0).size() == 2)
//       {
// 	tSch.tbSize_2[i] = tSchInfo.tbSizePerCodeword(0)(1);
//       }
//       
//       tSch.nLayers[i] = tSchInfo.nLayers(0);
//       tSch.isRetx1[i] = sScheduleReturn::FRESH_TRANSMISSION;
//       tSch.isRetx2[i] = sScheduleReturn::FRESH_TRANSMISSION;
//       /// Other parameters like PMI, Transmission mode, MCS will be set later.
//     }
//   }
//   
//   return;
// }

void Scheduler::fUpdateUEBufferInfo(Array<UEBufferInfo_S>& ueBufferInfo, ScheduledUEInfo_S tSchInfo)
{
    for(int i = 0; i < (int)ueBufferInfo.size(); i++)
    {
        for(int j=0;j<tSchInfo.ueid.size();j++)
        {
            if(ueBufferInfo(i).mUEId == tSchInfo.ueid(j))
            {
                int tAllocInBytes = 0;
                if (tSchInfo.tbSizeInBytesPerCodeword(j).size() == 2)
                    tAllocInBytes = tSchInfo.tbSizeInBytesPerCodeword(j)(0) + tSchInfo.tbSizeInBytesPerCodeword(j)(1);
                else
                    tAllocInBytes = tSchInfo.tbSizeInBytesPerCodeword(j)(0);

                if(ueBufferInfo(i).tbSizeInBitsForReTx.length()==0) // new transmission
                {
                    if (ueBufferInfo(i).mDataInBufferInBytes <= tAllocInBytes)
                        ueBufferInfo(i).mDataInBufferInBytes = 0;
                }
                else // retransmission
                {
                    // exhausting the UE when allocation size crosses half the original TB size
                    if (sum(ueBufferInfo(i).tbSizeInBitsForReTx)/2 <= (tAllocInBytes*8)) 
                        ueBufferInfo(i).mDataInBufferInBytes = 0;
                }

            }
        }
    }
}
void Scheduler::fUpdateSchReturn(sScheduleReturn& tSch, ScheduledUEInfo_S tSchInfo)
{
    for (int i = 0; i < (int)tSch.UEIndex.size(); i++)
    {
        for(int j=0;j<tSchInfo.ueid.size();j++)
        {
            if (tSch.UEIndex[i] == tSchInfo.ueid(j))
            {
                tSch.tbSize_1[i] = tSchInfo.tbSizeInBytesPerCodeword(j)(0);

                if (tSchInfo.tbSizeInBytesPerCodeword(j).size() == 2)
                {
                    tSch.tbSize_2[i] = tSchInfo.tbSizeInBytesPerCodeword(j)(1);
                }
                tSch.nLayers[i] = tSchInfo.nLayers(j);
                tSch.isRetx1[i] = sScheduleReturn::FRESH_TRANSMISSION;
                tSch.isRetx2[i] = sScheduleReturn::FRESH_TRANSMISSION;
                /// Other parameters like PMI, Transmission mode, MCS will be set later.
            }
        }
    }
    return ;
}

CQIFeedback_S Scheduler::fGetCQIInfo(sCqiList &cqiList, ivec prbs)
{
    if(mIsDownlink)
        return(fGetDLCQIInfo(cqiList,prbs));
    else
        return(fGetULCQIInfo(cqiList,prbs));
}

CQIFeedback_S Scheduler::fGetDLCQIInfo(sCqiList &cqiList, ivec prbs)
{
    CQIFeedback_S cqiInfo;
    if(cqiList.ri!=NOT_DEFINED_8BIT)
    {
        cqiInfo.ri=(int)cqiList.ri;
    }
    else
        cqiInfo.ri=nDlLayers;

  // reciprocity 
    cqiInfo.reciproPrecoder.set_length(0);
  cqiInfo.cqi_reciprocity.set_length(0);
    if(cqiList.ulCqiReportList.size()!=0) // if UL CqiReport present
    {
        for(int rb_cnt=0;rb_cnt<prbs.length();rb_cnt++)
        {
            if(cqiList.ulCqiReportList[prbs(rb_cnt)].reciprocity_precoder.size() != 0)
            {
                append(cqiInfo.reciproPrecoder,stdVecOfVectToITPPMat(cqiList.ulCqiReportList[prbs(rb_cnt)].reciprocity_precoder));
            }
      if(cqiList.ulCqiReportList[prbs(rb_cnt)].reciprocity_DLCQI.length()!=0)
      {
        append(cqiInfo.cqi_reciprocity,cqiList.ulCqiReportList[prbs(rb_cnt)].reciprocity_DLCQI);
        
      }
        }
    }

    if(cqiList.tx_scheme == _MODE_NR_)
    {
        if(cqiList.cqiType == A30 || cqiList.cqiType == A31 || cqiList.cqiType == A32)
        {
            bool isWideband = mBWPartInfo.subbandIndxPerRB_higherLayerConfigured(0)==-1;  
            cqiInfo.cqi.set_length(prbs.length());
            if(cqiList.multiCqiList[0].ri != NOT_DEFINED_8BIT)
            {
                cqiInfo.ri=(int)cqiList.multiCqiList[0].ri;
            }
            else
                cqiInfo.ri=nDlLayers;
    
    //reciprocity
    if(cqiList.ulCqiReportList.size()!=0)
    {
    if(cqiList.ulCqiReportList[prbs(0)].reciprocity_DLRank!=0)
        cqiInfo.ri_reciprocity=cqiList.ulCqiReportList[prbs(0)].reciprocity_DLRank;
    }
      
            cqiInfo.pmi=-1*ones_i(prbs.length());
            for(int rb_cnt=0;rb_cnt<prbs.length();rb_cnt++) // coping sb cqi in scheduler
            {
                cqiInfo.cqi(rb_cnt).set_length(0);
                if(isWideband)
                {
                    append(cqiInfo.cqi(rb_cnt),(int)cqiList.multiCqiList[0].wb_cqi[0]);
                    if(cqiList.multiCqiList[0].wb_cqi[1]!=NOT_DEFINED_8BIT)
                    {
                        append(cqiInfo.cqi(rb_cnt),(int)cqiList.multiCqiList[0].wb_cqi[1]);
                    }
                }
                else if(cqiList.multiCqiList[0].higherLyrSelected.size())
                {
                    append(cqiInfo.cqi(rb_cnt),(int)cqiList.multiCqiList[0].higherLyrSelected[mBWPartInfo.subbandIndxPerRB_higherLayerConfigured(prbs(rb_cnt))].sb_cqi[0]);

                    if(cqiList.multiCqiList[0].higherLyrSelected[mBWPartInfo.subbandIndxPerRB_higherLayerConfigured(prbs(rb_cnt))].sb_cqi[1]!=NOT_DEFINED_8BIT)
                        append(cqiInfo.cqi(rb_cnt),(int)cqiList.multiCqiList[0].higherLyrSelected[mBWPartInfo.subbandIndxPerRB_higherLayerConfigured(prbs(rb_cnt))].sb_cqi[1]);
                }
            }
            if(cqiList.cqiType == A31)
            {
                cqiInfo.pmi=(int)cqiList.multiCqiList[0].wb_pmi*ones_i(prbs.length());
                cqiInfo.i1_1=(int)cqiList.multiCqiList[0].wb_i1_1;
                cqiInfo.i1_2=(int)cqiList.multiCqiList[0].wb_i1_2;
                cqiInfo.i1_3=(int)cqiList.multiCqiList[0].wb_i1_3;

            }
            if(cqiList.cqiType == A32)
            {
                if(cqiList.multiCqiList[0].nrCodeBookType == _eNR_CODEBOOK_TYPE_I_)
                {
                    cqiInfo.pmi=(int)cqiList.multiCqiList[0].wb_pmi;
                    cqiInfo.i1_1=(int)cqiList.multiCqiList[0].wb_i1_1;
                    cqiInfo.i1_2=(int)cqiList.multiCqiList[0].wb_i1_2;
                    cqiInfo.i1_3=(int)cqiList.multiCqiList[0].wb_i1_3;

                    type1PMI_S i1i2Index;
                    i1i2Index.i11 = cqiInfo.i1_1;
                    i1i2Index.i12 = cqiInfo.i1_2;
                    i1i2Index.i13 = cqiInfo.i1_3;

                    cmat wbPrecoder;int wb_pmi = (int)cqiList.multiCqiList[0].wb_pmi;
                    if(isWideband)
                    {
                        i1i2Index = mCodebook->getPMIIndices(cqiInfo.ri,wb_pmi);
                        wbPrecoder = mCodebook->getPrecoderNR(cqiInfo.ri,i1i2Index);
                    }

                    cqiInfo.i2.set_size(prbs.length());
                    cqiInfo.pmi.set_size(prbs.length());
                    cqiInfo.sbPrecoder.set_length(prbs.length());
                    for(int rb_cnt=0;rb_cnt<prbs.length();rb_cnt++) // coping sb i2 in scheduler
                    {
                        if(isWideband)
                        {
                            cqiInfo.i2(rb_cnt) = i1i2Index.i2;
                            cqiInfo.sbPrecoder(rb_cnt) = wbPrecoder;
                        }
                        else
                        {
                            cqiInfo.i2(rb_cnt) = (int8_t)cqiList.multiCqiList[0].higherLyrSelected[mBWPartInfo.subbandIndxPerRB_higherLayerConfigured(prbs(rb_cnt))].sb_pmi;
                            i1i2Index.i2 = cqiInfo.i2(rb_cnt);
                            cqiInfo.pmi(rb_cnt) = mCodebook->getPrecoderIndxAbsNR(cqiInfo.ri,i1i2Index);
                            cqiInfo.sbPrecoder(rb_cnt) = mCodebook->getPrecoderNR(cqiInfo.ri,i1i2Index);
                        }
                    }
                }
                else if(cqiList.multiCqiList[0].nrCodeBookType == _eNR_CODEBOOK_TYPE_I_MULTIPANEL_)
                {
                    cqiInfo.pmi=(int)cqiList.multiCqiList[0].wb_pmi;
                    cqiInfo.i1_1=(int)cqiList.multiCqiList[0].wb_i1_1;
                    cqiInfo.i1_2=(int)cqiList.multiCqiList[0].wb_i1_2;
                    cqiInfo.i1_3=(int)cqiList.multiCqiList[0].wb_i1_3;
                    cqiInfo.i1_4=(int)cqiList.multiCqiList[0].wb_i1_4;

                    type1PMI_S i1i2Index;
                    i1i2Index.i11 = cqiInfo.i1_1;
                    i1i2Index.i12 = cqiInfo.i1_2;
                    i1i2Index.i13 = cqiInfo.i1_3;
                    i1i2Index.i14 = cqiInfo.i1_4;

                    cmat wbPrecoder;int wb_pmi = (int)cqiList.multiCqiList[0].wb_pmi;
                    if(isWideband)
                    {
                        i1i2Index = mCodebook->getPMIIndices(cqiInfo.ri,wb_pmi);
                        wbPrecoder = mCodebook->getPrecoderNR(cqiInfo.ri,i1i2Index);
                    }

                    cqiInfo.i2.set_size(prbs.length());
                    cqiInfo.pmi.set_size(prbs.length());
                    cqiInfo.sbPrecoder.set_length(prbs.length());
                    for(int rb_cnt=0;rb_cnt<prbs.length();rb_cnt++) // coping sb i2 in scheduler
                    {
                        if(isWideband)
                        {
                            cqiInfo.i2(rb_cnt) = i1i2Index.i2;
                            cqiInfo.sbPrecoder(rb_cnt) = wbPrecoder;
                        }
                        else
                        {
                            cqiInfo.i2(rb_cnt) = (int8_t)cqiList.multiCqiList[0].higherLyrSelected[mBWPartInfo.subbandIndxPerRB_higherLayerConfigured(prbs(rb_cnt))].sb_pmi;
                            i1i2Index.i2 = cqiInfo.i2(rb_cnt);
                            cqiInfo.pmi(rb_cnt) = mCodebook->getPrecoderIndxAbsNR(cqiInfo.ri,i1i2Index);
                            cqiInfo.sbPrecoder(rb_cnt) = mCodebook->getPrecoderNR(cqiInfo.ri,i1i2Index);
                        }
                    }
                }
                else if(cqiList.multiCqiList[0].nrCodeBookType == _eNR_CODEBOOK_TYPE_II_)
                {
                    cqiInfo.isTypeIIPrecoderEnabled=true;
                    cqiInfo.sbPrecoder.set_size(prbs.length());
                    for(int rb_cnt=0;rb_cnt<prbs.length();rb_cnt++) // coping sb i2 in scheduler
                    {
                        cqiInfo.sbPrecoder(rb_cnt) = cqiList.multiCqiList[0].higherLyrSelected[mBWPartInfo.subbandIndxPerRB_higherLayerConfigured(prbs(rb_cnt))].sb_precoder;
                    }
                }
                if(!isWideband){
                    ivec subbandsToCollect(0);
                    for(int rb_cnt=0;rb_cnt<prbs.length();rb_cnt++) 
                        append(subbandsToCollect,mBWPartInfo.subbandIndxPerRB_higherLayerConfigured(prbs(rb_cnt)));


                    subbandsToCollect=findUniqueNumbers(subbandsToCollect);
                    cqiInfo.sbChannel.set_size(0);
                    for(int sb_cnt=0; sb_cnt<subbandsToCollect.length(); sb_cnt++) 
                        append( cqiInfo.sbChannel,cqiList.multiCqiList[0].higherLyrSelected[subbandsToCollect(sb_cnt)].sb_channel);

                }
            }

        }
    }
    else // LTE Tx modes
    {
        if(cqiList.cqiType==P10 || cqiList.cqiType==A12)
        {
            cqiInfo.pmi=-1;
            cqiInfo.cqi.set_length(prbs.length());
            for(int rb_cnt=0;rb_cnt<prbs.length();rb_cnt++)
            {
                cqiInfo.cqi(rb_cnt).set_size(0);
                append(cqiInfo.cqi(rb_cnt),(int)cqiList.wb_cqi[0]);
                if(cqiList.wb_cqi[1]!=NOT_DEFINED_8BIT)
                    append(cqiInfo.cqi(rb_cnt),(int)cqiList.wb_cqi[1]);
            }
            if(cqiList.cqiType==A12)
                cqiInfo.pmi=cqiList.higherLyrSelected[mBWPartInfo.subbandIndxPerRB_higherLayerConfigured(prbs(0))].sb_pmi;
        }
        else if(cqiList.cqiType==P11)
        {
            cqiInfo.i1=(int)cqiList.wb_i1;
            cqiInfo.pmi=(int)cqiList.wb_pmi;
            cqiInfo.cqi.set_length(prbs.length());
            for(int rb_cnt=0;rb_cnt<prbs.length();rb_cnt++)
            {
                cqiInfo.cqi(rb_cnt).set_length(0);
                append(cqiInfo.cqi(rb_cnt),(int)cqiList.wb_cqi[0]);
                if(cqiList.wb_cqi[1]!=NOT_DEFINED_8BIT)
                    append(cqiInfo.cqi(rb_cnt),(int)cqiList.wb_cqi[1]);
            }
        }
        else if(cqiList.cqiType==P20 || cqiList.cqiType==P21 )
        {
            cqiInfo.cqi.set_length(prbs.length());
            for(int rb_cnt=0;rb_cnt<prbs.length();rb_cnt++)
            {
                cqiInfo.cqi(rb_cnt).set_length(0);

                if(cqiList.bwPart.length()>mBWPartInfo.bwPartPerRB(prbs(rb_cnt)))
                {
                    //! If rank changed in TM3 and TM4 in subband modes, Subband CQI cannot be used since wideband cqi and subband cqi will have different length
                    bool isSubbandValid=false;
                    if((cqiList.bwPart(mBWPartInfo.bwPartPerRB(prbs(rb_cnt))).cqi[1] == NOT_DEFINED_8BIT && cqiList.wb_cqi[1] ==NOT_DEFINED_8BIT) ||(cqiList.bwPart(mBWPartInfo.bwPartPerRB(prbs(rb_cnt))).cqi[1] != NOT_DEFINED_8BIT && cqiList.wb_cqi[1] != NOT_DEFINED_8BIT))
                    {
                        isSubbandValid=true;
                    }
                    if(cqiList.bwPart(mBWPartInfo.bwPartPerRB(prbs(rb_cnt))).sb==mBWPartInfo.subbandIndxPerRB(prbs(rb_cnt)) && isSubbandValid) //!If subbandIndx of the rb matches the reported subband in the BW part
                    {
                        cqiInfo.cqi(rb_cnt).set_size(0);
                        append(cqiInfo.cqi(rb_cnt),(int)cqiList.bwPart(mBWPartInfo.bwPartPerRB(prbs(rb_cnt))).cqi[0]);
                        if(cqiList.bwPart(mBWPartInfo.bwPartPerRB(prbs(rb_cnt))).cqi[1]!=NOT_DEFINED_8BIT)
                            append(cqiInfo.cqi(rb_cnt),(int)cqiList.bwPart(mBWPartInfo.bwPartPerRB(prbs(rb_cnt))).cqi[1]);
                    }
                    else //! Takw wb_cqi for the rb if the reported subbandIndx is different
                    {
                        append(cqiInfo.cqi(rb_cnt),(int)cqiList.wb_cqi[0]);
                        if(cqiList.wb_cqi[1]!=NOT_DEFINED_8BIT)
                            append(cqiInfo.cqi(rb_cnt),(int)cqiList.wb_cqi[1]);
                    }
                }
                else //! Takw wb_cqi for the rb if the bwpart repost is not available
                {
                    append(cqiInfo.cqi(rb_cnt),(int)cqiList.wb_cqi[0]);
                    if(cqiList.wb_cqi[1]!=NOT_DEFINED_8BIT)
                        append(cqiInfo.cqi(rb_cnt),(int)cqiList.wb_cqi[1]);
                }
            }

            if(cqiList.cqiType==P21)
            {
                cqiInfo.i1=(int)cqiList.wb_i1;
                cqiInfo.pmi=(int)cqiList.wb_pmi;
            }
        }
        //     else if(cqiList.cqiType==A12)
        //     {
        //        cqiInfo.pmi=cqiList.higherLyrSelected[mBWPartInfo.bwPartPerRB(prbs(0))].sb_pmi;
        //     }
        else if(cqiList.cqiType==A20 || cqiList.cqiType==A22)
        {
            cqiInfo.cqi.set_length(prbs.length());
            cqiInfo.pmi=-1;

            for(int rb_cnt=0;rb_cnt<prbs.length();rb_cnt++)
            {
                bool UESelected_subband_found=false;
                cqiInfo.cqi(rb_cnt).set_length(0);
                for(int i=0; i<MAX_UE_SELECTED_SB; i++)
                {
                    if((int)(cqiList.ueSelected.sbList[i])==mBWPartInfo.subbandIndxPerRB(prbs(rb_cnt)))
                    {
                        UESelected_subband_found=true;
                        break;
                    }    
                }

                if(UESelected_subband_found)
                {
                    append(cqiInfo.cqi(rb_cnt),(int)cqiList.ueSelected.sb_cqi[0]);
                    if(cqiList.ueSelected.sb_cqi[1]!=NOT_DEFINED_8BIT)
                        append(cqiInfo.cqi(rb_cnt),(int)cqiList.ueSelected.sb_cqi[1]);
                    if(cqiList.cqiType == A22)
                    {
                        cqiInfo.pmi=(int)cqiList.ueSelected.sb_pmi;
                    }
                }
                else
                {
                    append(cqiInfo.cqi(rb_cnt),(int)cqiList.wb_cqi[0]);
                    if(cqiList.wb_cqi[1]!=NOT_DEFINED_8BIT)
                        append(cqiInfo.cqi(rb_cnt),(int)cqiList.wb_cqi[1]);
                    if(cqiList.cqiType == A22)
                    {
                        cqiInfo.pmi=(int)cqiList.wb_pmi;
                    }
                }
            }
        }
        else if(cqiList.cqiType == A30 || cqiList.cqiType == A31 || cqiList.cqiType == A32)
        {
            cqiInfo.cqi.set_length(prbs.length());
            cqiInfo.pmi=-1;
            for(int rb_cnt=0;rb_cnt<prbs.length();rb_cnt++)
            {
                cqiInfo.cqi(rb_cnt).set_length(0);
                append(cqiInfo.cqi(rb_cnt),(int)cqiList.higherLyrSelected[mBWPartInfo.subbandIndxPerRB_higherLayerConfigured(prbs(rb_cnt))].sb_cqi[0]);
                if(cqiList.higherLyrSelected[mBWPartInfo.subbandIndxPerRB_higherLayerConfigured(prbs(rb_cnt))].sb_cqi[1]!=NOT_DEFINED_8BIT)
                    append(cqiInfo.cqi(rb_cnt),(int)cqiList.higherLyrSelected[mBWPartInfo.subbandIndxPerRB_higherLayerConfigured(prbs(rb_cnt))].sb_cqi[1]);

            }
            if(cqiList.cqiType == A31)
                cqiInfo.pmi=(int)cqiList.wb_pmi;
            if(cqiList.cqiType == A32)
                cqiInfo.pmi=(int)cqiList.higherLyrSelected[mBWPartInfo.subbandIndxPerRB_higherLayerConfigured(prbs(0))].sb_pmi;
        }
    }
    return(cqiInfo);
}


CQIFeedback_S Scheduler::fGetULCQIInfo(sCqiList &cqiList, ivec prbs)
{
    CQIFeedback_S cqiInfo;
    cqiInfo.cqi.set_length(prbs.length());
    cqiInfo.effSINRs.set_length(prbs.length());
    cqiInfo.pmi.set_length(prbs.length());
    cqiInfo.reciproPrecoder.set_length(0);

    if(cqiList.ulCqiReportList.size()!=0)
    {
        for(int rb_cnt=0;rb_cnt<prbs.length();rb_cnt++)
        {
            //        cqiInfo.cqi(rb_cnt).set_length(0);
            //        for(int cwd_cnt=0;cwd_cnt<cqiList.ulCqiReportList[prbs(rb_cnt)].sb_cqi.size();cwd_cnt++)
            // 	  append(cqiInfo.cqi(rb_cnt),(int)cqiList.ulCqiReportList[prbs(rb_cnt)].sb_cqi[cwd_cnt]);
            cqiInfo.effSINRs(rb_cnt).set_length(0);
            //if(cqiList.ulCqiReportList[prbs(rb_cnt)].sb!=-1)
            //       if(cqiList.cqiType != CQIType_UNINIT)
            {
                for(int cwd_cnt=0;cwd_cnt<(int)cqiList.ulCqiReportList[prbs(rb_cnt)].sb_SINR.size();cwd_cnt++)
                    append(cqiInfo.effSINRs(rb_cnt),(double)cqiList.ulCqiReportList[prbs(rb_cnt)].sb_SINR[cwd_cnt]);

                if(cqiList.ulCqiReportList[prbs(rb_cnt)].reciprocity_precoder.size() != 0)
                {
                    cout<<"Reciprocity precoder is available in fGetULCQIInfo"<<endl;

                    append(cqiInfo.reciproPrecoder, stdVecOfVectToITPPMat(cqiList.ulCqiReportList[prbs(rb_cnt)].reciprocity_precoder));
                    // 	  cout<<"cqiInfo.reciproPrecoder(rb_cnt):"<<cqiInfo.reciproPrecoder(rb_cnt)<<endl;
                    // 	  cout<<"cqiList.ulCqiReportList[prbs(rb_cnt)].sb_precoder:"<<cqiList.ulCqiReportList[prbs(rb_cnt)].sb_precoder[0][0]<<endl;
                }

                if(cqiList.ulCqiReportList[prbs(rb_cnt)].sb_pmi!=NOT_DEFINED_8BIT)
                { 
                    cqiInfo.pmi(rb_cnt) = cqiList.ulCqiReportList[prbs(rb_cnt)].sb_pmi;
                }
                else
                    cqiInfo.pmi(rb_cnt) = -1;

                if(cqiList.ri!=NOT_DEFINED_8BIT)
                    cqiInfo.ri=(int)cqiList.ri;
                else
                    cqiInfo.ri=nUlLayers;
                cqiInfo.sbChannel.set_length(0);
                for(int rb_cnt=0;rb_cnt<prbs.length();rb_cnt++) 
                {
                    append(cqiInfo.sbChannel, cqiList.ulCqiReportList[prbs(rb_cnt)].sb_channel);
                }

            }
            //       else
            //       {
            // 	cqiInfo.pmi=-1;   
            // 	cqiInfo.effSINRs(rb_cnt)=(-5)*ones(nCodeWords);
            //       }
        }
    }
    else
    {
        cqiInfo.pmi=-1*ones_i(prbs.length());
        cqiInfo.cqi.set_length(prbs.length());

        for(int rb_cnt=0;rb_cnt<prbs.length();rb_cnt++)
            cqiInfo.cqi(rb_cnt)=ones_i(1);

        cqiInfo.effSINRs.set_length(prbs.length());

        for(int rb_cnt=0;rb_cnt<prbs.length();rb_cnt++)
            cqiInfo.effSINRs(rb_cnt)=(-5)*ones(1);

        cqiInfo.ri=nUlLayers;
    }

    return(cqiInfo);
}


TransmissionMode_E Scheduler::fGetTMMode (sCqiList& cqiList)
{
    return (TransmissionMode_E)cqiList.tx_scheme;
}


//int Scheduler::fUpdateSchInfo(sScheduleReturn &schReturn,Array< vector <int> > &schInfo, int cwdIndx,sUEswithData &tUEData,int ueid,ivec vrbs,TransmissionMode_E txMode, int mcs,int nLayers,int pmi,int dataSendableInBytes,int tIndx){
int Scheduler::fUpdateSchInfo(sScheduleReturn& schReturn, Array< ivec >& schInfo, int cwdIndx, sUEswithData& tUEData, int ueid, ivec vrbs, TransmissionMode_E txMode, int mcs, int nLayers, int pmi, int dataSendableInBytes, int tIndx, bool paired){

    ivec tempVec(0); 
    ivec temp(0);
    //!Note : dataSendableInBytes can go negative (i.e TBSize and MCS could get reduced) to adjust for CQI rate with new allocations
    if(cwdIndx==2)
    {
        append(temp,ueid);
        append(temp,-1);
        append(temp,-2);
        append(schInfo,temp);
        return 1;
    }
    if(dataSendableInBytes > 0)
    {
        if(dataSendableInBytes <= tUEData.mDataInBuffer[tIndx])
        {
            if(dataSendableInBytes <= 4) /// Since Minimum 2 bytes of MAC Subheader and 2 bytes of RLC Header is added to for MAC PDU
            {
                ivec temp;
                if(schInfo.size() && (schInfo(schInfo.size() - 1)[0] == ueid))
                {
                    temp = schInfo(schInfo.size() - 1);
                    temp[1] += dataSendableInBytes;
                    dataSendableInBytes = 0;
                    deleteInArray(schInfo,ivec(schInfo.size() - 1));
                }
                else
                {
                    append(temp,ueid);
                    append(temp,dataSendableInBytes);
                    dataSendableInBytes = 0;
                    append(temp,tUEData.mRBId[tIndx]);
                }

                append(schInfo,temp);
                schReturn.fUpdateResourceMapping(ueid, vrbs);
                int ueIndx=fFindInVec(schReturn.UEIndex, ueid);
                schReturn.transmissionMode[ueIndx]=(int)txMode;
                schReturn.nLayers[ueIndx]=nLayers;
                schReturn.pmi[ueIndx]=pmi;
                schReturn.isPaired[ueIndx]=paired;
                if(cwdIndx==0)
                    schReturn.mcs_1[ueIndx] = mcs;
                else
                    schReturn.mcs_2[ueIndx] = mcs;
            }
            else ///if(DataSent > 4)
            {
                append(tempVec,ueid);
                tUEData.mDataInBuffer[tIndx] -= dataSendableInBytes;
                append(tempVec,dataSendableInBytes);
                append(tempVec,tUEData.mRBId[tIndx]);
                dataSendableInBytes = 0;
            }
        }

        else
        {
            if(tUEData.mDataInBuffer[tIndx] <= 3)
            {
                /// Since Minimum 1 byte of MAC Subheader and 2 bytes of RLC Header is addeed to for MAC PDU
                tUEData.mDataInBuffer[tIndx] += 3;
            }
            append(tempVec,ueid);

            append(tempVec,dataSendableInBytes);
            append(tempVec,tUEData.mRBId[tIndx]);
            dataSendableInBytes  -= tUEData.mDataInBuffer[tIndx];
            tUEData.mDataInBuffer[tIndx] = 0;
        }
    }
    else
    {
        append(tempVec,ueid);
        tUEData.mDataInBuffer[tIndx] -= dataSendableInBytes; //Adding back the excess bits into the buffer(dataSendableInBytes is -ve here)
        append(tempVec,dataSendableInBytes);
        append(tempVec,tUEData.mRBId[tIndx]);
        dataSendableInBytes = 0;
    }
    if(tempVec.size() > 0)
    {
        append(schInfo,tempVec);
        schReturn.fUpdateResourceMapping(tempVec[0], vrbs);
        int ueIndx=fFindInVec(schReturn.UEIndex, ueid);
        schReturn.transmissionMode[ueIndx]=(int)txMode;
        schReturn.nLayers[ueIndx]=nLayers;
        schReturn.pmi[ueIndx]=pmi;
        schReturn.isPaired[ueIndx]=paired;
        if(cwdIndx==0)
            schReturn.mcs_1[ueIndx] = mcs;
        else
            schReturn.mcs_2[ueIndx] = mcs;
        tempVec.set_size(0);
    }
    return dataSendableInBytes;
}

int Scheduler::fFindInVec(vector< int > arg1, int arg2)
{
    int indx = -1;
    for(int i=0;i<(int)arg1.size();i++)
    {
        if(arg1[i] == arg2)
        {
            indx = i;
        }
    }
    return indx;
}
///Add UE to master list
void Scheduler::fAddUE(int tUEId){

    append(mUEIds, tUEId);
    //cout<<"mUE "<<mUEIds<<endl;

    if(mIsPF)
        append(mUEAvgRate, mTBTables->cqiToMaxMCSRate.getLeastMCSRate());
}

/// Remove UE from master list
void Scheduler::fRemoveUE(int tUEId)
{
    ivec tUEIds;
    vec tUEAvgRate;

    for(int i=0; i<mUEIds.size(); i++)
    {
        if(mUEIds[i] != tUEId) 
        {
            append(tUEIds, mUEIds[i]);
            if(mIsPF) append(tUEAvgRate, mUEAvgRate[i]);
        }
    }

    //mUEIds.set_size(0);
    mUEIds = tUEIds;

    if(mIsPF)
    {
        //mUEAvgRate.set_size(0);
        mUEAvgRate = tUEAvgRate;
    }

}

int Scheduler::fGetUEAntennaCount(int tUEId)
{
    ivec tempUEIds = fGetUEIds();
    int tAntennaCount = -1;

    //int ueIndx=findInIvec(tempUEIds, tUEId);
    int ueIndx= -1;
    for(int iUE=0; iUE<tempUEIds.size(); iUE++)
    {
        if (tempUEIds[iUE]== tUEId)
        {
            ueIndx = iUE;
            break;
        }
    }

    if(ueIndx != -1) 
    {
        tAntennaCount = ueAntennaCount[ueIndx];
    }
    else
        cout<<"Invalid UE Id"<<endl;

    return tAntennaCount;
}

double Scheduler::fGetUESCpowerIndBm(int tUEId)
{
    ivec tempUEIds = fGetUEIds();
    double scPower = -1;

    //int ueIndx=findInIvec(tempUEIds, tUEId);
    int ueIndx= -1;
    for(int iUE=0; iUE<tempUEIds.size(); iUE++)
    {
        if (tempUEIds[iUE]== tUEId)
        {
            ueIndx = iUE;
            break;
        }
    }

    if(ueIndx != -1) 
    {
        scPower = UETxSCPowerIndBm[ueIndx];
    }
    else
        cout<<"Invalid UE Id"<<endl;

    return scPower;
}


LinkAdaptation_S Scheduler::fDoLinkAdaptation(int UEid, int nLayers, TransmissionMode_E txMode,ivec rbs, ivec symbolsToSchedule, Array< imat >& possibleRBStructures, Array<vec> possibleEffSINRsInDB, OuterLoopLinkAdaptation_S& tOuterLoopLinkAdaptation, bool isCQIReported)
{
    sort(rbs);  
    int nNumOfRbs=rbs.length();  
    if(symbolsToSchedule.length()==0) symbolsToSchedule=getIntegers(0,13);
    LinkAdaptation_S tLinkAdaptation;
    ivec REsPerRB(possibleRBStructures.length());
    for(int rb_cnt=0;rb_cnt<possibleRBStructures.length();rb_cnt++)
        REsPerRB(rb_cnt)=count(possibleRBStructures(rb_cnt),0);

    Array<vec> possibleEffSINRsOverCodeword= swapDimensions(possibleEffSINRsInDB);
    //cout<<"possibleEffSINRsOverCodeword : "<<possibleEffSINRsOverCodeword<<endl;
    int tUEAntennaCount = fGetUEAntennaCount(UEid);
    double tUESCPowerIndBm = fGetUESCpowerIndBm(UEid);

    int nCodewords=possibleEffSINRsOverCodeword.length();

    vec sinrOffsetIndB=zeros(nCodewords);

    vec targetBLER(1);
    targetBLER(0)=0.1;
    vec rateThresholds(0);
    if(tOuterLoopLinkAdaptation.isEnabled)
    {
        sinrOffsetIndB = tOuterLoopLinkAdaptation.getSINROffsetPerCWIndB(UEid).left(nCodewords);
        targetBLER = tOuterLoopLinkAdaptation.TargetBLER;
        rateThresholds= tOuterLoopLinkAdaptation.rateThresholds;
    }

    int mimoRate = nLayers;
    if(txMode == _TRANSMISSION_MODE_2_)
        mimoRate = 1;

    int nSymbols=symbolsToSchedule.length();

    if(!mIsDownlink && isCQIReported)
    {
        PoAlphaSet_S po(powerControlInfo.targetPowerForLTEPowerControlindBm,powerControlInfo.pathlossCompensation);
        po.pcfValueForPowerControl=powerControlInfo.fValueForPowerControl;	
        double maxSCPowInDBm=tUESCPowerIndBm;
        double srsTransmitPowerInDB=(23-30)-dB(300);
        if(powerControlInfo.enablePowerBackoffModel)
        {
            PoAlphaSet_S poTemp=po;
            sBeamPairLink myBeamPair=beamPair(find(mUEIds,UEid));
        getUplinkPower(poTemp,nNumOfRbs,maxSCPowInDBm,myBeamPair.rspIndBm,myBeamPair.rsrpIndBm,TPCcmdFieldperUE(find(mUEIds,UEid)));

            Array<vec> sinrTemp(nCodewords);
            for(int Cwd_cnt=0;Cwd_cnt<nCodewords;Cwd_cnt++)
                sinrTemp(Cwd_cnt)=possibleEffSINRsOverCodeword(Cwd_cnt)+(dB(poTemp.pcReqParaByMeasure.finalTxULPowerPerSubCarrierInWatts)-srsTransmitPowerInDB);

            LinkAdaptation_S temp = mTBTables->getMCSPerCodeWord(sinrTemp,nNumOfRbs,nSymbols,(int)sum(REsPerRB),mimoRate,mIsDownlink,sinrOffsetIndB,rateThresholds,targetBLER);

            // Powerbackoff Model
            powerBackoff_S powerBackoff;
            ModulationScheme_E mosc;
            mcsidxMapping(mTBTables->awgnTablesConvCoder.is256QAMenabled, mTBTables->awgnTablesConvCoder.isPi2BPSKenabled,temp.mcsIndxPerCodeword(0),mosc,mIsDownlink);
            if(mosc==1)
            {
                maxSCPowInDBm+=3;
            }
            else
            {
                double maxPowerReduction=powerBackoff.getMPR(mosc,mWaveform,rbs,mFrameStructure->getRuCountPerLevel());
                maxSCPowInDBm-=maxPowerReduction;
            }
        }

        sBeamPairLink myBeamPair=beamPair(find(mUEIds,UEid));
    getUplinkPower(po,nNumOfRbs,maxSCPowInDBm,myBeamPair.rspIndBm,myBeamPair.rsrpIndBm,TPCcmdFieldperUE(find(mUEIds,UEid)));

        for(int Cwd_cnt=0;Cwd_cnt<nCodewords;Cwd_cnt++)
            possibleEffSINRsOverCodeword(Cwd_cnt)+=(dB(po.pcReqParaByMeasure.finalTxULPowerPerSubCarrierInWatts)-srsTransmitPowerInDB);
    }
    tLinkAdaptation = mTBTables->getMCSPerCodeWord(possibleEffSINRsOverCodeword,nNumOfRbs,nSymbols,(int)sum(REsPerRB),mimoRate,mIsDownlink,sinrOffsetIndB,rateThresholds,targetBLER);

    tLinkAdaptation.UEid = UEid;

    return tLinkAdaptation;

}

UEInfo_S Scheduler::fCollectUEInfo(sCqiList cqiList, TransmissionMode_E txMode, ivec vrbNumbers)
{
    UEInfo_S tUEInfo;

    tUEInfo.UEid = cqiList.rnti;
    tUEInfo.txMode = txMode;
    tUEInfo.cqiType = cqiList.cqiType;
    tUEInfo.vrbNumbers = vrbNumbers;

    if(tUEInfo.txMode == _TRANSMISSION_MODE_NR_)
    {
        tUEInfo.cqiType = cqiList.cqiType;
        if((mIsDownlink && cqiList.cqiType!=CQIType_UNINIT) || (!mIsDownlink && cqiList.ulCqiReportList.size()!=0))
        {
            tUEInfo.isCQIReported=true;
            CQIFeedback_S cqiInfo=fGetCQIInfo(cqiList,vrbNumbers);

            /*vec sinrPerCQI = -100*ones(16);
              for(int k = 1; k<=15; k++)
              sinrPerCQI(k) = mTBTables->awgnTablesConvCoder.getSINRIndBfromCQI(k);
              cout<<"sinrPerCQI = "<<sinrPerCQI<<endl;abort();*/

            if(mIsDownlink)
                tUEInfo.sinrInDB=mTBTables->awgnTablesConvCoder.getSINRIndBfromCQI(cqiInfo.cqi);
            else
                tUEInfo.sinrInDB=cqiInfo.effSINRs;

            if(mIsMUMIMOEnabled && !cqiInfo.sbChannel.length())
            {
                cout<<"MU-MIMO doesnot support Wideband Transmission..."<<endl;abort();
            }

            tUEInfo.sbChannel=cqiInfo.sbChannel;

            tUEInfo.isPrecoderAvailable=true;
            tUEInfo.precoder = cqiInfo.sbPrecoder;

            tUEInfo.i1_1=cqiInfo.i1_1;
            tUEInfo.i1_2=cqiInfo.i1_2;
            tUEInfo.i1_3=cqiInfo.i1_3;
            tUEInfo.i1_4 = cqiInfo.i1_4;

            tUEInfo.pmi=cqiInfo.pmi; // Wb pmi actual index
            tUEInfo.i2=cqiInfo.i2; // sb pmi for passed subband


            // Extracted reciprocity precoder per RBG if present
            if(cqiInfo.reciproPrecoder.length() != 0)
            {
                // 	cmat tempPrecoder = zeros_c(cqiInfo.reciproPrecoder(0).rows(),cqiInfo.reciproPrecoder(0).cols());
                // 	for(int i=0;i<vrbNumbers.length();i++)
                // 	  tempPrecoder += cqiInfo.reciproPrecoder(i);    
                // 	tUEInfo.precoder=tempPrecoder/vrbNumbers.length();
	tUEInfo.reciproPrecoder=cqiInfo.reciproPrecoder;
            }  
    
            if(cqiInfo.ri!=-1)
            {
                tUEInfo.nLayers=cqiInfo.ri;
            }
            else
            {
                if(mIsDownlink)
                    tUEInfo.nLayers = nDlLayers;
                else
                    tUEInfo.nLayers = nUlLayers;
            }

      //reciprocity
      if(mIsDownlink && useReciproPrecode)
      {
        if(cqiInfo.ri_reciprocity!=-1 && cqiInfo.cqi_reciprocity.length()!=0)
        {

            tUEInfo.nLayers=cqiInfo.ri_reciprocity;
            tUEInfo.sinrInDB=mTBTables->awgnTablesConvCoder.getSINRIndBfromCQI(cqiInfo.cqi_reciprocity);
            //cout<<"DL reciprocity CQI updated"<<endl;
        } 
	
      }
            
      
      
        }
        else//! CQI not yet reported
        {
            tUEInfo.isCQIReported=false;

            tUEInfo.i1_1=0;
            tUEInfo.i1_2=0;
            tUEInfo.i1_3=0;
            tUEInfo.i1_4=0;
            tUEInfo.pmi=zeros_i(vrbNumbers.length()); // Wb pmi actual index
            tUEInfo.i2=zeros_i(vrbNumbers.length()); // sb pmi for passed subband
            tUEInfo.isPrecoderAvailable=false;
            tUEInfo.precoder.set_length(0);
            if(mIsDownlink)
                tUEInfo.nLayers = nDlLayers;
            else
                tUEInfo.nLayers = nUlLayers;

            tUEInfo.sinrInDB.set_length(vrbNumbers.length());

            int nCodewords = (tUEInfo.nLayers > 4) ? 2 : 1;

            for(int rb_cnt=0;rb_cnt<vrbNumbers.length();rb_cnt++)
                if(mIsDownlink)    
                    tUEInfo.sinrInDB(rb_cnt)=mTBTables->awgnTablesConvCoder.getSINRIndBfromCQI(defaultCQI)*ones(nCodewords);
                else
                    tUEInfo.sinrInDB(rb_cnt)=5*ones(nCodewords); // default SNR for uplink


        }
    }
    else // For LTE
    {
        if(!mIsDownlink)
        {
            cout<<"Uplink Not supported.. "<<endl;abort();
        }
        if(tUEInfo.cqiType!=CQIType_UNINIT)
        {
            tUEInfo.isCQIReported=true;
            CQIFeedback_S cqiInfo=fGetCQIInfo(cqiList,vrbNumbers);
            if(mIsModeAdaptationEnabled)
                tUEInfo.txMode=fGetTMMode(cqiList);
            if(mIsDownlink)
                tUEInfo.sinrInDB=mTBTables->awgnTablesConvCoder.getSINRIndBfromCQI(cqiInfo.cqi);
            else
                tUEInfo.sinrInDB=cqiInfo.effSINRs;
            tUEInfo.i1=cqiInfo.i1;
            tUEInfo.pmi=cqiInfo.pmi;

            // Extracted reciprocity precoder per RBG if present
            if(cqiInfo.reciproPrecoder.length() != 0)
            {
                cmat tempPrecoder = zeros_c(cqiInfo.reciproPrecoder(0).rows(),cqiInfo.reciproPrecoder(0).cols());
                for(int i=0;i<vrbNumbers.length();i++)
                    tempPrecoder += cqiInfo.reciproPrecoder(i);    
                tUEInfo.precoder=tempPrecoder/vrbNumbers.length();
            }


            if(cqiInfo.ri!=-1)
            {
                if(tUEInfo.txMode==_TRANSMISSION_MODE_3_ && cqiInfo.ri==1)
                {
                    tUEInfo.nLayers=nCRSPorts;
                    tUEInfo.txMode=_TRANSMISSION_MODE_2_;
                }
                else if(tUEInfo.txMode==_TRANSMISSION_MODE_2_)
                {
                    tUEInfo.nLayers=nCRSPorts;
                }
                else
                    tUEInfo.nLayers=cqiInfo.ri;
            }
            else
            {
                if(tUEInfo.txMode == _TRANSMISSION_MODE_6_)
                    tUEInfo.nLayers = 1;
                else
                    tUEInfo.nLayers = 1; // assuming initial transmission with single layer
            }
        }

        else//! CQI not yet reported
        {

            tUEInfo.isCQIReported=false;
            //cout<<"UE "<<tUEInfo.UEid<<" CQI not yet reported"<<endl;
            if(txMode == _TRANSMISSION_MODE_11_)
            {
                tUEInfo.sinrInDB.set_length(vrbNumbers.length());
                for(int rb_cnt=0;rb_cnt<vrbNumbers.length();rb_cnt++)
                    if(mIsDownlink)    
                        tUEInfo.sinrInDB(rb_cnt)=mTBTables->awgnTablesConvCoder.getSINRIndBfromCQI(defaultCQI)*ones(1);
                    else
                        tUEInfo.sinrInDB(rb_cnt)=5*ones(1); // default SNR for uplink

                if(isSemiOpenloopScheme)
                {
                    tUEInfo.i1 = 1;
                    tUEInfo.pmi = 1;
                }
                tUEInfo.nLayers=1;
            }
            else
            {
                tUEInfo.sinrInDB.set_length(vrbNumbers.length());
                for(int rb_cnt=0;rb_cnt<vrbNumbers.length();rb_cnt++)
                    if(mIsDownlink)    
                        tUEInfo.sinrInDB(rb_cnt)=mTBTables->awgnTablesConvCoder.getSINRIndBfromCQI(defaultCQI)*ones(1);
                    else
                        tUEInfo.sinrInDB(rb_cnt)=5*ones(1); // default SNR for uplink

                tUEInfo.i1 = 1;

                if(tUEInfo.txMode>2)
                    tUEInfo.pmi = 1;

                tUEInfo.nLayers=1;
            }
        }
    }

    return tUEInfo;
}

vec Scheduler::fCollectAvgRate(ivec& contendingUEs)
{
    vec avgRatePerUE = zeros(contendingUEs.size());
    for(int ue_cnt=0;ue_cnt<(int)contendingUEs.size();ue_cnt++)
    {
        int ueIndx=find(mUEIds,contendingUEs[ue_cnt]);
        if(ueIndx!=-1)
        {
            avgRatePerUE(ue_cnt)=mUEAvgRate(ueIndx);
        }
        else
        {
            cout<<"[detl:]Error. Unknown UEid..."<<endl;
            abort();
        }
    }

    return avgRatePerUE;
}


/*
 * ScheduledUEInfo_S Scheduler::fFindBestUEforULNewFornonSlot(ivec &contendingUEs, ivec &miniSlotIndxPerUE, Array<sCqiList> &cqiList, ivec &scheduledUEsPerRB, Array<ivec> &cqiForScheduledUEsPerRB, Array< vec >& effSINRsForScheduledUEsPerRB, Array< imat >& rbStructureForScheduledUEsPerRB, sScheduleReturn& schReturn, ivec vrbNumbers,OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, int prevUEId)
 * {
 * 
 *  ScheduledUEInfo_S schUEInfo;
 *  
 *  if(symbolsPerTTI.length() ==0)
 *  {
 *    cout<<"forgot to mention the symbol length ::"<<endl;
 *  }
 *  
 *  for(int minislotIndx =0; minislotIndx < symbolsPerTTI.length();minislotIndx++)
 *  {
 *    ScheduledUEInfo_S tempScheduledUEinfoForSUMIMO;
 *    
 *    bvec selectedUE=zeros_b(contendingUEs.length());
 *    for(int cnt=0;cnt<contendingUEs.length();cnt++)
 *      if(miniSlotIndxPerUE(cnt)==minislotIndx || miniSlotIndxPerUE(cnt)==-1)
 *	selectedUE(cnt)=(bin)1;  
 *      
 *    ivec shortlistedUsers =contendingUEs(find(selectedUE));
 *    tempScheduledUEinfoForSUMIMO = fFindBestUEforULNew(shortlistedUsers, cqiList, scheduledUEsPerRB, cqiForScheduledUEsPerRB, effSINRsForScheduledUEsPerRB, rbStructureForScheduledUEsPerRB, schReturn,vrbNumbers,symbolsPerTTI(minislotIndx), tOuterLoopLinkAdaptation, prevUEId);
 *    if(tempScheduledUEinfoForSUMIMO.ueid.length() == 0)
 *    {
 *      continue;
 *    }
 *    miniSlotIndxPerUE(find(contendingUEs,tempScheduledUEinfoForSUMIMO.ueid(0)))=minislotIndx;
 *    append(schUEInfo.ueid,tempScheduledUEinfoForSUMIMO.ueid);
 *    append(schUEInfo.mcsRate,tempScheduledUEinfoForSUMIMO.mcsRate);
 *    append(schUEInfo.instantMcsRate,tempScheduledUEinfoForSUMIMO.instantMcsRate); 
 *    append(schUEInfo.cqiIndices,tempScheduledUEinfoForSUMIMO.cqiIndices);
 *    append(schUEInfo.mcsIndxPerCodeword,tempScheduledUEinfoForSUMIMO.mcsIndxPerCodeword);
 *    append( schUEInfo.tbSizePerCodeword,tempScheduledUEinfoForSUMIMO.mcsIndxPerCodeword);
 *    //What is this precoder, I cant run for other modes.
 *    schUEInfo.precoderPerRB.push_back(tempScheduledUEinfoForSUMIMO.PrecoderPerRB[0]);
 *    //append(schUEInfo.pmi,tempScheduledUEinfoForSUMIMO.pmi);
 *    append(schUEInfo.nLayers,tempScheduledUEinfoForSUMIMO.nLayers); 
 *    append(schUEInfo.effSINRs,tempScheduledUEinfoForSUMIMO.effSINRs); 
 *    
 *    //I didnt change the frame structure length while passing on.
 *    //     cout<<"check the structure ::"<<tempScheduledUEinfoForSUMIMO.rbStructurePerRB.length()<<endl;
 *    append(schUEInfo.rbStructurePerRB,tempScheduledUEinfoForSUMIMO.rbStructurePerRB); 
 *    schUEInfo.txMode=tempScheduledUEinfoForSUMIMO.txMode;
 *    //We have to pass the length of symbols also ::
 *    schUEInfo.vrbs=tempScheduledUEinfoForSUMIMO.vrbs;
 *    append(schUEInfo.symPos,symbolsPerTTI(minislotIndx));
 *  }
 *  return(schUEInfo);
 * }
 * 
 * ScheduledUEInfo_S Scheduler::SemiOLSUMIMOScheduler(ivec &contentingUEs,ivec symbolsToSchedule, Array<sCqiList> &cqiList, ivec scheduledUEsPerRB, Array<ivec> &cqiForScheduledUEsPerRB,Array<imat> &rbStructureForScheduledUEsPerRB,sScheduleReturn &schReturn, int txAntennaCount, Array<ivec> vrbNumbers,OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, ivec& scheduledRBGIndices, int& selectedRBG,ivec invalidUEsForSUMIMO,int eNBID){
 *  
 *  if(symbolsToSchedule.length()==0) symbolsToSchedule=getIntegers(0,13);
 *  
 *  ScheduledUEInfo_S schUEInfo;
 *  int nSubbands, nUEs;
 *  int selectedRBGtemp = selectedRBG;
 *  if(selectedRBGtemp != -1)
 *  {
 *    ivec rbNumbers = vrbNumbers(selectedRBGtemp);
 *    vrbNumbers.set_length(1);
 *    vrbNumbers(0) = rbNumbers;
 *  }
 *  
 *  nSubbands = vrbNumbers.length();
 *  nUEs = contentingUEs.size();
 *  
 *  //! Collecting CQI Indices of the contenting UEs
 *  Array< Array< UEInfo_S > > tUEInfoPerSubbandsPerUEs(nSubbands);
 *  
 *  for(int iSB = 0; iSB < nSubbands; iSB++)
*  {
    *    // Skipping already scheduled RBG
        *    if(nSubbands > 1 && find(scheduledRBGIndices,iSB) !=-1)
        *    {
            *      continue;
            *    }
    *    tUEInfoPerSubbandsPerUEs(iSB).set_length(nUEs);
    *    for(int ue_cnt=0;ue_cnt< (int)cqiList.size();ue_cnt++)
        *    {
            *      // Skipping UE which is already scheduled in MUMIMO mode
                *      if(find(invalidUEsForSUMIMO,cqiList(ue_cnt).rnti)!=-1)
                *      {
                    *	continue;
                    *      }
            *      int ueIndx=find(contentingUEs, cqiList(ue_cnt).rnti);
            *      
                *      if(ueIndx!=-1)
                *      {
                    *	if(mIsDownlink)
                        *	  tUEInfoPerSubbandsPerUEs(iSB)(ueIndx) = fCollectUEInfo(cqiList(ue_cnt),dlTransmissionMode(ue_cnt),vrbNumbers(iSB));
                    *	else
                        *	  tUEInfoPerSubbandsPerUEs(iSB)(ueIndx) = fCollectUEInfo(cqiList(ue_cnt),ulTransmissionMode(ue_cnt),vrbNumbers(iSB));
                    *      }
            *    }
    *  }
    *  
    *  //! Collecting average Rate so far of the contenting UEs
    *  vec avgRatePerUE = fCollectAvgRate(contentingUEs);
    *  
    *  Array<Array<ivec> > possbileMCSPerCodeword(nSubbands);
    *  Array<Array<ivec> > tbSizePerCodewordInBytes(nSubbands);
    *  Array<vec> finalMCSRatePerUE(nSubbands);
    *  Array<vec> instantaneousMCSRatePerUE(nSubbands);
    *  Array<Array< Array<imat> > > rbStructurePerRB(nSubbands);
    *  Array<vec> pfMetric(nSubbands);
    *  
    *  double maxPFMetric = -1.0;
    *  int selectedUEIndx=-1;
    *  
    *  for(int iSB = 0; iSB < nSubbands; iSB++)
    *  {
        *    possbileMCSPerCodeword(iSB).set_length(contentingUEs.size());
        *    tbSizePerCodewordInBytes(iSB).set_length(contentingUEs.size());
        *    finalMCSRatePerUE(iSB)=zeros(contentingUEs.size());
        *    instantaneousMCSRatePerUE(iSB)=zeros(contentingUEs.size());
        *    pfMetric(iSB)=zeros(contentingUEs.size())-1;
        *    rbStructurePerRB(iSB).set_length(contentingUEs.size());
        *    
            *    // Skipping already scheduled RBG
            *    if(nSubbands > 1 && find(scheduledRBGIndices,iSB) !=-1)
            *    {
                *      continue;
                *    }
        *    
            *    for(int ue_cnt=0;ue_cnt < (int)contentingUEs.size();ue_cnt++)
            *    {
                *      // Skipping UE which is already scheduled in MUMIMO mode
                    *      if(find(invalidUEsForSUMIMO,contentingUEs[ue_cnt])!=-1)
                    *      {
                        *	continue;
                        *      }
                *      
                    *      Array<ivec> possibleCQIIndices;
                *      Array<imat> possibleRBStructures;
                *      LinkAdaptation_S tLinkAdaptation;
                *      ivec tbSizePerCodewordOldInBytes(0);
                *      //! Collecting scheduled CQI Indices for the selected UE
                    *      if(scheduledUEsPerRB.length()!=0)
                    *      {
                        *	ivec scheduledRBIndices=find(scheduledUEsPerRB,contentingUEs[ue_cnt],true);
                        *	if(scheduledRBIndices.length() != 0)
                            *	{
                                *	  possibleCQIIndices=getFromArray(cqiForScheduledUEsPerRB,scheduledRBIndices);
                                *	  possibleRBStructures=getFromArray(rbStructureForScheduledUEsPerRB,scheduledRBIndices);
                                *	  tLinkAdaptation = fDoLinkAdaptationDL(contentingUEs[ue_cnt],tUEInfoPerSubbandsPerUEs(iSB)(ue_cnt).nLayers,tUEInfoPerSubbandsPerUEs(iSB)(ue_cnt).txMode,scheduledRBIndices.length(),symbolsToSchedule,possibleRBStructures,possibleCQIIndices,tOuterLoopLinkAdaptation,-3);
                                *	  tbSizePerCodewordOldInBytes = tLinkAdaptation.tbSizePerCodewordInBytes;
                                *	}
                        *      }
                *      RSInfo_S rsInfo;
                *      setRsInfo(rsInfo,*mPortsInfo,*mFrameStructure,0,TransmissionScheme_E((int)tUEInfoPerSubbandsPerUEs(iSB)(ue_cnt).txMode),tUEInfoPerSubbandsPerUEs(iSB)(ue_cnt).nLayers,txAntennaCount,0,mIsDownlink,isCQIPortAvailable,symbolsToSchedule(0));
                *    
                    *      rbStructurePerRB(iSB)(ue_cnt)=getMergedRBStructures(mFrameStructure,rsInfo,vrbNumbers(iSB),true,true,mFrameReservationInfo);
                *      
                    *      tLinkAdaptation = fDoLinkAdaptationDL(contentingUEs[ue_cnt],tUEInfoPerSubbandsPerUEs(iSB)(ue_cnt).nLayers,tUEInfoPerSubbandsPerUEs(iSB)(ue_cnt).txMode,vrbNumbers(iSB).length(),symbolsToSchedule,rbStructurePerRB(iSB)(ue_cnt),tUEInfoPerSubbandsPerUEs(iSB)(ue_cnt).cqi,tOuterLoopLinkAdaptation,-3);
                *      instantaneousMCSRatePerUE(iSB)(ue_cnt) = tLinkAdaptation.mcsRate;
                *      
                    *      if(tLinkAdaptation.mcsIndxPerCodeword.size()==0)
                    *      {
                        *	continue;
                        *      }
                *      
                    *      //! Appending the current CQI Indices
                    *      append(possibleCQIIndices,tUEInfoPerSubbandsPerUEs(iSB)(ue_cnt).cqi);
                *      append(possibleRBStructures,rbStructurePerRB(iSB)(ue_cnt));
                *      
                    *      int nNumOfRbs = vrbNumbers(iSB).length() + fFindNumOfRBsScheduled(schReturn.mResourceBlockMapping,schReturn.UEIndex,contentingUEs[ue_cnt]);
                *      
                    *      tLinkAdaptation = fDoLinkAdaptationDL(contentingUEs[ue_cnt],tUEInfoPerSubbandsPerUEs(iSB)(ue_cnt).nLayers,tUEInfoPerSubbandsPerUEs(iSB)(ue_cnt).txMode,nNumOfRbs,symbolsToSchedule,possibleRBStructures,possibleCQIIndices,tOuterLoopLinkAdaptation,-3);
                *      
                    *      if(tbSizePerCodewordOldInBytes.length() > 0 && sum(tbSizePerCodewordOldInBytes) >= sum(tLinkAdaptation.tbSizePerCodewordInBytes))
                    *      {
                        *	instantaneousMCSRatePerUE(iSB)(ue_cnt) = 0.0;
                        *      }
                *      
                    *      possbileMCSPerCodeword(iSB)(ue_cnt) = tLinkAdaptation.mcsIndxPerCodeword;
                *      tbSizePerCodewordInBytes(iSB)(ue_cnt) = tLinkAdaptation.tbSizePerCodewordInBytes;
                *      finalMCSRatePerUE(iSB)(ue_cnt) = tLinkAdaptation.mcsRate;
                *      
                    *    }
        *    pfMetric(iSB)=elem_div(instantaneousMCSRatePerUE(iSB),avgRatePerUE);
        *    
            *  }
            *  
            *  //! Selecting UE based on PF Metric
            *  
            *  vec maxPFMetricOverSBs = zeros(pfMetric.length());
            *  ivec maxIndexOverSBs = zeros_i(pfMetric.length()) - 1;
            *  for(int sb_cnt = 0; sb_cnt < pfMetric.length(); sb_cnt++)
            *  {
                *    if(pfMetric(sb_cnt).length()>0)
                    *    {
                        *      maxPFMetricOverSBs(sb_cnt) = max(pfMetric(sb_cnt));
                        *      ivec possibleselectedUEIndices = find(pfMetric(sb_cnt),max(pfMetric(sb_cnt)),true);
                        *      if(possibleselectedUEIndices.length()==1)
                            *	maxIndexOverSBs(sb_cnt)=possibleselectedUEIndices(0);
                        *      else
                            *	maxIndexOverSBs(sb_cnt)=randPick(possibleselectedUEIndices,1)(0);
                        *      
                            *      #ifdef MUMIMO_Debug
                            *      cout<<"SB : "<<sb_cnt<<" r : "<<instantaneousMCSRatePerUE(sb_cnt)(maxIndexOverSBs(sb_cnt))<<" R : "<<avgRatePerUE(maxIndexOverSBs(sb_cnt))<<" PF : "<<max(pfMetric(sb_cnt))<<" UE : "<<contentingUEs[maxIndexOverSBs(sb_cnt)]<<endl;
                        *      #endif
                            *    }
                *  }
                *  maxPFMetric = max(maxPFMetricOverSBs);
                *  ivec possibleselectedSBs = find(maxPFMetricOverSBs,max(maxPFMetricOverSBs),true);
*  if(possibleselectedSBs.length()==1)
    *    selectedRBG=possibleselectedSBs(0);
    *  else
    *    selectedRBG=randPick(possibleselectedSBs,1)(0);
    *  
    *  selectedUEIndx = maxIndexOverSBs(selectedRBG);
    *  
    *  #ifdef MUMIMO_Debug
    *  cout<<endl<<"selectedRBG : "<<selectedRBG<<endl;
    *  cout<<"maxPFMetric : "<<maxPFMetric<<endl;
    *  cout<<"UE : "<<contentingUEs[selectedUEIndx]<<endl;
    *  //     int breakPoint;
    *  //     cin>>breakPoint;
    *  #endif
    *  
    *  // Skipping the scheduling
*  if(maxPFMetric == 0)
    *  {
        *    if(selectedRBGtemp != -1)
            *      selectedRBG = selectedRBGtemp;
        *    return schUEInfo;
        *  }
        *  
        *  schUEInfo.pfMetric = maxPFMetric;
        *  
        *  schUEInfo.ueid.set_size(1);
        *  schUEInfo.mcsRate.set_size(1);
        *  schUEInfo.instantMcsRate.set_size(1);
        *  schUEInfo.cqiIndices.set_size(1);
        *  schUEInfo.mcsIndxPerCodeword.set_size(1);
        *  schUEInfo.tbSizePerCodeword.set_size(1);
        *  schUEInfo.nLayers.set_size(1);
        *  
        *  
        *  schUEInfo.ueid(0)=contentingUEs[selectedUEIndx];
        *  schUEInfo.mcsRate(0)=finalMCSRatePerUE(selectedRBG)(selectedUEIndx);
        *  schUEInfo.instantMcsRate(0)=instantaneousMCSRatePerUE(selectedRBG)(selectedUEIndx);
        *  schUEInfo.cqiIndices(0)=tUEInfoPerSubbandsPerUEs(selectedRBG)(selectedUEIndx).cqi;
        *  schUEInfo.cqiType=tUEInfoPerSubbandsPerUEs(selectedRBG)(selectedUEIndx).cqiType;
        *  schUEInfo.mcsIndxPerCodeword(0)=possbileMCSPerCodeword(selectedRBG)(selectedUEIndx);
        *  schUEInfo.tbSizePerCodeword(0)=tbSizePerCodewordInBytes(selectedRBG)(selectedUEIndx);
        *  schUEInfo.symPos.set_length(1);schUEInfo.symPos(0)=symbolsToSchedule;
        *  schUEInfo.precoderPerRB.resize(1);
        *  schUEInfo.precoderPerRB[0].resize(vrbNumbers(selectedRBG).length());
        *  
        *  Array<cmat> nCodebookMatrix=mCodebook->getCodebook(txAntennaCount,tUEInfoPerSubbandsPerUEs(selectedRBG)(selectedUEIndx).nLayers,mIsDownlink);
        *  int wb_i1=tUEInfoPerSubbandsPerUEs(selectedRBG)(selectedUEIndx).i1;
        *  for(int rb_cnt = 0; rb_cnt < vrbNumbers(selectedRBG).length(); rb_cnt++)
        *  {
            *    schUEInfo.precoderPerRB[0][rb_cnt] = nCodebookMatrix( (wb_i1*16 + rb_cnt)%4 );
            *  }
            *  
            *  schUEInfo.nLayers(0)=tUEInfoPerSubbandsPerUEs(selectedRBG)(selectedUEIndx).nLayers;
            *  schUEInfo.txMode=tUEInfoPerSubbandsPerUEs(selectedRBG)(selectedUEIndx).txMode;
            *  schUEInfo.vrbs=vrbNumbers(selectedRBG);
            *  schUEInfo.rbStructurePerRB=rbStructurePerRB(selectedRBG)(selectedUEIndx);
            *  schUEInfo.PairedLayers=-1;
    *  
*  if(selectedRBGtemp != -1)
    *    selectedRBG = selectedRBGtemp;
    *  
    *  #ifdef MUMIMO_Debug
    *  std::ofstream PFlogs;
    *  string PFlogsFileName = "Results/PFlogs/PFlogs_eNB_ID_"+toString(eNBID)+".txt";
    *  
    *  static bool isNewPrint=true;
*  if(isNewPrint)
    *  {
        *    PFlogs.open(PFlogsFileName.c_str(),ios::out);
        *    
            *    if(currentTaskid==0)
            *      PFlogs<<"% SBIndx, UE_ID,\tInstantRate,\tAvgRate,\tPFMetric,\tFinalRate\n";
        *    isNewPrint=false;
        *  }
        *  else
        *    PFlogs.open(PFlogsFileName.c_str(),ios::app);
        *  
        *  PFlogs<<setw(3)<<selectedRBG<<",\t"<<setw(5)<<schUEInfo.scheduledUEs(0)<<",\t"<<setw(8)<<schUEInfo.instantMcsRate(0)<<",\t\t"<<setw(8)<<avgRatePerUE(selectedUEIndx)<<",\t"<<setw(8)<<maxPFMetric<<",\t"<<setw(8)<<schUEInfo.mcsRate(0)<<endl;
        *  #endif
        *  
        *  return(schUEInfo);
        *  
        * }*/
        /*
         * 
         * void fReArrangeUEsForOLMUMIMO(Array<ivec>& MUMIMOoptions, Array<ivec> pairedUEsInCurrSubframe){
         *  
         *  if(pairedUEsInCurrSubframe.length() > 0)
         *  {
         *    ivec UEList(0), uniqueUEs(0);
         *    
         *    for(int j = 0 ; j < MUMIMOoptions.length(); j++)
         *    {
         *      append(UEList,MUMIMOoptions(j));
         *    }
         *    uniqueUEs = findUniqueNumbers(UEList);
         *    
         *    ivec orderPariedUEs = -1*ones_i(uniqueUEs.length());
         *    
         *    for(int i = 0; i < uniqueUEs.length(); i++)
         *    {
         *      for(int j = 0 ; j < pairedUEsInCurrSubframe.length(); j++)
         *      {
         *	int indx = find(pairedUEsInCurrSubframe(j),uniqueUEs(i));
         *	if(indx != -1)
         *	  orderPariedUEs(i) = indx;
         *      }
         *    }
         *    
         *    ivec IndicesToRemove(0);
         *    
         *    for(int j = 0 ; j < MUMIMOoptions.length(); j++)
         *    {
         *      ivec indices = find(uniqueUEs,MUMIMOoptions(j));
         *      
         *      if( (orderPariedUEs(indices(0)) == 1 && orderPariedUEs(indices(1)) != 1) || (orderPariedUEs(indices(1)) == 0 && orderPariedUEs(indices(0)) != 0) )
         *      {
         *	MUMIMOoptions(j) = swapVec(MUMIMOoptions(j));
         *      }
         *      else if( (orderPariedUEs(indices(0)) == 1 && orderPariedUEs(indices(1)) == 1) || (orderPariedUEs(indices(0)) == 0 && orderPariedUEs(indices(1)) == 0) )
         *      {
         *	append(IndicesToRemove,j);
         *      }
         *      
         *    }
         *    deleteInArray(MUMIMOoptions,IndicesToRemove);
         *    
         *  }
         *  
         * }*/

        // ScheduledUEInfo_S Scheduler::SemiOLMUMIMOScheduler(ivec& contentingUEs, ivec symbolsToSchedule, Array< sCqiList >& cqiList, ivec scheduledUEsPerRB, ivec& isMUMIMOPerRB, Array< ivec >& cqiForScheduledUEsPerRB, Array< imat >& rbStructureForScheduledUEsPerRB, sScheduleReturn& schReturn, int txAntennaCount, Array< ivec > vrbNumbers, OuterLoopLinkAdaptation_S tOuterLoopLinkAdaptation, ivec& scheduledRBGIndices, int& selectedRBG, int eNBID){
        //   
        //   ScheduledUEInfo_S schUEInfo;
        //   
        //   //   cout<<"Scheduling OL MUMIMO scheduler for eNB "<<eNBID<<" subband : "<<selectedRBG<<endl;
        //   
        //   ScheduledUEInfo_S tempScheduledUEinfoForSUMIMO;
        //   
        //   ivec invalidUEsForSUMIMO(0), invalidUEsForMUMIMO(0);
        //   for(int cnt = 0; cnt < scheduledUEsPerRB.length(); cnt++)
        //   {
        //     if(isMUMIMOPerRB(cnt) != -1)
        //       append(invalidUEsForSUMIMO,scheduledUEsPerRB(cnt));
        //     else
        //       append(invalidUEsForMUMIMO,scheduledUEsPerRB(cnt));
        //   }
        //   
        //   invalidUEsForSUMIMO = findUniqueNumbers(invalidUEsForSUMIMO);
        //   invalidUEsForMUMIMO = findUniqueNumbers(invalidUEsForMUMIMO);
        //   
        //   tempScheduledUEinfoForSUMIMO = SemiOLSUMIMOScheduler(contentingUEs,symbolsToSchedule,cqiList,scheduledUEsPerRB,cqiForScheduledUEsPerRB,rbStructureForScheduledUEsPerRB,schReturn,txAntennaCount,vrbNumbers,tOuterLoopLinkAdaptation,scheduledRBGIndices,selectedRBG,invalidUEsForSUMIMO,eNBID);
        //   ivec vrbNumbers1 = vrbNumbers(selectedRBG);
        //   
        //   int nUEs = contentingUEs.size();
        //   
        //   //! Collecting CQI Indices of the contenting UEs
        //   Array<UEInfo_S> tUEInfoPerUEs(nUEs);
        //   
        //   for(int cqiList_cnt = 0; cqiList_cnt < (int)cqiList.size(); cqiList_cnt++)
        //   {
        //     for(int ue_cnt = 0; ue_cnt < nUEs; ue_cnt++)
        //     {
        //       if(contentingUEs[ue_cnt] == cqiList(cqiList_cnt).rnti)
        //       {
        // 	if(mIsDownlink)
        // 	  tUEInfoPerUEs(ue_cnt) = fCollectUEInfo(cqiList(cqiList_cnt),dlTransmissionMode(cqiList_cnt),vrbNumbers1);
        // 	else
        // 	  tUEInfoPerUEs(ue_cnt) = fCollectUEInfo(cqiList(cqiList_cnt),ulTransmissionMode(cqiList_cnt),vrbNumbers1);
        //       }
        //     }
        //   }
        //   
        //   //! Collecting average Rate so far of the contenting UEs
        //   vec avgRatePerUE = fCollectAvgRate(contentingUEs);
        //   
        //   Array<MUMIMOPairingInfo_S> MUMIMOPairingUsers;
        //   int nUserPairing = 2;
        //   if(mIsDownlink)
        // 	MUMIMOPairingUsers = fFindMUMIMOPairingOptions(invalidUEsForMUMIMO,tUEInfoPerUEs,*mCodebook,nUserPairing,txAntennaCount,dlTransmissionMode(0));
        //   else
        // 	MUMIMOPairingUsers = fFindMUMIMOPairingOptions(invalidUEsForMUMIMO,tUEInfoPerUEs,*mCodebook,nUserPairing,txAntennaCount,ulTransmissionMode(0));
        //     
        //   //fReArrangeUEsForOLMUMIMO(MUMIMOPairingUsers,pairedUEsInCurrentSubframe);
        //   
        //   #ifdef MUMIMO_Debug
        //   cout<<"MU-MIMO pairingOptions in SB "<<selectedRBG << ": " <<MUMIMOPairingUsers<<endl;
        //   #endif
        //   
        //   Array<vec> finalMCSRatePerUEpair(MUMIMOPairingUsers.size());
        //   Array<vec> instantMCSRatePerUEpair(MUMIMOPairingUsers.size());
        //   Array< Array <ivec> > tbSizePerCodewordInBytesPerUEpair(MUMIMOPairingUsers.size());
        //   Array< Array <ivec> > possbileMCSPerCodewordPerUEpair(MUMIMOPairingUsers.size());
        //   Array < Array< Array<imat>  > > rbStructurePerRBPerUEpair(MUMIMOPairingUsers.size());
        //   vec pfMetricPerUEPair = zeros(MUMIMOPairingUsers.size());
        //   
        //   //MU-MIMO
        //   for(int pair_cnt=0;pair_cnt<MUMIMOPairingUsers.size();pair_cnt++)
        //   {
        //     finalMCSRatePerUEpair(pair_cnt) = zeros(MUMIMOPairingUsers(pair_cnt).mUEIds.size());
        //     instantMCSRatePerUEpair(pair_cnt) = zeros(MUMIMOPairingUsers(pair_cnt).mUEIds.size());
        //     tbSizePerCodewordInBytesPerUEpair(pair_cnt).set_length(MUMIMOPairingUsers(pair_cnt).mUEIds.size());
        //     possbileMCSPerCodewordPerUEpair(pair_cnt).set_length(MUMIMOPairingUsers(pair_cnt).mUEIds.size());
        //     rbStructurePerRBPerUEpair(pair_cnt).set_length(MUMIMOPairingUsers(pair_cnt).mUEIds.size());
        //     
        //     for(int ue_cnt=0;ue_cnt<MUMIMOPairingUsers(pair_cnt).mUEIds.size();ue_cnt++)
        //     {
        //       int UEid = -1, UEindx = -1;
        //       for(int i = 0; i < tUEInfoPerUEs.length(); i++)
        //       {
        // 	if(tUEInfoPerUEs(i).UEid == MUMIMOPairingUsers(pair_cnt).mUEIds(ue_cnt))
        // 	{
        // 	  UEid = tUEInfoPerUEs(i).UEid;
        // 	  UEindx = i;
        // 	}
        //       }
        //       if(UEindx == -1)
        //       {
        // 	cerr<<"Error in MUMIMO scheduler "<<endl; abort();
        //       }
        //       
        //       Array<ivec> possibleCQIIndices;
        //       Array<imat> possibleRBStructures;
        //       LinkAdaptation_S tLinkAdaptation;
        //       ivec tbSizePerCodewordOldInBytes(0);
        //       
        //       double SINRoffsetForMUMIMO = MUMIMOPairingUsers(pair_cnt).SINRoffset(ue_cnt);
        //       
        //       //! Collecting scheduled CQI Indices for the selected UE
        //       if(scheduledUEsPerRB.length()!=0)
        //       {
        // 	ivec scheduledRBIndices = find(scheduledUEsPerRB,UEid,true);
        // 	possibleCQIIndices = getFromArray(cqiForScheduledUEsPerRB,scheduledRBIndices);
        // 	possibleRBStructures = getFromArray(rbStructureForScheduledUEsPerRB,scheduledRBIndices);
        // 	tLinkAdaptation = fDoLinkAdaptationDL(UEid,tUEInfoPerUEs(UEindx).nLayers,tUEInfoPerUEs(UEindx).txMode,scheduledRBIndices.length(),symbolsToSchedule,possibleRBStructures,possibleCQIIndices,tOuterLoopLinkAdaptation,SINRoffsetForMUMIMO);
        // 	tbSizePerCodewordOldInBytes = tLinkAdaptation.tbSizePerCodewordInBytes;
        //       }
        //       
        //       RSInfo_S rsInfo;
        //       setRsInfo(rsInfo,*mPortsInfo,*mFrameStructure,0,TransmissionScheme_E((int)tUEInfoPerUEs(UEindx).txMode),tUEInfoPerUEs(UEindx).nLayers,txAntennaCount,0,mIsDownlink,isCQIPortAvailable,symbolsToSchedule(0));
        //       
        //       rbStructurePerRBPerUEpair(pair_cnt)(ue_cnt)=getMergedRBStructures(mFrameStructure,rsInfo,vrbNumbers1,true,true,mFrameReservationInfo);
        //       
        //       
        //       
        //       tLinkAdaptation = fDoLinkAdaptationDL(UEid,tUEInfoPerUEs(UEindx).nLayers,tUEInfoPerUEs(UEindx).txMode,vrbNumbers1.length(),symbolsToSchedule,rbStructurePerRBPerUEpair(pair_cnt)(ue_cnt),tUEInfoPerUEs(UEindx).cqi,tOuterLoopLinkAdaptation,SINRoffsetForMUMIMO);
        //       
        //       //       cout<<"tLinkAdaptation.mcsRate : "<<tLinkAdaptation.mcsRate<<endl;
        //       instantMCSRatePerUEpair(pair_cnt)(ue_cnt) = tLinkAdaptation.mcsRate;
        //       
        //       //! Appending the current CQI Indices
        //       
        //       append(possibleCQIIndices,tUEInfoPerUEs(UEindx).cqi);
        //       append(possibleRBStructures,rbStructurePerRBPerUEpair(pair_cnt)(ue_cnt));
        //       
        //       int nNumOfRbs = vrbNumbers1.length() + fFindNumOfRBsScheduled(schReturn.mResourceBlockMapping,schReturn.UEIndex,UEid);
        //       
        //       
        //       tLinkAdaptation = fDoLinkAdaptationDL(UEid,tUEInfoPerUEs(UEindx).nLayers,tUEInfoPerUEs(UEindx).txMode,nNumOfRbs,symbolsToSchedule,possibleRBStructures,possibleCQIIndices,tOuterLoopLinkAdaptation,SINRoffsetForMUMIMO);
        //       
        //       if(tbSizePerCodewordOldInBytes.length() > 0 && sum(tbSizePerCodewordOldInBytes) >= sum(tLinkAdaptation.tbSizePerCodewordInBytes))
        //       {
        // 	instantMCSRatePerUEpair(pair_cnt)(ue_cnt) = 0.0;
        //       }
        //       possbileMCSPerCodewordPerUEpair(pair_cnt)(ue_cnt) = tLinkAdaptation.mcsIndxPerCodeword;
        //       tbSizePerCodewordInBytesPerUEpair(pair_cnt)(ue_cnt) = tLinkAdaptation.tbSizePerCodewordInBytes;
        //       finalMCSRatePerUEpair(pair_cnt)(ue_cnt) = tLinkAdaptation.mcsRate;
        //       
        //       if(instantMCSRatePerUEpair(pair_cnt)(ue_cnt) != 0.0)
        // 	pfMetricPerUEPair(pair_cnt) += (instantMCSRatePerUEpair(pair_cnt)(ue_cnt)/avgRatePerUE(find(contentingUEs,UEid)));
        //       else
        // 	pfMetricPerUEPair(pair_cnt) = 0.0;
        //       
        //     }
        //   }
        //   
        //   int selectedUEPairIndx=-1;
        //   int tempselectedUEPairIndx=-1;
        //   if( pfMetricPerUEPair.size() > 0 && max(pfMetricPerUEPair) > 0.0 )
        //     tempselectedUEPairIndx =  max_index(pfMetricPerUEPair);
        //   
        //   #ifdef MUMIMO_Debug
        //   if( pfMetricPerUEPair.size() > 0)
        //     cout<<"pfMetricMU : "<<max(pfMetricPerUEPair)<<"\t";
        //   
        //   cout<<"pfMetricSU : "<<tempScheduledUEinfoForSUMIMO.pfMetric<<endl;
        //   
        //   if( tempselectedUEPairIndx != -1)
        //     cout<<"MCSRatePerUEpair : "<<sum(instantMCSRatePerUEpair(tempselectedUEPairIndx))<<"\t";
        //   
        //   cout<<"MCSRatePerUE : "<<tempScheduledUEinfoForSUMIMO.instantMcsRate<<endl;
        //   #endif
        //   
        //   int nUEsInMUMIMO;
        //   if(tempselectedUEPairIndx != -1)
        //     nUEsInMUMIMO = MUMIMOPairingUsers(tempselectedUEPairIndx).mUEIds.length();
        //   
        //   //   if( tempselectedUEPairIndx != -1 && sum(instantMCSRatePerUEpair(tempselectedUEPairIndx)) >= tempScheduledUEinfoForSUMIMO.instantMcsRate(0) && max(pfMetricPerUEPair) >= tempScheduledUEinfoForSUMIMO.pfMetric)
        //   if( tempselectedUEPairIndx != -1 &&  max(pfMetricPerUEPair) >= tempScheduledUEinfoForSUMIMO.pfMetric)
        //   {
        //     selectedUEPairIndx = max_index(pfMetricPerUEPair);
        //   }
        //   
        //   if(selectedUEPairIndx!=-1)
        //   {
        //     schUEInfo.ueid.set_size(nUEsInMUMIMO);
        //     schUEInfo.mcsRate.set_size(nUEsInMUMIMO);
        //     schUEInfo.instantMcsRate.set_size(nUEsInMUMIMO);
        //     schUEInfo.cqiIndices.set_size(nUEsInMUMIMO);
        //     schUEInfo.mcsIndxPerCodeword.set_size(nUEsInMUMIMO);
        //     schUEInfo.tbSizePerCodeword.set_size(nUEsInMUMIMO);
        //     
        //     schUEInfo.nLayers.set_size(nUEsInMUMIMO);
        //     schUEInfo.precoderPerRB.resize(nUEsInMUMIMO);
        //     schUEInfo.symPos.set_length(1);schUEInfo.symPos(0)=symbolsToSchedule;
        //     for(int ue_cnt = 0; ue_cnt < nUEsInMUMIMO; ue_cnt++)
        //     {
        //       
        //       schUEInfo.ueid(ue_cnt) = MUMIMOPairingUsers(selectedUEPairIndx).mUEIds(ue_cnt);
        //       schUEInfo.mcsRate(ue_cnt)=finalMCSRatePerUEpair(selectedUEPairIndx)(ue_cnt);
        //       schUEInfo.instantMcsRate(ue_cnt)=instantMCSRatePerUEpair(selectedUEPairIndx)(ue_cnt);
        //       
        //       schUEInfo.precoderPerRB[ue_cnt].resize(vrbNumbers1.length());
        //       
        //       int indx = -1;
        //       for(int i = 0; i < tUEInfoPerUEs.length(); i++)
        //       {
        // 	if(tUEInfoPerUEs(i).UEid == schUEInfo.ueid(ue_cnt))
        // 	  indx = i;
        //       }
        //       if(indx == -1)
        //       {
        // 	cerr<<"Error in MUMIMO scheduler "<<endl;
        //       }
        //       schUEInfo.cqiIndices(ue_cnt)=tUEInfoPerUEs(indx).cqi;
        //       schUEInfo.cqiType=tUEInfoPerUEs(indx).cqiType;
        //       schUEInfo.mcsIndxPerCodeword(ue_cnt)=possbileMCSPerCodewordPerUEpair(selectedUEPairIndx)(ue_cnt);
        //       schUEInfo.tbSizePerCodeword(ue_cnt)=tbSizePerCodewordInBytesPerUEpair(selectedUEPairIndx)(ue_cnt);
        //       schUEInfo.nLayers(ue_cnt)=tUEInfoPerUEs(indx).nLayers;
        //       schUEInfo.rbStructurePerRB=rbStructurePerRBPerUEpair(selectedUEPairIndx)(ue_cnt);
        //       
        //       
        //       Array<cmat> nCodebookMatrix=mCodebook->getCodebook(txAntennaCount,tUEInfoPerUEs(indx).nLayers,mIsDownlink);
        //       int wb_i1=tUEInfoPerUEs(indx).i1;
        //       for(int rb_cnt = 0; rb_cnt < vrbNumbers(selectedRBG).length(); rb_cnt++)
        //       {
        // 	schUEInfo.precoderPerRB[ue_cnt][rb_cnt] = (1.0/sqrt(nUEsInMUMIMO))*nCodebookMatrix( (wb_i1*16 + rb_cnt)%4 );
        //       }
        //       
        // 	
        //       schUEInfo.txMode=tUEInfoPerUEs(indx).txMode;
        //    
        //       
        //     }
        //     schUEInfo.vrbs=vrbNumbers1;
        //     
        //     schUEInfo.PairedLayers=1;
        //     
        //     append(pairedUEsInCurrentSubframe,schUEInfo.ueid);
        //     
        //     #ifdef MUMIMO_Debug
        //     cout<<"SB : "<<selectedRBG<<" MU-MIMO selected UEs "<<schUEInfo.scheduledUEs<<endl<<endl;
        //     #endif
        //     
        //     
        //     
        //   }
        //   else
        //   {
        //     schUEInfo = tempScheduledUEinfoForSUMIMO;
        //     #ifdef MUMIMO_Debug
        //     cout<<"SB : "<<selectedRBG<<" SU-MIMO selected UE "<<schUEInfo.scheduledUEs<<endl<<endl;
        //     #endif
        //   }
        //   
        //   return(schUEInfo);
        // }

void fRemoveRank2UEs(Array<UEInfo_S>& tUEInfoPerUEs)
{
    ivec IndicesToRemove(0);
    for(int ue_cnt = 0; ue_cnt < tUEInfoPerUEs.size(); ue_cnt++)
    {
        if(tUEInfoPerUEs(ue_cnt).nLayers == 2)
        {
            append(IndicesToRemove,ue_cnt);
        }
    }
    if(IndicesToRemove.size() > 0)
        deleteInArray(tUEInfoPerUEs,IndicesToRemove);
}
// void fSearchSemiOLMUMIMOPairing(Array<MUMIMOPairingInfo_S>& MUMIMOPairingOptions, Array<UEInfo_S>& tUEInfoPerUEs, Codebook& mCodeBook, int txAntennaCount,int nUsers)
// {
//   (void)mCodeBook;
//   (void)txAntennaCount;
//   
//   fRemoveRank2UEs(tUEInfoPerUEs);
//   
//   for(int ue_cnt = 0; ue_cnt < tUEInfoPerUEs.size(); ue_cnt++)
//   {
//     for(int ue_cnt1 = 0; ue_cnt1 < tUEInfoPerUEs.size(); ue_cnt1++)
//     {
//       int Diff = abs(tUEInfoPerUEs(ue_cnt).i1 - tUEInfoPerUEs(ue_cnt1).i1);
//       int i1Offset;
//       if(Diff > 8)
// 	i1Offset = 8 - (Diff % 8);
//       else
// 	i1Offset = Diff;
//       int cqi1 = tUEInfoPerUEs(ue_cnt).cqi(0)(0);
//       int cqi2 = tUEInfoPerUEs(ue_cnt1).cqi(0)(0);
//       if(ue_cnt != ue_cnt1 && i1Offset >= 4 && cqi1 > 3 && cqi2 > 3 && cqi1 < 13 && cqi2 < 13)
//       {
// 	bool isRepeat = false;
// 	MUMIMOPairingInfo_S tMUMIMOPairingInfo;
// 	tMUMIMOPairingInfo.mUEIds.set_size(nUsers);
// 	tMUMIMOPairingInfo.SINRoffsetInDB.set_size(nUsers);
// 	tMUMIMOPairingInfo.mUEIds(0) = tUEInfoPerUEs(ue_cnt).UEid;
// 	tMUMIMOPairingInfo.mUEIds(1) = tUEInfoPerUEs(ue_cnt1).UEid;
// 	
// 	for(int cnt = 0; cnt < MUMIMOPairingOptions.length(); cnt++)
// 	{
// 	  if( tMUMIMOPairingInfo.mUEIds == MUMIMOPairingOptions(cnt).mUEIds || (tUEInfoPerUEs(ue_cnt).UEid == MUMIMOPairingOptions(cnt).mUEIds(1) && tUEInfoPerUEs(ue_cnt1).UEid == MUMIMOPairingOptions(cnt).mUEIds(0)))
// 	    isRepeat = true;
// 	}
// 	if(!isRepeat)
// 	{
// 	  switch(i1Offset)
// 	  {
// 	    case 4:
// 	    case 5:
// 	      tMUMIMOPairingInfo.SINRoffsetInDB(0) = -7.5;
// 	      tMUMIMOPairingInfo.SINRoffsetInDB(1) = -7.5;
// 	      break;
// 	    case 6:
// 	    case 7:
// 	      tMUMIMOPairingInfo.SINRoffsetInDB(0) = -6.0;
// 	      tMUMIMOPairingInfo.SINRoffsetInDB(1) = -6.0;
// 	      break;
// 	    case 8:
// 	      tMUMIMOPairingInfo.SINRoffsetInDB(0) = -4.5;
// 	      tMUMIMOPairingInfo.SINRoffsetInDB(1) = -4.5;
// 	      break;
// 	    default:
// 	      cout<<"Error in fSearchSemiOLMUMIMOPairing()"<<endl;abort();
// 	  }
// 	  append(MUMIMOPairingOptions,tMUMIMOPairingInfo);
// 	}
//       }
//     }
//     
//   }
//   
// }

void Scheduler::fFindMUMIMOPairingOptions(Array<MUMIMOPairingInfo_S>& MUMIMOPairingOptions,Array< UEInfo_S >& tUEInfoPerUEs, Codebook* mCodeBook, int eNBAntennaCount, TransmissionMode_E txMode, Array<ivec> &pairedUEs, Array<vec> &SINRInDBOfPairedUEs)
{

    MUMIMOPairingOptions.set_length(0);

    //   fRemoveInvalidUEsForMUMIMO(tUEInfoPerUEs,invalidUEsForMUMIMO,txMode);

    //   if(mIsDownlink)
    //   {
    //     if(txMode == _TRANSMISSION_MODE_5_)
    //     {
    //       fSearchMUMIMOPairingForTM5(MUMIMOPairingOptions,tUEInfoPerUEs,mCodeBook,eNBAntennaCount,nUserPairing);
    //     }
    //     else if(txMode == _TRANSMISSION_MODE_9_)
    //     {
    //       fSearchMUMIMOPairingForTM9(MUMIMOPairingOptions,tUEInfoPerUEs,mCodeBook,eNBAntennaCount,nUserPairing);
    //     }
    //     else if(txMode == _TRANSMISSION_MODE_11_)
    //     {
    //       fSearchSemiOLMUMIMOPairing(MUMIMOPairingOptions,tUEInfoPerUEs,mCodeBook,eNBAntennaCount,nUserPairing);
    //     }
    //     else if(txMode == _TRANSMISSION_MODE_NR_ )
    //     {
    //       fSearchMUMIMOPairingForNR(MUMIMOPairingOptions,tUEInfoPerUEs,mCodeBook,eNBAntennaCount);
    //     }
    //     else
    //     {
    //       cout<<"MU-MIMO is enabled "<<endl;
    //       cout<<"Error in TX Mode. Please set either TM5 or TM9 or TM11 or TMNR for using MU-MIMO"<<endl;
    //       abort();
    //     }
    //   }
    //   else
    //   {
    if(txMode == _TRANSMISSION_MODE_NR_ )
    {
        fSearchMUMIMOPairingForNR(MUMIMOPairingOptions,tUEInfoPerUEs,mCodeBook,eNBAntennaCount,pairedUEs,SINRInDBOfPairedUEs);
    }
    else
    {
        cout<<"MU-MIMO is enabled "<<endl;
        cout<<"Error in TX Mode. Please set TMNR for using MU-MIMO"<<endl;
        abort();
    }
    //   }

    //MUMIMOPairingOptions.set_size(0);
}

void Scheduler::fRemoveInvalidUEsForMUMIMO(Array<UEInfo_S>& tUEInfoPerUEs, ivec invalidUEsForMUMIMO, TransmissionMode_E txMode)
{  
    ivec IndicesToRemove(0);
    for(int ue_cnt = 0; ue_cnt < tUEInfoPerUEs.size(); ue_cnt++)
    {
        if(find(invalidUEsForMUMIMO,tUEInfoPerUEs(ue_cnt).UEid) != -1 || tUEInfoPerUEs(ue_cnt).txMode != txMode)
        {
            append(IndicesToRemove,ue_cnt);
        }
    }

    deleteInArray(tUEInfoPerUEs,IndicesToRemove);
}

// void Scheduler::fSearchMUMIMOPairingForTM5(Array<MUMIMOPairingInfo_S>& MUMIMOPairingOptions, Array<UEInfo_S>& tUEInfoPerUEs, Codebook& mCodeBook, int txAntennaCount, int nUserPairing)
// {
//   Array<cmat> nCodebookMatrix = mCodeBook.getCodebook(txAntennaCount,1,1);
//   
//   nUserPairing = 2; // For TM5, Number of UEs paired is hardcoded to 2
//   
//   for(int ue_cnt = 0; ue_cnt < tUEInfoPerUEs.size(); ue_cnt++)
//   {
//     for(int ue_cnt1 = 0; ue_cnt1 < tUEInfoPerUEs.size(); ue_cnt1++)
//     {
//       if(ue_cnt!=ue_cnt1)
//       {
// 	bool isRepeat = false;
// 	MUMIMOPairingInfo_S tMUMIMOPairingInfo;
// 	tMUMIMOPairingInfo.mUEIds.set_size(nUserPairing);
// 	tMUMIMOPairingInfo.SINRoffsetInDB.set_size(nUserPairing);
// 	tMUMIMOPairingInfo.mUEIds(0) = tUEInfoPerUEs(ue_cnt).UEid;
// 	tMUMIMOPairingInfo.mUEIds(1) = tUEInfoPerUEs(ue_cnt1).UEid;
// 	
// 	for(int cnt = 0; cnt < MUMIMOPairingOptions.length(); cnt++)
// 	{
// 	  if( tMUMIMOPairingInfo.mUEIds == MUMIMOPairingOptions(cnt).mUEIds || (tUEInfoPerUEs(ue_cnt).UEid == MUMIMOPairingOptions(cnt).mUEIds(1) && tUEInfoPerUEs(ue_cnt1).UEid == MUMIMOPairingOptions(cnt).mUEIds(0)))
// 	    isRepeat = true;
// 	}
// 	if(!isRepeat)
// 	{
// 	  if(abs((nCodebookMatrix(tUEInfoPerUEs(ue_cnt).pmi(0)).hermitian_transpose()*nCodebookMatrix(tUEInfoPerUEs(ue_cnt1).pmi(0))).get(0))<1e-10)
// 	  {
// 	    tMUMIMOPairingInfo.SINRoffsetInDB(0) = vec("-6.0");
// 	    tMUMIMOPairingInfo.SINRoffsetInDB(1) = vec("-6.0");
// 	    append(MUMIMOPairingOptions,tMUMIMOPairingInfo);
// 	  }
// 	}
//       }
//     }
//     
//   }
//   
// }
// 
// void Scheduler::fSearchMUMIMOPairingForTM9(Array<MUMIMOPairingInfo_S>& MUMIMOPairingOptions, Array<UEInfo_S>& tUEInfoPerUEs, Codebook& mCodeBook, int txAntennaCount, int nUserPairing)
// {
//   //This function is a placeholder for MU-MIMO User pairing for TM9
//   
//   switch(nUserPairing)
//   {
//     case 1:
//     {
//       MUMIMOPairingOptions.set_size(0); // When the nUserPairing==1, pairing is not done
//       break;
//     }
//     case 2:
//     {
//       fRemoveRank2UEs(tUEInfoPerUEs);
//       Array<cmat> nCodebookMatrix = mCodeBook.getCodebook(txAntennaCount,1,1); 
//       for(int ue_cnt = 0; ue_cnt < tUEInfoPerUEs.size(); ue_cnt++)
//       {
// 	for(int ue_cnt1 = 0; ue_cnt1 < tUEInfoPerUEs.size(); ue_cnt1++)
// 	{
// 	  int Diff = abs(tUEInfoPerUEs(ue_cnt).i1 - tUEInfoPerUEs(ue_cnt1).i1);
// 	  int i1Offset;
// 	  if(Diff > 8)
// 	    i1Offset = 8 - (Diff % 8);
// 	  else
// 	    i1Offset = Diff;
// 	  int cqi1 = tUEInfoPerUEs(ue_cnt).cqi(0)(0);
// 	  int cqi2 = tUEInfoPerUEs(ue_cnt1).cqi(0)(0);
// 	  if(ue_cnt != ue_cnt1 && (i1Offset >= 4 || abs((nCodebookMatrix(tUEInfoPerUEs(ue_cnt).pmi(0)).hermitian_transpose()*nCodebookMatrix(tUEInfoPerUEs(ue_cnt1).pmi(0))).get(0))<1e-10) && cqi1 > 3 && cqi2 > 3 && cqi1 < 13 && cqi2 < 13)
// 	  {
// 	    bool isRepeat = false;
// 	    MUMIMOPairingInfo_S tMUMIMOPairingInfo;
// 	    tMUMIMOPairingInfo.mUEIds.set_size(nUserPairing);
// 	    tMUMIMOPairingInfo.SINRoffsetInDB.set_size(nUserPairing);
// 	    tMUMIMOPairingInfo.mUEIds(0) = tUEInfoPerUEs(ue_cnt).UEid;
// 	    tMUMIMOPairingInfo.mUEIds(1) = tUEInfoPerUEs(ue_cnt1).UEid;
// 	    
// 	    for(int cnt = 0; cnt < MUMIMOPairingOptions.length(); cnt++)
// 	    {
// 	      if( tMUMIMOPairingInfo.mUEIds == MUMIMOPairingOptions(cnt).mUEIds || (tUEInfoPerUEs(ue_cnt).UEid == MUMIMOPairingOptions(cnt).mUEIds(1) && tUEInfoPerUEs(ue_cnt1).UEid == MUMIMOPairingOptions(cnt).mUEIds(0)))
// 		isRepeat = true;
// 	    }
// 	    
// 	    if(!isRepeat)
// 	    {
// 	      switch(i1Offset)
// 	      {
// 		case 4:
// 		case 5:
// 		  tMUMIMOPairingInfo.SINRoffsetInDB(0) = vec("-7.5");
// 		  tMUMIMOPairingInfo.SINRoffsetInDB(1) = vec("-7.5");
// 		  break;
// 		case 6:
// 		case 7:
// 		  tMUMIMOPairingInfo.SINRoffsetInDB(0) = vec("-6.0");
// 		  tMUMIMOPairingInfo.SINRoffsetInDB(1) = vec("-6.0");
// 		  break;
// 		case 8:
// 		  tMUMIMOPairingInfo.SINRoffsetInDB(0) = vec("-4.5");
// 		  tMUMIMOPairingInfo.SINRoffsetInDB(1) = vec("-4.5");
// 		  break;
// 		default:
// 		  tMUMIMOPairingInfo.SINRoffsetInDB(0) = vec("-4.5");
// 		  tMUMIMOPairingInfo.SINRoffsetInDB(1) = vec("-4.5");
// 	      }
// 	      
// 	      append(MUMIMOPairingOptions,tMUMIMOPairingInfo);
// 	    }
// 	  }
// 	}
//       }
//       break;
//     } 
// 		case 4:
// 		{  
// 		  // Sample algorithm to pick 4 UEs randmonly. This section can be replaced with proprietary algorithm   
// 		  int noOfUEs=tUEInfoPerUEs.size();
// 		  if(noOfUEs < 4)
// 		  {
// 		    MUMIMOPairingOptions.set_size(0);
// 		    break;
// 		  }
// 		  ivec UEIndicesForPairing;
// 		  MUMIMOPairingInfo_S tMUMIMOPairingInfo;
// 		  tMUMIMOPairingInfo.mUEIds.set_size(nUserPairing);
// 		  tMUMIMOPairingInfo.SINRoffsetInDB.set_size(nUserPairing);
// 		  UEIndicesForPairing.set_size(0);
// 		  
// 		  while(UEIndicesForPairing.size() != nUserPairing)
// 		  {  
// 		    int UEIndexToChoose = rand() % noOfUEs;
// 		    append(UEIndicesForPairing,UEIndexToChoose); 
// 		    UEIndicesForPairing = findUniqueNumbers(UEIndicesForPairing);
// 		  }
// 		  
// 		  int nLayersUEs = tUEInfoPerUEs(UEIndicesForPairing(0)).nLayers; // Save first UE's nLayers for checking with other UEs
// 		  
// 		  for(int i = 0; i < nUserPairing; i++)
// 		  {
// 		    if(nLayersUEs == tUEInfoPerUEs(UEIndicesForPairing(i)).nLayers)
// 		    {
// 		      tMUMIMOPairingInfo.mUEIds(i) = tUEInfoPerUEs(UEIndicesForPairing(i)).UEid;
// 		    }
// 		  }
// 		  
// 		  if(tMUMIMOPairingInfo.mUEIds.size() == 4)
// 		    append(MUMIMOPairingOptions,tMUMIMOPairingInfo);
// 		  else
// 		    MUMIMOPairingOptions.set_size(0);
// 		  
// 		  break;
// 		} 
// 		default:
// 		{  
// 		  MUMIMOPairingOptions.set_size(0);
// 		  break;
// 		} 
//   }
//   
// }
bool isValidPair(Array<ivec> &pairedUEs, ivec newPair, int nUserPairing, PairingInfo_S &pairingInfo,int dmrsType,int maxLength)
{
    int nPositions=nUserPairing; // Considering only one CDM per UE . Need to change in future
    if(pairedUEs.length())
    {
        ivec ueIDs = joinArrayToVector(pairedUEs);
        append(ueIDs,newPair);
        ueIDs = findUniqueNumbers(ueIDs);
        Array<ivec> scheduledRBGsPerUE(ueIDs.length());
        for(int cnt=0;cnt<ueIDs.length();cnt++)
            scheduledRBGsPerUE(cnt).set_length(0);

        int rbg_cnt=0;
        for(;rbg_cnt<pairedUEs.length();rbg_cnt++)
        {
            ivec ueIndices=find(ueIDs,pairedUEs(rbg_cnt));
            for(int ue_cnt=0;ue_cnt<pairedUEs(rbg_cnt).length();ue_cnt++)
                append(scheduledRBGsPerUE(ueIndices(ue_cnt)),rbg_cnt);
        }
        ivec ueIndices=find(ueIDs,newPair);
        for(int ue_cnt=0;ue_cnt<newPair.length();ue_cnt++)
            append(scheduledRBGsPerUE(ueIndices(ue_cnt)),rbg_cnt); // setting new pair in last RBG

        ivec rbgNumbers=getIntegers(0,pairedUEs.length());
        for(int ueStart=0;ueStart<ueIDs.length();ueStart++)
        {
            ivec pairingPositionPerUE=zeros_i(ueIDs.length())-1;
            imat pairingPositionMatrix=zeros_i(rbgNumbers.length(),nPositions)-1;
            for(int ue_cnt=0;ue_cnt<ueIDs.length();ue_cnt++)
            {
                int ueIndx=(ue_cnt+ueStart)%ueIDs.length();
                for(int pos_cnt=0;pos_cnt<nPositions;pos_cnt++)
                {
                    ivec pairingPositionCol=pairingPositionMatrix.get_col(pos_cnt);
                    ivec pairingPositions=pairingPositionCol(scheduledRBGsPerUE(ueIndx));
                    if(findNot(pairingPositions,-1).length()) 
                        continue;
                    else
                    {
                        setInVec(pairingPositionCol,scheduledRBGsPerUE(ueIndx),ueIDs(ueIndx));
                        pairingPositionMatrix.set_col(pos_cnt,pairingPositionCol);
                        pairingPositionPerUE(ueIndx)=pos_cnt;
                        break;
                    }
                }
                if(pairingPositionPerUE(ueIndx)==-1) break;
            }

            if(find(pairingPositionPerUE,-1)==-1)
            {
                pairingInfo.ueIDs=ueIDs;
                pairingInfo.pairingPositionPerUE=pairingPositionPerUE;
                return true;
            }
        }
        return false;
    }
    else
    {
        pairingInfo.ueIDs=newPair;
        pairingInfo.pairingPositionPerUE=getIntegers(0,newPair.length()-1);
        return true;
    }
}

bool isValidPair(Array< ivec >& pairedUEs, ivec newPair, int nUserPairing, PairingInfo_S& pairingInfo,int dmrsType,int maxLength,ivec nLayersforNewPair,Array<ivec>& nLayersPerPairedUEs){

    if(sum(nLayersforNewPair>2*maxLength) && nLayersforNewPair.length()>1)
    {
        cout<<"Rank of UE must not exceed "<<2*maxLength<<" for dmrsType "<<dmrsType<<" with maxLength "<<maxLength<<" for MU-MIMO pairing."<<endl;abort();
    }
    return isValidPair(pairedUEs,newPair,nUserPairing,pairingInfo,dmrsType,maxLength);
}

Array<cmat> getZFMuMimoPrecoders(Array<cmat>& precodersOfComb, bool &isZFpossible)
{
    int nUsers = precodersOfComb.length();
    Array<cmat> modifiedPrecoders(nUsers);
    ivec ranksOfUsers(nUsers);
    cmat Q=precodersOfComb(0);
    for(int usrCnt=0;usrCnt<nUsers;usrCnt++)
    {
        ranksOfUsers(usrCnt) = precodersOfComb(usrCnt).cols();
        modifiedPrecoders(usrCnt).set_size(Q.rows(),ranksOfUsers(usrCnt));
        if(usrCnt>0)
            Q = concat_horizontal(Q,precodersOfComb(usrCnt));
    }
    //     cout<<"Q: "<<Q<<endl;
    cmat G;
    if(abs(det(Q.H()*Q))>1e-2)
    {
        G = Q*inv(Q.H()*Q);
        isZFpossible = true;
    }
    else
    {
        modifiedPrecoders.set_length(0);
        isZFpossible = false;
    }

    int usrCnt=0,sumRank=ranksOfUsers(0), localCol=0;

    // separating individual modified precoders from G
    for(int colCnt=0;colCnt<G.cols();colCnt++)
    {
        if(colCnt>sumRank-1)
        {
            usrCnt++;
            sumRank += ranksOfUsers(usrCnt);
            localCol = 0;
        }
        // Normalization and scaling for rank
        modifiedPrecoders(usrCnt).set_col(localCol, (1/(norm(G.get_col(colCnt)*sqrt(ranksOfUsers(usrCnt))))*G.get_col(colCnt)));
        localCol++;
    }
    return modifiedPrecoders;
}

void computeSinrOffsetForMUMIMOPair(MUMIMOPairingInfo_S& MUMIMOPair,Array< UEInfo_S >& tUEInfoPerUEs,dComplex txSubcarrierPower)
{
    int nPairedUEs = MUMIMOPair.mUEIds.length();
    MUMIMOPair.expectedSINRInDB.set_length(nPairedUEs);
    MUMIMOPair.modifiedMuMimoPrecoders.set_length(nPairedUEs);
    MUMIMOPair.SINRoffsetInDB.set_length(nPairedUEs);

    int nCDM = nPairedUEs;

    for(int ue_cnt=0; ue_cnt<nPairedUEs; ue_cnt++)
    {
        vec sinrOffsetInDB;
        vec oldPPSinr=inv_dB(tUEInfoPerUEs(ue_cnt).sinrInDB(0));
        if(nPairedUEs>1)
        {
            Array<cmat> sbChannels;
            //               if(tUEInfoPerUEs(pairedUEIndices(ue_cnt)).sbChannel.length()>10)
            //                   sbChannels=randPick(tUEInfoPerUEs(pairedUEIndices(ue_cnt)).sbChannel,10);
            //               else
            sbChannels=tUEInfoPerUEs(ue_cnt).sbChannel;

            cmat P=tUEInfoPerUEs(ue_cnt).precoder(0);
            complex< double > additionalScaling=complex< double >(sqrt(1.0/(double)nCDM),0.0);

            int nR=sbChannels(0).rows();
            int nT=sbChannels(0).cols();
            cmat avgH=zeros_c(nR,nT);
            cmat avgAdditonalIntrCov=zeros_c(nR,nR);
            for(int cnt=0; cnt<sbChannels.length(); cnt++)
            {

                cmat H=sbChannels(cnt);
                avgH+=H;
                for(int others_cnt=0; others_cnt<nPairedUEs; others_cnt++)
                {
                    if(ue_cnt!=others_cnt)
                    {
                        cmat Pi=tUEInfoPerUEs(others_cnt).precoder(0);
                        cmat HPi=H*Pi*additionalScaling*txSubcarrierPower;
                        avgAdditonalIntrCov+=HPi*HPi.H();
                    }
                }
            }
            avgAdditonalIntrCov/=(double)sbChannels.length();
            avgH/=(double)sbChannels.length();
            sinrOffsetInDB=getSINROffsetInDBwithMMSEperCodeword(oldPPSinr,avgH*P*txSubcarrierPower,avgAdditonalIntrCov,additionalScaling);
        }
        else
            sinrOffsetInDB = zeros(oldPPSinr.length());

        MUMIMOPair.modifiedMuMimoPrecoders(ue_cnt) = tUEInfoPerUEs(ue_cnt).precoder(0);
        MUMIMOPair.SINRoffsetInDB(ue_cnt) = sinrOffsetInDB; 
        MUMIMOPair.expectedSINRInDB(ue_cnt) = dB(oldPPSinr)+sinrOffsetInDB;
    }
}

void computeSinrOffsetForMUMIMOPairWithZeroForcing(MUMIMOPairingInfo_S& MUMIMOPair,Array< UEInfo_S >& tUEInfoPerUEs,dComplex txSubcarrierPower)
{
    int nPairedUEs = MUMIMOPair.mUEIds.length();

    MUMIMOPair.expectedSINRInDB.set_length(nPairedUEs);
    MUMIMOPair.modifiedMuMimoPrecoders.set_length(nPairedUEs);
    MUMIMOPair.SINRoffsetInDB.set_length(nPairedUEs);

    Array<cmat> precodersOfComb(nPairedUEs);
    // collecting precoders of all UEs in this combination
    for(int tmpCnt=0; tmpCnt<nPairedUEs; tmpCnt++)
    {
        precodersOfComb(tmpCnt) = tUEInfoPerUEs(tmpCnt).precoder(0);
    }
    bool isZFpossible;
    Array<cmat> ZFprecoders=getZFMuMimoPrecoders(precodersOfComb,isZFpossible);// = getZFMuMimoPrecoders(precodersOfComb);
    for(int ue_cnt=0; ue_cnt<nPairedUEs; ue_cnt++)
    {
        vec sinrOffsetInDB;
        cmat modifiedPrecoder;
        vec oldPPSinr=inv_dB(tUEInfoPerUEs(ue_cnt).sinrInDB(0));
        if(nPairedUEs>1)
        {
            if(isZFpossible)
            {
                int nCodeWords=oldPPSinr.length();
                modifiedPrecoder = ZFprecoders(ue_cnt);

                cmat P=tUEInfoPerUEs(ue_cnt).precoder(0);
                sinrOffsetInDB.set_length(nCodeWords);
                for(int cwdCnt=0;cwdCnt<nCodeWords;cwdCnt++)
                {
                    cmat VherP = modifiedPrecoder.H()*P;
                    vec sinrOffsetPerlayer = sqr(abs(diag(VherP)));
                    sinrOffsetInDB(cwdCnt) = dB(min(sinrOffsetPerlayer));
                }
            }
            else
            {
                // ZF is not possible, so eliminating this combination by choosing large SINR offset
                int nCodeWords=oldPPSinr.length();
                modifiedPrecoder = tUEInfoPerUEs(ue_cnt).precoder(0);
                sinrOffsetInDB = ones(nCodeWords)*(-100);  
            }

        }
        else
        {
            sinrOffsetInDB = zeros(oldPPSinr.length());
            modifiedPrecoder = tUEInfoPerUEs(ue_cnt).precoder(0);
        }

        MUMIMOPair.modifiedMuMimoPrecoders(ue_cnt) = modifiedPrecoder;
        MUMIMOPair.SINRoffsetInDB(ue_cnt) = sinrOffsetInDB; 
        MUMIMOPair.expectedSINRInDB(ue_cnt) = dB(oldPPSinr)+sinrOffsetInDB;
    }
}

void computeSinrOffsetForMUMIMOPairWithSuccessiveMMSE(MUMIMOPairingInfo_S& MUMIMOPair,Array< UEInfo_S >& tUEInfoPerUEs,dComplex txSubcarrierPower)
{
    int nPairedUEs = MUMIMOPair.mUEIds.length();

    MUMIMOPair.expectedSINRInDB.set_length(nPairedUEs);
    MUMIMOPair.modifiedMuMimoPrecoders.set_length(nPairedUEs);
    MUMIMOPair.SINRoffsetInDB.set_length(nPairedUEs);

    int nCDM = nPairedUEs;
    Array<cmat> precodersOfComb(nPairedUEs);
    Array<vec> oldPPSINRsSfComb(nPairedUEs);
    // collecting precoders of all UEs in this combination
    for(int tmpCnt=0; tmpCnt<nPairedUEs; tmpCnt++)
    {
        precodersOfComb(tmpCnt) = tUEInfoPerUEs(tmpCnt).precoder(0);
        oldPPSINRsSfComb(tmpCnt) = inv_dB(tUEInfoPerUEs(tmpCnt).sinrInDB(0));
    }
    for(int ue_cnt=0; ue_cnt<nPairedUEs; ue_cnt++)
    {
        cmat modifiedPrecoder;

        Array<vec> modifiedSINR;
        vec oldPPSinr=inv_dB(tUEInfoPerUEs(ue_cnt).sinrInDB(0));
        //                   cout<< "Reported SINR (linear scale) : "<< oldPPSinr<<endl;

        if(nPairedUEs>1)
        {
            cmat P=tUEInfoPerUEs(ue_cnt).precoder(0);
            modifiedSINR = getModifiedSINRInDBperCodeword_SMMSE(P,precodersOfComb, oldPPSINRsSfComb,ue_cnt,modifiedPrecoder);
        }
        else
            modifiedPrecoder = tUEInfoPerUEs(ue_cnt).precoder(0);
        MUMIMOPair.modifiedMuMimoPrecoders(ue_cnt) = modifiedPrecoder;
    }

    for(int ue_cnt=0; ue_cnt<nPairedUEs; ue_cnt++)
    {
        vec oldPPSinr=inv_dB(tUEInfoPerUEs(ue_cnt).sinrInDB(0));
        vec sinrOffsetInDB(oldPPSinr.length());
        if(nPairedUEs>1)
        {
            Array<cmat> sbChannels;
            sbChannels=tUEInfoPerUEs(ue_cnt).sbChannel;
            cmat MuMimoPrecoder=MUMIMOPair.modifiedMuMimoPrecoders(ue_cnt);
            dComplex additionalScaling = dComplex(sqrt(1.0/(double)nCDM),0.0);

            int nR=sbChannels(0).rows();
            int nT=sbChannels(0).cols();
            cmat avgH=zeros_c(nR,nT);
            cmat avgAdditonalIntrCov=zeros_c(nR,nR);
            for(int cnt=0; cnt<sbChannels.length(); cnt++)
            {

                cmat H=sbChannels(cnt);
                avgH+=H;
                for(int others_cnt=0; others_cnt<nPairedUEs; others_cnt++)
                {
                    if(ue_cnt!=others_cnt)
                    {
                        cmat Pi=MUMIMOPair.modifiedMuMimoPrecoders(others_cnt);
                        cmat HPi=H*Pi*additionalScaling*txSubcarrierPower;
                        avgAdditonalIntrCov+=HPi*HPi.H();
                    }
                }
            }
            avgAdditonalIntrCov/=(double)sbChannels.length();
            avgH/=(double)sbChannels.length();
            sinrOffsetInDB=getSINROffsetInDBwithMMSEperCodeword(oldPPSinr,avgH*MuMimoPrecoder*txSubcarrierPower,avgAdditonalIntrCov,additionalScaling);
        }
        else
        {
            sinrOffsetInDB = zeros(oldPPSinr.length());
        }
        MUMIMOPair.SINRoffsetInDB(ue_cnt) = sinrOffsetInDB;
        MUMIMOPair.expectedSINRInDB(ue_cnt) = dB(oldPPSinr)+sinrOffsetInDB;
    }
}

void Scheduler::fFindMUMIMOPairingOptionsForNR(Array<MUMIMOPairingInfo_S>& MUMIMOPairingOptions,Array< UEInfo_S >& tUEInfoPerUEs, Array<ivec> &pairedUEs)
{
    MUMIMOPairingOptions.set_length(0);

    int nUEs = tUEInfoPerUEs.size();
    if(nUEs==0) return;

    ivec ueIDs(nUEs);
    ivec nLayersPerUE(nUEs);
    Array<vec> reportedSINRPerUE(nUEs);

    int maxUEPairs=nUserPairing;
    for(int ue_cnt = 0; ue_cnt < nUEs; ue_cnt++)
    {
        ueIDs(ue_cnt)=tUEInfoPerUEs(ue_cnt).UEid;
        nLayersPerUE(ue_cnt)=tUEInfoPerUEs(ue_cnt).nLayers;

        reportedSINRPerUE(ue_cnt)=tUEInfoPerUEs(ue_cnt).sinrInDB(0);    // Initializing MU-MIMO precoder variable of each UE
    }

    bmat pairedUECombinations=getAllBinaryCombinations(ueIDs.length(),maxUEPairs);
    int nCombinations=pairedUECombinations.rows();

    MUMIMOPairingInfo_S MUMIMOPair;

    for(int comb_cnt=0; comb_cnt<nCombinations; comb_cnt++)
    {
        ivec pairedUEIndices=find(pairedUECombinations.get_row(comb_cnt));
        int nPairedUEs=pairedUEIndices.length();
        if(mForceMUMIMO && nPairedUEs==1)
            continue;       //Skipping the computations for single users.

        if(isValidPair(pairedUEs,ueIDs(pairedUEIndices),nUserPairing,MUMIMOPair.pairingInfo,dmrsType,dmrsLength))
        {
            MUMIMOPair.mUEIds = ueIDs(pairedUEIndices);
            MUMIMOPair.nLayers = nLayersPerUE(pairedUEIndices);

            dComplex txSubcarrierPower=dComplex(sqrt(inv_dB(eNodeBTxSCPowerIndBm)),0.0);

            Array<UEInfo_S> pairedUEInfo(pairedUEIndices.length());
            for(int cnt=0;cnt<pairedUEIndices.length();cnt++)
                pairedUEInfo(cnt) = tUEInfoPerUEs(pairedUEIndices(cnt));

            switch(pairingMethod)
            {
                case _IDEAL_PAIRING_: 
                    computeSinrOffsetForMUMIMOPair(MUMIMOPair,pairedUEInfo,txSubcarrierPower);
                    break;
                case _ZERO_FORCING_:
                    computeSinrOffsetForMUMIMOPairWithZeroForcing(MUMIMOPair,pairedUEInfo,txSubcarrierPower);
                    break;
                case _SUCCESSIVE_MMSE_:
                    computeSinrOffsetForMUMIMOPairWithSuccessiveMMSE(MUMIMOPair,pairedUEInfo,txSubcarrierPower);
                    break;
                case _BLOCK_DIAGONALISATION_:
                    cout<<"Block Diagonalisation is yet to be implemented..."<<endl;abort();
                    break;
                default:
                    cout<<"Invalid Pairing Method."<<endl;abort();
                    break;
            }
            double capacity=sum(log2(1+inv_dB(joinArrayToVector(MUMIMOPair.expectedSINRInDB))));
            if(capacity>0)
                append(MUMIMOPairingOptions,MUMIMOPair);
        }
    }
}

void Scheduler::fSearchMUMIMOPairingForNR(Array< MUMIMOPairingInfo_S >& MUMIMOPairingOptions, Array< UEInfo_S >& tUEInfoPerUEs, Codebook* mCodeBook, int eNBAntennaCount, Array<ivec> &pairedUEs, Array<vec> &SINRInDBOfPairedUEs)
{
    MUMIMOPairingOptions.set_length(0);

    //     #ifdef MUMIMO_DEBUG
    //     cout<<"searching MU-MIMO pairs "<<endl;
    //     #endif
    int nUEs = tUEInfoPerUEs.size();

    ivec ueIDs(nUEs);
    ivec nLayersPerUE(nUEs);
    int maxLayers=(eNBAntennaCount>12)?12:eNBAntennaCount;
    Array<vec> reportedSINRPerUE(nUEs);
    vec firstSINRPerUE(nUEs);

    int maxUEPairs=nUserPairing;
    int nCombinations=sumNCr(ueIDs.length(),maxUEPairs);
    for(int ue_cnt = 0; ue_cnt < nUEs; ue_cnt++)
    {
        ueIDs(ue_cnt)=tUEInfoPerUEs(ue_cnt).UEid;
        nLayersPerUE(ue_cnt)=tUEInfoPerUEs(ue_cnt).nLayers;

        reportedSINRPerUE(ue_cnt)=tUEInfoPerUEs(ue_cnt).sinrInDB(0);
        firstSINRPerUE(ue_cnt)=tUEInfoPerUEs(ue_cnt).sinrInDB(0)(0);
        // Initializing MU-MIMO precoder variable of each UE
    }

    if(firstSINRPerUE.length() == 0)
        return;

    Array<UEInfo_S> pairedUEInfo=tUEInfoPerUEs;
    vec capacity=zeros(nCombinations);
    bvec singleUEComb=zeros_b(nCombinations);
    Array<Array<cmat>> MuMimoPrecodersPerCombination(nCombinations);
    Array<vec> sinrOffsetinDBPerCombination(nCombinations);
    Array<vec> expectedSINRinDBPerCombination(nCombinations);

    bmat pairedUECombinations=getAllBinaryCombinations(ueIDs.length(),maxUEPairs);

    ivec decEquivalents(nCombinations);

    ivec alreadyPairedUEs(0);
    Array<vec> SINRofAlreadyPairedUEs(0);
    for(int cnt=0; cnt<pairedUEs.length(); cnt++)
    {
        for(int cnt1=0; cnt1<pairedUEs(cnt).length(); cnt1++)
        {
            int indx=find(alreadyPairedUEs,pairedUEs(cnt)(cnt1));
            if(indx==-1)
            {
                append(alreadyPairedUEs,pairedUEs(cnt)(cnt1));
                append(SINRofAlreadyPairedUEs,to_vec(SINRInDBOfPairedUEs(cnt)(cnt1)));
            }
            else
                append(SINRofAlreadyPairedUEs(indx),SINRInDBOfPairedUEs(cnt)(cnt1));
        }
    }
    for(int comb_cnt=(nCombinations-1); comb_cnt>=0; comb_cnt--)
    {
        bvec temp= pairedUECombinations.get_row(comb_cnt);
        decEquivalents(comb_cnt)=bin2dec(temp);
    }
    bvec validPair=zeros_b(nCombinations);
    Array<PairingInfo_S> pairingInfoPerComb(nCombinations);
    for(int comb_cnt=(nCombinations-1); comb_cnt>=0; comb_cnt--)
    {
        if(validPair(comb_cnt)==1)
            continue;
        else
        {
            ivec pairedUEIndices=find(pairedUECombinations.get_row(comb_cnt));
            PairingInfo_S pairingInfo;
            //       vec sinrPerPairedUEs=firstSINRPerUE(pairedUEIndices);
            //
            //       ivec alreadyPairedIndices=find(alreadyPairedUEs,ueIDs(pairedUEIndices));
            //       bool skipComb=false;
            //       for(int cnt=0;cnt<alreadyPairedIndices.length();cnt++)
            //       {
            //         if(alreadyPairedIndices(cnt)!=-1)
            //         {
            //             double minScheduledSINR=min(SINRofAlreadyPairedUEs(alreadyPairedIndices(cnt)));
            //             if(abs(sinrPerPairedUEs(cnt)-minScheduledSINR)>10.0)
            //                 skipComb=true;
            //         }
            //       }
            //       if(skipComb) continue;

            //       if(pairedUEIndices.length()>1 && find(sinrPerPairedUEs<5.0).length()) // not considering MUMIMO pairs with UE sinr<10
            //           continue;

            if(isValidPair(pairedUEs,ueIDs(pairedUEIndices),nUserPairing,pairingInfo,dmrsType,dmrsLength))
            {
                validPair(comb_cnt)=1;
                pairingInfoPerComb(comb_cnt)=pairingInfo;
            }
        }
    }

    for(int comb_cnt=nCombinations-1; comb_cnt>=0; comb_cnt--)
    {
        if(validPair(comb_cnt))
        {
            ivec pairedUEIndices=find(pairedUECombinations.get_row(comb_cnt));
            int nPairedUEs=pairedUEIndices.length();
            //          int nPairedLayers=sum(nLayersPerUE(pairedUEIndices));
            //           cout<< "\n Comb index: "<< comb_cnt<<endl;
            MuMimoPrecodersPerCombination(comb_cnt).set_size(0);
            sinrOffsetinDBPerCombination(comb_cnt).set_size(0);
            expectedSINRinDBPerCombination(comb_cnt).set_size(0);

            int nCDM=nPairedUEs; // This assumptions is not valid intra-CDM multiplexing of UEs - Dhiv

            complex<double> txSubcarrierPower=dComplex(sqrt(inv_dB(eNodeBTxSCPowerIndBm)),0.0);
            Array<cmat> precodersOfComb(nPairedUEs);
            Array<vec> oldPPSINRsSfComb(nPairedUEs);
            // collecting precoders of all UEs in this combination
            for(int tmpCnt=0; tmpCnt<nPairedUEs; tmpCnt++)
            {
                precodersOfComb(tmpCnt) = pairedUEInfo(pairedUEIndices(tmpCnt)).precoder(0);
                oldPPSINRsSfComb(tmpCnt) = inv_dB(reportedSINRPerUE(pairedUEIndices(tmpCnt)));
            }
            bool isZFpossible;
            Array<cmat> ZFprecoders=getZFMuMimoPrecoders(precodersOfComb,isZFpossible);// = getZFMuMimoPrecoders(precodersOfComb);

            for(int pair_cnt=0; pair_cnt<nPairedUEs; pair_cnt++)
            {
                //               cout<<"\n UE Id: "<< pairedUEInfo(pairedUEIndices(pair_cnt)).UEid<<endl;
                if(pairingMethod == _ZERO_FORCING_)
                {
                    vec sinrOffsetInDB;
                    cmat modifiedPrecoder;
                    vec oldPPSinr=inv_dB(reportedSINRPerUE(pairedUEIndices(pair_cnt)));
                    if(nPairedUEs>1)
                    {
                        if(isZFpossible)
                        {
                            int nCodeWords=oldPPSinr.length();
                            modifiedPrecoder = ZFprecoders(pair_cnt);
                            cmat P=pairedUEInfo(pairedUEIndices(pair_cnt)).precoder(0);
                            sinrOffsetInDB.set_length(nCodeWords);
                            for(int cwdCnt=0;cwdCnt<nCodeWords;cwdCnt++)
                            {
                                cmat VherP = modifiedPrecoder.H()*P;
                                vec sinrOffsetPerlayer = sqr(abs(diag(VherP)));
                                sinrOffsetInDB(cwdCnt) = dB(min(sinrOffsetPerlayer));
                            }
                        }
                        else
                        {
                            // ZF is not possible, so eliminating this combination by choosing large SINR offset
                            int nCodeWords=oldPPSinr.length();
                            modifiedPrecoder = pairedUEInfo(pairedUEIndices(pair_cnt)).precoder(0);
                            sinrOffsetInDB = ones(nCodeWords)*(-100);  
                        }
                    }
                    else
                    {
                        sinrOffsetInDB = zeros(oldPPSinr.length());
                        modifiedPrecoder = pairedUEInfo(pairedUEIndices(pair_cnt)).precoder(0);
                    }
                    append(MuMimoPrecodersPerCombination(comb_cnt),modifiedPrecoder);
                    append(sinrOffsetinDBPerCombination(comb_cnt),sinrOffsetInDB);
                    append(expectedSINRinDBPerCombination(comb_cnt),dB(oldPPSinr)+sinrOffsetInDB);
                }
                else if(pairingMethod == _SUCCESSIVE_MMSE_)
                {
                    int nCodeWords=1; // Modify for layers more than 4
                    cmat modifiedPrecoder;

                    vec sinrOffsetInDB(nCodeWords);
                    Array<vec> modifiedSINR;
                    vec avgModifiedSINR(nCodeWords);
                    vec oldPPSinr=inv_dB(reportedSINRPerUE(pairedUEIndices(pair_cnt)));
                    //                   cout<< "Reported SINR (linear scale) : "<< oldPPSinr<<endl;

                    if(nPairedUEs>1)
                    {
                        cmat P=pairedUEInfo(pairedUEIndices(pair_cnt)).precoder(0);
                        modifiedSINR = getModifiedSINRInDBperCodeword_SMMSE(P,precodersOfComb, oldPPSINRsSfComb,pair_cnt,modifiedPrecoder);
                    }
                    else
                        modifiedPrecoder = pairedUEInfo(pairedUEIndices(pair_cnt)).precoder(0);
                    append(MuMimoPrecodersPerCombination(comb_cnt),modifiedPrecoder);
                }
                else if(pairingMethod == _BLOCK_DIAGONALISATION_)
                {
                    cout<<"Block Diag to be implemented"<<endl;
                    abort();
                }
                // old method
                else if(pairingMethod == _IDEAL_PAIRING_)
                {
                    vec sinrOffsetInDB;
                    vec oldPPSinr=inv_dB(reportedSINRPerUE(pairedUEIndices(pair_cnt)));
                    if(nPairedUEs>1)
                    {
                        Array<cmat> sbChannels;
                        //               if(pairedUEInfo(pairedUEIndices(pair_cnt)).sbChannel.length()>10)
                        //                   sbChannels=randPick(pairedUEInfo(pairedUEIndices(pair_cnt)).sbChannel,10);
                        //               else
                        sbChannels=pairedUEInfo(pairedUEIndices(pair_cnt)).sbChannel;
                        cmat P=pairedUEInfo(pairedUEIndices(pair_cnt)).precoder(0);
                        complex< double > additionalScaling=complex< double >(sqrt(1.0/(double)nCDM),0.0);
                        int nR=sbChannels(0).rows();
                        int nT=sbChannels(0).cols();
                        cmat avgH=zeros_c(nR,nT);
                        cmat avgAdditonalIntrCov=zeros_c(nR,nR);
                        for(int cnt=0; cnt<sbChannels.length(); cnt++)
                        {
                            cmat H=sbChannels(cnt);
                            avgH+=H;
                            for(int others_cnt=0; others_cnt<nPairedUEs; others_cnt++)
                            {
                                if(pair_cnt!=others_cnt)
                                {
                                    cmat Pi=pairedUEInfo(pairedUEIndices(others_cnt)).precoder(0);
                                    cmat HPi=H*Pi*additionalScaling*txSubcarrierPower;
                                    avgAdditonalIntrCov+=HPi*HPi.H();
                                }
                            }
                        }
                        avgAdditonalIntrCov/=(double)sbChannels.length();
                        avgH/=(double)sbChannels.length();
                        sinrOffsetInDB=getSINROffsetInDBwithMMSEperCodeword(oldPPSinr,avgH*P*txSubcarrierPower,avgAdditonalIntrCov,additionalScaling);
                    }
                    else
                        sinrOffsetInDB = zeros(oldPPSinr.length());
                    append(MuMimoPrecodersPerCombination(comb_cnt),pairedUEInfo(pairedUEIndices(pair_cnt)).precoder(0));
                    append(sinrOffsetinDBPerCombination(comb_cnt),sinrOffsetInDB);
                    append(expectedSINRinDBPerCombination(comb_cnt),dB(oldPPSinr)+sinrOffsetInDB);
                }
            }
            if(pairingMethod == _SUCCESSIVE_MMSE_)
            {
                int nCodeWords=1; // Modify for layers more than 4
                vec sinrOffsetInDB(nCodeWords);
                Array<vec> modifiedSINR;
                vec avgModifiedSINR(nCodeWords);
                for(int pair_cnt=0; pair_cnt<nPairedUEs; pair_cnt++)
                {
                    //                   cout<<"\n UE Id: "<< pairedUEInfo(pairedUEIndices(pair_cnt)).UEid<<endl;
                    vec oldPPSinr=inv_dB(reportedSINRPerUE(pairedUEIndices(pair_cnt)));
                    if(nPairedUEs>1)
                    {
                        Array<cmat> sbChannels;
                        sbChannels=pairedUEInfo(pairedUEIndices(pair_cnt)).sbChannel;
                        cmat MuMimoPrecoder=MuMimoPrecodersPerCombination(comb_cnt)((pair_cnt));
                        complex< double > additionalScaling=complex< double >(sqrt(1.0/(double)nCDM),0.0);
                        int nR=sbChannels(0).rows();
                        int nT=sbChannels(0).cols();
                        cmat avgH=zeros_c(nR,nT);
                        cmat avgAdditonalIntrCov=zeros_c(nR,nR);
                        for(int cnt=0; cnt<sbChannels.length(); cnt++)
                        {
                            cmat H=sbChannels(cnt);
                            avgH+=H;
                            for(int others_cnt=0; others_cnt<nPairedUEs; others_cnt++)
                            {
                                if(pair_cnt!=others_cnt)
                                {
                                    cmat Pi=MuMimoPrecodersPerCombination(comb_cnt)((others_cnt));
                                    cmat HPi=H*Pi*additionalScaling*txSubcarrierPower;
                                    avgAdditonalIntrCov+=HPi*HPi.H();
                                }
                            }
                        }
                        avgAdditonalIntrCov/=(double)sbChannels.length();
                        avgH/=(double)sbChannels.length();
                        sinrOffsetInDB=getSINROffsetInDBwithMMSEperCodeword(oldPPSinr,avgH*MuMimoPrecoder*txSubcarrierPower,avgAdditonalIntrCov,additionalScaling);
                    }
                    else
                    {
                        sinrOffsetInDB = zeros(oldPPSinr.length());
                    }
                    append(sinrOffsetinDBPerCombination(comb_cnt),sinrOffsetInDB);
                    append(expectedSINRinDBPerCombination(comb_cnt),dB(oldPPSinr)+sinrOffsetInDB);
                }
            }
            capacity(comb_cnt)=sum(log2(1+inv_dB(expectedSINRinDBPerCombination(comb_cnt))));
            if(nPairedUEs==1)
            {
                singleUEComb(comb_cnt)=(bin)1;
                if(mForceMUMIMO) // Making SU-MIMO combinations to have zero capacity - Dhiv
                    capacity(comb_cnt)=0;
            }
        }
    }
    //     #ifdef MUMIMO_DEBUG
    //     cout<<"nCombinations : "<<nCombinations<<endl;
    //     //   cout<<"capacity : "<<capacity<<endl;
    //     #endif
    //   ivec singleUEIndices=find(singleUEComb);
    //   double maxSingleUECapacity=max(capacity(singleUEIndices));
    //   ivec bestPairs=find(capacity>maxSingleUECapacity); // considering only pairing combinations greater then max single UE capacity
    //   append(bestPairs,singleUEIndices);

    ivec bestPairs=find(capacity>0);
    int nPairs = bestPairs.length();
    for(int pair_cnt = 0; pair_cnt < nPairs; pair_cnt++)
    {
        ivec pairedUEIndices=find(pairedUECombinations.get_row(bestPairs(pair_cnt)));
        int nPairedUEs=pairedUEIndices.length();
        MUMIMOPairingInfo_S MUMIMOPairingOption;
        MUMIMOPairingOption.mUEIds = ueIDs(pairedUEIndices);
        MUMIMOPairingOption.nLayers=nLayersPerUE(pairedUEIndices);
        MUMIMOPairingOption.modifiedMuMimoPrecoders.set_size(nPairedUEs);
        MUMIMOPairingOption.SINRoffsetInDB.set_size(nPairedUEs);
        MUMIMOPairingOption.expectedSINRInDB.set_size(nPairedUEs);
        MUMIMOPairingOption.pairingInfo=pairingInfoPerComb(bestPairs(pair_cnt));
        //     if(!isValidPair(pairedUEs,MUMIMOPairingOption.mUEIds,nUserPairing,MUMIMOPairingOption.pairingInfo))
        //     {
        //         cout<<"pairedUEs : "<<pairedUEs<<endl;
        //         cout<<"nUserPairing : "<<nUserPairing<<endl;
        //         cout<<"Pairing Error.. "<<endl;abort();
        //     }
        //
        int indx=0;
        bool skipflag=false;
        for(int ue_cnt=0; ue_cnt<nPairedUEs; ue_cnt++)
        {
            int nCodeWords=(MUMIMOPairingOption.nLayers(ue_cnt)>4)?2:1;
            MUMIMOPairingOption.modifiedMuMimoPrecoders(ue_cnt)=MuMimoPrecodersPerCombination(bestPairs(pair_cnt))(ue_cnt);
            MUMIMOPairingOption.SINRoffsetInDB(ue_cnt)=sinrOffsetinDBPerCombination(bestPairs(pair_cnt))(indx,indx+nCodeWords-1);
            MUMIMOPairingOption.expectedSINRInDB(ue_cnt)=expectedSINRinDBPerCombination(bestPairs(pair_cnt))(indx,indx+nCodeWords-1);
            indx+=nCodeWords;
            //       if(find(MUMIMOPairingOption.SINRoffsetInDB(ue_cnt)<-10.0).length())
            //           skipflag=true;
        }
        if(!skipflag)
            append(MUMIMOPairingOptions,MUMIMOPairingOption);
    }
}

// void Scheduler::fSearchMUMIMOPairingForNR(Array< MUMIMOPairingInfo_S >& MUMIMOPairingOptions, Array< UEInfo_S >& tUEInfoPerUEs, Codebook& mCodeBook, int eNBAntennaCount, Array<ivec> &pairedUEs, Array<vec> &SINRInDBOfPairedUEs)
// {
//   MUMIMOPairingOptions.set_length(0);
//   
// //   #ifdef MUMIMO_DEBUG
// //   cout<<"searching MU-MIMO pairs "<<endl;
// //   #endif
//   int nUEs = tUEInfoPerUEs.size();
//   
//   ivec ueIDs(nUEs);
//   ivec nLayersPerUE(nUEs);
//   Array<vec> reportedSINRPerUE(nUEs);
//   vec firstSINRPerUE(nUEs);
//   
//   for(int ue_cnt = 0; ue_cnt < nUEs; ue_cnt++)
//   {
//     ueIDs(ue_cnt)=tUEInfoPerUEs(ue_cnt).UEid;
//     nLayersPerUE(ue_cnt)=tUEInfoPerUEs(ue_cnt).nLayers;
//     reportedSINRPerUE(ue_cnt)=tUEInfoPerUEs(ue_cnt).sinrInDB(0);
//     firstSINRPerUE(ue_cnt)=tUEInfoPerUEs(ue_cnt).sinrInDB(0)(0);
//   }
//   
//   if(firstSINRPerUE.length() == 0)
//     return;
//   
//   Array<UEInfo_S> pairedUEInfo=tUEInfoPerUEs;
//   
//   int maxCDMs = dmrsType+1;
//   
//   int maxUEPairs=std::min(nUserPairing,maxCDMs);
//   int nCombinations=sumNCr(ueIDs.length(),maxUEPairs); 
//   vec capacity=zeros(nCombinations);
//   bvec singleUEComb=zeros_b(nCombinations);
//   Array<vec> sinrOffsetinDBPerCombination(nCombinations);
//   Array<vec> expectedSINRinDBPerCombination(nCombinations);
//   
//   bmat pairedUECombinations=getAllBinaryCombinations(ueIDs.length(),maxUEPairs);
//   ivec decEquivalents(nCombinations);
//   
//   ivec alreadyPairedUEs(0);
//   Array<vec> SINRofAlreadyPairedUEs(0);
//   Array<ivec> ranksOfPairedUEs(pairedUEs.length());
//   for(int cnt=0;cnt<pairedUEs.length();cnt++)
//   {
//       for(int cnt1=0;cnt1<pairedUEs(cnt).length();cnt1++)
//       {
//           int indx=find(alreadyPairedUEs,pairedUEs(cnt)(cnt1));
//           if(indx==-1)
//           {
//               append(alreadyPairedUEs,pairedUEs(cnt)(cnt1));
//               append(SINRofAlreadyPairedUEs,to_vec(SINRInDBOfPairedUEs(cnt)(cnt1)));
//           }
//           else
//               append(SINRofAlreadyPairedUEs(indx),SINRInDBOfPairedUEs(cnt)(cnt1));
//       }
//       ranksOfPairedUEs(cnt) = nLayersPerUE(find(ueIDs,pairedUEs(cnt)));
//   }
//   for(int comb_cnt=(nCombinations-1);comb_cnt>=0;comb_cnt--)
//   {
//       bvec temp= pairedUECombinations.get_row(comb_cnt);
//       decEquivalents(comb_cnt)=bin2dec(temp);
//   }
//   bvec validPair=zeros_b(nCombinations);
//   Array<PairingInfo_S> pairingInfoPerComb(nCombinations);
//   for(int comb_cnt=(nCombinations-1);comb_cnt>=0;comb_cnt--)
//   {
//     if(validPair(comb_cnt)==1 || (find(pairedUECombinations.get_row(comb_cnt)).length()==1 && mForceMUMIMO)) //Rejecting all Single User Combinations for Forced MU-MIMO.
//           continue;
//     else
//     {
//       ivec pairedUEIndices=find(pairedUECombinations.get_row(comb_cnt));
//       PairingInfo_S pairingInfo;
//       
// //       vec sinrPerPairedUEs=firstSINRPerUE(pairedUEIndices);
// //       
// //       ivec alreadyPairedIndices=find(alreadyPairedUEs,ueIDs(pairedUEIndices));
// //       bool skipComb=false;
// //       for(int cnt=0;cnt<alreadyPairedIndices.length();cnt++)
// //       {
// //         if(alreadyPairedIndices(cnt)!=-1)
// //         {
// //             double minScheduledSINR=min(SINRofAlreadyPairedUEs(alreadyPairedIndices(cnt)));
// //             if(abs(sinrPerPairedUEs(cnt)-minScheduledSINR)>10.0)
// //                 skipComb=true;
// //         }
// //       }
// //       if(skipComb) continue;
// 
// //       if(pairedUEIndices.length()>1 && find(sinrPerPairedUEs<5.0).length()) // not considering MUMIMO pairs with UE sinr<10
// //           continue;
//             
//       if(isValidPair(pairedUEs,ueIDs(pairedUEIndices),maxUEPairs,pairingInfo,dmrsType,dmrsLength,nLayersPerUE(pairedUEIndices),ranksOfPairedUEs))  
//       {
//           validPair(comb_cnt)=1;
//           pairingInfoPerComb(comb_cnt)=pairingInfo;
//       }
//     }
//   }
// 
//   for(int comb_cnt=nCombinations-1;comb_cnt>=0;comb_cnt--)
//   {
//       ivec pairedUEIndices=find(pairedUECombinations.get_row(comb_cnt));
//       int nPairedUEs=pairedUEIndices.length();
//       
//       if(validPair(comb_cnt))  
//       {
//           sinrOffsetinDBPerCombination(comb_cnt).set_size(0);
//           expectedSINRinDBPerCombination(comb_cnt).set_size(0);
//           
//           int CDM,nCDM;
//           computeCDMsizeAndcount(nLayersPerUE(pairedUEIndices),CDM,nCDM,dmrsType,dmrsLength);
//           
//           //int nCDM=nPairedUEs; // This assumptions is not valid intra-CDM multiplexing of UEs - Dhiv
//           
//           complex<double> txSubcarrierPower=dComplex(sqrt(inv_dB(eNodeBTxSCPowerIndBm)),0.0);
//           
//           for(int pair_cnt=0;pair_cnt<nPairedUEs;pair_cnt++)
//           {
//               vec sinrOffsetInDB;
//               vec oldPPSinr=inv_dB(reportedSINRPerUE(pairedUEIndices(pair_cnt)));
//               if(nPairedUEs>1)
//               {
//                 
//                 Array<cmat> sbChannels;
//     //               if(pairedUEInfo(pairedUEIndices(pair_cnt)).sbChannel.length()>10)
//     //                   sbChannels=randPick(pairedUEInfo(pairedUEIndices(pair_cnt)).sbChannel,10);
//     //               else
//                     sbChannels=pairedUEInfo(pairedUEIndices(pair_cnt)).sbChannel;
//                     
//                 cmat P=pairedUEInfo(pairedUEIndices(pair_cnt)).precoder(0);
//                 complex< double > additionalScaling=complex< double >(sqrt(1.0/(double)nCDM),0.0);	
//                 
//                 int nR=sbChannels(0).rows();
//                 int nT=sbChannels(0).cols();
//                 cmat avgH=zeros_c(nR,nT);
//                 cmat avgAdditonalIntrCov=zeros_c(nR,nR);
//                 for(int cnt=0;cnt<sbChannels.length();cnt++)
//                 {
//                     cmat H=sbChannels(cnt);
//                     avgH+=H;
//                     for(int others_cnt=0;others_cnt<nPairedUEs;others_cnt++)
//                     {
//                         if(pair_cnt!=others_cnt)
//                         {
//                             cmat Pi=pairedUEInfo(pairedUEIndices(others_cnt)).precoder(0);
//                             cmat HPi=H*Pi*additionalScaling*txSubcarrierPower;
//                             avgAdditonalIntrCov+=HPi*HPi.H();
//                         }
//                     }
//                 }
//                 avgAdditonalIntrCov/=(double)sbChannels.length();
//                 avgH/=(double)sbChannels.length();
//                 sinrOffsetInDB=getSINROffsetInDBwithMMSEperCodeword(oldPPSinr,avgH*P*txSubcarrierPower,avgAdditonalIntrCov,additionalScaling);
//             }
//               else
//                   sinrOffsetInDB = zeros(oldPPSinr.length());
//        
//             append(sinrOffsetinDBPerCombination(comb_cnt),sinrOffsetInDB);
//             append(expectedSINRinDBPerCombination(comb_cnt),dB(oldPPSinr)+sinrOffsetInDB);
//           }
//           capacity(comb_cnt)=sum(log2(1+inv_dB(expectedSINRinDBPerCombination(comb_cnt))));
//           if(nPairedUEs==1)
//           {
//               singleUEComb(comb_cnt)=(bin)1;
//               if(mForceMUMIMO) // Making SU-MIMO combinations to have zero capacity - Dhiv
//                 capacity(comb_cnt)=0;
//           }
//       }
//   }
// //   #ifdef MUMIMO_DEBUG
// //   cout<<"nCombinations : "<<nCombinations<<endl;
// // //   cout<<"capacity : "<<capacity<<endl;
// //   #endif
// //   ivec singleUEIndices=find(singleUEComb);
// //   double maxSingleUECapacity=max(capacity(singleUEIndices));
// //   ivec bestPairs=find(capacity>maxSingleUECapacity); // considering only pairing combinations greater then max single UE capacity
// //   append(bestPairs,singleUEIndices);
//   
//   ivec bestPairs=find(capacity>0);
//   int nPairs = bestPairs.length();
//   for(int pair_cnt = 0; pair_cnt < nPairs; pair_cnt++)
//   {
//     ivec pairedUEIndices=find(pairedUECombinations.get_row(bestPairs(pair_cnt)));
//     int nPairedUEs=pairedUEIndices.length();
//     MUMIMOPairingInfo_S MUMIMOPairingOption;
//     MUMIMOPairingOption.mUEIds = ueIDs(pairedUEIndices);
//     MUMIMOPairingOption.nLayers=nLayersPerUE(pairedUEIndices);
//     MUMIMOPairingOption.SINRoffsetInDB.set_size(nPairedUEs);
//     MUMIMOPairingOption.expectedSINRInDB.set_size(nPairedUEs);
//     MUMIMOPairingOption.pairingInfo=pairingInfoPerComb(bestPairs(pair_cnt));
// 
//     int indx=0;
//     bool skipflag=false;
//     
//     for(int ue_cnt=0;ue_cnt<nPairedUEs;ue_cnt++)
//     {
//       int nCodeWords=(MUMIMOPairingOption.nLayers(ue_cnt)>4)?2:1;
//       MUMIMOPairingOption.SINRoffsetInDB(ue_cnt)=sinrOffsetinDBPerCombination(bestPairs(pair_cnt))(indx,indx+nCodeWords-1);
//       MUMIMOPairingOption.expectedSINRInDB(ue_cnt)=expectedSINRinDBPerCombination(bestPairs(pair_cnt))(indx,indx+nCodeWords-1);
//       
//       indx+=nCodeWords;
//       
// //       if(find(MUMIMOPairingOption.SINRoffsetInDB(ue_cnt)<-10.0).length())
// //           skipflag=true;
//     }
//     if(!skipflag)
//         append(MUMIMOPairingOptions,MUMIMOPairingOption);
//   }
// }

void Scheduler::fFindRank1UEs(Array<UEInfo_S>& tUEInfoPerUEs)
{
    ivec IndicesToRemove;
    for(int ue_cnt = 0; ue_cnt < tUEInfoPerUEs.size(); ue_cnt++)
    {
        if(tUEInfoPerUEs(ue_cnt).nLayers != 1)
        {
            append(IndicesToRemove,ue_cnt);
        }
    }
    deleteInArray(tUEInfoPerUEs,IndicesToRemove);
}

void Scheduler::fFindRank2UEs(Array<UEInfo_S>& tUEInfoPerUEs)
{
    ivec IndicesToRemove;
    for(int ue_cnt = 0; ue_cnt < tUEInfoPerUEs.size(); ue_cnt++)
    {
        if(tUEInfoPerUEs(ue_cnt).nLayers != 2)
        {
            append(IndicesToRemove,ue_cnt);
        }
    }
    deleteInArray(tUEInfoPerUEs,IndicesToRemove);
}

Array<LayerInfo_S> getLayersInfo(ivec mUEIds, ivec nLayers, ivec nCDMsPerUE, ivec CDMPerUE,int dmrsType,int maxLength){

    Array<LayerInfo_S> layerInfoPerUE(mUEIds.length());
    for(int ue_cnt=0;ue_cnt<mUEIds.length();ue_cnt++)
    {
        if(mUEIds(ue_cnt)!=-1)
        {
            layerInfoPerUE(ue_cnt).dmrstype=dmrsType;
            layerInfoPerUE(ue_cnt).maxLength=maxLength;
            layerInfoPerUE(ue_cnt).nLayers=nLayers(ue_cnt);
            layerInfoPerUE(ue_cnt).nCDM=nCDMsPerUE(ue_cnt);
            layerInfoPerUE(ue_cnt).CDM=CDMPerUE(ue_cnt);
            layerInfoPerUE(ue_cnt).CDMMap=zeros_b(mUEIds.length());
            layerInfoPerUE(ue_cnt).CDMMap(ue_cnt)=1;
        }
    }
    return(layerInfoPerUE);
}

void ScheduledRBsInfo_S::assignCDMPerUE(ivec &CDMPerUE, ivec &nCDMsPerUE,ivec& CDMMapLengthsPerUE,int dmrsType,int maxLength)
{
    nCDMsPerUE=zeros_i(ueIDs.length());
    CDMPerUE=zeros_i(ueIDs.length());
    CDMMapLengthsPerUE=zeros_i(ueIDs.length());
    ivec pairedUECount=zeros_i(rbNumbers.length());
    for(int rb_cnt=0;rb_cnt<rbNumbers.length();rb_cnt++)
        pairedUECount(rb_cnt)=scheduledUEsPerRB(rb_cnt).length();

    ivec rbOrder=reverse(sort_index(pairedUECount));

    for(int itr_cnt=0;itr_cnt<2;itr_cnt++) // 2 iterations to re-set values if necessary - Dhiv
        for(int rb_cnt=0;rb_cnt<rbNumbers.length();rb_cnt++)
        {
            ivec ueIndices=find(ueIDs,scheduledUEsPerRB(rbOrder(rb_cnt)));
            ivec layersPerUE=scheduledLayersPerRB(rbOrder(rb_cnt));
            int currentCDMSize,currentCDMCount;

            if(max(layersPerUE)>2*maxLength && ueIndices.length()>1) {cout<<"Layers beyond "<<2*maxLength<<" not supported by DMRS type "<<dmrsType<<" with maxLength "<<maxLength<<" for MU-MIMO.. "<<endl;abort();}

            computeCDMsizeAndcount(layersPerUE,currentCDMSize,currentCDMCount,dmrsType,maxLength);

            //currentCDMCount = ueIndices.length(); // currently CDM count is nUEs paired in the RB .. Need to change - Dhiv

            //       ivec requiredCDMs=pairingPositionPerUE(ueIndices)+1;
            //       
            //       if(max(requiredCDMs)>currentCDMCount)
            //         currentCDMCount=max(requiredCDMs);

            int currentCDMMapLength=max(pairingPositionPerUE(ueIndices))+1;

            if(max(nCDMsPerUE(ueIndices))>currentCDMCount)
                currentCDMCount=max(nCDMsPerUE(ueIndices));

            if(max(CDMPerUE(ueIndices))>currentCDMSize)
                currentCDMSize=max(CDMPerUE(ueIndices));

            if(max(CDMMapLengthsPerUE(ueIndices))>currentCDMMapLength)
                currentCDMMapLength=max(CDMMapLengthsPerUE(ueIndices));

            setInVec(nCDMsPerUE,ueIndices,currentCDMCount);
            setInVec(CDMPerUE,ueIndices,currentCDMSize);
            setInVec(CDMMapLengthsPerUE,ueIndices,currentCDMMapLength);
        }

}
//To check whether the DMRS ports are unique for each paired user for MU-MIMO.
void ScheduledRBsInfo_S::checkDMRSportOverlapping()
{
    Array<ivec> dmrsPortsPerUE(ueIDs.length());
    for(int ue_cnt=0;ue_cnt<ueIDs.length();ue_cnt++)
    {
        dmrsPortsPerUE(ue_cnt) = getDMRSports(layersInfoPerUE(ue_cnt),1);

#ifdef MUMIMO_DEBUG
        cout<<"ue : "<<ueIDs(ue_cnt)<<"\tLayersInfo : "<<layersInfoPerUE(ue_cnt).nLayers;
        cout<<", "<<layersInfoPerUE(ue_cnt).CDMMap<<", "<<dmrsPortsPerUE(ue_cnt)<<endl;
#endif
    }
    for(int rb_cnt=0;rb_cnt<rbNumbers.length();rb_cnt++)
    {
        ivec ueIndices=find(ueIDs,scheduledUEsPerRB(rb_cnt));
        for(int i=0;i<ueIndices.length();i++)
        {
            ivec myPorts = dmrsPortsPerUE(ueIndices(i));
            for(int j=i+1;j<ueIndices.length();j++)
            {
                ivec MyPorts = dmrsPortsPerUE(ueIndices(j)); 
                if(sum(find(myPorts,MyPorts)!=-1))
                {
                    cout<<"\nueID: "<<ueIDs(ueIndices(i))<<" DMRSports: "<<myPorts;layersInfoPerUE(ueIndices(i)).print();
                    cout<<"\nueID: "<<ueIDs(ueIndices(j))<<" DMRSports: "<<MyPorts;layersInfoPerUE(ueIndices(j)).print();
                    cout<<"Same DMRS ports cannot be used for different UEs in MU-MIMO...Check..."<<endl;
                    abort();
                }
            }
        }
    }
}

void ScheduledRBsInfo_S::updateLayersMapInfo(int dmrsType,int maxLength)
{
    if(pairingPositionPerUE.length()==0)
        pairingPositionPerUE=zeros_i(ueIDs.length());

    ivec nCDMsPerUE,CDMPerUE,CDMMapLengthsPerUE;
    assignCDMPerUE(CDMPerUE,nCDMsPerUE,CDMMapLengthsPerUE,dmrsType,maxLength);

#ifdef SCH_DEBUG
    cout<<"ueIDs : "<<ueIDs<<endl;
    cout<<"RanksPerUE : "<<RanksPerUE<<endl;
    cout<<"CDMPerUE : "<<CDMPerUE<<endl;
    cout<<"nCDMsPerUE : "<<nCDMsPerUE<<endl;
#endif

    layersInfoPerUE.set_length(ueIDs.length());

    for(int rb_cnt=0;rb_cnt<rbNumbers.length();rb_cnt++)
    {
        ivec layersPerUE = scheduledLayersPerRB(rb_cnt);
        ivec ueIndices=find(ueIDs,scheduledUEsPerRB(rb_cnt));

        ivec pairingPositions= pairingPositionPerUE(ueIndices);
        //int nCDM=max(nCDMsPerUE(ueIndices)); //nCDM will be same for all paired UEs (need to think) - Dhiv
        //int CDM=CDMPerUE(ueIndices(0)); //CDM will be same for all paired UEs (need to think) - Dhiv
        int CDMMapLength = max(CDMMapLengthsPerUE(ueIndices));
        ivec ues=zeros_i(CDMMapLength)-1;
        ivec layers=zeros_i(CDMMapLength); //scaling of 2 as pairing positons is considered from 0-5 - Dhiv
        ivec nCDMs(CDMMapLength),CDM(CDMMapLength);
        setInVec(ues,pairingPositions,scheduledUEsPerRB(rb_cnt));
        setInVec(layers,pairingPositions,scheduledLayersPerRB(rb_cnt));
        setInVec(nCDMs,pairingPositions,nCDMsPerUE(ueIndices));
        setInVec(CDM,pairingPositions,CDMPerUE(ueIndices));

        Array<LayerInfo_S> layersInfo=getLayersInfo(ues,layers,nCDMs,CDM,dmrsType,maxLength);
        for(int ue_cnt=0;ue_cnt<ueIndices.length();ue_cnt++)
        {
            if(layersInfoPerUE(ueIndices(ue_cnt)).CDMMap.length()<layersInfo(pairingPositions(ue_cnt)).CDMMap.length())
            {
                layersInfoPerUE(ueIndices(ue_cnt))=layersInfo(pairingPositions(ue_cnt));
            }
        }
    }
    //To check whether the DMRS ports are unique per UE or not...
    if(max(pairingPositionPerUE)){checkDMRSportOverlapping();}
}

void printMUMIMOpairingInfo(ScheduledUEInfo_S& schUEInfo,int subframenumber,int eNBID,int beamID, int RBG,string MUMIMOLogsfilename)
{
    std::ofstream MUMIMOpairingLogs;
    string mumimoDir = MUMIMOLogsfilename;

    MUMIMOLogsfilename = mumimoDir +"/eNB_ID_"+toString(eNBID)+".txt";
    ifstream f(MUMIMOLogsfilename.c_str());
    bool IsFileExist = f.good();f.close();
    if(!IsFileExist)
    {
        static bool isMUMIMODirExist=false;
        if(!isMUMIMODirExist)
        {
            createDirectory(mumimoDir);isMUMIMODirExist=true;
        }
        MUMIMOpairingLogs.open(MUMIMOLogsfilename.c_str(),ios::out);
        MUMIMOpairingLogs<<"%subframeNumber,\tBeamID,\tRBGnumber,\tassociatedRBs,\tpairedUEs,\tRankPerUE,\tSINRoffsetPerUE"<<endl;
    }
    else
        MUMIMOpairingLogs.open(MUMIMOLogsfilename.c_str(),ios::app);

    MUMIMOpairingLogs<<subframenumber<<",\t"<<beamID<<",\t"<<RBG<<",\t"<<schUEInfo.vrbs<<",\t"<<schUEInfo.ueid<<",\t"<<schUEInfo.nLayers<<",\t"<<schUEInfo.MUMIMOPair.SINRoffsetInDB<<endl;
    MUMIMOpairingLogs.close();
}

void printSchInfoLogs(SchedulingInfo_S& schedulingInfo,int subframenumber,int eNBID,int beamID,int nRBs, int nMUMIMO, int nSUMIMO,double avgRankPerSB,string schInfoLogsFileName)
{
    std::ofstream schInfoLogs;
    string dir = schInfoLogsFileName;
    schInfoLogsFileName = dir +"/eNB_ID_"+ toString(eNBID)+".txt";
    ifstream f(schInfoLogsFileName.c_str());
    bool IsFileExist = f.good();f.close();
    if(!IsFileExist)
    {
        static bool isSchDirExist=false;
        if(!isSchDirExist)
        {
            createDirectory(dir);isSchDirExist=true;
        }
        schInfoLogs.open(schInfoLogsFileName.c_str(),ios::out);
        schInfoLogs<<"%subframeNumber,\tBeamID,\tscheduledUEs,\trankPerUE,\tnRBsPerUE,\tnRBsWithMUMIMO,\tnRBsWithSUMIMO,\t% MUMIMOpairing,\tavgTxLayers,\tresourceUtilsation,\tassociatedDMRSPortIndices"<<endl;
    }
    else
        schInfoLogs.open(schInfoLogsFileName.c_str(),ios::app);
    ivec ranksPerUE(0), nRBsPerUE(0);
    Array<ivec> dmrsPortsPerUE(0);
    for(int ue_cnt=0;ue_cnt<schedulingInfo.ueIDs.length();ue_cnt++)
    {
        append(ranksPerUE,schedulingInfo.dataBlkInfo(ue_cnt).dataGenInfo.nLayers);
        append(nRBsPerUE,schedulingInfo.dataBlkInfo(ue_cnt).dataGenInfo.scheduledPRBs);
        append(dmrsPortsPerUE,getDMRSports(schedulingInfo.dataBlkInfo(ue_cnt).dataGenInfo.layersInfo,1));
    }
    double tPairingEfficiency = (double)(nMUMIMO)/(double)(nMUMIMO+nSUMIMO);
    double resourceUtilisation = (double) (sum(nRBsPerUE))/nRBs;
    schInfoLogs<<subframenumber<<",\t"<<beamID<<",\t"<<schedulingInfo.ueIDs<<",\t"<<ranksPerUE<<",\t"<<nRBsPerUE<<",\t"<<nMUMIMO<<",\t"<<nSUMIMO<<",\t"<<tPairingEfficiency*100<<",\t"<<avgRankPerSB<<",\t"<<resourceUtilisation<<",\t"<<dmrsPortsPerUE<<endl;
    schInfoLogs.close();
}

PairingMethod_E getPairingMethod(string name)
{
    PairingMethod_E toReturn;
    if(name == "_IDEAL_PAIRING_")
        toReturn = _IDEAL_PAIRING_;
    else if(name == "_ZERO_FORCING_")
        toReturn = _ZERO_FORCING_;
    else if(name == "_SUCCESSIVE_MMSE_")
        toReturn = _SUCCESSIVE_MMSE_;
    else if(name == "_BLOCK_DIAGONALISATION_")
        toReturn = _BLOCK_DIAGONALISATION_;
    else
    {
        cout<<"MU-MIMO Pairing Method not Initialized...setting _IDEAL_PAIRING_ as default..."<<endl;
        toReturn = _IDEAL_PAIRING_;
    }
    return toReturn;
}
