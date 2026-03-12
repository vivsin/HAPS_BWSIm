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



#include "../include/vec.h"
#include "../include/mat.h"
namespace wrapper{

bVec::bVec(const bMat &m){if(m.cols()==1)armaVec=m.armaMat;else armaVec=m.armaMat.st();  }

bVec bVec::operator()(const Vec<int> &indexlist) const {bVec toReturn; toReturn.armaVec=armaVec.elem(getUVec(indexlist.armaVec)); return(toReturn);}


bmat bvec::transpose() {
    bmat temp;
    temp.armaMat=armaVec.st();
    return temp;
}

bmat bvec::T() {
    bmat temp;
    temp.armaMat=armaVec.st();
    return temp;
}


bmat bvec::hermitian_transpose() {
    bmat temp;
    temp.armaMat=armaVec.t();
    return temp;
}

bmat bvec::H() {
    bmat temp;
    temp.armaMat=armaVec.t();
    return temp;
}


bVec bVec::get(const Vec<int>& indexlist) const{bVec toReturn; toReturn.armaVec=armaVec.elem(getUVec(indexlist.armaVec)); return(toReturn);}

bVec getBVec(arma::uvec input) {

  bVec output(input.size());
  for(arma::uword cnt=0;cnt<output.armaVec.size();cnt++)
    output(cnt)=(char)(input(cnt)!=0);
  return(output);
}

arma::uvec getUVec(arma::Col<int> input) {

  arma::uvec output(input.size());
  for(arma::uword cnt=0;cnt<output.size();cnt++)    output(cnt)=input(cnt);
  return(output);
}

arma::Col<int> getIVec(arma::uvec input) {

  arma::Col<int> output(input.size());
  for(arma::uword cnt=0;cnt<output.size();cnt++)    output(cnt)=input(cnt);
  return(output);
}


bVec operator+(const bVec &v1, const bVec &v2){bVec toReturn(v1.length()); for(arma::uword cnt=0;cnt<toReturn.armaVec.n_elem;cnt++) toReturn.armaVec(cnt)=v1.armaVec(cnt)^v2.armaVec(cnt); return(toReturn);}
bVec operator+(const bVec &v, char t){bVec toReturn(v.length()); for(arma::uword cnt=0;cnt<toReturn.armaVec.n_elem;cnt++) toReturn.armaVec(cnt)=v.armaVec(cnt)^t; return(toReturn);}
bVec operator+(char t, const bVec &v){bVec toReturn(v.length()); for(arma::uword cnt=0;cnt<toReturn.armaVec.n_elem;cnt++) toReturn.armaVec(cnt)=v.armaVec(cnt)^t; return(toReturn);}
bVec operator-(const bVec &v1, const bVec &v2){bVec toReturn(v1.length()); for(arma::uword cnt=0;cnt<toReturn.armaVec.n_elem;cnt++) toReturn.armaVec(cnt)=v1.armaVec(cnt)^v2.armaVec(cnt); return(toReturn);}
bVec operator-(const bVec &v, char t){bVec toReturn(v.length()); for(arma::uword cnt=0;cnt<toReturn.armaVec.n_elem;cnt++) toReturn.armaVec(cnt)=v.armaVec(cnt)^t; return(toReturn);}
bVec operator-(char t, const bVec &v){bVec toReturn(v.length()); for(arma::uword cnt=0;cnt<toReturn.armaVec.n_elem;cnt++) toReturn.armaVec(cnt)=v.armaVec(cnt)^t; return(toReturn);}
bVec operator-(const bVec &v){return(v);}
bVec operator*(const bVec &v, char t){bVec toReturn(v.length()); for(arma::uword cnt=0;cnt<toReturn.armaVec.n_elem;cnt++) toReturn.armaVec(cnt)=v.armaVec(cnt)&t; return(toReturn);}
bVec operator*(char t, const bVec &v){bVec toReturn(v.length()); for(arma::uword cnt=0;cnt<toReturn.armaVec.n_elem;cnt++) toReturn.armaVec(cnt)=v.armaVec(cnt)&t; return(toReturn);}
bVec elem_mult(const bVec &a, const bVec &b){bVec toReturn(a.length()); for(arma::uword cnt=0;cnt<toReturn.armaVec.n_elem;cnt++) toReturn.armaVec(cnt)=a.armaVec(cnt)&b.armaVec(cnt); return(toReturn);}
bVec operator/(const bVec &v, char t){bVec toReturn(v.length()); for(arma::uword cnt=0;cnt<toReturn.armaVec.n_elem;cnt++) toReturn.armaVec(cnt)=v.armaVec(cnt)|t; return(toReturn);}
bVec operator/(char t, const bVec &v){bVec toReturn(v.length()); for(arma::uword cnt=0;cnt<toReturn.armaVec.n_elem;cnt++) toReturn.armaVec(cnt)=v.armaVec(cnt)|t; return(toReturn);}
bVec elem_div(const bVec &v1, const bVec &v2){bVec toReturn(v1.length()); for(arma::uword cnt=0;cnt<toReturn.armaVec.n_elem;cnt++) toReturn.armaVec(cnt)=v1.armaVec(cnt)|v2.armaVec(cnt); return(toReturn);}
bVec elem_div(char t, const bVec &v){bVec toReturn(v.length()); for(arma::uword cnt=0;cnt<toReturn.armaVec.n_elem;cnt++) toReturn.armaVec(cnt)=v.armaVec(cnt)|t; return(toReturn);}
bVec concat(const bVec &v, char t){bVec toReturn;
if(v.size()>0)toReturn=v;toReturn.armaVec.resize(v.armaVec.size()+1);toReturn.armaVec(v.armaVec.size())=t;  return(toReturn);}
bVec concat(char t, const bVec &v){bVec toReturn(v.length()+1);toReturn.armaVec((arma::uword)0)=t;
if(v.size()>0)toReturn.armaVec.subvec((arma::uword)1,toReturn.armaVec.n_elem-(arma::uword)1)=v.armaVec;return(toReturn);}
bVec concat(const bVec &v1, const bVec &v2){bVec toReturn;if(v1.size()>0)toReturn=v1;toReturn.armaVec.resize(v1.armaVec.size()+v2.armaVec.size());if(v2.size()>0)toReturn.armaVec.subvec(v1.armaVec.size(),toReturn.armaVec.n_elem-(arma::uword)1)=v2.armaVec;return(toReturn);}
//!  
bVec concat(const bVec &v1, const bVec &v2, const bVec &v3) {
  bVec toReturn;
  if(v1.size()>0)toReturn=v1;
  toReturn.armaVec.resize(v1.armaVec.size()+v2.armaVec.size()+v3.armaVec.size());
  if(v2.size()>0)toReturn.armaVec.subvec(v1.armaVec.size(),toReturn.armaVec.n_elem-(arma::uword)(1+v3.armaVec.size()))=v2.armaVec;
  if(v3.size()>0)toReturn.armaVec.subvec(v1.armaVec.size()+v2.armaVec.size(),toReturn.armaVec.n_elem-(arma::uword)1)=v3.armaVec;
  return(toReturn);
}



std::ostream &operator<<(std::ostream &os, const bVec &v)
{
  arma::uword sz = v.armaVec.n_elem;
  os << "[" ;
  for (arma::uword i = 0; i < sz; i++)
  {
    os << (int)(v.armaVec(i)) ;
    if (i < (sz - 1))
      os << " ";
  }
  os << "]" ;
  return os;
}

bVec to_bvec(const bVec &v){  return(v);}

sVec to_svec(const bVec &v)
{
  sVec temp(v.length());
  for (int i = 0; i < v.length(); ++i) temp(i) = (short)(v.armaVec(i)&1);
  return temp;
}

iVec to_ivec(const bVec &v)
{
  iVec temp(v.length());
  for (int i = 0; i < v.length(); ++i)	temp(i) = (int)(v.armaVec(i)&1);
  return temp;
}

dVec to_vec(const bVec &v)
{
  dVec temp(v.length());
  for (int i = 0; i < v.length(); ++i)
    temp(i) = (double)(v.armaVec(i)&1);
  return temp;
}

cVec to_cvec(const bVec &v)
{
  cVec temp(v.length());
  for (int i = 0; i < v.length(); ++i)
    temp(i) = std::complex<double>((double)(v.armaVec(i)&1), 0.0);
  return temp;
}

std::ostream &operator<<(std::ostream &os, const std::complex<double>  &v)
  {
    if(imag(v)>=0)
      os << real(v)<<"+"<<imag(v)<<"i";
    else
      os << real(v)<<""<<imag(v)<<"i";
        return os;
  }



Vec< std::complex<double> > operator+(const bVec &v1, const Vec<std::complex< double > > &v2) {
    Vec<std::complex< double > > toReturn;
    toReturn.armaVec=v1.armaVec+v2.armaVec;
    return(toReturn);
}
 Vec< std::complex<double> > operator+( const Vec<std::complex< double > > &v1,const bVec &v2) {
    Vec<std::complex< double > > toReturn;
    toReturn.armaVec=v1.armaVec+v2.armaVec;
    return(toReturn);
}
Vec<double > operator+(const iVec &v1, const Vec< double  > &v2) {
    Vec< double  > toReturn;
    toReturn.armaVec=v1.armaVec+v2.armaVec;
    return(toReturn);
}
Vec< std::complex<double> > operator+(const Vec<std::complex< double > > & v1, const Vec<std::complex< double > > &v2) {
    Vec<std::complex< double > > toReturn;
    toReturn.armaVec=v1.armaVec+v2.armaVec;
    return(toReturn);
}


Vec< std::complex<double> > operator-(const bVec &v1, const Vec<std::complex< double > > &v2) {
    Vec<std::complex< double > > toReturn;
    toReturn.armaVec=v1.armaVec-v2.armaVec;
    return(toReturn);
}
 Vec< std::complex<double> > operator-( const Vec<std::complex< double > > &v1,const bVec &v2) {
    Vec<std::complex< double > > toReturn;
    toReturn.armaVec=v1.armaVec-v2.armaVec;
    return(toReturn);
}
Vec<double > operator-(const iVec &v1, const Vec< double  > &v2) {
    Vec< double  > toReturn;
    toReturn.armaVec=v1.armaVec-v2.armaVec;
    return(toReturn);
}

Vec<double > operator-(const Vec< double  > &v1,const iVec  &v2) {
    Vec< double  > toReturn;
    toReturn.armaVec=v1.armaVec-v2.armaVec;
    return(toReturn);
}

Vec< std::complex<double> > operator-(const Vec<std::complex< double > > & v1, const Vec<std::complex< double > > &v2) {
    Vec<std::complex< double > > toReturn;
    toReturn.armaVec=v1.armaVec-v2.armaVec;
    return(toReturn);
}



