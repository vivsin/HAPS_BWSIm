#ifndef _CSIRSTX_H_
#define _CSIRSTX_H_

/*************************************************************************
 * 
 * CEWiT CONFIDENTIAL
 * __________________
 *
 * All Rights Reserved © 2014 CEWiT, India
 *
 * \ NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
 * and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
 * Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
 * express, printed and signed license for use is strictly forbidden.
 */

#include "../../PHY/include/Codebook.h"
#include "../../PHY/include/ReferenceSignalGenerator.h"


struct CSIRS_output
{
    int nRBsAllocated;
    int nCDM;
    Array<cvec> csirsPerPort;
    imat rbRSPattern;
};

CSIRS_output generateCSIRS(int slotIndex,int dlNRBs,int nCSIRSPorts, int cellID);
Array<cmat> fillCSIRS(CSIRS_output csirs_output, int fftSize);
Array<cmat> precodeAndFillCSIRS(CSIRS_output csirs_output,CodeBookConfig_S dlCodeBookConfig,Array<type1PMI_S> widebandPMI,int trpIndx,bool precodeCSIRS);


#endif










