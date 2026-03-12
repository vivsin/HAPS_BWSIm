#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <limits>

#include "../include/polarCodec.hpp"
#include "../include/polarDecoder.hpp"

// #include <iostream>
// template <class stream_t, typename T>
// stream_t &
// operator<<(stream_t &os, const std::vector<T> &vector2) {
//   for (const auto &v : vector2) {
//     // os << setw(2) << v << " ";
//     os << v << " ";
//   }
//   return os;
// }
// 
// template <class stream_t, typename T>
// stream_t &
// operator<<(stream_t &os, const std::list<T> &vector2) {
//   for (const auto &v : vector2) {
//     // os << setw(2) << v << " ";
//     os << v << std::endl;
//     ;
//   }
//   return os;
// }

// Row weight of the G_n matrix of size 1024 x 1024
std::array<int, 1024> G_n_row_weight = {
    1,   2,   2,   4,   2,   4,   4,   8,   2,   4,   4,   8,   4,   8,   8,
    16,  2,   4,   4,   8,   4,   8,   8,   16,  4,   8,   8,   16,  8,   16,
    16,  32,  2,   4,   4,   8,   4,   8,   8,   16,  4,   8,   8,   16,  8,
    16,  16,  32,  4,   8,   8,   16,  8,   16,  16,  32,  8,   16,  16,  32,
    16,  32,  32,  64,  2,   4,   4,   8,   4,   8,   8,   16,  4,   8,   8,
    16,  8,   16,  16,  32,  4,   8,   8,   16,  8,   16,  16,  32,  8,   16,
    16,  32,  16,  32,  32,  64,  4,   8,   8,   16,  8,   16,  16,  32,  8,
    16,  16,  32,  16,  32,  32,  64,  8,   16,  16,  32,  16,  32,  32,  64,
    16,  32,  32,  64,  32,  64,  64,  128, 2,   4,   4,   8,   4,   8,   8,
    16,  4,   8,   8,   16,  8,   16,  16,  32,  4,   8,   8,   16,  8,   16,
    16,  32,  8,   16,  16,  32,  16,  32,  32,  64,  4,   8,   8,   16,  8,
    16,  16,  32,  8,   16,  16,  32,  16,  32,  32,  64,  8,   16,  16,  32,
    16,  32,  32,  64,  16,  32,  32,  64,  32,  64,  64,  128, 4,   8,   8,
    16,  8,   16,  16,  32,  8,   16,  16,  32,  16,  32,  32,  64,  8,   16,
    16,  32,  16,  32,  32,  64,  16,  32,  32,  64,  32,  64,  64,  128, 8,
    16,  16,  32,  16,  32,  32,  64,  16,  32,  32,  64,  32,  64,  64,  128,
    16,  32,  32,  64,  32,  64,  64,  128, 32,  64,  64,  128, 64,  128, 128,
    256, 2,   4,   4,   8,   4,   8,   8,   16,  4,   8,   8,   16,  8,   16,
    16,  32,  4,   8,   8,   16,  8,   16,  16,  32,  8,   16,  16,  32,  16,
    32,  32,  64,  4,   8,   8,   16,  8,   16,  16,  32,  8,   16,  16,  32,
    16,  32,  32,  64,  8,   16,  16,  32,  16,  32,  32,  64,  16,  32,  32,
    64,  32,  64,  64,  128, 4,   8,   8,   16,  8,   16,  16,  32,  8,   16,
    16,  32,  16,  32,  32,  64,  8,   16,  16,  32,  16,  32,  32,  64,  16,
    32,  32,  64,  32,  64,  64,  128, 8,   16,  16,  32,  16,  32,  32,  64,
    16,  32,  32,  64,  32,  64,  64,  128, 16,  32,  32,  64,  32,  64,  64,
    128, 32,  64,  64,  128, 64,  128, 128, 256, 4,   8,   8,   16,  8,   16,
    16,  32,  8,   16,  16,  32,  16,  32,  32,  64,  8,   16,  16,  32,  16,
    32,  32,  64,  16,  32,  32,  64,  32,  64,  64,  128, 8,   16,  16,  32,
    16,  32,  32,  64,  16,  32,  32,  64,  32,  64,  64,  128, 16,  32,  32,
    64,  32,  64,  64,  128, 32,  64,  64,  128, 64,  128, 128, 256, 8,   16,
    16,  32,  16,  32,  32,  64,  16,  32,  32,  64,  32,  64,  64,  128, 16,
    32,  32,  64,  32,  64,  64,  128, 32,  64,  64,  128, 64,  128, 128, 256,
    16,  32,  32,  64,  32,  64,  64,  128, 32,  64,  64,  128, 64,  128, 128,
    256, 32,  64,  64,  128, 64,  128, 128, 256, 64,  128, 128, 256, 128, 256,
    256, 512, 2,   4,   4,   8,   4,   8,   8,   16,  4,   8,   8,   16,  8,
    16,  16,  32,  4,   8,   8,   16,  8,   16,  16,  32,  8,   16,  16,  32,
    16,  32,  32,  64,  4,   8,   8,   16,  8,   16,  16,  32,  8,   16,  16,
    32,  16,  32,  32,  64,  8,   16,  16,  32,  16,  32,  32,  64,  16,  32,
    32,  64,  32,  64,  64,  128, 4,   8,   8,   16,  8,   16,  16,  32,  8,
    16,  16,  32,  16,  32,  32,  64,  8,   16,  16,  32,  16,  32,  32,  64,
    16,  32,  32,  64,  32,  64,  64,  128, 8,   16,  16,  32,  16,  32,  32,
    64,  16,  32,  32,  64,  32,  64,  64,  128, 16,  32,  32,  64,  32,  64,
    64,  128, 32,  64,  64,  128, 64,  128, 128, 256, 4,   8,   8,   16,  8,
    16,  16,  32,  8,   16,  16,  32,  16,  32,  32,  64,  8,   16,  16,  32,
    16,  32,  32,  64,  16,  32,  32,  64,  32,  64,  64,  128, 8,   16,  16,
    32,  16,  32,  32,  64,  16,  32,  32,  64,  32,  64,  64,  128, 16,  32,
    32,  64,  32,  64,  64,  128, 32,  64,  64,  128, 64,  128, 128, 256, 8,
    16,  16,  32,  16,  32,  32,  64,  16,  32,  32,  64,  32,  64,  64,  128,
    16,  32,  32,  64,  32,  64,  64,  128, 32,  64,  64,  128, 64,  128, 128,
    256, 16,  32,  32,  64,  32,  64,  64,  128, 32,  64,  64,  128, 64,  128,
    128, 256, 32,  64,  64,  128, 64,  128, 128, 256, 64,  128, 128, 256, 128,
    256, 256, 512, 4,   8,   8,   16,  8,   16,  16,  32,  8,   16,  16,  32,
    16,  32,  32,  64,  8,   16,  16,  32,  16,  32,  32,  64,  16,  32,  32,
    64,  32,  64,  64,  128, 8,   16,  16,  32,  16,  32,  32,  64,  16,  32,
    32,  64,  32,  64,  64,  128, 16,  32,  32,  64,  32,  64,  64,  128, 32,
    64,  64,  128, 64,  128, 128, 256, 8,   16,  16,  32,  16,  32,  32,  64,
    16,  32,  32,  64,  32,  64,  64,  128, 16,  32,  32,  64,  32,  64,  64,
    128, 32,  64,  64,  128, 64,  128, 128, 256, 16,  32,  32,  64,  32,  64,
    64,  128, 32,  64,  64,  128, 64,  128, 128, 256, 32,  64,  64,  128, 64,
    128, 128, 256, 64,  128, 128, 256, 128, 256, 256, 512, 8,   16,  16,  32,
    16,  32,  32,  64,  16,  32,  32,  64,  32,  64,  64,  128, 16,  32,  32,
    64,  32,  64,  64,  128, 32,  64,  64,  128, 64,  128, 128, 256, 16,  32,
    32,  64,  32,  64,  64,  128, 32,  64,  64,  128, 64,  128, 128, 256, 32,
    64,  64,  128, 64,  128, 128, 256, 64,  128, 128, 256, 128, 256, 256, 512,
    16,  32,  32,  64,  32,  64,  64,  128, 32,  64,  64,  128, 64,  128, 128,
    256, 32,  64,  64,  128, 64,  128, 128, 256, 64,  128, 128, 256, 128, 256,
    256, 512, 32,  64,  64,  128, 64,  128, 128, 256, 64,  128, 128, 256, 128,
    256, 256, 512, 64,  128, 128, 256, 128, 256, 256, 512, 128, 256, 256, 512,
    256, 512, 512, 1024};

