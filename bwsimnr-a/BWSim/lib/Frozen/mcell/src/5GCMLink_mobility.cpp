/*************************************************************************
 * 
 * CEWiT CONFIDENTIAL
 * __________________
 *
 * All Rights Reserved © 2012 CEWiT, India
 *
 * NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
 * and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
 * Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
 * express, printed and signed license for use is strictly forbidden.
 */
#include "../include/SCMLink.h"
void SCM5GLink::update5GChannelForMobility(Location_S nodeLocationTx, Location_S nodeLocationRx,double sigma_DS, double deltaT)
{
    
    if(!isReadyFor5GChannel)
    {
        cout<<"SCM uninitialized.."<<endl;abort();
    }
    
    linkStartLoc = nodeLocationRx,linkEndLoc = nodeLocationTx;
    getBoresight_LOS_3Dangle(nodeLocationTx,nodeLocationRx); //step 1c.
    
    update5GChannelPowerDelayProfiles(sigma_DS,deltaT);
    update5GChannelArrivalAndDepartureAngles(deltaT);
    initialize5GChannelModelFromStep9();
    mobiltyModeling.k++;
}

//Equation 7.6-9 in 38.901
void SCM5GLink::update5GChannelPowerDelayProfiles(double sigma_DS, double deltaT)
{
    vec tau_n_Tilde(mobiltyModeling.tau_n_tilde_kminus1.length());
    if(mobiltyModeling.k==0)
    {
        double tau_delta;
        if(scmLinkCondition==_LOS_)
            tau_delta = 0;
        else
            tau_delta = minTau;
        
       double distance3D = find3Ddistance(mobiltyModeling.t0TxLoc,mobiltyModeling.t0RxLoc);  
       
       tau_n_Tilde = mobiltyModeling.tau_n0 + tau_delta + distance3D / lightSpeed;
       
    }
    else
    {
        mat r_tx_kminus1,r_rx_kminus1;
        
        for(int n=0;n<mobiltyModeling.tau_n_tilde_kminus1.length();n++) 
        {
            r_rx_kminus1 = getCartesianVector(pathAoA5GChannel(n),pathZoD5GChannel(n)); 
        
            r_tx_kminus1 = getCartesianVector(pathAoD5GChannel(n),pathZoD5GChannel(n)); 
            
            tau_n_Tilde(n) = mobiltyModeling.tau_n_tilde_kminus1(n) - (r_rx_kminus1.T()*rxVelocityVector+r_tx_kminus1.T()*txVelocityVector)(0) * deltaT / lightSpeed;
        }
    }
    minTau = min(tau_n_Tilde);
    vec tau = tau_n_Tilde - minTau; //step 5
    sort(tau);
    
    //step6 ///////////////////
    
    vec Pn_dash,Pn,Zn;
    double zeta,r_tau;
    ivec Pn_sort_index,skip_index;  
    double cutoff;
    
    r_tau = get5GChannelDelayScaling(scmGridIndx);
    zeta = get5GChannelClusterShadow(scmGridIndx); 
    
    if(isSpConsistEnable)
    {
        vec dummyXn;
        getSpatiallyConsistentClusterDelayAndPower(dummyXn,Zn);
        Zn = zeta*Zn;
    }
    else
    {
        Zn = zeta*randn(nClusters5GChannel); 
    }
    
    Pn_dash = elem_mult(exp(-1*(r_tau-1)*tau/(r_tau*sigma_DS)),inv_dB(-1*Zn));
    Pn = Pn_dash/sum(Pn_dash);
    
    ivec requiredIndices = getIntegers(0,Pn.length()-1);
    
    if(scmLinkCondition == _LOS_ && nClusters5GChannel!=25)
    {
        vec P_Los = Pn;
        P_Los(0) *= K_rice_5G;
        P_Los /= (K_rice_5G+1);
        cutoff = inv_dB(-25)*max(P_Los);
        requiredIndices = find(P_Los>=cutoff);
    }
    else
    {
        cutoff = inv_dB(-25)*max(Pn);
        requiredIndices = find(Pn>=cutoff);
    }
    
    tapPow5GChannel = Pn(requiredIndices); 
    
    ///////////////////////////
    
    double K_dB = 10*log10(K_rice_5G);
    double C_tau = (scmLinkCondition==_LOS_) ? 0.7705 - 0.0433*K_dB + 0.0002*pow(K_dB,2) + 0.000017*pow(K_dB,3) : 1.0;
       
    tapDelays5GChannel = tau(requiredIndices)/C_tau;
    
    mobiltyModeling.tau_n_tilde_kminus1 = tau_n_Tilde;
    
    nTaps5GChannel = tapPow5GChannel.length();
    
    if(isFlatFadedChannel)
    {
        tapPow5GChannel=ones(1);
        nTaps5GChannel = 1;
        tapDelays5GChannel.set_length(1,true);
        mobiltyModeling.tau_n_tilde_kminus1.set_length(1,true);
    }
    
}

