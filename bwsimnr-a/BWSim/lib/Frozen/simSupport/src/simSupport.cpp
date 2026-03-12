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

#include "../include/simSupport.h"
#include<stdarg.h>

//! Moved following definition from main.cpp to here, in-order to avoid compilation error for testcase
std::map <std::string,std::string> traceValuePerTraceName;


ivec getUniqueIntegers(int from,int to,int numberofpicks)
{
  ivec uniqueNumbers;
  if(to>=from)
  {
    if((to-from+1)>=numberofpicks)
    {
      ivec temp=getIntegers(from,to);
      uniqueNumbers=randPick(temp,numberofpicks);
    }
    else
      cout << "invalid picks or from is greaterthan to " << endl;
  }
    
    return uniqueNumbers;
}

int find(imat matrix, int numToFind)
{
  int indx=0;
  for (int col_cnt=0;col_cnt<matrix.cols();col_cnt++)
	for (int row_cnt=0;row_cnt<matrix.rows();row_cnt++,indx++)
	  if (matrix(row_cnt,col_cnt)==numToFind) return(indx);
  return(-1);
}

double besselj(int n,double x)
{ 
  vec A(52), B=zeros(52);
  double y;
  int    m=50;
  double a1=1.0/pow(2.0,(double)n);
  double b1;
  
  b1 = 1.0;
  for (int    i = 1; i < n+1; i++)  {
    b1 = b1 * i; 
  }
  
  b1 = a1 / b1; //n!/2^n
  
  a1 = 1.0;
  int i = 0;
  while (i <= m)  {
    A[i] = a1 * b1; A[i + 1] = 0;
    a1 = -a1 / ((i + 2) * (n + n + i + 2));
    i = i + 2;
  }
  a1 = a1 / 2.0;
  for (i = 0; i < m+1; i++)  B[i + n] = A[i];
  for (i = 0; i < n+m+1; i++)  A[i] = B[i];
  
 
  y = A[0] + A[1] * x;
  if (m > 1)  y = y + A[2] * x * x;
  if (m > 2)  y = y + A[3] * x * x * x;
  if (m > 3)  y = y + A[4] * pow(x,4.0);
  for (i = 4; i < m; i++)
    if (m > i)  y = y + A[i + 1] * pow(x,(double)(i+1));
    return y;
}

ivec findNot(ivec vector, int numToOmit){

  ivec index(0);
  for (int num_cnt=0;num_cnt<length(vector);num_cnt++)
	if(vector(num_cnt)!=numToOmit)
	  append(index,num_cnt);
  return(index);
}

ivec findNot(ivec vector, ivec numsToOmit){

  ivec index(0);
  bvec pickFlag=ones_b(vector.length());
  for (int num_cnt=0;num_cnt<length(numsToOmit);num_cnt++)
    for (int num_cnt1=0;num_cnt1<length(vector);num_cnt1++)
	if(vector(num_cnt1)==numsToOmit(num_cnt))
	  pickFlag(num_cnt1)=false;
  if(pickFlag.length())
    return(find(pickFlag));
  else
    return(ivec(""));
}

ivec findNot(vec vector, double numToOmit){

  ivec index(0);
  for (int num_cnt=0;num_cnt<length(vector);num_cnt++)
	if(vector(num_cnt)!=numToOmit)
	  append(index,num_cnt);
  return(index);
}

ivec find(imat matrix, int numToFind, bool findAll)
{
  ivec index(0);
  
  if(matrix.size()==0)
  {
    cout<<"[both:] matix is empty for the find function "<<endl;abort();
    return index;
  }
  
  if (findAll)
  {
    for (int col_cnt=0,indx=0;col_cnt<matrix.cols();col_cnt++)
	  for (int row_cnt=0;row_cnt<matrix.rows();row_cnt++,indx++)
		if (matrix(row_cnt,col_cnt)==numToFind) append(index,indx);
  }
  else
  {
	if(find(matrix, numToFind)!=-1)
	  append(index,find(matrix, numToFind));
  }
  return(index);
}

vec findUniqueNumbers(vec input){

  vec uniqueNumbers(0);
  sort(input);
  ivec temp;
  while(input.length())
  {
	temp=find(input,input(0),true);
	append(uniqueNumbers,input(0));
	input.del(temp(0),temp(temp.length()-1));
  }
  return(uniqueNumbers);
}

ivec findUniqueNumbers(ivec input){

  ivec uniqueNumbers(0);
  sort(input);
  ivec temp;
  while(input.length())
  {
	temp=find(input,input(0),true);
	append(uniqueNumbers,input(0));
	input.del(temp(0),temp(temp.length()-1));
  }
  return(uniqueNumbers);
}

