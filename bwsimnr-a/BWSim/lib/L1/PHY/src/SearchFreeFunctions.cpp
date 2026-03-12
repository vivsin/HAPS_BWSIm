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

#include "../include/CQIEstimatorNR.h"
#ifndef USING_ITPP
cvec conj(cvec A)
{
    cvec B(A.length());
    for(int i=0;i<A.length();i++)
    {
        B(i)=conj(A(i));
    }
    return B;
}
dComplex operator*(cvec a,cvec b)
{
    return sum(elem_mult(a,b));
}
#endif

//Modifies theta into [0,2*pi)
double Adjust(double a)
{
    return circmod(a,2*pi);
}
//Thetas are adjusted to 0 to 2*pi from -pi to pi 
vec Adjust(vec Theta)
{
    for(int i=0;i<Theta.length();i++)
        Theta(i)= Adjust(Theta(i));
    return Theta;
}
//Computes Wideband PMI Indices for Single Layer Transmission and Single Panel Codebook Mode 1
type1PMI_S CQIEstimatorNR::computeWideBandPMI(cmat channel, int nLayers)
{
    // Computing average wideband channel
    //     cmat avgChannel = zeros_c(wideBandChannel(0).rows(), wideBandChannel(0).cols());
    //     for(int i=0; i< wideBandChannel.length(); i++)
    //         avgChannel += wideBandChannel(i);
    //     avgChannel = avgChannel/wideBandChannel.length();
    
    // SVD decomposition of average channel
    cmat U,V;  vec S;
    svd(channel,U,S,V);
    
    CodeBookConfig_S codebookConfig = mCodebook->getCodeBookConfig();
    // initializing codebook related parameters
    int N1=codebookConfig.N1, N2=codebookConfig.N2, P=codebookConfig.P, O1=codebookConfig.O1,O2=codebookConfig.O2, i2Size=4;
    
    // Taking the first column in V (This is only for Rank one)
    cvec v = V.get_col(0);
    
    //cout<<"v : \n"<<v/v(0)<<endl;
    
    // Decalarations of sub vectors v0, v1, and v2
    Array<cvec> vSub1, vSub2, vSub0;
    vSub2.set_length(N1*P);
    vSub1.set_length(N2*P);
    vSub0.set_length(N1*N2);
    
    double thetaM=0.0, thetaL=0.0, thetaN = 0.0;
    dComplex tmpSum(0,0);
    
    // Necessary variable declarations for forming vectors beta1, beta2, and beta0
    mat angleL(N1,1), angleM(N2,1), angleN(P,1), B1(N2*P,1), B2(N1*P,1), B0(0,0);
    
    
    // Coarse thetaM estimation using Equation 8 and extracing the v2 sub vectors
    for(int n1p = 0; n1p<N1*P; n1p++)
    {
        vSub2(n1p).set_length(N2);
        int n2=0;
        for(n2=0; n2<N2-1; n2++)
        {
            tmpSum +=  conj(v(n1p*N2+n2))*v(n1p*N2+n2+1);    // Equation 8
            vSub2(n1p)(n2) = v(n1p*N2+n2);                   // forming sub vectors v2
        }
        vSub2(n1p)(n2) = v(n1p*N2+n2);
    }
    thetaM = arg(tmpSum);   // Equation 8
    thetaM = thetaM<0?2*pi+thetaM:thetaM;
    
    
    // Coarse thetaL estimation based on  Equation 8 
    tmpSum = dComplex(0,0);
    for(int p=0; p<P; p++)
        for(int n2=0; n2<N2; n2++)
            for(int n1 =1; n1<N1; n1++) // n1 starting from 1
            {
                tmpSum +=  conj(v(n2+(n1-1)*N2+N1*N2*p))*v(n2+n1*N2+N1*N2*p);  // Equation 8 for thetaL
            }
            
            thetaL = arg(tmpSum);        // Equation 8 for thetaL
            thetaL = thetaL<0?2*pi+thetaL:thetaL;
        
        // Extracing the v1 sub vectors
        int cnt=0;
        for(int p=0; p<P; p++)
            for(int n2=0; n2<N2; n2++)
            {
                vSub1(cnt).set_length(N1);
                for(int n1=0; n1<N1; n1++)
                {
                    vSub1(cnt)(n1)= v(n1*N2+n2+p*N1*N2);   // forming sub vectors v1
                }
                cnt++;
            }
            
            // Coarse thetaN estimation based on Equation 8 and extracing the v0 sub vectors
            tmpSum = dComplex(0,0);
            cnt = 0;
            for(int n1=0; n1<N1; n1++)
                for(int n2=0; n2<N2; n2++)
                {
                    vSub0(cnt).set_length(P);
                    tmpSum +=  conj(v(n2+n1*N2+N1*N2))*v(n2+n1*N2);   // Equation 8 for thetaN
                    for(int p=0; p<P; p++)
                    {
                        vSub0(cnt)(p)=v(n2+n1*N2+p*N1*N2);   // forming sub vectors v0
                    }
                    cnt++;
                }
                thetaN = abs(arg(tmpSum));  // Equation 8 for thetaN
                thetaN = thetaN<0?2*pi+thetaN:thetaN;
                
                
                /* Forming Beta1, Beta2, and Beta0*/
                angleL.set_col(0,to_vec(thetaL*getIntegers(0,N1-1)));
                B2 = concat_vertical(angleL, thetaN+angleL);   // Equation 7
                
                angleM.set_col(0,to_vec(thetaM*getIntegers(0,N2-1)));
                B1 = concat_vertical(angleM, thetaN+angleM);   // Equation 9
                
                angleN.set_col(0,to_vec(thetaN*getIntegers(0,P-1)));
                for(int i=0; i<N1; i++)
                    B0 = concat_vertical(B0, i*thetaL+angleM);  // Equation 10
                    
                    
                    /* Estimation of avg v1 using (EQUATION 11)*/
                    
                    cvec V1est = zeros_c(N1);
                for(int n=0; n<N2*P; n++)
                    V1est += vSub1(n)*expj(-B1(n)); //Equation 11
                    
                    V1est = V1est/(N2*P);
                
                /* Fine tuning thetaL and estimation of l  (EQUATION 12 and 13)*/
                
                tmpSum = dComplex(0,0);
                for(int n1=0; n1<N1-1; n1++)
                    tmpSum +=  conj(V1est(n1))*V1est(n1+1);    // Equation 12
                    
                    thetaL = arg(tmpSum);  
                thetaL = thetaL<0?2*pi+thetaL:thetaL;
                int l = mod((int)std::round(O1*N1*thetaL/(2*pi)),O1*N1);   // Equation 13
                
                /* Estimation of avg v2 using (EQUATION 11)*/
                cvec V2est = zeros_c(N2);
                for(int n=0; n<N1*P; n++)
                    V2est += vSub2(n)*expj(-B2(n)); //Equation 11
                    
                    V2est = V2est/(N1*P);
                
                /* Fine tuning thetaM and estimation of m  (EQUATION 12 and 13)*/
                tmpSum = dComplex(0,0);
                for(int n2=0; n2<N2-1; n2++)
                    tmpSum +=  conj(V2est(n2))*V2est(n2+1);    // Equation 12
                    
                    thetaM = arg(tmpSum);
                thetaM = thetaM<0?2*pi+thetaM:thetaM;
                int m = mod((int)std::round(O2*N2*thetaM/(2*pi)),O2*N2);   // Equation 13
                
                /* Estimation of avg v0 using (EQUATION 11)*/
                cvec V0est = zeros_c(P);
                for(int n=0; n<N1*N2; n++)
                    V0est += vSub0(n)*expj(-B0(n)); //Equation 11
                    
                    V0est = V0est/(N1*N2);
                
                /* Fine tuning thetaM and estimation of m  (EQUATION 12 and 13)*/
                
                thetaN = arg(conj(V0est(0))*V0est(1));   // Equation 12
                thetaN = thetaN<0?2*pi+thetaN:thetaN;
                int n = mod((int)std::round(2*thetaN/pi),i2Size);   // Equation 13
                
                type1PMI_S precIndex;
                precIndex.i11 = l;precIndex.i12 = m;precIndex.i2 = n; precIndex.i13 = 0;
                
                //int wideBandPMI = mCodebook->getPrecoderIndxAbsNR(nLayers, precIndex);
                cout<<precIndex.printValues()<<endl;
                return precIndex;
}

