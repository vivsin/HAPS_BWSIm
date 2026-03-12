#ifndef LDPCNODES_H
#define LDPCNODES_H
#include<fstream>
#include<vector>
#include<map>
#include<iostream>
#include<cmath>

typedef double data_t;

template<class T> union UnionTI{
  T val;
  long int x;
  UnionTI(T v):val(v){};
  UnionTI()=default;
};

template<class T> struct LNode{
public:
  int nodeid = -1;
  int nodetype = -1;
  int degree = 0;//no of connected nodes "degree"
  
  std::vector<UnionTI<T>>ext_received;
  std::vector<UnionTI<T>>ext_transmitted;
  std::vector<bool>signbits;//used in check nodes only in process()
  std::vector<LNode<T> *>connectedLNodes;
  std::vector<data_t *> transmittedValueRefs;
  std::vector<data_t *> receivedValueRefs;
  std::map<LNode<T>*,int>nodeIndexMap;
  void connectLNode(LNode<T>* x)
  {
    connectedLNodes.push_back(x);
    nodeIndexMap[x]=degree++;
    ext_received.resize(degree);
    ext_transmitted.resize(degree);
  }
  
  virtual T getValue(LNode<T>* np){
    std::cout<<"base class getValue this shall not be called"<<std::endl;exit(5);
  }
  virtual T* getTxValueRef(LNode<T>* np)
  {
    std::cout<<"base class getValue this shall not be called"<<std::endl;exit(5);
  }
  virtual void setValue(LNode<T>* np,T t){
    std::cout<<"base class setValue this shall not be called"<<std::endl;exit(5);
  }
  virtual T* getRxValueRef(LNode<T>* np)
  {
    std::cout<<"base class getValue this shall not be called"<<std::endl;exit(5);
  }
  virtual T totVal(){
    std::cout<<"base class totval this shall not be called"<<std::endl;
    exit(5);
  }
  int getLNodeIndex(LNode <T>* np)
  {
    auto it=nodeIndexMap.find(np);
    if(it != nodeIndexMap.end())
    {
      return it->second;
    }
    else
    {
      
      //*std::cout<<"there is error in linking node with index in var node aborting.. "<<nodeid<<"  with cnode * "<<np<<std::endl;
      //*std::cout<<std::endl<<this<<" "<<*this<<std::endl<<np<<"  "<<*np<<std::endl;
      exit(100);
    }
    
  }
  
  void storeTransmittedValueRefs()
  {
    LNode<T>::transmittedValueRefs.reserve(LNode<T>::degree);
    for(int i=0;i<LNode<T>::degree;++i)
    {
      LNode<T>::transmittedValueRefs.push_back(LNode<T>::connectedLNodes[i]->getTxValueRef(this));
      //       LNode<T>::transmittedValueRefs[i]=LNode<T>::connectedLNodes[i]->getTxValueRef(this);
    }
  }
  
  void storeReceivedValueRefs()
  {
    LNode<T>::receivedValueRefs.reserve(LNode<T>::degree);
    for(int i=0;i<LNode<T>::degree;++i)
    {
      LNode<T>::receivedValueRefs.push_back(LNode<T>::connectedLNodes[i]->getRxValueRef(this));
      //       LNode<T>::transmittedValueRefs[i]=LNode<T>::connectedLNodes[i]->getTxValueRef(this);
    }
  }
  
  LNode(){
    ext_received.reserve(16);
    ext_transmitted.reserve(16);
    signbits.reserve(64);
    transmittedValueRefs.resize(0);
    receivedValueRefs.resize(0);
  }
  
};

template<class T>std::ostream & operator<<(std::ostream& a, LNode<T>b)
{
  a<< " nodeid="<<b.nodeid<<" type-"<<b.nodetype<<" degree="<<b.degree<<" connected nodes ..\t";
  for(auto x:b.connectedLNodes)
  {
    a<<" ("<<x->nodeid<<","<<x->nodetype<<") ";
  }
  for(auto it=b.nodeIndexMap.begin();it!=b.nodeIndexMap.end();++it)
  {
    a<<" ["<<it->first<<" - "<<it->second<<"] ";
  }
  
  a<<"\t size"<<b.ext_received.size()<<"  "<<b.ext_transmitted.size()<<"   ";
  return a;
}

