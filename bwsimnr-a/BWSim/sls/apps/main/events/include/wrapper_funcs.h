
#ifndef _WRAPFUNC_H_
#define  _WRAPFUNC_H_

#include "../../../../sim/include/BWSimSystem.h"
#include <functional>
#include "events.h"


// void initLinkSimulation_event(BWSimSystem_S& bwsimSystem);
void nodeUpdate_event(BWSimSystem_S& bwsimSystem, ivec serverNodes, int nuID=0);
void nodeProcess_event(BWSimSystem_S& bwsimSystem, ivec rxNodeIDs, double currentTime, int nuID,ivec rxBeamIDs);
void nodeReceive_event(BWSimSystem_S& bwsimSystem, ivec rxNodeIDs, int nuID, double currentTime, ivec rxBeamIDs);
void nodeTransmit_event(BWSimSystem_S& bwsimSystem, ivec txNodeIDs, int nuCaID, ivec txBeamIDs);
void nodeGenerateData_event(BWSimSystem_S& bwsimSystem, ivec txNodeIDs, double currentTime, int nuID, ivec beamID);
void nodeHARQReport_event(BWSimSystem_S& bwsimSystem, ivec rxNodeIDs, double currentTime, int nuID,ivec rxBeamIDs);
void nodeCQIReport_event(BWSimSystem_S& bwsimSystem, int rxNodeID, ivec validServiceNodes, double currentTime, int nuID);
void nodePostSchedule_event(BWSimSystem_S& bwsimSystem, ivec serverNodes, double currentTime, int nuID,int beamID);
void nodeExchangeMPIData_event(BWSimSystem_S& bwsimSystem, int nuCaID, int beamID);
void nodeSchedule_event(BWSimSystem_S& bwsimSystem, ivec serverNodes, double currentTime, int nuID,int beamID);
void initLinkSimulation_event(BWSimSystem_S& bwsimSystem);
void initializeEvents(BWSimSystem_S& bwsimSystem);


//SSB Events
void nodeTransmitSSB_event(BWSimSystem_S& bwsimSystem , ivec txNodeIDs,ivec txBeamIDs,ivec txPanelIDs, double currentTime);
void nodeReceiveSSB_event(BWSimSystem_S& bwsimSystem , ivec rxNodeIDs,double currentTime);

    

#endif
