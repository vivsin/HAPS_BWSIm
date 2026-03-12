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
#include "../include/SupportingFunctions.h"

cvec generateNoiseSamples(double N0,int nSamples)
{
    cvec noise = sqrt(N0)*randn_c(nSamples);
    return noise;
}

void parseAntennaParams(AntennaParameters& antParam,string fileName)
{
//     parse(fileName,"numTxRUs",antParam.numTXRUs);
    parse(fileName,"TXRUMap",antParam.TXRUMap);
    parse(fileName,"beamMap",antParam.beamMap);
    parse(fileName,"crossPolarizationAngles",antParam.crossPolarizationAngles);
    parse(fileName,"AntStructure",antParam.antStructure);
    
    antParam.numTXRUs.set_length(antParam.TXRUMap.length());
    for(int nodeTypeCnt = 0;nodeTypeCnt < antParam.TXRUMap.length();nodeTypeCnt++){
        antParam.numTXRUs(nodeTypeCnt) = prod(antParam.TXRUMap(nodeTypeCnt))*antParam.antStructure(nodeTypeCnt)(4);
    }
    
    parse(fileName,"AntSpacing",antParam.antSpacing);
    parse(fileName,"zenithBeamAngles",antParam.zenithBeamAngles);
    parse(fileName,"azimuthBeamAngles",antParam.azimuthBeamAngles);
    parse(fileName,"coPolarizationSlantAngle",antParam.coPolarizationSlantAngle);
    
    parse(fileName,"nodeAntennaType",antParam.antennaType); 
    parse(fileName,"nodeAntennaHorizontalBeamWidth",antParam.hBW);               
    parse(fileName,"nodeAntennaVerticalBeamWidth",antParam.vBW);
    if(!parse(fileName,"nodeAntGainIndB",antParam.antennaGain))
    {
        cout<<"[detl:]Incorrect macroAntGainIndB value in mySysConfig... Aborting... "<<endl;
        abort();
    }
    parse(fileName,"nodeAntennaHorizontalMaxAttenuation",antParam.hMaxAtt);
    parse(fileName,"nodeAntennaVerticalMaxAttenuation",antParam.vMaxAtt);
    parse(fileName,"antennaFeederLossIndB",antParam.feederLoss);
    
    string antCorrType;
    parse(fileName,"antennaCorrelation",antCorrType);
    if(antCorrType=="_ANTENNA_CORRELATION_HIGH_")
        antParam.antCorrValues = 0.9*ones(2);
    else if(antCorrType=="_ANTENNA_CORRELATION_MEDIUM_")
        antParam.antCorrValues = getVectorWithNumbers(2,0.9,0.3);
    else if(antCorrType=="_ANTENNA_CORRELATION_LOW_" || antCorrType=="_ANTENNA_CORRELATION_UNCORRELATED_")
        antParam.antCorrValues = zeros(2);
    else
    {
        cout<<"Unknown antenna correlation type..."<<endl;abort();
    }
    if(currentTaskid==0)
    {
        cout<<"Antenna Correlation Type: "<<antCorrType<<endl;
    }
}

void loadAntennaParam(Antenna& antenna,AntennaParameters& antParam,int index,double carrierFrequency,bool enableAAS)
{
    if(enableAAS)
    {
        antenna.loadAASInfo(antParam.antStructure(index),antParam.TXRUMap(index),antParam.antSpacing(index),antParam.zenithBeamAngles(index),antParam.azimuthBeamAngles(index));
        antenna.setPolarisationAngles(antParam.crossPolarizationAngles(index),antParam.coPolarizationSlantAngle(index));
        if(antenna.numTxRUs!=antParam.numTXRUs(index))
        {
            cout<<"Improper TXRUMap.."<<endl;exit(0);
        }
    }
    else 
        antenna.numTxRUs = antParam.numTXRUs(index);
    antenna.setAntenna(getAntennaType(antParam.antennaType(index)),antParam.antennaGain(index),antParam.hMaxAtt(index),antParam.vMaxAtt(index),0,90,antParam.hBW(index),antParam.vBW(index),antParam.feederLoss(index));
    antenna.computeAntennaCorrMatrix(antParam.antCorrValues(index));
}
