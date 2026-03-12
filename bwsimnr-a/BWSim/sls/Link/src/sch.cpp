#include "../include/sch.h"

extern PowerControl_S powerControl;


cmat computeDataIntrCov(int nRBs,Array<cmat>& rxGrid, ivec allocatedRBs, imat rbRSPattern)
{
    int nRxAntenna = rxGrid.length();
    
    int nSymbolsPerSlot = 14,nSubCarriersPerRB = 12;
    
    cmat intrCov=zeros_c(nRxAntenna,nRxAntenna);
    int re_cnt=0;
    int fftSize  = rxGrid(0).rows();
    int gSc = (fftSize-nRBs*nSubCarriersPerRB)/2;
    
    
    for(int sym_cnt=0;sym_cnt<nSymbolsPerSlot;sym_cnt++)
        for(int rb_cnt=0;rb_cnt<allocatedRBs.length();rb_cnt++)
        {
            ivec scIndices = allocatedRBs(rb_cnt)*nSubCarriersPerRB + getIntegers(0,nSubCarriersPerRB-1);
            for(int sc_cnt=0;sc_cnt<scIndices.length();sc_cnt++)
            {
                if(rbRSPattern(sc_cnt,sym_cnt)==0) // Retrieving only from data positions
                {
                    cmat sym = zeros_c(nRxAntenna,1);
                    for(int rxAntCnt=0;rxAntCnt<nRxAntenna;rxAntCnt++)
                        sym(rxAntCnt,0) = rxGrid(rxAntCnt)(gSc + scIndices(sc_cnt),sym_cnt);
                    
                    intrCov+=sym*sym.H();
                    re_cnt++;
                }
            }
        }
        
        intrCov/=dComplex(re_cnt,0);
        return intrCov;    
}
Array<cmat> calculateWBIntCovPerUE(MCell &mcell, int slot_cnt,int nSubcarriers, int nSymbols, int nRBs, ivec uesForMCS)
{
    int fftSize = nSubcarriers;
    int gc = (fftSize-nRBs*12)/2;
    ivec allScIndices;
    allScIndices = getIntegers(0,nRBs*12-1);
    allScIndices = gc+mod(allScIndices,nRBs*12);
    ivec allRBs = getIntegers(0,nRBs-1);
    double samplingTimeinSec = 1.0/mcell.mySys.channelCloud.getCommonSamplingFrequencyInHz();
    
    double currentTimeInSec = (((nSubcarriers+80.0/1024.0*nSubcarriers)*2.0+(nSubcarriers+72.0/1024.0*nSubcarriers)*12)*slot_cnt)*samplingTimeinSec;
    double endTimeInSec = (((nSubcarriers+80.0/1024.0*nSubcarriers)*2+(nSubcarriers+72.0/1024.0*nSubcarriers)*12)*(slot_cnt+1))*samplingTimeinSec;
    
    
    //Reception/////////////////////////////////////////////////////////
    
    ivec ueIndices = find(mcell.associationInfo.serviceNodes,uesForMCS);
    Array<cmat> intCovPerUE(uesForMCS.length());
    for(int ue_cnt=0;ue_cnt<uesForMCS.length();ue_cnt++)
    {
        ChannelOutput_S channelOutput = mcell.mySys.receiveData(uesForMCS(ue_cnt),0,_ANTENNA_CORRELATION_MEDIUM_,0,currentTimeInSec,endTimeInSec,false,mcell.associationInfo.serverNodes);
        Array<cmat> resourceGridPerRxAntenna = getFreqDomainFrame(channelOutput.data,nSubcarriers,0,nSymbols);
            
        cmat interferenceCovariance;
        ivec intrgNBs = mcell.associationInfo.serverNodes;
        intrgNBs=remove(intrgNBs,mcell.associationInfo.associatedServerNodesForEachServiceNode(ueIndices(ue_cnt)));
        if(intrgNBs.length())
        {
            ChannelOutput_S channelOutput = mcell.mySys.receiveData(uesForMCS(ue_cnt),0,_ANTENNA_CORRELATION_MEDIUM_,0,currentTimeInSec,endTimeInSec,false,intrgNBs);
            Array<cmat> intrResourceGridPerRxAntenna = getFreqDomainFrame(channelOutput.data,fftSize,0,14);
            intCovPerUE(ue_cnt) = computeDataIntrCov(nRBs,intrResourceGridPerRxAntenna,allRBs,getDMRSPattern(1, 1)); 
        }
        else
        {
            double N0=mcell.mySys.channelCloud.getNoiseVariancePerDimension(uesForMCS(ue_cnt))*2.0;
            intCovPerUE(ue_cnt) = N0*eye_c(mcell.mySys.aNodes(uesForMCS(ue_cnt)).getAntennaCount());
        }
        
    }
    
    return intCovPerUE;
}
               
