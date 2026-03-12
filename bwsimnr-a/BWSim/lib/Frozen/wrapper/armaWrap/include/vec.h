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


#ifndef VEC_H
#define VEC_H
#include<cmath>
#include<cstdlib>
#include<ctime>
#include "algo.h"
#include "constants.h"

#include <armadillo>
using std::cout;
using std::endl;


namespace wrapper {

template<class Num_T> class Vec;
class bVec;
bVec getBVec(arma::uvec input);
arma::uvec getUVec(arma::Col<int> input);
arma::Col<int> getIVec(arma::uvec input);

class bMat;

template<typename Num_T> class Mat;




class bVec
{
public:
    arma::Col<char> armaVec;

    bVec():armaVec((arma::uword)0) {}
    bVec(int size):armaVec((arma::uword)size) {}
    bVec(const bVec &v) {
        armaVec=v.armaVec;
        makebool();
    }
    template<class Num_T>explicit bVec(const Vec<Num_T> &v) {
      bVec v1=to_bvec(v);
        armaVec=v1.armaVec;
        makebool();
    }
    bVec(const char *str):armaVec(str) {makebool();}
    bVec(const std::string &str):armaVec(str) {makebool();}
    bVec(arma::Col<char> v):armaVec(v) {makebool();}
    bVec(const bMat &m);//
    bVec(const std::vector<char>& x):armaVec(x) {makebool();};
    const bVec& operator=(const std::vector<char>& x) {
        armaVec=x;
	makebool();
        return *this;
    };

// #if defined(ARMA_USE_CXX11)
//     bVec(const std::initializer_list<int>& list):armaVec(list) {};
//     const bVec& operator=(const std::initializer_list<int>& list) {
//         armaVec=list;
//         return *this;
//     };
// #endif

    
void makebool()
{
  for(bVec::iterator it=this->begin();it!=this->end();++it )
  {
    *it&=1;
  }
}
    int length() const {
        return armaVec.size();
    }
    int size() const {
        return armaVec.size();
    }
    void set_size(int size, bool copy = false) {
        if(copy) armaVec.resize((arma::uword)size);
        else
	{
	  armaVec.set_size((arma::uword)size);
	  armaVec.zeros();
	}
    }
    void set_length(int size, bool copy = false) {
        set_size(size, copy);
    }
    void zeros() {
        armaVec.zeros();
    }
    void clear() {
        zeros();
    }
    void ones() {
        armaVec.ones();
    }
    void set(const char *str) {
        armaVec=arma::Col<char>(str);
    }
    void set(const std::string &str) {
        armaVec=arma::Col<char>(str);
    }
    char operator[](int i)const {  // Warning fix: const keyword before this function is removed.
        return(armaVec[(arma::uword)i]);
    }
    char operator()(int i)const {  // Warning fix: const keyword before this function is removed.
        return(armaVec((arma::uword)i));
    }

    char &operator[](int i) {
        return(armaVec[(arma::uword)i]);
    }
    char &operator()(int i) {
        return(armaVec((arma::uword)i));
    }
    bVec operator()(int i1, int i2) const {
        bVec toReturn;
        toReturn.armaVec=armaVec.subvec((arma::uword)i1,(arma::uword)i2);
        return(toReturn);
    }
    bVec operator()(const Vec<int> &indexlist) const;
    char get(int i) const {
        return(armaVec[(arma::uword)i]);
    }
    bVec get(int i1, int i2) const {
        bVec toReturn;
        toReturn.armaVec=armaVec.subvec((arma::uword)i1,(arma::uword)i2);
        return(toReturn);
    }
    bVec get(const wrapper::Vec< int >& indexlist) const;
    void set(int i, char t) {
        armaVec[(arma::uword)i]=t;
    }

    bMat transpose();
    bMat T();
    bMat hermitian_transpose();
    bMat H();
    bVec& operator+=(const bVec &v)  {
        for(arma::uword cnt=0; cnt<armaVec.n_elem; cnt++) armaVec(cnt)^=v.armaVec(cnt);
	return *this;
    }
    bVec& operator+=(char t) {
        for(arma::uword cnt=0; cnt<armaVec.n_elem; cnt++) armaVec(cnt)^=t;
	return *this;
    }
    bVec& operator-=(const bVec &v) {
        for(arma::uword cnt=0; cnt<armaVec.n_elem; cnt++) armaVec(cnt)^=v.armaVec(cnt);
	return *this;
    }
    bVec& operator-=(int t) {
        for(arma::uword cnt=0; cnt<armaVec.n_elem; cnt++) armaVec(cnt)^=t;
	return *this;
    }
    bVec& operator*=(int t) {
        for(arma::uword cnt=0; cnt<armaVec.n_elem; cnt++) armaVec(cnt)&=t;
	return *this;
    }
    bVec& operator/=(int t) {
        for(arma::uword cnt=0; cnt<armaVec.n_elem; cnt++) armaVec(cnt)|=t;
	return *this;
    }
    bVec& operator/=(const bVec &v) {
        for(arma::uword cnt=0; cnt<armaVec.n_elem; cnt++) armaVec(cnt)|=v.armaVec(cnt);
	return *this;
    }
    bVec right(int nr) const {
        bVec toReturn;
	if(nr>0)
	{
	  toReturn.armaVec=armaVec.subvec(armaVec.size()-(arma::uword)nr,armaVec.size()-1);
	}
        return(toReturn);
    }
    bVec left(int nr) const {
        bVec toReturn (0);
	if(nr>0){
        toReturn.armaVec=armaVec.subvec(0,(arma::uword)nr-1);
	}
        return(toReturn);
    }
    bVec mid(int start, int nr) const {
        bVec toReturn;
	if(nr>0){
        toReturn.armaVec=armaVec.subvec((arma::uword)start,(arma::uword)start+(arma::uword)nr-1);
	}
        return(toReturn);
    }

