/*************************************************************************
*
* CEWiT CONFIDENTIAL
* __________________
*
* All Rights Reserved © 2014 CEWiT, India
*
\ NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
* and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
* Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
* express, printed and signed license for use is strictly forbidden.
*/

#include "../include/hadamard.h"

namespace wrapper{

  dMat hadamard_::operator()(const int n){
    if(n!=pow(2,ceil(log2(double(n)))) ){std::cout<<" input to hadamard_ not power of 2"<<endl;}
    
    dMat h(n,n);
    
    h(0,0)=1;
    
    if(n==1)return h;
    
    for(int i=2;i<=n;i<<=1)
    {
      int st=i>>1;
      for(int j=st;j<i;j++)
      {
	for(int k=0;k<st;k++)
	{
	  h(k,j)=h(k,j-st);
	  h(j,k)=h(j-st,k);
	  h(k+st,j)=-h(k,j-st);
	}
      }
      
    }
    
    return h;
  }
  
  dMat hadamard(const int n)
  {
    hadamard_ hadamardF;
    return hadamardF(n);
  }
}


