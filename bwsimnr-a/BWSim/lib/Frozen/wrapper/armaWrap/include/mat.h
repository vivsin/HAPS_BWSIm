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

#ifndef MAT_H
#define MAT_H

#include "vec.h"
namespace wrapper {
using arma::span;
class bMat
{
public:

    arma::Mat<char> armaMat;

    bMat():armaMat() {};
    bMat(int rows, int cols):armaMat((arma::uword)rows,(arma::uword)cols) {}
    bMat(const bMat &m) {
        armaMat=m.armaMat;
    }
    bMat(const bVec &v) {
        armaMat=v.armaVec;
    }
    bMat(const std::string &str):armaMat(str) {makebool();}
    bMat(const char *str):armaMat(str) {makebool();}

    bMat(const std::vector<char>& x):armaMat(x) {makebool();}
    const bMat& operator=(const std::vector<char>& x) {
        armaMat=x;
	makebool();
        return *this;
    };

#if defined(ARMA_USE_CXX11)
    bMat(const std::initializer_list<char>& list):armaMat(list) {};
    const bMat& operator=(const std::initializer_list<char>& list) {
        armaMat=list;
        return *this;
    };
#endif

    

void makebool()
{
  for(bMat::iterator it=this->begin();it!=this->end();++it )
  {
    *it&=1;
  }
}
    int cols() const {
        return armaMat.n_cols;
    }
    int rows() const {
        return armaMat.n_rows;
    }
    int size() const {
        return armaMat.n_elem;
    }
    void set_size(int rows, int cols, bool copy = false) {
        if(copy) armaMat.resize((arma::uword)rows,(arma::uword)cols);
	else {
	  armaMat.set_size((arma::uword)rows,(arma::uword)cols);
	  armaMat.zeros();
	}
    }
    void zeros() {
        armaMat.zeros();
    }
    void clear() {
        zeros();
    }
    void ones() {
        armaMat.ones();
    }
    void set(const std::string &str) {
        armaMat=arma::Mat<char>(str);
    }
    void set(const char *str) {
        armaMat=arma::Mat<char>(str);
    }
    char operator()(int r, int c) const {     // Warning fix: const keyword before this function is removed.
        return(armaMat((arma::uword)r,(arma::uword)c)); 
    }
    char &operator()(int r, int c) {
        return(armaMat((arma::uword)r,(arma::uword)c));
    }
    char operator()(int i) const {  // Warning fix: const keyword before this function is removed.
        return(armaMat((arma::uword)i));
    }
    char &operator()(int i) {
        return(armaMat((arma::uword)i));
    }
    char get(int r, int c) const {  // Warning fix: const keyword before this function is removed.
        return(armaMat((arma::uword)r,(arma::uword)c));
    }
    char get(int i) const {  // Warning fix: const keyword before this function is removed.
        return(armaMat((arma::uword)i));
    }
    void set(int r, int c, char t) {
        armaMat((arma::uword)r,(arma::uword)c)=t;
    }
    bMat operator()(int r1, int r2, int c1, int c2) const {
        bMat a;
        a.armaMat=armaMat.submat((arma::uword)r1,(arma::uword)c1,(arma::uword)r2,(arma::uword)c2);
        return(a);
    }
    bMat get(int r1, int r2, int c1, int c2) const {
        bMat a;
        a.armaMat=armaMat.submat((arma::uword)r1,(arma::uword)c1,(arma::uword)r2,(arma::uword)c2);
        return(a);
    }
    bVec get_row(int r) const {
        bVec a;
        a.armaVec=arma::Col<char>((armaMat.row((arma::uword)r)).st());
        return(a);
    }
    bMat get_rows(int r1, int r2) const {
        bMat a;
        a.armaMat=armaMat.rows((arma::uword)r1,(arma::uword)r2);
        return(a);
    }
    bMat get_rows(const Vec<int> &indexlist) const {
        bMat a;
        a.armaMat=armaMat.rows(getUVec(indexlist.armaVec));
        return(a);
    }
    bVec get_col(int c) const {
        bVec a;
        a.armaVec=arma::Col<char>(armaMat.col((arma::uword)c));
        return(a);
    }
    bMat get_cols(int c1, int c2) const {
        bMat a;
        a.armaMat=armaMat.cols((arma::uword)c1,(arma::uword)c2);
        return(a);
    }
    bMat get_cols(const Vec<int> &indexlist) const {
        bMat a;
        a.armaMat=armaMat.cols(getUVec(indexlist.armaVec));
        return(a);
    }
    void set_row(int r, const bVec &v) {
        armaMat.row((arma::uword)r)=arma::Mat<char>(v.armaVec.st());
    }
    void set_col(int c, const bVec &v) {
        armaMat.col((arma::uword)c)=arma::Mat<char>(v.armaVec);
    }
    void set_rows(int r, const bMat &m) {
        armaMat.submat( span((arma::uword)r,((arma::uword)r)+m.armaMat.n_rows-1),span::all)=m.armaMat;
    }
    void set_cols(int c, const bMat &m) {
        armaMat.submat(span::all, span((arma::uword)c,((arma::uword)c)+m.armaMat.n_cols-1))=m.armaMat;
    }
    void copy_row(int to, int from) {
        armaMat.row((arma::uword)to)=armaMat.row((arma::uword)from);
    }
    void copy_col(int to, int from) {
        armaMat.col((arma::uword)to)=armaMat.col((arma::uword)from);
    }
    void swap_rows(int r1, int r2) {
        armaMat.swap_rows((arma::uword)r1,(arma::uword)r2);
    }
    void swap_cols(int c1, int c2) {
        armaMat.swap_cols((arma::uword)c1,(arma::uword)c2);
    }
    void set_submatrix(int r1, int r2, int c1, int c2, const bMat &m) {
        armaMat.submat( span((arma::uword)r1,(arma::uword)r2),span((arma::uword)c1,(arma::uword)c2))=m.armaMat;
    }
    void set_submatrix(int r, int c, const bMat &m) {
        armaMat.submat( span((arma::uword)r,((arma::uword)r)+m.armaMat.n_rows-1),span((arma::uword)c,((arma::uword)c)+m.armaMat.n_cols-1))=m.armaMat;
    }
    void set_submatrix(int r1, int r2, int c1, int c2, char t) {
        bMat m;
        m.armaMat.zeros((arma::uword)(r2-r1),(arma::uword)(c2-c1));
        m+=t;
        set_submatrix(r1,r2,c1,c2,m);
    }
    void del_row(int r) {
        armaMat.shed_row((arma::uword)r);
    }
    void del_rows(int r1, int r2) {
        armaMat.shed_rows((arma::uword)r1,(arma::uword)r2);
    }
    void del_col(int c) {
        armaMat.shed_col((arma::uword)c);
    }
    void del_cols(int c1, int c2) {
        armaMat.shed_cols((arma::uword)c1,(arma::uword)c2);
    }
    void ins_row(int r, const bVec &v) {
        armaMat.insert_rows((arma::uword)r,v.armaVec.st());
    }
    void ins_col(int c, const bVec &v) {
        armaMat.insert_cols((arma::uword)c,v.armaVec);
    }
    void append_row(const bVec &v) {
        armaMat.resize(armaMat.n_rows+1,armaMat.n_cols);
        set_row(armaMat.n_rows-1,v);
    }
    void append_col(const bVec &v) {
        armaMat.resize(armaMat.n_rows,armaMat.n_cols+1);
        set_col(armaMat.n_cols-1,v);
    }
    bMat transpose() const {
        bMat toReturn;
        toReturn.armaMat=armaMat.st();
        return(toReturn);
    }
    bMat T() const {
        return transpose();
    }
    bMat hermitian_transpose() const {
        bMat toReturn;
        toReturn.armaMat=armaMat.t();
        return(toReturn);
    }
    bMat H() const {
        return hermitian_transpose();
    }
    bMat& operator=(char t) {
        armaMat.zeros();
        armaMat+=t;
	return *this;
    }
    bMat& operator=(const bMat &m) {
        armaMat=m.armaMat;
	return *this;
    }
    bMat& operator=(const std::string &str) {
        armaMat=arma::Mat<char>(str);
	return *this;
    }
    bMat& operator=(const char *str) {
        armaMat=arma::Mat<char>(str);
	return *this;
    }
    bMat& operator+=(const bMat &m) {
        for(arma::uword cnt=0; cnt<armaMat.n_elem; cnt++) armaMat(cnt)^=m.armaMat(cnt);
	return *this;
    }
    bVec operator=( bMat &v) {
        bVec a;
        a.armaVec=v.armaMat;
        return a;
    };
    bMat& operator+=(char t) {
        for(arma::uword cnt=0; cnt<armaMat.n_elem; cnt++) armaMat(cnt)^=t;
	return *this;
    }
    bMat& operator-=(const bMat &m) {
      for(arma::uword cnt=0; cnt<armaMat.n_elem; cnt++) armaMat(cnt)^=m.armaMat(cnt);return *this;
    }
    bMat& operator-=(char t) {
      for(arma::uword cnt=0; cnt<armaMat.n_elem; cnt++) armaMat(cnt)^=t;return *this;
    }
    bMat& operator*=(const bMat &m) {
      for(arma::uword cnt=0; cnt<armaMat.n_elem; cnt++) armaMat(cnt)&=m.armaMat(cnt);return *this;
    }
    bMat& operator*=(char t) {
      for(arma::uword cnt=0; cnt<armaMat.n_elem; cnt++) armaMat(cnt)&=t;return *this;
    }
    bMat& operator/=(char t) {
      for(arma::uword cnt=0; cnt<armaMat.n_elem; cnt++) armaMat(cnt)|=t;return *this;
    }
    bMat& operator/=(const bMat &m) {
      for(arma::uword cnt=0; cnt<armaMat.n_elem; cnt++) armaMat(cnt)|=m.armaMat(cnt);return *this;
    }
    bool operator==(const bMat &m) const {
        arma::umat out=(armaMat==m.armaMat);
        return(arma::sum(arma::sum(out))==out.size());
    }
    bool operator!=(const bMat &m) const {
        arma::umat out=(armaMat==m.armaMat);
        return(arma::sum(arma::sum(out))!=out.size());
    }
    bool in_range(int r, int c) const {
        return(armaMat.in_range((arma::uword)r,(arma::uword)c));
    }


