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

#include "../include/Codebook.h"
#include "../include/CSIEstimator.h"

cvec coeffAndResidueUsingOMP(cvec halfSingularVector, const cmat orthBeamSet, double& residueNorm, int Lbeams)
{
    residueNorm = -1;
    int N1timesN2 = halfSingularVector.length(); // N1*N2
    if(N1timesN2!=orthBeamSet.rows())
    {
        abort();
    }
//     double normOfHalfSingularVector = norm(halfSingularVector);
//     halfSingularVector = halfSingularVector*(1/normOfHalfSingularVector); // Normalizing the vector to remove channel losses
    int cntLbeams=0;  // Count for number of beams 
    cvec residueVec = halfSingularVector; // Initializing residue with layerBeam for 1st iteration
    ivec maxColIndices(Lbeams); // Stores the index of best match beam in each iteration
    cvec projections(orthBeamSet.cols()); // projections of halfSingularVector on to columns of the library (orthBeamSet)  
    double maxProj=0.0;
    // After the last iteration of while iteration, tmpCoeff contains normalized coeff. return variable is coeffients
    cvec coeffients = zeros_c(orthBeamSet.cols()), tmpCoeff=zeros_c(orthBeamSet.cols());  
    cmat basisMat;
    //     cout << "tmpCoeff 1: "<<tmpCoeff<<endl;
    while(cntLbeams<Lbeams)
    {
        projections = orthBeamSet.hermitian_transpose()*residueVec; // Projections of halfSingularVector/residue onto library
        maxProj = max(abs(projections)); // maximum of projections
        
        if(maxProj<1e-6)  // Residue is negligible, breaking 
            break;
        // Stores the column index of max projection in each iteration
       // cout<<endl<<"maxProj: "<<maxProj<<", max_index(abs(projections)): "<<max_index(abs(projections))<<endl;
        maxColIndices(cntLbeams) = max_index(abs(projections)); 
        // basis matrix's columns are appended with column of library with max projection in each iteration
        basisMat = orthBeamSet.get_cols(maxColIndices(0,cntLbeams));
        // computing normalized coefficients
        tmpCoeff = inv(basisMat.hermitian_transpose()*basisMat)*basisMat.hermitian_transpose()*halfSingularVector;
        //         cout << "tmpCoeff 2: "<<tmpCoeff<<endl;
        // Storing residue vector
        residueVec = halfSingularVector - basisMat*tmpCoeff; // residue of each iteration
        cntLbeams++;
    }
    // Multiplying the normalized coeff with norm of halfSingularVector
    // Storing the coefficients in the corresponding locations, all other locations are zeroes
    for(int q =0;q<cntLbeams;q++)
        coeffients(maxColIndices(q)) = tmpCoeff(q);//*normOfHalfSingularVector; 
    // storing norm of residue vector
    residueNorm = norm(residueVec);
//     cout<<endl<<"Coeeficients: "<<endl<<coeffients<<endl<<"Norm: "<<norm(coeffients)<<endl;
    return coeffients;
}

ivec Codebook::quantizeCoefficients(const cvec actualCoefficients, vec& quantizedAmp, vec& quantizedPhases, cvec& quantizedCoeff,bool isWideband)
{
    vec P;
    if(isWideband)
    {
        int nWideAmp = dlCodeBookConfig.nWideAmp;
        int nWidePow = (dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_II_) ? 2 : 4;
        // Table 5.2.2.2.3-2 38.214 v 15.1.0
        P.set_length(nWideAmp); // Pw is wideband amp coeffs,
        for(int i=0;i<nWideAmp;i++)
            P(i)= pow(pow(2.0,nWideAmp-1-i),-(1.0/nWidePow));//1/sqrt(pow(2,7-i));
      
    }
    else
    {
        int nSubPow = 2,nSubAmp = dlCodeBookConfig.nSubAmp;
        P.set_length(nSubAmp);
        // Table 5.2.2.2.3-3 38.214 v 15.1.0
        for(int i=0;i<nSubAmp;i++)
            P(i)= pow(pow(2.0,nSubAmp-1-i),-(1.0/nSubPow));//1/sqrt(pow(2,7-i));
    }
    // type 2 codebook phase parameters
    int nPSK = dlCodeBookConfig.nPSK; vec type2Phases(nPSK);
    for(int i=0;i<nPSK;i++)
        type2Phases(i) = (2.0*pi*i)/(double)nPSK;
    
    double nonNegPhase=0.0;  // a temp variable to store non neg phase i.e if theta is -ve, then  2*pi+theta is stored
    vec actualAmps(actualCoefficients.length());
    vec actualPhases(actualCoefficients.length());
    cvec scaledActualCoefficients(actualCoefficients.length());
    ivec amplitudeCoefficients(actualCoefficients.length());
    
    // Scaling the strong beam amplitude to 1 by dividing coeff with the max(abs(coeff)), and scaling the other amplitudes accordingly.
//     scaledActualCoefficients = actualCoefficients/max(abs(actualCoefficients));
    if(!quantizedAmp.length())
    {
        quantizedAmp.set_length(actualCoefficients.length());
        quantizedPhases.set_length(actualCoefficients.length());
        quantizedCoeff.set_length(actualCoefficients.length());
    }
    scaledActualCoefficients = actualCoefficients;
//      cout<<endl<<"scaledActualCoefficients: "<<abs(scaledActualCoefficients)<<endl;
    // Quatization of amplitudes and phases
//     cout<<endl<<"----Subband Loop----"<<endl;
    for(int i=0;i<scaledActualCoefficients.length();i++)
    {
        // Getting the angle/phase of complex coeeficient
        nonNegPhase= (arg(scaledActualCoefficients(i))); 
        actualPhases(i) = nonNegPhase;
        nonNegPhase = (nonNegPhase<0)?(nonNegPhase+2*pi):nonNegPhase;
//         if(i==0){cout<<endl<<"nonNegPhase: "<<nonNegPhase<<endl;}
        if(dlCodeBookConfig.isType2IdealCoeff)
        {
            quantizedAmp(i) = abs(scaledActualCoefficients(i));
            quantizedPhases(i) = actualPhases(i);
        }
        else
        {
            // Quatizing amplitudes, by selecting closest from the Pw vector
           
            amplitudeCoefficients(i) = findClosest(P,abs(scaledActualCoefficients(i)));
            quantizedAmp(i) = P(amplitudeCoefficients(i));
//             if(i==0){cout<<endl<<"SB Amp UnQ: "<<abs(scaledActualCoefficients(i))<<" ,Quantized: "<<quantizedAmp(i)<<endl;}
            //Quatizing phases
            vec d = abs(expj(type2Phases) - expj(nonNegPhase));
            quantizedPhases(i) = type2Phases(findClosest(d,0.0));
//             if(i==0){cout<<endl<<"SB Phase UnQ: "<<nonNegPhase<<" ,Quantized: "<<quantizedPhases(i)<<endl;}
        }
        
        //Quantizing only Non-zero Coeeficients
        if(abs(scaledActualCoefficients(i))<0.001){
           quantizedAmp(i)=0; 
        }
            
        // Generating quatized coeff for calculating residue
        quantizedCoeff(i) = quantizedAmp(i)*expj(quantizedPhases(i));
    }
//       cout<<endl<<"quantizedCoeff: "<<quantizedCoeff<<endl;
    return amplitudeCoefficients;
}

int Codebook::selectSetAndComputeCoeff(const Array<cmat> orthBeamsTotalSets, const cvec halfSingularVector, int Lbeams, vec& quantizedAmplitudes, vec& quatizedPhases, cvec& quantizedCoefficients)
{
    int selectedSet, totalOrthSets = orthBeamsTotalSets.length();
    vec residueNormOverSets(totalOrthSets);   // Actual residue after OMP over the sets
    Array<cvec> coeffientsOverSets(totalOrthSets);  // linear combination coefficients over sets
    Array<vec> quantizedAmplitudesOverSets(totalOrthSets); // Quatized amplitudes 
    Array<vec> quantizedPhasesOverSets(totalOrthSets);  // Quatized phases 
    //     Array<vec> nonNeg(totalOrthSets);
    // quantizedCoefficientsOverSets is used to calculate residue after quatization
    Array<cvec> quantizedCoefficientsOverSets(totalOrthSets); // Quatized coefficients = Quatized amplitudes * Quatized phases
    vec quantizedResidue(totalOrthSets); cvec tmpDiff(halfSingularVector.length());
    for(int setCnt=0;setCnt<totalOrthSets;setCnt++)
    {
        // Find coefficients over the sets
        coeffientsOverSets(setCnt) = coeffAndResidueUsingOMP(halfSingularVector, orthBeamsTotalSets(setCnt), residueNormOverSets(setCnt), Lbeams);
        //initializations
        quantizedAmplitudesOverSets(setCnt).set_length(coeffientsOverSets(setCnt).length());
        quantizedPhasesOverSets(setCnt).set_length(coeffientsOverSets(setCnt).length());
        quantizedCoefficientsOverSets(setCnt).set_length(coeffientsOverSets(setCnt).length());
        // Quatization of amplitudes and phases
        quantizeCoefficients(coeffientsOverSets(setCnt), quantizedAmplitudesOverSets(setCnt), quantizedPhasesOverSets(setCnt), quantizedCoefficientsOverSets(setCnt));
        // NOTE: comment following line for quantized coeeficients
        //         quantizedCoefficientsOverSets(setCnt) = coeffientsOverSets(setCnt);
        tmpDiff = halfSingularVector - orthBeamsTotalSets(setCnt)*quantizedCoefficientsOverSets(setCnt);
        quantizedResidue(setCnt) = norm(tmpDiff);
    }
    
    // Return values and writing into call by address variables
    selectedSet = min_index(quantizedResidue);
    
    //     cout << "Residue before quatization: "<<residueNormOverSets(selectedSet)<<endl;
    //     cout << "Residue after quatization: "<<quantizedResidue(selectedSet)<<endl;
    quantizedAmplitudes = quantizedAmplitudesOverSets(selectedSet);
    quatizedPhases = quantizedPhasesOverSets(selectedSet);
    quantizedCoefficients = quantizedCoefficientsOverSets(selectedSet);
    
    return selectedSet;
}

int Codebook::type1Layer1BeamIndx_OMP(const Array<cmat> orthBeamsTotalSets, cvec halfSingularVector, int& beamIndx)
{
    int selectedSet=-1;
    int numOfSets = orthBeamsTotalSets.length();
    int setCnt=0;
    Array<vec> projections(numOfSets);
    double normOfHalfSingularVector = norm(halfSingularVector);
    halfSingularVector = halfSingularVector*(1/normOfHalfSingularVector); // Normalizing the vector to remove channel losses
    vec maxProjectionsOverSets(numOfSets); 
    while(setCnt<numOfSets)
    {
        projections(setCnt).set_length(orthBeamsTotalSets(0).cols());
        projections(setCnt) = abs(orthBeamsTotalSets(setCnt).hermitian_transpose()*halfSingularVector);
        maxProjectionsOverSets(setCnt) = max(projections(setCnt));
        setCnt++;
    }
    selectedSet = max_index(maxProjectionsOverSets);
    
    beamIndx = max_index(projections(selectedSet));
    
    return selectedSet;
}

void Codebook::computeCoeff(const cmat orthBeamsSet, const cvec halfSingularVector, int Lbeams, vec& quantizedAmplitudes, vec& quantizedPhases, cvec& quantizedCoefficients)
{
    cvec coeffients;  // linear combination coefficients over sets
    double dummyResidue=0;
    // Find coefficients over the sets
    coeffients = coeffAndResidueUsingOMP(halfSingularVector, orthBeamsSet, dummyResidue, Lbeams);
//     cout << endl<< endl<<"dummyResidue: "<<dummyResidue<<endl;
    //initializations
    quantizedAmplitudes.set_length(coeffients.length());
    quantizedPhases.set_length(coeffients.length());
    quantizedCoefficients.set_length(coeffients.length()); // Quatized coefficients = Quatized amplitudes * Quatized phases
    // Quatization of amplitudes and phases
    quantizeCoefficients(coeffients, quantizedAmplitudes, quantizedPhases, quantizedCoefficients);
    // NOTE: comment following line for quantized coeeficients
    quantizedCoefficients = coeffients;
}