template <typename Q, typename B>
std::map<int, std::vector<B>>
PolarCodec<Q, B>::codeBlockSegmentation(const std::vector<B> &a, int RNTI) {
  int A = a.size(); // Input sequence length.
  // TS 38.212 sec 5.2.1
  assert(A <= 1706 && "Input code length cannot be greater than 1706.");

  int C;
  I_seg == 1 ? C = 2 : C = 1; // Number of code blocks.

  std::map<int, std::vector<B>> codeBlocks;

  // Extend the input bits by inserting 0's so the size can match the
  // code block size.
  int A_prime = (int)std::ceil((float)A / (float)C) * C;
  std::vector<B> a_prime(A_prime, 0);
  for (int i = 0; i < A_prime - A; ++i) {
    a_prime[i] = 0;
  }
  for (int i = A_prime - A; i < A_prime; ++i) {
    a_prime[i] = a.at(i - (A_prime - A));
  }

  // Copy the input data to code blocks and attach CRC to each.
  int s = 0;
  for (int r = 0; r < C; ++r) {
    codeBlocks[r] = std::vector<B>();
    codeBlocks[r].reserve(A_prime + L);
    for (int k = 0; k < A_prime / C; ++k) {
      codeBlocks[r].push_back(a_prime.at(s++));
    }

    // For each code block attach and scramble CRC.
    attachCRC(codeBlocks[r], L);
    scrambleCRC(codeBlocks[r], RNTI);
  }

  return (codeBlocks);
}

