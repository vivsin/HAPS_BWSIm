#ifndef NRDERATEMATCHING_H
#define NRDERATEMATCHING_H

#include<vector>
#include<cmath>
#include<iostream>

template <class T> void nrDerateMatching(std::vector<T>& d, const std::vector<T>& f_input, int rvid, int BG,int z)
{
    
    int startPos[4] = {0, (signed)(d.size())-37*z, (signed)d.size()-(41-8*BG)*z, (signed)d.size()-(13-3*BG)*z};   // Find the explanation for startPos in nrRatematching
    
    auto pos =startPos[rvid];
    if(rvid == 1 && BG==1)
    {
        auto msgSize = d.size()-46*z; // Message bits size = 66z - parity bits size(46z) excluding filler bits
        if(msgSize<17*z)  //17Z and 13Z
            pos = d.size()-46*z;
        else
            pos = 17*z;
    }
    for(auto x:f_input)
    {
        auto temp =(pos++)%d.size();
//         std::cout << "temp: " << temp << "  x: " << x << std::endl;
       d[temp] = d[temp]+x;        
    }
    
    
// std::cout<<"derated = [ " ;
// for(auto x : d)
// { 
// 
//         std::cout<< x<<",";
// }
// 
//  std::cout<<"\b ]"<<std::endl;
// std::cout << d.size() << std::endl;
}


#endif
