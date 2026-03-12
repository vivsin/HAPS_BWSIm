#ifndef _GOLDSEQUENCE_H_
#define _GOLDSEQUENCE_H_

#include "../../../Frozen/simSupport/include/simSupport.h"

// Define _UNIT_TEST_ to enable self-check.
// #define _UNIT_TEST_

namespace PRSG {
  using packedPtr =  union _packedPtr {
    uint64_t *lp;
    uint32_t *ip;
    char     *cp;
  };

  const int MAX_LENGTH = 500000;
  const int LENGTH     = 50000;
  const int N_c        = 1600;
  const int N_cBy8     = 200;


  class GoldSequence {
    char x1[LENGTH]   = { 0 };
    char x2[LENGTH]   = { 0 };
    char gold[LENGTH] = { 0 };

    static GoldSequence *instance;

    GoldSequence() {
    };

public:

    void
    updateX1(int length);
    void
    updateX2(int seed, int length);

    void
    xorX1X2(int length);

    void
    generateGoldSequence(int seed, int length);

    char*
    getX1() {
      return(&x1[0]);
    }

    char*
    getX2() {
      return(&x2[0]);
    }

    char*
    getGoldSequence() {
      return(&gold[0]);
    }

    static GoldSequence*
    getInstance() {
      if (!instance) {
        instance = new GoldSequence;
      }

      return(instance);
    };
  };

  void
  generateGoldSequence(int seed, int length, char *goldvec);
}

namespace goldIvec {
  ivec
  generateGoldSequence(int seed, int length);
}

#endif  // _GOLDSEQUENCE_H_