template <typename Q, typename B>
void
PolarCodec<Q, B>::attachCRC(std::vector<B> &a, int L) {
  // For info on CRC computation for Polar codes
  // refer TS 38.212 sec 7.3.2

  // The NRCRC functions expect a bool vector, so do conversion
  // We need to insert L = 24 1's to our input data vector and then compute
  // the CRC and the therefore the extra L in the initialization of boolvec.
  std::vector<bool> boolvec;
  boolvec.reserve(a.size() + L);
  if (padOneBeforeCRC) {
    // Add 24 ones before computing CRC
    // Needed for DCI transmission.
    boolvec.insert(boolvec.end(), L, 1);
  }
  std::transform(a.begin(), a.end(), std::back_inserter(boolvec),
                 [](B val) { return ((bool)val); });

  // For polar coding we use generator polynomial g_CRC24C()
  if (L == 24) {
    crc.setCRCTypeNR(CRC24C);
  } else {
    assert(0 && "Only L = 24 supported as of now.");
  }

  std::vector<bool> pbits = crc.performCRCEncodingNR(boolvec);

  // Attach the parity bits to our data
  for (auto it = pbits.end() - L; it != pbits.end(); ++it) {
    a.push_back((B)*it);
  }
}

template <typename Q, typename B>
void
PolarCodec<Q, B>::scrambleCRC(std::vector<B> &c, int RNTI) {
  // We need to scramble the last 16 bits of the parity bits
  // using the given RNTI.
  // Refer TS 38.212 sec7.3.2 for more info
  uint16_t mask = 0x0001;
  for (auto it = c.rbegin(); it != c.rbegin() + 16; ++it) {
    *it = *it ^ (mask & RNTI);
    RNTI >>= 1;
  }
}

template <typename Q, typename B>
bool
PolarCodec<Q, B>::checkCRC(std::vector<B> &c, int RNTI) {
  // Descramble the parity bits
  this->scrambleCRC(c, RNTI);

  // Add L 1's to the beginning before checking for parity.
  std::vector<bool> bvec;
  bvec.reserve(c.size() + L);
  if (padOneBeforeCRC) {
    // Add 24 ones before computing CRC
    // Needed for DCI transmission.
    bvec.insert(bvec.end(), L, 1);
  }
  std::transform(c.begin(), c.end(), std::back_inserter(bvec),
                 [](B val) { return ((bool)val); });

  // Now check for parity
  if (L == 24) {
    crc.setCRCTypeNR(CRC24C);
  } else {
    assert(0 && "Only L = 24 supported as of now.");
  }

  return (crc.performCRCCheckNR(bvec));
}

template <typename Q, typename B>
int
PolarCodec<Q, B>::findMotherCodeLength(int K, int E) {
  // K is the input code length.
  // E is the rate matched output length.
  // Also extracts the reliability sequence of length 2^n from
  // the given reliability sequence Q_0_Nmax
  // Refer TS 38.212 sec 5.3.1
  //  R_min = 1 / 8, Min rate for calculating mother code length

  int n, n_1;
  int n_min = 5;
  // n_2 = log2(K /R_min)
  // R_min = 1 / 8
  // K / R_min = 8 * K
  int n_2    = std::ceil(std::log2(K << 3));
  int log2_E = std::ceil(std::log2(E));
  int rhs    = (9 * (1 << (log2_E - 1))) >> 3;

  if ((E <= rhs) && (((double)K / E) < (9.0 / 16))) {
    n_1 = log2_E - 1;
  } else {
    n_1 = log2_E;
  }

  n = std::max(std::min(std::min(n_1, n_2), n_max), n_min);

  int N = 1 << n; // 2^n
  // Generate Q_0_N1
  Q_0_N1.clear(); // Clear the vector
  for (auto &val : Q_0_Nmax) {
    if (val.second < N) {
      Q_0_N1.push_back(val.second);
    }
  }

  return (N);
}

