#include "../include/nrldpccodec.h"

void NRLDPCcodec::findParamKz(int bg, int k, int &Kb, int &K, int &z)
{
    if(bg==1)
  {
    Kb=22;
  }
  else if(bg==2)
  {
      if(k>640)
      {
          Kb = 10;
      }
      else if(k>560)
      {
          Kb = 9;
      }
      else if(k>192)
      {
          Kb = 8;
      }
      else
      {
          Kb = 6;
      }
  }
  else
  {
     std::cout<<"Wrong BG number chosen"<<std::endl;
      std::abort();   
  }
  for (auto x:liftingSizeSet)
    {
      if(x*Kb>=k)
        {
          z=x;
          break;
        }

    }
//   std::cout<<"z: "<<z<<std::endl;
//   if(z<24)
//     {
//       std::cout<<"z<24 changing to z=24"<<std::endl;
//       z=24;//this is minimum value according to qualcom pcm for bg1, will change later with 38.212
// 
//     }

//   if(2*z>=k)
//     {
//       std::cout<<"2z<k input length is too less exiting  z="<<z<<" k= "<<k<<std::endl;
//       std::abort();
//     }
  
  K=z*(34-12*bg);   // 22z for Bg=1, 10z for BG =2
//   std::cout<<"K: "<<K<<std::endl;
  
  if(k>K)
    {
      std::cout<<"k>K somethink is wrong exiting ...."<<k<<" >"<<K<<std::endl;
      std::abort();
    }
}