Array<cmat> Codebook::getType2Precoder_OMP(Type2coefficients_N_OMP_S& type2Coefficients, int nSubbands, int nLayers)
{
    Array<cmat> precoderPerSubband(nSubbands);
    cmat W1 = kron(eye_c(2),type2Coefficients.orthBeams);
    int L = type2Coefficients.orthBeams.cols();
    for(int layerCnt = 0; layerCnt<nLayers; layerCnt++)
    {
//         cout<<"Layer: "<<layerCnt<<endl;
        cmat W2;
        switch(dlCodeBookConfig.codeBookType)
        {
            case _NR_CODEBOOK_TYPE_II_:
            case _NR_CODEBOOK_PORT_SELECTION_:
                {
                    mat amp = diag(type2Coefficients.wbAmplitudesPerLayer(layerCnt))*type2Coefficients.sbAmplitudesPerLayer(layerCnt);
                    cmat phase = expj(type2Coefficients.phasePerLayer(layerCnt));
                    if(dlCodeBookConfig.isType2IdealCoeff){
                        W2=type2Coefficients.W2PerLayer(layerCnt);
    
                    }
                    else{
                        W2 = elem_mult(to_cmat(amp),phase);
//                     cout<<endl<<"W2 TypeII Amp: "<<endl<<abs(W2)<<endl;
//                     cout<<endl<<"W2 TypeII Phase: "<<endl<<arg(W2)<<endl;
                    }
                }
                break;
            case _NR_CODEBOOK_TYPE_II_ENHANCED_:
            case _NR_CODEBOOK_TYPE_II_ENHANCED_CJT_:
            case _NR_CODEBOOK_PORT_SELECTION_ENHANCED_:
            case _NR_CODEBOOK_PORT_SELECTION_FURTHER_ENHANCED_:
                {
                    mat wbAmp = kron(diag(type2Coefficients.wbAmplitudesPerLayer(layerCnt)),eye(L));
                    mat amp = wbAmp*type2Coefficients.sbAmplitudesPerLayer(layerCnt);
                    cmat phase = expj(type2Coefficients.phasePerLayer(layerCnt));
                    cmat W2TildaQuantized = elem_mult(to_cmat(amp),phase);
                    W2 = W2TildaQuantized*type2Coefficients.WfMatrixPerLayer(layerCnt).H();
//                     cout<<endl<<"W2TildaQuantized: "<<endl<<abs(W2TildaQuantized)<<endl;
//                     cout<<endl<<"W2 Type II Enhanced: "<<endl<<abs(W2)<<endl;

                }
                break;
            default:
                cout<<"Invalid codebook Type"<<endl;
                abort();
        }
        cmat precoderPerSubbandPerLayer = W1*W2;
        
        for(int sb_cnt=0;sb_cnt<nSubbands;sb_cnt++)
        {
            cmat precoder = precoderPerSubbandPerLayer.get_col(sb_cnt);
            precoder = precoder/norm(precoder.get_col(0));
            appendHorizontal(precoderPerSubband(sb_cnt),precoder);
        }
        
    }
    
    for(int sb_cnt=0;sb_cnt<nSubbands;sb_cnt++){
        precoderPerSubband(sb_cnt) = precoderPerSubband(sb_cnt)/sqrt(nLayers);
    }
    
    return precoderPerSubband;
}

Array<cmat> Codebook::getNRPrecoderPerSubband(int nSubbands, TypeIIPMIIndices& pmiIndices)
{
    int nLayers = pmiIndices.nLayers;
    int O1=dlCodeBookConfig.O1, O2 = dlCodeBookConfig.O2,N1= dlCodeBookConfig.N1,N2= dlCodeBookConfig.N2;
    int q1 = pmiIndices.i11/O2,q2 = pmiIndices.i11%O2;
    int L = dlCodeBookConfig.LBeams;
    ivec n1,n2;
    computeBeamIndices(n1,n2,pmiIndices.i12,N1,N2,L);
    cmat orthBeams(dlCodeBookConfig.nCSIRSPorts/2,L);
    switch(dlCodeBookConfig.codeBookType)
    {
        case _NR_CODEBOOK_TYPE_II_:
        case _NR_CODEBOOK_TYPE_II_ENHANCED_:
        case _NR_CODEBOOK_TYPE_II_ENHANCED_CJT_:
            for(int i=0;i<L;i++)
            {
                cmat vlm = getVlm(n1(i)*O1+q1,n2(i)*O2+q2,N1,N2,O1,O2);
                orthBeams.set_col(i,vlm.get_col(0));
            }
            break;
        case _NR_CODEBOOK_PORT_SELECTION_:
        case _NR_CODEBOOK_PORT_SELECTION_ENHANCED_:
            orthBeams = eye_c(dlCodeBookConfig.nCSIRSPorts/2).get_cols(pmiIndices.i11*dlCodeBookConfig.d+getIntegers(0,L-1));
            break;
        case _NR_CODEBOOK_PORT_SELECTION_FURTHER_ENHANCED_:
            {
                ivec m,n;
                computeBeamIndices(m,n,pmiIndices.i12,dlCodeBookConfig.nCSIRSPorts/2,1,L);
                orthBeams = eye_c(dlCodeBookConfig.nCSIRSPorts/2).get_cols(m);
            }
            break;
        default:
            cout<<"Unsupported Codebook Type"<<endl;
            abort();
    }
    orthBeams /= sqrt(N1*N2);
    
    int nWideAmp = dlCodeBookConfig.nWideAmp;
    int nWidePow = (dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_II_) ? 2 : 4;
    // Table 5.2.2.2.3-2 38.214 v 15.1.0
    vec widebandAmp(nWideAmp); // Pw is wideband amp coeffs,
    for(int i=0;i<nWideAmp;i++)
        widebandAmp(i)= pow(pow(2.0,nWideAmp-1-i),-(1.0/nWidePow));//1/sqrt(pow(2,7-i));
        
    int nSubPow = 2,nSubAmp = dlCodeBookConfig.nSubAmp;
    vec subbandAmp(nSubAmp);
    // Table 5.2.2.2.3-3 38.214 v 15.1.0
    for(int i=0;i<nSubAmp;i++)
        subbandAmp(i)= pow(pow(2.0,nSubAmp-1-i),-(1.0/nSubPow));
    
    cmat W1 = kron(eye_c(2),orthBeams);
    Array<cmat> precoderPerSubband(nSubbands);
    
    switch(dlCodeBookConfig.codeBookType)
    {
        case _NR_CODEBOOK_TYPE_II_:
        case _NR_CODEBOOK_PORT_SELECTION_:
            {
                for(int layer_cnt=0;layer_cnt<nLayers;layer_cnt++)
                {
                    mat phase = to_mat(pmiIndices.i21(layer_cnt))*2*pi/dlCodeBookConfig.nPSK;
                    mat subAmp = zeros(pmiIndices.i22(layer_cnt).rows(),nSubbands);
                    for(int m_cnt=0;m_cnt<nSubbands;m_cnt++)
                        subAmp.set_col(m_cnt,subbandAmp(pmiIndices.i22(layer_cnt).get_col(m_cnt)));
                    vec wideAmp = widebandAmp(pmiIndices.i14(layer_cnt));
                    mat amp = diag(wideAmp)*subAmp;
                    cmat W2 = elem_mult(to_cmat(amp),expj(phase));
                    cmat W = W1*W2; 
                    for(int sb_cnt=0;sb_cnt<nSubbands;sb_cnt++)
                    {
                        cmat P = W.get_col(sb_cnt);
                        appendHorizontal(precoderPerSubband(sb_cnt),P/norm(P,"fro"));
                        if(layer_cnt==nLayers-1)
                            precoderPerSubband(sb_cnt) *= (1.0/sqrt(nLayers));
                    }
                }
                return precoderPerSubband;
            }
            break;
        case _NR_CODEBOOK_TYPE_II_ENHANCED_:
        case _NR_CODEBOOK_TYPE_II_ENHANCED_CJT_:
        case _NR_CODEBOOK_PORT_SELECTION_ENHANCED_:
            {
                int M = pmiIndices.i25(0).cols();
                cmat Wf = generateDFTMatrix(nSubbands);
                for(int layer_cnt=0;layer_cnt<nLayers;layer_cnt++)
                {
                    mat phase = to_mat(pmiIndices.i25(layer_cnt))*2.0*pi/dlCodeBookConfig.nPSK;
                    mat subAmp = zeros(pmiIndices.i24(layer_cnt).rows(),M);
                    for(int m_cnt=0;m_cnt<M;m_cnt++)
                        subAmp.set_col(m_cnt,subbandAmp(pmiIndices.i24(layer_cnt).get_col(m_cnt)));
                    vec wbCoeff = widebandAmp(pmiIndices.i23(layer_cnt));
                    mat wideAmp = kron(diag(wbCoeff),eye(L));
                    
                    mat amp = wideAmp*subAmp;
                    
                    cmat coeff = elem_mult(to_cmat(amp),expj(phase));
                    ivec n3 = getn3Indices(pmiIndices.i15,pmiIndices.i16(layer_cnt),nSubbands,M);
                    
                    cmat W2 = coeff*Wf.get_cols(n3).H();
                    cmat W = W1*W2;    
                    for(int sb_cnt=0;sb_cnt<nSubbands;sb_cnt++)
                    {
                        cmat P = W.get_col(sb_cnt);
                        appendHorizontal(precoderPerSubband(sb_cnt),P/norm(P,"fro"));
                        if(layer_cnt==nLayers-1)
                            precoderPerSubband(sb_cnt) *= (1.0/sqrt(nLayers));
                    }
                }
            }
            break;
        case _NR_CODEBOOK_PORT_SELECTION_FURTHER_ENHANCED_:
            {
                int M = (pmiIndices.i15==-1) ? 1: 2;
                ivec n3 = to_ivec(0);
                if(M==2)
                    append(n3,pmiIndices.i15+1);
                cmat Wf = generateDFTMatrix(nSubbands).get_rows(n3);
                for(int layer_cnt=0;layer_cnt<nLayers;layer_cnt++)
                {
                    mat phase = to_mat(pmiIndices.i25(layer_cnt))*2*pi/dlCodeBookConfig.nPSK;
                    mat subAmp = zeros(pmiIndices.i24(layer_cnt).rows(),M);
                    for(int m_cnt=0;m_cnt<M;m_cnt++)
                        subAmp.set_col(m_cnt,subbandAmp(pmiIndices.i24(layer_cnt).get_col(m_cnt)));
                    vec wbCoeff = widebandAmp(pmiIndices.i23(layer_cnt));
                    mat wideAmp = concat_vertical(wbCoeff(0)*ones(L,M),wbCoeff(1)*ones(L,M));
                    
                    mat amp = elem_mult(wideAmp,subAmp);
                    cmat coeff = elem_mult(to_cmat(amp),expj(phase));
                    
                    cmat W2 = coeff*Wf; 
                    for(int sb_cnt=0;sb_cnt<nSubbands;sb_cnt++)
                    {
                        cvec W2S = W2.get_col(sb_cnt);
                        
                        cmat P = W1*W2S;
                        precoderPerSubband(sb_cnt) = concat_horizontal(precoderPerSubband(sb_cnt),P/norm(P,"fro"));
                        
                        if(layer_cnt==nLayers-1)
                            precoderPerSubband(sb_cnt) *= (1.0/sqrt(nLayers));
                    }
                }
            }
            break;
        default:
            cout<<"Unsupported Codebook Type"<<endl;abort();
    }
    
    return precoderPerSubband;
}