 Vec< std::complex<double> > operator*(std::complex<double> t, const Vec<double> &v) {

    return( t* to_cvec<double>(v) );
}

 Vec< std::complex<double> > operator*( const Vec<double> &v,std::complex<double> t) {

    return(  t*to_cvec<double>(v));
}

std::complex<double> operator*( Vec< std::complex<double> > &v, Vec< std::complex<double> > &v1) {
    std::complex<double> returnValue;
    cMat m=v;
    cMat m1=v1.H();
    cMat pm=m*m1;
    returnValue=pm(0,0);
    return(returnValue);
}

double operator*( iVec &v, iVec &v1) {
    double returnValue;
    iMat m=v;
    iMat m1=v1.H();
    iMat pm=m*m1;
    returnValue=pm(0,0);
    return((double)returnValue);
}

double operator*( dVec &v, dVec &v1) {
    double returnValue;
    dMat m=v;
    dMat m1=v1.H();
    dMat pm=m*m1;
    returnValue=pm(0,0);
    return((double)returnValue);
}

double operator*( sVec &v, sVec &v1) {
    double returnValue;
    sMat m=v;
    sMat m1=v1.H();
    sMat pm=m*m1;
    returnValue=pm(0,0);
    return((double)returnValue);
}


double operator*( bVec &v, bVec &v1) {
    double returnValue;
    bMat m=v;
    bMat m1=v1.H();
    bMat pm=m*m1;
    returnValue=pm(0,0);
    return((double)returnValue);
}

Vec< std::complex<double> >  operator/(Vec< std::complex<double> > &v, const int t) {

  cVec temp(v.length());
    for (int i = 0; i < v.length(); ++i)
    {
      std::complex<double> val=v[i];std::complex<double>tc=std::complex<double>(t,0.0);
        temp[i] =std::complex<double>( val/tc);
    }
    return temp;
}

Vec< std::complex<double> >  operator/(int t,Vec< std::complex<double> > &v) {
   Vec< std::complex<double> > toReturn;
    toReturn.armaVec=(std::complex<double>) t/v.armaVec;
    return(toReturn);
}



bVec to_bvec(const char c){bVec temp(1); temp.armaVec(0)=c;return temp;}
sVec to_svec(const short c){sVec temp(1); temp.armaVec(0)=c;return temp;}
iVec to_ivec(const int c){iVec temp(1); temp.armaVec(0)=c;return temp;}
dVec to_vec(const double c){dVec temp(1); temp.armaVec(0)=c;return temp;}
cVec to_cvec(const std::complex<double> c){cVec temp(1); temp.armaVec(0)=c;return temp;}

cVec to_cvec(double real, double imag){cVec temp(1); temp.armaVec(0)=std::complex<double>(real,imag);return temp;}


//the following and automatically generated functions


vec operator + (const float  &s , const vec  &v)
{				//scalar op vecrix
vec o(v.size());
 add_ < double , const float &, double >  opObject ;
 scalar_binary_Function (s,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

vec operator + (const short  &s , const vec  &v)
{				//scalar op vecrix
vec o(v.size());
 add_ < double , const short &, double >  opObject ;
 scalar_binary_Function (s,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

vec operator + (const int  &s , const vec  &v)
{				//scalar op vecrix
vec o(v.size());
 add_ < double , const int &, double >  opObject ;
 scalar_binary_Function (s,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

vec operator + (const vec  &v , const float  &s)
{				//vecrix op scalar
vec o(v.size());
 add_ < double , double, const float & >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),s,o.begin(), opObject) ;
return o ;
 }

vec operator + (const vec  &v , const short  &s)
{				//vecrix op scalar
vec o(v.size());
 add_ < double , double, const short & >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),s,o.begin(), opObject) ;
return o ;
 }

vec operator + (const vec  &v , const int  &s)
{				//vecrix op scalar
vec o(v.size());
 add_ < double , double, const int & >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),s,o.begin(), opObject) ;
return o ;
 }

vec operator - (const float  &s , const vec  &v)
{				//scalar op vecrix
vec o(v.size());
 substract_ < double , const float &, double >  opObject ;
 scalar_binary_Function (s,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

vec operator - (const short  &s , const vec  &v)
{				//scalar op vecrix
vec o(v.size());
 substract_ < double , const short &, double >  opObject ;
 scalar_binary_Function (s,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

vec operator - (const int  &s , const vec  &v)
{				//scalar op vecrix
vec o(v.size());
 substract_ < double , const int &, double >  opObject ;
 scalar_binary_Function (s,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

vec operator - (const vec  &v , const float  &s)
{				//vecrix op scalar
vec o(v.size());
 substract_ < double , double, const float & >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),s,o.begin(), opObject) ;
return o ;
 }

vec operator - (const vec  &v , const short  &s)
{				//vecrix op scalar
vec o(v.size());
 substract_ < double , double, const short & >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),s,o.begin(), opObject) ;
return o ;
 }

vec operator - (const vec  &v , const int  &s)
{				//vecrix op scalar
vec o(v.size());
 substract_ < double , double, const int & >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),s,o.begin(), opObject) ;
return o ;
 }

vec operator * (const float  &s , const vec  &v)
{				//scalar op vecrix
vec o(v.size());
 mult_ < double , const float &, double >  opObject ;
 scalar_binary_Function (s,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

vec operator * (const short  &s , const vec  &v)
{				//scalar op vecrix
vec o(v.size());
 mult_ < double , const short &, double >  opObject ;
 scalar_binary_Function (s,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

vec operator * (const int  &s , const vec  &v)
{				//scalar op vecrix
vec o(v.size());
 mult_ < double , const int &, double >  opObject ;
 scalar_binary_Function (s,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }



vec operator * (const vec  &v , const float  &s)
{				//vecrix op scalar
vec o(v.size());
 mult_ < double , double, const float & >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),s,o.begin(), opObject) ;
return o ;
 }

vec operator * (const vec  &v , const short  &s)
{				//vecrix op scalar
vec o(v.size());
 mult_ < double , double, const short & >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),s,o.begin(), opObject) ;
return o ;
 }

vec operator * (const vec  &v , const int  &s)
{				//vecrix op scalar
vec o(v.size());
 mult_ < double , double, const int & >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),s,o.begin(), opObject) ;
return o ;
 }



