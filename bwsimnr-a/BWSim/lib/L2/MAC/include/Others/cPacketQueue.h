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
#ifndef CPACKETQUEUE_H
#define CPACKETQUEUE_H

#include<iostream>
#include <sstream>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <sys/time.h>	//! For Time difference in microseconds resolution
#include <map>		//! For captureTime() function
#include <assert.h>     //! For using assert()
#include <iomanip>
#include<vector>
#include<list>
#include<cstdlib>

//Sim inclue belongs to 
// #include"../lib/L2Support/include/Sim/SimInclude.h"
#include "../../../../L1/PHY/include/StructsAndEnums.h"

#include "../../../../L1/PHY/include/FrameStructure.h"
#include "../../../../L1/PHY/include/Codebook.h"
//#include <lib/L1/PHY/include/AWGNTables.h>
#include "../../../../L1/PHY/include/AWGNTables.h"
// #include "../lib/L1/PHY/include/cLTETables.h"
#include "../../../../L1/PHY/include/cNRTables.h"
#include "../../../../L1/PHY/include/SupportingFunctions.h"
// #include "../lib/FAPISupport/UE_l1_l2/include/Subframe/cUEL1Config.h"
// #include "../lib/FAPISupport/l1_l2_Interface/include/Subframe/cSubFrameIndication.h"
// #include "../lib/FAPISupport/l1_l2_Interface/include/Subframe/cDLConfigRequest.h"
// #include "../lib/FAPISupport/l1_l2_Interface/include/Subframe/cTXRequest.h"
// #include "../lib/FAPISupport/l1_l2_Interface/include/Subframe/UplinkData/cRXCQIIndication.h"
// #include "../lib/FAPISupport/l1_l2_Interface/include/Subframe/UplinkData/cHARQIndication.h"
// #include "../lib/FAPISupport/macInterface/include/data_structure.h"
#include <arpa/inet.h>
#include "cPacket.h"
#include "math.h"
//#include "../../lib/simSupport/include/simSupport.h"


class cPacketQueue
{
    list<cPacket> mPackets; /**< Queue of Packets */
    vector<bool> mPQ;

public:
    /**
   * @brief
   *
   * @fn cPacketQueue
   * @param void
  */
    cPacketQueue ();

    cPacketQueue(cPacketQueue const& tPktQueue);
    cPacketQueue& operator=(cPacketQueue const& tPktQueue);
    /**
   * @brief
   *
   * @fn ~cPacketQueue
   * @param void
  */
    ~cPacketQueue (void);

    /**
   * @brief
   *
   * @fn fAddPacket
   * @param tpacket
  */
    void fAddPacket (cPacket& tpacket);

    /**
   * @brief
   *
   * @fn fGetPackets
   * @param void
   * @return std::list<cPacket>
  */
    list<cPacket > fGetPackets (void) const;
//     list<bool > fGetbool (void) const;
//     void fSetbool(int i);
//     std::vector< bool > fGetbool();
    void fSetPackets (list<cPacket > tPkts);
    void fResizeToZero(void); //Sriharsha(12/9/14)

    /**
   * @brief
   *
   * @fn fGetNumberOfPackets
   * @param void
   * @return unsigned int
  */
    unsigned int fGetNumberOfPackets (void) ;

    /**
   * @brief
   *
   * @fn fGetSize
   * @param void
   * @return unsigned int
  */
    unsigned int fGetSize (void) ;
    void fAddPktQueue(cPacketQueue tPktQueue);
    cPacket fGetPeekPacket();
    void fDequeuePkt();
    void fDeleteAllPkts();
    void fUpdatePeerPacket(cPacket tPkt);

    /**
   * @brief
   * This function identify the lowest SN among packets present in the queue
   * @fn fGetLowestSN
   * @return int
   * Returns lowest SN value
  */
//     int fGetLowestSN();
   

};

#endif // CPACKETQUEUE_H