void Codebook::computeBeamSetForTypeII(Type2coefficients_N_OMP_S& type2Coefficients,Array<cmat>& entireRawChannel ,int& Lbeams,int nLayers)
{
    int nTx = entireRawChannel(0).cols();
    int selectedSet = -1;
    CodeBookConfig_S codebookConfig = dlCodeBookConfig;
    int N1=codebookConfig.N1, N2=codebookConfig.N2;
    int O1=4,O2=(N2==1) ? 1:4;
    Array<ivec> i11_IndicesPerSet(O1*O2);  // Stores corresponding i11 index of the beams in set
    Array<ivec> i12_IndicesPerSet(O1*O2);  // Stores corresponding i12 index of the beams in set
    Array<cmat> orthBeamsTotalSets = type2OrthogonalBeamsSet(N1, N2, i11_IndicesPerSet, i12_IndicesPerSet); // Generating orthogonal beam sets
    
    ivec indices;
    
//     selectedSet = selectSet(orthBeamsTotalSets,halfSingularVector);
//     double dummyResidue;
//     cvec coeffients = coeffAndResidueUsingOMP(halfSingularVector, orthBeamsTotalSets(selectedSet), dummyResidue, Lbeams);
//     indices = reverse(sort_index(abs(coeffients)))(0,Lbeams-1);

    if(nLayers==0)
        selectedSet = selectSet(orthBeamsTotalSets,entireRawChannel,Lbeams,indices);
    else
    {
        // Average channel covariance
        // Computing average of wideband channel (one polarizartion's channel)
        cmat avgChannelCovar = avgCovariance(entireRawChannel);
        
        cmat U,V;  vec S; cvec eigenValues;
        bool isSuccess = eig(avgChannelCovar, eigenValues, V);
        V = V.get_cols(reverse(sort_index(abs(eigenValues))));
        cmat normMatrix = V.H()*V;
        cvec normVec = diag(normMatrix);
        
        V = inv(diag(normVec))*V; //Normalising individual singular vector with its norm. 
        
        selectedSet = selectSetWithChannelCovariance(orthBeamsTotalSets,avgChannelCovar,Lbeams,indices);
    
//          cout<<"selectedSet Type II : "<<selectedSet<<endl;
//          cout<<"indices Type II: "<<indices<<endl;
        
    }
    type2Coefficients.orthBeams = orthBeamsTotalSets(selectedSet).get_cols(indices);
    type2Coefficients.i11Indices = i11_IndicesPerSet(selectedSet)(indices);
    type2Coefficients.i12Indices = i12_IndicesPerSet(selectedSet)(indices);
    
    int q1 = type2Coefficients.i11Indices(0)%O1,q2 = type2Coefficients.i12Indices(0)%O2;
    ivec n1 = (type2Coefficients.i11Indices-q1)/O1,n2 = (type2Coefficients.i12Indices-q2)/O2;
    ivec n = N1*n2+n1;
    indices = sort_index(n);
    n = n(indices);
    int i12 = 0;
    for(int i=0;i<Lbeams;i++){
        i12 += NCr(N1*N2-1-n(i),Lbeams-i);
    }
    type2Coefficients.pmiIndices.i11 = q1*O2+q2;
    type2Coefficients.pmiIndices.i12 = i12;
    
    type2Coefficients.orthBeams = type2Coefficients.orthBeams.get_cols(indices);
    type2Coefficients.i11Indices = type2Coefficients.i11Indices(indices);
    type2Coefficients.i12Indices = type2Coefficients.i12Indices(indices);
    
    
//     cout<<"type2Coefficients.orthBeams : "<<type2Coefficients.orthBeams<<endl;
}

void Codebook::computeBeamSetForTypeIIPortSelection(Type2coefficients_N_OMP_S& type2Coefficients,Array<cmat>& entireRawChannel ,int& Lbeams,int nLayers)
{
    int nTx = entireRawChannel(0).cols();
    
    Array<ivec> portIndicesPerSet;
    Array<cmat> orthBeamsTotalSets = getPortSelectionSets(dlCodeBookConfig.N1,dlCodeBookConfig.N2,dlCodeBookConfig.d,Lbeams,portIndicesPerSet); // Generating orthogonal beam sets
    
    ivec indices;
    int selectedSet;
    
    if(nLayers==1)
        selectedSet = selectSet(orthBeamsTotalSets,entireRawChannel,Lbeams,indices);
    else
    {
        // Average channel covariance
        // Computing average of wideband channel (one polarizartion's channel)
        cmat avgChannelCovar = avgCovariance(entireRawChannel);
        
        cmat U,V;  vec S; cvec eigenValues;
        bool isSuccess = eig(avgChannelCovar, eigenValues, V);
        cmat normMatrix = V.H()*V;
        cvec normVec = diag(normMatrix);
        
        V = inv(diag(normVec))*V; //Normalising individual singular vector with its norm. 
        
        selectedSet = selectSet(orthBeamsTotalSets,V(0,nTx/2-1,0,nLayers-1),Lbeams,indices);
    }
    
    if(dlCodeBookConfig.d!=-1) indices = getIntegers(0,Lbeams-1);
    type2Coefficients.orthBeams = orthBeamsTotalSets(selectedSet).get_cols(indices);
    
    if(dlCodeBookConfig.d!=-1)
        type2Coefficients.pmiIndices.i11=selectedSet;
    else
    {
        ivec m = portIndicesPerSet(selectedSet);
        int i12=0;
        for(int i=0;i<Lbeams;i++)
            i12 += NCr(nTx/2-1-m(i),Lbeams-i);
        type2Coefficients.pmiIndices.i12=i12;
    }
}

void Codebook::computeSubbandCoefficientsForTypeII(Type2coefficients_N_OMP_S& type2Coefficients,Array<cmat>& entireRawChannel,int& Lbeams,int nSubbands,int subbandSize,int reCountPerRb, const int nLayers)
{
    cmat orthogonalBeams = type2Coefficients.orthBeams;
    cmat W1 = kron(eye_c(2),orthogonalBeams);
    
    type2Coefficients.precoderPerSubband.set_length(nSubbands);
    type2Coefficients.sbAmplitudesPerLayer.set_length(nLayers);
    type2Coefficients.phasePerLayer.set_length(nLayers);
    type2Coefficients.wbAmplitudesPerLayer.set_length(nLayers);
    type2Coefficients.pmiIndices.init(nLayers,dlCodeBookConfig.codeBookType);
    
    for(int layer_cnt=0;layer_cnt<nLayers;layer_cnt++)
    {
//         cout<<endl<<"------Layer: "<<layer_cnt<<"------"<<endl;

        mat ampW2 = abs(type2Coefficients.W2PerLayer(layer_cnt));
//         cout<<endl<<">----------Before Normalization------------<"<<endl;
//         cout<<endl<<"W2 Type II: "<<endl<<type2Coefficients.W2PerLayer(layer_cnt)<<endl;
//          cout<<endl<<"Magnitude Unquantized coefficients: "<<endl<<(ampW2)<<endl;
//          cout<<endl<<"Phase Unquantized coefficients: "<<endl<<arg(type2Coefficients.W2PerLayer(layer_cnt))<<endl;
        complex<double> tempMaxValue;double tempMaxAmp = 0;int rMax,cMax;
        for(int r = 0;r < ampW2.rows();r++){
            for(int c = 0;c < ampW2.cols();c++){
                if(ampW2(r,c) > tempMaxAmp){
                    rMax = r;cMax = c;
                    tempMaxAmp = ampW2(rMax,cMax);
                }
            }
        }
        tempMaxValue = type2Coefficients.W2PerLayer(layer_cnt)(rMax,cMax);
        
        
        vec maxAmpPerBeam = max(ampW2.T());
        int strongestBeam = max_index(maxAmpPerBeam); 
        double maxValue = maxAmpPerBeam(strongestBeam);
//         type2Coefficients.W2PerLayer(layer_cnt) /= maxValue;
        type2Coefficients.W2PerLayer(layer_cnt) /= tempMaxValue;
       /* 
         cout<<endl<<">----------After Normalization------------<"<<endl;
        cout<<endl<<"W2 Type II: "<<endl<<type2Coefficients.W2PerLayer(layer_cnt)<<endl;
         cout<<endl<<"Magnitude Unquantized coefficients: "<<endl<<abs(type2Coefficients.W2PerLayer(layer_cnt))<<endl;
         cout<<endl<<"Phase Unquantized coefficients: "<<endl<<arg(type2Coefficients.W2PerLayer(layer_cnt))<<endl;*/
        type2Coefficients.pmiIndices.i13(layer_cnt) = strongestBeam;
        ampW2 /= maxValue;
        for(int beam_cnt=0;beam_cnt<2*Lbeams;beam_cnt++)
        {
            int nWideAmp = dlCodeBookConfig.nWideAmp;
            int nWidePow = (dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_II_) ? 2 : 4;
            // Table 5.2.2.2.3-2 38.214 v 15.1.0
            cvec value = type2Coefficients.W2PerLayer(layer_cnt).get_row(beam_cnt);
//             cout<<endl<<"Quantizing --> "<<value<<endl;
            vec error(nWideAmp);
            vec error_1(nWideAmp);
            vec P(nWideAmp); // Pw is wideband amp coeffs,
            Array<vec> sbAmpPerWbCoeff(nWideAmp),sbPhasePerWbCoeff(nWideAmp);
            Array<imat> sbAmpCoeffPerWbCoeff(nWideAmp);
            for(int i=0;i<nWideAmp;i++)
            {
                cvec tempValue;
                if(i==0){
                    P(i)=0;
                    tempValue = P(i)*value;
                }
                else{
                    P(i)= pow(pow(2.0,nWideAmp-1-i),-(1.0/nWidePow));//1/sqrt(pow(2,7-i));
                    tempValue = value/dComplex(P(i),0);
                }
//                 cout<<endl<<"WB Amp: "<<P(i)<<endl;
//                 cout<<endl<<"i: "<<i<<endl;
// 
//                 cout<<endl<<"tempValue B: "<<endl<<tempValue<<endl;
                sbAmpCoeffPerWbCoeff(i) = quantizeCoefficients(tempValue,sbAmpPerWbCoeff(i),sbPhasePerWbCoeff(i),tempValue);
                tempValue *= P(i);
//                 cout<<endl<<"tempValue A: "<<endl<<tempValue<<endl;
//                 cout<<endl<<"Value: "<<abs(value)<<endl<<"arg(Value): "<<arg(value)<<endl;
//                 cout<<endl<<"tempValue: "<<abs(tempValue)<<endl<<"arg(tempValue): "<<arg(tempValue)<<endl;
                error(i) = sum_sqr(abs((value)-(tempValue)));
//                 error(i)=sum_sqr(abs(value)-abs(tempValue));
//                 cout<<endl<<"value: "<<endl<<(value)<<endl<<"tempValue: "<<endl<<(tempValue)<<endl<<"error: "<<error(i)<<endl;
//                 cout<<endl<<"sbAmpPerWbCoeff(i): "<<sbAmpPerWbCoeff(i)<<endl<<"sbPhasePerWbCoeff(i): "<<sbPhasePerWbCoeff(i)<<endl;
            }
//               cout<<endl<<"Error: "<<error<<endl;
//               cout<<endl<<"Error: "<<error_1<<endl;
            int minIndex = min_index(error);
//             cout<<endl<<"Selected WB Amplitude: "<<P(minIndex)<<endl;
            append(type2Coefficients.wbAmplitudesPerLayer(layer_cnt),P(minIndex));
            append(type2Coefficients.pmiIndices.i14(layer_cnt),minIndex);
            mat amp = sbAmpPerWbCoeff(minIndex);
//              cout<<endl<<"Selected SB Amplitude: "<<amp(0)<<endl;
//              cout<<endl<<"P(minIndex)*amp: "<<P(minIndex)*amp(0)<<endl;
            appendVertical(type2Coefficients.sbAmplitudesPerLayer(layer_cnt),amp.T());
            appendVertical(type2Coefficients.pmiIndices.i22(layer_cnt),sbAmpCoeffPerWbCoeff(minIndex).T());
            
            mat phase = sbPhasePerWbCoeff(minIndex);
//             cout<<endl<<"Selected SB Phase: "<<phase(0)<<endl;
            appendVertical(type2Coefficients.phasePerLayer(layer_cnt),phase.T());
            phase = phase*dlCodeBookConfig.nPSK/(2*pi);
            imat phaseCoeff = round_i(phase);
            appendVertical(type2Coefficients.pmiIndices.i21(layer_cnt),phaseCoeff.T());
        }
//         cout<<endl<<"WB Amplitudes: "<<endl<<type2Coefficients.wbAmplitudesPerLayer(layer_cnt)<<endl;
//         cout<<endl<<"SB Amplitudes: "<<endl<<type2Coefficients.sbAmplitudesPerLayer(layer_cnt)<<endl;
         mat amp1 = diag(type2Coefficients.wbAmplitudesPerLayer(layer_cnt))*type2Coefficients.sbAmplitudesPerLayer(layer_cnt);
         
                    cmat phase = expj(type2Coefficients.phasePerLayer(layer_cnt));
//                      cout<<endl<<"UnQuantized: "<<endl<<(type2Coefficients.W2PerLayer(layer_cnt))<<endl;
//                     cout<<endl<<"Quantized: "<<endl<<(elem_mult(to_cmat(amp1),phase))<<endl;
//          mat wbAmp = kron(diag(type2Coefficients.wbAmplitudesPerLayer(layerCnt)),eye(L));
//                     mat amp = wbAmp*type2Coefficients.sbAmplitudesPerLayer(layerCnt);
//                     cmat phase = expj(type2Coefficients.phasePerLayer(layerCnt));
//                     cmat W2TildaQuantized = elem_mult(to_cmat(amp),phase);
//                     W2 = W2TildaQuantized*type2Coefficients.WfMatrixPerLayer(layerCnt).H();
    }
}

