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
    
  
//     Antenna antenna = mcell.mySys.aNodes(2).txAntenna;
//     vec hAngle = to_vec(getIntegers(-180,180));
//     vec vAngle = to_vec(getIntegers(-90,90));
//     //cout << "anode" << aNodes << endl;
//     mat antGain(hAngle.length(),vAngle.length());
//     //cout<< "antenna gain for :" << hAngle << "," <<vAngle << antGain << endl;
//     for(int h=0;h<hAngle.length();h++)
//     {
//         for(int v=0;v<vAngle.length();v++)
//         {
//             antGain(h,v) = antenna.getAntennaGain(hAngle(h),vAngle(v),2000000000);
//             //cout<< "antenna gain for :" << hAngle(h) << "," <<vAngle(v) << antGain << endl;
//         }
//     }
    /*
    for(int ai = 0;ai<61;ai++)
    {
        double ag = mcell.mySys.aNodes(0).txAntenna.getAntennaGain(mcell.mySys.aNodes(ai).coverageInfo.centre,2000000000);
        cout << "Antenna gain from beam 0 to centre location(" << mcell.mySys.aNodes(ai).coverageInfo.centre << ") of hexagon "<< ai << " is: " << ag << endl;
    }
    */
    //ofstream MyFile("ant_apattern2.txt");

    // Write to the file
    //MyFile << antGain;

    // Close the file
    //MyFile.close();
        
        
//     Location_S satLoc = mcell.mySys.aNodes(0).getLoc();
//     Location_S ueLoc = 
//     
//     
//     mat antGain(hAngle.length(),vAngle.length());
//     for(int h=0;h<hAngle.length();h++)
//         for(int v=0;v<vAngle.length();v++)
//             antGain = antenna.getAntennaGain(hAngle(h),vAngle(v),2000);

        
        
        
        
    
    #ifdef USING_MPI
    MPI_Finalize();
    #endif
}

