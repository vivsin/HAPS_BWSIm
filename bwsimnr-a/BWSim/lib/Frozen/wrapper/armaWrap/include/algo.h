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

#ifndef ALGO_H
#define ALGO_H

#include <stdlib.h>


namespace wrapper{
  
  

  template <class T1, class T2, class unaryOP> void unary_Function(const T1 & i1st ,const T1 & i1end ,  T2 ost,unaryOP unaryObjectFunction)
 {
   
   T2 it3=ost;
   
  for(T1 it1=i1st;it1 != i1end;++it1,++it3)
  {
      *it3=unaryObjectFunction(*it1);
  }
 }
  
  template <class T1, class T2,class T3, class binaryOP> void binary_Function(T1 i1st,T1 i1end, T2 i2st, T3 ost,binaryOP binaryObjectFunction)
 {
   T2 it2=i2st;
   T3 it3=ost;
   
  for(T1 it1=i1st;it1 != i1end;++it1,++it2,++it3)
  {
      *it3=binaryObjectFunction(*it1,*it2);
  }
 }
 
 template <class T1, class T2,class T3, class binaryOP> void binary_Function_scalar(T1 i1st,T1 i1end, const T2 & ip, T3 ost,binaryOP binaryObjectFunction)
 {
   T3 it3=ost;
   
  for(T1 it1=i1st;it1 != i1end;++it1,++it3)
  {
      *it3=binaryObjectFunction(*it1,ip);
  }
 }
 
 template <class T2, class T1,class T3, class binaryOP> void scalar_binary_Function(const T1 & ip,T2 i2st,T2 i2end,  T3 ost,binaryOP binaryObjectFunction)
 {
   
   T3 it3=ost;
   
  for(T2 it2=i2st;it2 != i2end;++it2,++it3)
  {
      *it3=binaryObjectFunction(ip,*it2);
  }
 }
  
  
  
}

#endif // ALGO_H