template <typename Q, typename B>
std::map<int, int>
PolarCodec<Q, B>::genInterleaverPattern(int K) {
  // Generate interleaver pattern for Polar encoder.
  // Refer TS 38.212 sec 5.3.1.1
  std::map<int, int> Pi;

  if (I_IL == 0) {
    for (int k = 0; k < K; ++k) {
      Pi[k] = k;
    }
  } else {
    int k = 0;

    for (int m = 0; m < K_IL_max; ++m) {
      int val = Pi_IL_max[m] - (K_IL_max - K);
      if (val >= 0) {
        Pi[k++] = val;
      }
    }
  }

  return (Pi);
}

template <typename Q, typename B>
std::vector<B>
PolarCodec<Q, B>::interleaver(std::vector<B> &c) {
  // Interleave the input bits
  // Refer TS 38.212 sec 5.3.1.1
  //
  auto Pi = this->genInterleaverPattern(c.size());

  std::vector<B> c_prime(c.size(), 0);
  for (const auto &pair : Pi) {
    c_prime[pair.first] = c[pair.second];
  }

  return (c_prime);
}

template <typename Q, typename B>
std::vector<B>
PolarCodec<Q, B>::deinterleaver(std::vector<B> &c_prime) {
  // Deinterleave the input bits
  // Refer TS 38.212 sec 5.3.1.1
  //
  auto Pi = this->genInterleaverPattern(c_prime.size());

  std::vector<B> c(c_prime.size(), 0);
  for (const auto &pair : Pi) {
    c[pair.second] = c_prime[pair.first];
  }

  return (c);
}

template <typename Q, typename B>
std::vector<B>
PolarCodec<Q, B>::_encoder(std::vector<B> &c, int E) {
  // Encode the given sequence c
  int K = c.size();

  // Find the mother code length.
  int N = findMotherCodeLength(K, E);

  // Compute the frozen bits and data bit indices.
  // Information bit indices in ascending order of reliability.
  Q_I_N_bar.clear();
  Q_F_N_bar.clear();
  this->genInfoFrozenBitIndices(K, N, E, Q_I_N_bar, Q_F_N_bar, Q_PC_N);

  // Generate the interleaver pattern
  auto Pi = genInterleaverPattern(K);

  // Generate the data vector u
  std::vector<B> u(N, 0);

  int k = 0;
  std::vector<B> y(5, 0);

  // Function to check if element in array
  // Returns true if element i is in vector v.
  auto checkIn = [](std::vector<int> &v, int i) -> bool {
    return (std::find(v.begin(), v.end(), i) != v.end());
  };

  if (n_PC > 0) {
    // We need to add parity bits
    for (int n = 0; n < N; ++n) {
      // Rotate y vector to left
      std::rotate(y.begin(), y.begin() + 1, y.end());

      if (checkIn(Q_I_N_bar, n)) {
        if (checkIn(Q_PC_N, n)) {
          u[n] = y[0];
        } else {
          u[n] = c[k++];
          y[0] = (y[0] + u[n]) % 2;
        }
      } else {
        // u[n] = 0;
        continue;
      }
    }
  } else {
    // No parity bits added
    std::sort(Q_I_N_bar.begin(), Q_I_N_bar.end());
    for (const auto &idx : Q_I_N_bar) {
      u[idx] = c[k++];
    }
    // for (int n = 0; n < N; ++n) {
    //   if (checkIn(Q_I_N_bar, n)) {
    //     u[n] = c[k++];
    //   } else {
    //     // u[n] = 0;
    //     continue;
    //   }
    // }
  }

  // Do Kronecker product with G_n
  for (auto n = (N >> 1); n > 0; n >>= 1)
    for (auto j = 0; j < N; j += 2 * n)
      for (auto i = 0; i < n; i++)
        u[j + i] = u[j + i] ^ u[n + j + i];

  return (u);
}