void Codebook::computeSubbandCoefficientsForIdealPrecoder(Type2coefficients_N_OMP_S& type2Coefficients,Array<cmat>& entireRawChannel,int& Lbeams,int nSubbands,int subbandSize,int reCountPerRb, const int nLayers)
{
    int nTones = entireRawChannel.length();
    cmat U,V;  vec S; cvec eigenValues;
    cmat orthogonalBeams = type2Coefficients.orthBeams;
    cmat W1 = kron(eye_c(2),orthogonalBeams);
    Array<vec> widebandAmp(nLayers);
    cvec maxCoeffPerLayer(nLayers);
    type2Coefficients.W2PerLayer.set_length(nLayers);
    type2Coefficients.idealPrecoderperSubband.set_length(nSubbands);
   // cout<<endl<<"nSubbands: "<<nSubbands<<endl;
    for(int subbandCnt=0;subbandCnt<nSubbands;subbandCnt++)
    {
//         cout<<endl<<"subbandCnt: "<<subbandCnt<<endl;
        int start = subbandCnt*subbandSize*reCountPerRb;
        int stop = (subbandCnt+1)*subbandSize*reCountPerRb-1;
        
        if(subbandCnt==nSubbands-1)
            stop = nTones-1;
        double dummyResidue=0.0;
        cvec tmpCoeff(Lbeams);
        //cout<<endl<<"start: "<<start<<" Stop: "<<stop<<endl;
        Array<cmat> subbandChannel = entireRawChannel(start,stop);
//         cout<<endl<<"subband Channel: "<<endl<<abs(subbandChannel(0)/norm(subbandChannel(0),"fro"))<<endl;
//           cout<<endl<<"subband Channel Phase: "<<endl<<arg(subbandChannel(0))<<endl;
        cmat avgSubbandChannelCovar = avgCovariance(subbandChannel);
        bool isSuccess = eig(avgSubbandChannelCovar, eigenValues, V);
//    /*     cout<<endl<<"chammel Cov: "<<endl<<avgSubbandChannelCovar.get_row(0)(0,3)<<endl;
//         cout<<endl<<abs(subbandChannel(0)*V.get_cols(0,0))<<endl;
//         cout<<endl<<"channel: "<<norm(subbandChannel(0),"fro")<<endl;
//         cout<<endl<<"eigen values: "<<eigenValues(0)<<endl;
//         cout<<endl<<"ratio: "<<(subbandChannel(0)*V.get_cols(0,0))(0)/(subbandChannel(0)*subbandChannel(0).H())(0)<<endl;
//         cout<<endl<<"lam: "<<eigenValues(0)*V.get_cols(0,0)<<endl;*/
 
//         cmat eigen_D=V*diag(eigenValues)*V.H();
       // cout<<endl<<"eigen_D: "<<endl<<eigen_D.get_row(0)(0,3)<<endl;
//         abort();
        V = V.get_cols(reverse(sort_index(abs(eigenValues))));
//         cout<<endl<<abs(subbandChannel(0)*V.get_cols(0,0))<<endl;
//         cout<<endl<<"channel: "<<norm(subbandChannel(0),"fro")<<endl;abort();
//         cout<<endl<<"subbandCnt: "<<subbandCnt<<endl<<"Eigen: "<<V<<endl;
        cmat temp_gain;
        temp_gain=subbandChannel(0)*V;
        
//         cout<<endl<<"sunsize: "<<subbandChannel.size()<<endl;
       type2Coefficients.idealPrecoderperSubband(subbandCnt) = (1/sqrt(nLayers))*V.get_cols(0,nLayers-1);
//        if(subbandCnt==0){
//             cout<<endl<<"Ideal VP: "<<abs(V.get_cols(0,nLayers-1).H()*type2Coefficients.idealPrecoderperSubband(0))<<endl;
//             cout<<endl<<"Ideal V-P: "<<norm(V.get_cols(0,nLayers-1)-type2Coefficients.idealPrecoderperSubband(0),"fro")<<endl;
//        }
//        cout<<endl<<subbandChannel(subbandCnt).rows()<<" "<<subbandChannel(subbandCnt).cols()<<endl;
//          type2Coefficients.idealPrecoderperSubband(subbandCnt)=subbandChannel(0).H()/norm(subbandChannel(0),"fro");
//          cout<<endl<<"Ideal precoder: "<<endl<<abs((type2Coefficients.idealPrecoderperSubband(0)).H())<<endl;
//          cout<<endl<<"Ideal precoder pahse: "<<endl<<arg((type2Coefficients.idealPrecoderperSubband(0)).H())<<endl;
//        cmat elem=elem_div(subbandChannel(0)/norm(subbandChannel(0),"fro"),type2Coefficients.idealPrecoderperSubband(0));
//          cout<<endl<<"el: "<<endl<<abs(elem)<<endl;
//         abort();
//         type2Coefficients.idealPrecoderperSubband(subbandCnt) /=norm(type2Coefficients.idealPrecoderperSubband(subbandCnt),"fro");
        cvec fullSingularVector,halfSingularVector;
        ivec sortIndx;
        for(int layerCnt=0;layerCnt<nLayers;layerCnt++)
        {
            fullSingularVector = V.get_col(layerCnt);
            //cout<<endl<<"layercnt: "<<layerCnt<<endl<<"fullSingularMatrix: "<<fullSingularVector<<endl;;
            fullSingularVector = (1/norm(fullSingularVector))*fullSingularVector;
            cmat tmpCoeff = coeffAndResidueUsingOMP(fullSingularVector,W1,dummyResidue,2*Lbeams);
//             if(subbandCnt==0)
//             cout<<endl<<"tmpCoeff: "<<abs(tmpCoeff)<<endl;
            appendHorizontal(type2Coefficients.W2PerLayer(layerCnt),tmpCoeff);
        }
    }
//   abort();  
    
}

ivec findPossibleFFTIndicesForAllLayers(Array<cmat>& W2PerLayer,int M,int N3,int nLayers,int& Minit)
{
    cmat Wf = generateDFTMatrix(N3);
    vec residue(2*M);
    Array<ivec> indicesPerSet;
    for(int m_init=-2*M+1,i=0;m_init<=0;m_init++,i++)
    {
        int start = circmod(m_init,N3),end = circmod(m_init+2*M-1,N3);
        ivec indices = getIntegers(start,end);
        if(start>end)
        {
            indices = concat(getIntegers(start,N3-1),getIntegers(0,end));
        }
        double res;
        for(int layer_cnt=0;layer_cnt<nLayers;layer_cnt++)
        {   
            cmat W2 = W2PerLayer(layer_cnt);
            vec normW2(W2.rows());
            for(int c=0;c<W2.rows();c++)
            {
                normW2(c) = norm(W2.get_row(c));
            }
            int maxCol = max_index(normW2);
            cvec coEff = coeffAndResidueUsingOMP(W2.get_row(maxCol),Wf.get_cols(indices),res,M-1);
            append(indicesPerSet,indices);
            residue(i) += res;
        }
    }
    int setIndex = min_index(residue);
    ivec possibleFFTIndices = indicesPerSet(setIndex);
    Minit = -2*M+1+setIndex;
    return possibleFFTIndices;
}

int rearrangeW2(cmat& W2tilda,ivec &fftIndices,int M,int N3)
{
    mat absW2 = abs(W2tilda);
    int fl_,il_;double maxVal=0.0;
    for(int f=0;f<M;f++)
    {
        int maxIndx = max_index(absW2.get_col(f));
        if(absW2(maxIndx,f)>maxVal)
        {
            maxVal = absW2(maxIndx,f);
            fl_ = f;
            il_ = maxIndx;
        }
    }
    ivec f = getIntegers(0,M-1);
    ivec n3l = fftIndices;
    for(int i=0;i<M;i++)
    {
        n3l(i) = circmod(n3l(i) - fftIndices(fl_),N3);
        f(i) = circmod(f(i)-fl_,M);
    }
    cmat W2New = W2tilda;

    for(int i=0;i<M;i++)
    {
        W2New.set_col(f(i),W2tilda.get_col(i));
        fftIndices(f(i)) = n3l(i);
    }
//     cout<<endl<<"W2tilda_old: "<<W2tilda<<endl;
    W2tilda = W2New.get_cols(sort_index(fftIndices));
//     cout<<endl<<"W2tilda_new: "<<W2tilda<<endl;
    sort(fftIndices);
    return il_;
}

