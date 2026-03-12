/*************************************************************************
 * CEWiT CONFIDENTIAL
 * __________________
 *
 * All Rights Reserved © 2014 CEWiT, India
 *
 * \NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
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

    cout<<"Generating M cell configuration"<<endl;
    for(int ue = 18 ; ue < 23 ; ue ++)
    {
        cout<<"For ue node id :"<<ue<<endl;
        for(int bs = 0 ; bs < 18 ; bs ++)
        {
        cvec channelMat;
        cout<<"Generating channel between user node and base station :"<<bs<<endl;
        ChannelOutput_S channel=mcell.mySys.generateChannel(ue, 0, bs, 0, 1024);

        channelMat.set_size(channel.channels(0)(0)(0).length(),1);
//      channelMat.set_col(0,channel.channels(0)(0)(0));

        for(int cnt=0;cnt<1;cnt++)
        {
            channel= mcell.mySys.generateChannel(ue, 0, bs, 71e-6*cnt, 1024);
            channelMat = channel.channels(0)(0)(0);
        }
        std::ofstream fout("channel/channel_bs"+to_str(bs)+"_ue_"+to_str(ue)+".txt", std::ios::out);

        cout<<"\t"<<"Dumping Channel"<<endl;
        fout<<"channelMat : "<<channelMat<<endl;
        cout<<"Length"<<channelMat.length()<<endl;
        }
    }
    #ifdef USING_MPI
    MPI_Finalize();
    #endif
}
