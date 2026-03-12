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

#ifndef ARRAY_H
#define ARRAY_H

#include "algo.h"
#include "vec.h"

#include <armadillo>
namespace wrapper{

  using  arma::span;
template<class T> class Array;
template<class T> const Array<T> concat(const Array<T> &a, const T &e);
template<class T> const Array<T> concat(const T &e, const Array<T> &a);
template<class T> const Array<T> concat(const Array<T> &a1,const Array<T> &a2);

template<class T>class Array
{
  

public:
  arma::field<T> armaArray;
  Array():armaArray((arma::uword)0){}
  Array(int n):armaArray((arma::uword)n){}
  Array(const Array<T> &a):armaArray(a.armaArray){}
  Array(const std::string& values):armaArray(values){}
  Array(const char* values){std::istringstream s(values); s>>*this;}
  Array(const std::vector<char>& x):armaArray(x){};
    const Array& operator=(const std::vector<char>& x){armaArray=x;return *this;};
    
    #if defined(ARMA_USE_CXX11)
    Array(const std::initializer_list<char>& list):armaArray(list){};
    const Array& operator=(const std::initializer_list<char>& list){armaArray=list;return *this;};
    #endif
  
  T &operator()(int i){return(armaArray((arma::uword)i));}
  const T &operator()(int i) const{return(armaArray((arma::uword)i));}
  const Array<T> operator()(int i1, int i2) const{Array<T> toReturn; toReturn.armaArray=armaArray.subfield(span((arma::uword)i1,(arma::uword)i2),span::all);return(toReturn);}
  const Array<T> operator()(const Array<int> &indices) const{Array<T> toReturn(indices.length()); for(arma::uword cnt=0;cnt<indices.armaArray.n_elem;cnt++) toReturn.armaArray(cnt)=armaArray((arma::uword)indices(cnt));return(toReturn);}
  const Array<T> operator()(const Vec<int> &indices) const{Array<T> toReturn(indices.length()); for(arma::uword cnt=0;cnt<indices.armaVec.n_elem;cnt++) toReturn.armaArray(cnt)=armaArray((arma::uword)indices(cnt));return(toReturn);}
  Array<T> left(int n) const{Array<T> toReturn; if(n>0){ toReturn.armaArray=armaArray.subfield(span(0,(arma::uword)(n-1)),span::all);} return(toReturn);}
  Array<T> right(int n) const{Array<T> toReturn;if(n>0){ toReturn.armaArray=armaArray.subfield(span((arma::uword)(armaArray.n_elem-n),armaArray.n_elem-1),span::all);}return(toReturn);}
  Array<T> mid(int pos, int n) const{Array<T> toReturn;if(n>0){ toReturn.armaArray=armaArray.subfield(span((arma::uword)pos,(arma::uword)(pos-n+1)),span::all);} return(toReturn);}
  Array<T>& operator=(const T &e){for(arma::uword cnt=0;cnt<armaArray.n_elem;cnt++) armaArray(cnt)=e;return *this;}
  Array<T>& operator=(const Array<T> &a){armaArray=a.armaArray; return *this;}
  Array<T>& operator=(const char* values){std::istringstream s(values); s>>*this; return *this;}
  int size() const { return (int)armaArray.n_elem; }
  int length() const { return (int)armaArray.n_elem; }
  void set_size(int n, bool copy = false){
    if(copy && armaArray.n_elem>0)
  {
    arma::field<T> temp=armaArray;
    armaArray.set_size(n);if((unsigned int)n>=temp.n_elem){
      armaArray.subfield(span(0,temp.n_elem-1),span::all)=temp;
    }
    else
    {
      armaArray=temp.subfield(span(0,(arma::uword)n-1),span::all);
    }
  }
    else 
      armaArray.set_size(n);
    
  }
  void set_length(int n, bool copy = false) { set_size(n, copy); }
  T shift_right(const T& e)
  {
    T a=armaArray(armaArray.n_elem-1);
    Array<T> x; x.armaArray=armaArray.subfield(span(0,armaArray.n_elem-2),span::all);
    armaArray.subfield(span(1,armaArray.n_elem-1),span::all)=x.armaArray; 
    armaArray(0)=e;
    return(a);
  }
  const Array<T> shift_right(const Array<T> &a)
  {
    Array<T> a1=right(a.size()); 
    Array<T> x; x.armaArray=armaArray.subfield(span(0,armaArray.n_elem-a.armaArray.n_elem-1),span::all);
    armaArray.subfield(span(a.armaArray.n_elem,armaArray.n_elem-1),span::all)=x.armaArray; 
    armaArray.subfield(span(0,a.armaArray.n_elem-1),span::all)=a.armaArray;
    return(a1);
  }
  T shift_left(const T& e)
  {
    T a=armaArray(0);
    Array<T> x; x.armaArray=armaArray.subfield(span(1,armaArray.n_elem-1),span::all);
    armaArray.subfield(span(0,armaArray.n_elem-2),span::all)=x.armaArray; 
    armaArray(armaArray.n_elem-1)=e;
    return(a);
  }
  const Array<T> shift_left(const Array<T> &a)
  {
    Array<T> a1=left(a.size()); 
    Array<T> x; x.armaArray=armaArray.subfield(span(a.armaArray.n_elem,armaArray.n_elem-1),span::all);
    armaArray.subfield(span(0,armaArray.n_elem-a.armaArray.n_elem-1),span::all)=x.armaArray; 
    armaArray.subfield(span(armaArray.n_elem-a.armaArray.n_elem,armaArray.n_elem-1),span::all)=a.armaArray;
    return(a1);
  }
  void swap(int i, int j){T a=armaArray((arma::uword)j);armaArray((arma::uword)j)=armaArray((arma::uword)i);armaArray((arma::uword)i)=a;}
  void set_subarray(int i1, int i2, const Array<T> &a){armaArray.subfield(span((arma::uword)i1,(arma::uword)i2),span::all)=a.armaArray;}
  void set_subarray(int i1, int i2, const T &t){for(arma::uword cnt=(arma::uword)i1;cnt<=(arma::uword)i2;cnt++) armaArray(cnt)=t;}

};

template<class T> const Array<T> concat(const Array<T> &a1, const T &e){Array<T> a2(a1.armaArray.n_elem+1); if(a1.armaArray.n_elem!=0)a2.armaArray.subfield((arma::uword)0,(arma::uword)0,(arma::uword)(a1.armaArray.n_elem-1),(arma::uword)0)=a1.armaArray;  a2.armaArray(a1.armaArray.n_elem)=e; return(a2);}
template<class T> const Array<T> concat(const T &e, const Array<T> &a1)
{
  Array<T> a2(a1.armaArray.n_elem+1);a2.armaArray(0)=e; if(a1.armaArray.n_elem!=0)a2.armaArray.subfield((arma::uword)1,(arma::uword)0,(arma::uword)(a1.armaArray.n_elem),(arma::uword)0)=a1.armaArray;   return(a2);
}
template<class T> const Array<T> concat(const Array<T> &a1, const Array<T> &a2){
  
  Array<T> a3(a1.length()+a2.length());
  arma::uword tcnt=0;
  for(arma::uword cnt=0;cnt<a1.armaArray.n_elem;cnt++) a3.armaArray(tcnt++)=a1.armaArray(cnt);
  for(arma::uword cnt=0;cnt<a2.armaArray.n_elem;cnt++) a3.armaArray(tcnt++)=a2.armaArray(cnt);
  return(a3);
}

template<class T> std::ostream &operator<<(std::ostream &os, const Array<T> &a)
{
  os << "{";
  for (int i = 0; i < a.size() - 1; i++)
    os << a(i) << " ";
  if (a.size() > 0)
    os << a(a.size() - 1);
  os << "}";

  return os;
}
//! \warning: Following function is copied down, for making specialisation for Array<bvec>. Any Bug-Fixes must be reflected in both the places...
template<class T>
std::istream &operator>>(std::istream &is, Array<T> &a)
{
  int nrof_elements = 0;
  char c=0;
  is >> c;
  if (c == '{') {
    is >> c;
    while (c != '}') {
      if (is.eof()) {
	is.setstate(std::ios_base::failbit);
	break;
      }
      if (c != ',') {  // Discard comma signs between elements
	is.putback(c);
      }
      if (++nrof_elements > a.size()) {
	a.set_size(nrof_elements, true);  
      }
      is >> a(nrof_elements - 1);
      is >> c;
    }
    if (a.size() > nrof_elements) {
      a.set_size(nrof_elements, true);
    }
  }
  else {
    is.setstate(std::ios_base::failbit);
  }
  
  return is;
}

inline std::istream &operator>> (std::istream &is, Array<bvec> &a)
{
  int nrof_elements = 0;
  char c=0;
  is >> c;
  if (c == '{') {
    is >> c;
    while (c != '}') {
      if (is.eof()) {
    is.setstate(std::ios_base::failbit);
    break;
      }
      if (c != ',') {  // Discard comma signs between elements
    is.putback(c);
      }
      if (++nrof_elements > a.size()) {
    a.set_size(nrof_elements, true);  // Too slow?
      }
      is >> a(nrof_elements - 1);
      is >> c;
    }
    if (a.size() > nrof_elements) {
      a.set_size(nrof_elements, true);
    }
  }
  else {
    is.setstate(std::ios_base::failbit);
  }

  for(int i=0; i<a.length(); i++) {
    a(i).makebool();
  }

  return is;
}

}//namespace wrapper
#endif 