vec operator / (const vec  &v , const float  &s)
{				//vecrix op scalar
vec o(v.size());
 divide_ < double , double, const float & >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),s,o.begin(), opObject) ;
return o ;
 }

vec operator / (const vec  &v , const short  &s)
{				//vecrix op scalar
vec o(v.size());
 divide_ < double , double, const short & >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),s,o.begin(), opObject) ;
return o ;
 }

vec operator / (const vec  &v , const int  &s)
{				//vecrix op scalar
vec o(v.size());
 divide_ < double , double, const int & >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),s,o.begin(), opObject) ;
return o ;
 }

vec operator + (const double  &s , const ivec  &v)
{				//scalar op vecrix
vec o(v.size());
 add_ < double , const double &, int >  opObject ;
 scalar_binary_Function (s,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

vec operator + (const ivec  &v , const double  &s)
{				//vecrix op scalar
vec o(v.size());
 add_ < double , int, const double & >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),s,o.begin(), opObject) ;
return o ;
 }

vec operator - (const double  &s , const ivec  &v)
{				//scalar op vecrix
vec o(v.size());
 substract_ < double , const double &, int >  opObject ;
 scalar_binary_Function (s,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

vec operator - (const ivec  &v , const double  &s)
{				//vecrix op scalar
vec o(v.size());
 substract_ < double , int, const double & >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),s,o.begin(), opObject) ;
return o ;
 }

