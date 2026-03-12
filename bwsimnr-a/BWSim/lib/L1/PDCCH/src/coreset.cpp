#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <numeric>

#include "../include/coreset.hpp"
#include "../include/dmrs.hpp"

// Config for Polar decoding
// Refer 3GPP TS 38.212 sec 7.3.2 and 7.3.3
namespace Polar {
int I_seg   = 0; // Higher layer parameter indicating code block segmentation.
int I_IL    = 1; // Higher layer parameter indicating intereleaving.
int I_BIL   = 0; // Higher layer parameter indicating triangular intereleaving.
int n_PC    = 0; // Number of additional parity bits for early termination.
int n_PC_wm = 0; // Number of PC bits to be kept together.

int n_max = 9;  // Max allowed mother code lenght.
int L     = 24; // number of CRC parity bits

bool padOneBeforeCRC = true; // Insert L one's before calculating the CRC
} // namespace Polar

// Refer 3GPP 38.211 sec 7.3.2 for details
// Also refer 3GPP 38.213 sec 10.1

template <class T>
Coreset<T>::Coreset(const ControlResourceSetIE &IE) {
  this->controlResourceSetIE = IE;

  // Do some sanity checks
  assert(controlResourceSetId != -1 && "controlResourceSetId not set.");
  assert(duration != -1 && "duration not set.");
  assert(regBundleSize != -1 && "regBundleSize not set.");
  assert(interleaverSize != -1 && "interleaverSize not set.");
  assert(shiftIndex != -1 && "shiftIndex not set.");

  this->codec = PolarCodec<double, bool>(
      Polar::I_seg, Polar::I_IL, Polar::I_BIL, Polar::n_PC, Polar::n_PC_wm,
      Polar::n_max, Polar::L, Polar::padOneBeforeCRC);

  qpskModem.set_M(4); // M = 4 for QPSK

  N_rb_coreset =
      6 * std::accumulate(
              std::begin(controlResourceSetIE.frequencyDomainResources),
              std::end(controlResourceSetIE.frequencyDomainResources), 0);
  N_symb_coreset = duration;
  N_reg_coreset  = N_rb_coreset * N_symb_coreset;
  assert(N_rb_coreset != 0 && "Zero RB's allocated for CORESET.");

  // Check if its CORESET-0(part of MIB) or not.
  // TODO: Verify that ServingCellConfigCommon configures group coreset.
  if (controlResourceSetId == 0) {
    // CORESET part of MIB
    N_shift = N_id_cell;
    N_id    = N_id_cell;
  } else {
    // UE CORESET
    N_shift = shiftIndex;
    N_id    = controlResourceSetIE.pdcch_DMRS_ScramblingID;
    assert(N_id != -1 && "pdcch_DMRS_ScramblingID not set.");
  }

  // Check if coreset if interleaved or not
  if (controlResourceSetIE.cce_REG_MappingType == interleaved) {
    isInterleaved = true;
  } else {
    isInterleaved = false;
  }

  // RNTI is 0 if pdcch_DMRS_ScramblingID is not configured.
  if (controlResourceSetIE.pdcch_DMRS_ScramblingID == -1) {
    n_RNTI = 0;
  }
}

template <class T>
void
Coreset<T>::scrambler(std::vector<T> &in_bits) {
  int length = in_bits.size();

  // Sanity Check
  assert(((n_RNTI != -1) && (N_id != -1)) && "RNTI or ID not set");

  int c_init = getSeed();

  // Generate the gold sequence with given seed and lenght.
  char goldSeq[length];

  PRSG::generateGoldSequence(c_init, length, goldSeq);

  char *gptr = goldSeq;
  std::transform(in_bits.begin(), in_bits.end(), in_bits.begin(),
                 [&gptr](T b) -> T { return b ^ *gptr++; });
}

template <class T>
std::vector<std::complex<double>>
Coreset<T>::modulater(const std::vector<T> &in_bits) {
  // QPSK modulate the incoming bits

  // modulate_bits() take a bool vector as argument.
  // std::vector<bool> bits(in_bits.size());
  // ;
  // std::transform(in_bits.begin(), in_bits.end(), bits.begin(),
  //                [](T v) -> bool { return (static_cast<bool>(v)); });

  bvec bits(in_bits.size());
  for (int i = 0; i < (int)in_bits.size(); i++) {
      bits(i) = in_bits[i];
  }

  auto modSymbs = qpskModem.modulate_bits(bits);
  std::vector<std::complex<double>> modSymbols(modSymbs.length());
  for (int i = 0; i < (int)modSymbs.length(); ++i) {
    modSymbols[i] = modSymbs(i);
  }

  return (modSymbols);
}