ivec findUniqueNumbersWithoutSorting(ivec input){
    
    ivec uniqueNumbers(0);
    int temp;
    append(uniqueNumbers,input(0));
    for(int i = 1;i < input.length();i++){
        ivec indices;
        indices = find(uniqueNumbers,input(i),false);
        if(indices.length()==0){append(uniqueNumbers,input(i));}
        else{continue;}
    }
    return(uniqueNumbers);
}



ivec findUniqueNumbers(imat input){

  ivec vectorInput=cvectorize(input);
  return(findUniqueNumbers(vectorInput));
}

int findClosest(vec input, double number){

  vec temp=abs(input-number);
  return(min_index(temp));
}

int findClosest(ivec input, int number){

  input-=number;
  ivec temp=abs(input);
  return(min_index(temp));

}

int count(ivec vector, int numToCount){

  int cnt=0;
  for (int vec_cnt=0;vec_cnt<length(vector);vec_cnt++)
	if (vector(vec_cnt)==numToCount)
	  cnt++;
	return(cnt);
}

int count(imat matrix, int numToCount){

  int cnt=0;
  for (int col_cnt=0;col_cnt<matrix.cols();col_cnt++)
	for (int row_cnt=0;row_cnt<matrix.rows();row_cnt++)
	  if (matrix(row_cnt,col_cnt)==numToCount)
		cnt++;
	  return(cnt);
}

int findInStringArray(Array<string> stringArray, string stringToFind) {

  int indx=0;
  while ((indx)<stringArray.length())
  { 	if (stringArray(indx)==stringToFind) return(indx);
	indx++;
  }
  return(-1);
}

ivec findAllInStringArray(Array<string> stringArray, string stringToFind) {

    int indx=0;
    ivec toReturn(0);  
    while ((indx)<stringArray.length())
    { 	if (stringArray(indx)==stringToFind) append(toReturn,indx);
        indx++;
    }
    return toReturn;
  
}

ivec getIntegers(int from,int to, int interval){
  int count;
  if(to<from && interval>0)
	interval=(-interval);
  if(to>from && interval<0)
	interval=(-interval);

  count= floor_i((double)(to-from)/(double)interval)+1;

  ivec output(count);
  for(int cnt=0;cnt<count;cnt++)
	output(cnt)=from+cnt*interval;
  return(output);
}

vec getVector(double from,double to, double interval){

  int count;
  if(to<from && interval>0)
	interval=(-interval);
  if(to>from && interval<0)
	interval=(-interval);

  count= round_i((to-from)/interval)+1;
  vec output(count);
  output(0)=from;
  for(int cnt=1;cnt<count;cnt++)
    output(cnt)=output(cnt-1)+interval;
  return(output);

}

ivec remove(ivec input,int numberToremove){

  ivec indices=findNot(input,numberToremove);
  if(indices.length())
    return(input(indices));
  else
    return(ivec(""));
  
}

ivec remove(ivec input,ivec numbersToremove){

  ivec indices=findNot(input,numbersToremove);
  indices=remove(indices,-1);
  return(input(indices));
}


/*! \brief Function used to return simulation run-time(ie time between two consecutive calls of \b captureTime function), in the resolution of microseconds..
    \param[in] captureID Used add more flexiblity for the function to run in parallel. captureTime function called with different captureID will never interfere with each other.
                        \note If(captureID==0), then captureTime function is called for all the captureID available currently.
 *  \param[in] captureTimeFormat Specifies the action to be taken for the specified captureID as follows,
                        "START"     ==> Store the currentTime to startTimes & startClockTimes
                        "PAUSE"     ==> Store the time-difference to captureDuration and delete startTimes
                        "RERUN"     ==> Check if the process is "PAUSE" 'ed. if then, store the currentTime to startTimes.
                        "STOP"      ==> "PAUSE" the process and print the captureDuration. Then delete Everything.
                        "DEFAULT"   ==> Proceeds to either "START" or "STOP" as below,
                                            If(captureDuration exists)  captureTimeFormat = START
                                            Else                        captureTimeFormat = STOP

                        "PRINT" 		==> "PAUSE" the process, then "RERUN" the process. Then print the captureDuration
                        "PRINT_STARTCLOCK"      ==> Print the startClockTimes
                        "PRINT_CURRENTCLOCK"    ==> Print the currentClockTime

 *  \return streamToReturn Contains the difference of time in string format.

 *  \section some Keynotes upon C-code below,
        1. \param startClockTimes[captureID] indicate the start time of the process avoiding "PAUSE" & "RERUN" happened in between
        1. \param captureDuration.count(captureID) tells whether the Capturing process(corrresponding to captureID) still exists or not.
        2. \param startTimes.count(captureID) tells whether the Capturing process(corrresponding to captureID) is currently paused or not...
 *   		*/
