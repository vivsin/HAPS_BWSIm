#ifndef _PC_H_
#define _PC_H_
#include "../../../Frozen/simSupport/include/simSupport.h"

struct PowerControl_S
{
    ivec serviceNodes;
    vec pathlossPerServiceNode;
    
    float alpha;
    double p0dBm;
    double PCmax;
    
    
    void init(float tAlpha, double tp0dBm, double tPCmax, ivec tServiceNodes, vec tpathloss){
        
        alpha = tAlpha;
        p0dBm = tp0dBm;
        PCmax = tPCmax;
        serviceNodes = tServiceNodes;
        pathlossPerServiceNode = tpathloss;
    }
    double getUplinkPowerInWatts(int serviceNode, int nRBs){
        
        double pathloss = pathlossPerServiceNode(find(serviceNodes,serviceNode));
        
        double pTemp;
        pTemp = p0dBm + 10*log10(nRBs) + alpha*pathloss;
        pTemp = min(pTemp,PCmax);
        pTemp = pow(10,(pTemp-30)/10);
        
        return pTemp;
    }
    
};

#endif
