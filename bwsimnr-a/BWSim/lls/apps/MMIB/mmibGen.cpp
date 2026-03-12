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

#include <itpp/itbase.h>
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace itpp;
using namespace std;

int main()
{
    RNG_randomize();
    
    // ================= USER PARAMETERS =================
    int    M         = 2;        // 2(BPSK),4(QPSK),16,64,256,1024
    double snr_start = -10.0;    // dB
    double snr_end   = 40.0;     // dB
    double snr_step  = 0.05;      // dB (coarse grid, interpolate later)
    int    Nsym      = 20000;    // MC symbols per SNR
    // ===================================================
    
    int m = round_i(log2(double(M)));     // bits per symbol
    int Nsnr = int((snr_end - snr_start) / snr_step) + 1;
    
    vec mmib(Nsnr);
    
    // ================= Constellation + Bit labels =================
    cvec constellation;
    bmat bit_labels;
    
    if (M == 2) {
        // ---------- TRUE BPSK ----------
        constellation.set_size(2);
        constellation(0) = -1.0;
        constellation(1) = +1.0;
        
        bit_labels.set_size(2,1);
        bit_labels(0,0) = 0;
        bit_labels(1,0) = 1;
    }
    else {
        // ---------- QPSK / QAM ----------
        QAM qam(M);
        constellation = qam.get_symbols();
        
        // Normalize to unit average energy
        constellation /= sqrt(mean(sqr(abs(constellation))));
        
        bit_labels.set_size(M, m);
        for (int i = 0; i < M; i++)
            bit_labels.set_row(i, dec2bin(m, i));
    }
    
    // ================= SNR LOOP =================
    for (int is = 0; is < Nsnr; is++) {
        
        double snr_db  = snr_start + is * snr_step;
        double snr_lin = pow(10.0, snr_db / 10.0);
        double sigma2  = 1.0 / snr_lin;
        
        // -------- Random symbols (CORRECT randi syntax) --------
        ivec idx = randi(Nsym, 0, M - 1);
        
        // Transmit symbols
        cvec tx(Nsym);
        for (int n = 0; n < Nsym; n++)
            tx(n) = constellation(idx(n));
        
        // AWGN
        cvec rx = tx + sqrt(sigma2 / 2.0) * randn_c(Nsym);
        
        vec Ik(m);
        Ik.zeros();
        
        // ================= BICM-MI (Max-Log) =================
        for (int k = 0; k < m; k++) {
            
            vec mi_samples(Nsym);
            
            for (int n = 0; n < Nsym; n++) {
                
                double min0 = 1e12;
                double min1 = 1e12;
                
                for (int s = 0; s < M; s++) {
                    double d = norm(rx(n) - constellation(s)); // |.|^2
                    
                    if (bit_labels(s,k) == 0)
                        min0 = std::min(min0, d);
                    else
                        min1 = std::min(min1, d);
                }
                
                // Max-Log LLR
                double llr = (min1 - min0) / sigma2;
                
                // Clip for numerical safety
                if (llr > 50.0)  llr = 50.0;
                if (llr < -50.0) llr = -50.0;
                
                // Transmitted bit (CRITICAL)
                int b = bit_labels(idx(n), k);
                
                // Correct BICM-MI sample
                mi_samples(n) = log2(1.0 + exp((2*b - 1) * llr));
            }
            
            Ik(k) = 1.0 - mean(mi_samples);
        }
        
        mmib(is) = mean(Ik);
        
        cout << "SNR = " << snr_db
        << " dB  MMIB = " << mmib(is) << endl;
    }
    
    // ================= Save LUT =================
    ofstream fout("mmib_table.txt");
    for (int i = 0; i < Nsnr; i++)
        fout << mmib(i) << " ";
    fout << endl;
    fout.close();
    
    return 0;
}