template <class T>
std::map<int, std::vector<std::complex<double>>>
Coreset<T>::interleaver(std::vector<std::complex<double>> &in_symbs) {
  // Given a array of input symbols(CCE) map them to REG bundles
  // and interleave them if necessary.

  // Make sure that the given data can be split into CCE's
  // 1 CCE = 54 RE's after DMRS mapping.
  assert(in_symbs.size() % n_symbol_cce == 0 &&
         "Input size not in terms of CCE");

  int aggregationLevel = in_symbs.size() / n_symbol_cce; // Number of CCE

  // Get the interleaver map
  auto interleaverMap = genInterleaverMap(aggregationLevel);

  // Rearrange the REG as per the interleaver.
  std::map<int, std::vector<std::complex<double>>> cceToRegMap;
  auto it = in_symbs.begin();

  for (const auto &r : interleaverMap) {
    assert(it != in_symbs.end()); // DEBUG
    cceToRegMap[r.second] = std::vector<std::complex<double>>(it, it + 9);
    it += 9; // with 9 RE's + 3 DMRS.
  }

  return ((cceToRegMap));
}

template <class T>
std::map<int, int>
Coreset<T>::genInterleaverMap(int aggregationLevel) {
  int n_shift = this->N_shift;
  int L       = regBundleSize;   // REG bundle size
  int R       = interleaverSize; // Interleaver size
  int C;

  // Sanity Check
  assert(n_shift != -1 && "n_shift not set.");

  if (isInterleaved) {
    C = N_reg_coreset / (L * R);

    if ((N_reg_coreset % (L * R) != 0)) {
      // ts 38.211 7.3.2.2
      // The UE is not expected to handle configurations
      // resulting in the quantity C not being an integer.
      std::cout << "N_reg_coreset must be a multiple of L and R.\n";
      std::cout << "N_reg_coreset = " << N_reg_coreset << "L = " << L;
      std::cout << "C = " << C << std::endl;
      abort();
    }
  }

  int nRegBundles = N_reg_coreset / L; // Total Number of REG bundles.

  // Some units
  // 1 CCE = 6 REG
  // 1 CCE = 54 RE (after DMRS mapping)
  // 1 REG = 9 RE(CCE) + 3 RE(DMRS)
  // Interleaver rearranges a REG bundle
  //
  // Define the interleaver function
  std::function<int(int)> f;
  if (isInterleaved) {
    // The interleaver.
    f = [&R, &C, &n_shift, &nRegBundles](int j) -> int {
      // r = j % R;
      // c = j / R:
      return (((j % R) * C + (j / R) + n_shift) % nRegBundles);
    };
  } else {
    // Without interleaver, REG bundle size is always 6.
    assert(L == 6);
    f = [](int j) -> int { return (j); };
  }

  int reg = 0;
  std::map<int, int> interleaverMap;
  for (int j = 0; j < aggregationLevel; ++j) { // Take a CCE,
    for (int i = 6 * j / L; i < (6 * j / L) + (6 / L); ++i) {
      // split it into REG bundles,
      int idx = f(i); // inteleave the REG bundle and
      // fill the REG's in REG bundles
      for (int k = idx * L; k < (idx * L) + L; ++k) {
        interleaverMap[reg++] = k;
      }
    }
  }

  return (interleaverMap);
}

