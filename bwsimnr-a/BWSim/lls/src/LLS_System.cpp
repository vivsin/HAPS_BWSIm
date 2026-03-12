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

#include "../include/LLS_System.h"

void LLS_System::initSystem()
{
    string configFileDir = "./configFiles/";
    string toExec =  "rm -rf "+configFileDir+"; mkdir "+configFileDir+"; cp -r ../lls/configFiles/* "+configFileDir+";"; 
    system_exec(toExec.c_str());
    
    parseSimulationParameters();
    
    
    
    
 
    bool enableAAS = isCDLModel(simParam.channelParam.fadingModel);

    // Considering first entry in AAS as BS and second as UE. Need to change for UL - Dhiv
    loadAntennaParam(txAntenna,simParam.antennaParam,0,simParam.nuParam.carrierFrequencyInHz,enableAAS);
    loadAntennaParam(rxAntenna,simParam.antennaParam,1,simParam.nuParam.carrierFrequencyInHz,enableAAS);
    
    vec vel = getVectorWithNumbers(3,simParam.channelParam.velocity,randu()*360.0,90.0);
    
    ueVelocity = zeros(3,1);
    ueVelocity.set_col(0,vel);
    
    int nTRPs=1;
    channel.set_length(nTRPs);
    
    for(int i=0;i<nTRPs;i++)
        channel(i).initChannel(simParam.channelParam.fadingModel,simParam.nuParam.carrierFrequencyInHz,simParam.channelParam.delaySpread,ueVelocity,&txAntenna,&rxAntenna);
    
}

void LLS_System::parseSimulationParameters(string simFile,string AASFile)
{
    string fileName = (simFile=="") ? "./configFiles/mySimulationConfig.txt" : simFile;
    
    simParam.channelParam.load(fileName);
    simParam.nuParam.load(fileName);
 
    randomizeSeed();
    unsigned int simulationSeed=(unsigned int)randi(1,99999);
    
    
    bool useFixedSeed;
    if(!parse(fileName,"useFixedSeed",useFixedSeed))
        useFixedSeed=false;
    
    if(useFixedSeed)
        if(!parse(fileName,"seed",simulationSeed))
        {
            simulationSeed=12345;//(unsigned int)randi(0,99999);
            cout<<"[detl:]Loading default Fixed simulation seed as "<<simulationSeed<<endl;
        }
          
    
    if(simulationSeed==0)
        simulationSeed=(unsigned int)randi(1,99999);
    
    cout<<"[detl:]Using "<<simulationSeed<<" as Simulation Seed."<<endl;
    setSeed(simulationSeed);
    
        
    string antennaFile = (AASFile=="") ? "./configFiles/ActiveAntennaSystemConfig.txt" : AASFile;
    parseAntennaParams(simParam.antennaParam,antennaFile);
    
    simParam.load(fileName);
    
    
    
}

