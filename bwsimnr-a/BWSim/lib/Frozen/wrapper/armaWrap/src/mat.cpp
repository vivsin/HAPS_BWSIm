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


#include "../include/mat.h"
namespace wrapper{
bMat concat_horizontal(const bMat &m1,const bMat &m2){bMat toReturn;toReturn.armaMat=join_rows(m1.armaMat,m2.armaMat);return(toReturn);}
bMat concat_vertical(const bMat &m1,const bMat &m2){bMat toReturn;toReturn.armaMat=join_cols(m1.armaMat,m2.armaMat);return(toReturn);}
bMat operator+(const bMat &m1, const bMat &m2){bMat toReturn=m1;toReturn+=m2;return(toReturn);}
bMat operator+(const bMat &m, char t){bMat toReturn=m;toReturn+=t;return(toReturn);}
bMat operator+(char t, const bMat &m){bMat toReturn=m;toReturn+=t;return(toReturn);}
bMat operator-(const bMat &m1, const bMat &m2){bMat toReturn=m1;toReturn-=m2;return(toReturn);}
bMat operator-(const bMat &m, char t){bMat toReturn=m;toReturn-=t;return(toReturn);}
bMat operator-(char t, const bMat &m){bMat toReturn=m;toReturn-=t;return(toReturn);}
bMat operator-(const bMat &m){bMat toReturn;toReturn.armaMat=-m.armaMat;return(toReturn);}
bMat operator*(const bMat &m1, const bMat &m2){bMat toReturn=m1;toReturn*=m2;return(toReturn);}
bVec operator*(const bVec &v, const bMat &m){
  
  if(v.length()!=m.rows()){cout<<"[both:] Dimension Mismatch in operator* of bMat..."<<endl;std::abort();}
  
  bVec toReturn(m.cols());
  toReturn.clear();
  for(int c=0;c<m.cols();c++)
    for(int r=0;r<m.rows();r++)
      toReturn(c)+=v(r)&m(r,c);
    
    return(toReturn);
}
bVec operator*(const bMat &m, const bVec &v){
  
  if(v.length()!=m.cols()){cout<<"[both:] Dimension Mismatch in operator* of bMat..."<<endl;std::abort();}
  
  bVec toReturn(m.rows());
  toReturn.clear();
  for(int c=0;c<m.cols();c++)
    for(int r=0;r<m.rows();r++)
      toReturn(r)+=v(c)&m(r,c);
    
    return(toReturn);
}
bMat operator*(const bMat &m, char t){bMat toReturn=m;toReturn*=t;return(toReturn);}
bMat operator*(char t, const bMat &m){bMat toReturn=m;toReturn*=t;return(toReturn);}
bMat elem_mult(const bMat &m1, const bMat &m2){bMat toReturn=m1;toReturn/=m2;return(toReturn);}
bMat operator/(const bMat &m, char t){bMat toReturn=m;toReturn/=t;return(toReturn);}
bMat operator/(char t, const bMat &m){bMat toReturn=m;toReturn/=t;return(toReturn);}
bMat elem_div(const bMat &m1, const bMat &m2){bMat toReturn=m1;toReturn/=m2;return(toReturn);}
std::ostream &operator<<(std::ostream &os, const bMat &m)
{
  
  int i;
  switch (m.rows()) {
    case 0 :
      os << "[]";
      break;
    case 1 :
      os << '[' << m.get_row(0) << ']';
      break;
    default:
      os << '[' << m.get_row(0) << endl;
      for (i = 1; i < m.rows() - 1; i++)
	os << ' ' << m.get_row(i) << endl;
      os << ' ' << m.get_row(m.rows() - 1) << ']';
  }
  return os;
}

 bMat to_bmat(const bMat &m){  return m;}

