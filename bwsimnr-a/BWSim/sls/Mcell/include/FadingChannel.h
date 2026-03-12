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

#ifndef _FADING_CHANNEL_H_
#define _FADING_CHANNEL_H_

#include <lib/Frozen/mcell/include/System.h>
#ifdef USING_MPI
#include "../../MPI/include/MpiSupport.h"
#endif

void syncServiceNodesWithAssociatedServerNodes(ChannelCloud &channelCloud,AssociationInfo_S &associationInfo);
//void loadDoppler(ChannelCloud &myChannel,ivec &serviceNodeList,vec &serviceNodeDopplerInHz);
void loadLinkSpecificChannelInfo(string linkTableFileName,bool isFixedVelocity,ChannelCloud &myChannel,ivec serverNodeList,ivec &serviceNodeList,SCMPropagationScenario scmScenario=_SCM_UNINIT_);
void initSCMLink(McellSystem &mySys, ivec& serviceNodeIDs, Array< ivec >& serverNodeIDsPerServiceNode,bool initializeReverseLink , bool maintainReciprocity, bool reinitialize=false);
bool isSCMLinksToBeInitialized(ChannelCloud& channelCloud, ivec& rxNodeIDs, Array< ivec >& txNodeIDsPerRxNode);
void retainSCMLinks(ChannelCloud& channelCloud, ivec& rxNodeIDs, Array< ivec >& txNodeIDs);
void createChannelCloud(McellSystem &mySys , ConfigFileNames_S myConfigFiles , ResultFileNames_S resultFiles, double samplingFreqInHz);
    
void printBeamManagementInfo(string filename,string nodeType,Antenna* antenna,double carrierFrequencyInHz);
 void printAntennaLocations(string aasFolderName,int nodeType,double carrierFrequencyInHz,Location_S loc,Antenna* antennaPtr);
#endif
