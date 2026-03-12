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

#include "../include/csirsTx.h"


CSIRS_output generateCSIRS(int slotIndex,int dlNRBs,int nCSIRSPorts, int cellID)
{
    int FDCDM = 2,TDCDM = (nCSIRSPorts>8) ?  2 : 1;
    
    Array<ivec> allocatedRBs(1);
    allocatedRBs(0) = getIntegers(0,dlNRBs-1);
    
    CSIRS_output csirs_output;
    csirs_output.nRBsAllocated = dlNRBs;
    csirs_output.nCDM = FDCDM*TDCDM;
    csirs_output.rbRSPattern = "0 0 3400 3408 0 0 0 3416 3424 0 0 0 0 0;"
        "0 0 3400 3408 0 0 0 3416 3424 0 0 0 0 0;"
        "0 0    0    0 0 0 0    0    0 0 0 0 0 0;"
        "0 0    0    0 0 0 0    0    0 0 0 0 0 0;"
        "0 0 3402 3410 0 0 0 3418 3426 0 0 0 0 0;"
        "0 0 3402 3410 0 0 0 3418 3426 0 0 0 0 0;"
        "0 0    0    0 0 0 0    0    0 0 0 0 0 0;"
        "0 0    0    0 0 0 0    0    0 0 0 0 0 0;"
        "0 0 3404 3412 0 0 0 3420 3428 0 0 0 0 0;"
        "0 0 3404 3412 0 0 0 3420 3428 0 0 0 0 0;"
        "0 0 3406 3414 0 0 0 3422 3430 0 0 0 0 0;"
        "0 0 3406 3414 0 0 0 3422 3430 0 0 0 0 0";
    
    ivec csirsPorts = 3400 + getIntegers(0,nCSIRSPorts-1);
    
    csirs_output.csirsPerPort.set_length(nCSIRSPorts);
    for(int port_cnt=0;port_cnt<nCSIRSPorts;port_cnt++)
    {
        int symIndex = (port_cnt<4) ? 2 : (port_cnt<8) ? 3 : (port_cnt<12) ? 7 : 8;
        int symno=0;
        if((port_cnt>=8 && port_cnt<=15) || (port_cnt>=24 && port_cnt<=31)) symno=1;
        csirs_output.csirsPerPort(port_cnt) = getNRCSIRSsequence(cellID,symIndex,slotIndex,dlNRBs,2,symno,FDCDM,TDCDM,csirsPorts(port_cnt));
    }
    switch(nCSIRSPorts)
    {
        case 1:
        case 2:
        case 4:
        case 8:
            if(nCSIRSPorts<=2)
            {
                csirs_output.rbRSPattern.set_col(2,"3400 3400 0 0 0 0 0 0 0 0 0 0");
            }
            else if(nCSIRSPorts==4)
            {
                csirs_output.rbRSPattern.set_col(2,"3400 3400 0 0 3402 3402 0 0 0 0 0 0");
            }
            csirs_output.rbRSPattern.set_col(3,zeros_i(12));
        case 12:
            if(nCSIRSPorts==12)
                csirs_output.rbRSPattern.set_col(3,"3408 3408 0 0 3410 3410 0 0 0 0 0 0");
        case 16:
            csirs_output.rbRSPattern.set_col(7,zeros_i(12));
        case 24:
            csirs_output.rbRSPattern.set_col(8,zeros_i(12));
            break;
    }
//     cout<<"csirs_output.rbRSPattern : "<<csirs_output.rbRSPattern<<endl;
    return csirs_output;
}

int getCSIRSPortReferenceNumber(int portNumber)
{
    return portNumber - portNumber%2;
}


Array<cmat> fillCSIRS(CSIRS_output csirs_output, int fftSize)
{
 
    int nSymbolsPerSlot = 14;
    int subCarrierCountPerRB = 12;
    
    int nCSIRSPorts = csirs_output.csirsPerPort.length();

    Array<cmat> resourceGridPerPort(nCSIRSPorts);
    

    ivec csirsPorts = 3400 + getIntegers(0,nCSIRSPorts-1);
    int gSc = (fftSize-csirs_output.nRBsAllocated*subCarrierCountPerRB)/2;
    
    for(int port_cnt=0;port_cnt<nCSIRSPorts;port_cnt++)
    {
        resourceGridPerPort(port_cnt) = zeros_c(fftSize,nSymbolsPerSlot);
        int portRefNumber = getCSIRSPortReferenceNumber(csirsPorts(port_cnt));
        for(int sym_cnt=0,rs_cnt=0;sym_cnt<nSymbolsPerSlot;sym_cnt++)
        {
            for(int rb_cnt=0;rb_cnt<csirs_output.nRBsAllocated;rb_cnt++)
            {
                for(int sc_cnt=0;sc_cnt<subCarrierCountPerRB;sc_cnt++)
                {
                    if(csirs_output.rbRSPattern(sc_cnt,sym_cnt)==portRefNumber)
                    {
                        int scIndx = rb_cnt*subCarrierCountPerRB+sc_cnt;
                        resourceGridPerPort(port_cnt)(gSc + scIndx,sym_cnt) = csirs_output.csirsPerPort(port_cnt)(rs_cnt++);
                    }
                }
            }
        }
//         cout<<"resourceGridPerPort : "<<resourceGridPerPort(port_cnt).get_rows(gSc,gSc+11)<<endl;
        
    }/*abort();*/
    return resourceGridPerPort;
}