 sMat to_smat(const bMat &m)
{
  sMat temp(m.rows(), m.cols());
  for (int i = 0; i < temp.rows(); ++i) {
    for (int j = 0; j < temp.cols(); ++j) {
      temp.armaMat(i, j) = (short)m.armaMat(i, j);
    }
  }
  return temp;
}

iMat to_imat(const bMat &m)
{
  iMat temp(m.rows(), m.cols());
  for (int i = 0; i < temp.rows(); ++i) {
    for (int j = 0; j < temp.cols(); ++j) {
      temp.armaMat(i, j) = (int)m.armaMat(i, j);
    }
  }
  return temp;
}

 dMat to_mat(const bMat &m)
{
  dMat temp(m.rows(), m.cols());
  for (int i = 0; i < temp.rows(); ++i) {
    for (int j = 0; j < temp.cols(); ++j) {
      temp.armaMat(i, j) = (double)m.armaMat(i, j);
    }
  }
  return temp;
}

 cMat to_cmat(const bMat &m)
{
  cMat temp(m.rows(), m.cols());
  for (int i = 0; i < temp.rows(); ++i) {
    for (int j = 0; j < temp.cols(); ++j) {
      temp.armaMat(i, j) = std::complex<double>((double)m.armaMat(i, j), 0.0);
    }
  }
  return temp;
}



mat inv(const mat & m)
{
   mat o(m.rows(),m.cols());
   o.armaMat=inv(m.armaMat); 
   return o;
}

cmat inv(const cmat & m)
{
   cmat o(m.rows(),m.cols());
   o.armaMat=inv(m.armaMat); 
   return o;
}
vec solve(const mat & a,const vec & b)
{
   vec o(b.size());
   o.armaVec=solve(a.armaMat,b.armaVec); 
   return o;  
}

mat operator+ (const double &s, const mat &m)
{
  add_<double,const double&,double>  a;
  return (scalar_binary_applyFunction<double,const double &,double,add_<double,const double&,double> >(s,m,a));
  
}


mat operator+ (const short &s, const mat &m)
{
   mat o(m.rows(),m.cols());
   add_<double,const short &,double>  a;
   scalar_binary_Function(s,m.begin(),m.end(),o.begin(),a);
    
   return o;
}



//The following operator defenitions are autogenerated 




mat operator + (const int  &s , const mat  &m)
{				//scalar op matrix
mat o(m.rows(),m.cols()); 
 add_ < double , const int &, double >  opObject ; 
 scalar_binary_Function (s,m.begin(),m.end(),o.begin(), opObject) ; 
return o ; 
 }
 
mat operator + (const mat  &m , const double  &s)
{				//matrix op scalar
mat o(m.rows(),m.cols()); 
 add_ < double , double, const double & >  opObject ; 
 binary_Function_scalar (m.begin(),m.end(),s,o.begin(), opObject) ; 
return o ; 
 }
 
mat operator + (const mat  &m , const short  &s)
{				//matrix op scalar
mat o(m.rows(),m.cols()); 
 add_ < double , double, const short & >  opObject ; 
 binary_Function_scalar (m.begin(),m.end(),s,o.begin(), opObject) ; 
return o ; 
 }
 
mat operator + (const mat  &m , const int  &s)
{				//matrix op scalar
mat o(m.rows(),m.cols()); 
 add_ < double , double, const int & >  opObject ; 
 binary_Function_scalar (m.begin(),m.end(),s,o.begin(), opObject) ; 
return o ; 
 }
 
mat operator - (const double  &s , const mat  &m)
{				//scalar op matrix
mat o(m.rows(),m.cols()); 
 substract_ < double , const double &, double >  opObject ; 
 scalar_binary_Function (s,m.begin(),m.end(),o.begin(), opObject) ; 
return o ; 
 }
 
mat operator - (const short  &s , const mat  &m)
{				//scalar op matrix
mat o(m.rows(),m.cols()); 
 substract_ < double , const short &, double >  opObject ; 
 scalar_binary_Function (s,m.begin(),m.end(),o.begin(), opObject) ; 
return o ; 
 }
 
mat operator - (const int  &s , const mat  &m)
{				//scalar op matrix
mat o(m.rows(),m.cols()); 
 substract_ < double , const int &, double >  opObject ; 
 scalar_binary_Function (s,m.begin(),m.end(),o.begin(), opObject) ; 
return o ; 
 }
 
mat operator - (const mat  &m , const double  &s)
{				//matrix op scalar
mat o(m.rows(),m.cols()); 
 substract_ < double , double, const double & >  opObject ; 
 binary_Function_scalar (m.begin(),m.end(),s,o.begin(), opObject) ; 
return o ; 
 }
 
mat operator - (const mat  &m , const short  &s)
{				//matrix op scalar
mat o(m.rows(),m.cols()); 
 substract_ < double , double, const short & >  opObject ; 
 binary_Function_scalar (m.begin(),m.end(),s,o.begin(), opObject) ; 
return o ; 
 }
 
mat operator - (const mat  &m , const int  &s)
{				//matrix op scalar
mat o(m.rows(),m.cols()); 
 substract_ < double , double, const int & >  opObject ; 
 binary_Function_scalar (m.begin(),m.end(),s,o.begin(), opObject) ; 
return o ; 
 }
 
mat operator * (const double  &s , const mat  &m)
{				//scalar op matrix
mat o(m.rows(),m.cols()); 
 mult_ < double , const double &, double >  opObject ; 
 scalar_binary_Function (s,m.begin(),m.end(),o.begin(), opObject) ; 
return o ; 
 }
 
mat operator * (const short  &s , const mat  &m)
{				//scalar op matrix
mat o(m.rows(),m.cols()); 
 mult_ < double , const short &, double >  opObject ; 
 scalar_binary_Function (s,m.begin(),m.end(),o.begin(), opObject) ; 
return o ; 
 }
 
mat operator * (const int  &s , const mat  &m)
{				//scalar op matrix
mat o(m.rows(),m.cols()); 
 mult_ < double , const int &, double >  opObject ; 
 scalar_binary_Function (s,m.begin(),m.end(),o.begin(), opObject) ; 
return o ; 
 }
 
mat operator * (const mat  &m , const double  &s)
{				//matrix op scalar
mat o(m.rows(),m.cols()); 
 mult_ < double , double, const double & >  opObject ; 
 binary_Function_scalar (m.begin(),m.end(),s,o.begin(), opObject) ; 
return o ; 
 }
 
mat operator * (const mat  &m , const short  &s)
{				//matrix op scalar
mat o(m.rows(),m.cols()); 
 mult_ < double , double, const short & >  opObject ; 
 binary_Function_scalar (m.begin(),m.end(),s,o.begin(), opObject) ; 
return o ; 
 }
 
mat operator * (const mat  &m , const int  &s)
{				//matrix op scalar
mat o(m.rows(),m.cols()); 
 mult_ < double , double, const int & >  opObject ; 
 binary_Function_scalar (m.begin(),m.end(),s,o.begin(), opObject) ; 
return o ; 
 }
 
mat operator / (const mat  &m , const double  &s)
{				//matrix op scalar
mat o(m.rows(),m.cols()); 
 divide_ < double , double, const double & >  opObject ; 
 binary_Function_scalar (m.begin(),m.end(),s,o.begin(), opObject) ; 
return o ; 
 }
 
mat operator / (const mat  &m , const short  &s)
{				//matrix op scalar
mat o(m.rows(),m.cols()); 
 divide_ < double , double, const short & >  opObject ; 
 binary_Function_scalar (m.begin(),m.end(),s,o.begin(), opObject) ; 
return o ; 
 }
 
mat operator / (const mat  &m , const int  &s)
{				//matrix op scalar
mat o(m.rows(),m.cols()); 
 divide_ < double , double, const int & >  opObject ; 
 binary_Function_scalar (m.begin(),m.end(),s,o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator + (const double  &s , const cmat  &m)
{				//scalar op matrix
 std::complex <double> v2=std::complex <double> (s,0.0);
cmat o(m.rows(),m.cols()); 
 add_ < std::complex< double > , const std::complex< double >&, std::complex< double > >  opObject ; 
 scalar_binary_Function (v2,m.begin(),m.end(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator - (const double  &s , const cmat  &m)
{				//scalar op matrix
 std::complex <double> v2=std::complex <double> (s,0.0);
cmat o(m.rows(),m.cols()); 
 substract_ < std::complex< double > , const std::complex< double >&, std::complex< double > >  opObject ; 
 scalar_binary_Function (v2,m.begin(),m.end(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator * (const double  &s , const cmat  &m)
{				//scalar op matrix
 std::complex <double> v2=std::complex <double> (s,0.0);
cmat o(m.rows(),m.cols()); 
 mult_ < std::complex< double > , const std::complex< double >&, std::complex< double > >  opObject ; 
 scalar_binary_Function (v2,m.begin(),m.end(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator * (const std::complex< double >  &s , const mat  &m)
{				//scalar op matrix
 cmat v2=to_cmat(m);
cmat o(v2.rows(),v2.cols()); 
 mult_ < std::complex< double > , const std::complex< double > &, std::complex< double > >  opObject ; 
 scalar_binary_Function (s,v2.begin(),v2.end(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator * (const mat  &m , const std::complex< double >  &s)
{				//matrix op scalar
 cmat v1=to_cmat(m);
cmat o(v1.rows(),v1.cols()); 
 mult_ < std::complex< double > , std::complex< double >, const std::complex< double > & >  opObject ; 
 binary_Function_scalar (v1.begin(),v1.end(),s,o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator / (const cmat  &m , const double  &s)
{				//matrix op scalar
 std::complex <double> v2=std::complex <double> (s,0.0);
cmat o(m.rows(),m.cols()); 
 divide_ < std::complex< double > , std::complex< double >, const std::complex< double >& >  opObject ; 
 binary_Function_scalar (m.begin(),m.end(),v2,o.begin(), opObject) ; 
return o ; 
 }
 
mat operator + (const bmat  &a , const mat  &b)
{                                  //matrix operation 
mat o(a.rows(),a.cols()); 
 add_ < double , char, double >  opObject ; 
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
mat operator + (const smat  &a , const mat  &b)
{                                  //matrix operation 
mat o(a.rows(),a.cols()); 
 add_ < double , short int, double >  opObject ; 
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
mat operator + (const imat  &a , const mat  &b)
{                                  //matrix operation 
mat o(a.rows(),a.cols()); 
 add_ < double , int, double >  opObject ; 
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
mat operator + (const mat  &a , const bmat  &b)
{                                  //matrix operation 
mat o(a.rows(),a.cols()); 
 add_ < double , double, char >  opObject ; 
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
mat operator + (const mat  &a , const smat  &b)
{                                  //matrix operation 
mat o(a.rows(),a.cols()); 
 add_ < double , double, short int >  opObject ; 
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
mat operator + (const mat  &a , const imat  &b)
{                                  //matrix operation 
mat o(a.rows(),a.cols()); 
 add_ < double , double, int >  opObject ; 
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
mat operator - (const bmat  &a , const mat  &b)
{                                  //matrix operation 
mat o(a.rows(),a.cols()); 
 substract_ < double , char, double >  opObject ; 
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
mat operator - (const smat  &a , const mat  &b)
{                                  //matrix operation 
mat o(a.rows(),a.cols()); 
 substract_ < double , short int, double >  opObject ; 
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
mat operator - (const imat  &a , const mat  &b)
{                                  //matrix operation 
mat o(a.rows(),a.cols()); 
 substract_ < double , int, double >  opObject ; 
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
mat operator - (const mat  &a , const bmat  &b)
{                                  //matrix operation 
mat o(a.rows(),a.cols()); 
 substract_ < double , double, char >  opObject ; 
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
mat operator - (const mat  &a , const smat  &b)
{                                  //matrix operation 
mat o(a.rows(),a.cols()); 
 substract_ < double , double, short int >  opObject ; 
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
mat operator - (const mat  &a , const imat  &b)
{                                  //matrix operation 
mat o(a.rows(),a.cols()); 
 substract_ < double , double, int >  opObject ; 
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator + (const bmat  &a , const cmat  &b)
{                                  //matrix operation 
 cmat v1=to_cmat(a);
cmat o(v1.rows(),v1.cols()); 
 add_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ; 
 binary_Function (v1.begin(),v1.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator + (const smat  &a , const cmat  &b)
{                                  //matrix operation 
 cmat v1=to_cmat(a);
cmat o(v1.rows(),v1.cols()); 
 add_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ; 
 binary_Function (v1.begin(),v1.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator + (const imat  &a , const cmat  &b)
{                                  //matrix operation 
 cmat v1=to_cmat(a);
cmat o(v1.rows(),v1.cols()); 
 add_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ; 
 binary_Function (v1.begin(),v1.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator + (const mat  &a , const cmat  &b)
{                                  //matrix operation 
 cmat v1=to_cmat(a);
cmat o(v1.rows(),v1.cols()); 
 add_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ; 
 binary_Function (v1.begin(),v1.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator + (const cmat  &a , const bmat  &b)
{                                  //matrix operation 
 cmat v2=to_cmat(b);
cmat o(a.rows(),a.cols()); 
 add_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ; 
 binary_Function (a.begin(),a.end(),v2.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator + (const cmat  &a , const smat  &b)
{                                  //matrix operation 
 cmat v2=to_cmat(b);
cmat o(a.rows(),a.cols()); 
 add_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ; 
 binary_Function (a.begin(),a.end(),v2.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator + (const cmat  &a , const imat  &b)
{                                  //matrix operation 
 cmat v2=to_cmat(b);
cmat o(a.rows(),a.cols()); 
 add_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ; 
 binary_Function (a.begin(),a.end(),v2.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator + (const cmat  &a , const mat  &b)
{                                  //matrix operation 
 cmat v2=to_cmat(b);
cmat o(a.rows(),a.cols()); 
 add_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ; 
 binary_Function (a.begin(),a.end(),v2.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator - (const bmat  &a , const cmat  &b)
{                                  //matrix operation 
 cmat v1=to_cmat(a);
cmat o(v1.rows(),v1.cols()); 
 substract_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ; 
 binary_Function (v1.begin(),v1.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator - (const smat  &a , const cmat  &b)
{                                  //matrix operation 
 cmat v1=to_cmat(a);
cmat o(v1.rows(),v1.cols()); 
 substract_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ; 
 binary_Function (v1.begin(),v1.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator - (const imat  &a , const cmat  &b)
{                                  //matrix operation 
 cmat v1=to_cmat(a);
cmat o(v1.rows(),v1.cols()); 
 substract_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ; 
 binary_Function (v1.begin(),v1.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator - (const mat  &a , const cmat  &b)
{                                  //matrix operation 
 cmat v1=to_cmat(a);
cmat o(v1.rows(),v1.cols()); 
 substract_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ; 
 binary_Function (v1.begin(),v1.end(),b.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator - (const cmat  &a , const bmat  &b)
{                                  //matrix operation 
 cmat v2=to_cmat(b);
cmat o(a.rows(),a.cols()); 
 substract_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ; 
 binary_Function (a.begin(),a.end(),v2.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator - (const cmat  &a , const smat  &b)
{                                  //matrix operation 
 cmat v2=to_cmat(b);
cmat o(a.rows(),a.cols()); 
 substract_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ; 
 binary_Function (a.begin(),a.end(),v2.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator - (const cmat  &a , const imat  &b)
{                                  //matrix operation 
 cmat v2=to_cmat(b);
cmat o(a.rows(),a.cols()); 
 substract_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ; 
 binary_Function (a.begin(),a.end(),v2.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator - (const cmat  &a , const mat  &b)
{                                  //matrix operation 
 cmat v2=to_cmat(b);
cmat o(a.rows(),a.cols()); 
 substract_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ; 
 binary_Function (a.begin(),a.end(),v2.begin(),o.begin(), opObject) ; 
return o ; 
 }
 
cmat operator * (const mat  &a , const cmat  &b)
{                                  //matrix operation 
 cmat o(a.rows(),b.cols());
  o.armaMat=a.armaMat*b.armaMat;
return o ; 
 }
 
cmat operator * (const bmat  &a , const cmat  &b)
{                                  //matrix operation 
 cmat o(a.rows(),b.cols());
  o.armaMat=a.armaMat*b.armaMat;
return o ; 
 }
 
cmat operator * (const smat  &a , const cmat  &b)
{                                  //matrix operation 
 cmat o(a.rows(),b.cols());
  o.armaMat=a.armaMat*b.armaMat;
return o ; 
 }
 
cmat operator * (const imat  &a , const cmat  &b)
{                                  //matrix operation 
 cmat o(a.rows(),b.cols());
  o.armaMat=a.armaMat*b.armaMat;
return o ; 
 }
 
cmat operator * (const cmat  &a , const mat  &b)
{                                  //matrix operation 
 cmat o(a.rows(),b.cols());
  o.armaMat=a.armaMat*b.armaMat;
return o ; 
 }
 
cmat operator * (const cmat  &a , const bmat  &b)
{                                  //matrix operation 
 cmat o(a.rows(),b.cols());
  o.armaMat=a.armaMat*b.armaMat;
return o ; 
 }
 
cmat operator * (const cmat  &a , const smat  &b)
{                                  //matrix operation 
 cmat o(a.rows(),b.cols());
  o.armaMat=a.armaMat*b.armaMat;
return o ; 
 }
 
cmat operator * (const cmat  &a , const imat  &b)
{                                  //matrix operation 
 cmat o(a.rows(),b.cols());
  o.armaMat=a.armaMat*b.armaMat;
return o ; 
 }
 
std::istream &operator>>(std::istream &is, bMat &m)
{
   std::ostringstream buffer;
   bool started = false;
   bool finished = false;
   bool brackets = false;
   bool within_double_brackets = false;
   char c;
   
   while (!finished) {
     if (is.eof()) {
       finished = true;
     }
     else {
       is.get(c);
       
       if (is.eof() || (c == '\n')) {
	 if (brackets) {
	   // Right bracket missing
	   is.setstate(std::ios_base::failbit);
	   finished = true;
	 }
	 else if (!((c == '\n') && !started)) {
	   finished = true;
	 }
       }
       else if ((c == ' ') || (c == '\t')) {
	 if (started) {
	   buffer << ' ';
	 }
       }
       else if (c == '[') {
	 if ((started && !brackets) || within_double_brackets) {
	   // Unexpected left bracket
	   is.setstate(std::ios_base::failbit);
	   finished = true;
	 }
	 else if (!started) {
	   started = true;
	   brackets = true;
	 }
	 else {
	   within_double_brackets = true;
	 }
       }
       else if (c == ']') {
	 if (!started || !brackets) {
	   // Unexpected right bracket
	   is.setstate(std::ios_base::failbit);
	   finished = true;
	 }
	 else if (within_double_brackets) {
	   within_double_brackets = false;
	   buffer << ';';
	 }
	 else {
	   finished = true;
	 }
	 while (!is.eof() && (((c = static_cast<char>(is.peek())) == ' ')
	   || (c == '\t'))) {
	   is.get();
	 }
	 if (!is.eof() && (c == '\n')) {
	   is.get();
	 }
       }
       else {
	 started = true;
	 buffer << c;
       }
     }
   }
   
   if (!started) {
     m.set_size(0, false);
   }
   else {
     m.set(buffer.str());
   }
   
   return is;
 }

cMat to_cmat(const cMat& m) {
    return m;
}





}//namespace wrapper
