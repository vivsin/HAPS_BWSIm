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

#include "../include/schTx.h"

int getBG(int dataLength,double codeRate)
{
    int Bg;
    if(dataLength<=292 || (dataLength<=3824 && codeRate<=0.67) || codeRate<= 0.25) 
        Bg=2;
    else
        Bg=1;
    return Bg;
}

cvec modulate(bvec ip,int Q)
{
    QAM qamModem;
    qamModem.set_M(pow2i(Q));
    return qamModem.modulate_bits(ip);
}

int getNumCRCBits(CRCTYPE crcType) {
  switch(crcType) {
    case CRC8: return 8;
    case CRC11: return 11;
    case CRC16: return 16;
    case CRC24A: return 24;
    case CRC24B: return 24;
    case CRC24C: return 24;
    default: cout<<"[both:] Error in getNumOfCRCBits: specify proper crcType"; abort();
  }
}

bvec encode(bvec ip,int E,int Q,double codeRate,CoderType_E coderType,CRCTYPE crcType, int rvid)
{
    vector<bool> input = BVecToBoolVec(ip);
    vector<bool> encoded;
    if(coderType == _CODERTYPE_LDPC_)
    {
        int inputLength = ip.length(),Bg=getBG(inputLength+getNumCRCBits(crcType),codeRate);
        NrCRC crc;crc.setCRCTypeNR(crcType);
        input  = crc.performCRCEncodingNR(input);
        
        encoded = dataChannelEncoder(input,E/Q,Q,rvid,Bg,crcType);
    }
    else if (coderType == _CODERTYPE_POLAR_)
    {
        int RNTI = 0;

        int I_seg = 0; // Higher layer parameter indicating code block segmentation.
        int I_IL  = 0; // Higher layer parameter indicating intereleaving.
        int I_BIL = 1; // Higher layer parameter indicating triangular intereleaving.

        int n_PC    = 0; // Number of additional parity bits for early termination.
        int n_PC_wm = 0; // Number of PC bits to be kept together.

        int n_max            = 10;  // Max allowed mother code lenght.
        int L                = getNumCRCBits(crcType); // number of CRC parity bits
        bool padOneBeforeCRC = false;

        PolarCodec<double, bool> codec(I_seg, I_IL, I_BIL, n_PC, n_PC_wm, n_max, L,
                                        padOneBeforeCRC);
        encoded = codec.polarEncode(input, E, RNTI);
    }
    return BoolVectortoBVec(encoded);
}


ivec getDMRSPorts(int nLayers,int nFrontLoadedSymbols,int dmrsType)
{
    ivec portIndices;
    if(dmrsType==1)
    {
        if(nFrontLoadedSymbols==1)
            portIndices = "0 1 2 3";
        else
            portIndices = "0 1 4 5 2 3 6 7";
        
        portIndices += 1000;
    }
    else
    {
        if(nFrontLoadedSymbols==1)
            portIndices = "0 1 2 3 4 5";
        else
            portIndices = "0 1 6 7 2 3 8 9 4 5 10 11";
        portIndices += 1100;
    }
    return portIndices(0,nLayers-1);
}

int getDMRSPortReferenceNumber(int port,int dmrsType)
{
    if(dmrsType==1)
    {
        switch(port)
        {
            case 1000:
            case 1001:
            case 1004:
            case 1005:
                return 1000;
                break;
            case 1002:
            case 1003:
            case 1006:
            case 1007:
                return 1002;
                break;
            default:
                cout<<"Unknown Port Index.."<<port<<endl;
                abort();
        }
    }
    else if(dmrsType==2)
    {
        switch(port)
        {
            case 1100:
            case 1101:
            case 1106:
            case 1107:
                return 1100;
                break;
            case 1102:
            case 1103:
            case 1108:
            case 1109:
                return 1102;
                break;
            case 1104:
            case 1105:
            case 1110:
            case 1111:
                return 1104;
                break;
            default:
                cout<<"Unknown Port Index.."<<port<<endl;
                abort();
        }
    }
    return -1;
}

ivec getDMRSRSLoc(int nPorts,int dmrsType)
{
    if(dmrsType==1)
    {
        if(nPorts<=2)
            return "1000 0 1000 0 1000 0 1000 0 1000 0 1000 0";
        else if(nPorts<=4)
            return "1000 1002 1000 1002 1000 1002 1000 1002 1000 1002 1000 1002";
    }
    else if(dmrsType==2)
    {
        if(nPorts<=2)
            return "1100 1100 0 0 0 0 1100 1100 0 0 0 0";
        else if(nPorts<=4)
            return "1100 1100 1102 1102 0 0 1100 1100 1102 1102 0 0";
        else if(nPorts<=6)
            return "1100 1100 1102 1102 1104 1104 1100 1100 1102 1102 1104 1104";
    }
    cout<<"Unsupported nLayers.."<<endl;
    abort();
}