template <typename Q, typename B>
void
PolarCodec<Q, B>::genInfoFrozenBitIndices(int K, int N, int E,
                                          std::vector<int> &Q_I_N_bar,
                                          std::vector<int> &Q_F_N_bar,
                                          std::vector<int> &Q_PC_N) {
  // Generate the information and frozen bit indices.
  // This includes generating
  // the sub block interleaving pattern and generating the bit index
  // sequences Q_I_N_bar and Q_F_N_bar needed for polar encoding.
  //
  // K         : size of input bits to encoder
  // N         : size of encoded bits
  // E         : size of rate matched output
  // Q_I_N_bar : Bit indices for information bits arranged in ascending order
  // Q_F_N_bar : Bit indices for frozen bits
  // Q_0_N1    : bit indexes arranged in ascending reliability
  //
  // Refer TS 38.212 sec 5.4.1.1 for more info

  // Generate sub block interleaver pattern
  Q_I_N_bar.clear();
  Q_F_N_bar.clear();
  Q_PC_N.clear();
  int J[N];
  int N_5 = N >> 5;
  for (int n = 0; n < N; ++n) {
    int i = (n << 5) / N;
    // TODO : Replace modulo by a lookup table.
    J[n] = P_i[i] * N_5 + (n % N_5);
  }

  // Generate the bit index sequences
  Q_Ftmp_N_bar.clear();

  if (E < N) {
    // Rate matched output length is less that encoded bits length.
    // Do either puncturing or shortening.
    if ((double)K / (double)E < 7.0 / 16.0) {
      // Rate < 1 / 2. Do puncturing
      for (int n = 0; n < N - E; ++n) {
        Q_Ftmp_N_bar.insert(J[n]);
      }

      if (E >= 3 * N / 4) {
        for (int i = 0; i < std::ceil((3 * N / 4) - (E / 2.0)); ++i) {
          Q_Ftmp_N_bar.insert(i);
        }
      } else {
        for (int i = 0; i < std::ceil((9 * N / 16) - (E / 4.0)); ++i) {
          Q_Ftmp_N_bar.insert(i);
        }
      }
    } else {
      // Rate > 1 /2. Do shortening.
      for (int n = E; n < N; ++n) {
        Q_Ftmp_N_bar.insert(J[n]);
      }
    }
  }

  Q_Itmp_N_bar.clear();
  // Q_Itmp_N_bar = Q_0_N1 \ Q_Ftmp_N_bar.
  // Q_0_N1 is ordered in ascending order of reliability and
  // so is Q_Itmp_N_bar by initializing it as below.
  for (auto val : Q_0_N1) {
    if (Q_Ftmp_N_bar.find(val) == Q_Ftmp_N_bar.end()) {
      // val is not in Q_Ftmp_N_bar. So we add it to Q_Itmp_N_bar.
      Q_Itmp_N_bar.push_back(val);
    }
  }

  // Q_Itmp_N_bar is ordered in ascending order of reliability.
  // Q_I_N_bar comprises (K + n_PC) most reliable bit indices of
  // Q_Itmp_N_bar, i.e, the last (K - n_PC) elements of the set
  // Q_Itmp_N_bar.
  Q_I_N_bar_set.clear();
  for (auto it = Q_Itmp_N_bar.end() - (K + n_PC); it != Q_Itmp_N_bar.end();
       ++it) {
    Q_I_N_bar.push_back(*it);
    Q_I_N_bar_set.insert(*it);
  }

  // Q_F_N_bar = Q_0_N1 \ Q_I_N_bar
  for (auto val : Q_0_N1) {
    if (Q_I_N_bar_set.find(val) == Q_I_N_bar_set.end()) {
      // val is not in Q_I_N_bar
      Q_F_N_bar.push_back(val);
    }
  }

  // Generate bit indices for parity bits, if any
  if (n_PC > 0) {
    Q_PC_N.clear();
    //  TODO : Generate parity bits
    //   total of n_pc - n_pc_wm bits are placed in (n_pc - n_pc_wm) least
    //   reliable location in Q_I_N_bar.
    //   This means the first (n_pc - n_pc_wm) location in Q_I_N_bar are for
    //   parity bits.
    Q_PC_N.insert(Q_PC_N.end(), Q_I_N_bar.begin(),
                  Q_I_N_bar.begin() + (n_PC - n_PC_wm));

    // The rest of the n_pc_wm parity bits are placed at location with
    // minimum row weight in Q_I_N_bar.
    // If there are multiple rows with minimum row weight, then the most
    // reliable among them is choosed.
    std::vector<std::pair<int, int>> rowWeight;
    for (auto it = Q_I_N_bar.rbegin(); it != Q_I_N_bar.rend() - n_PC; ++it) {
      rowWeight.push_back(std::pair<int, int>(G_n_row_weight[*it], *it));
    }

    auto cmp = [](const std::pair<int, int> &a, const std::pair<int, int> &b) {
      return a.first < b.first;
    };

    // Only n_pc_wm = 1 supported for now
    assert(n_PC_wm == 1 && "Only n_pc_wm = 1 supported for now");
    auto minIt = std::min_element(rowWeight.begin(), rowWeight.end(), cmp);

    Q_PC_N.push_back((*minIt).second);
  }
}

