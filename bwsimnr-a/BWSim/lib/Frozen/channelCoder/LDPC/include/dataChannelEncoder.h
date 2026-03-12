#ifndef DATACHANNELENCODER_H
#define DATACHANNELENCODER_H

// #include"ldpccodec.h"
// #include"ldpcencoder.h"
#include "nrldpccodec.h"
#include "NRSegmentConcat.h"
#include "NRRateMatching.h"
#include "NRDerateMatching.h"
#include "NRBitInterleaving.h"
#include "NRCRC.h"
inline void print(std::vector<bool> ip)
{
    for(auto x : ip)
    { 
        std::cout<< x<<" ";
    }
    std::cout<<"\b ]"<<std::endl;
}

inline std::vector<bool> dataChannelEncoder(std::vector<bool> inBits, int E, int Q, int rvid, int BG,CRCTYPE crcType)
{
    //ref 38.212
    std::vector<bool> g;
    auto C = getNumberOfCodeblocks(inBits.size(),BG);

//     std::cout << "No.of CB: "<<C<<std::endl;
//     std::cout << "inbits size:  "<<inBits.size()<<std::endl;

    int ipSizeForBG= (BG==1)?22:10; 
    g.reserve(ipSizeForBG*500*14*C);   // Reserving a approx max vlue for g. Formula: [(22 or 10)*(max z)*(1/lowest coderate)* No.ofCodeBlocks] is for lowest coderate, 384 is max z and C is no of codeblocks. g will  be resized to appropriate value at the end.

    auto c = segmentInputbitsWithoutCRC(inBits,C);
   
//     for(auto x : c)
//     { 
//         std::cout<<"Segment size: "<< x.size()<<std::endl;
// //         std::cout<<"Segmented [ " ;
// //         print(x);
//     }
    
    
    NrCRC nrCRC;
    int K,Kb,z;
    NRLDPCcodec nrldpccodec;
    
    int RE = E;
    auto rem = (RE)%C;
    int RMsize[C];   // For storing the size of RateMatching output for each codeblock
    for(auto i =0; i<C; i++)
    {
        if(i<rem)
            RMsize[i] = ((RE/C)+1)*Q;
        else
            RMsize[i] = (RE/C)*Q;  // Integer division resuts the floored integer
    }
    
    
    int c_cnt=0;  // Codeblock count
    
    
    for(auto &cr:c)
    {
        nrCRC.setCRCTypeNR(crcType); 
        std::vector<bool> cCRC;
      if(C>1)
        cCRC = nrCRC.performCRCEncodingNR(cr);
      else
        cCRC = cr;
      
//       std::cout<<"Segment with CRC size: "<< cCRC.size()<<std::endl;
//       std::cout<<"cCRC [ " ;
//       print(cCRC);
      
      nrldpccodec.findParamKz(BG,cCRC.size(),Kb,K,z);

      auto d = nrldpccodec.encode(cCRC,BG);
//       std::cout<<"Encoded size: "<<d.size()<<std::endl;
//       std::cout<<"Encoded [ " ;
//       print(d);
      
      auto e = nrRateMatching(d,RMsize[c_cnt],rvid,BG,z);
      c_cnt++;  
//       std::cout<<"RateMatched size: "<<e.size()<<std::endl;
//       std::cout<<"RateMatched [ " ;
//       print(e);  
    
      auto f = nrBitInterleaving(e,Q);
      
//       std::cout<<"BitInterleaved size: "<< f.size()<< std::endl;
//       std::cout<<"BitInterleaved [ " ;
//       print(f);
      
      if(g.size()>ipSizeForBG*500*14*C)
      {
	std::cout <<"Try providing bigger size for g...not a big problem. This makes to allocate memory dynamically"<<std::endl;
// 	abort();
      }
      g.insert(g.end(),f.begin(),f.end());
    }
//       std::cout<<"Output size: "<<g.size()<<std::endl;
//     std::cout<<"Output [ " ;
//     print(g);
    c_cnt=0;
     g.resize(g.size());
    return g;    
}

