/*************************************************************************
 * 
 * CEWiT CONFIDENTIAL
 * __________________
 *
 * All Rights Reserved © 2014 CEWiT, India
 *
 * \ NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
 * and its suppliers, if any. The intellectual and technical concepts cont
 * ained herein may be proprietary to CEWiT.
 * Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
 * express, printed and signed license for use is strictly forbidden.
 */


#include "../../include/LLS_System.h"
#include "../../../lib/L1/SSB/include/Generate_PSS_and_SSS.h"
#include "../../../lib/L1/SSB/include/DLSyncDetector.h"

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
    
    imat ssbStructure;
    parse("./configFiles/SSBPatterns/SSBlock0.txt","rbStructure",ssbStructure);
    
    ssbRefNumbers_S ssbRefNumbers;
    ssbRefNumbers.pssRefNumber = 411;
    ssbRefNumbers.sssRefNumber = 431;
    ssbRefNumbers.pbchRefNumber = 4234;
    ssbRefNumbers.dmrsRefNumber = 42341;
    
    int fftSize=1024;
    int nSymbols=14;
    
    imat frameStructure(fftSize,nSymbols);
    frameStructure.set_submatrix((512-120),0,ssbStructure);
    
    int nSlots = 10;
    int cellID = 1;
    
    cmat ssb = GenerateAndFillPSSandSSS(ssbStructure,ssbRefNumbers,cellID);
    
    for(int snr_cnt=0;snr_cnt<llsSystem.simParam.snr.length();snr_cnt++)
    {
        double snrInDB = llsSystem.simParam.snr(snr_cnt);
        
        cout<<"Simulating for SNR : "<<snrInDB<<endl;
        
        double N0 = inv_dB(-snrInDB); // Scaling added to make Subcarrier noise power as -snrInDB - Dhiv
        int err_cnt=0;
        for(int slot_cnt=0;slot_cnt<nSlots;slot_cnt++)
        {
            
            //Transmission//////////////////////////////////////////////////////
            
            
            
            //Reception/////////////////////////////////////////////////////////
            
        
//             err_cnt+=(!isReceived);    
        }
//         bler(snr_cnt) = (double)err_cnt / (double)nSlots;
        
    }
    
//     cout<<"BLER : "<< bler <<endl;
    
}
