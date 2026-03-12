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

    // =======================================================================
    // CONFIGURATION AREA
    // =======================================================================

    int nSlots = 8000;         // Total physical slots available
    int nRepetitions = 8;     // CHANGE THIS MANUALLY (e.g., 1, 2, 4, 8)

    // Safety check to ensure slots divide evenly
    if (nSlots % nRepetitions != 0) {
        cout << "Warning: nSlots (" << nSlots << ") is not divisible by nRepetitions ("
             << nRepetitions << "). Some slots at the end will be unused." << endl;
    }
    // =======================================================================

    vec bler=zeros(llsSystem.simParam.snr.length());

    int rv_sequence[] = {0, 2, 3, 1}; // The standard pattern

    for(int snr_cnt=0;snr_cnt<llsSystem.simParam.snr.length();snr_cnt++)
    {
        double snrInDB = llsSystem.simParam.snr(snr_cnt);
        cout<<"Simulating for SNR : "<<snrInDB<<endl;
        double N0 = inv_dB(-snrInDB);
        int err_cnt=0;

        for(int slot_cnt=0; slot_cnt <= (nSlots - nRepetitions); slot_cnt += nRepetitions)
        {
            // 1. GENERATE DATA (Once per packet) ///////////////////////////////////
            CodingParameters codingParam;
            codingParam.load(tbSizeWithCRC, modulationOrder, codeRate, nREsAllocated, 1);

            ivec dataLength = codingParam.tbSizePerCWInBits - ((codingParam.coderType==_CODERTYPE_LDPC_) ? getNumCRCBits(codingParam.crcType) : 0);
            //-------------------
            // *** ADD THIS CHECK ***
            if (dataLength(0) <= 0) {
                cout << "ERROR: dataLength is 0! Check config file." << endl;
                abort();
            }
            //-------------------

            ivec Q = codingParam.Q,E = elem_mult(codingParam.Q,codingParam.nLayersPerCW)*codingParam.nREs;
            vec actualCodeRate = elem_div(to_vec(codingParam.tbSizePerCWInBits),to_vec(E));

            Array<bvec> ip(1);
            ip = randb(dataLength(0));

            // This acts as "Memory" for the Decoder (explained previously)
             vector<double> hip_buffer;
             hip_buffer.clear();
             bool packetSuccess = false;
             bvec cbSuccess(0);

            // 2. REPETITION LOOP (Inner Loop) //////////////////////////////////////
            // This runs 'nRepetitions' times.
            // It generates new noise every time, demodulates, and sums the LLRs.
            for(int r = 0; r < nRepetitions; r++)
            {
                // 1. Pick RV ID
                int current_rvid = rv_sequence[r % 4];

            //  CALL THE ENCODE FUNCTION
               bvec encoded = encode(ip(0), E(0), Q(0), actualCodeRate(0),codingParam.coderType, codingParam.crcType,current_rvid);


            //  Modulate
               cvec modSymbols =  modulate(encoded,Q(0));

            // Generate independent noise for this specific repetition
                cvec noise = sqrt(N0)*randn_c(modSymbols.length());
                cvec rxSymbols = modSymbols + noise;

            // Calculate LLRs for this specific repetition
                vec currentLLRs = demodulate(codingParam.Q(0), rxSymbols, "", N0);


                // 3. DECODE
                bvec outputBits = decode(currentLLRs,current_rvid, hip_buffer,  E(0), Q(0), actualCodeRate(0),
                                 codingParam.coderType, codingParam.crcType, packetSuccess,cbSuccess);
            }

        err_cnt += (!packetSuccess);
        cout<<"Decode status for slot pair "<< slot_cnt<<" to " <<slot_cnt+nRepetitions-1<< " (Rep Factor: "<< nRepetitions <<"): "<< packetSuccess<<endl;

        }
        // BLER Calculation:
        // Number of distinct packets = Total Slots / Repetitions
        double totalPackets = (double)nSlots / (double)nRepetitions;
        bler(snr_cnt) = (double)err_cnt / totalPackets;
    }

    cout<<"BLER : "<< bler <<endl;
}
