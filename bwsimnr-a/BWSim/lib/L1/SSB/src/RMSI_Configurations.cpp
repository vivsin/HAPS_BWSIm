/*************************************************************************
 * 
 * CEWiT CONFIDENTIAL
 * __________________
 *
 * All Rights Reserved © 2014 CEWiT, India
 *
 * NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
 * and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
 * Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
 * express, printed and signed license for use is strictly forbidden.
 */

#include "../include/RMSI_Configurations.h"

void pbchContents::init(string configFile)
{   
    int prbOffsetIndx,rmsiTMOIndx;
    parse(configFile,"kSSB",kSSB);
    parse(configFile,"subCarrierSpacingCommon",SubCarrierSpacingCommon);
    parse(configFile,"rmsiPRBOffsetIndx",prbOffsetIndx);
    rmsiConfiguration.prbOffsetIndx=prbOFFSET_E(prbOffsetIndx);
    parse(configFile,"rmsiTimingAndMonitoringOccasionsIndx",rmsiTMOIndx);
    rmsiConfiguration.tmoIndx=TMO_E(rmsiTMOIndx);
}

void ssbCoreset0Configuration::init(int ssbSCS,pbchContents& pbchContents)
{
    prbOffsetConfiguration.prbOffsetIndx=pbchContents.rmsiConfiguration.prbOffsetIndx;
    timingAndMonitoringOccasionsConfiguration.tmoIndx=pbchContents.rmsiConfiguration.tmoIndx;
    ssbSubCarrierSpacing=ssbSCS;
    //Possible ueTxBandwidth={5,10,40} for FR1
    //setting ueTxBandwidth to default minUEBandwidth based on ssbSCS
    if     (ssbSCS==15) ueTxBandwidth=5;//inMHz
    else if(ssbSCS==30) ueTxBandwidth=10;
    else                ueTxBandwidth=-1;//implicit from ssbSCS
}

void ssbCoreset0Configuration::init(int ssbSCS, pbchContents& pbchContents, int UEBandwidth)
{
    prbOffsetConfiguration.prbOffsetIndx=pbchContents.rmsiConfiguration.prbOffsetIndx;
    timingAndMonitoringOccasionsConfiguration.tmoIndx=pbchContents.rmsiConfiguration.tmoIndx;
    ssbSubCarrierSpacing=ssbSCS;
    ueTxBandwidth=UEBandwidth;
}

void LoadCoreset0Tables(string FileName,Coreset0Tables& Coreset0Tables)
{
    Coreset0Tables.RbSymOffConfigTables.set_length(13);
    Coreset0Tables.TMOConfigTables.set_length(4);
    string filename1,filename2;
    for(int count=0;count<13;count++)
    {
        filename1=FileName+"_RbSymOffConfig_Mat"+toString(count+1)+".txt";
        parse(filename1,"CORESET0_RbSymOffConfig_Mat",Coreset0Tables.RbSymOffConfigTables(count));
        if(count<4)
        {
            filename2=FileName+"_TMO_"+toString(count+1)+".txt";
            parse(filename2,"CORESET0_TMOConfig_Mat",Coreset0Tables.TMOConfigTables(count));
        }
    }
}

