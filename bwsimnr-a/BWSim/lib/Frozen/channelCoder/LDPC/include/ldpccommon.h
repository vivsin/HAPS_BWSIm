#ifndef LDPCCOMMON_H
#define LDPCCOMMON_H

#include<vector>
#include<iostream>
#include<cmath>
#include<set>
#include<map>
#include<cstdint>
#include<string>
#include<fstream>
#include<sstream>
//#include<cstdlib>

#include <list>

inline std::vector<std::vector<int>> readCsvFile(std::string filename);
inline std::vector<std::vector<int>> convertbgtobgp(std::vector<std::vector<int> > bg);

struct LdpcCodeParam{
 alignas(long int)  int16_t z;
  int16_t bg;
};


union LdpcCodeParamKey{
  alignas(long int) LdpcCodeParam ldpcCodeparm;
  alignas(long int) long int key;
  LdpcCodeParamKey(const LdpcCodeParam lcp):ldpcCodeparm(lcp){}
};

struct LdpcParameters{

int16_t z;
int16_t bgp;
int16_t pij;
};

inline std::ostream & operator <<(std::ostream & a,LdpcParameters b)
{
  a<<"Lift Z="<<b.z<<" bgp  "<<b.bgp<<"pij "<<b.pij<<" stp ="<< b.z*b.bgp+b.pij<<" " ;
  return a;
}



union UnionVector{
std::vector<bool>   b;
std::vector<long int>  c;
UnionVector(std::vector<bool>& B):b(B){}
UnionVector(const std::vector<bool>& B):b(B){}

~UnionVector(){}

};

// extern std::map<int,std::vector<std::vector<int>>>allperm;
// extern std::set<std::set<int>>liftValueZSet;

//class Vec{
//public:
//  UnionVector a;
//    Vec(std::vector<bool> b):a(b){  }
//    Vec(const Vec & B):a(B.a.b){}
//Vec & operator &(Vec & B);
//Vec & operator ^(Vec & B);
//Vec & operator |(Vec & B);
//};

//inline Vec &Vec::operator&( Vec & B)
//{
//  if(a.b.size() != B.a.b.size())
//    {
//        //*std::cout<<"size mismatch aborting "<<a.b.size()<<" != "<<B.a.b.size()<<std::endl;exit (10);
//    }
//  Vec &&  C(B.a.b);
//  for(int i=0;i<a.b.capacity()/64;++i)
//    {
//            C.a.c[i]&=a.c[i];
//    }
//  return (C);
//}
//inline Vec &Vec::operator^(Vec &B)
//{
//  if(a.b.size() != B.a.b.size())
//    {
//        //*std::cout<<"size mismatch aborting "<<a.b.size()<<" != "<<B.a.b.size()<<std::endl;exit (10);
//    }
//  Vec &&  C(B.a.b);
//  for(int i=0;i<a.b.capacity()/64;++i)
//    {
//            C.a.c[i]^=a.c[i];
//    }
//  return (C);
//}


//inline Vec &Vec::operator|(Vec &B)
//{
//  if(a.b.size() != B.a.b.size())
//    {
//        //*std::cout<<"size mismatch aborting "<<a.b.size()<<" != "<<B.a.b.size()<<std::endl;exit (10);
//    }
//  Vec &&  C(B.a.b);
//  for(int i=0;i<a.b.capacity()/64;++i)
//    {
//            C.a.c[i]|=a.c[i];
//    }
//  return (C);
//}


class LDPCcodec
{
public:
  LDPCcodec();
};



union LdpcParamKey{
  LdpcParameters ldpcparm;
  long int key;
  LdpcParamKey(const LdpcParameters lp):ldpcparm(lp){}
};

