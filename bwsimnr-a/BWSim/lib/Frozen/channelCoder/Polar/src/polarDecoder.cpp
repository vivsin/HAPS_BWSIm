#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <list>

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

namespace Polar {
template <typename Q, typename B>
void
recurssiveSC(typename std::vector<Q>::iterator y,
             typename std::vector<B>::iterator u_hat,
             typename std::vector<B>::iterator u_hat_xored,
             std::vector<bool>::iterator F, const int N) {
  /*
   *  A recursive implementation of successive cancellation decoder by
   *  Arikan. For more info about the SC decoder refer the paper by Arikan
   *  about polar codes. Also refer the course on NPTEL by Prof. Andrew
   *  named "LDPC and Polar Codes in 5G standards" for details about
   *  implementation.
   *  Here we consider a tree based Polar code structure, where the leaf
   *  leaf nodes of the tree corresponds to the bits to be encoded(bit channel)
   *  and the  root node corresponds to the encoded bits(N bit/vector channel).
   *  The LLR values received from the channel corresponds to the LLR values
   *  at the root nodes. The LLR values from root nodes are then passed down
   *  to the other nodes till we reach a leaf node where decision is made.
   *  There are three operations used to calculate the LLR of other nodes
   *  by using the received LLRs and already decoded bits. These are
   *    1. f operation : This is used for passing a message from parent node
   *                     to its left child. f() is a min-sum operation
   *                     combining two LLR to one given by
   *          f(l1, l2) = sgn(l1) * sgn(l2) * min(abs(l1), abs(l2))
   *    2. g operation : This is used for passing a message from parent node
   *                     to its right node. This also need the bit decoded
   *                     by the left child as its input. This is given by
   *       g(l1, l2, b) = l2 + (1 - 2*b)*l1
   *    3. combine     : Combines the bits decoded by a nodes left and right
   *                     child and then passes this combined bits to its
   *                     parent. This is given by
   *      combine(b1, b2) = [b1 xor b2 , b2]
   *
   */

  // Check if its a leaf node
  if (N == 1) {
    if (*F == 1) {
      // Frozen bit
      *u_hat = 0;
    } else {
      *u_hat = static_cast<B>((*y) < 0.0);
    }

    *u_hat_xored = *u_hat;
  } else {
    // Regular Node
    //

    // f() operation
    // TODO: replace 1 - 2b by look up table
    // TODO: replace N / 2 by var
    std::transform(y, y + (N / 2), y + (N / 2), y + N, [](Q l1, Q l2) {
      return (1 - 2 * ((int)(l1 < 0))) * (1 - 2 * ((int)(l2 < 0))) *
             (std::min(std::abs(l1), std::abs(l2)));
    });

    recurssiveSC<Q, B>(y + N, u_hat, u_hat_xored, F, (N / 2));

    // g() operation
    auto it = u_hat_xored;
    std::transform(y, y + (N / 2), y + (N / 2), y + N,
                   [&it](Q l1, Q l2) { return l2 + ((1 - 2 * (*it++)) * l1); });

    recurssiveSC<Q, B>(y + N, u_hat + (N / 2), u_hat_xored + (N / 2),
                       F + (N / 2), (N / 2));

    // combine operation
    // TODO: replace x + y % 2 by xor
    std::transform(u_hat_xored, u_hat_xored + (N / 2), u_hat_xored + (N / 2),
                   u_hat_xored, [](B x, B y) { return (x + y) % 2; });
  }
}

template <typename Q, typename B>
void
recurssiveSSC(typename std::vector<Q>::iterator y,
              typename std::vector<B>::iterator u_hat,
              typename std::vector<B>::iterator u_hat_xored,
              std::vector<bool>::iterator F, const int N) {

  /*
   *  Simplified successive cancellation decoder is a simplified version of
   *  SC decoder where decisions are made a intermediary nodes also. While
   *  decoding a intermediary nodes if we can find out if all the leaves of
   *  the sub-tree of the node is either frozen or information node, then we
   *  can do a one shot decoding instead of traversing the tree.
   *  If all the leaves frozen, then its a rate-0 zero node all the bits are
   *  determined to be 0.
   *  Else if all the leaves are information nodes, then its a rate-1 node and
   *  the LLR are hard decoded and using this the final message is calculated.
   *  For more info refer the paper by Alamdar-Yazdi, 2011.
   */
  // Check if its a leaf node
  if (N == 1) {
    if (*F == 1) {
      // Frozen bit
      *u_hat = 0;
    } else {
      *u_hat = static_cast<B>((*y) < 0.0);
    }

    *u_hat_xored = *u_hat;
  } else {
    // Regular Node
    //
    // Check if its a rate 0 or rate 1 node
    // if all of F is true, then its a rate 0 node
    // F is indicator array whose elements are true if the bit corresponding
    // to array index is frozen.
    if (std::all_of(
            F, F + N,
            std::bind(std::equal_to<bool>(), std::placeholders::_1, true))) {
      // All bits are frozen, i.e, rate-0 node
      std::fill(u_hat, u_hat + N, 0);
      std::fill(u_hat_xored, u_hat_xored + N, 0);
      return;
    } else if (std::all_of(F, F + N,
                           std::bind(std::equal_to<bool>(),
                                     std::placeholders::_1, false))) {
      // All bits are information node, i.e, rate-1 node
      std::transform(y, y + N, u_hat_xored,
                     [](Q _y) { return static_cast<B>((_y) < 0.0); });

      std::copy(u_hat_xored, u_hat_xored + N, u_hat);

      // Do Kronecker product with G_n
      for (auto n = (N >> 1); n > 0; n >>= 1)
        for (auto j = 0; j < N; j += 2 * n)
          for (auto i = 0; i < n; i++)
            *(u_hat + j + i) = *(u_hat + j + i) ^ *(u_hat + n + j + i);
      return;
    }

    // Not a rate-1/0 node
    // Just another regular node
    // f()
    std::transform(y, y + (N / 2), y + (N / 2), y + N, [](Q l1, Q l2) {
      return (1 - 2 * ((int)(l1 < 0))) * (1 - 2 * ((int)(l2 < 0))) *
             (std::min(std::abs(l1), std::abs(l2)));
    });

    recurssiveSSC<Q, B>(y + N, u_hat, u_hat_xored, F, (N / 2));

    // g()
    auto it = u_hat_xored;
    std::transform(y, y + (N / 2), y + (N / 2), y + N,
                   [&it](Q l1, Q l2) { return l2 + ((1 - 2 * (*it++)) * l1); });

    recurssiveSSC<Q, B>(y + N, u_hat + (N / 2), u_hat_xored + (N / 2),
                        F + (N / 2), (N / 2));

    // combine
    std::transform(u_hat_xored, u_hat_xored + (N / 2), u_hat_xored + (N / 2),
                   u_hat_xored, [](B x, B y) { return (x + y) % 2; });
  }
}

template <typename Q, typename B>
void
recurssiveSCL(std::list<Path<Q, B>> &paths,
              typename std::vector<bool>::iterator F, const int N) {
  // Check if leaf node
  if (N == 1) {
    if (!paths.front().isLeaf()) {
      abort();
    }

    if (*F == 1) {
      // Frozen
      for (auto &path : paths) {
        path.setBit(0);
        auto dm = path.getLeafLLR();
        if (dm < 0.0) {
          path.updatePM(std::abs(dm));
        }
      }
    } else {
      // Info
      // Get the maximum path metric values in the list
      int maxListSize = MAX_LIST_SIZE;
      auto maxPM      = paths.back().getPM();
      for (auto &path : paths) {
        if (path.isFlipped) {
          continue;
        }

        B bit = path.getLeafLLR() < 0.0;
        path.setBit(bit);

        // Should we split paths
        auto newPM = path.getPM() + std::abs(path.getLeafLLR());
        if ((newPM < maxPM) || ((int)paths.size() < maxListSize)) {
          // Split path and add it.

          paths.emplace_back(Path<Q, B>());
          auto &newPath = paths.back();
          auto newDm    = std::abs(path.getLeafLLR());
          newPath.splitPath(&path); // Copy the current path
          newPath.setBit(!bit);     // Flip the bit
          newPath.updatePM(newDm);  // Update the PM
          newPath.isFlipped = true; // Set the path as flipped
        }
      }
    }
    // Done splitting paths, now prune them
    paths.sort();
    if ((int)paths.size() > MAX_LIST_SIZE) {
      paths.resize(MAX_LIST_SIZE);
    }

    // Go to the leaf's parent
    for (auto &path : paths) {
      path.isFlipped = false;
      path.goUp();
    }
  } else {
    // Not a leaf node, so a normal node.

    int Nby2 = N >> 1;
    for (auto &path : paths) {
      path.goLeft();
    }

    recurssiveSCL(paths, F, Nby2);

    for (auto &path : paths) {
      path.goRight();
    }

    recurssiveSCL(paths, F + Nby2, Nby2);

    for (auto &path : paths) {
      path.goUp();
    }
  }
}

template <typename Q, typename B>
void
recurssiveSSCL(std::list<Path<Q, B>> &paths,
               typename std::vector<bool>::iterator F, const int N) {
  // Check if leaf node
  if (N == 1) {
    if (!paths.front().isLeaf()) {
      abort();
    }

    if (*F == 1) {
      // Frozen
      for (auto &path : paths) {
        path.setBit(0);
        auto dm = path.getLeafLLR();
        if (dm < 0.0) {
          path.updatePM(std::abs(dm));
        }
      }
    } else {
      // Info
      // Get the maximum path metric values in the list
      int maxListSize = MAX_LIST_SIZE;
      auto maxPM      = paths.back().getPM();
      for (auto &path : paths) {
        if (path.isFlipped) {
          continue;
        }

        B bit = path.getLeafLLR() < 0.0;
        path.setBit(bit);

        // Should we split paths
        auto newPM = path.getPM() + std::abs(path.getLeafLLR());
        if ((newPM < maxPM) || ((int)paths.size() < maxListSize)) {
          // Split path and add it.

          paths.emplace_back(Path<Q, B>());
          auto &newPath = paths.back();
          auto newDm    = std::abs(path.getLeafLLR());
          newPath.splitPath(&path); // Copy the current path
          newPath.setBit(!bit);     // Flip the bit
          newPath.updatePM(newDm);  // Update the PM
          newPath.isFlipped = true; // Set the path as flipped
        }
      }
    }
    // Done splitting paths, now prune them
    paths.sort();
    if ((int)paths.size() > MAX_LIST_SIZE) {
      paths.resize(MAX_LIST_SIZE);
    }

    // Go to the leaf's parent
    for (auto &path : paths) {
      path.isFlipped = false;
      path.goUp();
    }
  } else {
    // Not a leaf node, so a normal node.
    // Check for rate 0/1 node
    auto allTrue =
        std::bind(std::equal_to<bool>(), std::placeholders::_1, true);
    auto allFalse =
        std::bind(std::equal_to<bool>(), std::placeholders::_1, false);

    if (std::all_of(F, F + N, allTrue)) {
      // A Rate 0 node, all the bits are set to 0.
      for (auto &path : paths) {
        auto &llr = path.llrs.back();
        assert((int)llr.size() == N);
        for (int i = 0; i < N; i++) {
          path.setBit(0);
          Q dm = llr[i];
          if (dm < 0.0)
            path.updatePM(std::abs(dm));
        }
        path.goUp();
      }

      paths.sort();

      return;
    } else if (std::all_of(F, F + N, allFalse)) {
      // A Rate 1 node, hard code the llr bits to find the encoded bits.
      auto maxPM    = paths.back().getPM();
      auto listSize = paths.size();
      for (auto &path : paths) {
        if (path.isFlipped)
          continue;

        auto &llr = path.llrs.back();
        assert((int)llr.size() == N);

        // The encoded bits are obtained by directly hard decoding the LLR
        // values
        std::transform(llr.begin(), llr.end(),
                       std::back_inserter(path.u_hat_xored),
                       [](Q _y) { return static_cast<B>(_y < 0.0); });

        // To get the u_hat multiply by G_n
        // First copy encoded bits to decoded bits
        path.u_hat.insert(path.u_hat.end(), path.u_hat_xored.end() - N,
                          path.u_hat_xored.end());
        auto u_hat = path.u_hat.end() - N;
        for (auto n = (N >> 1); n > 0; n >>= 1)
          for (auto j = 0; j < N; j += 2 * n)
            for (auto i = 0; i < n; i++)
              *(u_hat + j + i) = *(u_hat + j + i) ^ *(u_hat + n + j + i);

        path.bitIdx += N;

        // Now we will flip the bits
        // We will be flipping only 2 bits which are least reliable.
        // Reliability is determined by the abs of llr values.
        // So we want to two bits corresponding to the location of the
        // two minimum absolute llr values.
        std::array<Q, 2> _llr;
        std::partial_sort_copy(
            llr.begin(), llr.end(), _llr.begin(), _llr.end(),
            [](Q x, Q y) { return std::abs(x) < std::abs(y); });
        auto min1 = std::find(llr.begin(), llr.end(), _llr[0]);
        auto min2 = std::find(llr.begin(), llr.end(), _llr[1]);

        // Now find the path metric for the new path.
        Q newPM1 = path.getPM() + std::abs(*min1);
        Q newPM2 = path.getPM() + std::abs(*min2);

        if ((newPM1 < maxPM) || (listSize < MAX_LIST_SIZE)) {
          // Add the path with bit at min2 location flipped.
          auto distance = std::distance(llr.begin(), min1);

          // Copy the current path.
          paths.push_back(path);
          auto &newPath     = paths.back();
          newPath.isFlipped = true; // Set the path as flipped
          newPath.updatePM(std::abs(*min1));

          // Flip the bit.
          *(newPath.u_hat_xored.end() - N + distance) =
              *(newPath.u_hat_xored.end() - N + distance) ^ true;

          // The encoded bits are obtained by directly hard decoding the LLR
          // values
          //
          // To get the u_hat multiply by G_n
          // First copy encoded bits to decoded bits
          std::copy(newPath.u_hat_xored.end() - N, newPath.u_hat_xored.end(),
                    newPath.u_hat.end() - N);

          // Decode the xor'ed bits
          u_hat = newPath.u_hat.end() - N;
          for (auto n = (N >> 1); n > 0; n >>= 1)
            for (auto j = 0; j < N; j += 2 * n)
              for (auto i = 0; i < n; i++)
                *(u_hat + j + i) = *(u_hat + j + i) ^ *(u_hat + n + j + i);
        }

        if ((newPM2 < maxPM) || (listSize < MAX_LIST_SIZE)) {
          // Add the path with bit at min2 location flipped.
          auto distance = std::distance(llr.begin(), min2);

          // Copy the current path.
          paths.push_back(path);
          auto &newPath     = paths.back();
          newPath.isFlipped = true; // Set the path as flipped
          newPath.updatePM(std::abs(*min2));

          // Flip the bit.
          *(newPath.u_hat_xored.end() - N + distance) =
              *(newPath.u_hat_xored.end() - N + distance) ^ true;

          // The encoded bits are obtained by directly hard decoding the LLR
          // values
          //
          // To get the u_hat multiply by G_n
          // First copy encoded bits to decoded bits
          std::copy(newPath.u_hat_xored.end() - N, newPath.u_hat_xored.end(),
                    newPath.u_hat.end() - N);

          // Decode the xor'ed bits
          u_hat = newPath.u_hat.end() - N;
          for (auto n = (N >> 1); n > 0; n >>= 1)
            for (auto j = 0; j < N; j += 2 * n)
              for (auto i = 0; i < n; i++)
                *(u_hat + j + i) = *(u_hat + j + i) ^ *(u_hat + n + j + i);
        }

        if ((newPM1 + newPM2 < maxPM) || (listSize < MAX_LIST_SIZE)) {
          // Add the path with bit at min2 location flipped.
          auto distance1 = std::distance(llr.begin(), min1);
          auto distance2 = std::distance(llr.begin(), min2);

          // Copy the current path.
          paths.push_back(path);
          auto &newPath     = paths.back();
          newPath.isFlipped = true; // Set the path as flipped
          newPath.updatePM(std::abs(*min1) + std::abs(*min2));

          // Flip the bits
          *(newPath.u_hat_xored.end() - N + distance1) =
              *(newPath.u_hat_xored.end() - N + distance1) ^ true;
          *(newPath.u_hat_xored.end() - N + distance2) =
              *(newPath.u_hat_xored.end() - N + distance2) ^ true;

          // The encoded bits are obtained by directly hard decoding the LLR
          // values
          //
          // To get the u_hat multiply by G_n
          // First copy encoded bits to decoded bits
          std::copy(newPath.u_hat_xored.end() - N, newPath.u_hat_xored.end(),
                    newPath.u_hat.end() - N);

          // Decode the xor'ed bits
          u_hat = newPath.u_hat.end() - N;
          for (auto n = (N >> 1); n > 0; n >>= 1)
            for (auto j = 0; j < N; j += 2 * n)
              for (auto i = 0; i < n; i++)
                *(u_hat + j + i) = *(u_hat + j + i) ^ *(u_hat + n + j + i);
        }
      }

      // Done splitting paths, now prune them
      paths.sort();
      if ((int)paths.size() > MAX_LIST_SIZE) {
        paths.resize(MAX_LIST_SIZE);
      }

      // Go to the leaf's parent
      for (auto &path : paths) {
        path.isFlipped = false;
        // Do not do XOR operation while going up.
        path.goUp(false);
      }

      return;
    }

    int Nby2 = N >> 1;
    for (auto &path : paths) {
      path.goLeft();
    }

    recurssiveSSCL(paths, F, Nby2);

    for (auto &path : paths) {
      path.goRight();
    }

    recurssiveSSCL(paths, F + Nby2, Nby2);

    for (auto &path : paths) {
      path.goUp();
    }
  }
}

//
// Explicit template initialization.
//
template void
recurssiveSC(std::vector<double>::iterator y, std::vector<bool>::iterator u_hat,
             std::vector<bool>::iterator u_hat_xored,
             std::vector<bool>::iterator F, const int N);

template void
recurssiveSSC(std::vector<double>::iterator y,
              std::vector<bool>::iterator u_hat,
              std::vector<bool>::iterator u_hat_xored,
              std::vector<bool>::iterator F, const int N);

template class Path<double, bool>;
template class Path<double, int>;

template void
recurssiveSCL(std::list<Polar::Path<double, bool>> &paths,
              typename std::vector<bool>::iterator F, const int N);

template void
recurssiveSCL(std::list<Polar::Path<double, int>> &paths,
              typename std::vector<bool>::iterator F, const int N);

template void
recurssiveSSCL(std::list<Path<double, bool>> &paths,
               typename std::vector<bool>::iterator F, const int N);
} // namespace Polar
