#ifndef _ITPPWRAP_H_
#define _ITPPWRAP_H_


#include <stdlib.h>
#include<map>
#include<vector>
#include<cmath>
#include <complex>
#include<iostream>
#include <string>
#include <sstream>          //added for int to string conversion
#include <complex>
#include <ios>
#include <iomanip>
#include <typeinfo>
#include<cmath>
#include<ctime>

#include<bits/stl_algo.h>

#include <itpp/itbase.h>
#include <itpp/itcomm.h>
#include <itpp/itstat.h>


using namespace itpp;
using namespace std;
//The following variables are not related to parse : is related to MPI support   
extern int currentTaskid; // Used for MPI
extern int nTasks;

static unsigned int currentSimulationSeed;
void randomizeSeed();
void setSeed(unsigned int seed=0);
unsigned int getSeed();

string exec(string cmd);
bool isParseValueBlank(string inputFileName, string stringToParse);

template <class T>  bool parse(string cfile,string variablename,T &value){
  
//   if(isParseValueBlank(cfile,variablename))
//     return false;
  Parser P(cfile);
  P.set_silentmode();
  return P.get(value,variablename);
}

template <class T>ivec to_ivec(const Mat<T> &m)
{
  ivec temp(m.rows()*m.cols());
  for (int i = 0, cnt = 0; i < m.rows(); ++i) {
    for (int j = 0; j < m.cols(); ++j, ++cnt) {
      temp(cnt) = (int)m(i, j);
    }
  }
  return temp;
}

cvec conv(cvec &a, cvec &b);
vec conv(vec &a, vec &b);


#endif
