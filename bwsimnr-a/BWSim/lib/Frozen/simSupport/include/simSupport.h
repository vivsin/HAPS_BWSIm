/*************************************************************************
 * 
 * CEWiT CONFIDENTIAL
 * __________________
 *
 * All Rights Reserved © 2012 CEWiT, India
 *
 * NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
 * and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
 * Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
 * express, printed and signed license for use is strictly forbidden.
 */

#ifndef _SIMSUP_H_
#define _SIMSUP_H_

//This header is altered by CMake during make to include libraries adaptively...
#include "../../../libListConfig.h"

#ifdef USING_ITPP
#include "../../wrapper/itppWrap/include/itppWrap.h"
using namespace itpp;
#else
#include "../../wrapper/armaWrap/include/array.h"
#include "../../wrapper/armaWrap/include/SupportingFunctions.h"
#include "../../wrapper/armaWrap/include/Parse.h"
#include "../../wrapper/armaWrap/include/modulation.h"
#include "../../wrapper/armaWrap/include/svec.h"
#include "../../wrapper/armaWrap/include/smat.h"
#include "../../wrapper/armaWrap/include/sort.h"
using namespace wrapper;
typedef char bin;
#endif

#ifdef USING_SECURITY
#include "../../Security/include/Mainfunctions.h"
#endif

#include<iostream>
#include <sstream>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <sys/time.h>	//! For Time difference in microseconds resolution
#include <map>		//! For captureTime() function
#include <assert.h>     //! For using assert()
#include <iomanip>
#include<vector>
#include<list>
#include<cstdlib>
#include<random>

#include "signal.h"

#include <queue>
#include <ctime>
#include <fstream>
#include <assert.h>
#include<cstdlib>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <unistd.h>	//! For usage of function getpid()

using namespace std;

using std::cout;
using std::endl;

#define SQRT_2 1.41421356
#define SQRT_3 1.73205081

typedef std::complex<double> dComplex;
#define enumToStr(ENUM) std::string(#ENUM)

double besselj(int n,double x);
ivec getUniqueIntegers(int from,int to,int numberofpicks);
int find(imat matrix, int numToFind);
ivec findNot(ivec vector, int numToOmit);
ivec findNot(ivec vector, ivec numsToOmit);
ivec findNot(vec vector, double numToOmit);
ivec find(imat matrix, int numToFind, bool findAll);
vec findUniqueNumbers(vec input); //!extra added
ivec findUniqueNumbers(ivec input);
ivec findUniqueNumbersWithoutSorting(ivec input);
ivec findUniqueNumbers(imat input);
int findClosest(vec input, double number);
int findClosest(ivec input, int number);
int count(ivec vector, int numToCount);
int count(imat matrix, int numToCount);
int findInStringArray(Array<string> stringArray, string stringToFind);
ivec findAllInStringArray(Array<string> stringArray, string stringToFind);
ivec getIntegers(int from,int to, int interval=1);
vec getVector(double from,double to, double interval);
ivec remove(ivec input,int numberToremove);
ivec remove(ivec input,ivec numbersToremove);
cmat ifftNorm(cmat &input, bool isForColumns=true);
cvec ifftNorm(cvec &input);
cmat fftNorm(cmat &input, bool isForColumns=true);
cvec fftNorm(cvec &input);
int pow_i(int x, int y);
double getNormalizedErrorPercent(vec mainInput, vec inputB);
double getNormalizedErrorPercent(cvec mainInput, cvec inputB);
double getNormalizedErrorPercent(mat mainInput, mat inputB);
double getNormalizedErrorPercent(cmat mainInput, cmat inputB);
bvec operator^(const bvec& v1, const bvec& v2);
cvec fftShift(cvec &input);
bool tossACoin(double probabilityOfHead);
ivec mod(ivec &a,int b);
bvec operator > (ivec a,ivec b);
ivec cumprodback(ivec s);
int decode(ivec sizes,ivec p);
ivec encode(ivec sizes,int num);
int circmod(int a,int b);//Maps every integer a into the range 0 to b-1
double circmod(double a, double b);//Maps double a into the range [0,y).
ivec getVectorWithIntegers(int a,...);
vec getVectorWithNumbers(int a,...);
vec circmod(vec a,double b);
inline bool IsFileExists (const std::string& name) {
    
    ifstream f(name.c_str());
    if (f.good()) {
        f.close();
        return true;
    } else {
        f.close();
        return false;
    }   
}
inline int getLineCountFromFile(const std::string& name)
{
    ifstream f(name.c_str());
    if(f.good())
    {
        return count(istreambuf_iterator<char>(f),istreambuf_iterator<char>(),'\n');
    }
    return 0;
}
inline string getDirectory(string fileName)
{
    stringstream ss(fileName);
    string word;
    vector<string> words;
    while(getline(ss, word,'/')) 
    { 
        words.push_back(word); 
    }
    if(words[words.size()-1].find(".")==-1)
    {
        return fileName;
    }
    stringstream dir;
    for(int i=0;i<words.size()-1;i++)
        dir<<words[i]<<"/";
    return dir.str();
}

void createDirectory(string dirName);
bool mergeFiles(string ipFileName,string opFileName="",string ext="temp");

/*! \brief Function used to return simulation run-time(ie time between two consecutive calls of \b captureTime function), in the resolution of microseconds..
 *    \param[in] captureID Used add more flexiblity for the function to run in parallel. captureTime function called with different captureID will never interfere with each other.
 *                        \note If(captureID==0), then captureTime function is called for all the captureID available currently.
 *  \param[in] captureTimeFormat Specifies the action to be taken for the specified captureID as follows,
 *                        "START"     ==> Store the currentTime to startTimes & startClockTimes
 *                        "PAUSE"     ==> Store the time-difference to captureDuration and delete startTimes
 *                        "RERUN"     ==> Check if the process is "PAUSE" 'ed. if then, store the currentTime to startTimes.
 *                        "STOP"      ==> "PAUSE" the process and print the captureDuration. Then delete Everything.
 *                        "DEFAULT"   ==> Proceeds to either "START" or "STOP" as below,
 *                                            If(captureDuration exists)  captureTimeFormat = START
 *                                            Else                        captureTimeFormat = STOP
 * 
 *                        "PRINT"                 ==> "PAUSE" the process, then "RERUN" the process. Then print the captureDuration
 *                        "PRINT_STARTCLOCK"      ==> Print the startClockTimes
 *                        "PRINT_CURRENTCLOCK"    ==> Print the currentClockTime
 * 
 *  \return streamToReturn Contains the difference of time in string format.
 * 
 *  \section some Keynotes upon C-code below,
 *        1. \param startClockTimes[captureID] indicate the start time of the process avoiding "PAUSE" & "RERUN" happened in between
 *        1. \param captureDuration.count(captureID) tells whether the Capturing process(corrresponding to captureID) still exists or not.
 *        2. \param startTimes.count(captureID) tells whether the Capturing process(corrresponding to captureID) is currently paused or not...
 *              */
enum CaptureTimeFormat_E { DEFAULT =0, START, PAUSE, RERUN, STOP, PRINT, PRINT_STARTCLOCK, PRINT_CURRENTCLOCK };
std::string captureTime(const int captureID,CaptureTimeFormat_E captureTimeFormat = DEFAULT);


/*! \brief Parse all elements in inputFile ...
 *  \param[in] inputFileName --> file to parse values
 *  \return --> A 2D map with strings on left of '=' as keys & strings on right of '=' as values
 *  \warning --> No line in input file should contain multiple '=' characters
 *  \note --> input-file can contain comments in C++ format (Function discards those comments)
 *  \note --> To check if "traceFlag1" (as key) exists, then use 'if(output.count("traceFlag1"))'
 *  \note --> To check if "traceFlag2" (as key) exist & has value "true", then use 'if(output["traceFlag1"]=="true")'
 *    */
extern std::map <std::string,std::string> traceValuePerTraceName;



