#ifndef NRRATEMATCHING_H
#define NRRATEMATCHING_H

#include<vector>
#include<cmath>
#include<iostream>

inline std::vector<bool> nrRateMatching(std::vector<bool> d, int E, int rvid, int BG, int z)
{
    std::vector<bool> f;  // Output
    f.reserve(E);
    int startPos[4] = {0, (signed)(d.size())-37*z, (signed)d.size()-(41-8*BG)*z, (signed)d.size()-(13-3*BG)*z};   // Based on BG (1 or 2) starting position is decided according rvid. 
    // rvid =0, startPos =0  No problem with filler bits (in both BG cases)
    // rvid = 1, BG=2: startPos = 13z which is same as 50z - 37z =  (signed)(d.size())-37*z. Reason for setting starting position from the end is to avoid filler bit locations. BG=1: This case is handled seperately in the next if condition.
    // rvid =2, BG=1: startPos = 33z , BG=2: startPos = 25z (Substitute BG in the formula) 
    // rvid =3, BG=1: startPos = 56z = 66z-10z , BG=2: startPos = 43z = 50z - 7z(Substitute BG in the formula).  Reason for setting starting position from the end is to avoid filler bit locations.
    
    
    
    auto pos =startPos[rvid];
    if(rvid == 1 && BG==1)    
    {
        auto msgSize = d.size()-46*z; // Message bits size = 66z - parity bits size(46z) excluding filler bits
        if(msgSize<17*z)  //17Z and 13Z    // If message size is less than 17z, startPos = 20z = 66z-46z (To avoid filler bits)
            pos = d.size()-46*z;
        else
            pos = 17*z;
    }
    while((E-f.size())>d.size()-pos)       
    {
        f.insert(f.end(),d.begin()+pos,d.end());
        pos=0;
    }
    f.insert(f.end(),d.begin()+pos, d.begin()+pos+E-f.size());
    
    return std::move(f);
}

#endif