template <typename Q, typename B>
std::vector<B>
PolarCodec<Q, B>::subBlockInterleaver(std::vector<B> &d) {
  int N = d.size();

  // The interleaved vector
  std::vector<B> y(N, 0);

  // Generate sub block interleaver pattern
  // std::map<int, int> J;
  for (int n = 0; n < N; ++n) {
    int i   = (n << 5) / N;
    int J_n = P_i[i] * (N >> 5) + (n % (N >> 5));

    // Interleave
    y[n] = d[J_n];
  }

  return (y);
}

template <typename Q, typename B>
std::vector<Q>
PolarCodec<Q, B>::subBlockDeinterleaver(std::vector<Q> &y) {
  int N = y.size();

  // The interleaved vector
  std::vector<Q> d(N, 0);

  // Generate sub block interleaver pattern
  // std::map<int, int> J;
  for (int n = 0; n < N; ++n) {
    int i   = (n << 5) / N;
    int J_n = P_i[i] * (N >> 5) + (n % (N >> 5));

    // Interleave
    d[J_n] = y[n];
  }

  return (d);
}

template <typename Q, typename B>
std::vector<B>
PolarCodec<Q, B>::bitSelector(std::vector<B> &y, int K, int E) {
  int N = y.size();

  std::vector<B> e;
  e.reserve(E);

  if (E >= N) {
    // Repetition
    for (int k = 0; k + N <= E; k += N) {
      e.insert(e.end(), y.begin(), y.end());
    }
    e.insert(e.end(), y.begin(), y.begin() + (E % N));
  } else {

    // Puncturing or Shortening
    if ((double)K / E <= 7.0 / 16.0) {
      // Puncturing
      e.insert(e.end(), y.begin() + (N - E), y.end());
    } else {
      // Shortening
      e.insert(e.end(), y.begin(), y.begin() + E);
    }
  }

  return (e);
}

template <typename Q, typename B>
std::vector<Q>
PolarCodec<Q, B>::rebitSelector(std::vector<Q> &e, int K, int E) {
  int N = this->findMotherCodeLength(K, E);

  std::vector<Q> y;
  y.reserve(N);
  if (E >= N) {
    // Repetition
    y.insert(y.end(), e.begin(), e.begin() + N);
    // Add the repeated LLR values
    for (int k = N; k + N <= E; k += N) {
      std::transform(e.begin() + k, e.begin() + k + N, y.begin(), y.begin(),
                     [](Q _e, Q _y) { return _e + _y; });
    }
    if (E % N) {
      std::transform(e.end() - (E % N), e.end(), y.begin(), y.begin(),
                     [](Q _e, Q _y) { return _e + _y; });
    }
  } else {
    // Puncturing or Shortening
    if ((double)K / E <= 7.0 / 16.0) {
      // Puncturing
      // In puncturing the puncture bits are assumed to be erased.
      // So we assume LLR value of punctured bits to be 0 (can be either 0 or
      // 1 with same probability).
      y.insert(y.end(), N - E, 0);
      y.insert(y.end(), e.begin(), e.end());
    } else {
      // Shortening
      // In shortening the bits selection is done in such a way that, in the
      // encoded code, the bits to be removed are always 0.
      // The decoded can make use of this a priori information by setting the
      // LLR values to shortened bits to be +inf (probability of 1 is  0).
      y.insert(y.end(), e.begin(), e.end());
      y.insert(y.end(), N - E, 1e20); // std::numeric_limits<Q>::max());
      std::copy(e.begin(), e.end(), y.begin());
    }
  }

  return y;
}

template <typename Q, typename B>
std::vector<B>
PolarCodec<Q, B>::interleaveCodedBits(std::vector<B> &e) {
  // Interleave after bits are coded and rate matched.
  int E = e.size();

  std::vector<B> f(E, 0);
  if (I_BIL == 1) {
    // Find T s.t T(T + 1)/2 >= E
    int t = std::ceil((-1.0 + std::sqrt(1.0 + 8 * E)) / 2);

    int k = 0;
    for (int i = 0; i < t; ++i) {
      int l = i;
      for (int j = 0; j < t - i; ++j) {
        if ((k < E) && (l < E)) {
          f.at(k++) = e.at(l);
          l += (t - j);
        } else {
          break;
        }
      }
    }
  } else {
    std::copy(e.begin(), e.end(), f.begin());
  }

  return (f);
}

template <typename Q, typename B>
std::vector<Q>
PolarCodec<Q, B>::deinterleaveCodedBits(std::vector<Q> &f) {
  // Interleave after bits are coded and rate matched.
  int E = f.size();

  std::vector<Q> e(E, 0);
  if (I_BIL == 1) {
    // Find T s.t T(T + 1)/2 >= E
    int t = std::ceil((-1.0 + std::sqrt(1.0 + 8 * E)) / 2);

    int k = 0;
    for (int i = 0; i < t; ++i) {
      int l = i;
      for (int j = 0; j < t - i; ++j) {
        if ((k < E) && (l < E)) {
          e.at(l) = f.at(k++);
          l += (t - j);
        } else {
          break;
        }
      }
    }
  } else {
    std::copy(f.begin(), f.end(), e.begin());
  }

  return (e);
}