// Computes i13 from the given k1,k2 and nLayers
int CQIEstimatorNR::computei13(int k1,int k2,int nLayers)
{
    CodeBookConfig_S codebookConfig = mCodebook->getCodeBookConfig();
    int N1 =codebookConfig.N1,N2=codebookConfig.N2,O1=codebookConfig.O1,O2=codebookConfig.O2,i13=0;
    //modifying k1 and k2 to nearest multiples of O1 and O2
    k1 = (k1/O1 + (k1%O1>2))*O1;
    k2 = (k2/O2 + (k2%O2>2))*O2;
    int k1bO1 = k1/O1;
    int k2bO2 = k2/O2;
    if(k1bO1==N1){k1bO1--;} // Special cases like i11=0; i11+k1 = N101-1
    if(k2bO2==N2){k2bO2--;} // Special cases like i12=0; i12+k2 = N202-1
    //Mapping is done according to the table 5.2.2.2.1-3,4, 5.2.2.2.2-2 TS 38.214
    if(nLayers==2)
    {
        if(N2>1 && N1>N2 && k1bO1==2){i13=3;}
        else if(k2==0){i13 = k1bO1;}
        else{i13 = k1bO1 + 2*k2bO2;}
    }
    else if(nLayers<=4)
    {
        if(k1==0 && k2==0){ i13=0;} //Selecting the nearest possible beam due to invalid combination
        else if( N2==2 && k1bO1==2 && k2bO2==0 && N1>N2){ i13 = 3;}
        else if(k2==0){i13 = k1bO1-1;}
        else{ i13 = k1bO1-1 + 2*k2bO2;}
    }
    return i13;
}

//Computes i14 and i2 for multipanel codebook mode 2 by finding the Spectral Efficiency of the adjacent precoders
//possible for one particular p
// If p+n=3, then two combinations are possible ie p=3,n=0 or p=2,n=1
//Amplitudes of precoders corresponding to p=3,n=1 and p=2,n=0 are compared and corresponding combination
//is selected
CSI_Info_S CQIEstimatorNR::computeCSIinfoForMultipanelMode2(vec Thetas,int nLayers,type1PMI_S precIndex,Link_Param_S* linkParam)
{
    CBParam_S CBparam = mCodebook->getCodebookParam(nLayers);
    ivec n_(3),p_(2);
    n_(0) = mod((int)std::round(Thetas(4)*2/pi),CBparam.i2Sizes(0));
    
    ivec i2plusi14(2);
    i2plusi14(0) = mod((int)std::round(Thetas(5)*2/pi),CBparam.i2Sizes(1)+CBparam.i14Sizes(0)-1);//represents p1+n1
    i2plusi14(1) = mod((int)std::round(Thetas(6)*2/pi),CBparam.i2Sizes(2)+CBparam.i14Sizes(1)-1);//p2+n2
    
    precIndex.i14=0;precIndex.i2=0;
    int startIndx = mCodebook->getPrecoderIndxAbsNR(nLayers,precIndex);
    int stopIndx = startIndx + CBparam.i14Size*CBparam.i2Size - 1;
    
    Array<cmat> codebook= mCodebook->getCodebook(nCSIRSPorts,nLayers,1)(startIndx,stopIndx); //Codebook of precoders for all possible i14 and i2 
    
    ivec Indices(0);
    int i2Size=CBparam.i2Size;

    for(int i=0;i<2;i++)
    {
        ivec n(3),p(2);
        n(0)=n_(0);
        if(i2plusi14(1-i)==4)
        {
            p(1-i)=3;n(2-i)=1; 
        }
        else
        {
            p(1-i)=i2plusi14(1-i);n(2-i)=0;
        }
        if(i2plusi14(i)==0 || i2plusi14(i)==4) // compare 0,0 and 3,1 since both give same phase of 2*pi.
        {
            p(i)=0;n(i+1) = 0; 
            append(Indices,decode(CBparam.i14Sizes,p)*i2Size+decode(CBparam.i2Sizes,n)); // Append the indices which are possible for same i14plusi2
            p(i)= 3;n(i+1) = 1;
            append(Indices,decode(CBparam.i14Sizes,p)*i2Size+decode(CBparam.i2Sizes,n));
        }
        else
        {
            p(i)=i2plusi14(i)-1;n(i+1) = 1; 
            append(Indices,decode(CBparam.i14Sizes,p)*i2Size+decode(CBparam.i2Sizes,n)); // Append the indices which are possible for same i14plusi2
            p(i)= i2plusi14(i);n(i+1) = 0;
            append(Indices,decode(CBparam.i14Sizes,p)*i2Size+decode(CBparam.i2Sizes,n));
        }
    }
    
    Array<cmat> partialCodebook = getFromArray(codebook,Indices);                
    
    CSI_Info_S csiInfo = getCSIinfoOfBestPrecoderUsingAvgChannel(nLayers,partialCodebook,linkParam); //used to compute PMI only with less time complexity.
    int PMI = startIndx + Indices(csiInfo.PMI);
    partialCodebook = partialCodebook(csiInfo.PMI,csiInfo.PMI);
    csiInfo = getCSIinfoOfBestPrecoder(nLayers,partialCodebook,linkParam); //used to compute actual CQI for the selected precoder.

    csiInfo.PMI = PMI;
    return csiInfo;
}