//Equation 7.6-10a&b,11,12 - 38.901
void SCM5GLink::update5GChannelArrivalAndDepartureAngles(double deltaT)
{
    if(mobiltyModeling.v_bar_dash_rx.length()==0)
    {
        mobiltyModeling.v_bar_dash_rx.set_length(pathAoA5GChannel.length());   
        mobiltyModeling.v_bar_dash_tx.set_length(pathAoA5GChannel.length());   
    }
    for(int n=0;n<pathAoA5GChannel.length();n++)
    {
        if(mobiltyModeling.k>0)
        {
            pathAoD5GChannel(n) += (mobiltyModeling.v_bar_dash_rx(n).T()*PhiCap(deg_to_rad(pathZoD5GChannel(n))))(0)*deltaT/(lightSpeed*mobiltyModeling.tau_n_tilde_kminus1(n)*sin(deg_to_rad(pathZoD5GChannel(n))));
            pathZoD5GChannel(n) += (mobiltyModeling.v_bar_dash_rx(n).T()*ThetaCap(deg_to_rad(pathZoD5GChannel(n)),deg_to_rad(pathAoD5GChannel(n))))(0)*deltaT/(lightSpeed*mobiltyModeling.tau_n_tilde_kminus1(n));
            pathAoA5GChannel(n) += (mobiltyModeling.v_bar_dash_tx(n).T()*PhiCap(deg_to_rad(pathZoA5GChannel(n))))(0)*deltaT/(lightSpeed*mobiltyModeling.tau_n_tilde_kminus1(n)*sin(deg_to_rad(pathZoA5GChannel(n))));
            pathZoA5GChannel(n) += (mobiltyModeling.v_bar_dash_tx(n).T()*ThetaCap(deg_to_rad(pathZoA5GChannel(n)),deg_to_rad(pathAoA5GChannel(n))))(0)*deltaT/(lightSpeed*mobiltyModeling.tau_n_tilde_kminus1(n));
        }
              
        mat Rtx=Rz(-deg_to_rad(pathAoD5GChannel(n)))*
                Ry(pi/2-deg_to_rad(pathZoD5GChannel(n)))*
                mobiltyModeling.XnMat*
                Ry(pi/2-deg_to_rad(pathZoA5GChannel(n)))*
                Rz(deg_to_rad(pathAoA5GChannel(n))+pi);
                
        mat Rrx=Rz(deg_to_rad(pathAoD5GChannel(n))+pi)*
                Ry(pi/2-deg_to_rad(pathZoD5GChannel(n)))*
                mobiltyModeling.XnMat*
                Ry(pi/2-deg_to_rad(pathZoA5GChannel(n)))*
                Rz(-deg_to_rad(pathAoA5GChannel(n)));
   
        // 38.901- Equation 7.6-10b&c
        if(scmLinkCondition==_LOS_)
        {
            mobiltyModeling.v_bar_dash_rx(n) = rxVelocityVector - txVelocityVector;
            mobiltyModeling.v_bar_dash_tx(n) = txVelocityVector - rxVelocityVector;
        }
        else
        {
            mobiltyModeling.v_bar_dash_rx(n) = Rrx*rxVelocityVector - txVelocityVector;
            mobiltyModeling.v_bar_dash_tx(n) = Rtx*txVelocityVector - rxVelocityVector;
        }
        
    }
}
