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
	static const std::string ecc_dir = macEnvExpand("$(ECLAB)/ecc/");
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
	createParam(P_saveDataString, asynParamInt32, &P_saveData);
	
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
	}
	
	char version[32];
	unsigned ver_size = sizeof(version);
	ECLabInterface::GetLibVersion(version, &ver_size);
	setStringParam(P_version, version);
	unsigned int timeout = 5;
	ECLabInterface::Connect(const_cast<char *>(ip), timeout, &m_ID, &m_infos);
	ECLabInterface::testConnect(m_ID);

	uint8 chan = 1;
	int res;
	std::string kernel_file = ecc_dir + "kernel4.bin";
	std::replace(kernel_file.begin(), kernel_file.end(), '/', '\\');
	std::string xlx_file = ecc_dir + "Vmp_iv_0395_aa.xlx";
	std::replace(xlx_file.begin(), xlx_file.end(), '/', '\\');
	ECLabInterface::LoadFirmware(m_ID, &chan, &res, 1, 0, 0, kernel_file.c_str(), xlx_file.c_str());

	setStringParam(P_host, ip);
	setIntegerParam(P_devCode, m_infos.DeviceCode);
	setIntegerParam(P_numChannels, m_infos.NumberOfChannels);
	setIntegerParam(P_numSlots, m_infos.NumberOfSlots);
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
	setIntegerParam(chan, P_memFilled, cvals.MemFilled);
	setDoubleParam(chan, P_currEWE, cvals.Ewe);
	setDoubleParam(chan, P_currECE, cvals.Ece);
	setDoubleParam(chan, P_currI, cvals.I);
	setDoubleParam(chan, P_currTIME, cvals.ElapsedTime);
	setDoubleParam(chan, P_currRCOMP, cvals.Rcomp);
	setDoubleParam(chan, P_currFREQ, cvals.Freq);
	setIntegerParam(chan, P_currSTATE, cvals.State);
	setDoubleParam(chan, P_currTimeBase, cvals.TimeBase);
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
    return (((uint64_t)thigh << 32) + tlow) * time_base + start_time;
}

void ECLabDriver::processPEISData(std::fstream& fs0, std::fstream& fs1, int nrows, int ncols, int technique_index, int process_index, 
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
			fs0 << t << "," << ewe << "," << currI << "\n";
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
			fs1 << tf << "," << freq << "," << eweMod
                << "," << currIMod << "," << phaseZwe << "," << ewe	
                << "," << currI << "," << eceMod << "," << iceMod	
                << "," << phaseZce << "," << ece << std::endl;
		}		
	}
}

void ECLabDriver::processOCVData(std::fstream& fs, int nrows, int ncols, int technique_index, int process_index, 
                     int loop, double start_time, double time_base, TDataBuffer_t* dbuffer)
{
	unsigned* data = dbuffer->data;
	double t;
	int idx, ret;
	float ewe;
	if (ncols != 3)
	{
		std::cerr << "OCV: incorrect number of columns in data" << std::endl;
		return;
	}
	for(int i=0; i<nrows; ++i)
	{
		idx = i * ncols;
		t = getTime(data[idx + 0], data[idx + 1], start_time, time_base);
		ret = BL_ConvertNumericIntoSingle(data[idx + 2], &ewe);
//		fs << loop << "," << technique_index << "," << t << "," << ewe << "\n";
		fs << t << "," << ewe << "\n";
	}
}

void ECLabDriver::ECLabDataTask() 
{
    TCurrentValues_t cvals;
	TDataInfos_t dinfo;
	TDataBuffer_t dbuffer;
	int ndata;
	std::fstream all_fs0[100], all_fs1[100];
	unsigned file_index[100];
	char filename[256];
	char fileprefix[256];
	char tbuff[64];
	struct tm* pstm;
	time_t now;
	memset(file_index, 0, sizeof(file_index));
	int savedata = 0;
	while(true)
	{
	    ndata = 0;
		getIntegerParam(P_saveData, &savedata);
	    for(int i=0; i<m_infos.NumberOfChannels; ++i)
		{
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
				}
				lock();
				updateCvals(i, cvals);
                callParamCallbacks();
				unlock();
				time(&now);
				pstm = localtime(&now);
				strftime(tbuff, sizeof(tbuff), "%Y%m%dT%H%M%S", pstm);
				if (dinfo.TechniqueID == KBIO_TECHID_OCV)
				{
				    if ( cvals.State == KBIO_STATE_RUN && !(fs0.is_open()) )
				    {
					    getStringParam(P_filePrefix, sizeof(fileprefix), fileprefix);
					    sprintf(filename, "%s_%s_%d_0.csv", fileprefix, tbuff, i);
					    fs0.open(filename, std::ios::out);					
						fs0 << "Time,Ewe\n";
				    }
					processOCVData(fs0, dinfo.NbRows, dinfo.NbCols, dinfo.TechniqueIndex, dinfo.ProcessIndex, 
					        dinfo.loop, dinfo.StartTime, cvals.TimeBase, &dbuffer);
				}
				else if (dinfo.TechniqueID == KBIO_TECHID_PEIS)
				{
				    if ( cvals.State == KBIO_STATE_RUN && !(fs0.is_open()) )
				    {
					    getStringParam(P_filePrefix, sizeof(fileprefix), fileprefix);
					    sprintf(filename, "%s_%s_%d_0.csv", fileprefix, tbuff, i);
					    fs0.open(filename, std::ios::out);					
						fs0 << "Time,Ewe,I\n";
						sprintf(filename, "%s_%s_%d_1.csv", fileprefix, tbuff, i);
					    fs1.open(filename, std::ios::out);
						fs1 << "Time,Freq,Mod Ewe,Mod I,Phase Zwe,Ewe,I,Mod Ece,Mod Ice,Phase Zce,Ece\n";
				    }
					processPEISData(fs0, fs1, dinfo.NbRows, dinfo.NbCols, dinfo.TechniqueIndex, dinfo.ProcessIndex, 
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