//provides estimates using inter-layer Searchfree
cvec getCrossLayerThetaEstimates(cmat V,int nLayers,ivec N)
{
    int CBtype = N(0),CBmode=N(1),Ng=N(2),N1=N(3),N2 = N(4),P=N(5);
    
    int N1N2 =N1*N2,N1N2P = P*N1N2;
    int nPorts = Ng*N1N2P;
    cvec thetaEst= zeros_c(5+Ng-1+(CBmode+CBtype)/2);
    if(nLayers>2)
    {
        for(int cnt=0;cnt<2;cnt++) // i11 and i12
        {
            int L = (cnt==0) ? N2 : 1;
            int size =  nPorts/L;
            for(int i=0;i<size-1;i++)
            {
                thetaEst(cnt) += conj(V.get_col(0)(i*L,(i+1)*L-1))*V.get_col(2)((i+1)*L,(i+2)*L-1) + conj(V.get_col(2)(i*L,(i+1)*L-1))*V.get_col(0)((i+1)*L,(i+2)*L-1);
                if(nLayers==4) //thetaL', thetaM'
                    thetaEst(cnt+2) += conj(V.get_col(1)(i*L,(i+1)*L-1))*V.get_col(3)((i+1)*L,(i+2)*L-1) + conj(V.get_col(3)(i*L,(i+1)*L-1))*V.get_col(1)((i+1)*L,(i+2)*L-1);
            }
        }
    }
    if(CBmode*CBtype)
    {
        for(int i=1;i<P*Ng;i++)
            for(int l1=0;l1<nLayers;l1++)
                for(int l2=l1+1;l2<nLayers;l2++)
                {
                    //phi_n0, ap_1*bn_1 , ap_2*b_n2
                    thetaEst(3+i) += conj(V.get_col(l1)(0,N1N2-1))*V.get_col(l2)(i*N1N2,(i+1)*N1N2-1) + conj(V.get_col(l2)(0,N1N2-1))*V.get_col(l1)(i*N1N2,(i+1)*N1N2-1);
                }
    }
    else
    {
        for(int i=0;i<Ng;i++)
            for(int l1=0;l1<nLayers;l1++)
                for(int l2=l1+1;l2<nLayers;l2++)
                {
                    //phi_n
                    thetaEst(4) += conj(V.get_col(l1)(2*i*N1N2,(2*i+1)*N1N2-1))*V.get_col(l2)((2*i+1)*N1N2,(2*i+2)*N1N2-1) + conj(V.get_col(l2)(2*i*N1N2,(2*i+1)*N1N2-1))*V.get_col(l1)((2*i+1)*N1N2,(2*i+2)*N1N2-1);
                    if(i>0)//phi_p's
                        thetaEst(4+i) += conj(V.get_col(l1)(0,N1N2P-1))*V.get_col(l2)(i*N1N2P,(i+1)*N1N2P-1) + conj(V.get_col(l2)(0,N1N2P-1))*V.get_col(l1)(i*N1N2P,(i+1)*N1N2P-1);
                }
    }
    return thetaEst;
}

// Thetas is a vector of estimates of i11,i12,i11_,i12_,i2,i14
// for mode 1 and single panel mode 2 Thetas= (i11,i12,i11_,i12_,i2,i14_1,i14_2) / (l,m,l_,m_,n,p1,p2)
//for multipanel mode 2 Thetas = (i11,i12,i11_,i12_,i20,i21+i141,i22+i142) / (l,m,l_,m_,n0,p1+n1,p2+n2)
// Computes Initial Thetas from the V matrix  
vec computeCoarseThetas(cmat V,int nLayers,ivec N)
{
    int CBtype = N(0),CBmode=N(1),Ng=N(2),N1=N(3),N2 = N(4),P=N(5);
    
    int N1N2 =N1*N2,N1N2P = P*N1N2;
    
    dComplex tmpSum1(0,0);
    cvec tmpSum = zeros_c(4+3*(CBmode*CBtype));
    
    // computing theta l,m,l_,m_
    for(int l=0;l<nLayers;l++)
    {
        int lp2=l%2;
        cvec v=V.get_col(l);
        for(int ng=0;ng<Ng;ng++)
            for(int p=0;p<P;p++)
            {
                for(int n1=0;n1<N1;n1++)
                    for(int n2=0;n2<N2-1;n2++)
                    {
                        tmpSum(2*lp2+1)+= conj(v((ng*P+p)*N1N2+n1*N2+n2))*v((ng*P+p)*N1N2+n1*N2+n2+1); //theta m,m_
                    }
                for(int n2=0;n2<N2;n2++)
                    for(int n1=0;n1<N1-1;n1++)
                    {
                        tmpSum(2*lp2)+= conj(v((ng*P+p)*N1N2+n1*N2+n2))*v((ng*P+p)*N1N2+(n1+1)*N2+n2); // theta l,l_
                    }
                if((ng*P+p)>0 && CBmode*CBtype)
                {
                    cvec V_n = v((ng*P+p)*N1N2,((ng*P+p)+1)*N1N2-1);
                    tmpSum(4+(ng*P+p)-1) += conj(v(0,N1N2-1))*V_n; //theta n0,n1+p0,n2+p1
                }
            }
    }
    
    if(!(CBtype && CBmode)) // for thetaN for mode 1 and SP mode 2 
    {
        cvec tempSum;
        tmpSum1=dComplex(0,0);tempSum.set_length(Ng-1);
        for(int l=0;l<nLayers;l++)
        {
            cvec v = V.get_col(l);
            for(int ng=0;ng<Ng;ng++)
                for(int n1=0;n1<N1;n1++)
                    for(int n2=0; n2<N2; n2++)
                        for(int p=0; p<P-1; p++)
                        {
                            tmpSum1 += conj(v(ng*N1N2P + p*N1N2 + n1*N2 + n2))*v(ng*N1N2P + (p+1)*N1N2 + n1*N2 + n2);
                        }
            for(int ng=1;ng<Ng;ng++)
            {
                tempSum(ng-1) = conj(v(0,N1N2P-1))*v(N1N2P*ng,N1N2P*(ng+1)-1); 
            }
        }
        append(tmpSum,concat(tmpSum1,tempSum));
    }
    if(nLayers>1)
        tmpSum +=  getCrossLayerThetaEstimates(V,nLayers,N);
    
    vec courseThetas = arg(tmpSum);
    if(courseThetas.length() != 5+Ng-1+(CBmode+CBtype)/2)
    {
        cout<<"Inappropriate Theta Length..."<<endl;abort();
    }
    return courseThetas;
}

