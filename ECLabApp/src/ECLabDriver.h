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
	virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
//	virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
//	virtual asynStatus readFloat64(asynUser *pasynUser, epicsFloat64 *value);
//	virtual asynStatus readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual, int *eomReason);
	virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);
//	virtual asynStatus readFloat64Array(asynUser *pasynUser, epicsFloat64 *value, size_t nElements, size_t *nIn);
//	virtual asynStatus readInt32Array(asynUser *pasynUser, epicsInt32 *value, size_t nElements, size_t *nIn);
	virtual void report(FILE* fp, int details);
//	void testPrintMap(techniqueMap_t);

private:
	int P_version; // string
	int P_devCode; 
	int P_defineTech;
	int P_numChannels;
	int P_loadTech;
	int P_channelInfo;
	int P_currentValues;
	int P_channelNum;
	int P_devID;
	int P_stopChannel;
	int P_firmwareVers;
	
	
	TDeviceInfos_t m_infos;
	
	#define FIRST_ECLAB_PARAM P_version
	#define LAST_ECLAB_PARAM P_firmwareVers

	static void ECLabTaskC(void* arg);
	void ECLabTask();
};

#define NUM_ECLAB_PARAMS    (&LAST_ECLAB_PARAM - &FIRST_ECLAB_PARAM + 1)

#define P_versionString "VERSION"
#define P_devCodeString "DEVCODE"
#define P_defineTechString "DEFINETECH"
#define P_firmwareVersString "FIRMWAREVERS" 
#define P_numChannelsString "NUMCHANNELS"
#define P_loadTechString "LOADTECH"
#define P_channelInfoString "CHANNELINFO"
#define P_channelNumString "CHANNELNUM"
#define P_devIDString "DEVID"
#define P_stopChannelString "STOPCHANNEL"
#define P_currentValuesString "CURRENTVALUES"
#endif /* ECLABDRIVER_H */
