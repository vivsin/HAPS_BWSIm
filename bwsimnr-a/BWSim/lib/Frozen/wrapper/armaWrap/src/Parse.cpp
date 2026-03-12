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

#include "../include/Parse.h"
#include<cstdio>
//The following variables are not related to parse : is related to MPI support   : initialized here to be available globally . extern declaration in parse.h
int currentTaskid=0; //! Used for MPI 
int nTasks=1;
/////////////////////////////////////////////////////

namespace wrapper{
  
  
  /*! Following function is similar to system() call, except that it returns terminal output instead of Command status */
std::string exec(std::string cmd) {
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "ERROR";
    int length = cmd.length()+1;
    char* buffer = new char [length];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, length, pipe) != NULL)
                result += buffer;
    }
    pclose(pipe);
    delete [] buffer;
    return result;
}


}
