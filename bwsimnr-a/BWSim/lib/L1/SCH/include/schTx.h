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
#ifndef _PDSCHTX_H_
#define _PDSCHTX_H_

#include "../../PHY/include/Codebook.h"
#include "../../PHY/include/ReferenceSignalGenerator.h"
#include "../../PHY/include/ChannelCoder.h"
#include "../../PHY/include/LayerMapper.h"

struct SCHGrant_S
{
    int ueID;
    int gNBID;
    ivec mcsIndicesPerCW;
    ivec modulationOrderPercw;
    vec codeRatePercw;
    ivec allocatedRBs;
    int nREsAllocated;
    int nLayers;
    Array<cmat> precoderPerRB;
    HarqInfo harqInfo;
    
};

struct CodingParameters
{
    vec codeRatePerCW;
    int nREs;
    ivec tbSizePerCWInBits;
    ivec rvidPerCW;
    ivec Q;
    int nLayers;
    ivec nLayersPerCW;
    CoderType_E coderType;
    CRCTYPE crcType;
    int nCodewords;
    Array<bvec> infobitsPerCW;

    void load(ivec tbSizePercwInBits, ivec modulationOrderPercw, vec codeRatePercw, int nREsAllocated, int nlayers, Array<bvec> infobits="", ivec rvIndx="", CoderType_E codertype=_CODERTYPE_LDPC_, CRCTYPE crctype=CRC24B)
    {
        tbSizePerCWInBits=tbSizePercwInBits;   //CRC will not be added for polar coding.
        codeRatePerCW = codeRatePercw;
        Q = modulationOrderPercw;
        coderType = codertype;
        crcType = crctype;
        nLayers=nlayers;

        nCodewords = tbSizePerCWInBits.length();
        infobitsPerCW = infobits;
        rvidPerCW = rvIndx;
        if(rvidPerCW.length()==0)
            rvidPerCW=zeros_i(nCodewords);
        
        nLayersPerCW.set_length(nCodewords);
        Array<ivec> layerMap = getLayerToCWMapping(nLayers,nCodewords);
        for(int cw=0;cw<nCodewords;cw++)
        {
           nLayersPerCW(cw) = layerMap(cw).length();
        }
        nREs = nREsAllocated;
    }
};

struct DMRSInfo_S
{
    int dmrsType;
    ivec dmrsPorts;
    Array<cvec> dmrsPerPort;
    imat rbRSPattern;
};



int getBG(int dataLength,double codeRate);
int getDMRSPortReferenceNumber(int port,int dmrsType);

imat getDMRSPattern(int dmrsType, int nLayers);
DMRSInfo_S generateDMRS(int slotIndex, int dmrsType, int nLayers, int nRBs, ivec allocatedRBs);
void generateSCH(Array<cmat> &resourceGridPerTxAntenna , int slotIndex, CodingParameters &codingParam, int nRBs, ivec allocatedRBs , Array<cmat> precoderPerRB, int dmrsType);
int getNumCRCBits(CRCTYPE crcType);

cvec modulate(bvec ip,int Q);
bvec encode(bvec ip,int E,int Q,double codeRate,CoderType_E coderType,CRCTYPE crcType, int rvid);

#endif