//Computes Betas from the given Thetas Based on Eqns 7,9,10
//Each Beta constitutes of matrices of angles required for finding corresponding Theta
Array<mat> getBetas(vec Thetas,int nLayers,ivec N)
{
    int CBtype = N(0),CBmode=N(1),Ng=N(2),N1=N(3),N2 = N(4),P=N(5);
    
    mat angleL(N1,1), angleM(N2,1), AngleN(P,1),AngleP(Ng,1),AnglePplusN(P*Ng,1);
    
    //For mode 1, one matrix to find each thetal,l_,m,m_,n and one matrix to find all the angles of i14 ie p(1,Ng-1)
    //For mode 2, one matrix to find each thetal,l_,m,m_ and one matrix to find each n0,n1+p1,n2+p2
    Array<mat> Betas(5+std::min(Ng-1,1)+(CBtype+CBmode)/2); 
    
    angleL.set_col(0,to_vec(getIntegers(0,N1-1)));
    
    angleM.set_col(0,to_vec(getIntegers(0,N2-1)));
    
    AngleN.set_col(0,to_vec(Thetas(4)*getIntegers(0,P-1)));
    
    if(Ng!=1){AngleP.set_col(0,concat(0.0,Thetas(5,5+Ng-2)));}
    else{AngleP.set_col(0,to_vec(getIntegers(0,Ng-1)));}
    
    if(CBmode*CBtype)
    {
        AnglePplusN.set_col(0,concat(0.0,Thetas(4,6)));
    }
    
    Array<mat> AngleL(2),AngleM(2);
    
    AngleL(0)=Thetas(0)*angleL;AngleL(1)=Thetas(2)*angleL;
    AngleM(0)=Thetas(1)*angleM;AngleM(1)=Thetas(3)*angleM;
    
    for(int l=0;l<nLayers;l++)
    {
        int lp2 = l%2; // used to separate l,m and l_,m_
        // if 0 means l,m else l_,m_
        // All the betas corresponding to one theta are concatenated vertically for all layers from which
        //they are calculated.
        //for l,m betas are formed only from odd layers similarly l_,m_ from even layers.
        //for n, p they are formed from all layers.
        if(CBmode && CBtype) //Multipanel Mode 2
        {
            Betas(2*lp2)=concat_vertical(Betas(2*lp2),kronAdd(AnglePplusN,AngleM(lp2)));
            Betas(2*lp2+1)=concat_vertical(Betas(2*lp2+1),kronAdd(AnglePplusN,AngleL(lp2)));
            for(int n=4;n<7;n++)
            {
                Betas(n) = concat_vertical(Betas(n),kronAdd(AngleL(lp2),AngleM(lp2)));
            }
        }
        else    
        {
            Betas(2*lp2)=concat_vertical(Betas(2*lp2),kronAdd(AngleP,kronAdd(AngleN,AngleM(lp2))));
            Betas(2*lp2+1)=concat_vertical(Betas(2*lp2+1),kronAdd(AngleP,kronAdd(AngleN,AngleL(lp2))));
            Betas(4) = concat_vertical(Betas(4),kronAdd(AngleP,kronAdd(AngleL(lp2),AngleM(lp2))));
            if(Ng>1)
            {
                Betas(5) = concat_vertical(Betas(5),kronAdd(AngleN,kronAdd(AngleL(lp2),AngleM(lp2))));
            }
        }
    }
    return Betas;
}

//Computes Subvectors Required for each Theta for Tuning
Array<cmat> getSubVectors(cmat V,int nLayers,ivec N)
{
    int CBtype = N(0),CBmode=N(1),Ng=N(2),N1=N(3),N2 = N(4),P=N(5);
    
    int N1N2 = N1*N2,N1N2P = P*N1N2;
    
    //Subvectors are formed similar to Beta Matrices.
    // All the subvectors corresponding to one theta from all layers are concatenated horizontally 
    //to form a matrix.
    Array<cmat> vSub(5+std::min(Ng-1,1)+(CBtype+CBmode)/2);
    
    for(int l=0;l<nLayers;l++)
    {
        int lp2=l%2;
        //if 0 represents l,m else l_,m_
        cvec v=V.get_col(l);
        for(int ng=0;ng<Ng;ng++)
        {
            for(int p=0;p<P;p++)
            {
                for(int n2=0;n2<N2;n2++)        //for l,l_
                {
                    cmat Vsub1(N1,1);
                    for(int n1=0;n1<N1;n1++)
                        Vsub1(n1) = v(ng*N1N2P + p*N1N2 + n1*N2 +n2);
                    vSub(2*lp2)=concat_horizontal(vSub(2*lp2),Vsub1);
                }
                for(int n1=0;n1<N1;n1++)        //for m,m_
                {
                    cmat Vsub2(N2,1);
                    for(int n2=0;n2<N2;n2++)
                        Vsub2(n2) = v(ng*N1N2P + p*N1N2 + n1*N2 +n2);
                    vSub(2*lp2+1)=concat_horizontal(vSub(2*lp2+1),Vsub2);
                }
            }
            if(!(CBmode && CBtype))         //for n
            {
                for(int n1=0;n1<N1;n1++)
                    for(int n2=0;n2<N2;n2++)
                    {
                        cmat Vsub0(P,1);
                        for(int p=0;p<P;p++)
                        {
                            Vsub0(p) = v(ng*N1N2P + p*N1N2 + n1*N2 +n2);
                        }
                        vSub(4) = concat_horizontal(vSub(4),Vsub0);
                    }
            }
        }
        if(CBmode*CBtype)      //for n0,n1+p0,n2+p1
        {
            for(int n1=0;n1<N1;n1++){
                for(int n2=0;n2<N2;n2++){
                    for(int ng=0;ng<Ng;ng++){
                        for(int p=0;p<P;p++)
                        {
                            int halfpanel_num=ng*P+p;
                            if(halfpanel_num!=0)
                            {
                                cmat Vsub(2,1);
                                Vsub(0,0) = v(n1*N2+n2);Vsub(1,0) = v(halfpanel_num*N1N2+n1*N2+n2);
                                vSub(3+halfpanel_num) = concat_horizontal(vSub(3+halfpanel_num),Vsub);
                            }
                        }
                }}}
        }
        if(Ng>1 && !CBmode)         //for p
        {
            for(int n=0;n<N1N2P;n++)
            {
                cmat Vsub3(Ng,1);
                for(int ng=0;ng<Ng;ng++)
                {
                    Vsub3(ng) = v(ng*N1N2P + n);
                }
                vSub(5) = concat_horizontal(vSub(5),Vsub3);
            }
        }
    }
    return vSub;
}