void Coreset0Configuration(prbOffsetConfig_S *RbSymOffConfig,TMO_Config_S *tmoConfig,int ssbSCS,int rmsiSCS,int ssbIndx, int sfnSSB,int slotSSB, int kSSB, int ueTxBandwidth,rmsiConfig_S rmsiConfig, Coreset0Tables coreset0Tables)
{    
    if((ssbSCS==15 || ssbSCS==30)/*FR1*/ && kSSB>23)
    {
        cout<<"Invalid SSB: Coreset0Configuration doesnot exit"<<endl;
        abort();
    }
    else if((ssbSCS==15 || ssbSCS==30)/*FR2*/ && kSSB>12)
    {
        cout<<"Invalid SSB: Coreset0Configuration doesnot exit"<<endl;
        abort();
    }
    
    RbSymOffConfig->prbOffsetIndx=rmsiConfig.prbOffsetIndx; 
    tmoConfig->tmoIndx=rmsiConfig.tmoIndx;
    
   //Assigning Coreset0 Rb's,Symbols, RBOffset from ssbPRB0 
    if(RbSymOffConfig->prbOffsetIndx!=PRB_OFFSET_IDX_UNINIT_)
    {
        
    if(ssbSCS==15 && rmsiSCS==15)
    {
     RbSymOffConfig->Pattern= SSB_CORESET_MUX_PATTERN_E(coreset0Tables.RbSymOffConfigTables(0)(RbSymOffConfig->prbOffsetIndx,1)); 
     RbSymOffConfig->nCORESET0RBs=coreset0Tables.RbSymOffConfigTables(0)(RbSymOffConfig->prbOffsetIndx,2);   
     RbSymOffConfig->nCORESET0Symbols=coreset0Tables.RbSymOffConfigTables(0)(RbSymOffConfig->prbOffsetIndx,3);     
     RbSymOffConfig->RBOffset=coreset0Tables.RbSymOffConfigTables(0)(RbSymOffConfig->prbOffsetIndx,4);   
    } 
    
    else if(ssbSCS==15 && rmsiSCS==30)
    {
        RbSymOffConfig->Pattern= SSB_CORESET_MUX_PATTERN_E(coreset0Tables.RbSymOffConfigTables(1)(RbSymOffConfig->prbOffsetIndx,1)); 
        RbSymOffConfig->nCORESET0RBs=coreset0Tables.RbSymOffConfigTables(1)(RbSymOffConfig->prbOffsetIndx,2);   
        RbSymOffConfig->nCORESET0Symbols=coreset0Tables.RbSymOffConfigTables(1)(RbSymOffConfig->prbOffsetIndx,3);     
        RbSymOffConfig->RBOffset=coreset0Tables.RbSymOffConfigTables(1)(RbSymOffConfig->prbOffsetIndx,4);   
    }    
    
    else if(ssbSCS==30 && rmsiSCS==15)
    {
        if     (ueTxBandwidth!=40)
        {
            RbSymOffConfig->Pattern= SSB_CORESET_MUX_PATTERN_E(coreset0Tables.RbSymOffConfigTables(2)(RbSymOffConfig->prbOffsetIndx,1)); 
            RbSymOffConfig->nCORESET0RBs=coreset0Tables.RbSymOffConfigTables(2)(RbSymOffConfig->prbOffsetIndx,2);   
            RbSymOffConfig->nCORESET0Symbols=coreset0Tables.RbSymOffConfigTables(2)(RbSymOffConfig->prbOffsetIndx,3);     
            RbSymOffConfig->RBOffset=coreset0Tables.RbSymOffConfigTables(2)(RbSymOffConfig->prbOffsetIndx,4);   
        }    
        
        else 
        {
            RbSymOffConfig->Pattern= SSB_CORESET_MUX_PATTERN_E(coreset0Tables.RbSymOffConfigTables(4)(RbSymOffConfig->prbOffsetIndx,1)); 
            RbSymOffConfig->nCORESET0RBs=coreset0Tables.RbSymOffConfigTables(4)(RbSymOffConfig->prbOffsetIndx,2);   
            RbSymOffConfig->nCORESET0Symbols=coreset0Tables.RbSymOffConfigTables(4)(RbSymOffConfig->prbOffsetIndx,3);     
            RbSymOffConfig->RBOffset=coreset0Tables.RbSymOffConfigTables(4)(RbSymOffConfig->prbOffsetIndx,4);   
        }    
    }
    
    else if(ssbSCS==30 && rmsiSCS==30)
    {
        if     (ueTxBandwidth!=40)
        {
            RbSymOffConfig->Pattern= SSB_CORESET_MUX_PATTERN_E(coreset0Tables.RbSymOffConfigTables(3)(RbSymOffConfig->prbOffsetIndx,1)); 
            RbSymOffConfig->nCORESET0RBs=coreset0Tables.RbSymOffConfigTables(3)(RbSymOffConfig->prbOffsetIndx,2);   
            RbSymOffConfig->nCORESET0Symbols=coreset0Tables.RbSymOffConfigTables(3)(RbSymOffConfig->prbOffsetIndx,3);     
            RbSymOffConfig->RBOffset=coreset0Tables.RbSymOffConfigTables(3)(RbSymOffConfig->prbOffsetIndx,4);   
        }
        
        else 
        {
            RbSymOffConfig->Pattern= SSB_CORESET_MUX_PATTERN_E(coreset0Tables.RbSymOffConfigTables(5)(RbSymOffConfig->prbOffsetIndx,1)); 
            RbSymOffConfig->nCORESET0RBs=coreset0Tables.RbSymOffConfigTables(5)(RbSymOffConfig->prbOffsetIndx,2);   
            RbSymOffConfig->nCORESET0Symbols=coreset0Tables.RbSymOffConfigTables(5)(RbSymOffConfig->prbOffsetIndx,3);     
            RbSymOffConfig->RBOffset=coreset0Tables.RbSymOffConfigTables(5)(RbSymOffConfig->prbOffsetIndx,4);   
        } 
    }
        
    else if(ssbSCS==120 && rmsiSCS==60)
    {   
        if(kSSB==0)
        {
            RbSymOffConfig->Pattern= SSB_CORESET_MUX_PATTERN_E(coreset0Tables.RbSymOffConfigTables(6)(RbSymOffConfig->prbOffsetIndx,1)); 
            RbSymOffConfig->nCORESET0RBs=coreset0Tables.RbSymOffConfigTables(6)(RbSymOffConfig->prbOffsetIndx,2);   
            RbSymOffConfig->nCORESET0Symbols=coreset0Tables.RbSymOffConfigTables(6)(RbSymOffConfig->prbOffsetIndx,3);     
            RbSymOffConfig->RBOffset=coreset0Tables.RbSymOffConfigTables(6)(RbSymOffConfig->prbOffsetIndx,4);   
        }
        
        else 
        {
            RbSymOffConfig->Pattern= SSB_CORESET_MUX_PATTERN_E(coreset0Tables.RbSymOffConfigTables(7)(RbSymOffConfig->prbOffsetIndx,1)); 
            RbSymOffConfig->nCORESET0RBs=coreset0Tables.RbSymOffConfigTables(7)(RbSymOffConfig->prbOffsetIndx,2);   
            RbSymOffConfig->nCORESET0Symbols=coreset0Tables.RbSymOffConfigTables(7)(RbSymOffConfig->prbOffsetIndx,3);     
            RbSymOffConfig->RBOffset=coreset0Tables.RbSymOffConfigTables(7)(RbSymOffConfig->prbOffsetIndx,4);   
        } 
    }
    
    else if(ssbSCS==120 && rmsiSCS==120)
    {
        if(kSSB==0)
        {
            RbSymOffConfig->Pattern= SSB_CORESET_MUX_PATTERN_E(coreset0Tables.RbSymOffConfigTables(8)(RbSymOffConfig->prbOffsetIndx,1)); 
            RbSymOffConfig->nCORESET0RBs=coreset0Tables.RbSymOffConfigTables(8)(RbSymOffConfig->prbOffsetIndx,2);   
            RbSymOffConfig->nCORESET0Symbols=coreset0Tables.RbSymOffConfigTables(8)(RbSymOffConfig->prbOffsetIndx,3);     
            RbSymOffConfig->RBOffset=coreset0Tables.RbSymOffConfigTables(8)(RbSymOffConfig->prbOffsetIndx,4);   
        }
        
        else 
        {
            RbSymOffConfig->Pattern= SSB_CORESET_MUX_PATTERN_E(coreset0Tables.RbSymOffConfigTables(9)(RbSymOffConfig->prbOffsetIndx,1)); 
            RbSymOffConfig->nCORESET0RBs=coreset0Tables.RbSymOffConfigTables(9)(RbSymOffConfig->prbOffsetIndx,2);   
            RbSymOffConfig->nCORESET0Symbols=coreset0Tables.RbSymOffConfigTables(9)(RbSymOffConfig->prbOffsetIndx,3);     
            RbSymOffConfig->RBOffset=coreset0Tables.RbSymOffConfigTables(9)(RbSymOffConfig->prbOffsetIndx,4);   
        }
    } 
    
    else if(ssbSCS==240 && rmsiSCS==60)
    {
        RbSymOffConfig->Pattern= SSB_CORESET_MUX_PATTERN_E(coreset0Tables.RbSymOffConfigTables(10)(RbSymOffConfig->prbOffsetIndx,1)); 
        RbSymOffConfig->nCORESET0RBs=coreset0Tables.RbSymOffConfigTables(10)(RbSymOffConfig->prbOffsetIndx,2);   
        RbSymOffConfig->nCORESET0Symbols=coreset0Tables.RbSymOffConfigTables(10)(RbSymOffConfig->prbOffsetIndx,3);     
        RbSymOffConfig->RBOffset=coreset0Tables.RbSymOffConfigTables(10)(RbSymOffConfig->prbOffsetIndx,4);   
    } 
    
    else if(ssbSCS==240 && rmsiSCS==120)
    {
        if(kSSB==0)
        {
            RbSymOffConfig->Pattern= SSB_CORESET_MUX_PATTERN_E(coreset0Tables.RbSymOffConfigTables(11)(RbSymOffConfig->prbOffsetIndx,1)); 
            RbSymOffConfig->nCORESET0RBs=coreset0Tables.RbSymOffConfigTables(11)(RbSymOffConfig->prbOffsetIndx,2);   
            RbSymOffConfig->nCORESET0Symbols=coreset0Tables.RbSymOffConfigTables(11)(RbSymOffConfig->prbOffsetIndx,3);     
            RbSymOffConfig->RBOffset=coreset0Tables.RbSymOffConfigTables(11)(RbSymOffConfig->prbOffsetIndx,4);   
        }
        
        else 
        {
            RbSymOffConfig->Pattern= SSB_CORESET_MUX_PATTERN_E(coreset0Tables.RbSymOffConfigTables(12)(RbSymOffConfig->prbOffsetIndx,1)); 
            RbSymOffConfig->nCORESET0RBs=coreset0Tables.RbSymOffConfigTables(12)(RbSymOffConfig->prbOffsetIndx,2);   
            RbSymOffConfig->nCORESET0Symbols=coreset0Tables.RbSymOffConfigTables(12)(RbSymOffConfig->prbOffsetIndx,3);     
            RbSymOffConfig->RBOffset=coreset0Tables.RbSymOffConfigTables(12)(RbSymOffConfig->prbOffsetIndx,4);   
        }
    } 
    
    else
    {
        cout<<"Incorrect ssbSCS or rmsiSCS setting"<<endl;
        abort();
    }
    }
    
//Now Assigning TimingAndMonitoringOccasions 
    int mu=log2(rmsiSCS/15);
    bool isnormalCP=1;// assuming not extended CP, later the variable had to be passed to this function
    int NslotsPerFrame=getSlotsPerFrame(mu,isnormalCP);//according to rmsi scs
    double y;
    int k;
    
    if(tmoConfig->tmoIndx!=TMO_IDX_UNINIT_)
    {
        if(RbSymOffConfig->Pattern==SSB_CORESET_MUX_PATTERN_1_)
        {
            if(ssbSCS==15 || ssbSCS==30)//implies FR1
            {
                tmoConfig->O=coreset0Tables.TMOConfigTables(0)(tmoConfig->tmoIndx,1);
                tmoConfig->N=coreset0Tables.TMOConfigTables(0)(tmoConfig->tmoIndx,2);
                tmoConfig->M=coreset0Tables.TMOConfigTables(0)(tmoConfig->tmoIndx,3);
                
                if(ssbIndx%2==0) tmoConfig->firstSymbolIndxOfCoreset0=coreset0Tables.TMOConfigTables(0)(tmoConfig->tmoIndx,4);
                else 
                {   
                    if(coreset0Tables.TMOConfigTables(1)(tmoConfig->tmoIndx,4)!=-10)
                        tmoConfig->firstSymbolIndxOfCoreset0=coreset0Tables.TMOConfigTables(1)(tmoConfig->tmoIndx,4);
                    else
                        tmoConfig->firstSymbolIndxOfCoreset0=RbSymOffConfig->nCORESET0Symbols;
                }
                
                y=tmoConfig->O*pow(2,mu)+floor_i(ssbIndx*tmoConfig->M);
                                
                if(mod(floor_i(y/NslotsPerFrame),2)==0) 
                {   
                    if(sfnSSB%2==0) tmoConfig->sfnC0=sfnSSB;//sfnC0 is even
                    else tmoConfig->sfnC0=sfnSSB+1;
                }
                else 
                {   
                    if(sfnSSB%2==0) tmoConfig->sfnC0=sfnSSB+1;//sfnC0 is Odd
                    else tmoConfig->sfnC0=sfnSSB;
                }
                
                tmoConfig->slotC0.set_length(2);
                tmoConfig->slotC0(0)=mod((int)y,NslotsPerFrame);
                tmoConfig->slotC0(1)=tmoConfig->slotC0(0)+1;
            }
            
            else if(ssbSCS==120 || ssbSCS==240)//implies FR2
            {
                tmoConfig->O=coreset0Tables.TMOConfigTables(0)(tmoConfig->tmoIndx,1);
                tmoConfig->N=coreset0Tables.TMOConfigTables(0)(tmoConfig->tmoIndx,2);
                tmoConfig->M=coreset0Tables.TMOConfigTables(0)(tmoConfig->tmoIndx,3);
                
                if(ssbIndx%2==0) tmoConfig->firstSymbolIndxOfCoreset0=coreset0Tables.TMOConfigTables(0)(tmoConfig->tmoIndx,4);
                else 
                {   
                    if(coreset0Tables.TMOConfigTables(1)(tmoConfig->tmoIndx,4)!=-10)
                    tmoConfig->firstSymbolIndxOfCoreset0=coreset0Tables.TMOConfigTables(1)(tmoConfig->tmoIndx,4);
                    else
                        tmoConfig->firstSymbolIndxOfCoreset0=RbSymOffConfig->nCORESET0Symbols;
                }
                
                y=tmoConfig->O*pow(2,mu)+floor_i(ssbIndx*tmoConfig->M);
                
                if(mod(floor_i(y/NslotsPerFrame),2)==0) 
                {   
                    if(sfnSSB%2==0) tmoConfig->sfnC0=sfnSSB;//sfnC0 is even
                    else tmoConfig->sfnC0=sfnSSB+1;
                }
                else 
                {   
                    if(sfnSSB%2==0) tmoConfig->sfnC0=sfnSSB+1;//sfnC0 is Odd
                    else tmoConfig->sfnC0=sfnSSB;
                }
                
                tmoConfig->slotC0.set_length(2);
                tmoConfig->slotC0(0)=mod((int)y,NslotsPerFrame);
                tmoConfig->slotC0(1)=tmoConfig->slotC0(0)+1;
            }
            
            else
            {
                cout<<"Incorrectly set ssbSCS "<<endl;
                abort();
            }
        }
        
        else if(RbSymOffConfig->Pattern==SSB_CORESET_MUX_PATTERN_2_)
        {
            if(ssbSCS==120 && rmsiSCS==60)
            { 
              if(tmoConfig->tmoIndx==0)
              {
               tmoConfig->sfnC0=sfnSSB;
               k=ssbIndx%4;
               tmoConfig->slotC0=slotSSB;
               ivec symbolMap="0,1,6,7";
               tmoConfig->firstSymbolIndxOfCoreset0=symbolMap(k);
              }
              else
              {
               cout<<"Accessed Reserved tmoIndices"<<endl; 
               abort();
              }
            }
            else if(ssbSCS==240 && rmsiSCS==120)
            {
                if(tmoConfig->tmoIndx==0)
                {
                    tmoConfig->sfnC0=sfnSSB;
                    k=ssbIndx%8;
                    if(k==4 || k==5) tmoConfig->slotC0=slotSSB-1;
                    else tmoConfig->slotC0=slotSSB;
                    ivec symbolMap="0,1,2,3,12,13,0,1";
                    tmoConfig->firstSymbolIndxOfCoreset0=symbolMap(k);
                }
                else
                {
                    cout<<"Accessed Reserved tmoIndices"<<endl; 
                    abort();
                }  
            }
            
            else
            {
                cout<<"Incorrectly set ssbSCS "<<endl;
                abort();
            }
        }
        
        else if(RbSymOffConfig->Pattern==SSB_CORESET_MUX_PATTERN_3_)
        {
            if(ssbSCS!=120 && rmsiSCS!=240)
            {
                cout<<"Incorrectly set ssbSCS "<<endl;
                abort(); 
            }
            
            tmoConfig->sfnC0=sfnSSB;
            k=ssbIndx%4;
            tmoConfig->slotC0=slotSSB;
            ivec symbolMap="4,8,2,6";
            tmoConfig->firstSymbolIndxOfCoreset0=symbolMap(k);
        }
        
        else
        {
            cout<<"INVALID SSB_CORESET MULTIPLEXING PATTERN"<<endl;
            abort();
        }
            
    }
        
}