inline std::vector<bool> dataChannelEncoder(std::vector<bool> inBits, int E, int Q, int rvid, int BG, std::vector<std::vector<bool>> &dV, std::vector<int> &zPerCodeBlock)
{
    //ref 38.212
    std::vector<bool> g;
    auto C = getNumberOfCodeblocks(inBits.size(),BG);

//     std::cout << "No.of CB: "<<C<<std::endl;
//     std::cout << "inbits size:  "<<inBits.size()<<std::endl;

    int ipSizeForBG= (BG==1)?22:10; 
    g.reserve(ipSizeForBG*500*14*C);   // Reserving a approx max vlue for g. Formula: [(22 or 10)*(max z)*(1/lowest coderate)* No.ofCodeBlocks] is for lowest coderate, 384 is max z and C is no of codeblocks. g will  be resized to appropriate value at the end.
    dV.reserve(C);
    zPerCodeBlock.reserve(C);
    auto c = segmentInputbitsWithoutCRC(inBits,C);
   
    NrCRC nrCRC;
    int K,Kb,z;
    NRLDPCcodec nrldpccodec;
    
    int RE = E;
    auto rem = (RE)%C;
    int RMsize[C];   // For storing the size of RateMatching output for each codeblock
    for(auto i =0; i<C; i++)
    {
        if(i<rem)
            RMsize[i] = ((RE/C)+1)*Q;
        else
            RMsize[i] = (RE/C)*Q;  // Integer division resuts the floored integer
    }
    
    
    int c_cnt=0;  // Codeblock count
    
    
    for(auto &cr:c)
    {
        nrCRC.setCRCTypeNR(CRC24B); 
        std::vector<bool> cCRC;
      if(C>1)
        cCRC = nrCRC.performCRCEncodingNR(cr);
      else
        cCRC = cr;
      
//       std::cout<<"Segment with CRC size: "<< cCRC.size()<<std::endl;
//       std::cout<<"cCRC [ " ;
//       print(cCRC);
      
      nrldpccodec.findParamKz(BG,cCRC.size(),Kb,K,z);
      zPerCodeBlock.push_back(z);

      auto d = nrldpccodec.encode(cCRC,BG);
      dV.push_back(d);
//       std::cout<<"Encoded size: "<<d.size()<<std::endl;
//       std::cout<<"Encoded [ " ;
//       print(d);
      
      auto e = nrRateMatching(d,RMsize[c_cnt],rvid,BG,z);
      c_cnt++;  
//       std::cout<<"RateMatched size: "<<e.size()<<std::endl;
//       std::cout<<"RateMatched [ " ;
//       print(e);  
    
      auto f = nrBitInterleaving(e,Q);
      
//       std::cout<<"BitInterleaved size: "<< f.size()<< std::endl;
//       std::cout<<"BitInterleaved [ " ;
//       print(f);
      
      if(g.size()>ipSizeForBG*500*14*C)
      {
	std::cout <<"Try providing bigger size for g...not a big problem. This makes to allocate memory dynamically"<<std::endl;
// 	abort();
      }
      g.insert(g.end(),f.begin(),f.end());
    }
//       std::cout<<"Output size: "<<g.size()<<std::endl;
//     std::cout<<"Output [ " ;
//     print(g);
    c_cnt=0;
     g.resize(g.size());
return g;    
}


inline std::vector<bool> dataChannelEncoderRetransmit( std::vector<std::vector<bool>> &dV, int E, int Q, int rvid, int BG, std::vector<int> zPerCodeBlock)
{   
    
     std::vector<bool> g;
     auto C = dV.size();
     int ipSizeForBG= (BG==1)?22:10; 
    g.reserve(ipSizeForBG*500*14*C);   // Reserving a approx max vlue for g. Formula: [(22 or 10)*(max z)*(1/lowest coderate)* No.ofCodeBlocks] is for lowest coderate, 384 is max z and C is no of codeblocks. g will  be resized to appropriate value at the end.

     int RE = E;
     auto rem = RE%C;
    int RMsize[C],z;   // For storing the size of RateMatching output for each codeblock
    for(auto i =0; i<C; i++)
    {
        if(i<rem)
           RMsize[i] = ((RE/C)+1)*Q;
        else
            RMsize[i] = (RE/C)*Q;  // Integer division resuts the floored integer
    }
    auto c_cnt =0;
    for(auto &d:dV)
    {
        z = zPerCodeBlock[c_cnt];
	if(z>384 || z<2)
	{
	  std::cout <<"Wrong Z stored... aborting!!"<<std::endl;
	  abort();
	}
        auto e = nrRateMatching(d,RMsize[c_cnt],rvid,BG,z);
      c_cnt++;  
//       std::cout<<"RateMatched size: "<<e.size()<<std::endl;
//       std::cout<<"RateMatched [ " ;
//       print(e);  
    
      auto f = nrBitInterleaving(e,Q);
      
//       std::cout<<"BitInterleaved size: "<< f.size()<< std::endl;
//       std::cout<<"BitInterleaved [ " ;
//       print(f);
      
      if(g.size()>ipSizeForBG*500*14*C)
      {
	std::cout <<"Try providing bigger size for g...not a big problem. This makes to allocate memory dynamically"<<std::endl;
// 	abort();
      }
      g.insert(g.end(),f.begin(),f.end());
    }
    g.resize(g.size());
    return g;
}





#endif
