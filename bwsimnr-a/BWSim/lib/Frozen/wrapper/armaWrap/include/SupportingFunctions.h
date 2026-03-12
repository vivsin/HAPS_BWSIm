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

#ifndef SFUNC_H
#define SFUNC_H

#include "mat.h"
#include "trignometric.h"
#include <string>
#include "hadamard.h"

namespace wrapper {
    
static unsigned int currentSimulationSeed;

int pow2i(int x);  ///< returns 2^x: 1<<x , for x  integer(non zero +)
double  pow2(double x);///< returns 2^x, x is double
double  pow10(double x);  ///<returns10^x x is double;

struct dB_{
  double operator()(const double x)
  {
    if(x>=0)
    return 10*log10(x);
    else
    {
      std::cout<<"[both:] Error dB(-ve): returning -nan"<<endl; abort(); return (0/0.0);
    }

  }
};

struct inv_dB_{
  double operator()(double x)
  {
    return pow(10,x/10.0);

  }
};
struct exp_{

  double operator()(double x)
  {
    return std::exp(x);
  }
  std::complex<double> operator()(const std::complex<double> & x)
  {
    return std::exp(x);
  }
};

struct pow_{
  double base;
  pow_(double Base=0):base(Base){}

  double operator()(double Base,double exponent)
  {
    return std::pow(Base,exponent);
  }
  double operator()(double exponent){return std::pow(base,exponent);}
  std::complex<double> operator()(std::complex<double> exponent)
  {
    return std::pow(base,exponent);
  }
};

template<class Num_T> Num_T sqr(Num_T t) {
    return(t*t);
};

struct pow2_{
  static pow_ pow2;
  double operator()(double exponent){return pow2(exponent);}
};
//pow_ pow2_::pow2=pow_(2);
struct pow10_{
  static pow_ pow10;
  double operator()(double exponent){return pow10(exponent);}
};
//pow_ pow10_::pow10=pow_(10);

struct CSQRT_{
  //returning complex sqrt;
  std::complex<double> operator()(double exponent){return std::sqrt(std::complex<double>(exponent,0.0));}
  std::complex<double> operator()(std::complex<double> exponent){return std::sqrt(exponent);}
};

struct log_{
  double operator()(double x)
  {
    if(x>=0)
    return std::log(x);
    else
    {
      std::cout<<"[both:] Error log(-ve): returning -nan"<<endl; return (0/0.0);
    }
  }
  std::complex<double> operator()(std::complex<double> x)
  {
    return std::log(x);
  }
};
struct log2_{
  //Note in std::library log2(x)=log(x)/log(2);
   double log2val;
  log2_():log2val(std::log(2)){}
  double operator()(double x)
  {
    return( std::log(x)/log2val );
  }
  std::complex<double> operator()(std::complex<double> x)
  {
    return( std::log(x)/log2val );
  }
};



struct logb_{
  //Note in std::library log2(x)=log(x)/log(2);
   double logbval;
  logb_(double b):logbval(std::log(b)){}

  double operator()(double b,double x)
  {
    return( std::log(x)/std::log(b) );
  }
  std::complex<double> operator()(double b,std::complex<double> x)
  {
    return( std::log(x)/std::log(b));
  }
  double operator()(double x)
  {
    return( std::log(x)/logbval );
  }
  std::complex<double> operator()(std::complex<double> x)
  {
    return( std::log(x)/logbval );
  }
};


struct rem_{

