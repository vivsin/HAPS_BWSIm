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
#ifndef CCBR_H
#define CCBR_H

#include "cTrafficClass.h"
#include "../Others/L2SimConfig.h"


class cCBR : public cTrafficClass
{
private:

    int mPktSize;
    int mInterPktDelay;

public:
    /**
  * @brief
  * VoIP Traffic Generator Constructor.
  * @fn cVoip
  */
    cCBR();

    /**
  * @brief
  * VoIP Traffic Generator Constructor.
  * @fn cVoip
  * @param trafficType
  * Type of Traffic Generator.
  * @param int dest
  * Destination Identification Number
  * @param int src
  * Source Identification Number
  */
    cCBR(eTrafficType trafficType, int tSrc, int tDest,int tAppId, double frameDuration , int mPacketSize , double mInterPacketDelay, double mSimulationTime);
    cCBR(eTrafficType trafficType, string l2SupportFileName);
    cCBR(cTrafficClass& tTrafficClass);
    cCBR& operator=(cCBR const& tVoip);


    /**
  * @brief
  * VoIP Traffic Generator Destructor.
  * @fn ~cVoip
  */
    ~cCBR();

    /**
  * @brief
  * Function is called to Generate VoIP PacketQueue at the instant n.
  * @fn fCallTraffic
  * @param int instant
  * Instant when the fCallTraffic() function is called.
  * @return cPacketQueue &
  * Reference to the object of PacketQueue is returned.
  */

//     cPacketQueue fCallTraffic(int tSubframe);
    void genCBRTraffic(int tStartSubFrameNo, int tEndSubFrameNo);

    void fSetPktSize(int tPktSize);

    int fGetPktSize();

    void fSetInterPktDelay(int tInterPktDelay);

    int fGetInterPktDelay();

};
#endif // CCBR_H