    void shift_right(char t, int n = 1) {
        bVec v((arma::uword)n);
        v.zeros();
        v+=t;
        armaVec.shed_rows(armaVec.n_elem-(arma::uword)n,armaVec.n_elem-1);
        armaVec.insert_rows(0,v.armaVec);
    }
    void shift_right(const bVec &v) {
        armaVec.shed_rows(armaVec.n_elem-v.armaVec.n_elem,armaVec.n_elem-1);
        armaVec.insert_rows(0,v.armaVec);
    }
    void shift_left(char t, int n = 1) {
        bVec v((arma::uword)n);
        v.zeros();
        v+=t;
        armaVec.shed_rows(0,(arma::uword)(n-1));
        armaVec.insert_rows(armaVec.size()-1,v.armaVec);
    }
    void shift_left(const bVec &v) {
        armaVec.shed_rows(0,v.armaVec.n_elem-(arma::uword)1);
        armaVec.insert_rows(armaVec.size()-1,v.armaVec);
    }

    void set_subvector(int i1, int i2, const bVec &v) {
        armaVec.subvec((arma::uword)i1,(arma::uword)i2)=v.armaVec;
    }
    void set_subvector(int i, const bVec &v) {
        set_subvector((arma::uword)i,(arma::uword)i+(arma::uword)v.length()-1,v);
    }
    void set_subvector(int i1, int i2, char t) {
        bVec v(i2-i1+1);
        v.zeros();
        v+=t;
        set_subvector(i1,i2,v);
    }
    void replace_mid(int i,const bVec &v) {
        set_subvector(i,v);
    }
    void del(int i) {
        armaVec.shed_row((arma::uword)i);
    }
    void del(int i1, int i2) {
        armaVec.shed_rows((arma::uword)i1,(arma::uword)i2);
    }
    void ins(int i, char t) {
        armaVec.insert_rows((arma::uword)i,1);
        armaVec((arma::uword)i)=t;
    }
    void ins(int i, const bVec &v) {
        armaVec.insert_rows((arma::uword)i,v.armaVec);
    }
    bVec& operator=(char t) {
        armaVec.zeros();
        armaVec+=t;
    makebool();
    return *this;
    }
     bVec& operator=(const bVec &v) {
        armaVec=v.armaVec;
    makebool();
    return *this;
    }



    bVec& operator=(const char *str) {
        armaVec=arma::Col<char>(str);
    makebool();
	return *this;
    }
    bVec& operator=(const std::string &str) {
        armaVec=arma::Col<char>(str);
    makebool();
    return *this;
    }

    bVec operator==(char t) const {
        bVec v(armaVec.size());
        v.zeros();
        v+=t;
        bVec toReturn;
        toReturn=getBVec(armaVec==v.armaVec);
        return(toReturn);
    }

    bVec operator!=(char t) const {
        bVec v(armaVec.size());
        v.zeros();
        v+=t;
        bVec toReturn;
        toReturn=getBVec(armaVec!=v.armaVec);
        return(toReturn);
    }
    bVec operator<(char t) const {
        bVec v(armaVec.size());
        v.zeros();
        v+=t;
        bVec toReturn;
        toReturn=getBVec(armaVec<v.armaVec);
        return(toReturn);
    }
    bVec operator<=(char t) const {
        bVec v(armaVec.size());
        v.zeros();
        v+=t;
        bVec toReturn;
        toReturn=getBVec(armaVec<=v.armaVec);
        return(toReturn);
    }
    bVec operator>(char t) const {
        bVec v(armaVec.size());
        v.zeros();
        v+=t;
        bVec toReturn;
        toReturn=getBVec(armaVec>v.armaVec);
        return(toReturn);
    }
    bVec operator>=(char t) const {
        bVec v(armaVec.size());
        v.zeros();
        v+=t;
        bVec toReturn;
        toReturn=getBVec(armaVec>=v.armaVec);
        return(toReturn);
    }
    bool operator==(const bVec &v) const {
        bool toReturn=true;
	if(v.armaVec.n_elem != armaVec.n_elem)return false;
        for(arma::uword cnt=0; cnt<armaVec.n_elem; cnt++) if(armaVec(cnt)!=v.armaVec(cnt)) toReturn=false;
        return(toReturn);
    }
    bool operator!=(const bVec &v) const {
        bool toReturn=false;
	if(v.armaVec.n_elem != armaVec.n_elem)return true;
        for(arma::uword cnt=0; cnt<armaVec.n_elem; cnt++) if(armaVec(cnt)!=v.armaVec(cnt)) toReturn=true;
        return(toReturn);
    }
    bool in_range(int i) const {
        return (((arma::uword)i < armaVec.size()) && (i >= 0));
    }

    typedef arma::Col<char>::iterator iterator;
    typedef arma::Col<char>::const_iterator const_iterator;

    iterator begin() { return armaVec.begin(); }

    const_iterator begin() const { return armaVec.begin(); }

    iterator end() { return armaVec.end(); }

    const_iterator end() const { return armaVec.end(); }


};



template<class Num_T>
class Vec
{
public:
    arma::Col<Num_T> armaVec;
    Vec():armaVec((arma::uword)0) {}
    Vec(int size):armaVec((arma::uword)size) {}
    Vec(const Vec<Num_T> &v) {
        armaVec=v.armaVec;
    }
    Vec(const char *str):armaVec(str) {}
    Vec(const std::string &str):armaVec(str) {}
    Vec(arma::Col<Num_T> v):armaVec(v) {}

    Vec(const std::vector<Num_T>& x):armaVec(x) {};
    const Vec& operator=(const std::vector<Num_T>& x) {
        armaVec=x;
        return *this;
    };

#if defined(ARMA_USE_CXX11)
    Vec(const std::initializer_list<Num_T>& list):armaVec(list) {};
    const Vec& operator=(const std::initializer_list<Num_T>& list) {
        armaVec=list;
        return *this;
    };
#endif

    template<typename T1> Vec(const arma::Base<Num_T,T1>& X):armaVec(X) {};
    template<typename T1>  const Vec&  operator=(const arma::Base<Num_T,T1>& X) {
        armaVec=X;
        return *this;
    };