  double Y;
  rem_(double y=0.0):Y(y){}
  double operator()(const double x,const double y)
  {
    return std::fmod(x,y);
  }
  double operator()(const double x)
  {
    return std::fmod(x,Y);
  }

};




struct log10_{
  double operator()(double x)
  {
    if(x>=0)
    return std::log10(x);
    else
    {
      std::cout<<"[both:] Error log10(-ve): returning -nan"<<endl;abort(); return (0/0.0);
    }
  }
  std::complex<double> operator()(std::complex<double> x)
  {
    return std::log10(x);
  }
};
/*!
  \brief Convert a decimal int \a index to bvec using \a length bits in the representation
*/
wrapper::bVec dec2bin(int length, int index);

/*!
  \brief Convert a decimal int \a index to bvec. Value returned in \a temp.
*/
void dec2bin( int index,bvec &temp);

/*!
  \brief Convert a decimal int \a index to bvec with the first bit as MSB if \a msb_first == true
*/
bvec dec2bin(int index, bool msb_first=true);

/*!
  \brief Calculate the number of bits needed to represent an inteager n.
*/
int int2bits(int n);

/*!
  \brief Convert a bvec to decimal int with the first bit as MSB if \a msb_first == true
*/
int bin2dec( wrapper::bVec &inbvec, bool msb_first=true);

/*!
  \brief Convert ivec of octal form to bvec

  Converts from ivec containing {0,1,2,...,7} to bvec containing {0,1}.
  Removes zeros to the left if keepzeros = 0 (default).
  Example: oct2bin("3 5 5 1") returns {1 1 1 0 1 1 0 1 0 0 1}.
*/
bvec oct2bin(const ivec &octalindex, short keepzeros);

/*!
  \brief Convert bvec to octal ivec

  Converts from  bvec containing {0,1} to ivec containing {0,1,2,...,7}.
  Adds zeros to the left if inbits.length() is not a factor of 3.
  Example: bin2oct("1 1 1 0 1 1 0 1 0 0 1") returns {3 5 5 1}.
*/
ivec bin2oct(wrapper::bVec& inbits);

wrapper::iVec find( bVec input);
wrapper::iVec find( bMat input);
wrapper::dVec ones(int length);
wrapper::dMat ones(int r, int c);
wrapper::iVec ones_i(int length);
wrapper::bVec ones_b(int length);
wrapper::cVec ones_c(int length);
wrapper::iMat ones_i(int r, int c);
wrapper::bMat ones_b(int r, int c);
wrapper::cMat ones_c(int r, int c);
wrapper::dVec  zeros(int length);
wrapper::dMat  zeros(int r, int c);

wrapper::iVec  zeros_i(int length);
wrapper::bVec  zeros_b(int length);
wrapper::cVec  zeros_c(int length);
wrapper::iMat  zeros_i(int r, int c);
wrapper::bMat  zeros_b(int r, int c);
wrapper::cMat  zeros_c(int r, int c);


/* Base module ........functions */


double round(double x);
int round_i(double x);
int floor_i(double x);
int ceil_i(double x);
double sign(double x);
int sign_i(int x);
int sign_i(double x);
bool is_even(int x);
bool is_int(double x);



template<class Num_T> Num_T sqr_(Num_T t) {
    return(t*t);
}
template<class Num_T> Num_T abs_(Num_T a) {
    return std::abs(a);
}
template<class Num_T> Num_T sqrt_(Num_T a) {
    if(a>=0)return (Num_T)std::sqrt(a);
    std::cout<<"[both:] Error returning (1^31) as no sqrt of -ve number "<<a<<endl;
    return 1<<31;
}

double dB(double x);
double inv_dB(double x);
double logb(double b,double x);

typedef double(*dfpd)(double);

typedef int(*ifpd)(double);
typedef int(*ifpi)(int);
typedef double(*dfpc)(std::complex<double>);
typedef std::complex<double>(*cfpc)(std::complex<double>);


wrapper::dVec ceil( wrapper::dVec &x);

wrapper::dMat ceil( wrapper::dMat &x);

wrapper::dVec floor(wrapper::dVec &x);

wrapper::dMat floor( wrapper::dMat &x);
wrapper::iVec ceil_i( wrapper::dVec &x);
wrapper::iMat ceil_i( wrapper::dMat &x);

wrapper::iVec ceil_i( wrapper::dVec x);

wrapper::iVec floor_i(  dVec x);
wrapper::iMat floor_i( dMat &x);


wrapper::dVec round(wrapper::dVec &x);

wrapper::dVec round(wrapper::dVec x);
wrapper::dMat round( wrapper::dMat &x);
wrapper::iVec round_i( wrapper::dVec &x);
wrapper::iMat round_i( wrapper::dMat &x);

wrapper::dVec sqr( wrapper::dVec x);

wrapper::dMat sqr( wrapper::dMat &x);

wrapper::iVec sqr( wrapper::iVec &x);

wrapper::iMat sqr( wrapper::iMat &x);

wrapper::dVec abs( wrapper::dVec x);

wrapper::dMat abs( wrapper::dMat &x);
wrapper::iVec abs( wrapper::iVec &x);
wrapper::iMat abs( wrapper::iMat x);


wrapper::dVec sign( wrapper::dVec &x);

wrapper::dMat sign( wrapper::dMat &x);
wrapper::iVec sign_i( wrapper::iVec &x);

wrapper::iMat sign_i( wrapper::iMat &x);


wrapper::iVec sign_i( wrapper::dVec &x);
wrapper::iMat sign_i( wrapper::dMat &x) ;
wrapper::dVec sqrt( wrapper::dVec x);
wrapper::dMat sqrt( wrapper::dMat x) ;
wrapper::iVec sqrt( wrapper::iVec &x);
wrapper::iMat sqrt( wrapper::iMat &x);
wrapper::cVec csqrt( wrapper::cVec x);


wrapper::cMat csqrt( wrapper::cMat x) ;
wrapper::dVec dB(const dVec & x);

wrapper::dMat dB(const dMat & x);

wrapper::dVec inv_dB(const dVec & x);

wrapper::dMat inv_dB(const dMat & x);

wrapper::dVec exp(const dVec & x);

wrapper::dMat exp(const dMat & x);
wrapper::cVec exp(const cVec & x);

wrapper::cMat exp(const cMat & x);



wrapper::dVec pow(const double y,const dVec & x);

wrapper::dMat pow(const double y,const dMat & x);

wrapper::dVec pow(const dVec & x,const double y);

wrapper::dMat pow(const dMat & x,const double y);

double rem(double x,double y);
wrapper::dVec rem(const dVec & x,const double y);
wrapper::dMat rem(const dMat & x,const double y);


wrapper::dVec rem(const double y, const dVec & x);

wrapper::dMat rem(const double y, const dMat & x);
int mod(const int a,const int b);
double mod(const double x,const double y);


wrapper::dVec pow2(const dVec & x);

wrapper::dMat pow2(const dMat & x);
;
wrapper::dVec pow10(const dVec & x);

wrapper::dMat pow10(const dMat & x);

wrapper::dVec log(const dVec & x);

wrapper::dMat log(const dMat & x);
wrapper::cVec log(const cVec & x);

wrapper::cMat log(const cMat & x);


double log2( int  x);
// double log2( double  x);

wrapper::dVec log2(const dVec & x);

wrapper::dMat log2(const dMat & x);

wrapper::dVec log10(const dVec & x);

wrapper::dMat log10(const dMat & x);


wrapper::dVec logb(double b,const dVec & x);

wrapper::dMat logb(double b,const dMat & x);


/*Elementary Math Functions....*/


double sqr(std::complex<double> a);// this is same as norm

int sqr(int a);// this is same as norm

double norm(std::complex<double> a);// this is same as norm

double abs(std::complex<double> a);

double real(std::complex<double> a);

double imag(std::complex<double> a);
double arg(std::complex<double> a);

std::complex<double> polar(double r,double t);
std::complex<double> conj(std::complex<double> a);
wrapper::dVec abs( wrapper::cVec x);
wrapper::dMat abs( wrapper::cMat x);

wrapper::dVec sqr( wrapper::cVec &x) ;
wrapper::dMat sqr( wrapper::cMat &x) ;

double norm( wrapper::cVec x);
double norm( wrapper::cMat x);


wrapper::dVec real( wrapper::cVec x);
wrapper::dMat real( wrapper::cMat x) ;

wrapper::dVec imag( wrapper::cVec x);
wrapper::dMat imag( wrapper::cMat x) ;

wrapper::dVec arg( wrapper::cVec &x);
wrapper::dMat arg( wrapper::cMat &x);

wrapper::dVec angle( wrapper::cVec &x);
wrapper::dMat angle( wrapper::cMat &x);



wrapper::cVec conj( wrapper::cVec &x);
wrapper::cMat conj( wrapper::cMat &x) ;





//armadillo based functions
template<class Num_T> Num_T max(const wrapper::Vec<Num_T> &x) {
    return x.armaVec.max();
}
template<class Num_T> Vec<Num_T> max(const wrapper::Mat<Num_T> &x) {
    Vec<Num_T> rx;    
    rx.armaVec= x.armaMat.max();
    return rx;
}

template<class Num_T> Num_T min(const wrapper::Vec<Num_T> &x) {
    return x.armaVec.min();
}
template<class Num_T> Num_T min(const wrapper::Mat<Num_T> &x) {
    return x.armaMat.min();   
}

template<class Num_T> Num_T max(const wrapper::Vec<Num_T> &x,  int & index) {
    unsigned int id;
    Num_T max= x.armaVec.max(id);
    index=id;
    return max;
}
template<class Num_T> Num_T max(const wrapper::Mat<Num_T> &x, int & index) {
    unsigned int id;    
    Num_T max= x.armaMat.max(id);
    index=id;
    return max;
}




template<class Num_T> Num_T min(const wrapper::Vec<Num_T> &x, int &index) {
    return x.armaVec.min();
}
template<class Num_T> Num_T min(const wrapper::Mat<Num_T> &x, int &index) {
    return x.armaMat.min();   
}


template<class Num_T> int max_index(const wrapper::Vec<Num_T> &x) {
    arma::uword id;
    x.armaVec.max(id);
    return (int)id;
}


template<class Num_T> int min_index(const wrapper::Vec<Num_T> &x) {
    arma::uword id;
    x.armaVec.min(id);
    return (int)id;
}





//rand functions;
static bool rnd_initialized=false;
struct rand_i
{