Array<cmat> precodeAndFillCSIRS(CSIRS_output csirs_output,CodeBookConfig_S dlCodeBookConfig,Array<type1PMI_S> widebandPMI,int trpIndx,bool precodeCSIRS)
{
    
    int nSymbolsPerSlot = 14;
    int subCarrierCountPerRB = 12;
    
    int nCSIRSPorts = dlCodeBookConfig.nCSIRSPorts;
    NRCodeBookType cbtype = dlCodeBookConfig.codeBookType;
    
    Array<cmat> resourceGridPerPort(nCSIRSPorts);
    
    Array<cvec> precCSIRSPerPort = csirs_output.csirsPerPort;
    
    if((cbtype == _NR_CODEBOOK_PORT_SELECTION_ || cbtype == _NR_CODEBOOK_PORT_SELECTION_ENHANCED_ || cbtype == _NR_CODEBOOK_PORT_SELECTION_FURTHER_ENHANCED_) && precodeCSIRS)
    {
        cmat vlm = getVlm(widebandPMI(trpIndx).i11,widebandPMI(trpIndx).i12,dlCodeBookConfig.N1,dlCodeBookConfig.N2,dlCodeBookConfig.O1,dlCodeBookConfig.O2);
        cmat W = kron(ones_c(1,nCSIRSPorts/2),vlm), N = zeros_c(nCSIRSPorts/2,nCSIRSPorts/2);
        W = concat_vertical(concat_horizontal(W,N),concat_horizontal(N,W));
        for(int tone_cnt=0;tone_cnt<csirs_output.csirsPerPort(0).length();tone_cnt++)
        {
            cmat csirs(nCSIRSPorts,1);
            for(int port_cnt=0;port_cnt<nCSIRSPorts;port_cnt++)
            {
                csirs(port_cnt,0) = csirs_output.csirsPerPort(port_cnt)(tone_cnt);
            }
            csirs = W.H()*csirs;
            
            for(int port_cnt=0;port_cnt<nCSIRSPorts;port_cnt++)
            {
                precCSIRSPerPort(port_cnt)(tone_cnt) = csirs(port_cnt,0);
            }
        }
    }
    ivec csirsPorts = 3400 + getIntegers(0,nCSIRSPorts-1);
    for(int port_cnt=0;port_cnt<nCSIRSPorts;port_cnt++)
    {
        resourceGridPerPort(port_cnt) = zeros_c(csirs_output.nRBsAllocated*subCarrierCountPerRB,nSymbolsPerSlot);
        int portRefNumber = getCSIRSPortReferenceNumber(csirsPorts(port_cnt));
        for(int sym_cnt=0,rs_cnt=0;sym_cnt<nSymbolsPerSlot;sym_cnt++)
        {
            for(int rb_cnt=0;rb_cnt<csirs_output.nRBsAllocated;rb_cnt++)
            {
                for(int sc_cnt=0;sc_cnt<subCarrierCountPerRB;sc_cnt++)
                {
                    if(csirs_output.rbRSPattern(sc_cnt,sym_cnt)==portRefNumber)
                    {
                        int scIndx = rb_cnt*subCarrierCountPerRB+sc_cnt;
                        resourceGridPerPort(port_cnt)(scIndx,sym_cnt) = precCSIRSPerPort(port_cnt)(rs_cnt++);
                    }
                }
            }
        }
    }
    return resourceGridPerPort;
}

// Array<cmat> precodeAndFillCSIRS(CSIRS_output csirs_output,Array<cvec> precoderPerCSIRSPort)
// {
//     int nSymbolsPerSlot = 14;
//     int subCarrierCountPerRB = 12;
//     
//     Array<cvec> precCSIRSPerPort = csirs_output.csirsPerPort;
//     int nCSIRSPorts = precCSIRSPerPort.length();
//     
//     Array<cmat> resourceGridPerPort(nCSIRSPorts);
//     
//     for(int tone_cnt=0;tone_cnt<csirs_output.csirsPerPort(0).length();tone_cnt++)
//     {
//         cmat csirs(nCSIRSPorts,1);
//         for(int port_cnt=0;port_cnt<nCSIRSPorts;port_cnt++)
//         {
//             csirs(port_cnt,0) = csirs_output.csirsPerPort(port_cnt)(tone_cnt);
//         }
//         csirs = W.H()*csirs;
//         
//         for(int port_cnt=0;port_cnt<nCSIRSPorts;port_cnt++)
//         {
//             precCSIRSPerPort(port_cnt)(tone_cnt) = csirs(port_cnt,0);
//         }
//     }
//     
//     ivec csirsPorts = 3400 + getIntegers(0,nCSIRSPorts-1);
//     for(int port_cnt=0;port_cnt<nCSIRSPorts;port_cnt++)
//     {
//         resourceGridPerPort(port_cnt) = zeros_c(csirs_output.nRBsAllocated*subCarrierCountPerRB,nSymbolsPerSlot);
//         int portRefNumber = getCSIRSPortReferenceNumber(csirsPorts(port_cnt));
//         for(int sym_cnt=0,rs_cnt=0;sym_cnt<nSymbolsPerSlot;sym_cnt++)
//         {
//             for(int rb_cnt=0;rb_cnt<csirs_output.nRBsAllocated;rb_cnt++)
//             {
//                 for(int sc_cnt=0;sc_cnt<subCarrierCountPerRB;sc_cnt++)
//                 {
//                     if(csirs_output.rbRSPattern(sc_cnt,sym_cnt)==portRefNumber)
//                     {
//                         int scIndx = rb_cnt*subCarrierCountPerRB+sc_cnt;
//                         resourceGridPerPort(port_cnt)(scIndx,sym_cnt) = precCSIRSPerPort(port_cnt)(rs_cnt++);
//                     }
//                 }
//             }
//         }
//     }
//     return resourceGridPerPort;
// }