std::string captureTime(const int captureID,CaptureTimeFormat_E captureTimeFormat)
{
    static std::map <int,std::string> startClockTimes; //! map < captureID, startClockTime >
    static std::map <int,struct timeval> startTimes; //! map < captureID, startTime >
    static std::map <int,double> captureDuration;    //! map < captureID, pauseTime >
    time_t currentClockTime;

    std::stringstream streamToReturn;

    //! Return the current-clock time (not related to any capture process)...
    if(captureTimeFormat == PRINT_CURRENTCLOCK)
    {
        streamToReturn<<" "<<asctime((struct tm *)localtime(&(currentClockTime=time(0))));
        return streamToReturn.str();
    }

    //! If captureID ==0, then do the operations for all captureIDs
    if(captureID==0)
    {
        std::vector<int> captureIDs;
        for (std::map <int,double>::iterator mi = captureDuration.begin(); mi != captureDuration.end(); ++mi)
          captureIDs.push_back(mi->first);

        for(int icaptureIDs=0; icaptureIDs < (int)captureIDs.size(); icaptureIDs++)
            streamToReturn<<"\n For captureID ="<<captureIDs[icaptureIDs]<<", Time ="<<captureTime(captureIDs[icaptureIDs],captureTimeFormat);
        return streamToReturn.str();
    }

    //! Return the startClockTime of the process...
    if(captureTimeFormat == PRINT_STARTCLOCK)
    {
        if(startClockTimes.count(captureID)>0)
            streamToReturn<<" "<<startClockTimes[captureID];
        return streamToReturn.str();
    }

    static struct timeval currentTime;
    gettimeofday(&currentTime, NULL);

    //! If captureTimeFormat is not specified, then it made to either "START" on 1st call, "STOP" on 2nd call (then again "START" on 3rd call)
    if(captureTimeFormat == DEFAULT)
    {
        if(captureDuration.count(captureID)==0) captureTimeFormat = START;
        else                                    captureTimeFormat = STOP;
    }

    //! If RERUN called first-time (before initializing with START), then the default behaviour is to START
    if(captureTimeFormat == RERUN && captureDuration.count(captureID)==0)
        captureTimeFormat = START;

    //! Calculate the startTimes[captureID] & captureDuration[captureID]
    switch(captureTimeFormat)
    {
        case START:
                startClockTimes[captureID] = (std::stringstream(asctime((struct tm *)localtime(&(currentClockTime=time(0)))))).str();
                startTimes[captureID] = currentTime;
                captureDuration[captureID] = 0;
                break;
        case PRINT:
        case STOP:
        case PAUSE:
                if(captureDuration.count(captureID)>0 && startTimes.count(captureID)>0)
                        captureDuration[captureID] += (currentTime.tv_sec-startTimes[captureID].tv_sec)+1e-6*(currentTime.tv_usec-startTimes[captureID].tv_usec);
                startTimes.erase(captureID);
                if(captureTimeFormat != PRINT)	break;
        case RERUN:
                if(startTimes.count(captureID)==0)       //! Re-Run only if it's previously "PAUSE" 'ed or currently "PRINT"
                    startTimes[captureID] = currentTime;
                break;
        default:
                cout<<"\n Warning: captureTime can't be executed, since captureTimeFormat ="<<captureTimeFormat<<" is unknown \n";
    }

    //! Generating Output string, if the corresponding Capturing process exists...
    if((captureTimeFormat == PRINT || captureTimeFormat == STOP) && captureDuration.count(captureID)>0)
    {
        double timeDifference = captureDuration[captureID];
        //! Now to calculate the Time Difference......
        int hours = timeDifference/(3600);           timeDifference -= hours*(3600);
        int minutes = timeDifference/(60);           timeDifference -= minutes*(60);
        int seconds = timeDifference;                timeDifference -= seconds;
        int milliseconds = timeDifference/(1e-3);    timeDifference -= milliseconds*(1e-3);
        int microseconds = timeDifference/(1e-6);

        //! Now to print the Time Difference......
        streamToReturn<<" ";
        if(hours)           streamToReturn<<hours<<" hours,  ";
        if(minutes)         streamToReturn<<minutes<<" minutes,  ";
        if(seconds)         streamToReturn<<seconds<<" seconds,  ";
        if(milliseconds)    streamToReturn<<milliseconds<<" milliseconds,  ";
        if(microseconds)    streamToReturn<<microseconds<<" microseconds,  \n";
    }

    //! For "STOP", Delete the entire Capture-Process..
    if(captureTimeFormat == STOP)
    {
            captureDuration.erase(captureID);
            startClockTimes.erase(captureID);
    }

    return streamToReturn.str();
}