void transmitPDSCH(MCell &mcell, Array<cMAC> &MACObjs, int slot_cnt,int nSubcarriers, int nSymbols, int nRBs, double txPowerIndBm , Array<Array<SCHGrant_S>> &grantInfo, int ngNBsWithActualReception)
{
        int ngNB  = grantInfo.length();
        int fftSize = nSubcarriers;
        int gc = (fftSize-nRBs*12)/2;
        ivec allScIndices;
        allScIndices = getIntegers(0,nRBs*12-1);
        allScIndices = gc+mod(allScIndices,nRBs*12);
        
        double samplingTimeinSec = 1.0/mcell.mySys.channelCloud.getCommonSamplingFrequencyInHz();
        
        double currentTimeInSec = (((nSubcarriers+80.0/1024.0*nSubcarriers)*2.0+(nSubcarriers+72.0/1024.0*nSubcarriers)*12)*slot_cnt)*samplingTimeinSec;
        double endTimeInSec = (((nSubcarriers+80.0/1024.0*nSubcarriers)*2+(nSubcarriers+72.0/1024.0*nSubcarriers)*12)*(slot_cnt+1))*samplingTimeinSec;
        
        double txSubcarrierPowerInWatts = inv_dB(txPowerIndBm-30)/(nRBs*12);
        
        //PDSCH Transmission//////////////////////////////////////////////////////
        for(int gNB_cnt=0;gNB_cnt<ngNB;gNB_cnt++)
        {
            int gNBID = mcell.associationInfo.serverNodes(gNB_cnt);
            int nTxAntenna = mcell.mySys.channelCloud.nodesInfo.getNumberOfTxRU(gNBID);
            Array<cmat> resourceGridPerTxAntenna(nTxAntenna);
            for(int tx_cnt=0;tx_cnt<nTxAntenna;tx_cnt++)
                resourceGridPerTxAntenna(tx_cnt)=zeros_c(nSubcarriers,nSymbols);
            
            
            for(int ue_cnt=0;ue_cnt<grantInfo(gNB_cnt).length();ue_cnt++)
            {
                
                SCHGrant_S ueGrantInfo = grantInfo(gNB_cnt)(ue_cnt);
                
                int ueIndx = find(mcell.associationInfo.associatedServiceNodesPerServerNode(gNB_cnt),ueGrantInfo.ueID);
                if(gNB_cnt<ngNBsWithActualReception)
                {
                    if(ueGrantInfo.harqInfo.isNewTransmission(0))
                        cout<<"Performing fresh transmission in harqID : "<<ueGrantInfo.harqInfo.harqProcessID<<" for UE : "<<ueGrantInfo.ueID<<" with (scheduledRBs,MCS) :"<<ueGrantInfo.allocatedRBs.length()<<","<<ueGrantInfo.mcsIndicesPerCW<<endl;
                    else
                        cout<<"Performing re-transmission in harqID : "<<ueGrantInfo.harqInfo.harqProcessID<<" for UE : "<<ueGrantInfo.ueID<<" with (scheduledRBs,MCS) :"<<ueGrantInfo.allocatedRBs.length()<<","<<ueGrantInfo.mcsIndicesPerCW<<endl;
                }   
                MACObjs(gNB_cnt).harqManager(ueIndx)->setTransmissionStatus(ueGrantInfo.harqInfo , slot_cnt);
                CodingParameters codingParam;
                codingParam.load(ueGrantInfo.harqInfo.TBSizePerCodewordInBits, ueGrantInfo.modulationOrderPercw, ueGrantInfo.codeRatePercw, ueGrantInfo.nREsAllocated, ueGrantInfo.nLayers,ueGrantInfo.harqInfo.infoBitsperCodeWord,ueGrantInfo.harqInfo.rvIndicesPerCodeword);
                generateSCH(resourceGridPerTxAntenna , slot_cnt, codingParam, nRBs, ueGrantInfo.allocatedRBs, ueGrantInfo.precoderPerRB, 1);
                
            }
            for(int portIndx = 0;portIndx < resourceGridPerTxAntenna.length();portIndx++)
                resourceGridPerTxAntenna(portIndx) = sqrt(txSubcarrierPowerInWatts)*resourceGridPerTxAntenna(portIndx);

            Array<cvec> timeDomainFrame = getTimeDomainFrame(resourceGridPerTxAntenna,nSubcarriers);

            mcell.mySys.transmitData(gNBID,0,nSubcarriers,timeDomainFrame,currentTimeInSec,1);
        }
}

