#include <algorithm>

#include "../../../Frozen/simSupport/include/simSupport.h"
#include "../include/dmrs.hpp"

DMRS *DMRS::instance = 0;

// Refer TS 38.211 v150200 sec 7.4.1.3 for details
//
std::vector<std::complex<double>>
DMRS::getDMRS(int l, int N_id, int N_sf, int length) {
  // Get the seed uning the N_id and symbol number
  unsigned int seed = getSeed(l, N_id, N_sf);

  // Generate the gold sequence using the seed
  PRSG::generateGoldSequence((int)seed, length, goldSeq);

  // Convert the Gold sequence to vector bool
  goldBits.clear();
  std::copy(goldSeq, goldSeq + length, std::back_inserter(goldBits));

  // Return the modulated Gold sequence
  // TODO : Change goldBits to ivec as needed.
  bvec bits(goldBits.size());
  for (int i = 0; i < (int)goldBits.size(); i++) {
      bits(i) = goldBits[i];
  }

  auto modSymbs = qpskModem.modulate_bits(bits);
  std::vector<std::complex<double>> modSymbols(modSymbs.length());
  for (int i = 0; i < (int)modSymbs.length(); ++i) {
    modSymbols[i] = modSymbs(i);
  }

  return (modSymbols);
}

std::vector<std::complex<double>>
generateDMRS(int l, int N_id, int N_sf, int length) {
  DMRS *dmrsInstance = DMRS::getInstance();

  return (dmrsInstance->getDMRS(l, N_id, N_sf, length));
}
