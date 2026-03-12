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

#ifndef CBYTE_H
#define CBYTE_H

#include<cstdlib>
#include <stdlib.h>
#include <math.h>
#include <ostream>
#include <assert.h>
#include <vector>
#include <iostream>
//#include "../../lib/simSupport/include/simSupport.h"
#include "../../../../Frozen/wrapper/itppWrap/include/itppWrap.h"

using namespace std;

class cByte
{
    unsigned char mByte;
    unsigned char fGetByte() const;

public:
    cByte();
    void fSetBitAtPostion(bool tBit, int tPos);
    bool fGetBitAtPostion(int tPos);
    friend ostream &operator<<( ostream &output, const cByte &D );
    void fGenerateRamdomBits();
    void fUpdateBits(unsigned char tVal,unsigned tFrom, unsigned int tTo);
    unsigned int fGetBits(unsigned tFrom, unsigned int tTo);
    void fReset();
    char fGetChar();
    void fSetChar(char tVal);
};

#endif // CBYTE_H
