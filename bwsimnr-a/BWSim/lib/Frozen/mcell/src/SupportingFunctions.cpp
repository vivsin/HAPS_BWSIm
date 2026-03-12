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

#include "../include/SupportingFunctions.h"
#include "../include/SoS.h"
#include "../include/SCMLink.h"

string DropMethod_Str[]={
  "_DROPMETHOD_RANDOM_",
  "_DROPMETHOD_HEXAGONAL_",
  "_DROPMETHOD_RANDOMHEX_",
  "_DROPMETHOD_INBUILDING_"
};

string TxRxMode_Str[]={
  "_TXRXMODE_TX_",
  "_TXRXMODE_RX_"
};

string AntennaType_Str[]={
  "_ANTENNA_TYPE_OMNI_",        
  "_ANTENNA_TYPE_PARABOLIC_",
  "_ANTENNA_TYPE_CUSTOM_",
  "_ANTENNA_TYPE_QUASI_ISOTROPIC_",
  "_ANTENNA_TYPE_CIRCULAR_",
  "_ANTENNA_TYPE_HAPS_"
};

string CellLayout_Str[]={
  "_CELLLAYOUT_HEXAGONAL_",
  "_CELLLAYOUT_RANDOM_"
};

string ShadowType_Str[]={
  "_SHADOWTYPE_CORRELATED_",
  "_SHADOWTYPE_UNCORRELATED_"
};


bool isInsideHexCell(double x, double y, Location_S hexCellLoc, double cellISD, double minDistFromCenter){
  
  x-=hexCellLoc.x;
  y-=hexCellLoc.y;
  
  bool toReturn=true;
  
  if(sqrt(sqr(x)+sqr(y))<minDistFromCenter) toReturn=false;
  if(abs(x)>cellISD/2.0) toReturn=false;
  if(abs(y)>(cellISD-abs(x))/sqrt(3)) toReturn=false;	
  
  return(toReturn);
}
//NOTE: This Function gives the values of radius and angle of each hexagonal cell centre in X-Y plane.
//The X-axis is taken as one of the line along cellISD. 
//The distance of each cell is computed using sqrt(m^2+n^2+m*n)times cellISD. 
void generateRadiusAndAngleForHexCellCenterLocation(vec& radius,vec& angle,int nCell,double cellRadius)
{
    int nTiers=0;
    double cellISD = sqrt(3)*cellRadius;
    //double cellISD = cellRadius;
    //cout<<"cellISD:"<<cellISD <<endl;
    while(nCell-1>0)
    {
        nTiers++;
        nCell -= 6*nTiers;
        //cout << "nCell" << nCell<<endl;
    }
    if((nCell-1) != 0)
    {
        cout<<"Improper nCell provided..."<<endl;abort();
    }
    Array<vec> r,theta;
    
    theta.set_length(nTiers);r.set_length(nTiers);
    
    for(int tier_cnt=1;tier_cnt<=nTiers;tier_cnt++)
    {
        int nCellsInTier = 6*tier_cnt;//No.of cells present in that tier. 
        //cout << "tier_cnt and nCellsInTier : "<<tier_cnt <<"," <<nCellsInTier<<endl;
        double diffAngle = (double)60.0/(tier_cnt);
        //cout << "tier_cnt and diffAngle : "<<tier_cnt <<"," <<diffAngle<<endl;
        theta(tier_cnt-1) = diffAngle*getIntegers(0,nCellsInTier-1);
        //cout << "tier_cnt and theta : "<<tier_cnt <<"," <<theta<<endl;
        r(tier_cnt-1).set_length(nCellsInTier);
        vec radius(tier_cnt);
        for(int cnt=0;cnt<tier_cnt;cnt++)
        {
            radius(cnt) = sqrt(sqr(cnt) + sqr(tier_cnt-cnt) - 2*cnt*(tier_cnt-cnt)*cos(deg_to_rad(120.0)))*cellISD;
            //cout << "tier_cnt and radius : "<<tier_cnt <<"," <<radius<<endl;
        }
        for(int cell_cnt=0;cell_cnt<nCellsInTier;cell_cnt++)
        {
            r(tier_cnt-1)(cell_cnt) = radius(cell_cnt%tier_cnt);
            theta(tier_cnt-1)(cell_cnt) = 360.0 - theta(tier_cnt-1)(cell_cnt);
            //cout << "r and theta : "<<r <<"," <<theta<<endl;
        }
    }
    radius = to_vec(0);angle = to_vec(0); //First cell Coordinate is (0,0).
    append(radius,joinArrayToVector(r));
    append(angle,joinArrayToVector(theta));
    //cout << "radius and angle : "<<radius <<"," <<angle<<endl;
}
Array <Location_S> generateHexCellCenterLocation(int nSec,double cellRadius)
{
  int nCell;
  double sqrtThree=1.732;
  if(nSec==1 || nSec==7 || nSec==19)
	nCell=nSec;
  else if(nSec==3)
      nCell=1;
  else if(nSec==21 || nSec==57)
	nCell=nSec/3;
  else if(nSec==42 || nSec==114)
      nCell=nSec/6;
  else{
   cout<<"[both:] unsupported number of sectors nSec "<<nSec<<" aborting ...."<<endl;abort(); 
  }
  
  Array <Location_S> BS_loc;
  BS_loc.set_size(nCell);
  
  if(nCell==1)
  {
	BS_loc(0).x = 0.0; BS_loc(0).y=0.0;
  }
  if(nCell==7)
  {
	
	BS_loc(0).x = 0.0;						BS_loc(0).y = 0.0;
	BS_loc(1).x = sqrtThree*cellRadius;	BS_loc(1).y = 0.0;
	BS_loc(2).x = sqrtThree*cellRadius/2.0;	BS_loc(2).y = -3.0*cellRadius/2.0;
	BS_loc(3).x = -sqrtThree*cellRadius/2.0;	BS_loc(3).y = -3.0*cellRadius/2.0;
	BS_loc(4).x = -sqrtThree*cellRadius;	BS_loc(4).y = 0;
	BS_loc(5).x = -sqrtThree*cellRadius/2.0;	BS_loc(5).y = 3.0*cellRadius/2.0;
	BS_loc(6).x = sqrtThree*cellRadius/2.0;	BS_loc(6).y = 3.0*cellRadius/2.0;
  }
  if(nCell==19)
  {	  
	
	BS_loc(0).x = 0.0;							BS_loc(0).y = 0.0;	
	BS_loc(1).x = sqrtThree*cellRadius;		BS_loc(1).y = 0.0;
	BS_loc(2).x = sqrtThree*cellRadius/2.0;		BS_loc(2).y = -3.0*cellRadius/2.0;
	BS_loc(3).x = -sqrtThree*cellRadius/2.0;		BS_loc(3).y = -3.0*cellRadius/2.0;
	BS_loc(4).x = -sqrtThree*cellRadius;		BS_loc(4).y = 0;
	BS_loc(5).x = -sqrtThree*cellRadius/2.0;		BS_loc(5).y = 3.0*cellRadius/2.0;
	BS_loc(6).x = sqrtThree*cellRadius/2.0;		BS_loc(6).y = 3.0*cellRadius/2.0;
	BS_loc(7).x = 2.0*sqrtThree*cellRadius;		BS_loc(7).y = 0;
	BS_loc(8).x = 3.0*sqrtThree*cellRadius/2.0;	BS_loc(8).y = -3.0*cellRadius/2.0;
	BS_loc(9).x = sqrtThree*cellRadius;		BS_loc(9).y = -3.0*cellRadius;
	BS_loc(10).x = 0.0;						BS_loc(10).y = -3.0*cellRadius;
	BS_loc(11).x = -sqrtThree*cellRadius;		BS_loc(11).y = -3.0*cellRadius;
	BS_loc(12).x = -3.0*sqrtThree*cellRadius/2.0;  BS_loc(12).y = -3.0*cellRadius/2.0;
	BS_loc(13).x = -2.0*sqrtThree*cellRadius;	BS_loc(13).y = 0;
	BS_loc(14).x = -3.0*sqrtThree*cellRadius/2.0;  BS_loc(14).y = 3.0*cellRadius/2.0;
	BS_loc(15).x = -sqrtThree*cellRadius;		BS_loc(15).y = 3.0*cellRadius;
	BS_loc(16).x = 0.0;						BS_loc(16).y = 3.0*cellRadius;
	BS_loc(17).x = sqrtThree*cellRadius;		BS_loc(17).y = 3.0*cellRadius;
	BS_loc(18).x = 3.0*sqrtThree*cellRadius/2.0;	BS_loc(18).y = 3.0*cellRadius/2.0;
  }
  return(BS_loc);
}
double getNorm(mat u)
{
    return sqrt((u.T()*u)(0,0));
}

//Returns unit vector perpendicular to a and b and in the direction of axb.
mat getCrossProductUnitVector(mat a,mat b)
{
    mat u(3,1);
    u(0,0) = a(1,0)*b(2,0) - b(1,0)*a(2,0);
    u(1,0) = a(2,0)*b(0,0) - b(2,0)*a(0,0);
    u(2,0) = a(0,0)*b(1,0) - b(0,0)*a(1,0);
    double norm = getNorm(u);
    return u/norm;
}

//Computes the angle between two vectors. 
double getAngle(mat u1,mat u2)
{
    double cosTheta = (u1.T()*u2)(0,0)/(getNorm(u1)*getNorm(u2));
    double theta = (cosTheta>1.0 && cosTheta<1.00000001) ? 0.0 : (cosTheta<-1.0 && cosTheta>-1.00000001) ? pi: acos(cosTheta);
    return theta;
}

//Returns the 3D rotation matrix to rotate a plane.
//theta is the angle between perpendiculars of the given plane and rotated plane.
//crossProductUnitVector is the unit vector along the cross product of perpendiculars of the given plane and rotated plane.
mat get3DPlaneRotationMatrix(double theta,mat crossProductUnitVector)
{
    if(theta==0.0)
        return eye(3);
    double c = cos(theta),s=abs(sin(theta)), C = 1-c;
    double x = crossProductUnitVector(0,0),y = crossProductUnitVector(1,0),z = crossProductUnitVector(2,0);
    mat matrix1 = crossProductUnitVector*crossProductUnitVector.T()*C;
    mat matrix2(3,3);
    matrix2(0,0) = c; matrix2(0,1) = -z*s; matrix2(0,2) = y*s;
    matrix2(1,0) = z*s; matrix2(1,1) = c; matrix2(1,2) = -x*s;
    matrix2(2,0) = -y*s; matrix2(2,1) = x*s; matrix2(2,2) = c;
    return matrix1+matrix2;
}

//Returns the 3D rotation matrix which rotates the plane perpendicular to u1 to the plane perpendicular to u2.  
mat get3DPlaneRotationMatrix(mat u1,mat u2)
{
    return get3DPlaneRotationMatrix(getAngle(u1,u2),getCrossProductUnitVector(u1,u2));
}

//In this method, the hexagonal cell locations are generated in 2D with centre Location as (0,0,re)
//Later the coordinates of centre are modified to add the effect of earth's curvature.
//Later each cell centre is rotated using a matrix that shifts the centre cell location to given centre location.
Array<Location_S> generateHexCellCenterLocationIn3D(int nCell,double cellRadius,Location_S centreLoc)
{
    double re=(double)earthRadius;
    vec r,phi;
    generateRadiusAndAngleForHexCellCenterLocation(r,phi,nCell,cellRadius);
    phi = phi*pi/180.0;
    //cout << "phi:"<< phi << ",r: " << r << "cellRadius"<< cellRadius <<endl;
    mat planeRotationMatrix = get3DPlaneRotationMatrix(Location_S(0,0,re).getAsMatrix(3),centreLoc.getAsMatrix(3));
    
    Array<Location_S> loc;loc.set_length(nCell);
    for(int cnt=0;cnt<nCell;cnt++)
    {
        double theta = r(cnt)/re;  //theta
        //cout<<"hex cell loc theta :"<<theta<<endl;
        double z_ = re*cos(theta);
        double d_ = re*sin(theta);
        //cout<<"hex cell loc z_ and d_ :"<<z_<<"  "<<d_<<endl;
        loc(cnt).x = d_ * cos(phi(cnt)); //Modified location due to curvature of earth. 
        //loc(cnt).x = 0;
        //cout << "loc(cnt).x : " << loc(cnt).x <<endl;
        loc(cnt).y = d_ * sin(phi(cnt));
        //loc(cnt).y = 0;
        //cout << "loc(cnt).y : " << loc(cnt).y <<endl;
        loc(cnt).z = z_;
        //loc(cnt).z = 0;
//         double theta = asin(r(cnt)/re);
//         loc(cnt).x = r(cnt)*cos(phi(cnt));
//         loc(cnt).y = r(cnt)*sin(phi(cnt));
//         loc(cnt).z = re*cos(theta);
        //cout<<"loc x,y,x : "<<loc(cnt)<<endl;
        loc(cnt).setLoc(planeRotationMatrix*loc(cnt).getAsMatrix(3));
        //cout << "loc : " << abs(re-getNorm(loc(cnt).getAsMatrix(3)) )<< endl;
        if(abs(re-getNorm(loc(cnt).getAsMatrix(3)))>0.0001)
        {
            cout<<planeRotationMatrix<<endl;
            cout<<loc(cnt)<<" "<<getNorm(loc(cnt).getAsMatrix(3))<<endl;
            cout<<"The given point is not on earth"<<endl;abort();
        }
    }
    //cout<<"hex cell loc :"<<loc<<endl;
    return loc; 
}

//NOTE: This Method computes random locations in hex cell using circular coordinates. 
//The x-axis assumed to be the line joining the centre to centre of a side.
//The hexagon is divided into 6 triangular sectors and each sector is divided to 2 sub-sectors.
//The max radius depends on the angle subtended at origin with x-axis like for 30 degrees it is cellRadius and 0 degrees it is 0.5*sqrt(3)*cellRadius. 
//The adjacent subSectors in a sector are like mirror images, hence angle is mapped as 60-theta in order to have same radius.  
Array <Location_S>  generateUniformDropLocationInHexCell(double cellRadius,double minDistFromCenter,int locationCount)
{
    Array<Location_S> loc(locationCount);
    for(int loc_cnt=0;loc_cnt<locationCount;loc_cnt++)
    {
        double theta = randu()*30.0; //angle subtended by point at origin wrt to x axis
        double maxRadius = 0.5*sqrt(3)*cellRadius/cos(deg_to_rad(theta)); 
        double radius = minDistFromCenter + (maxRadius - minDistFromCenter)*randu();
        int subSecNo = randi(0,1),secNo = randi(0,5);
        //cout << "locationCount, theta, maxRadius and radius "<< locationCount << "," << theta << ","<< maxRadius <<"," << radius <<endl;
        theta = 60.0*secNo + ((subSecNo==0) ? theta : 60 - theta);
        //cout << "theta, maxRadius and radius "<<theta << ","<< maxRadius <<"," << radius <<endl;
        loc(loc_cnt).x = radius*cos(deg_to_rad(theta));//-5.81174e+06;//
        loc(loc_cnt).y = radius*sin(deg_to_rad(theta));// -2.39042e+06;//
        loc(loc_cnt).z = 0;
        //cout << "loc1,Maxradius and Radius: " << loc << "," <<maxRadius << "," << radius <<endl;
    }
    return loc;
}

//In this method, the locations are generated in 2D assuming the centre is (0,0,re).
//Later the effect of curvature of earth is applied.
//Later the rotation is applied such that the centre is mapped to given cellLoc. 
Array <Location_S>  generateUniformDropLocationInHexCell3D(Location_S cellLoc,double cellRadius,double minDistFromCenter,double height,int locationCount)
{
    double re = (double) earthRadius;
    Array<Location_S> tempLoc = generateUniformDropLocationInHexCell(cellRadius,minDistFromCenter,locationCount);
    //cout << "cellLoc : "<< cellLoc << ","<<cellRadius <<endl;
    mat planeRotationMatrix = get3DPlaneRotationMatrix(Location_S(0,0,re).getAsMatrix(3),cellLoc.getAsMatrix(3));
    
    Array<Location_S> loc;loc.set_length(locationCount);
    for(int cnt=0;cnt<locationCount;cnt++)
    {
        double theta = findDistance(Location_S(0,0,re),tempLoc(cnt))/re;  //theta
        double phi = atan2(tempLoc(cnt).y,tempLoc(cnt).x);
        double z_ = re*cos(theta);
        double d_ = re*sin(theta);
        loc(cnt).x = d_ * cos(phi); // -5.81174e+06; -2.39042e+06;//// Modified location due to curvature of earth. 
        loc(cnt).y = d_ * sin(phi); 
        loc(cnt).z =z_; //-1.04846e+06;//
        //cout<<"location of drop initial"<<loc<<endl;
        		
        loc(cnt).setLoc(planeRotationMatrix*loc(cnt).getAsMatrix(3));
        //cout << "loc: " << loc <<endl;
        //cout<<getNorm(loc(cnt).getAsMatrix(3))<<endl;
        //loc(cnt).x =  -5.81168e+06; //-5.81174e+06; //// Modified location due to curvature of earth. 
        //loc(cnt).y = 	-2.39789e+06;//-2.39042e+06; 
        //loc(cnt).z = -1.03156e+06; //-1.04846e+06;//,,	
        //loc(cnt).setLoc(loc(cnt).getAsMatrix(3));
        //cout << "loc: " << loc <<endl;
        //cout<<getNorm(loc(cnt).getAsMatrix(3))<<endl;
        if(isnan(loc(cnt).z))
        {
            cout<<planeRotationMatrix<<endl;
            cout<<theta<<" "<<phi<<endl;
            cout<<loc(cnt)<<endl;abort();
        }
        if(abs(re-getNorm(loc(cnt).getAsMatrix(3)))>0.0001)
        {
            cout<<getNorm(loc(cnt).getAsMatrix(3))<<endl;
            cout<<"The given point is not on earth..."<<endl;
            abort();
        }
        if(height>0)
        {
            mat newLoc = loc(cnt).getAsMatrix(3)*(1.0+height/re);
            loc(cnt).setLoc(newLoc);
            //cout << "height" << newLoc <<endl;
        }
        //cout<<"location of drop"<<loc<<endl;
    }
    return loc;
}

Array<Neighbour_S> getWrapAroundNeighbours3D(int cellCnt, int cellID, Location_S cellLocation, double cellRadius)
{
    ivec neighbours(cellCnt-1);
    
    switch(cellCnt)
    {
        case 1:
            break;
        case 7: 
            switch(cellID)
            {
                case 0: neighbours="1 2 3 4 5 6"; break;
                case 1: neighbours="3 5 2 0 6 4"; break;
                case 2: neighbours="5 4 6 3 0 1"; break;
                case 3: neighbours="2 6 5 1 4 0"; break;
                case 4: neighbours="0 3 1 6 2 5"; break;
                case 5: neighbours="6 0 4 2 1 3"; break;
                case 6: neighbours="4 1 0 5 3 2"; break;
            }
            break;
        case 19:
            switch(cellID)
            {
                case 0: neighbours="1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18"; break;
                case 1: neighbours="7 8 2 0 6 18 15 14 13 9 10 3 4 5 16 17 12 11"; break;
                case 2: neighbours="8 9 10 3 0 1 14 13 17 16 15 11 12 4 5 6 18 7"; break;
                case 3: neighbours="2 10 11 12 4 0 8 9 16 15 7 18 17 13 14 5 6 1"; break;
                case 4: neighbours="0 3 12 13 14 5 1 2 10 11 18 17 9 8 7 15 16 6"; break;
                case 5: neighbours="6 0 4 14 15 16 18 1 2 3 12 13 8 7 11 10 9 17"; break;
                case 6: neighbours="18 1 0 5 16 17 11 7 8 2 3 4 14 15 10 9 13 12"; break;
                case 7: neighbours="15 14 8 1 18 11 16 5 4 13 9 2 0 6 17 12 3 10"; break;
                case 8: neighbours="14 5 13 9 2 1 7 15 4 12 17 16 10 3 0 6 18 11"; break;
                case 9: neighbours="13 17 16 10 2 8 4 12 18 6 5 15 11 3 0 1 7 14"; break;
                case 10: neighbours="9 16 15 11 3 2 13 17 6 5 14 7 18 12 4 0 1 8"; break;
                case 11: neighbours="10 15 7 18 12 3 9 16 5 14 8 1 6 17 13 4 0 2"; break;
                case 12: neighbours="3 11 18 17 13 4 2 10 15 7 1 6 16 9 8 14 5 0"; break;
                case 13: neighbours="4 12 17 9 8 14 0 3 11 18 6 16 10 2 1 7 15 5"; break;
                case 14: neighbours="5 4 13 8 7 15 6 0 3 12 17 9 2 1 18 11 10 16"; break;
                case 15: neighbours="16 5 14 7 11 10 17 6 0 4 13 8 1 18 12 3 2 9"; break;
                case 16: neighbours="17 6 5 15 10 9 12 18 1 0 4 14 7 11 3 2 8 13"; break;
                case 17: neighbours="12 18 6 16 9 13 3 11 7 1 0 5 15 10 2 8 14 4"; break;
                case 18: neighbours="11 7 1 6 17 12 10 15 14 8 2 0 5 16 9 13 4 3"; break;
            }
            break;
        default:
            cout<<"Invalid Cell Count"<<endl;abort();
            break;
    }
    
    Array<Location_S> loc = generateHexCellCenterLocationIn3D(cellCnt,cellRadius,cellLocation);
    
    Array<Neighbour_S> neighbourInfo;
    neighbourInfo.set_size(cellCnt-1);
    
    for(int neighbour_cnt=0;neighbour_cnt<cellCnt-1;neighbour_cnt++)
    {
        neighbourInfo(neighbour_cnt).cellID=neighbours(neighbour_cnt);
        neighbourInfo(neighbour_cnt).cellXYLocation = loc(neighbour_cnt+1); //Skipping the first location as it is the current cell location 
    }
    return(neighbourInfo);
}

Location_S WrapMyLocation3D(Location_S wrapTxCellLoc, Location_S txCellLoc, Location_S txLoc)
{
    double re = (double) earthRadius;
    vec angle = findAngle(Location_S(0,0,0),txLoc);
    double theta = (-txCellLoc.x+wrapTxCellLoc.x)/re;
    double phi = (-txCellLoc.y+wrapTxCellLoc.y)/re;
    mat loc = getCartesianVector(deg_to_rad(angle(0))+theta,deg_to_rad(angle(1))-phi,1,re);
    txLoc.setLoc(loc);
    return txLoc;
}
Array <Location_S>  generateUniformDropLocationInHexCell(Location_S cellloc,double cellRadius,double minDistFromCenter,int locationCount)
{
  Array<Location_S> locations(locationCount);
  for(int loc_cnt=0;loc_cnt<locationCount;loc_cnt++)
  {
	int areaNo,subAreaNo;
	double rnd,x,y;
	double theta,rrand=0.0;
	int secid;
	Location_S loc;
	secid=randi(0,1000)%3;

	rnd = randu();
	areaNo = (int )floor(rnd*2)+2*(secid);
	rnd = randu();
    subAreaNo = (int) (floor(rnd*2));

	do
	{		
	  rnd = randu();
	  x = sqrt(3)*0.5*rnd*cellRadius;
	  rnd = randu();
	  y = 0.5*rnd*cellRadius;
	  theta = atan(y/x);
	  if (theta>(pi/6))
	  {
		x = sqrt(3)*0.5*cellRadius-x;
		y = 0.5*cellRadius-y;
		theta = atan(y/x);
	  }
	  rrand = sqrt(x*x+y*y);
	}
	while(rrand<minDistFromCenter);
	
	if(subAreaNo==1)
		  theta=-theta+pi/3;
	theta = theta + ((double)areaNo)*pi/3;
	if (theta>2*pi)
		theta = theta-2*pi;

	loc.x = cellloc.x+rrand*cos(theta);
	loc.y = cellloc.y+rrand*sin(theta);
	
	locations(loc_cnt)=loc;
  }
  return(locations);
}

Array <Location_S>  generateUniformDropLocationInHexCell(Location_S cellloc,int sectorsPerCell,int secid,double cellRadius,double minDistFromCenter,int locationCount)
{
  Array<Location_S> locations(locationCount);
  
  for(int loc_cnt=0;loc_cnt<locationCount;loc_cnt++)
  {
	int areaNo,subAreaNo;
	double rnd,x,y;
	double theta,rrand=0.0;
	Location_S loc;
		
	rnd = randu();
	if(sectorsPerCell==3)
	  areaNo = (int )floor(rnd*2)+2*(secid);
	else if(sectorsPerCell==6)
	  areaNo = secid;
	else
	  {cout<<" [both:] Unsupported sectorsPerCell to generateUniformDropLocationInHexCell()..."<<endl;abort();}
	  
	rnd = randu();
	subAreaNo = (int) (floor(rnd*2));
	
	do
	{		
	  rnd = randu();
	  x = sqrt(3)*0.5*rnd*cellRadius;
	  rnd = randu();
	  y = 0.5*rnd*cellRadius;
	  theta = atan(y/x);
	  if (theta>(pi/6))
	  {
		x = sqrt(3)*0.5*cellRadius-x;
		y = 0.5*cellRadius-y;
		theta = atan(y/x);
	  }
	  rrand = sqrt(x*x+y*y);
	}
	while(rrand<minDistFromCenter);
	
	if(subAreaNo==1)
	  theta=-theta+pi/3;
	  theta = theta + ((double)areaNo)*pi/3;
	  if (theta>2*pi)
		theta = theta-2*pi;
	  
	  loc.x = cellloc.x+rrand*cos(theta);
	loc.y = cellloc.y+rrand*sin(theta);
	
	locations(loc_cnt)=loc;
  }
  return(locations);
}

Array <Location_S> generateUniformDropLocation(Area_S cvrgArea, int locationCount){
  
  Array<Location_S> locations(locationCount);
  Location_S loc;
  
  for(int loc_cnt=0;loc_cnt<locationCount;loc_cnt++)
  {
	loc.x=(randu()-0.5)*cvrgArea.xLen;
	loc.y=(randu()-0.5)*cvrgArea.yLen;
	locations(loc_cnt)=loc;
  }
  return(locations);
}
Array <Location_S> generateEquiDistantLocationInRect(Area_S cvrgArea, int locationCount, double xminDist, double yminDist){
    
    Array<Location_S> locations(locationCount);
    Location_S loc;
    
    if (yminDist==-1) yminDist=xminDist; 
    
    int xNodes=floor_i(cvrgArea.xLen/xminDist);
    int yNodes=floor_i(cvrgArea.yLen/yminDist);

    if((xNodes*yNodes) !=locationCount)
    {
        cout<<"xNodes*yNodes:"<<xNodes<<" , "<<yNodes<<" , "<<locationCount<<endl;
        cout<<"Error.. locationCount mismatch for minDist provided."<<endl;
        abort();
    }
    
    double xEdgeDist=(cvrgArea.xLen-(xNodes-1)*xminDist)/2.0;
    double yEdgeDist=(cvrgArea.yLen-(yNodes-1)*yminDist)/2.0;
    for(int yNode_cnt=0,loc_cnt=0;yNode_cnt<yNodes;yNode_cnt++)
    for(int xNode_cnt=0;xNode_cnt<xNodes;xNode_cnt++,loc_cnt++)
    {
        loc.x=xEdgeDist-0.5*cvrgArea.xLen+xminDist*xNode_cnt;
        loc.y=yEdgeDist-0.5*cvrgArea.yLen+yminDist*yNode_cnt;
        locations(loc_cnt)=loc;
    }
    return(locations);
}

Array <Location_S> generateIndoorLoc(Array<Location_S>& cellLoc,Area_S coverageArea,double minDist,int locationCount)
{
    Array<Location_S> locations(locationCount);
    Location_S loc;
    vec distance=to_vec(0);
    
    for(int loc_cnt=0;loc_cnt<locationCount;loc_cnt++)
    {
        do
        {
            loc.x=(randu()-0.5)*coverageArea.xLen;
            loc.y=(randu()-0.5)*coverageArea.yLen;
            if(minDist>0)
            {
                distance.set_length(cellLoc.length());
                for(int i=0;i<cellLoc.length();i++)
                {
                    distance(i) = findDistance(loc,cellLoc(i));
                }
            }
        }
        while(min(distance)<minDist);
        locations(loc_cnt)=loc;
    }
    return(locations);
}

Array <Location_S>  generateUniformDropLocationInCircle(Location_S circleCenter,double circleRadius,double minDistFromCenter,int locationCount){
  
  if(minDistFromCenter>=circleRadius){cout<<"[both:] Invalid minDistFromCenter for the given circleRadius in generateUniformDropLocationInCircle()..."<<endl;abort();}
  Array<Location_S> locations(locationCount);
  double radius;
  double theta;
  Location_S loc;
  for(int loc_cnt=0;loc_cnt<locationCount;loc_cnt++)
  {
	
	theta=randu()*2*pi;
	do
	{
	  radius=sqrt(randu())*circleRadius;
	}while(radius<minDistFromCenter);
	loc.x=radius*cos(theta);
	loc.y=radius*sin(theta);
	loc=loc+circleCenter;
	locations(loc_cnt)=loc;
  }
  return(locations);
}

Array <Location_S>  generateEquiDistantLocationOnCircle(Location_S circleCenter,double circleRadius,double phaseOffsetInDegree,int locationCount){
  
  Array<Location_S> locations(locationCount);
  
  if(phaseOffsetInDegree<0) {cout<<"[both:] Negative Offset not possible for generateEquiDistantLocationInCircle()...";exit(0);}
  
  vec theta=getVector(0,(locationCount-1)*2*pi/locationCount,2*pi/locationCount);
  theta=theta+phaseOffsetInDegree;
  for(int loc_cnt=0;loc_cnt<locationCount;loc_cnt++)
  {
	Location_S loc;
	loc.x=circleRadius*cos(theta(loc_cnt));
	loc.y=circleRadius*sin(theta(loc_cnt));
	loc=loc+circleCenter;
	locations(loc_cnt)=loc;
  }
  return(locations);
}
Array<Location_S> generate3DSatelliteDropLocation(double height,int locationCount)
{
    //cout<<"height"<<height<<"\n locationCount:"<<locationCount<<endl;
    double circleRadius = earthRadius + height;
    //cout<<"satellite orbit radius : "<<circleRadius<<endl;
    Array<Location_S> loc;loc.set_length(locationCount);
    for(int cnt=0;cnt<locationCount;cnt++)
    {
        double theta = randu()*pi;
        double phi = randu()*2*pi;
        //cout<<"sat_theta and sat_phi : "<<theta<<"  "<<phi<<endl;
//         loc(cnt).x = circleRadius*sin(theta)*cos(phi);
//         loc(cnt).y = circleRadius*sin(theta)*sin(phi);
//         loc(cnt).z = circleRadius*cos(theta);
        loc(cnt).x = 0;
        loc(cnt).y = 0;
        loc(cnt).z = circleRadius;
        //cout << "loc(cnt).x" << loc(cnt).x <<endl;
        //cout << "loc(cnt).y" << loc(cnt).y <<endl;
        //cout << "loc(cnt).z" << loc(cnt).z <<endl;
    }
    //cout<<"sat3Dloc"<<loc<<endl;
    return loc;
}
double findDistance(Location_S N1Loc, Location_S N2Loc){
  
  double x=N1Loc.x-N2Loc.x, y=N1Loc.y-N2Loc.y;
  return(sqrt(sqr(x)+sqr(y)));
}

double find3Ddistance(Location_S N1Loc, Location_S N2Loc){
  
  double x=N1Loc.x-N2Loc.x, y=N1Loc.y-N2Loc.y, z=N1Loc.z-N2Loc.z;
  return(sqrt(sqr(x)+sqr(y)+sqr(z)));
}

vec findAngle(Location_S N1Loc, Location_S N2Loc){

  vec nodeAngle=zeros(2);
  double x=N2Loc.x-N1Loc.x, y=N2Loc.y-N1Loc.y, z=N1Loc.z-N2Loc.z;
  if(x==0)
  {
      nodeAngle(0)= 0.0;
  }
  else
  {
    if(y>=0)
      if(x>=0)
        nodeAngle(0)=rad_to_deg(atan(y/x));
      else
        nodeAngle(0)=180+rad_to_deg(atan(y/x));
    else
      if(x>=0)
        nodeAngle(0)=360+rad_to_deg(atan(y/x));
      else
        nodeAngle(0)=180+rad_to_deg(atan(y/x));
  }

  nodeAngle(1)=rad_to_deg(atan(z/sqrt(pow(x,2.0)+pow(y,2.0))))+90.0;	 // 90 added to make zero reference points upwards
  
  if(nodeAngle(0)>=360) nodeAngle(0)-=360;
  return(nodeAngle);
}

vec find3DAngle(Location_S N1Loc, Location_S N2Loc,double verAntElementSpacing,int antElementsPerCol){
  
  vec nodeAngle(2);
  double x=N2Loc.x-N1Loc.x, y=N2Loc.y-N1Loc.y,  z=N1Loc.z-N2Loc.z + double(verAntElementSpacing*(antElementsPerCol-1)/2.0);    
  if(!std::isnan(y/x))
  {
    if(y>0)
      if(x>0)
	nodeAngle(0)=rad_to_deg(atan(y/x));
      else
	nodeAngle(0)=180+rad_to_deg(atan(y/x));
      else
	if(x>0)
	  nodeAngle(0)=360+rad_to_deg(atan(y/x));
	else
	  nodeAngle(0)=180+rad_to_deg(atan(y/x));	
	
    nodeAngle(1)=rad_to_deg(atan(z/(sqrt(pow(x,2.0)+pow(y,2.0)))))+90.0;   // 90 added to make zero reference points upwards 
      
      
      if(nodeAngle(0)>=360) nodeAngle(0)-=360;
      return(nodeAngle);
      
  }
  else{
      cout<<"Both node locations are same causing node angles NaN... Aborting()... "<<endl;abort();}
    }
 
//AAS
vec find3DAngle(Location_S N1Loc, Location_S N2Loc/*,double verAntElementSpacing,double horAntElementSpacing, int antElementsPerCol, int antElementsPerRow*//*,double sectorTilt*/)
{
  vec nodeAngle(2);
//   double refPosOfElem_y = double(horAntElementSpacing*(antElementsPerRow-1)/2.0);
//   double refPosOfElem_x = 0; //Assumed panal is placed on y axis
// //   double angleOffset = deg_to_rad(sectorTilt);
//   double curPanalAng = std::isnan(atan(refPosOfElem_y/refPosOfElem_x))?deg_to_rad(90):atan(refPosOfElem_y/refPosOfElem_x);
//   double panalOffset_x,panalOffset_y;
//   panalOffset_x = refPosOfElem_y*cos(curPanalAng /*+ angleOffset*/);
//   panalOffset_y = refPosOfElem_y*sin(curPanalAng /*+ angleOffset*/);
  double x=N2Loc.x-N1Loc.x/*+panalOffset_x*/, y= N2Loc.y-N1Loc.y /*+ panalOffset_y*/,  z = N1Loc.z-N2Loc.z /*+ double(verAntElementSpacing*(antElementsPerCol-1)/2.0*/;

  if(x==0)
  {
      nodeAngle(0)= 0.0;
  }
  else
  {
      if(y>=0)
          if(x>=0){
          nodeAngle(0)=rad_to_deg(atan(y/x));
          //cout<<"node angle if x,y>0: "<<nodeAngle(0)<<endl;
      }
      else{
          nodeAngle(0)=180+rad_to_deg(atan(y/x));
          //cout<<"node angle if x<0,y>0: "<<nodeAngle(0)<<endl;
      }
    else
        if(x>=0){
              nodeAngle(0)=rad_to_deg(atan(y/x));
              //cout<<"node angle if x>0,y<0: "<<nodeAngle(0)<<endl;
        }
        else{
              nodeAngle(0)=-180+rad_to_deg(atan(y/x));
              //cout<<"node angle if x,y<0: "<<nodeAngle(0)<<endl;
        }
  }

  nodeAngle(1)=rad_to_deg(atan(z/(sqrt(pow(x,2.0)+pow(y,2.0)))))+90.0; // 90 added to make zero reference points upwards
    if(nodeAngle(0)>=180) nodeAngle(0)-=360;
    //cout<<"nodeAngle : "<<nodeAngle<<endl;
    return(nodeAngle);
}



// mat Rx(double gamma)
// {
// };

mat Ry(double betaInRadian)
{
    mat Ry=zeros(3,3);
    Ry(0,0)=cos(betaInRadian);Ry(0,2)=sin(betaInRadian);
    Ry(1,1)=1.0;
    Ry(2,0)=-sin(betaInRadian);Ry(2,2)=cos(betaInRadian);
    
    return Ry;
};

mat Rz(double aplhaInRadian)
{
    mat Rz=zeros(3,3);
    Rz(0,0)=cos(aplhaInRadian);Rz(0,1)=-sin(aplhaInRadian);
    Rz(1,0)=sin(aplhaInRadian);Rz(1,1)=cos(aplhaInRadian);
    Rz(2,2)=1.0;
    
    return Rz;
};

mat ThetaCap(double thetaInRadian, double phiInRadian)
{
    mat thetaCap=zeros(3,1);
    thetaCap(0,0) = cos(thetaInRadian)*cos(phiInRadian);
    thetaCap(1,0) = cos(thetaInRadian)*sin(phiInRadian);
    thetaCap(2,0) = -sin(thetaInRadian);
    return thetaCap;
}

mat PhiCap(double phiInRadian)
{
    mat phiCap=zeros(3,1);
    phiCap(0,0) = -sin(phiInRadian);
    phiCap(1,0) = cos(phiInRadian);
    return phiCap;
}

void convertAngleFromGCStoLCS(double &ZOD,double &AOD,double alpha,double beta,double gamma,bool isInRadian)
{
    mat RotMat,rho_prime,LCScoord;
    double zodLCS,aodLCS;
    double deg2rad = pi/180.0;
    RotMat.set_size(3,3);
    rho_prime.set_size(3,1);
    if(!isInRadian)
    {
        ZOD *= deg2rad;
        AOD *= deg2rad;
        alpha *= deg2rad;
        beta *= deg2rad;
        gamma *= deg2rad;
    }
    
    RotMat(0,0)=cos(alpha)*cos(beta); 
    RotMat(0,1)=cos(alpha)*sin(beta)*sin(gamma)-sin(alpha)*cos(gamma);
    RotMat(0,2)=cos(alpha)*sin(beta)*cos(gamma)+sin(alpha)*sin(gamma);
    RotMat(1,0)=sin(alpha)*cos(beta);
    RotMat(1,1)=sin(alpha)*sin(beta)*sin(gamma)+cos(alpha)*cos(gamma);
    RotMat(1,2)=sin(alpha)*sin(beta)*cos(gamma)-cos(alpha)*sin(gamma);
    RotMat(2,0)=-sin(beta);
    RotMat(2,1)=cos(beta)*sin(gamma);
    RotMat(2,2)=cos(beta)*cos(gamma); 
    //cout<<"GCS to LCS RotMat  :  "<<RotMat<<endl;
    rho_prime(0,0)= sin(ZOD)*cos(AOD);
    rho_prime(1,0)= sin(ZOD)*sin(AOD);
    rho_prime(2,0)= cos(ZOD);
    //cout<<"GCS to LCS rho_prime  :  "<<rho_prime<<endl;
    LCScoord = RotMat.T()*rho_prime;
    //cout<<"LCScoord  :  "<<LCScoord<<endl;
    zodLCS = acos(LCScoord(2,0));
    if(std::isnan(LCScoord(1,0)/LCScoord(0,0)))
    {
        aodLCS = 0.0;
    }
    else
    {
        aodLCS = atan(LCScoord(1,0)/LCScoord(0,0));
        if(LCScoord(1,0)>0.0)
        {
            if(LCScoord(0,0)>0.0)
                aodLCS = atan(LCScoord(1,0)/LCScoord(0,0));
            else
                aodLCS=pi+atan(LCScoord(1,0)/LCScoord(0,0));
        }
        else
        {
            if(LCScoord(0,0)>0.0)
                aodLCS=atan(LCScoord(1,0)/LCScoord(0,0));
            else
                aodLCS=-pi+atan(LCScoord(1,0)/LCScoord(0,0));
        }
    }
    ZOD = zodLCS*180/pi;
    AOD = aodLCS*180/pi;
    //cout<<"aod and zod :  "<<ZOD<<"   "<<AOD<<endl;
    return;
}

//returns the rotation matrix constructed using alpha beta gamma.
mat getRotationMatrix(double alpha,double beta,double gamma,bool isInRadian)
{
    //Refer eqn 7.1-4 in TR 38.901
    if(!isInRadian)
    {
        double deg2rad = pi/180.0;
        alpha *=deg2rad;beta *=deg2rad;gamma *=deg2rad;
    }
    mat R = zeros(3,3);
    R(0,0) = cos(alpha)*cos(beta);R(0,1)=cos(alpha)*sin(beta)*sin(gamma) - sin(alpha)*cos(gamma);R(0,2) = cos(alpha)*sin(beta)*cos(gamma) + sin(alpha)*sin(gamma);
    R(1,0) = sin(alpha)*cos(beta);R(1,1)=sin(alpha)*sin(beta)*sin(gamma) + cos(alpha)*cos(gamma);R(1,2) = sin(alpha)*sin(beta)*cos(gamma) - cos(alpha)*sin(gamma);
    R(2,0) = -sin(beta);R(2,1) = cos(beta)*sin(gamma);R(2,2)=cos(beta)*cos(gamma);
    return R;
}

void convertAngleFromLCStoGCS(double& ZOD, double& AOD, double alpha, double beta, double gamma, bool isInRadian)

{
    mat RotMat,rho_prime,LCScoord;
    double zodLCS,aodLCS;
    double deg2rad = pi/180.0;
    RotMat.set_size(3,3);
    rho_prime.set_size(3,1);
    if(!isInRadian)
    {
        ZOD *= deg2rad;
        AOD *= deg2rad;
        alpha *= deg2rad;
        beta *= deg2rad;
        gamma *= deg2rad;
    }

    RotMat(0,0)=cos(alpha)*cos(beta);
    RotMat(0,1)=cos(alpha)*sin(beta)*sin(gamma)-sin(alpha)*cos(gamma);
    RotMat(0,2)=cos(alpha)*sin(beta)*cos(gamma)+sin(alpha)*sin(gamma);
    RotMat(1,0)=sin(alpha)*cos(beta);
    RotMat(1,1)=sin(alpha)*sin(beta)*sin(gamma)+cos(alpha)*cos(gamma);
    RotMat(1,2)=sin(alpha)*sin(beta)*cos(gamma)-cos(alpha)*sin(gamma);
    RotMat(2,0)=-sin(beta);
    RotMat(2,1)=cos(beta)*sin(gamma);
    RotMat(2,2)=cos(beta)*cos(gamma);

    rho_prime(0,0)= sin(AOD)*cos(ZOD);
    rho_prime(1,0)= sin(AOD)*sin(ZOD);
    rho_prime(2,0)= cos(AOD);

    LCScoord = inv(RotMat).T()*rho_prime;
    zodLCS = acos(LCScoord(2,0));
    if(std::isnan(LCScoord(1,0)/LCScoord(0,0)))
    {
        aodLCS = 0.0;
    }
    else
    {
        aodLCS = atan(LCScoord(1,0)/LCScoord(0,0));
        if(LCScoord(1,0)>0.0)
        {
            if(LCScoord(0,0)>0.0)
                aodLCS = atan(LCScoord(1,0)/LCScoord(0,0));
            else
                aodLCS=pi+atan(LCScoord(1,0)/LCScoord(0,0));
        }
        else
        {
            if(LCScoord(0,0)>0.0)
                aodLCS=atan(LCScoord(1,0)/LCScoord(0,0));
            else
                aodLCS=-pi+atan(LCScoord(1,0)/LCScoord(0,0));
        }
    }
    ZOD = zodLCS*180/pi;
    AOD = aodLCS*180/pi;
    return;
}



//gives x,y,z coordinates from theta, phi,rho.
mat getCartesianVector(double theta,double phi,bool isInRadian,double rho)
{
    //Refer Eqn 7.5-23/24/25 in TR 38.901
    //phi refers to zenith and theta refers to azimuth
    if(!isInRadian)
    {
        double deg2rad = pi/180.0;
        phi *=deg2rad,theta *= deg2rad; 
    }
    mat cartesianVector(3,1);
    cartesianVector(0,0) = sin(phi)*cos(theta);
    cartesianVector(1,0) = sin(phi)*sin(theta);
    cartesianVector(2,0) = cos(phi);
    //cartesianVector(0,0) = sin(theta)*cos(phi);
    //cartesianVector(1,0) = sin(theta)*sin(phi);
    //cartesianVector(2,0) = cos(theta);
    //cout<<"cartesianVector : "<<cartesianVector<<endl;
    return rho*cartesianVector;
}

//return theta phi from the x,y,z coordinates.
void getAngles(mat unitVector,double& AOD,double& ZOD,bool returnInRadian)
{
    double zodLCS,aodLCS;
    zodLCS = acos(unitVector(2,0));
    if(std::isnan(unitVector(1,0)/unitVector(0,0)))
    {
        aodLCS = 0.0;
    }
    else
    {
        if(unitVector(1,0)>0.0)
        {
            if(unitVector(0,0)>0.0)
                aodLCS = atan(unitVector(1,0)/unitVector(0,0));
            else
                aodLCS=pi+atan(unitVector(1,0)/unitVector(0,0));
        }
        else
        {
            if(unitVector(0,0)>0.0)
                aodLCS = atan(unitVector(1,0)/unitVector(0,0));
            else
                aodLCS= -pi+atan(unitVector(1,0)/unitVector(0,0));
        }
    }
    if(returnInRadian)
    {
        AOD = aodLCS;ZOD = zodLCS;
    }
    else
    {
        AOD = aodLCS*180/pi;ZOD = zodLCS*180/pi;
    }
}

double getShadowLoss(Location_S nodeLoc,mat shdwGrid,double shadowStdDev,double corrDist){
  
  mat myGridCorner(2,2),x(1,2),y(2,1);
  mat shadow;
  int myGridX,myGridY;
  
  myGridX=floor_i((nodeLoc.x+(double)(shdwGrid.cols()-1)*corrDist/2)/corrDist);
  myGridY=floor_i((nodeLoc.y+(double)(shdwGrid.rows()-1)*corrDist/2)/corrDist);
  if(myGridX<0 || (myGridX>=(shdwGrid.cols()-1))) { cout<<"[both:] Warning .. X Grid Exceeded in shdwGrid for x location "<<nodeLoc.x<<","<<(shdwGrid.cols()-1)*corrDist/2<<endl; myGridX-=1;}
  if(myGridY<0 || (myGridY>=(shdwGrid.rows()-1))) { cout<<"[both:] Warning .. Y Grid Exceeded in shdwGrid for y location "<<nodeLoc.y<<endl;myGridY-=1;}
  
  myGridCorner(0,0)=shdwGrid(myGridY,myGridX);
  myGridCorner(1,0)=shdwGrid(myGridY+1,myGridX);
  myGridCorner(0,1)=shdwGrid(myGridY,myGridX+1);
  myGridCorner(1,1)=shdwGrid(myGridY+1,myGridX+1);
  
  
  y(0,0) = (ceil(nodeLoc.y/corrDist)*corrDist - nodeLoc.y) / corrDist;
  y(1,0) = 1- y(0,0);
  y = sqrt(y);
  
  x(0,0) = (ceil(nodeLoc.x/corrDist)*corrDist - nodeLoc.x) / corrDist;
  x(0,1) = 1- x(0,0);
  x = sqrt(x);
  
  shadow = x*myGridCorner*y;
  cout<<"shadow:"<<shadow<<endl;
  return(shadow(0,0)*shadowStdDev);
}

Array<Neighbour_S> getWrapAroundNeighbours(int cellCnt, int cellID, Location_S cellLocation, int cellISD){
 
  Array<Neighbour_S> neighbourInfo;
  ivec neighbours(cellCnt-1);
  double neighbourXLoc[cellCnt-1],neighbourYLoc[cellCnt-1];
  double sqrtThree=1.732;
  double cellRadius=cellISD/sqrtThree;
  neighbourInfo.set_size(cellCnt-1);
  
  if(cellCnt==7)
  {
	neighbourXLoc[0] = sqrtThree*cellRadius;		neighbourYLoc[0] = 0;
	neighbourXLoc[1] = sqrtThree*cellRadius/2.0;	neighbourYLoc[1] = -3.0*cellRadius/2.0;
	neighbourXLoc[2] = -sqrtThree*cellRadius/2.0;	neighbourYLoc[2] = -3.0*cellRadius/2.0;
	neighbourXLoc[3] = -sqrtThree*cellRadius;		neighbourYLoc[3] = 0;

	neighbourXLoc[4] = -sqrtThree*cellRadius/2.0;	neighbourYLoc[4] = 3.0*cellRadius/2.0;
	neighbourXLoc[5] = sqrtThree*cellRadius/2.0;	neighbourYLoc[5] = 3.0*cellRadius/2.0;
	
	switch(cellID)
	{
	  case 0: neighbours="1 2 3 4 5 6"; break;
	  case 1: neighbours="3 5 2 0 6 4"; break;
	  case 2: neighbours="5 4 6 3 0 1"; break;
	  case 3: neighbours="2 6 5 1 4 0"; break;
	  case 4: neighbours="0 3 1 6 2 5"; break;
	  case 5: neighbours="6 0 4 2 1 3"; break;
	  case 6: neighbours="4 1 0 5 3 2"; break;
	}
  }
  else if(cellCnt==19)
  {
	neighbourXLoc[0] = sqrtThree*cellRadius;			neighbourYLoc[0] = 0;
	neighbourXLoc[1] = sqrtThree*cellRadius/2.0;		neighbourYLoc[1] = -3.0*cellRadius/2.0;
	neighbourXLoc[2] = -sqrtThree*cellRadius/2.0;		neighbourYLoc[2] = -3.0*cellRadius/2.0;
	neighbourXLoc[3] = -sqrtThree*cellRadius;			neighbourYLoc[3] = 0;
	neighbourXLoc[4] = -sqrtThree*cellRadius/2.0;		neighbourYLoc[4] = 3.0*cellRadius/2.0;
	neighbourXLoc[5] = sqrtThree*cellRadius/2.0;		neighbourYLoc[5] = 3.0*cellRadius/2.0;
	neighbourXLoc[6] = 2.0*sqrtThree*cellRadius;		neighbourYLoc[6] = 0;
	neighbourXLoc[7] = 3.0*sqrtThree*cellRadius/2.0;		neighbourYLoc[7] = -3.0*cellRadius/2.0;
	neighbourXLoc[8] = sqrtThree*cellRadius;			neighbourYLoc[8] = -3.0*cellRadius;
	neighbourXLoc[9] = 0;							neighbourYLoc[9] = -3.0*cellRadius;
	neighbourXLoc[10] = -sqrtThree*cellRadius;		neighbourYLoc[10] = -3.0*cellRadius;
	neighbourXLoc[11] = -3.0*sqrtThree*cellRadius/2.0; 	neighbourYLoc[11] = -3.0*cellRadius/2.0;
	neighbourXLoc[12] = -2.0*sqrtThree*cellRadius;		neighbourYLoc[12] = 0;
	neighbourXLoc[13] = -3.0*sqrtThree*cellRadius/2.0;  	neighbourYLoc[13] = 3.0*cellRadius/2.0;
	neighbourXLoc[14] = -sqrtThree*cellRadius;		neighbourYLoc[14] = 3.0*cellRadius;
	neighbourXLoc[15] = 0;							neighbourYLoc[15] = 3.0*cellRadius;
	neighbourXLoc[16] = sqrtThree*cellRadius;			neighbourYLoc[16] = 3.0*cellRadius;
	neighbourXLoc[17] = 3.0*sqrtThree*cellRadius/2.0;		neighbourYLoc[17] = 3.0*cellRadius/2.0;
	
	switch(cellID)
	{
	  case 0: neighbours="1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18"; break;
	  case 1: neighbours="7 8 2 0 6 18 15 14 13 9 10 3 4 5 16 17 12 11"; break;
	  case 2: neighbours="8 9 10 3 0 1 14 13 17 16 15 11 12 4 5 6 18 7"; break;
	  case 3: neighbours="2 10 11 12 4 0 8 9 16 15 7 18 17 13 14 5 6 1"; break;
	  case 4: neighbours="0 3 12 13 14 5 1 2 10 11 18 17 9 8 7 15 16 6"; break;
	  case 5: neighbours="6 0 4 14 15 16 18 1 2 3 12 13 8 7 11 10 9 17"; break;
	  case 6: neighbours="18 1 0 5 16 17 11 7 8 2 3 4 14 15 10 9 13 12"; break;
	  case 7: neighbours="15 14 8 1 18 11 16 5 4 13 9 2 0 6 17 12 3 10"; break;
	  case 8: neighbours="14 5 13 9 2 1 7 15 4 12 17 16 10 3 0 6 18 11"; break;
	  case 9: neighbours="13 17 16 10 2 8 4 12 18 6 5 15 11 3 0 1 7 14"; break;
	  case 10: neighbours="9 16 15 11 3 2 13 17 6 5 14 7 18 12 4 0 1 8"; break;
	  case 11: neighbours="10 15 7 18 12 3 9 16 5 14 8 1 6 17 13 4 0 2"; break;
	  case 12: neighbours="3 11 18 17 13 4 2 10 15 7 1 6 16 9 8 14 5 0"; break;
	  case 13: neighbours="4 12 17 9 8 14 0 3 11 18 6 16 10 2 1 7 15 5"; break;
	  case 14: neighbours="5 4 13 8 7 15 6 0 3 12 17 9 2 1 18 11 10 16"; break;
	  case 15: neighbours="16 5 14 7 11 10 17 6 0 4 13 8 1 18 12 3 2 9"; break;
	  case 16: neighbours="17 6 5 15 10 9 12 18 1 0 4 14 7 11 3 2 8 13"; break;
	  case 17: neighbours="12 18 6 16 9 13 3 11 7 1 0 5 15 10 2 8 14 4"; break;
	  case 18: neighbours="11 7 1 6 17 12 10 15 14 8 2 0 5 16 9 13 4 3"; break;
	}
  }
  else if(cellCnt==1)
  {}
  else
  {
	cout<<"[both:] Unsupported cell count in getWrapAroundNeighbours()... ";
	exit(0);
  }
  
  
  for(int neighbour_cnt=0;neighbour_cnt<(cellCnt-1);neighbour_cnt++)
  {
	neighbourInfo(neighbour_cnt).cellID=neighbours(neighbour_cnt);
	neighbourInfo(neighbour_cnt).cellXYLocation.x=cellLocation.x+neighbourXLoc[neighbour_cnt];
	neighbourInfo(neighbour_cnt).cellXYLocation.y=cellLocation.y+neighbourYLoc[neighbour_cnt];
	neighbourInfo(neighbour_cnt).cellXYLocation.z=NAN;
  }
  return(neighbourInfo);
}

Location_S WrapMyLocation(Location_S wrapTxCellLoc, Location_S txCellLoc, Location_S txLoc){
  
  Location_S wrapTxLoc=txLoc;
  wrapTxLoc.x=txLoc.x-txCellLoc.x+wrapTxCellLoc.x;
  wrapTxLoc.y=txLoc.y-txCellLoc.y+wrapTxCellLoc.y;
  wrapTxLoc.z=txLoc.z;
  return(wrapTxLoc);
}

ivec lspTransformIndex(int length)
{
    ivec indx(length);
    if(length==6)
    {
        indx = "3 0 1 2 5 4";
    }
    else if(length==7)
    {
        indx = "3 4 0 1 2 6 5";
    }
    else if(length==14)
    {
        indx = "6 7 8 9 0 1 2 3 4 5 12 13 10 11";
    }
    else if(length==12)
    {
        indx = "6 7 0 1 2 3 4 5 10 11 8 9";
    }
    return indx;
}

ivec invTransformIndx(int length)
{
    if(length==7)
        return "2 3 4 0 1 6 5";
    else if(length==6)
        return "1 2 3 0 5 4";
    abort();
}

mat applySpatialFilter(mat& data,mat& filter,bool isSame)
{
    int n = filter.rows();
    if(n%2==0)
    {
        cout<<"m and n must be odd for spatial filtering..."<<endl;abort();
    }
    if(n!=filter.cols())
    {
        cout<<"filter Matrix must be square.."<<endl;abort();
    }
    int M = data.rows(),N = data.cols();
    mat op = zeros(M+n-1,N+n-1),out=op;
    op.set_submatrix((n-1)/2,M+(n-1)/2-1,(n-1)/2,N+(n-1)/2-1,data);
    for(int row_cnt=0;row_cnt<op.rows();row_cnt++)
    {
        for(int col_cnt=0;col_cnt<op.cols();col_cnt++)
        {
            double val = 0.0;
            for(int i=-(n-1)/2;i<(n-1)/2;i++)
            {
                for(int j=-(n-1)/2;j<(n-1)/2;j++)
                {
                    if(row_cnt+i>=0 && row_cnt+i<=op.rows()-1 && col_cnt+j>=0 && col_cnt+j<=op.cols()-1)
                    {
                        val += filter(i+(n-1)/2,j+(n-1)/2)*op(row_cnt+i,col_cnt+j);
                    }
                }
            }
            out(row_cnt,col_cnt) = val;
        }
    }
    if(isSame)
        return out((n-1)/2,M+(n-1)/2-1,(n-1)/2,N+(n-1)/2-1);
    return out;
}

mat getSpatialCorrelatedMatrix(double normDist,int rowCount,int colCount,double dcorr)
{
    int M = 2*normDist + 1;
    mat h(M,M);
    for(int i=0;i<M;i++)
    {
        for(int j=0;j<M;j++)
        {
            double d = sqrt(sqr((M+1)/2-i)+sqr((M+1)/2-j));
            h(i,j) = exp(-d*normDist/dcorr);
        }
    }
    mat initMap = randn(rowCount+M,colCount+M);
    mat corrMapPad = applySpatialFilter(initMap,h,1);
    mat corrMap = corrMapPad((M-1)/2,rowCount+(M-1)/2,(M-1)/2,colCount+(M-1)/2);
    return corrMap;
}

void applyCorrelationFilter(mat& grid,double normDist,double dcorr,double normDmax)
{
    int delta = ceil_i(normDmax/normDist);   //Assuming correlation is zero after normDmax*dcorr
    int M = 2*delta + 1;
    mat h(M,M);
    for(int i=0;i<M;i++)
    {
        for(int j=0;j<M;j++)
        {
            double d = sqrt(sqr((M+1)/2-i)+sqr((M+1)/2-j));
            h(i,j) = exp(-d*normDist/dcorr);
        }
    }
    grid = applySpatialFilter(grid,h,1);
}

double getCorrDistForDeltaTau(InFScenario_E infScenario)
{
    return (infScenario == _INF_SL_ || infScenario == _INF_DL_) ? 6.0 : 11.0;
}

void initGridsFor5GSCMLargeScaleParameters(Array< Array< Array<mat> > >& scmGrids,SCMInfo_S *scmInfo,SCMPropagationScenario scmScenario,InFScenario_E infScenario)
{
    #ifdef _USING_CALIBRATION_
    if(scmScenario == _SCM5G_RURAL_MACRO_)
    {
        cout<<"_USING_CALIBRATION_ is cannot used for Rural Macro...please disable it.."<<endl;abort();
    }
    #endif
    int numberOfFloorsFor3D = (scmScenario == _SCM5G_INDOOR_OFFICE_ || scmScenario == _SCM5G_INDOOR_FACTORY_) ? 1 : 8;//as per TR 36.893
    scmGrids.set_length(numberOfFloorsFor3D);
    int size = (scmScenario == _SCM5G_INDOOR_OFFICE_ || scmScenario == _SCM5G_INDOOR_FACTORY_) ? 2 : 3; //LOS, NLOS, O2I
    int nLSPs= (scmScenario==_SCM5G_INDOOR_FACTORY_) ? 8 : 7; //DS,ASD,ASA,SF,K,ZSA,ZSD deltaTau
    scmInfo->normGridDist = 7.0*ones(nLSPs); //DS,ASD,ASA,SF,K,ZSA,ZSD
    bool useSpatialFilter = false;
    double dCorrForDeltaTau = 0.0;
    if(infScenario!=_INF_UNDEF_)
    {
        dCorrForDeltaTau = getCorrDistForDeltaTau(infScenario);
    }
    
    for(int floorIndx=0;floorIndx<numberOfFloorsFor3D;floorIndx++)//This is for to have uncorrelated lsps for each floor
    {
        scmGrids(floorIndx).set_length(nLSPs);// DS,ASD,ASA,SF,K,ZSA,ZSD
        int rowCount, colCount;
        for(int lsp = 0; lsp < nLSPs ; lsp++) // DS,ASD,ASA,SF,K,ZSA,ZSD
        {
            LSPParameter lsp_param = LSPParameter(lsp);
            scmGrids(floorIndx)(lsp).set_length(size);
            for(int indx = 0;indx<size;indx++) // LOS, NLOS, O2I
            {
                SCMLinkCondition linkCondition = SCMLinkCondition(indx);
                int gridIndx = get5GSCMGridIndx(scmScenario,linkCondition);
                vec dCorr = scmInfo->correlationDistance.get_col(gridIndx);
                if(scmScenario==_SCMNTN_RURAL_){
                  dCorr=getCorrelationDistancesForNTN(scmScenario,90,1,0);
                }
                double gridDist = scmInfo->normGridDist(lsp)*((infScenario!=_INF_UNDEF_ && lsp_param==_DELTA_TAU_) ? dCorrForDeltaTau: dCorr(lsp));
                //cout<< "dcorr"<<dCorr<<endl;
                //cout<<"grid distance"<<gridDist<<endl;

                if(lsp_param == _K_ && (linkCondition == _NLOS_ || linkCondition == _O2I_)) // For K --->(NLOS and O2I)
                {
                    rowCount = 0; colCount = 0;
                }
                else
                {
                    //cout<<"coverage area x and y length"<<scmInfo->coverageArea.xLen<<" | "<< scmInfo->coverageArea.yLen <<endl;
                    rowCount = ceil_i((scmInfo->coverageArea.yLen/(gridDist)) + 1);
                    colCount = ceil_i((scmInfo->coverageArea.xLen/(gridDist)) + 1);
                }
                if(!useSpatialFilter)
                {
                    scmGrids(floorIndx)(lsp)(indx) = randn(rowCount,colCount);
                }
                else 
                {   
                    mat grid = randn(rowCount,colCount);
                    if(rowCount*colCount!=0)
                    {
                        double mu = sum(sum(grid))/(rowCount*colCount);
                        double sigma = sqrt(sum(sum_sqr(grid-mu))/(rowCount*colCount));
                        grid = (grid-mu)/sigma;
                    }
                    scmGrids(floorIndx)(lsp)(indx) = grid;
                    double normDmax = scmInfo->normGridDist(lsp)-1;
                    if(scmInfo->normGridDist(lsp)<=normDmax && rowCount*colCount>0)
                        applyCorrelationFilter(scmGrids(floorIndx)(lsp)(indx),scmInfo->normGridDist(lsp),scmInfo->correlationDistance(lsp,gridIndx),normDmax);
                }
            }
        }
    }
//     cout<<"size of scm grid:"<<scmGrids.length()<<","<<scmGrids(0).length()<<","<<scmGrids(0)(0).length()<<","<<scmGrids(0)(2)(0).rows()<<","<<scmGrids(0)(2)(0).cols()<<","<<endl;

}
 
void initGridsForSCMLargeScaleParameters(Array< Array< Array<mat> > > &scmGrids,SCMInfo_S &scmInfo,SCMPropagationScenario scmScenario,InFScenario_E infScenario){
  
  if(scmScenario==_SCM2D_URBAN_MACRO_ || scmScenario==_SCM2D_URBAN_MICRO_ || scmScenario==_SCM2D_RURAL_MACRO_ || scmScenario==_SCM2D_INDOOR_HOTSPOT_)
  {
    scmGrids.set_length(1);
    scmGrids(0).set_length(5);
    int rowCount, colCount;
    if(scmScenario==_SCM2D_URBAN_MICRO_)
    {
      
      int size = 3; // LOS , NLOS , O2I
      for(int lsp = 0; lsp < 5 ; lsp++) // DS,ASD,ASA,SF,K
      {
	scmGrids(0)(lsp).set_length(size);
	for(int indx = 3;indx<6;indx++) // LOS, NLOS, O2I
	{
	  rowCount = (ceil) ((scmInfo.coverageArea.yLen/(5*scmInfo.correlationDistance(lsp,indx))) + 1);
	  colCount = (ceil) ((scmInfo.coverageArea.xLen/(5*scmInfo.correlationDistance(lsp,indx))) + 1);
	  if(lsp == 4 && (indx == 4 || indx == 5)) // For K and (NLOS or O2I)
	  {
	    rowCount = 0; colCount = 0;
	  }
	  
	  scmGrids(0)(lsp)(indx%3) = randn(rowCount,colCount);
	}
      }
    }else if(scmScenario==_SCM2D_URBAN_MACRO_)
    {
      int size = 3; // LOS , NLOS
      for(int lsp = 0; lsp < 5 ; lsp++) // DS,ASD,ASA,SF,K
      {
	scmGrids(0)(lsp).set_length(size);
	for(int indx = 6;indx<8;indx++) // LOS, NLOS, O2I
	{
	  rowCount = (ceil) ((scmInfo.coverageArea.yLen/(5*scmInfo.correlationDistance(lsp,indx))) + 1);
	  colCount = (ceil) ((scmInfo.coverageArea.xLen/(5*scmInfo.correlationDistance(lsp,indx))) + 1);
	  if(lsp == 4 && indx == 7) // For K and NLOS
	  {
	    rowCount = 0; colCount = 0;
	  }
	  scmGrids(0)(lsp)(indx%3) = randn(rowCount,colCount);
	}
      }
    }else if(scmScenario==_SCM2D_RURAL_MACRO_)
    {
      int size = 2; // LOS , NLOS
      for(int lsp = 0; lsp < 5 ; lsp++) // DS,ASD,ASA,SF,K
      {
	scmGrids(0)(lsp).set_length(size);
	for(int indx = 9;indx<11;indx++) // LOS, NLOS, O2I
	{
	  rowCount = (ceil) ((scmInfo.coverageArea.yLen/(5*scmInfo.correlationDistance(lsp,indx))) + 1);
	  colCount = (ceil) ((scmInfo.coverageArea.xLen/(5*scmInfo.correlationDistance(lsp,indx))) + 1);
	  if(lsp == 4 && indx == 10) // For K and NLOS
	  {
	    rowCount = 0; colCount = 0;
	  }
	  scmGrids(0)(lsp)(indx%3) = randn(rowCount,colCount);
	}
      }
    }
    else if(scmScenario==_SCM2D_INDOOR_HOTSPOT_)
    {
      int size = 2; // LOS , NLOS
      for(int lsp = 0; lsp < 5 ; lsp++) // DS,ASD,ASA,SF,K
      {
	scmGrids(0)(lsp).set_length(size);
	for(int indx = 0;indx<2;indx++) // LOS, NLOS
	{
	  rowCount = (ceil) ((scmInfo.coverageArea.yLen/(5*scmInfo.correlationDistance(lsp,indx))) + 1);
	  colCount = (ceil) ((scmInfo.coverageArea.xLen/(5*scmInfo.correlationDistance(lsp,indx))) + 1);
	  if(lsp == 4 && indx == 1) // For K and NLOS
	  {
	    rowCount = 0; colCount = 0;
	  }
	  scmGrids(0)(lsp)(indx) = randn(rowCount,colCount);
	}
      }
      
    }
//     else
//     {
//       cout<<"Unsupported IMT Terrain for initGridsForSCMLargeScaleParameters()..."<<endl;
//     }
  }//End for 2D SCM
  else if(scmScenario==_SCM3D_URBAN_MICRO_ || scmScenario==_SCM3D_URBAN_MACRO_) //3D Channel Model
  {
    int numberOfFloorsFor3D=8;//as per TR 36.893
    scmGrids.set_length(numberOfFloorsFor3D);
    
    for(int floorIndx=0;floorIndx<numberOfFloorsFor3D;floorIndx++)//This is for to have uncorrelated lsps for each floor
    {
	scmGrids(floorIndx).set_length(7);// DS,ASD,ASA,SF,K,ZSA,ZSD
	int rowCount, colCount;
	if(scmScenario==_SCM3D_URBAN_MICRO_)
	{
	  int size = 3; // LOS , NLOS , O2I
	  for(int lsp = 0; lsp < 7 ; lsp++) // DS,ASD,ASA,SF,K,ZSA,ZSD
	  {
	    scmGrids(floorIndx)(lsp).set_length(size);
	    for(int indx = 0;indx<3;indx++) // LOS, NLOS, O2I
	    {
	      rowCount = (ceil) ((scmInfo.coverageArea.yLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
	      colCount = (ceil) ((scmInfo.coverageArea.xLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
	      if(lsp == 4 && (indx == 1 || indx == 2)) // For K --->(NLOS and O2I)
	      {
		rowCount = 0; colCount = 0;
	      }
	      scmGrids(floorIndx)(lsp)(indx) = randn(rowCount,colCount);
	    }
	  }
	}else if(scmScenario==_SCM3D_URBAN_MACRO_)
	{
	  int size = 3; // LOS , NLOS, O2I
	  for(int lsp = 0; lsp < 7 ; lsp++) // DS,ASD,ASA,SF,K,ZSA,ZSD
	  {
	    scmGrids(floorIndx)(lsp).set_length(size);
	    for(int indx = 3;indx<6;indx++) // LOS, NLOS, O2I
	    {
	      rowCount = (ceil) ((scmInfo.coverageArea.yLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
	      colCount = (ceil) ((scmInfo.coverageArea.xLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
	      if(lsp == 4 && (indx == 4 || indx == 5)) // For K --->(NLOS and O2I)
	      {
		rowCount = 0; colCount = 0;
	      }
	      scmGrids(floorIndx)(lsp)(indx%3) = randn(rowCount,colCount);
	    }
	  }
	}
    }//end for floorIndx
  }


  else if(scmScenario==_SCM5G_URBAN_MICRO_ || scmScenario== _SCM5G_URBAN_MACRO_ ||scmScenario== _SCM5G_RURAL_MACRO_ ||scmScenario== _SCM5G_INDOOR_OFFICE_ ||scmScenario==_SCMNTN_RURAL_ ) //5G Channel Model
  { //cout<<"Entring into grid place 2:"<<scmScenario<<endl; abort();
      return initGridsFor5GSCMLargeScaleParameters(scmGrids,&scmInfo,scmScenario,infScenario);


    // FIXME! Using same grid as 3DSCM. If changes in 5G CM need to be changed
    //initGridsFor5GSCMLargeScaleParameters(scmGrids,&scmInfo,scmScenario);return;  
    int numberOfFloorsFor3D = 8;//as per TR 36.893
    scmGrids.set_length(numberOfFloorsFor3D);
    
    for(int floorIndx=0;floorIndx<numberOfFloorsFor3D;floorIndx++)//This is for to have uncorrelated lsps for each floor
    {
      scmGrids(floorIndx).set_length(7);// DS,ASD,ASA,SF,K,ZSA,ZSD
      int rowCount, colCount;
      if(scmScenario==_SCM5G_URBAN_MICRO_)
      {
	int size = 3; // LOS , NLOS , O2I
	for(int lsp = 0; lsp < 7 ; lsp++) // DS,ASD,ASA,SF,K,ZSA,ZSD
	{
	  scmGrids(floorIndx)(lsp).set_length(size);
	  for(int indx = 0;indx<3;indx++) // LOS, NLOS, O2I
	  {
	    rowCount = (ceil) ((scmInfo.coverageArea.yLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
	    colCount = (ceil) ((scmInfo.coverageArea.xLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
	    if(lsp == 4 && (indx == 1 || indx == 2)) // For K --->(NLOS and O2I)
	    {
	      rowCount = 0; colCount = 0;
	    }
	    scmGrids(floorIndx)(lsp)(indx) = randn(rowCount,colCount);
	  }
	}
      }
      else if(scmScenario==_SCM5G_URBAN_MACRO_)
      {
	int size = 3; // LOS , NLOS, O2I
	for(int lsp = 0; lsp < 7 ; lsp++) // DS,ASD,ASA,SF,K,ZSA,ZSD
	{
	  scmGrids(floorIndx)(lsp).set_length(size);
	  for(int indx = 3;indx<6;indx++) // LOS, NLOS, O2I
	  {
	    rowCount = (ceil) ((scmInfo.coverageArea.yLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
	    colCount = (ceil) ((scmInfo.coverageArea.xLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
	    if(lsp == 4 && (indx == 4 || indx == 5)) // For K --->(NLOS and O2I)
	    {
	      rowCount = 0; colCount = 0;
	    }
	    scmGrids(floorIndx)(lsp)(indx%3) = randn(rowCount,colCount);
	  }
	}

      }
      else if(scmScenario==_SCM5G_RURAL_MACRO_||scmScenario==_SCMNTN_RURAL_ )
      {
	int size = 3; // LOS , NLOS, O2I
	for(int lsp = 0; lsp < 7 ; lsp++) // DS,ASD,ASA,SF,K,ZSA,ZSD
	{
	  scmGrids(floorIndx)(lsp).set_length(size);
	  for(int indx = 6;indx<9;indx++) // LOS, NLOS, O2I
	  {
	    rowCount = (ceil) ((scmInfo.coverageArea.yLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
	    colCount = (ceil) ((scmInfo.coverageArea.xLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
        //change 7*correlationDistance to 3*correlationDistance for testing -deep
	    if(lsp == 4 && (indx == 7 ||indx == 8)) // For K --->(NLOS and O2I)
	    {
	      rowCount = 0; colCount = 0;
	    }
	    scmGrids(floorIndx)(lsp)(indx%3) = randn(rowCount,colCount);
        //cout<<"_scmGrids(floorIndx)(lsp)(indx%3)_"<<scmGrids(floorIndx)(lsp)(indx%3)<<endl;
	  }
	}
      }
      else if(scmScenario==_SCM5G_INDOOR_OFFICE_)
      {
	int size = 2; // LOS , NLOS, O2I
	for(int lsp = 0; lsp < 7 ; lsp++) // DS,ASD,ASA,SF,K,ZSA,ZSD
	{
	  scmGrids(floorIndx)(lsp).set_length(size);
	  for(int indx = 9;indx<11;indx++) // LOS, NLOS, O2I
	  {
	    rowCount = (ceil) ((scmInfo.coverageArea.yLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
	    colCount = (ceil) ((scmInfo.coverageArea.xLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
	    if(lsp == 4 && (indx == 10)) // For K --->(NLOS and O2I)
	    {
	      rowCount = 0; colCount = 0;
	    }
	    scmGrids(floorIndx)(lsp)(indx%3) = randn(rowCount,colCount);
	  }
	}
      }
    }//end for floorIndx
  }
  else if(scmScenario==_SCM5G_INDOOR_FACTORY_)
  {
      initGridsFor5GSCMLargeScaleParameters(scmGrids,&scmInfo,scmScenario,infScenario);
  }
  else
  {
    cout<<"Unsupported Terrain for initGridsForSCMLargeScaleParameters()..."<<endl; abort();
  }
}

void initGridsForSpatialConsistencyParameters(Array< Array<mat> >  &scmGridsForSpConsist,SCMInfo_S &scmInfo,SCMPropagationScenario scmScenario){
  
    /* Assumed hexagonal grid layout for generation of random values for spatialConsistency parameters
     *  _ _ _ _
     *  |_|_|_|  0 Cluster& ray spacific random variables
     *  |_|_|_|  1 LOS/NLOS state
     *  |_|_|_|  2 Indoor/outdoor state
     *   0 1 2  
     *  
     */
  if(scmScenario==_SCM5G_URBAN_MICRO_ || scmScenario== _SCM5G_URBAN_MACRO_ ||scmScenario== _SCM5G_RURAL_MACRO_ ||scmScenario== _SCM5G_INDOOR_OFFICE_||scmScenario==_SCMNTN_RURAL_ ) //5G Channel Model
    {
        // Assumed 2D random process (in horizontal plane only) as per TR38.901 
        scmGridsForSpConsist.set_length(3);  // scmGridsForSpConsist(parameter)(indoo/outdoo.o2i)(grid)
        // scp =  spacial Consistency parameters
        int rowCount, colCount;
        if(scmScenario==_SCM5G_URBAN_MICRO_)
        {
            for(int scp = 0; scp < 3 ; scp++) // Cluster& ray spacific random variables,LOS/NLOS state,Indoor/outdoor state
            {
                if(scp == 0) // only for Cluster& ray spacific random variables
                    scmGridsForSpConsist(scp).set_length(3);// LOS, NLOS, O2I
                    else
                        scmGridsForSpConsist(scp).set_length(1);
                    
                    if(scp == 0) // only for Cluster& ray spacific random variables
                    {
                        for(int indx = 0;indx<3;indx++) // LOS, NLOS, O2I
                        {
                            rowCount = (ceil) ((scmInfo.coverageArea.yLen/(3*scmInfo.dcorrForSPConsistency(scp/2,indx))) + 1);
                            colCount = (ceil) ((scmInfo.coverageArea.xLen/(3*scmInfo.dcorrForSPConsistency(scp/2,indx))) + 1);
                            scmGridsForSpConsist(scp)(indx%3) = randn(rowCount,colCount);	
                        }
                    }
                    else
                    {
                        rowCount = (ceil) ((scmInfo.coverageArea.yLen/(3*scmInfo.dcorrForSPConsistency(scp/2,0))) + 1);
                        colCount = (ceil) ((scmInfo.coverageArea.xLen/(3*scmInfo.dcorrForSPConsistency(scp/2,0))) + 1);
                        scmGridsForSpConsist(scp)(0) = randn(rowCount,colCount);
                        break;
                    }
            }
            
        }
        else if(scmScenario==_SCM5G_URBAN_MACRO_)
        {
            for(int scp = 0; scp < 3 ; scp++) // Cluster& ray spacific random variables,LOS/NLOS state,Indoor/outdoor state
            {
                if(scp == 0) // only for Cluster& ray spacific random variables
                    scmGridsForSpConsist(scp).set_length(3);// LOS, NLOS, O2I
                    else
                        scmGridsForSpConsist(scp).set_length(1);
                    
                    if(scp == 0) // only for Cluster& ray spacific random variables
                    {
                        for(int indx = 3;indx<6;indx++) // LOS, NLOS, O2I
                        {
                            rowCount = (ceil) ((scmInfo.coverageArea.yLen/(3*scmInfo.dcorrForSPConsistency(scp/2,indx))) + 1);
                            colCount = (ceil) ((scmInfo.coverageArea.xLen/(3*scmInfo.dcorrForSPConsistency(scp/2,indx))) + 1);
                            scmGridsForSpConsist(scp)(indx%3) = randu(rowCount,colCount);
                        }
                    }
                    else
                    {
                        rowCount = (ceil) ((scmInfo.coverageArea.yLen/(3*scmInfo.dcorrForSPConsistency(scp/2,3))) + 1);
                        colCount = (ceil) ((scmInfo.coverageArea.xLen/(3*scmInfo.dcorrForSPConsistency(scp/2,3))) + 1);
                        scmGridsForSpConsist(scp)(0) = randu(rowCount,colCount);
                        break;
                    }
            }
        } 
    else if((scmScenario==_SCM5G_RURAL_MACRO_)||(scmScenario== _SCMNTN_RURAL_))
        {
            for(int scp = 0; scp < 3 ; scp++) // Cluster& ray spacific random variables,LOS/NLOS state,Indoor/outdoor state
            {
                if(scp == 0) // only for Cluster& ray spacific random variables
                    scmGridsForSpConsist(scp).set_length(3);// LOS, NLOS, O2I
                    else
                        scmGridsForSpConsist(scp).set_length(1);
                    
                    if(scp == 0) // only for Cluster& ray spacific random variables
                    {
                        for(int indx = 6;indx<9;indx++) // LOS, NLOS, O2I
                        {
                            rowCount = (ceil) ((scmInfo.coverageArea.yLen/(3*scmInfo.dcorrForSPConsistency(scp,indx))) + 1);
                            colCount = (ceil) ((scmInfo.coverageArea.xLen/(3*scmInfo.dcorrForSPConsistency(scp,indx))) + 1);
                            scmGridsForSpConsist(scp)(indx%3) = randn(rowCount,colCount);	
                        }
                    }
                    else
                    {
                        rowCount = (ceil) ((scmInfo.coverageArea.yLen/(3*scmInfo.dcorrForSPConsistency(scp,6))) + 1);
                        colCount = (ceil) ((scmInfo.coverageArea.xLen/(3*scmInfo.dcorrForSPConsistency(scp,6))) + 1);
                        scmGridsForSpConsist(scp)(0) = randn(rowCount,colCount);
                        break;
                    }    
                    
                    
                    
            }
        }
        else if(scmScenario==_SCM5G_INDOOR_OFFICE_)
        {
            for(int scp = 0; scp < 2 ; scp++) // Cluster& ray spacific random variables,LOS/NLOS state
            {
                scmGridsForSpConsist(scp).set_length(1);
                
                rowCount = (ceil) ((scmInfo.coverageArea.yLen/(3*scmInfo.dcorrForSPConsistency(scp,9))) + 1);
                colCount = (ceil) ((scmInfo.coverageArea.xLen/(3*scmInfo.dcorrForSPConsistency(scp,9))) + 1);
                scmGridsForSpConsist(scp)(0) = randn(rowCount,colCount);
            }
        }
    }
    else
    {
        
        cout<<"Node.cpp....Unsupported Terrain for initGridsForSpatialConsistencyParameters()..."<<endl;abort();
    }
    
}

vec getZSDparams(NodeLinkInfo_S& linkInfo,SCMPropagationScenario scmScenario,ChannelModelType channelModelType,double fc,bool isDownlink,SCMLinkCondition linkCondition)
{
    double sigma,mu,muOffSetZOD,ueHeight,bsHeight,muZSD;
    double d2D = findDistance(linkInfo.rxLocation,linkInfo.txLocation);
    if(isDownlink)
    {
        ueHeight=linkInfo.rxLocation.z;
        bsHeight=linkInfo.txLocation.z;
    }
    else
    {
        ueHeight=linkInfo.txLocation.z;
        bsHeight=linkInfo.rxLocation.z;	
    }
    if(scmScenario==_SCM5G_URBAN_MICRO_) //UMi
    {
        if(fc<=6 && channelModelType == _MODEL_A_)
        {
            if(linkInfo.isLOS)//! LOS/LOS O-to_I
            {
                muOffSetZOD=0.0;
                sigma=0.40; 
                mu=max(-0.5,-2.1*(d2D/1000.0)+0.01*abs(ueHeight-bsHeight)+0.75);	
            }
            else //! ! NLOS/NLOS O_to_I
            {
                muOffSetZOD= -pow(10,(-0.55*log10(max(10.0,d2D))+1.6));
                sigma=0.60; 
                mu=max(-0.5,-2.1*(d2D/1000.0)+0.01*max(ueHeight-bsHeight,0.0)+0.9);	
                
            }
        }
        else 
        {
            if(linkInfo.isLOS)//! LOS/LOS O-to_I
            {
                muOffSetZOD=0.0;
                sigma=0.35;
                mu=max(-0.21,-14.8*(d2D/1000.0)-0.01*abs(ueHeight-bsHeight)+0.83) /*+ muOffSetZOD*/;		
            }
            else //! ! NLOS/NLOS O_to_I
            {
                muOffSetZOD= -pow(10,(-1.5*log10(max(10.0,d2D))+3.3));
                sigma=0.35; 
                mu=max(-0.5,-3.1*(d2D/1000.0)+0.01*max(ueHeight-bsHeight,0.0)+0.2) /*+ muOffSetZOD*/;	
            }
        }
        muZSD=mu;
    }
    else if(scmScenario==_SCM5G_URBAN_MACRO_) //UMa
    {
        if(linkInfo.isLOS)//! LOS/LOS O-to_I
        {
            muOffSetZOD=0.0;
            sigma=0.4; 
            mu=max(-0.5,-2.1*(d2D/1000.0)-0.01*(ueHeight-1.5)+0.75) /*+ muOffSetZOD*/;		
        }
        else //! ! NLOS/NLOS O_to_I
        {
            
            //FIXME CHECK_VALIDITY - Need for randon floor selection for UE need to be checked
            
            // 	    vec temp_Vec = "4 5 6 7 8";
            // 	    vec temp_Vec1 = randPick(temp_Vec,1);
            // 	    
            // 	    int Nff = temp_Vec1(0);	  
            // 	    vec temp_Vec2 = getVector(1,Nff,1);
            // 	    vec temp_Vec3 = randPick(temp_Vec2,1);
            // 	    int numberOfFloors = temp_Vec3(0); 
            // 	    ueHeight=3*(numberOfFloors -1) + 1.5; 
            
            //NLOS
            double e = 7.66*log10(fc)-5.96;
            double a = 0.208*log10(fc)-0.782;
            double b = 25;
            double c = -0.13*log10(fc)+2.03;
            
            if(channelModelType == _MODEL_A_)
            {
                if(fc<=6 && fc>=0.5)
                {
                    muOffSetZOD= -1*pow(10.0,0.62*log10(max(10.0,d2D))+1.93-0.07*(ueHeight-1.5));
                }
                else if(fc<=100 && fc>6)
                {
                    muOffSetZOD= e-1*pow(10.0,a*log10(max(b,d2D))+c);
                }
                else{
                    abort();
                }
            }
            else if(channelModelType == _MODEL_B_)
            {
                muOffSetZOD= e-1*pow(10.0,a*log10(max(b,d2D))+c-0.07*(ueHeight-1.5));
            }
            else
            {
                muOffSetZOD= e-1*pow(10.0,a*log10(max(b,d2D))+c-0.07*(ueHeight-1.5));
            }
            sigma=0.49; 
            mu=max(-0.5,-2.1*(d2D/1000.0)-0.01*(ueHeight-1.5)+0.9) /*+ muOffSetZOD*/;	    
        }	  
        muZSD=mu;
    }
    else if(scmScenario==_SCM5G_RURAL_MACRO_||scmScenario==_SCMNTN_RURAL_) //RMa
    {
        if(linkCondition == _O2I_)
        {
            muOffSetZOD= atan((35.0-3.5)/d2D)-atan((35.0-1.5)/d2D);
            sigma=0.30; 
            mu=max(-1.0,-0.19*(d2D/1000)-0.01*(ueHeight-1.5)+0.28);
        }
        else
        {
            if(linkInfo.isLOS)//! LOS/LOS O-to_I
            {
                muOffSetZOD=0.0;
                sigma=0.34; 
                mu= max(-1.0,-0.17*(d2D/1000)-0.01*(ueHeight-1.5)+0.22);	
            }
            else //! ! NLOS/NLOS O_to_I
            {
                muOffSetZOD= atan((35-3.5)/d2D)-atan((35-1.5)/d2D);
                sigma=0.30; 
                mu=max(-1.0,-0.19*(d2D/1000)-0.01*(ueHeight-1.5)+0.28);
            }
        }	  
        muZSD=mu;
    }
    else if(scmScenario==_SCM5G_INDOOR_OFFICE_) //InH
    {
        if(linkCondition == _LOS_)//! LOS/LOS O-to_I
        {
            if(fc<=6 && channelModelType == _MODEL_A_)
            {
                muOffSetZOD=0.0;
                sigma=0.41; 
                mu=1.02;	
            }
            else
            {
                muOffSetZOD=0.0;
                #ifdef _USING_CALIBRATION_
                mu=-1.43*log10(1+fc)+2.25; 
                sigma=0.13*log10(1+fc)+0.15;
                #else
                mu=-1.43*log10(1+fc)+2.228; 
                sigma=0.13*log10(1+fc)+0.30;
                #endif
            }	    	
        }
        else //! ! NLOS/NLOS O_to_I
        {	    
            muOffSetZOD = 0.0;
            #ifdef _USING_CALIBRATION_
            mu = 1.37;sigma = 0.38;
            #else
            mu = 1.08;
            sigma = 0.36;
            #endif
        }	  
        muZSD=mu;
    }
    else if(scmScenario==_SCM5G_INDOOR_FACTORY_) //InF
    {
        muOffSetZOD=0.0;
        if(linkCondition == _LOS_)
        {
            mu = 1.35;sigma = 0.35;
        }
        else
        {
            mu = 1.20;sigma = 0.55; 
        }
        muZSD = mu;
    }
    else
    {
        cout<<"UnKnown Propagation scenario..."<<endl;abort();
    }
    vec zsdParams = getVectorWithNumbers(4,mu,sigma,muOffSetZOD,muZSD);
    return zsdParams;
}

double rho(double norm_d)
{
   return exp(-norm_d);
}
vec rho(vec norm_d)
{
    vec r(norm_d.length());
    for(int i=0;i<norm_d.length();i++)
        r(i) = rho(norm_d(i));
    return r;
}

double getSigmaSFForInF(InFScenario_E infScenario)
{
   double sigmaSF = 0.0;
   #ifdef _USING_CALIBRATION_
   switch(infScenario)
   {
       case _INF_SL_ : sigmaSF = 5.7; break;    //sigmaSF
       case _INF_DL_ : sigmaSF = 7.3; break;
       case _INF_SH_ : sigmaSF = 5.4; break;
       case _INF_DH_ : sigmaSF = 3.6; break;
       case _INF_HH_ : sigmaSF = 0.0; break;
       default : cout<<"Unknown InF Scenario..."<<endl;abort();break;
   }
   #else
   switch(infScenario)
   {
       case _INF_SL_ : sigmaSF = 5.7; break;    //sigmaSF
       case _INF_DL_ : sigmaSF = 7.2; break;
       case _INF_SH_ : sigmaSF = 5.9; break;
       case _INF_DH_ : sigmaSF = 4.0; break;
       case _INF_HH_ : sigmaSF = 0.0; break;
       default : cout<<"Unknown InF Scenario..."<<endl;abort();break;
   }
   #endif
   return sigmaSF;
}

mat getlspMuSigmaOfInFForCalibration(Area_S coverageArea,double fc,InFScenario_E infScenario)
{
    mat lspMuSigma(12,2);
   int r=0;
   double lfc = log10(1+fc);
   double V = coverageArea.getVolume();
   double S = coverageArea.getSurfaceArea(1);
   double V_S_ratio = (V/S);
   
   //       LOS                                     NLOS   
   double sigmaDS = 0.18,muDS = log10(70*V_S_ratio+10)-9-2.33*sigmaDS;
   
   lspMuSigma(r,0) = muDS+log10(0.76);       lspMuSigma(r++,1) = muDS; //DS
   lspMuSigma(r,0) = sigmaDS;                lspMuSigma(r++,1) = sigmaDS;
   lspMuSigma(r,0) = 1.56;                   lspMuSigma(r++,1) = 1.57;            //ASD
   lspMuSigma(r,0) = 0.25;                   lspMuSigma(r++,1) = 0.2;
   lspMuSigma(r,0) = -0.18*lfc+1.78;         lspMuSigma(r++,1) = 1.72;            //ASA
   lspMuSigma(r,0) = 0.12*lfc+0.2;           lspMuSigma(r++,1) = 0.3;
   lspMuSigma(r,0) = 0.0;                    lspMuSigma(r++,1) = 0.0;              // SF
   lspMuSigma(r,0) = 4.32;                   lspMuSigma(r++,1) = getSigmaSFForInF(infScenario);
   lspMuSigma(r,0) = 7.0;                    lspMuSigma(r++,1) = -1;              // K
   lspMuSigma(r,0) = 8.0;                    lspMuSigma(r++,1) = -1;
   lspMuSigma(r,0) = -0.2*lfc+1.5;           lspMuSigma(r++,1) = -0.13*lfc+1.45;  //ZSA
   lspMuSigma(r,0) = 0.35;                   lspMuSigma(r++,1) = 0.45;
   
   return lspMuSigma;
}

mat getlspMuSigmaForInF(Area_S coverageArea,double fc,InFScenario_E infScenario)
{
   #ifdef _USING_CALIBRATION_ 
    return getlspMuSigmaOfInFForCalibration(coverageArea,fc,infScenario);
   #endif
   mat lspMuSigma(12,2);
   int r=0;
   double lfc = log10(1+fc);
   double V = coverageArea.getVolume();
   double S = coverageArea.getSurfaceArea(1);
   double V_S_ratio = (V/S);
   
   //       LOS                                     NLOS   
   lspMuSigma(r,0) = log10(26*V_S_ratio+14)-9.35;   lspMuSigma(r++,1) = log10(30*V_S_ratio+32)-9.44; //DS
   lspMuSigma(r,0) = 0.15;                          lspMuSigma(r++,1) = 0.19;
   lspMuSigma(r,0) = 1.56;                          lspMuSigma(r++,1) = 1.57;            //ASD
   lspMuSigma(r,0) = 0.25;                          lspMuSigma(r++,1) = 0.2;
   lspMuSigma(r,0) = -0.18*lfc+1.78;                lspMuSigma(r++,1) = 1.72;            //ASA
   lspMuSigma(r,0) = 0.12*lfc+0.2;                  lspMuSigma(r++,1) = 0.3;
   lspMuSigma(r,0) = 0.0;                           lspMuSigma(r++,1) = 0.0;              // SF
   lspMuSigma(r,0) = 4.32;                          lspMuSigma(r++,1) = getSigmaSFForInF(infScenario);
   lspMuSigma(r,0) = 7.0;                           lspMuSigma(r++,1) = -1;              // K
   lspMuSigma(r,0) = 8.0;                           lspMuSigma(r++,1) = -1;
   lspMuSigma(r,0) = -0.2*lfc+1.5;                  lspMuSigma(r++,1) = -0.13*lfc+1.45;  //ZSA
   lspMuSigma(r,0) = 0.35;                          lspMuSigma(r++,1) = 0.45;
   
   return lspMuSigma;
}


vec generateSpatialCorrelatedRandomVariables(Location_S nodeLocation,vec corrDist,Array< Array< Array<mat> > > &scmGrids,Area_S coverageArea,InFScenario_E infScenario,SCMLinkCondition linkCondition,vec normGridDist)
{
    dComplex normLocation  = dComplex(nodeLocation.x+coverageArea.xLen/2,nodeLocation.y+coverageArea.yLen/2); 
    
    int floorNumFor3D=nodeLocation.floorNumber;
    int nLSPs=(infScenario== _INF_UNDEF_) ? 7 : 8;
    vec lsp = zeros(nLSPs);// lsp(0)-->DS,lsp(1)-->ASD,lsp(2)-->ASA,lsp(3)-->SF,lsp(4)-->K,lsp(5)-->ZSA,lsp(6)-->ZSD
    if(!normGridDist.length())
    {
        normGridDist = 7.0*ones(nLSPs);
    }
    LSPParameter lsp_param;
    for(int lsp_cnt=0;lsp_cnt<nLSPs;lsp_cnt++)
    {
        lsp_param = LSPParameter(lsp_cnt);
        if(lsp_param == _K_ && (linkCondition == _NLOS_ || linkCondition == _O2I_)) //  For K-->  NLOS , O2I
            lsp(lsp_cnt)= 0.0;
        else
        {
            double dCorr = (infScenario!=_INF_UNDEF_ && lsp_param==_DELTA_TAU_) ? getCorrDistForDeltaTau(infScenario) : corrDist(lsp_cnt); 
            double gridDist = normGridDist(lsp_cnt)*dCorr;
            int rowIndx = floor_i(imag(normLocation)/(gridDist)); 
            int colIndx = floor_i(real(normLocation)/(gridDist));
            dComplex ni  = normLocation;
            cvec n =zeros_c(4);vec n_val = zeros(4);
            for(int i=0,index=0;i<2;i++)
            for(int j=0;j<2;j++,index++)
            {
                n(index) = dComplex((colIndx+j)*gridDist,(rowIndx+i)*gridDist);
                n_val(index) = scmGrids(floorNumFor3D)(lsp_cnt)(linkCondition)(rowIndx+i,colIndx+j);
            }
                
            vec r = exp(-1*abs(n-ni)/dCorr),c = sqrt(r);
            double sum_ri = sum(r), ri_ni = sum(elem_mult(c,n_val));
            
//             if(sum_ri<1)
//                 lsp(lsp_cnt) = ri_ni + sqrt(1-sum_ri)*randn();
//             else
                lsp(lsp_cnt) = ri_ni/sum_ri;
        }
    }

    return lsp;

}

vec generateUncorrelatedRandomVariablesForNTN(InFScenario_E infScenario,SCMLinkCondition linkCondition)
{ 
    int nLSPs=(infScenario== _INF_UNDEF_) ? 7 : 7;
    vec lsp = zeros(nLSPs);// lsp(0)-->DS,lsp(1)-->ASD,lsp(2)-->ASA,lsp(3)-->SF,lsp(4)-->K,lsp(5)-->ZSA,lsp(6)-->ZSD
    int count = 0;
    LSPParameter lsp_param;
    for(int lsp_cnt=0;lsp_cnt<nLSPs;lsp_cnt++)
    { 
        count++;
        lsp_param = LSPParameter(lsp_cnt);
        if(lsp_param == _K_ && (linkCondition == _NLOS_ || linkCondition == _O2I_)) //  For K-->  NLOS , O2I
            lsp(lsp_cnt)= 0.0;
        else
        { 
                lsp(lsp_cnt) = randn();
        }
    }
    return lsp;
}

vec generateSpatialCorrelatedRandomVariablesForNTN(Location_S nodeLocation,vec corrDist,Array< Array< Array<mat> > > &scmGrids,Area_S coverageArea,InFScenario_E infScenario,SCMLinkCondition linkCondition,vec normGridDist)
{ //cout<<"checkpoint01"<<endl;
  //calculate latitude and longitude using ecef coordinate
   int longitude =atan2(nodeLocation.x,nodeLocation.y);
   int p =sqrt(pow(nodeLocation.x,2)+pow(nodeLocation.y,2));
   int latitude =atan2(nodeLocation.z,p);
   //local coordinate axis at the given node(point) : determine local east and north directions at the node
   /*int east_x=-sin(longitude);
   int east_y=cos(longitude);
   int east_z=0;
   int north_x=-sin(latitude)*cos(longitude);
   int north_y=-sin(latitude)*sin(longitude);
   int north_z=cos(latitude);*/
   //to convert the shift to ecef
   int east_x=-sin(longitude)*(coverageArea.xLen/2);
   int east_y=cos(longitude)*(coverageArea.yLen/2);
   int north_x=-sin(latitude)*cos(longitude)*(coverageArea.xLen/2);
   int north_y=-sin(latitude)*sin(longitude)*(coverageArea.yLen/2);
   //calculate ecef shift using local east and north
   int delta_nodeLoc_x=east_x+north_x;
   int delta_nodeLoc_y=east_y+north_y;
   //cout<<"delta_nodeLoc_x :"<<delta_nodeLoc_x<<endl;
   //cout<<"delta_nodeLoc_y :"<<delta_nodeLoc_y<<endl;
   dComplex normLocation  = dComplex(delta_nodeLoc_x+coverageArea.xLen/2,delta_nodeLoc_y+coverageArea.yLen/2);
    //dComplex normLocation  = dComplex(nodeLocation.x+coverageArea.xLen/2,nodeLocation.y+coverageArea.yLen/2);
    //cout<<"scn grid"<<scmGrids<<endl;
    //cout<<"norm location"<<normLocation<<endl;
    //cout<<"nodeLocation x and y"<<nodeLocation.x<<" | "<<nodeLocation.y<<endl;
    //cout<<"coverageArea.xLen"<<coverageArea.xLen<<endl;
    //cout<<"coverageArea.yLen"<<coverageArea.yLen<<endl;
    int floorNumFor3D=nodeLocation.floorNumber;
    int nLSPs=(infScenario== _INF_UNDEF_) ? 7 : 8;
    vec lsp = zeros(nLSPs);// lsp(0)-->DS,lsp(1)-->ASD,lsp(2)-->ASA,lsp(3)-->SF,lsp(4)-->K,lsp(5)-->ZSA,lsp(6)-->ZSD
    if(!normGridDist.length())
    {
        normGridDist = 7.0*ones(nLSPs);
    }
    int count = 0;
    LSPParameter lsp_param;
    for(int lsp_cnt=0;lsp_cnt<nLSPs;lsp_cnt++)
    { //cout<<"checkpoint02"<<lsp<<endl;
    /*cout<<"scm_lsp_0"<<" : "<<scmGrids(0)(0)(0)(5761,17012)<<endl;
    cout<<"scm_lsp_1"<<" : "<<scmGrids(0)(1)(0)(11522,34025)<<endl;
    cout<<"scm_lsp_2"<<" : "<<scmGrids(0)(2)(0)(8230,24303)<<endl;
      if(count==0){*/
        count++;
       lsp_param = LSPParameter(lsp_cnt);
        if(lsp_param == _K_ && (linkCondition == _NLOS_ || linkCondition == _O2I_)) //  For K-->  NLOS , O2I
            lsp(lsp_cnt)= 0.0;
        else
        { 
            //cout<<"checkpoint03"<<lsp<<endl;
            double dCorr = (infScenario!=_INF_UNDEF_ && lsp_param==_DELTA_TAU_) ? getCorrDistForDeltaTau(infScenario) : corrDist(lsp_cnt);
            double gridDist = normGridDist(lsp_cnt)*dCorr;
            //cout<<"grid distance"<<gridDist<<endl;
            int rowIndx = floor_i(imag(normLocation)/(gridDist));
            int colIndx = floor_i(real(normLocation)/(gridDist));
            dComplex ni  = normLocation;
            cvec n =zeros_c(4);vec n_val = zeros(4);
            for(int i=0,index=0;i<2;i++)
            for(int j=0;j<2;j++,index++)
            {
                //cout <<"index"<<index<<endl;
                n(index) = dComplex((colIndx+j)*gridDist,(rowIndx+i)*gridDist);
                //cout<<"n(index)"<<n(index)<<endl;
                //cout<<"checkpoint04"<<lsp<<endl;
                //cout<<"scmgrids"<< floorNumFor3D <<" | "<< lsp_cnt <<" | "<< linkCondition <<" | "<< rowIndx <<" | "<<colIndx<<endl;
                //cout<<"scmgrid size"<<scmGrids.length()<<" | "<<scmGrids(0).length()<<" | "<<scmGrids(0)(0).length()<<" | "<<scmGrids(0)(0)(0).rows()<<" | "<<scmGrids(0)(0)(0).cols()<<endl;
                n_val(index) = scmGrids(floorNumFor3D)(lsp_cnt)(linkCondition)(rowIndx+i,colIndx+j);
                //cout<<"n_val(index)"<<n_val(index)<<endl;
                //cout<<"scm grid"<<scmGrids(floorNumFor3D)(lsp_cnt)(linkCondition).size()<<endl;
            }
            vec r = exp(-1*abs(n-ni)/dCorr),c = sqrt(r);
            double sum_ri = sum(r), ri_ni = sum(elem_mult(c,n_val));
            //cout<<"sum_ri : "<<sum_ri<<endl;
           if(sum_ri<1){
                /*if(lsp_cnt==0){
                  lsp(lsp_cnt) = ri_ni + sqrt(1-sum_ri)*randexp();
                }
                else if(lsp_cnt==5 || lsp_cnt ==6){
                  lsp(lsp_cnt)= ri_ni + sqrt(1-sum_ri)*randlap();
                }
                else*/
                lsp(lsp_cnt) = ri_ni + sqrt(1-sum_ri)*randn();
                //cout<<"lsp(lsp_cnt) : "<<lsp(lsp_cnt)<<endl;
            }
            else{
                lsp(lsp_cnt) = ri_ni/sum_ri;
        }}
        //cout<<"checkpoint05"<<lsp<<endl;
    /*}
    else
    {lspTransformIndex
      continue;}
    }*/
    }
    return lsp;
}

void computeLSPsForNTN(NodeLinkInfo_S& linkInfo,vec lsp,vec lspMuSigma,SCMPropagationScenario scmScenario,ChannelModelType channelModelType,SCMLinkCondition linkCondition,int isDownlink)
{   NTNSCMLink ntnlink;
    // for shadowFading (SF) only...
    random_device rd{};
    mt19937 gen{rd()};
    normal_distribution<double> normalDistributionVariable_shadowLoss{0,pow(10, 0.1*lspMuSigma(2*_SF_+1))};
    //normal_distribution<double> normalDistributionVariable_shadowLoss{0,pow(10, 0.1*lspMuSigma(2*_SF_+1))};
    //cout << "sigma : "<< normalDistributionVariable_shadowLoss{0,pow(10, 0.1*lspMuSigma(2*_SF_+1))} <<","<< pow(10, 0.1*lspMuSigma(2*_SF_+1))<<endl;
    //cout << "sigma : "<< lspMuSigma(2*_SF_+1)<<","<< pow(10, 0.1*lspMuSigma(2*_SF_+1))<<endl;
    // mean = 0; stddev = lspMuSigma(2*_SF_+1);
    //..............................
    double fc = linkInfo.linkCarrierFrequencyInHz/1e9;// link carrier frequecny in GHz
    //cout<<"carrier frequency :"<<fc<<endl;
    double muZSD = 0.0,muOffSetZOD = 0.0;
    //cout << "Sigma and linkInfo.isLOS : "<< lspMuSigma(2*_SF_+1)<<","<< linkInfo.isLOS<<endl;
    mat C_mxm = computeNTNCorrelationMatrices(scmScenario,linkInfo.elevationAngle,linkInfo.isLOS,isKaBand(linkInfo.linkCarrierFrequencyInHz));
    //cout<<"mat cmxm :"<<C_mxm<<endl;
    C_mxm = C_mxm.get_cols(lspTransformIndex(C_mxm.cols()));
    C_mxm = C_mxm.get_rows(lspTransformIndex(C_mxm.cols()));
    C_mxm = chol(C_mxm).T();//NOTE: The transpose is required since the chol() considers F'*F = X whereas theoritically F*F' = X needs to be considered. 
    //cout << "Calling lspTransformIndex with length = " << lsp.length() << endl; //santhosh
    Vec<int> idx = lspTransformIndex(lsp.length()); //santhosh
    //cout << "lspTransformIndex: " << idx << endl; //santhosh
    lsp = lsp(idx); //santhosh
    
    //lsp = lsp(lspTransformIndex(lsp.length()));
    //cout<<"Cmxm"<<C_mxm<<endl;
    //cout<<"lsp_length"<<lsp<<endl;
    if(linkCondition == _LOS_) //! since there are 7 large scale parameters
    {   lsp = C_mxm*lsp;
        //cout<<"lsp_LOS"<<lsp<<endl;
    }
    else
    {
        lsp.del(1);// K
        lsp = C_mxm*lsp;
        lsp.ins(1,0.0);
    }
    lsp = lsp(invTransformIndx(lsp.length()));
    
    for(int lsp_cnt=0;lsp_cnt<lsp.length();lsp_cnt++)
    {
        LSPParameter lsp_param = LSPParameter(lsp_cnt);
        double mu = (lsp_param!=_SF_) ? lspMuSigma(2*lsp_cnt) : 0.0;
        double sigma = lspMuSigma(2*lsp_cnt+1);
        
        if(lsp_param == _K_ && (linkCondition !=_LOS_)) // K in LSP
        {
            sigma = -1.0;
            mu = -100.0; //setting K equal to -100 dB which is close to zero.
        }

//         if(lsp_param == _K_)
//             cout<<"K u sigma : "<<linkInfo.rxID<<" , "<<sigma<<" , "<<mu<<endl;
        
        //cout<<"lsp(lsp_cnt)"<<lsp(lsp_cnt)<<endl;

        lsp(lsp_cnt) = sigma*lsp(lsp_cnt) + mu;
        /*if (lsp_cnt==0){
          double delayspread = ntnlink.getRMSDelaySpread(lsp(lsp_cnt));
             cout<<"delayspread"<<delayspread<<endl;
        }*/
        //cout << "mu : "<< mu<<endl;

   }
   //cout << "lsp : "<< lsp<<endl;
    muZSD = lspMuSigma(2*(int)_ZSD_);
    append(lsp,getVectorWithNumbers(2,muOffSetZOD,muZSD));
    //cout<<"muZSD and lsp"<<muZSD<<lsp<<endl;
    linkInfo.shadowLoss = -lsp(_SF_); //normalDistributionVariable_shadowLoss(gen); //santhosh
    //cout << "5G SF and NTN SF"<< -lsp(_SF_) << "," <<linkInfo.shadowLoss <<endl;
    //cout << "gen :" << gen <<"linkInfo.shadowLoss : "<< linkInfo.shadowLoss<<endl;
    linkInfo.scmLargeScaleParams.load(lsp);
    //cout<<"linkInfo.scmLargeScaleParams"<<linkInfo.scmLargeScaleParams.getVector()<<endl;




    if(isnan(sum(linkInfo.scmLargeScaleParams.getVector())))
    {
        cout<<linkInfo.scmLargeScaleParams.getVector()<<endl;
        cout<<"LSP cannot be NaN..."<<endl;abort();
    }
}




void computeLSPs(NodeLinkInfo_S& linkInfo,vec lsp,vec lspMuSigma,SCMPropagationScenario scmScenario,ChannelModelType channelModelType,SCMLinkCondition linkCondition,int isDownlink)
{
    double fc = linkInfo.linkCarrierFrequencyInHz/1e9; // link carrier frequecny in GHz
    double muZSD = 0.0,muOffSetZOD = 0.0;
    double lspDelTau = 0.0;
    if(scmScenario==_SCM5G_INDOOR_FACTORY_)
    {
        lspDelTau = lsp(7);
        lsp.set_length(7,true);
    }
    // NOTE: 
    // According to 3GPP TR 38.901 the order of LSPs is [SF,K,DS,ASD,ASA,ZSD,ZSA].
    // Here all the parameters are generated considering the order [DS,ASD,ASA,SF,K,ZSA,ZSD].
    // Hence the C_mxm and lsp is appropriately transformed according to 38.901 document for applying cross correlation. 
    // After cross-correlation is applied, the LSPs are arranged back to the original order i.e. [DS,ASD,ASA,SF,K,ZSA,ZSD].  
    mat C_mxm = compute5GChannelSiteCorrelationMatrices(scmScenario-6,linkCondition == _LOS_,linkCondition == _O2I_);
//     C_mxm = real(sqrtm(C_mxm));
    C_mxm = C_mxm.get_cols(lspTransformIndex(C_mxm.cols()));
    C_mxm = C_mxm.get_rows(lspTransformIndex(C_mxm.cols()));
    C_mxm = chol(C_mxm).T();//NOTE: The transpose is required since the chol() considers F'*F = X whereas theoritically F*F' = X needs to be considered. 
    lsp = lsp(lspTransformIndex(lsp.length()));
    if(linkCondition == _LOS_) //! since there are 7 large scale parameters
        lsp = C_mxm*lsp;
    else
    {
        lsp.del(1);// K
        lsp = C_mxm*lsp;
        lsp.ins(1,0.0);
    }
    lsp = lsp(invTransformIndx(lsp.length()));
    for(int lsp_cnt=0;lsp_cnt<lsp.length();lsp_cnt++)
    {
        LSPParameter lsp_param = LSPParameter(lsp_cnt);
        double mu=0 ;
        double sigma=0;
        if(lsp_param!=_ZSD_)
        {
            if(lsp_param!=_SF_)//SF in LSP
                mu = lspMuSigma(2*lsp_cnt); // DS mu, NLOS
            else
                mu = 0.0;
                
            if(lsp_param == _K_ && (linkCondition !=_LOS_)) // K in LSP
            {
                sigma = -1.0;
                mu = -100.0; //setting K equal to -100 dB which is close to zero.
            }
            else if(lsp_param == _SF_ && scmScenario == _SCM5G_RURAL_MACRO_ && linkCondition == _LOS_)
            {
                double d2D = findDistance(linkInfo.txLocation,linkInfo.rxLocation);
                double dBP = 2*pi*(linkInfo.linkCarrierFrequencyInHz/lightSpeed)*linkInfo.txLocation.z*linkInfo.rxLocation.z;
                sigma = (d2D<=dBP) ? 4.0 : 6.0;
            }
            else
                sigma = lspMuSigma(2*lsp_cnt+1);
        }
        else
        {
            vec zsdParams = getZSDparams(linkInfo,scmScenario,channelModelType,fc,isDownlink,linkCondition);
            mu = zsdParams(0),sigma = zsdParams(1),muOffSetZOD = zsdParams(2),muZSD=zsdParams(3);
        }
        lsp(lsp_cnt) = sigma*lsp(lsp_cnt) + mu;
    }
    append(lsp,getVectorWithNumbers(2,muOffSetZOD,muZSD));
    linkInfo.shadowLoss = -lsp(_SF_);
    linkInfo.scmLargeScaleParams.load(lsp);
    if(scmScenario==_SCM5G_INDOOR_FACTORY_)
    {
        linkInfo.scmLargeScaleParams.deltaTau = -7.5 + 0.4*lspDelTau; //Refer Table 7.6.9-1 in 3GPP TR 38.901
    }
    if(isnan(sum(linkInfo.scmLargeScaleParams.getVector())))
    {
        cout<<linkInfo.scmLargeScaleParams.getVector()<<endl;
        cout<<"LSP cannot be NaN..."<<endl;abort();
    }
    cout<<"lsp_5G"<<lsp<<endl;
}

void generateSCMLargeScaleParameters(NodeLinkInfo_S &linkInfo,Array< Array< Array<mat> > > &scmGrids,SCMPropagationScenario scmScenario,InFScenario_E infScenario, SCMInfo_S &scmInfo, Location_S nodeLocation, ChannelModelType channelModelType,bool isOutdoorToIndoor, int isDownlink)
{
  if(scmScenario==_SCM2D_URBAN_MACRO_ || scmScenario==_SCM2D_URBAN_MICRO_ || scmScenario==_SCM2D_RURAL_MACRO_ || scmScenario==_SCM2D_INDOOR_HOTSPOT_)
  {
      int losGridIndx;
      complex<double> normLocation;
      int rowIndx , colIndx;
      normLocation = complex< double >(nodeLocation.x+scmInfo.coverageArea.xLen/2, nodeLocation.y+scmInfo.coverageArea.yLen/2);
      linkInfo.isForSCM=true;
      if (scmScenario == _SCM2D_URBAN_MICRO_)
      {
	if( linkInfo.isLOS ==  true )
	  losGridIndx = 3;
	else
	  losGridIndx = 4;
	if (isOutdoorToIndoor)
	  losGridIndx = 5;
	
	vec  lsp;
	lsp.set_size(5) ;
	lsp.zeros();
	
	for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
	{
	  complex<double>  n[4] , ni ;
	  double d[4]   , r[4] , dcorr , n_val[4];
	  dcorr = scmInfo.correlationDistance(lsp_cnt,losGridIndx);
	  
	  if(lsp_cnt == 4 && (losGridIndx == 4 || losGridIndx ==5)) // islos - NLOS , O2I
	    lsp(lsp_cnt)= 0.0;
	  else
	  {
	    rowIndx = floor_i(imag(normLocation)/(5*dcorr)); // DS, NLOS
	    colIndx = floor_i(real(normLocation)/(5*dcorr));
	    
	    
	    ni  = normLocation;
	    n[0] = complex<double>(colIndx * 5*dcorr,(rowIndx+1)*5*dcorr);
	    n[1] = complex<double>(colIndx * 5*dcorr,rowIndx*5*dcorr);
	    n[2] = complex<double>((colIndx+1) * 5*dcorr,rowIndx*5*dcorr);
	    n[3] = complex<double>((colIndx+1) * 5*dcorr,(rowIndx+1)*5*dcorr);
	    
	    n_val[0] = scmGrids(0)(lsp_cnt)(losGridIndx%3)(rowIndx+1,colIndx);//NLOS,DS
	    n_val[1] = scmGrids(0)(lsp_cnt)(losGridIndx%3)(rowIndx,colIndx);//NLOS,DS
	    n_val[2] = scmGrids(0)(lsp_cnt)(losGridIndx%3)(rowIndx,colIndx+1);//NLOS,DS
	    n_val[3] = scmGrids(0)(lsp_cnt)(losGridIndx%3)(rowIndx+1,colIndx+1);//NLOS,DS
	    
	    for(int ii = 0;ii<4;ii++)
	    {
	      d[ii] = abs(ni-n[ii]);
	      r[ii] = exp(-d[ii]/dcorr);
	    }
	    
	    double sum_ri=0.0, ri_ni=0.0;
	    for(int ii=0;ii<4;ii++)
	    {
	      sum_ri+=r[ii]*r[ii];
	      ri_ni+=r[ii]*n_val[ii];
	    }
// 	    if(sum_ri<1)
// 	    {
// 	      lsp(lsp_cnt) = ri_ni+sqrt(1-sum_ri)*randn() ;
// 	    }
// 	    else
	    {
	      lsp(lsp_cnt) = ri_ni ;
	    }
	  }
	}
	int o2i_val,los_val;
	if (isOutdoorToIndoor)
	{
	  o2i_val = 1;
	  los_val = 0;
	}
	else
	{
	  o2i_val = 0;
	  if(linkInfo.isLOS)
	    los_val = 1;
	  else
	    los_val = 0;
	}
	
	int scenario= 1; // Urban_Micro
	mat C_mxm = IMT_computeSiteCorrelationMatrices(scenario,los_val,o2i_val);
	C_mxm = real(sqrtm(C_mxm));
	
	if(C_mxm.rows() == 5)
	  lsp = C_mxm * lsp;
	else
	{
	  vec lsp_tmp = lsp.mid(0,4);
	  lsp_tmp = C_mxm * lsp_tmp;
	  lsp.set_size(0);
	  lsp = lsp_tmp;
	  lsp.set_size(5,true);
	  lsp(4)=0;
	}
	for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
	{
	  double mu=0 ;
	  double sigma=0;
	  if(lsp_cnt!=3)//SF in LSP
	    mu = scmInfo.lspMuSigma(2*lsp_cnt,losGridIndx); // DS mu, NLOS
	    if(!(lsp_cnt==4 && losGridIndx%3!=0))
	      sigma = scmInfo.lspMuSigma(2*lsp_cnt+1,losGridIndx); // DS sigma, NLOS
	      if(lsp_cnt==4 && losGridIndx%3!=0)
		mu = 0.0;
	      lsp(lsp_cnt) = sigma*lsp(lsp_cnt)+mu;
	}
      
	linkInfo.scmLargeScaleParams.ds = lsp(0);
	linkInfo.scmLargeScaleParams.asd = lsp(1);
	linkInfo.scmLargeScaleParams.asa = lsp(2);
	linkInfo.scmLargeScaleParams.sf = linkInfo.shadowLoss = lsp(3);// SF from LSP
	linkInfo.scmLargeScaleParams.k = lsp(4);
      
      }
      
      else if (scmScenario == _SCM2D_URBAN_MACRO_)
      {
	if( linkInfo.isLOS ==  true )
	  losGridIndx = 6;
	else
	  losGridIndx = 7;
	
	vec  lsp;
	lsp.set_size(5) ;
	lsp.zeros();
	
	for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
	{
	  complex<double>  n[4] , ni ;
	  double d[4]   , r[4] , dcorr , n_val[4];
	  
	  dcorr = scmInfo.correlationDistance(lsp_cnt,losGridIndx);
	  
	  if(lsp_cnt == 4 && (losGridIndx == 7)) // islos - NLOS
	    lsp(lsp_cnt)= 0.0;
	  else
	  {
	    rowIndx = floor_i(imag(normLocation)/(5*dcorr)); // DS, NLOS
	    colIndx = floor_i(real(normLocation)/(5*dcorr));
	    
	    
	    ni  = normLocation;
	    n[0] = complex<double>(colIndx * 5*dcorr,(rowIndx+1)*5*dcorr);
	    n[1] = complex<double>(colIndx * 5*dcorr,rowIndx*5*dcorr);
	    n[2] = complex<double>((colIndx+1) * 5*dcorr,rowIndx*5*dcorr);
	    n[3] = complex<double>((colIndx+1) * 5*dcorr,(rowIndx+1)*5*dcorr);
	    n_val[0] = scmGrids(0)(lsp_cnt)(losGridIndx%3)(rowIndx+1,colIndx);//NLOS,DS
	    n_val[1] = scmGrids(0)(lsp_cnt)(losGridIndx%3)(rowIndx,colIndx);//NLOS,DS
	    n_val[2] = scmGrids(0)(lsp_cnt)(losGridIndx%3)(rowIndx,colIndx+1);//NLOS,DS
	    n_val[3] = scmGrids(0)(lsp_cnt)(losGridIndx%3)(rowIndx+1,colIndx+1);//NLOS,DS
	    
	    
	    
	    for(int ii = 0;ii<4;ii++)
	    {
	      d[ii] = abs(ni-n[ii]);
	      r[ii] = exp(-d[ii]/dcorr);
	    }
	    
	    double sum_ri=0.0, ri_ni=0.0;
	    for(int ii=0;ii<4;ii++)
	    {
	      sum_ri+=r[ii]*r[ii];
	      ri_ni+=r[ii]*n_val[ii];
	    }
// 	    if(sum_ri<1)
// 	    {
// 	      lsp(lsp_cnt) = ri_ni+sqrt(1-sum_ri)*randn() ;
// 	    }
// 	    else
	    {
	      lsp(lsp_cnt) = ri_ni ;
	    }
	  }
	}
	int o2i_val,los_val;
	{
	  o2i_val = 0;
	  if(linkInfo.isLOS)
	    los_val = 1;
	  else
	    los_val = 0;
	}
	
	int scenario= 0; // Urban_Macro
	mat C_mxm = IMT_computeSiteCorrelationMatrices(scenario,los_val,o2i_val);
	C_mxm = real(sqrtm(C_mxm));
	
	if(C_mxm.rows() == 5)
	  lsp = C_mxm * lsp;
	else
	{
	  vec lsp_tmp = lsp.mid(0,4);
	  lsp_tmp = C_mxm * lsp_tmp;
	  lsp.set_size(0);
	  lsp = lsp_tmp;
	  lsp.set_size(5,true);
	  lsp(4)=0;
	}
	
	for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
	{
	  double mu=0 ;
	  double sigma=0;
	  if(lsp_cnt!=3)//SF in LSP
	    mu = scmInfo.lspMuSigma(2*lsp_cnt,losGridIndx); // DS mu, NLOS
	    if(!(lsp_cnt==4 && losGridIndx%3!=0))
	      sigma = scmInfo.lspMuSigma(2*lsp_cnt+1,losGridIndx); // DS sigma, NLOS
	      if(lsp_cnt==4 && losGridIndx%3!=0) // K in LSP
		mu = 0.0;
	      lsp(lsp_cnt) = sigma*lsp(lsp_cnt)+mu;
	    
	}                            
	
	linkInfo.scmLargeScaleParams.ds = lsp(0);
	linkInfo.scmLargeScaleParams.asd = lsp(1);
	linkInfo.scmLargeScaleParams.asa = lsp(2);
	linkInfo.scmLargeScaleParams.sf = linkInfo.shadowLoss =  lsp(3);
	linkInfo.scmLargeScaleParams.k = lsp(4);
	
      }
      
      else if (scmScenario == _SCM2D_RURAL_MACRO_)
      {
	if( linkInfo.isLOS ==  true )
	  losGridIndx = 9;
	else
	  losGridIndx = 10;
	
	vec  lsp;
	lsp.set_size(5) ;
	lsp.zeros();
	
	for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
	{
	  complex<double>  n[4] , ni ;
	  double d[4]   , r[4] , dcorr , n_val[4];
	  
	  dcorr = scmInfo.correlationDistance(lsp_cnt,losGridIndx);
	  
	  if(lsp_cnt == 4 && (losGridIndx == 10)) // islos - NLOS
	    lsp(lsp_cnt)= 0.0;
	  else
	  {
	    rowIndx = floor_i(imag(normLocation)/(5*dcorr)); // DS, NLOS
	    colIndx = floor_i(real(normLocation)/(5*dcorr));
	    
	    
	    ni  = normLocation;
	    n[0] = complex<double>(colIndx * 5*dcorr,(rowIndx+1)*5*dcorr);
	    n[1] = complex<double>(colIndx * 5*dcorr,rowIndx*5*dcorr);
	    n[2] = complex<double>((colIndx+1) * 5*dcorr,rowIndx*5*dcorr);
	    n[3] = complex<double>((colIndx+1) * 5*dcorr,(rowIndx+1)*5*dcorr);
	    
	    n_val[0] = scmGrids(0)(lsp_cnt)(losGridIndx%3)(rowIndx+1,colIndx);//NLOS,DS
	    n_val[1] = scmGrids(0)(lsp_cnt)(losGridIndx%3)(rowIndx,colIndx);//NLOS,DS
	    n_val[2] = scmGrids(0)(lsp_cnt)(losGridIndx%3)(rowIndx,colIndx+1);//NLOS,DS
	    n_val[3] = scmGrids(0)(lsp_cnt)(losGridIndx%3)(rowIndx+1,colIndx+1);//NLOS,DS
	    
	    
	    for(int ii = 0;ii<4;ii++)
	    {
	      d[ii] = abs(ni-n[ii]);
	      r[ii] = exp(-d[ii]/dcorr);
	    }
	    
	    double sum_ri=0.0, ri_ni=0.0;
	    for(int ii=0;ii<4;ii++)
	    {
	      sum_ri+=r[ii]*r[ii];
	      ri_ni+=r[ii]*n_val[ii];
	    }
// 	    if(sum_ri<1)
// 	    {
// 	      lsp(lsp_cnt) = ri_ni+sqrt(1-sum_ri)*randn() ;
// 	    }
// 	    else
	    {
	      lsp(lsp_cnt) = ri_ni ;
	    }
	  }
	}
	int o2i_val,los_val;
	{
	  o2i_val = 0;
	  if(linkInfo.isLOS)
	    los_val = 1;
	  else
	    los_val = 0;
	}
	
	int scenario= 2; // Rural_Macro
	mat C_mxm = IMT_computeSiteCorrelationMatrices(scenario,los_val,o2i_val);
	C_mxm = real(sqrtm(C_mxm));
	
	if(C_mxm.rows() == 5)
	  lsp = C_mxm * lsp;
	else
	{
	  vec lsp_tmp = lsp.mid(0,4);
	  lsp_tmp = C_mxm * lsp_tmp;
	  lsp.set_size(0);
	  lsp = lsp_tmp;
	  lsp.set_size(5,true);
	  lsp(4)=0;
	}
	for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
	{
	  double mu=0 ;
	  double sigma=0;
	  if(lsp_cnt!=3)//SF in LSP
	    mu = scmInfo.lspMuSigma(2*lsp_cnt,losGridIndx); // DS mu, NLOS
      if(!(lsp_cnt==4 && losGridIndx%3!=0))
        sigma = scmInfo.lspMuSigma(2*lsp_cnt+1,losGridIndx); // DS sigma, NLOS
      if(lsp_cnt==4 && losGridIndx%3!=0) // K in LSP
		mu = 0.0;
	      lsp(lsp_cnt) = sigma*lsp(lsp_cnt)+mu;
	}
	
	linkInfo.scmLargeScaleParams.ds = lsp(0);
	linkInfo.scmLargeScaleParams.asd = lsp(1);
	linkInfo.scmLargeScaleParams.asa = lsp(2);
	linkInfo.scmLargeScaleParams.sf = linkInfo.shadowLoss =  lsp(3);
	linkInfo.scmLargeScaleParams.k = lsp(4);
	
      } 
      else if(scmScenario==_SCM2D_INDOOR_HOTSPOT_)
      {
	if( linkInfo.isLOS ==  true )
	  losGridIndx = 0;
	else
	  losGridIndx = 1;
	
	vec  lsp;
	lsp.set_size(5) ;
	lsp.zeros();
	
	for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
	{
	  complex<double>  n[4] , ni ;
	  double d[4]   , r[4] , dcorr , n_val[4];
	  
	  dcorr = scmInfo.correlationDistance(lsp_cnt,losGridIndx);
	  if(lsp_cnt == 4 && (losGridIndx == 1)) // islos - NLOS
	    lsp(lsp_cnt)= 0.0;
	  else
	  {
	    rowIndx = floor_i(imag(normLocation)/(5*dcorr)); // DS, NLOS
	    colIndx = floor_i(real(normLocation)/(5*dcorr));
	    
	    
	    ni  = normLocation;
	    n[0] = complex<double>(colIndx * 5*dcorr,(rowIndx+1)*5*dcorr);
	    n[1] = complex<double>(colIndx * 5*dcorr,rowIndx*5*dcorr);
	    n[2] = complex<double>((colIndx+1) * 5*dcorr,rowIndx*5*dcorr);
	    n[3] = complex<double>((colIndx+1) * 5*dcorr,(rowIndx+1)*5*dcorr);
	    n_val[0] = scmGrids(0)(lsp_cnt)(losGridIndx)(rowIndx+1,colIndx);//NLOS,DS
	    n_val[1] = scmGrids(0)(lsp_cnt)(losGridIndx)(rowIndx,colIndx);//NLOS,DS
	    n_val[2] = scmGrids(0)(lsp_cnt)(losGridIndx)(rowIndx,colIndx+1);//NLOS,DS
	    n_val[3] = scmGrids(0)(lsp_cnt)(losGridIndx)(rowIndx+1,colIndx+1);//NLOS,DS
	    
	    
	    
	    for(int ii = 0;ii<4;ii++)
	    {
	      d[ii] = abs(ni-n[ii]);
	      r[ii] = exp(-d[ii]/dcorr);
	    }
	    
	    double sum_ri=0.0, ri_ni=0.0;
	    for(int ii=0;ii<4;ii++)
	    {
	      sum_ri+=r[ii]*r[ii];
	      ri_ni+=r[ii]*n_val[ii];
	    }
// 	    if(sum_ri<1)
// 	    {
// 	      lsp(lsp_cnt) = ri_ni+sqrt(1-sum_ri)*randn() ;
// 	    }
// 	    else
	    {
	      lsp(lsp_cnt) = ri_ni ;
	    }
	  }
	}
	int o2i_val,los_val;
	{
	  o2i_val = 0;
	  if(linkInfo.isLOS)
	    los_val = 1;
	  else
	    los_val = 0;
	}

	int scenario= 3; // Indoor Hotspot
	mat C_mxm = IMT_computeSiteCorrelationMatrices(scenario,los_val,o2i_val);
	C_mxm = real(sqrtm(C_mxm));
	
	if(C_mxm.rows() == 5)
	  lsp = C_mxm * lsp;
	else
	{
	  vec lsp_tmp = lsp.mid(0,4);
	  lsp_tmp = C_mxm * lsp_tmp;
	  lsp.set_size(0);
	  lsp = lsp_tmp;
	  lsp.set_size(5,true);
	  lsp(4)=0;
	}
	
	for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
	{
	  double mu=0 ;
	  double sigma=0;
	  if(lsp_cnt!=3)//SF in LSP
	    mu = scmInfo.lspMuSigma(2*lsp_cnt,losGridIndx); // DS mu, NLOS
	  if(!(lsp_cnt==4 && (losGridIndx)!=0))
	    sigma = scmInfo.lspMuSigma(2*lsp_cnt+1,losGridIndx); // DS sigma, NLOS
	  if(lsp_cnt==4 && (losGridIndx)!=0) // K in LSP
	    mu = 0.0;
	  lsp(lsp_cnt) = sigma*lsp(lsp_cnt)+mu;
	    
	}                            

	linkInfo.scmLargeScaleParams.ds = lsp(0);
	linkInfo.scmLargeScaleParams.asd = lsp(1);
	linkInfo.scmLargeScaleParams.asa = lsp(2);
	linkInfo.scmLargeScaleParams.sf = linkInfo.shadowLoss =  lsp(3);
	linkInfo.scmLargeScaleParams.k = lsp(4);
      }
  }//End for 2D SCM
  else if(scmScenario==_SCM3D_URBAN_MICRO_ || scmScenario==_SCM3D_URBAN_MACRO_ ) //3D Channel Model
  {

    if(isDownlink==-1)// isDownlink=-1 by default
      {cout<<"3D MIMO Channel Model can not be used for Cross-Links"<<endl;abort();}
      
      int floorNumFor3D=nodeLocation.floorNumber;
      double  muOffSetZOD =0.0;
      double muZSD;
      int losGridIndx;
      complex<double> normLocation;
      int rowIndx , colIndx;
      normLocation = complex< double >(nodeLocation.x+scmInfo.coverageArea.xLen/2, nodeLocation.y+scmInfo.coverageArea.yLen/2);
      linkInfo.isForSCM = true;
    
      double bsHeight,ueHeight;
    
      if (scmScenario == _SCM3D_URBAN_MICRO_)
      {
	if( linkInfo.isLOS ==  true )
	  losGridIndx = 0;
	else
	  losGridIndx = 1;
	if (isOutdoorToIndoor)
	  losGridIndx = 2;
	
	vec  lsp;
	lsp.set_size(7) ;// lsp(0)-->DS,lsp(1)-->ASD,lsp(2)-->ASA,lsp(3)-->SF,lsp(4)-->K,lsp(5)-->ZSA,lsp(6)-->ZSD
	lsp.zeros();
	
	for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
	{
	  complex<double>  n[4] , ni ;
	  double d[4]   , r[4] , dcorr , n_val[4];
	  dcorr = scmInfo.correlationDistance(lsp_cnt,losGridIndx);
	
	  
	  if(lsp_cnt == 4 && (losGridIndx == 1 || losGridIndx ==2)) // For K-->  NLOS , O2I
	    lsp(lsp_cnt)= 0.0;
	  else
	  {
	    rowIndx = floor_i(imag(normLocation)/(7*dcorr)); // DS, NLOS
	    colIndx = floor_i(real(normLocation)/(7*dcorr));
	    
	    
	    ni  = normLocation;
	    n[0] = complex<double>(colIndx * 7*dcorr,(rowIndx+1)*7*dcorr);
	    n[1] = complex<double>(colIndx * 7*dcorr,rowIndx*7*dcorr);
	    n[2] = complex<double>((colIndx+1) * 7*dcorr,rowIndx*7*dcorr);
	    n[3] = complex<double>((colIndx+1) * 7*dcorr,(rowIndx+1)*7*dcorr);	    
	    
	    n_val[0] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx+1,colIndx); 
	    n_val[1] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx,colIndx); 
	    n_val[2] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx,colIndx+1); 
	    n_val[3] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx+1,colIndx+1); 
	    
	    for(int ii = 0;ii<4;ii++)
	    {
	      d[ii] = abs(ni-n[ii]);
	      r[ii] = exp(-d[ii]/dcorr);
	    }
	    
	    double sum_ri=0.0, ri_ni=0.0;
	    for(int ii=0;ii<4;ii++)
	    {
	      sum_ri+=r[ii]*r[ii];
	      ri_ni+=r[ii]*n_val[ii];
	    }
// 	    if(sum_ri<1)
// 	    {
// 	      lsp(lsp_cnt) = ri_ni+sqrt(1-sum_ri)*randn() ;
// 	    }
// 	    else
	    {
	      lsp(lsp_cnt) = ri_ni ;
	    }
	  }
	}
	
	int o2i_val,los_val;
	if (isOutdoorToIndoor)
	{
	  o2i_val = 1;
	  los_val = 0;
	}
	else
	{
	  o2i_val = 0;
	  if(linkInfo.isLOS)
	    los_val = 1;
	  else
	    los_val = 0;
	}
	
	int scenario= 0; // Urban_Micro
	mat C_mxm = compute3DChannelSiteCorrelationMatrices(scenario,los_val,o2i_val);
	C_mxm = real(sqrtm(C_mxm));	
	if(C_mxm.rows() == 7) //! since there are 7 large scale parameters
	  lsp = C_mxm * lsp;
	else
	{
	  lsp.del(4);//! fourth index element is K
	  vec lsp_tmp = lsp;
	  lsp_tmp = C_mxm * lsp_tmp;
	  lsp.set_size(0);
	  lsp.set_size(7,true);
	  int temp_K=0;
	  for(int lspIndx=0;lspIndx<lsp_tmp.length();lspIndx++)
	  {
	       if(lspIndx == 4){lsp(lspIndx)=0,temp_K=1;}
	        lsp(lspIndx+temp_K)=lsp_tmp(lspIndx);
	  }

	}
	for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
	{
	  
	  double mu=0 ;
	  double sigma=0;
	  
	  if(lsp_cnt != 6)
	  {
	      if(lsp_cnt!=3)//SF in LSP
		mu = scmInfo.lspMuSigma(2*lsp_cnt,losGridIndx); // DS mu, NLOS
	      if(!(lsp_cnt==4 && losGridIndx%3!=0))
		sigma = scmInfo.lspMuSigma(2*lsp_cnt+1,losGridIndx); // DS sigma, NLOS
	      if(lsp_cnt==4 && losGridIndx%3!=0) // K in LSP
		mu = 0.0;	 
	  }
	  else // if lsp is ZSD
	  {
	    	   
	      if(isDownlink)
	      {
		ueHeight=linkInfo.rxLocation.z;
		bsHeight=linkInfo.txLocation.z;		 
		
	      }
	      else if(!isDownlink)
	      {
		ueHeight=linkInfo.txLocation.z;
		bsHeight=linkInfo.rxLocation.z;	
		
	      }
	      else// isDownlink=-1 by default
	      {cout<<"3D MIMO Channel Model can not be used for Cross-Links"<<endl;abort();}        
	  
	  
	  
	  
	    double d2D = findDistance(linkInfo.rxLocation,linkInfo.txLocation);
	    if(!isOutdoorToIndoor)
	    {
		if(linkInfo.isLOS)
		{ 
		  muOffSetZOD=0.0; 
		  sigma=0.4; 
		  mu=max(-0.5,-2.1*(d2D/1000.0)+0.01*abs(ueHeight-bsHeight)+0.75) + muOffSetZOD;     
		  
		} 
		else //NLOS
		{ 
		  muOffSetZOD=-1*pow(10.0,-0.55*log10(max(10.0,d2D))+1.6); 
		  sigma=0.6; 
		  mu=max(-0.5,-2.1*(d2D/1000.0)+0.01*max(ueHeight-bsHeight,0.0)+0.9) + muOffSetZOD;	       
		  
		}
	    }
	    else //! Indoor
	    {
		if(linkInfo.isLOS)
		{ 
		  muOffSetZOD=0.0; 
		  sigma=0.4; 
		  mu=max(-0.5,-2.1*(d2D/1000.0)+0.01*abs(ueHeight-bsHeight)+0.75) + muOffSetZOD;		
		  
		} 
	      else //NLOS
		{ 
		  muOffSetZOD=-1*pow(10.0,-0.55*log10(max(10.0,d2D))+1.6); 
		  sigma=0.6; 
		  mu=max(-0.5,-2.1*(d2D/1000.0)+0.01*max(ueHeight-bsHeight,0.0)+0.9) + muOffSetZOD;        
		  
		  
		}
	    }
	    
	    muZSD=mu;
	    
	  } //! end for lsp==6
	      
	      lsp(lsp_cnt) = sigma*lsp(lsp_cnt)+mu;
	}
      
	linkInfo.scmLargeScaleParams.ds = lsp(0);
	linkInfo.scmLargeScaleParams.asd = lsp(1);
	linkInfo.scmLargeScaleParams.asa = lsp(2);
	linkInfo.scmLargeScaleParams.sf = linkInfo.shadowLoss = lsp(3);// SF from LSP
	linkInfo.scmLargeScaleParams.k = lsp(4);
	linkInfo.scmLargeScaleParams.zsa = lsp(5);
	linkInfo.scmLargeScaleParams.zsd = lsp(6);
	linkInfo.scmLargeScaleParams.muOffSetZOD = muOffSetZOD;
	linkInfo.scmLargeScaleParams.muZSD = muZSD;
      
      }      
      else if (scmScenario == _SCM3D_URBAN_MACRO_)
      {
	if( linkInfo.isLOS ==  true )
	  losGridIndx = 3;
	else
	  losGridIndx = 4;
	if (isOutdoorToIndoor)
	  losGridIndx = 5;
	
	vec  lsp;
	lsp.set_size(7);// lsp(0)-->DS,lsp(1)-->ASD,lsp(2)-->ASA,lsp(3)-->SF,lsp(4)-->K,lsp(5)-->ZSA,lsp(6)-->ZSD
	lsp.zeros();
	
	for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
	{
	  complex<double>  n[4] , ni ;
	  double d[4]   , r[4] , dcorr , n_val[4];
	  
	  dcorr = scmInfo.correlationDistance(lsp_cnt,losGridIndx);
	  
	  if(lsp_cnt == 4 && (losGridIndx == 4 || losGridIndx ==5)) // islos - NLOS
	    lsp(lsp_cnt)= 0.0;
	  else
	  {
	    rowIndx = floor_i(imag(normLocation)/(7*dcorr)); // DS, NLOS
	    colIndx = floor_i(real(normLocation)/(7*dcorr));
	    
	    
	    ni  = normLocation;
	    n[0] = complex<double>(colIndx * 7*dcorr,(rowIndx+1)*7*dcorr);
	    n[1] = complex<double>(colIndx * 7*dcorr,rowIndx*7*dcorr);
	    n[2] = complex<double>((colIndx+1) * 7*dcorr,rowIndx*7*dcorr);
	    n[3] = complex<double>((colIndx+1) * 7*dcorr,(rowIndx+1)*7*dcorr);
	    n_val[0] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx+1,colIndx);//NLOS,DS
	    n_val[1] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx,colIndx);//NLOS,DS
	    n_val[2] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx,colIndx+1);//NLOS,DS
	    n_val[3] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx+1,colIndx+1);//NLOS,DS
	    
	    
	    for(int ii = 0;ii<4;ii++)
	    {
	      d[ii] = abs(ni-n[ii]);
	      r[ii] = exp(-d[ii]/dcorr);
	    }
	    
	    double sum_ri=0.0, ri_ni=0.0;
	    for(int ii=0;ii<4;ii++)
	    {
	      sum_ri+=r[ii]*r[ii];
	      ri_ni+=r[ii]*n_val[ii];
	    }
// 	    if(sum_ri<1)
// 	    {
// 	      lsp(lsp_cnt) = ri_ni+sqrt(1-sum_ri)*randn() ;
// 	    }
// 	    else
	    {
	      lsp(lsp_cnt) = ri_ni ;
	    }
	  }
	}
	int o2i_val,los_val;
	if (isOutdoorToIndoor)
	{
	  o2i_val = 1;
	  los_val = 0;
	}
	else
	{
	  o2i_val = 0;
	  if(linkInfo.isLOS)
	    los_val = 1;
	  else
	    los_val = 0;
	}
// 	{
// 	  o2i_val = 0;
// 	  if(linkInfo.isLOS)
// 	    los_val = 1;
// 	  else
// 	    los_val = 0;
// 	}
	
	int scenario= 1; // Urban_Macro
	mat C_mxm = compute3DChannelSiteCorrelationMatrices(scenario,los_val,o2i_val);
	C_mxm = real(sqrtm(C_mxm));
	if(C_mxm.rows() == 7) //! since there are 7 large scale parameters
	  lsp = C_mxm * lsp;
	else
	{
	  lsp.del(4);//! fourth index element is K
	  vec lsp_tmp = lsp;
	  lsp_tmp = C_mxm * lsp_tmp;
	  lsp.set_size(0);	  
	  lsp.set_size(7,true);	  
	  int temp_K=0;
	  for(int lspIndx=0;lspIndx<lsp_tmp.length();lspIndx++)
	  {
	    if(lspIndx == 4){lsp(lspIndx)=0,temp_K=1;}
	      lsp(lspIndx+temp_K)=lsp_tmp(lspIndx);
	  }
	}
	
	for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
	{
	  double mu=0 ;
	  double sigma=0;	  
	  
	  if(lsp_cnt != 6)
	  {
	    if(lsp_cnt!=3)//SF in LSP
	      mu = scmInfo.lspMuSigma(2*lsp_cnt,losGridIndx); // DS mu, NLOS
	    if(!(lsp_cnt==4 && losGridIndx%3!=0))
	      sigma = scmInfo.lspMuSigma(2*lsp_cnt+1,losGridIndx); // DS sigma, NLOS
	    if(lsp_cnt==4 && losGridIndx%3!=0) // K in LSP
	      mu = 0.0;
	  }
	  else// if lsp is ZSD
	  {
	 
	      if(isDownlink)
	      {
		ueHeight=linkInfo.rxLocation.z;
		bsHeight=linkInfo.txLocation.z;	
	      }
	      else if(!isDownlink)
	      {
		ueHeight=linkInfo.txLocation.z;
		bsHeight=linkInfo.rxLocation.z;		
		
	      }
	      else// isDownlink=-1 by default
	      {cout<<"3D MIMO Channel Model can not be used for Cross-Links"<<endl;abort();}
	      
	      
	  
	     double d2D = findDistance(linkInfo.rxLocation,linkInfo.txLocation);
	      
	      if(!isOutdoorToIndoor)
	      {
		  if(linkInfo.isLOS)
		  { 
		    muOffSetZOD=0.0;
		    sigma=0.4; 
		    mu=max(-0.5,-2.1*(d2D/1000.0)-0.01*(ueHeight-1.5)+0.75) + muOffSetZOD;		
		    
		  } 
		  else //NLOS
		  { 
		    muOffSetZOD=-1*pow(10.0,-0.62*log10(max(10.0,d2D))+1.93-0.07*(ueHeight-1.5)); 
		    sigma=0.49; 
		    mu=max(-0.5,-2.1*(d2D/1000.0)-0.01*(ueHeight-1.5)+0.9) + muOffSetZOD;		
		    
		  }
	      }
	      else //! Indoor
	      {
		
		  vec temp_Vec = "4 5 6 7 8";
		  vec temp_Vec1 = randPick(temp_Vec,1);
		  
		  int Nff = temp_Vec1(0);	  
		  vec temp_Vec2 = getVector(1,Nff,1);
		  vec temp_Vec3 = randPick(temp_Vec2,1);
		  int numberOfFloors = temp_Vec3(0); 
		  ueHeight=3*(numberOfFloors -1) + 1.5; 
		
		  if(linkInfo.isLOS)
		  { 
		    muOffSetZOD=0.0;
		    sigma=0.4; 
		    mu=max(-0.5,-2.1*(d2D/1000.0)-0.01*(ueHeight-1.5)+0.75) + muOffSetZOD;		  
		    
		  } 
		else //NLOS
		  { 
		    muOffSetZOD=-1*pow(10.0,-0.62*log10(max(10.0,d2D))+1.93-0.07*(ueHeight-1.5));
		    sigma=0.49; 
		    mu=max(-0.5,-2.1*(d2D/1000.0)-0.01*(ueHeight-1.5)+0.9) + muOffSetZOD;	
		    
		  }
	      }
	      
	      muZSD=mu;	      
	      
	    } //! end for lsp==6
	  
	  lsp(lsp_cnt) = sigma*lsp(lsp_cnt)+mu;
	    
	}                            
	
	linkInfo.scmLargeScaleParams.ds = lsp(0);
	linkInfo.scmLargeScaleParams.asd = lsp(1);
	linkInfo.scmLargeScaleParams.asa = lsp(2);
	linkInfo.scmLargeScaleParams.sf = linkInfo.shadowLoss = lsp(3); 
	linkInfo.scmLargeScaleParams.k = lsp(4);
	linkInfo.scmLargeScaleParams.zsa = lsp(5);
	linkInfo.scmLargeScaleParams.zsd = lsp(6);
	linkInfo.scmLargeScaleParams.muOffSetZOD = muOffSetZOD;
	linkInfo.scmLargeScaleParams.muZSD = muZSD;
	
      }
  }
  else if(scmScenario==_SCM5G_URBAN_MICRO_ || scmScenario==_SCM5G_URBAN_MACRO_ || scmScenario==_SCM5G_RURAL_MACRO_ || scmScenario==_SCM5G_INDOOR_OFFICE_ || scmScenario == _SCM5G_INDOOR_FACTORY_) //5G Channel Model
  {
    double fc = linkInfo.linkCarrierFrequencyInHz/1e9; // link carrier frequecny in GHz
    
    if( (scmScenario==_SCM5G_URBAN_MACRO_ || scmScenario==_SCM5G_INDOOR_OFFICE_) && fc<=6)
    {
      fc = 6;  // TR 38.901 foot node after Table 7.5-6 Part-1: Channel model parameters UMa and indoor
    }
    else if(scmScenario==_SCM5G_URBAN_MICRO_ && fc<=2)
    {
      fc = 2;  //2 TR 38.901 foot node after Table 7.5-6 Part-1:  for UMi-Street Canyon
    }

    if(isDownlink==-1)// isDownlink=-1 by default
    {cout<<"5G MIMO Channel Model can not be used for Cross-Links"<<endl;abort();}
    
    int floorNumFor3D=nodeLocation.floorNumber;
    double  muOffSetZOD = 0.0;
    double muZSD;
    int losGridIndx;
    complex<double> normLocation;
    int rowIndx , colIndx;
    normLocation  = complex<double>(nodeLocation.x+scmInfo.coverageArea.xLen/2,nodeLocation.y+scmInfo.coverageArea.yLen/2); 
    linkInfo.isForSCM=true;
    
    double bsHeight,ueHeight;
    SCMLinkCondition linkCondition;
    if (scmScenario == _SCM5G_URBAN_MICRO_)
    {
      if( linkInfo.isLOS ==  true )
      {
	losGridIndx = 0;
	linkCondition = _LOS_;
      }
      else
      {
	losGridIndx = 1;
	linkCondition = _NLOS_;
      }
      
      if(isOutdoorToIndoor)
      {
	losGridIndx = 2;
	linkCondition = _O2I_;
      }
      
      vec  lsp;
      lsp.set_size(7) ;// lsp(0)-->DS,lsp(1)-->ASD,lsp(2)-->ASA,lsp(3)-->SF,lsp(4)-->K,lsp(5)-->ZSA,lsp(6)-->ZSD
      lsp.zeros();
      LSPParameter lsp_param;
      for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
      {
	lsp_param = LSPParameter(lsp_cnt);
	complex<double>  n[4] , ni ;
	double d[4]   , r[4] , dcorr , n_val[4];
	dcorr = scmInfo.correlationDistance(lsp_cnt,losGridIndx);
	
	if(lsp_param == _K_ && (linkCondition == _NLOS_ ||linkCondition == _O2I_)) //  For K-->  NLOS , O2I
	  lsp(lsp_cnt)= 0.0;
	else
	{
	  rowIndx = floor_i(imag(normLocation)/(7*dcorr)); // DS, NLOS
	  colIndx = floor_i(real(normLocation)/(7*dcorr));  
 
	  ni  = normLocation;
	  n[0] = complex<double>(colIndx * 7 * dcorr,(rowIndx+1) * 7 * dcorr);
	  n[1] = complex<double>(colIndx * 7*dcorr,rowIndx*7*dcorr);
	  n[2] = complex<double>((colIndx+1) * 7*dcorr,rowIndx*7*dcorr);
	  n[3] = complex<double>((colIndx+1) * 7*dcorr,(rowIndx+1)*7*dcorr);	    
	  
	  n_val[0] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx+1,colIndx); 
	  n_val[1] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx,colIndx); 
	  n_val[2] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx,colIndx+1); 
	  n_val[3] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx+1,colIndx+1); 
	  
	  for(int ii = 0;ii<4;ii++)
	  {
	    d[ii] = abs(ni-n[ii]);
	    r[ii] = exp(-d[ii]/dcorr);
	  }
	  
	  double sum_ri=0.0, ri_ni=0.0;
	  for(int ii=0;ii<4;ii++)
	  {
	    sum_ri+=r[ii]*r[ii];
	    ri_ni+=r[ii]*n_val[ii];
	  }
// 	  if(sum_ri<1)
// 	  {
// 	    lsp(lsp_cnt) = ri_ni+sqrt(1-sum_ri)*randn();
// 	  }
// 	  else
	  {
	    lsp(lsp_cnt) = ri_ni ;
	  }
	}
      }      
      int o2i_val,los_val;
      if (isOutdoorToIndoor)
      {
	o2i_val = 1;
	los_val = 0;
      }
      else
      {
	o2i_val = 0;
	if(linkInfo.isLOS)
	  los_val = 1;
	else
	  los_val = 0;
      }
      
      int scenario= 0; // Urban_Micro
      mat C_mxm = compute5GChannelSiteCorrelationMatrices(scenario,los_val,o2i_val);
      C_mxm = real(sqrtm(C_mxm));
      
      if(C_mxm.rows() == 7) //! since there are 7 large scale parameters
	lsp = C_mxm * lsp;
      else
      {
	lsp.del(4);//! fourth index element is K
	vec lsp_tmp = lsp;
	lsp_tmp = C_mxm * lsp_tmp;
	lsp.set_size(0);
	lsp.set_size(7,true);
	int temp_K=0;
	for(int lspIndx=0;lspIndx<lsp_tmp.length();lspIndx++)
	{
	  if(lspIndx == 4){lsp(lspIndx)=0,temp_K=1;}
	  lsp(lspIndx+temp_K)=lsp_tmp(lspIndx);
	}
	
      }
      for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
      {
	lsp_param = LSPParameter(lsp_cnt);
	double mu=0 ;
	double sigma=0;
	
	if(lsp_param != _ZSD_)
	{
	  if(lsp_param!=_SF_)//SF in LSP
	    mu = scmInfo.lspMuSigma(2*lsp_cnt,losGridIndx); // DS mu, NLOS
	    else
	      mu = 0.0;
	    
        if(lsp_param==_K_ && (linkCondition == _NLOS_ || linkCondition == _O2I_)) // K in LSP
	    {
	      sigma = -1.0;
	      mu = -1.0;
	    }
	    else if(lsp_param ==_SF_ && linkCondition !=_O2I_ && ((channelModelType==_MODEL_A_ && fc>6.0) || channelModelType == _MODEL_B_)) //FIXME verify this condition again.
	      sigma = linkInfo.shadowStdDev;
	    else
	      sigma = scmInfo.lspMuSigma(2*lsp_cnt+1,losGridIndx);
	}
	else // if lsp is ZSD
	{	  
	  if(isDownlink)
	  {
	    ueHeight=linkInfo.rxLocation.z;
	    bsHeight=linkInfo.txLocation.z;		 
	    
	  }
	  else if(!isDownlink)
	  {
	    ueHeight=linkInfo.txLocation.z;
	    bsHeight=linkInfo.rxLocation.z;	
	    
	  }
	  else// isDownlink=-1 by default
	  {cout<<"3D MIMO Channel Model can not be used for Cross-Links"<<endl;abort();}  
	  
	  double d2D = findDistance(linkInfo.rxLocation,linkInfo.txLocation);
      if(fc<=6 && channelModelType == _MODEL_A_)
	  {
	    if(linkInfo.isLOS)//! LOS/LOS O-to_I
	    {
	      muOffSetZOD=0.0;
	      sigma=0.40; 
	      mu=max(-0.5,-2.1*(d2D/1000.0)+0.01*abs(ueHeight-bsHeight)+0.75);	
	    }
	    else //! ! NLOS/NLOS O_to_I
	    {
	      muOffSetZOD= -pow(10,(-0.55*log10(max(10.0,d2D))+1.6));
	      sigma=0.60; 
	      mu=max(-0.5,-2.1*(d2D/1000.0)+0.01*max(ueHeight-bsHeight,0.0)+0.9);	
	      
	    }
	  }
	  else 
	  {
	    if(linkInfo.isLOS)//! LOS/LOS O-to_I
	    {
	      muOffSetZOD=0.0;
	      sigma=0.35;
	      mu=max(-0.21,-14.8*(d2D/1000.0)-0.01*abs(ueHeight-bsHeight)+0.83) /*+ muOffSetZOD */;		
	    }
	    else //! ! NLOS/NLOS O_to_I
	    {
	      muOffSetZOD= -pow(10,(-1.5*log10(max(10.0,d2D))+3.3));
	      sigma=0.35; 
	      mu=max(-0.5,-3.1*(d2D/1000.0)+0.01*max(ueHeight-bsHeight,0.0)+0.2) /*+ muOffSetZOD*/;	
	      
	    }
	    muZSD=mu;
	  }
	  
	} //! end for lsp==6
	
	lsp(lsp_cnt) = sigma*lsp(lsp_cnt)+mu;
      }
      
      linkInfo.scmLargeScaleParams.ds = lsp(0);  //-->DS      
      linkInfo.scmLargeScaleParams.asd = lsp(1); //-->ASD
      linkInfo.scmLargeScaleParams.asa = lsp(2); //-->ASA
      linkInfo.scmLargeScaleParams.sf = linkInfo.shadowLoss = lsp(3);// SF from LSP
      linkInfo.scmLargeScaleParams.k = lsp(4); //-->K
      linkInfo.scmLargeScaleParams.zsa = lsp(5); //-->ZSA
      linkInfo.scmLargeScaleParams.zsd = lsp(6); //)-->ZSD
      linkInfo.scmLargeScaleParams.muOffSetZOD = muOffSetZOD;
      linkInfo.scmLargeScaleParams.muZSD = muZSD;     

    }
    
    else if (scmScenario == _SCM5G_URBAN_MACRO_)
    {
      if( linkInfo.isLOS ==  true )
      {
	losGridIndx = 3;
	linkCondition = _LOS_;
      }
      else
      {
	losGridIndx = 4;
	linkCondition = _NLOS_;
      }
      
      if(isOutdoorToIndoor)
      {
	losGridIndx = 5;
	linkCondition = _O2I_;
      }
      
      vec  lsp;
      lsp.set_size(7);// lsp(0)-->DS,lsp(1)-->ASD,lsp(2)-->ASA,lsp(3)-->SF,lsp(4)-->K,lsp(5)-->ZSA,lsp(6)-->ZSD
      lsp.zeros();
      LSPParameter lsp_param;
      for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
      {
	lsp_param = LSPParameter(lsp_cnt);
	complex<double>  n[4] , ni ;
	double d[4]   , r[4] , dcorr , n_val[4];
	
	dcorr = scmInfo.correlationDistance(lsp_cnt,losGridIndx);
		
	if(lsp_param == _K_ && (linkCondition == _NLOS_ || linkCondition == _O2I_)) // islos - NLOS
	  lsp(lsp_cnt)= 0.0;
	else
	{
	  rowIndx = floor_i(imag(normLocation)/(7*dcorr)); // DS, NLOS
	  colIndx = floor_i(real(normLocation)/(7*dcorr));	  
	  
	  ni  = normLocation;
	  n[0] = complex<double>(colIndx * 7*dcorr,(rowIndx+1)*7*dcorr);
	  n[1] = complex<double>(colIndx * 7*dcorr,rowIndx*7*dcorr);
	  n[2] = complex<double>((colIndx+1) * 7*dcorr,rowIndx*7*dcorr);
	  n[3] = complex<double>((colIndx+1) * 7*dcorr,(rowIndx+1)*7*dcorr);
	  
	  n_val[0] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx+1,colIndx);//NLOS,DS
	  n_val[1] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx,colIndx);//NLOS,DS
	  n_val[2] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx,colIndx+1);//NLOS,DS
	  n_val[3] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx+1,colIndx+1);//NLOS,DS	  
	  
	  for(int ii = 0;ii<4;ii++)
	  {
	    d[ii] = abs(ni-n[ii]);
	    r[ii] = exp(-d[ii]/dcorr);
	  }
	  
	  double sum_ri=0.0, ri_ni=0.0;
	  for(int ii=0;ii<4;ii++)
	  {
	    sum_ri+=r[ii]*r[ii];
	    ri_ni+=r[ii]*n_val[ii];
	  }
// 	  if(sum_ri<1)
// 	  {
// 	    lsp(lsp_cnt) = ri_ni+sqrt(1-sum_ri)*randn();
// 	  }
// 	  else
	  {
	    lsp(lsp_cnt) = ri_ni ;
	  }
	}
      }
      
      int o2i_val,los_val;
      if (isOutdoorToIndoor)// Conditions are correct. do not change...
      {
	o2i_val = 1;
	los_val = 0;
      }
      else
      {
	o2i_val = 0;
	if(linkInfo.isLOS)
	  los_val = 1;
	else
	  los_val = 0;
      }
      int scenario= 1; // Urban_Macro
      mat C_mxm = compute5GChannelSiteCorrelationMatrices(scenario,los_val,o2i_val);
      C_mxm = real(sqrtm(C_mxm));
      if(C_mxm.rows() == 7) //! since there are 7 large scale parameters
	lsp = C_mxm * lsp;
      else
      {
	lsp.del(4);//! fourth index element is K
	vec lsp_tmp = lsp;
	lsp_tmp = C_mxm * lsp_tmp;
	lsp.set_size(0);	  
	lsp.set_size(7,true);	  
	int temp_K=0;
	for(int lspIndx=0;lspIndx<lsp_tmp.length();lspIndx++)
	{
	  if(lspIndx == 4){lsp(lspIndx)=0,temp_K=1;}
	  lsp(lspIndx+temp_K)=lsp_tmp(lspIndx);
	}
      }
      for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
      {
	lsp_param = LSPParameter(lsp_cnt);
	double mu=0 ;
	double sigma=0;   
	
	if(lsp_param != _ZSD_)
	{
	  if(lsp_param!=_SF_)//SF in LSP
	    mu = scmInfo.lspMuSigma(2*lsp_cnt,losGridIndx); // DS mu, NLOS
	  else
	    mu = 0.0;
	  
      if(lsp_param==_K_ && (linkCondition == _NLOS_ || linkCondition== _O2I_)) // K in LSP
	  {
	    sigma = -1.0;
	    mu = -1.0;
	  }
	  else if(lsp_param ==_SF_ && linkCondition !=_O2I_ && ((channelModelType==_MODEL_A_ && fc>6) || channelModelType==_MODEL_B_))//FIXME verify this condition again.
	    sigma = linkInfo.shadowStdDev;
	  else
	    sigma = scmInfo.lspMuSigma(2*lsp_cnt+1,losGridIndx);
	    
	}
	else// if lsp is ZSD
	{	  
	  if(isDownlink)
	  {
	    ueHeight=linkInfo.rxLocation.z;
	    bsHeight=linkInfo.txLocation.z;	
	  }
	  else if(!isDownlink)
	  {
	    ueHeight=linkInfo.txLocation.z;
	    bsHeight=linkInfo.rxLocation.z;	    
	  }
	  else// isDownlink=-1 by default
	  {cout<<"5G MIMO Channel Model can not be used for Cross-Links"<<endl;abort();}

	  double d2D = findDistance(linkInfo.rxLocation,linkInfo.txLocation);
	  
	  if(linkInfo.isLOS)//! LOS/LOS O-to_I
	  {
	    muOffSetZOD=0.0;
	    sigma=0.4; 
	    mu=max(-0.5,-2.1*(d2D/1000.0)-0.01*(ueHeight-1.5)+0.75) /*+ muOffSetZOD*/;		
	  }
	  else //! ! NLOS/NLOS O_to_I
	  {
	    
	    //FIXME CHECK_VALIDITY - Need for randon floor selection for UE need to be checked
	    
// 	    vec temp_Vec = "4 5 6 7 8";
// 	    vec temp_Vec1 = randPick(temp_Vec,1);
// 	    
// 	    int Nff = temp_Vec1(0);	  
// 	    vec temp_Vec2 = getVector(1,Nff,1);
// 	    vec temp_Vec3 = randPick(temp_Vec2,1);
// 	    int numberOfFloors = temp_Vec3(0); 
// 	    ueHeight=3*(numberOfFloors -1) + 1.5; 
	    
	  //NLOS
	    double e = 7.66*log10(fc)-5.96;
	    double a = 0.208*log10(fc)-0.782;
	    double b = 25;
	    double c = -0.13*log10(fc)+2.03;

	    if(channelModelType == _MODEL_A_)
	    {
	      if(fc<=6 && fc>=0.5)
	      {
		muOffSetZOD= -1*pow(10.0,-0.62*log10(max(10.0,d2D))+1.93-0.07*(ueHeight-1.5));
	      }
	      else if(fc<=100 && fc>6)
	      {
		muOffSetZOD= e-1*pow(10.0,a*log10(max(b,d2D))+c);
	      }
	      else{
		abort();
	      }
	    }
	    else if(channelModelType == _MODEL_B_)
	    {
	      muOffSetZOD= e-1*pow(10.0,a*log10(max(b,d2D))+c-0.07*(ueHeight-1.5));
	    }
	    else{
	      muOffSetZOD= e-1*pow(10.0,a*log10(max(b,d2D))+c-0.07*(ueHeight-1.5));
	    }
	    sigma=0.49; 
	    mu=max(-0.5,-2.1*(d2D/1000.0)-0.01*(ueHeight-1.5)+0.9) /*+ muOffSetZOD*/;	    
	  }	  
	  muZSD=mu;	      
	  
	} //! end for lsp==6
	
	lsp(lsp_cnt) = sigma*lsp(lsp_cnt)+mu;
// 	cout<<"sigma:"<<sigma<<"\t mu"<<mu<<endl;
	
      }                            
      
      linkInfo.scmLargeScaleParams.ds = lsp(0);
      linkInfo.scmLargeScaleParams.asd = lsp(1);
      linkInfo.scmLargeScaleParams.asa = lsp(2);
      linkInfo.scmLargeScaleParams.sf = linkInfo.shadowLoss = lsp(3); 
      linkInfo.scmLargeScaleParams.k = lsp(4);
      linkInfo.scmLargeScaleParams.zsa = lsp(5);
      linkInfo.scmLargeScaleParams.zsd = lsp(6);
      linkInfo.scmLargeScaleParams.muOffSetZOD = muOffSetZOD;
      linkInfo.scmLargeScaleParams.muZSD = muZSD;
      
      //calibration
      // print in file
//       if(traceValuePerTraceName["printSCMAngleSpread"]=="true")
//       {
//       string angleSpread= "angleSpread_ALL_LargeScale.txt";
//       std::ofstream AS_AODFile;
//       static bool isNewPrint=1;
//       if(isNewPrint)
//       {
// 	AS_AODFile.open(angleSpread.c_str(),ios::out);
// 	AS_AODFile<<"% DS"<<",\t ASD"<<",\t ASA"<<",\t ZSA"<<",\t ZSD"<<endl;
// 	isNewPrint=0;
//       }
//       else
//       {
// 	AS_AODFile.open(angleSpread.c_str(),ios::app);
//       }
//       
//       AS_AODFile<<double(0)<<",\t"<<lsp(0)<<",\t"<<lsp(1)<<",\t"<<lsp(2)<<",\t"<<lsp(5)<<",\t"<<lsp(6)<<endl;
//     }
    }
  
    else if (scmScenario == _SCM5G_RURAL_MACRO_)
    {
     
      if( linkInfo.isLOS ==  true )
      {
	  losGridIndx = 6;
	  linkCondition = _LOS_;
      }
      else
      {
	  losGridIndx = 7;
	  linkCondition = _NLOS_;
      }
      
      if(isOutdoorToIndoor)
      {
	losGridIndx = 8;
	linkCondition = _O2I_;
      }
      vec  lsp;
      lsp.set_size(7);// lsp(0)-->DS,lsp(1)-->ASD,lsp(2)-->ASA,lsp(3)-->SF,lsp(4)-->K,lsp(5)-->ZSA,lsp(6)-->ZSD
      LSPParameter lsp_param;
      lsp.zeros();
      
      for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
      {
	lsp_param = LSPParameter(lsp_cnt);
	complex<double>  n[4] , ni ;
	double d[4]   , r[4] , dcorr , n_val[4];
	
	dcorr = scmInfo.correlationDistance(lsp_cnt,losGridIndx);
	
	if(lsp_param == _K_ && (linkCondition == _NLOS_ ||linkCondition == _O2I_)) // islos - NLOS
	  lsp(lsp_cnt)= 0.0;
	else
	{
	  rowIndx = floor_i(imag(normLocation)/(7*dcorr)); // DS, NLOS
	  colIndx = floor_i(real(normLocation)/(7*dcorr));
	  
	  ni  = normLocation;
	  n[0] = complex<double>(colIndx * 7*dcorr,(rowIndx+1)*7*dcorr);
	  n[1] = complex<double>(colIndx * 7*dcorr,rowIndx*7*dcorr);
	  n[2] = complex<double>((colIndx+1) * 7*dcorr,rowIndx*7*dcorr);
	  n[3] = complex<double>((colIndx+1) * 7*dcorr,(rowIndx+1)*7*dcorr);
	  n_val[0] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx+1,colIndx);//NLOS,DS
	  n_val[1] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx,colIndx);//NLOS,DS
	  n_val[2] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx,colIndx+1);//NLOS,DS
	  n_val[3] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx+1,colIndx+1);//NLOS,DS	  
	  
	  for(int ii = 0;ii<4;ii++)
	  {
	    d[ii] = abs(ni-n[ii]);
	    r[ii] = exp(-d[ii]/dcorr);
	  }
	  
	  double sum_ri=0.0, ri_ni=0.0;
	  for(int ii=0;ii<4;ii++)
	  {
	    sum_ri+=r[ii]*r[ii];
	    ri_ni+=r[ii]*n_val[ii];
	  }
// 	  if(sum_ri<1)
// 	  {
// 	    lsp(lsp_cnt) = ri_ni+sqrt(1-sum_ri)*randn() ;
// 	  }
// 	  else
	  {
	    lsp(lsp_cnt) = ri_ni ;
	  }
	}
      }
      
      int o2i_val,los_val;
      if (isOutdoorToIndoor)
      {
	o2i_val = 1;
      }
      else
      {
	o2i_val = 0;
      }
      if(linkInfo.isLOS)
	los_val = 1;
      else
	los_val = 0;
      int scenario= 2; // rural_macro
      mat C_mxm = compute5GChannelSiteCorrelationMatrices(scenario,los_val,o2i_val);
      C_mxm = real(sqrtm(C_mxm));    
      if(C_mxm.rows() == 7) //! since there are 7 large scale parameters
	lsp = C_mxm * lsp;
      else
      {
	lsp.del(4);//! fourth index element is K
	vec lsp_tmp = lsp;
	lsp_tmp = C_mxm * lsp_tmp;
	lsp.set_size(0);	  
	lsp.set_size(7,true);	  
	int temp_K=0;
	for(int lspIndx=0;lspIndx<lsp_tmp.length();lspIndx++)
	{
	  lsp_param =  LSPParameter(lspIndx);
	  if(lsp_param == _K_){lsp(lspIndx)=0,temp_K=1;}
	  lsp(lspIndx+temp_K)=lsp_tmp(lspIndx);
	}
	
      }
      
      for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
      {
	lsp_param = LSPParameter(lsp_cnt);
	double mu=0 ;
	double sigma=0;	  
	
	if(lsp_param != _ZSD_)
	{
	  if(lsp_param!=_SF_)//SF in LSP
	    mu = scmInfo.lspMuSigma(2*lsp_cnt,losGridIndx); // DS mu, NLOS
	  else
	    mu = 0.0;
	  
      if(lsp_param==_K_ && (linkCondition == _NLOS_ || linkCondition== _O2I_)) // K in LSP
	  {
	    sigma = -1.0;
	    mu = -1.0;
	  }
	  else if(lsp_param ==_SF_ && linkCondition !=_O2I_)
	    sigma = linkInfo.shadowStdDev;
	  else
	    sigma = scmInfo.lspMuSigma(2*lsp_cnt+1,losGridIndx);
	 	  
	}
	else// if lsp is ZSD
	{	  
	  if(isDownlink)
	  {
	    ueHeight=linkInfo.rxLocation.z;
	    bsHeight=linkInfo.txLocation.z;	
	  }
	  else if(!isDownlink)
	  {
	    ueHeight=linkInfo.txLocation.z;
	    bsHeight=linkInfo.rxLocation.z;		
	    
	  }
	  else// isDownlink=-1 by default
	  {cout<<"5G MIMO Channel Model can not be used for Cross-Links"<<endl;abort();}
	  
	  double d2D = findDistance(linkInfo.rxLocation,linkInfo.txLocation);
	  
	  if(isOutdoorToIndoor)
	  {
	    muOffSetZOD= atan((35.0-3.5)/d2D)-atan((35.0-1.5)/d2D);
	    sigma=0.30; 
	    mu=max(-1.0,-0.19*(d2D/1000)-0.01*(ueHeight-1.5)+0.28); //+ muOffSetZOD;
	  }
	  else
	  {
	    if(linkInfo.isLOS)//! LOS/LOS O-to_I
	    {
	      muOffSetZOD=0.0;
	      sigma=0.34; 
	      mu= max(-1.0,-0.17*(d2D/1000)-0.01*(ueHeight-1.5)+0.22);	
	    }
	    else //! ! NLOS/NLOS O_to_I
	    {
	      
	      //FIXME CHECK_VALIDITY - Need for randon floor selection for UE need to be checked
	      
	      // 	    vec temp_Vec = "4 5 6 7 8";
	      // 	    vec temp_Vec1 = randPick(temp_Vec,1);
	      // 	    
	      // 	    int Nff = temp_Vec1(0);	  
	      // 	    vec temp_Vec2 = getVector(1,Nff,1);
	      // 	    vec temp_Vec3 = randPick(temp_Vec2,1);
	      // 	    int numberOfFloors = temp_Vec3(0); 
	      // 	    ueHeight=3*(numberOfFloors -1) + 1.5; 
	      // 	    
	      //NLOS
	      
	      muOffSetZOD= atan((35-3.5)/d2D)-atan((35-1.5)/d2D);
	      sigma=0.30; 
	      mu=max(-1.0,-0.19*(d2D/1000)-0.01*(ueHeight-1.5)+0.28);// + muOffSetZOD;	
	      
	    }
	  }
	  muZSD=mu;		  
	} //! end for lsp==6
	
	lsp(lsp_cnt) = sigma*lsp(lsp_cnt)+mu;
	
      }    
      linkInfo.scmLargeScaleParams.ds = lsp(0);
      linkInfo.scmLargeScaleParams.asd = lsp(1);
      linkInfo.scmLargeScaleParams.asa = lsp(2);
      linkInfo.scmLargeScaleParams.sf = linkInfo.shadowLoss = lsp(3); 
      linkInfo.scmLargeScaleParams.k = lsp(4);
      linkInfo.scmLargeScaleParams.zsa = lsp(5);
      linkInfo.scmLargeScaleParams.zsd = lsp(6);
      linkInfo.scmLargeScaleParams.muOffSetZOD = muOffSetZOD;
      linkInfo.scmLargeScaleParams.muZSD = muZSD;
      
    }
    else if (scmScenario == _SCM5G_INDOOR_OFFICE_)
    {
      if( linkInfo.isLOS ==  true )
      {
	losGridIndx = 9;
	linkCondition = _LOS_;
      }
      else
      {
	losGridIndx = 10;
	linkCondition = _NLOS_;
      }
      
      vec  lsp;
      lsp.set_size(7);// lsp(0)-->DS,lsp(1)-->ASD,lsp(2)-->ASA,lsp(3)-->SF,lsp(4)-->K,lsp(5)-->ZSA,lsp(6)-->ZSD
      lsp.zeros();
      LSPParameter lsp_param;
      for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
      {
	lsp_param = LSPParameter(lsp_cnt);
	complex<double>  n[4] , ni ;
	double d[4]   , r[4] , dcorr , n_val[4];
	
	dcorr = scmInfo.correlationDistance(lsp_cnt,losGridIndx);
	
	if(lsp_param == _K_ && (linkCondition == _NLOS_)) // islos - NLOS
	  lsp(lsp_cnt)= 0.0;
	else
	{
	  rowIndx = floor_i(imag(normLocation)/(7*dcorr)); // DS, NLOS
	  colIndx = floor_i(real(normLocation)/(7*dcorr));	  
	  ni  = normLocation;
	  n[0] = complex<double>(colIndx * 7*dcorr,(rowIndx+1)*7*dcorr);
	  n[1] = complex<double>(colIndx * 7*dcorr,rowIndx*7*dcorr);
	  n[2] = complex<double>((colIndx+1) * 7*dcorr,rowIndx*7*dcorr);
	  n[3] = complex<double>((colIndx+1) * 7*dcorr,(rowIndx+1)*7*dcorr);	
	  
	  n_val[0] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx+1,colIndx);//NLOS,DS
	  n_val[1] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx,colIndx);//NLOS,DS
	  n_val[2] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx,colIndx+1);//NLOS,DS
	  n_val[3] = scmGrids(floorNumFor3D)(lsp_cnt)(losGridIndx%3)(rowIndx+1,colIndx+1);//NLOS,DS	  
	  
	  for(int ii = 0;ii<4;ii++)
	  {
	    d[ii] = abs(ni-n[ii]);
	    r[ii] = exp(-d[ii]/dcorr);
	  }
	  double sum_ri=0.0, ri_ni=0.0;
	  for(int ii=0;ii<4;ii++)
	  {
	    sum_ri+=r[ii]*r[ii];
	    ri_ni+=r[ii]*n_val[ii];
	  }
// 	  if(sum_ri<1)
// 	  {
// 	    lsp(lsp_cnt) = ri_ni+sqrt(1-sum_ri)*randn() ;
// 	  }
// 	  else
	  {
	    lsp(lsp_cnt) = ri_ni ;
	  }
	}
      }
      
      int o2i_val,los_val;
      o2i_val = 0;
      if(linkInfo.isLOS)
	los_val = 1;
      else
	los_val = 0;
      
      int scenario= 3; // Indoor
      mat C_mxm = compute5GChannelSiteCorrelationMatrices(scenario,los_val,o2i_val);
      C_mxm = real(sqrtm(C_mxm));    
      if(C_mxm.rows() == 7) //! since there are 7 large scale parameters
	lsp = C_mxm * lsp;
      else
      {
	lsp.del(4);//! fourth index element is K
	vec lsp_tmp = lsp;
	lsp_tmp = C_mxm * lsp_tmp;
	lsp.set_size(0);	  
	lsp.set_size(7,true);	  
	int temp_K=0;
	for(int lspIndx=0;lspIndx<lsp_tmp.length();lspIndx++)
	{
	  if(lspIndx == 4){lsp(lspIndx)=0,temp_K=1;}
	  lsp(lspIndx+temp_K)=lsp_tmp(lspIndx);
	}
      }       
      for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
      {
	lsp_param = LSPParameter(lsp_cnt);
	double mu=0 ;
	double sigma=0;   
	
	if(lsp_param != _ZSD_)
	{
	  if(lsp_param!=_SF_)//SF in LSP
	    mu = scmInfo.lspMuSigma(2*lsp_cnt,losGridIndx); // DS mu, NLOS
	    else
	      mu = 0.0;
	    
	    if(lsp_param==_K_ && (linkCondition == _NLOS_)) // K in LSP
	    {
	      sigma = -1.0;
	      mu = -1.0;
	    }
	    else if(lsp_param ==_SF_ && fc >6)
	      sigma = linkInfo.shadowStdDev;
	    else
	      sigma = scmInfo.lspMuSigma(2*lsp_cnt+1,losGridIndx);
	    
	}	
	else// if lsp is ZSD
	{	  
	  if(isDownlink)
	  {
	    ueHeight=linkInfo.rxLocation.z;
	    bsHeight=linkInfo.txLocation.z;	
	  }
	  else if(!isDownlink)
	  {
	    ueHeight=linkInfo.txLocation.z;
	    bsHeight=linkInfo.rxLocation.z;		
	    
	  }
	  else// isDownlink=-1 by default
	  {cout<<"5G MIMO Channel Model can not be used for Cross-Links"<<endl;abort();}
	  
	  double d2D = findDistance(linkInfo.rxLocation,linkInfo.txLocation);
	  
	  if(linkInfo.isLOS)//! LOS/LOS O-to_I
	  {
	    if(fc <=6 && channelModelType == _MODEL_A_)
	    {
	      muOffSetZOD=0.0;
	      sigma=0.41; 
	      mu=1.02;	
	    }
	    else{
	      muOffSetZOD=0.0;
	      mu=-1.43*log10(1+fc)+2.228; 
	      sigma=0.13*log10(1+fc)+0.30;	
	    }	    	
	  }
	  else //! ! NLOS/NLOS O_to_I
	  {	    
	    muOffSetZOD = 0.0;
        mu = 1.08;
	    sigma = 0.36; 	    
	  }	  
	  muZSD=mu;		  
	} //! end for lsp==6	
	lsp(lsp_cnt) = sigma*lsp(lsp_cnt)+mu;	
      } 
      if(std::isnan(lsp(0))) 
      {
	cout<<" linkInfo: "<< linkInfo.rxType<<"\n "<< linkInfo.txType<<endl;
	abort();
      }
      
      linkInfo.scmLargeScaleParams.ds = lsp(0);
      linkInfo.scmLargeScaleParams.asd = lsp(1);
      linkInfo.scmLargeScaleParams.asa = lsp(2);
      linkInfo.scmLargeScaleParams.sf = linkInfo.shadowLoss = lsp(3); 
      linkInfo.scmLargeScaleParams.k = lsp(4);
      linkInfo.scmLargeScaleParams.zsa = lsp(5);
      linkInfo.scmLargeScaleParams.zsd = lsp(6);
      linkInfo.scmLargeScaleParams.muOffSetZOD = muOffSetZOD;
      linkInfo.scmLargeScaleParams.muZSD = muZSD;     
    }
    else if (scmScenario == _SCM5G_INDOOR_FACTORY_)
    {
        if( linkInfo.isLOS ==  true )
        {
            losGridIndx = 11;
            linkCondition = _LOS_;
        }
        else
        {
            losGridIndx = 12;
            linkCondition = _NLOS_;
        }
        
        vec  lsp;
        lsp.set_size(7);// lsp(0)-->DS,lsp(1)-->ASD,lsp(2)-->ASA,lsp(3)-->SF,lsp(4)-->K,lsp(5)-->ZSA,lsp(6)-->ZSD
        lsp.zeros();
        LSPParameter lsp_param;
        for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
        {
            lsp_param = LSPParameter(lsp_cnt);
            complex<double>  n[4] , ni ;
            double d[4]   , r[4] , dcorr , n_val[4];
            
            dcorr = scmInfo.correlationDistance(lsp_cnt,losGridIndx);
            
            if(lsp_param == _K_ && (linkCondition == _NLOS_)) // islos - NLOS
                lsp(lsp_cnt)= 0.0;
            else
            {
                rowIndx = floor_i(imag(normLocation)/(7*dcorr)); // DS, NLOS
                colIndx = floor_i(real(normLocation)/(7*dcorr));	  
                ni  = normLocation;
                n[0] = complex<double>(colIndx * 7*dcorr,(rowIndx+1)*7*dcorr);
                n[1] = complex<double>(colIndx * 7*dcorr,rowIndx*7*dcorr);
                n[2] = complex<double>((colIndx+1) * 7*dcorr,rowIndx*7*dcorr);
                n[3] = complex<double>((colIndx+1) * 7*dcorr,(rowIndx+1)*7*dcorr);	
                
                n_val[0] = scmGrids(floorNumFor3D)(lsp_cnt)((losGridIndx+1)%3)(rowIndx+1,colIndx);//NLOS,DS
                n_val[1] = scmGrids(floorNumFor3D)(lsp_cnt)((losGridIndx+1)%3)(rowIndx,colIndx);//NLOS,DS
                n_val[2] = scmGrids(floorNumFor3D)(lsp_cnt)((losGridIndx+1)%3)(rowIndx,colIndx+1);//NLOS,DS
                n_val[3] = scmGrids(floorNumFor3D)(lsp_cnt)((losGridIndx+1)%3)(rowIndx+1,colIndx+1);//NLOS,DS	  
                
                for(int ii = 0;ii<4;ii++)
                {
                    d[ii] = abs(ni-n[ii]);
                    r[ii] = exp(-d[ii]/dcorr);
                }
                double sum_ri=0.0, ri_ni=0.0;
                for(int ii=0;ii<4;ii++)
                {
                    sum_ri+=r[ii]*r[ii];
                    ri_ni+=r[ii]*n_val[ii];
                }
                // 	  if(sum_ri<1)
                // 	  {
                // 	    lsp(lsp_cnt) = ri_ni+sqrt(1-sum_ri)*randn() ;
                // 	  }
                // 	  else
                {
                    lsp(lsp_cnt) = ri_ni ;
                }
            }
        }
        
        int o2i_val,los_val;
        o2i_val = 0;
        if(linkInfo.isLOS)
            los_val = 1;
        else
            los_val = 0;
        
        int scenario= 4; // InF
        mat C_mxm = compute5GChannelSiteCorrelationMatrices(scenario,los_val,o2i_val);
        C_mxm = real(sqrtm(C_mxm));    
        if(C_mxm.rows() == 7) //! since there are 7 large scale parameters
            lsp = C_mxm * lsp;
        else
        {
            lsp.del(4);//! fourth index element is K
            vec lsp_tmp = lsp;
            lsp_tmp = C_mxm * lsp_tmp;
            lsp.set_size(0);	  
            lsp.set_size(7,true);	  
            int temp_K=0;
            for(int lspIndx=0;lspIndx<lsp_tmp.length();lspIndx++)
            {
                if(lspIndx == 4){lsp(lspIndx)=0,temp_K=1;}
                lsp(lspIndx+temp_K)=lsp_tmp(lspIndx);
            }
        }       
        for(int lsp_cnt = 0 ; lsp_cnt < lsp.length(); lsp_cnt++)
        {
            lsp_param = LSPParameter(lsp_cnt);
            double mu=0 ;
            double sigma=0;   
            
            if(lsp_param != _ZSD_)
            {
                if(lsp_param!=_SF_)//SF in LSP
                    mu = scmInfo.lspMuSigma(2*lsp_cnt,losGridIndx); // DS mu, NLOS
                else
                    mu = 0.0;
                    
                if(lsp_param==_K_ && (linkCondition == _NLOS_)) // K in LSP
                {
                    sigma = -1.0;
                    mu = -1.0;
                }
                else if(lsp_param ==_SF_ && fc >6)
                    sigma = linkInfo.shadowStdDev;
                else
                    sigma = scmInfo.lspMuSigma(2*lsp_cnt+1,losGridIndx);
                    
            }	
            else// if lsp is ZSD
            {	  
                if(isDownlink)
                {
                    ueHeight=linkInfo.rxLocation.z;
                    bsHeight=linkInfo.txLocation.z;	
                }
                else if(!isDownlink)
                {
                    ueHeight=linkInfo.txLocation.z;
                    bsHeight=linkInfo.rxLocation.z;		
                    
                }
                else// isDownlink=-1 by default
                {cout<<"5G MIMO Channel Model can not be used for Cross-Links"<<endl;abort();}
                
                double d2D = findDistance(linkInfo.rxLocation,linkInfo.txLocation);
                
                if(linkInfo.isLOS)//! LOS/LOS O-to_I
                {
                    if(fc <=6 && channelModelType == _MODEL_A_)
                    {
                        cout<<"_MODEL_A_ not supported for InF"<<endl;abort();
                    }
                    else{
                        muOffSetZOD=0.0;
                        mu=1.35; 
                        sigma=0.35;	
                    }	    	
                }
                else //! ! NLOS/NLOS O_to_I
                {	    
                    muOffSetZOD = 0.0;
                    mu = 1.2;
                    sigma = 0.55; 	    
                }	  
                muZSD=mu;		  
            } //! end for lsp==6	
            
            lsp(lsp_cnt) = sigma*lsp(lsp_cnt)+mu;	
        } 
        if(std::isnan(lsp(0))) 
        {
            cout<<" linkInfo: "<< linkInfo.rxType<<"\n "<< linkInfo.txType<<endl;
            abort();
        }
        
        linkInfo.scmLargeScaleParams.ds = lsp(0);
        linkInfo.scmLargeScaleParams.asd = lsp(1);
        linkInfo.scmLargeScaleParams.asa = lsp(2);
        linkInfo.scmLargeScaleParams.sf = linkInfo.shadowLoss = lsp(3); 
        linkInfo.scmLargeScaleParams.k = lsp(4);
        linkInfo.scmLargeScaleParams.zsa = lsp(5);
        linkInfo.scmLargeScaleParams.zsd = lsp(6);
        linkInfo.scmLargeScaleParams.muOffSetZOD = muOffSetZOD;
        linkInfo.scmLargeScaleParams.muZSD = muZSD;     
    }
  }
  else if(scmScenario==_SCMNTN_RURAL_)
  {
      generateNTNSCMLargeScaleParameters(linkInfo,scmScenario,infScenario,channelModelType,isDownlink);
      
  }
  
//   else if(scmScenario == _SCM5G_INDOOR_FACTORY_)
//   {
//       if(isDownlink==-1)// isDownlink=-1 by default
//       {
//           cout<<"5G MIMO Channel Model can not be used for Cross-Links"<<endl;abort();
//       }
//       if(isOutdoorToIndoor)
//       {
//           cout<<"O2I not supported for Indoor Factory Model...Aborting..."<<endl;abort();
//       }
//       double fc = linkInfo.linkCarrierFrequencyInHz/1e9; // link carrier frequency in GHz
//       
//       SCMLinkCondition linkCondition = (linkInfo.isLOS) ? _LOS_ : _NLOS_;
//       int scmGridIndx = get5GSCMGridIndx(scmScenario,linkCondition);
//       
//       linkInfo.isForSCM=true; 
//       vec corrDist = scmInfo.correlationDistance.get_col(scmGridIndx);
//       vec lspMuSigma = getlspMuSigmaForInF(scmInfo.coverageArea,fc,infScenario).get_col(linkCondition);
//       vec scRVs = generateSpatialCorrelatedRandomVariables(nodeLocation,corrDist,scmGrids,scmInfo.coverageArea,infScenario,linkCondition,scmInfo.normGridDist);
//       
//       computeLSPs(linkInfo,scRVs,lspMuSigma,scmScenario,channelModelType,linkCondition,isDownlink);
//   }
  else
    {
      cout<<"Unsupported IMT Terrain for generateSCMLargeScaleParameters()..."<<endl;abort();
    }
}


vec generateSCRVsUsingSOS(Location_S txLocation,Location_S rxLocation,mat dcorr,SCMPropagationScenario scmScenario,InFScenario_E infScenario,SCMLinkCondition linkCondition,int cellID,ACF_type acfType)
{
    mat srvcLoc=rxLocation.getAsMatrix(2),srvrLoc=txLocation.getAsMatrix(2);
    //cout << "srvcLoc and srvrLoc"<< srvcLoc <<"," << srvrLoc <<endl;
    int nLSPs = (scmScenario==_SCM5G_INDOOR_FACTORY_) ? 8 : 7,nLinkConditions = (scmScenario == _SCM5G_INDOOR_OFFICE_ || scmScenario == _SCM5G_INDOOR_FACTORY_) ? 2 : 3,nFloorsMax = (scmScenario == _SCM5G_INDOOR_OFFICE_ || scmScenario == _SCM5G_INDOOR_FACTORY_) ? 1 : 8;
    vec scRVs(nLSPs);// lsp(0)-->DS,lsp(1)-->ASD,lsp(2)-->ASA,lsp(3)-->SF,lsp(4)-->K,lsp(5)-->ZSA,lsp(6)-->ZSD
    ivec sizes = getVectorWithIntegers(4,128,nFloorsMax,nLinkConditions,nLSPs);
    ivec indices = getVectorWithIntegers(4,cellID,(int)rxLocation.floorNumber,(int)linkCondition,0);
    
    cout<<"Info : "<<sizes<<" , "<<indices<<endl;
    int startIndx = decode(sizes,indices);
    //cout<<"startIndx : "<<startIndx<<endl;
    for(int lsp_cnt=0;lsp_cnt<nLSPs;lsp_cnt++)
    {
        mat dCorrMat = (lsp_cnt==7) ? getCorrDistForDeltaTau(infScenario)*ones(1,1): dcorr.get_cols(lsp_cnt,lsp_cnt);
        //cout<<"dCorrMat : "<<dCorrMat<<endl;
        if( LSPParameter(lsp_cnt)==_K_ && linkCondition!=_LOS_)
            scRVs(lsp_cnt) = 0;            

       /* else if(LSPParameter(lsp_cnt)==_DS_&& linkCondition==_LOS_)
        {
            scRVs(lsp_cnt) = rand_sc(dCorrMat,srvcLoc,srvrLoc,Exponential,acfType,startIndx+lsp_cnt,prod(sizes))(0,0);
        }
         else if((LSPParameter(lsp_cnt)==_ZSA_||LSPParameter(lsp_cnt)==_ZSD_)&& linkCondition==_LOS_)
        {
            scRVs(lsp_cnt) = rand_sc(dCorrMat,srvcLoc,srvrLoc,Laplace,acfType,startIndx+lsp_cnt,prod(sizes))(0,0);
        }*/
        else
            scRVs(lsp_cnt) = rand_sc(dCorrMat,srvcLoc,srvrLoc,Normal,acfType,startIndx+lsp_cnt,prod(sizes))(0,0);
        
    }
    return scRVs;
}

void print5GParams(SCMInfo_S* scmInfo,NodeLinkInfo_S* linkInfo,SCMPropagationScenario scmScenario,ChannelModelType channelModelType,InFScenario_E infScenario,double fc,int isDownlink)
{
    ofstream op("5GChannelParams.txt",std::ios::out);
    int start = 3*(scmScenario-6) - int(scmScenario==10),nLinks = (scmScenario >= 9) ? 2 : 3;
    op<<"lspMuSigma: [DS ASD ASA SF K ZSA ZSD]"<<endl;
    if(scmScenario!=_SCM5G_INDOOR_FACTORY_)
    {
        for(int i=0;i<nLinks;i++)
        {
            vec lspMuSigma = scmInfo->lspMuSigma.get_col(start+i);
            op<<getString(SCMLinkCondition(i))<<",\t"<<lspMuSigma<<endl;
        }
    }
    else
    {
        mat lspMuSigmaInF = getlspMuSigmaForInF(scmInfo->coverageArea,fc,infScenario);
        for(int i=0;i<nLinks;i++)
        {
            vec scmParams = lspMuSigmaInF.get_col(i);
            //cout<<"scmParams check"<<scmParams<<endl;
            vec zsdParams = getZSDparams(*linkInfo,scmScenario,channelModelType,fc,isDownlink,SCMLinkCondition(i));
            append(scmParams,zsdParams(0,2));
            op<<getString(SCMLinkCondition(i))<<",\t"<<scmParams<<endl;
        }
    }

    op<<"\n\ncorrDist: [DS ASD ASA SF K ZSA ZSD]"<<endl;
    for(int i=0;i<nLinks;i++)
        op<<getString(SCMLinkCondition(i))<<",\t"<<scmInfo->correlationDistance.get_col(start+i)<<endl;
    string lspName[] = {"DS","ASD","ASA","SF","K","ZSA","ZSD"};
    op<<"\n\ncrossCorrValues:\n";
    vec rowIndx = "1 2 2 1 0 1 1 2 0 3 6 5 6 5 6 5 6 5 6 5 6", colIndx = "0 0 3 3 3 2 4 4 4 4 3 3 4 4 0 0 1 1 2 2 5";
    
    for(int i=0;i<nLinks;i++)
    {
        SCMLinkCondition tempLinkCondition = SCMLinkCondition(i);
        mat corrMat = compute5GChannelSiteCorrelationMatrices(scmScenario-6,tempLinkCondition==_LOS_,tempLinkCondition==_O2I_);
        if(tempLinkCondition!=_LOS_)
        {
            corrMat.ins_col(4,-1*ones(6));
            corrMat.ins_row(4,-1*ones(7));
        }
        if(i==0)
        {
            op<<"linkCondition,\t"<<setw(6);
            for(int cnt=0;cnt<rowIndx.length();cnt++)
            {
                int r = rowIndx(cnt),c = colIndx(cnt);
                op<<lspName[r]+"_"+lspName[c]<<",\t"<<setw(6);
            }
            op<<endl;
        }
        op<<getString(tempLinkCondition)<<",\t"<<setw(10);
        for(int cnt=0;cnt<rowIndx.length();cnt++)
            op<<corrMat(rowIndx(cnt),colIndx(cnt))<<",\t"<<setw(6);
        op<<endl;
    }
    op.close();
}


void generateNTNSCMLargeScaleParameters(NodeLinkInfo_S& linkInfo,SCMPropagationScenario scmScenario,InFScenario_E infScenario,ChannelModelType channelModelType,int isDownlink)
{
    if(isDownlink==-1)// isDownlink=-1 by default
        {cout<<"NTN MIMO Channel Model can not be used for Cross-Links"<<endl;abort();}

    if(linkInfo.elevationAngle==0)
    {
//         cout<<"txID: "<<linkInfo.txLocation.LocId<<" "<<linkInfo.txLocation<<" rxID: "<<linkInfo.rxLocation.LocId<<" "<<linkInfo.rxLocation<<endl;
//         cout<<"elevationAngle of 0 is not supported for NTN..."<<endl;
//         abort();
        linkInfo.scmLargeScaleParams.load(zeros(9));return;
    }

    linkInfo.isForSCM=true;
    bool isKaBandCarrier = isKaBand(linkInfo.linkCarrierFrequencyInHz);
    //cout<<"scmScenario,linkInfo.elevationAngle,linkInfo.isLOS,isKaBandCarrier"<<scmScenario<< linkInfo.elevationAngle<< linkInfo.isLOS<< isKaBandCarrier<<endl;
    vec lspMuSigma = getlspMuSigmaForNTN(scmScenario,linkInfo.elevationAngle,linkInfo.isLOS,isKaBandCarrier);
    //cout<<"lspMuSigma"<<lspMuSigma<<endl;

    vec dCorr = getCorrelationDistancesForNTN(scmScenario,linkInfo.elevationAngle,linkInfo.isLOS,isKaBandCarrier);
     SCMLinkCondition linkCondition = (linkInfo.isLOS) ? _LOS_ : _NLOS_;
    int scmGridIndx = get5GSCMGridIndx(scmScenario,linkCondition);
    //cout << "dcorr"<<dCorr<<endl;
    vec scRVs;

//     if(scmInfo.useSOSForSpCorr){
//       mat dcorr(1,7);dcorr.set_row(0,dCorr);
//       scRVs = generateSCRVsUsingSOS(linkInfo.txLocation,linkInfo.rxLocation,dcorr,scmScenario,infScenario,linkCondition,cellID,scmInfo.acfType);
//     }
//   else
    {
//         vec corrDist = scmInfo.correlationDistance.get_col(scmGridIndx);
//         cout<<"corrDist"<<corrDist<<endl;
//         Location_S ueLoc = (isDownlink==1) ? linkInfo.rxLocation : linkInfo.txLocation;
//         cout<<"ueLoc"<<ueLoc<<endl;
//         cout<<"scm grid distance"<<scmInfo.normGridDist<<endl;
//         scRVs = generateSpatialCorrelatedRandomVariablesForNTN(ueLoc,corrDist,scmGrids,scmInfo.coverageArea,infScenario,linkCondition,scmInfo.normGridDist);
        scRVs =generateUncorrelatedRandomVariablesForNTN(infScenario,linkCondition);
        
        //cout<<"scRVs"<<scRVs<<endl;
    }
    //cout<<"scRVs"<<scRVs<<endl;
    computeLSPsForNTN(linkInfo,scRVs,lspMuSigma,scmScenario,channelModelType,linkCondition,isDownlink);
// cout<<"K : "<<linkInfo.scmLargeScaleParams.k<<endl;
    if(isDownlink==1 && currentTaskid==0)
    {
        ofstream op;
        string fileName = "lsp.txt";
        if(!IsFileExists(fileName))
        {
            op.open(fileName.c_str(),std::ios::out);
            op<<"%srvcNode,\tsrvrNode,\tLSPs[DS ASD ASA SF K ZSA ZSD muOffsetZOD muZSD]"<<endl;
        }
        else
            op.open(fileName.c_str(),std::ios::app);
        op<<linkInfo.rxLocation.LocId<<",\t"<<linkInfo.txLocation.LocId<<",\t"<<linkInfo.scmLargeScaleParams.getVector()<<";"<<endl;
        op.close();
    }
}




// void generate5GSCMLargeScaleParameters(NodeLinkInfo_S& linkInfo,Array< Array< Array<mat> > > &scmGrids,SCMInfo_S& scmInfo,SCMPropagationScenario scmScenario,InFScenario_E infScenario,ChannelModelType channelModelType,bool isOutdoorToIndoor,int isDownlink,int cellID)
// {
//     if(isNTNSCMScenario(scmScenario))
//     {
//         //return generateNTNSCMLargeScaleParameters(linkInfo,scmInfo,scmScenario,infScenario,channelModelType,isOutdoorToIndoor,isDownlink,cellID);
//         return generateNTNSCMLargeScaleParameters(linkInfo,scmScenario,infScenario,channelModelType,isDownlink);
// 
// 
//     }
//     if(!is5GSCMScenario(scmScenario))
//     {
//         cout<<"Invalid SCM Scenario for 5G.."<<endl;abort();
//     }
//     double fc = linkInfo.linkCarrierFrequencyInHz/1e9; // link carrier frequency in GHz
//     
//     #ifndef _USING_CALIBRATION_
//     if((scmScenario==_SCM5G_URBAN_MACRO_ || scmScenario==_SCM5G_INDOOR_OFFICE_) && fc<=6)
//     {
//         fc = 6;  // TR 38.901 foot node after Table 7.5-6 Part-1: Channel model parameters UMa and indoor
//     }
//     else if(scmScenario==_SCM5G_URBAN_MICRO_ && fc<=2)
//     {
//         fc = 2;  // TR 38.901 foot node after Table 7.5-6 Part-1:  for UMi-Street Canyon
//     }
//     #endif
//     if(isDownlink==-1)// isDownlink=-1 by default
//     {cout<<"5G MIMO Channel Model can not be used for Cross-Links"<<endl;abort();}
//     
//     if((scmScenario == _SCM5G_INDOOR_OFFICE_ || scmScenario == _SCM5G_INDOOR_FACTORY_) && isOutdoorToIndoor)
//     {
//         cout<<"O2I not supported for indoor scenarios..."<<endl;abort();
//     }
//     
//     SCMLinkCondition linkCondition = (isOutdoorToIndoor) ? _O2I_ : (linkInfo.isLOS) ? _LOS_ :   _NLOS_;
//     int scmGridIndx = get5GSCMGridIndx(scmScenario,linkCondition);
//     
//     linkInfo.isForSCM=true; 
//     
//     vec lspMuSigma;
//     if(scmScenario==_SCM5G_INDOOR_FACTORY_) // Indoor Factory
//         lspMuSigma = getlspMuSigmaForInF(scmInfo.coverageArea,fc,infScenario).get_col(linkCondition);
//     else
//         lspMuSigma = scmInfo.lspMuSigma.get_col(scmGridIndx);
//     
//     vec scRVs;
//     static bool isToPrint = true;
//     if(currentTaskid==0 && isToPrint)
//     {
//         print5GParams(&scmInfo,&linkInfo,scmScenario,channelModelType,infScenario,fc,isDownlink);
//         isToPrint = false;
//     }    
//     
//     if(scmInfo.useSOSForSpCorr)
//     {
//         mat dcorr = scmInfo.correlationDistance.get_cols(scmGridIndx,scmGridIndx).T();
//         scRVs = generateSCRVsUsingSOS(linkInfo.txLocation,linkInfo.rxLocation,dcorr,scmScenario,infScenario,linkCondition,cellID,scmInfo.acfType);
//     }
//     else
//     {
//         vec corrDist = scmInfo.correlationDistance.get_col(scmGridIndx);
//         Location_S ueLoc = (isDownlink==1) ? linkInfo.rxLocation : linkInfo.txLocation;
//         scRVs = generateSpatialCorrelatedRandomVariables(ueLoc,corrDist,scmGrids,scmInfo.coverageArea,infScenario,linkCondition,scmInfo.normGridDist);
//     }
//     computeLSPs(linkInfo,scRVs,lspMuSigma,scmScenario,channelModelType,linkCondition,isDownlink);
//     if(scmScenario == _SCM5G_INDOOR_FACTORY_)
//     {
//         double maxDim = max(max(scmInfo.coverageArea.xLen,scmInfo.coverageArea.yLen),scmInfo.coverageArea.zLen);
//         if(linkInfo.scmLargeScaleParams.deltaTau > log10(2*maxDim/lightSpeed))
//             linkInfo.scmLargeScaleParams.deltaTau = log10(2*maxDim/lightSpeed);
//     }
//     
//     if(isDownlink==1 && currentTaskid==0)
//     {
//         ofstream op;
//         string fileName = "lsp.txt";
//         if(!IsFileExists(fileName))
//         {
//             op.open(fileName.c_str(),std::ios::out);
//             op<<"%srvcNode,\tsrvrNode,\tLSPs[DS ASD ASA SF K ZSA ZSD muOffsetZOD muZSD]"<<endl;
//         }
//         else
//             op.open(fileName.c_str(),std::ios::app);
//         op<<linkInfo.rxLocation.LocId<<",\t"<<linkInfo.txLocation.LocId<<",\t"<<linkInfo.scmLargeScaleParams.getVector()<<endl;
//         op.close();
//     }
// }

mat IMT_computeSiteCorrelationMatrices (int Scenario,int isLOS, int isOtoI)
{
  double ASD_DS,ASA_DS,ASA_SF,ASD_SF,DS_SF,ASD_ASA,ASD_K,ASA_K,DS_K,SF_K;
  mat Corr;
  
  switch (Scenario) // see 3GPP TR 36.814 v 9.0.0, Table B.1.2.2.1-4
  {
    case 0: //urban macro
      if (isLOS == 1)
      {
	ASD_DS = 0.4;
	ASA_DS = 0.8;
	ASA_SF = -0.5;
	ASD_SF = -0.5;
	DS_SF = -0.4;
	ASD_ASA = 0.0;
	ASD_K = -0.0;
	ASA_K = -0.2;
	DS_K = -0.4;
	SF_K = 0.0;
      }
      else
      {
	ASD_DS = 0.4;
	ASA_DS = 0.6;
	ASA_SF = 0.0;
	ASD_SF = -0.6;
	DS_SF = -0.4;
	ASD_ASA = 0.4;
      }
      break;
      
    case 1: //urban micro
      if (isLOS==1)
      {
	ASD_DS = 0.5;
	ASA_DS = 0.8;
	ASA_SF = -0.4;
	ASD_SF = -0.5;
	DS_SF = -0.4;
	ASD_ASA = 0.4;
	ASD_K = -0.2;
	ASA_K = -0.3;
	DS_K = -0.7;
	SF_K = 0.5;
      }
      else
      {
	if (isOtoI == 1)
	{
	  ASD_DS = 0.4;
	  ASA_DS = 0.4;
	  ASA_SF = 0.0;
	  ASD_SF = 0.2;
	  DS_SF = -0.5;
	  ASD_ASA = 0.0;
	}
	else
	{
	  ASD_DS = 0.0;
	  ASA_DS = 0.4;
	  ASA_SF = -0.4;
	  ASD_SF = 0.0;
	  DS_SF = -0.7;
	  ASD_ASA = 0.0;
	}
      }
      break;
      
    case 2: //rural macro
      if (isLOS == 1)
      {
	ASD_DS = 0.0;
	ASA_DS = 0.0;
	ASA_SF = 0.0;
	ASD_SF = 0.0;
	DS_SF = -0.5;
	ASD_ASA = 0.0;
	ASD_K = 0.0;
	ASA_K = 0.0;
	DS_K = 0.0;
	SF_K = 0.0;
      }
      else
      {
	ASD_DS = -0.4;
	ASA_DS = 0.0;
	ASA_SF = 0.0;
	ASD_SF = 0.6;
	DS_SF = -0.5;
	ASD_ASA = 0.0;
      }
      break;
      
    case 3: //_IMT_INDOOR_HOTSPOT
      if (isLOS == 1)
      {
	ASD_DS = 0.6;
	ASA_DS = 0.8;
	ASA_SF = -0.5;
	ASD_SF = -0.4;
	DS_SF = -0.8;
	ASD_ASA = 0.4;
	ASD_K = 0.0;
	ASA_K = 0.0;
	DS_K = -0.5;
	SF_K = 0.5;
      }
      else
      {
	ASD_DS = 0.4;
	ASA_DS = 0.0;
	ASA_SF = -0.4;
	ASD_SF = 0.0;
	DS_SF = -0.5;
	ASD_ASA = 0.0;
      }
      break;
    default :
      cout << "\n\n\nIMT_computeSiteCorrelationMatrices: unknown Propagation scenario -- exiting program" << endl;
      exit(0);
      break;
  }
  
  if (isLOS == 1)
  {
    Corr.set_size(5,5);Corr.zeros(); 
    Corr(0,0) = 1.0; Corr(0,1) = ASD_DS; Corr(0,2) = ASA_DS; Corr(0,3) = DS_SF; Corr(0,4) = DS_K;
    Corr(1,0) = ASD_DS; Corr(1,1) = 1.0; Corr(1,2) = ASD_ASA; Corr(1,3) = ASD_SF; Corr(1,4) = ASD_K;
    Corr(2,0) = ASA_DS; Corr(2,1) = ASD_ASA; Corr(2,2) = 1.0; Corr(2,3) = ASA_SF; Corr(2,4) = ASA_K;
    Corr(3,0) = DS_SF; Corr(3,1) = ASD_SF; Corr(3,2) = ASA_SF; Corr(3,3) = 1.0; Corr(3,4) = SF_K;
    Corr(4,0) = DS_K; Corr(4,1) = ASD_K; Corr(4,2) = ASA_K; Corr(4,3) = SF_K; Corr(4,4) = 1.0;
    
  }
  else
  {
    Corr.set_size(4,4);Corr.zeros(); 
    Corr(0,0) = 1.0; Corr(0,1) = ASD_DS; Corr(0,2) = ASA_DS; Corr(0,3) = DS_SF;
    Corr(1,0) = ASD_DS; Corr(1,1) = 1.0; Corr(1,2) = ASD_ASA; Corr(1,3) = ASD_SF;
    Corr(2,0) = ASA_DS; Corr(2,1) = ASD_ASA; Corr(2,2) = 1.0; Corr(2,3) = ASA_SF;
    Corr(3,0) = DS_SF; Corr(3,1) = ASD_SF; Corr(3,2) = ASA_SF; Corr(3,3) = 1.0;
  }
  
  return Corr;
  
}

mat compute3DChannelSiteCorrelationMatrices (int Scenario,int isLOS, int isOtoI)
{
  double ASD_DS,ASA_DS,ASA_SF,ASD_SF,DS_SF,ASD_ASA,ASD_K,ASA_K,DS_K,SF_K;
  
  double ZSD_SF, ZSA_SF, ZSD_K, ZSA_K, ZSD_DS, ZSA_DS, ZSD_ASD, ZSA_ASD, ZSD_ASA, ZSA_ASA, ZSD_ZSA;
  mat Corr;
  
  switch (Scenario) // see 3GPP TR 36.873 V2.0.0 (2014-03), Table 7.3-6: Channel model parameters
  {
    
    case 0: //3D-UMi
      if (isLOS==1)
      {
	ASD_DS = 0.5;
	ASA_DS = 0.8;
	ASA_SF = -0.4;
	ASD_SF = -0.5;
	DS_SF = -0.4;
	ASD_ASA = 0.4;
	ASD_K = -0.2;
	ASA_K = -0.3;
	DS_K = -0.7;
	SF_K = 0.5;
       //!---------------
	ZSD_SF=0.0; 
	ZSA_SF=0.0;
	ZSD_K=0.0;
	ZSA_K=0.0;
	ZSD_DS=0.0;
	ZSA_DS=0.2;
	ZSD_ASD=0.5;
	ZSA_ASD=0.3;
	ZSD_ASA=0.0;
	ZSA_ASA=0.0;
	ZSD_ZSA=0.0;
      }
      else//!Non-LOS
      {
	if (isOtoI == 1)
	{
	  ASD_DS = 0.4;
	  ASA_DS = 0.4;
	  ASA_SF = 0.0;
	  ASD_SF = 0.2;
	  DS_SF = -0.5;
	  ASD_ASA = 0.0;
	  //!---------------
	  ZSD_SF=0.0; 
	  ZSA_SF=0.0;
	  ZSD_DS=-0.6;
	  ZSA_DS=-0.2;
	  ZSD_ASD=-0.2;
	  ZSA_ASD=0.0;
	  ZSD_ASA=0.0;
	  ZSA_ASA=0.5;
	  ZSD_ZSA=0.5;
	}
	else
	{
	  ASD_DS = 0.0;
	  ASA_DS = 0.4;
	  ASA_SF = -0.4;
	  ASD_SF = 0.0;
	  DS_SF = -0.7;
	  ASD_ASA = 0.0;
	  //!---------------
	  ZSD_SF=0.0; 
	  ZSA_SF=0.0;
	  ZSD_DS=-0.5;
	  ZSA_DS=0.0;
	  ZSD_ASD=0.5;
	  ZSA_ASD=0.5;
	  ZSD_ASA=0.0;
	  ZSA_ASA=0.2;
	  ZSD_ZSA=0.0;
	}
      }
      break;
      
     case 1: //3D-UMa
      if (isLOS==1)
      {
	ASD_DS = 0.4;
	ASA_DS = 0.8;
	ASA_SF = -0.5;
	ASD_SF = -0.5;
	DS_SF = -0.4;
	ASD_ASA = 0.0;
	ASD_K = 0.0;
	ASA_K = -0.2;
	DS_K = -0.4;
	SF_K = 0.0;
       //!---------------
	ZSD_SF=0.0; 
	ZSA_SF=-0.8;
	ZSD_K=0.0;
	ZSA_K=0.0;
	ZSD_DS=-0.2;
	ZSA_DS=0.0;
	ZSD_ASD=0.5;
	ZSA_ASD=0.0;
	ZSD_ASA=-0.3;
	ZSA_ASA=0.4;
	ZSD_ZSA=0.0;
      }
      else//!Non-LOS
      {
	if (isOtoI == 1)
	{
	  ASD_DS = 0.4;
	  ASA_DS = 0.4;
	  ASA_SF = 0.0;
	  ASD_SF = 0.2;
	  DS_SF = -0.5;
	  ASD_ASA = 0.0;
	  //!---------------
	  ZSD_SF=0.0; 
	  ZSA_SF=0.0;
	  ZSD_DS=-0.6;
	  ZSA_DS=-0.2;
	  ZSD_ASD=-0.2;
	  ZSA_ASD=0.0;
	  ZSD_ASA=0.0;
	  ZSA_ASA=0.5;
	  ZSD_ZSA=0.5;
	}
	else
	{
	  ASD_DS = 0.4;
	  ASA_DS = 0.6;
	  ASA_SF = 0.0;
	  ASD_SF = -0.6;
	  DS_SF = -0.4;
	  ASD_ASA = 0.4;
	  //!---------------
	  ZSD_SF=0.0; 
	  ZSA_SF=-0.4;
	  ZSD_DS=-0.5;
	  ZSA_DS=0.0;
	  ZSD_ASD=0.5;
	  ZSA_ASD=-0.1;
	  ZSD_ASA=0.0;
	  ZSA_ASA=0.0;
	  ZSD_ZSA=0.0;
	}
      }
      break; 
    
    default :
      cout << "\n\n\ncompute3DChannelSiteCorrelationMatrices: unknown Propagation scenario -- exiting program" << endl;
      exit(0);
      break;
  }
  
  if (isLOS == 1)
  {
    
    /*
     * This is matirx for correlation matrix
     * 
     *       DS  ASD  ASA  SF  K  ZSA  ZSD
     * DS
     * 
     * ASD
     * 
     * ASA
     * 
     * SF
     * 
     * K
     * 
     * ZSA
     * 
     * ZSD
     */   
    
    Corr.set_size(7,7);Corr.zeros(); 
    Corr(0,0) = 1.0; Corr(0,1) = ASD_DS; Corr(0,2) = ASA_DS; Corr(0,3) = DS_SF; Corr(0,4) = DS_K;Corr(0,5) = ZSA_DS; Corr(0,6) = ZSD_DS;    
    Corr(1,0) = ASD_DS; Corr(1,1) = 1.0; Corr(1,2) = ASD_ASA; Corr(1,3) = ASD_SF; Corr(1,4) = ASD_K;Corr(1,5) = ZSA_ASD; Corr(1,6) = ZSD_ASD;    
    Corr(2,0) = ASA_DS; Corr(2,1) = ASD_ASA; Corr(2,2) = 1.0; Corr(2,3) = ASA_SF; Corr(2,4) = ASA_K;Corr(2,5) = ZSA_ASA; Corr(2,6) = ZSD_ASA;    
    Corr(3,0) = DS_SF; Corr(3,1) = ASD_SF; Corr(3,2) = ASA_SF; Corr(3,3) = 1.0; Corr(3,4) = SF_K;Corr(3,5) = ZSA_SF; Corr(3,6) = ZSD_SF;    
    Corr(4,0) = DS_K; Corr(4,1) = ASD_K; Corr(4,2) = ASA_K; Corr(4,3) = SF_K; Corr(4,4) = 1.0;Corr(4,5) = ZSA_K; Corr(4,6) = ZSD_K;
    
    Corr(5,0) = ZSA_DS; Corr(5,1) = ZSA_ASD; Corr(5,2) = ZSA_ASA; Corr(5,3) = ZSA_SF; Corr(5,4) = ZSA_K;Corr(5,5) = 1.0; Corr(5,6) = ZSD_ZSA;    
    Corr(6,0) = ZSD_DS; Corr(6,1) = ZSD_ASD; Corr(6,2) = ZSD_ASA; Corr(6,3) = ZSD_SF; Corr(6,4) = ZSD_K;Corr(6,5) = ZSD_ZSA; Corr(6,6) = 1.0;    
    
    
  }
  else
  {
    Corr.set_size(6,6);Corr.zeros(); 
    Corr(0,0) = 1.0; Corr(0,1) = ASD_DS; Corr(0,2) = ASA_DS; Corr(0,3) = DS_SF;Corr(0,4) = ZSA_DS; Corr(0,5) = ZSD_DS; 
    Corr(1,0) = ASD_DS; Corr(1,1) = 1.0; Corr(1,2) = ASD_ASA; Corr(1,3) = ASD_SF;Corr(1,4) = ZSA_ASD; Corr(1,5) = ZSD_ASD;
    Corr(2,0) = ASA_DS; Corr(2,1) = ASD_ASA; Corr(2,2) = 1.0; Corr(2,3) = ASA_SF;Corr(2,4) = ZSA_ASA; Corr(2,5) = ZSD_ASA;
    Corr(3,0) = DS_SF; Corr(3,1) = ASD_SF; Corr(3,2) = ASA_SF; Corr(3,3) = 1.0;Corr(3,4) = ZSA_SF; Corr(3,5) = ZSD_SF; 
    
    Corr(4,0) = ZSA_DS; Corr(4,1) = ZSA_ASD; Corr(4,2) = ZSA_ASA; Corr(4,3) = ZSA_SF; Corr(4,4) = 1.0; Corr(4,5) = ZSD_ZSA;    
    Corr(5,0) = ZSD_DS; Corr(5,1) = ZSD_ASD; Corr(5,2) = ZSD_ASA; Corr(5,3) = ZSD_SF; Corr(5,4) = ZSD_ZSA; Corr(5,5) = 1.0;    
    
  }
  
  return Corr;
  
}

mat compute5GChannelSiteCorrelationMatricesForCalibration(int Scenario,bool isLOS, bool isOtoI)
{
    double ASD_DS,ASA_DS,ASA_SF,ASD_SF,DS_SF,ASD_ASA,ASD_K=-1.0,ASA_K=-1.0,DS_K=-1.0,SF_K=-1.0;
    
    double ZSD_SF, ZSA_SF, ZSD_K=-1.0, ZSA_K=-1.0, ZSD_DS, ZSA_DS, ZSD_ASD, ZSA_ASD, ZSD_ASA, ZSA_ASA, ZSD_ZSA;
    mat Corr;
    
    switch (Scenario) // see 3GPP TR 38.900 , Table 7.5-6: Channel model parameters
    {
        case 0: //5G-UMi
            if(isOtoI == 1)
            {
                ASD_DS = 0.4;
                ASA_DS = 0.0;
                ASA_SF = 0.53;
                ASD_SF = 0.0;
                DS_SF = -0.5;
                ASD_ASA = 0.0;
                //!---------------
                ZSD_SF=0.0; 
                ZSA_SF=0.4;
                ZSD_DS=0.0;
                ZSA_DS=-0.53;
                ZSD_ASD=0.0;
                ZSA_ASD=0.42;
                ZSD_ASA=0.0;
                ZSA_ASA=0.0;
                ZSD_ZSA=0.0;  // changed according to TR 38.901 table 7.5.6
            }
            else if(isLOS==1)
            {
                ASD_DS = 0.5;
                ASA_DS = 0.8;
                ASA_SF = -0.4;
                ASD_SF = -0.5;
                DS_SF = -0.4;
                ASD_ASA = 0.4;
                ASD_K = -0.2;
                ASA_K = -0.3;
                DS_K = -0.7;
                SF_K = 0.5;
                //!---------------
                ZSD_SF=0.0; 
                ZSA_SF=0.0;
                ZSD_K=0.0;
                ZSA_K=0.0;
                ZSD_DS=0.0;
                ZSA_DS=0.2;
                ZSD_ASD=0.5;
                ZSA_ASD=0.3;
                ZSD_ASA=0.0;
                ZSA_ASA=0.0;
                ZSD_ZSA=0.0;
            }
            else
            {
                ASD_DS = 0.0;
                ASA_DS = 0.4;
                ASA_SF = -0.4;
                ASD_SF = 0.0;
                DS_SF = -0.7;
                ASD_ASA = 0.0;
                //!---------------
                ZSD_SF=0.0; 
                ZSA_SF=0.0;
                ZSD_DS=-0.5;
                ZSA_DS=0.0;
                ZSD_ASD=0.5;
                ZSA_ASD=0.5;
                ZSD_ASA=0.0;
                ZSA_ASA=0.2;
                ZSD_ZSA=0.0;
            }
        break;      
        case 1: //5G-UMa
            if (isOtoI == 1)
            {
                ASD_DS = 0.4;
                ASA_DS = 0.0;
                ASA_SF = 0.53;
                ASD_SF = 0.0;
                DS_SF = -0.5;
                ASD_ASA = 0.0;
                //!---------------
                ZSD_SF=0.0; 
                ZSA_SF=0.4;
                ZSD_DS=0.0;
                ZSA_DS=-0.53;
                ZSD_ASD=0.0;
                ZSA_ASD=0.42;
                ZSD_ASA=0.0;
                ZSA_ASA=0.0;
                ZSD_ZSA=0.0;    
            }
            else if(isLOS == 1)
            {
                ASD_DS = 0.4;
                ASA_DS = 0.8;
                ASA_SF = -0.5;
                ASD_SF = -0.5;
                DS_SF = -0.4;
                ASD_ASA = 0.0;
                ASD_K = 0.0;
                ASA_K = -0.2;
                DS_K = -0.4;
                SF_K = 0.0;
                //!---------------
                ZSD_SF=0.0; 
                ZSA_SF=-0.8;
                ZSD_K=0.0;
                ZSA_K=0.0;
                ZSD_DS=-0.2;
                ZSA_DS=0.0;
                ZSD_ASD=0.5;
                ZSA_ASD=0.0;
                ZSD_ASA=-0.3;
                ZSA_ASA=0.4;
                ZSD_ZSA=0.0;
            }
            else
            {
                ASD_DS = 0.4;
                ASA_DS = 0.6;
                ASA_SF = 0.0;
                ASD_SF = -0.6;
                DS_SF = -0.4;
                ASD_ASA = 0.4;
                //!---------------
                ZSD_SF=0.0; 
                ZSA_SF=-0.4;
                ZSD_DS=-0.5;
                ZSA_DS=0.0;
                ZSD_ASD=0.5;
                ZSA_ASD=-0.1;
                ZSD_ASA=0.0;
                ZSA_ASA=0.0;
                ZSD_ZSA=0.0;
            }
        break; 	
        case 3: //5G-Indoor
            if (isLOS==1)
            {
                ASD_DS = 0.6;
                ASA_DS = 0.8;
                ASA_SF = -0.5;
                ASD_SF = -0.4;
                DS_SF = -0.8;
                ASD_ASA = 0.4;
                ASD_K = 0.0;
                ASA_K = 0.0;
                DS_K = -0.5;
                SF_K = 0.5;
                //!---------------
                ZSD_SF=0.2; 
                ZSA_SF=-0.1;
                ZSD_K=0.0;
                ZSA_K=0.1;
                ZSD_DS=0.1;
                ZSA_DS=0.2;
                ZSD_ASD=0.2;
                ZSA_ASD=0.2;
                ZSD_ASA=0.1;
                ZSA_ASA=0.3;
                ZSD_ZSA =0.2;
            }
            else//!Non-LOS
            {            
                ASD_DS = 0.4;
                ASA_DS = 0.0;
                ASA_SF = -0.4;
                ASD_SF = 0.0;
                DS_SF = -0.5;
                ASD_ASA = 0.0;
                
                //!---------------
                ZSD_SF=0.0; 
                ZSA_SF=-0.1;
                ZSD_DS=-0.1;
                ZSA_DS=-0.1;
                ZSD_ASD=0.3;
                ZSA_ASD=0.2;
                ZSD_ASA=0.1;
                ZSA_ASA=0.0;
                ZSD_ZSA =0.4;
            }
        break;
        case 4:
            if(isLOS==1)
            {
                mat corr = eye(7);
                corr(4,0) = -0.7;corr(0,4) = -0.7; //K_DS
                corr(4,1) = -0.5;corr(1,4) = -0.5; //K_ASD
                return corr;
            }
            else
            {
                return eye(6);
            }
        break;
        default :
        cout << "\n\n\ncompute5GChannelSiteCorrelationMatrices: unknown Propagation scenario -- exiting program" << endl;
        exit(0);
        break;
    }
    
    if (isLOS == 1)
    {    
        /*
         * This is matirx for correlation matrix
         * 
         *      DS  ASD  ASA  SF  K  ZSA  ZSD
         * DS
         * 
         * ASD
         * 
         * ASA
         * 
         * SF
         * 
         * K
         * 
         * ZSA
         * 
         * ZSD
         */   
        
        Corr.set_size(7,7);Corr.zeros(); 
        Corr(0,0) = 1.0; Corr(0,1) = ASD_DS; Corr(0,2) = ASA_DS; Corr(0,3) = DS_SF; Corr(0,4) = DS_K;Corr(0,5) = ZSA_DS; Corr(0,6) = ZSD_DS;    
        Corr(1,0) = ASD_DS; Corr(1,1) = 1.0; Corr(1,2) = ASD_ASA; Corr(1,3) = ASD_SF; Corr(1,4) = ASD_K;Corr(1,5) = ZSA_ASD; Corr(1,6) = ZSD_ASD;    
        Corr(2,0) = ASA_DS; Corr(2,1) = ASD_ASA; Corr(2,2) = 1.0; Corr(2,3) = ASA_SF; Corr(2,4) = ASA_K;Corr(2,5) = ZSA_ASA; Corr(2,6) = ZSD_ASA;    
        Corr(3,0) = DS_SF; Corr(3,1) = ASD_SF; Corr(3,2) = ASA_SF; Corr(3,3) = 1.0; Corr(3,4) = SF_K;Corr(3,5) = ZSA_SF; Corr(3,6) = ZSD_SF;    
        Corr(4,0) = DS_K; Corr(4,1) = ASD_K; Corr(4,2) = ASA_K; Corr(4,3) = SF_K; Corr(4,4) = 1.0;Corr(4,5) = ZSA_K; Corr(4,6) = ZSD_K;
        
        Corr(5,0) = ZSA_DS; Corr(5,1) = ZSA_ASD; Corr(5,2) = ZSA_ASA; Corr(5,3) = ZSA_SF; Corr(5,4) = ZSA_K;Corr(5,5) = 1.0; Corr(5,6) = ZSD_ZSA;    
        Corr(6,0) = ZSD_DS; Corr(6,1) = ZSD_ASD; Corr(6,2) = ZSD_ASA; Corr(6,3) = ZSD_SF; Corr(6,4) = ZSD_K;Corr(6,5) = ZSD_ZSA; Corr(6,6) = 1.0;    
        
    }
    else
    {
        /*
         * This is matirx for correlation matrix
         * 
         *     DS  ASD  ASA  SF  ZSA  ZSD
         * DS
         * 
         * ASD
         * 
         * ASA
         * 
         * SF
         * 
         * ZSA
         * 
         * ZSD
         */     
        Corr.set_size(6,6);Corr.zeros(); 
        Corr(0,0) = 1.0; Corr(0,1) = ASD_DS; Corr(0,2) = ASA_DS; Corr(0,3) = DS_SF;Corr(0,4) = ZSA_DS; Corr(0,5) = ZSD_DS; 
        Corr(1,0) = ASD_DS; Corr(1,1) = 1.0; Corr(1,2) = ASD_ASA; Corr(1,3) = ASD_SF;Corr(1,4) = ZSA_ASD; Corr(1,5) = ZSD_ASD;
        Corr(2,0) = ASA_DS; Corr(2,1) = ASD_ASA; Corr(2,2) = 1.0; Corr(2,3) = ASA_SF;Corr(2,4) = ZSA_ASA; Corr(2,5) = ZSD_ASA;
        Corr(3,0) = DS_SF; Corr(3,1) = ASD_SF; Corr(3,2) = ASA_SF; Corr(3,3) = 1.0;Corr(3,4) = ZSA_SF; Corr(3,5) = ZSD_SF; 
        
        Corr(4,0) = ZSA_DS; Corr(4,1) = ZSA_ASD; Corr(4,2) = ZSA_ASA; Corr(4,3) = ZSA_SF; Corr(4,4) = 1.0; Corr(4,5) = ZSD_ZSA;    
        Corr(5,0) = ZSD_DS; Corr(5,1) = ZSD_ASD; Corr(5,2) = ZSD_ASA; Corr(5,3) = ZSD_SF; Corr(5,4) = ZSD_ZSA; Corr(5,5) = 1.0;    
        
    }
    return Corr;
}

mat compute5GChannelSiteCorrelationMatrices(int Scenario,bool isLOS, bool isOtoI)
{
    #ifdef _USING_CALIBRATION_
        return compute5GChannelSiteCorrelationMatricesForCalibration(Scenario,isLOS,isOtoI);
    #endif
    
  double ASD_DS,ASA_DS,ASA_SF,ASD_SF,DS_SF,ASD_ASA,ASD_K=-1.0,ASA_K=-1.0,DS_K=-1.0,SF_K=-1.0;
  
  double ZSD_SF, ZSA_SF, ZSD_K=-1.0, ZSA_K=-1.0, ZSD_DS, ZSA_DS, ZSD_ASD, ZSA_ASD, ZSD_ASA, ZSA_ASA, ZSD_ZSA;
  mat Corr;
  
  switch (Scenario) // see 3GPP TR 38.900 , Table 7.5-6: Channel model parameters
  {
    
    case 0: //5G-UMi
      if (isOtoI == 1)
      {
	ASD_DS = 0.4;
	ASA_DS = 0.4;
	ASA_SF = 0.0;
	ASD_SF = 0.2;
	DS_SF = -0.5;
	ASD_ASA = 0.0;
	//!---------------
	ZSD_SF=0.0; 
	ZSA_SF=0.0;
	ZSD_DS=-0.6;
	ZSA_DS=-0.2;
	ZSD_ASD=-0.2;
	ZSA_ASD=0.0;
	ZSD_ASA=0.0;
	ZSA_ASA=0.5;
	ZSD_ZSA=0.5;  // changed according to TR 38.901 table 7.5.6
      }
      else{
	if (isLOS==1)
	{
	  ASD_DS = 0.5;
	  ASA_DS = 0.8;
	  ASA_SF = -0.4;
	  ASD_SF = -0.5;
	  DS_SF = -0.4;
	  ASD_ASA = 0.4;
	  ASD_K = -0.2;
	  ASA_K = -0.3;
	  DS_K = -0.7;
	  SF_K = 0.5;
	  //!---------------
	  ZSD_SF=0.0; 
	  ZSA_SF=0.0;
	  ZSD_K=0.0;
	  ZSA_K=0.0;
	  ZSD_DS=0.0;
	  ZSA_DS=0.2;
	  ZSD_ASD=0.5;
	  ZSA_ASD=0.3;
	  ZSD_ASA=0.0;
	  ZSA_ASA=0.0;
	  ZSD_ZSA=0.0;
	}
	else//!Non-LOS
	{
	
	  ASD_DS = 0.0;
	  ASA_DS = 0.4;
	  ASA_SF = -0.4;
	  ASD_SF = 0.0;
	  DS_SF = -0.7;
	  ASD_ASA = 0.0;
	  //!---------------
	  ZSD_SF=0.0; 
	  ZSA_SF=0.0;
	  ZSD_DS=-0.5;
	  ZSA_DS=0.0;
	  ZSD_ASD=0.5;
	  ZSA_ASD=0.5;
	  ZSD_ASA=0.0;
	  ZSA_ASA=0.2;
	  ZSD_ZSA=0.0;
	  
	}
      }
      break;      
    case 1: //5G-UMa
      if (isOtoI == 1)
      {
	  ASD_DS = 0.4;
	  ASA_DS = 0.4;
	  ASA_SF = 0.0;
	  ASD_SF = 0.2;
	  DS_SF = -0.5;
	  ASD_ASA = 0.0;
	  //!---------------
	  ZSD_SF=0.0; 
	  ZSA_SF=0.0;
	  ZSD_DS=-0.6;
	  ZSA_DS=-0.2;
	  ZSD_ASD=-0.2;
	  ZSA_ASD=0.0;
	  ZSD_ASA=0.0;
	  ZSA_ASA=0.5;
	  ZSD_ZSA=0.5;
      }
      else {
	if (isLOS==1)
	{
	  ASD_DS = 0.4;
	  ASA_DS = 0.8;
	  ASA_SF = -0.5;
	  ASD_SF = -0.5;
	  DS_SF = -0.4;
	  ASD_ASA = 0.0;
	  ASD_K = 0.0;
	  ASA_K = -0.2;
	  DS_K = -0.4;
	  SF_K = 0.0;
	  //!---------------
	  ZSD_SF=0.0; 
	  ZSA_SF=-0.8;
	  ZSD_K=0.0;
	  ZSA_K=0.0;
	  ZSD_DS=-0.2;
	  ZSA_DS=0.0;
	  ZSD_ASD=0.5;
	  ZSA_ASD=0.0;
	  ZSD_ASA=-0.3;
	  ZSA_ASA=0.4;
	  ZSD_ZSA=0.0;
	}
      	else
	{
	  ASD_DS = 0.4;
	  ASA_DS = 0.6;
	  ASA_SF = 0.0;
	  ASD_SF = -0.6;
	  DS_SF = -0.4;
	  ASD_ASA = 0.4;
	  //!---------------
	  ZSD_SF=0.0; 
	  ZSA_SF=-0.4;
	  ZSD_DS=-0.5;
	  ZSA_DS=0.0;
	  ZSD_ASD=0.5;
	  ZSA_ASD=-0.1;
	  ZSD_ASA=0.0;
	  ZSA_ASA=0.0;
	  ZSD_ZSA=0.0;
	}
      }
      break;
    case 2: //5G-RMa
      if(isOtoI)
      {
	ASD_DS = 0.0;
	ASA_DS = 0.0;
	ASA_SF = 0.0;
	ASD_SF = 0.0;
	DS_SF = 0.0;
	ASD_ASA = -0.7;
	//!---------------
	ZSD_SF=0.0; 
	ZSA_SF=0.0;
	ZSD_DS=0.0;
	ZSA_DS=0.0;
	ZSD_ASD=0.66;
	ZSA_ASD=0.47;
	ZSD_ASA=-0.55;
	ZSA_ASA=-0.22;
	ZSD_ZSA=0.0;
	
	
      }
      else{
	if (isLOS==1)
	{	
	  ASD_DS = 0.0;
	  ASA_DS = 0.0;
	  ASA_SF = 0.0;
	  ASD_SF = 0.0;
	  DS_SF = -0.5;
	  ASD_ASA = 0.0;
	  ASD_K = 0.0;
	  ASA_K = 0.0;
	  DS_K = 0.0;
	  SF_K = 0.0;
	  //!---------------
	  ZSD_SF=0.01; 
	  ZSA_SF=-0.17;
	  ZSD_K=0.0;
	  ZSA_K=-0.02;
	  ZSD_DS=-0.05;
	  ZSA_DS=0.27;
	  ZSD_ASD=0.73;
	  ZSA_ASD=-0.14;
	  ZSD_ASA=-0.20;
	  ZSA_ASA=0.24;
	  ZSD_ZSA =-0.07;  
	  
	}
	else//!Non-LOS
	{
	  
	  ASD_DS = -0.4;
	  ASA_DS = 0.0;
	  ASA_SF = 0.0;
	  ASD_SF = 0.6;
	  DS_SF = -0.5;
	  ASD_ASA = 0.0;
	  //!---------------
	  ZSD_SF=-0.04; 
	  ZSA_SF=-0.25;
	  ZSD_DS=-0.10;
	  ZSA_DS=-0.40;
	  ZSD_ASD=0.42;
	  ZSA_ASD=-0.27;
	  ZSD_ASA=-0.18;
	  ZSA_ASA=0.26;
	  ZSD_ZSA=-0.27;
	
	} 
      }
      break; 	
    case 3: //5G-Indoor
      if (isLOS==1)
      {
	ASD_DS = 0.6;
	ASA_DS = 0.8;
	ASA_SF = -0.5;
	ASD_SF = -0.4;
	DS_SF = -0.8;
	ASD_ASA = 0.4;
	ASD_K = 0.0;
	ASA_K = 0.0;
	DS_K = -0.5;
	SF_K = 0.5;
	//!---------------
	ZSD_SF=0.2; 
	ZSA_SF=0.3;
	ZSD_K=0.0;
	ZSA_K=0.1;
	ZSD_DS=0.1;
	ZSA_DS=0.2;
	ZSD_ASD=0.5;
	ZSA_ASD=0;
	ZSD_ASA=0;
	ZSA_ASA=0.5;
	ZSD_ZSA =0.0;  
      }
      else//!Non-LOS
      {
      
	ASD_DS = 0.4;
	ASA_DS = 0.0;
	ASA_SF = -0.4;
	ASD_SF = 0.0;
	DS_SF = -0.5;
	ASD_ASA = 0.0;
      
	//!---------------
	ZSD_SF=0.0; 
	ZSA_SF=0.0;
	ZSD_DS=-0.27;
	ZSA_DS=-0.06;
	ZSD_ASD=0.35;
	ZSA_ASD=0.23;
	ZSD_ASA=-0.08;
	ZSA_ASA=0.43;
	ZSD_ZSA =0.42;  
      }
      break;
    case 4:  //InF 
        if(isLOS==1)
        {
            mat corr = eye(7);
            corr(4,0) = -0.7;corr(0,4) = -0.7; //K_DS
            corr(4,1) = -0.5;corr(1,4) = -0.5; //K_ASD
            return corr;
        }
        else
        {
            return eye(6);
        }
        break;
    default :
      cout << "\n\n\ncompute5GChannelSiteCorrelationMatrices: unknown Propagation scenario -- exiting program" << endl;
      exit(0);
      break;
  }
  
  if (isLOS == 1)
  {    
    /*
     * This is matirx for correlation matrix
     * 
     *      DS  ASD  ASA  SF  K  ZSA  ZSD
     * DS
     * 
     * ASD
     * 
     * ASA
     * 
     * SF
     * 
     * K
     * 
     * ZSA
     * 
     * ZSD
     */   
    
    Corr.set_size(7,7);Corr.zeros(); 
    Corr(0,0) = 1.0; Corr(0,1) = ASD_DS; Corr(0,2) = ASA_DS; Corr(0,3) = DS_SF; Corr(0,4) = DS_K;Corr(0,5) = ZSA_DS; Corr(0,6) = ZSD_DS;    
    Corr(1,0) = ASD_DS; Corr(1,1) = 1.0; Corr(1,2) = ASD_ASA; Corr(1,3) = ASD_SF; Corr(1,4) = ASD_K;Corr(1,5) = ZSA_ASD; Corr(1,6) = ZSD_ASD;    
    Corr(2,0) = ASA_DS; Corr(2,1) = ASD_ASA; Corr(2,2) = 1.0; Corr(2,3) = ASA_SF; Corr(2,4) = ASA_K;Corr(2,5) = ZSA_ASA; Corr(2,6) = ZSD_ASA;    
    Corr(3,0) = DS_SF; Corr(3,1) = ASD_SF; Corr(3,2) = ASA_SF; Corr(3,3) = 1.0; Corr(3,4) = SF_K;Corr(3,5) = ZSA_SF; Corr(3,6) = ZSD_SF;    
    Corr(4,0) = DS_K; Corr(4,1) = ASD_K; Corr(4,2) = ASA_K; Corr(4,3) = SF_K; Corr(4,4) = 1.0;Corr(4,5) = ZSA_K; Corr(4,6) = ZSD_K;
    
    Corr(5,0) = ZSA_DS; Corr(5,1) = ZSA_ASD; Corr(5,2) = ZSA_ASA; Corr(5,3) = ZSA_SF; Corr(5,4) = ZSA_K;Corr(5,5) = 1.0; Corr(5,6) = ZSD_ZSA;    
    Corr(6,0) = ZSD_DS; Corr(6,1) = ZSD_ASD; Corr(6,2) = ZSD_ASA; Corr(6,3) = ZSD_SF; Corr(6,4) = ZSD_K;Corr(6,5) = ZSD_ZSA; Corr(6,6) = 1.0;    

  }
  else
  {
    /*
     * This is matirx for correlation matrix
     * 
     *     DS  ASD  ASA  SF  ZSA  ZSD
     * DS
     * 
     * ASD
     * 
     * ASA
     * 
     * SF
     * 
     * ZSA
     * 
     * ZSD
     */     
    Corr.set_size(6,6);Corr.zeros(); 
    Corr(0,0) = 1.0; Corr(0,1) = ASD_DS; Corr(0,2) = ASA_DS; Corr(0,3) = DS_SF;Corr(0,4) = ZSA_DS; Corr(0,5) = ZSD_DS; 
    Corr(1,0) = ASD_DS; Corr(1,1) = 1.0; Corr(1,2) = ASD_ASA; Corr(1,3) = ASD_SF;Corr(1,4) = ZSA_ASD; Corr(1,5) = ZSD_ASD;
    Corr(2,0) = ASA_DS; Corr(2,1) = ASD_ASA; Corr(2,2) = 1.0; Corr(2,3) = ASA_SF;Corr(2,4) = ZSA_ASA; Corr(2,5) = ZSD_ASA;
    Corr(3,0) = DS_SF; Corr(3,1) = ASD_SF; Corr(3,2) = ASA_SF; Corr(3,3) = 1.0;Corr(3,4) = ZSA_SF; Corr(3,5) = ZSD_SF; 
    
    Corr(4,0) = ZSA_DS; Corr(4,1) = ZSA_ASD; Corr(4,2) = ZSA_ASA; Corr(4,3) = ZSA_SF; Corr(4,4) = 1.0; Corr(4,5) = ZSD_ZSA;    
    Corr(5,0) = ZSD_DS; Corr(5,1) = ZSD_ASD; Corr(5,2) = ZSD_ASA; Corr(5,3) = ZSD_SF; Corr(5,4) = ZSD_ZSA; Corr(5,5) = 1.0;    
    
  }
  return Corr;
}

mat computeNTNCorrelationMatrices(SCMPropagationScenario scmScenario,double elevationAngle,bool isLOS,bool isKaBand)
{
    //NOTE: Refer to Table 6.7.2 in 3GPP TR 38.811.
    
    double ASD_DS,ASA_DS,ASA_SF,ASD_SF,DS_SF,ASD_ASA,ASD_K=-1.0,ASA_K=-1.0,DS_K=-1.0,SF_K=-1.0;
  
    double ZSD_SF, ZSA_SF, ZSD_K=-1.0, ZSA_K=-1.0, ZSD_DS, ZSA_DS, ZSD_ASD, ZSA_ASD, ZSD_ASA, ZSA_ASA, ZSD_ZSA;
    mat Corr;
    int index = round_i(elevationAngle/10)-1;
    //cout<<"elevation"<<elevationAngle<<endl;
    //cout<<"index value"<<index<<endl;
    switch(scmScenario)
    {
        case _SCMNTN_DENSE_URBAN_:
            if(isLOS)
            {
                if(!isKaBand)
                {
                    vec asd_ds =    0.4*ones(9);  ASD_DS = asd_ds(index);
                    vec asa_ds =    0.8*ones(9);  ASA_DS = asa_ds(index);
                    vec asa_sf =   -0.5*ones(9);  ASA_SF = asa_sf(index);
                    vec asd_sf =   -0.5*ones(9);  ASD_SF = asd_sf(index);
                    vec ds_sf  =   -0.4*ones(9);  DS_SF  = ds_sf(index);
                    vec asd_asa =   0.0*ones(9);  ASD_ASA = asd_asa(index);
                    vec asd_k =     0.0*ones(9);  ASD_K = asd_k(index);
                    vec asa_k  =   -0.2*ones(9);  ASA_K = asa_k(index);
                    vec ds_k   =   -0.4*ones(9);  DS_K  = ds_k(index);
                    vec sf_k =      0.0*ones(9);  SF_K = sf_k(index);
                    vec zsd_sf =    0.0*ones(9);  ZSD_SF = zsd_sf(index);
                    vec zsa_sf =   -0.8*ones(9);  ZSA_SF = zsa_sf(index);
                    vec zsd_k =     0.0*ones(9);  ZSD_K = zsd_k(index);
                    vec zsa_k =     0.0*ones(9);  ZSA_K = zsa_k(index);
                    vec zsd_ds  =  -0.2*ones(9);  ZSD_DS = zsd_ds(index);
                    vec zsa_ds =    0.0*ones(9);  ZSA_DS = zsa_ds(index);
                    vec zsd_asd =   0.5*ones(9);  ZSD_ASD = zsd_asd(index);
                    vec zsa_asd =   0.0*ones(9);  ZSA_ASD = zsa_asd(index);
                    vec zsd_asa =  -0.3*ones(9);  ZSD_ASA = zsd_asa(index);
                    vec zsa_asa =   0.4*ones(9);  ZSA_ASA = zsa_asa(index);
                    vec zsd_zsa =   0.0*ones(9);  ZSD_ZSA = zsd_zsa(index);
                }
                else
                {
                    vec asd_ds =    0.4*ones(9);  ASD_DS = asd_ds(index);
                    vec asa_ds =    0.8*ones(9);  ASA_DS = asa_ds(index);
                    vec asa_sf =   -0.5*ones(9);  ASA_SF = asa_sf(index);
                    vec asd_sf =   -0.5*ones(9);  ASD_SF = asd_sf(index);
                    vec ds_sf  =   -0.4*ones(9);  DS_SF  = ds_sf(index);
                    vec asd_asa =   0.0*ones(9);  ASD_ASA = asd_asa(index);
                    vec asd_k =     0.0*ones(9);  ASD_K = asd_k(index);
                    vec asa_k  =   -0.2*ones(9);  ASA_K = asa_k(index);
                    vec ds_k   =   -0.4*ones(9);  DS_K  = ds_k(index);
                    vec sf_k =      0.0*ones(9);  SF_K = sf_k(index);
                    vec zsd_sf =    0.0*ones(9);  ZSD_SF = zsd_sf(index);
                    vec zsa_sf =   -0.8*ones(9);  ZSA_SF = zsa_sf(index);
                    vec zsd_k =     0.0*ones(9);  ZSD_K = zsd_k(index);
                    vec zsa_k =     0.0*ones(9);  ZSA_K = zsa_k(index);
                    vec zsd_ds  =  -0.2*ones(9);  ZSD_DS = zsd_ds(index);
                    vec zsa_ds =    0.0*ones(9);  ZSA_DS = zsa_ds(index);
                    vec zsd_asd =   0.5*ones(9);  ZSD_ASD = zsd_asd(index);
                    vec zsa_asd =   0.0*ones(9);  ZSA_ASD = zsa_asd(index);
                    vec zsd_asa =  -0.3*ones(9);  ZSD_ASA = zsd_asa(index);
                    vec zsa_asa =   0.4*ones(9);  ZSA_ASA = zsa_asa(index);
                    vec zsd_zsa =   0.0*ones(9);  ZSD_ZSA = zsd_zsa(index);
                }
            }
            else
            {
                if(!isKaBand)
                {
                    vec asd_ds =    0.4*ones(9);  ASD_DS = asd_ds(index);
                    vec asa_ds =    0.6*ones(9);  ASA_DS = asa_ds(index);
                    vec asa_sf =    0.0*ones(9);  ASA_SF = asa_sf(index);
                    vec asd_sf =   -0.6*ones(9);  ASD_SF = asd_sf(index);
                    vec ds_sf  =   -0.4*ones(9);  DS_SF  = ds_sf(index);
                    vec asd_asa =   0.4*ones(9);  ASD_ASA = asd_asa(index);
                    vec zsd_sf =    0.0*ones(9);  ZSD_SF = zsd_sf(index);
                    vec zsa_sf =   -0.4*ones(9);  ZSA_SF = zsa_sf(index);
                    vec zsd_ds  =  -0.5*ones(9);  ZSD_DS = zsd_ds(index);
                    vec zsa_ds =    0.0*ones(9);  ZSA_DS = zsa_ds(index);
                    vec zsd_asd =   0.5*ones(9);  ZSD_ASD = zsd_asd(index);
                    vec zsa_asd =  -0.1*ones(9);  ZSA_ASD = zsa_asd(index);
                    vec zsd_asa =   0.0*ones(9);  ZSD_ASA = zsd_asa(index);
                    vec zsa_asa =   0.0*ones(9);  ZSA_ASA = zsa_asa(index);
                    vec zsd_zsa =   0.0*ones(9);  ZSD_ZSA = zsd_zsa(index);
                }
                else
                {
                    vec asd_ds =    0.4*ones(9);  ASD_DS = asd_ds(index);
                    vec asa_ds =    0.6*ones(9);  ASA_DS = asa_ds(index);
                    vec asa_sf =    0.0*ones(9);  ASA_SF = asa_sf(index);
                    vec asd_sf =   -0.6*ones(9);  ASD_SF = asd_sf(index);
                    vec ds_sf  =   -0.4*ones(9);  DS_SF  = ds_sf(index);
                    vec asd_asa =   0.4*ones(9);  ASD_ASA = asd_asa(index);
                    vec zsd_sf =    0.0*ones(9);  ZSD_SF = zsd_sf(index);
                    vec zsa_sf =   -0.4*ones(9);  ZSA_SF = zsa_sf(index);
                    vec zsd_ds  =  -0.5*ones(9);  ZSD_DS = zsd_ds(index);
                    vec zsa_ds =    0.0*ones(9);  ZSA_DS = zsa_ds(index);
                    vec zsd_asd =   0.5*ones(9);  ZSD_ASD = zsd_asd(index);
                    vec zsa_asd =  -0.1*ones(9);  ZSA_ASD = zsa_asd(index);
                    vec zsd_asa =   0.0*ones(9);  ZSD_ASA = zsd_asa(index);
                    vec zsa_asa =   0.0*ones(9);  ZSA_ASA = zsa_asa(index);
                    vec zsd_zsa =   0.0*ones(9);  ZSD_ZSA = zsd_zsa(index);
                }
            }
            break;
            
        case _SCMNTN_URBAN_:
            if(isLOS)
            {
                if(!isKaBand)
                {
                    vec asd_ds =    0.4*ones(9);  ASD_DS = asd_ds(index);
                    vec asa_ds =    0.8*ones(9);  ASA_DS = asa_ds(index);
                    vec asa_sf =   -0.5*ones(9);  ASA_SF = asa_sf(index);
                    vec asd_sf =   -0.5*ones(9);  ASD_SF = asd_sf(index);
                    vec ds_sf  =   -0.4*ones(9);  DS_SF  = ds_sf(index);
                    vec asd_asa =   0.0*ones(9);  ASD_ASA = asd_asa(index);
                    vec asd_k =     0.0*ones(9);  ASD_K = asd_k(index);
                    vec asa_k  =   -0.2*ones(9);  ASA_K = asa_k(index);
                    vec ds_k   =   -0.4*ones(9);  DS_K  = ds_k(index);
                    vec sf_k =      0.0*ones(9);  SF_K = sf_k(index);
                    vec zsd_sf =    0.0*ones(9);  ZSD_SF = zsd_sf(index);
                    vec zsa_sf =   -0.8*ones(9);  ZSA_SF = zsa_sf(index);
                    vec zsd_k =     0.0*ones(9);  ZSD_K = zsd_k(index);
                    vec zsa_k =     0.0*ones(9);  ZSA_K = zsa_k(index);
                    vec zsd_ds  =  -0.2*ones(9);  ZSD_DS = zsd_ds(index);
                    vec zsa_ds =    0.0*ones(9);  ZSA_DS = zsa_ds(index);
                    vec zsd_asd =   0.5*ones(9);  ZSD_ASD = zsd_asd(index);
                    vec zsa_asd =   0.0*ones(9);  ZSA_ASD = zsa_asd(index);
                    vec zsd_asa =  -0.3*ones(9);  ZSD_ASA = zsd_asa(index);
                    vec zsa_asa =   0.4*ones(9);  ZSA_ASA = zsa_asa(index);
                    vec zsd_zsa =   0.0*ones(9);  ZSD_ZSA = zsd_zsa(index);
                }
                else
                {
                    vec asd_ds =    0.4*ones(9);  ASD_DS = asd_ds(index);
                    vec asa_ds =    0.8*ones(9);  ASA_DS = asa_ds(index);
                    vec asa_sf =   -0.5*ones(9);  ASA_SF = asa_sf(index);
                    vec asd_sf =   -0.5*ones(9);  ASD_SF = asd_sf(index);
                    vec ds_sf  =   -0.4*ones(9);  DS_SF  = ds_sf(index);
                    vec asd_asa =   0.0*ones(9);  ASD_ASA = asd_asa(index);
                    vec asd_k =     0.0*ones(9);  ASD_K = asd_k(index);
                    vec asa_k  =   -0.2*ones(9);  ASA_K = asa_k(index);
                    vec ds_k   =   -0.4*ones(9);  DS_K  = ds_k(index);
                    vec sf_k =      0.0*ones(9);  SF_K = sf_k(index);
                    vec zsd_sf =    0.0*ones(9);  ZSD_SF = zsd_sf(index);
                    vec zsa_sf =   -0.8*ones(9);  ZSA_SF = zsa_sf(index);
                    vec zsd_k =     0.0*ones(9);  ZSD_K = zsd_k(index);
                    vec zsa_k =     0.0*ones(9);  ZSA_K = zsa_k(index);
                    vec zsd_ds  =  -0.2*ones(9);  ZSD_DS = zsd_ds(index);
                    vec zsa_ds =    0.0*ones(9);  ZSA_DS = zsa_ds(index);
                    vec zsd_asd =   0.5*ones(9);  ZSD_ASD = zsd_asd(index);
                    vec zsa_asd =   0.0*ones(9);  ZSA_ASD = zsa_asd(index);
                    vec zsd_asa =  -0.3*ones(9);  ZSD_ASA = zsd_asa(index);
                    vec zsa_asa =   0.4*ones(9);  ZSA_ASA = zsa_asa(index);
                    vec zsd_zsa =   0.0*ones(9);  ZSD_ZSA = zsd_zsa(index);
                }
            }
            else
            {
                if(!isKaBand)
                {
                    vec asd_ds =    "0.54 0.46 0.56 0.52 0.60 0.59 0.60 0.57 0.64";  ASD_DS = asd_ds(index);
                    vec asa_ds =    "0.38 0.36 0.27 0.29 0.21 0.24 0.22 0.24 0.24";  ASA_DS = asa_ds(index);
                    vec asa_sf =    "0.05 0.04 0.04 0.04 0.03 0.05 0.02 0.01 0.00";  ASA_SF = -asa_sf(index);
                    vec asd_sf =    "0.48 0.53 0.52 0.52 0.54 0.51 0.50 0.48 0.43";  ASD_SF = -asd_sf(index);
                    vec ds_sf  =    "0.22 0.26 0.21 0.25 0.21 0.19 0.19 0.20 0.20";  DS_SF  = -ds_sf(index);
                    vec asd_asa =   "0.41 0.40 0.33 0.37 0.23 0.23 0.22 0.23 0.21";  ASD_ASA = asd_asa(index);
                    vec zsd_sf =    "-0.02 0.0 0.01 0.0 0.01 0.01 -0.02 -0.08 -0.12";  ZSD_SF = zsd_sf(index);
                    vec zsa_sf =    "0.31 0.33 0.33 0.33 0.38 0.39 0.37 0.37 0.36";  ZSA_SF = -zsa_sf(index);
                    vec zsd_ds  =   "0.69 0.72 0.68 0.68 0.64 0.65 0.64 0.61 0.53";  ZSD_DS = zsd_ds(index);
                    vec zsa_ds =    "0.05 0.09 0.09 0.09 -0.03 -0.15 -0.13 -0.29 -0.19";  ZSA_DS = zsa_ds(index);
                    vec zsd_asd =   "0.52 0.48 0.60 0.56 0.62 0.60 0.65 0.59 0.64";  ZSD_ASD = zsd_asd(index);
                    vec zsa_asd =   "0.05 0.11 0.13 0.14 -0.02 -0.11 -0.13 -0.26 -0.22";  ZSA_ASD = zsa_asd(index);
                    vec zsd_asa =   "0.40 0.39 0.34 0.37 0.31 0.28 0.23 0.21 0.28";  ZSD_ASA = zsd_asa(index);
                    vec zsa_asa =   "0.04 0.13 0.16 0.13 0.13 0.14 -0.02 -0.20 -0.46";  ZSA_ASA = zsa_asa(index);
                    vec zsd_zsa =   "-0.03 0.04 0.07 0.07 -0.01 -0.12 -0.15 -0.34 -0.36";  ZSD_ZSA = zsd_zsa(index);
                }
                else
                {
                    vec asd_ds =    "0.55 0.47 0.55 0.52 0.55 0.57 0.61 0.59 0.65";  ASD_DS = asd_ds(index);
                    vec asa_ds =    "0.38 0.37 0.29 0.30 0.23 0.21 0.23 0.23 0.26";  ASA_DS = asa_ds(index);
                    vec asa_sf =    "0.05 0.04 0.04 0.04 0.03 0.05 0.03 0.01 0.03";  ASA_SF = -asa_sf(index);
                    vec asd_sf =    "0.48 0.52 0.52 0.53 0.57 0.53 0.50 0.49 0.38";  ASD_SF = -asd_sf(index);
                    vec ds_sf  =    "0.21 0.25 0.21 0.26 0.25 0.20 0.19 0.20 0.19";  DS_SF  = -ds_sf(index);
                    vec asd_asa =   "0.41 0.42 0.34 0.38 0.28 0.20 0.26 0.23 0.21";  ASD_ASA = asd_asa(index);
                    vec zsd_sf =    "-0.02 0.0 0.01 0.01 0.03 0.03 -0.02 -0.05 -0.12";  ZSD_SF = zsd_sf(index);
                    vec zsa_sf =    "0.31 0.32 0.33 0.33 0.41 0.40 0.36 0.37 0.33";  ZSA_SF = -zsa_sf(index);
                    vec zsd_ds  =   "0.68 0.72 0.68 0.67 0.65 0.67 0.63 0.61 0.54";  ZSD_DS = zsd_ds(index);
                    vec zsa_ds =    "0.06 0.10 0.11 0.13 -0.04 -0.14 -0.11 -0.24 -0.19";  ZSA_DS = zsa_ds(index);
                    vec zsd_asd =   "0.52 0.48 0.59 0.55 0.54 0.60 0.64 0.60 0.60";  ZSD_ASD = zsd_asd(index);
                    vec zsa_asd =   "0.06 0.12 0.14 0.18 0.01 -0.1 -0.11 -0.24 -0.20";  ZSA_ASD = zsa_asd(index);
                    vec zsd_asa =   "0.40 0.41 0.34 0.38 0.31 0.25 0.23 0.22 0.29";  ZSD_ASA = zsd_asa(index);
                    vec zsa_asa =   "0.05 0.13 0.16 0.16 0.18 0.21 0.02 -0.13 -0.35";  ZSA_ASA = zsa_asa(index);
                    vec zsd_zsa =   "-0.02 0.04 0.09 0.11 0 -0.09 -0.15 -0.29 -0.33";  ZSD_ZSA = zsd_zsa(index);
                }
            }
            break;
            
        case _SCMNTN_SUB_URBAN_:
            if(isLOS)
            {
                if(!isKaBand)
                {
                    vec asd_ds =    0.4*ones(9);  ASD_DS = asd_ds(index);
                    vec asa_ds =    0.8*ones(9);  ASA_DS = asa_ds(index);
                    vec asa_sf =   -0.5*ones(9);  ASA_SF = asa_sf(index);
                    vec asd_sf =   -0.5*ones(9);  ASD_SF = asd_sf(index);
                    vec ds_sf  =   -0.4*ones(9);  DS_SF  = ds_sf(index);
                    vec asd_asa =   0.0*ones(9);  ASD_ASA = asd_asa(index);
                    vec asd_k =     0.0*ones(9);  ASD_K = asd_k(index);
                    vec asa_k  =   -0.2*ones(9);  ASA_K = asa_k(index);
                    vec ds_k   =   -0.4*ones(9);  DS_K  = ds_k(index);
                    vec sf_k =      0.0*ones(9);  SF_K = sf_k(index);
                    vec zsd_sf =    0.0*ones(9);  ZSD_SF = zsd_sf(index);
                    vec zsa_sf =   -0.8*ones(9);  ZSA_SF = zsa_sf(index);
                    vec zsd_k =     0.0*ones(9);  ZSD_K = zsd_k(index);
                    vec zsa_k =     0.0*ones(9);  ZSA_K = zsa_k(index);
                    vec zsd_ds  =  -0.2*ones(9);  ZSD_DS = zsd_ds(index);
                    vec zsa_ds =    0.0*ones(9);  ZSA_DS = zsa_ds(index);
                    vec zsd_asd =   0.5*ones(9);  ZSD_ASD = zsd_asd(index);
                    vec zsa_asd =   0.0*ones(9);  ZSA_ASD = zsa_asd(index);
                    vec zsd_asa =  -0.3*ones(9);  ZSD_ASA = zsd_asa(index);
                    vec zsa_asa =   0.4*ones(9);  ZSA_ASA = zsa_asa(index);
                    vec zsd_zsa =   0.0*ones(9);  ZSD_ZSA = zsd_zsa(index);
                }
                else
                {
                    vec asd_ds =    0.4*ones(9);  ASD_DS = asd_ds(index);
                    vec asa_ds =    0.8*ones(9);  ASA_DS = asa_ds(index);
                    vec asa_sf =   -0.5*ones(9);  ASA_SF = asa_sf(index);
                    vec asd_sf =   -0.5*ones(9);  ASD_SF = asd_sf(index);
                    vec ds_sf  =   -0.4*ones(9);  DS_SF  = ds_sf(index);
                    vec asd_asa =   0.0*ones(9);  ASD_ASA = asd_asa(index);
                    vec asd_k =     0.0*ones(9);  ASD_K = asd_k(index);
                    vec asa_k  =   -0.2*ones(9);  ASA_K = asa_k(index);
                    vec ds_k   =   -0.4*ones(9);  DS_K  = ds_k(index);
                    vec sf_k =      0.0*ones(9);  SF_K = sf_k(index);
                    vec zsd_sf =    0.0*ones(9);  ZSD_SF = zsd_sf(index);
                    vec zsa_sf =   -0.8*ones(9);  ZSA_SF = zsa_sf(index);
                    vec zsd_k =     0.0*ones(9);  ZSD_K = zsd_k(index);
                    vec zsa_k =     0.0*ones(9);  ZSA_K = zsa_k(index);
                    vec zsd_ds  =  -0.2*ones(9);  ZSD_DS = zsd_ds(index);
                    vec zsa_ds =    0.0*ones(9);  ZSA_DS = zsa_ds(index);
                    vec zsd_asd =   0.5*ones(9);  ZSD_ASD = zsd_asd(index);
                    vec zsa_asd =   0.0*ones(9);  ZSA_ASD = zsa_asd(index);
                    vec zsd_asa =  -0.3*ones(9);  ZSD_ASA = zsd_asa(index);
                    vec zsa_asa =   0.4*ones(9);  ZSA_ASA = zsa_asa(index);
                    vec zsd_zsa =   0.0*ones(9);  ZSD_ZSA = zsd_zsa(index);
                }
            }
            else
            {
                if(!isKaBand)
                {
                    vec asd_ds =    0.4*ones(9);  ASD_DS = asd_ds(index);
                    vec asa_ds =    0.6*ones(9);  ASA_DS = asa_ds(index);
                    vec asa_sf =    0.0*ones(9);  ASA_SF = asa_sf(index);
                    vec asd_sf =   -0.6*ones(9);  ASD_SF = asd_sf(index);
                    vec ds_sf  =   -0.4*ones(9);  DS_SF  = ds_sf(index);
                    vec asd_asa =   0.4*ones(9);  ASD_ASA = asd_asa(index);
                    vec zsd_sf =    0.0*ones(9);  ZSD_SF = zsd_sf(index);
                    vec zsa_sf =   -0.4*ones(9);  ZSA_SF = zsa_sf(index);
                    vec zsd_ds  =  -0.5*ones(9);  ZSD_DS = zsd_ds(index);
                    vec zsa_ds =    0.0*ones(9);  ZSA_DS = zsa_ds(index);
                    vec zsd_asd =   0.5*ones(9);  ZSD_ASD = zsd_asd(index);
                    vec zsa_asd =  -0.1*ones(9);  ZSA_ASD = zsa_asd(index);
                    vec zsd_asa =   0.0*ones(9);  ZSD_ASA = zsd_asa(index);
                    vec zsa_asa =   0.0*ones(9);  ZSA_ASA = zsa_asa(index);
                    vec zsd_zsa =   0.0*ones(9);  ZSD_ZSA = zsd_zsa(index);
                }
                else
                {
                    vec asd_ds =    0.4*ones(9);  ASD_DS = asd_ds(index);
                    vec asa_ds =    0.6*ones(9);  ASA_DS = asa_ds(index);
                    vec asa_sf =    0.0*ones(9);  ASA_SF = asa_sf(index);
                    vec asd_sf =   -0.6*ones(9);  ASD_SF = asd_sf(index);
                    vec ds_sf  =   -0.4*ones(9);  DS_SF  = ds_sf(index);
                    vec asd_asa =   0.4*ones(9);  ASD_ASA = asd_asa(index);
                    vec zsd_sf =    0.0*ones(9);  ZSD_SF = zsd_sf(index);
                    vec zsa_sf =   -0.4*ones(9);  ZSA_SF = zsa_sf(index);
                    vec zsd_ds  =  -0.5*ones(9);  ZSD_DS = zsd_ds(index);
                    vec zsa_ds =    0.0*ones(9);  ZSA_DS = zsa_ds(index);
                    vec zsd_asd =   0.5*ones(9);  ZSD_ASD = zsd_asd(index);
                    vec zsa_asd =  -0.1*ones(9);  ZSA_ASD = zsa_asd(index);
                    vec zsd_asa =   0.0*ones(9);  ZSD_ASA = zsd_asa(index);
                    vec zsa_asa =   0.0*ones(9);  ZSA_ASA = zsa_asa(index);
                    vec zsd_zsa =   0.0*ones(9);  ZSD_ZSA = zsd_zsa(index);
                }
            }
            break;
            
        case _SCMNTN_RURAL_:
            if(isLOS)
            {
                if(!isKaBand)
                {
                    vec asd_ds =    0.0*ones(9);  ASD_DS = asd_ds(index);
                    vec asa_ds =    0.0*ones(9);  ASA_DS = asa_ds(index);
                    vec asa_sf =    0.0*ones(9);  ASA_SF = asa_sf(index);
                    vec asd_sf =    0.0*ones(9);  ASD_SF = asd_sf(index);
                    vec ds_sf  =   -0.5*ones(9);  DS_SF  = ds_sf(index);
                    vec asd_asa =   0.0*ones(9);  ASD_ASA = asd_asa(index);
                    vec asd_k =     0.0*ones(9);  ASD_K = asd_k(index);
                    vec asa_k  =    0.0*ones(9);  ASA_K = asa_k(index);
                    vec ds_k   =    0.0*ones(9);  DS_K  = ds_k(index);
                    vec sf_k =      0.0*ones(9);  SF_K = sf_k(index);
                    vec zsd_sf =    0.01*ones(9);  ZSD_SF = zsd_sf(index);
                    vec zsa_sf =   -0.17*ones(9);  ZSA_SF = zsa_sf(index);
                    vec zsd_k =     0.0*ones(9);  ZSD_K = zsd_k(index);
                    vec zsa_k =    -0.02*ones(9);  ZSA_K = zsa_k(index);
                    vec zsd_ds  =  -0.05*ones(9);  ZSD_DS = zsd_ds(index);
                    vec zsa_ds =    0.27*ones(9);  ZSA_DS = zsa_ds(index);
                    vec zsd_asd =   0.73*ones(9);  ZSD_ASD = zsd_asd(index);
                    vec zsa_asd =  -0.14*ones(9);  ZSA_ASD = zsa_asd(index);
                    vec zsd_asa =  -0.20*ones(9);  ZSD_ASA = zsd_asa(index);
                    vec zsa_asa =   0.24*ones(9);  ZSA_ASA = zsa_asa(index);
                    vec zsd_zsa =  -0.07*ones(9);  ZSD_ZSA = zsd_zsa(index);
                     //cout<<"ASD_DS,ASA_DS,ASA_SF,ASD_SF,DS_SF,ASD_ASA,ASD_K,ASA_K,DS_K,SF_K,ZSD_SF, ZSA_SF, ZSD_K, ZSA_K, ZSD_DS, ZSA_DS, ZSD_ASD, ZSA_ASD, ZSD_ASA, ZSA_ASA, ZSD_ZSA"<<ASD_DS<<","<<ASA_DS<<","<<ASA_SF<<","<<ASD_SF<<","<<DS_SF<<","<<ASD_ASA<<","<<ASD_K<<","<<ASA_K<<","<<DS_K<<","<<SF_K<<","<<ZSD_SF<<","<< ZSA_SF<<","<<ZSD_K<<","<<ZSA_K<<","<<ZSD_DS<<","<<ZSA_DS<<","<<ZSD_ASD<<","<< ZSA_ASD<<","<<ZSD_ASA<<","<<ZSA_ASA<<","<<ZSD_ZSA<<endl;
                }

                else
                {
                    vec asd_ds =    0.0*ones(9);  ASD_DS = asd_ds(index);
                    vec asa_ds =    0.0*ones(9);  ASA_DS = asa_ds(index);
                    vec asa_sf =    0.0*ones(9);  ASA_SF = asa_sf(index);
                    vec asd_sf =    0.0*ones(9);  ASD_SF = asd_sf(index);
                    vec ds_sf  =   -0.5*ones(9);  DS_SF  = ds_sf(index);
                    vec asd_asa =   0.0*ones(9);  ASD_ASA = asd_asa(index);
                    vec asd_k =     0.0*ones(9);  ASD_K = asd_k(index);
                    vec asa_k  =    0.0*ones(9);  ASA_K = asa_k(index);
                    vec ds_k   =    0.0*ones(9);  DS_K  = ds_k(index);
                    vec sf_k =      0.0*ones(9);  SF_K = sf_k(index);
                    vec zsd_sf =    0.01*ones(9);  ZSD_SF = zsd_sf(index);
                    vec zsa_sf =   -0.17*ones(9);  ZSA_SF = zsa_sf(index);
                    vec zsd_k =     0.0*ones(9);  ZSD_K = zsd_k(index);
                    vec zsa_k =    -0.02*ones(9);  ZSA_K = zsa_k(index);
                    vec zsd_ds  =  -0.05*ones(9);  ZSD_DS = zsd_ds(index);
                    vec zsa_ds =    0.27*ones(9);  ZSA_DS = zsa_ds(index);
                    vec zsd_asd =   0.73*ones(9);  ZSD_ASD = zsd_asd(index);
                    vec zsa_asd =  -0.14*ones(9);  ZSA_ASD = zsa_asd(index);
                    vec zsd_asa =  -0.20*ones(9);  ZSD_ASA = zsd_asa(index);
                    vec zsa_asa =   0.24*ones(9);  ZSA_ASA = zsa_asa(index);
                    vec zsd_zsa =  -0.07*ones(9);  ZSD_ZSA = zsd_zsa(index);
                }
            }
            else
            {
                if(!isKaBand)
                {
                    vec asd_ds =    "0.32 0.18 0.23 0.25 0.15 0.08 0.13 0.15 0.64";  ASD_DS = asd_ds(index);
                    vec asa_ds =    "0.30 0.32 0.32 0.40 0.45 0.39 0.51 0.27 0.05";  ASA_DS = asa_ds(index);
                    vec asa_sf =    "0.02 0.00 0.00 0.01 0.02 0.02 0.04 0.01 0.06";  ASA_SF = asa_sf(index);
                    vec asd_sf =    "0.45 0.52 0.54 0.53 0.55 0.56 0.56 0.58 0.47";  ASD_SF = asd_sf(index);
                    vec ds_sf  =    "0.36 0.39 0.41 0.37 0.40 0.41 0.40 0.46 0.30";  DS_SF  = -ds_sf(index);
                    vec asd_asa =   "0.45 0.12 0.07 0.22 0.16 0.14 0.20 -0.04 -0.11";  ASD_ASA = asd_asa(index);
                    vec zsd_sf =    "0.06 0.04 0.04 0.05 0.06 0.07 0.11 0.05 0.10";  ZSD_SF = -zsd_sf(index);
                    vec zsa_sf =    "0.07 0.17 0.19 0.17 0.19 0.20 0.19 0.23 0.13";  ZSA_SF = -zsa_sf(index);
                    vec zsd_ds  =   "0.58 0.67 0.65 0.73 0.79 0.81 0.79 0.70 0.42";  ZSD_DS = zsd_ds(index);
                    vec zsa_ds =    "0.06 0.03 0.0 -0.09 -0.20 -0.22 -0.32 -0.41 -0.35";  ZSA_DS = zsa_ds(index);
                    vec zsd_asd =   "0.60 0.41 0.37 0.32 0.19 0.16 0.20 0.15 0.28";  ZSD_ASD = zsd_asd(index);
                    vec zsa_asd =   "0.21 -0.02 -0.09 -0.1 -0.12 -0.11 -0.1 -0.14 -0.25";  ZSA_ASD = zsa_asd(index);
                    vec zsd_asa =   "0.33 0.35 0.31 0.37 0.46 0.44 0.49 0.27 0.07";  ZSD_ASA = zsd_asa(index);
                    vec zsa_asa =   "0.10 0.21 0.22 0.07 -0.04 -0.12 -0.29 -0.26 -0.36";  ZSA_ASA = zsa_asa(index);
                    vec zsd_zsa =   "0.01 -0.02 -0.12 -0.21 -0.27 -0.27 -0.38 -0.35 -0.36";  ZSD_ZSA = zsd_zsa(index);
                }
                else
                {
                    vec asd_ds =    "0.33 0.24 0.21 0.26 0.16 0.12 0.29 0.14 0.59";  ASD_DS = asd_ds(index);
                    vec asa_ds =    "0.32 0.34 0.33 0.43 0.46 0.38 0.37 0.28 0.06";  ASA_DS = asa_ds(index);
                    vec asa_sf =    "0.02 0.00 0.00 0.01 0.01 0.02 0.04 0.01 0.04";  ASA_SF = asa_sf(index);
                    vec asd_sf =    "0.45 0.52 0.54 0.53 0.55 0.56 0.54 0.57 0.46";  ASD_SF = asd_sf(index);
                    vec ds_sf  =    "0.36 0.38 0.42 0.36 0.39 0.42 0.36 0.44 0.27";  DS_SF  = -ds_sf(index);
                    vec asd_asa =   "0.45 0.13 0.08 0.21 0.12 0.15 0.22 -0.03 -0.11";  ASD_ASA = asd_asa(index);
                    vec zsd_sf =    "0.07 0.04 0.04 0.05 0.06 0.06 0.09 0.06 0.08";  ZSD_SF = -zsd_sf(index);
                    vec zsa_sf =    "0.06 0.16 0.19 0.16 0.19 0.20 0.17 0.22 0.11";  ZSA_SF = -zsa_sf(index);
                    vec zsd_ds  =   "0.55 0.65 0.64 0.73 0.78 0.77 0.74 0.75 0.52";  ZSD_DS = zsd_ds(index);
                    vec zsa_ds =    "0.06 0.02 0.04 -0.06 -0.16 -0.17 -0.30 -0.35 -0.28";  ZSA_DS = zsa_ds(index);
                    vec zsd_asd =   "0.61 0.41 0.39 0.44 0.15 0.20 0.30 0.11 0.41";  ZSD_ASD = zsd_asd(index);
                    vec zsa_asd =   "0.19 -0.02 -0.06 -0.08 -0.13 -0.09 -0.09 -0.14 -0.25";  ZSA_ASD = zsa_asd(index);
                    vec zsd_asa =   "0.38 0.35 0.33 0.40 0.46 0.45 0.33 0.29 0.06";  ZSD_ASA = zsd_asa(index);
                    vec zsa_asa =   "0.12 0.21 0.22 0.11 0.02 -0.08 -0.20 -0.16 -0.18";  ZSA_ASA = zsa_asa(index);
                    vec zsd_zsa =   "0.05 -0.03 -0.08 -0.20 -0.25 -0.24 -0.37 -0.31 -0.32";  ZSD_ZSA = zsd_zsa(index);
                }
            }
            break;
            
        default:
            cout<<"Invalid scmScenario for NTN..."<<endl;abort();
            break;
    }
    
    if (isLOS == 1)
    {    
        /*
        * This is matirx for correlation matrix
        * 
        *      DS  ASD  ASA  SF  K  ZSA  ZSD
        * DS
        * 
        * ASD
        * 
        * ASA
        * 
        * SF
        * 
        * K
        * 
        * ZSA
        * 
        * ZSD
        */   
        
        Corr.set_size(7,7);Corr.zeros(); 
        Corr(0,0) = 1.0; Corr(0,1) = ASD_DS; Corr(0,2) = ASA_DS; Corr(0,3) = DS_SF; Corr(0,4) = DS_K;Corr(0,5) = ZSA_DS; Corr(0,6) = ZSD_DS;

        Corr(1,0) = ASD_DS; Corr(1,1) = 1.0; Corr(1,2) = ASD_ASA; Corr(1,3) = ASD_SF; Corr(1,4) = ASD_K;Corr(1,5) = ZSA_ASD; Corr(1,6) = ZSD_ASD;

        Corr(2,0) = ASA_DS; Corr(2,1) = ASD_ASA; Corr(2,2) = 1.0; Corr(2,3) = ASA_SF; Corr(2,4) = ASA_K;Corr(2,5) = ZSA_ASA; Corr(2,6) = ZSD_ASA;

        Corr(3,0) = DS_SF; Corr(3,1) = ASD_SF; Corr(3,2) = ASA_SF; Corr(3,3) = 1.0; Corr(3,4) = SF_K;Corr(3,5) = ZSA_SF; Corr(3,6) = ZSD_SF;

        Corr(4,0) = DS_K; Corr(4,1) = ASD_K; Corr(4,2) = ASA_K; Corr(4,3) = SF_K; Corr(4,4) = 1.0;Corr(4,5) = ZSA_K; Corr(4,6) = ZSD_K;
        
        Corr(5,0) = ZSA_DS; Corr(5,1) = ZSA_ASD; Corr(5,2) = ZSA_ASA; Corr(5,3) = ZSA_SF; Corr(5,4) = ZSA_K;Corr(5,5) = 1.0; Corr(5,6) = ZSD_ZSA;

        Corr(6,0) = ZSD_DS; Corr(6,1) = ZSD_ASD; Corr(6,2) = ZSD_ASA; Corr(6,3) = ZSD_SF; Corr(6,4) = ZSD_K;Corr(6,5) = ZSD_ZSA; Corr(6,6) = 1.0;    

    }
    else
    {
        /*
        * This is matirx for correlation matrix
        * 
        *     DS  ASD  ASA  SF  ZSA  ZSD
        * DS
        * 
        * ASD
        * 
        * ASA
        * 
        * SF
        * 
        * ZSA
        * 
        * ZSD
        */     
        Corr.set_size(6,6);Corr.zeros(); 
        Corr(0,0) = 1.0; Corr(0,1) = ASD_DS; Corr(0,2) = ASA_DS; Corr(0,3) = DS_SF;Corr(0,4) = ZSA_DS; Corr(0,5) = ZSD_DS; 
        Corr(1,0) = ASD_DS; Corr(1,1) = 1.0; Corr(1,2) = ASD_ASA; Corr(1,3) = ASD_SF;Corr(1,4) = ZSA_ASD; Corr(1,5) = ZSD_ASD;
        Corr(2,0) = ASA_DS; Corr(2,1) = ASD_ASA; Corr(2,2) = 1.0; Corr(2,3) = ASA_SF;Corr(2,4) = ZSA_ASA; Corr(2,5) = ZSD_ASA;
        Corr(3,0) = DS_SF; Corr(3,1) = ASD_SF; Corr(3,2) = ASA_SF; Corr(3,3) = 1.0;Corr(3,4) = ZSA_SF; Corr(3,5) = ZSD_SF; 
        
        Corr(4,0) = ZSA_DS; Corr(4,1) = ZSA_ASD; Corr(4,2) = ZSA_ASA; Corr(4,3) = ZSA_SF; Corr(4,4) = 1.0; Corr(4,5) = ZSD_ZSA;    
        Corr(5,0) = ZSD_DS; Corr(5,1) = ZSD_ASD; Corr(5,2) = ZSD_ASA; Corr(5,3) = ZSD_SF; Corr(5,4) = ZSD_ZSA; Corr(5,5) = 1.0;    
        
    }
    return Corr;
}

vec getlspMuSigmaForNTN(SCMPropagationScenario scmScenario,double elevationAngle,bool isLOS,bool isKaBand)
{
    mat lspMuSigma;
    switch(scmScenario)
    {
        case _SCMNTN_DENSE_URBAN_:
            if(isLOS)
            {
                if(!isKaBand)
                {
                    lspMuSigma =    "-7.12	-7.28	-7.45	-7.73	-7.91	-8.14	-8.23	-8.28	-8.36;"
                                    "0.8	0.67	0.68	0.66	0.62	0.51	0.45	0.31	0.08;"
                                    "-3.06	-2.68	-2.51	-2.4	-2.31	-2.2	-2	-1.64	-0.63;"
                                    "0.48	0.36	0.38	0.32	0.33	0.39	0.4	0.32	0.53;"
                                    "0.94	0.87	0.92	0.79	0.72	0.6	0.55	0.71	0.81;"
                                    "0.7	0.66	0.68	0.64	0.63	0.54	0.52	0.53	0.62;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "3.5 3.4 2.9 3.0 3.1 2.7 2.5 2.3 1.2;"
                                    "4.4	9	9.3	7.9	7.4	7	6.9	6.5	6.8;"
                                    "3.3	6.6	6.1	4	3	2.6	2.2	2.1	1.9;"
                                    "0.82	0.5	0.82	1.23	1.43	1.56	1.66	1.73	1.79;"
                                    "0.03	0.09	0.05	0.03	0.06	0.05	0.05	0.02	0.01;"
                                    "-2.52	-2.29	-2.19	-2.24	-2.3	-2.48	-2.64	-2.68	-2.61;"
                                    "0.5	0.53	0.58	0.51	0.46	0.35	0.31	0.39	0.28;";
                }
                else
                {
                    lspMuSigma =    "-7.43	-7.62	-7.76	-8.02	-8.13	-8.3	-8.34	-8.39	-8.45,"
                                    "0.9	0.78	0.8	0.72	0.61	0.47	0.39	0.26	0.01,"
                                    "-3.43	-3.06	-2.91	-2.81	-2.74	-2.72	-2.46	-2.3	-1.11,"
                                    "0.54	0.41	0.42	0.34	0.34	0.7	0.4	0.78	0.51,"
                                    "0.65	0.53	0.6	0.43	0.36	0.16	0.18	0.24	0.36,"
                                    "0.82	0.78	0.83	0.78	0.77	0.84	0.64	0.81	0.65,"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "2.9 2.4 2.7 2.4 2.4 2.7 2.6 2.8 0.6;"
                                    "6.1	13.7	12.9	10.3	9.2	8.4	8	7.4	7.6,"
                                    "2.6	6.8	6	3.3	2.2	1.9	1.5	1.6	1.3;"
                                    "0.82	0.47	0.8	1.23	1.42	1.56	1.65	1.73	1.79,"
                                    "0.05	0.11	0.05	0.04	0.1	0.06	0.07	0.02	0.01,"
                                    "-2.75	-2.64	-2.49	-2.51	-2.54	-2.71	-2.85	-3.01	-3.08,"
                                    "0.55	0.64	0.69	0.57	0.5	0.37	0.31	0.45	0.27,";
                }
            }
            else
            {
                if(!isKaBand)
                {
                    lspMuSigma =    "-6.84	-6.81	-6.94	-7.14	-7.34	-7.53	-7.67	-7.82	-7.84;"
                                    "0.82	0.61	0.49	0.49	0.51	0.47	0.44	0.42	0.55;"
                                    "-2.08	-1.68	-1.46	-1.43	-1.44	-1.33	-1.31	-1.11	-0.11;"
                                    "0.87	0.73	0.53	0.5	0.58	0.49	0.65	0.69	0.53;"
                                    "1	1.44	1.54	1.53	1.48	1.39	1.42	1.38	1.23;"
                                    "1.6	0.87	0.64	0.56	0.54	0.68	0.55	0.6	 0.6;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "15.5 13.9 12.4 11.7 10.6 10.5 10.1 9.2 9.2;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "1	0.94	1.15	1.35	1.44	1.56	1.64	1.7	 1.7;"
                                    "0.63	0.65	0.42	0.28	0.25	0.16	0.18	0.09	0.17;"
                                    "-2.08	-1.66	-1.48	-1.46	-1.53	-1.61	-1.77	-1.9	-1.99;"
                                    "0.58	0.5	0.4	0.37	0.47	0.43	0.5 0.42	0.5;";
                }
                else 
                {
                    lspMuSigma =    "-6.86 -6.84	-7	-7.21	-7.42	-7.86	-7.76	-8.07	-7.95;"
                                    "0.81	0.61	0.56	0.56	0.57	0.55	0.47	0.42	0.59;"
                                    "-2.12	-1.74	-1.56	-1.54	-1.45	-1.64	-1.37	-1.29	-0.41;"
                                    "0.94	0.79	0.66	0.63	0.56	0.78	0.56	0.76	0.59;"
                                    "1.02	1.44	1.48	1.46	1.4	0.97	1.33	1.12	1.04;"
                                    "1.44	0.77	0.7	0.6	0.59	1.27	0.56	1.04	0.63;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "17.1 17.1 15.6 14.6 14.2 12.6 12.1 12.3 12.3;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "1.01	0.96	1.13	1.3	1.4	1.41	1.63	1.68	1.7;"
                                    "0.56	0.55	0.43	0.37	0.32	0.45	0.17	0.14	0.17;"
                                    "-2.11	-1.69	-1.52	-1.51	-1.54	-1.84	-1.86	-2.16	-2.21;"
                                    "0.59	0.51	0.46	0.43	0.45	0.63	0.51	0.74	0.61;";
                }
            }
            break;
        case _SCMNTN_URBAN_:
            if(isLOS)
            {
                if(!isKaBand)
                {
                    lspMuSigma =    "-7.97	-8.12	-8.21	-8.31	-8.37	-8.39	-8.38	-8.35	-8.34;"
                                    "1	0.83	0.68	0.48	0.38	0.24	0.18	0.13	0.09;"
                                    "-2.6	-2.48	-2.44	-2.6	-2.71	-2.76	-2.78	-2.65	-2.27;"
                                    "0.79	0.8 0.91	1.02	1.17	1.17	1.2	1.45	1.85;"
                                    "0.18	0.42	0.41	0.18	-0.07	-0.43	-0.64	-0.91	-0.54;"
                                    "0.74   0.9	    1.3	    1.69	2.04	2.54	2.47	2.69	1.66;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "4 4 4 4 4 4 4 4 4;"
                                    "31.83	18.78	10.49	7.46	6.52	5.47	4.54	4.03	3.68;"
                                    "13.84	13.78	10.42	8.01	8.27	7.26	5.53	4.49	3.14;"\
                                    "-0.63	-0.15	0.54	0.35	0.27	0.26	-0.12	-0.21	-0.07;"
                                    "2.6	3.31	1.1	1.59	1.62	0.97	1.99	1.82	1.43;"
                                    "-2.54	-2.67	-2.03	-2.28	-2.48	-2.56	-2.96	-3.08	-3;"
                                    "2.62	2.96	0.86	1.19	1.4	0.85	1.61	1.49	1.09;";
                }
                else
                {
                    lspMuSigma =    "-8.52	-8.59	-8.51	-8.49	-8.48	-8.44	-8.4	-8.37	-8.35;"
                                    "0.92	0.79	0.65	0.48	0.46	0.34	0.27	0.19	0.14;"
                                    "-3.18	-3.05	-2.98	-3.11	-3.19	-3.25	-3.33	-3.22	-2.83;"
                                    "0.79	0.87	1.04	1.06	1.12	1.14	1.25	1.35	1.62;"
                                    "-0.4	-0.15	-0.18	-0.31	-0.58	-0.9	-1.16	-1.48	-1.14;"
                                    "0.77	0.97	1.58	1.69	2.13	2.51	2.47	2.61	1.7;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "4 4 4 4 4 4 4 4 4;"
                                    "40.18	23.62	12.48	8.56	7.42	5.97	4.88	4.22	3.81;"
                                    "16.99	18.96	14.23	11.06	11.21	9.47	7.24	5.79	4.25"
                                    "-0.67	-0.34	0.07	-0.08	-0.21	-0.25	-0.61	-0.79	-0.58;"
                                    "2.22	3.04	1.33	1.45	1.62	1.06	1.88	1.87	1.19;"
                                    "-2.61	-2.82	-2.48	-2.76	-2.93	-3.05	-3.45	-3.66	-3.56;"
                                    "2.41	2.59	1.02	1.27	1.38	0.96	1.51	1.49	0.89;";
                }
            }
            else
            {
                if(!isKaBand)
                {
                    lspMuSigma =    "-7.21	-7.63	-7.75	-7.97	-7.99	-8.01	-8.09	-7.97	-8.17;"
                                    "1.19	0.98	0.84	0.73	0.73	0.72	0.71	0.78	0.67;"
                                    "-1.55	-1.61	-1.73	-1.95	-1.94	-1.88	-2.1	-1.8	-1.77;"
                                    "0.87	0.88	1.15	1.13	1.21	0.99	1.77	1.54	1.4;"
                                    "0.17	0.32	0.52	0.61	0.68	0.64	0.58	0.71	0.49;"
                                    "2.97	2.99	2.71	2.26	2.08	1.93	1.71	0.96	1.16;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "6 6 6 6 6 6 6 6 6;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "-0.97	0.49	1.03	1.12	1.3	1.32	1.35	1.31	1.5;"
                                    "2.35	2.11	1.29	1.45	1.07	1.2	1.1	1.35	0.56;"
                                    "-2.86	-2.64	-2.05	-2.18	-2.24	-2.21	-2.69	-2.81	-4.29;"
                                    "2.77	2.79	1.53	1.67	1.95	1.87	2.72	2.98	4.37;";
                }
                else
                {
                    lspMuSigma =    "-7.24	-7.7	-7.82	-8.04	-8.08	-8.1	-8.16	-8.03	-8.33;"
                                    "1.26	0.99	0.86	0.75	0.77	0.76	0.73	0.79	0.7;"
                                    "-1.58	-1.67	-1.84	-2.02	-2.06	-1.99	-2.19	-1.88	-2;"
                                    "0.89	0.89	1.3	1.15	1.23	1.02	1.78	1.55	1.4;"
                                    "0.13	0.19	0.44	0.48	0.56	0.55	0.48	0.53	0.32;"
                                    "2.99	3.12	2.69	2.45	2.17	1.93	1.72	1.51	1.2;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "6 6 6 6 6 6 6 6 6;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "-1.13	0.49	0.95	1.15	1.14	1.13	1.16	1.28	1.42;"
                                    "2.66	2.03	1.54	1.02	1.61	1.84	1.81	1.35	0.6;"
                                    "-2.87	-2.68	-2.12	-2.27	-2.5	-2.47	-2.83	-2.82	-4.55;"
                                    "2.76	2.76	1.54	1.77	2.36	2.33	2.84	2.87	4.27;";
                }
                    
            }
            break;
        case _SCMNTN_SUB_URBAN_:
            if(isLOS)
            {
                if(!isKaBand)
                {
                    lspMuSigma =    "-8.16	-8.56	-8.72	-8.71	-8.72	-8.66	-8.38	-8.34	-8.34;"
                                    "0.99	0.96	0.79	0.81	1.12	1.23	0.55	0.63	0.63;"
                                    "-3.57	-3.8	-3.77	-3.57	-3.42	-3.27	-3.08	-2.75	-2.75;"
                                    "1.62	1.74	1.72	1.6	1.49	1.43	1.36	1.26	1.26;"
                                    "0.05	-0.38	-0.56	-0.59	-0.58	-0.55	-0.28	-0.17	-0.17;"
                                    "1.84	1.94	1.75	1.82	1.87	1.92	1.16	1.09	1.09;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "1.79 1.14 1.14 0.92 1.42 1.56 0.85 0.72 0.72;"
                                    "11.4	19.45	20.8	21.2	21.6	19.75	12	12.85	12.85;"
                                    "6.26	10.32	16.34	15.63	14.22	14.19	5.7	9.91	9.91;"
                                    "-1.78	-1.84	-1.67	-1.59	-1.55	-1.51	-1.27	-1.28	-1.28;"
                                    "0.62	0.81	0.57	0.86	1.05	1.23	0.54	0.67	0.67;"
                                    "-1.06	-1.21	-1.28	-1.32	-1.39	-1.36	-1.08	-1.31	-1.31;"
                                    "0.96	0.95	0.49	0.79	0.97	1.17	0.62	0.76	0.76;";
                }
                else
                {
                    lspMuSigma =    "-8.07	-8.61	-8.72	-8.63	-8.54	-8.48	-8.42	-8.39	-8.37;"
                                    "0.46	0.45	0.28	0.17	0.14	0.15	0.09	0.05	0.02;"
                                    "-3.55	-3.69	-3.59	-3.38	-3.23	-3.19	-2.83	-2.66	-1.22;"
                                    "0.48	0.41	0.41	0.35	0.35	0.43	0.33	0.44	0.31;"
                                    "0.89	0.31	0.02	-0.1	-0.19	-0.54	-0.24	-0.52	-0.15;"
                                    "0.67	0.78	0.75	0.65	0.55	0.96	0.43	0.93	0.44;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "1.9 1.6 1.9 2.3 2.7 3.1 3.0 3.6 0.4;"
                                    "8.9	14	11.3	9	7.5	6.6	5.9	5.5	5.4;"
                                    "4.4	4.6	3.7	3.5	3	2.6	1.7	0.7	0.3;"
                                    "0.63	0.76	1.11	1.37	1.53	1.65	1.74	1.82	1.87;"
                                    "0.35	0.3	0.28	0.23	0.23	0.17	0.11	0.05	0.02;"
                                    "-3.37	-3.28	-3.04	-2.88	-2.83	-2.86	-2.95	-3.21	-3.49;"
                                    "0.28	0.27	0.26	0.21	0.18	0.17	0.1	0.07	0.24;";
                }
            }
            else
            {
                if(!isKaBand)
                {
                    lspMuSigma =    "-7.91	-8.39	-8.69	-8.59	-8.64	-8.74	-8.98	-9.28	-9.28;"
                                    "1.42	1.46	1.46	1.21	1.18	1.13	1.37	1.5	1.5;"
                                    "-3.54	-3.63	-3.66	-3.66	-3.66	-3.57	-3.18	-2.71	-2.71;"
                                    "1.8	1.43	1.68	1.48	1.55	1.38	1.62	1.63	1.63;"
                                    "0.91	0.7	0.38	0.3	0.28	0.23	0.1	0.04	0.04;"
                                    "1.7	1.33	1.52	1.46	1.44	1.44	1.24	1.04	1.04;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "8.93 9.08 8.78 10.25 10.56 10.74 0.17 11.52 11.52;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "-1.9	-1.7	-1.75	-1.8	-1.8	-1.85	-1.45	-1.19	-1.19;"
                                    "1.63	1.24	1.54	1.25	1.21	1.2	1.38	1.58	1.58;"
                                    "-2.01	-1.67	-1.75	-1.49	-1.53	-1.57	-1.48	-1.62	-1.62;"
                                    "1.79	1.31	1.42	1.28	1.4	1.24	0.98	0.88	0.88;";
                }
                else
                {
                    lspMuSigma =    "-7.43	-7.63	-7.86	-7.96	-7.98	-8.45	-8.21	-8.69	-8.69;"
                                    "0.5	0.61	0.56	0.58	0.59	0.47	0.36	0.29	0.29;"
                                    "-2.89	-2.76	-2.64	-2.41	-2.42	-2.53	-2.35	-2.31	-2.31;"
                                    "0.41	0.41	0.41	0.52	0.7	0.5	0.58	0.73	0.73;"
                                    "1.49	1.24	1.06	0.91	0.98	0.49	0.73	-0.04	-0.04;"
                                    "0.4	0.82	0.71	0.55	0.58	1.37	0.49	1.48	1.48;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "10.7 10.0 11.2 11.6 11.8 10.8 10.8 10.8 10.8;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "0.81	1.06	1.12	1.14	1.29	1.38	1.36	1.38	1.38;"
                                    "0.36	0.41	0.4	0.39	0.35	0.36	0.29	0.2	0.2;"
                                    "-3.09	-2.93	-2.91	-2.78	-2.7	-3.03	-2.9	-3.2	-3.2;"
                                    "0.32	0.47	0.46	0.54	0.45	0.36	0.42	0.3	0.3;";
                }
            }
            break;
        case _SCMNTN_RURAL_:
            if(isLOS)
            {
                if(!isKaBand)
                {
                    lspMuSigma =    "-9.55	-8.68	-8.46	-8.36	-8.29	-8.26	-8.22	-8.2	-8.19;"
                                    "0.66	0.44	0.28	0.19	0.14	0.1	0.1	0.05	0.06;"
                                    "-3.42	-3	-2.86	-2.78	-2.7	-2.66	-2.53	-2.21	-1.78;"
                                    "0.89	0.63	0.52	0.45	0.42	0.41	0.42	0.5	0.91;"
                                    "-9.45	-4.45	-2.39	-1.28	-0.99	-1.05	-0.9	-0.89	-0.81;"
                                    "7.83	6.86	5.14	3.44	2.59	2.42	1.78	1.65	1.26;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "1.79 1.14 1.14 0.92 1.42 1.56 0.85 0.72 0.72;"  //santhosh lsp
                                    "24.72	12.31	8.05	6.21	5.04	4.42	3.92	3.65	3.59;"
                                    "5.07	5.75	5.46	5.23	3.95	3.75	2.56	1.77	1.77;"
                                    "-4.2	-2.31	-0.28	-0.38	-0.38	-0.46	-0.49	-0.53	-0.46;"
                                    "6.3	5.04	0.81	1.16	0.82	0.67	1	1.18	0.91;"
                                    "-6.03	-4.31	-2.57	-2.59	-2.59	-2.65	-2.69	-2.65	-2.65;"
                                    "5.19	4.18	0.61	0.79	0.65	0.52	0.78	1.01	0.71;";
                }
                else
                {
                    lspMuSigma =    "-9.68	-8.86	-8.59	-8.46	-8.36	-8.3	-8.26	-8.22	-8.21;"
                                    "0.46	0.29	0.18	0.19	0.14	0.15	0.13	0.03	0.07;"
                                    "-4.03	-3.55	-3.45	-3.38	-3.33	-3.29	-3.24	-2.9	-2.5;"
                                    "0.91	0.7	0.55	0.52	0.46	0.43	0.46	0.44	0.82;"
                                    "-9.74	-4.88	-2.6	-1.92	-1.56	-1.66	-1.59	-1.58	-1.51;"
                                    "7.52	6.67	4.63	3.45	2.44	2.38	1.67	1.44	1.13;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "1.9 1.6 1.9 2.3 2.7 3.1 3.0 3.6 0.4;"
                                    "25.43	12.72	8.4	6.52	5.24	4.57	4.02	3.7	3.62;"
                                    "7.04	7.47	7.18	6.88	5.28	4.92	3.4	2.22	2.28;"
                                    "-5.85	-3.27	-0.88	-0.93	-0.99	-1.04	-1.17	-1.19	-1.13;"
                                    "6.51	5.36	0.93	0.96	0.97	0.83	1.01	1.01	0.85;"
                                    "-7.45	-5.25	-3.16	-3.15	-3.2	-3.27	-3.42	-3.36	-3.35;"
                                    "5.3	4.42	0.68	0.73	0.77	0.61	0.74	0.79	0.65;";

                }
            }
            else
            {
                if(!isKaBand)
                {
                    lspMuSigma =    "-9.01	-8.37	-8.05	-7.92	-7.92	-7.96	-7.91	-7.79	-7.74;"
                                    "1.59	0.95	0.92	0.92	0.87	0.87	0.82	0.86	0.81;"
                                    "-2.9	-2.5	-2.12	-1.99	-1.9	-1.85	-1.69	-1.46	-1.32;"
                                    "1.34	1.18	1.08	1.06	1.05	1.06	1.14	1.16	1.3;"
                                    "-3.33	-0.74	0.08	0.32	0.53	0.33	0.55	0.45	0.4;"
                                    "6.22	4.22	3.02	2.45	1.63	2.08	1.58	2.01	2.19;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "8.93 9.08 8.78 10.25 10.56 10.74 0.17 11.52 11.52;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "-0.88	-0.07	0.75	0.72	0.95	0.97	1.1	0.97	1.35;"
                                    "3.26	3.29	1.92	1.92	1.45	1.62	1.43	1.88	0.62;"
                                    "-4.92	-4.06	-2.33	-2.24	-2.24	-2.22	-2.19	-2.41	-2.45;"
                                    "3.96	4.07	1.7	2.01	2	1.82	1.66	2.58	2.52;";
                }
                else
                {
                    lspMuSigma =    "-9.13	-8.39	-8.1	-7.96	-7.99	-8.05	-8.01	-8.05	-7.91;"
                                    "1.91	0.94	0.92	0.94	0.89	0.87	0.82	1.65	0.76;"
                                    "-2.9	-2.53	-2.16	-2.04	-1.99	-1.95	-1.81	-1.56	-1.53;"
                                    "1.32	1.18	1.08	1.09	1.08	1.06	1.17	1.2	1.27;"
                                    "-3.4	-0.51	0.06	0.2	0.4	0.32	0.46	0.33	0.24;"
                                    "6.28	3.75	2.95	2.65	1.85	1.83	1.57	1.99	2.18;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "10.7 10.0 11.2 11.6 11.8 10.8 10.8 10.8 10.8;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                                    "-1.19	-0.11	0.72	0.69	0.84	0.99	0.95	0.92	1.29;"
                                    "3.81	3.33	1.93	1.91	1.7	1.27	1.86	1.84	0.59;"
                                    "-5.47	-4.06	-2.32	-2.19	-2.16	-2.24	-2.29	-2.65	-2.23;"
                                    "4.39	4.04	1.54	1.73	1.5	1.64	1.66	2.86	1.12;";
                }
            }
            break;
        default:
            cout<<"Invalid scmScenario for NTN..."<<endl;abort();
            break;
    }
    
    int index = round_i(elevationAngle/10)-1;
//     cout<<"elevationAngle : "<<elevationAngle<<" , "<<index<<endl;abort();
    
    return lspMuSigma.get_col(index);
}

vec getCorrelationDistancesForNTN(SCMPropagationScenario scmScenario,double elevationAngle,bool isLOS,bool isKaBand)
{
    mat corrDist;
    switch(scmScenario)
    {
        case _SCMNTN_DENSE_URBAN_:
            if(isLOS)
            {
                if(!isKaBand)
                {
                    corrDist =  "30	30	30	30	30	30	30	30	30;"
                                "18	18	18	18	18	18	18	18	18;"
                                "15	15	15	15	15	15	15	15	15;"
                                "37	37	37	37	37	37	37	37	37;"
                                "12	12	12	12	12	12	12	12	12;"
                                "15	15	15	15	15	15	15	15	15;"
                                "15	15	15	15	15	15	15	15	15;";
                }
                else
                {
                    corrDist =  "30	30	30	30	30	30	30	30	30;"
                                "18	18	18	18	18	18	18	18	18;"
                                "15	15	15	15	15	15	15	15	15;"
                                "37	37	37	37	37	37	37	37	37;"
                                "12	12	12	12	12	12	12	12	12;"
                                "15	15	15	15	15	15	15	15	15;"
                                "15	15	15	15	15	15	15	15	15;";
                }
            }
            else
            {
                if(!isKaBand)
                {
                    corrDist =  "40	40	40	40	40	40	40	40	40;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;"
                                "-1  -1  -1  -1  -1  -1  -1  -1  -1;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;";
                }
                else
                {
                    corrDist =  "40	40	40	40	40	40	40	40	40;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;"
                                "-1  -1  -1  -1  -1  -1  -1  -1  -1;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;";
                }
            }
            break;
        case _SCMNTN_URBAN_:
            if(isLOS)
            {
                if(!isKaBand)
                {
                     corrDist = "30 30	30	30	30	30	30	30	30;"
                                "18	18	18	18	18	18	18	18	18;"
                                "15	15	15	15	15	15	15	15	15;"
                                "37	37	37	37	37	37	37	37	37;"
                                "12	12	12	12	12	12	12	12	12;"
                                "15	15	15	15	15	15	15	15	15;"
                                "15	15	15	15	15	15	15	15	15;";
                }
                else
                {
                    corrDist =  "30 30	30	30	30	30	30	30	30;"
                                "18	18	18	18	18	18	18	18	18;"
                                "15	15	15	15	15	15	15	15	15;"
                                "37	37	37	37	37	37	37	37	37;"
                                "12	12	12	12	12	12	12	12	12;"
                                "15	15	15	15	15	15	15	15	15;"
                                "15	15	15	15	15	15	15	15	15;";
                }
            }
            else 
            {
                if(!isKaBand)
                {
                    corrDist =  "40	40	40	40	40	40	40	40	40;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;"
                                "-1  -1  -1  -1  -1  -1  -1  -1  -1;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;";
                }
                else
                {
                    corrDist =  "40	40	40	40	40	40	40	40	40;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;"
                                "-1  -1  -1  -1  -1  -1  -1  -1  -1;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;";
                }
            }
            break;
        case _SCMNTN_SUB_URBAN_:
            if(isLOS)
            {
                if(!isKaBand)
                {
                    corrDist = "30 30	30	30	30	30	30	30	30;"
                                "18	18	18	18	18	18	18	18	18;"
                                "15	15	15	15	15	15	15	15	15;"
                                "37	37	37	37	37	37	37	37	37;"
                                "12	12	12	12	12	12	12	12	12;"
                                "15	15	15	15	15	15	15	15	15;"
                                "15	15	15	15	15	15	15	15	15;";
                }
                else
                {
                    corrDist = "30 30	30	30	30	30	30	30	30;"
                                "18	18	18	18	18	18	18	18	18;"
                                "15	15	15	15	15	15	15	15	15;"
                                "37	37	37	37	37	37	37	37	37;"
                                "12	12	12	12	12	12	12	12	12;"
                                "15	15	15	15	15	15	15	15	15;"
                                "15	15	15	15	15	15	15	15	15;";
                }
            }
            else
            {
                if(!isKaBand)
                {
                    corrDist =  "40	40	40	40	40	40	40	40	40;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;"
                                "-1  -1  -1  -1  -1  -1  -1  -1  -1;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;";
                }
                else
                {
                    corrDist =  "40	40	40	40	40	40	40	40	40;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;"
                                "-1  -1  -1  -1  -1  -1  -1  -1  -1;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;";
                }
            }
            break;
        case _SCMNTN_RURAL_:
            if(isLOS)
            {
                if(!isKaBand)
                {
                    corrDist = "50	50	50	50	50	50	50	50	50;"
                                "25	25	25	25	25	25	25	25	25;"
                                "35	35	35	35	35	35	35	35	35;"
                                "37	37	37	37	37	37	37	37	37;"
                                "40	40	40	40	40	40	40	40	40;"
                                "15	15	15	15	15	15	15	15	15;"
                                "15	15	15	15	15	15	15	15	15;";
                }
                else
                {
                    corrDist = "50	50	50	50	50	50	50	50	50;"
                                "25	25	25	25	25	25	25	25	25;"
                                "35	35	35	35	35	35	35	35	35;"
                                "37	37	37	37	37	37	37	37	37;"
                                "40	40	40	40	40	40	40	40	40;"
                                "15	15	15	15	15	15	15	15	15;"
                                "15	15	15	15	15	15	15	15	15;";    //verfied
                }
            }
            else
            {
                if(!isKaBand)
                {
                    corrDist =  "36	36	36	36	36	36	36	36	36;"
                                "30	30	30	30	30	30	30	30	30;"
                                "40	40	40	40	40	40	40	40	40;"
                                "120 120 120 120 120 120	120	120	120;"
                                "-1  -1  -1  -1  -1  -1  -1  -1  -1;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;";
                }
                else
                {
                    corrDist =  "36	36	36	36	36	36	36	36	36;"
                                "30	30	30	30	30	30	30	30	30;"
                                "40	40	40	40	40	40	40	40	40;"
                                "120 120 120 120 120 120	120	120	120;"
                                "-1  -1  -1  -1  -1  -1  -1  -1  -1;"
                                "50	50	50	50	50	50	50	50	50;"
                                "50	50	50	50	50	50	50	50	50;";
                }
            }
            break;
        default:
            cout<<"Invalid scmScenario for NTN..."<<endl;abort();
            break;
    }
    int index = round_i(elevationAngle/10)-1;
    return corrDist.get_col(index);
}

double gaussian(double mu, double var)
{
    return mu + sqrt(var)*randn();
}

complex<double> complexGaussian(double mu, double var)
{
  return mu + sqrt(var)*randn_c();
};

cvec complexGaussian(double mu, double var,int lengthOfVector)
{
  return mu + sqrt(var)*randn_c(lengthOfVector);
};

SCMPropagationScenario loadSCMScenario(string fileName,bool isFileName){
  
  SCMPropagationScenario scmScenario;
  string sysTerr = fileName;
  if(isFileName)
  if(!parse(fileName,"sysTerrain",sysTerr)){cout<<"[both:] error in parsing sysTerr aborting ...."<<endl; abort();}
  if(sysTerr!="")
  {
    if(sysTerr=="_TERRAIN_IMT_URBAN_MACRO_" 
      || sysTerr=="_TERRAIN_IMT_SUBURBAN_MACRO_" 
      || sysTerr=="_TERRAIN_WINNER_URBAN_MACRO_"
      || sysTerr=="_TERRAIN_WINNER_PLUS_URBAN_MACRO_"
      || sysTerr=="_TERRAIN_WINNER_PLUS_SUBURBAN_MACRO_"
      || sysTerr=="_TERRAIN_HATA_URBAN_"
      || sysTerr=="_TERRAIN_3GPP_CASE1_"
      || sysTerr=="_TERRAIN_3GPP_CASE3_"
      || sysTerr=="_TERRAIN_HATA_SUBURBAN_")
      {
	if(currentTaskid==0)
	  cout<<"[detl:]Loading _SCM2D_URBAN_MACRO_ as scmScenario."<<endl;
	scmScenario=_SCM2D_URBAN_MACRO_;
      }
    else if(sysTerr=="_TERRAIN_IMT_URBAN_MICRO_" 
      || sysTerr=="_TERRAIN_WINNER_URBAN_MICRO_" 
      || sysTerr=="_TERRAIN_WINNER_PLUS_URBAN_MICRO_")
      {
	if(currentTaskid==0)
	  cout<<"[detl:]Loading _SCM2D_URBAN_MICRO_ as scmScenario."<<endl;
	scmScenario=_SCM2D_URBAN_MICRO_;
      }
    else if(sysTerr=="_TERRAIN_IMT_RURAL_MACRO_"
      || sysTerr=="_TERRAIN_HATA_OPEN_")
      {
	if(currentTaskid==0)
	  cout<<"[detl:]Loading _SCM2D_RURAL_MACRO_ as scmScenario."<<endl;
	scmScenario=_SCM2D_RURAL_MACRO_;
      }
      else if(sysTerr=="_TERRAIN_SCM3D_URBAN_MICRO_")//3GPP TR 36.873 V2.0.0 (2014-03)
      {
	if(currentTaskid==0)
	  cout<<"[detl:]Loading _SCM3D_URBAN_MICRO_ as scmScenario."<<endl;
	scmScenario=_SCM3D_URBAN_MICRO_;
      }
      else if(sysTerr=="_TERRAIN_SCM3D_URBAN_MACRO_")//3GPP TR 36.873 V2.0.0 (2014-03)
      {
	if(currentTaskid==0)
	  cout<<"[detl:]Loading _SCM3D_URBAN_MACRO_ as scmScenario."<<endl;
	scmScenario=_SCM3D_URBAN_MACRO_;
      }
      else if(sysTerr=="_TERRAIN_5G_URBAN_MACRO_" || sysTerr=="_TERRAIN_5G_URBAN_MACRO_LOS_" || sysTerr=="_TERRAIN_5G_URBAN_MACRO_NLOS_")//3GPP TR 38.900
      {
	if(currentTaskid==0)
	  cout<<"[detl:]Loading _SCM5G_URBAN_MACRO_ as scmScenario."<<endl;
	scmScenario=_SCM5G_URBAN_MACRO_;
      }
      else if(sysTerr=="_TERRAIN_5G_URBAN_MICRO_" || sysTerr=="_TERRAIN_5G_URBAN_MICRO_LOS_" || sysTerr=="_TERRAIN_5G_URBAN_MICRO_NLOS_")//3GPP TR 38.900
      {
	if(currentTaskid==0)
	  cout<<"[detl:]Loading _SCM5G_URBAN_MICRO_ as scmScenario."<<endl;
	scmScenario=_SCM5G_URBAN_MICRO_;
      }
      else if(sysTerr=="_TERRAIN_5G_RURAL_MACRO_" || sysTerr=="_TERRAIN_5G_RURAL_MACRO_LOS_" || sysTerr=="_TERRAIN_5G_RURAL_MACRO_NLOS_")//3GPP TR 38.900
      {
	if(currentTaskid==0)
	  cout<<"[detl:]Loading _SCM5G_RURAL_MACRO_ as scmScenario."<<endl;
	scmScenario=_SCM5G_RURAL_MACRO_;
      }
      else if(sysTerr=="_TERRAIN_5G_INDOOR_" || sysTerr=="_TERRAIN_5G_INDOOR_LOS_" || sysTerr=="_TERRAIN_5G_INDOOR_NLOS_")//3GPP TR 38.900
      {
	if(currentTaskid==0)
	  cout<<"[detl:]Loading _SCM5G_INDOOR_ as scmScenario."<<endl;
	scmScenario=_SCM5G_INDOOR_OFFICE_;
      }
      else if(sysTerr=="_TERRAIN_5G_INDOOR_FACTORY_" || sysTerr=="_TERRAIN_5G_INDOOR_FACTORY_LOS_" || sysTerr=="_TERRAIN_5G_INDOOR_FACTORY_NLOS_")//3GPP TR 38.900
      {
          if(currentTaskid==0)
              cout<<"[detl:]Loading _SCM5G_INDOOR_FACTORY_ as scmScenario."<<endl;
          scmScenario=_SCM5G_INDOOR_FACTORY_;
      }
      else if(sysTerr=="_TERRAIN_NTN_DENSE_URBAN_" || sysTerr=="_TERRAIN_NTN_DENSE_URBAN_LOS_" || sysTerr=="_TERRAIN_NTN_DENSE_URBAN_NLOS_")
      {
          if(currentTaskid==0)
              cout<<"[detl:]Loading _SCMNTN_DENSE_URBAN_ as scmScenario."<<endl;
        scmScenario = _SCMNTN_DENSE_URBAN_;
     }
    else if(sysTerr=="_TERRAIN_NTN_URBAN_" || sysTerr=="_TERRAIN_NTN_URBAN_LOS_" || sysTerr=="_TERRAIN_NTN_URBAN_NLOS_")
    {
        if(currentTaskid==0)
              cout<<"[detl:]Loading _SCMNTN_URBAN_ as scmScenario."<<endl;
        scmScenario = _SCMNTN_URBAN_;
    }
    else if(sysTerr=="_TERRAIN_NTN_SUB_URBAN_" || sysTerr=="_TERRAIN_NTN_SUB_URBAN_LOS_" || sysTerr=="_TERRAIN_NTN_SUB_URBAN_NLOS_")
    {
        if(currentTaskid==0)
              cout<<"[detl:]Loading _SCMNTN_SUB_URBAN_ as scmScenario."<<endl;
        scmScenario = _SCMNTN_SUB_URBAN_;
    }
    else if(sysTerr=="_TERRAIN_NTN_RURAL_" || sysTerr=="_TERRAIN_NTN_RURAL_LOS_" || sysTerr=="_TERRAIN_NTN_RURAL_NLOS_")
    {
        if(currentTaskid==0)
              cout<<"[detl:]Loading _SCMNTN_RURAL_ as scmScenario."<<endl;
        scmScenario = _SCMNTN_RURAL_;
    }
    else
    {
      cout<<"[both:] Unsupported Terrain in config file for loadSCMScenario()... loading UMa as default"<<endl;
      abort();
    }
  }
  else
  {
    cout<<"[both:] Unsupported Terrain in config file for loadSCMScenario()..."<<endl;
    abort();
  }
  return(scmScenario);
}

Array<NodeCategory_E> parseNodeCategory(string configFile,string variableName){
  
  Array<string> nodeCategory; 
  Array<NodeCategory_E> nodeCategoryEnum(0);
  if(parse(configFile,variableName,nodeCategory))
  {
    nodeCategoryEnum.set_length(nodeCategory.length());
    for(int string_cnt=0;string_cnt<nodeCategory.length();string_cnt++)
      if((nodeCategory(string_cnt))=="_NODECATEGORY_SERVERNODE_")
	nodeCategoryEnum(string_cnt)=_NODECATEGORY_SERVERNODE_;
      else if((nodeCategory(string_cnt))=="_NODECATEGORY_SERVICENODE_") nodeCategoryEnum(string_cnt)=_NODECATEGORY_SERVICENODE_;
      else {cout<<"[both:] Unknown node category in the config File for parseNodeCategory()...";abort();}
  }
  else
  {
    cout<<"Unable to parse Node Category from the config file ..."<<endl;
  }
  return(nodeCategoryEnum);
}

FadingChannelModel_E parseChannelModel(string chnModel){
  
  FadingChannelModel_E channelModel;
  if(chnModel!="")
  {
    if(chnModel=="_FADING_CHANNEL_MODEL_SUI_3_")
      channelModel=_FADING_CHANNEL_MODEL_SUI_3_;
#ifdef USING_CALIBRATION 
    else if(chnModel=="_FADING_CHANNEL_MODEL_2_TAP_")
      channelModel=_FADING_CHANNEL_MODEL_2_TAP_;
#endif
    else if(chnModel=="_FADING_CHANNEL_MODEL_PED_A_")
      channelModel=_FADING_CHANNEL_MODEL_PED_A_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_PED_B_")
      channelModel=_FADING_CHANNEL_MODEL_PED_B_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_VEH_A_")
      channelModel=_FADING_CHANNEL_MODEL_VEH_A_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_MODIFIED_PED_B_")
      channelModel=_FADING_CHANNEL_MODEL_MODIFIED_PED_B_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_MODIFIED_VEH_A_")
      channelModel=_FADING_CHANNEL_MODEL_MODIFIED_VEH_A_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_EVA_")
      channelModel=_FADING_CHANNEL_MODEL_EVA_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_EPA_")
      channelModel=_FADING_CHANNEL_MODEL_EPA_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_ETU_")
      channelModel=_FADING_CHANNEL_MODEL_ETU_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_URBAN_MACRO_")
      channelModel=_FADING_CHANNEL_MODEL_URBAN_MACRO_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_TDL_A_")
      channelModel=_FADING_CHANNEL_MODEL_TDL_A_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_TDL_B_")
      channelModel=_FADING_CHANNEL_MODEL_TDL_B_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_TDL_C_")
      channelModel=_FADING_CHANNEL_MODEL_TDL_C_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_TDL_D_")
      channelModel=_FADING_CHANNEL_MODEL_TDL_D_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_TDL_E_")
      channelModel=_FADING_CHANNEL_MODEL_TDL_E_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_CDL_A_")
      channelModel=_FADING_CHANNEL_MODEL_CDL_A_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_CDL_B_")
      channelModel=_FADING_CHANNEL_MODEL_CDL_B_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_CDL_C_")
      channelModel=_FADING_CHANNEL_MODEL_CDL_C_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_CDL_D_")
      channelModel=_FADING_CHANNEL_MODEL_CDL_D_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_CDL_E_")
      channelModel=_FADING_CHANNEL_MODEL_CDL_E_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_FLAT_FADING_")
      channelModel=_FADING_CHANNEL_MODEL_FLAT_FADING_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_CUSTOM_")
      channelModel=_FADING_CHANNEL_MODEL_CUSTOM_;
    else if(chnModel=="_FADING_CHANNEL_MODEL_AWGN_")
        channelModel=_FADING_CHANNEL_MODEL_AWGN_;
    else
    {
      cout<<"[both:] Unknown Channel model to parseChannelModel()..."<<endl;
      abort();
    }
  }
  else
  {
      cout<<"[both:] Unknown Channel model to parseChannelModel()...empty string ...."<<endl;
      abort();
  }
  return(channelModel);
}

int findClosestNeighbourCellIndx(Array<Neighbour_S> &neighbourInfo, Location_S referenceLocation){
  
  vec distance(neighbourInfo.length());
  for(int cnt=0;cnt<neighbourInfo.length();cnt++)
    distance(cnt)=findDistance(neighbourInfo(cnt).cellXYLocation,referenceLocation);
  
  return(min_index(distance));
}

Array<Location_S> getRotatedLocations(Array<Location_S> &locations, double rotationInDegree){
  
  Array<Location_S> output(locations.length());
  double rotationInRad=deg_to_rad(rotationInDegree);
  for(int cnt=0;cnt<locations.length();cnt++)
  {
    output(cnt)=locations(cnt);
    output(cnt).x=locations(cnt).x+cos(rotationInRad);
    output(cnt).y=locations(cnt).y+sin(rotationInRad);
  }
  return(output);  
}

Array<Location_S> getShiftedLocations(Array<Location_S> &locations, Location_S xyShift){
  
  Array<Location_S> output(locations.length());
  for(int cnt=0;cnt<locations.length();cnt++)
  {
    output(cnt)=locations(cnt);
    output(cnt).x=locations(cnt).x+xyShift.x;
    output(cnt).y=locations(cnt).y+xyShift.y;
  }
  return(output);  
}


void BuildingInfo_S::init(string configfile, bool createbuildings){
  
  
  parse(configfile,"roomLength",roomSize.length);
  parse(configfile,"roomWidth",roomSize.width);
  parse(configfile,"roomHeight",roomSize.height);
  parse(configfile,"indoorWallSpacing",indoorWallSpacing);
  parse(configfile,"maximumFloors",maximumFloors);
  parse(configfile,"ceilingLossInDB",ceilingLossInDB);
  parse(configfile,"thickWallLossInDB",thickWallLossInDB);
  parse(configfile,"thinWallLossInDB",thinWallLossInDB);

  isBuildingsAvailable=createbuildings;
  
  if(createbuildings)
  {
    
    cout<<"[both:]Initializing Floor Plan for building model."<<endl;
    
    parse(configfile,"nBuildings",nBuildings);
    if(nBuildings==0) 
    {	
      isBuildingsAvailable=false;
      return;
    }
    
    parse(configfile,"isBuildingsPlanned",isBuildingsPlanned);
    parse(configfile,"isPlannedBuildingLocationsforASector",isPlannedBuildingLocationsforASector);
    parse(configfile,"isFixedFloors",isFixedFloors);
    parse(configfile,"enableWallCountForPathloss",enableWallCountForPathloss);
    
    
    vec locations;
    
    parse(configfile,"plannedBuildingLocations",locations);
    if(isBuildingsPlanned)
    {
      if(locations.length()/2!=nBuildings) {cout<<"[both:] Insufficient building location values."<<endl;abort();}
      
      plannedLocations.set_length(locations.length()/2); 
      for(int cnt=0;cnt<plannedLocations.length();cnt++)
      {
	plannedLocations(cnt).x=locations(2*cnt);
	plannedLocations(cnt).y=locations(2*cnt+1);
      }
    }
    
    
    mat room2DModel=zeros(roomSize.length+1,roomSize.length+1);		   
    //Creating thin walls
    int nSmallWalls=floor_i((double)(roomSize.length-2)/(double)indoorWallSpacing); //2 included to remove the roomwalls from consideration 
    for(int smallwall_cnt=0,row=indoorWallSpacing;smallwall_cnt<nSmallWalls;smallwall_cnt++,row+=indoorWallSpacing)
      room2DModel.set_row(row,ones(roomSize.length+1)*thinWallLossInDB);
      
    //Creating room boundary		    
    room2DModel.set_col(0,ones(roomSize.length+1)*thickWallLossInDB);		    
    room2DModel.set_col(roomSize.length,ones(roomSize.length+1)*thickWallLossInDB);		    
    room2DModel.set_row(0,ones(roomSize.length+1)*thickWallLossInDB);		    
    room2DModel.set_row(roomSize.length,ones(roomSize.length+1)*thickWallLossInDB);		    
    
    cout<<"[detl:]Creating Room 2D-plan"<<endl;
    
    imat floorPlan;
    if(!parse(configfile,"floorPlan",floorPlan))
    { cout<<"[both:] Unable to parse floorPlan from the configfile. Default value not possible."<<endl;abort();    }
    
    ivec hallwayRows=findNot(floorPlan.get_col(0),0);
    
    for(int col_cnt=1;col_cnt<floorPlan.cols();col_cnt++)
      if(hallwayRows!=findNot(floorPlan.get_col(col_cnt),0))
      {cout<<"[both:] [det1]Unsupported Floorplan format in the configfile."<<endl;abort();}
      
    imat fpCopy=floorPlan;
    ivec zeroLoc=find(floorPlan,0,true);
    setInMatrix(fpCopy,zeroLoc,ones_i(zeroLoc.length())*roomSize.length);
    int buildingCols=max(sum(fpCopy,2));buildingCols++; 
    int buildingRows=max(sum(fpCopy,1));buildingRows++;
    
    buildingRows+=hallwayRows.length(); 
    
    mat floor2DModel=zeros(buildingRows,buildingCols);
    int buildingRowIndx=0;
    for(int row_cnt=0;row_cnt<floorPlan.rows();row_cnt++)
    {
      if(find(hallwayRows,row_cnt)!=-1) // It's a hallway
      {
	buildingRowIndx++;
	floor2DModel.set_submatrix(buildingRowIndx,0,zeros(floorPlan(row_cnt,0),buildingCols));
	buildingRowIndx+=floorPlan(row_cnt,0);
      }
      else
      {
	for(int room_cnt=0;room_cnt<floorPlan.cols();room_cnt++)
	if(floorPlan(row_cnt,room_cnt)==0) // Room
	  floor2DModel.set_submatrix(buildingRowIndx,room_cnt*roomSize.length,room2DModel);
	else if(floorPlan(row_cnt,room_cnt)==-1)
	{
	  cout<<"Empty rooms modelling not ready in building model."<<endl;
	}
	else
	{
	  cout<<"[both:]Invalid value in floorPlan."<<endl;abort();
	}
	buildingRowIndx+=roomSize.length;
      }
    }  
    
    //First row is hallway.. So adding a wall for it
    mat wallMat(0,0);
    if(find(hallwayRows,0)!=-1)
    {
      wallMat=ones(1,floor2DModel.cols())*thickWallLossInDB;
      floor2DModel=concat_vertical(wallMat,floor2DModel);
    }
    //Last row is hallway.. So adding a wall for it
    if(find(hallwayRows,floorPlan.rows()-1)!=-1)
    {
      wallMat=ones(1,floor2DModel.cols())*thickWallLossInDB;
      floor2DModel=concat_vertical(floor2DModel,wallMat);
    }
    
    cout<<"[detl:]Created buildings of dimension : "<<floor2DModel.rows()<<"x"<<floor2DModel.cols()<<" meters."<<endl;
    buildingArea.xLen=floor2DModel.cols();
    buildingArea.yLen=floor2DModel.rows();
    
    floor3DModel.set_length(roomSize.height*maximumFloors);
    for(int cnt=0;cnt<floor3DModel.length();cnt++)
      if(cnt%roomSize.height==(roomSize.height-1)) //Ceiling
	floor3DModel=ones(floor2DModel.rows(),floor2DModel.cols())*ceilingLossInDB;
      else
	floor3DModel=floor2DModel;
  }
}

ChannelGenerationMethod_E parseChannelGenerationMethod(string fileName){
  
  ChannelGenerationMethod_E channelGenerationMethod;
  string channelGenMethod="";
  if(!parse(fileName,"channelGenerationMethod",channelGenMethod)){cout<<"[both:]error in parsing channelGenerationMethod exiting...."<<endl;abort();}
  if(channelGenMethod!="")
  {
    if(channelGenMethod=="_CHANNEL_GENERATION_METHOD_JAKES_")
      channelGenerationMethod=_CHANNEL_GENERATION_METHOD_JAKES_;
    else if(channelGenMethod=="_CHANNEL_GENERATION_METHOD_MOD_JAKES_")
      channelGenerationMethod=_CHANNEL_GENERATION_METHOD_MOD_JAKES_;
    else if(channelGenMethod=="_CHANNEL_GENERATION_METHOD_SMITH_")
      channelGenerationMethod=_CHANNEL_GENERATION_METHOD_SMITH_;
    else if(channelGenMethod=="_CHANNEL_GENERATION_METHOD_SCM_")
      channelGenerationMethod=_CHANNEL_GENERATION_METHOD_SCM_;
    else if(channelGenMethod=="_CHANNEL_GENERATION_METHOD_CDL_")
      channelGenerationMethod=_CHANNEL_GENERATION_METHOD_CDL_;
    else
    {
      cout<<"[both:]Error. Unknown Channel model to load from config file."<<endl;
      exit(0);
    }
  }
  else
  {
    cout<<"[both:]Error. Unknown Channel model to load from config file."<<endl;
    exit(0);
  }
  return(channelGenerationMethod);
}

ChannelModelType parseChannelModelType(string fileName){
  
  ChannelModelType channelModelType;
  string channelType="";
  parse(fileName,"channelModelType",channelType);
    
  if(channelType!="")
  {
    if(channelType=="_MODEL_A_")
      channelModelType=_MODEL_A_;
    else if(channelType=="_MODEL_B_")
      channelModelType=_MODEL_B_;
    else
    {
      channelModelType=_MODEL_UNINIT_;
    }
  }
  else
  {
    channelModelType=_MODEL_UNINIT_;
  }

  return(channelModelType);
}

void NodeInfo_S::initBeamParameters(int nodeID, double carrierFrequencyInHz, ivec antStructure, ivec txruMap, vec antSpacing, Array<vec> azimuthAngles, Array<vec> zenithAngles, OrientationInfo_S orientationInfo,bool nodetype ,bool reInitialize){
  
  int nodeIndx = find(nodeList,nodeID);
  
  if(nodeIndx != -1)
  {
    if(reInitialize || !beamParametersPerNode(nodeIndx).isInitialized)
    {
      //if(currentTaskid==0) cout<<"Setting beam parameter for node : "<<nodeID<<endl;
//       cout<<"[DEBUG] Setting beam parameter for node : "<<nodeID<<endl;
//       cout<<"[DEBUG] AntStructure: "<<antStructure<<endl;
      beamParametersPerNode(nodeIndx).configureBeamParameters(antStructure,txruMap,antSpacing,azimuthAngles,zenithAngles,carrierFrequencyInHz,orientationInfo);
//       cout<<"[DEBUG] Configured beam parameter for node : "<<nodeID<<endl;
      
      Array<ivec> txRUsPerBeam = beamParametersPerNode(nodeIndx).txRUsPerBeam;
      
      //cout<<"txRUsPerBeam : "<<txRUsPerBeam<<endl;
      
      Array<ivec> possibleBeamsPerTxRU(beamParametersPerNode(nodeIndx).TXRUparameters.numberOfTXRU);
      for(int txru_cnt=0;txru_cnt<beamParametersPerNode(nodeIndx).TXRUparameters.numberOfTXRU;txru_cnt++)
          possibleBeamsPerTxRU(txru_cnt).set_length(0);
      
      for(int beam_cnt=0;beam_cnt<txRUsPerBeam.length();beam_cnt++)
          for(int txru_cnt=0;txru_cnt<txRUsPerBeam(beam_cnt).length();txru_cnt++)
              append(possibleBeamsPerTxRU(txRUsPerBeam(beam_cnt)(txru_cnt)),beam_cnt);
      
          
      // Setting lowest beam as default beam for each TxRU    
      beamIDPerTxRU(nodeIndx).set_length(beamParametersPerNode(nodeIndx).TXRUparameters.numberOfTXRU);
      for(int txru_cnt=0;txru_cnt<beamParametersPerNode(nodeIndx).TXRUparameters.numberOfTXRU;txru_cnt++)
          beamIDPerTxRU(nodeIndx)(txru_cnt) = min(possibleBeamsPerTxRU(txru_cnt));
          
    }
  }
  else
  {
    cout<<"nodeID " << nodeID <<" is not found in nodeList "<<nodeList<<endl;abort();
  }
}

int NodeInfo_S::getNumberOfTxRU(int nodeID){
  
  int nTXRUs = -1;
  int nodeIndx = find(nodeList,nodeID);
  
  if(nodeIndx != -1)
  {
    if(beamParametersPerNode(nodeIndx).isInitialized)
      nTXRUs = beamParametersPerNode(nodeIndx).TXRUparameters.numberOfTXRU;
    else
      nTXRUs = antennaCount(nodeIndx);
  }
  else
  {
    cout<<"nodeID " << nodeID <<" is not found in nodeList "<<nodeList<<endl;abort();
  }
  return nTXRUs;
}

int NodeInfo_S::getNumberOfElementsPerTxRU(int nodeID){
  
  int numberOfElementsPerTXRU = -1;
  int nodeIndx = find(nodeList,nodeID);
  
  if(nodeIndx != -1)
  {
    if(beamParametersPerNode(nodeIndx).isInitialized)
      numberOfElementsPerTXRU = beamParametersPerNode(nodeIndx).TXRUparameters.numberOfElementsPerTXRU;
    else
      numberOfElementsPerTXRU = 1;
  }
  else
  {
    cout<<"nodeID " << nodeID <<" is not found in nodeList "<<nodeList<<endl;abort();
  }
  return numberOfElementsPerTXRU;
}

ivec NodeInfo_S::getTXRUAntennaElements(int nodeID, int TXRUindx){
  
  int nodeIndx = find(nodeList,nodeID);
  ivec toReturn(0);
  
  if(nodeIndx != -1)
  {
    if(beamParametersPerNode(nodeIndx).isInitialized)
      toReturn = beamParametersPerNode(nodeIndx).TXRUparameters.getTXRUAntennaElements(TXRUindx);
    else
    {
      cout<<"Beam and TXRU parameters are not initialized"<<endl;abort();
    }
  }
  else
  {
    cout<<"nodeID " << nodeID <<" is not found in nodeList "<<nodeList<<endl;abort();
  }
  return toReturn;
}

ivec NodeInfo_S::getOrientationPerPanel(int nodeID)
{
    int nodeIndx = find(nodeList,nodeID);
    ivec toReturn(0);
    
    if(nodeIndx != -1)
    {
        if(beamParametersPerNode(nodeIndx).isInitialized)
            toReturn = beamParametersPerNode(nodeIndx).TXRUparameters.orientationIDPerPanel;
        else
        {
            cout<<"Beam and TXRU parameters are not initialized"<<endl;abort();
        }
    }
    else
    {
        cout<<"nodeID " << nodeID <<" is not found in nodeList "<<nodeList<<endl;abort();
    }
    return toReturn;
}
int NodeInfo_S::getOrientationID(int nodeID,int panelID)
{
    int nodeIndx = find(nodeList,nodeID);
    if(nodeIndx != -1)
    {
        if(beamParametersPerNode(nodeIndx).isInitialized)
            return beamParametersPerNode(nodeIndx).TXRUparameters.orientationIDPerPanel(panelID);
        else
        {
            cout<<"Beam and TXRU parameters are not initialized"<<endl;abort();
        }
    }
    else
    {
        cout<<"nodeID " << nodeID <<" is not found in nodeList "<<nodeList<<endl;abort();
    }
}
int NodeInfo_S::getNumberOfOrientations(int nodeID)
{
    int nodeIndx = find(nodeList,nodeID);
    if(nodeIndx != -1)
    {
        if(beamParametersPerNode(nodeIndx).isInitialized)
            return beamParametersPerNode(nodeIndx).TXRUparameters.nOrientations;
        else
        {
            cout<<"Beam and TXRU parameters are not initialized"<<endl;abort();
        }
    }
    else
    {
        cout<<"nodeID " << nodeID <<" is not found in nodeList "<<nodeList<<endl;abort();
    }
}


int NodeInfo_S::getTXRUCount(int nodeID, int beamID){
  
  int txruCount=0;
  int nodeIndx=find(nodeList,nodeID);
  if(beamParametersPerNode(nodeIndx).isInitialized)
  {
      txruCount = count(beamIDPerTxRU(nodeIndx),beamID);
  }
  else
    txruCount = antennaCount(nodeIndx);
  
  return txruCount;
}

ivec NodeInfo_S::getTXRUCountForAllBeams(int nodeID){
  
  int nodeIndx=find(nodeList,nodeID);
  
  if(nodeIndx == -1)
  {
    cout<<"nodeID " << nodeID <<" is not found in nodeList "<<nodeList<<endl;abort();
  }
  
  ivec TXRUCounts(1);
  
  if(beamParametersPerNode(nodeIndx).isInitialized)
  {
    TXRUCounts.set_length(beamParametersPerNode(nodeIndx).nTotalBeams);
    for(int beam_cnt=0;beam_cnt<TXRUCounts.length();beam_cnt++)
    {
        TXRUCounts(beam_cnt)=count(beamIDPerTxRU(nodeIndx),beam_cnt);
    }
  }
  else
  {
    TXRUCounts(0) = antennaCount(nodeIndx);
  }
  
  return TXRUCounts;
}

int NodeInfo_S::getNumberOfBeams(int nodeID){
  
  int nBeams = 1;
  int nodeIndx=find(nodeList,nodeID);
  
  if(beamParametersPerNode(nodeIndx).isInitialized)
    nBeams = beamParametersPerNode(nodeIndx).nTotalBeams;

  return nBeams;
  
}

int NodeInfo_S::getNumberOfPanels(int nodeID){
    
    int nPanels = 1;
    int nodeIndx=find(nodeList,nodeID);
    
    if(beamParametersPerNode(nodeIndx).isInitialized)
        nPanels = beamParametersPerNode(nodeIndx).TXRUparameters.nPanels;
    
    return nPanels;
    
}


ivec NodeInfo_S::getTXRUs(int nodeID, int beamID){
  
  ivec TXRUs;
  int nodeIndx=find(nodeList,nodeID);
  if(beamParametersPerNode(nodeIndx).isInitialized)
  {
      TXRUs = find(beamIDPerTxRU(nodeIndx),beamID,true);
  }
  else
    TXRUs = getIntegers(0,antennaCount(nodeIndx)-1);
  
  return TXRUs;
}

void NodeInfo_S::resetBeamParameters(ivec nodeIDs)
{
    ivec nodeIndices = find(nodeList,nodeIDs);
    for(int node_cnt=0;node_cnt<nodeIDs.length();node_cnt++)
    {
        beamParametersPerNode(nodeIndices(node_cnt)) = sBeamParameters();
    }
}

string getString(SCMLinkCondition linkCondition)
{
    string toReturn;
    if(linkCondition<0 || linkCondition>2)
    {
        cout<<"Undefined Link Condition..."<<endl;return "";
    }
    toReturn = (linkCondition==0) ? "_LOS_" : (linkCondition==1) ? "_NLOS_" : "_O2I_";
    return toReturn;
}

bool is2DSCMScenario(SCMPropagationScenario scmScenario)
{
    if(scmScenario == _SCM2D_URBAN_MICRO_ || scmScenario == _SCM2D_URBAN_MACRO_ || scmScenario == _SCM2D_RURAL_MACRO_ || scmScenario == _SCM2D_INDOOR_HOTSPOT_)
        return true;
    return false;
}

bool is3DSCMScenario(SCMPropagationScenario scmScenario)
{
    if(scmScenario == _SCM3D_URBAN_MICRO_ || scmScenario == _SCM3D_URBAN_MACRO_)
        return true;
    return false;
}

bool is5GSCMScenario(SCMPropagationScenario scmScenario)
{
    if(scmScenario == _SCM5G_URBAN_MICRO_ || scmScenario == _SCM5G_URBAN_MACRO_ || scmScenario == _SCM5G_RURAL_MACRO_ || scmScenario == _SCM5G_INDOOR_OFFICE_ || scmScenario == _SCM5G_INDOOR_FACTORY_)
        return true;
    return false;
}
bool isNTNSCMScenario(SCMPropagationScenario scmScenario)
{
    if(scmScenario == _SCMNTN_DENSE_URBAN_ || scmScenario == _SCMNTN_URBAN_ || scmScenario == _SCMNTN_SUB_URBAN_ || scmScenario == _SCMNTN_RURAL_)
        return true;
    return false;
}

int get5GSCMGridIndx(SCMPropagationScenario scmScenario,SCMLinkCondition linkCondition)
{
  // scmGridIndx    LOS  NLOS  O2I
  //                 0     1    2       UMi
  //                 3     4    5       UMa
  //                 6     7    8       RMa
  //                 9     10   -       InH
  //                 11    12   -       InF
    if(is5GSCMScenario(scmScenario))
    {
        int indx = (linkCondition==_LOS_) ? 0 : (linkCondition==_NLOS_) ? 1 : (linkCondition == _O2I_) ? 2 : -20;
        switch(scmScenario)
        {
            case _SCM5G_URBAN_MICRO_: return indx;
            case _SCM5G_URBAN_MACRO_: return 3+indx;
            case _SCM5G_RURAL_MACRO_: return 6+indx;
            case _SCM5G_INDOOR_OFFICE_ : return 9+indx;
            case _SCM5G_INDOOR_FACTORY_ : return 11+indx;
            default: return -1;
        }
    }
    else if(isNTNSCMScenario(scmScenario)) //Deepitha D
    {
      int indx = (linkCondition==_LOS_) ? 0 : (linkCondition==_NLOS_) ? 1 : (linkCondition == _O2I_) ? 2 : -20;
        switch(scmScenario)
        {
            case _SCMNTN_DENSE_URBAN_:return indx;
            case _SCMNTN_URBAN_ :return 3+indx;
            case _SCMNTN_RURAL_: return 6+indx;
            case _SCMNTN_SUB_URBAN_: return 9+indx;

            default: return -1;
        }
    }
    else
    {
        cout<<"Undefined 5G SCM Propagation Scenario..."<<endl;abort();
    }
}

double getElevationAngle(double bsHeight, double d)
{
    double Re = (double) earthRadius;
    double h0 = bsHeight - Re;
    
    double cosA = double((sqr(d)+sqr(Re)-sqr(h0+Re)) / (2*d*Re)); // A = 90+alpha
    
    if(cosA<-1 && cosA>-1.000001) cosA = -1;
    else if(cosA>1 && cosA<1.000001) cosA = 1;
    
    double alpha = rad_to_deg(acos(cosA))-90;
    if(alpha<5)
    {
        alpha = 0;
    }
    return alpha;
}
double getElevationAngle(Location_S bsLoc,Location_S ueLoc)
{
    mat ueVec = ueLoc.getAsMatrix(3);
    mat bsUeVec = bsLoc.getAsMatrix(3) - ueLoc.getAsMatrix(3);
    double alpha = rad_to_deg(getAngle(-ueVec,bsUeVec))-90;
    if(alpha<5)
    {
        alpha = 0;
    }
    //cout << "alpha elevation angle : " <<  alpha<<endl;
    return alpha;
}
bool isKaBand(double carrierFrequencyInHz)
{
    #ifdef _USING_CALIBRATION_
    if(carrierFrequencyInHz>10e9)
    #else
    if(carrierFrequencyInHz>=26.5e9 && carrierFrequencyInHz<=40e9)
    #endif
        return true;
    return false;
}

Array<ivec> getNodeIndicesForFrequencyReuse(int reuseFactor,int nCells)
{
    Array<ivec> nodeIndices(reuseFactor);
    if(reuseFactor==1)
    {
        nodeIndices(0) = getIntegers(0,nCells-1);
    }
    else if(reuseFactor==3)
    {
        switch(nCells)
        {
            case 7:
                nodeIndices(0) = "2 4 6"; 
                nodeIndices(1) = "0";
                nodeIndices(2) = "1 3 5";
                break;
            case 19:
                nodeIndices(0) = "2 4 6 7 11 15";
                nodeIndices(1) = "0 8 10 12 14 16 18";
                nodeIndices(2) = "1 3 5 9 13 17";
                break;
            case 37:
                nodeIndices(0) = "2 4 6 7 11 15 21 23 27 29 33 35";  //R    
                nodeIndices(1) = "0 8 10 12 14 16 18 19 22 25 28 31 34";    //G 
                nodeIndices(2) = "1 3 5 9 13 17 20 24 26 30 32 36";     //B
                break;
            case 61:
                nodeIndices(0) = "2 4 6 7 11 15 21 23 27 29 33 35 38 41 44 46 49 52 54 57 60";
                nodeIndices(1) = "0 8 10 12 14 16 18 19 22 25 28 31 34 39 43 47 51 55 59";
                nodeIndices(2) = "1 3 5 9 13 17 20 24 26 30 32 36 37 40 42 45 48 50 53 56 58";
                break;
            case 91:
                nodeIndices(0) = "2 4 6 7 11 15 21 23 27 29 33 35 38 41 44 46 49 52 54 57 60 61 64 68 71 74 78 81 84 88";
                nodeIndices(1) = "0 8 10 12 14 16 18 19 22 25 28 31 34 39 43 47 51 55 59 62 65 67 70 72 75 77 80 82 85 87 90";
                nodeIndices(2) = "1 3 5 9 13 17 20 24 26 30 32 36 37 40 42 45 48 50 53 56 58 63 66 69 73 76 79 83 86 89";
                break;
            case 127:
                nodeIndices(0) = "2 4 6 7 11 15 21 23 27 29 33 35 38 41 44 46 49 52 54 57 60 61 64 68 71 74 78 81 84 88 93 96 98 101 105 108 110 113 117 120 122 125";
                nodeIndices(1) = "0 8 10 12 14 16 18 19 22 25 28 31 34 39 43 47 51 55 59 62 65 67 70 72 75 77 80 82 85 87 90 91 94 97 100 103 106 109 112 115 118 121 124";
                nodeIndices(2) = "1 3 5 9 13 17 20 24 26 30 32 36 37 40 42 45 48 50 53 56 58 63 66 69 73 76 79 83 86 89 92 95 99 102 104 107 111 114 116 119 123 126";
                break;
            default:
                cout<<"Unsupported nCell for FRF=3..."<<endl;abort();
                break;
        }
    }
    else
    {
        cout<<"Unsupported value of frequency reuse factor..."<<endl;abort();
    }
    if(sum(findUniqueNumbers(joinArrayToVector(nodeIndices))-getIntegers(0,nCells-1))!=0)
    {
        cout<<"Improper mapping of cells for FRF"<<endl;abort();
    }
    return nodeIndices;
}

Array<ivec> getNodeIndicesForPolarisationReuse(int nCells)
{
    Array<ivec> nodeIndices(4);
    
    switch(nCells)
    {
        case 7:
            nodeIndices(0) = "0"; //RHCP part 1
            nodeIndices(1) = "1 4"; //RHCP part 2
            nodeIndices(2) = "2 5";//LHCP part 1
            nodeIndices(3) = "3 6";    //LHCP part 2 
            break;
        case 19:
            nodeIndices(0) = "0 7 9 11 13 15 17"; //RHCP part 1
            nodeIndices(1) = "1 4 10 16"; //RHCP part 2
            nodeIndices(2) = "2 5 12 18";//LHCP part 1
            nodeIndices(3) = "3 6 8 14";    //LHCP part 2 
            break;
        case 37:
            nodeIndices(0) = "0 7 9 11 13 15 17 "; //RHCP part 1
            nodeIndices(1) = "1 4 10 16 19 21 26 28 30 35"; //RHCP part 2
            nodeIndices(2) = "2 5 12 18 20 22 24 29 31 33";//LHCP part 1
            nodeIndices(3) = "3 6 8 14 23 25 27 32 34 36";    //LHCP part 2 
            break;
        case 61:
            nodeIndices(0) = "0 7 9 11 13 15 17 37 39 41 43 45 47 49 51 53 55 57 59"; //RHCP part 1
            nodeIndices(1) = "1 4 10 16 19 21 26 28 30 35 42 44 54 56"; //RHCP part 2
            nodeIndices(2) = "2 5 12 18 20 22 24 29 31 33 46 48 58 60";//LHCP part 1
            nodeIndices(3) = "3 6 8 14 23 25 27 32 34 36 38 40 50 52";    //LHCP part 2 
            break;
        case 91:
            nodeIndices(0) = "0 7 9 11 13 15 17 37 39 41 43 45 47 49 51 53 55 57 59"; //RHCP part 1
            nodeIndices(1) = "1 4 10 16 19 21 26 28 30 35 42 44 54 56 61 63 65 72 74 76 78 80 87 89"; //RHCP part 2
            nodeIndices(2) = "2 5 12 18 20 22 24 29 31 33 46 48 58 60 62 64 66 68 70 77 79 81 83 85";//LHCP part 1
            nodeIndices(3) = "3 6 8 14 23 25 27 32 34 36 38 40 50 52 67 69 71 73 75 82 84 86 88 90";    //LHCP part 2 
            break;
        case 127:
            nodeIndices(0) = "0 7 9 11 13 15 17 37 39 41 43 45 47 49 51 53 55 57 59 91 93 95 97 99 101 103 105 107 109 111 113 115 117 119 121 123 125"; //RHCP part 1
            nodeIndices(1) = "1 4 10 16 19 21 26 28 30 35 42 44 54 56 61 63 65 72 74 76 78 80 87 89 98 100 102 116 118 120"; //RHCP part 2
            nodeIndices(2) = "2 5 12 18 20 22 24 29 31 33 46 48 58 60 62 64 66 68 70 77 79 81 83 85 104 106 108 122 124 126";//LHCP part 1
            nodeIndices(3) = "3 6 8 14 23 25 27 32 34 36 38 40 50 52 67 69 71 73 75 82 84 86 88 90 92 94 96 110 112 114";    //LHCP part 2 
            break;
        default:
            cout<<"Unsupported nCells for FRF=2..."<<endl;abort();
            break;
    }
    
    if(sum(findUniqueNumbers(joinArrayToVector(nodeIndices))-getIntegers(0,nCells-1))!=0)
    {
        cout<<"Improper mapping of cells for FRF"<<endl;abort();
    }
    return nodeIndices;
}

ivec getPolarisationIDPerCell(int nCells)
{
    Array<ivec> nodeIndices = getNodeIndicesForPolarisationReuse(nCells);
    ivec polIDPerCell = zeros_i(nCells) - 1;
    for(int freq_cnt=0;freq_cnt<4;freq_cnt++)
    {
        setInVec(polIDPerCell,nodeIndices(freq_cnt),freq_cnt);
    }
    return polIDPerCell;
}

ivec getFrequencyIDPerCell(int reuseFactor,int nCells)
{
    if(reuseFactor==2) return getPolarisationIDPerCell(nCells);
    
    Array<ivec> nodeIndices = getNodeIndicesForFrequencyReuse(reuseFactor,nCells);
    ivec freqIDPerCell = zeros_i(nCells) - 1;
    for(int freq_cnt=0;freq_cnt<reuseFactor;freq_cnt++)
    {
        setInVec(freqIDPerCell,nodeIndices(freq_cnt),freq_cnt);
    }
    return freqIDPerCell;
}

ivec getRefCellID(int reuseFactor)
{
    if(reuseFactor==1)
        return "0";
    else if(reuseFactor==2)
        return "0 1 2 3";
    else if(reuseFactor==3)
        return "2 0 1";
    cout<<"Unsupported Frequency re-use factor"<<endl;abort();
}