ivec getDMRSPortIndices(int refPort,int nFrontLoadedSymbols,int dmrsType)
{
    switch(dmrsType)
    {
        case 1: 
            switch(refPort)
            {
                case 1000:
                    if(nFrontLoadedSymbols==1)
                        return "1000 1001";
                    else 
                        return "1000 1001 1004 1005";
                    break;
                case 1002:
                    if(nFrontLoadedSymbols==1)
                        return "1002 1003";
                    else 
                        return "1002 1003 1006 1007";
                    break;
                default:
                    cout<<"Unknown Reference Port Number for DMRS Type1.."<<endl;abort();
                    break;
            }
            break;
        case 2:
            switch(refPort)
            {
                case 1100:
                    if(nFrontLoadedSymbols==1)
                        return "1100 1101";
                    else 
                        return "1100 1101 1106 1107";
                    break;
                case 1102:
                    if(nFrontLoadedSymbols==1)
                        return "1102 1103";
                    else 
                        return "1102 1103 1108 1109";
                    break;
                case 1104:
                    if(nFrontLoadedSymbols==1)
                        return "1104 1105";
                    else 
                        return "1104 1105 1111 1111";
                    break;
                default:
                    cout<<"Unknown Reference Port Number for DMRS Type2.."<<endl;abort();
                    break;
            }
            break;
        default:
            cout<<"Invalid DMRS Type.."<<endl;abort();
            break;
    }
    abort();
}

imat getDMRSPattern(int dmrsType, int nLayers)
{
    imat rbRSPattern = zeros_i(12,14);
    if((nLayers<=4 && dmrsType==1) || (dmrsType==2 && nLayers<=6))
    {
        ivec rsLoc = getDMRSRSLoc(nLayers,dmrsType);
        rbRSPattern.set_col(0,rsLoc);
    }
    else 
    {
        ivec rsLoc = getDMRSRSLoc(4,dmrsType);
        rbRSPattern.set_col(0,rsLoc);
        rbRSPattern.set_col(1,rsLoc);
    }
    return rbRSPattern;
}

DMRSInfo_S generateDMRS(int slotIndex, int dmrsType, int nLayers, int nRBs, ivec allocatedRBs)
{
    DMRSInfo_S dmrsInfo;
    
    int nCDM = (dmrsType==1) ? ((nLayers<=2) ? 1 : 2) : ((nLayers<=2) ? 1 : (nLayers<=4) ? 2 : 3);
    int nDMRSSymbols = (dmrsType==1) ? ((nLayers<=4) ? 1 : 2) :((nLayers<=6) ? 1 : 2);
    int nDMRSPositionsPerRB = (dmrsType==1) ? 6 : 4;
    
    dmrsInfo.dmrsType = dmrsType;
    dmrsInfo.dmrsPorts = getDMRSPorts(nLayers,nDMRSSymbols,dmrsType);
    dmrsInfo.dmrsPerPort.set_length(dmrsInfo.dmrsPorts.length());
    dmrsInfo.rbRSPattern = getDMRSPattern(dmrsType, nLayers);
    
    for(int port=0;port<nLayers;port++)
    {
        for(int sym_cnt=0;sym_cnt<nDMRSSymbols;sym_cnt++)
        {
            cvec dmrsPerSymbol = getNRDMRSsequence(0,0,0,slotIndex,dmrsInfo.dmrsPorts(port),nRBs,allocatedRBs,nDMRSPositionsPerRB,0,sym_cnt,dmrsType,nCDM);
            append(dmrsInfo.dmrsPerPort(port),dmrsPerSymbol);
        }
    }
    return dmrsInfo;
}

void fillDMRS(DMRSInfo_S dmrsInfo ,Array<cmat>& resourceGridPerTxAntenna, int dmrsType, ivec allocatedRBs, Array<cmat> precoderPerRB, int guardSCs)
{
    int nLayers = precoderPerRB(0).cols(),nTxAntenna = precoderPerRB(0).rows();
    int nDMRSSymbols = (dmrsType==1) ? ((nLayers<=4) ? 1 : 2) :((nLayers<=6) ? 1 : 2);
    int nSubCarriersPerRB = 12;
    int nCDM = (dmrsType==1) ? ((nLayers<=2) ? 1 : 2) : ((nLayers<=2) ? 1 : (nLayers<=4) ? 2 : 3);
    
    double betaDMRS = 1.0/sqrt(nCDM);
    
    ivec posPerPort = zeros_i(dmrsInfo.dmrsPorts.length());
    
    for(int sym_cnt=0;sym_cnt<nDMRSSymbols;sym_cnt++)
    {
        for(int rb_cnt=0;rb_cnt<allocatedRBs.length();rb_cnt++)
        {
            int rbIndx = allocatedRBs(rb_cnt);
            cmat antennaPrecoder = precoderPerRB(rb_cnt);
            
            for(int sc_cnt=0;sc_cnt<nSubCarriersPerRB;sc_cnt++)
            {
                int scIndx = guardSCs + allocatedRBs(rb_cnt)*nSubCarriersPerRB + sc_cnt;
                cmat symbolPerLayer = zeros_c(nLayers,1);
                int refPortNumber = dmrsInfo.rbRSPattern(sc_cnt,sym_cnt);
                if(refPortNumber>=1000)
                {
                    ivec portIndices = getDMRSPortIndices(refPortNumber,nDMRSSymbols,dmrsType);
                    for(int port_cnt=0;port_cnt<portIndices.length();port_cnt++)
                    {
                        int index = find(dmrsInfo.dmrsPorts,portIndices(port_cnt));
                        if(index!=-1)
                        {
                            dComplex dmrsSymbol = dmrsInfo.dmrsPerPort(index)(posPerPort(index)++);
                            int portPosition = index;//(dmrsType==1) ? portIndices(port_cnt)%1000 : portIndices(port_cnt)%1100;
                            symbolPerLayer(portPosition,0) = dmrsSymbol;
                        }         
                    }
                    
                    cmat precodedDMRS = betaDMRS*antennaPrecoder*symbolPerLayer;
                    for(int ant_cnt=0;ant_cnt<nTxAntenna;ant_cnt++)
                    {
                        resourceGridPerTxAntenna(ant_cnt)(scIndx,sym_cnt) = precodedDMRS(ant_cnt,0);
                    }
                }
            }
        }
    }
}