void applyAntennaCorrelation(Array<Array<cvec>>& channel,cmat txAntennaCorrelationMatrix,cmat rxAntennaCorrelationMatrix)
{
    int rxAntennaCount = channel.length();
    int txAntennaCount = channel(0).length();
    
    cmat instantChannel = zeros_c(rxAntennaCount,txAntennaCount);
    for(int channelSample_cnt=0;channelSample_cnt<channel(0)(0).length();channelSample_cnt++)
    {
        for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
            for(int txAntenna_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
                instantChannel(rxAntenna_cnt,txAntenna_cnt)=channel(rxAntenna_cnt)(txAntenna_cnt).get(channelSample_cnt);
            
            instantChannel = sqrtm(rxAntennaCorrelationMatrix)*instantChannel*transpose(sqrtm(txAntennaCorrelationMatrix));
        
        for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
            for(int txAntenna_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
                channel(rxAntenna_cnt)(txAntenna_cnt).set(channelSample_cnt,instantChannel(rxAntenna_cnt,txAntenna_cnt));
    }
}

Array<Array<cvec>> LLS_System:: generateChannelForAllAntennas(double currentTimeInSeconds,bool isTimeDomain,int channelIndx)
{
    int nTxAntenna = txAntenna.numTxRUs,nRxAntenna = rxAntenna.numTxRUs;
    LLS_NUParams_S nuParams = simParam.nuParam;
    Array<Array<cvec>> channelTapsPerPort;
    if(simParam.channelParam.fadingModel==_FADING_CHANNEL_MODEL_AWGN_)
    {
        cvec sampleSpacedChannel(1);
        sampleSpacedChannel(0) = 1;
        append(sampleSpacedChannel,zeros_c(20));
        if(!isTimeDomain)
        {
            sampleSpacedChannel = fft(sampleSpacedChannel,nuParams.fftSize);
        }
        channelTapsPerPort.set_length(nRxAntenna);
        for(int rxAntCnt=0;rxAntCnt<nRxAntenna;rxAntCnt++)
        {
            channelTapsPerPort(rxAntCnt).set_length(nTxAntenna);
            for(int txAntCnt=0;txAntCnt<nTxAntenna;txAntCnt++)
            {
                channelTapsPerPort(rxAntCnt)(txAntCnt) = sampleSpacedChannel;
            }
        }
        return channelTapsPerPort;
    }
    
    double blockageLossPerTxAntenna = 1;
    channelTapsPerPort.set_length(nRxAntenna);
    for(int rxAntCnt=0;rxAntCnt<nRxAntenna;rxAntCnt++)
    {
        channelTapsPerPort(rxAntCnt).set_length(nTxAntenna);
        for(int txAntCnt=0;txAntCnt<nTxAntenna;txAntCnt++)
        {
            channelTapsPerPort(rxAntCnt)(txAntCnt) = blockageLossPerTxAntenna*channel(channelIndx).generateTapWeights(txAntCnt,rxAntCnt,currentTimeInSeconds);
        }
    }
    
    vec tapDelays = channel(channelIndx).getClusterDelays();
    int nTaps = tapDelays.length();
    
    Array<Array<cvec>> channel;
    channel.set_length(nRxAntenna);
    for(int rxAntenna_cnt=0;rxAntenna_cnt<nRxAntenna;rxAntenna_cnt++)
    {
        channel(rxAntenna_cnt).set_length(nTxAntenna);
        for(int txAntenna_cnt=0;txAntenna_cnt<nTxAntenna;txAntenna_cnt++)
        {
            cvec myChannel;
            myChannel = zeros_c(floor_i(tapDelays(tapDelays.length()-1)*nuParams.samplingFrequencyInHz)+1);
            
            int channelIndx=0;
            for(int tap_cnt=0;tap_cnt<nTaps;tap_cnt++)
            {
                channelIndx=floor_i(tapDelays(tap_cnt)*nuParams.samplingFrequencyInHz);
                if(channelIndx>=myChannel.length())
                {
                    cout<<"[both:] Insufficient Channel length to represent the Channel Model at the given sampling Frequency."<<endl;
                    abort();
                }
                myChannel(channelIndx) = myChannel(channelIndx) + channelTapsPerPort(rxAntenna_cnt)(txAntenna_cnt)(tap_cnt);
            }
            channel(rxAntenna_cnt)(txAntenna_cnt) = myChannel;
        }
    }
    
    if(!isCDLModel(simParam.channelParam.fadingModel))
        applyAntennaCorrelation(channel,txAntenna.getAntennaCorrMatrix(),rxAntenna.getAntennaCorrMatrix());
    
    if(!isTimeDomain)
    {
        for(int rxAntenna_cnt=0;rxAntenna_cnt<nRxAntenna;rxAntenna_cnt++)
        {
            for(int txAntenna_cnt=0;txAntenna_cnt<nTxAntenna;txAntenna_cnt++)
            {
                cvec myChannel = channel(rxAntenna_cnt)(txAntenna_cnt);
                myChannel = fft(myChannel,nuParams.fftSize); 
                channel(rxAntenna_cnt)(txAntenna_cnt) = myChannel;
            }
        }
    }
    
    return channel;
}

double LLS_System::getDopplerSpreadInHz()
{
    vec relVelocity = ueVelocity.get_col(0);
//     double relSpeed = sqrt(relVelocity*relVelocity);
    double relSpeed = relVelocity(0);
    return relSpeed*simParam.nuParam.carrierFrequencyInHz/lightSpeed;
}

Array<cmat> LLS_System::passThroughChannel(Array<cmat>& txGridWithCP,Array<Array<Array<cvec>>>& channelPerAntennaPerSymbol,double N0,double currentTimeInSeconds,int channelIndx)
{
    int nTxAntenna = txAntenna.numTxRUs,nRxAntenna = rxAntenna.numTxRUs;
    LLS_NUParams_S nuParams = simParam.nuParam;
    
    channelPerAntennaPerSymbol.set_length(nuParams.nSymbolsPerSlot);
    
    bool genChannelPerSymbol = false;
    
    if(!genChannelPerSymbol)
    {
        Array<Array<cvec>> channel = generateChannelForAllAntennas(currentTimeInSeconds,1,channelIndx);
        
        channelPerAntennaPerSymbol = repeatAsArray(channel,nuParams.nSymbolsPerSlot);
    }
    else
    {
        for(int sym_cnt=0;sym_cnt<nuParams.nSymbolsPerSlot;sym_cnt++)
        {
            channelPerAntennaPerSymbol(sym_cnt) = generateChannelForAllAntennas(currentTimeInSeconds+sym_cnt*nuParams.slotDurationInSec/nuParams.nSymbolsPerSlot,1,channelIndx);
        }
    }
    
    // Disable fading
//     for(int sym_cnt=0;sym_cnt<nuParams.nSymbolsPerSlot;sym_cnt++)
//         for(int rxAntCnt=0;rxAntCnt<nRxAntenna;rxAntCnt++)
//             for(int txAntCnt=0;txAntCnt<nTxAntenna;txAntCnt++)
//                 channelPerAntennaPerSymbol(sym_cnt)(rxAntCnt)(txAntCnt) = ones_c(1);
           
    int nTaps = channelPerAntennaPerSymbol(0)(0)(0).length(),nSymbols = txGridWithCP(0).cols(),symlength = txGridWithCP(0).rows();
    
    Array<cmat> rxGridWithCP;rxGridWithCP.set_length(nRxAntenna);
    
    for(int rxAntCnt=0;rxAntCnt<nRxAntenna;rxAntCnt++)
    {
        cmat grid = zeros_c(nTaps+symlength-1,nSymbols);
        for(int txAntCnt=0;txAntCnt<nTxAntenna;txAntCnt++)
        {
            for(int sym_cnt=0;sym_cnt<txGridWithCP(txAntCnt).cols();sym_cnt++)
            {
                cvec data = txGridWithCP(txAntCnt).get_col(sym_cnt);
                cvec rxSymbols = conv(data,channelPerAntennaPerSymbol(sym_cnt)(rxAntCnt)(txAntCnt));
                if(txAntCnt==0)
                {
                    cvec noise = sqrt(N0)*randn_c(rxSymbols.length()); //generateNoiseSamples(N0,rxSymbols.length());
                    rxSymbols += noise;
                }
                rxSymbols += grid.get_col(sym_cnt);
                grid.set_col(sym_cnt,rxSymbols);
                
                channelPerAntennaPerSymbol(sym_cnt)(rxAntCnt)(txAntCnt) = fft(channelPerAntennaPerSymbol(sym_cnt)(rxAntCnt)(txAntCnt),nuParams.fftSize);
            }
        }
        rxGridWithCP(rxAntCnt) = grid;
    }
    return rxGridWithCP;
}

Array<cmat> LLS_System::multipleWithFDChannel(Array<cmat>& txGridInFrequencyDomain,Array<Array<Array<cvec>>>& channelPerAntennaPerSymbol,double N0,double currentTimeInSeconds,int channelIndx)
{
    int nTxAntenna = txAntenna.numTxRUs,nRxAntenna = rxAntenna.numTxRUs;
    LLS_NUParams_S nuParams = simParam.nuParam;
    
    channelPerAntennaPerSymbol.set_length(nuParams.nSymbolsPerSlot);
    
    bool genChannelPerSymbol = false;
    
    if(!genChannelPerSymbol)
    {
        Array<Array<cvec>> channel = generateChannelForAllAntennas(currentTimeInSeconds,1,channelIndx);
        channelPerAntennaPerSymbol = repeatAsArray(channel,nuParams.nSymbolsPerSlot);
    }
    else
    {
        for(int sym_cnt=0;sym_cnt<nuParams.nSymbolsPerSlot;sym_cnt++)
        {
            channelPerAntennaPerSymbol(sym_cnt) = generateChannelForAllAntennas(currentTimeInSeconds+sym_cnt*nuParams.slotDurationInSec/nuParams.nSymbolsPerSlot,1,channelIndx);
        }
    }
    
    int nTaps = channelPerAntennaPerSymbol(0)(0)(0).length(),nSymbols = txGridInFrequencyDomain(0).cols(),symlength = txGridInFrequencyDomain(0).rows();
    
    Array<cmat> rxGridWithCP;rxGridWithCP.set_length(nRxAntenna);
    
    for(int rxAntCnt=0;rxAntCnt<nRxAntenna;rxAntCnt++)
    {
        cmat grid = zeros_c(symlength,nSymbols);
        for(int txAntCnt=0;txAntCnt<nTxAntenna;txAntCnt++)
        {
            for(int sym_cnt=0;sym_cnt<txGridInFrequencyDomain(txAntCnt).cols();sym_cnt++)
            {
                cvec data = txGridInFrequencyDomain(txAntCnt).get_col(sym_cnt);
                channelPerAntennaPerSymbol(sym_cnt)(rxAntCnt)(txAntCnt) = fft(channelPerAntennaPerSymbol(sym_cnt)(rxAntCnt)(txAntCnt),nuParams.fftSize);
//                 if(sym_cnt==0) cout<<"Ch : "<< channelPerAntennaPerSymbol(sym_cnt)(rxAntCnt)(txAntCnt)(0,10)<<endl;
                
//                 channelPerAntennaPerSymbol(sym_cnt)(rxAntCnt)(txAntCnt).ones();
                
                cvec rxSymbols = elem_mult(data,channelPerAntennaPerSymbol(sym_cnt)(rxAntCnt)(txAntCnt));

                if(txAntCnt==0)
                {
                    cvec noise = sqrt(N0)*randn_c(rxSymbols.length()); //generateNoiseSamples(N0,rxSymbols.length());
                    rxSymbols += noise;
                }
                rxSymbols += grid.get_col(sym_cnt);
                grid.set_col(sym_cnt,rxSymbols);
            }
        }
        rxGridWithCP(rxAntCnt) = grid;
    }
    return rxGridWithCP;
}