    template<typename T1, typename T2>
    explicit Vec(const Vec<T1>& A, const Vec<T2>& B):armaVec(A.armaVec,B.armaVec) {};
    explicit  Vec(const Mat<Num_T>& m){
      if(m.armaMat.n_cols==1)
      {
	armaVec=m.armaMat;

      }
      else if(m.armaMat.n_rows==1)
      {
	armaVec=m.armaMat.st();
      }
      else
      {
	std::cout<<"[both:]  Error this is not a handled case as the input is not a column Matrix or row Matric ......."<<endl;
      }
    };
    Num_T* _data()
    {
        Num_T* t = new Num_T(armaVec.size());
        for (int i=0;i<armaVec.size();i++)
            t[i] = armaVec[(arma::uword)i];
        return t;
    }
    int length() const {
        return armaVec.size();
    }
    int size() const {
        return armaVec.size();
    }
    void set_size(int size, bool copy = false) {
        if(copy) armaVec.resize((arma::uword)size);
        else
	{
	  armaVec.set_size((arma::uword)size);
	  armaVec.zeros();
	}
    }
    void set_length(int size, bool copy = false) {
        set_size(size, copy);
    }
    void zeros() {
        armaVec.zeros();
    }
    void clear() {
        zeros();
    }
    void ones() {
        armaVec.ones();
    }
    void set(const char *str) {
        armaVec=arma::Col<Num_T>(str);
    }
    void set(const std::string &str) {
        armaVec=arma::Col<Num_T>(str);
    }
    Num_T &operator[](int i) {
        return(armaVec[(arma::uword)i]);
    }
    Num_T &operator()(int i) {
        return(armaVec((arma::uword)i));
    }
    const Num_T operator[](int i)const {
        return(armaVec[(arma::uword)i]);
    }
    const Num_T operator()(int i)const {
        return(armaVec((arma::uword)i));
    }

    Vec<Num_T> operator()(int i1, int i2) const {
        Vec<Num_T> toReturn;
        toReturn.armaVec=armaVec.subvec((arma::uword)i1,(arma::uword)i2);
        return(toReturn);
    }
    Vec<Num_T> operator()(const Vec<int> &indexlist) const {
        Vec<Num_T> toReturn;
        toReturn.armaVec=armaVec.elem(getUVec(indexlist.armaVec));
        return(toReturn);
    }
    const Num_T get(int i) const {
        return(armaVec[(arma::uword)i]);
    }
    Vec<Num_T> get(int i1, int i2) const {
        Vec<Num_T> toReturn;
        toReturn.armaVec=armaVec.subvec((arma::uword)i1,(arma::uword)i2);
        return(toReturn);
    }
    Vec<Num_T> get(const Vec<int> &indexlist) const {
        Vec<Num_T> toReturn;
        toReturn.armaVec=armaVec.elem(getUVec(indexlist.armaVec));
        return(toReturn);
    }
    void set(int i, Num_T t) {
        armaVec[(arma::uword)i]=t;
    }
    Mat<Num_T> transpose();
    Mat<Num_T> T();
    Mat<Num_T> hermitian_transpose();
    Mat<Num_T> H();

    Vec<Num_T>& operator+=(const Vec<Num_T> &v)  {
        armaVec+=v.armaVec;
	return *this;
    }
    Vec<Num_T>& operator+=(Num_T t) {
        armaVec+=t;
	return *this;
    }
    Vec<Num_T>& operator-=(const Vec<Num_T> &v) {
        armaVec-=v.armaVec;
	return *this;
    }
    Vec<Num_T>& operator-=(Num_T t) {
        armaVec-=t;
	return *this;
    }
    Vec<Num_T>& operator*=(Num_T t) {
        armaVec*=t;
	return *this;
    }
    Vec<Num_T>& operator/=(Num_T t) {
        armaVec/=t;
	return *this;
    }
    Vec<Num_T>& operator/=(const Vec<Num_T> &v) {
        armaVec/=v.armaVec;
	return *this;
    }
    Vec<Num_T> right(int nr) const {
        Vec<Num_T> toReturn;
	if(nr>0){
        toReturn.armaVec=armaVec.subvec(armaVec.size()-(arma::uword)nr,armaVec.size()-1);
	}
        return(toReturn);
    }
    Vec<Num_T> left(int nr) const {
        Vec<Num_T> toReturn;
	if(nr>0){
        toReturn.armaVec=armaVec.subvec(0,(arma::uword)nr-1);
	}
        return(toReturn);
    }
    Vec<Num_T> mid(int start, int nr) const {
        Vec<Num_T> toReturn;
	if(nr>0){
        toReturn.armaVec=armaVec.subvec((arma::uword)start,(arma::uword)start+(arma::uword)nr-1);
	}
        return(toReturn);
    }

    void shift_right(Num_T t, int n = 1) {
        arma::Col<Num_T> v((arma::uword)n);
        v.zeros();
        v+=t;
        armaVec.shed_rows(armaVec.n_elem-(arma::uword)n,armaVec.n_elem-1);
        armaVec.insert_rows(0,v.st());
    }
    void shift_right(const Vec<Num_T> &v) {
        armaVec.shed_rows(armaVec.n_elem-v.armaVec.n_elem,armaVec.n_elem-1);
        armaVec.insert_rows(0,v.armaVec);
    }
    void shift_left(Num_T t, int n = 1) {
        arma::Col<Num_T> v((arma::uword)n);
        v.zeros();
        v+=t;
        armaVec.shed_rows(0,(arma::uword)(n-1));
        armaVec.insert_rows(armaVec.size(),v.st());
    }
    void shift_left(const Vec<Num_T> &v) {
        armaVec.shed_rows(0,v.armaVec.n_elem-(arma::uword)1);
        armaVec.insert_rows(armaVec.size(),v.armaVec);
    }