cvec fftNorm(cvec &input)
{
  return(fft(input)/sqrt((double)input.length()));
}

cmat fftNorm(cmat &input, bool isForColumns)
{
  cmat output(input.rows(),input.cols());
  cvec temp;
  if(isForColumns)
  {
    for(int col_cnt=0;col_cnt<input.cols();col_cnt++)
    {
      temp=input.get_col(col_cnt);
      output.set_col(col_cnt,fftNorm(temp));
    }
    return(output);
  }
  else
  {
    for(int row_cnt=0;row_cnt<input.rows();row_cnt++)
    {
      temp=input.get_row(row_cnt);
      output.set_row(row_cnt,fftNorm(temp));
    }
    return(output);
  }
}

ivec getVectorWithIntegers(int a,...)
{
    va_list valist;
    ivec num(a);
    va_start(valist, a);
    for (int i=0;i<a;i++)
        num(i)=va_arg(valist,int);
    va_end(valist);
    return num;
}
vec getVectorWithNumbers(int a,...)
{
    va_list valist;
    vec num(a);
    va_start(valist, a);
    for (int i=0;i<a;i++)
        num(i)=va_arg(valist,double);
    va_end(valist);
    return num;
}

cvec ifftNorm(cvec &input)
{
  return(ifft(input)*sqrt((double)input.length()));
}

cmat ifftNorm(cmat &input, bool isForColumns)
{
  cmat output(input.rows(),input.cols());
  cvec temp;
  if(isForColumns)
  {
    for(int col_cnt=0;col_cnt<input.cols();col_cnt++)
    {
      temp=input.get_col(col_cnt);
      output.set_col(col_cnt,ifftNorm(temp));
    }
    return(output);
  }
  else
  {
    for(int row_cnt=0;row_cnt<input.rows();row_cnt++)
    {
      temp=input.get_row(row_cnt);
      output.set_row(row_cnt,ifftNorm(temp));
    }
    return(output);
  }
}

int pow_i(int x, int y)
{
  int output=1;
  for(int cnt=0;cnt<y;cnt++)
    output*=x;
  return(output);
}

double getNormalizedErrorPercent(vec mainInput, vec inputB){

  return((sum(mainInput-inputB)/sum(mainInput))*100);
}

double getNormalizedErrorPercent(cvec mainInput, cvec inputB){

  return((abs(sum(mainInput-inputB))/abs(sum(mainInput)))*100);
}

double getNormalizedErrorPercent(mat mainInput, mat inputB){

  return((sumsum(mainInput-inputB)/sumsum(mainInput))*100);
}

double getNormalizedErrorPercent(cmat mainInput, cmat inputB){

  return((abs(sumsum(mainInput-inputB))/abs(sumsum(mainInput)))*100);
}

std::map <std::string,std::string> parseAll(std::string inputFileName) {

    std::string inputFileNameTemp = inputFileName+".temp"+toString(currentTaskid);

    //! Delete all comments/blank-lines, and spaces @ beginning/end/around '=' ..
    std::ostringstream shellScript;
    shellScript<<"perl -pe 'BEGIN{undef $/;} s/\\n?[[:blank:]]*\\/\\*\\X*?\\*\\/[[:blank:]]*//smg, s/(\\n)*[[:blank:]]*\\/\\/[[:print:]]*//smg, s/^[[:blank:]]*\\n//smg, s/[[:blank:]]*=[[:blank:]]*/=/sg, s/[[:blank:]]*\\;[[:blank:]]*/\\;/sg, s/[[:blank:]]*\\n[[:blank:]]*/\\n/smg' "<<inputFileName<<" > "<<inputFileNameTemp<<";";
    cout<<exec(shellScript.str())<<endl;

    std::map <std::string,std::string> traceValuePerTraceName_;
    std::string myInputFileAsString = convertFileToString(inputFileNameTemp);
    std::vector<std::string> tracesPerLine = splitString(myInputFileAsString,'\n');

    std::vector<std::string> tempVecOfStrings;
    for(unsigned int itracesPerLine=0; itracesPerLine<tracesPerLine.size(); itracesPerLine++) {
        std::vector<std::string> tracesPerBlock = splitString(tracesPerLine.at(itracesPerLine),';');
        for(unsigned int itracesPerBlock=0; itracesPerBlock<tracesPerBlock.size(); itracesPerBlock++) {
            tempVecOfStrings = splitString(tracesPerBlock.at(itracesPerBlock),'=');

            if(tempVecOfStrings.size()==1)
                traceValuePerTraceName_[tempVecOfStrings.at(0)] = "";
            else if(tempVecOfStrings.size()==2) {
                if(tempVecOfStrings.at(0)=="") {
                    cout<<"[both:] Error: TraceFlagName (Left-side of '=' character) is blank in traceFile = "<<inputFileName<<endl;
                    abort();
                }
                traceValuePerTraceName_[tempVecOfStrings.at(0)] = tempVecOfStrings.at(1);
            }
            else {
                cout<<"[both:] Error: multiple '=' characters for single trace in traceFile = "<<inputFileName<<endl;
                abort();
            }
        }
    }
   cout<<exec(std::string("rm -f ")+inputFileNameTemp)<<endl;

   return traceValuePerTraceName_;
}

