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
        if (function == P_par2)
        {
             char version[32], mesg[255];
	         unsigned ver_size = sizeof(version);
	         ECLabInterface::GetLibVersion(version, &ver_size);
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
ECLabDriver::ECLabDriver(const char *portName) 
	: asynPortDriver(portName, 
	0, /* maxAddr */ 
	NUM_ECLAB_PARAMS,
	asynInt32Mask | asynInt32ArrayMask | asynFloat64Mask | asynFloat64ArrayMask | asynOctetMask | asynDrvUserMask, /* Interface mask */
	asynInt32Mask | asynInt32ArrayMask | asynFloat64Mask | asynFloat64ArrayMask | asynOctetMask,  /* Interrupt mask */
	ASYN_CANBLOCK, /* asynFlags.  This driver can block but it is not multi-device */
	1, /* Autoconnect */
	0, /* Default priority */
	0)	/* Default stack size*/
	
{
	int i;
	const char *functionName = "ECLabDriver";
	createParam(P_par1String, asynParamFloat64, &P_par1);
	createParam(P_par2String, asynParamOctet, &P_par2);
    
    setDoubleParam(P_par1, 1.0);
    setStringParam(P_par2, "unknown");


	// Create the thread for background tasks (not used at present, could be used for I/O intr scanning) 
	if (epicsThreadCreate("ECLabDriverTask",
		epicsThreadPriorityMedium,
		epicsThreadGetStackSize(epicsThreadStackMedium),
		(EPICSTHREADFUNC)ECLabTaskC, this) == 0)
	{
		printf("%s:%s: epicsThreadCreate failure\n", driverName, functionName);
		return;
	}
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
    double val = 10.0;
    while(true)
    {
        setDoubleParam(P_par1, val);
        callParamCallbacks();
        val += 1.0;
        epicsThreadSleep(1.0);        
    }
}

extern "C" {

	/// The function is registered via EClabRegister().
	///
	/// @param[in] portName @copydoc initArg0
	int ECLabConfigure(const char *portName)
	{
		try
		{
			new ECLabDriver(portName);
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

	static const iocshArg * const initArgs[] = { &initArg0 };

	static const iocshFuncDef initFuncDef = {"ECLabConfigure", sizeof(initArgs) / sizeof(iocshArg*), initArgs};

	static void initCallFunc(const iocshArgBuf *args)
	{
		ECLabConfigure(args[0].sval);
	}
	
	/// Register new commands with EPICS IOC shell
	static void ECLabRegister(void)
	{
		iocshRegister(&initFuncDef, initCallFunc);
	}

	epicsExportRegistrar(ECLabRegister);

}
