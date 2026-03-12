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
#ifndef CTRAFFICCLASS_H
#define CTRAFFICCLASS_H

// #include "../Others/cPacketQueue.h"
#include "../Others/cPacketQueue.h"
typedef struct _sPacketSubFrameNoPair {
	unsigned int subFrameNo;
	unsigned int pktSize;
	unsigned int arrivalSubFrameNo;
  double       arrivalTime;

  _sPacketSubFrameNoPair()
      : subFrameNo(0), pktSize(0), arrivalSubFrameNo(0), arrivalTime(-1.0){};
} sPacketSubFrameNoPair;

#define _frameDuration 1e-3

class cTrafficClass
{
protected:	  
  int mSourceId; /**< Source Identification Number */
  int mDestinationId; /**< Destination Identification Number */
  eTrafficType mTrafficType; /**< Type of Traffic */
  long int mSubFrameIndx;	/**< A sub frame index starts from ZERO. */
  int mAppId;  /**< Application Identification Number */
  int mSimTime;	/** Simulation duration **/
  double mFrameDuration;
  int startTime;
  vector <sPacketSubFrameNoPair> mPacketSubFrameNoPair;
  int mPktToSendIndx;
public:
  unsigned long int mDataToTransmit = 0; //Total Number of bytes to be transmitted


        /**
         * @brief
         *
         * @fn cTrafficClass
        */
        cTrafficClass();


        /**
         * @brief
         *
         * @fn cTrafficClass
         * @param src
         * @param dest
         * @param tAppId
        */
        cTrafficClass(eTrafficType applicationType,int src, int dest,int tAppId, int tSimTime);
        cTrafficClass(int src, int dest,int tAppId, int tSimTime );
        /**
         * @brief
         *
         * @fn ~cTrafficClass
        */
        ~cTrafficClass();
	
        /**
         * @brief
         *
         * @fn fSetTrafficType
         * @param applicationType
        */
        void fSetTrafficType (eTrafficType applicationType);

        /**
         * @brief
         *
         * @fn fGetTrafficType
         * @param void
         * @return eTrafficType
        */
        eTrafficType fGetTrafficType (void) const;
	
        /**
         * @brief
         * Set Source Identification Number
         * @fn fSetSourceID
         * @param int
        */
        void fSetSourceID (int src);
	
        /**
         * @brief
         * Returns Source Identification Number
         * @fn fGetSourceId
         * @param void
         * @return int
        */
        int fGetSourceId (void) const;


        /**
         * @brief
         * Sets Destination Identification Number
         * @fn fSetDestinationId
         * @param int dest
	 * Takes Destination Identification Number as Input
        */
        void fSetDestinationId (int dest);
	
        /**
         * @brief
         * Returns Destination Identification Number
         * @fn fGetDestinationId
         * @param void
         * @return int
	 * Destination Identification Number
        */
        int fGetDestinationId (void) const;
	
	/**
         * @brief
         * Intialize the Traffic Generator associated with the Radio Bearer.
         * @fn fInitialize
         * @param eTrafficType tTrafficClass
	 * Traffic Class passed.
         * @param int src
	 * Source Identification Number       
         * @param int dest
	 * Destination Identification Number
	*/
       void fInitialize(eTrafficType trafficType, int src, int dest);
	
	 
        /**
         * @brief
         * Sets the member variable mSubFrameIndx 
         * @fn fSetSubFrameIndx
         * @param long int tSubFrameIndx
	 * 
        */
        void fSetSubFrameIndx(long int tSubFrameIndx);
	
        /**
         * @brief
         * Retrieves the current subframe index
         * @fn fGetSubFrameIndx
         * @return int mSubFrameIndx
	 * Returns current subframe index 
        */
         long int fGetSubFrameIndx (void) const;


        /**
         * @brief
         *
         * @fn fCallTraffic
         * @return cPacketQueue
        */
        cPacketQueue fCallTraffic();
        cPacketQueue fCallTraffic(int subframeNumber);
        cPacketQueue fCallTraffic(double miniSlotStart, double miniSlotEnd);
        void fEnqueuePktToQueue(cPacketQueue *tPktQueue, int pktIndx);
	
        /**
         * @brief
         *
         * @fn fGetAppId
         * @return int
        */
        int fGetAppId() const;

        /**
         * @brief
         *
         * @fn fSetAppId
         * @param tAppId
        */
        void fSetAppId(int tAppId);
	
	
	/**
	  * @brief
	  *
	  * @fn fGetSimTime
	  * @return int
	  */
	int fGetSimTime() const;

	/**
	  * @brief
	  *
	  * @fn fSetAppId
	  * @param tAppId
	  */
	void fSetSimTime(int tSimTime);
    int getNumOfPackets(){return mPacketSubFrameNoPair.size();}
    int getEndTime(){ return ((!getNumOfPackets()) ? 0 : mPacketSubFrameNoPair[mPacketSubFrameNoPair.size()-1].subFrameNo);}
    void generatePacket(int subframe,int pktSize);
    void generatePacket(int subframe,int pktSize, double curTime);
    void print(int nSessions,int rbID=0);
};

#endif // CTRAFFICCLASS_H
