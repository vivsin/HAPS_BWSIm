#pragma once

#include <fstream>
#include <sstream>
#include <list>
#include <map>
#include <unordered_set>
#include <vector>

#include "../../LDPC/include/NRCRC.h"
#include "polarConfFiles.hpp"

#include "../../../simSupport/include/simSupport.h"

/* The procedures are based on 3GPP document TS 38.212 v 140200
 * The notations also follows the nomenclature used in the TS doc.
 * TODO: Make sure that code works for every scenario. As of now only Coreset
 *       related work has been done. Will need to make changes to CRC
 *       calculation and attachment and maybe other changes too.
 */

enum DECODETYPE {
  SCL, // Successive cancellation list
  SSC, // Simplified SC
  SC   // Successive cancellation
};

namespace Polar {
// Read a csv file and make a vector
inline std::map<int, int>
readCsvFile(std::string filename) {

  std::ifstream fin(filename);
  std::vector<std::string> lines;
  for (std::string line; std::getline(fin, line);) {
    lines.push_back(line);
  }
  std::map<int, int> table;

  for (auto &line : lines) {
    std::stringstream ss;
    ss << line;
    std::string val;
    std::getline(ss, val, ',');

    int key = std::stoi(val);
    std::getline(ss, val, ',');
    table[key] = std::stoi(val);
  }

  return (table);

}
}

template <typename Q, typename B>
class PolarCodec {
  int I_seg; // Higher layer parameter indicating code block segmentation.
  int I_IL;  // Higher layer parameter indicating intereleaving.
  int I_BIL; // Higher layer parameter indicating triangular intereleaving.

  int n_PC;    // Number of additional parity bits for early termination.
  int n_PC_wm; // Number of PC bits to be kept together.

  int n_max; // Max allowed mother code lenght.
  int L;     // number of CRC parity bits

  bool padOneBeforeCRC; // if true, Add L ones before computing CRC

  NrCRC crc; // For computing & validating CRC bits.

  // From TS 38.212 sec 5.3.1
  double R_min = 1.0 / 8; // Min rate for calcuating mother code length
  int N_max    = 1024;  // Max mother code length

  int K_IL_max = 164; // Max interleaver size.

  std::map<int, int> Pi_IL_max; // Interleaver pattern
  std::map<int, int> Q_0_Nmax;  // Full reliability sequence
  std::map<int, int> P_i;       // Block interleaver pattern

  std::vector<int> Q_0_N1;    // Reliablity sequence of length N
  std::vector<int> Q_I_N_bar; // Info bit indices
  std::vector<int> Q_F_N_bar; // Frozen bit indices.
  std::vector<int> Q_PC_N;    // Parity bits indices.

  std::unordered_set<int> Q_Ftmp_N_bar;
  std::unordered_set<int> Q_I_N_bar_set; // For making find work.
  std::vector<int> Q_Itmp_N_bar;

public:
  PolarCodec() {};

  PolarCodec(int I_seg, int I_IL, int I_BIL, int n_PC, int n_PC_wm, int n_max,
             int L, bool padOneBeforeCRC)
      : I_seg(I_seg), I_IL(I_IL), I_BIL(I_BIL), n_PC(n_PC), n_PC_wm(n_PC_wm),
        n_max(n_max), L(L), padOneBeforeCRC(padOneBeforeCRC) {
    Pi_IL_max = Polar::readCsvFile(PI_IL_MAX_FILE);
    Q_0_Nmax  = Polar::readCsvFile(Q_0_NMAX_FILE);
    P_i       = Polar::readCsvFile(P_I_FILE);

    Q_0_N1.reserve(N_max);
    Q_I_N_bar.reserve(N_max);
    Q_F_N_bar.reserve(N_max);
    Q_Ftmp_N_bar.reserve(N_max);
    Q_Itmp_N_bar.reserve(N_max);
    Q_I_N_bar_set.reserve(N_max);
    Q_PC_N.reserve(n_PC);
  };

  std::vector<B>
  polarEncode(std::vector<B> &in_bits, int E, int RNTI);

  std::vector<B>
  polarDecode(std::vector<Q> &coded_bits, bool& isSuccess, int A, int E, int RNTI,
              DECODETYPE = SCL);

  bool
  polarDecode(std::vector<Q> &coded_bits, int A, int E, int RNTI,
              std::vector<B> &decodedDCI, DECODETYPE = SCL);

  std::map<int, std::vector<B>>
  codeBlockSegmentation(const std::vector<B> &a, int RNTI);

  void
  attachCRC(std::vector<B> &a, int L);

  void
  scrambleCRC(std::vector<B> &c, int RNTI);

  bool
  checkCRC(std::vector<B> &c, int RNTI);

  int
  findMotherCodeLength(int K, int E);

  std::map<int, int>
  genInterleaverPattern(int K);

  std::vector<B>
  interleaver(std::vector<B> &c);

  std::vector<B>
  deinterleaver(std::vector<B> &c_prime);

  std::vector<B>
  _encoder(std::vector<B> &c, int E);

  std::vector<B>
  _decoder(std::vector<Q> &d, int K, int E);

  std::vector<B>
  _decoderSCL(std::vector<Q> &d, int K, int E, int RNTI);

  std::vector<B>
  rateMatcher(std::vector<B> &d, int E);

  void
  genInfoFrozenBitIndices(int K, int N, int E, std::vector<int> &Q_I_N_bar,
                          std::vector<int> &Q_F_N_bar,
                          std::vector<int> &Q_PC_N);

  std::vector<B>
  subBlockInterleaver(std::vector<B> &d);

  std::vector<Q>
  subBlockDeinterleaver(std::vector<Q> &d);

  std::vector<B>
  bitSelector(std::vector<B> &y, int K, int E);

  std::vector<Q>
  rebitSelector(std::vector<Q> &e, int K, int E);

  std::vector<B>
  interleaveCodedBits(std::vector<B> &e);

  std::vector<Q>
  deinterleaveCodedBits(std::vector<Q> &f);

  std::vector<B>
  codeBlockConcatenation(std::map<int, std::vector<B>> &f);

  std::map<int, std::vector<Q>>
  codeBlockdeconcatenation(std::vector<Q> &g, int E);
};