vec operator * (const double  &s , const ivec  &v)
{				//scalar op vecrix
vec o(v.size());
 mult_ < double , const double &, int >  opObject ;
 scalar_binary_Function (s,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

vec operator * (const ivec  &v , const double  &s)
{				//vecrix op scalar
vec o(v.size());
 mult_ < double , int, const double & >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),s,o.begin(), opObject) ;
return o ;
 }

vec operator / (const double  &s , const ivec  &v)
{				//scalar op vecrix
vec o(v.size());
 divide_ < double , const double &, int >  opObject ;
 scalar_binary_Function (s,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

vec operator / (const ivec  &v , const double  &s)
{				//vecrix op scalar
vec o(v.size());
 divide_ < double , int, const double & >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),s,o.begin(), opObject) ;
return o ;
 }

cvec operator + (const std::complex< double >  &s , const ivec  &v)
{				//scalar op vecrix
 cvec v2=to_cvec(v);
cvec o(v2.size());
 add_ < std::complex< double > , const std::complex< double > &, std::complex< double > >  opObject ;
 scalar_binary_Function (s,v2.begin(),v2.end(),o.begin(), opObject) ;
return o ;
 }

cvec operator + (const ivec  &v , const std::complex< double >  &s)
{				//vecrix op scalar
 cvec v1=to_cvec(v);
cvec o(v1.size());
 add_ < std::complex< double > , std::complex< double >, const std::complex< double > & >  opObject ;
 binary_Function_scalar (v1.begin(),v1.end(),s,o.begin(), opObject) ;
return o ;
 }

