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

#ifndef M_CELL_H
#define M_CELL_H

#include "FadingChannel.h"
#include "SupportingFunctions.h"

#include "../../MPI/include/MpiSupport.h"

bvec selectNodesforSimulation(string sysConfigFileName,bool isWithPicos=false, bool isWithRelays=false);
void generateMyMultiCell(McellSystem &mySys, ConfigFileNames_S myConfigFiles , ResultFileNames_S resultFiles, unsigned int simulationSeed);
void assignIDCell(McellSystem &mySys);
void printTDDConfig(string linkTableFileName, string simulationConfigFileName);
void assignNodeVelocity(McellSystem &mySys,string sysConfigFileName);
void initMcellForLinkSimulation(McellSystem &mySys,string sysConfigFileName,string AASFileName);
void generateLinkTableAndChannelCloudPerCarrier(McellSystem &mySys, AssociationInfo_S &associationInfo, ConfigFileNames_S myConfigFiles , ResultFileNames_S resultFiles);

class MCell {
    
public :
    McellSystem mySys;
    AssociationInfo_S associationInfo;
    
    MCell() { }
};


MCell createMcellNetwork(ConfigFileNames_S myConfigFiles , ResultFileNames_S resultFiles, unsigned int simSeed = 0);
void performBeamBasedAssociation(McellSystem& mySys, AssociationInfo_S& associationInfo, int nRBs, string linkTableFileName);

#endif
