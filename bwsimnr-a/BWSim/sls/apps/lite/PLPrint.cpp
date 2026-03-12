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


#include "../../Mcell/include/MCell.h"
#include "../../sim/include/SupportingFunctions.h"

int main(int argc, char** argv){
    
    
    #ifdef USING_MPI
    initMPI(&(nTasks),&(currentTaskid),argc,argv);
    #endif
    

    ConfigFileNames_S myConfigFiles;
    ResultFileNames_S resultFiles;
    initAllFiles(argc,argv,myConfigFiles,resultFiles);

    MCell mcell = createMcellNetwork(myConfigFiles,resultFiles);
    
    
    vec distance = to_vec(getIntegers(10,3000,10));
    vec ruPL(distance.length());
    Location_S bsLoc(0,0,1.5);
    for(int cnt=0;cnt<distance.length();cnt++)
    {
        Location_S ueLoc(distance(cnt),0,1.5);
        ruPL(cnt) = (-pathLoss5GRuralMacro(bsLoc,ueLoc,3.5e9,1,_NODECATEGORY_SERVERNODE_,_NODECATEGORY_SERVICENODE_,_MODEL_A_).pathLoss);
    }
    cout<<ruPL<<endl;
    
   
    #ifdef USING_MPI
    MPI_Finalize();
    #endif
}
