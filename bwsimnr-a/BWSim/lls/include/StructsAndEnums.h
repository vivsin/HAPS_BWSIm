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

#ifndef _LLS_SAE_H_
#define _LLS_SAE_H_

#include "../../lib/Frozen/simSupport/include/simSupport.h"
#include "../../lib/Frozen/mcell/include/Channel.h"
#include "../../lib/Frozen/mcell/include/Antenna.h"


struct AntennaParameters
{
    ivec numTXRUs;
    Array<ivec> antStructure,TXRUMap,beamMap;
    Array<Array<vec>> azimuthBeamAngles,zenithBeamAngles;
    Array<vec> antSpacing;
    Array<vec> crossPolarizationAngles;
    vec coPolarizationSlantAngle;
    Array<string> antennaType;
    vec hMaxAtt,vMaxAtt,hBW,vBW;
    vec feederLoss,antennaGain;
    vec antCorrValues;
};

struct LLS_NUParams_S
{  
    int nuID;
    double subCarrierSpacingInHz;
    double slotDurationInSec;
    
    double carrierFrequencyInHz;
    double samplingFrequencyInHz;
    int subCarrierCountPerRB;
    
    double bandwidthInHz;// So fill this bandwidth
    
    int fftSize;
    int dlNRBs;
    int ulNRBs;
    int prgSize;
    int nSymbolsPerSlot;
    
    LLS_NUParams_S()
    {
        nuID=-1;
        subCarrierSpacingInHz=-1;
        samplingFrequencyInHz=-1;
        slotDurationInSec=-1;
        carrierFrequencyInHz=-1;
        fftSize=-1;
        dlNRBs=-1;
        ulNRBs=-1;
        subCarrierCountPerRB=-1;
        prgSize=-1;
        nSymbolsPerSlot = -1;
    }
    void load(string fileName)
    {
        double scs,samplingFrequency,bandwidthInMHz,carrierFrequency;
        parse(fileName,"samplingFrequencyInMHz",samplingFrequency);
        parse(fileName,"subCarrierSpacingInKHz",scs);
        parse(fileName,"bandwidthInMHz",bandwidthInMHz);
        parse(fileName,"carrierFrequencyInMHz",carrierFrequency);
        double usableBandwidth;
        parse(fileName,"usableBW",usableBandwidth);
        parse(fileName,"prgSize",prgSize);
        samplingFrequencyInHz = samplingFrequency*1e6;
        subCarrierSpacingInHz = scs*1e3;
        fftSize = round_i(samplingFrequency*1e3/scs);
        bandwidthInHz = bandwidthInMHz*1e6;
        carrierFrequencyInHz = carrierFrequency*1e6;
        fftSize = int(samplingFrequencyInHz/subCarrierSpacingInHz);
        nuID = int(subCarrierSpacingInHz/15e3);
        subCarrierCountPerRB = 12;
        dlNRBs = round_i(bandwidthInHz*usableBandwidth/(12*subCarrierSpacingInHz));
        ulNRBs = dlNRBs;
        slotDurationInSec = 15.0/subCarrierSpacingInHz;
        nSymbolsPerSlot = 14;
        if(prgSize>dlNRBs) prgSize = dlNRBs;
        if(currentTaskid==0)
        {
            cout<<"\nSampling Frequency: "<<samplingFrequency<<" MHz"<<endl;
            cout<<"Carrier Frequency: "<<carrierFrequency*1e-3<<" GHz"<<endl;
            cout<<"Bandwidth: "<<bandwidthInMHz<<" MHz"<<endl;
            cout<<"Sub-carrier Spacing: "<<scs<<" KHz"<<endl;
            cout<<"FFT size: "<<fftSize<<endl;
            cout<<"nRBs: "<<dlNRBs<<endl;
            cout<<"prgSize: "<<prgSize<<endl;
        }
    }
};

struct ChannelParameters
{
    double delaySpread;
    FadingChannelModel_E fadingModel;
    double velocity;
    double rxNoiseFigureInDB;
    double txPowerInDBm;
    double blockageProb;
    double powerOffSetForBlockageInDB;
    ChannelParameters()
    {
        delaySpread = 100e-9;
        fadingModel = _FADING_CHANNEL_MODEL_TDL_A_;
        velocity = 0.833;
        blockageProb = 0.1;
        powerOffSetForBlockageInDB = 10;
    }
    void load(string fileName)
    {
        string channelModel;
        parse(fileName,"fadingChannelModel",channelModel);
        fadingModel = parseChannelModel(channelModel);
        parse(fileName,"delaySpread",delaySpread);
        delaySpread *=1e-9;
        parse(fileName,"ueVelocity",velocity);
        parse(fileName,"nodeReceiveNoiseFigureInDB",rxNoiseFigureInDB);
        parse(fileName,"nodeTransmitPowerInDBm",txPowerInDBm);
        parse(fileName,"blockageProbability",blockageProb);
        parse(fileName,"powerOffSetForBlockageInDB",powerOffSetForBlockageInDB);
        
        if(currentTaskid==0)
        {
            cout<<"\nChannel Model: "<<channelModel<<endl;
            cout<<"Delay Spread: "<<delaySpread*1e9<<" ns"<<endl;
            cout<<"UE velocity: "<<velocity<<" m/s"<<endl;
            cout<<"blockageProb: "<<blockageProb<<endl;
            cout<<"powerOffSetForBlockageInDB: "<<powerOffSetForBlockageInDB<<endl;
        }
    }
};


struct SimulationParameters
{
    ChannelParameters channelParam;
    AntennaParameters antennaParam;
    LLS_NUParams_S nuParam;
    int nSlots;
    int nPacketsPerSlot;
    vec snr;
    bool useChannelEstimation;
    SimulationParameters()
    {
        nSlots = 50;
        nPacketsPerSlot=10;
        useChannelEstimation = false;
    }
    void load(string fileName)
    {
        double minSNR,maxSNR,step;
        parse(fileName,"minimumSNR",minSNR);
        parse(fileName,"maximumSNR",maxSNR);
        parse(fileName,"stepSizeForSNR",step);
        snr =  getVector(minSNR,maxSNR,step);
        
        double simTime;
        parse(fileName,"simulationTimeInSec",simTime);
        nSlots = round_i(simTime/nuParam.slotDurationInSec);
        
        parse(fileName,"nPacketsPerSlot",nPacketsPerSlot);
        if(currentTaskid==0)
        {
            cout<<"\nGiven SNR values: "<<snr<<endl;
            cout<<"nSlots: "<<nSlots<<endl;
        }
        parse(fileName,"useChannelEstimation",useChannelEstimation);
    }
};
#endif
