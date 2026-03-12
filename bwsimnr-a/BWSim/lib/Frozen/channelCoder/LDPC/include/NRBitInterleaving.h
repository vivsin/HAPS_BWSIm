#ifndef NRBITINTERLEAVING_H
#define NRBITINTERLEAVING_H

#include<vector>
#include<cmath>
#include<iostream>

template <class T> std::vector<T> nrBitInterleaving(std::vector<T> e_ip, int Q)
{   
//     if(Q == 1 ||Q == 2 ||Q == 4 || Q == 6 ||Q == 8 || Q == 10  || Q == 12)
    {
        auto rem = e_ip.size()%Q;
        if(rem!=0)     //make e_ip multiple of Q
        {
            std::cout << "Warning: Resizing BitInterleaver input by appending zeros to be multiple of Q" << std::endl;
            e_ip.resize(e_ip.size()+Q-rem,0);
        }
    //     std::cout<<"[ " ;
    //     for(auto x : e_ip)
    //     { 
    //         std::cout<< x<<",";
    //     }
    //     std::cout<<"\b ]"<<std::endl;
        std::vector<T> f_op(e_ip.size(),0);
        auto ncol = e_ip.size()/Q;
        for(auto j=0;j < ncol;j++)
        {
            for (auto i = 0; i < Q;i++)
            {
                f_op[i+j*Q] = e_ip[(i*ncol)+j];
            }
        }
                
    //     std::cout<<"[ " ;
    //     for(auto x : f_op)
    //     { 
    //         std::cout<< x<<",";
    //     }
    //     std::cout<<"\b ]"<<std::endl;
        
        return std::move(f_op);
    }
//     else
//     {
//         std::cout << "Wrong number of coded bits per QAM symbol 'Q' chosen: "<< Q <<std::endl;
//         abort();
//     }
    
    
}

template <class T> std::vector<T> nrBitDeInterleaving(std::vector<T> f_ip, int Q, int rem2)
{
    std::vector<T> e_op(f_ip.size(),0);
    auto ncol = e_op.size()/Q;
    for(auto j=0;j < ncol;j++)
    {
        for (auto i = 0; i < Q;i++)
        {
            e_op[(i*ncol)+j] = f_ip[i+j*Q];   // Reverse of Interleaving
        }
    }
   // e_op.resize(f_ip.size()-Q+rem2);
/*            
    std::cout<<"[ " ;
    for(auto x : e_op)
    { 
        std::cout<< x<<",";
    }
    std::cout<<"\b ]"<<std::endl;
    */
    return std::move(e_op);
}

#endif

