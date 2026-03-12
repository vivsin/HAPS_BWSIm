
#ifndef _PDSCH_H_
#define _PDSCH_H_

#include "../../../lib/Frozen/simSupport/include/simSupport.h"


#include "../../../lib/L1/SCH/include/schTx.h"
#include "../../../lib/L1/SCH/include/schRx.h"
#include "../../Mcell/include/MCell.h"

#include "../../../lib/L2/MAC/include/MAC.h"


struct NRTables_S
{
    cTBTables tbTables;
    AWGNTablesConvCoder awgnTables;

    void init()
    {
        bool isDownlink=true;
        string dirPrefix = "./configFiles/NU1/LTETables/";
        awgnTables.init_AWGNTables(isDownlink,dirPrefix+"transportBlockMCSMapping.txt",dirPrefix+"cqiToMaxMCSRate.txt");
        tbTables.load(isDownlink,dirPrefix+"interleaverParameters.txt",dirPrefix+"transportBlockMCSMapping.txt",dirPrefix+"tbsTranslation.txt",dirPrefix+"cqiToMaxMCSRate.txt",awgnTables);
    }
};


void transmitPDSCH(MCell &mcell, Array<cMAC> &MACObjs, int slot_cnt,int nSubcarriers, int nSymbols, int nRBs, double txPowerIndBm , Array<Array<SCHGrant_S>> &grantInfo, int ngNBsWithActualReception=0);
DecoderOutput_S receiveAndProcessPDSCH(MCell &mcell, cMAC &MACObjs, int slot_cnt,int nSubcarriers, int nSymbols, int nRBs, double txPowerIndBm , SCHGrant_S &ueGrantInfo, bool withActualReception=true);

Array<cmat> calculateWBIntCovPerUE(MCell &mcell, int slot_cnt,int nSubcarriers, int nSymbols, int nRBs, ivec uesForMCS);

void transmitPUSCH(MCell &mcell, Array<cMAC> &MACObjs, int slot_cnt,int fftSize, int nSymbols, int nRBs, Array<Array<SCHGrant_S>> &grantInfo, int ngNBsWithActualReception=0);
ChannelOutput_S receivePUSCH(MCell &mcell, int gNBIndx, int slot_cnt,int fftSize, int nSymbols, int nRBs, Array<Array<SCHGrant_S>> &grantInfo, Array<cmat> &intCovPerUE);
DecoderOutput_S processPUSCH(MCell& mcell, cMAC& MACObjs, ChannelOutput_S channelOutput, itpp::cmat interferenceCovariance, int slot_cnt, int fftSize, int nSymbols, int nRBs, SCHGrant_S &ueGrantInfo, bool withActualReception);

#endif