struct Graphs{
std::vector<std::vector<int>> bg1=readCsvFile("configFiles/LDPCConfig/ldpcbg1.csv");
std::vector<std::vector<int>> bg2=readCsvFile("configFiles/LDPCConfig/ldpcbg2.csv");
// std::vector<std::vector<int>> P24=readCsvFile("configFiles/LDPCConfig/Graph24_PCM.csv");
// std::vector<std::vector<int>> P32=readCsvFile("configFiles/LDPCConfig/Graph32_PCM.csv");
// std::vector<std::vector<int>> P48=readCsvFile("configFiles/LDPCConfig/Graph48_PCM.csv");
// std::vector<std::vector<int>> P64=readCsvFile("configFiles/LDPCConfig/Graph64_PCM.csv");
// std::vector<std::vector<int>> P96=readCsvFile("configFiles/LDPCConfig/Graph96_PCM.csv");
// std::vector<std::vector<int>> P128=readCsvFile("configFiles/LDPCConfig/Graph128_PCM.csv");
// std::vector<std::vector<int>> P192=readCsvFile("configFiles/LDPCConfig/Graph192_PCM.csv");
// std::vector<std::vector<int>> P256=readCsvFile("configFiles/LDPCConfig/Graph256_PCM.csv");
std::vector<std::vector<int>> BG1_Ils1 = readCsvFile("configFiles/LDPCConfig/BG1_Ils_1.csv");
std::vector<std::vector<int>> BG1_Ils2 = readCsvFile("configFiles/LDPCConfig/BG1_Ils_2.csv");
std::vector<std::vector<int>> BG1_Ils3 = readCsvFile("configFiles/LDPCConfig/BG1_Ils_3.csv");
std::vector<std::vector<int>> BG1_Ils4 = readCsvFile("configFiles/LDPCConfig/BG1_Ils_4.csv");
std::vector<std::vector<int>> BG1_Ils5 = readCsvFile("configFiles/LDPCConfig/BG1_Ils_5.csv");
std::vector<std::vector<int>> BG1_Ils6 = readCsvFile("configFiles/LDPCConfig/BG1_Ils_6.csv");
std::vector<std::vector<int>> BG1_Ils7 = readCsvFile("configFiles/LDPCConfig/BG1_Ils_7.csv");
std::vector<std::vector<int>> BG1_Ils8 = readCsvFile("configFiles/LDPCConfig/BG1_Ils_8.csv");

std::vector<std::vector<int>> BG2_Ils1 = readCsvFile("configFiles/LDPCConfig/BG2_Ils_1.csv");
std::vector<std::vector<int>> BG2_Ils2 = readCsvFile("configFiles/LDPCConfig/BG2_Ils_2.csv");
std::vector<std::vector<int>> BG2_Ils3 = readCsvFile("configFiles/LDPCConfig/BG2_Ils_3.csv");
std::vector<std::vector<int>> BG2_Ils4 = readCsvFile("configFiles/LDPCConfig/BG2_Ils_4.csv");
std::vector<std::vector<int>> BG2_Ils5 = readCsvFile("configFiles/LDPCConfig/BG2_Ils_5.csv");
std::vector<std::vector<int>> BG2_Ils6 = readCsvFile("configFiles/LDPCConfig/BG2_Ils_6.csv");
std::vector<std::vector<int>> BG2_Ils7 = readCsvFile("configFiles/LDPCConfig/BG2_Ils_7.csv");
std::vector<std::vector<int>> BG2_Ils8 = readCsvFile("configFiles/LDPCConfig/BG2_Ils_8.csv");
std::vector<std::vector<int>>bg1p=convertbgtobgp( readCsvFile("configFiles/LDPCConfig/ldpcbg1.csv"));
std::vector<std::vector<int>>bg2p=convertbgtobgp( readCsvFile("configFiles/LDPCConfig/ldpcbg2.csv"));
};
struct PCM{
  Graphs G;
  std::map<int,std::vector<std::vector<int>>&>mapztopcm;
  std::vector<std::vector<int>>& getBg(int z)
  {
    switch (z) {
      case 1:
        return G.bg1;
        break;

      case 2:

        return G.bg2;
        break;

      default:
        std::cerr<<" looks like BGz is not a supported value exiting ..... z="<<z<<std::endl;
        exit(2);
        break;
      }

  }
  std::vector<std::vector<int>>& getBgp(int z)
  {
    switch (z) {
      case 1:
        return G.bg1p;
        break;

      case 2:

        return G.bg2p;
        break;

      default:
        std::cerr<<" looks like BGzp is not a supported value exiting ..... z="<<z<<std::endl;
        exit(2);
        break;
      }

  }

  std::vector<std::vector<int>>& getPcm(int z, int bg)
  {
    switch (z) {
      case 2:
        case 4:
        case 8:
        case 16:
        case 32:
        case 64:
        case 128:
        case 256:
        if(bg==1)
        return G.BG1_Ils1;
        else if(bg == 2)
        return G.BG2_Ils1;
        break;
        
        case 3:
        case 6:
        case 12:
        case 24:
        case 48:
        case 96:
        case 192:
        case 384:
        if(bg==1)
        return G.BG1_Ils2;
        else if(bg == 2)
        return G.BG2_Ils2;
        break;
        
        case 5:
        case 10:
        case 20:
        case 40:
        case 80:
        case 160:
        case 320:
        if(bg==1)
        return G.BG1_Ils3;
        else if(bg == 2)
        return G.BG2_Ils3;
        break;
        
        case 7:
        case 14:
        case 28:
        case 56:
        case 112:
        case 224:
        if(bg==1)
        return G.BG1_Ils4;
        else if(bg == 2)
        return G.BG2_Ils4;
        break;
      
        case 9:
        case 18:
        case 36:
        case 72:
        case 144:
        case 288:
        if(bg==1)
        return G.BG1_Ils5;
        else if(bg == 2)
        return G.BG2_Ils5;
        break;

        case 11:
        case 22:
        case 44:
        case 88:
        case 176:
        case 352:
        if(bg==1)
        return G.BG1_Ils6;
        else if(bg == 2)
        return G.BG2_Ils6;
        break;

        case 13:
        case 26:
        case 52:
        case 104:
        case 208:
        if(bg==1)
        return G.BG1_Ils7;
        else if(bg == 2)
        return G.BG2_Ils7;
        break;

        case 15:
        case 30:
        case 60:
        case 120:
        case 240:
        if(bg==1)
        return G.BG1_Ils8;
        else if(bg == 2)
        return G.BG2_Ils8;
        break;

      default:
        std::cerr<<" looks like z is not a supported value exiting ..... z="<<z<<std::endl;
        exit(5);
        break;
      }
      std::cerr<<" looks like z is not a supported value exiting ..... z="<<z<<std::endl;
      exit(5);
  }
  };

