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

/// EPICS Asyn port driver class. 
class ECLabDriver : public asynPortDriver 
{
public:
	ECLabDriver(const char *portName);

	// These are the methods that we override from asynPortDriver
//	virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
	virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
//	virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
//	virtual asynStatus readFloat64(asynUser *pasynUser, epicsFloat64 *value);
	virtual asynStatus readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual, int *eomReason);
	virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);
//	virtual asynStatus readFloat64Array(asynUser *pasynUser, epicsFloat64 *value, size_t nElements, size_t *nIn);
//	virtual asynStatus readInt32Array(asynUser *pasynUser, epicsInt32 *value, size_t nElements, size_t *nIn);
	virtual void report(FILE* fp, int details);

private:
    int P_par1; // double
	int P_par2; // string
	#define FIRST_ECLAB_PARAM P_par1
	#define LAST_ECLAB_PARAM P_par2

	static void ECLabTaskC(void* arg);
	void ECLabTask();
};

#define NUM_ECLAB_PARAMS    (&LAST_ECLAB_PARAM - &FIRST_ECLAB_PARAM + 1)

#define P_par1String "PAR1"
#define P_par2String "PAR2"

#endif /* ECLABDRIVER_H */