    bool isRangeSet;
    int min;
    int max;
    int max_min_p1;
    rand_i(unsigned int seed=0):isRangeSet(false) {
        if(seed)
        {
            std::srand(seed);
            seedSet=seed;
        }
        else if(rnd_initialized==false)
        {
            seedSet=std::time(NULL);
            std::srand(seedSet);
            rnd_initialized=true;
        }
    }
    rand_i(int minv,int maxv, unsigned int seed=0):isRangeSet(true),min(minv),max(maxv),max_min_p1(maxv-minv+1) {

        if(seed)
        {
            std::srand(seed);
            seedSet=seed;
        }
        else if(rnd_initialized==false)
        {
            seedSet=std::time(NULL);
            std::srand(seedSet);
            rnd_initialized=true;
        }
    }
    unsigned int seedSet;

    unsigned int operator()() {
        if(isRangeSet)return(min+(std::rand()%(max_min_p1)));
        return std::rand();
    }
    unsigned int operator()(int low, int high) {
        return (low+(std::rand()%(high-low+1)));
    }
};

struct rand_u
{
    unsigned int seedSet;
    rand_u(unsigned int seed=0) {
        if(seed)
        {
            std::srand(seed);
            seedSet=seed;
        }
        else if(rnd_initialized==false)
        {
            seedSet=std::time(NULL);
            std::srand(seedSet);
            rnd_initialized=true;
        }
    }
    double operator()() {
        return((double) std::rand()/(double)RAND_MAX);
    }

};

static bool randn_state=false;
static  double previously_generated_randn(0.0);

struct rand_n
{
    unsigned int seedSet;
    double mu;
    double var;