cvec operator - (const std::complex< double >  &s , const ivec  &v)
{				//scalar op vecrix
 cvec v2=to_cvec(v);
cvec o(v2.size());
 substract_ < std::complex< double > , const std::complex< double > &, std::complex< double > >  opObject ;
 scalar_binary_Function (s,v2.begin(),v2.end(),o.begin(), opObject) ;
return o ;
 }

cvec operator - (const ivec  &v , const std::complex< double >  &s)
{				//vecrix op scalar
 cvec v1=to_cvec(v);
cvec o(v1.size());
 substract_ < std::complex< double > , std::complex< double >, const std::complex< double > & >  opObject ;
 binary_Function_scalar (v1.begin(),v1.end(),s,o.begin(), opObject) ;
return o ;
 }

cvec operator * (const std::complex< double >  &s , const ivec  &v)
{				//scalar op vecrix
 cvec v2=to_cvec(v);
cvec o(v2.size());
 mult_ < std::complex< double > , const std::complex< double > &, std::complex< double > >  opObject ;
 scalar_binary_Function (s,v2.begin(),v2.end(),o.begin(), opObject) ;
return o ;
 }

cvec operator * (const ivec  &v , const std::complex< double >  &s)
{				//vecrix op scalar
 cvec v1=to_cvec(v);
cvec o(v1.size());
 mult_ < std::complex< double > , std::complex< double >, const std::complex< double > & >  opObject ;
 binary_Function_scalar (v1.begin(),v1.end(),s,o.begin(), opObject) ;
return o ;
 }