std::vector<std::string> &splitString(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> splitString(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return splitString(s, delim, elems);
}

std::string convertFileToString(const std::string &fileName) {
    std::ifstream traceFile(fileName.c_str(),std::ifstream::in);
    std::ostringstream output;
    output<<traceFile.rdbuf();
    return output.str();
}


void setInMatrix(bmat &inputMatrix, ivec row1stIndices, bvec values){

  if(row1stIndices.length() != values.length())
  {
    cout<<"[both:] Error... Indices and the values must be of same size in setInMatrix()..."<<endl;
    abort();
  }

  if(sum(row1stIndices>= inputMatrix.size()))
  {
    cout<<"[both:] Error... Indices exceed matrix dimension in setInMatrix()..."<<endl;
    abort();
  }


  for(int indx_cnt=0;indx_cnt<row1stIndices.length();indx_cnt++)
    inputMatrix.set(row1stIndices(indx_cnt)%inputMatrix.rows(),row1stIndices(indx_cnt)/inputMatrix.rows(),values(indx_cnt));
}

void setInMatrix(bmat &inputMatrix, ivec row1stIndices, int valueToSet){

  if(sum(row1stIndices>= inputMatrix.size()))
  {
    cout<<"[both:] Error... Indices exceed matrix dimension in setInMatrix()..."<<endl;
    abort();
  }

  for(int indx_cnt=0;indx_cnt<row1stIndices.length();indx_cnt++)
    inputMatrix.set(row1stIndices(indx_cnt)%inputMatrix.rows(),row1stIndices(indx_cnt)/inputMatrix.rows(),valueToSet);
}

void setInVec(bvec &inputVec, ivec indices, bvec values){
  
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

void append(bvec &baseVector, const bin t)
{
  int size = baseVector.size();
  baseVector.set_size(size+1,true);
  baseVector(size) = t;
}

void append(bvec &baseVector, const bvec &newVector)
{
  int size1 = baseVector.size();
  int size2 = newVector.size();
  baseVector.set_size(size1+size2,true);
  if(size2>0)baseVector.set_subvector(size1,newVector);

}

void append(bvec &baseVector, const bvec &newVector1, bvec &newVector2)
{
  int size1 = baseVector.size();
  int size2 = newVector1.size();
  int size3 = newVector2.size();
  baseVector.set_size(size1+size2+size3,true);
  if(size2>0) baseVector.set_subvector(size1,newVector1);
  if(size3>0) baseVector.set_subvector(size1+size2,newVector2);
}




/*!   */
bvec operator^(const bvec& v1, const bvec& v2)
{
  assert(v1.length()==v2.length() && "Error: Inputs length mismatch in operator^ fucntion");
  bvec out(v1.length());

  for(int i=0; i<v1.length(); i++)
    out(i) = v1(i)^v2(i);

  return out;
}

cvec fftShift(cvec &input)
{
  cvec tmp1;
  if(rem(input.length(),2)==0)
    tmp1 = concat(input(input.length()/2,input.length()-1),input(0,input.length()/2-1));
  else
    tmp1 = concat(input(input.length()/2+1,input.length()-1),input(0,input.length()/2));

  return tmp1;
}


bool tossACoin(double probabilityOfHead){
  
  if(probabilityOfHead<0 || probabilityOfHead>1) {cout<<"[both:] Invalid probabilityOfHead to tossACoin().."<<endl;abort();}
  
  if(randu()<=probabilityOfHead)
    return(true);
  else
    return(false);
}


ivec mod(ivec &a,int b)
{
  ivec toReturn(a.length());
  for(int cnt=0;cnt<a.length();cnt++)
    toReturn(cnt)=a(cnt)%b;
  return(toReturn);
}
bvec operator > (ivec a,ivec b)
{
    if(a.size()!=b.size())
    {cout<<"Length of vectors must be same for comparison"<<endl;abort();}
    bvec out(a.size());
    for(int i=0;i<a.length();i++)
    {
        out(i) = a(i)>b(i);
    }
    return out;
}
ivec cumprodback(ivec s)
{
    for(int i=s.length()-2;i>=0;i--)
        s[i]=s[i]*s[i+1];
    return s(1,s.length()-1);
}
//Similar to binary to decimal conversion
// Example:  sizes = [2 2 2 2 2] and p = [1 0 0 1 0] then returns 18.
//Example: sizes = [4 3 2 4] and p = [2 2 1 3] then returns 2*(3*2*4) + 2*(2*4) + 1*(4) + 3 = 71
int decode(ivec sizes,ivec p)   
{
    if(p.length()!=sizes.length() || sum(p>sizes))
    {
        cout<<sizes<<" "<<p<<endl;cout<<"Invalid Vectors for Decoding..."<<endl;abort();
    }
    if(p.length()==1){return p(0);}
    int A = sum(elem_mult(concat(cumprodback(sizes),1),p));
    if(A>=prod(sizes)){cout<<p<<" "<<A<<" "<<sizes;cout<<" Wrong Decoding ..."<<endl;abort();}
    return A;
}
//Similar to decimal to binary conversion
//Example: sizes = [2 2 2 2 2] and num = 20 then returns [1 0 1 0 0]
//Example: sizes = [7 3 4 2] and num = 114 then returns [4 2 2 0]
ivec encode(ivec sizes,int num)
{
    if(num>=prod(sizes) || num<0)
    {
        cout<<"sizes: "<<sizes<<"\tNumber: "<<num<<endl;
        cout<<"Invalid number and sizes combination...Aborting in encode()"<<endl;abort();
    }
    if(sizes.length()==1){return to_ivec(num);}
    ivec b(sizes.length());
    for(int i=sizes.length()-1;i>=0;i--)
    {
        b[i] = num % sizes(i);
        num  = num / sizes(i);
    }
    return b;
}
//Maps every integer a into the range 0 to b-1
int circmod(int a,int b)
{
    while(a<0)
        a +=b;
    return a%b;
}
//Maps double a into the range [0,b).
double circmod(double a, double b)
{
    while(a<0)
        a +=b;
    return std::fmod(a,b);
}
vec circmod(vec a,double b)
{
    for(int i=0;i<a.length();i++)
        a(i) = circmod(a(i),b);
    return a;
}
void system_exec(const char * command)
{
  int status=system(command);
  if (status < 0)
  {
    std::cout << "Error in  system_exec(). command = "<<command<< "\t status = "<<status<< endl;
    cout<<briefSimulationDetails()<<endl;
    abort();
  }
  else
  {
    if (!WIFEXITED(status))
    {
      std::cout << "Program aborted abnormaly. system_exec(). command = "<<command<< "\t status = "<<status<< endl;
      abort();
    }
  }
  
}

int NCr(int N,int k)
{
  int result;
  if(N<0 || k<0)
  {
    cout<<"Negative integers not valid for combination"<<endl;abort();
  }
  if(N<k)
    result=0;
  if(N>=k)
  {
      result=round_i(factorial(N)/(factorial(k)*factorial(N-k)));
  }
  return(result);
  
}

int sumNCr(int N, int maxK)
{
  int nComb=0;
  for(int K=1;K<=maxK;K++)
    nComb+=NCr(N,K);
  return nComb;
}

bmat getBinaryCombinations(int N,int K)
{
  int nComb=NCr(N,K);
  bmat output=zeros_b(nComb,N);
  int l=K-1;
  int rowIndx=0;
  for(int n=(K-1);n<N;n++)
  {
    int nCl=NCr(n,l);
    bmat temp=ones_b(nCl,1);
    
    output.set_submatrix(rowIndx,n,temp);
    if(n>0 && l>0)
    {
      temp=getBinaryCombinations(n,l);
      output.set_submatrix(rowIndx,0,temp);
    }
    rowIndx+=nCl;
  }
  return output;
}
//Returns a bmat which shows which UEs are considered for a particular combination
bmat getAllBinaryCombinations(int N,int maxK)
{
  int nComb=sumNCr(N,maxK);
  
  bmat output=zeros_b(nComb,N);
  int rowIndx=0;
  for(int K=1;K<=std::min(N,maxK);K++)
  {
    bmat temp=getBinaryCombinations(N,K);
    output.set_submatrix(rowIndx,0,temp);
    rowIndx+=NCr(N,K);
  }
  return output;
}

double factorial(int N) // return type is double to avoid overflow
{
  if(N<0)
  {cout<<"Negative integers not valid for factorial"<<endl;abort();}
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


ivec swapVec(ivec inputVector){
  
  ivec outputVector(inputVector.length());
  
  for(int i = inputVector.length() - 1 , j = 0; i >= 0 && j < inputVector.length(); i--, j++)
  {
    outputVector(i) = inputVector(j);
  }
  
  return outputVector;
}


Array<ivec> splitContiguousNumbers(ivec inputVec, int bundlingSize){
  
  ivec splitPoints="-1";
  for(int cnt=1;cnt<inputVec.length();cnt++)
    if((inputVec(cnt)-inputVec(cnt-1))!=1 || (cnt % bundlingSize) == 0) append(splitPoints,cnt-1);
  append(splitPoints,inputVec.length()-1);
    
  Array<ivec> output(0);  
  for(int cnt=1;cnt<splitPoints.length();cnt++)
    append(output,inputVec(splitPoints(cnt-1)+1,splitPoints(cnt)));
    
  return output;
}

void createDirectory(string dirName)
{
    string cmd = "[ ! -d "+dirName+" ] && mkdir "+dirName;
    system_exec(cmd.c_str());
}

bool mergeFiles(string ipFileName,string opFileName,string ext)
{
    if(opFileName=="")
        opFileName=ipFileName;
    string tempFileName = ipFileName+"."+ext+"0";
    if(IsFileExists(tempFileName))
    {
        tempFileName = "cat "+ipFileName+"."+ext+"* > "+opFileName+"; rm -rf "+ipFileName+"."+ext+"*";
        system_exec(tempFileName.c_str());
        return true;
    }
    return false;
}

//! Returns a string which contains Simulation Details in brief
string briefSimulationDetails(double currentTime)
{
  stringstream toReturn;
  toReturn<<"\n%[both:]........... Brief Simulation Details ..........\n";
  toReturn<<"%[both:]..Simulation Start Time = "<<captureTime(1,PRINT_STARTCLOCK);
  toReturn<<"%[detl:].......... Current Time = "<<captureTime(1,PRINT_CURRENTCLOCK);
  toReturn<<"%[detl:]....... Difference Time = "<<captureTime(1,PRINT);
  if(currentTime!=-1)
  toReturn<<"%[detl:]Current Simulation Time = "<<currentTime<<endl;
  toReturn<<"%[detl:]........Simulation Seed = "<<getSeed()<<endl;
  toReturn<<"%[detl:].............Process ID = "<<getpid()<<endl;
  toReturn<<"%[detl:].............nProcesses = "<<nTasks<<endl;
  toReturn<<"%[detl:]...................PATH = "<<getenv ("PWD")<<endl;
  return toReturn.str();
}

/*! \brief Returns a string which contains all configFiles in specified order...
 *  \param[in] INCLUDE_FILES_FIRST specifies the files to be Dumped first in the same order
 *  \param[in] EXCLUDE_FILES specifies the files to be excluded from dumping            */
string dumpConfigFiles()
{
  std::stringstream shellScript, toReturn;
  
  //! Specify the config files names to be dumped first(in the order of dumping)..
  shellScript<<"INCLUDE_FILES_FIRST=\"mySimulationConfig.txt myTxRxConfig.txt mySysConfig.txt\";";    /*! Client can input here */
  //! Specify the config files names to be excluded from dumping..
  shellScript<<"EXCLUDE_FILES=\"myTxAssociationInfo.txt \";";                                         /*! Client can input here */
  shellScript<<"export INCLUDE_FILES_FIRST EXCLUDE_FILES;";
  
  //! Finding out the relative path to INCLUDE_FILES_FIRST
  shellScript<<"prioFiles=`echo $INCLUDE_FILES_FIRST |xargs -n1 |xargs -I{} find ./configFiles/ -name {}`;"; shellScript<<"export prioFiles;";
  //! Finding out the relative path to EXCLUDE_FILES
  shellScript<<"excludeFiles=`echo $EXCLUDE_FILES |xargs -n1 |xargs -I{} find ./configFiles/ -name {}`;"; shellScript<<"export excludeFiles;";
  //! Finding out all remaining files. (\note Here maxdepth=1, ie find only configFiles/*.txt)
  shellScript<<"remFiles=`find ./configFiles/ -maxdepth 1 -name '*.txt' |xargs -n1 |xargs -I{} -i sh -c 'if echo $prioFiles $excludeFiles |grep -q -v {}; then echo {}; fi;'`;";
  shellScript<<"export remFiles;";
  shellScript<<"echo INCLUDE_FILES_FIRST = $prioFiles \\\\nEXCLUDE_FILES = $excludeFiles \\\\nRemaining Files = $remFiles;";
  
  //! Script for printing the config-files...
  shellScript<<"echo \\\\n\\\\n\\\\n%%%%%%%%%%%% --------------- PRINTING PRIORITY CONFIG-FILES FIRSTLY ------------------ %%%%%%%%%%%%%%%;";
  shellScript<<"echo $prioFiles |xargs -n1 |xargs -I{} -i sh -c 'echo \\\\n\\\\n%%%%%% File name: {} %%%%%%%%; cat {}';";
  shellScript<<"echo \\\\n\\\\n\\\\n%%%%%%%%%%%% --------------- PRINTING REMAINING CONFIG-FILES SECONDLY ------------------ %%%%%%%%%%%%%%%;";
  shellScript<<"echo $remFiles |xargs -n1 |xargs -I{} -i sh -c 'echo \\\\n\\\\n%%%%%% File name: {} %%%%%%%%; cat {}';";
  
  toReturn<<exec(shellScript.str())<<"%--------------------- Dumping Over ----------------------------------------------%"<<endl;
  return toReturn.str();
}

double exp_rand(double tLambda)
{
  double t, tExp;
  
  t = randu();
  tExp = -log(1-t)/tLambda;
  
  return tExp;
}

int poisson_rand(double tLambda)
{
//   double tExp;
//   
//   int t = randi(0,10000);
//   tExp = (exp(-tLambda)*pow(tLambda,t))/factorial(t);
//   
//   return tExp;
  
  double cdf = randu(), sum = cdf*exp(tLambda),s=0,term=1;
  for(int cnt=0;cnt<10000;cnt++)
  {
      term = (cnt>0) ? term*tLambda/cnt : 1;
      s += term;
      if(s>sum) return cnt;
  }
  return 10000;
}

double lognormal_rand(double mean, double sigma)
{
	double tZ, tY, tX, randN;

	tZ = randn();

	/* generate the normal variate from the standard
	 * normal variate */
	tY = (sigma * tZ) + mean;

	/* log normal variate is the obtained by raising
	 * normal variate to the power of e */
	tX = pow(M_E, tY);
	randN = (unsigned long int) tX;

	return randN;
}

double pareto_rand(double threshold, double index) 
{
	double tExpRand = exp_rand(index);
	double tParetoRand = threshold * pow(M_E, tExpRand);

	return tParetoRand;
}

/* TODO: Still not clear if this is the correct method to generate gamma random number.
* the inverse of the scale param is taken as the rate param of exponetial distribution.
then Shape param exponential variable are generated and summed up to generate a gamma variable.
The mean of the generated gamma variable this way is matching the given mean, but not the variance. */
double gamma_rand(int tShapeParam, double tScaleParam) 
{
#ifdef USING_ITPP
    Gamma_RNG gamma(tShapeParam,1.0/tScaleParam);
    return gamma.sample();
#else
	double tRate, tExp, tGamma;

	tRate = 1/tScaleParam;
	tGamma = 0.0;

	for (int i = 0; i < tShapeParam; i++) {
		tExp = exp_rand(tRate);
		tGamma += tExp;
	}

	return tGamma;
#endif
}


int nearestPrime(int x){
    int prime;
    if(x == 2){
        prime = 2;
    }
    else if(x > 2){
        for(int i = x;i > 2;i--){
            bool flag = 1;
            for(int j = 2;j*j <= i;j++){
                if(i % j == 0){
                    flag = 0;
                    break;
                }
            }
            if(flag == 1){
                prime = i;
                break;
            }
        }
    }
    else{
        cout<<"Abort: Input should be greater than 1"<<endl;
        abort();
    }
    return prime;
}

imat computePermutations(int N, int r)
{
    //NOTE: when there are r places which can take values from 0 to N-1, this function returns all such combinations possible. 
    // Example if N=2, r=3 then it returns  [0 0 0;0 0 1; 0 1 0; 0 1 1;1 0 0;1 0 1; 1 1 0;1 1 1]
    int nComb = pow(N,r);
    imat allComb(nComb,r);
    for(int cnt=0;cnt<nComb;cnt++)
    {
        allComb.set_row(cnt,encode(N*ones_i(r),cnt));
    }
    return allComb;
}
mat lognormal_rand(int rows, int cols, double mean, double sigma) {
    mat result(rows, cols);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {

            //double mu_mean=log(mean)-0.5*log(1+((sigma*sigma)/(mean*mean)));
           // double sig=sqrt(log(1+((sigma*sigma)/(mean*mean))));
            double tZ = randn();
            //double tY = sig * tZ + mu_mean;
            double tY = inv_dB(sigma) * tZ + inv_dB(mean);
            //cout<<"tY :"<<tY;
            //double tX = exp(tY);
            //double randN = tX;
            double tX = pow(10, (tY/10));
	        double randN = tX;

            result(i, j) = randN;
        }
    }

    return result;
}