    rand_n(unsigned int seed=0):seedSet(0),mu(0),var(1) {
        if(seed)
        {
            std::srand(seed);
            seedSet=seed;
        }
        else if(rnd_initialized==false)
        {
            seedSet=std::time(NULL);
            std::srand(seedSet);
            rnd_initialized=true;
        }
    }
    //Warning Fix: seeSet might be uninitialized
    rand_n(double meanv,double varv,unsigned int seed=0):seedSet(0),mu(meanv),var(varv) {
        if(seed)
        {
            std::srand(seed);
            seedSet=seed;
        }
        else if(rnd_initialized==false)
        {
	    seedSet=std::time(NULL);
            std::srand(seedSet);
            rnd_initialized=true;
        }
    }

    double operator()() {
        if(randn_state==true) 
        {
            randn_state =false;
            return previously_generated_randn;
        }
        else 
        {
            double A,B,R,G;
            A=((double)rand()/(double)RAND_MAX);
            B=(((double)rand()*2.0*3.14)/(double)RAND_MAX);
            if(A>=1.0) A=0.999999;
            R=std::sqrt(2.0*var*std::log(1.0/(1.0-A)));
            G=(R*std::cos(2.0*3.14*B))+mu;
            previously_generated_randn=(R*std::sin(2*3.14*B))+mu;
            randn_state =true;
            return G;
        }
    }

};


struct rand_nC
{
    unsigned int seedSet;
    double mu;
    double var;


