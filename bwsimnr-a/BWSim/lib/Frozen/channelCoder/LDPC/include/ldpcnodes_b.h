#ifndef LDPCNODES_B_H
#define LDPCNODES_B_H


#include<fstream>
#include<vector>
#include<map>
#include<iostream>
#include<cmath>
#include <cstdlib>

typedef bool datab_t;


 struct  LNode_b{
public:
  int nodeid;
  int nodetype;
  int degree;//no of connected nodes "degree"
  bool value;//in var node bits input or encoded in check node paritycheck value

  std::vector<LNode_b*>connectedLNodes;
    void connectLNode(LNode_b* x)
  {
    connectedLNodes.push_back(x);
    degree++;
  }

  virtual bool getValue(){return value;}
  virtual void setValue(bool t){value=t;}

  LNode_b()=default;
 LNode_b(int nid,int ntype):nodeid(nid),nodetype(ntype){}
};

inline std::ostream & operator<<(std::ostream& a, LNode_b b)
{
  a<< " nodeid="<<b.nodeid<<" type-"<<b.nodetype<<" degree="<<b.degree<<" connected nodes ..\t";
  for(auto x:b.connectedLNodes)
    {
      a<<" ("<<x->nodeid<<","<<x->nodetype<<") ";
    }


 // a<<"\t size"<<b.ext_received.size()<<"  "<<b.ext_transmitted.size()<<"   ";
  return a;
}

 struct  VarLNode_b:public  LNode_b{

  VarLNode_b()=default;

  VarLNode_b(int nid):
    LNode_b(nid,1){}

};


inline std::ostream & operator<<(std::ostream& a,VarLNode_b & b)
{
  a<< " nodeid="<<b.nodeid<<" type-"<<b.nodetype<<" value="<<b.value<<"  degree="<<b.degree<<"\n connected nodes ..\n";
  for(auto x:b.connectedLNodes)
    {
      a<<" ("<<x->nodeid<<","<<x->nodetype<<"  value ="<<x->value<<") \n";
    }


//  a<<"\t size"<<b.ext_received.size()<<"  "<<b.ext_transmitted.size()<<"   ";
  return a;
}





struct CheckLNode_b:public  LNode_b{
   CheckLNode_b(int nid):LNode_b(nid,0){}
   CheckLNode_b()=default;



   bool parityCheck()
   {
     if(LNode_b::degree>1)//0 nothing to do, also the case with 1
       {
         value=false;//initializing with false;

         for(auto &x:LNode_b::connectedLNodes)
           {
             value^=x->value;
           }
         return value;
       }
       return false;
   }
   bool parityCheck_exceptLastandSecondLastBit()
   {
     //this assuming nodes are degree morethan 2;
     if(LNode_b::degree>2)
       {
         this->value=0;
         auto rit=connectedLNodes.rbegin();//last value need to skipped;
         ++rit;
         ++rit;//advance by two nodes ; excluding last two nodes
         for(;rit!=connectedLNodes.rend();++rit)
           {
             this->LNode_b::value ^=
                 (*rit)->LNode_b::value;
           }
         return value;
       }
       return false;
   }
   bool parityCheck_exceptLastandSecondandThirdLastBit()
   {
     //this assuming nodes are degree morethan 2;
     if(LNode_b::degree>2)
       {
         this->value=0;
         auto rit=connectedLNodes.rbegin();//last value need to skipped;
         ++rit;
         ++rit;//advance by two nodes ; excluding last two nodes
         ++rit;//advance to third node
         for(;rit!=connectedLNodes.rend();++rit)
           {
             this->LNode_b::value ^=
                 (*rit)->LNode_b::value;
           }
         return value;
       }
       return false;
   }


   bool parityCheck_exceptLastBit()
   {
     if(LNode_b::degree>1)//0 nothing to do, also the case with 1
       {
          LNode_b::value=false;//initializing with false;
         auto rit=connectedLNodes.rbegin();//last value need to skipped;
         rit++;
         for(;rit!=connectedLNodes.rend();++rit)
           {
              LNode_b::value^=(*rit)->value;
           }
         return value;
       }
       return false;
   }
   void encodeLastBit(bool val)
   {
     auto x=connectedLNodes.rbegin();
     (* x)->value=val;
     //for debugging
     if(parityCheck()!=0)
       {
           //*std::cout<<"there is error in encoding ; parity is not 0 "<<parityCheck()<<std::endl;
         std::abort();
       }

   }

   void xorSecondLastBit(bool val)
   {
     auto x=connectedLNodes.rbegin();
    ++x;//advance to second last position
     (*x)->value^=val;
   }
   void xorThirdLastBit(bool val)
   {
     auto x=connectedLNodes.rbegin();
     ++x;//advance to second last position
     ++x;//advance to third last position
     (*x)->value^=val;
   }
   void process(){
     //  //*std::cout<<"check node before processing "<<*this<<std::endl;//printing for debug
     bool v=parityCheck_exceptLastBit();
     encodeLastBit(v);
     //   //*std::cout<<"check node after processing "<<*this<<std::endl;//printing for debug
   }



};

inline std::ostream & operator<<(std::ostream& a,CheckLNode_b & b)
{
   a<< " nodeid="<<b.nodeid<<" type-"<<b.nodetype<<" value="<<b.value<<"  degree="<<b.degree<<"\n connected nodes ..\n";
   for(auto x:b.connectedLNodes)
     {
       a<<" ("<<x->nodeid<<","<<x->nodetype<<"  value ="<<x->value<<") \n";
     }

   return a;
}




#endif // LDPCNODES_B_H