    void set_subvector(int i1, int i2, const Vec<Num_T> &v) {
        armaVec.subvec((arma::uword)i1,(arma::uword)i2)=v.armaVec;
    }
    void set_subvector(int i, const Vec<Num_T> &v) {
        set_subvector((arma::uword)i,(arma::uword)i+(arma::uword)v.length()-1,v);
    }
    void set_subvector(int i1, int i2, Num_T t) {
        Vec<Num_T> v(i2-i1+1);
        v.zeros();
        v+=t;
        set_subvector(i1,i2,v);
    }
    void replace_mid(int i, const Vec<Num_T> &v) {
        set_subvector(i,v);
    }
    void del(int i) {
        armaVec.shed_row((arma::uword)i);
    }
    void del(int i1, int i2) {
        armaVec.shed_rows((arma::uword)i1,(arma::uword)i2);
    }
    void ins(int i, Num_T t) {
        armaVec.insert_rows((arma::uword)i,1);
        armaVec((arma::uword)i)=t;
    }
    void ins(int i, const Vec<Num_T> &v) {
        armaVec.insert_rows((arma::uword)i,v);
    }
    Vec<Num_T>& operator=(Num_T t) {
        armaVec.zeros();
        armaVec+=t;
	return *this;
    }
    Vec<Num_T>& operator=(const Vec<Num_T> &v) {
        armaVec=v.armaVec;
	return *this;
    }


    Vec<Num_T>& operator=(const char *str) {
        armaVec=arma::Col<Num_T>(str);
	return *this;
    }
    Vec<Num_T>& operator=(const std::string &str) {
        armaVec=arma::Col<Num_T>(str);
	return *this;
    }

    bVec operator==(Num_T t) const {
        arma::Col<Num_T> v(armaVec.size());
        v.zeros();
        v+=t;
        bVec toReturn;
        toReturn=getBVec(armaVec==v);
        return(toReturn);
    }

    bVec operator!=(Num_T t) const {
        arma::Col<Num_T> v(armaVec.size());
        v.zeros();
        v+=t;
        bVec toReturn;
        toReturn=getBVec(armaVec!=v);
        return(toReturn);
    }

    bVec operator<(Num_T t) const {
        arma::Col<Num_T> v(armaVec.size());
        v.zeros();
        v+=t;
        bVec toReturn;
        toReturn=getBVec(armaVec<v);
        return(toReturn);
    }
    bVec operator<=(Num_T t) const {
        arma::Col<Num_T> v(armaVec.size());
        v.zeros();
        v+=t;
        bVec toReturn;
        toReturn=getBVec(armaVec<=v);
        return(toReturn);
    }
    bVec operator>(Num_T t) const {
        arma::Col<Num_T> v(armaVec.size());
        v.zeros();
        v+=t;
        bVec toReturn;
        toReturn=getBVec(armaVec>v);
        return(toReturn);
    }
    bVec operator>=(Num_T t) const {
        arma::Col<Num_T> v(armaVec.size());
        v.zeros();
        v+=t;
        bVec toReturn;
        toReturn=getBVec(armaVec>=v);
        return(toReturn);
    }
    bool operator==(const Vec<Num_T> &v) const {
        bool toReturn=true;
	if(armaVec.n_elem != v.armaVec.n_elem) return false;
        for(arma::uword cnt=0; cnt<armaVec.n_elem; cnt++) if(armaVec(cnt)!=v.armaVec(cnt))toReturn=false;
        return(toReturn);
    }
    bool operator!=(const Vec<Num_T> &v) const {
        bool toReturn=false;
	if(armaVec.n_elem != v.armaVec.n_elem) return true;
        for(arma::uword cnt=0; cnt<armaVec.n_elem; cnt++) if(armaVec(cnt)!=v.armaVec(cnt))toReturn=true;
        return(toReturn);
    }
    bool in_range(int i) const {
        return ((i < armaVec.size()) && (i >= 0));
    }

    typedef typename arma::Col<Num_T>::iterator iterator;
    typedef typename arma::Col<Num_T>::const_iterator const_iterator;

    iterator begin() { return armaVec.begin(); }

    const_iterator begin() const { return armaVec.begin(); }

    iterator end() { return armaVec.end(); }

    const_iterator end() const { return armaVec.end(); }



};

template <typename Num_T> wrapper::Mat<Num_T>  Vec<Num_T>::transpose() {
    Mat<Num_T> temp;
    temp.armaMat=armaVec.st();
    return temp;
}

template <typename Num_T> Mat<Num_T> Vec<Num_T>::T() {
    Mat<Num_T> temp;
    temp.armaMat=armaVec.st();
    return temp;
}


template <typename Num_T> Mat<Num_T> Vec<Num_T>::hermitian_transpose() {
    Mat<Num_T> temp;
    temp.armaMat=armaVec.t();
    return temp;
}

template <typename Num_T> Mat<Num_T> Vec<Num_T>::H() {
    Mat<Num_T> temp;
    temp.armaMat=armaVec.t();
    return temp;
}






std::ostream &operator<<(std::ostream &os, const bVec &v);

typedef Vec<double> dVec;
typedef Vec< std::complex<double> > cVec;
typedef Vec<int> iVec;
typedef Vec<short int> sVec;
std::ostream &operator<<(std::ostream &os, const std::complex<double>  &v);
// char operator+(char a,char b);

template<class Num_T>std::ostream &operator<<(std::ostream &os, const Vec<Num_T> &v)
{
    arma::uword sz = v.armaVec.n_elem;
    os << "[" ;
    for (arma::uword i = 0; i < sz; i++)
    {
        os << v.armaVec(i) ;
        if (i < (sz - 1))
            os << " ";
    }
    os << "]" ;
    return os;
}





template <class T>bVec to_bvec( Vec<T> v)
{
    bVec temp(v.length());
    for (int i = 0; i < v.length(); ++i)
    {

        temp[i] = v[i]!=0;
    }
    return temp;
}

template <class T>sVec to_svec(const Vec<T> &v)
{
    sVec temp(v.length());
    for (int i = 0; i < v.length(); ++i) temp(i) = (short)v(i);
    return temp;
}

// template <class T>iVec to_ivec( Vec<T> &v)
// {
//     iVec temp(v.length());
//     for (int i = 0; i < v.length(); ++i)	temp(i) = (int)(v[i]);
//     return temp;
// }

template <class T>iVec to_ivec( Vec<T> v)
{
    iVec temp(v.length());
    for (int i = 0; i < v.length(); ++i)	temp(i) = (int)(v[i]);
    return temp;
}

template <class T>dVec to_vec(const Vec<T> &v)
{
    dVec temp(v.length());
    for (int i = 0; i < v.length(); ++i)
        temp[i] =
            (double)v[i];
    return temp;
}

template <class T> cVec to_cvec(const Vec<T> &v)
{
    cVec temp(v.length());
    for (int i = 0; i < v.length(); ++i)
        temp[i] = std::complex<double>((double)v[i], 0.0);
    return temp;
}

template<> inline cVec to_cvec(const cVec& v) {
    return v;
}

template <class T> cVec to_cvec(const Vec<T> &real, const Vec<T> &imag)
{
    if(real.length() != imag.length())
    {
        cout<<"[both:] to_cvec(): real and imaginary parts must have the same length"<<endl;
        abort();
    }
    cVec temp(real.length());
    for (int i = 0; i < real.length(); ++i)
        temp(i) = std::complex<double>(static_cast<double>(real(i)),
                                       static_cast<double>(imag(i)));
    return temp;
}


template<class Num_T>  Vec<Num_T> operator+(const Vec<Num_T> &v1, const Vec<Num_T> &v2) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=v1.armaVec+v2.armaVec;
    return(toReturn);
}