DecoderOutput_S receiveAndProcessPDSCH(MCell &mcell, cMAC &MACObjs, int slot_cnt,int nSubcarriers, int nSymbols, int nRBs, double txPowerIndBm , SCHGrant_S &ueGrantInfo, bool withActualReception)
{
    int fftSize = nSubcarriers;
    int gc = (fftSize-nRBs*12)/2;
    ivec allScIndices;
    allScIndices = getIntegers(0,nRBs*12-1);
    allScIndices = gc+mod(allScIndices,nRBs*12);
    
    double samplingTimeinSec = 1.0/mcell.mySys.channelCloud.getCommonSamplingFrequencyInHz();
    
    double currentTimeInSec = (((nSubcarriers+80.0/1024.0*nSubcarriers)*2.0+(nSubcarriers+72.0/1024.0*nSubcarriers)*12)*slot_cnt)*samplingTimeinSec;
    double endTimeInSec = (((nSubcarriers+80.0/1024.0*nSubcarriers)*2+(nSubcarriers+72.0/1024.0*nSubcarriers)*12)*(slot_cnt+1))*samplingTimeinSec;
    
    double txSubcarrierPowerInWatts = inv_dB(txPowerIndBm-30)/(nRBs*12);
    
    DecoderOutput_S decoderOutput;
    int gNB_cnt = ueGrantInfo.gNBID;
    
    //PDSCH Reception/////////////////////////////////////////////////////////
    if(withActualReception)
    {
        ChannelOutput_S channelOutput = mcell.mySys.receiveData(ueGrantInfo.ueID,0,_ANTENNA_CORRELATION_MEDIUM_,0,currentTimeInSec,endTimeInSec,false,mcell.associationInfo.serverNodes);
        Array<cmat> resourceGridPerRxAntenna = getFreqDomainFrame(channelOutput.data,nSubcarriers,0,nSymbols);
        
        
        // Getting Ideal Channel
        Array<Array<cvec>> idealChannelTD = channelOutput.channels(gNB_cnt);
        Array<Array<cvec>> idealChannelFD(idealChannelTD.length());
        
        
        Array<Array<Array<cvec>>> channelPerAntennaPerSymbol(nSymbols);
        for(int txAntIndx = 0;txAntIndx < idealChannelFD.length();txAntIndx++)
        {
            idealChannelFD(txAntIndx).set_length(idealChannelTD(0).length());
            for(int rxAntIndx = 0;rxAntIndx < idealChannelFD(0).length();rxAntIndx++)
            {
                
                cvec temp = fft(idealChannelTD(txAntIndx)(rxAntIndx),fftSize);
                temp = fftShift(temp);
                idealChannelFD(txAntIndx)(rxAntIndx) = temp*dComplex(sqrt(txSubcarrierPowerInWatts),0.0);
            }
        }
        
        
        for(int symCnt = 0;symCnt < nSymbols;symCnt++){
            channelPerAntennaPerSymbol(symCnt).set_length(idealChannelFD(0).length());
            for(int rxAntIndx = 0;rxAntIndx < idealChannelFD(0).length();rxAntIndx++)
            {
                channelPerAntennaPerSymbol(symCnt)(rxAntIndx).set_length(idealChannelFD.length());
                for(int txAntIndx = 0;txAntIndx < idealChannelFD.length();txAntIndx++)
                {
                    channelPerAntennaPerSymbol(symCnt)(rxAntIndx)(txAntIndx) = idealChannelFD(txAntIndx)(rxAntIndx);
                }
            }
        }
        
        cmat interferenceCovariance;
        ivec intrgNBs = mcell.associationInfo.serverNodes;
        intrgNBs.del(gNB_cnt);
        if(intrgNBs.length())
        {
            ChannelOutput_S channelOutput = mcell.mySys.receiveData(ueGrantInfo.ueID,0,_ANTENNA_CORRELATION_MEDIUM_,0,currentTimeInSec,endTimeInSec,false,intrgNBs);
            Array<cmat> intrResourceGridPerRxAntenna = getFreqDomainFrame(channelOutput.data,fftSize,0,14);
            interferenceCovariance = computeDataIntrCov(nRBs,intrResourceGridPerRxAntenna,ueGrantInfo.allocatedRBs,getDMRSPattern(1, ueGrantInfo.nLayers)); 
        }
        else
        {
            double N0=mcell.mySys.channelCloud.getNoiseVariancePerDimension(ueGrantInfo.ueID)*2.0;
            interferenceCovariance=N0*eye_c(mcell.mySys.aNodes(ueGrantInfo.ueID).getAntennaCount());
        }
        
        FadingParameters_S fadingParam = mcell.mySys.channelCloud.getLinkSpecificFadingParameters(ueGrantInfo.gNBID,ueGrantInfo.ueID);
        double dopplerSpreadInHz = mcell.mySys.channelCloud.getDopplerSpreadInHz(ueGrantInfo.gNBID,ueGrantInfo.ueID);
        
        CodingParameters codingParam;
        codingParam.load(ueGrantInfo.harqInfo.TBSizePerCodewordInBits, ueGrantInfo.modulationOrderPercw, ueGrantInfo.codeRatePercw, ueGrantInfo.nREsAllocated, ueGrantInfo.nLayers,ueGrantInfo.harqInfo.infoBitsperCodeWord,ueGrantInfo.harqInfo.rvIndicesPerCodeword);
        
        decoderOutput = processSCH(resourceGridPerRxAntenna, ueGrantInfo.harqInfo.harqBuffer, interferenceCovariance , slot_cnt, codingParam, fadingParam, dopplerSpreadInHz, nRBs, ueGrantInfo.allocatedRBs ,1, channelPerAntennaPerSymbol, ueGrantInfo.precoderPerRB);
        cout<<"Decode status : "<<decoderOutput.isSuccess<<" , "<<decoderOutput.isSuccessPerCB<<" for "<<ueGrantInfo.ueID<<endl;

        if(to_ivec(decoderOutput.isSuccess)(0)==0 && find(to_ivec(decoderOutput.isSuccessPerCB(0)),0,true).length()==0)
        {
            cout<<"CB passed and TB failed"<<endl;
            
            cout<<"decoderOutput : "<<decoderOutput.outputBits<<endl;
            cout<<"infobits : "<<ueGrantInfo.harqInfo.infoBitsperCodeWord<<endl;
            
            abort();
        }
        
        
        int ueIndx = find(mcell.associationInfo.associatedServiceNodesPerServerNode(gNB_cnt),ueGrantInfo.ueID);
        MACObjs.harqManager(ueIndx)->setReceptionStatus(decoderOutput.isSuccess,decoderOutput.isSuccessPerCB , ueGrantInfo.harqInfo.harqProcessID, ueGrantInfo.harqInfo.harqBuffer);
    
    }
    else
    {
        decoderOutput.isSuccess=randu(ueGrantInfo.harqInfo.TBSizePerCodewordInBits.length())>0.1;
        //                 cout<<"Decode status : "<<decoderOutput.isSuccess<<" for "<<ueGrantInfo.ueID<<endl;
        int ueIndx = find(mcell.associationInfo.associatedServiceNodesPerServerNode(gNB_cnt),ueGrantInfo.ueID);
        MACObjs.harqManager(ueIndx)->setReceptionStatus(decoderOutput.isSuccess,decoderOutput.isSuccessPerCB , ueGrantInfo.harqInfo.harqProcessID);
    }
    return decoderOutput;
}


