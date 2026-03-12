/*************************************************************************
 * 
 * CEWiT CONFIDENTIAL
 * __________________
 *
 * All Rights Reserved © 2014 CEWiT, India
 *
 * NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
 * and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
 * Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
 * express, printed and signed license for use is strictly forbidden.
 */

#include "../include/rachRx.h"

/* ---------------------------------------------------------
 * NR PRACH detection (correlation-based, TS 38.211 aligned)
 * --------------------------------------------------------- */
bool detect_nr_prach(const cvec &rx_cp, int N_ZC, int root_index, int N_cs, int fft_size, int cp_length, double noise_var, double Pfa, int &detected_preamble, double &peak_metric)
{
    /* ---------------- CP removal ---------------- */
    if (rx_cp.length() < fft_size + cp_length)
        return false;
    
    cvec rx = rx_cp.mid(cp_length, fft_size);
    
    /* ---------------- FFT ---------------- */
    cvec RX = fft(rx);
    
    /* ---------------- Extract PRACH subcarriers ---------------- */
    cvec rx_zc(N_ZC);
    int start_sc = fft_size / 2 - N_ZC / 2;
    
    for (int n = 0; n < N_ZC; n++) {
        rx_zc[n] = RX[start_sc + n];
    }
    
    /* ---------------- Reference ZC sequence ---------------- */
    cvec zc_ref(N_ZC);
    for (int n = 0; n < N_ZC; n++) {
        zc_ref[n] = exp(std::complex<double>(
            0,
            -M_PI * root_index * n * (n + 1) / N_ZC
        ));
    }
    
    /* ---------------- Correlate over cyclic shifts ---------------- */
    int max_preambles = N_ZC / N_cs;
    peak_metric = 0.0;
    detected_preamble = -1;
    
    for (int v = 0; v < max_preambles; v++) {
        
        int shift = (v * N_cs) % N_ZC;
        std::complex<double> corr(0.0, 0.0);
        
        for (int n = 0; n < N_ZC; n++) {
            corr += rx_zc[n] * conj(zc_ref[(n + shift) % N_ZC]);
        }
        
        double metric = abs(corr);
        
        if (metric > peak_metric) {
            peak_metric = metric;
            detected_preamble = v;
        }
    }
    
    /* ---------------- Spec-accurate threshold ---------------- */
    double threshold = sqrt(-noise_var * N_ZC * log(Pfa));
    
    return (peak_metric > threshold);
}
