#include "../include/scheduler.h"

extern cTBTables tbTables;
extern AWGNTablesConvCoder awgnTables;


Scheduler_C::Scheduler_C()
{
    roundRobinIndex=0;
}

Scheduler_C::Scheduler_C(double invT,ivec associatedUES)
{
    invTau=invT;
    associatedUEs = associatedUES;
    R=zeros(associatedUES.length())+0.2;
    roundRobinIndex=0;
}

void Scheduler_C::updateRForAll(ivec contendingUEs, int winnerUE, float r)
{
    int winnerUEIndx = find(associatedUEs , winnerUE);
    R(winnerUEIndx) = ((1 - invTau) * R(winnerUEIndx)) + r * invTau;
            
    ivec loserUEs = contendingUEs(findNot(contendingUEs,winnerUE));
    ivec loserUEIndices =  find(associatedUEs,loserUEs);
    vec updatedR = (1 - invTau) * R(loserUEIndices); // r is not used!
    setInVec(R,loserUEIndices,updatedR);
    
}

void Scheduler_C::performRateOffsetForUplink(ivec contendingUEs, vec &r, schInfo_S schInfo, vec srsTxSCPowerInWatts)
{
    for(int ue_cnt=0;ue_cnt<contendingUEs.length();ue_cnt++)
    {
        int ue = contendingUEs(ue_cnt);
        int ueIndx = find(associatedUEs,ue);
        int nLayers = latestCSIFeedbackInfoPerAssociatedUE(ueIndx).nLayers;
        double ratePerLayer = r(ue_cnt) / nLayers;
        int scheduledRBs = (schInfo.getScheduledRBGs(ue)+1)*rbgSize;
        double txSCPowerInWatts =  powerControl.getUplinkPowerInWatts(ue,scheduledRBs)/(scheduledRBs*12*4);
        ratePerLayer = awgnTables.getRateOffset(ratePerLayer,dB(txSCPowerInWatts/srsTxSCPowerInWatts(ue_cnt)));
        r(ue_cnt) = ratePerLayer*nLayers;
        
    }
    
}


void Scheduler_C::performRateOffsetForOLLA(ivec contendingUEs, vec &r, vec powerOffsetInDB)
{
    for(int ue_cnt=0;ue_cnt<contendingUEs.length();ue_cnt++)
    {
        int ue = contendingUEs(ue_cnt);
        int ueIndx = find(associatedUEs,ue);
        int nLayers = latestCSIFeedbackInfoPerAssociatedUE(ueIndx).nLayers;
        double ratePerLayer = r(ue_cnt) / nLayers;
        ratePerLayer = awgnTables.getRateOffset(ratePerLayer,powerOffsetInDB(ue_cnt));
        r(ue_cnt) = ratePerLayer*nLayers;
        
    }
    
}

