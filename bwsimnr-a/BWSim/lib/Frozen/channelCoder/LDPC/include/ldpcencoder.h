#ifndef LDPCENCODER_H
#define LDPCENCODER_H
#include "ldpccommon.h"
#include "ldpcnodes_b.h"

inline std::vector<std::vector<LdpcLift>> getCode_b(int Z, int BG)
{
//   if(BG!=1)
//     {
//   //*std::cout<<" this case is not handled "<<std::endl;
//       exit(10);
//     }

  PCM pcm;
  auto & p=pcm.getPcm(Z,BG);
  auto & bg=pcm.getBg(BG);
  auto & bgp=pcm.getBgp(BG);

    //*std::cout<<std::endl;
      //*std::cout<<std::endl;
       //*std::cout<<std::endl;
//   for(auto &x:p)
//     {
//     for(auto & y:x)
//       {
//           //*std::cout<<y<<",";
//       }
// 
// 
// 
//                      //*std::cout<<std::endl;
//   }

    //*std::cout<<std::endl;
     //*std::cout<<std::endl;
//   for(auto &x:bg)
//   {
//   for(auto & y:x)
//     {
//         //*std::cout<<y<<",";
//     }
// 
// 
// 
//                    //*std::cout<<std::endl;
//   }

//   for(auto &x:bgp)
//   {
//   for(auto & y:x)
//     {
//         //*std::cout<<y<<",";
//     }
// 
// 
// 
//                    //*std::cout<<std::endl;
//   }
// 


  int row=0;
  std::vector<std::vector<LdpcLift>>codeLP;
  for(auto &r:bgp)
    {
      std::vector<LdpcLift> lprow;
      for(auto &cp:r)
        {

            //*std::cout<<Z<<" "<<cp<<" "<<p[row][cp]<<std::endl;
            LdpcParameters lp{(int16_t)Z,(int16_t)cp,(int16_t)p[row][cp]};

            if(p[row][cp]<0)
                std::cout<<"there is some error in pij "<<p[row][cp]<<std::endl;
          LdpcLift ll(lp);
          lprow.push_back(ll);
           // auto v=getVectorforZ_Bgp_Pij(lp);
            for (auto y:ll.lpv)
                {
                      //*std::cout<<y<<",";
                  }
                  //*std::cout<<std::endl;
        }
      codeLP.push_back(lprow);
  ++row;
    }

  for(auto r:codeLP)
    {
      for(auto ll:r)
        {
            //*std::cout<<ll.lp;

        }
        //*std::cout<<std::endl;
    }
  return codeLP;
}