void transmitPUSCH(MCell &mcell, Array<cMAC> &MACObjs, int slot_cnt,int fftSize, int nSymbols, int nRBs, Array<Array<SCHGrant_S>> &grantInfo, int ngNBsWithActualReception)
{
    int ngNB  = grantInfo.length();
    
    double samplingTimeinSec = 1.0/mcell.mySys.channelCloud.getCommonSamplingFrequencyInHz();
    
    double currentTimeInSec = (((fftSize+80.0/1024.0*fftSize)*2.0+(fftSize+72.0/1024.0*fftSize)*12)*slot_cnt)*samplingTimeinSec;
    double endTimeInSec = (((fftSize+80.0/1024.0*fftSize)*2+(fftSize+72.0/1024.0*fftSize)*12)*(slot_cnt+1))*samplingTimeinSec;
    
    Array<Array<DecoderOutput_S>> decoderOutput(ngNB);
    Array<ivec> scheduledUEs(ngNB);
    //PUSCH Transmission//////////////////////////////////////////////////////
    for(int gNB_cnt=0;gNB_cnt<ngNB;gNB_cnt++)
    {
        scheduledUEs(gNB_cnt).set_length(grantInfo(gNB_cnt).length());
        decoderOutput(gNB_cnt).set_length(grantInfo(gNB_cnt).length());
        for(int ue_cnt=0;ue_cnt<grantInfo(gNB_cnt).length();ue_cnt++)
        {
            SCHGrant_S ueGrantInfo = grantInfo(gNB_cnt)(ue_cnt);
            
            int ueIndx = find(mcell.associationInfo.associatedServiceNodesPerServerNode(gNB_cnt),ueGrantInfo.ueID);
            MACObjs(gNB_cnt).harqManager(ueIndx)->setTransmissionStatus(ueGrantInfo.harqInfo , slot_cnt);
            
            scheduledUEs(gNB_cnt)(ue_cnt) = ueGrantInfo.ueID;
            double txSubcarrierPowerInWatts = powerControl.getUplinkPowerInWatts(ueGrantInfo.ueID,ueGrantInfo.allocatedRBs.length())/(ueGrantInfo.allocatedRBs.length()*12);
            
            int nTxAntenna = mcell.mySys.channelCloud.nodesInfo.getNumberOfTxRU(grantInfo(gNB_cnt)(ue_cnt).ueID);
            
            Array<cmat> resourceGridPerTxAntenna(nTxAntenna);
            for(int tx_cnt=0;tx_cnt<nTxAntenna;tx_cnt++)
                resourceGridPerTxAntenna(tx_cnt)=zeros_c(fftSize,nSymbols);
            
            if(gNB_cnt<ngNBsWithActualReception)
            {
                if(ueGrantInfo.harqInfo.isNewTransmission(0))
                    cout<<"Performing fresh transmission in harqID : "<<ueGrantInfo.harqInfo.harqProcessID<<" for UE : "<<ueGrantInfo.ueID<<" with (scheduledRBs,MCS) :"<<ueGrantInfo.allocatedRBs.length()<<","<<ueGrantInfo.mcsIndicesPerCW<<endl;
                else
                    cout<<"Performing re-transmission in harqID : "<<ueGrantInfo.harqInfo.harqProcessID<<" for UE : "<<ueGrantInfo.ueID<<" with (scheduledRBs,MCS) :"<<ueGrantInfo.allocatedRBs.length()<<","<<ueGrantInfo.mcsIndicesPerCW<<endl;
            }
            
            CodingParameters codingParam;
            codingParam.load(ueGrantInfo.harqInfo.TBSizePerCodewordInBits, ueGrantInfo.modulationOrderPercw, ueGrantInfo.codeRatePercw, ueGrantInfo.nREsAllocated, ueGrantInfo.nLayers,ueGrantInfo.harqInfo.infoBitsperCodeWord,ueGrantInfo.harqInfo.rvIndicesPerCodeword);
            generateSCH(resourceGridPerTxAntenna , slot_cnt, codingParam, nRBs, ueGrantInfo.allocatedRBs, ueGrantInfo.precoderPerRB, 1);
            for(int portIndx = 0;portIndx < resourceGridPerTxAntenna.length();portIndx++)
                resourceGridPerTxAntenna(portIndx) = sqrt(txSubcarrierPowerInWatts)*resourceGridPerTxAntenna(portIndx);
            
            Array<cvec> timeDomainFrame = getTimeDomainFrame(resourceGridPerTxAntenna,fftSize);
            mcell.mySys.transmitData(ueGrantInfo.ueID,0,fftSize,timeDomainFrame,currentTimeInSec,1);
            
        }
        
    }

}

