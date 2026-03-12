#ifndef DATACHANNELDECODER_H
#define DATACHANNELDECODER_H
// #include"ldpccodec.h"
// #include"ldpcencoder.h"
#include "nrldpccodec.h"
#include "NRSegmentConcat.h"
#include "NRRateMatching.h"
#include "NRDerateMatching.h"
#include "NRBitInterleaving.h"
#include "NRCRC.h"
using namespace std;
// Common printing function
template <class T> void printDe(std::vector<T> ip)
{
    for(auto x : ip)
    { 
        std::cout<< x<<" ";
    }
    std::cout<<"\b ]"<<std::endl;
}

template <class T> std::vector<bool> dataChannelDecoder(std::vector<T> input, std::vector<T> &Hip, int E, int B_, int Q, int rvid, int BG, bvec &isSuccesPerCB, CRCTYPE crcType=CRC24B)   
{
//     std::cout << "\n \n \n ****************Data Channel Decoder**************\n \n \n "<<std::endl;
    bool freshTx = false;
    if(Hip.size()==0)
        freshTx =true;
    //ref 38.212
    std::vector<bool> b;
    auto C = getNumberOfCodeblocks(B_,BG);
    
    int ipSizeForBG= (BG==1)?22:10; 
    if(freshTx)
    {
      Hip.reserve(ipSizeForBG*500*14*C);   // Reserving a approx max vlue for Hip so that no need to resize its size in case of fresh tx for every cobeblock count in the following code.
      std::vector<T> temp(ipSizeForBG*384*9*C,0);
      Hip.insert(Hip.end(),temp.begin(),temp.end());  // Filling Hip with zeros
      temp.clear();
      // 14 is for lowest coderate, 384 is max z and C is no of codeblocks. Hip will  be resized to appropriate value at the end.
    }
    
    std::vector<std::vector<bool>> cWithAllCBs;
    cWithAllCBs.reserve(C);
    isSuccesPerCB.set_length(C);
    
    
    int RE = E;
    auto rem = RE%C;
    int rem2[C];
    int RMsize[C];   // For storing the size of RateMatching output for each codeblock
    int RMsize2[C];
    for(auto i =0; i<C; i++)
    {
        if(i<rem)
        {
            RMsize[i] = ((RE/C)+1)*Q;
            RMsize2[i] = ((RE/C)+1)*Q;
        }
        else
        {
            RMsize[i] = (RE/C)*Q;  // Integer division resuts the floored integer
            RMsize2[i] = (RE/C)*Q;
        }
//        rem2[i] = RMsize[i] %Q;    // Taking care of resized zeros in BitInterleaving
////        if(rem2[i] != 0)
////            RMsize[i] += Q-rem2[i];
    }
    
    
    
    
    rem= B_%C;
    int K_[C];  
    for (int i = 0; i<C;i++)
    {
        if(i<rem)   // This is assuming B_ may not be multiple of C.
        {
          K_[i] = B_/C+1;  // //FIXME ceiling for LTE compatability
        }
        else
        {
          K_[i] = B_/C;
        }
    }
    
    
    int K=0,Kb=0,z=0;
    NRLDPCcodec nrldpccodec;
    int pos = 0,pos2 =0;
    NrCRC nrCRC;
    
    bool crcCheck=false;
    int HipSize=0;
    
    for(int c_cnt=0; c_cnt<C;c_cnt++)
    {
        std::vector<T> f(0);
        std::vector<T> Hip_seg(0);
        nrldpccodec.findParamKz(BG,K_[c_cnt],Kb,K,z);
        auto r = (B_)%C;
        int poffset=0;
	if(c_cnt<r)
	  poffset = (48-4*BG)*z+K_[c_cnt];
	else
	  poffset = (48-4*BG)*z+K_[c_cnt];
        
	
        //(82-16*BG)*z-rem2[c_cnt]
	f.reserve(RMsize[c_cnt]);
        f.insert(f.end(), input.begin()+pos,input.begin()+pos+RMsize[c_cnt]);
	
//       std::cout<<"BitInterleaved [ " ;
//       printDe(f);
        
        if(freshTx)
	{
//             Hip.resize(Hip.size()+poffset,0);
	      Hip.resize(Hip.size()+poffset,0);
	}
//         Hip_seg.insert(Hip_seg.end(),Hip.begin()+pos2,Hip.begin()+pos2+(82-(16*BG))*z);
        Hip_seg.reserve(poffset);

        Hip_seg.insert(Hip_seg.end(),Hip.begin()+pos2,Hip.begin()+pos2+poffset);
//         cout << "Receiver RateMatched (f) size: "<< f.size()<<endl;
        auto e = nrBitDeInterleaving(f,Q,rem2[c_cnt]);
//       std::cout<<"BitDeInterleaved [ " ;
//       printDe(e);
        

        nrDerateMatching(Hip_seg,e,rvid,BG,z);   // Output is written into Hip_seg
	
	
        auto hipit=Hip.begin()+pos2;
	HipSize = HipSize+poffset;
// 	std::cout<<"Hip Size: "<< HipSize<< "  ipSizeForBG*384*9*C: " <<ipSizeForBG*384*9*C <<std::endl;
	if(Hip.size()>ipSizeForBG*500*14*C)
	{
	  std::cout <<" aborting...Try providing bigger size for Hip...not a big problem. This makes to allocate memory dynamically"<<std::endl;
	  abort();
	}
        for(auto &x:Hip_seg)  // Writing Hip_seg data into Hip for retransmission use
        {
            (*hipit++)=x;
        }
        pos += RMsize[c_cnt];
        pos2 += poffset;
        
        
        
        auto cCRC = nrldpccodec.decode(Hip_seg, K_[c_cnt], BG);
	
        
//       std::cout<<"Decoded [ " ;
//       printDe(cCRC);
        nrCRC.setCRCTypeNR(crcType);
        if(C==1)
        {
            b= cCRC;
            isSuccesPerCB(c_cnt) = true;
//             std::cout<<"Output [ " ;
//             printDe(b);
            return b;
        }
        else
        {
            crcCheck = nrCRC.performCRCCheckNR(cCRC);
            if (crcCheck != true)
            {
                isSuccesPerCB(c_cnt) = false;
                cWithAllCBs.push_back(nrCRC.removeCRCNR(cCRC));
//                 std::cout << "CRC check failed for CB number:"<< c_cnt <<std::endl;
            }
            else
            {
                isSuccesPerCB(c_cnt) = true;
                cWithAllCBs.push_back(nrCRC.removeCRCNR(cCRC));
            }
        }
    }
    Hip.resize(Hip.size());
    b = desegmentInputbitsWithoutCRC(cWithAllCBs);
//     std::cout<<"Output [ " ;
//     printDe(b);

    
    return b;
    
    
}





#endif