void Codebook::compressCoefficientsAcrossSubbands(Type2coefficients_N_OMP_S& type2Coefficients,int& Lbeams,int nSubbands, const int nLayers)
{
    int M,N3 = nSubbands, R=1;double pv = 0.25;
    if(nLayers<=2) pv = 0.25;
    else if(Lbeams<=4) pv = 0.125;
    
    M = ceil_i(pv*N3/R);
//     M=N3;
    int L = Lbeams;
    type2Coefficients.precoderPerSubband.set_length(nSubbands);
    type2Coefficients.pmiIndices.init(nLayers,dlCodeBookConfig.codeBookType);
    type2Coefficients.WfMatrixPerLayer.set_length(nLayers);
    type2Coefficients.sbAmplitudesPerLayer.set_length(nLayers);
    type2Coefficients.phasePerLayer.set_length(nLayers);
    type2Coefficients.wbAmplitudesPerLayer.set_length(nLayers);
    
    cmat Wf = generateDFTMatrix(N3);
    
    Array<cmat> W2PerLayer = type2Coefficients.W2PerLayer;
    ivec possibleFFTIndices = (M>1) ? getIntegers(0,N3-1) : to_ivec(0);
    if(N3>19)
    {
        int Minit;
        possibleFFTIndices = findPossibleFFTIndicesForAllLayers(W2PerLayer,M,N3,nLayers,Minit);
        type2Coefficients.pmiIndices.i15 = (Minit==0) ? 0 : Minit + 2*M;
    }
    
    cmat W1 = kron(eye_c(2),type2Coefficients.orthBeams);
    for(int layer_cnt=0;layer_cnt<nLayers;layer_cnt++)
    {
//         cout<<endl<<"------Layer "<<layer_cnt<<"------"<<endl;
        ivec fftIndices = to_ivec(0);
        cmat W2 = W2PerLayer(layer_cnt);
//         cout<<endl<<"Unquantized coefficients: "<<endl<<abs(W2)<<endl;
        if(M>=2)
        { 
            vec normW2(W2.rows());
            for(int c=0;c<W2.rows();c++)
            {
                normW2(c) = norm(W2.get_row(c));
            }
            int maxCol = max_index(normW2);
            double residue;
            cvec coEff = coeffAndResidueUsingOMP(W2.get_row(maxCol),Wf.get_cols(possibleFFTIndices),residue,M);
            fftIndices = possibleFFTIndices(reverse(sort_index(abs(coEff)))(0,M-1));
        }
        cmat W2tilda = W2*Wf.get_cols(fftIndices); // Should be Hermitian of Wf (Doesn't matter for flat fading)  - Pardhu
//         cout<<endl<<"Abs W2tilda Unquantized: "<<endl<<abs(W2tilda)<<endl;
//         cout<<endl<<"Arg W2tilda Unquantized: "<<endl<<arg(W2tilda)<<endl;
        int il_ = rearrangeW2(W2tilda,fftIndices,M,N3);
        W2tilda /= W2tilda(il_,0);
//         cout<<endl<<"Abs W2tilda after normalization: "<<endl<<abs(W2tilda)<<endl;
//         cout<<endl<<"Arg W2tilda after normalization: "<<endl<<arg(W2tilda)<<endl;
//          cout<<endl<<"UnQ: "<<endl<<abs(W2tilda)<<endl;
        type2Coefficients.pmiIndices.i18(layer_cnt) = il_;
        if(N3<=19)
        {

            int i16=0;
            ivec n3 = fftIndices;
            for(int f=1;f<M;f++)
                i16 += NCr(N3-1-n3(f),M-f);
            type2Coefficients.pmiIndices.i16(layer_cnt) = i16;
        }
        else
        {

            int i16=0;
            ivec n3 = fftIndices;
            int Minit = (type2Coefficients.pmiIndices.i15 == 0) ? 0 : type2Coefficients.pmiIndices.i15 -2*M;
            for(int f=1;f<M;f++)
            {
                int nlf = -1;
                if(n3(f)<=Minit+2*M-1)
                    nlf = n3(f);
                else if(n3(f)>Minit+N3-1)
                    nlf = n3(f) - N3 + 2*M;
                else
                {
                    cout<<"Invalid case"<<endl;abort();
                }
                i16 += NCr(2*M-1-nlf,M-f);
            }
            type2Coefficients.pmiIndices.i16(layer_cnt) = i16;
        }
        type2Coefficients.WfMatrixPerLayer = Wf.get_cols(fftIndices);
        
        for(int p=0;p<2;p++)
        {
//             cout<<endl<<"--Polarization "<<p<<"--"<<endl;
            int nWideAmp = dlCodeBookConfig.nWideAmp;
            int nWidePow = (dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_II_) ? 2 : 2;
            //nWidePow=4; //HardCoding this line
            // Table 5.2.2.2.3-2 38.214 v 15.1.0
            cvec value = reshape(W2tilda.get_rows(p*L,(p+1)*L-1),1,L*M).get_row(0);
//             cout<<endl<<"Quantizing -- > "<<abs(value)<<", "<<arg(value)<<endl;
//             cout<<endl<<"polarization_cnt: "<<p<<endl<<"value: "<<abs(value)<<endl;
            cvec value_dumm=zeros_c(value.length());
            int c=0;
            for(int i=0;i<value.length();i++){
                if(abs(value(i))>1e-6){
                    value_dumm(c)=value(i);
                    c=c+1;
                }
            }
//             cout<<endl<<"polarization_cnt: "<<p<<endl<<"value_dumm: "<<abs(value_dumm)<<endl;
            vec error(nWideAmp);
            vec P(nWideAmp); // Pw is wideband amp coeffs,
            Array<vec> sbAmpPerWbCoeff(nWideAmp),sbPhasePerWbCoeff(nWideAmp);
            Array<imat> sbAmpCoeffPerWbCoeff(nWideAmp);
            for(int i=0;i<nWideAmp;i++)
            {
                P(i)= pow(pow(2.0,nWideAmp-1-i),-(1.0/nWidePow));//1/sqrt(pow(2,7-i));
                if(i == 0){P(i)=0;}
//                 cout<<endl<<"WB Amp: "<<P(i)<<endl;
                cvec tempValue;
                if(i==0){tempValue = value_dumm*dComplex(P(i),0);}
                else{tempValue = value_dumm/dComplex(P(i),0);}
//                 cout<<endl<<"Pola Cnt: "<<p<<" : "<<"widecnt: "<<i<<endl<<"Wide Remove: "<<tempValue<<endl;
                sbAmpCoeffPerWbCoeff(i) = quantizeCoefficients(tempValue,sbAmpPerWbCoeff(i),sbPhasePerWbCoeff(i),tempValue);
                tempValue *= P(i);
                for(int i=0;i<tempValue.length();i++){
                    if(abs(value_dumm(i))<1e-6){
                        tempValue(i)=value_dumm(i);
                    }
                }
                error(i) = sum_sqr(abs(value_dumm-tempValue));
            }
//             cout<<endl<<"error: "<<error<<endl;
//             cout<<endl<<"error*13: "<<error*13<<endl;
            int minIndex = min_index(error);
//             cout<<endl<<"Selected WB Amp: "<<P(minIndex)<<endl;
            append(type2Coefficients.wbAmplitudesPerLayer(layer_cnt),P(minIndex));
            append(type2Coefficients.pmiIndices.i23(layer_cnt),minIndex);
            
            mat amp = sbAmpPerWbCoeff(minIndex);
//             cout<<endl<<"selected SB Amp: "<<amp<<endl;
//             cout<<endl<<"P(minIndex)*amp: "<<P(minIndex)*amp<<endl;
            amp = reshape(amp,L,M);
//             cout<<endl<<"wbamp: "<<endl<<amp<<endl;
            imat ampCoeff = sbAmpCoeffPerWbCoeff(minIndex);
            ampCoeff = reshape(ampCoeff,L,M);
            appendVertical(type2Coefficients.sbAmplitudesPerLayer(layer_cnt),amp);
            appendVertical(type2Coefficients.pmiIndices.i24(layer_cnt),ampCoeff);
            
                    
            mat phase = sbPhasePerWbCoeff(minIndex);
//             cout<<endl<<"selected SB Phase: "<<phase<<endl;
            phase = reshape(phase,L,M);
            appendVertical(type2Coefficients.phasePerLayer(layer_cnt),phase);
            phase = phase*dlCodeBookConfig.nPSK/(2*pi);
            imat phaseCoeff = round_i(phase);
            appendVertical(type2Coefficients.pmiIndices.i25(layer_cnt),phaseCoeff);
        
        }
              mat wbAmp = kron(diag(type2Coefficients.wbAmplitudesPerLayer(layer_cnt)),eye(Lbeams));
                    mat amp1 = wbAmp*type2Coefficients.sbAmplitudesPerLayer(layer_cnt);
//                     cout<<endl<<"Quantized Coeeficients: "<<endl<<amp1<<endl;
                    cmat phase = expj(type2Coefficients.phasePerLayer(layer_cnt));
                    cmat W2TildaQuantized = elem_mult(to_cmat(amp1),phase);
                    W2 = W2TildaQuantized*type2Coefficients.WfMatrixPerLayer(layer_cnt).H();
//                     cout<<endl<<"W2 Quantized: "<<endl<<abs(W2)<<endl;
                    
    }
}

void Codebook::compressCoefficientsAcrossTRPs(Array<Type2coefficients_N_OMP_S>& type2Coefficients,int& Lbeams,int nSubbands, const int nLayers)
{
    int M,N3 = nSubbands, R=1;double pv = 0.25;
    if(nLayers<=2) pv = 0.25;
    else if(Lbeams<=4) pv = 0.125;
    M = ceil_i(pv*N3/R);int L = Lbeams;
    int nTRPs = type2Coefficients.length();
    
    Array<Array<cvec>> avgAmpPerTRPPerLayerPerPol(nTRPs);
    for(int trp_cnt=0;trp_cnt<nTRPs;trp_cnt++)
    {
        type2Coefficients(trp_cnt).precoderPerSubband.set_length(nSubbands);
        for(int sb_cnt=0;sb_cnt<nSubbands;sb_cnt++)
        {
            type2Coefficients(trp_cnt).precoderPerSubband(sb_cnt) = zeros_c(dlCodeBookConfig.nCSIRSPorts,nLayers);
        }
        type2Coefficients(trp_cnt).WfMatrixPerLayer.set_length(nLayers);
        type2Coefficients(trp_cnt).pmiIndices.init(nLayers,dlCodeBookConfig.codeBookType);
        type2Coefficients(trp_cnt).sbAmplitudesPerLayer.set_length(nLayers);
        type2Coefficients(trp_cnt).phasePerLayer.set_length(nLayers);
        type2Coefficients(trp_cnt).wbAmplitudesPerLayer.set_length(nLayers);
    }
    
    Array<cmat> W2PerLayer(nLayers);
    for(int layer_cnt=0;layer_cnt<nLayers;layer_cnt++)
    {
        for(int trp_cnt=0;trp_cnt<nTRPs;trp_cnt++)
            appendVertical(W2PerLayer(layer_cnt),type2Coefficients(trp_cnt).W2PerLayer(layer_cnt));
    }
    cmat Wf = generateDFTMatrix(N3);
    
    ivec possibleFFTIndices = (M>1) ? getIntegers(0,N3-1) : to_ivec(0);
    if(N3>19)
    {
        int Minit;
        possibleFFTIndices = findPossibleFFTIndicesForAllLayers(W2PerLayer,M,N3,nLayers,Minit);
        for(int trp_cnt=0;trp_cnt<nTRPs;trp_cnt++)
            type2Coefficients(trp_cnt).pmiIndices.i15 = (Minit==0) ? 0 : Minit + 2*M;
    }
    
    for(int layer_cnt=0;layer_cnt<nLayers;layer_cnt++)
    {
        ivec fftIndices = to_ivec(0);
        cmat W2 = W2PerLayer(layer_cnt);
        
        if(M>=2)
        { 
            vec normW2(W2.rows());
            for(int c=0;c<W2.rows();c++)
            {
                normW2(c) = norm(W2.get_row(c));
            }
            int maxCol = max_index(normW2);
            double residue;
            cvec coEff = coeffAndResidueUsingOMP(W2.get_row(maxCol),Wf.get_cols(possibleFFTIndices),residue,M);
            
            fftIndices = possibleFFTIndices(reverse(sort_index(abs(coEff)))(0,M-1));
        }
        cmat W2tilda = W2*Wf.get_cols(fftIndices);
        int il_ = rearrangeW2(W2tilda,fftIndices,M,N3);
        W2tilda /= W2tilda(il_,0);
        
        for(int trp_cnt=0;trp_cnt<nTRPs;trp_cnt++)
        {
            type2Coefficients(trp_cnt).pmiIndices.i18(layer_cnt) = il_;
            type2Coefficients(trp_cnt).WfMatrixPerLayer(layer_cnt) = Wf.get_cols(fftIndices);
        }
        
        if(N3<=19)
        {
            int i16=0;
            ivec n3 = fftIndices;
            for(int f=1;f<M;f++)
                i16 += NCr(N3-1-n3(f),M-f);
            
            for(int trp_cnt=0;trp_cnt<nTRPs;trp_cnt++)
                type2Coefficients(trp_cnt).pmiIndices.i16(layer_cnt) = i16;
        }
        else
        {
            int i16=0;
            ivec n3 = fftIndices;
            int Minit = (type2Coefficients(0).pmiIndices.i15 == 0) ? 0 : type2Coefficients(0).pmiIndices.i15 -2*M;
            for(int f=1;f<M;f++)
            {
                int nlf = (n3(f)<=Minit+2*M-1) ? n3(f): n3(f)-(N3-2*M); 
                i16 += NCr(2*M-1-nlf,M-f);
            }
            
            for(int trp_cnt=0;trp_cnt<nTRPs;trp_cnt++)
                type2Coefficients(trp_cnt).pmiIndices.i16(layer_cnt) = i16;
        }
        
        for(int trp_cnt=0;trp_cnt<nTRPs;trp_cnt++)
        {
            cmat W2Tilda = W2tilda.get_rows(trp_cnt*2*L,(trp_cnt+1)*2*L-1);
            
            for(int p=0;p<2;p++)
            {
                int nWideAmp = dlCodeBookConfig.nWideAmp;
                int nWidePow = (dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_II_) ? 2 : 4;
                // Table 5.2.2.2.3-2 38.214 v 15.1.0
                cvec value = reshape(W2Tilda.get_rows(p*L,(p+1)*L-1),1,L*M).get_row(0);
                vec error(nWideAmp);
                vec P(nWideAmp); // Pw is wideband amp coeffs,
                Array<vec> sbAmpPerWbCoeff(nWideAmp),sbPhasePerWbCoeff(nWideAmp);
                Array<imat> sbAmpCoeffPerWbCoeff(nWideAmp);
                for(int i=0;i<nWideAmp;i++)
                {
                    P(i)= pow(pow(2.0,nWideAmp-1-i),-(1.0/nWidePow));//1/sqrt(pow(2,7-i));
                    cvec tempValue = value/dComplex(P(i),0);
                    sbAmpCoeffPerWbCoeff(i) = quantizeCoefficients(tempValue,sbAmpPerWbCoeff(i),sbPhasePerWbCoeff(i),tempValue);
                    tempValue *= P(i);
                    error(i) = sum_sqr(abs(value-tempValue));
                }
                int minIndex = min_index(error);
                append(type2Coefficients(trp_cnt).wbAmplitudesPerLayer(layer_cnt),P(minIndex));
                append(type2Coefficients(trp_cnt).pmiIndices.i23(layer_cnt),minIndex);
                
                mat amp = sbAmpPerWbCoeff(minIndex);
                amp = reshape(amp,L,M);
                imat ampCoeff = sbAmpCoeffPerWbCoeff(minIndex);
                ampCoeff = reshape(ampCoeff,L,M);
                appendVertical(type2Coefficients(trp_cnt).sbAmplitudesPerLayer(layer_cnt),amp);
                appendVertical(type2Coefficients(trp_cnt).pmiIndices.i24(layer_cnt),ampCoeff);
                
                mat phase = sbPhasePerWbCoeff(minIndex);
                phase = reshape(phase,L,M);
                appendVertical(type2Coefficients(trp_cnt).phasePerLayer(layer_cnt),phase);
                phase = phase*dlCodeBookConfig.nPSK/(2*pi);
                imat phaseCoeff = round_i(phase);
                appendVertical(type2Coefficients(trp_cnt).pmiIndices.i25(layer_cnt),phaseCoeff);
            }
        }
    }
}