// Computes thetas from the V matrix
vec computeThetas(cmat V,int nLoops,int nLayers,ivec N)
{
    //Get The coarse thetas 
    //Get the Subvectors from V matrix
    //Form the beta matrix from the thetas
    //remove the effect of older thetas and find the new thetas.
    vec courseThetas = computeCoarseThetas(V,nLayers,N); 
    if(nLoops<=0){return Adjust(courseThetas);}
    
    vec tunedThetas = courseThetas;
    int CBtype = N(0),CBmode=N(1),Ng=N(2),N1=N(3),N2 = N(4),P=N(5);
    
    Array<cmat> vSub = getSubVectors(V,nLayers,N);
    
    int OddLayers = (nLayers>=3) ? 2 : 1;int EvenLayers=nLayers-OddLayers;
    
    for(int loop=0;loop<nLoops;loop++)
    {
        Array<mat> Betas = getBetas(tunedThetas,nLayers,N);
        if(Betas.length()!=vSub.length())
        {
            cout<<"Number of Beta and Subvector Matrices must be same..."<<endl;abort();
        }
        vec tempThetas = zeros(tunedThetas.length());
        for(int i=0;i<Betas.length();i++)
        {
            int l = (i<=1) ? OddLayers : (i<=3) ? EvenLayers : nLayers;
            if(vSub(i).cols()!=Betas(i).rows())
            {
                cout<<i<<" nSubvectors: "<<vSub(i).cols()<<" nBetas: "<<Betas(i).rows()<<" Aborting..."<<endl;abort();
            }
            if(!l || (N2==1 && (i==1 || i==3))){continue;}
            
            cmat V_est = vSub(i)*expj(-Betas(i)) / Betas(i).rows(); 
            //Removes the effects of thetas other than Thetas(i) 
            //Compute the New Thetas(i) from the residual matrix.
            if(!(CBmode && CBtype)){
                if(i!=5)
                {
                    dComplex tmpSum(0,0);
                    for(int m=0;m<V_est.rows()-1;m++)
                    {
                        tmpSum +=conj(V_est(m))*V_est(m+1); 
                    }
                    tempThetas(i) = arg(tmpSum);
                }
                else
                {
                    vec phases(Ng-1);
                    for(int ng=1;ng<Ng;ng++)
                    {
                        phases(ng-1) = arg(conj(V_est(0))*V_est(ng));
                    }
                    tempThetas.set_subvector(5,phases);
                }
            }
            else
            {
                if(i>3 && V_est.rows()!=2)
                {
                    cout<<"Invalid no. of rows for V_est..."<<endl;abort();
                }
                dComplex tmpSum(0,0);
                for(int m=0;m<V_est.rows()-1;m++)
                {
                    tmpSum +=conj(V_est(m))*V_est(m+1); 
                }
                tempThetas(i) = arg(tmpSum);
            }
            if(tempThetas==tunedThetas){return Adjust(tunedThetas);}
            tunedThetas=tempThetas;
        }   
    }
    if(tunedThetas.length() != 5+Ng-1+(CBmode+CBtype)/2)
    {
        cout<<"Inappropriate Theta Length..."<<endl;abort();
    }
    return Adjust(tunedThetas);
}