bool validateSelection(int currentRBG, int scheduledUE, int rbgSize, int nSymbols, schInfo_S &schInfo, ivec &contendingUEs,ivec &contendingUEIndices, Array<HarqInfo> &harqInfoPerAssociatedUE, ivec &bufferSizes, ivec &uesForCSI, Array<CSI_Feedback_Info_S> &csiFeedbackInfoPerUE)
{
    
    vec targetBLER(1);
    targetBLER(0)=0.1;
    vec rateThresholds(0);
   
    int selectedUEIndexForCSI = find(uesForCSI,scheduledUE);
    int tbSizeInBytesForScheduledUE;
    ivec scheduledRBGs = find(schInfo.selectedUEs,scheduledUE,true);
    append(scheduledRBGs,currentRBG);
    int nRBsAllocated = scheduledRBGs.length()*rbgSize;
    CSI_Feedback_Info_S csiFB = csiFeedbackInfoPerUE(selectedUEIndexForCSI);
    
    ivec mcsIndicesPerCW;
        
    imat rbRSPattern=getDMRSPattern(1,csiFB.nLayers);
    rbRSPattern  = rbRSPattern.get_cols(0,nSymbols-1); // Need to be careful with this truncation
    
    int nREsPerRB=count(rbRSPattern,0);
    int nREsAllocated = nRBsAllocated*nREsPerRB;
    int nCodewords = csiFB.cqiPerSubbandPerCW(0).length();
    
    ivec nLayersPerCW = getNumLayersPerCodeWord(nCodewords,csiFB.nLayers);
    Array<ivec> cqi = getFromArray(csiFB.cqiPerSubbandPerCW,scheduledRBGs);  //Assuming RBG and subband size are same - Dhiv
    Array<ivec> cqiPerCW = swapDimensions(cqi);
    Array<vec> sinrInDBPerCW = awgnTables.getSINRIndBfromCQI(cqiPerCW);
    
    vec sinrOffsetIndB=zeros(nCodewords);
    LinkAdaptation_S linkAdaptationInfo = tbTables.getMCSPerCodeWord(sinrInDBPerCW, nRBsAllocated, nSymbols, nREsAllocated, csiFB.nLayers, true, sinrOffsetIndB,rateThresholds,targetBLER);
    tbSizeInBytesForScheduledUE = sum(linkAdaptationInfo.tbSizePerCodewordInBytes);
    
    int scheduledUEIndex = find(contendingUEs,scheduledUE);
    
    if(harqInfoPerAssociatedUE(scheduledUEIndex).isNewTransmission(0)) // No assumption of reTx multiplexed with new transmission
    {
        if(bufferSizes.length())
        {
            if((bufferSizes(scheduledUEIndex)-tbSizeInBytesForScheduledUE)>=0)
            {
                bufferSizes(scheduledUEIndex)-=tbSizeInBytesForScheduledUE;
                return true;
            }
            else
            {
                contendingUEs.del(scheduledUEIndex);
                contendingUEIndices.del(scheduledUEIndex);
                bufferSizes.del(scheduledUEIndex);
                
                return false;
            }
        }
        return true;
    }
    else
    {
        //Assuming half allocation for HARQ
        if((sum(harqInfoPerAssociatedUE(scheduledUEIndex).TBSizePerCodewordInBits)/2-tbSizeInBytesForScheduledUE)>=0)
        {
            harqInfoPerAssociatedUE(scheduledUEIndex).TBSizePerCodewordInBits-=(tbSizeInBytesForScheduledUE/2);
            return true;
        }
        else
        {
            contendingUEs.del(scheduledUEIndex);
            contendingUEIndices.del(scheduledUEIndex);
            deleteInArray(harqInfoPerAssociatedUE,to_ivec(scheduledUEIndex));
            
            return false;
        }
    }
}


