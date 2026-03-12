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

#ifndef _SF_mcell_H_
#define _SF_mcell_H_

#include <lib/Frozen/mcell/include/System.h>

#ifdef USING_MPI
#include "../../MPI/include/MpiSupport.h"
#endif 


struct CarrierInfo_S{
    
    vec carrierFreqInMHzPerServerNodeType;
    double carrierBandwidthInMHz;
    double samplingFrequencyInMHz;
    
    CarrierInfo_S(){
        
        carrierFreqInMHzPerServerNodeType="";
        carrierBandwidthInMHz=-1;
        samplingFrequencyInMHz=-1;
    }
    
    CarrierInfo_S(string configFileName){
        
        parse(configFileName,"carrierFreqInMHzPerServerNodeType",carrierFreqInMHzPerServerNodeType);
        parse(configFileName,"carrierBandwidthInMHz",carrierBandwidthInMHz);
        parse(configFileName,"samplingFrequencyInMHz",samplingFrequencyInMHz);
    }   
};
void associateNodes(AssociationInfo_S& associationInfo,string linkTableFileName,CarrierInfo_S carrierInfo,int strongInterfererCount, Array<ivec> &strongServerNodesPerServiceNode,Array<string> filenames,int maximumAssociationPerServerNode=-1, InitialAssociationMetric initialAssociatioMetric=_UNINIT_);
void printAssociationLogs(mat& couplingGainInDB,mat& downlinkPowerMatrixInDBm,AssociationInfo_S &associationInfo,vec serviceNodeNoiseVariance,string fileName,McellSystem* mySysPtr,bool isBeam=false);
void printDetailedAssociationLogs(mat& couplingGain,mat& downlinkPowerMatrixInDBm,AssociationInfo_S &associationInfo,vec serviceNodeNoiseVariance,string fileName,bool isBeam=false);
void computeRSRPforAllNodes(McellSystem& mySys,AssociationInfo_S& associationInfo,InitialAssociationMetric associationMetric,mat& couplingGainInDB,mat& RSRPindB,string beamRSRPfileName,string linkTableFileName);
void computeMaxRSRPforAllNodes(McellSystem& mySys,AssociationInfo_S& associationInfo,InitialAssociationMetric associationMetric,mat& couplingGainInDB,mat& RSRPindB,string beamRSRPfileName,string linkTableFileName);
void printLinkGain(string linkGainFileName,string linkLogsFileName,AssociationInfo_S& associationInfo,ChannelCloud* channelCloud);
void printLinkLogs(McellSystem* mySysPtr,AssociationInfo_S* associationInfoPtr,mat couplingGain,mat RSRPinDB,vec serviceNodeReceiveNoiseVariance,string fileName);
void printClusterAngleSpread(ChannelCloud* channelCloud,AssociationInfo_S* associationInfoPtr,string fileName);
void printSCMparams(McellSystem* mySysPtr,ivec serviceNodes,ivec serverNodes,string filename);
// void printSingularValues(ChannelCloud* channelCloud,AssociationInfo_S& associationInfo,NUParams_S* nuParams,string fileName);
void printAngleSpread(ChannelCloud* channelCloud,AssociationInfo_S& associationInfo,string fileName,string clusterSpreadFileName);
void printCouplingGain(AssociationInfo_S& associationInfo,mat linkGainInDB,string filename,bool isBeam=false);
void printSINRDistribution(AssociationInfo_S& associationInfo,mat downlinkPowerMatrixInDBm,vec serviceNodeNoiseVariance,string filename,bool isBeam=false);

#endif