// Maps thetas to PMI indices 
CSI_Info_S CQIEstimatorNR::computeCSIinfoOfPrecoderFromThetas(vec Thetas,int nLayers,int CBmode,CBParam_S CBparam,Link_Param_S* linkParam,bool enable)
{
    if(sum(Thetas<0)){cout<<"Thetas should be in 0 to 2*pi"<<endl;abort();}
    double ThetaL,ThetaM;
    CodeBookConfig_S codebookConfig = mCodebook->getCodeBookConfig();
    int CBtype = codebookConfig.codeBookType;if(CBtype==2){CBtype=0;} //Useful when rank Computation is done based on Searchfree for Codebook TypeII
    int N1 = codebookConfig.N1,N2 = codebookConfig.N2,Ng = codebookConfig.Ng;
    
    int i11,i12,i13,k1,k2,i14 = CBtype-1;
    int T = (!CBtype) ? CBmode+1 : 1;
    
    // For single panel Codebook mode 2 we will be finding 2*i11+a and 2*i12+b
    //So accordingly the i11Size and i12Size is scaled.
    int i11Size = T*CBparam.i11Size,i12Size = T*CBparam.i12Size;
    bool special=false;
    
    if(!CBtype && nCSIRSPorts>=16 && (nLayers==3 || nLayers==4))
    {
        //i11 and i12 will be same for all the layers 
        //So ThetaL and ThetaM is found using all layers.
        //The Precoder matrix in this case will be similar to the Multipanel Codebook mode 1 precoder with Ng=2
        //with phi_n representing i13 and phi_p representing i2.
        ThetaL = Adjust(arg(expj(Thetas(0))+expj(Thetas(2))));
        ThetaM = Adjust(arg(expj(Thetas(1))+expj(Thetas(3))));
        i11 = mod((int)std::round(i11Size*ThetaL/(2*pi)),i11Size);
        i12 = (N2==1) ? 0 : mod((int)std::round(i12Size*ThetaM/(2*pi)),i12Size);
        i13 = mod((int)std::round(Thetas(4)*4/pi),CBparam.i13Size);
        special=true;
    }
    else if(nLayers<=8)
    {
            ThetaL = Thetas(0);ThetaM = Thetas(1); i13=0;
            
            i11 = mod((int)std::round(i11Size*ThetaL/(2*pi)),i11Size);
            i12 = (N2==1) ? 0 : mod((int)std::round(i12Size*ThetaM/(2*pi)),i12Size);
            
            if(nLayers<=4 && nLayers>1)
            {
                int i11_ = mod((int)std::round(i11Size*Thetas(2)/(2*pi)),i11Size);
                int i12_ = (N2==1) ? 0 : mod((int)std::round(i12Size*Thetas(3)/(2*pi)),i12Size);
                
                k1 = i11_ - i11;
                k2 = i12_ - i12;
                
                if(k1*k2<0) //If only one of them is negative, take nearest beam
                {
                    if(k1>0 && k2<0)
                    {
                        if(abs(k1)>abs(k2))
                        {
                            k2=0;
                        }
                        else
                        {
                            i12=i12_;k1=0;k2=-k2;
                        }
                    }
                    else
                    {
                        if(abs(k1)>abs(k2))
                        {
                            i11=i11_;k1=-k1;k2=0;
                        }
                        else
                        {
                            k1=0;
                        }
                    }
                }
                else if(k1<0 && k2<0) //If both are negative, interchange the beams
                {   
                    i11 = i11_; i12 = i12_;
                    k1 = -k1; k2 = -k2;
                }
                i13 = computei13(k1,k2,nLayers); //Calculate i13 from k1 and k2 from the tables
            }
    }
    else{cout<<"Rank greater than 8 not Supported..."<<endl;abort();}
    
    double ThetaN = (special) ? Thetas(5) : Thetas(4);
    
    int i2Size = (nLayers==1) ? 4:2;
    int i2 = mod((int)std::round(ThetaN*2/pi),i2Size);
    
    if(CBtype) //finding i14 and i2 For multipanel codebook 
    {
        if(CBmode)
        {
            //if i13 is valid then find the csi for it else find the csi for all the possible i13s and take the best of it.
            ivec i13s = (i13<CBparam.i13Size && i13>=0) ? to_ivec(i13) : getIntegers(0,CBparam.i13Size-1);
            
            vec specEff(i13s.length());
            Array<CSI_Info_S> csiInfoPerI13(i13s.length());
            for(int i13Index = 0;i13Index<i13s.size();i13Index++)
            {
                int i13temp = i13s(i13Index);
                csiInfoPerI13(i13Index) = computeCSIinfoForMultipanelMode2(Thetas,nLayers,type1PMI_S(i11,i12,i13temp,0,i2),linkParam);
                specEff(i13Index) = csiInfoPerI13(i13Index).specEff;
            }
            int maxIndex = max_index(specEff);
            csiInfoPerI13(maxIndex).type1PMI = mCodebook->getPMIIndices(nLayers,csiInfoPerI13(maxIndex).PMI);
            return csiInfoPerI13(maxIndex);
        }
        else
        {
            i14 = decode(CBparam.i14Sizes,getNearestIndices(Thetas(5,5+Ng-2),to_ivec(4)));
        }
    }
    else if(CBmode)  // for Codebook mode 2 of Single panel (only for layers 1 and 2)
    {
        int l = i11,m=i12;
        int L = (N2==1) ? 4 : 2;
        if(N2==1)
        {
            // 
            //if i13 is valid then find the csi for it else find the csi for all the possible i13s and take the best of it.
            
            ivec i13s = (i13<CBparam.i13Size && i13>=0) ? to_ivec(i13) : getIntegers(0,CBparam.i13Size-1);
            
            vec SpecEffperi13(i13s.length());
            Array<CSI_Info_S> csiInfoPerPMI(i13s.length());
            for(int i13_itr=0;i13_itr<i13s.size();i13_itr++)
            {
                int i13temp = i13s(i13_itr);
                
                //Selecting the best beam group by finding the specEff for precoders in that group
                //Each l can be formed by two i11s ie l/2 and (l-2)/2
                vec SE(2);
                ivec a = ("-1 0");
                int n_L = (nLayers==1) ? 4 : 2;
                Array<cmat> P(2);
                for(int i=0;i<P.length();i++)
                {
                    int i2_ = n_L*(l%2) + 2*n_L*(1 - i%2)+i2;
                    int i11_ = circmod((l+2*a(i))/2,CBparam.i11Size);
                    P(i) = mCodebook->getPrecoderNR(nLayers,type1PMI_S(i11_,0,i13temp,-1,i2_));
                }
                CSI_Info_S csiInfo = getCSIinfoOfBestPrecoder(nLayers,P,linkParam);
                int i2_;
                if(!csiInfo.PMI)
                {
                    i2_ = n_L*(l%2) + 2*n_L +i2;
                    i11 = circmod((l-2)/2,CBparam.i11Size);
                    i12=0;
                }
                else
                {
                    i2_ = n_L*(l%2) + i2;
                    i11 = circmod(l/2,CBparam.i11Size);
                    i12=0;
                }
                csiInfo.PMI = mCodebook->getPrecoderIndxAbsNR(nLayers,type1PMI_S(i11,i12,i13temp,-1,i2_));
                csiInfoPerPMI(i13_itr) = csiInfo;
                SpecEffperi13(i13_itr) = csiInfo.specEff;
            }
            int maxIndex = max_index(SpecEffperi13);
            csiInfoPerPMI(maxIndex).type1PMI = mCodebook->getPMIIndices(nLayers,csiInfoPerPMI(maxIndex).PMI);
            return csiInfoPerPMI(maxIndex);
        }
        else
        {
            int l_res = l%2, m_res = m%2;
            int M_F = (nLayers==1) ? 4 : 2;
        
            i11 = (l - l_res)/2;
            i12 = (m - m_res)/2;
            i2 +=  M_F * (l_res + 2*m_res);
        }
    }
    
    ivec i13s = (i13<CBparam.i13Size && i13>=0) ? to_ivec(i13) : getIntegers(0,CBparam.i13Size-1);
    
    Array<cmat> PartialCodebook(i13s.length());
    ivec PMIIndices(0);
    for(int i13Index = 0;i13Index<i13s.length();i13Index++)
    {
        int i13temp = i13s(i13Index);
        int PMI = mCodebook->getPrecoderIndxAbsNR(nLayers,type1PMI_S(i11,i12,i13temp,i14,i2));
        append(PMIIndices,PMI);
        PartialCodebook(i13Index) = mCodebook->getPrecoderNR(nLayers,type1PMI_S(i11,i12,i13temp,i14,i2));
    }
    CSI_Info_S csiInfo = getCSIinfoOfBestPrecoder(nLayers,PartialCodebook,linkParam);
    csiInfo.PMI = PMIIndices(csiInfo.PMI);
    csiInfo.type1PMI = mCodebook->getPMIIndices(nLayers,csiInfo.PMI);
    return csiInfo;
}