struct LdpcH_b{
  std::vector<std::vector<LdpcLift>> hMatpcm;
  std::vector<CheckLNode_b> cnodes;
  std::vector<VarLNode_b>vnodes;
  bool codeset=false;
  int n;
  int k;
  int z;
  int bg;
 void connect(LNode_b* x, LNode_b* y)
  {
    x->connectLNode(y);
    y->connectLNode(x);
    //   //*std::cout<<(*x)<<std::endl<<(*y)<<std::endl;
  }

void populateLNodes()
{
  if(codeset==true) return;
  codeset=true;
  vnodes.resize(0);
  cnodes.resize(0);
  vnodes.resize(n);
  cnodes.resize(n-k);
  {
    int i=0;
  for(auto &x:vnodes)
    {
      x.nodeid=i++;
      x.nodetype=1;
    }
  }
  {
    int i=0;
  for(auto &x:cnodes)
    {
      x.nodeid=i++;
      x.nodetype=0;
    }
  }
  int brow=0;
  int col=0;
  for(auto & br:hMatpcm)
    {

        for(auto &bc:br)
          {auto row=brow;
              for(auto vn:bc.lpv)
                {
                      connect(&cnodes[row++],&vnodes[vn]);
                }


          }
        brow+=z;
    }
}

LdpcH_b()=default;
LdpcH_b(int16_t Z,int BG):z(Z),bg(BG){
  if(BG==1)
    {
      n=z*68;//basegraph1
      k=z*22;
    }
  if(BG==2)
    {
      n=z*52;//basegraph1
      k=z*10;
    }
      hMatpcm=getCode_b(z,bg);
      populateLNodes();

  }


void encodeKernel()
{
  if(bg==1)
  {
   //base graph 1 QC version of PCM
   // std::vector<bool>firstencodedbit[z];
    for(int i=0;i<z;i++)
    { 
        bool encodedparity=cnodes[i].parityCheck_exceptLastandSecondLastBit();
        
//         std::cout<<"i="<<i<<" cnode "<<cnodes[i]<<"\nencodedparity"<<encodedparity<<std::endl;
        encodedparity^=cnodes[z+i].parityCheck_exceptLastandSecondandThirdLastBit();
//             std::cout<<"z+i="<<i<<" cnode "<<cnodes[z+i]<<"\nencodedparity"<<encodedparity<<std::endl;
        encodedparity^=cnodes[2*z+i].parityCheck_exceptLastandSecondLastBit();
//             std::cout<<"2z+i="<<2*z+i<<" cnode "<<cnodes[2*z+i]<<"\nencodedparity"<<encodedparity<<std::endl;
        encodedparity^=cnodes[3*z+i].parityCheck_exceptLastandSecondLastBit();
//             std::cout<<"3z+i="<<3*z+i<<" cnode "<<cnodes[3*z+i]<<"\nencodedparity"<<encodedparity<<std::endl;
        cnodes[z+i].xorThirdLastBit( encodedparity);
//         std::cout<<cnodes[z+i];
    }
  }
  else if(bg ==2)
  {
    for(int i=0;i<z;i++)
    {
      bool encodedparity=cnodes[i].parityCheck_exceptLastandSecondLastBit();
        
//         std::cout<<"i="<<i<<" cnode "<<cnodes[i]<<"\nencodedparity"<<encodedparity<<std::endl;
        encodedparity^=cnodes[z+i].parityCheck_exceptLastandSecondLastBit();
//             std::cout<<"z+i="<<i<<" cnode "<<cnodes[z+i]<<"\nencodedparity"<<encodedparity<<std::endl;
        encodedparity^=cnodes[2*z+i].parityCheck_exceptLastandSecondandThirdLastBit();
//             std::cout<<"2z+i="<<2*z+i<<" cnode "<<cnodes[2*z+i]<<"\nencodedparity"<<encodedparity<<std::endl;
        encodedparity^=cnodes[3*z+i].parityCheck_exceptLastandSecondLastBit();
//             std::cout<<"3z+i="<<3*z+i<<" cnode "<<cnodes[3*z+i]<<"\nencodedparity"<<encodedparity<<std::endl;
        cnodes[2*z+i].xorThirdLastBit( encodedparity);
//         std::cout<<cnodes[z+i];
  
    }   
  }
}
void encodeLastBit()
{

  for(auto &cn:cnodes)
    {
      bool parity=cn.parityCheck_exceptLastBit();
      cn.encodeLastBit(parity);
        //*std::cout<<cn<<std::endl;
    }
}

std::vector<bool> encode()
{
    encodeKernel();
//             std::cout<<"BUg1"<<std::endl;
    encodeLastBit();
//         std::cout<<"BUg1"<<std::endl;

    std::vector<bool> outputBits;
    outputBits.reserve(n);
//         std::cout<<"BUg1"<<std::endl;

    for(auto &x:vnodes)
      {
        outputBits.push_back(x.getValue());
      }
//               std::cout<<"BUg1"<<std::endl;

    return outputBits;
}

//void decode_ms()
//{
//    for(auto &vn:vnodes)
//      vn.process();
//    for(auto &cn :cnodes)
//      cn.process();
//}
//bool checkCorrect()
//{
// bool correctnessOfAllChecknodes(true);

//  for(auto &cn :cnodes)
//      {
//        //*std::cout<<cn.parityCheck()<<" ";
//        correctnessOfAllChecknodes&=cn.parityCheck();
//      }
//  //*std::cout<<" cor:"<<correctnessOfAllChecknodes<<"  ";
//return(correctnessOfAllChecknodes);
//}
//void decode_n(int iteration)
//{
//  for(int i=0;i<iteration;++i)
//    {
//      decode_ms();
//    }
//}
//std::vector<datab_t> getSoftDecodedData()
//{
//  std::vector<datab_t> a;
//  a.reserve(n);
//  for(auto &vn:vnodes)
//    {
//      auto x=vn.totVal();
//      a.push_back(x);
//    }
//  return a;
//}

//std::vector<bool> decode(int iterations=8)//n is iterations
//{
//  decode_n(iterations);
//  auto data=getSoftDecodedData();
//  std::vector<bool>decodedData; decodedData.reserve(n);
//  for(auto &x:data)
//    {
//      (x>=0)?decodedData.push_back(0):decodedData.push_back(1);
//    }
//  return decodedData;
//}

};

