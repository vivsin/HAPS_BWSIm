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
    MPI_Barrier(MPI_COMM_WORLD);
    #endif
    
    ConfigFileNames_S myConfigFiles;
    ResultFileNames_S resultFiles;
    initAllFiles(argc,argv,myConfigFiles,resultFiles);

    MCell mcell = createMcellNetwork(myConfigFiles,resultFiles);
    
    cmat channelMat;
    ChannelOutput_S channel=mcell.mySys.generateChannel(4, 0, 0, 0, 1024);
    
    channelMat.set_size(channel.channels(0)(0)(0).length(),10000);
    channelMat.set_col(0,channel.channels(0)(0)(0));
    for(int cnt=1;cnt<10000;cnt++)
    {
        channel=mcell.mySys.generateChannel(4, 0, 0, 71e-6*cnt, 1024);
        channelMat.set_col(cnt,channel.channels(0)(0)(0));
    }
    std::ofstream fout("channel.txt", std::ios::out);
    fout<<"channelMat : "<<channelMat<<endl;
    
    
    #ifdef USING_MPI
    MPI_Finalize();
    #endif
}
