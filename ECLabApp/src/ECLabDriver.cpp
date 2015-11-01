/*************************************************************************\ 
* Copyright (c) 2013 Science and Technology Facilities Council (STFC), GB. 
* All rights reverved. 
* This file is distributed subject to a Software License Agreement found 
* in the file LICENSE.txt that is included with this distribution. 
\*************************************************************************/ 

/// @file ECLabDriver.cpp Implementation of #ECLabDriver class and ECLabConfigure() iocsh command
/// @author Freddie Akeroyd, STFC ISIS Facility, GB

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <exception>
#include <iostream>
#include <map>
#include <vector>

#include <epicsTypes.h>
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsString.h>
#include <epicsTimer.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <errlog.h>
#include <iocsh.h>

#include "BLFunctions.h"
#include "ECLabInterface.h"
#include "ECLabDriver.h"
#include "ECLabParams.h"

#include <epicsExport.h>

static const char *driverName="ECLabDriver"; ///< Name of driver for use in message printing 

#if 0
asynStatus ECLabDriver::readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual, int *eomReason)
{
	int function = pasynUser->reason;
	int status=0;
	const char *functionName = "readOctet";
	const char *paramName = NULL;
	getParamName(function, &paramName);
	std::string value_s;
 	try
	{
	
		if ( value_s.size() > maxChars ) // did we read more than we have space for?
		{
			*nActual = maxChars;
			if (eomReason) { *eomReason = ASYN_EOM_CNT | ASYN_EOM_END; }
			asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
				"%s:%s: function=%d, name=%s, value=\"%s\" (TRUNCATED from %d chars)\n", 
				driverName, functionName, function, paramName, value_s.substr(0,*nActual).c_str(), value_s.size());
		}
		else
		{
			*nActual = value_s.size();
			if (eomReason) { *eomReason = ASYN_EOM_END; }
			asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
				"%s:%s: function=%d, name=%s, value=\"%s\"\n", 
				driverName, functionName, function, paramName, value_s.c_str());
		}
		strncpy(value, value_s.c_str(), maxChars); // maxChars  will NULL pad if possible, change to  *nActual  if we do not want this
		return asynSuccess;
	}
	catch(const std::exception& ex)
	{
		epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize, 
			"%s:%s: status=%d, function=%d, name=%s, value=\"%s\", error=%s", 
			driverName, functionName, status, function, paramName, value_s.c_str(), ex.what());
		*nActual = 0;
		if (eomReason) { *eomReason = ASYN_EOM_END; }
		value[0] = '\0';
		return asynError;
	}
}
#endif

asynStatus ECLabDriver::writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual) 
{
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	const char *paramName = NULL;
	getParamName(function, &paramName);
	const char* functionName = "writeOctet";
	std::string value_s(value, maxChars);
	int addr = 0; // channel number
	getAddress(pasynUser, &addr);
	try
	{
		if (function == P_loadTech)
		{
			char* valueCopy = strdup(value_s.c_str()); // as strtok modfies the string
			char *tok_save = NULL;
			char * tech = epicsStrtok_r(valueCopy, " ,;", &tok_save); //epics vers of strtok
			m_techniques.clear();
			while (tech != NULL)
			{
			    m_techniques.push_back(tech);
				tech = epicsStrtok_r(NULL, " ,;", &tok_save); 				
			}
			free(valueCopy);	
			TEccParams_t params; 
			for (int i = 0; i < m_techniques.size(); ++i)
			{
			    bool first = ( i == 0 );
				bool last = ( i == (m_techniques.size() - 1) );
				std::vector<TEccParam_t> values;
				getTechniqueParams(m_techniques[i], addr, values, false);
				if (values.size() > 0)
				{
				    params.pParams = &(values[0]);
				}
				else
				{
					params.pParams = NULL;
				}
				params.len = values.size();
				ECLabInterface::LoadTechnique(m_ID, addr, const_cast<char*>((m_techniques[i]+".ecc").c_str()), params, first, last, true);
			}
		}	
		asynPortDriver::writeOctet(pasynUser, value, maxChars, nActual);
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
			"%s:%s: function=%d, name=%s, value=%s\n", 
			driverName, functionName, function, paramName, value_s.c_str());
		*nActual = value_s.size();
		return asynSuccess;
	}
	catch(const std::exception& ex)
	{
		epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize, 
			"%s:%s: status=%d, function=%d, name=%s, value=%s, error=%s", 
			driverName, functionName, status, function, paramName, value_s.c_str(), ex.what());
		*nActual = 0;
		return asynError;
	}
}

