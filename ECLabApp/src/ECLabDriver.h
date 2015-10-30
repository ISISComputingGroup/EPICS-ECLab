/*************************************************************************\ 
* Copyright (c) 2013 Science and Technology Facilities Council (STFC), GB. 
* All rights reverved. 
* This file is distributed subject to a Software License Agreement found 
* in the file LICENSE.txt that is included with this distribution. 
\*************************************************************************/ 

/// @file ECLabDriver.h Header for #ECLabDriver class.
/// @author Freddie Akeroyd, STFC ISIS Facility, GB

#ifndef ECLABDRIVER_H
#define ECLABDRIVER_H

#include "asynPortDriver.h"
//#include "ECLabTechnique.h"
/// EPICS Asyn port driver class. 
class ECLabDriver : public asynPortDriver 
{
public:
	ECLabDriver(const char *portName, const char *ip);
	
	// These are the methods that we override from asynPortDriver
	virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
//	virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
	virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
//	virtual asynStatus readFloat64(asynUser *pasynUser, epicsFloat64 *value);
//	virtual asynStatus readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual, int *eomReason);
	virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);
//	virtual asynStatus readFloat64Array(asynUser *pasynUser, epicsFloat64 *value, size_t nElements, size_t *nIn);
//	virtual asynStatus readInt32Array(asynUser *pasynUser, epicsInt32 *value, size_t nElements, size_t *nIn);
	virtual void report(FILE* fp, int details);
//	void testPrintMap(techniqueMap_t);

private:
	int P_version; // string
    int P_host; // string
	int P_devCode; // int
	int P_numChannels; //int
	int P_numSlots; //int
    int P_numTech; // int
    int P_memFilled; // int
    int P_currEWE; // float
    int P_currECE; // float
    int P_currI; // float
    int P_currTIME; // float
    int P_currRCOMP; // float
    int P_currFREQ; // float
    int P_currSTATE; // int
	int P_loadTech; // string
	int P_startChannel; // int
	int P_stopChannel; //int
	
	TDeviceInfos_t m_infos;
	int m_ID; // connection ID
	
	#define FIRST_ECLAB_PARAM P_version
	#define LAST_ECLAB_PARAM P_stopChannel

	static void ECLabTaskC(void* arg);
	void ECLabTask();
};

#define NUM_ECLAB_PARAMS    (&LAST_ECLAB_PARAM - &FIRST_ECLAB_PARAM + 1)

#define P_versionString "VERSION"
#define P_hostString "HOST"
#define P_devCodeString "DEVCODE"
#define P_numChannelsString "NUMCHANNELS"
#define P_numSlotsString "NUMSLOTS"
#define P_numTechString "NUMTECH"
#define P_memFilledString "MEMFILLED"
#define  P_currEWEString "CURR_EWE"
#define  P_currECEString "CURR_ECE"
#define  P_currIString   "CURR_I"
#define  P_currTIMEString  "CURR_TIME"
#define  P_currRCOMPString   "CURR_RCOMP"
#define  P_currFREQString    "CURR_FREQ"
#define  P_currSTATEString    "CURR_STATE"
#define P_loadTechString "LOADTECH"
#define P_startChannelString "STARTCHANNEL"
#define P_stopChannelString "STOPCHANNEL"

#endif /* ECLABDRIVER_H */
