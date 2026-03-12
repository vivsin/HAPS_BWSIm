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


#ifndef RACH_TX_H
#define RACH_TX_H

#include "../../../Frozen/simSupport/include/simSupport.h"


cvec generate_nr_prach( 
int N_ZC,              // 839 (long) or 139 / 571 (short)
int root_index,        // ZC root u
int preamble_index,    // v
int N_cs,              // cyclic shift (from PRACH config)
int fft_size,          // e.g., 1024 / 2048
int cp_length          // CP length in samples (optional)
);

#endif