ChannelOutput_S receivePUSCH(MCell &mcell, int gNBIndx, int slot_cnt,int fftSize, int nSymbols, int nRBs, Array<Array<SCHGrant_S>> &grantInfo, Array<cmat> &intCovPerUE)
{
    int ngNB  = grantInfo.length();
    
    double samplingTimeinSec = 1.0/mcell.mySys.channelCloud.getCommonSamplingFrequencyInHz();
    
    double currentTimeInSec = (((fftSize+80.0/1024.0*fftSize)*2.0+(fftSize+72.0/1024.0*fftSize)*12)*slot_cnt)*samplingTimeinSec;
    double endTimeInSec = (((fftSize+80.0/1024.0*fftSize)*2+(fftSize+72.0/1024.0*fftSize)*12)*(slot_cnt+1))*samplingTimeinSec;
    
    Array<DecoderOutput_S> decoderOutput(grantInfo(gNBIndx).length());
    
    Array<ivec> scheduledUEs(ngNB);
    for(int gNB_cnt=0;gNB_cnt<ngNB;gNB_cnt++)
    {
        scheduledUEs(gNB_cnt).set_length(grantInfo(gNB_cnt).length());
        for(int ue_cnt=0;ue_cnt<grantInfo(gNB_cnt).length();ue_cnt++)
        {
            SCHGrant_S ueGrantInfo = grantInfo(gNB_cnt)(ue_cnt);
            scheduledUEs(gNB_cnt)(ue_cnt) = ueGrantInfo.ueID;
        }
    }
     
    cout<<"scheduledUEs : "<<scheduledUEs<<endl;
    ivec allScheduledUEs = joinArrayToVector(scheduledUEs);
    //PUSCH Reception/////////////////////////////////////////////////////////
    
    ChannelOutput_S channelOutput = mcell.mySys.receiveData(mcell.associationInfo.serverNodes(gNBIndx),0,_ANTENNA_CORRELATION_MEDIUM_,0,currentTimeInSec,endTimeInSec,false,allScheduledUEs);
    Array<cmat> resourceGridPerRxAntenna = getFreqDomainFrame(channelOutput.data,fftSize,0,nSymbols);
    
    ivec intrUEs = remove(allScheduledUEs,scheduledUEs(gNBIndx));
    intCovPerUE.set_length(scheduledUEs(gNBIndx).length());
    if(intrUEs.length())
    {
        ChannelOutput_S channelOutput = mcell.mySys.receiveData(mcell.associationInfo.serverNodes(gNBIndx),0,_ANTENNA_CORRELATION_MEDIUM_,0,currentTimeInSec,endTimeInSec,false,intrUEs);
        Array<cmat> intrResourceGridPerRxAntenna = getFreqDomainFrame(channelOutput.data,fftSize,0,14);
        for(int ue_cnt=0;ue_cnt<grantInfo(gNBIndx).length();ue_cnt++)
        {
            ivec allocatedRBs = grantInfo(gNBIndx)(ue_cnt).allocatedRBs;
            intCovPerUE(ue_cnt) = computeDataIntrCov(nRBs,intrResourceGridPerRxAntenna,getIntegers(0,nRBs-1),getDMRSPattern(1, 4)); 
        }
    }
    else
    {
        double N0=mcell.mySys.channelCloud.getNoiseVariancePerDimension(mcell.associationInfo.serverNodes(gNBIndx))*2.0;
        for(int ue_cnt=0;ue_cnt<grantInfo(gNBIndx).length();ue_cnt++)
            intCovPerUE(ue_cnt)=N0*eye_c(mcell.mySys.aNodes(mcell.associationInfo.serverNodes(gNBIndx)).getAntennaCount());
    }

    return channelOutput;
}