cvec operator / (const std::complex< double >  &s , const ivec  &v)
{				//scalar op vecrix
 cvec v2=to_cvec(v);
cvec o(v2.size());
 divide_ < std::complex< double > , const std::complex< double > &, std::complex< double > >  opObject ;
 scalar_binary_Function (s,v2.begin(),v2.end(),o.begin(), opObject) ;
return o ;
 }

cvec operator / (const ivec  &v , const std::complex< double >  &s)
{				//vecrix op scalar
 cvec v1=to_cvec(v);
cvec o(v1.size());
 divide_ < std::complex< double > , std::complex< double >, const std::complex< double > & >  opObject ;
 binary_Function_scalar (v1.begin(),v1.end(),s,o.begin(), opObject) ;
return o ;
 }

cvec operator + (const double  &s , const cvec  &v)
{				//scalar op vecrix
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 add_ < std::complex< double > , const std::complex< double >&, std::complex< double > >  opObject ;
 scalar_binary_Function (v2,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

cvec operator + (const float  &s , const cvec  &v)
{				//scalar op vecrix
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 add_ < std::complex< double > , const std::complex< double >&, std::complex< double > >  opObject ;
 scalar_binary_Function (v2,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

cvec operator + (const short  &s , const cvec  &v)
{				//scalar op vecrix
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 add_ < std::complex< double > , const std::complex< double >&, std::complex< double > >  opObject ;
 scalar_binary_Function (v2,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

cvec operator + (const int  &s , const cvec  &v)
{				//scalar op vecrix
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 add_ < std::complex< double > , const std::complex< double >&, std::complex< double > >  opObject ;
 scalar_binary_Function (v2,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

cvec operator + (const cvec  &v , const float  &s)
{				//vecrix op scalar
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 add_ < std::complex< double > , std::complex< double >, const std::complex< double >& >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),v2,o.begin(), opObject) ;
return o ;
 }

cvec operator + (const cvec  &v , const double  &s)
{				//vecrix op scalar
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 add_ < std::complex< double > , std::complex< double >, const std::complex< double >& >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),v2,o.begin(), opObject) ;
return o ;
 }

cvec operator + (const cvec  &v , const short  &s)
{				//vecrix op scalar
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 add_ < std::complex< double > , std::complex< double >, const std::complex< double >& >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),v2,o.begin(), opObject) ;
return o ;
 }

cvec operator + (const cvec  &v , const int  &s)
{				//vecrix op scalar
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 add_ < std::complex< double > , std::complex< double >, const std::complex< double >& >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),v2,o.begin(), opObject) ;
return o ;
 }

cvec operator - (const double  &s , const cvec  &v)
{				//scalar op vecrix
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 substract_ < std::complex< double > , const std::complex< double >&, std::complex< double > >  opObject ;
 scalar_binary_Function (v2,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

cvec operator - (const float  &s , const cvec  &v)
{				//scalar op vecrix
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 substract_ < std::complex< double > , const std::complex< double >&, std::complex< double > >  opObject ;
 scalar_binary_Function (v2,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

cvec operator - (const short  &s , const cvec  &v)
{				//scalar op vecrix
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 substract_ < std::complex< double > , const std::complex< double >&, std::complex< double > >  opObject ;
 scalar_binary_Function (v2,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

cvec operator - (const int  &s , const cvec  &v)
{				//scalar op vecrix
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 substract_ < std::complex< double > , const std::complex< double >&, std::complex< double > >  opObject ;
 scalar_binary_Function (v2,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

cvec operator - (const cvec  &v , const float  &s)
{				//vecrix op scalar
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 substract_ < std::complex< double > , std::complex< double >, const std::complex< double >& >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),v2,o.begin(), opObject) ;
return o ;
 }

cvec operator - (const cvec  &v , const double  &s)
{				//vecrix op scalar
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 substract_ < std::complex< double > , std::complex< double >, const std::complex< double >& >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),v2,o.begin(), opObject) ;
return o ;
 }

cvec operator - (const cvec  &v , const short  &s)
{				//vecrix op scalar
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 substract_ < std::complex< double > , std::complex< double >, const std::complex< double >& >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),v2,o.begin(), opObject) ;
return o ;
 }

cvec operator - (const cvec  &v , const int  &s)
{				//vecrix op scalar
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 substract_ < std::complex< double > , std::complex< double >, const std::complex< double >& >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),v2,o.begin(), opObject) ;
return o ;
 }

