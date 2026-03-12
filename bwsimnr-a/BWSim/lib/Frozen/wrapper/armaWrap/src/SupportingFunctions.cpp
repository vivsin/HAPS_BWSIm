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


#include "../include/SupportingFunctions.h"

namespace wrapper{


int pow2i(int x) {
    return ((x < 0) ? 0 : (1 << x));    ///< returns 2^x: 1<<x , for x  integer(non zero +)
}
double  pow2(double x) {
    return ( std::pow(2,x));    ///< returns 2^x, x is double
}
double  pow10(double x) {
    return ( std::pow(10,x));    ///<returns10^x x is double;
}

wrapper::bVec dec2bin(int length, int index)
{
    int i, bintemp = index;
    wrapper::bVec temp(length);
    for (i = length - 1; i >= 0; i--) {
        temp(i) = (bintemp & 1);
        bintemp = (bintemp >> 1);
    }
    return temp;
}
void dec2bin( int index,bvec &temp)
{
    int i, bintemp = index;
    unsigned length=sizeof(int)*8;
    temp.set_size(length);
    for (i = length - 1; i >= 0; i--) {
        temp(i) = (bintemp & 1);
        bintemp = (bintemp >> 1);
    }

}

bvec dec2bin(int index, bool msb_first)
{
  int length = int2bits(index);
  int i, bintemp = index;
  bvec temp(length);

  for (i = length - 1; i >= 0; i--) {
    temp(i) = (bintemp & 1);
    bintemp = (bintemp >> 1);
  }
  if (msb_first) {
    return temp;
  }
  else {
    return reverse(temp);
   }
}

int int2bits(int n)
{
  if (n == 0)
    return 1;

  int b = 0;
  while (n) {
    n >>= 1;
    ++b;
  }
  return b;
}


int bin2dec( wrapper::bVec &inbvec, bool msb_first)
{
    int i, temp = 0;
    int sizebvec = inbvec.length();
    if(sizebvec<=(int)sizeof(int)*8)
    {
        if (msb_first) {
            for (i = 0; i < sizebvec; i++) {
                const int v=inbvec[i];
                temp += wrapper::pow2i(sizebvec - i - 1) * v;
            }
        }
        else {
            for (i = 0; i < sizebvec; i++) {
	      temp += wrapper::pow2i(i) * int(inbvec(i));
            }
        }
        return temp;
    }
    else {
        std::cout<<"bin2dec, the values will overflow: length of bvec is greater than integer size "<<inbvec.size()<<endl;abort();
    }
}

bvec oct2bin(const ivec &octalindex, short keepzeros)
{
  int length = octalindex.length(), i;
  bvec out(3*length);
  for (i = 0; i < length; i++) {
    out.replace_mid(3*i, dec2bin(3, octalindex(i)));
  }
  //remove zeros if keepzeros = 0
  if (keepzeros == 0) {
    for (i = 0; i < out.length(); i++) {
      if ((short)out(i) != 0) {
    	return out.right(out.length() - i);
        break;
      }
    }
   return bvec("0");
  }

  else {
    return out;
  }
}

ivec bin2oct(wrapper::bVec &inbits)
{
  int start, Itterations = ceil_i(inbits.length() / 3.0);
  ivec out(Itterations);
  for (int i = Itterations - 1; i > 0; i--) {
    start = 3 * i - (3 * Itterations - inbits.length());
    bvec temp=inbits.mid(start, 3);
    out(i) = bin2dec(temp);
  }
  bvec temp1=inbits.left(inbits.length() - ((Itterations - 1) * 3));
  out(0) = bin2dec(temp1);
  return out;
}

wrapper::iVec find( bVec input)
{
    iVec temp(input.size());
    int pos = 0;
    for (int i = 0; i < input.size(); i++) {
        if (input[i]==1 ) {
            temp(pos) = i;//
            pos++;
        }
    }
    temp.set_size(pos, true);
    return temp;
}

wrapper::iVec find( bMat input)
{
  iVec temp(input.size());
  int pos = 0;
  for (int i = 0; i < input.size(); i++) {
    if (1==input(i) ) {
      temp(pos) = i;//
      pos++;
    }
  }
  temp.set_size(pos, true);
  return temp;
}

wrapper::dVec ones(int length) {
    dVec m;
    m.armaVec.ones((arma::uword)length);
    return m;
}
wrapper::dMat ones(int r, int c) {
    dMat m;
    m.armaMat.ones((arma::uword)r,(arma::uword)c);
    return m;
}

wrapper::iVec ones_i(int length) {
    iVec m;
    m.armaVec.ones((arma::uword)length);
    return m;
}
wrapper::bVec ones_b(int length) {
    bVec m;
    m.armaVec.ones((arma::uword)length);
    return m;;
}
wrapper::cVec ones_c(int length) {
    cVec m;
    m.armaVec.ones((arma::uword)length);
    return m;;
}

wrapper::iMat ones_i(int r, int c) {
    iMat m;
    m.armaMat.ones((arma::uword)r,(arma::uword)c);
    return m;
}
wrapper::bMat ones_b(int r, int c) {
    bMat m;
    m.armaMat.ones((arma::uword)r,(arma::uword)c);
    return m;
}
wrapper::cMat ones_c(int r, int c) {
    cMat m;
    m.armaMat.ones((arma::uword)r,(arma::uword)c);
    return m;
}

wrapper::dVec  zeros(int length) {
    dVec m;
    m.armaVec. zeros((arma::uword)length);
    return m;
}
wrapper::dMat  zeros(int r, int c) {
    dMat m;
    m.armaMat. zeros((arma::uword)r,(arma::uword)c);
    return m;
}

wrapper::iVec  zeros_i(int length) {
    iVec m;
    m.armaVec. zeros((arma::uword)length);
    return m;
}
wrapper::bVec  zeros_b(int length) {
    bVec m;
    m.armaVec. zeros((arma::uword)length);
    return m;;
}
wrapper::cVec  zeros_c(int length) {
    cVec m;
    m.armaVec. zeros((arma::uword)length);
    return m;;
}

wrapper::iMat  zeros_i(int r, int c) {
    iMat m;
    m.armaMat. zeros((arma::uword)r,(arma::uword)c);
    return m;
}
wrapper::bMat  zeros_b(int r, int c) {
    bMat m;
    m.armaMat. zeros((arma::uword)r,(arma::uword)c);
    return m;
}
wrapper::cMat  zeros_c(int r, int c) {
    cMat m;
    m.armaMat. zeros((arma::uword)r,(arma::uword)c);
    return m;
}









/* Base module ........functions */


double round(double x)
{
    return(std::floor(x+0.5));
}

int round_i(double x)
{
    return(std::floor(x+0.5));
}
int floor_i(double x)
{
    return((int)std::floor(x));
}
int ceil_i(double x)
{
    return((int)std::ceil(x));
}

double sign(double x)
{
    return( (x>0.0)?1.0:(x<0.0)?-1.0:0.0) ;
}


int sign_i(int x)
{
    return( (x>0)-(x<0.0)) ;
}

int sign_i(double x)
{
    return( (x>0.0)?1:(x<0.0)?-1:0) ;
}
bool is_even(int x) {
    return(!(x&1));
}
bool is_int(double x) {
    return((int)std::ceil(x)==(int)std::floor(x));
}

double dB(double x)
{
dB_ db;return db(x);
}
double inv_dB(double x)
{
inv_dB_ inv_db;return inv_db(x);
}
double logb(double b,double x)
{
logb_ db(b);return db(x);
}

wrapper::dVec ceil( wrapper::dVec &x) {
    return (unary_applyFunction<double,double,dfpd>(x,std::ceil));
}

wrapper::dMat ceil( wrapper::dMat &x)  {
    return (unary_applyFunction<double,double,dfpd>(x,std::ceil));
}

wrapper::dVec floor(wrapper::dVec &x) {
    return (unary_applyFunction<double,double,dfpd>(x,std::floor));
}

wrapper::dMat floor( wrapper::dMat &x) {
    return (unary_applyFunction<double,double,dfpd>(x,std::floor));
}
wrapper::iVec ceil_i( wrapper::dVec &x) {
    return (unary_applyFunction<double,int,dfpd>(x,std::ceil));
}
wrapper::iMat ceil_i( wrapper::dMat &x) {
    return (unary_applyFunction<double,int,dfpd>(x,std::ceil));
}

wrapper::iVec ceil_i( wrapper::dVec x) {
    return (unary_applyFunction<double,int,dfpd>(x,std::ceil));
}

wrapper::iVec floor_i(  dVec x) {
    return (unary_applyFunction<double,int,dfpd>(x,std::floor));
}
wrapper::iMat floor_i( dMat &x) {
    return (unary_applyFunction<double,int,dfpd>(x,std::floor));
}


wrapper::dVec round(wrapper::dVec &x) {
    return (unary_applyFunction<double,double,dfpd>(x,wrapper::round));
}

wrapper::dVec round(wrapper::dVec x) {
    return (unary_applyFunction<double,double,dfpd>(x,wrapper::round));
}
wrapper::dMat round( wrapper::dMat &x) {
    return (unary_applyFunction<double,double,dfpd>(x,wrapper::round));
}
wrapper::iVec round_i( wrapper::dVec &x) {
    return (unary_applyFunction<double,int,ifpd>(x,wrapper::round_i));
}
wrapper::iMat round_i( wrapper::dMat &x) {
    return (unary_applyFunction<double,int,ifpd>(x,wrapper::round_i));
}

wrapper::dVec sqr( wrapper::dVec x) {
    return (unary_applyFunction<double,double,dfpd>(x,wrapper::sqr_<double>));
}

wrapper::dMat sqr( wrapper::dMat &x)  {
    return (unary_applyFunction<double,double,dfpd>(x,wrapper::sqr_<double>));
}

wrapper::iVec sqr( wrapper::iVec &x) {
    return (unary_applyFunction<int,int,ifpi>(x,wrapper::sqr_<int>));
}

wrapper::iMat sqr( wrapper::iMat &x)  {
    return (unary_applyFunction<int,int,ifpi>(x,wrapper::sqr_<int>));
}

wrapper::dVec abs( wrapper::dVec x) {
    return (unary_applyFunction<double,double,dfpd>(x,wrapper::abs_<double>));
}

wrapper::dMat abs( wrapper::dMat &x)  {
    return (unary_applyFunction<double,double,dfpd>(x,wrapper::abs_<double>));
}

wrapper::iVec abs( wrapper::iVec &x) {
    return (unary_applyFunction<int,int,ifpi>(x,wrapper::abs_<int>));
}

wrapper::iMat abs( wrapper::iMat x)  {
    return (unary_applyFunction<int,int,ifpi>(x,wrapper::abs_<int>));
}


wrapper::dVec sign( wrapper::dVec &x) {
    return (unary_applyFunction<double,double,dfpd>(x,wrapper::sign));
}

wrapper::dMat sign( wrapper::dMat &x)  {
    return (unary_applyFunction<double,double,dfpd>(x,wrapper::sign));
}

wrapper::iVec sign_i( wrapper::iVec &x) {
    return (unary_applyFunction<int,int,ifpi>(x,wrapper::sign_i));
}

wrapper::iMat sign_i( wrapper::iMat &x)  {
    return (unary_applyFunction<int,int,ifpi>(x,wrapper::sign_i));
}


wrapper::iVec sign_i( wrapper::dVec &x) {
    return (unary_applyFunction<double,int,ifpd>(x,wrapper::sign_i));
}
wrapper::iMat sign_i( wrapper::dMat &x)  {
    return (unary_applyFunction<double,int,ifpd>(x,wrapper::sign_i));
}

wrapper::dVec sqrt( wrapper::dVec x) {
    return (unary_applyFunction<double,double,dfpd>(x,std::sqrt));
}

wrapper::cVec csqrt( wrapper::cVec x) {
   cVec o(x.size());
  CSQRT_ csqrt;
    unary_Function(x.begin(),x.end(),o.begin(),csqrt);
    return o;

}

wrapper::dMat sqrt( wrapper::dMat x)  {
    return (unary_applyFunction<double,double,dfpd>(x,std::sqrt));
}

wrapper::cMat csqrt( wrapper::cMat x)  {
  cMat o(x.rows(),x.cols());
  CSQRT_ csqrt;
    unary_Function(x.begin(),x.end(),o.begin(),csqrt);
    return o;

}

wrapper::iVec sqrt( wrapper::iVec &x) {
    return (unary_applyFunction<int,int,ifpi>(x,wrapper::sqrt_<int>));
}

wrapper::iMat sqrt( wrapper::iMat &x)  {
    return (unary_applyFunction<int,int,ifpi>(x,wrapper::sqrt_<int>));
}

wrapper::dVec dB(const dVec & x)
{
  dVec op(x.size());
  dB_ db;
  unary_Function(x.begin(),x.end(),op.begin(),db);
  return op;
}

wrapper::dMat dB(const dMat & x)
{
  dMat op(x.rows(),x.cols());
  dB_ db;
  unary_Function(x.begin(),x.end(),op.begin(),db);
  return op;
}

wrapper::dVec inv_dB(const dVec & x)
{
  dVec op(x.size());
  inv_dB_ inv_db;
  unary_Function(x.begin(),x.end(),op.begin(),inv_db);
  return op;
}

wrapper::dMat inv_dB(const dMat & x)
{
  dMat op(x.rows(),x.cols());
  inv_dB_ inv_db;
  unary_Function(x.begin(),x.end(),op.begin(),inv_db);
  return op;
}

wrapper::dVec exp(const dVec & x)
{
  dVec op(x.size());
  exp_ expF;
  unary_Function(x.begin(),x.end(),op.begin(),expF);
  return op;
}

wrapper::dMat exp(const dMat & x)
{
  dMat op(x.rows(),x.cols());
  exp_ expF;
  unary_Function(x.begin(),x.end(),op.begin(),expF);
  return op;
}

wrapper::cVec exp(const cVec & x)
{
  cVec op(x.size());
  exp_ expF;
  unary_Function(x.begin(),x.end(),op.begin(),expF);
  return op;
}

wrapper::cMat exp(const cMat & x)
{
  cMat op(x.rows(),x.cols());
  exp_ expF;
  unary_Function(x.begin(),x.end(),op.begin(),expF);
  return op;
}



wrapper::dVec pow(const double y,const dVec & x)
{//y^x
  dVec op(x.size());
  pow_ powF(y);
  unary_Function(x.begin(),x.end(),op.begin(),powF);
  return op;
}

wrapper::dMat pow(const double y,const dMat & x)
{
  dMat op(x.rows(),x.cols());
  pow_ powF(y);
  unary_Function(x.begin(),x.end(),op.begin(),powF);
  return op;
}


wrapper::dVec pow(const dVec & x,const double y)
{
  dVec op(x.size());
  pow_ powF;
  binary_Function_scalar(x.begin(),x.end(),y,op.begin(),powF);
  return op;
}

wrapper::dMat pow(const dMat & x,const double y)
{
  dMat op(x.rows(),x.cols());
  pow_ powF;
  binary_Function_scalar(x.begin(),x.end(),y,op.begin(),powF);
  return op;
}


double rem(double x,double y)
{
  return std::fmod(x,y);
}

wrapper::dVec rem(const dVec & x,const double y)
{//y^x
  dVec op(x.size());
  rem_ remF(y);
  unary_Function(x.begin(),x.end(),op.begin(),remF);
  return op;
}

wrapper::dMat rem(const dMat & x,const double y)
{
  dMat op(x.rows(),x.cols());
  rem_ remF(y);
  unary_Function(x.begin(),x.end(),op.begin(),remF);
  return op;
}


wrapper::dVec rem(const double y, const dVec & x)
{
  dVec op(x.size());
  rem_ remF;
  scalar_binary_Function(y,x.begin(),x.end(),op.begin(),remF);
  return op;
}

wrapper::dMat rem(const double y, const dMat & x)
{
  dMat op(x.rows(),x.cols());
  rem_ remF;
  scalar_binary_Function(y,x.begin(),x.end(),op.begin(),remF);
  return op;
}

int mod(const int a,const int b)
{
  return a%b;
}

double mod(const double x,const double y)
{
  return std::fmod(x,y);
}


wrapper::dVec pow2(const dVec & x)
{
  dVec op(x.size());
  pow2_ pow2F;
  unary_Function(x.begin(),x.end(),op.begin(),pow2F);
  return op;
}

wrapper::dMat pow2(const dMat & x)
{
  dMat op(x.rows(),x.cols());
  pow2_ pow2F;
  unary_Function(x.begin(),x.end(),op.begin(),pow2F);
  return op;
}



wrapper::dVec pow10(const dVec & x)
{
  dVec op(x.size());
  pow10_ pow10F;
  unary_Function(x.begin(),x.end(),op.begin(),pow10F);
  return op;
}

wrapper::dMat pow10(const dMat & x)
{
  dMat op(x.rows(),x.cols());
  pow10_ pow10F;
  unary_Function(x.begin(),x.end(),op.begin(),pow10F);
  return op;
}

wrapper::dVec log(const dVec & x)
{
  dVec op(x.size());
  log_ logF;
  unary_Function(x.begin(),x.end(),op.begin(),logF);
  return op;
}

wrapper::dMat log(const dMat & x)
{
  dMat op(x.rows(),x.cols());
  log_ logF;
  unary_Function(x.begin(),x.end(),op.begin(),logF);
  return op;
}

wrapper::cVec log(const cVec & x)
{
  cVec op(x.size());
  log_ logF;
  unary_Function(x.begin(),x.end(),op.begin(),logF);
  return op;
}

wrapper::cMat log(const cMat & x)
{
  cMat op(x.rows(),x.cols());
  log_ logF;
  unary_Function(x.begin(),x.end(),op.begin(),logF);
  return op;
}


double log2( int  x)
{
  log2_ log2F;
  return log2F(double(x));
}

wrapper::dVec log2(const dVec & x)
{
  dVec op(x.size());
  log2_ log2F;
  unary_Function(x.begin(),x.end(),op.begin(),log2F);
  return op;
}

wrapper::dMat log2(const dMat & x)
{
  dMat op(x.rows(),x.cols());
  log2_ log2F;
  unary_Function(x.begin(),x.end(),op.begin(),log2F);
  return op;
}

wrapper::dVec log10(const dVec & x)
{
  dVec op(x.size());
  log10_ log10F;
  unary_Function(x.begin(),x.end(),op.begin(),log10F);
  return op;
}

wrapper::dMat log10(const dMat & x)
{
  dMat op(x.rows(),x.cols());
  log10_ log10F;
  unary_Function(x.begin(),x.end(),op.begin(),log10F);
  return op;
}


wrapper::dVec logb(double b,const dVec & x)
{
  dVec op(x.size());
  logb_ logbF(b);
  unary_Function(x.begin(),x.end(),op.begin(),logbF);
  return op;
}

wrapper::dMat logb(double b,const dMat & x)
{
  dMat op(x.rows(),x.cols());
  logb_ logbF(b);
  unary_Function(x.begin(),x.end(),op.begin(),logbF);
  return op;
}



/*Elementary Math Functions....*/


double sqr(std::complex<double> a)// this is same as norm
{
    return (real(a)*real(a)+imag(a)*imag(a));
}
int sqr(int a)// this is same as norm
{
    return (a*a);
}

double norm(std::complex<double> a)// this is same as norm
{
    return (std::norm(a));
}
double abs(std::complex<double> a)
{
    //return std::sqrt(real(a)*real(a)+imag(a)*imag(a));
    return std::abs(a);
}
double real(std::complex<double> a)
{
    return (double)std::real(a);
}
double imag(std::complex<double> a)
{
    return (double)std::imag(a);
}
double arg(std::complex<double> a)
{
    return (double)std::arg(a);
}

std::complex<double> polar(double r,double t)
{
    return (std::complex<double>)std::polar(r,t);
}

std::complex<double> conj(std::complex<double> a)
{
    return std::conj(a);
}
wrapper::dVec abs( wrapper::cVec x) {
    return (unary_applyFunction< std::complex<double>,double,dfpc>(x,wrapper::abs));
}
wrapper::dMat abs( wrapper::cMat x) {
    return (unary_applyFunction< std::complex<double>,double,dfpc>(x,wrapper::abs));
}

wrapper::dVec sqr( wrapper::cVec &x) {
    return (unary_applyFunction< std::complex<double>,double,dfpc>(x,wrapper::sqr));
}
wrapper::dMat sqr( wrapper::cMat &x) {
    return (unary_applyFunction< std::complex<double>,double,dfpc>(x,wrapper::sqr));
}

double norm( wrapper::cVec x ) {
    return arma::norm(x.armaVec,2);
}
double norm( wrapper::cMat x) {
    return arma::norm(x.armaMat,2);
}


wrapper::dVec real( wrapper::cVec x) {
    return (unary_applyFunction< std::complex<double>,double,dfpc >(x,wrapper::real));
}
wrapper::dMat real( wrapper::cMat x) {
    return (unary_applyFunction< std::complex<double>,double,dfpc >(x,wrapper::real));
}

wrapper::dVec imag( wrapper::cVec x) {
    return (unary_applyFunction< std::complex<double>,double,dfpc >(x,wrapper::imag));
}
wrapper::dMat imag( wrapper::cMat x) {
    return (unary_applyFunction< std::complex<double>,double,dfpc >(x,wrapper::imag));
}

wrapper::dVec arg( wrapper::cVec &x) {
    return (unary_applyFunction< std::complex<double>,double,dfpc >(x,wrapper::arg));
}
wrapper::dMat arg( wrapper::cMat &x) {
    return (unary_applyFunction< std::complex<double>,double,dfpc >(x,wrapper::arg));
}

wrapper::dVec angle( wrapper::cVec &x) {
    return (unary_applyFunction< std::complex<double>,double,dfpc >(x,wrapper::arg));
}
wrapper::dMat angle( wrapper::cMat &x) {
    return (unary_applyFunction< std::complex<double>,double,dfpc >(x,wrapper::arg));
}



wrapper::cVec conj( wrapper::cVec &x) {
    return (unary_applyFunction< std::complex<double>,std::complex<double>,cfpc >(x,wrapper::conj));
}
wrapper::cMat conj( wrapper::cMat &x) {
    return (unary_applyFunction< std::complex<double>,std::complex<double>,cfpc >(x,wrapper::conj));
}










int randi()
{
    rand_i b;
    rand_<int,rand_i> a;
    return a(b);
}

int randi(int min,int max)
{
    rand_i b(min,max);
    rand_<int,rand_i> a;
    return a(b);
}

bool randb()
{
    rand_i b(0,1);
    rand_<int,rand_i> a;
    return a(b);
}

double randu()
{
    rand_u b;
    rand_<double,rand_u> a;
    return a(b);
}

double randn()
{
    rand_n b(0,1);
    rand_<double,rand_n> a;
    return a(b);
}

// double gaussian(double mu,double var)
// {
//     rand_n b(mu,var);
//     rand_<double,rand_n> a;
//     return a(b);
// }

std::complex<double> randn_c()
{
    rand_n b(0,.5);
    rand_<double,rand_n> a;
    return std::complex<double>(a(b),a(b));
}
std::complex<double> gaussian_c(double mu,double var)
{
    rand_n b(mu,var/2.0);
    rand_<double,rand_n> a;
    return std::complex<double>(a(b),a(b));
}

void randomizeSeed()
{
    std::srand(std::time(NULL));
}


void setSeed(unsigned int seed)
{
    static bool isSet = false;
    if(!isSet)
    {
        currentSimulationSeed = seed;
        isSet = true;
    }
    std::srand(std::time(NULL));
    if(seed)
    {
        std::srand(seed);
    }
    
    randn_state=false;
    rnd_initialized=true;
}

void RNG_randomize()
{
    std::srand(std::time(NULL));
}


unsigned int getSeed()
{
    return currentSimulationSeed;
}


int randint(int randmax)//return
{

    return (std::rand()%randmax);
}



wrapper::iVec randi( int size, int low,int high) {
    rand_i b(low,high);
    rand_<int,rand_i> a;
    unsigned int usize=size;
    return (unary_genFunction<int,rand_<int,rand_i>,rand_i>(usize,a,b));

}

wrapper::bVec randb( int size) {
    rand_i b(0,1);
    rand_<int,rand_i> a;
    unsigned int usize=size;
    iVec c = unary_genFunction<int,rand_<int,rand_i>,rand_i>(usize,a,b);
    bVec d = to_bvec<int>(c);
    return(d);

}

wrapper::dVec randn( int size) {
    rand_n b(0,1);
    rand_<double,rand_n> a;
    unsigned int usize=size;
    return (unary_genFunction<double,rand_<double,rand_n>,rand_n>(usize,a,b));

}

wrapper::dVec randu( int size) {
    rand_u b;
    rand_<double,rand_u> a;
    unsigned int usize=size;
    return (unary_genFunction<double,rand_<double,rand_u>,rand_u>(usize,a,b));

}

wrapper::cVec randn_c( int size) {
    rand_nC b(0,0.5);
    rand_<std::complex<double>,rand_nC> a;
    unsigned int usize=size;
    return (unary_genFunction<std::complex<double>,rand_<std::complex<double>,rand_nC>,rand_nC>(usize,a,b));
}



const wrapper::iMat randi( int row,int col, int low,int high) {
    rand_i b(low,high);
    rand_<int,rand_i> a;

    return (unary_genFunction<int,rand_<int,rand_i>,rand_i>( row, col,a,b));

}

const  wrapper::bMat randb( int row,int col) {
    rand_i b(0,1);
    rand_<int,rand_i> a;
    iMat c = unary_genFunction<int,rand_<int,rand_i>,rand_i>( row, col,a,b);
    bMat d = to_bmat<int>(c);
    return(d);

}

const wrapper::dMat randn( int row,int col) {
    rand_n b(0,1);
    rand_<double,rand_n> a;
    return (unary_genFunction<double,rand_<double,rand_n>,rand_n>( row, col,a,b));

}

const wrapper::dMat randu( int row,int col) {
    rand_u b;
    rand_<double,rand_u> a;
    return (unary_genFunction<double,rand_<double,rand_u>,rand_u>( row, col,a,b));

}

const wrapper::cMat randn_c( int row,int col) {
    rand_nC b(0,0.5);
    rand_<std::complex<double>,rand_nC> a;
    return (unary_genFunction<std::complex<double>,rand_<std::complex<double>,rand_nC>,rand_nC>( row, col,a,b));
}

void I_Uniform_RNG::setup(const int min,const int max)
{
  Max=max;Min=min;
}
I_Uniform_RNG::I_Uniform_RNG(const int min,const int max):Min(min),Max(max)
{
}
ivec I_Uniform_RNG::operator()(int size){return randi(size,Min,Max);}
imat I_Uniform_RNG::operator()(int rows,int cols){return randi(rows,cols,Min,Max);}
int I_Uniform_RNG::sample(){return randi(Min,Max);}

const wrapper::bMat reshape(const wrapper::bMat &m, int r, int c){
  bMat op=m;
  op.armaMat.reshape(r,c);
  return op;
}

const wrapper::bMat reshape(const wrapper::bVec &v, int r, int c){
  bMat op(v.size(),1);
  op.set_col(0,v);
  op.armaMat.reshape(r,c);
  return op;
}