    rand_nC(unsigned int seed=0):mu(0),var(1) {
        if(seed)
        {
            std::srand(seed);
            seedSet=seed;
        }
        else if(rnd_initialized==false)
        {
            seedSet=std::time(NULL);
            std::srand(seedSet);
            rnd_initialized=true;
        }
    }

    rand_nC(double meanv,double varv,unsigned int seed=0):mu(meanv),var(varv) {
        if(seed)
        {
            std::srand(seed);
            seedSet=seed;
        }
        else if(rnd_initialized==false)
        {
            seedSet=std::time(NULL);
            std::srand(seedSet);
            rnd_initialized=true;
        }
    }

    std::complex<double> operator()() {


        double A,B,R,G,F;
        A=((double)rand()/(double)RAND_MAX);
        B=(((double)rand()*2.0*3.14)/(double)RAND_MAX);
        if(A>=1.0) A=0.999999;
        R=std::sqrt(2.0*var*std::log(1.0/(1.0-A)));
        G=(R*std::cos(2.0*3.14*B))+mu;
        F=(R*std::sin(2*3.14*B))+mu;

        return std::complex<double>(F,G);

    }

};



template<class Num_T,class randGenF> struct rand_ {

    rand_(unsigned int seed=0)
    {
        if(seed)
        {
            std::srand(seed);
        }
    }

    Num_T operator()(randGenF b) {
        return( (Num_T)(b()));
    }
    Num_T operator()(randGenF b,unsigned int min, unsigned int max) {
        return( (Num_T)(b(min,max)));
    }
};

int randi();

int randi(int min,int max);

bool randb();

double randu();

double randn();

//double gaussian(double mu, double var);

std::complex<double> randn_c();
std::complex<double> gaussian_c(double mu,double var);


void randomizeSeed();
void setSeed(unsigned int seed=0);
void RNG_randomize();
unsigned int getSeed();

int randint(int randmax);




wrapper::iVec randi( int size, int low,int high);

wrapper::bVec randb( int size);

wrapper::dVec randn( int size);

wrapper::dVec randu( int size);

wrapper::cVec randn_c( int size);


const wrapper::iMat randi( int row,int col, int low,int high) ;

const  wrapper::bMat randb( int row,int col);

const wrapper::dMat randn( int row,int col);
const wrapper::dMat randu( int row,int col) ;

const wrapper::cMat randn_c( int row,int col);

class I_Uniform_RNG
{
  int Min;
  int Max;
  
public:
  void setup(const int min,const int max);
  I_Uniform_RNG(const int min=0,const int max=0);
  ivec operator()(int size);
  imat operator()(int rows,int cols);
  int sample();
};



template <class T> Mat<T> reshape(const Mat<T> &m, int r, int c)
{
  Mat<T> op=m;
  op.armaMat.reshape(r,c);
  return op;

}

template <class T> Mat<T> reshape(const Vec<T> &v, int r, int c)
{
  Mat<T> op(v.size(),1);
  op.set_col(0,v);
  op.armaMat.reshape(r,c);
  return op;
}


template <class T> int length(const Vec<T> &v)
{
  return v.armaVec.n_elem;
}

template <class T> T sum(const Vec<T> &v)
{
  return arma::sum(v.armaVec);
}

template <class T> Vec<T> sum(const Mat<T> &v, int dim=1)
{
  Vec<T> output;
  if(dim==1)
  {
    output.armaVec=arma::Col<T>(arma::sum(v.armaMat,0).st());
    return output;
  }
  else if(dim==2)
  {
    output.armaVec=arma::sum(v.armaMat,1);
    return output;
  } 
  else
    {cout<<"[both:] Invalid Dimension to sum()."<<endl;abort();}
}


const bMat reshape(const bMat &m, int r, int c);
const bMat reshape(const bVec &v, int r, int c);
int length(const bVec &v);
int sum(const bVec &v);
double sum_sqr(dVec v);
dVec sum_sqr(dMat m,int dim=1);

template<class T> Vec<T> reverse(Vec<T> a)
{
  Vec<T> op;op.armaVec=flipud(a.armaVec);
  return op;
}