//Removing -ve signs according to the codebook
cmat RemoveNegatives(cmat V,int nCSIRSPorts,int nLayers,bool special,int VlmSize)
{
    int Ng = nCSIRSPorts/(2*VlmSize);
    cmat indices;
    if(special)
    {
        indices=ones_c(4,4);
        dComplex a(-1,0);
        indices(1,1)=a;indices(3,1)=a;
        indices(2,2)=a;indices(3,2)=a;
        indices(1,3)=a;indices(2,3)=a;
        indices = kron(indices,ones_c(nCSIRSPorts/4,1));
    }
    for(int l=1;l<=nLayers;l++) //Removing -ve signs according to the codebook
    {
        if(special)
            V.set_col(l-1,elem_mult(indices.get_col(l-1),V.get_col(l-1)));
        else
        {
            cvec elem;
            int Ind;
            if(nLayers<=4)
            {
                Ind = ((l==2 && nLayers==2) || l>2) ? -1 :1;
            }
            else
            {
                Ind = (l>3 && nLayers==7) ? -1*(l%2): -1*(1-(l%2));
            }
            for(int ng=0;ng<Ng;ng++)
            {
                append(elem,ones_c(VlmSize),Ind*ones_c(VlmSize));
            }
            V.set_col(l-1,elem_mult(elem,V.get_col(l-1)));
        }   
    }
    return V;
}

//Computes PMI Indices using Wideband Channel matrix
CSI_Info_S CQIEstimatorNR::computeWBCSIinfoUsingSearchFree(int nLayers,Link_Param_S* linkParam)
{
    if(nLayers>4){cout<<"Searchfree doesnot support Rank>4.."<<endl;abort();}
    CodeBookConfig_S codebookConfig = mCodebook->getCodeBookConfig();
    int Ng = codebookConfig.Ng;
    int N1 = codebookConfig.N1;
    int N2 = codebookConfig.N2;
    int P = codebookConfig.P;
    int CBType = codebookConfig.codeBookType;
    int CBmode = codebookConfig.codeBookMode;
    CBParam_S CBparam = mCodebook->getCodebookParam(nLayers);
    
    bool special=false;
    
    if(CBType==2){CBType=0;} //Same Procedure for Type I and II Single Panel Codebook. It is useful when Rank is Computed using Searchfree for Codebook Type II 
    
    if(!CBType)
    {
        if(nLayers>2)
            CBmode=0;// CBmode 1 and 2 are same beyond 2 layers for Single Panel.
        if(nCSIRSPorts>=16 && (nLayers==3 || nLayers==4))
        {
            //Precoder matrix in this case is similar to a multipanel mode 1 Precoder with Ng=2 and 
            //phi_n represented by i13 and phi_p represented by i2.
            special=true;Ng=2;N1 = N1/2;
        }
    }
    
    ivec N(6);N(0)=CBType,N(1)=CBmode;N(2)=Ng;N(3)=N1;N(4)=N2;N(5)=P;
    int VlmSize = nCSIRSPorts / (P*Ng);
    vec Thetas;
    
    int rbNumber = linkParam->rbNumber,DLRBs = linkParam->nRBs;
    int start = rbNumber*reCountPerRb;
    int stop = (rbNumber+DLRBs)*reCountPerRb-1;

    cmat V;cvec S;
    Array<cmat> widebandChannel = entireRawChannel(start,stop);
    cmat avgChannelCovariance = avgCovariance(widebandChannel);
    eig(avgChannelCovariance,S,V);
    cmat V_ = RemoveNegatives(V.get_cols(0,nLayers-1),nCSIRSPorts,nLayers,special,VlmSize);
    
    Thetas = computeThetas(V_,LoopsforWideband,nLayers,N);
    CSI_Info_S csiInfo = computeCSIinfoOfPrecoderFromThetas(Thetas,nLayers,CBmode,CBparam,linkParam);
    return csiInfo;
}

//Compute i2 for given wideband parameters and subband channel matrix indices
CSI_Info_S CQIEstimatorNR::computei2forSubband(int nLayers,type1PMI_S precIndex,Link_Param_S* linkParam)
{
    CodeBookConfig_S codebookConfig = mCodebook->getCodeBookConfig();
    int CBtype = codebookConfig.codeBookType;
    int CBmode = codebookConfig.codeBookMode;
    int Ng = codebookConfig.Ng;
    int N1 = codebookConfig.N1;
    int N2 = codebookConfig.N2;
    int P = codebookConfig.P;
    
    bool special=false;
    
    if(!CBtype)
    {
        if(nLayers>2)
            CBmode=0; // CBmode 1 and 2 are same beyond 2 layers for Single Panel.
        if(nCSIRSPorts>=16 && (nLayers==3 || nLayers==4))
        {
            special=true;Ng=2;N1 = N1/2;
        }
    }
    
    ivec N(6);N(0)=CBtype,N(1)=CBmode;N(2)=Ng;N(3)=N1;N(4)=N2;N(5)=P;
    
    int i11=precIndex.i11,i12=precIndex.i12,i13=precIndex.i13;
    
    CBParam_S CBparam = mCodebook->getCodebookParam(nLayers);
    vec Theta = getThetasFromPMIIndices(precIndex,special,CBtype,CBmode,CBparam,nLayers); //computes Thetal,l_,m,m_,p,n
    vec ThetaN;
    int rbNumber = linkParam->rbNumber,DLRBs = linkParam->nRBs;
    int start = rbNumber*reCountPerRb;
    int stop = (rbNumber+DLRBs)*reCountPerRb-1;
    Array<cmat> subbandChannel = entireRawChannel(start,stop);
    
    cmat V;cvec S;
    eig(avgCovariance(subbandChannel),S,V);
    ThetaN = computeThetaN(V,nLayers,precIndex,CBparam,Theta,N);
    
    ThetaN = Adjust(ThetaN);
    int i2=0;
    Array<cmat> Precoder(0);
    CSI_Info_S csiInfo;
    if(CBmode && CBtype)
    {
        ivec n = getNearestIndices(ThetaN,CBparam.i2Sizes);
        i2 = decode(CBparam.i2Sizes,n);
    }
    else
    {
        int i2Size = (nLayers==1) ? 4 :2;
        i2 = mod((int)std::round(ThetaN(0)*2/pi),i2Size);
        // this i2 represents the cophasing factor 
        //Actual i2 is computed by finding the Spectral Efficiency of this i2 with all the beams in the set.
        // ie all the precoders with this cophasing factor in a beam set corresponding
        // to i11 and i12 (provided), are compared through their Spectral Efficiency. 
        if(CBmode) 
        {
            int L = (nLayers==1) ? 4:2; //Represents i2Size per beam
            //Find the Spectral Efficiency of all the precoders with cophasing factor i2 in a beam set
            //given by i11 and i12.
            //Maximum amplitude beam is selected and i2 is modified to represent the best beam along with their
            //cophasing factor.
            int n_L1 = (N2==1) ? 4 : 2; //No. of beam groups
            int n_L2 = 4/n_L1;
            Precoder.set_length(4);
            for(int i=0;i<n_L1;i++)
            {
                for(int j=0;j<n_L2;j++)
                {
                    int i2_ = i2 + L * (i+2*j);
                    Precoder(n_L2*i+j) = mCodebook->getPrecoderNR(nLayers,type1PMI_S(i11,i12,i13,-1,i2_));
                }
            }
            csiInfo = getCSIinfoOfBestPrecoder(nLayers,Precoder,linkParam);
            int index = csiInfo.PMI;
            i2 += L * (index/n_L2 + n_L2*(index%n_L2));
            if(i2>=CBparam.i2Size || i2<0){cout<<"Invalid i2"<<endl;abort();}
            csiInfo.PMI = i2;
            return csiInfo;
        }
    }
    
    if(i2>=CBparam.i2Size || i2<0){cout<<"Invalid i2"<<endl;abort();}
    precIndex.i2 = i2;Precoder.set_length(1);
    Precoder(0) = mCodebook->getPrecoderNR(nLayers,precIndex);
    csiInfo = getCSIinfoOfBestPrecoder(nLayers,Precoder,linkParam);
    
    csiInfo.PMI = i2;
    return csiInfo;
}