 int length(const bVec &v)
{
  return v.armaVec.n_elem;
}

 int sum(const bVec &v)
{
  return arma::sum(v.armaVec);
}
double sum_sqr(dVec v)
{
  return sum(wrapper::sqr(v));
}

dVec sum_sqr(dMat m,int dim)
{
    dVec out;
    if (dim == 1) {
        out.set_size(m.cols(), false);
        
        for (int i = 0; i < m.cols(); i++)
            out(i) = sum_sqr(m.get_col(i));
    }
    else {
        out.set_size(m.rows(), false);
        
        for (int i = 0; i < m.rows(); i++)
            out(i) = sum_sqr(m.get_row(i));
    }
    return out;
}
 bVec reverse(bVec a)
{
  bVec op;op.armaVec=flipud(a.armaVec);
  return op;
}

mat eye(int n)
{
  mat op;op.armaMat.eye(n,n);
  return op;
}
cmat eye_c(int n)
{
  return to_cmat(wrapper::eye(n));
}
  std::complex<double>  sumsum(Mat< std::complex<double> >  m)
{
   std::complex<double>  a=0;for(int i=0;i<(int)m.armaMat.n_elem;i++)a+=m.armaMat[i];  return a;
}


vec linspace(double st,double end,int steps)
{
  vec A;
  A.armaVec=arma::linspace(st,end,steps);
  return A;

}



double mean(ivec m)
{
  return(mean(to_vec(m)));
}

cmat sqrtm(mat & A)
{

    cmat RightEigVec;
    cvec EigVal;
    bool correct=arma::eig_gen(EigVal.armaVec,RightEigVec.armaMat,A.armaMat);
    if(!correct)
    {
      std::cout<<"Eigen Value decomposition failed........."<<endl;
    }
    cvec sqrtEigVal=csqrt(EigVal);
    cmat sqrtEigMat=diag(sqrtEigVal);
    cmat B;
    B=RightEigVec*sqrtEigMat*inv(RightEigVec);

    return B;
}



cmat sqrtm(cmat & A)
{
    cmat RightEigVec;
    cvec EigVal;
    arma::eig_gen(EigVal.armaVec,RightEigVec.armaMat,A.armaMat);
    cvec sqrtEigVal=csqrt(EigVal);
    cmat sqrtEigMat=diag(sqrtEigVal);
    cmat B;
    B=RightEigVec*sqrtEigMat*inv(RightEigVec);
    return B;
}

std::string to_str(const double &i)
{
  std::ostringstream sstr;
  sstr<<i;
  return sstr.str();
}

bool 	 svd (const mat &A, vec &s){
  return svd(s.armaVec,A.armaMat);  
}
 	
bool 	 svd (const cmat &A, vec &s){
  return svd(s.armaVec,A.armaMat);  
}
 	
vec 	 svd (const mat &A){
 vec s; s.armaVec=svd(A.armaMat); return s;
}
 
vec 	 svd (const cmat &A){
 vec s; s.armaVec=svd(A.armaMat); return s;
}
 
bool 	 svd (const mat &A, mat &U, vec &s, mat &V){
  return svd(U.armaMat,s.armaVec,V.armaMat,A.armaMat);
}
 	
bool 	 svd (const cmat &A, cmat &U, vec &s, cmat &V){
 return svd(U.armaMat,s.armaVec,V.armaMat,A.armaMat);
}


//eig_sym

bool 	eig_sym (const mat &A, vec &d, mat &V){
  return eig_sym(d.armaVec,V.armaMat,A.armaMat);
}

bool 	eig_sym (const mat &A, vec &d){
  return eig_sym(d.armaVec,A.armaMat);
}

vec 	eig_sym (const mat &A){
  vec d; 
  d.armaVec = eig_sym(A.armaMat);
  return d;
}

bool 	eig_sym (const cmat &A, vec &d, cmat &V){
  return eig_sym(d.armaVec,V.armaMat,A.armaMat);
}

bool 	eig_sym (const cmat &A, vec &d){
  return eig_sym(d.armaVec,A.armaMat);
}

vec 	eig_sym (const cmat &A){
  vec d; 
  d.armaVec = eig_sym(A.armaMat);
  return d;
}

//eig_gen

bool 	 eig (const mat &A, cvec &d, cmat &V){
  return eig_gen(d.armaVec,V.armaMat,A.armaMat);
}

bool 	eig (const mat &A, cvec &d){
  return eig_gen(d.armaVec,A.armaMat);
}

cvec 	eig (const mat &A){
  cvec d; 
  d.armaVec = eig_gen(A.armaMat);
  return d;
}

bool 	 eig (const cmat &A, cvec &d, cmat &V){
  return eig_gen(d.armaVec,V.armaMat,A.armaMat);
}

bool 	eig (const cmat &A, cvec &d){
  return eig_gen(d.armaVec,A.armaMat);
}

cvec 	eig (const cmat &A){
  cvec d; 
  d.armaVec = eig_gen(A.armaMat);
  return d;
}

cmat 	 kron (const cmat &A,const cmat &U){

    cmat output;
    output.armaMat=arma::kron(A.armaMat,U.armaMat);
    return output;
}

mat 	 kron (const mat &A,const mat &U){
    
    mat output;
    output.armaMat=arma::kron(A.armaMat,U.armaMat);
    return output;
}


bvec cvectorize(bmat m)
{
  bvec op(m.rows()*m.cols());
  for(int c=0,n=0;c<m.cols();c++)
  {
    for(int r=0;r<m.rows();r++,n++)
    {
      op[n]=m(r,c);
    }
  }
  return op;
  
}
bvec rvectorize(bmat m)
{
  bvec op(m.rows()*m.cols());
  for(int r=0,n=0;r<m.rows();r++)
  {
    for(int c=0;c<m.cols();c++,n++)
    {
      op[n]=m(r,c);
    }
  }
  return op;
  
}

double fact(int N)
{
    if(N<0)
    {   return std::pow(-1.0,std::abs(N))*fact(std::abs(N));}
    else if(N>170)
    {cout<<"input acceptable by factorial function is <=170"<<endl;abort();}
    else
    {
        double fact=1;
        for(int cnt=1;cnt<=N;cnt++)
        fact=fact*cnt;    
        return(fact);
    }
}

//NOTE: This function has been tested with matlab for n=1 and x from [-200pi,200pi] and it is not tested for other cases. 
double besselj(int n, double x)
{   
    double val = 0;  
    if(x==0)
    {
        return (n==0) ? 1.0 : 0.0;
    }
    if(x<0 || n<0)
        val =  std::pow(-1,std::abs(n))*besselj(std::abs(n),std::abs(x));
    else
    {
        if(x<=0.1*std::sqrt(2*(n+1)))
        {
            val = std::pow(x/2,n)/fact(n);
        }
        else if(x<45*std::abs(sqr(n)-0.25))
        {
            double t0 = 1.0/fact(n);
            double sum = t0,R,t1;
            int k=1;
            for(int i=0;i<100;i++)
            {
                R = -(x*x/4)/(k*(n+k));
                t1 = R*t0;
                sum += t1;
                t0 = t1;
                k++;
            }
            sum *= std::pow(x/2,n);
            val = sum;
        }
        else
        {
            double arg = x - (2*n+1)*pi/4;
            double amp = std::sqrt(2.0/(pi*x));
            val = amp*std::cos(arg);
        }
    }
    if(std::abs(val)>1)
    {
            cout<<x<<" "<<val<<endl;abort();
    }
    return val;
}


#ifndef POW2_10_FUNCTION
#define POW2_10_FUNCTION
pow_ pow2_::pow2=pow_(2);
pow_ pow10_::pow10=pow_(10);
#endif //POW2_10_FUNCTION


}//namespace