cvec operator * (const double  &s , const cvec  &v)
{				//scalar op vecrix
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 mult_ < std::complex< double > , const std::complex< double >&, std::complex< double > >  opObject ;
 scalar_binary_Function (v2,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

cvec operator * (const float  &s , const cvec  &v)
{				//scalar op vecrix
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 mult_ < std::complex< double > , const std::complex< double >&, std::complex< double > >  opObject ;
 scalar_binary_Function (v2,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

cvec operator * (const short  &s , const cvec  &v)
{				//scalar op vecrix
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 mult_ < std::complex< double > , const std::complex< double >&, std::complex< double > >  opObject ;
 scalar_binary_Function (v2,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

cvec operator * (const int  &s , const cvec  &v)
{				//scalar op vecrix
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 mult_ < std::complex< double > , const std::complex< double >&, std::complex< double > >  opObject ;
 scalar_binary_Function (v2,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

cvec operator * (const cvec  &v , const float  &s)
{				//vecrix op scalar
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 mult_ < std::complex< double > , std::complex< double >, const std::complex< double >& >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),v2,o.begin(), opObject) ;
return o ;
 }

cvec operator * (const cvec  &v , const double  &s)
{				//vecrix op scalar
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 mult_ < std::complex< double > , std::complex< double >, const std::complex< double >& >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),v2,o.begin(), opObject) ;
return o ;
 }

cvec operator * (const cvec  &v , const short  &s)
{				//vecrix op scalar
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 mult_ < std::complex< double > , std::complex< double >, const std::complex< double >& >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),v2,o.begin(), opObject) ;
return o ;
 }

cvec operator * (const cvec  &v , const int  &s)
{				//vecrix op scalar
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 mult_ < std::complex< double > , std::complex< double >, const std::complex< double >& >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),v2,o.begin(), opObject) ;
return o ;
 }

cvec operator / (const cvec  &v , const double  &s)
{				//vecrix op scalar
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 divide_ < std::complex< double > , std::complex< double >, const std::complex< double >& >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),v2,o.begin(), opObject) ;
return o ;
 }

cvec operator / (const double  &s , const cvec  &v)
{				//scalar op vecrix
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 divide_ < std::complex< double > , const std::complex< double >&, std::complex< double > >  opObject ;
 scalar_binary_Function (v2,v.begin(),v.end(),o.begin(), opObject) ;
return o ;
 }

cvec operator / (const cvec  &v , const float  &s)
{				//vecrix op scalar
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 divide_ < std::complex< double > , std::complex< double >, const std::complex< double >& >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),v2,o.begin(), opObject) ;
return o ;
 }

cvec operator / (const cvec  &v , const short  &s)
{				//vecrix op scalar
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 divide_ < std::complex< double > , std::complex< double >, const std::complex< double >& >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),v2,o.begin(), opObject) ;
return o ;
 }

cvec operator / (const cvec  &v , const int  &s)
{				//vecrix op scalar
 std::complex <double> v2=std::complex <double> (s,0.0);
cvec o(v.size());
 divide_ < std::complex< double > , std::complex< double >, const std::complex< double >& >  opObject ;
 binary_Function_scalar (v.begin(),v.end(),v2,o.begin(), opObject) ;
return o ;
 }

vec operator + (const bvec  &a , const vec  &b)
{                                  //vecrix operation
vec o(a.size());
 add_ < double , char, double >  opObject ;
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ;
return o ;
 }

vec operator + (const svec  &a , const vec  &b)
{                                  //vecrix operation
vec o(a.size());
 add_ < double , short int, double >  opObject ;
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ;
return o ;
 }



vec operator + (const vec  &a , const bvec  &b)
{                                  //vecrix operation
vec o(a.size());
 add_ < double , double, char >  opObject ;
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ;
return o ;
 }

vec operator + (const vec  &a , const svec  &b)
{                                  //vecrix operation
vec o(a.size());
 add_ < double , double, short int >  opObject ;
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ;
return o ;
 }

vec operator + (const vec  &a , const ivec  &b)
{                                  //vecrix operation
vec o(a.size());
 add_ < double , double, int >  opObject ;
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ;
return o ;
 }

vec operator - (const bvec  &a , const vec  &b)
{                                  //vecrix operation
vec o(a.size());
 substract_ < double , char, double >  opObject ;
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ;
return o ;
 }

vec operator - (const svec  &a , const vec  &b)
{                                  //vecrix operation
vec o(a.size());
 substract_ < double , short int, double >  opObject ;
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ;
return o ;
 }



