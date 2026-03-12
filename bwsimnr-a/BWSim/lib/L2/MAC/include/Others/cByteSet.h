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

#ifndef CBYTESET_H
#define CBYTESET_H

#include "cByte.h"  

using namespace std;

class cByteSet
{
    vector<cByte> mBytes;

    vector<cByte> fGetBytesConst() const;

public:
    cByteSet();
    void fSetBytes(vector<cByte> tBytes);
    vector<cByte> fGetBytes();
    bool fGetNthBit(int tN);
    bool fGetNthBitInByte(int tN, int tByte);
    void fUpdateNthBit(bool tBit,int tN);
    void fUpdateNthBitInByte(bool tBit,int tN, int tByte);
    void fUpdateNBits(unsigned long int tVal,unsigned int tFrom,unsigned int tTo);
    void fUpdateNBitsInByte(unsigned int tVal,unsigned int tFrom,unsigned int tTo,unsigned int tByte);
    void fAddByte(cByte tByte);
    void fAddNBytes(int tN);
    void fCheckAndAdd(int tN);
    cByte fGetNthByte(int tN);
    friend ostream& operator<<( ostream &output, const cByteSet &D );
    void fGenerateRandomBytes(int tNBytes);
    void fAddBytesAtFirst(vector<cByte> tBytes);
    void fAddBytesAtLast(vector<cByte> tBytes);
    void fReset();
    vector<cByte> fGetAndRemoveFirstNBytes(int tBytesSize);
    unsigned int fGetBits(int tFrom, int tTo);
    vector<cByte> fGetAndRemoveLastNBytes(int tBytes);
    vector<cByte> fGetFirstNBytes(int tBytesSize);
    //char* fGetBytesInArray(); //WarningFix: unused function
    int fGetSize();
};

#endif // CBYTESET_H
