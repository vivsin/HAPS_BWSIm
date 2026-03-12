#ifndef NRSEGMENTCONCAT_H
#define NRSEGMENTCONCAT_H

#include<vector>
#include<cmath>
#include<iostream>
#include "NRCRC.h"
inline std::vector <std::vector<bool>> segmentInputbitsWithoutCRC(std::vector<bool> b, int C)
{
    std::vector <std::vector<bool>> c;
    c.reserve(C);
    auto L=24;   // 38.212 CRC size L=24
    int B_ = b.size()+C*L;
    auto r = (B_)%C;
    int K_,startPos=0;
    for (int i = 0; i<C;i++)
    {
        if(i<r)   // This is assuming B_ may not be multiple of C.
        {
          K_ = B_/C+1;  // //FIXME ceiling for LTE compatability
        }
        else
        {
          K_ = B_/C;
        }
//         std::cout << K_<< " " << r << " " << i << " " << startPos << " "<<std::endl;
        std::vector<bool> x (b.begin()+startPos,b.begin()+startPos+K_-L);
        c.push_back(x);
        startPos+=K_-L;
    }
   return std::move(c); 
}

inline std::vector<bool> desegmentInputbitsWithoutCRC (std::vector<std::vector<bool>> c)
{
    
    std::vector<bool> b;
    for(auto x:c)
        b.insert(b.end(),x.begin(),x.end());
    return std::move(b);
}


inline int getNumberOfCodeblocks(int nBits, int BG)
{
  //ref 38.212
  auto C = 0;
  auto L = 24;
  
  switch(BG)
  {
    case 1:
    {auto Kcb=8448;
      if(nBits<Kcb)
	C=1;
      else
	C = std::ceil((double)nBits/(double)(Kcb-L)); 
      break;
    }
    case 2:
    {
      auto Kcb=3840;
      if(nBits<Kcb)
	C=1;
      else
	
	C = std::ceil((double)nBits/(double)(Kcb-L));
      
      break;
    }
    default:
      std::cout << "Wrong Base graph selected: "<< BG <<std::endl;
      abort();
  }
  //     std::cout << "No.of CB: "<<C<<std::endl;
  //     std::cout << "inbits size:  "<<inBits.size()<<std::endl;
  
  
  return C;    
}


#endif