template<class Num_T>  Vec<Num_T> operator+(const Vec<Num_T> &v, Num_T t) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=v.armaVec+t;
    return(toReturn);
}




template<typename Num_T>  Vec<Num_T> operator+(Num_T t, const Vec<Num_T> &v) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=v.armaVec+t;
    return(toReturn);
}



template<class Num_T,class T>  Vec<Num_T> operator+(const Vec<Num_T> &v1, const Vec<T> &v2) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=v1.armaVec+v2.armaVec;
    return(toReturn);
}



template<class Num_T>  Vec< std::complex<double> > operator+(const Vec<Num_T> &v1, const Vec<std::complex< double > > &v2) {
    Vec<std::complex< double > > toReturn;
    toReturn.armaVec=v1.armaVec+v2.armaVec;
    return(toReturn);
}


template<class Num_T>  Vec<Num_T> operator+(const bVec &v1, const Vec<Num_T > &v2) {
    Vec<Num_T > toReturn;
    toReturn.armaVec=v1.armaVec+v2.armaVec;
    return(toReturn);
}
template<typename T> Vec<T> operator+( const Vec<T > &v1,const bVec &v2) {
    Vec<T>  toReturn;
    toReturn.armaVec=v1.armaVec+v2.armaVec;
    return(toReturn);
}

template<class Num_T,class T>  Vec<Num_T> operator+(const Vec<Num_T> &v, const T t) {
  Vec<Num_T> toReturn;
    toReturn.armaVec=v.armaVec+t;
    return(toReturn);
}

template<class Num_T,class T>  Vec<Num_T> operator+(T t, const Vec<Num_T> &v) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=v.armaVec+t;
    return(toReturn);
}

template<class Num_T>std::istream &operator>>(std::istream &is, Vec<Num_T> &v)
{
  std::ostringstream buffer;
  bool started = false;
  bool finished = false;
  bool brackets = false;
  char c=0;

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

std::istream &operator>>(std::istream &is, bVec &v);

Vec< std::complex<double> > operator+(const bVec &v1, const Vec<std::complex< double > > &v2) ;

Vec< std::complex<double> > operator+( const Vec<std::complex< double > > &v1,const bVec &v2) ;

Vec<double > operator+(const iVec &v1, const Vec< double  > &v2);

Vec< std::complex<double> > operator+(const Vec<std::complex< double > > & v1, const Vec<std::complex< double > > &v2);












template<class Num_T>  Vec<Num_T> operator-(const Vec<Num_T> &v1, const Vec<Num_T> &v2) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=v1.armaVec-v2.armaVec;
    return(toReturn);
}
template<class Num_T>  Vec<Num_T> operator-(const Vec<Num_T> &v, Num_T t) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=v.armaVec-t;
    return(toReturn);
}
template<class Num_T>  Vec<Num_T> operator-(Num_T t, const Vec<Num_T> &v) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=t-v.armaVec;
    return(toReturn);
}
template<class Num_T>  Vec<Num_T> operator-(const Vec<Num_T> &v) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=-v.armaVec;
    return(toReturn);
}

template<class Num_T,class T>  Vec<Num_T> operator-(const Vec<Num_T> &v1, const Vec<T> &v2) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=v1.armaVec-v2.armaVec;
    return(toReturn);
}
template<class Num_T>  Vec< std::complex<double> > operator-(const Vec<Num_T> &v1, const Vec<std::complex< double > > &v2) {
    Vec<std::complex< double > > toReturn;
    toReturn.armaVec=v1.armaVec-v2.armaVec;
    return(toReturn);
}



Vec< std::complex<double> > operator-(const bVec &v1, const Vec<std::complex< double > > &v2) ;

Vec< std::complex<double> > operator-( const Vec<std::complex< double > > &v1,const bVec &v2) ;

Vec<double > operator-(const iVec &v1, const Vec< double  > &v2);
Vec<double > operator-(const Vec< double  > &v1,const iVec &v2);

Vec< std::complex<double> > operator-(const Vec<std::complex< double > > & v1, const Vec<std::complex< double > > &v2);




template<class Num_T>  Vec<Num_T> operator-(const bVec &v1, const Vec<Num_T > &v2) {
    Vec<Num_T > toReturn;
    toReturn.armaVec=v1.armaVec-v2.armaVec;
    return(toReturn);
}
template<typename T> Vec<T> operator-( const Vec<T > &v1,const bVec &v2) {
    Vec<T>  toReturn;
    toReturn.armaVec=v1.armaVec-v2.armaVec;
    return(toReturn);
}






template<class Num_T,typename T>  Vec<Num_T> operator-(const Vec<Num_T> &v, const T t) {
  Vec<Num_T> toReturn;
    toReturn.armaVec=v.armaVec-t;
    return(toReturn);
}

template<typename Num_T,typename T>  Vec<Num_T> operator-(const T t, const Vec<Num_T> &v) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=t-v.armaVec;
    return(toReturn);
}




