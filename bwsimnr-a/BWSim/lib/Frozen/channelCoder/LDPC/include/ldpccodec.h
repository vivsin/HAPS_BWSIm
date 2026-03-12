#ifndef LDPCCODEC_H
#define LDPCCODEC_H
#include "ldpccommon.h"
#include "ldpcnodes.h"
#include "NRCRC.h"
inline std::vector<std::vector<LdpcLift>> getCode(int Z, int BG)
{

  PCM pcm;
  auto & p=pcm.getPcm(Z,BG);
  auto & bg=pcm.getBg(BG);
  auto & bgp=pcm.getBgp(BG);
  
  int row=0;
  std::vector<std::vector<LdpcLift>>codeLP;
  for(auto &r:bgp)
  {
    std::vector<LdpcLift> lprow;
    for(auto &cp:r)
    {
      LdpcParameters lp{(int16_t)Z,(int16_t)cp,(int16_t)p[row][cp]};
      
      if(p[row][cp]<0)
	std::cout<<"there is some error in pij "<<p[row][cp]<<std::endl;
      LdpcLift ll(lp);
      lprow.push_back(ll);
      
    }
    codeLP.push_back(lprow);
    ++row;
  }
  
  return codeLP;
}

struct LdpcH{
  std::vector<std::vector<LdpcLift>> hMatpcm;
  std::vector<CheckLNode<data_t>> cnodes;
  std::vector<VarLNode<data_t>>vnodes;
  bool codeset=false;
  int n;
  int k;
  int z;
  int bg;
  void connect(LNode<data_t>* x, LNode<data_t>* y)
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
    for(auto &cn :cnodes)
    {
      cn.storeTransmittedValueRefs();
      cn.storeReceivedValueRefs();
    }
    for(auto &vn :vnodes)
    {
      vn.storeTransmittedValueRefs();
      vn.storeReceivedValueRefs();
    }
  }
  
  LdpcH()=default;
  LdpcH(int16_t Z,int BG):z(Z),bg(BG){
    if(BG==1)
    {
      n=z*68;//basegraph1
      k=z*22;
    }
    if(BG==2)
    {
      n=z*52;//basegraph2
      k=z*10;
    }
    hMatpcm=getCode(z,bg);
    populateLNodes(); 
    
  }
  
  
  void decode_ms()
  {
    for(auto &vn:vnodes)
      vn.process();
    for(auto &cn :cnodes)
      cn.process();
  }
  bool checkCorrect()
  {
    bool correctnessOfAllChecknodes(true);
    
    for(auto &cn :cnodes)
    {
      //*std::cout<<cn.parityCheck()<<" ";
      correctnessOfAllChecknodes&=cn.parityCheck();
    }
    //*std::cout<<" cor:"<<correctnessOfAllChecknodes<<"  ";
    return(correctnessOfAllChecknodes);
  }
  void decode_n(int iteration)
  {
    bool crcCheck;
    NrCRC nrCRC;
    nrCRC.setCRCTypeNR(CRC24B);  // Should be careful while selecting polynomial. Should use the same polynomial used by channelcoder
    for(int i=0;i<iteration;++i)
    {
      decode_ms();  
      if(i%4==0)
      {
	auto data=getSoftDecodedData();
	std::vector<bool>decodedData; decodedData.reserve(k);
	for(int i=0;i<k;i++)
	{
	  (data[i]>=0)?decodedData.push_back(0):decodedData.push_back(1);
	}
	crcCheck = nrCRC.performCRCCheckNR(decodedData);
	if(crcCheck) 
	{
// 	  	  std::cout <<"itr: "<<i<<"  CRC Success" <<std::endl;
	  break;
	}
      }
    }
  }
  std::vector<data_t> getSoftDecodedData()
  {
    std::vector<data_t> a;
    a.reserve(n);
    for(auto &vn:vnodes)
    {
      auto x=vn.totVal();
      a.push_back(x);
    }
    return a;
  }
  
  std::vector<bool> decode(int iterations)//n is iterations
  {
    decode_n(iterations);
    auto data=getSoftDecodedData();
    
    
    std::vector<bool>decodedData; decodedData.reserve(n);
    for(auto &x:data)
    {
      (x>=0)?decodedData.push_back(0):decodedData.push_back(1);
    }
    return decodedData;
  }
  
};

struct CodeStore{
  std::list< LdpcH *> z_bg_hmatList;
  
  std::map<long int , decltype(z_bg_hmatList.begin())  > z_bg_hmatMap;
  inline void ldpchmatclean()
  {
    for(auto &x :z_bg_hmatList)
    {
      delete x;
    }
  }
  
  inline LdpcH * setVectorforZ_BG(LdpcCodeParam lcp)
  {
    LdpcH *hmat=new LdpcH(lcp.z,lcp.bg);
    //LdpcH &hmat=*hmatp;
    LdpcCodeParamKey lcpk(lcp);
    //*std::cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<std::endl;
    //*std::cout<<"cnode loc"<<&hmat->cnodes<<std::endl;
    //*std::cout<<"vnode loc"<<&hmat->vnodes<<std::endl;
    
    //*std::cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<std::endl;
    
    z_bg_hmatList.push_front(hmat);
    //z_bg_hmatList.begin()->populateLNodes();
    z_bg_hmatMap[lcpk.key]=z_bg_hmatList.begin();
    return (*z_bg_hmatList.begin());
    
    
  }
  
  
  
  
  inline LdpcH* getCodeZ_BG(const LdpcCodeParam lcp)
  {
    LdpcCodeParamKey lcpk(lcp);
    
    auto it=z_bg_hmatMap.find(lcpk.key);
    if(it!=z_bg_hmatMap.end())
    {
      return *(it->second);
    }
    else
    {
      return setVectorforZ_BG(lcp);
    }
  }
  inline LdpcH * getCodeZ_BG(int16_t Z,int BG)
  {
    return getCodeZ_BG(LdpcCodeParam{(int16_t)Z,(int16_t)BG});
  }
};
template <class T> class LDPCdecoder{
  LdpcH & ldpch;
  std::vector<T>rvalues;
  
  
public:
  LDPCdecoder(CodeStore &cs,int16_t Z,int BG):ldpch(*cs.getCodeZ_BG(Z,BG)){}
  LDPCdecoder(CodeStore &cs,std::vector<T> rv,int16_t Z,int BG):ldpch(*cs.getCodeZ_BG(Z,BG)),rvalues(rv){

    populateRvalues();
  }
  void setReceivedValues(const std::vector<T>rval)
  {
    rvalues=rval;
    populateRvalues();
  }
  
  void populateRvalues()
  {
    if(rvalues.size() != ldpch.n)
    {
      std::cout<<"channel coder expecting n="<<ldpch.n<<"but rvalues size="<<rvalues.size()<<"  aborting"<<std::endl;
      exit(5);
    }
    for(int i=0;i<ldpch.n;i++)
    {
      ldpch.vnodes[i].setValue(rvalues[i]);
    }
  }
  
  std::vector<bool> decode(int n)
  {
    return ldpch.decode(n);
  }
  std::vector<T> getSoftdecodedData(int n)
  {
    return ldpch.getSoftDecodedData();
  }
  
};


#endif // LDPCCODEC_H
