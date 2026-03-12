/*************************************************************************
*
* CEWiT CONFIDENTIAL
* __________________
*
* All Rights Reserved © 2014 CEWiT, India
*
\ NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
* and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
* Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
* express, printed and signed license for use is strictly forbidden.
*/

#ifndef _NRCRC_H
#define _NRCRC_H
#include<vector>
#include<cmath>
#include<iostream>
#include "../../../simSupport/include/simSupport.h"

enum CRCTYPE
{
  CRC8 = 0, CRC11 = 1,CRC16 = 2,CRC24A = 3,CRC24B = 4, CRC24C = 5  // crc11 and crc24c are new additons and crc8 excluded
};

class NrCRC
{

  public:
    CRCTYPE crcTypeNR;
    std::vector<int> crcPolynomialsNR;
    int getNumParity();
    std::vector<bool> performCRCEncodingNR(const std::vector<bool> &inBits);
    std::vector<bool> performCRCEncodingNR(const std::vector<bool> &inBits, CRCTYPE polyType);
    bool performCRCCheckNR(std::vector<bool> &inBits);
    std::vector<bool> removeCRCNR(const std::vector<bool> &inBits);
    std::vector<bool> removeCRCNR(const std::vector<bool> &inBits, CRCTYPE polyType, bool check);
    void setCRCTypeNR(CRCTYPE polyType)
    {
        crcTypeNR = polyType;
    }
  NrCRC()
  {
    crcPolynomialsNR.reserve(6);
    crcPolynomialsNR[0] = 0b110011011;
    crcPolynomialsNR[1] = 0b111000100001;
    crcPolynomialsNR[2] = 0b10001000000100001;
    crcPolynomialsNR[3] = 0b1100000000000000001100011;
    crcPolynomialsNR[4] = 0b1100001100100110011111011;
    crcPolynomialsNR[5] = 0b1101100101011000100010111;
//     crcType = CRC_24C;    //Used while testing
  }
  
 
    

};


#endif