template <class T>
std::map<int, std::vector<std::complex<double>>>
Coreset<T>::mapToPhyRB(
    std::map<int, std::vector<std::complex<double>>> &regMap) {
  // Map to store RB's in a symbol period.
  std::map<int, std::vector<std::complex<double>>> phyMap;

  // Initialize the vectors.
  for (int l = 0; l < duration; ++l) {
    phyMap[l] = std::vector<std::complex<double>>();
    phyMap[l].reserve(N_rb_coreset);
  }

  // Sanity check
  assert(N_id != -1 && N_sf != -1 && "N_id or N_sf not set.");

  // Fill the RB's
  // Refer ts 38.211 sec 7.4.1.3.2 for dmrs mapping details.
  for (int l = 0; l < duration; ++l) {
    std::vector<std::complex<double>> dmrs =
        generateDMRS(l, N_id, N_sf, dmrslen);
    // Shift the DMRS to the correct RB
    // TODO : Initialize ddmrs_shift properly
    int dmrs_shift = 0;
    auto dmrsIt    = dmrs.begin() + dmrs_shift;

    // A function to return dmrs symbols with some checks
    auto getOneDMRS = [&dmrsIt, &dmrs]() {
      if (dmrsIt != dmrs.end()) {
        return (*dmrsIt++);
      } else {
        std::cout << "Insufficient DMRS symbols." << std::endl;
        std::cout << "Increase the lenght of DMRS sequence generated.";
        std::cout << std::endl << "Aborting!!" << std::endl;
        abort();
      }
    };

    // for (const auto &reg : regMap) {
    for (int reg = 0; reg < N_reg_coreset; reg++) {
      // Check if REG belongs to the correct symbol.
      if (reg % duration != l) {
        continue;
      }

      // Check if the REG has CORESET data
      if (regMap.find(reg) == regMap.end()) {
        // If we do not have CORESET data mapped to these RE, fill it with 0's.
        // TODO : Fill the unused CORESET RE's appropriately.
        // TODO : take care of power factor beta. in ts
        // phyMap[l].insert(phyMap[l].end(), 12, 0);
        if (precoderGranularity == allContiguousRBs) {
          phyMap[l].insert(phyMap[l].end(), getOneDMRS());
          phyMap[l].insert(phyMap[l].end(), 3, 0);
          phyMap[l].insert(phyMap[l].end(), getOneDMRS());
          phyMap[l].insert(phyMap[l].end(), 3, 0);
          phyMap[l].insert(phyMap[l].end(), getOneDMRS());
          phyMap[l].insert(phyMap[l].end(), 3, 0);
        } else {
          phyMap[l].insert(phyMap[l].end(), 12, 0);
          dmrsIt += 3;
        }
        continue;
      }

      // RB's 1, 5 and 9 are DMRS.
      // This corresponds to 0, 4 and 8 in the vector.
      // TODO : Fill DMRS when PrecoderGranularity is sameAsREGBundle.
      // As of now we are filling DMRS if the REG is within the alloted
      // CORESET space.
      // But if the precoderGranularity is sameAsREGBundle size, then we
      // to fill DMRS symbols only if the REG has PDCCH data.
      // First DMRS at 1
      phyMap[l].insert(phyMap[l].end(), getOneDMRS());
      // Now 3 CORESET data at 2, 3 and 4
      phyMap[l].insert(phyMap[l].end(), regMap[reg].begin(),
                       regMap[reg].begin() + 3);
      // DMRS symbol at 5
      phyMap[l].insert(phyMap[l].end(), getOneDMRS());
      // Data at 6, 7 and 8
      phyMap[l].insert(phyMap[l].end(), regMap[reg].begin() + 3,
                       regMap[reg].begin() + 6);
      // DMRS at 9
      phyMap[l].insert(phyMap[l].end(), getOneDMRS());
      // Data at 10, 11 and 12
      phyMap[l].insert(phyMap[l].end(), regMap[reg].begin() + 6,
                       regMap[reg].end());
    }
  }

  return (phyMap);
}

template <class T>
std::vector<std::complex<double>>
Coreset<T>::extractCCE(std::map<int, std::vector<std::complex<double>>> &phyMap,
                       int aggregationLevel) {
  std::vector<std::complex<double>> cceData;

  // Get the interleaver map
  auto interleaverMap = genInterleaverMap(aggregationLevel);

  for (const auto &r : interleaverMap) {
    int symb = r.second % duration;
    int rb   = r.second / duration;

    auto regIt = phyMap[symb].begin() + (12 * rb);
    std::vector<std::complex<double>> cce(regIt, regIt + 12);

    // Remove the DMRS
    auto cceIt = cce.begin();
    cce.erase(cceIt + 8);
    cce.erase(cceIt + 4);
    cce.erase(cceIt);

    cceData.insert(cceData.end(), cce.begin(), cce.end());
  }

  return (cceData);
}

