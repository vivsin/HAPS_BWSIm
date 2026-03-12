#include "../include/goldSequence.h"

void packedToVector(int len, char *arr, char *vec) {
    int i = 0;
    int ccount = 0;
    while (i < len) {
        char c = arr[ccount];
        for (int k=0; k<8 && i<len; ++k) {
            vec[i] = ((char)(c>>k)&(0x01));
            i++;
        }
        ccount++;
    }
}

namespace PRSG {
// Convert a packed charecter array to int vector.
// void printAsVec(int len, char *arr, int start=0) {
//     char vec[len+start];
//     packedToVector(len+start, arr, vec);
// 
//     for(int i=start; i<len+start; ++i) {
//         std::cout<<vec[i]<<" ";
//     }
//     std::cout<<std::endl;
// }

void
GoldSequence::updateX1(int length) {
    packedPtr ptrx1;
    ptrx1.cp = &(x1[0]);

    // Initialize the first 31 bits.
    *(ptrx1.ip) = 0x00000001;

    // Update the 31st bit separately.
    *(ptrx1.ip) |= (((*(ptrx1.ip) ^ ((*(ptrx1.ip))>>3)) & 0x0001)<<31);

    // Update the rest of the bits.
    for (int i=0; i<length; i+=24) {
        *(ptrx1.ip+1) = ((*(ptrx1.ip))>>1) ^ ((*(ptrx1.ip))>>4);

        ptrx1.cp += 3;
    }
}

void
GoldSequence::updateX2(int seed, int length) {
    packedPtr ptrx2;
    ptrx2.cp = &(x2[0]);

    // Initialize the first 31 bits.
    *(ptrx2.ip) = (uint32_t)seed;

    // Update the 31st bit separately.
    *(ptrx2.ip) |= (((*(ptrx2.ip) ^ ((*(ptrx2.ip))>>1) ^
                    ((*(ptrx2.ip))>>2) ^ ((*(ptrx2.ip))>>3)) & 0x0001)<<31);

    // Update the rest of the bits.
    for (int i=0; i<length+N_c+256; i+=24) {
        *(ptrx2.ip+1) = ((*(ptrx2.ip))>>1) ^ (((*(ptrx2.ip))>>2) ^
                        ((*(ptrx2.ip))>>3) ^ (*(ptrx2.ip))>>4);

        ptrx2.cp += 3;
    }
}

void
GoldSequence::xorX1X2(int length) {
    packedPtr ptrgold, ptrx1, ptrx2;

    size_t st = N_cBy8;

    // We ignore the first Nc = 1600 bits of x1 and x2.
    ptrgold.cp  = &(gold[0]);
    ptrx1.cp    = &(x1[st]);
    ptrx2.cp    = &(x2[st]);

    for (int i=0; i<length+N_c; i+=64) {
        *(ptrgold.lp++) = *(ptrx1.lp++) ^ *(ptrx2.lp++);
    }
}

void
GoldSequence::generateGoldSequence(int seed, int length) {
    // Check if we can reuse the existing  X1 sequence
    if (!(x1[0] & 0x80000001)) {
        updateX1(8*LENGTH);
    }

    // Update the X2 sequence
    updateX2(seed, length);

    // Now we have both x1 and x2 ready and filled.
    // We can xor them to get the Gold sequence.
    xorX1X2(length);
}

void generateGoldSequence(int seed, int length, char *goldvec) {
    PRSG::GoldSequence *ginstance = PRSG::GoldSequence::getInstance();

    ginstance->generateGoldSequence(seed, length);
    char *gold = ginstance->getGoldSequence();

    packedToVector(length, gold, goldvec);

}
}



namespace goldIvec {
  ivec
  generateGoldSequence(int seed, int length) {
    if (length > PRSG::MAX_LENGTH) {
        std::cout<<"Maximum length allowed is : "<<PRSG::MAX_LENGTH<<std::endl;
        std::cout<<"Current lenght is : "<<length<<std::endl;
        std::cout<<"MAX_LENGTH is defined in lib/L1/include/goldSequence.h"<<std::endl;
        std::cout<<"Aborting!!"<<std::endl;
        abort();
    }

    char goldarr[PRSG::MAX_LENGTH];

    int newLen = (length / 8) * 8 + 256;
    if (newLen > PRSG::MAX_LENGTH) {
      newLen = PRSG::MAX_LENGTH;
    }

    PRSG::generateGoldSequence(seed, newLen, goldarr);

    ivec goldivec = zeros_i(length);

    for (int i=0; i<length; ++i) {
      goldivec[i] = static_cast<int>(goldarr[i]);
    }

    return(std::move(goldivec));
  }
}

PRSG::GoldSequence *PRSG::GoldSequence::instance = 0;

// #ifdef _UNIT_TEST_
// #include <climits>
// #include <iostream>
// #include <random>
// #include <itpp/base/mat.h>
// #include <itpp/base/matfunc.h>
// #include <itpp/base/specmat.h>
// 
// namespace ITPP {
// template<class Num_T>	void append(itpp::Vec<Num_T> &baseVector, const Num_T t)
// {
//   int size = baseVector.size();
//   baseVector.set_size(size+1,true);
//   baseVector(size) = t;
// }
// 
// itpp::ivec generateGoldSequence(int Cinit, int seqLength) {
//     int Nc = 1600,nSeq=31;
//     itpp::bvec firstSequence = itpp::zeros_b(nSeq);
//     firstSequence(0) = 1;
// 
//     itpp::bvec secSequence = itpp::reverse(itpp::dec2bin(nSeq, Cinit));
// 
//     for (int iBit = 0;iBit < (Nc+seqLength-nSeq);iBit ++)
//     {
//       append(firstSequence, firstSequence(iBit + 3) + firstSequence(iBit));
//       append(secSequence, secSequence(iBit + 3) + secSequence(iBit + 2) +
//               secSequence(iBit + 1) + secSequence(iBit));
//     }
// 
//     itpp::ivec tempGoldSequence = itpp::to_ivec(firstSequence(Nc,
//                             Nc+seqLength-1) + secSequence(Nc,Nc+seqLength-1));
// 
//     return tempGoldSequence;
// }
// }
// 
// int main() {
//     int n_iter  = 2;
//     int len     = 500000;
// 
//     std::cout<<"Checking if old and new method gives the same output.";
//     std::cout<<std::endl;
//     char vec[len];
//     for (int k=0; k<n_iter; ++k) {
//         std::random_device rd;
//         std::mt19937 rng(rd());
//         std::uniform_int_distribution<int> uni(0, INT_MAX);
// 
//         int seed = uni(rng);
//         std::cout<<"Checking for seed = "<<seed<<std::endl;
// 
//         generateGoldSequence(seed, len, vec);
// 
//         itpp::ivec goldivec = ITPP::generateGoldSequence(seed, len);
// 
//         for( int i=0 ; i<len; ++i) {
//             if ((bool)vec[i] != (bool)goldivec(i)) {
//                 std::cout<<"Not same at seed = "<<seed<<" at i = "<<i;
//                 std::cout<<std::endl;
//                 std::cout<<"vec[i] = "<<vec[i]<<std::endl;
//                 std::cout<<"ivec[i] = "<<goldivec[i]<<std::endl;
//                 abort();
//             }
//         }
//     }
//     std::cout<<"Both method gives the same output."<<std::endl;
//     return 0;
// }
// #endif