DecoderOutput_S processPUSCH(MCell &mcell, cMAC &MACObjs, ChannelOutput_S channelOutput, cmat interferenceCovariance, int slot_cnt,int fftSize, int nSymbols, int nRBs,  SCHGrant_S &ueGrantInfo , bool withActualReception)
{
    DecoderOutput_S decoderOutput;
    if(withActualReception)
    {
        // Getting Ideal Channel
        Array<Array<cvec>> idealChannelTD = channelOutput.channels(find(channelOutput.txNodeIDs,ueGrantInfo.ueID));
        Array<Array<cvec>> idealChannelFD(idealChannelTD.length());
        
        for(int txAntIndx = 0;txAntIndx < idealChannelFD.length();txAntIndx++)
        {
            idealChannelFD(txAntIndx).set_length(idealChannelTD(0).length());
            for(int rxAntIndx = 0;rxAntIndx < idealChannelFD(0).length();rxAntIndx++)
            {
                cvec temp = fft(idealChannelTD(txAntIndx)(rxAntIndx),fftSize);
                temp = fftShift(temp);
                idealChannelFD(txAntIndx)(rxAntIndx) = temp;
            }
        }
        
        double txSubcarrierPowerInWatts = powerControl.getUplinkPowerInWatts(ueGrantInfo.ueID,ueGrantInfo.allocatedRBs.length())/(ueGrantInfo.allocatedRBs.length()*12);
        Array<Array<Array<cvec>>> channelPerAntennaPerSymbol(nSymbols);
        for(int symCnt = 0;symCnt < nSymbols;symCnt++){
            channelPerAntennaPerSymbol(symCnt).set_length(idealChannelFD(0).length());
            for(int rxAntIndx = 0;rxAntIndx < idealChannelFD(0).length();rxAntIndx++){
                channelPerAntennaPerSymbol(symCnt)(rxAntIndx).set_length(idealChannelFD.length());
                for(int txAntIndx = 0;txAntIndx < idealChannelFD.length();txAntIndx++){
                    channelPerAntennaPerSymbol(symCnt)(rxAntIndx)(txAntIndx) = idealChannelFD(txAntIndx)(rxAntIndx)*dComplex(sqrt(txSubcarrierPowerInWatts),0.0);
                }
            }
        }
        
        Array<cmat> resourceGridPerRxAntenna = getFreqDomainFrame(channelOutput.data,fftSize,0,nSymbols);
        
        
        FadingParameters_S fadingParam = mcell.mySys.channelCloud.getLinkSpecificFadingParameters(ueGrantInfo.gNBID,ueGrantInfo.ueID);
        double dopplerSpreadInHz = mcell.mySys.channelCloud.getDopplerSpreadInHz(ueGrantInfo.gNBID,ueGrantInfo.ueID);
        
        CodingParameters codingParam;
        codingParam.load(ueGrantInfo.harqInfo.TBSizePerCodewordInBits, ueGrantInfo.modulationOrderPercw, ueGrantInfo.codeRatePercw, ueGrantInfo.nREsAllocated, ueGrantInfo.nLayers,ueGrantInfo.harqInfo.infoBitsperCodeWord,ueGrantInfo.harqInfo.rvIndicesPerCodeword);
        decoderOutput = processSCH(resourceGridPerRxAntenna, ueGrantInfo.harqInfo.harqBuffer, interferenceCovariance , slot_cnt, codingParam, fadingParam, dopplerSpreadInHz, nRBs, ueGrantInfo.allocatedRBs , 1,channelPerAntennaPerSymbol, ueGrantInfo.precoderPerRB);
        cout<<"Decode status : "<<decoderOutput.isSuccess<<" for "<<ueGrantInfo.ueID<<endl;
        
        int ueIndx = find(MACObjs.attachedUEids,ueGrantInfo.ueID);
        MACObjs.harqManager(ueIndx)->setReceptionStatus(decoderOutput.isSuccess,decoderOutput.isSuccessPerCB , ueGrantInfo.harqInfo.harqProcessID, ueGrantInfo.harqInfo.harqBuffer);
    }   
    else
    {
        decoderOutput.isSuccess=randu(ueGrantInfo.harqInfo.TBSizePerCodewordInBits.length())>0.1;
        int ueIndx = find(MACObjs.attachedUEids,ueGrantInfo.ueID);
        MACObjs.harqManager(ueIndx)->setReceptionStatus(decoderOutput.isSuccess,decoderOutput.isSuccessPerCB , ueGrantInfo.harqInfo.harqProcessID);
    }
    
    return decoderOutput;
}