// extern std::map<long int , std::vector<int>> z_bgp_pij_VectorMap;
// 
// inline std::vector<int> & setVectorforZ_Bgp_Pij(const LdpcParameters lp){
// LdpcParamKey lpk(lp);
// std::vector<int>   tmpVec;
// tmpVec.reserve(lp.z);
// auto bp=lp.z*lp.bgp;
// for(int i=0;i<lp.z;++i)
//   {
//       tmpVec.push_back(bp+((i+lp.pij)%lp.z));
//   }
// z_bgp_pij_VectorMap[lpk.key]=tmpVec;
// return z_bgp_pij_VectorMap[lpk.key];
// }

inline std::vector<int> getVectorforZ_Bgp_Pij(const LdpcParameters lp)
{
  LdpcParamKey lpk(lp);

  std::vector<int>   tmpVec;
  tmpVec.reserve(lp.z);
  auto bp=lp.z*lp.bgp;
  for(int i=0;i<lp.z;++i)
    {
        tmpVec.push_back(bp+((i+lp.pij)%lp.z));
    }
    //*std::cout<<"lp.z="<<lp.z<<std::endl;
    //*std::cout<<"tmpVec.size="<<tmpVec.size()<<std::endl;

  return tmpVec;
//  auto it=z_bgp_pij_VectorMap.find(lpk.key);
//  if(it!=z_bgp_pij_VectorMap.end())
//    {
//      return it->second;
//    }
//  else
//    {
//      return setVectorforZ_Bgp_Pij(lp);
//    }
}
inline std::vector<std::vector<int>> convertbgtobgp(std::vector<std::vector<int>>  bg)
{
  std::vector<std::vector<int>>bgp;
  bgp.reserve(bg.size());
  for(auto r:bg)
    {
      std::vector<int>rp;
      int col=0;
      for(auto c:r)
        {
          if(c==1){
              rp.push_back(col);
            }
          ++col;
        }
      bgp.push_back(rp);
    }

    //*std::cout<<bgp.size()<<"  bgp "<<std::endl;
  for(auto &x:bgp)
    {
    for(auto & y:x)
      {
          //*std::cout<<y<<",";
      }

                     //*std::cout<<"             si "<<x.size()<<std::endl;

}

  return bgp;
}

inline std::vector<std::vector<int>> readCsvFile(std::string filename)///readCSVfile function is suitable for ldpcmatrix base graph and permutations;
{

 std::ifstream fin(filename);
 std::vector<std::string> lines;
 for(std::string line;std::getline(fin,line);)
   {
     lines.push_back(line);

   }
 std::vector<std::vector<int>>rvec;
rvec.reserve(lines.size());

//for(auto &x:lines)
  //   //*std::cout<<x<<std::endl;

 for(auto &line:lines)
   {
     std::vector<int>vecv;
     std::stringstream ss;ss<<line;
     for(std::string val; std::getline(ss,val,',') ;)
      {
    //       //*std::cout<<val<<std::endl;
         vecv.push_back(std::stoi(val));
       }
     rvec.push_back(vecv);
   }

   //*std::cout<<rvec.size()<<"  "<<rvec[0].size()<<std::endl;
 for(auto &x:rvec)
   {
   for(auto & y:x)
     {
         //*std::cout<<y<<",";
     }

                    //*std::cout<<"             si "<<x.size()<<std::endl;
}
 return std::move(rvec);
   }


struct LdpcLift{
  LdpcParameters lp;
  std::vector<int>lpv;
  LdpcLift(LdpcParameters lpi):lp(lpi),lpv(0)
  {
    lpv=getVectorforZ_Bgp_Pij(lpi);
  }
};



class LDPCcommon
{
public:
  LDPCcommon();
};

#endif // LDPCCOMMON_H
