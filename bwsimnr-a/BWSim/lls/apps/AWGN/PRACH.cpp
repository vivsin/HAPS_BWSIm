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
#include "../../../lib/L1/PRACH/include/rachTx.h"
#include "../../../lib/L1/PRACH/include/rachRx.h"

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
    
    int nSlots = 10;
    int N_ZC=139;              // 839 (long) or 139 / 571 (short)
    int root_index=1;        // ZC root u
    int preamble_index=2;    // v
    int N_cs=0;              // cyclic shift (from PRACH config)
    int fft_size=1024;          // e.g., 1024 / 2048
    int cp_length=72; 
    double pfa=1e-4;
    
    vec detectionProb=zeros(llsSystem.simParam.snr.length());
    for(int snr_cnt=0;snr_cnt<llsSystem.simParam.snr.length();snr_cnt++)
    {
        
        double snrInDB = llsSystem.simParam.snr(snr_cnt);
        
        cout<<"Simulating for SNR : "<<snrInDB<<endl;
        
        double N0 = inv_dB(-snrInDB); // Scaling added to make Subcarrier noise power as -snrInDB - Dhiv
        int err_cnt=0;
        
        for(int slot_cnt=0;slot_cnt<nSlots;slot_cnt++)
        {
            
            //PRACH Transmission//////////////////////////////////////////////////////
            cvec prach = generate_nr_prach(N_ZC,root_index,preamble_index,N_cs,fft_size,cp_length);
            
            
            cvec noise = sqrt(N0)*randn_c(prach.length()); //generateNoiseSamples(N0,rxSymbols.length());
            cvec rxSymbols = prach + noise;
            
            //PRACH Detection/////////////////////////////////////////////////////////
            int detected_preamble;
            double peak_metric;
            bool isReceived = detect_nr_prach(rxSymbols,N_ZC,root_index,N_cs,fft_size,cp_length,N0,pfa,detected_preamble,peak_metric);
            
            cout<<"Detection status for slot : "<<slot_cnt<<" is "<< isReceived<<endl;
            
            err_cnt+=(!isReceived);    
        }
        detectionProb(snr_cnt) = (double)err_cnt / (double)nSlots;
        
    }
    
    cout<<"BLER : "<< detectionProb <<endl;
    
}