asynStatus ECLabDriver::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	const char *paramName = NULL;
	getParamName(function, &paramName);
	const char* functionName = "writeFloat64";
	int addr = 0; // channel number
	getAddress(pasynUser, &addr);
	try
	{
		setECSingleParam(this, addr, function, value);  // need to include addr at some point
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
			"%s:%s: function=%d, name=%s, value=%f\n", 
			driverName, functionName, function, paramName, value);
		return asynSuccess;
		
	}
	catch(const std::exception& ex)
	{
		epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize, 
			"%s:%s: status=%d, function=%d, name=%s, value=%f, error=%s", 
			driverName, functionName, status, function, paramName, value, ex.what());
		return asynError;
	}
}

asynStatus ECLabDriver::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	const char *paramName = NULL;
	getParamName(function, &paramName);
	const char* functionName = "writeInt32";
	int addr = 0; // channel number
	getAddress(pasynUser, &addr);

	try
	{
		if (function == P_startChannel)
        {
			 ECLabInterface::StartChannel(m_ID, addr);
        }
		else if (function == P_stopChannel)
        {
			 ECLabInterface::StopChannel(m_ID, addr);
		}
		else if (function == P_updateParams)
		{
			TEccParams_t params; 
			for (int i = 0; i < m_techniques.size(); ++i)
			{
				std::vector<TEccParam_t> values;
				getTechniqueParams(m_techniques[i], addr, values, true);
				int maxupdate = 10; // can only update a maximum of 10 parameters at a time with ECLabInterface::UpdateParameters
				for(int j=0; j<values.size(); j += maxupdate)
				{
					params.pParams = &(values[j]);
					int n = values.size() - j;
					params.len = (n > maxupdate ? maxupdate : n);
				    ECLabInterface::UpdateParameters(m_ID, addr, i, params, const_cast<char*>((m_techniques[i]+".ecc").c_str()));
				}
			}
		}
		else
		{
		    setECIntegerParam(this, addr, function, value);  // need to include addr at some point
		}
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
			"%s:%s: function=%d, name=%s, value=%d\n", 
			driverName, functionName, function, paramName, value);
		return asynSuccess;
		
	}
	catch(const std::exception& ex)
	{
		epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize, 
			"%s:%s: status=%d, function=%d, name=%s, value=%d, error=%s", 
			driverName, functionName, status, function, paramName, value, ex.what());
		return asynError;
	}
}

/// EPICS driver report function for iocsh dbior command
void ECLabDriver::report(FILE* fp, int details)
{
	fprintf(fp, "ECLabDriver report\n");
	std::ostringstream oss;
	printParams(oss);
	fwrite(oss.str().c_str(), 1, oss.str().size(), fp); 
	asynPortDriver::report(fp, details);
}


/// Constructor for the ECLabDriver class.
/// Calls constructor for the asynPortDriver base class and sets up driver parameters.
///
/// \param[in] portName @copydoc initArg0
ECLabDriver::ECLabDriver(const char *portName, const char *ip) 
	: asynPortDriver(portName, 
	16, /* maxAddr */ 
	NUM_ECLAB_PARAMS + 50,
	asynInt32Mask | asynInt32ArrayMask | asynInt8ArrayMask | asynFloat64Mask | asynFloat32ArrayMask | asynOctetMask | asynDrvUserMask, /* Interface mask */
	asynInt32Mask | asynInt32ArrayMask | asynInt8ArrayMask | asynFloat64Mask | asynFloat32ArrayMask | asynOctetMask,  /* Interrupt mask */
	ASYN_CANBLOCK, /* asynFlags.  This driver can block but it is not multi-device */
	1, /* Autoconnect */
	0, /* Default priority */
	0)	/* Default stack size*/
	