 bVec reverse(bVec a);

mat eye(int n);
cmat eye_c(int n);
bvec cvectorize(bmat m);
bvec rvectorize(bmat m);

template<class T> Vec<T> cvectorize(Mat<T> m)
{
  Vec<T> op(m.rows()*m.cols());
  for(int c=0,n=0;c<m.cols();c++)
  {
    for(int r=0;r<m.rows();r++,n++)
    {
      op[n]=m(r,c);
    }
  }
  return op;

}
template<class T> Vec<T> rvectorize(Mat<T> m)
{
  Vec<T> op(m.rows()*m.cols());
  for(int r=0,n=0;r<m.rows();r++)
  {
    for(int c=0;c<m.cols();c++,n++)
    {
      op[n]=m(r,c);
    }
  }
  return op;

}


template<class T> double mean(Vec<T>  m)
{
  double a=mean(m.armaVec);  return a;
}
template<class T> T det(Mat<T> m)
{
 return arma::det(m.armaMat);
}

template<class T> double mean(Mat<T>  m)
{
  double a=mean(mean(m.armaMat));  return a;
}
template<class T> double sumsum(Mat<T>  m)
{
  double a=arma::accu(m.armaMat);  return a;
}

template<class T> double median(Vec<T>  m)
{
  return(median(m.armaVec));
}

template<class T> double median(Mat<T>  m)
{
  return(median(median(m.armaMat)));
}