template<class T> struct  VarLNode:public LNode<T>{
  
  VarLNode()=default;
  T rvalue=0;//r soft value(llr)
  
  T totVal(){
    auto x=rvalue;
    for(auto er:LNode<T>::ext_received)
    {
      x+=er.val;
    }
    return x;
  }
  
  void process()
  {
    auto && tval=totVal();
    for(int i=0;i<LNode<T>::degree;i++)
    {
      //         LNode<T>:: ext_transmitted[i].val=tval-LNode<T>::ext_received[i].val;
      *LNode<T>::receivedValueRefs[i]=tval-LNode<T>::ext_received[i].val;
    }
    //*std::cout<<"\n Var LNode\n "<<*this<<std::endl;//printing for debug
  }
  T getValue(LNode<T>* np)
  {
    auto i=this->getLNodeIndex(np);
    return LNode<T>::ext_transmitted[i].val;
  }
  
  T* getTxValueRef(LNode<T>* np)
  {
    data_t * tmp;
    auto i=this->getLNodeIndex(np);
    //     std::cout<<"GET VALUE: \n";
    //     std::cout <<"i: "<<i<<"   np->nodeid: "<< np->nodeid<<std::endl;
    tmp = & LNode<T>::ext_transmitted[i].val;
    return tmp;
  }
  
  void setValue(LNode<T>* np,T t)
  {
    auto i=this->getLNodeIndex(np);
    LNode<T>::ext_received[i].val=t;
  }
  
  T* getRxValueRef(LNode<T>* np)
  {
    data_t * tmp;
    auto i=this->getLNodeIndex(np);
    //     std::cout<<"SET VALUE: \n";
    //     std::cout <<"i: "<<i<<"   np->nodeid: "<< np->nodeid<<std::endl;
    tmp = & LNode<T>::ext_received[i].val;
    return tmp;
  }
  
  VarLNode(int nid):LNode<T>::nodeid(nid),LNode<T>::nodetype(1){}
  void setValue(T val){
    rvalue=val;
    std::fill_n(LNode<T>::ext_received.begin(),LNode<T>::degree,0);
    std::fill_n(LNode<T>::ext_transmitted.begin(),LNode<T>::degree,0);
  }
  
};


template<class T>std::ostream & operator<<(std::ostream& a,VarLNode<T> & b)
{
  a<< " nodeid="<<b.nodeid<<" type-"<<b.nodetype<<"rval="<<b.rvalue<<"  degree="<<b.degree<<"final value="<<b.totVal()<<"\n connected nodes ..\n";
  for(auto x:b.connectedLNodes)
  {
    a<<" ("<<x->nodeid<<","<<x->nodetype<<"  ext_rval ="<<b.ext_received[b.getLNodeIndex(&*x)].val<<"rvalC"<<x->ext_transmitted[x->getLNodeIndex(&b)].val<<"  ext_tval ="<<b.ext_transmitted[b.getLNodeIndex(&*x)].val<<"rvalC"<<x->ext_received[x->getLNodeIndex(&b)].val<<") \n";
  }
  a<<"/n";
  a<<"/n";
  for(auto it=b.nodeIndexMap.begin();it!=b.nodeIndexMap.end();++it)
  {
    a<<" ["<<it->first<<" - "<<it->second<<"] ";
  }
  
  a<<"\t size"<<b.ext_received.size()<<"  "<<b.ext_transmitted.size()<<"   ";
  return a;
}