template<class Num_T>  Vec<Num_T> operator*(const Vec<Num_T> &v, Num_T t) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=v.armaVec*t;
    return(toReturn);
}

template<class Num_T>  Vec<Num_T> operator*(Num_T t, const Vec<Num_T> &v) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=v.armaVec*t;
    return(toReturn);
}


 Vec< std::complex<double> > operator*(std::complex<double> t, const Vec<double> &v);
 Vec< std::complex<double> > operator*( const Vec<double> &v,std::complex<double> t) ;



template<class Num_T>  Vec<Num_T> elem_mult(const Vec<Num_T> &a, const Vec<Num_T> &b) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=a.armaVec%b.armaVec;
    return(toReturn);
}


Vec< std::complex<double> >  operator/(Vec< std::complex<double> > &v, const int t) ;
Vec< std::complex<double> >  operator/(int t,Vec< std::complex<double> > &v);
template<class Num_T>  Vec<Num_T> operator/(const Vec<Num_T> &v, Num_T t) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=v.armaVec/t;
    return(toReturn);
}
template<class Num_T>  Vec<Num_T> operator/(Num_T t, const Vec<Num_T> &v) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=t/v.armaVec;
    return(toReturn);
}




template<class Num_T>  Vec<Num_T> elem_div(const Vec<Num_T> &v1, const Vec<Num_T> &v2) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=v1.armaVec/v2.armaVec;
    return(toReturn);
}
template<class Num_T>  Vec<Num_T> elem_div(Num_T t, const Vec<Num_T> &v) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=v.armaVec/t;
    return(toReturn);
}
template<class Num_T>  Vec<Num_T> concat(const Vec<Num_T> &v, Num_T t) {
    Vec<Num_T> toReturn;
    if(v.size()>0)toReturn=v;
    toReturn.armaVec.resize(v.armaVec.size()+1);
    toReturn.armaVec(v.armaVec.size())=t;
    return(toReturn);
}
template<class Num_T>  Vec<Num_T> concat(Num_T t, const Vec<Num_T> &v) {
    Vec<Num_T> toReturn(v.length()+1);
    toReturn.armaVec((arma::uword)0)=t;
    if(v.size()>0)
    toReturn.armaVec.subvec((arma::uword)1,toReturn.armaVec.n_elem-(arma::uword)1)=v.armaVec;
    return(toReturn);
}
template<class Num_T>  Vec<Num_T> concat(const Vec<Num_T> &v1, const Vec<Num_T> &v2) {
    Vec<Num_T> toReturn;
    if(v1.size()>0)toReturn=v1;
    toReturn.armaVec.resize(v1.armaVec.size()+v2.armaVec.size());
    if(v2.size()>0)
    toReturn.armaVec.subvec(v1.armaVec.size(),toReturn.armaVec.n_elem-(arma::uword)1)=v2.armaVec;
    return(toReturn);
}





std::complex<double> operator*( Vec< std::complex<double> > &v, Vec< std::complex<double> > &v1);

double operator*( iVec &v, iVec &v1);
double operator*( dVec &v, dVec &v1);
double operator*( sVec &v, sVec &v1);
double operator*( bVec &v, bVec &v1);









template<class Num_T> void  sort( Vec<Num_T> &v) {
    arma::Col<Num_T> sVec= sort(v.armaVec);
    v.armaVec=sVec;
}


template<class Num_T> iVec  sort_index( Vec<Num_T> v) {
  iVec toReturn;
  arma::uvec si=sort_index(v.armaVec);
    toReturn.armaVec=getIVec(si);
    return(toReturn);
}


template<class Num_T> Vec<Num_T>  repmat(const Vec<Num_T> & A, int n);

template<class Num_T> Vec<Num_T>  repmat(const Vec<Num_T> & A, int n)//moved from vec.cpp to make it inline
{
  Mat<Num_T>temp1=repmat(A,n,1);
  Vec<Num_T>temp;
  temp=temp1.get_col(0);
  return temp;
}

bVec operator+(const bVec &v1, const bVec &v2);
bVec operator+(const bVec &v, char t);
bVec operator+(char t, const bVec &v);
bVec operator-(const bVec &v1, const bVec &v2);
bVec operator-(const bVec &v, char t);
bVec operator-(char t, const bVec &v);
bVec operator-(const bVec &v);
bVec operator*(const bVec &v, char t);
bVec operator*(char t, const bVec &v);
bVec elem_mult(const bVec &a, const bVec &b);
bVec operator/(const bVec &v, char t);
bVec operator/(char t, const bVec &v);
bVec elem_div(const bVec &v1, const bVec &v2);
bVec elem_div(char t, const bVec &v);
bVec concat(const bVec &v, char t);
bVec concat(char t, const bVec &v);
bVec concat(const bVec &v1, const bVec &v2);
bVec concat(const bVec &v1, const bVec &v2, const bVec &v3);

bVec to_bvec(const bVec &v);
sVec to_svec(const bVec &v);
iVec to_ivec(const bVec &v);
dVec to_vec(const bVec &v);
cVec to_cvec(const bVec &v);

bVec to_bvec(const char c);
sVec to_svec(const short c);
iVec to_ivec(const int c);
dVec to_vec(const double c);
cVec to_cvec(const std::complex<double> c);
cVec to_cvec(double real, double imag);

typedef Vec<double> vec;
typedef Vec< std::complex<double> > cvec;
typedef Vec<int> ivec;
typedef Vec<short int> svec;
typedef bVec bvec;


template<class T, class T1, class T2> struct mult_{
T operator() ( T1 a, T2 b)
{
  return (T)(a*b);
}
};

template<class T, class T1, class T2> struct add_{
T operator() ( T1 a, T2 b)
{
  return (T)(a+b);
}
};

template<class T, class T1, class T2> struct substract_{
T operator() ( T1 a, T2 b)
{
  return (T)(a-b);
}
};

template<class T, class T1, class T2> struct divide_{
T operator() ( T1 a, T2 b)
{
  return (T)(a/b);
}
};


