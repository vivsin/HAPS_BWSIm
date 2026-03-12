
#ifndef _LS_CSI_H_
#define _LS_CSI_H_


#include "../../../lib/L1/CSIRS/include/csirsTx.h"
#include "../../../lib/L1/CSIRS/include/csirsRx.h"
#include "../../../lib/L1/SRS/include/srsTx.h"
#include "../../../lib/L1/SRS/include/srsRx.h"
#include "../../Mcell/include/MCell.h"
#include "../../../lib/L1/SRS/include/srsTx.h"

struct SystemParams_S
{
    int nSlots;
    int nSymbols;
    int fftSize;
    int scsInKhz;
    int RBGSize;
    int nRBs;
    int nRBGs;
    int nREsPerRBG;
    int csiFrequency;
    
    SystemParams_S(int slots,int symbols,int fftsize, int scsinkhz,int rbgSize,int rbs,int csiFreq)
    {
        nSlots=slots;
        nSymbols = symbols;
        fftSize = fftsize;
        scsInKhz = scsinkhz;
        RBGSize = rbgSize;
        nRBs = rbs;
        nRBGs = ceil_i((double)nRBs/(double)RBGSize);
        nREsPerRBG = 12*nSymbols*RBGSize;
        csiFrequency = csiFreq;    
    }
};

void fTransmitCSIRS_DL(SystemParams_S sysParams,int slotIndx,MCell &mcell, double txPowerIndBm);
void fTransmitSRS_UL(SystemParams_S sysParams,int slotIndx,MCell &mcell, Array<srsConfig> &srsconfig);

Array<CSI_Feedback_Info_S> fReturnCurrentMCSperUE_DL(SystemParams_S sysParams,int slotIndx,MCell &mcell,ivec uesForMCS, double txPowerIndBm, Array<cmat> intCovPerUE="");
Array<CSI_Feedback_Info_S> fReturnCurrentMCSperUERel19(SystemParams_S sysParams,int slotIndx,MCell &mcell,ivec uesForMCS, double txPowerIndBm);

Array<CSI_Feedback_Info_S> fReturnCurrentMCSperUE_UL(SystemParams_S sysParams,int slotIndx,MCell &mcell,ivec gNBsForMCS, Array<srsConfig> &srsconfig);

#endif