std::map <std::string,std::string> parseAll(std::string inputFileName);
std::vector<std::string> &splitString(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> splitString(const std::string &s, char delim);
std::string convertFileToString(const std::string &fileName);

/*!\brief Function to append a value(Vec or Array) to any of the  Vec or Array type in a more time saving way...
 * */

template<class Num_T>	void append(Vec<Num_T> &baseVector, const Num_T t)
{
    int size = baseVector.size();
    baseVector.set_size(size+1,true);
    baseVector(size) = t;
}

template<class Num_T>	void append(Vec<Num_T> &baseVector, const Vec<Num_T> &newVector)
{
    int size1 = baseVector.size();
    int size2 = newVector.size();
    baseVector.set_size(size1+size2,true);
    if(size2>0) baseVector.set_subvector(size1,newVector);
}



template<class Num_T>	void append(Vec<Num_T> &baseVector, const Vec<Num_T> &newVector1,const Vec<Num_T> &newVector2, const Vec<Num_T> &newVector3)
{
    int size1 = baseVector.size();
    int size2 = newVector1.size();
    int size3 = newVector2.size();
    int size4 = newVector3.size();
    baseVector.set_size(size1+size2+size3+size4,true);
    if(size2>0) baseVector.set_subvector(size1,newVector1);
    if(size3>0) baseVector.set_subvector(size1+size2,newVector2);
    if(size4>0)baseVector.set_subvector(size1+size2+size3,newVector3);
}

template<class Num_T>	void append(Vec<Num_T> &baseVector, const Vec<Num_T> &newVector1,const Vec<Num_T> &newVector2, const Vec<Num_T> &newVector3,const Vec<Num_T> &newVector4)
{
    int size1 = baseVector.size();
    int size2 = newVector1.size();
    int size3 = newVector2.size();
    int size4 = newVector3.size();
    int size5 = newVector3.size();
    baseVector.set_size(size1+size2+size3+size4+size5,true);
    if(size2>0)  baseVector.set_subvector(size1,newVector1);
    if(size3>0)  baseVector.set_subvector(size1+size2,newVector2);
    if(size4>0)  baseVector.set_subvector(size1+size2+size3,newVector3);
    if(size5>0)  baseVector.set_subvector(size1+size2+size3+size4,newVector4);
}

template<class Num_T>	void append(Vec<Num_T> &baseVector, const Vec<Num_T> &newVector1,const Vec<Num_T> &newVector2)
{
    int size1 = baseVector.size();
    int size2 = newVector1.size();
    int size3 = newVector2.size();
    baseVector.set_size(size1+size2+size3,true);
    if(size2>0) baseVector.set_subvector(size1,newVector1);
    if(size3>0) baseVector.set_subvector(size1+size2,newVector2);
}
template<class T>	void append(Array<T> &baseArray, const T &e)
{
    int size = baseArray.size();
    baseArray.set_size(size + 1,true);
    baseArray(size) = e;
    
}

template<class T>	void append(Array<T> &baseArray, const Array<T> &newArray)
{
    int size = baseArray.size();
    baseArray.set_size(size + newArray.size(),true);
    
    for (int i = 0; i < newArray.size(); i++)
        baseArray(size + i) = newArray(i);
}

template<class T>	void append(Array<T> &baseArray, const Array<T> &newArray1,const Array<T> &newArray2)
{
    int size = baseArray.size();
    baseArray.set_size(size + newArray1.size() + newArray2.size(),true);
    
    for (int i = 0; i < newArray1.size(); i++)
        baseArray(size + i) = newArray1(i);
    for (int i = 0; i < newArray2.size(); i++)
        baseArray(size + newArray1.size() + i) = newArray2(i);
}

template<class Num_T> void appendHorizontal(Mat<Num_T> &baseMatrix, const Mat<Num_T> &newMatrix)
{
    int rows1 = baseMatrix.rows(), cols1 = baseMatrix.cols();
    //int rows2 = newMatrix.rows();
    int cols2 = newMatrix.cols();
    
    if(rows1==0)
    {
        baseMatrix=newMatrix;
        return ;
    }
    
    if(baseMatrix.rows() != newMatrix.rows()){ cout<<"appendHorizontal(): Wrong sizes"<<endl;abort();}
    if (cols2 == 0)
        return ;
    
    baseMatrix.set_size(rows1, cols1 + cols2,true);
    baseMatrix.set_submatrix(0,cols1,newMatrix);
}

template<class Num_T> void appendVertical(Mat<Num_T> &baseMatrix, const Mat<Num_T> &newMatrix)
{
    int rows1 = baseMatrix.rows(), cols1 = baseMatrix.cols();
    int rows2 = newMatrix.rows();
    if(cols1==0)
    {
        baseMatrix=newMatrix;
        return ;
    }
    
    if(baseMatrix.cols() != newMatrix.cols()){ cout<<"appendVertical(): Wrong sizes"<<endl;abort();}
    
    if (rows2 == 0)
        return ;
    
    baseMatrix.set_size(rows1+rows2, cols1,true);
    baseMatrix.set_submatrix(rows1,0,newMatrix);
}

template <class T> int find(Vec<T> &vector, T toFind){
    
    for (int vec_cnt=0;vec_cnt<length(vector);vec_cnt++)
        if (vector(vec_cnt)==toFind) return(vec_cnt);
        return(-1);
}

template <class T> int find(vector<T> &vecInput, T toFind){
    
    for (int vec_cnt=0;vec_cnt<vecInput.size();vec_cnt++)
        if (vecInput[vec_cnt]==toFind) return(vec_cnt);
        return(-1);
}

template <class T> ivec find(Vec<T> &vector, T toFind, bool findAll){
    
    ivec toReturn(0);
    for (int vec_cnt=0;vec_cnt<length(vector);vec_cnt++)
    {
        if (vector(vec_cnt)==toFind)
        {
            if(findAll)
                append(toReturn,vec_cnt);
            else
                return(to_ivec(vec_cnt));
        }
    }
    
    return(toReturn);
}

template <class T> ivec find(Array<T> &array, T toFind, bool findAll){
    
    ivec toReturn(0);
    for (int array_cnt=0;array_cnt<array.length();array_cnt++)
    {
        if (array(array_cnt)==toFind)
        {
            if(findAll)
                append(toReturn,array_cnt);
            else
                return(to_ivec(array_cnt));
        }
    }
    
    return(toReturn);
}

template <class T> ivec find(Vec<T> &vector, Vec<T> toFind){
    
    ivec index(0);
    for (int cnt=0;cnt<length(toFind);cnt++)
        append(index,find(vector,toFind(cnt)));
    return(index);
}

template <class T> void deleteInVec(Vec<T> &vector, ivec indices){
    
    if(sum(indices>=vector.length())) {cout<<"Indices exceed vector length() in deleteInVec()..."<<endl;return;}
    ivec temp=findNot(getIntegers(0,vector.length()-1),indices);
    vector=vector(temp);
}

template <class T> void deleteInArray(Array<T> &array, ivec indices){
    
    if(sum(indices>=array.size()))
    {
        cout<<"Indices exceed array length() in deleteInArray()..."<<endl;
        return;    
    }
    ivec temp=findNot(getIntegers(0,array.size()-1),indices);
    Array<int> temp1(temp.length());
    for(int cnt=0;cnt<temp.length();cnt++)
        temp1(cnt)=temp(cnt);
    array=array(temp1);
}
template <class T> int find(Array<T> &array,T elem)
{
    for(int i=0;i<array.length();i++)
        if(array(i)==elem)
            return i;
    return -1;
}

template <class T> Array<T> findUniqueElements(Array<T> &array){
    
    if(array.length()==1)
        return array;
    else
    {
        Array<T> output(1);
        output(0)=array(0);
        for(int cnt1=1;cnt1<array.length();cnt1++)
        {
//             for(int cnt2=0;cnt2<output.length();cnt2++)
//                 if(array(cnt1)!=output(cnt2))
//                     append(output,array(cnt1));
            if(find(output,array(cnt1))==-1)
                append(output,array(cnt1));
        }
        return output;
    }
}


// void deleteElementsInArray(Array<ivec> &array, ivec indices){
// 
//   if(indices.length()>array.size())
//   {
//     cout<<"Indices exceed array length() in deleteInArray()..."<<endl;
//     return;    
//   }
//   
//   Array<ivec> newArray;
//   for(int indx=0; indx<array.size(); indx++)
//   {
//    ivec temp=array(indx);
//    find();
//   }
//   ivec temp=findNot(getIntegers(0,array.size()-1),indices);
//   Array<int> temp1(temp.length());
//   for(int cnt=0;cnt<temp.length();cnt++)
//     temp1(cnt)=temp(cnt);
//   array=array(temp1);
// }

template <class T> Mat<T> mean(Array<Mat<T>> C)
{
    Mat<T> A = C(0);
    for(int i=1;i<C.length();i++)
    {
        A = A + C(i);
    }
    return A / C.length();
}

template <class T> Mat<T> kronAdd(Mat<T> A,Mat<T> B)
{
    Mat<T> C(A.rows()*B.rows(),A.cols()*B.cols());
    
    for (int i = 0; i < A.rows(); i++)
        for (int j = 0; j < A.cols(); j++)
            C.set_submatrix(i * B.rows(), j * B.cols(), A(i, j) + B);
    return C;
}

template <class T> Mat<T> rotateRow(Mat<T> &input, int upShift){
    
    Mat<T> output(input.rows(), input.cols());
    for(int row_cnt=0;row_cnt<input.rows();row_cnt++)
    {
        // Check add to do downShift : (Need to verify) 
        if(upShift<0)
        {
            upShift = input.rows() + upShift;
        }
        output.set_row(row_cnt,input.get_row((row_cnt+upShift)%input.rows()));
    }
    return(output);
}

template <class T> Mat<T> rotateColumn(Mat<T> &input, int rightShift){
    
    Mat<T> output(input.rows(), input.cols());
    for(int col_cnt=0;col_cnt<input.cols();col_cnt++)
    {
        // Check add to do downShift : (Need to verify) 
        if(rightShift<0)
        {
            rightShift = input.rows() + rightShift;
        }
        output.set_col((col_cnt+rightShift)%input.cols(),input.get_col(col_cnt));
    }
    return(output);
}

template <class T> Mat<T> rotateVec(Vec<T> &input, int rightShift){
    
    Vec<T> output(input.length());
    if(rightShift<0)
        rightShift = input.length() + rightShift;
    
    
    for(int cnt=0;cnt<input.length();cnt++)
    {
        // Check add to do downShift : (Need to verify) 
        output((cnt+rightShift)%input.length())=input(cnt);
    }
    return(output);
}

template <class T> bool merge(Mat<T> &baseMatrix, Mat<T> &newMatrix , bmat &mergeFlags){
    
    if(baseMatrix.rows()!=newMatrix.rows() || baseMatrix.cols()!=newMatrix.cols() || baseMatrix.rows()!=mergeFlags.rows() || baseMatrix.cols()!=mergeFlags.cols())
    {
        cout<<"Dimension mismatch in merge()..."<<endl;abort();
        return(false);
    }
    for(int row_cnt=0;row_cnt<baseMatrix.rows();row_cnt++)
        for(int col_cnt=0;col_cnt<baseMatrix.cols();col_cnt++)
            if(mergeFlags(row_cnt,col_cnt))
                baseMatrix(row_cnt,col_cnt)=newMatrix(row_cnt,col_cnt);
            
            return(true);
}

template <class T> Vec<T> randPick(Vec<T> input, int numberOfPicks){
    
    ivec indices=getIntegers(0,input.length()-1);
    Vec<T> output(numberOfPicks);
    int indx;
    for(int cnt=0;cnt<numberOfPicks;cnt++)
    {
        indx=randi(0,indices.length()-1);
        output(cnt)=input(indices(indx));
        indices.del(indx);
    }
    return(output);
}

template <class T> Array<T> randPick(Array<T> input, int numberOfPicks){
    
    ivec indices=getIntegers(0,input.length()-1);
    Array<T> output(numberOfPicks);
    int indx;
    for(int cnt=0;cnt<numberOfPicks;cnt++)
    {
        indx=randi(0,indices.length()-1);
        output(cnt)=input(indices(indx));
        indices.del(indx);
    }
    return(output);
}

template <class T> Vec<T> shuffleVec(Vec<T> input){
    
    Vec<T> output=randPick(input,input.length());
    return(output);
}

template <class T1 > Array< Vec<T1> >  elem_mult(Array< Vec<T1> > &inputArray, Vec<T1> inputVector){
    
    Array< Vec<T1> > outputArray(0);
    if(inputArray(0).length()!=inputVector.length())
    {
        cout<<"[both:] Error ... Dimension mismatch in elem_mult()..."<<endl;
        return(outputArray);
    }
    else
    {
        outputArray.set_length(inputArray.length());
        for(int ary1_cnt=0;ary1_cnt<inputArray.length();ary1_cnt++)
        {
            outputArray(ary1_cnt).set_length(inputArray(ary1_cnt).length());
            for(int vec_cnt=0;vec_cnt<inputArray(ary1_cnt).length();vec_cnt++)
                outputArray(ary1_cnt)(vec_cnt)=inputArray(ary1_cnt)(vec_cnt)*inputVector(vec_cnt);
        }
        return(outputArray);
    }
}

template <class T1 > Array< Array< Vec<T1> > >  elem_mult(Array< Array< Vec<T1> > > &inputArray, Vec<T1> inputVector){
    
    Array< Array< Vec<T1> > > outputArray(inputArray.length());
    
    if(inputArray(0)(0).length()!=inputVector.length())
    {
        cout<<"Dimension mismatch in elem_mult()..."<<endl;
        return(outputArray);
    }
    else
    {
        outputArray.set_length(inputArray.length());
        for(int ary1_cnt=0;ary1_cnt<inputArray.length();ary1_cnt++)
        {
            outputArray(ary1_cnt).set_length(inputArray(ary1_cnt).length());
            for(int ary2_cnt=0;ary2_cnt<inputArray(ary1_cnt).length();ary2_cnt++)
            {
                outputArray(ary1_cnt)(ary2_cnt).set_length(inputArray(ary1_cnt)(ary2_cnt).length());
                for(int vec_cnt=0;vec_cnt<inputArray(ary1_cnt)(ary2_cnt).length();vec_cnt++)
                    outputArray(ary1_cnt)(ary2_cnt)(vec_cnt)=inputArray(ary1_cnt)(ary2_cnt)(vec_cnt)*inputVector(vec_cnt);
            }
        }
        return(outputArray);
    }
}


template <class T> void concatInArrayOfArray(Array< Array< Vec<T> > > &inputArray,Array< Array< Vec<T> > > &newArray){
    
    if(inputArray.length()!=newArray.length() || inputArray(0).length()!=newArray(0).length())
        cout<<"Dimension mismatch... Skipping concatInArrayOfArray()..."<<endl;
    else
        for(int ary1_cnt=0;ary1_cnt<inputArray.length();ary1_cnt++)
            for(int ary2_cnt=0;ary2_cnt<inputArray(ary1_cnt).length();ary2_cnt++)
                append(inputArray(ary1_cnt)(ary2_cnt),newArray(ary1_cnt)(ary2_cnt));
}

template <class T> void concatInArrayOfArray(Array< Array< Mat<T> > > &inputArray,Array< Array< Mat<T> > > &newArray, bool isRowConcat=true){
    
    if(inputArray.length()!=newArray.length() || inputArray(0).length()!=newArray(0).length())
        cout<<"Dimension mismatch... Skipping concatInArrayOfArray()..."<<endl;
    else
        if(isRowConcat)
            for(int ary1_cnt=0;ary1_cnt<inputArray.length();ary1_cnt++)
                for(int ary2_cnt=0;ary2_cnt<inputArray(ary1_cnt).length();ary2_cnt++)
                    appendVertical(inputArray(ary1_cnt)(ary2_cnt),newArray(ary1_cnt)(ary2_cnt));
                else
                    for(int ary1_cnt=0;ary1_cnt<inputArray.length();ary1_cnt++)
                        for(int ary2_cnt=0;ary2_cnt<inputArray(ary1_cnt).length();ary2_cnt++)
                            appendHorizontal(inputArray(ary1_cnt)(ary2_cnt),newArray(ary1_cnt)(ary2_cnt));
}


template <class T> void concatInArray(Array< Vec<T> > &inputArray,Array< Vec<T> > &newArray){
    
    if(inputArray.length()!=newArray.length())
    {
        cout<<"Length : "<<inputArray.length()<<" , "<<newArray.length()<<endl;
        cout<<"Dimension mismatch... Skipping concatInArray()..."<<endl;abort();
    }
    else
        for(int ary_cnt=0;ary_cnt<inputArray.length();ary_cnt++)
            append(inputArray(ary_cnt),newArray(ary_cnt));
}

template <class T> void concatInArray(Array< Mat<T> > &inputArray,Array< Mat<T> > &newArray, bool isRowConcat=true){
    
    if(inputArray.length()!=newArray.length())
    {
        cout<<"Length : "<<inputArray.length()<<" , "<<newArray.length()<<endl;
        cout<<"Dimension mismatch... Skipping concatInArray()..."<<endl;abort();
    }
    else
        if(isRowConcat)
            for(int ary_cnt=0;ary_cnt<inputArray.length();ary_cnt++)
                appendVertical(inputArray(ary_cnt),newArray(ary_cnt));
            else
                for(int ary_cnt=0;ary_cnt<inputArray.length();ary_cnt++)
                    appendHorizontal(inputArray(ary_cnt),newArray(ary_cnt));
}

template <class T> Array< Vec<T> > toColumnArray(Mat<T> &inputMatrix){
    
    Array< Vec<T> > output(inputMatrix.cols());
    for(int ary_cnt=0;ary_cnt<output.length();ary_cnt++)
        output(ary_cnt)=inputMatrix.get_col(ary_cnt);
    return(output);
}

template <class T> Array< Vec<T> > toRowArray(Mat<T> &inputMatrix){
    
    Array< Vec<T> > output(inputMatrix.rows());
    for(int ary_cnt=0;ary_cnt<output.length();ary_cnt++)
        output(ary_cnt)=inputMatrix.get_row(ary_cnt);
    return(output);
}

template <class T> Array< Vec<T> > splitVectorToArray(Vec<T> &inputVector, int outputVectorSize){
    
    int arraySize=inputVector.length()/outputVectorSize;
    if(outputVectorSize*arraySize<inputVector.length()) arraySize++;
    Array< Vec<T> > output(arraySize);
    int endIndx=0;
    for(int ary_cnt=0;ary_cnt<output.length();ary_cnt++)
    {
        endIndx+=outputVectorSize;
        if(endIndx>inputVector.length()) endIndx=inputVector.length();
        output(ary_cnt)=inputVector(ary_cnt*outputVectorSize,endIndx-1);
    }
    return(output);
}

template <class T> Array< Vec<T> > splitVectorToUniformArray(Vec<T> &inputVector, int arraySize){
    
    Array< Vec<T> > output(arraySize);
    if(arraySize>0){
    int startIndx=0;
    int extra = inputVector.length()%arraySize;
    int outputVectorSize=inputVector.length()/arraySize;
    for(int ary_cnt=0;ary_cnt<output.length();ary_cnt++)
    {
        int length = (ary_cnt<extra) ? outputVectorSize+1 : outputVectorSize;
        output(ary_cnt) = inputVector(startIndx,startIndx+length-1);
        startIndx += length; 
    }}
    return(output);
}

template <class T> Vec<T> joinArrayToVector(Array< Vec<T> >& input){
    
    Vec<T> output(0);
    if(input.length())
    {
        output=input(0);
        for(int ary_cnt=1;ary_cnt<input.length();ary_cnt++)
            append(output,input(ary_cnt));
    }
    return(output);
}

template <class T> Array< Array< Vec<T> > > getArrayOfColumnArray(Array< Mat<T> > &inputArrayOfMatrix){
    
    Array< Array< Vec<T> > > output(inputArrayOfMatrix.length());
    for(int ary_cnt=0;ary_cnt<inputArrayOfMatrix.length();ary_cnt++)
        output(ary_cnt)=toColumnArray(inputArrayOfMatrix(ary_cnt));
    return(output);
}

template <class T> Array< Array< Vec<T> > > getArrayOfRowArray(Array< Mat<T> > &inputArrayOfMatrix){
    
    Array< Array< Vec<T> > > output(inputArrayOfMatrix.length());
    for(int ary_cnt=0;ary_cnt<inputArrayOfMatrix.length();ary_cnt++)
        output(ary_cnt)=toRowArray(inputArrayOfMatrix(ary_cnt));
    return(output);
}

template <class T> Array< Vec<T> > getArrayOfColumnVector(Array< Mat<T> > &inputArrayOfMatrix, int columnIndx){
    
    Array< Vec<T> > output(0);
    for(int ary_cnt=0;ary_cnt<inputArrayOfMatrix.length();ary_cnt++)
    {
        if(inputArrayOfMatrix(ary_cnt).cols()<=columnIndx) {cout<<"[both:] Error ...ColumnIndx out of range in getArrayOfColumnVector() ..Returning partial output .."<<endl;return(output);}
        append(output,inputArrayOfMatrix(ary_cnt).get_col(columnIndx));
    }
    return(output);
}

template <class T> Array< Vec<T> > getArrayOfRowVector(Array< Mat<T> > &inputArrayOfMatrix, int rowIndx){
    
    Array< Vec<T> > output(0);
    for(int ary_cnt=0;ary_cnt<inputArrayOfMatrix.length();ary_cnt++)
    {
        if(inputArrayOfMatrix(ary_cnt).rows()<=rowIndx) {cout<<"[both:] Error ...rowIndx out of range in getArrayOfRowVector() ..Returning partial output .."<<endl;return(output);}
        append(output,inputArrayOfMatrix(ary_cnt).get_row(rowIndx));
    }
    return(output);
}

template <class T>  Vec<T> getFromMatrix(Mat<T> &inputMatrix, ivec row1stIndices){
    
    Vec<T> toReturn(0);
    if(sum(row1stIndices>=inputMatrix.size())) {cout<<"Index exceeds matrix dimensions in getFromMatrix.. Returning NULL..."<<endl;return(toReturn);}
    toReturn.set_length(row1stIndices.length());
    for(int indx_cnt=0;indx_cnt<row1stIndices.length();indx_cnt++)
        toReturn(indx_cnt)=inputMatrix(row1stIndices(indx_cnt)%inputMatrix.rows(),row1stIndices(indx_cnt)/inputMatrix.rows());
    return(toReturn);
}

template <class T>  Array< Vec<T> > getFromArrayOfMatrix(Array< Mat<T> > &inputMatrix, ivec row1stIndices){
    
    Array< Vec<T> > toReturn(0);
    
    if(inputMatrix.length()==0) return(toReturn);
    
    Vec<T> temp;
    for(int indx1_cnt=0;indx1_cnt<inputMatrix.length();indx1_cnt++)
    {
        if(sum(row1stIndices>=inputMatrix(indx1_cnt).size())) {cout<<"Index exceeds matrix dimensions in getFromArrayOfMatrix.. Returning Partial Output..."<<endl;return(toReturn);}
        temp.set_size(row1stIndices.length());
        for(int indx2_cnt=0;indx2_cnt<row1stIndices.length();indx2_cnt++)
            temp(indx2_cnt)=inputMatrix(indx1_cnt)(row1stIndices(indx2_cnt)%inputMatrix(indx1_cnt).rows(),row1stIndices(indx2_cnt)/inputMatrix(indx1_cnt).rows());
        
        append(toReturn,temp);
    }
    return(toReturn);
}

template <class T>  Array< Array< Vec<T> > > getFromArrayOfArrayOfMatrix(Array< Array< Mat<T> > > &inputMatrix, ivec row1stIndices){
    
    Array< Array< Vec<T> > > toReturn(0);
    if(inputMatrix.length()==0 ) return(toReturn);
    
    Array< Vec<T> > temp;
    for(int indx1_cnt=0;indx1_cnt<inputMatrix.length();indx1_cnt++)
    {
        temp=getFromArrayOfMatrix(inputMatrix(indx1_cnt),row1stIndices);
        if(temp.length()==0) {cout<<"Index out of range in getFromArrayOfArrayOfMatrix()...Returning partial output..."<<endl;return(toReturn);}
        else
            append(toReturn,temp);
    }
    return(toReturn);
}

template <class T>  Vec<T> getFromVector(Vec<T> &inputArray, ivec indices){
    
    Vec<T> toReturn(0);
    
    if(sum(indices>=inputArray.length())) {cout<<"[both:] Error .. Indices exceed array dimension in getFromArray()...Returning NULL..."<<endl;abort();return(toReturn);}
    
    toReturn.set_length(indices.length());
    for(int indx_cnt=0;indx_cnt<indices.length();indx_cnt++)
        toReturn(indx_cnt)=inputArray(indices(indx_cnt));
    return(toReturn);
}

template <class T>  Array<T> getFromArray(Array<T> &inputArray, ivec indices){
    
    Array<T> toReturn(0);
    
    if(sum(indices>=inputArray.length())) {cout<<"[both:] Error .. Indices exceed array dimension in getFromArray()...Returning NULL..."<<endl;abort();return(toReturn);}
    
    toReturn.set_length(indices.length());
    for(int indx_cnt=0;indx_cnt<indices.length();indx_cnt++)
        toReturn(indx_cnt)=inputArray(indices(indx_cnt));
    return(toReturn);
}

template <class T>  Array< Mat<T> > getFromArrayOfMatrix(Array< Mat<T> > &inputMatrix, int r1, int r2, int c1, int c2){
    
    Array< Mat<T> > toReturn(0);
    int indx_cnt=0;
    for(int indx_cnt=0;indx_cnt<inputMatrix.length();indx_cnt++)
    {
        if(r1<inputMatrix(indx_cnt).rows() && r2<inputMatrix(indx_cnt).rows() && c1<inputMatrix(indx_cnt).cols() && c2<inputMatrix(indx_cnt).cols())
            append(toReturn,inputMatrix(indx_cnt).get(r1,r2,c1,c2));
        else
        {cout<<"Index out-of-range in getFromArrayOfMatrix()...Returning partial output..."<<endl;return(toReturn);}
    }
    return(toReturn);
}


template <class T>  Array< Vec<T> > getFromArrayOfVector(Array< Vec<T> > &inputArray, int from , int to){
    
    
    Array< Vec<T> > toReturn(0);
    if(from>to || to<0 || from<0)
    {
        cout<<from<<","<<to<<","<<inputArray(0).length()<<endl;
        cout<<"[both:] Error in from-to Index in getFromArrayOfVector()..."<<endl;
        return(toReturn);
    }
    for(int indx_cnt=0;indx_cnt<inputArray.length();indx_cnt++)
    {
        if(to>=inputArray(indx_cnt).length() || from>=inputArray(indx_cnt).length())
        {
            cout<<from<<","<<to<<","<<inputArray(indx_cnt).length()<<endl;
            cout<<"[both:] Error in from-to Index in getFromArrayOfVector()..."<<endl;
            return(toReturn);
        }
        append(toReturn,inputArray(indx_cnt)(from,to));
    }
    return(toReturn);
}


template <class T>  Array< Vec<T> > getFromArrayOfVector(Array< Vec<T> > &inputArray, ivec &indices){
    
    Array< Vec<T> > toReturn(0);
    for(int ary_cnt=0;ary_cnt<inputArray.length();ary_cnt++)
    {
        if(sum(indices>=inputArray(ary_cnt).length())) {cout<<"[both:] Error .. Indices exceed array dimension in getFromArrayOfVector()...Returning partial output..."<<endl;return(toReturn);}
        append(toReturn,inputArray(ary_cnt)(indices));
    }
    return(toReturn);
}

template <class T>  Array< Array< Mat<T> > > getFromArrayOfArrayOfMatrix(Array< Array< Mat<T> > > &inputMatrix, int r1, int r2, int c1, int c2){
    
    Array< Array< Mat<T> > > toReturn(0);
    
    Array< Mat<T> > temp;
    for(int indx1_cnt=0;indx1_cnt<inputMatrix.length();indx1_cnt++)
    {
        temp=getFromArrayOfMatrix(inputMatrix(indx1_cnt),r1,r2,c1,c2);
        if(temp.length()==0){ cout<<"Index out of range in getFromArrayOfArrayOfMatrix()...Returning partial output... "<<endl;return(toReturn);}
        else
            append(toReturn,temp);
    }
    return(toReturn);
}

template <class T>  Array< Array< Array< Mat<T> > > > getFromArrayOfArrayOfArrayOfMatrix(Array< Array< Array< Mat<T> > > > &inputMatrix, int r1, int r2, int c1, int c2){
    
    Array< Array< Array< Mat<T> > > > toReturn(0);
    
    Array< Array< Mat<T> > > temp;
    for(int indx1_cnt=0;indx1_cnt<inputMatrix.length();indx1_cnt++)
    {
        temp=getFromArrayOfArrayOfMatrix(inputMatrix(indx1_cnt),r1,r2,c1,c2);
        if(temp.length()==0){ cout<<"Index out of range in getFromArrayOfArrayOfArrayOfMatrix()...Returning partial output... "<<endl;return(toReturn);}
        else
            append(toReturn,temp);
    }
    return(toReturn);
}

template <class T>  void setInVec(Vec<T> &inputVec, ivec indices, Vec<T> values){
    
    if(indices.length() != values.length())
    {
        cout<<"[both:] Error... Indices and the values must be of same size in setInVec()..."<<endl;
        abort();
    }
    
    if(sum(indices>= inputVec.size()))
    {
        cout<<"[both:] Error... Indices exceed vector dimension in setInVec()..."<<endl;
        abort();
    }
    
    
    for(int indx_cnt=0;indx_cnt<indices.length();indx_cnt++)
        inputVec.set(indices(indx_cnt),values(indx_cnt));
}

template <class T>  void setInVec(Vec<T> &inputVec, ivec indices, T value){
    
    if(sum(indices>= inputVec.size()))
    {
        cout<<"[both:] Error... Indices exceed vector dimension in setInVec()..."<<endl;
        abort();
    }
    
    for(int indx_cnt=0;indx_cnt<indices.length();indx_cnt++)
        inputVec.set(indices(indx_cnt),value);
}

template <class T>  void setInArray(Array<T> &inputArray, ivec indices, Array<T> values){
    
    if(indices.length() != values.length())
    {
        cout<<"[both:] Error... Indices and the values must be of same size in setInArray()..."<<endl;
        abort();
    }
    
    if(sum(indices>= inputArray.size()))
    {
        cout<<"[both:] Error... Indices exceed vector dimension in setInArray()..."<<endl;
        abort();
    }
    
    
    for(int indx_cnt=0;indx_cnt<indices.length();indx_cnt++)
        inputArray(indices(indx_cnt))=values(indx_cnt);
}

template <class T>  void setInArray(Array<T> &inputArray, ivec indices, T value){
    
    if(sum(indices>= inputArray.size()))
    {
        cout<<"[both:] Error... Indices exceed vector dimension in setInArray()..."<<endl;
        abort();
    }
    
    
    for(int indx_cnt=0;indx_cnt<indices.length();indx_cnt++)
        inputArray(indices(indx_cnt))=value;
}


template <class T>  void setInMatrix(Mat<T> &inputMatrix, ivec row1stIndices, Vec<T> values){
    
    if(row1stIndices.length() != values.length())
    {
        cout<<"[both:] Error... Indices and the values must be of same size in setInMatrix()..."<<endl;
        return;
    }
    
    if(sum(row1stIndices>= inputMatrix.size()))
    {
        cout<<"[both:] Error... Indices exceed matrix dimension in setInMatrix()..."<<endl;
        return;
    }
    
    
    for(int indx_cnt=0;indx_cnt<row1stIndices.length();indx_cnt++)
        inputMatrix.set(row1stIndices(indx_cnt)%inputMatrix.rows(),row1stIndices(indx_cnt)/inputMatrix.rows(),values(indx_cnt));
}

template <class T>  void setInMatrix(Mat<T> &inputMatrix, ivec row1stIndices, T valueToSet){
    
    if(sum(row1stIndices>= inputMatrix.size()))
    {
        cout<<"[both:] Error... Indices exceed matrix dimension in setInMatrix()..."<<endl;
        return;
    }
    
    for(int indx_cnt=0;indx_cnt<row1stIndices.length();indx_cnt++)
        inputMatrix.set(row1stIndices(indx_cnt)%inputMatrix.rows(),row1stIndices(indx_cnt)/inputMatrix.rows(),valueToSet);
}

template <class T>  Mat<T> formBlockMatrix(Mat<T> &inputMatrix1,Mat<T> &inputMatrix2){
    
    if(inputMatrix1.rows()!=inputMatrix2.rows() || inputMatrix1.cols()!=inputMatrix2.cols())
    {
        cout<<"[both:] Error... Dimension mismatch in formBlockMatrix()..."<<endl;
        abort();
    }
    Mat<T> toReturn(inputMatrix1.rows()*2,inputMatrix1.cols()*2);
    toReturn.clear();
    toReturn.set_submatrix(0,0,inputMatrix1);
    toReturn.set_submatrix(inputMatrix1.rows(),inputMatrix1.cols(),inputMatrix2);
    return(toReturn);
}


void setInMatrix(bmat &inputMatrix, ivec row1stIndices, bvec values);
void setInMatrix(bmat &inputMatrix, ivec row1stIndices, int valueToSet);
void setInVec(bvec &inputVec, ivec indices, bvec values);


void append(bvec &baseVector, const bin t);
void append(bvec &baseVector, const bvec &newVector);
void append(bvec &baseVector, const bvec &newVector1, bvec &newVector2);

template <class T>  void setInArrayOfMatrix(Array< Mat<T> > &inputMatrix, Array< Mat<T> > &newMatrix , int r1, int c1 ){
    
    if(inputMatrix.length() != newMatrix.length())
    {
        cout<<"Dimension mismatch in setInArrayOfMatrix()..."<<endl;
        return;
    }
    for(int ary_cnt=0;ary_cnt<inputMatrix.length();ary_cnt++)
    {
        if(r1>=inputMatrix(ary_cnt).rows() || c1>=inputMatrix(ary_cnt).cols())
        {	cout<<"Index exceeds matrix dimension in setInArrayOfMatrix()...Returning partial output..."<<endl; return; }
        inputMatrix(ary_cnt).set_submatrix(r1,c1,newMatrix(ary_cnt));
    }
    
}

template <class T>  void setInArrayOfMatrix(Array< Mat<T> > &inputMatrix, Array< Vec<T> > &newVector , int index, bool setColumn=true){
    
    if(inputMatrix.length() != newVector.length())
    {
        cout<<"Dimension mismatch in setInArrayOfMatrix()..."<<endl;
        return;
    }
    for(int ary_cnt=0;ary_cnt<inputMatrix.length();ary_cnt++)
    {
        if(setColumn)
        {
            if(inputMatrix(ary_cnt).rows()!=newVector(ary_cnt).length())
            {
                cout<<"Dimension mismatch in setInArrayOfMatrix()..."<<endl;
                return;
            }
            inputMatrix(ary_cnt).set_col(index,newVector(ary_cnt));
        }
        else
        {
            if(inputMatrix(ary_cnt).cols()!=newVector(ary_cnt).length())
            {
                cout<<"Dimension mismatch in setInArrayOfMatrix()..."<<endl;
                return;
            }
            inputMatrix(ary_cnt).set_row(index,newVector(ary_cnt));
        }
    }
}

template <class T>  void setInArrayOfArrayOfMatrix(Array< Array< Mat<T> > > &inputMatrix, Array< Array< Mat<T> > > &newMatrix , int r1, int c1 ){
    
    if(inputMatrix.length() != newMatrix.length())
    {
        cout<<"Dimension mismatch in setInArrayOfMatrix()..."<<endl;
        return;
    }
    for(int ary1_cnt=0;ary1_cnt<inputMatrix.length();ary1_cnt++)
        for(int ary2_cnt=0;ary2_cnt<inputMatrix(ary1_cnt).length();ary2_cnt++)
        {
            if(r1>=inputMatrix(ary1_cnt)(ary2_cnt).rows() || c1>=inputMatrix(ary1_cnt)(ary2_cnt).cols())
            {	cout<<"Index exceeds matrix dimension in setInArrayOfArrayOfMatrix()...Returning partial output..."<<endl; return; }
            inputMatrix(ary1_cnt)(ary2_cnt).set_submatrix(r1,c1,newMatrix(ary1_cnt)(ary2_cnt));
        }
}




template <class T> string toString(const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

template <class T> Vec<T> maxValuePerDimension(Mat<T> &inputMatrix, int dim=1){
    if(dim==1) // max values per column
    {
        Vec<T> toReturn(inputMatrix.cols());
        for(int col_cnt=0;col_cnt<inputMatrix.cols();col_cnt++)
            toReturn(col_cnt)=max(inputMatrix.get_col(col_cnt));
        return toReturn;
    }
    else
    {
        Vec<T> toReturn(inputMatrix.rows());
        for(int row_cnt=0;row_cnt<inputMatrix.rows();row_cnt++)
            toReturn(row_cnt)=max(inputMatrix.get_row(row_cnt));
        return toReturn;
    }
} 


template <class T> ivec maxIndexPerDimension(Mat<T> &inputMatrix, int dim=1){
    
    if(dim==1) // max values per column
    {
        ivec toReturn(inputMatrix.cols());
        for(int col_cnt=0;col_cnt<inputMatrix.cols();col_cnt++)
            toReturn(col_cnt)=max_index(inputMatrix.get_col(col_cnt));
        return toReturn;
    }
    else
    {
        ivec toReturn(inputMatrix.rows());
        for(int row_cnt=0;row_cnt<inputMatrix.rows();row_cnt++)
            toReturn(row_cnt)=max_index(inputMatrix.get_row(row_cnt));
        return toReturn;
    }
}

/*! \brief Cyclically extend the input to the targetLength(by repeating from beginning) & returns it.
 *  \note If targetLength < (length of input vector), then inputVector itself is returned.. */

template <class T> Vec<T> cyclicallySuffix(const Vec<T> inVector, int targetLength)
{
    
    if(inVector.length() >= targetLength)
        return inVector;
    else
    {
        Vec<T> tempVector = inVector;
        while(targetLength > 2*tempVector.length())
            append(tempVector,tempVector);
        
        return concat(tempVector,tempVector(0,targetLength-tempVector.length()-1));
    }
    
}

template <class T> Array< Mat<T> > reShape(Array< Vec<T> > input, int rowCount)
{
    Array< Mat<T> > output(0);
    Mat<T> temp;
    for(int cnt=0;cnt<input.length();cnt++)
    {
        if(input(cnt).length()%rowCount!=0)
        {cout<<"[both:] Error.. Input length must be a multiple of rowCount in reshape()...."<<endl;return(output);}
        
        
        temp.set_size(rowCount,input(cnt).length()/rowCount);
        temp.clear();
        for(int col_cnt=0;col_cnt<input(cnt).length()/rowCount;col_cnt++)
            temp.set_col(col_cnt,input(cnt).get(col_cnt*rowCount, (col_cnt+1)*rowCount-1));
        
        append(output,temp);
    }
    return(output);
    
}


/*! \brief Do the cyclic-shift to right by number of shifts = nShifts
 *  \note If nShifts <0, then do the corresponding left shift */
template <class T> Vec<T> cyclicallyShift(const Vec<T> inVector, int nShifts)
{
    Vec<T> tempVector = inVector;
    if(nShifts > 0)
        tempVector.shift_right(inVector(length(inVector)-nShifts,length(inVector)-1));
    else if(nShifts < 0)
        tempVector.shift_left(inVector.get(0,(-nShifts)-1));
    return tempVector;
}


template<class Num_T> Array<Num_T> repeatAsArray(Num_T &input, int count)
{
    Array<Num_T> output(0);
    
    if(count<0) {cout<<"Count cant' be -ve for repeatAsArray().. Returning NULL "<<endl;return(output);}
    
    for(int cnt=0;cnt<count;cnt++)
        append(output,input);
    return(output);
}

template<class Num_T> Vec<Num_T> repeatAsVector(Num_T &input, int count)
{
    Vec<Num_T> output(0);
    
    if(count<0) {cout<<"Count cant' be -ve for repeatAsVector().. Returning NULL ";return(output);}
    
    for(int cnt=0;cnt<count;cnt++)
        append(output,input);
    
    return(output);
}



template<class Num_T> Vec<Num_T> getAcrossArray(Array< Vec<Num_T> > &input, int vectorIndx)
{
    Vec<Num_T> output(0);
    for(int cnt=0;cnt<input.length();cnt++)
    {
        if(input(cnt).length()<=vectorIndx) {cout<<"Out-of range error in getAcrossArray()...Returning partial output"<<endl;abort();return(output);}
        append(output,input(cnt)(vectorIndx));
    }
    return(output);
}

template<class Num_T> Array< Vec<Num_T> > addAcrossArray(Array< Vec<Num_T> > &input, Vec<Num_T> toAdd)
{
    Array< Vec<Num_T> > output(input.length());
    for(int cnt=0;cnt<input.length();cnt++)
    {
        if(input(cnt).length()!=toAdd.length())
        {
            cout<<"Dimension mismatch in addAcrossArray().."<<input.length()<<" , "<<toAdd.length()<<endl;
            abort();
        }
        output(cnt)=input(cnt)+toAdd;
    }
    return(output);
}


template<class Num_T> Vec<Num_T> getAcrossArray(Array< Mat<Num_T> > &input, int matrixLinearIndx)
{
    Vec<Num_T> output(input.length());
    for(int cnt=0;cnt<input.length();cnt++)
        output(cnt)=input(cnt).get(matrixLinearIndx);
    
    return(output);
}

template<class Num_T> Mat<Num_T> getAcrossArray(Array< Array< Mat<Num_T> > > &input, int matrixLinearIndx, bool firstDimIsCol=true)
{
    Mat<Num_T> output(0,0);
    if(input.length()==0) return(output);
    if(firstDimIsCol)
    {
        output.set_size(input(0).length(),input.length());
        for(int col=0;col<input.length();col++)
            for(int row=0;row<input(col).length();row++)
                output(row,col)=input(col)(row).get(matrixLinearIndx);
    }
    else
    {
        output.set_size(input.length(),input(0).length());
        
        for(int row=0;row<input.length();row++)
            for(int col=0;col<input(row).length();col++)
                output(row,col)=input(row)(col).get(matrixLinearIndx);
    }
    
    return(output);
}
template<class Num_T>  Array< Mat<Num_T> > getAcrossArray(Array< Array< Vec<Num_T> > > &input, ivec indices="", bool firstDimIsCol=true)
{
    if(indices.length()==0)
        indices=getIntegers(0,input(0)(0).size()-1);
    
    
    Array< Mat<Num_T> > output(indices.length());
    if(input.length()==0) return(output);
    if(firstDimIsCol)
    {
        for(int k=0;k<indices.length();k++)
        {
            output(k).set_size(input(0).length(),input.length());
            for(int col=0;col<input.length();col++)
                for(int row=0;row<input(col).length();row++)
                    output(k)(row,col)=input(col)(row).get(indices(k));
        }
    }
    else
    {
        for(int k=0;k<indices.length();k++)
        {
            output(k).set_size(input.length(),input(0).length());
            
            for(int row=0;row<input.length();row++)
                for(int col=0;col<input(row).length();col++)
                    output(k)(row,col)=input(row)(col).get(indices(k));
        }
    }
    
    return(output);
}

template<class Num_T>  Array< Mat<Num_T> > getAcrossArray(Array< Array< Mat<Num_T> > > &input, ivec matrixLinearIndx="", bool firstDimIsCol=true)
{
    if(matrixLinearIndx.length()==0)
        matrixLinearIndx=getIntegers(0,input(0)(0).size()-1);
    
    
    Array< Mat<Num_T> > output(matrixLinearIndx.length());
    if(input.length()==0) return(output);
    if(firstDimIsCol)
    {
        for(int k=0;k<matrixLinearIndx.length();k++)
        {
            output(k).set_size(input(0).length(),input.length());
            for(int col=0;col<input.length();col++)
                for(int row=0;row<input(col).length();row++)
                    output(k)(row,col)=input(col)(row).get(matrixLinearIndx(k));
        }
    }
    else
    {
        for(int k=0;k<matrixLinearIndx.length();k++)
        {
            output(k).set_size(input.length(),input(0).length());
            
            for(int row=0;row<input.length();row++)
                for(int col=0;col<input(row).length();col++)
                    output(k)(row,col)=input(row)(col).get(matrixLinearIndx(k));
        }
    }
    
    return(output);
}
template<class Num_T> Mat<Num_T> getAcrossArray(Array< Array< Vec<Num_T> > > &input, int indx, bool firstDimIsCol=true)
{
    Mat<Num_T> output(0,0);
    if(input.length()==0) return(output);
    if(firstDimIsCol)
    {
        output.set_size(input(0).length(),input.length());
        for(int col=0;col<input.length();col++)
            for(int row=0;row<input(col).length();row++)
                output(row,col)=input(col)(row).get(indx);
    }
    else
    {
        output.set_size(input.length(),input(0).length());
        
        for(int row=0;row<input.length();row++)
            for(int col=0;col<input(row).length();col++)
                output(row,col)=input(row)(col).get(indx);
    }
    
    return(output);
}

template<class Num_T> void setAcrossArray(Array< Vec<Num_T> > &input, int vectorIndx,Vec<Num_T> toWriteIn)
{
    if(input.length()!=toWriteIn.length()){ cout<<"Length mismatch in setAcrossArray()..."<<endl;return;}
    for(int cnt=0;cnt<input.length();cnt++)
    {
        if(input(cnt).length()<=vectorIndx) { cout<<"Index out-of-range in setAcrossArray()...Aborting ..."<<endl;abort();}
        input(cnt)(vectorIndx)=toWriteIn(cnt);
    }
    
}


template<class Num_T> Array< Vec<Num_T> > swapDimensions(Array< Vec<Num_T> > &input)
{
    Array< Vec<Num_T> > output(0);
    if(input.length()>0)
    {
        for(int cnt=0;cnt<input(0).length();cnt++) // assuming all vectors are of same length.. - 10010
            append(output,getAcrossArray(input,cnt));
    }
    return(output);
}


template<class Num_T> void setAcrossArray(Array< Mat<Num_T> > &input, int matrixLinearIndx,Vec<Num_T> toWriteIn)
{
    if(input.length()!=toWriteIn.length()){ cout<<"Length mismatch in setAcrossArray()..."<<endl;return;}
    int rowIndx,colIndx;
    for(int cnt=0;cnt<input.length();cnt++)
    {
        if(matrixLinearIndx>=input(cnt).size()) { cout<<"Index out-of-range in setAcrossArray()...Aborting ..."<<endl;abort();}
        rowIndx=matrixLinearIndx%input(cnt).rows();
        colIndx=matrixLinearIndx/input(cnt).rows();
        input(cnt).set(rowIndx,colIndx,toWriteIn(cnt));
    }
    
}

template<class Num_T> void setAcrossArray(Array< Array< Mat<Num_T> > > &input, int matrixLinearIndx, Mat<Num_T> toWriteIn, bool firstDimIsCol=true)
{
    if(firstDimIsCol)
    {
        if(input.length()!= toWriteIn.cols()) { cout<<"Dimension mmismatch in setAcrossArray()...Aborting ..."<<endl;abort();}
        
        int rowIndx,colIndx;
        for(int d1=0;d1<input.length();d1++)
        {
            if(input(d1).length()!= toWriteIn.rows())  { cout<<"Dimension mmismatch in setAcrossArray()...Aborting ..."<<endl;abort();}
            for(int d2=0;d2<input(d1).length();d2++)
            {
                rowIndx=matrixLinearIndx%input(d1)(d2).rows();
                colIndx=matrixLinearIndx/input(d1)(d2).rows();
                
                input(d1)(d2).set(rowIndx,colIndx,toWriteIn(d2,d1));
                
            }
        }
    }
    else
    {
        if(input.length()!= toWriteIn.rows()) { cout<<"Dimension mmismatch in setAcrossArray()...Aborting ..."<<endl;abort();}
        
        int rowIndx,colIndx;
        for(int d1=0;d1<input.length();d1++)
        {
            if(input(d1).length()!= toWriteIn.cols())  { cout<<"Dimension mmismatch in setAcrossArray()...Aborting ..."<<endl;abort();}
            
            for(int d2=0;d2<input(d1).length();d2++)
            {
                rowIndx=matrixLinearIndx%input(d1)(d2).rows();
                colIndx=matrixLinearIndx/input(d1)(d2).rows();
                
                input(d1)(d2).set(rowIndx,colIndx,toWriteIn(d1,d2));
            }
        }
    }
}

template<class Num_T> void setAcrossArray(Array< Array< Vec<Num_T> > > &input, int indx, Mat<Num_T> toWriteIn, bool firstDimIsCol=true)
{
    if(firstDimIsCol)
    {
        if(input.length()!= toWriteIn.cols()) { cout<<"Dimension mmismatch in setAcrossArray()...Aborting ..."<<endl;abort();}
        
        int rowIndx,colIndx;
        for(int d1=0;d1<input.length();d1++)
        {
            if(input(d1).length()!= toWriteIn.rows())  { cout<<"Dimension mmismatch in setAcrossArray()...Aborting ..."<<endl;abort();}
            for(int d2=0;d2<input(d1).length();d2++)
            {
                input(d1)(d2).set(indx,toWriteIn(d2,d1));
                
            }
        }
    }
    else
    {
        if(input.length()!= toWriteIn.rows()) { cout<<"Dimension mmismatch in setAcrossArray()...Aborting ..."<<endl;abort();}
        
        int rowIndx,colIndx;
        for(int d1=0;d1<input.length();d1++)
        {
            if(input(d1).length()!= toWriteIn.cols())  { cout<<"Dimension mmismatch in setAcrossArray()...Aborting ..."<<endl;abort();}
            
            for(int d2=0;d2<input(d1).length();d2++)
            {
                input(d1)(d2).set(indx,toWriteIn(d1,d2));
            }
        }
    }
}

template<class Num_T> bool isEqual(Array< Num_T > &input1, Array< Num_T > &input2)
{
    if(input1.length()!=input2.length()){ cout<<"Length mismatch in isEqual()..."<<endl;return false;}
    for(int cnt=0;cnt<input1.length();cnt++)
        if(input1(cnt)!=input2(cnt)) return false;
        return true;
    
}


template<class Num_T> bool isEqual(Array< Array< Num_T > > &input1,Array< Array< Num_T > > &input2)
{
    if(input1.length()!=input2.length()){ cout<<"Length mismatch in isEqual()..."<<endl;return false;}
    for(int cnt1=0;cnt1<input1.length();cnt1++)
    {
        if(input1(cnt1).length()!=input2(cnt1).length()){ cout<<"Length mismatch in isEqual()..."<<endl;return false;}
        for(int cnt2=0;cnt2<input1(cnt1).length();cnt2++)
            if(input1(cnt1)(cnt2)!=input2(cnt1)(cnt2)) return false;
    }
    
    return true;
    
}

template<class Num_T> bool isEqual(Array< Array< Array< Num_T > > > &input1,Array< Array< Array< Num_T > > > &input2)
{
    if(input1.length()!=input2.length()){ cout<<"Length mismatch in isEqual()..."<<endl;return false;}
    for(int cnt1=0;cnt1<input1.length();cnt1++)
        if(!isEqual(input1(cnt1),input2(cnt1))) return false;
        
        return true;
    
}




/*! \brief Function to segment input vec into Array of vec's with arrays length specified as totalNumOfSegments
 *  split the input vec to Array of vecs(segments), with number of segments =1, in which segment size can be either big(S) or small(S-1)
 *    */
template<class vecType> inline
Array<vecType> segmentVecToArrayOfVecs(const vecType& input,const int& totalNumOfSegments=1)
{
    int bigSegmentLength =  ceil_i(length(input)/double(totalNumOfSegments));
    int numBigSegments = floor_i(length(input)/double(bigSegmentLength));
    int smallSegmentLength = length(input) - bigSegmentLength*numBigSegments;  
    int numSmallSegments = totalNumOfSegments-numBigSegments;
    Array<vecType> output; output.set_size(totalNumOfSegments);
    
    int runIndex=0;
    for(int iBigSegment=0; iBigSegment<numBigSegments; iBigSegment++) {
        output(iBigSegment) = input.mid(runIndex,bigSegmentLength);
        runIndex += bigSegmentLength;
    }
    
    for(int iSmallSegment=0; iSmallSegment<numSmallSegments; iSmallSegment++) {
        output(numBigSegments+iSmallSegment) = input.mid(runIndex,smallSegmentLength);
        runIndex += smallSegmentLength;
    }
    
    return output;
}


template<class Num_T> void printVariable(string fileName, string variable, Num_T data){
    
    std::ofstream fout(fileName.c_str(), std::ios::app);
    fout<<variable<<"="<<data<<endl;
    fout.close();
}

void system_exec(const char * command);

int NCr(int N,int k);
int sumNCr(int N, int maxK);
bmat getBinaryCombinations(int N,int K);
bmat getAllBinaryCombinations(int N,int maxK);
double factorial(int N);
ivec swapVec(ivec inputVector);
Array<ivec> splitContiguousNumbers(ivec inputVec, int bundlingSize);
string briefSimulationDetails(double currentTime=-1);
string dumpConfigFiles(void);


inline std::vector<bool> BVecToBoolVec(bvec itppvec)
{
    std::vector<bool>stdvector;
    stdvector.resize(itppvec.size());
    for (unsigned int i=0; i<stdvector.size(); i++)
    {
        stdvector[i]=(bool)itppvec[i];
    }
    return stdvector;
}

inline std::vector<std::vector<bool>> ArrayBVecTostdVecVecBool(Array<bvec> itppArrayVec)
{
    std::vector<std::vector<bool>> stdVecVec;
    stdVecVec.reserve(itppArrayVec.size());
    for (unsigned int i=0; i<itppArrayVec.size(); i++)
    {
        stdVecVec.push_back(BVecToBoolVec(itppArrayVec(i)));
    }
    return stdVecVec;
}

template<class Num_T> inline std::vector<std::vector<Num_T>> ArrayVecTostdVecVecGeneral(Array<Vec<Num_T>> itppArrayVec)
{
    std::vector<std::vector<Num_T>> stdVecVec;
    stdVecVec.reserve(itppArrayVec.size());
    Vec<Num_T> temp;
    for (unsigned int i=0; i<itppArrayVec.size(); i++)
    {
        temp = itppArrayVec(i);
        std::vector<Num_T>stdvector;
        stdvector.resize(temp.size());
        for (unsigned int i=0; i<stdvector.size(); i++)
        {
            stdvector[i]=temp[i];
        }
        stdVecVec.push_back(stdvector);
    }
    return stdVecVec;
}

inline Array<bvec> stdVecVecboolToArrayBvec(std::vector<std::vector<bool>> stdVecVec)
{
    Array<bvec> arrayBvec;
    arrayBvec.set_size(stdVecVec.size());
    
    for(unsigned int i=0; i<arrayBvec.size(); i++)
    {
        bvec temp;
        temp.set_size(stdVecVec[i].size());
        int j=0;
        for(auto x:stdVecVec[i])
        {
            
            temp[j] = x;
            j++;
        }
        arrayBvec(i) = temp;
    }
    
    return arrayBvec;
}

template<class Num_T> inline std::vector<Num_T> ITPPVectorstdVector(Vec<Num_T> itppvec)
{
    std::vector<Num_T>stdvector;
    stdvector.resize(itppvec.size());
    for (unsigned int i=0; i<stdvector.size(); i++)
    {
        stdvector[i]=itppvec[i];
    }
    return stdvector;
}

template<class T> inline std::vector<T> ITPPArraytoStdVector(Array<T> itppvec)
{
    std::vector<T>stdvector;
    stdvector.re_size(itppvec.size());
    for (int i=0; i<stdvector.size(); i++)
    {
        stdvector[i]=itppvec[i];
    }
    return stdvector;
}

template<class Num_T> inline Vec<Num_T> stdVectortoITPPVec(std::vector<Num_T> stdvector)
{
    Vec<Num_T>itppvec;
    itppvec.set_size(stdvector.size());
    for (int i=0; i<stdvector.size(); i++)
    {
        itppvec(i)=stdvector[i];
    }
    return itppvec;
}

// set columns of mat to outer vector and rows to inner vector 
template<class Num_T> inline std::vector< std::vector<Num_T> > ITPPMatTostdVectOfVect(Mat<Num_T> itppvec)
{
    std::vector< std::vector<Num_T> >stdvector;
    stdvector.resize(itppvec.cols());
    for (unsigned int j=0; j<stdvector.size(); j++)
    {
        stdvector[j].resize(itppvec.rows());
        for(unsigned int i=0; i<stdvector[j].size(); i++)
        {
            stdvector[j][i]=itppvec(i,j);
        }
    }
    return stdvector;
}

template<class Num_T> inline Mat<Num_T> stdVecOfVectToITPPMat(std::vector<std::vector<Num_T> > stdvector)
{
    Mat<Num_T>itppvec;
    itppvec.set_size(stdvector[0].size(),stdvector.size());
    for (int j=0; j<stdvector.size(); j++)
        for (int i=0; i<stdvector[0].size(); i++)
        {
            itppvec(i,j)=stdvector[j][i];
            
        }
        return itppvec;
}

template<class Num_T> inline Array<Num_T> stdVectortoITPPArray(std::vector<Num_T> stdvector)
{
    Array<Num_T> itppArray;
    itppArray.set_size(stdvector.size());
    for (int i=0; i<stdvector.size(); i++)
    {
        itppArray(i)=stdvector[i];
    }
    return itppArray;
}

inline bvec BoolVectortoBVec(std::vector<bool> stdvector)
{
    bvec itppvec;
    itppvec.set_size(stdvector.size());
    for (int i=0; i<stdvector.size(); i++)
    {
        itppvec(i)=(bool)stdvector[i];
    }
    return itppvec;
}

double exp_rand(double tLambda);
int poisson_rand(double tLambda);
double lognormal_rand(double mean, double sigma);
double pareto_rand(double threshold, double index);
double gamma_rand(int tShapeParam, double tScaleParam);
int nearestPrime(int x);
imat computePermutations(int N, int r);
mat lognormal_rand(int rows, int cols, double mean, double sigma);

#endif



