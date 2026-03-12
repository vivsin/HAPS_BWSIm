#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <list>
#include <vector>

// The maximum list size of polar list decoder.
// The recommended size is 8.
#define MAX_LIST_SIZE 8

namespace Polar {

template <class InputIt1, class InputIt2, class InputIt3, class OutputIt,
          class TernaryOperation>
OutputIt
transform(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt3 first3,
          OutputIt d_first, TernaryOperation ternary_op) {
  for (; first1 != last1; ++first1, ++first2, ++first3, ++d_first) {
    *d_first = ternary_op(*first1, *first2, *first3);
  }

  return d_first;
}

template <typename Q = double, typename B = bool>
void
recurssiveSC(typename std::vector<Q>::iterator y,
             typename std::vector<B>::iterator u_hat,
             typename std::vector<B>::iterator u_hat_xored,
             std::vector<bool>::iterator F, const int N);

template <typename Q = double, typename B = bool>
void
recurssiveSSC(typename std::vector<Q>::iterator y,
              typename std::vector<B>::iterator u_hat,
              typename std::vector<B>::iterator u_hat_xored,
              std::vector<bool>::iterator F, const int N);

template <typename Q, typename B>
class Path {
public:
  std::list<std::vector<Q>> llrs;
  std::vector<B> u_hat;
  std::vector<B> u_hat_xored;

  double pathMetric;

  int bitIdx;             // The bit index being set
  int N;                  // Mother code length
  int depth;              // Var to store current depth
  Q sgn[2] = {1.0, -1.0}; // 1 -2*b for b = 0 & 1

  int N_max; // Maximum value of N

  int curX; // Var to point towards correct index of xor'ed u_hat

  bool isFlipped; // In the current iteration is the path flipped.
  Path()
      : pathMetric(0.0), bitIdx(0), N(0), depth(0), N_max(0), curX(0),
        isFlipped(false){};

  void
  initPath(std::vector<Q> &llr) {
    /*
     *  Copy the channel LLR values and initialize everything.
     */
    llrs.push_back(llr);
    N     = llr.size();
    N_max = N;
    depth = (int)std::log2(N);

    u_hat       = std::vector<B>();
    u_hat_xored = std::vector<B>();
    u_hat.reserve(N);
    u_hat_xored.reserve(N);
  }

  void
  splitPath(Path<Q, B> *other) {
    // Copy values from other object.
    this->llrs        = other->llrs;
    this->u_hat       = other->u_hat;
    this->u_hat_xored = other->u_hat_xored;
    this->pathMetric  = other->pathMetric;
    this->bitIdx      = other->bitIdx - 1;
    this->N           = other->N;
    this->N_max       = other->N_max;
    this->curX        = other->curX;

    this->u_hat.pop_back();
    this->u_hat_xored.pop_back();
  }

  void
  goLeft() {
    // Operation to do while going left on the decode tree graph.

    auto &curLLR = llrs.back();
    llrs.emplace_back(std::vector<Q>());
    auto &nextLLRs = llrs.back();
    nextLLRs.reserve(N >> 1);

    std::transform(curLLR.begin(), curLLR.begin() + (N >> 1),
                   curLLR.begin() + (N >> 1), std::back_inserter(nextLLRs),
                   [this](Q l1, Q l2) {
                     return sgn[l1 < 0.0] * sgn[l2 < 0.0] *
                            (std::min(std::abs(l1), std::abs(l2)));
                   });

    N >>= 1;
    depth--;
  }

  void
  goRight() {
    auto &curLLR = llrs.back();
    llrs.emplace_back(std::vector<Q>());
    auto &nextLLRs = llrs.back();
    nextLLRs.reserve(N >> 1);
    auto xit = this->u_hat_xored.begin() + curX;

    Polar::transform(
        curLLR.begin(), curLLR.begin() + (N >> 1), curLLR.begin() + (N >> 1),
        xit, std::back_inserter(nextLLRs),
        [this](Q l1, Q l2, B b) { return l2 + (this->sgn[b] * l1); });

    N >>= 1;
    depth--;
    curX += N;
  }

  void
  goUp(bool doXor = true) {
    if (bitIdx == N_max) // Already decoded N_max bits, so done
      return;

    // Do combine operation.
    if ((N != 1) && doXor) {
      auto xit = u_hat_xored.begin() + curX;
      std::transform(xit, xit + (N >> 1), xit + (N >> 1), xit,
                     std::bit_xor<B>());
    }

    // FIgure out if we have to go up from the left or right child.
    bool fromRight = true; // True if going up from right child.
    if (bitIdx % 2 == 1) {
      fromRight = false;
    } else if (N == 1) {
      fromRight = true;
    } else {
      int subTreeBitIdx = (bitIdx - 1) % (N << 1);

      if (subTreeBitIdx < (N)) {
        fromRight = false;
      } else {
        fromRight = true;
      }
    }

    if (fromRight) {
      // Going up from right
      curX -= N;
    }
    if (curX < 0) {
      abort();
    }

    // Ignore unwanted LLR values.
    llrs.pop_back();
    depth++;
    N <<= 1;
  }

  bool
  isLeaf() {
    // Are we at a leaf node?
    return N == 1;
  }

  Q
  getLeafLLR() {
    // If we are at leaf node, return the LLR value.
    if (N != 1) {
      abort();
    }
    if (llrs.back().size() != 1) {
      abort();
    }
    return llrs.back().front();
  }

  void
  setBit(B bit) {
    u_hat.push_back(bit);
    u_hat_xored.push_back(bit);
    bitIdx++;
  }

  Q
  getPM() {
    return pathMetric;
  }

  void
  updatePM(Q dm) {
    pathMetric += dm;
  }

  bool
  operator<(const Path &other) const {
    return this->pathMetric < other.pathMetric;
  }

  std::vector<B>
  getUhat() {
    return (this->u_hat);
  }
};

template <typename Q = double, typename B = bool>
void
recurssiveSCL(std::list<Path<Q, B>> &paths,
              typename std::vector<bool>::iterator F, const int N);

template <typename Q = double, typename B = bool>
void
recurssiveSSCL(std::list<Path<Q, B>> &paths,
               typename std::vector<bool>::iterator F, const int N);
} // namespace Polar
