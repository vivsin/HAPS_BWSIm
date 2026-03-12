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

#include "../include/rachTx.h"

#include <itpp/itbase.h>
#include <itpp/itcomm.h>
#include <cmath>

using namespace itpp;

/* ---------------------------------------------------------
 * NR PRACH generation (TS 38.211 aligned)
 * --------------------------------------------------------- */
cvec generate_nr_prach(int N_ZC,int root_index,int preamble_index,int N_cs,int fft_size,int cp_length)
{
    /* ---------------- Zadoff–Chu sequence ---------------- */
    cvec zc(N_ZC);
    for (int n = 0; n < N_ZC; n++) {
        zc[n] = exp(std::complex<double>(
            0,
            -M_PI * root_index * n * (n + 1) / N_ZC
        ));
    }
    
    /* ---------------- Cyclic shift (preamble index v) ---------------- */
    int shift = (preamble_index * N_cs) % N_ZC;
    
    cvec zc_shifted(N_ZC);
    for (int n = 0; n < N_ZC; n++) {
        zc_shifted[n] = zc[(n + shift) % N_ZC];
    }
    
    /* ---------------- Frequency-domain PRACH mapping ---------------- */
    cvec freq(fft_size);
    freq.zeros();
    
    int start_sc = fft_size / 2 - N_ZC / 2;  // center mapping
    
    for (int n = 0; n < N_ZC; n++) {
        freq[start_sc + n] = zc_shifted[n];
    }
    
    /* ---------------- IFFT to time domain ---------------- */
    cvec time = ifft(freq);
    
    /* ---------------- CP insertion (optional) ---------------- */
    if (cp_length > 0) {
        cvec tx(time.length() + cp_length);
        tx.set_subvector(0, time.right(cp_length));
        tx.set_subvector(cp_length, time);
        return tx;
    }
    
    return time;
}