 std::complex<double> sumsum(Mat< std::complex<double> >  m);

template<class T> Mat<T> transpose(Mat<T> m)
{
  return m.T();
}
template<class T> Mat<T> hermitian_transpose(Mat<T> m)
{
  return m.H();
}


template<class T> T trace(Mat<T> m)
{
  return trace( m.armaMat);
}


vec linspace(double st,double end,int steps=10);


template<class T> Mat<T> diag(Vec<T> & v)
{
  Mat<T> m(v.size(),v.size());m.zeros();
  m.armaMat.diag()=v.armaVec;
  return m;
}

template<class T> Vec<T> diag(Mat<T> & m)
{
  Vec<T> v1(m.rows());
  v1.armaVec=m.armaMat.diag();
  return v1;
}

template<class Num_T> Mat<Num_T> chol(const wrapper::Mat<Num_T> &x) {

 Mat<Num_T> y;
 y.armaMat=arma::chol(x.armaMat);
 return y;   
}

double mean(ivec m);

cmat sqrtm(mat & A);

cmat sqrtm(cmat & A);

std::string to_str(const double &i);

bool 	 svd (const mat &A, vec &s);
 	
bool 	 svd (const cmat &A, vec &s);
 	
 
vec 	 svd (const mat &A);
 	
 
vec 	 svd (const cmat &A);
 	
 
bool 	 svd (const mat &A, mat &U, vec &s, mat &V);
 	
 
bool 	 svd (const cmat &A, cmat &U, vec &s, cmat &V);


bool 	eig_sym (const mat &A, vec &d, mat &V); //Calculates the eigenvalues and eigenvectors of a symmetric real matrix.
bool 	eig_sym (const mat &A, vec &d); //Calculates the eigenvalues of a symmetric real matrix.
vec 	eig_sym (const mat &A); //Calculates the eigenvalues of a symmetric real matrix.
bool 	eig_sym (const cmat &A, vec &d, cmat &V); //Calculates the eigenvalues and eigenvectors of a hermitian complex matrix.
bool 	eig_sym (const cmat &A, vec &d); //Calculates the eigenvalues of a hermitian complex matrix.
vec 	eig_sym (const cmat &A); //Calculates the eigenvalues of a hermitian complex matrix.

bool 	eig (const mat &A, cvec &d, cmat &V); //Calculates the eigenvalues and eigenvectors of a real non-symmetric matrix.
bool 	eig (const mat &A, cvec &d); //Calculates the eigenvalues of a real non-symmetric matrix.
cvec 	eig (const mat &A); //Calculates the eigenvalues of a real non-symmetric matrix.
bool 	eig (const cmat &A, cvec &d, cmat &V); //Calculates the eigenvalues and eigenvectors of a complex non-hermitian matrix.
bool 	eig (const cmat &A, cvec &d); //Calculates the eigenvalues of a complex non-hermitian matrix.
cvec 	eig (const cmat &A); //Calculates the eigenvalues of a complex non-hermitian matrix.

cmat 	 kron (const cmat &A,const cmat &U);	
mat 	 kron (const mat &A,const mat &U);
    
template<class T> Mat<T> sort_mat(const Mat<T> &t , int sortType, int dim)
{
  Mat<T>s;
  s.armaMat=arma::sort(t.armaMat,sortType,dim-1);
  return s;  
}

 std::complex<double> erf(const std::complex<double> z);

//! Inverse of error function
 double erfinv(double x);

//! Q-function
 double Qfunc(double x);
 double erf(double x);
 double erfc(double x);


// ----------------------------------------------------------------------
// functions for matrices and vectors
// ----------------------------------------------------------------------

//! Error function
 wrapper::dVec erf(wrapper::dVec x);
 wrapper::dMat erf(wrapper::dMat x);
 wrapper::cVec erf(wrapper::cVec x);
 wrapper::cMat erf(wrapper::cMat x);
 
 // Inverse of error function
 wrapper::dVec erfinv(wrapper::dVec x);
 wrapper::dMat erfinv(wrapper::dMat x);
 
 // Complementary error function
 wrapper::dVec erfc(wrapper::dVec x);
 wrapper::dMat erfc(wrapper::dMat x);
 
 // Q-function
 wrapper::dVec Qfunc(wrapper::dVec x);
 wrapper::dMat Qfunc(wrapper::dMat x);
 
 inline int levels2bits(int n)
 {
     if(n < 0){cout<< "levels2bits(): Improper argument value"<<endl;abort();}
     return int2bits(--n);
 }
 double besselj(int n, double x);
 double fact(int N);

}//namespace wrapper
#endif
