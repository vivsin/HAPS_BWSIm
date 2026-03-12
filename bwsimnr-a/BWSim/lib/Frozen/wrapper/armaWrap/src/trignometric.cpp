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

#include "../include/trignometric.h"

namespace wrapper
{
  double sinc_::operator()(double x)
  {
    if(x==0.0)
      return 1.0;
    double pix=pi*x;
    return(std::sin(pix)/(pix));
  }  
  
  double sinc(double x){sinc_ Sinc; return Sinc(x);}
  
  
   vec sin(const vec & x){
       vec op(x.size()); 
    sin_ sin;
    unary_Function(x.begin(),x.end(),op.begin(),sin);
    return op;
  }
  
  vec cos(const vec & x){
       vec op(x.size()); 
    cos_ cos;
    unary_Function(x.begin(),x.end(),op.begin(),cos);
    return op;
  }
  
  vec tan(const vec & x){
       vec op(x.size()); 
    tan_ tan;
    unary_Function(x.begin(),x.end(),op.begin(),tan);
    return op;
  }
  
  vec asin(const vec & x){
       vec op(x.size()); 
    asin_ asin;
    unary_Function(x.begin(),x.end(),op.begin(),asin);
    return op;
  }
  
  vec acos(const vec & x){
       vec op(x.size()); 
    acos_ acos;
    unary_Function(x.begin(),x.end(),op.begin(),acos);
    return op;
  }
  
  vec atan(const vec & x){
       vec op(x.size()); 
    atan_ atan;
    unary_Function(x.begin(),x.end(),op.begin(),atan);
    return op;
  }
  
  
  vec sinc(const vec & x){
    vec op(x.size());
    sinc_ sinc;
    unary_Function(x.begin(),x.end(),op.begin(),sinc);
    return op;
  }
  
  
    mat sin(const mat & x){
       mat op(x.rows(),x.cols()); 
    sin_ sin;
    unary_Function(x.begin(),x.end(),op.begin(),sin);
    return op;
  }
  
  mat cos(const mat & x){
       mat op(x.rows(),x.cols()); 
    cos_ cos;
    unary_Function(x.begin(),x.end(),op.begin(),cos);
    return op;
  }
  
  mat tan(const mat & x){
       mat op(x.rows(),x.cols()); 
    tan_ tan;
    unary_Function(x.begin(),x.end(),op.begin(),tan);
    return op;
  }
  
  mat asin(const mat & x){
       mat op(x.rows(),x.cols()); 
    asin_ asin;
    unary_Function(x.begin(),x.end(),op.begin(),asin);
    return op;
  }
  
  mat acos(const mat & x){
       mat op(x.rows(),x.cols()); 
    acos_ acos;
    unary_Function(x.begin(),x.end(),op.begin(),acos);
    return op;
  }
  
  mat atan(const mat & x){
       mat op(x.rows(),x.cols()); 
    atan_ atan;
    unary_Function(x.begin(),x.end(),op.begin(),atan);
    return op;
  }
  
  
  mat sinc(const mat & x){
    mat op(x.rows(),x.cols());
    sinc_ sinc;
    unary_Function(x.begin(),x.end(),op.begin(),sinc);
    return op;
  }
  
  
  double rad_to_deg(const double x){
   return(x*180/pi);  
  }  
  
  double deg_to_rad(const double x){
    return(x*pi/180);  
  } 
  
}