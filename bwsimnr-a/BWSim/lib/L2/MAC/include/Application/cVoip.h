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
#ifndef CVOIP_H
#define CVOIP_H

#include "cTrafficClass.h"
#include "../Others/L2SimConfig.h"

class cVoip : public cTrafficClass
{
private:
    int mState; /**< State of the Traffic Generator **/
    int mCount; /**< Counter used for Silent Period **/

    /**
  * @brief
  * Function is called to get the size of VoIP Packet when it is called depends on the current instant value.
  * @fn fVoipSize
  * @return double
  * Type of Traffic Generator.
  */
    double fVoipSize();
    double fStateTransition();
    double fInitState();

public:
    /**
  * @brief
  * VoIP Traffic Generator Constructor.
  * @fn cVoip
  */
    cVoip();

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
    cVoip(eTrafficType trafficType);
    cVoip(cTrafficClass& tTrafficClass);
    cVoip& operator=(cVoip const& tVoip);
    cVoip(eTrafficType trafficType, int tSrc, int tDest, int tAppId, double mSimulationTime, double frameDuration);

    void genVoipTraffic(int tStartSubFrameNo, int tEndSubFrameNo);
    /**
  * @brief
  * VoIP Traffic Generator Destructor.
  * @fn ~cVoip
  */
    ~cVoip();

    /**
  * @brief
  * Function is called to Generate VoIP PacketQueue at the instant n.
  * @fn fCallTraffic
  * @param int instant
  * Instant when the fCallTraffic() function is called.
  * @return cPacketQueue &
  * Reference to the object of PacketQueue is returned.
  */

//     cPacketQueue fCallTraffic();

    int fGetState();
    void fSetState(int tState);

    int fGetMCount();


};

#endif // CVOIP_H