{
	const char *functionName = "ECLabDriver";
	
	createParam(P_versionString, asynParamOctet, &P_version);
	createParam(P_hostString, asynParamOctet, &P_host);
	createParam(P_devCodeString,asynParamInt32, &P_devCode);
	createParam(P_numChannelsString, asynParamInt32, &P_numChannels);
	createParam(P_numSlotsString, asynParamInt32, &P_numSlots);
	createParam(P_numTechString, asynParamInt32, &P_numTech);
	createParam(P_memFilledString, asynParamInt32, &P_memFilled);
	createParam(P_currEWEString, asynParamFloat64, &P_currEWE);
	createParam(P_currECEString, asynParamFloat64, &P_currECE);
	createParam(P_currIString, asynParamFloat64, &P_currI);
	createParam(P_currTIMEString, asynParamFloat64, &P_currTIME);
	createParam(P_currRCOMPString, asynParamFloat64, &P_currRCOMP);
	createParam(P_currFREQString, asynParamFloat64, &P_currFREQ);
	createParam(P_currSTATEString, asynParamInt32, &P_currSTATE);
	createParam(P_loadTechString, asynParamOctet, &P_loadTech);
	createParam(P_updateParamsString, asynParamInt32, &P_updateParams);
	createParam(P_startChannelString, asynParamInt32, &P_startChannel);
	createParam(P_stopChannelString, asynParamInt32, &P_stopChannel);
	
    setStringParam(P_version, "unknown");
	setIntegerParam(P_devCode, KBIO_DEV_UNKNOWN);
	setIntegerParam(P_numChannels, 1);
	
	addAllParameters(this);
	
	// Create the thread for background tasks (not used at present, could be used for I/O intr scanning) 
	if (epicsThreadCreate("ECLabDriverTask",
		epicsThreadPriorityMedium,
		epicsThreadGetStackSize(epicsThreadStackMedium),
		(EPICSTHREADFUNC)ECLabTaskC, this) == 0)
	{
		printf("%s:%s: epicsThreadCreate failure\n", driverName, functionName);
		return;
	}
	char version[32];
	unsigned ver_size = sizeof(version);
	ECLabInterface::GetLibVersion(version, &ver_size);
	setStringParam(P_version, version);
	setStringParam(P_host, ip);
	unsigned int timeout = 5;
	ECLabInterface::Connect(const_cast<char *>(ip), timeout, &m_ID, &m_infos);
	setIntegerParam(P_devCode, m_infos.DeviceCode);
	setIntegerParam(P_numChannels, m_infos.NumberOfChannels);
	setIntegerParam(P_numSlots, m_infos.NumberOfSlots);
	ECLabInterface::testConnect(m_ID);
}

/// @todo Might use this for background polling if implementing I/O Intr scanning
void ECLabDriver::ECLabTaskC(void* arg) 
{ 
	ECLabDriver* driver = reinterpret_cast<ECLabDriver*>(arg);
    if (NULL != driver)
    {
        driver->ECLabTask();
    }
}

void ECLabDriver::ECLabTask() 
{
    TCurrentValues_t cvals;
	TChannelInfos_t cinfo;
    while(true)
    {
	    for(int i=0; i<m_infos.NumberOfChannels; ++i)
		{
		    if (ECLabInterface::IsChannelPlugged(m_ID, i))
			{
			    ECLabInterface::GetCurrentValues(m_ID, i, &cvals);
				ECLabInterface::GetChannelInfos(m_ID, i, &cinfo);
				lock();
				setIntegerParam(i, P_memFilled, cvals.MemFilled);
				setDoubleParam(i, P_currEWE, cvals.Ewe);
				setDoubleParam(i, P_currECE, cvals.Ece);
				setDoubleParam(i, P_currI, cvals.I);
				setDoubleParam(i, P_currTIME, cvals.ElapsedTime);
				setDoubleParam(i, P_currRCOMP, cvals.Rcomp);
				setDoubleParam(i, P_currFREQ, cvals.Freq);
				setIntegerParam(i, P_currSTATE, cvals.State);
				setIntegerParam(i, P_numTech, cinfo.NbOfTechniques);
                callParamCallbacks();
		        unlock();
			}
		}
        epicsThreadSleep(1.0);        
    }
}

extern "C" {

	/// The function is registered via EClabRegister().
	///
	/// @param[in] portName @copydoc initArg0
	int ECLabConfigure(const char *portName, const char *ip)
	{
		try
		{
			new ECLabDriver(portName, ip);
			return(asynSuccess);
		}
		catch(const std::exception& ex)
		{
			errlogSevPrintf(errlogFatal, "ECLabConfigure failed: %s\n", ex.what());
			return(asynError);
		}
	}

	// EPICS iocsh shell commands 

	static const iocshArg initArg0 = { "portName", iocshArgString};			///< A name for the asyn driver instance we will create - used to refer to it from EPICS DB files
	static const iocshArg initArg1 = { "ip", iocshArgString};			///< IP address used
	static const iocshArg * const initArgs[] = { &initArg0, &initArg1 };

	static const iocshFuncDef initFuncDef = {"ECLabConfigure", sizeof(initArgs) / sizeof(iocshArg*), initArgs};

	static void initCallFunc(const iocshArgBuf *args)
	{
		ECLabConfigure(args[0].sval, args[1].sval);
	}
	
	/// Register new commands with EPICS IOC shell
	static void ECLabRegister(void)
	{
		iocshRegister(&initFuncDef, initCallFunc);
	}

	epicsExportRegistrar(ECLabRegister);

}
