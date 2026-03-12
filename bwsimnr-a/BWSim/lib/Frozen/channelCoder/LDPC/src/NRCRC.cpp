#include "../include/NRCRC.h"

int NrCRC::getNumParity()
{
 int no_parity;
  switch(crcTypeNR)
  {
      case(0):
          no_parity = 8;
          break;
      case(1):
          no_parity = 11;
          break;
      case(2):
          no_parity = 16;
          break;
      case(3):
      case(4):
      case(5):
          no_parity = 24;
          break;
      default:
          std::cout << "Wrong crc polynomial type chosen, aborting.."<< std::endl;
          abort();
  }
  return no_parity;
}

std::vector<bool> NrCRC::performCRCEncodingNR(const std::vector<bool>& inBits)
{
  int polynomial = crcPolynomialsNR[crcTypeNR];   // Polynomial is stored as integer (Binary literal)
  std::vector<bool> ip(inBits);
  int no_parity=getNumParity() ;  // No of parity bits =  Degree of CRC polynomial 
  
//   std::cout << no_parity << std::endl; 
  std::vector<bool> parity(no_parity,0);   //Parity bits initialized with zeros
  int buffer=0;    // buffer contains the CRC bits in the form of interger
  std::vector<bool> output(ip);   // output is initialized with ip
//       std::cout<<"[ " ;
//     for(auto x : output)
//     { 
//         std::cout<< x<<",";
//     }
// 
//     std::cout<<"\b ]"<<std::endl;
  
  output.insert(output.end(),parity.begin(),parity.end());   // Appending Parity to Output (Intializing)
  for(auto x:output)
  {
      buffer<<=1;   // left shift the buffer by one position
      buffer+=x;    // Pushing x into the shifted position
      buffer ^= ((buffer & (1<<no_parity))>0)*polynomial;   // if the pushed x is 1, buffer is xor'ed with polynomial otherwise buffer remains as it is(here this is done with xor'ing with zero) 
  }
  for(auto it=output.rbegin();it != (output.rbegin()+no_parity); ++it)
  {
      *it=buffer&1;   // this for loop extracts each crc bit from buffer and stores in corresponding parity location
      buffer>>=1;
  }
  

  return output;
}

std::vector<bool> NrCRC::performCRCEncodingNR(const std::vector<bool>& inBits, CRCTYPE polyType)
{
    int polynomial = crcPolynomialsNR[polyType];   // Polynomial is stored as integer (Binary literal)
  std::vector<bool> ip(inBits);
  int no_parity = getNumParity() ;  // No of parity bits =  Degree of CRC polynomial 
  
//   std::cout << no_parity << std::endl; 
  std::vector<bool> parity(no_parity,0);   //Parity bits initialized with zeros
  int buffer=0;    // buffer contains the CRC bits in the form of interger
  std::vector<bool> output(ip);   // output is initialized with ip
//       std::cout<<"[ " ;
//     for(auto x : output)
//     { 
//         std::cout<< x<<",";
//     }
// 
//     std::cout<<"\b ]"<<std::endl;
  
  output.insert(output.end(),parity.begin(),parity.end());   // Appending Parity to Output (Intializing)
  for(auto x:output)
  {
      buffer<<=1;   // left shift the buffer by one position
      buffer+=x;    // Pushing x into the shifted position
      buffer ^= ((buffer & (1<<no_parity))>0)*polynomial;   // if the pushed x is 1, buffer is xor'ed with polynomial otherwise buffer remains as it is(here this is done with xor'ing with zero) 
  }
  for(auto it=output.rbegin();it != (output.rbegin()+no_parity); ++it)
  {
      *it=buffer&1;   // this for loop extracts each crc bit from buffer and stores in corresponding parity location
      buffer>>=1;
  }
  

  return output;
}


bool NrCRC::performCRCCheckNR(std::vector<bool>& inBits)  // Return true if crc check is successful otherwise return false
{
  int polynomial = crcPolynomialsNR[crcTypeNR];    // Polynomial is stored as integer (Binary literal)
  std::vector<bool> ip(inBits);
  int no_parity = getNumParity();  // No of parity bits =  Degree of CRC polynomial 
  
  int buffer=0;
  std::vector<bool> output(inBits);
  for(auto x:output)
  {
      buffer<<=1;             // See performCRCEncoding for reference
      buffer+=x;
      buffer ^= ((buffer & (1<<no_parity))>0)*polynomial;
  }
  
 if(buffer == 0)      //if buffer = 0, check is successful
      return true;
  else
      return false;
}

std::vector<bool> NrCRC::removeCRCNR(const std::vector<bool>& inBits)  // Discards CRC bits
{
    std::vector<bool> ip(inBits);
    std::vector<bool> output;
    int no_parity = getNumParity();
   
    output.insert(output.end(),ip.begin(),ip.end()-no_parity);
    
    return output;
}

std::vector<bool> NrCRC::removeCRCNR(const std::vector<bool>& inBits, CRCTYPE polyType, bool check)
{
    std::vector<bool> ip(inBits);
    std::vector<bool> output;
    if(check == true)
    {
        int no_parity = getNumParity();
   
        output.insert(output.end(),ip.begin(),ip.end()-no_parity);
        
    }
    else
    {
        std::cout << "CRC check failed" <<std::endl;
        abort();
    }
    
    return output;
}