// Generates Theta Vector from the Given PMI Indices
vec CQIEstimatorNR::getThetasFromPMIIndices(type1PMI_S precIndex,bool special,int CBtype,int CBmode,CBParam_S CBparam,int nLayers)
{
    CodeBookConfig_S codebookConfig = mCodebook->getCodeBookConfig();
    int Ng = codebookConfig.Ng;
    int N1 = codebookConfig.N1;
    int N2 = codebookConfig.N2;
    
    if(special){Ng=2;}
    
    vec Theta=zeros(5+Ng-1+(CBmode+CBtype)/2);
    
    Theta(0) = (double) precIndex.i11*2*pi/CBparam.i11Size;
    Theta(1) = (double) precIndex.i12*2*pi/CBparam.i12Size;
    if(!special)
    {
        if(nLayers<=4)
        {
            ivec k1k2 = getK1K2(nLayers,N1,N2,precIndex.i13);
            Theta(2) = (double) (precIndex.i11+k1k2(0))*2*pi/CBparam.i11Size;
            Theta(3) = (double) (precIndex.i12+k1k2(1))*2*pi/CBparam.i12Size;
        }
        else
        {
            Theta(2) = Theta(0);Theta(3) = Theta(1);
        }
        Theta(4) = (double) precIndex.i2*pi/2;
    }
    else
    {
        Theta(2) = Theta(0);
        Theta(3) = Theta(1);
        Theta(4) = (double) precIndex.i13*pi/4;
        Theta(5) = (double) precIndex.i2*pi/2;
    }
    ivec p;
    if(CBtype) //forming thetas for inter panel phases
    {
        p = encode(CBparam.i14Sizes,precIndex.i14);
        if(!CBmode)
        {
            vec phi = p*pi/2;
            Theta.set_subvector(5,phi);
        }
        else
        {
            ivec n = encode(CBparam.i2Sizes,precIndex.i2);
            Theta(4) = n(0)*pi/2;
            Theta(5) = arg(ap(p(0))*bn(n(1)));
            Theta(6) = arg(ap(p(1))*bn(n(2)));
        }
    }
    if(Theta.length() != 5+Ng-1 + (CBtype+CBmode)/2)
    {
        cout<<"Inappropriate Theta Length.."<<endl;abort();
    }
    return Theta;
}

//Computes phi_n for given i1 Indices and Subband Channel
vec computeThetaN(cmat V,int nLayers,type1PMI_S precIndex,CBParam_S CBparam,vec Theta,ivec N)
{
    int CBtype = N(0),CBmode=N(1),Ng=N(2),N1=N(3),N2 = N(4),P=N(5);
    int VlmSize = N1*N2; 
    
    bool special = (Ng==2 && !CBtype) ? true :false;
    
    cmat V_temp = V;
    
    V = RemoveNegatives(V,Ng*N1*N2*P,nLayers,special,VlmSize);
    
    int index = (special) ? 5:4;double ThetaN=0.0;
    
    Array<cmat> vSub = getSubVectors(V,nLayers,N);
    Array<mat> Betas = getBetas(Theta,nLayers,N);
    cmat Vsub,Vest; mat B;
    vec thetaN;
    if(CBtype*CBmode)
    {
        ivec p = encode(CBparam.i14Sizes,precIndex.i14);
        thetaN.set_length(3);
        
        for(int i=0;i<3;i++)
        {
            Vest = vSub(4+i)*expj(-Betas(4+i)) / Betas(4+i).rows();
            
            dComplex tmpSum = dComplex(0,0);
            for(int m=0;m<Vest.rows()-1;m++)
            {
                tmpSum +=conj(Vest(m))*Vest(m+1); 
            }
            thetaN(i) = (i==0) ? arg(tmpSum) : arg((tmpSum/ap(p(i-1)))*expj(pi/4));
        }
        return thetaN;
    }
    else if(nLayers<=4)
    {
        Vsub = vSub(index);
        B = Betas(index);
    
        Vest = Vsub*expj(-B) / B.rows();
    
        dComplex tmpSum(0,0);
        for(int m=0;m<P-1;m++)
        {
            tmpSum +=conj(Vest(m))*Vest(m+1); 
        }
        ThetaN = arg(tmpSum);
    }
    thetaN.set_length(1);
    thetaN(0) = ThetaN;
    
    return thetaN;
}
