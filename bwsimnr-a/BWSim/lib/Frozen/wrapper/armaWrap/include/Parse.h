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

#ifndef PARSE_HEADER_H
#define PARSE_HEADER_H

//#define _DEBUG_PARSE_ENABLE
///////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <string>
#include <sstream>          //added for int to string conversion
#include <stdlib.h>         //added for function system doing shell commands
#include <complex>
#include <ios>
#include <iomanip>
#include <typeinfo>
//The following variables are not related to parse : is related to MPI support   
extern int currentTaskid; // Used for MPI
extern int nTasks;
///////////////////////////////////////////////////////////////////////////


using std::cout;                        /* These lines are needed for use of cout and endl */
using std::string;
using std::endl;
///////////////////////////////////////////////////////////////////////////////////
namespace wrapper{
/*! Following function is similar to system() call, except that it returns terminal output instead of Command status */
std::string exec(std::string cmd);



template <class T>
std::istream& operator>>(std::istream &is, std::complex<T> &x)
{
  T re, im;
  char c=0;
  is >> c;
  if (c == '(') {
    is >> re >> c;
    if (c == ',') {
      is >> im >> c;
      if (c == ')') {
	x = std::complex<T>(re, im);
      }
      else {
	is.setstate(std::ios_base::failbit);
      }
    }
    else if (c == ')') {
      x = std::complex<T>(re, T(0));
    }
    else {
      is.setstate(std::ios_base::failbit);
    }
  }
  else {
    is.putback(c);
    is >> re;
    if (!is.eof() && ((c = static_cast<char>(is.peek())) == '+' || c == '-')) {
      is >> im >> c;
      if (c == 'i') {
	x = std::complex<T>(re, im);
      }
      else {
	is.setstate(std::ios_base::failbit);
      }
    }
    else {
      x = std::complex<T>(re, T(0));
    }
  }
  return is;
}



/*! \brief Search the string 'stringToParse' in file 'inputFileName' & place the correspoding value to 'value'
    \return Status of parsing. If file is missing or parse value is blank, then false is returned
     	*/



template <class T>  bool parse(string inputFileName,string stringToParse,T &value){

    std::ostringstream convert;
    convert << currentTaskid;
    std::string inputFileNameTemp = inputFileName+".temp"+convert.str();
    std::ostringstream shellScript;
    string cfile=inputFileName; string variablename=stringToParse;

    /*! Following Perl-script does following
    1. Delete all comments/blank-lines, and spaces @ beginning/end/around '='/';'. 2. concat lines ending with '...'
    3. split lines with multiple parses.   4. And print into inputFileNameTemp file ..    */
    shellScript<<"cp -f "<<inputFileName<<" "<<inputFileNameTemp<<"; perl -i -pe 'BEGIN{undef $/;} s/\\R?\\h*\\/\\*\\X*?\\*\\/\\h*//smg, s/(\\R)*\\h*\\/\\/[[:print:]]*//smg, s/^\\h*\\R//smg, s/\\h*=\\h*/=/sg, s/\\h*\\;\\h*/\\;/sg, s/\\h*\\R\\h*/\\n/smg, s/\\.\\.\\.\\s*\\n//smg, s/\\;([^;\\n]*=)/\\;\\n\\1/g, s/\\;\\h*$//g, s/\\\"\\h*$//g  ' "<<inputFileNameTemp<<";";
    //! Now grep the only right side of '=' by removing beginning/trailing '=','\s','"','\n'
    shellScript<<"grep -P '^\\s*"<<stringToParse<<"' "<<inputFileNameTemp<<" |grep -Po '=.*' |grep -Po '[^= \"].*' |grep -Po '.*[^\\010; \"]' |head -1 |perl -ne 'chomp and print'";
   // cout<<"shellScript.str() = "<<shellScript.str()<<endl;
    //! Now store the output of shell-command to shellScriptOutput
    std::stringstream shellScriptOutput;  shellScriptOutput<<exec(shellScript.str());
    //! Remove the temp-file (which got created by removing C-comments in input-file)
    cout<<exec(std::string("rm -f ")+inputFileNameTemp);
    //! If parse-value is blank, then return false..
    if( shellScriptOutput.str()=="") {
        shellScriptOutput >> value;
#ifdef _DEBUG_PARSE_ENABLE
        cout<<"cfile = "<<cfile<<"\t variablename = "<<variablename<<"\t value = "<<value<<endl;
#endif
        return false;
    }

    //! Move the shellScriptOutput to value
    bool valueTemp;
    if(typeid(value).name()==typeid(valueTemp).name()) {        //! If parse value expected is bool,,
        const char* s1="true";
        const char* s2="1";
        if( (shellScriptOutput.str().compare(s1)==0 ) || (shellScriptOutput.str().compare(s2)==0 ))
           valueTemp=true;
        else
           valueTemp=false;
        T* temp = reinterpret_cast<T*>(&valueTemp);
        value = *temp;
    }
    else
        shellScriptOutput >> value;

#ifdef _DEBUG_PARSE_ENABLE
        cout<<"cfile = "<<cfile<<"\t variablename = "<<variablename<<"\t value = "<<value<<endl;
#endif
    return true;

}


struct Parser{
  std::string configFile;
  Parser(string configFilename):configFile(configFilename){}
  template<class T>bool get(T & t,string configName)
  {
    return parse<T>( configFile,configName,t);
  }
};

}
#endif // PARSE_HEADER_H