void precodeAndFillData(Array<cmat> &txGridPerAntenna ,Array<cvec> dataPerLayer, imat rbRSPattern, ivec allocatedRBs ,Array<cmat> precoderPerRB,int guardSCs)
{
    int nSymbolsPerSlot=14;
    int nSubCarriersPerRB = 12;
    int nLayers = dataPerLayer.length();
    
    int nREs = dataPerLayer(0).length();
     
    int nRBsAllocated = allocatedRBs.length();
    
    for(int sym_cnt=0,pos_cnt=0;sym_cnt<nSymbolsPerSlot;sym_cnt++)
    {
        for(int rb_cnt=0;rb_cnt<nRBsAllocated;rb_cnt++)
        {
            cmat antennaPrecoder = precoderPerRB(rb_cnt);
            
            for(int sc_cnt=0;sc_cnt<nSubCarriersPerRB;sc_cnt++)
            {
                if(rbRSPattern(sc_cnt,sym_cnt)==0)
                {
                    int scIndx = guardSCs + allocatedRBs(rb_cnt)*nSubCarriersPerRB + sc_cnt;
                    
                    cmat symbolPerLayer(nLayers,1);
                    for(int layer_cnt=0;layer_cnt<nLayers;layer_cnt++)
                        symbolPerLayer(layer_cnt,0) =  dataPerLayer(layer_cnt)(pos_cnt);
                    
                    cmat data = antennaPrecoder*symbolPerLayer;
                    
                    for(int ant_cnt=0;ant_cnt<txGridPerAntenna.length();ant_cnt++)
                        txGridPerAntenna(ant_cnt)(scIndx,sym_cnt) = data(ant_cnt,0);

                    pos_cnt++;
                }
            }
        }
    }
   
}

void generateSCH(Array<cmat> &resourceGridPerTxAntenna , int slotIndex, CodingParameters &codingParam, int nRBs, ivec allocatedRBs , Array<cmat> precoderPerRB, int dmrsType)
{
    ivec Q = codingParam.Q,E = elem_mult(codingParam.Q,codingParam.nLayersPerCW)*codingParam.nREs;
    vec codeRate = elem_div(to_vec(codingParam.tbSizePerCWInBits),to_vec(E));
    
    int usedSubcarriers = 12 * nRBs;
    int fftSize = resourceGridPerTxAntenna(0).rows();
    int guardSubcarriersInOneSide = (fftSize - usedSubcarriers)/2;
    
    
    int nCodeWords = codingParam.nCodewords;
    Array<bvec> ip(nCodeWords);Array<cvec> modSymbols(nCodeWords);
    for(int cw=0;cw<nCodeWords;cw++)
    {
        bvec encoded = encode(codingParam.infobitsPerCW(cw),E(cw),Q(cw),codeRate(cw),codingParam.coderType,codingParam.crcType,codingParam.rvidPerCW(cw));
//         encoded = ones_b(encoded.length());
        modSymbols(cw) =  modulate(encoded,Q(cw));
    }
    
    DMRSInfo_S dmrsInfo = generateDMRS(slotIndex, dmrsType, codingParam.nLayers, nRBs, allocatedRBs);
        
    Array<cvec> dataPerLayer = performLayerMapping(modSymbols, codingParam.nCodewords, codingParam.nLayers);
    
    precodeAndFillData(resourceGridPerTxAntenna,dataPerLayer, dmrsInfo.rbRSPattern, allocatedRBs ,precoderPerRB, guardSubcarriersInOneSide);
    
    fillDMRS(dmrsInfo,resourceGridPerTxAntenna,dmrsType,allocatedRBs,precoderPerRB,guardSubcarriersInOneSide);
    
}














