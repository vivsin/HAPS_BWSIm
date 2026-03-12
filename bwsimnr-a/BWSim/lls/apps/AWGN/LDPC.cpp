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


#include "../../include/LLS_System.h"
#include "../../../lib/L1/SCH/include/schTx.h"
#include "../../../lib/L1/SCH/include/schRx.h"

#define FREQ_DOMAIN_CHANNEL

int main(int argc, char** argv){
  
    
    LLS_System llsSystem;
    llsSystem.initSystem();
    
    int nTxAntenna = llsSystem.simParam.antennaParam.numTXRUs(0);
    int nRxAntenna = llsSystem.simParam.antennaParam.numTXRUs(1);
    if(nTxAntenna>1 || nRxAntenna>1)
    {
        cout<<"Incompatible antenna configuration for AWGN simulation."<<endl;
        abort();
    }
    
    ivec tbSizeWithCRC,modulationOrder;
    vec codeRate;
    parse("./configFiles/mySimulationConfig.txt","tbSizeWithCRC",tbSizeWithCRC);
    parse("./configFiles/mySimulationConfig.txt","modulationOrder",modulationOrder);
    parse("./configFiles/mySimulationConfig.txt","codeRate",codeRate);
    int nREsAllocated = ceil_i((double)tbSizeWithCRC(0)/(modulationOrder(0)*codeRate(0)));
    
    int nSlots = 10;
    
    vec bler=zeros(llsSystem.simParam.snr.length());
    for(int snr_cnt=0;snr_cnt<llsSystem.simParam.snr.length();snr_cnt++)
    {
        
        double snrInDB = llsSystem.simParam.snr(snr_cnt);
        
        cout<<"Simulating for SNR : "<<snrInDB<<endl;
        
        double N0 = inv_dB(-snrInDB); // Scaling added to make Subcarrier noise power as -snrInDB - Dhiv
        int err_cnt=0;
        for(int slot_cnt=0;slot_cnt<nSlots;slot_cnt++)
        {
            
            //LDPC Transmission//////////////////////////////////////////////////////
            
            CodingParameters codingParam;
            codingParam.load(tbSizeWithCRC, modulationOrder, codeRate, nREsAllocated, 1);
            
            ivec dataLength = codingParam.tbSizePerCWInBits - ((codingParam.coderType==_CODERTYPE_LDPC_) ? getNumCRCBits(codingParam.crcType) : 0);
            ivec Q = codingParam.Q,E = elem_mult(codingParam.Q,codingParam.nLayersPerCW)*codingParam.nREs;
            vec actualCodeRate = elem_div(to_vec(codingParam.tbSizePerCWInBits),to_vec(E));
          
            bvec ip = randb(dataLength(0));
            bvec encoded = encode(ip,E(0),Q(0),actualCodeRate(0),codingParam.coderType,codingParam.crcType);
            cvec modSymbols =  modulate(encoded,Q(0));
            
            cvec noise = sqrt(N0)*randn_c(modSymbols.length()); //generateNoiseSamples(N0,rxSymbols.length());
            cvec rxSymbols = modSymbols + noise;
            
            //LDPC Reception/////////////////////////////////////////////////////////
            
            vec LLRs = demodulate(codingParam.Q(0),rxSymbols,"",N0);
            bool isReceived=false;
            bvec outputBits = decode(LLRs,E(0),Q(0),actualCodeRate(0),codingParam.coderType,codingParam.crcType,isReceived);
            
            cout<<"Decode status for slot : "<<slot_cnt<<" is "<< isReceived<<endl;
            
            err_cnt+=(!isReceived);    
        }
        bler(snr_cnt) = (double)err_cnt / (double)nSlots;
        
    }
    
    cout<<"BLER : "<< bler <<endl;
    
}