void Codebook::compressCoefficientsAcrossSubbandsAndLayers(Type2coefficients_N_OMP_S& type2Coefficients,int& Lbeams,int nSubbands, const int nLayers)
{
    int M,N3 = nSubbands, R=1;double pv = 0.25;
    if(nLayers<=2) pv = 0.25;
    else if(Lbeams<=4) pv = 0.125;
    int N = 4;
    
    M = min(2,N3);int L = Lbeams;
    
    type2Coefficients.precoderPerSubband.set_length(nSubbands);
    for(int sb_cnt=0;sb_cnt<nSubbands;sb_cnt++)
    {
        type2Coefficients.precoderPerSubband(sb_cnt) = zeros_c(dlCodeBookConfig.nCSIRSPorts,nLayers);
    }
    type2Coefficients.pmiIndices.init(nLayers,dlCodeBookConfig.codeBookType);
    type2Coefficients.WfMatrixPerLayer.set_length(nLayers);
    type2Coefficients.sbAmplitudesPerLayer.set_length(nLayers);
    type2Coefficients.phasePerLayer.set_length(nLayers);
    type2Coefficients.wbAmplitudesPerLayer.set_length(nLayers);
    
    Array<cmat> W2PerLayer = type2Coefficients.W2PerLayer;
    
    cmat Wf = generateDFTMatrix(N3);
    cmat W1 = kron(eye_c(2),type2Coefficients.orthBeams);
    vec normW2(nLayers*2*L);
    for(int layer_cnt=0;layer_cnt<nLayers;layer_cnt++)
    {
        cmat W2 = type2Coefficients.W2PerLayer(layer_cnt);
        
        for(int c=0;c<W2.rows();c++)
        {
            normW2(layer_cnt*2*L+c) = norm(W2.get_row(c));
        }
    }
    ivec fftIndices = to_ivec(0);type2Coefficients.pmiIndices.i15=-1;
    if(M==2)
    {
        int nVec = min(N,N3);
        int maxIndex = max_index(normW2);
        int layerIndx = maxIndex/(2*L);
        int rowIndx = maxIndex%(2*L);
        double residue;
        ivec possibleFFTIndices = getIntegers(0,nVec-1);
        cvec coEff = coeffAndResidueUsingOMP(W2PerLayer(layerIndx).get_row(rowIndx),Wf.get_cols(possibleFFTIndices),residue,M);
        
        fftIndices = reverse(sort_index(abs(coEff)))(0,M-1);
        type2Coefficients.pmiIndices.i15 = circmod(fftIndices(1) - fftIndices(0),nVec) - 1;
        fftIndices(0) = 0; 
        fftIndices(1) = type2Coefficients.pmiIndices.i15+1;
    }
    for(int layer_cnt=0;layer_cnt<nLayers;layer_cnt++)
    {
        cmat W2 = W2PerLayer(layer_cnt);
        cmat W2tilda = W2*Wf.get_cols(fftIndices);
        mat absW2 = abs(W2tilda);
        int maxIndex = max_index(absW2.get_col(0));
        type2Coefficients.pmiIndices.i18(layer_cnt) = maxIndex;
        
        W2tilda /= W2tilda(maxIndex,0);
        type2Coefficients.WfMatrixPerLayer(layer_cnt) = Wf.get_cols(fftIndices);
        
        for(int p=0;p<2;p++)
        {
            int nWideAmp = dlCodeBookConfig.nWideAmp;
            int nWidePow = (dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_II_) ? 2 : 4;
            // Table 5.2.2.2.3-2 38.214 v 15.1.0
            cvec value = reshape(W2tilda.get_rows(p*L,(p+1)*L-1),1,L*M).get_row(0);
            vec error(nWideAmp);
            vec P(nWideAmp); // Pw is wideband amp coeffs,
            Array<vec> sbAmpPerWbCoeff(nWideAmp),sbPhasePerWbCoeff(nWideAmp);
            Array<imat> sbAmpCoeffPerWbCoeff(nWideAmp);                                                         
            for(int i=0;i<nWideAmp;i++)
            {
                P(i)= pow(pow(2.0,nWideAmp-1-i),-(1.0/nWidePow));//1/sqrt(pow(2,7-i));
                cvec tempValue = value/dComplex(P(i),0);
                sbAmpCoeffPerWbCoeff(i) = quantizeCoefficients(tempValue,sbAmpPerWbCoeff(i),sbPhasePerWbCoeff(i),tempValue);
                tempValue *= P(i);
                error(i) = sum_sqr(abs(value-tempValue));
            }
            int minIndex = min_index(error);
            append(type2Coefficients.wbAmplitudesPerLayer(layer_cnt),P(minIndex));
            append(type2Coefficients.pmiIndices.i23(layer_cnt),minIndex);
            
            mat amp = sbAmpPerWbCoeff(minIndex);
            amp = reshape(amp,L,M);
            imat ampCoeff = sbAmpCoeffPerWbCoeff(minIndex);
            ampCoeff = reshape(ampCoeff,L,M);
            appendVertical(type2Coefficients.sbAmplitudesPerLayer(layer_cnt),amp);
            appendVertical(type2Coefficients.pmiIndices.i24(layer_cnt),ampCoeff);
            
            mat phase = sbPhasePerWbCoeff(minIndex);
            phase = reshape(phase,L,M);
            appendVertical(type2Coefficients.phasePerLayer(layer_cnt),phase);
            phase = phase*dlCodeBookConfig.nPSK/(2*pi);
            imat phaseCoeff = round_i(phase);
            appendVertical(type2Coefficients.pmiIndices.i25(layer_cnt),phaseCoeff);
        }
    }    
}

Type2coefficients_N_OMP_S Codebook::getType2OMPCoefficients(Array<cmat>& entireRawChannel,int nSubbands,int subbandSize,int reCountPerRb, const int nLayers)
{
    bool isEnhancedCodebook = true;
    if(dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_II_ || dlCodeBookConfig.codeBookType == _NR_CODEBOOK_PORT_SELECTION_)
        isEnhancedCodebook = false;
//     bool useIdealPrecoder = dlCodeBookConfig.isType2IdealCoeff; 
    bool useIdealPrecoder=dlCodeBookConfig.isType2IdealPrecoder;
    if(!useIdealPrecoder)
        dlCodeBookConfig.isType2IdealCoeff = isEnhancedCodebook;        //Check - Pardhu
    Type2coefficients_N_OMP_S type2Coefficients;
    type2Coefficients.pmiIndices.init(nLayers,dlCodeBookConfig.codeBookType);
    int Lbeams = dlCodeBookConfig.LBeams;
    switch(dlCodeBookConfig.codeBookType)
    {
        case _NR_CODEBOOK_TYPE_II_:
        case _NR_CODEBOOK_TYPE_II_ENHANCED_:
        case _NR_CODEBOOK_TYPE_II_ENHANCED_CJT_:
            computeBeamSetForTypeII(type2Coefficients,entireRawChannel,Lbeams,nLayers);
            break;
        case _NR_CODEBOOK_PORT_SELECTION_:
        case _NR_CODEBOOK_PORT_SELECTION_ENHANCED_:
        case _NR_CODEBOOK_PORT_SELECTION_FURTHER_ENHANCED_:
            computeBeamSetForTypeIIPortSelection(type2Coefficients,entireRawChannel,Lbeams,nLayers);
            break;
        default:
            cout<<"Unsupported Codebook Type"<<endl;
            abort();
    }
   // cout<<endl<<"Enter"<<endl;
    computeSubbandCoefficientsForIdealPrecoder(type2Coefficients,entireRawChannel,Lbeams,nSubbands,subbandSize,reCountPerRb,nLayers);
    if(useIdealPrecoder){
            type2Coefficients.precoderPerSubband = type2Coefficients.idealPrecoderperSubband;
    }
    else
    {
        if(!isEnhancedCodebook)
        {
//             cout<<endl<<"Type II"<<endl;
            computeSubbandCoefficientsForTypeII(type2Coefficients,entireRawChannel,Lbeams,nSubbands,subbandSize,reCountPerRb,nLayers);
        }
        else
        {
            dlCodeBookConfig.isType2IdealCoeff = false;
            if(dlCodeBookConfig.codeBookType == _NR_CODEBOOK_PORT_SELECTION_FURTHER_ENHANCED_)
                compressCoefficientsAcrossSubbandsAndLayers(type2Coefficients,Lbeams,nSubbands,nLayers);
            else{
//                 cout<<endl<<"Enhanced: "<<endl;
                compressCoefficientsAcrossSubbands(type2Coefficients,Lbeams,nSubbands,nLayers);
            }
        }
        
        type2Coefficients.precoderPerSubband = getType2Precoder_OMP(type2Coefficients,nSubbands,nLayers);
        cmat W1 = kron(eye_c(2),type2Coefficients.orthBeams);
        int start = 0*subbandSize*reCountPerRb;
        int stop = (0+1)*subbandSize*reCountPerRb-1;
        Array<cmat> subbandChannel = entireRawChannel(start,stop);
        cmat avgSubbandChannelCovar = avgCovariance(subbandChannel);
        cmat V;
        cvec eigenValues;
        bool isSuccess = eig(avgSubbandChannelCovar, eigenValues, V);

        V = V.get_cols(reverse(sort_index(abs(eigenValues))));
//         for(int i=0;i<nSubbands;i++){
//             cout<<endl<<"i: "<<i<<endl;
// //         cout<<endl<<"V*P: "<<abs(V.get_cols(0,0).H()*type2Coefficients.precoderPerSubband(i))<<endl;
//         }
//          cout<<endl<<"P: "<<endl<<(type2Coefficients.precoderPerSubband(0))<<endl;
//          cout<<endl<<"Type II V-P: "<<norm((V.get_cols(0,0))-(type2Coefficients.precoderPerSubband(0)),"fro")<<endl;
//                 cout<<endl<<"Type II Norm: "<<norm(type2Coefficients.precoderPerSubband(0),"fro")<<endl;


    }
    return type2Coefficients;
}