int nextValidSSB(int kSSB, int RMSIPDCCHConfiguration, bool isFR1)
{
    if(isFR1)
    {
        
        switch (kSSB)
        {
            case 24 : return RMSIPDCCHConfiguration+1;
            case 25 : return RMSIPDCCHConfiguration+1+256;
            case 26 : return RMSIPDCCHConfiguration+1+2*256;
            case 27 : return -(RMSIPDCCHConfiguration+1);
            case 28 : return -(RMSIPDCCHConfiguration+1+256);
            case 29 : return -(RMSIPDCCHConfiguration+1+2*256);
            case 30 : cout<<"Reserved Configuration."<<endl;abort();
            default : cout<<"Invalid nextValidSSB configuration"<<endl;abort();
        }
    }
    else
    {
        switch (kSSB)
        {
            case 12 : return RMSIPDCCHConfiguration+1;
            case 13 : return -(RMSIPDCCHConfiguration+1);
            case 14 : cout<<"Reserved Configuration."<<endl;abort();
            default : cout<<"Invalid nextValidSSB configuration"<<endl;abort();
        }
    }
}

int getSlotsPerFrame(int mu,bool isnormalCP)
{
   ivec NslotsPerFrameMap="10,20,40,80,100";
   if(isnormalCP) return(NslotsPerFrameMap(mu));
   else return(40);
}