template<class T> struct CheckLNode:public LNode<T>{
  
  T scaleValue=.65;
  UnionTI<T>signbit=0;
  CheckLNode(){};
  void receiveBelief()
  {
    for(int i=0;i<LNode<T>::degree;++i)
    {
      //         LNode<T>::ext_received[i].val=LNode<T>::connectedLNodes[i]->getValue(this);
      LNode<T>::ext_received[i].val = *LNode<T>::transmittedValueRefs[i]; 
    }
  }
  T getValue(LNode<T>* np){  std::cout<<"this shall not be called"<<std::endl;
    exit(5);}
    
    T* getTxValueRef(LNode<T>* np)
    {
      data_t * tmp;
      auto i=this->getLNodeIndex(np);
      //     std::cout<<"GET VALUE: \n";
      //     std::cout <<"i: "<<i<<"   np->nodeid: "<< np->nodeid<<std::endl;
      tmp = & LNode<T>::ext_transmitted[i].val;
      return tmp;
    }                            
    void propagateBelief()
    {
      for(int i=0;i<LNode<T>::degree;++i)
      {
	//         LNode<T>::connectedLNodes[i]->setValue(this, scaleValue*LNode<T>::ext_transmitted[i].val);
	*LNode<T>::receivedValueRefs[i] = scaleValue*LNode<T>::ext_transmitted[i].val;
      }
    }
    void setValue(LNode<T>* np,T t){  std::cout<<"setValue this shall not be called"<<std::endl;
      exit(5);}
      T* getRxValueRef(LNode<T>* np)
      {
	data_t * tmp;
	auto i=this->getLNodeIndex(np);
	//     std::cout<<"SET VALUE: \n";
	//     std::cout <<"i: "<<i<<"   np->nodeid: "<< np->nodeid<<std::endl;
	tmp = & LNode<T>::ext_received[i].val;
	return tmp;
      }  
      
      void minsum()
      {
	if(LNode<T>::degree>1)//0 nothing to do, also the case with 1
	{
	  signbit=LNode<T>::ext_received[0];
	  int mini=0;
	  T minV1=std::abs(signbit.val);
	  T minV2=std::abs(LNode<T>::ext_received[1].val);
	  
	  for(int i=1;i<LNode<T>::degree;++i)
	  {
	    UnionTI<T> s=LNode<T>::ext_received[i];
	    signbit.x^=s.x;
	    auto absSval=std::abs(s.val);
	    if(minV1>absSval)
	    {
	      minV2=minV1;
	      mini=i;
	      minV1=absSval;
	    }
	    else if (minV2>absSval)
	    {
	      minV2=absSval;
	    }
	  }
	  for(int i=0;i<LNode<T>::degree;++i)
	  {
	    UnionTI<T>s(signbit);s.x^=LNode<T>::ext_received[i].x;
	    //             LNode<T>::ext_transmitted[i].val=std::copysign(minV1,s.val);
	    *LNode<T>::receivedValueRefs[i] = scaleValue*std::copysign(minV1,s.val);
	  }
	  UnionTI<T>s(signbit);s.x^=LNode<T>::ext_received[mini].x;
	  //         LNode<T>::ext_transmitted[mini].val=std::copysign(minV2,s.val);
	  *LNode<T>::receivedValueRefs[mini] = scaleValue*std::copysign(minV2,s.val);
	}
      }
      
      void minsum_mod()
      { 
	bool sbit;
	if(LNode<T>::degree>1)//0 nothing to do, also the case with 1
	{
	  sbit=(LNode<T>::ext_received[0].val>=0)?0:1;
	  LNode<T>::signbits[0]=sbit;
	  int mini=0;
	  T minV1=std::abs(LNode<T>::ext_received[0].val);
	  T minV2=std::abs(LNode<T>::ext_received[1].val);
	  
	  for(int i=1;i<LNode<T>::degree;++i)
	  {
	    
	    LNode<T>::signbits[i]=(LNode<T>::ext_received[i].val>=0)?0:1;
	    sbit^=LNode<T>::signbits[i];
	    auto absSval=std::abs(LNode<T>::ext_received[i].val);
	    if(minV1>absSval)
	    {
	      minV2=minV1;
	      mini=i;
	      minV1=absSval;
	    }
	    else if (minV2>absSval)
	    {
	      minV2=absSval;
	    }
	  }
	  T smin[2]={minV1,-minV1};
	  T sminv2[2]={minV2,-minV2};
	  for(int i=0;i<LNode<T>::degree;++i) 
	  {
	    
	    
	    *LNode<T>::receivedValueRefs[i] = scaleValue*smin[ sbit^LNode<T>::signbits[i] ];
	  }
	  
	  *LNode<T>::receivedValueRefs[mini] = scaleValue*sminv2[ sbit^LNode<T>::signbits[mini]];
	}
      }
      
      bool parityCheck()
      {
	if(LNode<T>::degree>1)//0 nothing to do, also the case with 1
	{
	  signbit=1.0;//initializing with a +ve value;
	  
	  for(auto &x:LNode<T>::connectedLNodes)
	  {
	    signbit.x^=UnionTI<T>(x->totVal()).x;
	    //            UnionTI<T> s=x->totVal();
	    //            signbit.x^=s.x;
	  }
	  return signbit.val>=0;
	}
      }
      
      
      void process(){
	//*std::cout<<"\ncheck node before processing\n "<<*this<<std::endl;//printing for debug
	//     receiveBelief();
	minsum_mod();
	//     propagateBelief();
	//*std::cout<<"\ncheck node after processing\n "<<*this<<std::endl;//printing for debug
      }
      
      CheckLNode(int nid):LNode<T>::nodeid(nid),LNode<T>::nodetype(0){}
      
};

template<class T>std::ostream & operator<<(std::ostream& a,CheckLNode<T> & b)
{
  a<< " nodeid="<<b.nodeid<<" type-"<<b.nodetype<<"  degree="<<b.degree<<" \n connected nodes ..\n";
  for(auto x:b.connectedLNodes)
  {
    a<<" ("<<x->nodeid<<","<<x->nodetype<<"  ext_rval ="<<b.ext_received[b.getLNodeIndex(&*x)].val<<"rvalC"<<x->ext_transmitted[x->getLNodeIndex(&b)].val<<"  ext_tval ="<<b.ext_transmitted[b.getLNodeIndex(&*x)].val<<"rvalC"<<x->ext_received[x->getLNodeIndex(&b)].val<<") \n";
  }
  a<<"/n";
  a<<"/n";
  for(auto it=b.nodeIndexMap.begin();it!=b.nodeIndexMap.end();++it)
  {
    a<<" ["<<it->first<<" - "<<it->second<<"] ";
  }
  
  a<<"\t size"<<b.ext_received.size()<<"  "<<b.ext_transmitted.size()<<"   ";
  return a;
}


class LDPCnodes
{
public:
  LDPCnodes();
};

#endif // LDPCNODES_H
