#pragma once
#ifndef _CORESET_DMRS_H_
#define _CORESET_DMRS_H_

#include <complex>
#include <vector>

#include "../../../Frozen/sequence/include/goldSequence.h"

const int LENGTH     = 25000;
const int MAX_LENGTH = 2 * LENGTH;

class DMRS {
  
    QAM qpskModem;

  char goldSeq[MAX_LENGTH] = {0};
  std::vector<bool> goldBits;

  static DMRS *instance;

  DMRS() {
    // For QPSK M = 4
    qpskModem.set_M(4);

    goldBits.reserve(MAX_LENGTH);
  };

public:
  static DMRS *
  getInstance() {
    if (!instance) {
      instance = new DMRS;
    }

    return (instance);
  };

  unsigned int
  getSeed(int l, int N_id, int N_sf) {
    // Refer TS 38.211 v150200 sec 7.4.1.3 p88 eq for c_init.
    return ((((14 * N_sf + l + 1) * ((N_id << 1) + 1)) << 17) + (N_id << 1));
  };

  std::vector<std::complex<double>>
  getDMRS(int l, int N_id, int N_sf, int length);
};

std::vector<std::complex<double>>
generateDMRS(int l, int N_id, int N_sf, int length);

#endif // _CORESET_DMRS_H_