template <typename Q, typename B>
std::vector<B>
PolarCodec<Q, B>::codeBlockConcatenation(std::map<int, std::vector<B>> &f) {
  std::vector<B> g = f[0];
  if (I_seg == 1) {
    // There are two code blocks
    g.insert(g.end(), f[1].begin(), f[1].end());
  }

  return (g);
}

template <typename Q, typename B>
std::map<int, std::vector<Q>>
PolarCodec<Q, B>::codeBlockdeconcatenation(std::vector<Q> &g, int E) {
  // TODO: Handle different code block sizes.
  std::map<int, std::vector<Q>> f;
  f[0] = std::vector<Q>(g.begin(), g.begin() + E);
  if (I_seg == 1) {
    // There are two code blocks
    f[1] = std::vector<Q>(g.begin() + E, g.end());
  }

  return f;
}

template <typename Q, typename B>
std::vector<B>
PolarCodec<Q, B>::_decoder(std::vector<Q> &d, int K, int E) {
  // Find the mother code length
  int N = this->findMotherCodeLength(K, E);

  // Vector to store the LLR values at each stage
  std::vector<Q> y(2 * N, 0);
  std::copy(d.begin(), d.end(), y.begin()); // Copy the received LR values.

  // Vectors to store uhat and xored uhat
  std::vector<B> u_hat(N, 0);
  std::vector<B> u_hat_xored(N, 0);

  // Compute the frozen bits and data bit indices.
  // Information bit indices in ascending order of reliability.
  this->genInfoFrozenBitIndices(K, N, E, Q_I_N_bar, Q_F_N_bar, Q_PC_N);

  // Generate frozen bit indicator vector
  std::vector<bool> F(N, 0);
  for (const auto &f : Q_F_N_bar) {
    F.at(f) = 1;
  }

  // Call the SC decoding method
  Polar::recurssiveSSC(y.begin(), u_hat.begin(), u_hat_xored.begin(), F.begin(),
                       N);

  // Get the information bits from the decoded vector by erasing
  // frozen bits.
  std::sort(Q_I_N_bar.begin(), Q_I_N_bar.end());
  std::vector<B> c_prime(K, 0);
  int i = 0;
  for (auto it = Q_I_N_bar.begin(); it != Q_I_N_bar.end(); ++it) {
    c_prime[i++] = u_hat[*it];
  }
  
  // Now deinterleave and return
  return this->deinterleaver(c_prime);
}

template <typename Q, typename B>
std::vector<B>
PolarCodec<Q, B>::_decoderSCL(std::vector<Q> &d, int K, int E, int RNTI) {
  // Find the mother code length
  int N = this->findMotherCodeLength(K, E);

  // Compute the frozen bits and data bit indices.
  // Information bit indices in ascending order of reliability.
  this->genInfoFrozenBitIndices(K, N, E, Q_I_N_bar, Q_F_N_bar, Q_PC_N);

  // Generate frozen bit indicator vector
  std::vector<bool> F(N, 0);
  for (const auto &f : Q_F_N_bar) {
    F.at(f) = 1;
  }

  // Create a list for paths
  std::list<Polar::Path<Q, B>> paths;
  paths.emplace_back(Polar::Path<Q, B>());
  paths.front().initPath(d);

  // Call the SC decoding method
  Polar::recurssiveSSCL(paths, F.begin(), N);

  std::sort(Q_I_N_bar.begin(), Q_I_N_bar.end());
  bool crcPass = false;
  std::vector<B> c(K);
  std::vector<B> c_prime(K, 0);
  for (auto &p: paths) {
    int i = 0;
    for (auto it = Q_I_N_bar.begin(); it != Q_I_N_bar.end(); ++it) {
      c_prime[i++] = p.u_hat[*it];
    }

    // Now deinterleave
    c = this->deinterleaver(c_prime);

    // Check CRC
    if (this->checkCRC(c, RNTI)) {
      // CRC passed
      crcPass = true;
      break;
    }
  }

  if (!crcPass) {
    // If CRC check did not pass, return a all vector
    // TODO: Plan for blind decoding
    c = std::vector<B>(K, 0);
  }

  return c;
}