void Codebook::computeBeamSetForTypeIIMultiTRP(Array<Type2coefficients_N_OMP_S>& type2Coefficients,Array<Array<cmat>>& entireRawChannel,int& Lbeams,int nLayers)
{
    int nTx = entireRawChannel(0)(0).cols();
    int nTRPs = entireRawChannel.length();
    int N1=dlCodeBookConfig.N1, N2=dlCodeBookConfig.N2, O1=dlCodeBookConfig.O1, O2 = dlCodeBookConfig.O2;
    Array<ivec> i11_IndicesPerSet(O1*O2);  // Stores corresponding i11 index of the beams in set
    Array<ivec> i12_IndicesPerSet(O1*O2);  // Stores corresponding i12 index of the beams in set
    Array<cmat> orthBeamsTotalSets = type2OrthogonalBeamsSet(N1, N2, i11_IndicesPerSet, i12_IndicesPerSet); // Generating orthogonal beam sets
    
    Array<cmat> halfSingularMatrixPerTRP(nTRPs);
    for(int trp_cnt=0;trp_cnt<nTRPs;trp_cnt++)
    {
        type2Coefficients(trp_cnt).pmiIndices.init(nLayers,dlCodeBookConfig.codeBookType);
        cmat avgChannelCovar = avgCovariance(entireRawChannel(trp_cnt));
        cmat U,V;  vec S; cvec eigenValues;
        bool isSuccess = eig(avgChannelCovar, eigenValues, V);
        cvec firstSingularVector = (1/norm(V.get_col(0)))*V.get_col(0);
        halfSingularMatrixPerTRP(trp_cnt) = V(0,nTx/2-1,0,nLayers-1);
    }
    
    int nSets = orthBeamsTotalSets.length();
    vec metric = zeros(nSets);
    Array<ivec> indicesPerSet(nSets);
    for(int set_cnt=0;set_cnt<nSets;set_cnt++)
    {
        vec projOverBeams = zeros(orthBeamsTotalSets(set_cnt).cols());
        dComplex num= 0,denom = 0;
        for(int trp_cnt=0;trp_cnt<nTRPs;trp_cnt++)
        {
            cmat A = orthBeamsTotalSets(set_cnt).H()*halfSingularMatrixPerTRP(trp_cnt);
            cmat K = A*A.H();
            projOverBeams += abs(diag(K)); //elem_div(diag(K),sum(K,1)-diag(K));
            num += trace(K);
            denom += (sum(sum(K)) - trace(K));
        }
        metric(set_cnt) = abs(num)/abs(denom);
        ivec sortIndex = reverse(sort_index(projOverBeams));
        indicesPerSet(set_cnt) = sortIndex(0,Lbeams-1);
    }
    int selectedSet = max_index(metric);
    ivec indices = indicesPerSet(selectedSet);
    
    for(int trp_cnt=0;trp_cnt<nTRPs;trp_cnt++)
    {
        ivec selectedColumnsIndicesInLibrary = indices;
        cmat orthogonalBeams = orthBeamsTotalSets(selectedSet).get_cols(selectedColumnsIndicesInLibrary);
        type2Coefficients(trp_cnt).i11Indices = i11_IndicesPerSet(selectedSet)(selectedColumnsIndicesInLibrary);
        type2Coefficients(trp_cnt).i12Indices = i12_IndicesPerSet(selectedSet)(selectedColumnsIndicesInLibrary);
        type2Coefficients(trp_cnt).orthBeams = orthogonalBeams;
        
        int q1 = type2Coefficients(trp_cnt).i11Indices(0)%O1,q2 = type2Coefficients(trp_cnt).i12Indices(0)%O2;
        ivec n1 = (type2Coefficients(trp_cnt).i11Indices-q1)/O1,n2 = (type2Coefficients(trp_cnt).i12Indices-q2)/O2;
        ivec n = N1*n2+n1;
        indices = sort_index(n);
        n = n(indices);
        int i12 = 0;
        for(int i=0;i<Lbeams;i++)
            i12 += NCr(N1*N2-1-n(i),Lbeams-i);
        type2Coefficients(trp_cnt).pmiIndices.i11 = q1*O2+q2;
        type2Coefficients(trp_cnt).pmiIndices.i12 = i12;
        
        type2Coefficients(trp_cnt).orthBeams = type2Coefficients(trp_cnt).orthBeams.get_cols(indices);
        type2Coefficients(trp_cnt).i11Indices = type2Coefficients(trp_cnt).i11Indices(indices);
        type2Coefficients(trp_cnt).i12Indices = type2Coefficients(trp_cnt).i12Indices(indices);
    }
}

void Codebook::getType2OMPCoefficientsForMultiTRPCJT(Array<Type2coefficients_N_OMP_S>& type2CoefficientsPerTRP,Array<Array<cmat>>& entireRawChannel,int nSubbands,int subbandSize,int reCountPerRb, const int nLayers)
{
    int nTRPs = entireRawChannel.length();
    int Lbeams = dlCodeBookConfig.LBeams;
    
    type2CoefficientsPerTRP.set_length(nTRPs);
    
    bool useJointFDCompression=false;
    bool useCombinedBeamSet=false;
    if(useCombinedBeamSet)
        computeBeamSetForTypeIIMultiTRP(type2CoefficientsPerTRP,entireRawChannel,Lbeams,nLayers);
    for(int trp_cnt=0;trp_cnt<nTRPs;trp_cnt++)
    {
        if(!useCombinedBeamSet)
        {
            type2CoefficientsPerTRP(trp_cnt).pmiIndices.init(nLayers,dlCodeBookConfig.codeBookType);
            computeBeamSetForTypeII(type2CoefficientsPerTRP(trp_cnt),entireRawChannel(trp_cnt),Lbeams,nLayers);
        }
        dlCodeBookConfig.isType2IdealCoeff = true;
        computeSubbandCoefficientsForIdealPrecoder(type2CoefficientsPerTRP(trp_cnt),entireRawChannel(trp_cnt),Lbeams,nSubbands,subbandSize,reCountPerRb,nLayers);
        if(!useJointFDCompression)
        {
            dlCodeBookConfig.isType2IdealCoeff = false;
            compressCoefficientsAcrossSubbands(type2CoefficientsPerTRP(trp_cnt),Lbeams,nSubbands,nLayers);
            type2CoefficientsPerTRP(trp_cnt).precoderPerSubband = getType2Precoder_OMP(type2CoefficientsPerTRP(trp_cnt),nSubbands,nLayers);
        }
    }
    if(useJointFDCompression)
    {
        dlCodeBookConfig.isType2IdealCoeff = false;
        compressCoefficientsAcrossTRPs(type2CoefficientsPerTRP,Lbeams,nSubbands,nLayers);
        for(int trp_cnt=0;trp_cnt<nTRPs;trp_cnt++)
            type2CoefficientsPerTRP(trp_cnt).precoderPerSubband = getType2Precoder_OMP(type2CoefficientsPerTRP(trp_cnt),nSubbands,nLayers);
    }
}

Array<cmat> Codebook::getType2PrecoderbyCombineLbeams(Array< cmat > LHalfbeams, Array<Array<vec>> amp, Array< Array< ivec > > phase, int nLayers, int sb_cnt, int nPSK)
{
  int L=LHalfbeams.length();
  int nPrecoders = (nLayers == 1) ? 1 : 2;
  
  Array<cmat> precoder(nPrecoders);
  
  vec normalizationFactor = zeros(nLayers);
//   if(amp(sb_cnt).length()!= 2*L)
//   {
//     cout<<"Insufficient beams or weights!! aborting"<<endl;abort();
//   }
  vec Pw,Ps;
  Pw.set_length(8); // Pw is wideband amp coeffs
  Ps.set_length(2); // Ps is subband amp coeffs
  // Table 5.2.2.2.3-2 38.214 v 15.1.0
  Pw(0)=0;
  for(int i=1;i<8;i++)
    Pw(i)= 1/sqrt(pow(2,7-i));
    
  // Table 5.2.2.2.3-3 38.214 v 15.1.0
  Ps(0) = 1/sqrt(2); Ps(1)=1;
  
  Array<cmat> pol1,pol2,pol11;
  int nRows = LHalfbeams(0).rows();
  int nCols = LHalfbeams(0).cols();
  pol1.set_length(nLayers);
  pol2.set_length(nLayers);
  pol11.set_length(nLayers);
    
  for(int layer=0;layer<nLayers;layer++)
  {
      
    vec quantizedAmp;
    quantizedAmp.set_length(2*L);
    
    for(int i=0;i<2*L;i++)
        quantizedAmp(i) = Pw(findClosest(Pw,amp(sb_cnt)(layer)(i)));
      
    pol1(layer) = zeros_c(nRows,nCols);
    pol2(layer) = zeros_c(nRows,nCols);
    pol11(layer) = zeros_c(nRows,nCols);

    for(int i=0;i<L;i++)
    {
      
      pol1(layer) =pol1(layer) + (quantizedAmp(i))*Phi(phase(0)(i)(sb_cnt),nPSK)*LHalfbeams(i); 
      pol2(layer) =pol2(layer) + (quantizedAmp(i+L))*(layer==0?1:-1)*Phi(phase(1)(i)(sb_cnt),nPSK)*LHalfbeams(i);

      dComplex a = complex<double>(0,1);
      pol11(layer) = pol11(layer) + (quantizedAmp(i+L))* (layer==0?a:-a) *Phi(phase(1)(i)(sb_cnt),nPSK)*LHalfbeams(i);
      
      normalizationFactor(layer) += pow(quantizedAmp(i),2) + pow(quantizedAmp(i+L),2);
    }
    normalizationFactor(layer)=1/sqrt(dlCodeBookConfig.N1*dlCodeBookConfig.N2*normalizationFactor(layer));
  }
 
  if(nLayers==2)
  {
    precoder(0) = (1/sqrt(2))*concat_horizontal(normalizationFactor(0)*concat_vertical(pol1(0),pol2(0)), normalizationFactor(1)*concat_vertical(pol1(1),pol2(1)));
    precoder(1) = (1/sqrt(2))*concat_horizontal(normalizationFactor(0)*concat_vertical(pol1(0),pol11(0)), normalizationFactor(1)*concat_vertical(pol1(1),pol11(1)));
  }
  else
    precoder(0) = normalizationFactor(0)*concat_vertical(pol1(0),pol2(0));
  
  
  return precoder;
}