template <class T>
// std::vector < T >
std::vector<double>
Coreset<T>::demodulater(std::vector<std::complex<double>> &modSymbs,
                        std::vector<std::complex<double>> &channel, double N0) {
  cvec modSymbols(modSymbs.size());
  for (int i = 0; i < (int)modSymbs.size(); ++i) {
    modSymbols(i) = modSymbs[i];
  }
  
  cvec channelVec(channel.size());
  for (int i = 0; i < (int)channel.size(); ++i) {
      channelVec(i) = channel[i];
  }
  
  auto softBits = qpskModem.demodulate_soft_bits(modSymbols, channelVec, N0,
                                                 APPROX);

  std::vector<double> soft_bits(softBits.length());
  for (int i = 0; i < (int)softBits.length(); ++i) {
    soft_bits[i] = softBits(i);
  }
  return soft_bits;
}

template <class T>
void
Coreset<T>::descrambler(std::vector<double> &llrs) {
  int length = llrs.size();

  int c_init = getSeed();

  // Generate the gold sequence with given seed and lenght.
  char goldSeq[length];

  PRSG::generateGoldSequence(c_init, length, goldSeq);

  int sign[2] = {1, -1};
  char *gptr   = goldSeq;
  std::transform(
      llrs.begin(), llrs.end(), llrs.begin(),
      [&sign, &gptr](double llr) -> double { return (sign[*gptr++]) * llr; });
}

template <class T>
std::vector<T>
Coreset<T>::decode(std::vector<double> &soft_bits, int n_DCI,
                   int aggregationLevel) {
  // n_DCI : Size of the DCI to be decoded
  // Decode soft_bits
  int E = aggregationLevel * n_bits_cce;
  bool isSuccess;  
  return (codec.polarDecode(soft_bits,isSuccess, n_DCI, E, n_RNTI, SCL));
}

template <class T>
bool
Coreset<T>::decode(std::vector<double> &soft_bits, int n_DCI,
                   int aggregationLevel, std::vector<T> &decodedDCI) {
  // n_DCI : Size of the DCI to be decoded
  // Decode soft_bits
  int E = aggregationLevel * n_bits_cce;

  bool crcPass = codec.polarDecode(soft_bits, n_DCI, E, n_RNTI, decodedDCI, SCL);
  return (crcPass);
}
template <class T>
void
Coreset<T>::printValues() {
  std::cout << "Coreset ID       = " << controlResourceSetId << std::endl;
  std::cout << "duration         = " << duration << std::endl;
  std::cout << "isInterleaved    = " << isInterleaved << std::endl;
  std::cout << "regBundleSize    = " << regBundleSize << std::endl;
  std::cout << "interleaverSize  = " << interleaverSize << std::endl;
  std::cout << "shiftIndex       = " << shiftIndex << std::endl;
  std::cout << "N_rb_coreset     = " << N_rb_coreset << std::endl;
  std::cout << "N_symb_coreset   = " << N_symb_coreset << std::endl;
  std::cout << "N_reg_coreset    = " << N_reg_coreset << std::endl;
  std::cout << "N_shift          = " << N_shift << std::endl;
  std::cout << "n_RNTI           = " << n_RNTI << std::endl;
  std::cout << "N_id             = " << N_id << std::endl;
  std::cout << "N_id_cell        = " << N_id_cell << std::endl;
  std::cout << "N_sf             = " << N_sf << std::endl;
  std::cout << "dmrslen          = " << dmrslen << std::endl;
}

template <class T>
std::map<int, std::vector<std::complex<double>>>
Coreset<T>::getCoresetSymbols(std::vector<T> &dciBits, int aggregationLevel) {
  // Step for generating the coreset symbols for DCI bits
  // Refer TS 38.211 section 7.3.2 for PDCCH and section 7.4.1.3 for DMRS
  // for PDCCH.
  // First step is to polar encode the incoming bits.
  //
  //
  // TODO: Make sure its always 54;
  auto RNTI = this->getRNTI();
  int E     = aggregationLevel * n_bits_cce;

  assert((int)dciBits.size() <= E && "Current AL less that dci lenght.");
  auto enc_bits = codec.polarEncode(dciBits, E, RNTI);

  // Scrambling by a gold sequence.
  this->scrambler(enc_bits);

  // Now we have to modulate the scrambled bits.
  auto modulated_symbs = this->modulater(enc_bits);

  // Now interleave.
  auto regMap = this->interleaver(modulated_symbs);

  // Now map the symbols to CORESET block
  auto coresetBlock = this->mapToPhyRB(regMap);

  return (coresetBlock);
}