template<typename T1, typename T2,class T3> Vec<T2> unary_applyFunction(Vec<T1> & ip,T3 binaryFunctionPointer)
  {
    Vec<T2> op(ip.size());
    typename arma::Col<T1>::iterator vit=ip.armaVec.begin();
    typename arma::Col<T2>::iterator  oit=op.armaVec.begin();
    for(;vit!=ip.armaVec.end();++vit,++oit)
    {
      *oit=(T2)binaryFunctionPointer(*vit);
    }
    return op;
  }

 template<typename T1, class T2,class T3> Vec<T1> unary_genFunction(unsigned int size,T2 convFunction,T3 genFunction)
  {
    Vec<T1> op(size);

    for(typename arma::Col<T1>::iterator  oit=op.armaVec.begin();oit!=op.armaVec.end();++oit)
    {
      *oit=(T1)convFunction(genFunction);
    }
    return op;
  }





 //fucntion defenitions automatically generated

 vec operator + (const float  &s , const vec  &v);     ///Addition operator for float and vec.
vec operator + (const short  &s , const vec  &v);     ///Addition operator for short and vec.
vec operator + (const int  &s , const vec  &v);     ///Addition operator for int and vec.
vec operator + (const vec  &v , const float  &s);     ///Addition operator for vec and float.
vec operator + (const vec  &v , const short  &s);     ///Addition operator for vec and short.
vec operator + (const vec  &v , const int  &s);     ///Addition operator for vec and int.
vec operator - (const float  &s , const vec  &v);     ///Subtraction operator for float and vec.
vec operator - (const short  &s , const vec  &v);     ///Subtraction operator for short and vec.
vec operator - (const int  &s , const vec  &v);     ///Subtraction operator for int and vec.
vec operator - (const vec  &v , const float  &s);     ///Subtraction operator for vec and float.
vec operator - (const vec  &v , const short  &s);     ///Subtraction operator for vec and short.
vec operator - (const vec  &v , const int  &s);     ///Subtraction operator for vec and int.
vec operator * (const float  &s , const vec  &v);     ///Multiplication operator for float and vec.
vec operator * (const short  &s , const vec  &v);     ///Multiplication operator for short and vec.
vec operator * (const int  &s , const vec  &v);     ///Multiplication operator for int and vec.
//cvec operator * (const std::complex< double >  &s , const vec  &v);              ///Multiplication operator for complex<double> and vec.
vec operator * (const vec  &v , const float  &s);     ///Multiplication operator for vec and float.
vec operator * (const vec  &v , const short  &s);     ///Multiplication operator for vec and short.
vec operator * (const vec  &v , const int  &s);     ///Multiplication operator for vec and int.
//cvec operator * (const vec  &v , const std::complex< double >  &s);              ///Multiplication operator for vec and complex<double>
vec operator / (const vec  &v , const float  &s);     ///Division operator for vec and float.
vec operator / (const vec  &v , const short  &s);     ///Division operator for vec and short.
vec operator / (const vec  &v , const int  &s);     ///Division operator for vec and int.
vec operator + (const double  &s , const ivec  &v);     ///Addition operator for double and ivec.
vec operator + (const ivec  &v , const double  &s);     ///Addition operator for ivec and double.
vec operator - (const double  &s , const ivec  &v);     ///Subtraction operator for double and ivec.
vec operator - (const ivec  &v , const double  &s);     ///Subtraction operator for ivec and double.
vec operator * (const double  &s , const ivec  &v);     ///Multiplication operator for double and ivec.
vec operator * (const ivec  &v , const double  &s);     ///Multiplication operator for ivec and double.
vec operator / (const double  &s , const ivec  &v);     ///Division operator for double and ivec.
vec operator / (const ivec  &v , const double  &s);     ///Division operator for ivec and double.
cvec operator + (const std::complex< double >  &s , const ivec  &v);              ///Addition operator for complex<double> and ivec.
cvec operator + (const ivec  &v , const std::complex< double >  &s);              ///Addition operator for ivec and complex<double>
cvec operator - (const std::complex< double >  &s , const ivec  &v);              ///Subtraction operator for complex<double> and ivec.
cvec operator - (const ivec  &v , const std::complex< double >  &s);              ///Subtraction operator for ivec and complex<double>
cvec operator * (const std::complex< double >  &s , const ivec  &v);              ///Multiplication operator for complex<double> and ivec.
cvec operator * (const ivec  &v , const std::complex< double >  &s);              ///Multiplication operator for ivec and complex<double>
cvec operator / (const std::complex< double >  &s , const ivec  &v);              ///Division operator for complex<double> and ivec.
cvec operator / (const ivec  &v , const std::complex< double >  &s);              ///Division operator for ivec and complex<double>
cvec operator + (const double  &s , const cvec  &v);              ///Addition operator for double and cvec.
cvec operator + (const float  &s , const cvec  &v);              ///Addition operator for float and cvec.
cvec operator + (const short  &s , const cvec  &v);              ///Addition operator for short and cvec.
cvec operator + (const int  &s , const cvec  &v);              ///Addition operator for int and cvec.
cvec operator + (const cvec  &v , const float  &s);              ///Addition operator for cvec and float.
cvec operator + (const cvec  &v , const double  &s);              ///Addition operator for cvec and double.
cvec operator + (const cvec  &v , const short  &s);              ///Addition operator for cvec and short.
cvec operator + (const cvec  &v , const int  &s);              ///Addition operator for cvec and int.
cvec operator - (const double  &s , const cvec  &v);              ///Subtraction operator for double and cvec.
cvec operator - (const float  &s , const cvec  &v);              ///Subtraction operator for float and cvec.
cvec operator - (const short  &s , const cvec  &v);              ///Subtraction operator for short and cvec.
cvec operator - (const int  &s , const cvec  &v);              ///Subtraction operator for int and cvec.
cvec operator - (const cvec  &v , const float  &s);              ///Subtraction operator for cvec and float.
cvec operator - (const cvec  &v , const double  &s);              ///Subtraction operator for cvec and double.
cvec operator - (const cvec  &v , const short  &s);              ///Subtraction operator for cvec and short.
cvec operator - (const cvec  &v , const int  &s);              ///Subtraction operator for cvec and int.
cvec operator * (const double  &s , const cvec  &v);              ///Multiplication operator for double and cvec.
cvec operator * (const float  &s , const cvec  &v);              ///Multiplication operator for float and cvec.
cvec operator * (const short  &s , const cvec  &v);              ///Multiplication operator for short and cvec.
cvec operator * (const int  &s , const cvec  &v);              ///Multiplication operator for int and cvec.
cvec operator * (const cvec  &v , const float  &s);              ///Multiplication operator for cvec and float.
cvec operator * (const cvec  &v , const double  &s);              ///Multiplication operator for cvec and double.
cvec operator * (const cvec  &v , const short  &s);              ///Multiplication operator for cvec and short.
cvec operator * (const cvec  &v , const int  &s);              ///Multiplication operator for cvec and int.
cvec operator / (const cvec  &v , const double  &s);              ///Division operator for cvec and double.
cvec operator / (const double  &s , const cvec  &v);              ///Division operator for double and cvec.
cvec operator / (const cvec  &v , const float  &s);              ///Division operator for cvec and float.
cvec operator / (const cvec  &v , const short  &s);              ///Division operator for cvec and short.
cvec operator / (const cvec  &v , const int  &s);              ///Division operator for cvec and int.
vec operator + (const bvec  &a , const vec  &b);     ///Addition operator for bvec and vec.
vec operator + (const svec  &a , const vec  &b);     ///Addition operator for svec and vec.
vec operator + (const ivec  &a , const vec  &b);     ///Addition operator for ivec and vec.
vec operator + (const vec  &a , const bvec  &b);     ///Addition operator for vec and bvec.
vec operator + (const vec  &a , const svec  &b);     ///Addition operator for vec and svec.
vec operator + (const vec  &a , const ivec  &b);     ///Addition operator for vec and ivec.
vec operator - (const bvec  &a , const vec  &b);     ///Subtraction operator for bvec and vec.
vec operator - (const svec  &a , const vec  &b);     ///Subtraction operator for svec and vec.
vec operator - (const ivec  &a , const vec  &b);     ///Subtraction operator for ivec and vec.
vec operator - (const vec  &a , const bvec  &b);     ///Subtraction operator for vec and bvec.
vec operator - (const vec  &a , const svec  &b);     ///Subtraction operator for vec and svec.
vec operator - (const vec  &a , const ivec  &b);     ///Subtraction operator for vec and ivec.
cvec operator + (const bvec  &a , const cvec  &b);              ///Addition operator for bvec and cvec.
cvec operator + (const svec  &a , const cvec  &b);              ///Addition operator for svec and cvec.
cvec operator + (const ivec  &a , const cvec  &b);              ///Addition operator for ivec and cvec.
cvec operator + (const cvec  &a , const bvec  &b);              ///Addition operator for cvec and bvec.
cvec operator + (const cvec  &a , const svec  &b);              ///Addition operator for cvec and svec.
cvec operator + (const cvec  &a , const ivec  &b);              ///Addition operator for cvec and ivec.
cvec operator - (const bvec  &a , const cvec  &b);              ///Subtraction operator for bvec and cvec.
cvec operator - (const svec  &a , const cvec  &b);              ///Subtraction operator for svec and cvec.
cvec operator - (const ivec  &a , const cvec  &b);              ///Subtraction operator for ivec and cvec.
cvec operator - (const cvec  &a , const bvec  &b);              ///Subtraction operator for cvec and bvec.
cvec operator - (const cvec  &a , const svec  &b);              ///Subtraction operator for cvec and svec.
cvec operator - (const cvec  &a , const ivec  &b);              ///Subtraction operator for cvec and ivec.

