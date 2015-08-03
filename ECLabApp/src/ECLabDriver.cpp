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

asynStatus ECLabDriver::readInt32(asynUser *pasynUser, epicsInt32 *value)
{
    const char* functionName = "readInt32";
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	const char *paramName = NULL;
	getParamName(function, &paramName);
	try
	{
		if (function == P_devCode)
        {
             *value = m_infos.DeviceCode; 
			 printf("%d\n", *value);
        }
		else if (function == P_firmwareVers)
        {
             *value = m_infos.FirmwareVersion; 
			 printf("%d\n", *value);
        }
		else
		{
			asynPortDriver::readInt32(pasynUser, value);
			printf("%d\n", *value);
		}
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
			"%s:%s: function=%d, name=%s, value=%d\n", 
			driverName, functionName, function, paramName, *value);
		return asynSuccess;
	}
	catch(const std::exception& ex)
	{
		epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize, 
			"%s:%s: status=%d, function=%d, name=%s, value=%d, error=%s", 
			driverName, functionName, status, function, paramName, *value, ex.what());
		return asynError;
	}
}

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
        if (function == P_version)
        {
             char version[32], mesg[255];
	         unsigned ver_size = sizeof(version);
	         ECLabInterface::GetLibVersion(version, &ver_size);
			 
			 printf("%d\n", ver_size);
			 printf(version);
             value_s = version;
        }
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
	try
	{
	    if (function == P_loadTech)
		{
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


asynStatus ECLabDriver::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	const char *paramName = NULL;
	getParamName(function, &paramName);
	const char* functionName = "writeInt32";

	try
	{
		setECIntegerParam(this, function, value);
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
			"%s:%s: function=%d, name=%s, value=%d\n", 
			driverName, functionName, function, paramName, value);
		return asynSuccess;
	}
	catch(const std::exception& ex)
	{
		epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize, 
			"%s:%s: status=%d, function=%d, name=%s, value=%s, error=%s", 
			driverName, functionName, status, function, paramName);
		return asynError;
	}
}

/// EPICS driver report function for iocsh dbior command
void ECLabDriver::report(FILE* fp, int details)
{
	fprintf(fp, "ECLabDriver report\n");
	asynPortDriver::report(fp, details);
}


/// Constructor for the ECLabDriver class.
/// Calls constructor for the asynPortDriver base class and sets up driver parameters.
///
/// \param[in] portName @copydoc initArg0
ECLabDriver::ECLabDriver(const char *portName, const char *ip) 
	: asynPortDriver(portName, 
	0, /* maxAddr */ 
	NUM_ECLAB_PARAMS + 50,
	asynInt32Mask | asynInt32ArrayMask | asynInt8ArrayMask | asynFloat64Mask | asynFloat32ArrayMask | asynOctetMask | asynDrvUserMask, /* Interface mask */
	asynInt32Mask | asynInt32ArrayMask | asynInt8ArrayMask | asynFloat64Mask | asynFloat32ArrayMask | asynOctetMask,  /* Interrupt mask */
	ASYN_CANBLOCK, /* asynFlags.  This driver can block but it is not multi-device */
	1, /* Autoconnect */
	0, /* Default priority */
	0)	/* Default stack size*/
	
{
	int i;
	const char *functionName = "ECLabDriver";
	int ID;
	
	unsigned int timeout = 5;
	createParam(P_versionString, asynParamOctet, &P_version);
	createParam(P_devCodeString,asynParamInt32, &P_devCode);
	createParam(P_firmwareVersString,asynParamInt32, &P_firmwareVers);
	createParam(P_numChannelsString, asynParamInt32, &P_numChannels);
	createParam(P_loadTechString, asynParamOctet, &P_loadTech);

    setStringParam(P_version, "unknown");
	setIntegerParam(P_devCode, KBIO_DEV_UNKNOWN);
	setIntegerParam(P_numChannels, 0);
	
	addAllParameters(this);
	
//	g_map = globalMapGen();

	// Create the thread for background tasks (not used at present, could be used for I/O intr scanning) 
	if (epicsThreadCreate("ECLabDriverTask",
		epicsThreadPriorityMedium,
		epicsThreadGetStackSize(epicsThreadStackMedium),
		(EPICSTHREADFUNC)ECLabTaskC, this) == 0)
	{
		printf("%s:%s: epicsThreadCreate failure\n", driverName, functionName);
		return;
	}
	char version[32], mesg[255];
	unsigned ver_size = sizeof(version);
	ECLabInterface::GetLibVersion(version, &ver_size);
	setStringParam(P_version, version);
	ECLabInterface::Connect(const_cast<char *>(ip), timeout, &ID, &m_infos);
	setIntegerParam(P_devCode, m_infos.DeviceCode);
	setIntegerParam(P_firmwareVers, m_infos.FirmwareVersion);
	setIntegerParam(P_numChannels, m_infos.NumberOfChannels);
	ECLabInterface::testConnect(ID);
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
    while(true)
    {
	    lock();
		setIntegerParam(P_numChannels, m_infos.NumberOfChannels);
        callParamCallbacks();
		unlock();
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