template <class T>
std::vector<T>
Coreset<T>::getDciBits(std::map<int, std::vector<std::complex<double>>> &phyMap,
                       int dciSize, int aggregationLevel,
                       std::vector<std::complex<double>> &channel, double N0) {
  // Decode the DCI bits from the coreset data.
  // TODO: Blind decoding gives us the a bunch of CCE to decode.
  //       As of now, the function just performs the reverse operation
  //       for getCoresetSymbols() fn.
  //       Changes needed to support blind decoding.

  // Extract the CCE data
  auto cceData = this->extractCCE(phyMap, aggregationLevel);

  // TODO: Need to interleave channel also before demodulating.
  // Demodulate the symbols
  auto llrs = this->demodulater(cceData, channel, N0);

  // Descramble the llr values
  this->descrambler(llrs);

  // Decode the llr values
  auto decodedBits = this->decode(llrs, dciSize, aggregationLevel);

  return decodedBits;
}

template <typename T>
bvec
Coreset<T>::decodeCoreset(cvec coresetSymbols, cvec coresetChannel, int dciSize,
                          int aggregationLevel, double N0) {
  // Get the interleaver map
  auto interleaverMap = genInterleaverMap(aggregationLevel);

  std::vector<std::complex<double>> cceSymbols;
  std::vector<std::complex<double>> cceChannel;
  for (const auto &r : interleaverMap) {
    int symb = r.second % duration;
    int rb   = r.second / duration;

    int rb_cnt = (symb * N_rb_coreset) + (12 * rb);
    // The cce data is in the below re's. The rest of the re's(0, 4, & 8)
    // are dmrs.
    for (int re : {1, 2, 3, 5, 6, 7, 9, 10, 11}) {
      int tone_cnt = rb_cnt + re;
      cceSymbols.insert(cceSymbols.end(), coresetSymbols(tone_cnt));
      cceChannel.insert(cceChannel.end(), coresetChannel(tone_cnt));
    }
  }

  // Demodulate the symbols
  auto llrs = this->demodulater(cceSymbols, cceChannel, N0);

  // Descramble the llr values
  this->descrambler(llrs);

  // Decode the llr values
  auto decodedBits = this->decode(llrs, dciSize, aggregationLevel);

  bvec decodedDci(decodedBits.size());
  for (int i = 0; i < (int)decodedBits.size(); ++i) {
    decodedDci(i) = decodedBits[i];
  }

  return decodedDci;
}

template <typename T>
bool
Coreset<T>::decodeCoreset(cvec coresetSymbols, cvec coresetChannel, int dciSize,
                          int aggregationLevel, double N0, bvec &decodedDCI) {
  // Get the interleaver map
  auto interleaverMap = genInterleaverMap(aggregationLevel);

  std::vector<std::complex<double>> cceSymbols;
  std::vector<std::complex<double>> cceChannel;
  for (const auto &r : interleaverMap) {
    int symb = r.second % duration;
    int rb   = r.second / duration;

    int rb_cnt = (symb * N_rb_coreset) + (12 * rb);
    // The cce data is in the below re's. The rest of the re's(0, 4, & 8)
    // are dmrs.
    for (int re : {1, 2, 3, 5, 6, 7, 9, 10, 11}) {
      int tone_cnt = rb_cnt + re;
      cceSymbols.insert(cceSymbols.end(), coresetSymbols(tone_cnt));
      cceChannel.insert(cceChannel.end(), coresetChannel(tone_cnt));
    }
  }

  // Demodulate the symbols
  auto llrs = this->demodulater(cceSymbols, cceChannel, N0);

  // Descramble the llr values
  this->descrambler(llrs);

  // Decode the llr values
  std::vector<T> decodedBits;
  bool crcPass = this->decode(llrs, dciSize, aggregationLevel, decodedBits);

  bvec decodedDci(decodedBits.size());
  for (int i = 0; i < (int)decodedBits.size(); ++i) {
    decodedDci(i) = decodedBits[i];
  }

  decodedDCI = std::move(decodedDci);
  return crcPass;
}

// Explicit class template initiation
template class Coreset<bool>;
// template class Coreset<int>;

//