template <typename Q, typename B>
std::vector<B>
PolarCodec<Q, B>::polarEncode(std::vector<B> &in_bits, int E, int RNTI) {
  // Full encoding process
  // Check for code block segmentation
  auto codeBlocks = this->codeBlockSegmentation(in_bits, RNTI);

  // TODO: Different rate matched length for different code blocks.
  //
  std::map<int, std::vector<B>> codedBitsBlock;
  for (auto &cbs : codeBlocks) {
    auto &cb = cbs.second;

    // Size of the code block
    int K = cb.size();
    assert(K <= E && "Input bit size larger than encoded bits size.");

    // Interleaving before coding
    auto c_prime = this->interleaver(cb);

    // Encode
    auto d = this->_encoder(c_prime, E);

    // Rate matching
    auto y = this->subBlockInterleaver(d);
    auto e = this->bitSelector(y, K, E);
    auto f = this->interleaveCodedBits(e);

    codedBitsBlock[cbs.first] = f;
  }

  // Code block concatenation
  auto g = this->codeBlockConcatenation(codedBitsBlock);

  return g;
}

template <typename Q, typename B>
std::vector<B>
PolarCodec<Q, B>::polarDecode(std::vector<Q> &coded_bits,bool& isSuccess, int A, int E,
                              int RNTI, DECODETYPE dectype) {
  // Do polar decoding
  int K;
  if (I_seg == 1) {
    // TODO: Also take care of parity bits.
    K = ((A + (A & 0x00000001)) >> 1) + L;
  } else {
    K = A + L;
  }
  assert(K <= E && "Input bit size larger than encoded bits size.");

  // Do the reverse operation done in polar encoding.
  //
  // Split into code blocks.
  auto fr = this->codeBlockdeconcatenation(coded_bits, E);

  std::vector<B> a;
  a.reserve(A);
  for (auto &cb : fr) {
    auto &f = cb.second;

    // Do reverse rate matching
    auto e = this->deinterleaveCodedBits(f);
    auto y = this->rebitSelector(e, K, E);
    auto d = this->subBlockDeinterleaver(y);

    // Do decoding
    std::vector<B> c;
    if (dectype == SCL) {
      bool crcPass = false;
      // First try decoding using SC
      c = this->_decoder(d, K, E);
      if (this->checkCRC(c, RNTI)) {
        crcPass = true;
      }

      // If SC did not work, the try decoding using SCL method.
      if (!crcPass)
        c = this->_decoderSCL(d, K, E, RNTI);
    } else if ((dectype == SSC) || (dectype == SC)) {
      c = this->_decoder(d, K, E);
    }
    if(sum(BoolVectortoBVec(c))==0)
      isSuccess = false;
    else
      isSuccess = isSuccess && checkCRC(c, RNTI);
    // remove CRC and add to code blocks
    a.insert(a.end(), c.begin(), c.end() - L);
  }

  // TODO: Remove the padded zero added during segmentation, if any.
  return a;
}

template <typename Q, typename B>
bool
PolarCodec<Q, B>::polarDecode(std::vector<Q> &coded_bits, int A, int E,
                              int RNTI, std::vector<B> &decodedDCI,
                              DECODETYPE dectype) {
  // Do polar decoding
  int K;
  if (I_seg == 1) {
    // TODO: Also take care of parity bits.
    K = ((A + (A & 0x00000001)) >> 1) + L;
  } else {
    K = A + L;
  }
  assert(K <= E && "Input bit size larger than encoded bits size.");

  // Do the reverse operation done in polar encoding.
  //
  // Split into code blocks.
  auto fr = this->codeBlockdeconcatenation(coded_bits, E);

  std::vector<B> a;
  a.reserve(A);
  bool crcPass = false;
  for (auto &cb : fr) {
    auto &f = cb.second;

    // Do reverse rate matching
    auto e = this->deinterleaveCodedBits(f);
    auto y = this->rebitSelector(e, K, E);
    auto d = this->subBlockDeinterleaver(y);

    // Do decoding
    std::vector<B> c;
    if (dectype == SCL) {
      // First try decoding using SC
      c = this->_decoder(d, K, E);
      if (this->checkCRC(c, RNTI)) {
        crcPass = true;
      }

      // If SC did not work, the try decoding using SCL method.
      if (!crcPass)
        c = this->_decoderSCL(d, K, E, RNTI);
    } else if ((dectype == SSC) || (dectype == SC)) {
      c = this->_decoder(d, K, E);
    }

    // Check CRC for each block
    crcPass = this->checkCRC(c, RNTI);

    // remove CRC and add to code blocks
    a.insert(a.end(), c.begin(), c.end() - L);
  }

  // TODO: Remove the padded zero added during segmentation, if any.
  decodedDCI = std::move(a);
  return(crcPass);
}

// Explicit template initialization
template class PolarCodec<double, bool>;