vec operator - (const vec  &a , const bvec  &b)
{                                  //vecrix operation
vec o(a.size());
 substract_ < double , double, char >  opObject ;
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ;
return o ;
 }

vec operator - (const vec  &a , const svec  &b)
{                                  //vecrix operation
vec o(a.size());
 substract_ < double , double, short int >  opObject ;
 binary_Function (a.begin(),a.end(),b.begin(),o.begin(), opObject) ;
return o ;
 }



cvec operator + (const svec  &a , const cvec  &b)
{                                  //vecrix operation
 cvec v1=to_cvec(a);
cvec o(v1.size());
 add_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ;
 binary_Function (v1.begin(),v1.end(),b.begin(),o.begin(), opObject) ;
return o ;
 }

cvec operator + (const ivec  &a , const cvec  &b)
{                                  //vecrix operation
 cvec v1=to_cvec(a);
cvec o(v1.size());
 add_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ;
 binary_Function (v1.begin(),v1.end(),b.begin(),o.begin(), opObject) ;
return o ;
 }



cvec operator + (const cvec  &a , const svec  &b)
{                                  //vecrix operation
 cvec v2=to_cvec(b);
cvec o(a.size());
 add_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ;
 binary_Function (a.begin(),a.end(),v2.begin(),o.begin(), opObject) ;
return o ;
 }

cvec operator + (const cvec  &a , const ivec  &b)
{                                  //vecrix operation
 cvec v2=to_cvec(b);
cvec o(a.size());
 add_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ;
 binary_Function (a.begin(),a.end(),v2.begin(),o.begin(), opObject) ;
return o ;
 }



cvec operator - (const svec  &a , const cvec  &b)
{                                  //vecrix operation
 cvec v1=to_cvec(a);
cvec o(v1.size());
 substract_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ;
 binary_Function (v1.begin(),v1.end(),b.begin(),o.begin(), opObject) ;
return o ;
 }

cvec operator - (const ivec  &a , const cvec  &b)
{                                  //vecrix operation
 cvec v1=to_cvec(a);
cvec o(v1.size());
 substract_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ;
 binary_Function (v1.begin(),v1.end(),b.begin(),o.begin(), opObject) ;
return o ;
 }



cvec operator - (const cvec  &a , const svec  &b)
{                                  //vecrix operation
 cvec v2=to_cvec(b);
cvec o(a.size());
 substract_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ;
 binary_Function (a.begin(),a.end(),v2.begin(),o.begin(), opObject) ;
return o ;
 }

cvec operator - (const cvec  &a , const ivec  &b)
{                                  //vecrix operation
 cvec v2=to_cvec(b);
cvec o(a.size());
 substract_ < std::complex< double > , std::complex< double >, std::complex< double > >  opObject ;
 binary_Function (a.begin(),a.end(),v2.begin(),o.begin(), opObject) ;
return o ;
 }


std::istream &operator>>(std::istream &is, bVec &v)
{
    std::ostringstream buffer;
    bool started = false;
    bool finished = false;
    bool brackets = false;
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
    else if ((c == ' ') || (c == '\t') || (c == ',')) {
	  if (started) {
	    buffer << ' ';
	  }
	}
	else if (c == '[') {
	  if (started) {
	    // Unexpected left bracket
	    is.setstate(std::ios_base::failbit);
	    finished = true;
	  }
	  else {
	    started = true;
	    brackets = true;
	  }
	}
	else if (c == ']') {
	  if (!started || !brackets) {
	    // Unexpected right bracket
	    is.setstate(std::ios_base::failbit);
	    finished = true;
	  }
	  else {
	    finished = true;
	  }
	  while (!is.eof() && (((c = static_cast<char>(is.peek())) == ' ')
        || (c == '\t') || (c == ','))) {
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
    v.set_size(0, false);
  }
  else {
    v.set(buffer.str());
}

return is;
}


wrapper::cVec fft(const wrapper::cVec & v,int FFTsize)
{
  if(0==FFTsize)
    FFTsize=v.size();
  
  wrapper::cVec output;
  output.armaVec=arma::fft(v.armaVec,FFTsize);
  return output;
}
wrapper::cVec  ifft(const wrapper::cVec & v,int FFTsize)
{
  
  if(0==FFTsize)
    FFTsize=v.size();
  
  wrapper::cVec output;
  output.armaVec=arma::ifft(v.armaVec,FFTsize);
  return output;
}
}//namespace wrapper

