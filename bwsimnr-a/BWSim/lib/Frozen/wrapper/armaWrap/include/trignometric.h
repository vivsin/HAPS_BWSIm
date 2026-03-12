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

#ifndef TRIGNOMETRIC_H
#define TRIGNOMETRIC_H

#include "mat.h"
namespace wrapper
{
  
  struct sinc_{
  double operator()(double x);
  };
  
  struct sin_{
  double operator()(double x)
    {
      return(std::sin(x));
    }
  };
  struct cos_{
  double operator()(double x)
    {
      return(std::cos(x));
    }
  };
  
  struct tan_{
  double operator()(double x)
    {
      return(std::tan(x));
    }
  };
  
  struct asin_{
  double operator()(double x)
    {
      return(std::asin(x));
    }
  };
  
  struct acos_{
  double operator()(double x)
    {
      return(std::acos(x));
    }
  };
  struct atan_{
  double operator()(double x)
    {
      return(std::atan(x));
    }
  };
  
  
 double sinc(double x);
 
 
 
  vec sin(const vec & x);
  
  vec cos(const vec & x);
  
  vec tan(const vec & x);
  
  vec asin(const vec & x);
  
  vec acos(const vec & x);
  vec atan(const vec & x);
  
  
  vec sinc(const vec & x);
  
  
    mat sin(const mat & x);
  
  mat cos(const mat & x);
  
  mat tan(const mat & x);
  
  mat asin(const mat & x);
  
  mat acos(const mat & x);
  
  mat atan(const mat & x);
  
  
  mat sinc(const mat & x);
  
  
   double rad_to_deg(const double x);
  
  double deg_to_rad(const double x);
  
  
  
}


#endif // TRIGNOMETRIC_H
