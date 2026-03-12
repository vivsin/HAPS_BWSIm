/*************************************************************************
 * 
 * CEWiT CONFIDENTIAL
 * __________________
 *
 * All Rights Reserved © 2014 CEWiT, India
 *
 * \ NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
 * and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
 * Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
 * express, printed and signed license for use is strictly forbidden.
 */


#include "events/include/wrapper_funcs.h"

int main(int argc, char** argv){
    
#ifndef ENABLE_COUT
  // Disable cout
  //streambuf* orig_buf = cout.rdbuf();
  // set null
  //cout.rdbuf(NULL);
#endif
  
  
    BWSimSystem_S bwsimSystem;
    bwsimSystem.initializeSystem(argc,argv);
    initializeEvents(bwsimSystem);
    
  bool runcondition=true;  
  while(runcondition)
  {
     if(getnext_eventid()!=END_SIMULATION){
      auto ev=getnext_event();
      ev.object();
    }
    else{
      runcondition=false;
    }
  }
    
  if(currentTaskid==0)
  {
    string toExec = "rm "+bwsimSystem.mySysInfo.resultFiles.linkTableFileName;  
    system_exec(toExec.c_str());  
    completeSimulation(bwsimSystem.mySysInfo.myConfigFiles.directory,bwsimSystem.mySysInfo.resultFiles.directory,bwsimSystem.mySysInfo.simulationTimeInSec);
  }
  
  #ifdef USING_MPI
  MPI_Finalize();
  #endif
}