    typedef arma::Mat<char>::iterator iterator;
    typedef arma::Mat<char>::const_iterator const_iterator;

    iterator begin() { return armaMat.begin(); }

    const_iterator begin() const { return armaMat.begin(); }

    iterator end() { return armaMat.end(); }

    const_iterator end() const { return armaMat.end(); }



};

bMat concat_horizontal(const bMat &m1,const bMat &m2);
bMat concat_vertical(const bMat &m1,const bMat &m2);
bMat operator+(const bMat &m1, const bMat &m2);
bMat operator+(const bMat &m, char t);
bMat operator+(char t, const bMat &m);
bMat operator-(const bMat &m1, const bMat &m2);
bMat operator-(const bMat &m, char t);
bMat operator-(char t, const bMat &m);
bMat operator-(const bMat &m);
bMat operator*(const bMat &m1, const bMat &m2);
bVec operator*(const bVec &v, const bMat &m);
bVec operator*(const bMat &m, const bVec &v);
bMat operator*(const bMat &m, char t);
bMat operator*(char t, const bMat &m);
bMat elem_mult(const bMat &m1, const bMat &m2);
bMat operator/(const bMat &m, char t);
bMat operator/(char t, const bMat &m);
bMat elem_div(const bMat &m1, const bMat &m2);
std::ostream &operator<<(std::ostream &os, const bMat &m);

template<class Num_T>  class Mat
{
public:

    arma::Mat<Num_T> armaMat;

    Mat():armaMat() {};
    Mat(int rows, int cols):armaMat((arma::uword)rows,(arma::uword)cols) {}
    Mat(const Mat<Num_T> &m) {
        armaMat=m.armaMat;
    }
    Mat(Mat<Num_T> &m) {
        armaMat=m.armaMat;
    }
    Mat(const Vec<Num_T> &v) {
        armaMat=v.armaVec;
    }

    Mat(const std::string &str):armaMat(str) {}
    Mat(const char *str):armaMat(str) {}
    Mat(arma::Mat<Num_T> aMat):armaMat(aMat) {}


    Mat(const std::vector<Num_T>& x):armaMat(x) {};
    const Mat& operator=(const std::vector<Num_T>& x) {
        armaMat=x;
        return *this;
    };

#if defined(ARMA_USE_CXX11)
    Mat(const std::initializer_list<Num_T>& list):armaMat(list) {};
    const Mat& operator=(const std::initializer_list<Num_T>& list) {
        armaMat=list;
        return *this;
    };
#endif

    template<typename T1> Mat(const arma::Base<Num_T,T1>& X):armaMat(X) {};
    template<typename T1>  const Mat&  operator=(const arma::Base<Num_T,T1>& X) {
        armaMat=X;
        return *this;
    };

    template<typename T1, typename T2>
    explicit Mat(const Mat<T1>& A, const Mat<T2>& B):armaMat(A.armaMat,B.armaMat) {};
    int cols() const {
        return armaMat.n_cols;
    }
    int rows() const {
        return armaMat.n_rows;
    }
    int size() const {
        return armaMat.n_elem;
    }
    void set_size(int rows, int cols, bool copy = false) {
        if(copy) armaMat.resize((arma::uword)rows,(arma::uword)cols);
        else {
	  armaMat.set_size((arma::uword)rows,(arma::uword)cols);
	  armaMat.zeros();
	}
    }
    void zeros() {
        armaMat.zeros();
    }
    void clear() {
        zeros();
    }
    void ones() {
        armaMat.ones();
    }
    void set(const std::string &str) {
        armaMat=arma::Mat<Num_T>(str);
    }
    void set(const char *str) {
        armaMat=arma::Mat<Num_T>(str);
    }
    const Num_T operator()(int r, int c) const {
        return(armaMat((arma::uword)r,(arma::uword)c));
    }
    Num_T &operator()(int r, int c) {
        return(armaMat((arma::uword)r,(arma::uword)c));
    }
    const Num_T &operator()(int i) const {
        return(armaMat((arma::uword)i));
    }
    Num_T &operator()(int i) {
        return(armaMat((arma::uword)i));
    }
    const Num_T get(int r, int c) const {
        return(armaMat((arma::uword)r,(arma::uword)c));
    }
    const Num_T get(int i) const {
        return(armaMat((arma::uword)i));
    }
    void set(int r, int c, Num_T t) {
        armaMat((arma::uword)r,(arma::uword)c)=t;
    }
    Mat<Num_T> operator()(int r1, int r2, int c1, int c2) const {
        Mat<Num_T> a;
        a.armaMat=armaMat.submat((arma::uword)r1,(arma::uword)c1,(arma::uword)r2,(arma::uword)c2);
        return(a);
    }
    Mat<Num_T> get(int r1, int r2, int c1, int c2) const {
        Mat<Num_T> a;
        a.armaMat=armaMat.submat((arma::uword)r1,(arma::uword)c1,(arma::uword)r2,(arma::uword)c2);
        return(a);
    }
    Vec<Num_T> get_row(int r) const {
        Vec<Num_T> a;
        a.armaVec=arma::Col<Num_T>((armaMat.row((arma::uword)r)).st());
        return(a);
    }
    Mat<Num_T> get_rows(int r1, int r2) const {
        Mat<Num_T> a;
        a.armaMat=armaMat.rows((arma::uword)r1,(arma::uword)r2);
        return(a);
    }
    Mat<Num_T> get_rows(const Vec<int> &indexlist) const {
        Mat<Num_T> a;
        a.armaMat=armaMat.rows(getUVec(indexlist.armaVec));
        return(a);
    }
    Vec<Num_T> get_col(int c) const {
        Vec<Num_T> a;
        a.armaVec=arma::Col<Num_T>(armaMat.col((arma::uword)c));
        return(a);
    }
    Mat<Num_T> get_cols(int c1, int c2) const {
        Mat<Num_T> a;
        a.armaMat=armaMat.cols((arma::uword)c1,(arma::uword)c2);
        return(a);
    }
    Mat<Num_T> get_cols(const Vec<int> &indexlist) const {
        Mat<Num_T> a;
        a.armaMat=armaMat.cols(getUVec(indexlist.armaVec));
        return(a);
    }
    void set_row(int r, const Vec<Num_T> &v) {
        armaMat.row((arma::uword)r)=arma::Mat<Num_T>(v.armaVec.st());
    }
    void set_col(int c, const Vec<Num_T> &v) {
        armaMat.col((arma::uword)c)=arma::Mat<Num_T>(v.armaVec);
    }
    void set_rows(int r, const Mat<Num_T> &m) {
        armaMat.submat( span((arma::uword)r,((arma::uword)r)+m.armaMat.n_rows-1),span::all)=m.armaMat;
    }
    void set_cols(int c, const Mat<Num_T> &m) {
        armaMat.submat(span::all, span((arma::uword)c,((arma::uword)c)+m.armaMat.n_cols-1))=m.armaMat;
    }
    void copy_row(int to, int from) {
        armaMat.row((arma::uword)to)=armaMat.row((arma::uword)from);
    }
    void copy_col(int to, int from) {
        armaMat.col((arma::uword)to)=armaMat.col((arma::uword)from);
    }
    void swap_rows(int r1, int r2) {
        armaMat.swap_rows((arma::uword)r1,(arma::uword)r2);
    }
    void swap_cols(int c1, int c2) {
        armaMat.swap_cols((arma::uword)c1,(arma::uword)c2);
    }
    void set_submatrix(int r1, int r2, int c1, int c2, const Mat<Num_T> &m) {
        armaMat.submat( span((arma::uword)r1,(arma::uword)r2),span((arma::uword)c1,(arma::uword)c2))=m.armaMat;
    }
    void set_submatrix(int r, int c, const Mat<Num_T> &m) {
        armaMat.submat( span((arma::uword)r,((arma::uword)r)+m.armaMat.n_rows-1),span((arma::uword)c,((arma::uword)c)+m.armaMat.n_cols-1))=m.armaMat;
    }
    
    void set_submatrix(int r1, int r2, int c1, int c2, Num_T t) {
        Mat<Num_T> m;
        m.armaMat.zeros((arma::uword)(r2-r1+1),(arma::uword)(c2-c1+1));
        m+=t;
        set_submatrix(r1,r2,c1,c2,m);
    }
    void del_row(int r) {
        armaMat.shed_row((arma::uword)r);
    }
    void del_rows(int r1, int r2) {
        armaMat.shed_rows((arma::uword)r1,(arma::uword)r2);
    }
    void del_col(int c) {
        armaMat.shed_col((arma::uword)c);
    }
    void del_cols(int c1, int c2) {
        armaMat.shed_cols((arma::uword)c1,(arma::uword)c2);
    }
    void ins_row(int r, const Vec<Num_T> &v) {
        armaMat.insert_rows((arma::uword)r,v.armaVec.st());
    }
    void ins_col(int c, const Vec<Num_T> &v) {
        armaMat.insert_cols((arma::uword)c,v.armaVec);
    }
    void append_row(const Vec<Num_T> &v) {
        armaMat.resize(armaMat.n_rows+1,armaMat.n_cols);
        set_row(armaMat.n_rows-1,v);
    }
    void append_col(const Vec<Num_T> &v) {
        armaMat.resize(armaMat.n_rows,armaMat.n_cols+1);
        set_col(armaMat.n_cols-1,v);
    }
    Mat<Num_T> transpose() const {
        Mat<Num_T> toReturn;
        toReturn.armaMat=armaMat.st();
        return(toReturn);
    }
    Mat<Num_T> T() const {
        return transpose();
    }
    Mat<Num_T> hermitian_transpose() const {
        Mat<Num_T> toReturn;
        toReturn.armaMat=armaMat.t();
        return(toReturn);
    }
    Mat<Num_T> H() const {
        return hermitian_transpose();
    }
    Mat<Num_T>& operator=(Num_T t) {
        armaMat.zeros();
        armaMat+=t;
	return *this;
    }
    Mat<Num_T>& operator=(const Mat<Num_T> &m) {
        armaMat=m.armaMat;
	return *this;
    }
    Mat<Num_T>& operator=(const std::string &str) {
        armaMat=arma::Mat<Num_T>(str);
	return *this;
    }
    Mat<Num_T>& operator=(const char *str) {
        armaMat=arma::Mat<Num_T>(str);
	return *this;
    }
    Mat<Num_T>& operator+=(const Mat<Num_T> &m) {
        armaMat+=m.armaMat;
	return *this;
    }
    Mat<Num_T>& operator+=(Num_T t) {
        armaMat+=t;
	return *this;
    }
    Mat<Num_T>& operator-=(const Mat<Num_T> &m) {
        armaMat-=m.armaMat;
	return *this;
    }
    Mat<Num_T>& operator-=(Num_T t) {
        armaMat-=t;
	return *this;
    }
    Mat<Num_T>& operator*=(const Mat<Num_T> &m) {
        armaMat*=m.armaMat;
	return *this;
    }
    Mat<Num_T>& operator*=(Num_T t) {
        armaMat*=t;
	return *this;
    }
    Mat<Num_T>& operator/=(Num_T t) {
        armaMat/=t;
	return *this;
    }
    Mat<Num_T>& operator/=(const Mat<Num_T> &m) {
        armaMat/=m.armaMat;
	return *this;
    }
    bool operator==(const Mat<Num_T> &m) const {
        arma::umat out=(armaMat==m.armaMat);
        return(arma::sum(arma::sum(out))==out.size());
    }
    bool operator!=(const Mat<Num_T> &m) const {
        arma::umat out=(armaMat==m.armaMat);
        return(arma::sum(sum(out))!=out.size());
    }
    bool in_range(int r, int c) const {
        return(armaMat.in_range((arma::uword)r,(arma::uword)c));
    }
    typedef typename arma::Mat<Num_T>::iterator iterator;
    typedef typename arma::Mat<Num_T>::const_iterator const_iterator;

    iterator begin() { return armaMat.begin(); }

    const_iterator begin() const { return armaMat.begin(); }

    iterator end() { return armaMat.end(); }

    const_iterator end() const { return armaMat.end(); }
};

std::istream &operator>>(std::istream &is, bMat &m);


template<> class Mat<Vec<double> >{
//dummy function for catching some error
};

template<> class Mat<Vec<unsigned int> >{
//dummy function for catching some error
};

template<> class Mat<Vec< std::complex<double> > >{
//dummy function for catching some error
};


template <class Num_T> std::istream &operator>>(std::istream &is, Mat<Num_T> &m)
{
    std::ostringstream buffer;
    bool started = false;
    bool finished = false;
    bool brackets = false;
    bool within_double_brackets = false;
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



template<class Num_T>   Mat<Num_T> concat_horizontal(const Mat<Num_T> &m1,const Mat<Num_T> &m2) {
    Mat<Num_T> toReturn;
    toReturn.armaMat=join_rows(m1.armaMat,m2.armaMat);
    return(toReturn);
}
template<class Num_T>   Mat<Num_T> concat_vertical(const Mat<Num_T> &m1,const Mat<Num_T> &m2) {
    Mat<Num_T> toReturn;
    toReturn.armaMat=join_cols(m1.armaMat,m2.armaMat);
    return(toReturn);
}
template<class Num_T>   Mat<Num_T> operator+(const Mat<Num_T> &m1, const Mat<Num_T> &m2) {
    Mat<Num_T> toReturn;
    toReturn.armaMat=m1.armaMat+m2.armaMat;
    return(toReturn);
}
template<class Num_T>   Mat<Num_T> operator+(const Mat<Num_T> &m, Num_T t) {
    Mat<Num_T> toReturn;
    toReturn.armaMat=m.armaMat+t;
    return(toReturn);
}
template<class Num_T>   Mat<Num_T> operator+(Num_T t, const Mat<Num_T> &m) {
    Mat<Num_T> toReturn;
    toReturn.armaMat=m.armaMat+t;
    return(toReturn);
}
template<class Num_T>   Mat<Num_T> operator-(const Mat<Num_T> &m1, const Mat<Num_T> &m2) {
    Mat<Num_T> toReturn;
    toReturn.armaMat=m1.armaMat-m2.armaMat;
    return(toReturn);
}
template<class Num_T>   Mat<Num_T> operator-(const Mat<Num_T> &m, Num_T t) {
    Mat<Num_T> toReturn;
    toReturn.armaMat=m.armaMat-t;
    return(toReturn);
}
template<class Num_T>   Mat<Num_T> operator-(Num_T t, const Mat<Num_T> &m) {
    Mat<Num_T> toReturn;
    toReturn.armaMat=m.armaMat-t;
    return(toReturn);
}
template<class Num_T>   Mat<Num_T> operator-(const Mat<Num_T> &m) {
    Mat<Num_T> toReturn;
    toReturn.armaMat=-m.armaMat;
    return(toReturn);
}
template<class Num_T>   Mat<Num_T> operator*(const Mat<Num_T> &m1, const Mat<Num_T> &m2) {
    Mat<Num_T> toReturn;
    toReturn.armaMat=m1.armaMat*m2.armaMat;
    return(toReturn);
}
template<class Num_T>   Vec<Num_T> operator*(const Vec<Num_T> &v, const Mat<Num_T> &m) {

  if(v.length()!=m.rows()){cout<<"[both:] Dimension Mismatch in operator* of Mat..."<<endl;std::abort();}

  Vec<Num_T> toReturn;
  Mat<Num_T> temp(1,m.rows());
  temp.set_row(0,v);
  toReturn = (temp*m).get_row(0);

  return(toReturn);
}
template<class Num_T>   Vec<Num_T> operator*(const Mat<Num_T> &m, const Vec<Num_T> &v) {

  if(v.length()!=m.cols()){cout<<"[both:] Dimension Mismatch in operator* of Mat..."<<endl;std::abort();}

  Vec<Num_T> toReturn;
  Mat<Num_T> temp(m.cols(),1);
  temp.set_col(0,v);
  toReturn = (m*temp).get_col(0);

    return(toReturn);

}
template<class Num_T>   Mat<Num_T> operator*(const Mat<Num_T> &m, Num_T t) {
    Mat<Num_T> toReturn;
    toReturn.armaMat=m.armaMat*t;
    return(toReturn);
}
template<class Num_T>   Mat<Num_T> operator*(Num_T t, const Mat<Num_T> &m) {
    Mat<Num_T> toReturn;
    toReturn.armaMat=m.armaMat*t;
    return(toReturn);
}
template<class Num_T>   Mat<Num_T> elem_mult(const Mat<Num_T> &m1, const Mat<Num_T> &m2) {
    Mat<Num_T> toReturn;
    toReturn.armaMat=m1.armaMat%m2.armaMat;
    return(toReturn);
}
template<class Num_T>   Mat<Num_T> operator/(const Mat<Num_T> &m, Num_T t) {
    Mat<Num_T> toReturn;
    toReturn.armaMat=m.armaMat/t;
    return(toReturn);
}
template<class Num_T>   Mat<Num_T> operator/(Num_T t, const Mat<Num_T> &m) {
    Mat<Num_T> toReturn;
    toReturn.armaMat=m.armaMat/t;
    return(toReturn);
}
template<class Num_T>   Mat<Num_T> elem_div(const Mat<Num_T> &m1, const Mat<Num_T> &m2) {
    Mat<Num_T> toReturn;
    toReturn.armaMat=m1.armaMat/m2.armaMat;
    return(toReturn);
}


typedef Mat<double> dMat;
typedef Mat< std::complex<double> > cMat;
typedef Mat<int> iMat;
typedef Mat<short int> sMat;

template <class Num_T>std::ostream &operator<<(std::ostream &os, const Mat<Num_T> &m) {

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

template<class Num_T> Mat<Num_T>  repmat(const Mat<Num_T> &A, int r, int c)
{
  Mat<Num_T>temp;
  temp.armaMat=arma::repmat((const arma::Mat<Num_T> &)(A.armaMat),r,c);
  return temp;
}

template<class Num_T> Mat<Num_T>  repmat(const Vec<Num_T> &A, int r, int c)
{
  Mat<Num_T>temp;
  temp.armaMat=arma::repmat((const arma::Col<Num_T> &)(A.armaVec),r,c);
  return temp;
}

template <class T> bMat to_bmat(const Mat<T> &m)
{
    bMat temp(m.rows(), m.cols());
    for (int i = 0; i < temp.rows(); ++i) {
        for (int j = 0; j < temp.cols(); ++j) {
            temp(i, j) = m(i, j)!=0;
        }
    }
    return temp;
}

template <class T> sMat to_smat(const Mat<T> &m)
{
    sMat temp(m.rows(), m.cols());
    for (int i = 0; i < temp.rows(); ++i) {
        for (int j = 0; j < temp.cols(); ++j) {
            temp(i, j) = (short)m(i, j);
        }
    }
    return temp;
}

template <class T>iMat to_imat(const Mat<T> &m)
{
    iMat temp(m.rows(), m.cols());
    for (int i = 0; i < temp.rows(); ++i) {
        for (int j = 0; j < temp.cols(); ++j) {
            temp(i, j) = (int)m(i, j);
        }
    }
    return temp;
}

template <class T>iVec to_ivec(const Mat<T> &m)
{
  iVec temp(m.rows()*m.cols());
  for (int i = 0, cnt = 0; i < m.rows(); ++i) {
    for (int j = 0; j < m.cols(); ++j, ++cnt) {
      temp(cnt) = (int)m(i, j);
    }
  }
  return temp;
}

template <class T> dMat to_mat(const Mat<T> &m)
{
    dMat temp(m.rows(), m.cols());
    for (int i = 0; i < temp.rows(); ++i) {
        for (int j = 0; j < temp.cols(); ++j) {
            temp(i, j) = (double)m(i, j);
        }
    }
    return temp;
}

template <class T> cMat to_cmat(const Mat<T> m)
{
    cMat temp(m.rows(), m.cols());
    for (int i = 0; i < temp.rows(); ++i) {
        for (int j = 0; j < temp.cols(); ++j) {
            temp(i, j) = std::complex<double>((double)m(i, j), 0.0);
        }
    }
    return temp;
}

cMat to_cmat(const cMat& m);

template <class T>cMat to_cmat(const Mat<T> &real, const Mat<T> &imag)
{
    if((real.rows() != imag.rows())  || (real.cols() != imag.cols()))
    {
        cout<<"[both:] to_cmat(): real and imag part sizes does not match"<<endl;
        abort();
    }
    cMat temp(real.rows(), real.cols());
    for (int i = 0; i < temp.rows(); ++i) {
        for (int j = 0; j < temp.cols(); ++j) {
            temp(i, j) = std::complex<double>((double)(real(i, j)),(double)(imag(i, j)));
        }
    }
    return temp;
}

bMat to_bmat(const bMat &m);
sMat to_smat(const bMat &m);
iMat to_imat(const bMat &m);
dMat to_mat(const bMat &m);
cMat to_cmat(const bMat &m);

typedef Mat<double> mat;
typedef Mat< std::complex<double> > cmat;
typedef Mat<int> imat;
typedef Mat<short int> smat;
typedef bMat bmat;


  template<typename T1, typename T2,class T3> Mat<T2> unary_applyFunction(Mat<T1> & ip,T3 binaryFunctionPointer)
  {
    Mat<T2> op(ip.rows(),ip.cols());
    typename arma::Mat<T1>::iterator mit=ip.armaMat.begin();
    typename arma::Mat<T2>::iterator  oit=op.armaMat.begin();
    for(;mit!=ip.armaMat.end();++mit,++oit)
    {
      *oit=(T2)binaryFunctionPointer(*mit);
    }
    return op;
  }







  template<typename T1, class T2,class T3> Mat<T1> unary_genFunction(unsigned int rows,int cols,T2 convFunction,T3 genFunction)
  {
    Mat<T1> op(rows,cols);

    for(typename arma::Mat<T1>::iterator  oit=op.armaMat.begin();oit!=op.armaMat.end();++oit)
    {
      *oit=(T1)convFunction(genFunction);
    }
    return op;
  }

template<typename T0, typename T1,typename T2,class binaryOp>  Mat<T0> binary_applyFunction(Mat<T1> & ip1,Mat<T2> & ip2,binaryOp binaryObjectFunction)
  {///T0=T1 binaryop T2
    Mat<T0> op(ip1.rows(),ip1.cols());
    typename arma::Mat<T1>::iterator mit1=ip1.armaMat.begin();
    typename arma::Mat<T2>::iterator mit2=ip2.armaMat.begin();

    typename arma::Mat<T0>::iterator  oit=op.armaMat.begin();
    for(;mit1!=ip1.armaMat.end();++mit1,++mit2,++oit)
    {
      *oit=(T0)binaryObjectFunction(*mit1,*mit2);
    }
    return op;
  }

  template<typename T0, typename T1,typename T2,class binaryOp>  Mat<T0> binary_applyFunction_scalar(Mat<T1> & ip1,T2 & ip2,binaryOp binaryObjectFunction)
  {///T0=T1 binaryop IP2
    Mat<T0> op(ip1.rows(),ip1.cols());
    typename arma::Mat<T1>::iterator mit1=ip1.armaMat.begin();

    typename arma::Mat<T0>::iterator  oit=op.armaMat.begin();
    for(;mit1!=ip1.armaMat.end();++mit1,++oit)
    {
      *oit=(T0)binaryObjectFunction(*mit1,ip2);
    }
    return op;
  }

  template<typename T0, typename T1,typename T2,class binaryOp>  Mat<T0> scalar_binary_applyFunction(const T1 & ip2,const Mat<T2> & ip1,binaryOp binaryObjectFunction)
  {///T1=IP2 binaryop T1
    Mat<T0> op(ip1.rows(),ip1.cols());
    typename arma::Mat<T2>::const_iterator mit1=ip1.armaMat.begin();

    typename arma::Mat<T0>::iterator  oit=op.armaMat.begin();
    for(;mit1!=ip1.armaMat.end();++mit1,++oit)
    {
      *oit=(T0)binaryObjectFunction(ip2,*mit1);
    }
    return op;
  }


mat inv(const mat & m);
cmat inv(const cmat & m);
vec solve(const mat & a,const vec & b);


mat operator+ (const double &s, const mat &m);
mat operator+ (const short &s, const mat &m);



//following functions are automatically generated using a perl script.....

mat operator + (const int  &s , const mat  &m);  //Addition operator for int and mat.
mat operator + (const mat  &m , const double  &s); //Addition operator for mat and double.
mat operator + (const mat  &m , const short  &s); //Addition operator for mat and short.
mat operator + (const mat  &m , const int  &s); //Addition operator for mat and int.
mat operator - (const double  &s , const mat  &m); //Subtraction operator for double and mat.
mat operator - (const short  &s , const mat  &m); //Subtraction operator for short and mat.
mat operator - (const int  &s , const mat  &m); //Subtraction operator for int and mat.
mat operator - (const mat  &m , const double  &s); //Subtraction operator for mat and double.
mat operator - (const mat  &m , const short  &s); //Subtraction operator for mat and short.
mat operator - (const mat  &m , const int  &s); //Subtraction operator for mat and int.
mat operator * (const double  &s , const mat  &m); //Multiplication operator for double and mat.
mat operator * (const short  &s , const mat  &m); //Multiplication operator for short and mat.
mat operator * (const int  &s , const mat  &m); //Multiplication operator for int and mat.
mat operator * (const mat  &m , const double  &s); //Multiplication operator for mat and double.
mat operator * (const mat  &m , const short  &s); //Multiplication operator for mat and short.
mat operator * (const mat  &m , const int  &s); //Multiplication operator for mat and int.
mat operator / (const mat  &m , const double  &s); //Division operator for mat and double.
mat operator / (const mat  &m , const short  &s); //Division operator for mat and short.
mat operator / (const mat  &m , const int  &s); //Division operator for mat and int.
cmat operator + (const double  &s , const cmat  &m);          //Addition operator for double and cmat.
cmat operator - (const double  &s , const cmat  &m);          //Subtraction operator for double and cmat.
cmat operator * (const double  &s , const cmat  &m);          //Multiplication operator for double and cmat.
cmat operator / (const cmat  &m , const double  &s);           //Division operator for cmat and double.
cmat operator + (const cmat  &m , const double  &s);          //Addition operator for double and cmat.
cmat operator - (const cmat  &m , const double  &s);          //Subtraction operator for double and cmat.
cmat operator * (const cmat  &m , const double  &s);          //Multiplication operator for double and cmat.
cmat operator * (const std::complex< double >  &s , const mat  &m);          // Multiplication operator for complex<double> and mat.
cmat operator * (const mat  &m , const std::complex< double >  &s);           // Multiplication operator for mat and complex<double>
mat operator + (const bmat  &a , const mat  &b);  //Addition operator for bmat and mat.
mat operator + (const smat  &a , const mat  &b); //Addition operator for smat and mat.
mat operator + (const imat  &a , const mat  &b); // Addition operator for imat and mat.
mat operator + (const mat  &a , const bmat  &b); //Addition operator for mat and bmat.
mat operator + (const mat  &a , const smat  &b); //Addition operator for mat and smat.
mat operator + (const mat  &a , const imat  &b); //Addition operator for mat and imat.
mat operator - (const bmat  &a , const mat  &b); //Subtraction operator for bmat and mat.
mat operator - (const smat  &a , const mat  &b); //Subtraction operator for smat and mat.
mat operator - (const imat  &a , const mat  &b); //Subtraction operator for imat and mat.
mat operator - (const mat  &a , const bmat  &b); //Subtraction operator for mat and bmat.
mat operator - (const mat  &a , const smat  &b); //Subtraction operator for mat and smat.
mat operator - (const mat  &a , const imat  &b); //Subtraction operator for mat and imat.
cmat operator + (const bmat  &a , const cmat  &b);          //Addition operator for bmat and cmat.
cmat operator + (const smat  &a , const cmat  &b);          //Addition operator for smat and cmat.
cmat operator + (const imat  &a , const cmat  &b);          //Addition operator for imat and cmat.
cmat operator + (const mat  &a , const cmat  &b);          //Addition operator for mat and cmat.
cmat operator + (const cmat  &a , const bmat  &b);          //Addition operator for cmat and bmat.
cmat operator + (const cmat  &a , const smat  &b);          //Addition operator for cmat and smat.
cmat operator + (const cmat  &a , const imat  &b);          //Addition operator for cmat and imat.
cmat operator + (const cmat  &a , const mat  &b);          //Addition operator for cmat and mat.
cmat operator - (const bmat  &a , const cmat  &b);          //Subtraction operator for bmat and cmat.
cmat operator - (const smat  &a , const cmat  &b);          //Subtraction operator for smat and cmat.
cmat operator - (const imat  &a , const cmat  &b);          //Subtraction operator for imat and cmat.
cmat operator - (const mat  &a , const cmat  &b);          //Subtraction operator for mat and cmat.
cmat operator - (const cmat  &a , const bmat  &b);          //Subtraction operator for cmat and bmat.
cmat operator - (const cmat  &a , const smat  &b);          //Subtraction operator for cmat and smat.
cmat operator - (const cmat  &a , const imat  &b);          //Subtraction operator for cmat and imat.
cmat operator - (const cmat  &a , const mat  &b);          //Subtraction operator for cmat and mat.
cmat operator * (const mat  &a , const cmat  &b);          //Multiplication operator for mat and cmat.
cmat operator * (const bmat  &a , const cmat  &b);          //Multiplication operator for bmat and cmat.
cmat operator * (const smat  &a , const cmat  &b);          //Multiplication operator for smat and cmat.
cmat operator * (const imat  &a , const cmat  &b);          //Multiplication operator for imat and cmat.
cmat operator * (const cmat  &a , const mat  &b);          //Multiplication operator for cmat and mat.
cmat operator * (const cmat  &a , const bmat  &b);          //Multiplication operator for cmat and bmat.
cmat operator * (const cmat  &a , const smat  &b);          //Multiplication operator for cmat and smat.
cmat operator * (const cmat  &a , const imat  &b);          //Multiplication operator for cmat and imat.






}//namespace wrapper
#endif
