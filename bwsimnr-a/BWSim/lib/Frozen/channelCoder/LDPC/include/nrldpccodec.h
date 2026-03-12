#ifndef NRLDPCCODEC_H
#define NRLDPCCODEC_H

#include "ldpccodec.h"
#include "ldpcencoder.h"

void populateShiftCoefficientZset();

class NRLDPCcodec
{
    
public:
    
    CodeStore cs;
    CodeStore_b cs_b;
    std::set<int>liftingSizeSet;
    
    void populateShiftCoefficientZset()
    {
        auto sa={2,3,5,7,9,11,13,15};
        
        for(auto a:sa)
        {
            //  std::cout<<a<<std::endl;
        }
        auto sj={0,1,2,3,4,5,6,7};
        
        for(auto a:sa)
            
        {
            std::set<int> z;
            for(auto j:sj)
            {
                if(a<5 ||j<7)
                    if(a<7||j<6)
                        if(a<13||j<5)
                        {
                            auto Z=a*(0x1<<j);//a*pow(2,j)
                            z.insert(Z);
                            // std::cout<<a<<" "<<j<<" "<<Z<<std::endl;
                            liftingSizeSet.insert(Z);
                        }
                        
            }
            
            // liftValueZSet.insert(z);
            
        }
        //  for(auto x:liftingSizeSet)
        //    {
        //      std::cout<<x<<",";
        //    }
    }
    NRLDPCcodec(){populateShiftCoefficientZset();}
    ~NRLDPCcodec()
    {
        cs.ldpchmatclean();
        cs_b.ldpchmatclean_b();
    }
    std::vector<bool> encode(std::vector<bool> inputBits,int bg);
    template <class T> std::vector<T> encode(std::vector<T> inputVector, int bg);
    template<class T> std::vector<bool> decode(std::vector<T> softLLRval,int k,int bg);
    template<class T, class T1> T1 decode(T softLLRval,int k,int bg);
    void findParamKz(int bg, int k, int &Kb, int &K, int &z);
    
    
    
};
template<class T, class T1>
T1 NRLDPCcodec::decode(T softLLRval, int k, int bg)
{

}

template<class T>
std::vector<bool> NRLDPCcodec::decode(std::vector<T> softLLRval, int k, int bg)
{
 int K;
  int z;
  int Kb;

   findParamKz(bg,k, Kb, K, z);
   std::vector<T> llr(2*z,0);
   llr.reserve((84-16*bg)*z); //68z for bg=1, 52z for bg =2 //llr.reserve(3*K+2*z);
   llr.insert(llr.end(),softLLRval.begin(),softLLRval.begin()+k-2*z);
   llr.resize(K,100);     // As these filler bits are zeros (obvious), A high llr value (e.g. 100) is given for them.
   llr.insert(llr.end(),softLLRval.begin()+k-2*z,softLLRval.end());

  LDPCdecoder<T> ldpch(cs,llr,z,bg);


    auto decdata=ldpch.decode(50);

    std::vector<bool> opbit(decdata.begin(),decdata.begin()+k);
    
    return opbit;
}

template<class T>
std::vector<T> NRLDPCcodec::encode(std::vector<T> inputVector, int bg)
{


}

 void findParamKz(int bg, int k, int & Kb, int & K, int & z);

inline  std::vector<bool> NRLDPCcodec::encode(std::vector<bool> inputBits, int bg)
{

  int Kb;
  int k=inputBits.size();
  int K;
  int z;
  findParamKz(bg,k, Kb, K, z);
  //resizing inbits to K by appending zeros

  inputBits.resize(K,0);
  //encoding
  LDPCencoder ldpcenc(cs_b,z,bg);
  std::vector<bool> encodeddata= ldpcenc.encode(inputBits);
  //removing addedbits, first 2zbits also
  std::vector<bool> finalencodeddata((inputBits.begin()+(2*z)),(inputBits.begin()+k));

  finalencodeddata.insert(finalencodeddata.end(),encodeddata.begin()+K,encodeddata.end());

  return finalencodeddata;
}

#endif // NRLDPCCODEC_H
