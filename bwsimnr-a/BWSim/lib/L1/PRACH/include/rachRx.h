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


#ifndef RACH_RX_H
#define RACH_RX_H

#include "../../../Frozen/simSupport/include/simSupport.h"

bool detect_nr_prach(
    const cvec &rx_cp,   // time-domain PRACH with CP
    int N_ZC,
    int root_index,
    int N_cs,
    int fft_size,
    int cp_length,
    double noise_var,
    double Pfa,
    int &detected_preamble,
    double &peak_metric
);

#endif 