schInfo_S Scheduler_C::schedule(int nRBGs, bool isPF, ivec contendingUEs, Array<vec> ratePerRBGPerUE, Array<HarqInfo> harqInfoPerAssociatedUE, ivec bufferSizes, vec srsTxSCPowerInWatts)
{
    schInfo_S schInfo(nRBGs);
    ivec contendingUEIndices = find(associatedUEs,contendingUEs);
    ivec contentingUEList = getIntegers(0,contendingUEs.length()-1);
    
    vec powerOffsetInDB(contendingUEs.length());
    for(int ue_cnt=0;ue_cnt<contendingUEs.length();ue_cnt++)
        powerOffsetInDB(ue_cnt) = harqInfoPerAssociatedUE(contendingUEIndices(ue_cnt)).powerOffsetInDBFromOLLA;
    
    cout<<"powerOffsetInDB : "<<powerOffsetInDB<<endl;
    
    if(isPF)
    {
        if(contendingUEs.length())
            for(int rbg_cnt=0;rbg_cnt<nRBGs;rbg_cnt++)
            {
                vec r = ratePerRBGPerUE(rbg_cnt)(contentingUEList);
                
                performRateOffsetForOLLA(contendingUEs,r,powerOffsetInDB);
                
                if(!isDownlink)
                    performRateOffsetForUplink(contendingUEs,r,schInfo,srsTxSCPowerInWatts);

                vec pfMetric = elem_div(r,R(contendingUEIndices(contentingUEList)));

                int selectedUEIndx = max_index(pfMetric);
                int selectedUE = contendingUEs(selectedUEIndx);
                
                if(!validateSelection(rbg_cnt, selectedUE, rbgSize, 12, schInfo, contendingUEs,contentingUEList,harqInfoPerAssociatedUE,bufferSizes, associatedUEs, latestCSIFeedbackInfoPerAssociatedUE))
                {
                    rbg_cnt--;
                    
                    if(contendingUEs.length())
                        continue;
                    else
                        break;
                }
                    
                schInfo.add(rbg_cnt,selectedUE,r(selectedUEIndx),powerOffsetInDB(selectedUEIndx));
                updateRForAll(contendingUEs,selectedUE,r(selectedUEIndx));
            }
    }
    else //RR
    {
        if(contendingUEs.length())
            for(int rbg_cnt=0;rbg_cnt<nRBGs;rbg_cnt++)
            {
                roundRobinIndex=mod(roundRobinIndex,contendingUEIndices.length());
                int selectedUEIndx = roundRobinIndex;
                roundRobinIndex++;
                
                vec r = ratePerRBGPerUE(rbg_cnt);
                
                performRateOffsetForOLLA(contendingUEs,r,powerOffsetInDB);
                
                if(!isDownlink)
                    performRateOffsetForUplink(contendingUEs,r,schInfo,srsTxSCPowerInWatts);
                
                int selectedUE = contendingUEs(selectedUEIndx);
                if(!validateSelection(rbg_cnt, selectedUE, rbgSize, 12, schInfo, contendingUEs,contentingUEList,harqInfoPerAssociatedUE,bufferSizes, associatedUEs, latestCSIFeedbackInfoPerAssociatedUE))
                {
                    rbg_cnt--;
                    
                    if(contendingUEs.length())
                        continue;
                    else
                        break;
                }
                
                schInfo.add(rbg_cnt,selectedUE,r(selectedUEIndx),powerOffsetInDB(selectedUEIndx));
            }
    }
    
//     cout<<"schInfo : "<<schInfo.selectedUEs<<endl;
//     cout<<"schInfo : "<<schInfo.powerOffsetInDB<<endl;
    
    return schInfo;
}
    
schInfo_S Scheduler_C::schedule(int nRBGs , bool isPF , ivec contendingUEs, vec ratePerUE, Array<HarqInfo> harqInfoPerAssociatedUE, ivec bufferSizes, vec srsTxSCPowerInWatts)
{
    Array<vec> ratePerRBGPerUE(nRBGs);
    ratePerRBGPerUE = repeatAsArray(ratePerUE,nRBGs);
    return schedule( nRBGs ,  isPF ,  contendingUEs,  ratePerRBGPerUE, harqInfoPerAssociatedUE,  bufferSizes,  srsTxSCPowerInWatts);
}


