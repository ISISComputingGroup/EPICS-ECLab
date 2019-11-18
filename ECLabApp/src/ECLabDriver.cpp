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
#include <fstream>
#include <algorithm>

#include <epicsTypes.h>
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsString.h>
#include <epicsTimer.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <errlog.h>
#include <iocsh.h>
#include <macLib.h>

#include "BLFunctions.h"
#include "ECLabInterface.h"
#include "ECLabDriver.h"
#include "ECLabParams.h"
#include "ECLabCodeLookup.h"

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
	static const std::string ecc_dir = macEnvExpand("$(ECLAB)/ecc/");
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	const char *paramName = NULL;
	getParamName(function, &paramName);
	const char* functionName = "writeOctet";
	std::string value_s(value, maxChars);
	int addr = 0;
	getAddress(pasynUser, &addr);
	try
	{
		if (function == P_loadTech || function == P_defineTech) // addr is channel number
		{
			char* valueCopy = strdup(value_s.c_str()); // as strtok modfies the string
			char *tok_save = NULL;
			char * tech = epicsStrtok_r(valueCopy, " ,;", &tok_save); //epics vers of strtok
			m_techniques.clear();
			std::map<std::string,int> tech_count;
			while (tech != NULL)
			{
			    if (tech_count.find(tech) == tech_count.end())
				{
				    tech_count[tech] = 0;
				}
			    m_techniques.push_back(techinfo(tech, tech_count[tech]));
				++tech_count[tech];
				tech = epicsStrtok_r(NULL, " ,;", &tok_save); 				
			}
			free(valueCopy);	
		}
		if (function == P_loadTech)
		{
			TEccParams_t params; 
			for (int i = 0; i < m_techniques.size(); ++i)
			{
			    bool first = ( i == 0 );
				bool last = ( i == (m_techniques.size() - 1) );
				std::vector<TEccParam_t> values;
				getTechniqueParams(m_techniques[i].name, m_techniques[i].index, values, false);
				if (values.size() > 0)
				{
				    params.pParams = &(values[0]);
				}
				else
				{
					params.pParams = NULL;
				}
				params.len = values.size();
				std::string ecc_file = ecc_dir + m_techniques[i].name + "4.ecc";
				std::replace(ecc_file.begin(), ecc_file.end(), '/', '\\');
				ECLabInterface::LoadTechnique(m_ID, addr, const_cast<char*>(ecc_file.c_str()), params, first, last, false);
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
	int addr = 0; // channel number, but for params is asyn addr
	getAddress(pasynUser, &addr);
	try
	{
		setECSingleParam(this, addr, function, value);  // need to include channel at some point
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
	static const std::string ecc_dir = macEnvExpand("$(ECLAB)/ecc/");
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	const char *paramName = NULL;
	getParamName(function, &paramName);
	const char* functionName = "writeInt32";
	int addr = 0; // channel number, but for params is asyn addr
	getAddress(pasynUser, &addr);

	try
	{
		if (function == P_startChannel)
        {
			epicsTimeGetCurrent(&(m_start_time[addr]));
			ECLabInterface::StartChannel(m_ID, addr);
        }
		else if (function == P_stopChannel)
        {
			ECLabInterface::StopChannel(m_ID, addr);
			if (m_generalLog[addr].is_open())
			{
				m_generalLog[addr].close();
			}
//			TExperimentInfos_t TExpInfos;
//			ECLabInterface::GetExperimentInfos(m_ID, addr, &TExpInfos );
//			std::cerr << "Exp info time " << TExpInfos.TimeHMS << " filename " << TExpInfos.Filename << std::endl;
		}
		else if (function == P_updateParams)
		{
			TEccParams_t params; 
			for (int i = 0; i < m_techniques.size(); ++i)
			{
				std::vector<TEccParam_t> values;
				getTechniqueParams(m_techniques[i].name, m_techniques[i].index, values, true);
				int maxupdate = 10; // can only update a maximum of 10 parameters at a time with ECLabInterface::UpdateParameters
				for(int j=0; j<values.size(); j += maxupdate)
				{
					params.pParams = &(values[j]);
					int n = values.size() - j;
					params.len = (n > maxupdate ? maxupdate : n);
					std::string ecc_file = ecc_dir + m_techniques[i].name + "4.ecc";
					std::replace(ecc_file.begin(), ecc_file.end(), '/', '\\');
				    ECLabInterface::UpdateParameters(m_ID, addr, i, params, const_cast<char*>(ecc_file.c_str()));
				}
			}
		}
		else if (function == P_HWConn)
		{
			ECLabInterface::GetHardConf(m_ID, addr, &m_hwconf);
			m_hwconf.Conn = value;
			ECLabInterface::SetHardConf(m_ID, addr, m_hwconf);
			setIntegerParam(addr, P_HWConn, value);	
		}
		else if (function == P_HWGround)
		{
			ECLabInterface::GetHardConf(m_ID, addr, &m_hwconf);
			m_hwconf.Ground = value;
			ECLabInterface::SetHardConf(m_ID, addr, m_hwconf);
			setIntegerParam(addr, P_HWGround, value);	
		}
		else if (function == P_saveData)
		{
			setIntegerParam(P_saveData, value);	
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

asynStatus ECLabDriver::writeFloat32Array(asynUser *pasynUser, epicsFloat32 *value, size_t nElements)
{
	int function = pasynUser->reason;
	int addr = 0;
	getAddress(pasynUser, &addr);
	setECSingleArrayParam(this, addr, function, value, nElements);
	return asynSuccess;
}

asynStatus ECLabDriver::writeInt32Array(asynUser *pasynUser, epicsInt32 *value, size_t nElements)
{
	int function = pasynUser->reason;
	int addr = 0;
	getAddress(pasynUser, &addr);
	setECIntegerArrayParam(this, addr, function, value, nElements);
	return asynSuccess;
}

asynStatus ECLabDriver::writeInt8Array(asynUser *pasynUser, epicsInt8 *value, size_t nElements)
{
	int function = pasynUser->reason;
	int addr = 0;
	getAddress(pasynUser, &addr);
	setECBooleanArrayParam(this, addr, function, value, nElements);
	return asynSuccess;
}

void ECLabDriver::printIntParam(std::ostream& os, const char* desc, int param)
{
	int value;
	getIntegerParam(param, &value);
	os << desc << "=" << value << std::endl;
}

void ECLabDriver::printDoubleParam(std::ostream& os, const char* desc, int param)
{
	double value;
	getDoubleParam(param, &value);
	os << desc << "=" << value << std::endl;
}

/// EPICS driver report function for iocsh dbior command
void ECLabDriver::report(FILE* fp, int details)
{
	fprintf(fp, "ECLabDriver report\n");
	std::ostringstream oss;

	printIntParam(oss, "Number of techniques", P_numTech);
	printIntParam(oss, "Mem filled", P_memFilled);
	printIntParam(oss, "Mem size", P_memSize);
	printDoubleParam(oss, "Mem percent filled", P_memPercentFilled);
	printDoubleParam(oss, "TIME", P_currTIME);
	printDoubleParam(oss, "EWE", P_currEWE);
	printDoubleParam(oss, "ECE", P_currECE);
	printDoubleParam(oss, "I", P_currI);
	printDoubleParam(oss, "RCOMP", P_currRCOMP);
	printDoubleParam(oss, "FREQ", P_currFREQ);
	printIntParam(oss, "State", P_currSTATE);
	printDoubleParam(oss, "TIMEBASE", P_currTimeBase);

	printParams(this, oss);

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
	NUM_ECLAB_PARAMS + 100,
	asynInt32Mask | asynInt32ArrayMask | asynInt8ArrayMask | asynFloat64Mask | asynFloat32ArrayMask | asynOctetMask | asynDrvUserMask, /* Interface mask */
	asynInt32Mask | asynInt32ArrayMask | asynInt8ArrayMask | asynFloat64Mask | asynFloat32ArrayMask | asynOctetMask,  /* Interrupt mask */
	ASYN_CANBLOCK, /* asynFlags.  This driver can block but it is not multi-device */
	1, /* Autoconnect */
	0, /* Default priority */
	0)	/* Default stack size*/
	
{
	const char *functionName = "ECLabDriver";
	static const std::string ecc_dir = macEnvExpand("$(ECLAB)/ecc/");
	
	createParam(P_versionString, asynParamOctet, &P_version);
	createParam(P_hostString, asynParamOctet, &P_host);
	createParam(P_devCodeString,asynParamInt32, &P_devCode);
	createParam(P_numChannelsString, asynParamInt32, &P_numChannels);
	createParam(P_numSlotsString, asynParamInt32, &P_numSlots);
	createParam(P_numTechString, asynParamInt32, &P_numTech);
	createParam(P_memFilledString, asynParamInt32, &P_memFilled);
	createParam(P_memSizeString, asynParamInt32, &P_memSize);
	createParam(P_memPercentFilledString, asynParamFloat64, &P_memPercentFilled);
	createParam(P_currEWEString, asynParamFloat64, &P_currEWE);
	createParam(P_currECEString, asynParamFloat64, &P_currECE);
	createParam(P_currIString, asynParamFloat64, &P_currI);
	createParam(P_currTIMEString, asynParamFloat64, &P_currTIME);
	createParam(P_currRCOMPString, asynParamFloat64, &P_currRCOMP);
	createParam(P_currFREQString, asynParamFloat64, &P_currFREQ);
	createParam(P_currSTATEString, asynParamInt32, &P_currSTATE);
	createParam(P_currTimeBaseString, asynParamFloat64, &P_currTimeBase);
	createParam(P_loadTechString, asynParamOctet, &P_loadTech);
	createParam(P_defineTechString, asynParamOctet, &P_defineTech);
	createParam(P_updateParamsString, asynParamInt32, &P_updateParams);
	createParam(P_startChannelString, asynParamInt32, &P_startChannel);
	createParam(P_stopChannelString, asynParamInt32, &P_stopChannel);
	createParam(P_dataDoneString, asynParamInt32, &P_dataDone);
	createParam(P_filePrefixString, asynParamOctet, &P_filePrefix);
	createParam(P_fileNameString, asynParamOctet, &P_fileName);
	createParam(P_saveDataString, asynParamInt32, &P_saveData);
	
	createParam(P_currEWERangeMinString, asynParamFloat64, &P_currEWERangeMin);
	createParam(P_currEWERangeMaxString, asynParamFloat64, &P_currEWERangeMax);
	createParam(P_currECERangeMinString, asynParamFloat64, &P_currECERangeMin);
	createParam(P_currECERangeMaxString, asynParamFloat64, &P_currECERangeMax);
	createParam(P_currEOverflowString, asynParamInt32, &P_currEOverflow);
	createParam(P_currIOverflowString, asynParamInt32, &P_currIOverflow);
	createParam(P_currIRangeString, asynParamInt32, &P_currIRange);
	createParam(P_currSaturationString, asynParamInt32, &P_currSaturation);
	createParam(P_currOptErrString, asynParamInt32, &P_currOptErr);
	createParam(P_currOptPosString, asynParamInt32, &P_currOptPos);
	
	createParam(P_dataTechIndString, asynParamInt32, &P_dataTechInd);
	createParam(P_dataTechIDString, asynParamInt32, &P_dataTechID);
	createParam(P_dataLoopString, asynParamInt32, &P_dataLoop);
	createParam(P_dataIRQSkippedString, asynParamInt32, &P_dataIRQSkipped);
	createParam(P_dataStartTimeString, asynParamFloat64, &P_dataStartTime);

	createParam(P_HWConnString, asynParamInt32, &P_HWConn);
	createParam(P_HWGroundString, asynParamInt32, &P_HWGround);

    setStringParam(P_version, "unknown");
    setStringParam(P_host, "unknown");
	setIntegerParam(P_devCode, KBIO_DEV_UNKNOWN);
	setIntegerParam(P_numChannels, 0);
	setIntegerParam(P_numSlots, 0);
    setStringParam(P_filePrefix, "eclab");
	setIntegerParam(P_saveData, 1);
	
	addAllParameters(this);
	
	if ( !strcmp(ip, "SIM") )
	{
	    ECLabInterface::BLSIM = true;
		std::cerr << "Enabling simulation mode" << std::endl;
	}
	
	char version[32];
	unsigned ver_size = sizeof(version);
	ECLabInterface::GetLibVersion(version, &ver_size);
	setStringParam(P_version, version);
	std::cerr << "ECLab software library version: " << version << std::endl;
	unsigned int timeout = 5;
	std::cerr << "Connecting to " << ip << std::endl;
	ECLabInterface::Connect(const_cast<char *>(ip), timeout, &m_ID, &m_infos);
	ECLabInterface::testConnect(m_ID);
	std::cerr << "Connected to device code " << m_infos.DeviceCode << " which has " << m_infos.RAMSize << " MBytes RAM" << std::endl;
	std::cerr << "Device firmware version: " << m_infos.FirmwareVersion << " (" << m_infos.FirmwareDate_yyyy << "/" << m_infos.FirmwareDate_mm << "/" << m_infos.FirmwareDate_dd << ")" << std::endl;

    // loads firmware on first channel (first array element = 1)
	uint8 chans[] = { 1 };
	int res[] = { 0 };
	std::string kernel_file = ecc_dir + "kernel4.bin";
	std::replace(kernel_file.begin(), kernel_file.end(), '/', '\\');
	std::string xlx_file = ecc_dir + "Vmp_iv_0395_aa.xlx";
	std::replace(xlx_file.begin(), xlx_file.end(), '/', '\\');
	ECLabInterface::LoadFirmware(m_ID, chans, res, 1, 0, 0, kernel_file.c_str(), xlx_file.c_str());
	if (res[0] < 0)
	{
		std::cerr << "Error loading firmware on channel 0: " << res[0] << std::endl;
	}
	TChannelInfos_t cinfo;
	ECLabInterface::GetChannelInfos(m_ID, 0, &cinfo);
	std::cerr << "Board version: " << cinfo.BoardVersion << " Board serial number: " <<  cinfo.BoardSerialNumber << " FirmwareCode: \"" 
	<< firmwareCodeLookup(cinfo.FirmwareCode) << "\" Firmware version: " << cinfo.FirmwareVersion << " xilinx version: " << cinfo.XilinxVersion << std::endl;
	
	setStringParam(P_host, ip);
	setIntegerParam(P_devCode, m_infos.DeviceCode);
	setIntegerParam(P_numChannels, m_infos.NumberOfChannels);
	setIntegerParam(P_numSlots, m_infos.NumberOfSlots);
	
	ECLabInterface::GetHardConf(m_ID, 0, &m_hwconf);
	setIntegerParam(0, P_HWConn, m_hwconf.Conn);
	setIntegerParam(0, P_HWGround, m_hwconf.Ground);
	
	epicsTimeGetCurrent(&(m_start_time[0])); // initialise to sensible value, need to do other channels

	// Create the thread for background tasks (not used at present, could be used for I/O intr scanning) 
	if (epicsThreadCreate("ECLabValuesTask",
		epicsThreadPriorityMedium,
		epicsThreadGetStackSize(epicsThreadStackMedium),
		(EPICSTHREADFUNC)ECLabValuesTaskC, this) == 0)
	{
		printf("%s:%s: epicsThreadCreate failure\n", driverName, functionName);
		return;
	}
	if (epicsThreadCreate("ECLabDataTask",
		epicsThreadPriorityMedium,
		epicsThreadGetStackSize(epicsThreadStackMedium),
		(EPICSTHREADFUNC)ECLabDataTaskC, this) == 0)
	{
		printf("%s:%s: epicsThreadCreate failure\n", driverName, functionName);
		return;
	}
}

/// @todo Might use this for background polling if implementing I/O Intr scanning
void ECLabDriver::ECLabValuesTaskC(void* arg) 
{ 
	ECLabDriver* driver = reinterpret_cast<ECLabDriver*>(arg);
    if (NULL != driver)
    {
        driver->ECLabValuesTask();
    }
}

void ECLabDriver::ECLabDataTaskC(void* arg) 
{ 
	ECLabDriver* driver = reinterpret_cast<ECLabDriver*>(arg);
    if (NULL != driver)
    {
        driver->ECLabDataTask();
    }
}

void ECLabDriver::updateCvals(int chan, TCurrentValues_t& cvals)
{
	static int file_counter = 0;
	setIntegerParam(chan, P_memFilled, cvals.MemFilled);
	setDoubleParam(chan, P_currEWE, cvals.Ewe);
	setDoubleParam(chan, P_currECE, cvals.Ece);
	setDoubleParam(chan, P_currI, cvals.I);
	setDoubleParam(chan, P_currTIME, cvals.ElapsedTime);
	setDoubleParam(chan, P_currRCOMP, cvals.Rcomp);
	setDoubleParam(chan, P_currFREQ, cvals.Freq);
	setIntegerParam(chan, P_currSTATE, cvals.State);
	setDoubleParam(chan, P_currTimeBase, cvals.TimeBase);

	setDoubleParam(chan, P_currEWERangeMin, cvals.EweRangeMin);
	setDoubleParam(chan, P_currEWERangeMax, cvals.EweRangeMax);
	setDoubleParam(chan, P_currECERangeMin, cvals.EceRangeMin);
	setDoubleParam(chan, P_currECERangeMax, cvals.EceRangeMax);
	
	setIntegerParam(chan, P_currEOverflow, cvals.Eoverflow);
	setIntegerParam(chan, P_currIOverflow, cvals.Ioverflow);
	setIntegerParam(chan, P_currIRange, cvals.IRange);
	setIntegerParam(chan, P_currSaturation, cvals.Saturation);
	setIntegerParam(chan, P_currOptErr, cvals.OptErr);
	setIntegerParam(chan, P_currOptPos, cvals.OptPos);

	if (cvals.State == KBIO_STATE_STOP && cvals.MemFilled == 0 && m_generalLog[chan].is_open())
	{
		m_generalLog[chan].close();
		++file_counter;
	}	
	if (cvals.State != KBIO_STATE_STOP && !m_generalLog[chan].is_open())
	{
		char filename[256], fileprefix[256], tbuff[64];
		getStringParam(chan, P_filePrefix, sizeof(fileprefix), fileprefix);
		epicsTimeToStrftime(tbuff, sizeof(tbuff), "%Y%m%dT%H%M%S", &(m_start_time[chan]));
		sprintf(filename, "%s_%s_C%d_%d.csv", fileprefix, tbuff, chan, file_counter);
		m_generalLog[chan].open(filename, std::ios::out);					
		m_generalLog[chan] << "AbsTime1,AbsTime2,ElapsedTime,Ewe,Ece,I,MemFilled,Rcomp,Freq,State,TimeBase,EweRangeMin,EweRangeMax,EceRangeMin,EceRangeMax,Eoverflow,Ioverflow,IRange ,Saturation,OptErr,OptPos\n";
    }
	if (m_generalLog[chan].is_open())
	{
	    epicsTimeStamp ts;
	    epicsTimeGetCurrent(&ts);
		m_generalLog[chan] << getAbsTime(ts, 0.0) << "," <<  getAbsTime(m_start_time[chan], cvals.ElapsedTime) << "," << cvals.ElapsedTime << "," << cvals.Ewe << "," << cvals.Ece << "," << cvals.I << "," << cvals.MemFilled << "," << cvals.Rcomp << "," << cvals.Freq << "," << cvals.State << "," << cvals.TimeBase << "," << cvals.EweRangeMin << "," << cvals.EweRangeMax << "," << cvals.EceRangeMin << "," << cvals.EceRangeMax << "," << cvals.Eoverflow << "," << cvals.Ioverflow << "," << cvals.IRange << "," << cvals.Saturation << "," << cvals.OptErr << "," << cvals.OptPos << "\n"; 
	}
}

void ECLabDriver::ECLabValuesTask() 
{
    TCurrentValues_t cvals;
	TChannelInfos_t cinfo;
//	uint32_t len_buffer;
//	char buffer[256];
    while(true)
    {
	    for(int i=0; i<m_infos.NumberOfChannels; ++i)
		{
		    if (ECLabInterface::IsChannelPlugged(m_ID, i))
			{
				try
				{
					ECLabInterface::GetChannelInfos(m_ID, i, &cinfo);
			        ECLabInterface::GetCurrentValues(m_ID, i, &cvals);
				}
				catch(const std::exception& ex)
				{
					errlogSevPrintf(errlogInfo, "%s", ex.what());
				}
//				len_buffer = sizeof(buffer);
//				memset(buffer, 0, len_buffer);
//				ECLabInterface::GetMessage(m_ID, i, buffer, &len_buffer);				
				lock();
				updateCvals(i, cvals);
				setIntegerParam(i, P_numTech, cinfo.NbOfTechniques);
				setIntegerParam(i, P_memSize, cinfo.MemSize);
				setDoubleParam(i, P_memPercentFilled, 100.0 * static_cast<double>(cinfo.MemFilled) / static_cast<double>(cinfo.MemSize));
				
//				if (len_buffer > 0)
//				{
//				    std::cerr << buffer << std::endl;
//				}
                callParamCallbacks();
		        unlock();
			}
		}
        epicsThreadSleep(1.0);        
    }
}

double ECLabDriver::getTime(unsigned thigh, unsigned tlow, double start_time, double time_base)
{
    return (((__int64)thigh << 32) + tlow) * time_base + start_time;
}

std::string ECLabDriver::getAbsTime(epicsTimeStamp& base, double offset)
{
	char tbuff[64];
	epicsTime abs_time(base);
	abs_time += offset;
	abs_time.strftime(tbuff, sizeof(tbuff), "%Y-%m-%dT%H:%M:%S.%03f");
//	abs_time.strftime(tbuff, sizeof(tbuff), "%H:%M:%S.%03f");
	return static_cast<std::string>(tbuff);
}

static unsigned countBits(unsigned v)
{
	unsigned c;
    for (c = 0; v != 0; ++c)
	{
        v &= v - 1; // clear the least significant bit set
	}
	return c;
}

void ECLabDriver::processXCTRVals(std::fstream& fs, unsigned* row_data, unsigned xctr, int col_start, int ncols)
{
	int ret;
	float fval;
	if (xctr & 0x1) // ece
	{
	    ret = BL_ConvertNumericIntoSingle(row_data[col_start], &fval);
		fs << "," << fval;
		++col_start;
	}
	if (xctr & 0x20) // control
	{
	    ret = BL_ConvertNumericIntoSingle(row_data[col_start], &fval);
		fs << "," << fval;
		++col_start;
	}
	if (xctr & 0x40) // charge
	{
	    ret = BL_ConvertNumericIntoSingle(row_data[col_start], &fval);
		fs << "," << fval;
		++col_start;
	}
	if (xctr & 0x80) // irange
	{
		fs << "," << row_data[col_start];
		++col_start;
	}
}

void ECLabDriver::processXCTRHeader(std::fstream& fs, unsigned xctr)
{
	if (xctr & 0x1) // ece
	{
		fs << ",Ece";
	}
	if (xctr & 0x20) // control
	{
		fs << ",Control";
	}
	if (xctr & 0x40) // charge
	{
		fs << ",Q";
	}
	if (xctr & 0x80) // irange
	{
		fs << ",Irange";
	}
}

void ECLabDriver::processPEISData(std::fstream& fs0, std::fstream& fs1, epicsTimeStamp& chan_start_time, int nrows, int ncols, int technique_index, int process_index, 
                     int loop, double start_time, double time_base, TDataBuffer_t* dbuffer)
{
	unsigned* data = dbuffer->data;
	double t;
	float tf;
	int idx;
	float ewe, currI, freq, eweMod, currIMod, phaseZwe,eceMod, iceMod;
	float phaseZce, ece; 
	if (process_index == 0)
	{
	    if (ncols != 4)
		{
			std::cerr << "PEIS: incorrect number of columns in data for process 0" << std::endl;
			return;
		}
	    for(int i=0; i<nrows; ++i)
	    {
		    idx = i * ncols;
		    t = getTime(data[idx + 0], data[idx + 1], start_time, time_base);
			BL_ConvertNumericIntoSingle(data[idx + 2], &ewe);
			BL_ConvertNumericIntoSingle(data[idx + 3], &currI);
			fs0 << getAbsTime(chan_start_time, t) << "," << t << "," << loop << "," << ewe << "," << currI << "\n";
		}
	}
	else if (process_index == 1)
	{
	    if (ncols != 14)
		{
			std::cerr << "PEIS: incorrect number of columns in data for process 1" << std::endl;
			return;
		}
	    for(int i=0; i<nrows; ++i)
	    {
		    idx = i * ncols;
			BL_ConvertNumericIntoSingle(data[idx + 0], &freq);
			BL_ConvertNumericIntoSingle(data[idx + 1], &eweMod);
			BL_ConvertNumericIntoSingle(data[idx + 2], &currIMod);
			BL_ConvertNumericIntoSingle(data[idx + 3], &phaseZwe);
			BL_ConvertNumericIntoSingle(data[idx + 4], &ewe);
			BL_ConvertNumericIntoSingle(data[idx + 5], &currI);
			BL_ConvertNumericIntoSingle(data[idx + 7], &eceMod);
			BL_ConvertNumericIntoSingle(data[idx + 8], &iceMod);
			BL_ConvertNumericIntoSingle(data[idx + 9], &phaseZce);
			BL_ConvertNumericIntoSingle(data[idx + 10], &ece);
			BL_ConvertNumericIntoSingle(data[idx + 13], &tf);
			fs1 << getAbsTime(chan_start_time, tf) << "," << tf << "," << loop << "," << freq << "," << eweMod
                << "," << currIMod << "," << phaseZwe << "," << ewe	
                << "," << currI << "," << eceMod << "," << iceMod	
                << "," << phaseZce << "," << ece << std::endl;
		}		
	}
	else
	{
		std::cerr << "PEIS: incorrect process index" << std::endl;
		return;
	}
}

void ECLabDriver::processOCVData(std::fstream& fs, epicsTimeStamp& chan_start_time, int nrows, int ncols, int technique_index, int process_index, 
                     int loop, double start_time, double time_base, TDataBuffer_t* dbuffer, int xctr)
{
	unsigned* data = dbuffer->data;
	double t;
	int idx, ret;
	float ewe;
	if (m_techniques[technique_index].name != "ocv")
	{
		std::cerr << "OCV: not right technique" << std::endl;
		return;
	}
	int nxctr = countBits(xctr);
	if ( ncols != (3 + nxctr) )
	{
		std::cerr << "OCV: incorrect number of columns in data: " << ncols << " expected: " << 3 + nxctr << std::endl;
		return;
	}
	if (process_index != 0)
	{
		std::cerr << "OCV: incorrect process index" << std::endl;
		return;
	}
	for(int i=0; i<nrows; ++i)
	{
		idx = i * ncols;
		t = getTime(data[idx + 0], data[idx + 1], start_time, time_base);
		ret = BL_ConvertNumericIntoSingle(data[idx + 2], &ewe);
		fs << getAbsTime(chan_start_time, t) << "," << t << "," << loop << "," << ewe;
		processXCTRVals(fs, &(data[idx]), xctr, 3, ncols);
		fs << "\n";
	}
}

void ECLabDriver::processCACPData(std::fstream& fs, epicsTimeStamp& chan_start_time, int nrows, int ncols, int technique_index, int process_index, 
                     int loop, double start_time, double time_base, TDataBuffer_t* dbuffer, int xctr)
{
	unsigned* data = dbuffer->data;
	double t;
	int idx, ret;
	float ewe, current;
	int cycle;	
	if (m_techniques[technique_index].name != "ca" && m_techniques[technique_index].name != "cp")
	{
		std::cerr << "CA/CP: not right technique" << std::endl;
		return;
	}		
	int nxctr = countBits(xctr);
	if ( ncols != (5 + nxctr) )
	{
		std::cerr << "CA/CP: incorrect number of columns in data: " << ncols << " expected: " << 5 + nxctr << std::endl;
		return;
	}
	if (process_index != 0)
	{
		std::cerr << "CA/CP: incorrect process index" << std::endl;
		return;
	}
	for(int i=0; i<nrows; ++i)
	{
		idx = i * ncols;
		t = getTime(data[idx + 0], data[idx + 1], start_time, time_base);
		ret = BL_ConvertNumericIntoSingle(data[idx + 2], &ewe);
		ret = BL_ConvertNumericIntoSingle(data[idx + 3], &current);
		cycle = data[idx + 4];
		fs << getAbsTime(chan_start_time, t) << "," << t << "," << loop << "," << ewe << "," << current << "," << cycle;
		processXCTRVals(fs, &(data[idx]), xctr, 5, ncols);
		fs << "\n";
	}
}

void ECLabDriver::ECLabDataTask() 
{
    TCurrentValues_t cvals;
	TDataInfos_t dinfo;
	TDataBuffer_t dbuffer;
	int ndata;
	std::vector<int> last_tech(100, -1);
	std::vector<int> last_tech_index(100, -1);
	std::fstream all_fs0[100], all_fs1[100];
	unsigned file_index[100];
	char filename[256];
	char fileprefix[256];
	char tbuff[64];
	memset(file_index, 0, sizeof(file_index));
	int savedata = 0;
	bool dataDone = false;
	while(true)
	{
	    ndata = 0;
	    for(int i=0; i<m_infos.NumberOfChannels; ++i)
		{
			getIntegerParam(i, P_saveData, &savedata);
			std::fstream& fs0 = all_fs0[i];
			std::fstream& fs1 = all_fs1[i];
			if (savedata == 0)
			{
				if (fs0.is_open())
				{
					fs0.close();
					++(file_index[i]);
				}
				if (fs1.is_open())
				{
					fs1.close();
				}
				lock();
				setIntegerParam(i, P_dataDone, 1);
                callParamCallbacks();
				unlock();
				continue;
			}
		    if (ECLabInterface::IsChannelPlugged(m_ID, i))
			{
				try
				{
					ECLabInterface::GetData(m_ID, i, &dbuffer, &dinfo, &cvals);
				}
				catch(const std::exception& ex)
				{
					errlogSevPrintf(errlogInfo, "%s", ex.what());
					continue;
				}
				lock();
				updateCvals(i, cvals);
				setIntegerParam(i, P_dataTechInd, dinfo.TechniqueIndex);
				setIntegerParam(i, P_dataTechID, dinfo.TechniqueID);
				setIntegerParam(i, P_dataLoop, dinfo.loop);
				setDoubleParam(i, P_dataStartTime, dinfo.StartTime);
				setIntegerParam(i, P_dataIRQSkipped, dinfo.IRQskipped);
                callParamCallbacks();
				unlock();
				int xctr = 0, P_xctr = -1;
				if (m_techniques.size() > dinfo.TechniqueIndex)
				{
					std::string param_name = m_techniques[dinfo.TechniqueIndex].name;
					std::transform(param_name.begin(), param_name.end(), param_name.begin(), ::toupper);
					param_name += "_XCTR";
					if (findParam(param_name.c_str(), &P_xctr) == asynSuccess && P_xctr != -1)
					{
						getIntegerParam(m_techniques[dinfo.TechniqueIndex].index, P_xctr, &xctr);
					}
				}
				if ( fs0.is_open() && (last_tech[i] != dinfo.TechniqueID || last_tech_index[i] != dinfo.TechniqueIndex) )
				{
					fs0.close();
					if (fs1.is_open())
					{
						fs1.close();
					}
					++(file_index[i]);
				}
				epicsTimeToStrftime(tbuff, sizeof(tbuff), "%Y%m%dT%H%M%S", &(m_start_time[i]));
				if (dinfo.NbRows == 0)
				{
					;
				}
				else if (dinfo.TechniqueID == KBIO_TECHID_OCV)
				{	
				    if ( !(fs0.is_open()) )
				    {
					    getStringParam(i, P_filePrefix, sizeof(fileprefix), fileprefix);
					    sprintf(filename, "%s_%s_C%d_T%d_OCV_%d.csv", fileprefix, tbuff, i, dinfo.TechniqueIndex, file_index[i]);
					    fs0.open(filename, std::ios::out);					
					    setStringParam(i, P_fileName, filename);
						fs0 << "AbsTime,Time,Loop,Ewe";
						processXCTRHeader(fs0, xctr);
						fs0 << "\n";
				    }
					processOCVData(fs0, m_start_time[i], dinfo.NbRows, dinfo.NbCols, dinfo.TechniqueIndex, dinfo.ProcessIndex, 
					        dinfo.loop, dinfo.StartTime, cvals.TimeBase, &dbuffer, xctr);
				}
				else if (dinfo.TechniqueID == KBIO_TECHID_CA)
				{
				    if ( !(fs0.is_open()) )
				    {
					    getStringParam(i, P_filePrefix, sizeof(fileprefix), fileprefix);
					    sprintf(filename, "%s_%s_C%d_T%d_CA_%d.csv", fileprefix, tbuff, i, dinfo.TechniqueIndex, file_index[i]);
					    setStringParam(i, P_fileName, filename);
					    fs0.open(filename, std::ios::out);					
						fs0 << "AbsTime,Time,Loop,Ewe,I,Cycle";
						processXCTRHeader(fs0, xctr);
						fs0 << "\n";
				    }
					processCACPData(fs0, m_start_time[i], dinfo.NbRows, dinfo.NbCols, dinfo.TechniqueIndex, dinfo.ProcessIndex, 
					        dinfo.loop, dinfo.StartTime, cvals.TimeBase, &dbuffer, xctr);
				}
				else if (dinfo.TechniqueID == KBIO_TECHID_CP)
				{
				    if ( !(fs0.is_open()) )
				    {
					    getStringParam(i, P_filePrefix, sizeof(fileprefix), fileprefix);
					    sprintf(filename, "%s_%s_C%d_T%d_CP_%d.csv", fileprefix, tbuff, i, dinfo.TechniqueIndex, file_index[i]);
					    setStringParam(i, P_fileName, filename);
					    fs0.open(filename, std::ios::out);					
						fs0 << "AbsTime,Time,Loop,Ewe,I,Cycle";
						processXCTRHeader(fs0, xctr);
						fs0 << "\n";
				    }
					processCACPData(fs0, m_start_time[i], dinfo.NbRows, dinfo.NbCols, dinfo.TechniqueIndex, dinfo.ProcessIndex, 
					        dinfo.loop, dinfo.StartTime, cvals.TimeBase, &dbuffer, xctr);
				}
				else if (dinfo.TechniqueID == KBIO_TECHID_PEIS)
				{
				    if ( !(fs0.is_open()) )
				    {
					    getStringParam(i, P_filePrefix, sizeof(fileprefix), fileprefix);
					    sprintf(filename, "%s_%s_C%d_T%d_PEIS_%d_0.csv", fileprefix, tbuff, i, dinfo.TechniqueIndex, file_index[i]);
					    setStringParam(i, P_fileName, filename);
					    fs0.open(filename, std::ios::out);					
						fs0 << "AbsTime,Time,Loop,Ewe,I\n";
					    sprintf(filename, "%s_%s_C%d_T%d_PEIS_%d_1.csv", fileprefix, tbuff, i, dinfo.TechniqueIndex, file_index[i]);
					    fs1.open(filename, std::ios::out);
						fs1 << "AbsTime,Time,Loop,Freq,Mod Ewe,Mod I,Phase Zwe,Ewe,I,Mod Ece,Mod Ice,Phase Zce,Ece\n";
				    }
					processPEISData(fs0, fs1, m_start_time[i], dinfo.NbRows, dinfo.NbCols, dinfo.TechniqueIndex, dinfo.ProcessIndex, 
					        dinfo.loop, dinfo.StartTime, cvals.TimeBase, &dbuffer);
				}
				lock();
				if (cvals.State == KBIO_STATE_STOP && cvals.MemFilled == 0)
				{
					if (fs0.is_open())
					{
						fs0.close();
						++(file_index[i]);
					}
					if (fs1.is_open())
					{
						fs1.close();
					}
					setIntegerParam(i, P_dataDone, 1);
				}
				else
				{
				    ++ndata;
					setIntegerParam(i, P_dataDone, 0);
				}
                callParamCallbacks();
				unlock();
				last_tech[i] = dinfo.TechniqueID;
				last_tech_index[i] = dinfo.TechniqueIndex;
			}
		}
		if (ndata == 0)
		{
            epicsThreadSleep(2.0);        
		}
		else
		{
            epicsThreadSleep(0.2);
		}
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
