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


#include "../../include/Terrain.h"



void load3DChannelTerrains(Terrain &terrain){
  
  Array<string> terrainNames="{ _TERRAIN_SCM3D_URBAN_MICRO_ , _TERRAIN_SCM3D_URBAN_MACRO_ }";
  terrain.addTerrain(terrainNames,&get3DChannelPathLoss);
}

NodeLinkInfo_S get3DChannelPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType, const Terrain * terr,NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2)
{
    string myTerrain=terr->getTerrain();
    if (myTerrain=="_TERRAIN_SCM3D_URBAN_MICRO_")
        return(channel3DUrbanMicro(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,txNodeType,rxNodeType,terr,nodeCategory1,nodeCategory2));
    else if (myTerrain=="_TERRAIN_SCM3D_URBAN_MACRO_")
        return(channel3DUrbanMacro(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,txNodeType,rxNodeType,terr,nodeCategory1,nodeCategory2));   
    else
    {
        cout<<"[both:] Undefined Terrain for 3D Channel Pathloss model...."<<endl;
        exit(1);
    }
}

NodeLinkInfo_S channel3DUrbanMicro(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int /*txNodeType*/, int rxNodeType, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2)
{
	NodeLinkInfo_S tempLinkInfo;
	int isUEIndoor=-1;
	isUEIndoor = nodeLoc1.isIndoor || nodeLoc2.isIndoor;
	double d2Dout;
	double d2Din;
	double d2D;
	double carrierFreqInGigaHz;

	
	if(isUEIndoor)
	{
	   d2Dout = findDistance(nodeLoc1,nodeLoc2);	
	   d2Din=25.0*randu(); 
	   d2D=d2Dout+d2Din;
	  
	}
	else//OutDoor
	{
	   d2D = findDistance(nodeLoc1,nodeLoc2);
	   d2Din=0.0;	  
	}
		
// 	double PL;
	double dBP;
	double d3D;
	double ueHeight=0.0;
	double bsHeight=0.0;
	
	double temp_LOS_PathLoss=0.0;
// 	double temp_NLOS_PathLoss=0.0;
	tempLinkInfo.pathLoss=0.0;
        carrierFreqInGigaHz=carrierFreqInHz/pow(10.0,9); 
	
	
	if(nodeCategory1 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
	{
	  bsHeight=nodeLoc1.z;
	  ueHeight=nodeLoc2.z;	  
	}
	else if(nodeCategory2 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
	{
	   bsHeight=nodeLoc2.z;
	   ueHeight=nodeLoc1.z;
	}
	else
	{
	  cout<<"[both:]Error..  Either nodeCategory1 or nodeCategory1 should be _NODECATEGORY_SERVERNODE_, nodeCategory1 = "<<(int)nodeCategory1<<" nodeCategory2 = "<<(int)nodeCategory2<<endl;
	  abort();
	}

	dBP = 4*(bsHeight-1.0)*(ueHeight-1.0)*carrierFreqInHz/lightSpeed;
	 
	d3D = sqrt(pow(d2D,2.0) + pow((bsHeight-ueHeight),2.0));
	tempLinkInfo.distance=d3D;
    
	if(!isUEIndoor)// UE is outdoor
	{
	      if(isLOS==-1)
		{
		      double PLos=std::min(18.0/d2D,1.0)*(1.0-exp(-d2D/36.0))+exp(-d2D/36.0);
		      tempLinkInfo.isLOS=(randu()<PLos);
		}
		else
		      tempLinkInfo.isLOS=isLOS;
		
	      if(tempLinkInfo.isLOS)
	      {

		
		    if((d2D>10.0)&&(d2D<dBP))
		    {
			tempLinkInfo.pathLoss = 22.0*log10(d3D) + 28.0 + 20.0*log10(carrierFreqInGigaHz);
			tempLinkInfo.shadowStdDev=3.0;
		    }
		  else if((d2D>dBP)&&(d2D<5000.0))
		    {
		      tempLinkInfo.pathLoss = 40.0*log10(d3D) + 28.0 + 20.0*log10(carrierFreqInGigaHz) - 9.0*log10(pow(dBP,2) + pow((bsHeight-ueHeight),2));
		      tempLinkInfo.shadowStdDev=3.0;
		    }
		  else
		  {
		    cout<<"Error.... LOS Path Loss model for 3DUMi is not defined for d2D is greater than 5000.0m"<<endl;
		    abort();		    
		  }
		 
	      }
	      else//NLOS
	      {
		//------------------------------------
		 if((d2D>10.0)&&(d2D<dBP))		    
			tempLinkInfo.pathLoss = 22.0*log10(d3D) + 28.0 + 20.0*log10(carrierFreqInGigaHz);			
		  else if((d2D>dBP)&&(d2D<5000.0))		    
		      tempLinkInfo.pathLoss = 40.0*log10(d3D) + 28.0 + 20.0*log10(carrierFreqInGigaHz) - 9.0*log10(pow(dBP,2) + pow((bsHeight-ueHeight),2));
		  else
		  {
		    cout<<"Error.... LOS Path Loss model for 3DUMi is not defined for d2D is greater than 5000.0m"<<endl;
		    abort();
		    
		  }
		  temp_LOS_PathLoss=tempLinkInfo.pathLoss;
		//------------------------------------------------
			
		if((d2D>10.0)&&(d2D<2000.0))
		{
		  double PL_temp = 36.7*log10(d3D) + 22.7 + 26.0*log10(carrierFreqInGigaHz) - 0.3*(ueHeight-1.5);
		  tempLinkInfo.pathLoss = max(temp_LOS_PathLoss,PL_temp);
		  tempLinkInfo.shadowStdDev=4.0;
		  
		}
		else
		{
		  cout<<"Error.... NLOS Path Loss model for 3DUMi is not defined for d2D is greater than 2000.0m"<<endl;
		  abort();	      
		}
		
	      }
	  
	}
	else // UE is indoor
	{
	  
	    if(isLOS==-1)
	      {
		    double PLos=std::min(18.0/d2Dout,1.0)*(1.0-exp(-d2Dout/36.0))+exp(-d2Dout/36.0);
		    tempLinkInfo.isLOS=(randu()<PLos);
	      }
	      else
		    tempLinkInfo.isLOS=isLOS;
	      
	      
	      double temp_PathLoss=0.0;
	      
	      dBP = 4*(bsHeight-1.0)*(ueHeight-1.0)*carrierFreqInHz/lightSpeed;
	      
	      d3D = sqrt(pow(d2Dout,2.0) + pow((bsHeight-ueHeight),2.0));
	      tempLinkInfo.distance=d3D;
	      
	       if(tempLinkInfo.isLOS)
	       {
		 if((d2D>10.0)&&(d2D<dBP))		    
			tempLinkInfo.pathLoss = 22.0*log10(d3D) + 28.0 + 20.0*log10(carrierFreqInGigaHz);			
		  else if((d2D>dBP)&&(d2D<5000.0))		    
		      tempLinkInfo.pathLoss = 40.0*log10(d3D) + 28.0 + 20.0*log10(carrierFreqInGigaHz) - 9.0*log10(pow(dBP,2) + pow((bsHeight-ueHeight),2));
		  else
		  {
		    cout<<"Error.... LOS Path Loss model for 3DUMi is not defined for d2D is greater than 5000.0m"<<endl;
		    abort();		    
		  }
		  temp_PathLoss=tempLinkInfo.pathLoss;
		 
	       }
	       else
	       {
		  if((d2D>10.0)&&(d2D<dBP))		    
			tempLinkInfo.pathLoss = 22.0*log10(d3D) + 28.0 + 20.0*log10(carrierFreqInGigaHz);			
		  else if((d2D>dBP)&&(d2D<5000.0))		    
		      tempLinkInfo.pathLoss = 40.0*log10(d3D) + 28.0 + 20.0*log10(carrierFreqInGigaHz) - 9.0*log10(pow(dBP,2) + pow((bsHeight-ueHeight),2));
		  else
		  {
		    cout<<"Error.... LOS Path Loss model for 3DUMi is not defined for d2D is greater than 5000.0m"<<endl;
		    abort();
		    
		  }
		  temp_LOS_PathLoss=tempLinkInfo.pathLoss;
		//------------------------------------------------
			
		if((d2D>10.0)&&(d2D<2000.0))
		{
		  double PL_temp = 36.7*log10(d3D) + 22.7 + 26.0*log10(carrierFreqInGigaHz) - 0.3*(ueHeight-1.5);
		  tempLinkInfo.pathLoss = max(temp_LOS_PathLoss,PL_temp);		  
		}
		else
		{
		  cout<<"Error.... NLOS Path Loss model for 3DUMi is not defined for d2D is greater than 2000.0m"<<endl;
		  abort();	      
		}
	       
	       temp_PathLoss=tempLinkInfo.pathLoss;
		 
	       }
		
	       //--------------------------------------------------------------
	      
	      if((d2D>10.0)&&(d2D<1000.0))
	      {
		double PL_tw = 20;
		double PL_in = d2Din/2.0;
		double PL_b = temp_PathLoss;//! Need to confirm
		tempLinkInfo.pathLoss = PL_b + PL_tw + PL_in;
		tempLinkInfo.shadowStdDev=7.0;
	      }
 	      else
 	      {
 		cout<<"Error.... Indoor Path Loss model for 3DUMi is not defined for d2D is greater than 1000.0m"<<endl;
 		abort();	      
 	      }
	} 
	
    return (tempLinkInfo);
}


NodeLinkInfo_S channel3DUrbanMacro(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2)
{
  
	NodeLinkInfo_S tempLinkInfo;
	
	int isUEIndoor=-1;
	isUEIndoor = nodeLoc1.isIndoor || nodeLoc2.isIndoor;
	double d2Dout;
	double d2Din;
	double d2D;
	double carrierFreqInGigaHz;
	
	
	if(isUEIndoor)
	{
	   d2Dout = findDistance(nodeLoc1,nodeLoc2);	
	   d2Din=25.0*randu();  
	   d2D=d2Dout+d2Din;
	  
	}
	else//OutDoor
	{
	   d2D = findDistance(nodeLoc1,nodeLoc2);	
	}
	
	
	double PL;	
	double ueHeight=0.0;
	double bsHeight=0.0;	
	double temp_LOS_PathLoss=0.0;
	double temp_NLOS_PathLoss=0.0;
	tempLinkInfo.pathLoss=0.0;
	double dBP;
	double d3D;
	
	carrierFreqInGigaHz=carrierFreqInHz/pow(10.0,9); 	
	
	
	if(nodeCategory1 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
	{
	  bsHeight=nodeLoc1.z;
	  ueHeight=nodeLoc2.z;
	  
	}
	else if(nodeCategory2 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
	{
	   bsHeight=nodeLoc2.z;
	   ueHeight=nodeLoc1.z;
	}
	else
	{
	  cout<<"[both:]Error..  Either nodeCategory1 or nodeCategory1 should be _NODECATEGORY_SERVERNODE_, nodeCategory1 = "<<(int)nodeCategory1<<" nodeCategory2 = "<<(int)nodeCategory2<<endl;
	  abort();
	}
	
	
	 d3D = sqrt(pow(d2D,2.0) + pow((bsHeight-ueHeight),2.0));
	 tempLinkInfo.distance=d3D;
	
    
	if(!isUEIndoor)// UE is outdoor
	{
	  
	    double g,C;
	    if(d2D>18)
	      g = (1.25*pow(10.0,-6))*pow(d2D,3)*exp(-1.0*(d2D/150.0));
	    else
	      g=0.0;
	    if(ueHeight<13.0)
	      C=0.0;
	    else
	      C = pow(((ueHeight-13.0)/10.0),1.5)*g;	
	    
	  
	  
	  if(isLOS==-1)
	    {
		  double PLos = std::min(18.0/d2D,1.0)*(1.0-exp((-1.0*d2D)/63.0)) + exp((-1.0*d2D)/63.0)*(1+C);
		  tempLinkInfo.isLOS=(randu()<PLos);
	    }
	    else
		  tempLinkInfo.isLOS=isLOS;
	    
	    
	    
	   if(tempLinkInfo.isLOS)
	   {
	     
		double effectiveHeight;	  
		if(randu() < 1.0/(1.0+C))
		  effectiveHeight=1.0;
		else
		{
		  vec temp_Vec=getVector(12,ueHeight-1.5,3);
		  vec temp_Vec_1=randPick(temp_Vec,1);
		  effectiveHeight=temp_Vec_1(0);
		}
	      
	      
	        double dBP = 4.0*(bsHeight-effectiveHeight)*(ueHeight-effectiveHeight)*carrierFreqInHz/lightSpeed;
	      
		  if((d2D>10.0)&&(d2D<dBP))
		  {
			tempLinkInfo.pathLoss = 22.0*log10(d3D) + 28.0 + 20.0*log10(carrierFreqInGigaHz);
			tempLinkInfo.shadowStdDev=4.0;
		  }
		  else if((d2D>dBP)&&(d2D<5000.0))
		  {
			
			tempLinkInfo.pathLoss = 40.0*log10(d3D) + 28.0 + 20.0*log10(carrierFreqInGigaHz) - 9.0*log10(pow(dBP,2) + pow((bsHeight-ueHeight),2));
			tempLinkInfo.shadowStdDev=4.0;
		  }
		  else
		  {
		    cout<<"Error.... LOS Path Loss model for 3DUMa is not defined for d2D is greater than 5000.0m"<<endl;
		    abort();
		    
		  }
	      
	   }
	   else//NLOS
	   {
		//------------------------------------------
		  double effectiveHeight;	  
		  if(randu() < 1.0/(1.0+C))
		    effectiveHeight=1.0;
		  else
		  {
		    vec temp_Vec=getVector(12,ueHeight-1.5,3);
		    vec temp_Vec_1=randPick(temp_Vec,1);
		    effectiveHeight=temp_Vec_1(0);
		  }
		
		
		  dBP = 4.0*(bsHeight-effectiveHeight)*(ueHeight-effectiveHeight)*carrierFreqInGigaHz*pow(10.0,9)/lightSpeed;
		
		  if((d2D>10.0)&&(d2D<dBP))		 
			  tempLinkInfo.pathLoss = 22.0*log10(d3D) + 28.0 + 20.0*log10(carrierFreqInGigaHz);
		    else if((d2D>dBP)&&(d2D<5000.0))		      
			  tempLinkInfo.pathLoss = 40.0*log10(d3D) + 28.0 + 20.0*log10(carrierFreqInGigaHz) - 9.0*log10(pow(dBP,2) + pow((bsHeight-ueHeight),2));		
		    else
		    {
		      cout<<"Error.... LOS Path Loss model for 3DUMa is not defined for d2D is greater than 5000.0m"<<endl;
		      abort();
		      
		    }
		temp_LOS_PathLoss=tempLinkInfo.pathLoss;
		//-------------------------------------------------------
	     
	     
		double W = 20.0;//street width
		double 	H = 20.0; //Avg. building height 
		if((d2D<5000.0) && (d2D>10.0))
		{
			double PL_temp = 161.04 - 7.1*log10(W) + 7.5*log10(H) - (24.37-3.7*pow(H/bsHeight,2))*log10(bsHeight) + (43.42-3.1*log10(bsHeight))*(log10(d3D)-3.0) + 20*log10(carrierFreqInGigaHz) - (3.2*pow(log10(17.625),2) - 4.97) - 0.6*(ueHeight - 1.5);
			tempLinkInfo.pathLoss = max(temp_LOS_PathLoss,PL_temp);
			
		}
		else
		{
		  
		  cout<<"Error....  NLOS Path Loss model for 3DUMa is not defined for d2D is greater than 5000.0m"<<endl;
		  abort();
		  
		}
		tempLinkInfo.shadowStdDev=6.0;
		temp_NLOS_PathLoss=tempLinkInfo.pathLoss;
	   }
	  
	}
	else // UE is indoor
	{
	      double temp_PathLoss=0.0;
	      
	      double g,C;
	      if(d2Dout>18)
		g = (1.25*pow(10.0,-6))*pow(d2Dout,3)*exp(-1.0*(d2Dout/150.0));
	      else
		g=0.0;
	      if(ueHeight<13.0)
		C=0.0;
	      else
		C = pow(((ueHeight-13.0)/10.0),1.5)*g;  
	      
	      
		double effectiveHeight;	  
		if(randu() < 1.0/(1.0+C))
		  effectiveHeight=1.0;
		else
		{
		  vec temp_Vec=getVector(12,ueHeight-1.5,3);
		  vec temp_Vec_1=randPick(temp_Vec,1);
		  effectiveHeight=temp_Vec_1(0);
		}
		
		
		dBP = 4.0*(bsHeight-effectiveHeight)*(ueHeight-effectiveHeight)*carrierFreqInHz/lightSpeed;
	      
		 d3D = sqrt(pow(d2D,2.0) + pow((bsHeight-ueHeight),2.0));
	         tempLinkInfo.distance=d3D;
		 
		 
	      
	      if(isLOS==-1)
		{
		      double PLos = std::min(18.0/d2Dout,1.0)*(1.0-exp((-1.0*d2Dout)/63.0)) + exp((-1.0*d2Dout)/63.0)*(1+C);
		      tempLinkInfo.isLOS=(randu()<PLos);
		}
		else
		      tempLinkInfo.isLOS=isLOS;
	    
	    
	    //--------------------------------------------------------------------------------------------------------
	    
	    
	        if(tempLinkInfo.isLOS)
	        {
		  if((d2D>10.0)&&(d2D<dBP))		  
			tempLinkInfo.pathLoss = 22.0*log10(d3D) + 28.0 + 20.0*log10(carrierFreqInGigaHz);		  
		  else if((d2D>dBP)&&(d2D<5000.0))			
			tempLinkInfo.pathLoss = 40.0*log10(d3D) + 28.0 + 20.0*log10(carrierFreqInGigaHz) - 9.0*log10(pow(dBP,2) + pow((bsHeight-ueHeight),2));
		  
		  else
		  {
		    cout<<"Error.... LOS Path Loss model for 3DUMa is not defined for d2D is greater than 5000.0m"<<endl;
		    abort();
		    
		  }
		  
		  temp_PathLoss=tempLinkInfo.pathLoss;
	    
	      }
	      else//NLOS
	      {
		//------------------------------------------
		  if((d2D>10.0)&&(d2D<dBP))		 
			  tempLinkInfo.pathLoss = 22.0*log10(d3D) + 28.0 + 20.0*log10(carrierFreqInGigaHz);
		    else if((d2D>dBP)&&(d2D<5000.0))		      
			  tempLinkInfo.pathLoss = 40.0*log10(d3D) + 28.0 + 20.0*log10(carrierFreqInGigaHz) - 9.0*log10(pow(dBP,2) + pow((bsHeight-ueHeight),2));		
		    else
		    {
		      cout<<"Error.... LOS Path Loss model for 3DUMa is not defined for d2D is greater than 5000.0m"<<endl;
		      abort();
		      
		    }
		temp_LOS_PathLoss=tempLinkInfo.pathLoss;
		//-------------------------------------------------------
		
		
		    double W = 20.0;//street width
		    double H = 20.0; //Avg. building height 
		    if((d2D<5000.0) && (d2D>10.0))
		    {
			    double PL_temp = 161.04 - 7.1*log10(W) + 7.5*log10(H) - (24.37-3.7*pow(H/bsHeight,2))*log10(bsHeight) + (43.42-3.1*log10(bsHeight))*(log10(d3D)-3.0) + 20*log10(carrierFreqInGigaHz) - (3.2*pow(log10(17.625),2) - 4.97) - 0.6*(ueHeight - 1.5);
			    tempLinkInfo.pathLoss = max(temp_LOS_PathLoss,PL_temp);
			    
			    
		    }
		    else
		    {
		      
		      cout<<"Error....  NLOS Path Loss model for 3DUMa is not defined for d2D is greater than 5000.0m"<<endl;
		      abort();
		      
		    }
		    
		    temp_PathLoss=tempLinkInfo.pathLoss;
	   }
	    //-------------------------------------------------
	    
	    
	    //if((d2D>10.0)&&(d2D<1000.0))
	    {
	      double PL_tw = 20.0;
	      double PL_in = d2Din/2.0;	     
	      double PL_b = temp_PathLoss;
	      tempLinkInfo.pathLoss = PL_b + PL_tw + PL_in;
	      tempLinkInfo.shadowStdDev=7.0;
	    }
// 	    else
// 	    {
// 	      cout<<"Error.... Indoor Path Loss model for 3DUMa is not defined for d2D is greater than 1000.0m"<<endl;
// 	      abort();	      
// 	    }
	}
	
	
    return (tempLinkInfo);
}