Array<Array<SCHGrant_S>> generateGrantInfo(int nRBs, int rbgSize, int nSymbols, Array<schInfo_S> &schInfo, ivec &ueIDsForCSI, Array<CSI_Feedback_Info_S> &csiFeedbackInfoPerUE, Array<ivec> &ueIDsForHARQ, Array<Array<HarqInfo>> &harqInfoPerAssociatedUE)
 {
     
     int ngNB = schInfo.length();
     ivec rbs = getIntegers(0,nRBs-1);
         
     vec targetBLER(1);
     targetBLER(0)=0.1;
     vec rateThresholds(0);
     
     Array<ivec> rbsperRBG = splitVectorToArray(rbs,rbgSize);
     
     Array<Array<SCHGrant_S>> grantInfo(ngNB);
     for(int gNB_cnt=0;gNB_cnt<ngNB;gNB_cnt++)
     {
         ivec scheduledUEs = findUniqueNumbers(schInfo(gNB_cnt).selectedUEs);
         scheduledUEs = remove(scheduledUEs,-1);
         int nScheduledUEs = scheduledUEs.length();
         ivec ueIndicesForCSI = find(ueIDsForCSI,scheduledUEs);
         ivec ueIndicesForHARQ = find(ueIDsForHARQ(gNB_cnt),scheduledUEs);
         
         grantInfo(gNB_cnt).set_length(nScheduledUEs);
         
         for(int ue_cnt=0;ue_cnt<nScheduledUEs;ue_cnt++)
         {
             ivec scheduledRBGs = find(schInfo(gNB_cnt).selectedUEs,scheduledUEs(ue_cnt),true);
             Array<ivec> scheduledRBsPerRBG = getFromArray(rbsperRBG,scheduledRBGs);
             ivec scheduledRBs = joinArrayToVector(scheduledRBsPerRBG);
             
             CSI_Feedback_Info_S csiFB = csiFeedbackInfoPerUE(ueIndicesForCSI(ue_cnt));
             HarqInfo harqInfo = harqInfoPerAssociatedUE(gNB_cnt)(ueIndicesForHARQ(ue_cnt));
             
             SCHGrant_S schGrant;
             schGrant.ueID = scheduledUEs(ue_cnt);
             schGrant.gNBID = gNB_cnt; // using index as ID - Dhiv
             schGrant.nLayers = csiFB.nLayers;

             schGrant.allocatedRBs = scheduledRBs;
             schGrant.precoderPerRB.set_length(scheduledRBs.length());
             for(int rb_cnt=0;rb_cnt<scheduledRBs.length();rb_cnt++)
             {
                 int rbgIndx = floor_i((double)scheduledRBs(rb_cnt)/(double)rbgSize);
                 schGrant.precoderPerRB(rb_cnt) = csiFB.precoderPerSubband(rbgIndx);
             }
             
            
            ivec mcsIndicesPerCW;
                
            int nRBsAllocated = schGrant.allocatedRBs.length();
            
            imat rbRSPattern=getDMRSPattern(1,schGrant.nLayers);
            rbRSPattern  = rbRSPattern.get_cols(0,nSymbols-1); // Need to be careful with this truncation
            
            int nREsPerRB=count(rbRSPattern,0);
            int nREsAllocated = nRBsAllocated*nREsPerRB;
            schGrant.nREsAllocated = nREsAllocated;
            
            int nCodewords = csiFB.cqiPerSubbandPerCW(0).length();
            Array<ivec> cqiPerCW = swapDimensions(csiFB.cqiPerSubbandPerCW);
            Array<vec> sinrInDBPerCW = awgnTables.getSINRIndBfromCQI(cqiPerCW);
            vec sinrOffsetIndB=zeros(nCodewords)+schInfo(gNB_cnt).powerOffsetInDB(scheduledRBGs(0));
            LinkAdaptation_S linkAdaptationInfo = tbTables.getMCSPerCodeWord(sinrInDBPerCW, nRBsAllocated, nSymbols, nREsAllocated, schGrant.nLayers, true, sinrOffsetIndB,rateThresholds,targetBLER);
            
            schGrant.harqInfo = harqInfo;
            
            for(int cwd_cnt=0;cwd_cnt<nCodewords;cwd_cnt++)
            {
                if(schGrant.harqInfo.isNewTransmission(cwd_cnt))
                {
                    schGrant.harqInfo.TBSizePerCodewordInBits(cwd_cnt)=linkAdaptationInfo.tbSizePerCodewordInBytes(cwd_cnt)*8;
                    schGrant.harqInfo.infoBitsperCodeWord(cwd_cnt) = randb(schGrant.harqInfo.TBSizePerCodewordInBits(cwd_cnt) - getNumCRCBits(CRC24B));
                }
            }
            
            schGrant.mcsIndicesPerCW=linkAdaptationInfo.mcsIndxPerCodeword;
            schGrant.modulationOrderPercw.set_length(nCodewords);
            schGrant.codeRatePercw.set_length(nCodewords);
            
            ivec nLayersPerCW = getNumLayersPerCodeWord(nCodewords,schGrant.nLayers);
            
            for(int cwd_cnt=0;cwd_cnt<nCodewords;cwd_cnt++)
            {
               schGrant.modulationOrderPercw(cwd_cnt) = tbTables.transportBlockMCSMapping.modulationAlphabetPDSCH(schGrant.mcsIndicesPerCW(cwd_cnt));
               schGrant.codeRatePercw(cwd_cnt) = (double)schGrant.harqInfo.TBSizePerCodewordInBits(cwd_cnt)/(double)(nREsAllocated*schGrant.modulationOrderPercw(cwd_cnt)*nLayersPerCW(cwd_cnt));
            }
                        
            grantInfo(gNB_cnt)(ue_cnt) = schGrant;
         }
     }
     
     return grantInfo;
 }
 