ivec getn3Indices(int i15,int i16,int N3,int M)
{
    ivec n3 = zeros_i(M);
    int s=0;
    int Minit =(i15==0) ? 0: i15-2*M;
    for(int f=1;f<M;f++)
    {
        for(int x = N3-1-f;x>=M-1-f;x--)
        {
            int e = NCr(x,M-f);
            if(i16-s>=e)
            {
                s = s+e;
                if(N3<=19)
                    n3(f) = N3-1-x;
                else
                {
                    int nlf = 2*M-1-x;
                    if(nlf<=Minit+2*M-1)
                        n3(f) = nlf;
                    else
                        n3(f) = nlf + (N3-2*M);
                }
                break;
            }
        }
    }
    return n3;
}

void computeBeamIndices(ivec& n1,ivec& n2,int i12,int N1,int N2,int L)
{
    ivec n(L);n1.set_length(L);n2.set_length(L);
    int s = 0;
    for(int i=0;i<L;i++)
    {
        for(int x=N1*N2-1-i;x>=L-1-i;x--)
        {
            int e = NCr(x,L-i);
            if(i12-s>=e)
            {
                s = s+e;
                n(i) = N1*N2-1-x;
                n1(i) = n(i)%N1;
                n2(i) = (n(i) - n1(i))/N1;
                break;
            }
        }
    }
}

int selectSet(const Array<cmat> orthBeamsTotalSets, cmat halfSingularMatrix,int L,ivec& indices)
{
    int nSets = orthBeamsTotalSets.length();
    vec metric = zeros(nSets);
    Array<ivec> indicesPerSet(nSets);
    vec residuePerSet(nSets);
    for(int set_cnt=0;set_cnt<nSets;set_cnt++)
    {
        cmat A = orthBeamsTotalSets(set_cnt).H()*halfSingularMatrix;
        mat K = abs(A*A.H());
        vec p = diag(K);
        ivec sortIndex = reverse(sort_index(sum(abs(A),2)));
//         ivec sortIndex = reverse(sort_index(abs(sum(A,2))));
        indicesPerSet(set_cnt) = sortIndex(0,L-1);
        residuePerSet(set_cnt) = norm(halfSingularMatrix - orthBeamsTotalSets(set_cnt)*A,"fro");
        metric(set_cnt) = trace(K)/((sum(sum(K)))-trace(K));
    }
    int selectedSet = max_index(metric);
    indices = indicesPerSet(selectedSet);
    return selectedSet;
}

int selectSetWithFullEigenVector(const Array<cmat> orthBeamsTotalSets, cmat fullSingularMatrix,int L,ivec& indices)
{
    int nSets = orthBeamsTotalSets.length();
    vec metric = zeros(nSets);
    Array<ivec> indicesPerSet(nSets);
    vec residuePerSet(nSets);
    for(int set_cnt=0;set_cnt<nSets;set_cnt++)
    {
        cmat tempBeamSet = kron(eye_c(2),orthBeamsTotalSets(set_cnt));
        cmat A = tempBeamSet.H()*fullSingularMatrix;
        mat K = abs(A*A.H());
        vec p = diag(K);
        ivec sortIndex = reverse(sort_index(sum(abs(A),2)));
        sortIndex = mod(sortIndex,sortIndex.length()/2);
        sortIndex = findUniqueNumbersWithoutSorting(sortIndex);
        indicesPerSet(set_cnt) = sortIndex(0,L-1);
//        residuePerSet(set_cnt) = norm(fullSingularMatrix - orthBeamsTotalSets(set_cnt)*A);
        metric(set_cnt) = trace(K)/((sum(sum(K)))-trace(K));
    }
    int selectedSet = max_index(metric);
    indices = indicesPerSet(selectedSet);
    return selectedSet;
}

int selectSetWithChannelCovariance(const Array<cmat> orthBeamsTotalSets, cmat avgChannelCovar,int L,ivec& indices){
    int nTx = avgChannelCovar.cols();
    Array<vec> beamStrengthPerBeamSet(orthBeamsTotalSets.length());
    vec metric(orthBeamsTotalSets.length());
    Array<ivec> bestBeamsPerBeamSet(orthBeamsTotalSets.length());
    for(int beamSetIndx = 0;beamSetIndx < orthBeamsTotalSets.length();beamSetIndx++){
        cmat temp1 = orthBeamsTotalSets(beamSetIndx).H()*avgChannelCovar(0,nTx/2-1,0,nTx/2-1)*orthBeamsTotalSets(beamSetIndx);
        cmat temp2 = orthBeamsTotalSets(beamSetIndx).H()*avgChannelCovar(nTx/2,nTx-1,nTx/2,nTx-1)*orthBeamsTotalSets(beamSetIndx);
        cmat temp = temp1+temp2;
        beamStrengthPerBeamSet(beamSetIndx) = abs(diag(temp));
        ivec sortIndex = reverse(sort_index(beamStrengthPerBeamSet(beamSetIndx)));
        bestBeamsPerBeamSet(beamSetIndx) = sortIndex(0,L-1);
        metric(beamSetIndx) = sum(beamStrengthPerBeamSet(beamSetIndx)(bestBeamsPerBeamSet(beamSetIndx)));
    }
    int selectedSet = max_index(metric);
    indices = bestBeamsPerBeamSet(selectedSet);
    return selectedSet;
}


int selectSet(const Array<cmat> orthBeamsTotalSets, cvec halfSingularVector)
{
    int selectedSet=-1;
    int numOfSets = orthBeamsTotalSets.length();
    int setCnt=0;
    Array<vec> projections(numOfSets);
//     double normOfHalfSingularVector = norm(halfSingularVector);
//     halfSingularVector = halfSingularVector*(1/normOfHalfSingularVector); // Normalizing the vector to remove channel losses
    vec maxProjectionsOverSets(numOfSets); 
    while(setCnt<numOfSets)
    {
        projections(setCnt).set_length(orthBeamsTotalSets(0).cols());
        projections(setCnt) = abs(orthBeamsTotalSets(setCnt).H()*halfSingularVector);
        maxProjectionsOverSets(setCnt) = max(projections(setCnt));
        setCnt++;
    }
    selectedSet = max_index(maxProjectionsOverSets);
    return selectedSet;
}

int selectSet(Array<cmat>& orthBeamsTotalSets,Array<cmat>& entireRawChannel,int L,ivec& indices)
{
    int selectedSet = -1,nTx = entireRawChannel(0).cols();
    int nSets = orthBeamsTotalSets.length();
    vec maxAmpPerSet = zeros(nSets);
    Array<ivec> indicesPerSet(nSets);
    for(int set_cnt=0;set_cnt<nSets;set_cnt++)
    {
        vec ampPerBeam = zeros(orthBeamsTotalSets(set_cnt).cols());
        for(int tone_cnt=0;tone_cnt<entireRawChannel.length();tone_cnt++)
        {
            cmat amp1 = entireRawChannel(tone_cnt).get_cols(0,nTx/2-1)*orthBeamsTotalSets(set_cnt);
            amp1 = amp1.H()*amp1;
            ampPerBeam += abs(diag(amp1));
            
            amp1 = entireRawChannel(tone_cnt).get_cols(nTx/2,nTx-1)*orthBeamsTotalSets(set_cnt);
            amp1 = amp1.H()*amp1;
            ampPerBeam += abs(diag(amp1));
        }
        indicesPerSet(set_cnt) = reverse(sort_index(ampPerBeam))(0,L-1);
        maxAmpPerSet(set_cnt) = sum(ampPerBeam(indicesPerSet(set_cnt)));
    }
    selectedSet = max_index(maxAmpPerSet);
    indices = indicesPerSet(selectedSet);
    return selectedSet;
}

int selectSetUsingOMP(const Array<cmat> orthBeamsTotalSets, cmat halfSingularMatrix,int L,ivec& indices)
{
    int nSets = orthBeamsTotalSets.length();
    int selectedSet=-1;
    Array<ivec> indicesPerSet(nSets);
    vec residuePerSet(nSets);
    for(int set_cnt=0;set_cnt<nSets;set_cnt++)
    {
        int nBeams = orthBeamsTotalSets(set_cnt).cols();
        
        cmat X = orthBeamsTotalSets(set_cnt);
        cmat A = X.H()*halfSingularMatrix;
        bmat allComb = getBinaryCombinations(nBeams,L);
        int nComb = allComb.rows();
        vec residuePerComb(nComb);
        for(int comb_cnt=0;comb_cnt<nComb;comb_cnt++)
        {
            ivec beamIndices = find(allComb.get_row(comb_cnt));
            cmat x = X.get_cols(beamIndices);
            cmat a = A.get_rows(beamIndices);
            residuePerComb(comb_cnt) = norm(halfSingularMatrix - x*a,"fro");
        }
        int selectedComb = min_index(residuePerComb);
        residuePerSet(set_cnt) = residuePerComb(selectedComb);
        indicesPerSet(set_cnt) = find(allComb.get_row(selectedComb));
    }
    selectedSet = min_index(residuePerSet);
    indices = indicesPerSet(selectedSet);
    return selectedSet;
}

Array<cmat> getPortSelectionSets(int N1,int N2,int d,int Lbeams,Array<ivec>& indicesPerComb)
{
    Array<cmat> portSelectionSets;
    cmat W = eye_c(N1*N2);
    if(d!=-1)
    {
        int nSets = ceil_i((double)N1*N2/d);
        portSelectionSets.set_size(nSets);
        for(int set_cnt=0;set_cnt<nSets;set_cnt++)
        {
            portSelectionSets(set_cnt) = W.get_cols(set_cnt*d + getIntegers(0,Lbeams-1));
        }
    }
    else
    {
        bmat comb = getBinaryCombinations(N1*N2,Lbeams);
        int nComb = comb.rows();
        portSelectionSets.set_size(nComb);
        indicesPerComb.set_size(nComb);
        for(int set_cnt=0;set_cnt<nComb;set_cnt++)
        {
            indicesPerComb(set_cnt) = find(comb.get_row(set_cnt));
            portSelectionSets(set_cnt) = W.get_cols(indicesPerComb(set_cnt));
        }
    }
    return portSelectionSets;
}

Array<cmat> type2OrthogonalBeamsSet(int N1, int N2, Array<ivec>& i11_IndicesPerSet, Array<ivec>& i12_IndicesPerSet)
{
    int O1=4,O2 = (N2==1)? 1:4;
    ivec i = getIntegers(0,N1-1);
    ivec j = getIntegers(0,N2-1);
    Array<cmat> orthBeamsTotalSets;orthBeamsTotalSets.set_length(O1*O2);  // Each array contains mutually orthogonal beams
    i11_IndicesPerSet.set_length(O1*O2); // Stores i11 indices of the corresponding beams in orthogonal set
    i12_IndicesPerSet.set_length(O1*O2); // Stores i12 indices of the corresponding beams in orthogonal set
    int orthSetCnt=0; // Range O1*O2

    for(int q1=0;q1<O1;q1++)
    {
        for(int q2=0;q2<O2;q2++,orthSetCnt++)
        {
            cmat beamSet;
            for(int n1=0;n1<N1;n1++)
            {
                for(int n2=0;n2<N2;n2++)
                {
                    beamSet = concat_horizontal(beamSet,getVlm(n1*O1+q1,n2*O2+q2,N1,N2,O1,O2));
                    
                    append(i11_IndicesPerSet(orthSetCnt),n1*O1+q1);
                    append(i12_IndicesPerSet(orthSetCnt),n2*O2+q2);
                }
            }
            orthBeamsTotalSets(orthSetCnt) = beamSet/sqrt(2*N1*N2);
        }
    }
    
    return orthBeamsTotalSets;
}