struct CodeStore_b{
    std::list< LdpcH_b *> z_bg_hmatList_b;
    
    
    std::map<long int , decltype(z_bg_hmatList_b.begin())  > z_bg_hmatMap_b;
    
    inline void ldpchmatclean_b()
    {
        for(auto &x :z_bg_hmatList_b)
        {
            delete x;
        }
    }
    inline LdpcH_b * setVectorforZ_BG_b(LdpcCodeParam lcp)
    {
        LdpcH_b *hmat=new LdpcH_b(lcp.z,lcp.bg);
        //LdpcH &hmat=*hmatp;
        LdpcCodeParamKey lcpk(lcp);
        //*std::cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<std::endl;
        //*std::cout<<"cnode loc"<<&hmat->cnodes<<std::endl;
        //*std::cout<<"vnode loc"<<&hmat->vnodes<<std::endl;
        
        //*std::cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<std::endl;
        
        z_bg_hmatList_b.push_front(hmat);
        //z_bg_hmatList.begin()->populateLNodes();
        z_bg_hmatMap_b[lcpk.key]=z_bg_hmatList_b.begin();
        return (*z_bg_hmatList_b.begin());
        
        
    }
    
    
    
    
    inline LdpcH_b* getCodeZ_BG_b(const LdpcCodeParam lcp)
    {
        LdpcCodeParamKey lcpk(lcp);
        
        auto it=z_bg_hmatMap_b.find(lcpk.key);
        if(it!=z_bg_hmatMap_b.end())
        {
            return *(it->second);
        }
        else
        {
            return setVectorforZ_BG_b(lcp);
        }
    }
    inline LdpcH_b * getCodeZ_BG_b(int16_t Z,int BG)
    {
        return getCodeZ_BG_b(LdpcCodeParam{(int16_t)Z,(int16_t)BG});
    }
};




class LDPCencoder
{
    LdpcH_b & ldpch;
    std::vector<bool>inbits;
public:
  LDPCencoder();
  LDPCencoder(CodeStore_b &cs,int16_t Z,int BG):ldpch(*cs.getCodeZ_BG_b(Z,BG)){}
  LDPCencoder(CodeStore_b &cs,std::vector<bool> inBits,int16_t Z,int BG):ldpch(*cs.getCodeZ_BG_b(Z,BG)){
      setInputBits(inBits);
       //*std::cout<<"cnode loc"<<&ldpch.cnodes<<std::endl;
       //*std::cout<<"vnode loc"<<&ldpch.vnodes<<std::endl;
    }
   void setInputBits(const std::vector<bool>inBits)
   {
       inbits=inBits;
       if(inbits.size() != ldpch.k)
       {  
           std::cout<<"channel coder expecting k="<<ldpch.k<<"but input size="<<inbits.size()<<"  aborting"<<std::endl;exit(5);
       }
       populateInBits();
   }

    void populateInBits()
    {
      for(auto &x:ldpch.vnodes)
        {
          x.setValue(0);
        }
      for(auto &x:ldpch.cnodes)//this is redundant
        {
          x.setValue(0);
        }
      for(int i=0;i<ldpch.k;i++)
        {
          ldpch.vnodes[i].setValue(inbits[i]);
        }
    }

    std::vector<bool> encode(const std::vector<bool>inBits)
    { setInputBits(inBits);
      return ldpch.encode();
    }
};

#endif // LDPCENCODER_H