template<class T>
T prod(const Vec<T> &v)
{
  if(v.size() < 1){
    cout<< "prod: size of vector should be at least 1" <<endl; abort();
  }
  T out = v(0);
  for (int i = 1; i < v.size(); i++)
    out *= v(i);
  return out;
}

 template<class T>
  Vec<T> prod(const Mat<T> &m, int dim = 1)
  {
    if((dim == 1) || (dim == 2)){ cout<< "prod: dimension need to be 1 or 2"<<endl;abort(); }
    Vec<T> out(m.cols());

    if (dim == 1) {
      if((m.cols() >= 1) && (m.rows() >= 1)){ cout<< "prod: number of columns should be at least 1"<<endl;abort(); }
      out.set_size(m.cols(), false);

      for (int i = 0; i < m.cols(); i++)
        out(i) = prod(m.get_col(i));
    }
    else {
      if((m.cols() >= 1) && (m.rows() >= 1)){ cout<< "prod: number of rows should be at least 1"<<endl;abort(); }
      out.set_size(m.rows(), false);

      for (int i = 0; i < m.rows(); i++)
        out(i) = prod(m.get_row(i));
    }
    return out;
  }
  
  template<class Num_T>  Vec<Num_T> conv(const Vec<Num_T> &v1, const Vec<Num_T> &v2) {
    Vec<Num_T> toReturn;
    toReturn.armaVec=arma::conv(v1.armaVec,v2.armaVec);
    return(toReturn);
  }
  
 
 template<class T> Vec<T> cumsum(const Vec<T> &v)
 {
   int inSize = v.size();
   if(inSize < 1){
     cout<< "prod: size of vector should be at least 1" <<endl; abort();
   }
   
   Vec<T> out(inSize);
   T total = 0;
   
   for (int i = 0; i < inSize; i++) 
   {
     total += v[i];
     out[i] = total;
   }
   return out;
 }
 template<class T> Vec<T> cumprod(const Vec<T> &v)
 {
     int inSize = v.size();
     if(inSize < 1){
         cout<< "prod: size of vector should be at least 1" <<endl; abort();
     }
     
     Vec<T> out(inSize);
     T total = 0;
     
     for (int i = 0; i < inSize; i++) 
     {
         total *= v[i];
         out[i] = total;
     }
     return out;
 }
 
  wrapper::cVec  fft(const wrapper::cVec & v,int FFTsize=0); 
  
  wrapper::cVec  ifft(const wrapper::cVec & v,int FFTsize=0);

}//namespace wrapper
#endif

