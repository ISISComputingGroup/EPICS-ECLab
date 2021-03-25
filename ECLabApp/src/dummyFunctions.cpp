/*************************************************************************\ 
* Copyright (c) 2013 Science and Technology Facilities Council (STFC), GB. 
* All rights reverved. 
* This file is distributed subject to a Software License Agreement found 
* in the file LICENSE.txt that is included with this distribution. 
\*************************************************************************/ 


/// @file ECLabInterface.h
/// Provide an interface to EC lab functions, converting error codes into C++ exceptions 
/// 

#include <string>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <time.h>

#include <epicsStdio.h>

#include "ECLabInterface.h"

static float rand0to1()
{
    return (float)rand() / (float)RAND_MAX;    
}

static float g_ewe = 0.0;
static float g_ece = 0.0;
static float g_current = 0.0;

static int my_connection_id = 2382312;

bool ECLabInterface::BLSIM = false;

static const char* paramTypes[] = { "int32", "boolean", "single" };

static int currentTechIndex = 0;

struct MyTechnique
{
    std::string name;
	int techIndex;
	TEccParams_t params;
	MyTechnique() : techIndex(-1) { params.pParams = 0; }
	MyTechnique(const char* nname, int techIndex_, const TEccParams_t& nparams) { params.pParams = 0; copy(nname, techIndex_, nparams); }
	MyTechnique(const MyTechnique& t) { params.pParams = 0; copy(t.name.c_str(), t.techIndex, t.params); }
	
	void copy(const char* nname, int techIndex_, const TEccParams_t& nparams)
	{
	    name = nname;
		techIndex = techIndex_;
	    delete params.pParams;
		params.pParams = new TEccParam_t[nparams.len];
		params.len = nparams.len;
		memcpy(params.pParams, nparams.pParams,  nparams.len * sizeof(TEccParam_t));
	}
	
	void update(const char* nname, int techIndex_, const TEccParams_t& nparams)
	{
	    if (name != nname || techIndex != techIndex_)
		{
			std::cerr << "update params error" << std::endl;
			return;
		}			
		for(int i=0; i<nparams.len; ++i)
		{
			bool name_found = false, index_found = false;
			for(int j=0; j<params.len; ++j)
			{
				if (!strcmp(nparams.pParams[i].ParamStr, params.pParams[j].ParamStr))
				{
					name_found = true;
					if (nparams.pParams[i].ParamIndex == params.pParams[j].ParamIndex)
					{
						index_found = true;
						params.pParams[j].ParamVal = nparams.pParams[i].ParamVal;
					}
				}
			}
			if (!name_found)
			{
				std::cerr << "Unable to find parameter " << nparams.pParams[i].ParamStr << std::endl;
			}
			else if (!index_found)
			{
				std::cerr << "Unable to find parameter index " << nparams.pParams[i].ParamIndex << " for " << nparams.pParams[i].ParamStr << std::endl;				
			}
		}
	}
	
	double paramVal(int index)
	{
		int val = params.pParams[index].ParamVal;
		int type = params.pParams[index].ParamType;
		if (type == 0 || type == 1)
		{
			return val; 
		}
		else if (type == 2)
		{
			float fval;
			BL_ConvertNumericIntoSingle(val, &fval);
			return fval;
		}
		else
		{
			std::cerr << "param data type error" << std::endl;
			return -1000;
		}
	}
	
	void print(std::ostream& os)
	{
	    os << "Technique " << name << " index " << techIndex << std::endl;
		for (int i = 0; i<params.len; ++i)
		{
		  	os << "Param: \"" << params.pParams[i].ParamStr << "\" (" << paramTypes[params.pParams[i].ParamType] << ") = " << paramVal(i) << " (index=" << params.pParams[i].ParamIndex << ")" << std::endl;
		}
	}
};

struct MyChannel
{
    TChannelInfos_t info;
	time_t start;
	std::vector<MyTechnique> techniques;
	void print(std::ostream& os)
	{
	    for(int i=0; i<techniques.size(); ++i)
		{
		    techniques[i].print(os);
		}
	}
};

static MyChannel my_channels[16];

#define NCHANNELS 1  // 1 to `16

static void init_channels()
{
    for(int i=0; i<16; ++i)
	{
		memset(&(my_channels[i].info), 0, sizeof(TChannelInfos_t));
	    my_channels[i].info.Channel = i;
	    my_channels[i].info.State = KBIO_STATE_STOP;
	    my_channels[i].info.NbOfTechniques = 0;
        my_channels[i].info.FirmwareCode = KIBIO_FIRM_KERNEL;
	}
}

#define CHECK_CONNECTION_ID(__ID) \
    if (__ID != my_connection_id) \
	{ \
	    return -1; \
	}
#define CHECK_CHANNEL(__CH) \
	if (__CH >= NCHANNELS) \
	{ \
	    return -1; \
	}

//#define DEBUG_PRINT(__arg) std::cerr << __arg << std::endl
#define DEBUG_PRINT(__arg) 

BIOLOGIC_API(int) BL_GetLibVersionStub(char*  pVersion, unsigned int* psize)
{
    DEBUG_PRINT("BL_GetLibVersion");
    static const char* version = "5.34.0.1";
	strncpy(pVersion, version, *psize);
	pVersion[*psize-1] = '\0';
	*psize = strlen(version);
	return 0;
}

//BIOLOGIC_API(int) BL_GetErrorMsgStub( int errorcode, char*  pmsg, unsigned int* psize ) 
//{
//    DEBUG_PRINT("BL_GetErrorMsg");
//    if (errorcode == 0)
//	{
//	    strncpy(pmsg, "No ERROR", *psize);
//       return 0; 
//	}
//	else if (errorcode < 0)
//	{
//	    epicsSnprintf(pmsg, *psize, "Dummy Error code %d", errorcode);
//		return 0;
//	}
//	else
//	{
//		return -1;
//	}
//}

BIOLOGIC_API(int) BL_ConnectStub(const char* address, uint8 timeout, int* pID, TDeviceInfos_t* pInfos) 
{
    DEBUG_PRINT("BL_Connect");
	*pID = my_connection_id;
	pInfos->DeviceCode = KBIO_DEV_SP200;
	pInfos->NumberOfChannels = NCHANNELS;
	pInfos->NumberOfSlots = 1;
	init_channels();
	return 0;
}

BIOLOGIC_API(int) BL_TestConnectionStub(int ID)
{
    DEBUG_PRINT("BL_TestConnection");
	CHECK_CONNECTION_ID(ID);
	return 0;
}

BIOLOGIC_API(int) BL_DisconnectStub(int ID)
{
    DEBUG_PRINT("BL_Disconnect");
    if (ID == my_connection_id)
	{
		++my_connection_id;  // this ensures closing or using a previously closed channel will generate an error
	    return 0;
	}
	else
	{
		return -1;
	}
}

BIOLOGIC_API(int) BL_LoadFirmwareStub( int ID, uint8* pChannels, int* pResults, uint8 Length, 
                             bool ShowGauge, bool ForceReload, const char* BinFile, const char* XlxFile)
{
	return 0;
}

BIOLOGIC_API(bool) BL_IsChannelPluggedStub( int ID, uint8 ch ) 
{ 
    DEBUG_PRINT("BL_IsChannelPlugged");
    if (ID == my_connection_id)
	{
	    return true; 
	}
	else
	{
	    return false; 
	}
}

BIOLOGIC_API(int) BL_GetChannelsPluggedStub ( int ID, uint8* pChPlugged, uint8 Size ) 
{
    DEBUG_PRINT("BL_GetChannelsPlugged");
	CHECK_CONNECTION_ID(ID);
    memset(pChPlugged, 0, Size * sizeof(uint8));
	pChPlugged[0] = 1;
//	for(int i=0; i<Size && i < NCHANNELS; ++i)
//	{
//	    pChPlugged[i] = 1;
//	}
    return 0; 
}

BIOLOGIC_API(int) BL_GetChannelInfosStub( int ID, uint8 ch, TChannelInfos_t* pInfos ) 
{ 
    DEBUG_PRINT("BL_GetChannelInfos");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(ch);
	memcpy(pInfos, &(my_channels[ch].info), sizeof(TChannelInfos_t));
	return 0; 
}


BIOLOGIC_API(int) BL_LoadTechniqueStub(int ID, uint8 channel, const char* pFName, TEccParams_t Params, bool FirstTechnique, bool LastTechnique, bool DisplayParams)
{
    DEBUG_PRINT("BL_LoadTechnique");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(channel);
	if (FirstTechnique)
	{
	    my_channels[channel].info.NbOfTechniques = 0;
	    my_channels[channel].techniques.resize(0);
	}
	int techId = my_channels[channel].info.NbOfTechniques;
	my_channels[channel].techniques.push_back(MyTechnique(pFName, techId, Params));
	++(my_channels[channel].info.NbOfTechniques);
	if (DisplayParams == true)
	{
	    my_channels[channel].techniques.back().print(std::cout);
	}
	return 0;
}

BIOLOGIC_API(int) BL_UpdateParametersStub( int ID, uint8 channel, int TechIndx, TEccParams_t Params, const char* EccFileName ) 
{ 
    DEBUG_PRINT("BL_UpdateParameters");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(channel);
	if (TechIndx >= my_channels[channel].techniques.size())
	{
	    return -1;
	}
	my_channels[channel].techniques[TechIndx].update(EccFileName, TechIndx, Params);
	my_channels[channel].print(std::cout);
    return 0; 
}

BIOLOGIC_API(int) BL_StartChannelStub (int ID, uint8 channel)
{
    DEBUG_PRINT("BL_StartChannel");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(channel);
	my_channels[channel].info.State = KBIO_STATE_RUN;
	my_channels[channel].print(std::cout);
	time(&(my_channels[channel].start));
	currentTechIndex = 0;
    return 0; 
}

BIOLOGIC_API(int) BL_StopChannelStub (int ID, uint8 channel)
{
    DEBUG_PRINT("BL_StopChannel");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(channel);
	my_channels[channel].info.State = KBIO_STATE_STOP;
    return 0; 
}

BIOLOGIC_API(int) BL_GetCurrentValuesStub (int ID, uint8 channel, TCurrentValues_t *pValues) 
{
    DEBUG_PRINT("BL_GetCurrentValues");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(channel);
	memset(pValues, 0, sizeof(TCurrentValues_t));
	pValues->State = my_channels[channel].info.State;
	pValues->MemFilled = 0;
	pValues->TimeBase = 1e-6f;
	pValues->Ewe = g_ewe;
	pValues->EweRangeMin = 0.0;
	pValues->EweRangeMax = 0.0;
	pValues->Ece = g_ece;
	pValues->EceRangeMin = 0.0;
	pValues->EceRangeMax = 0.0;
	pValues->Eoverflow = 1;
	pValues->I = g_current;
	pValues->IRange = 0;
	pValues->Ioverflow = 0;
	pValues->ElapsedTime = time(NULL) - my_channels[channel].start;
	pValues->Freq = 0.0;
	pValues->Rcomp = 0.0;
	pValues->Saturation = 0;
	pValues->OptErr = 0;
	pValues->OptPos = 0;
	return 0;
}

static int castFloatToInt(float f)
{
	union
	{
		float f;
		int i;
	} cf;
	cf.f = f;
	return cf.i;
}


// when we move from VS2010 we can use an initialiser list for the above
static int getTechId(const std::string& file)
{
	static std::map<std::string, int> tech_map;
	static bool init_done = false;
	if (!init_done)
	{
		tech_map["cp4.ecc"] = KBIO_TECHID_CP;
		tech_map["ca4.ecc"] = KBIO_TECHID_CA;
		tech_map["cv4.ecc"] = KBIO_TECHID_CV;
		tech_map["cplimit4.ecc"] = KBIO_TECHID_CPLIMIT;
		tech_map["calimit4.ecc"] = KBIO_TECHID_CALIMIT;
		tech_map["peis4.ecc"] = KBIO_TECHID_PEIS;
		tech_map["ocv4.ecc"] = KBIO_TECHID_OCV;
		tech_map["loop4.ecc"] = KBIO_TECHID_LOOP;
		init_done = true;
	}
    return tech_map[file];
}

BIOLOGIC_API(int) BL_GetDataStub( int ID, uint8 channel, TDataBuffer_t* pBuf, TDataInfos_t* pInfos, TCurrentValues_t* pValues ) 
{ 
    DEBUG_PRINT("BL_GetData");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(channel);
    g_ewe = 1.0f + rand0to1();
    g_ece = 2.0f + 2.0f * rand0to1();
    g_current = 3.0f + 3.0f * rand0to1();
	memset(pInfos, 0, sizeof(TDataInfos_t));
	BL_GetCurrentValuesStub(ID, channel, pValues);
	pInfos->NbRows = 0;
	pInfos->NbCols = 0;
	if (my_channels[channel].info.State != KBIO_STATE_RUN)
	{
		return 0;
	}
	if (my_channels[channel].techniques.size() <= currentTechIndex)
	{
		return 0;
	}
	MyTechnique& my_tech = my_channels[channel].techniques[currentTechIndex];
	pInfos->TechniqueIndex = currentTechIndex;
	pInfos->StartTime = 0;
	size_t pos = my_tech.name.find_last_of("\\/");
    std::string tech_file = (pos != std::string::npos ? my_tech.name.substr(pos+1) : my_tech.name);
	if (tech_file == "ocv4.ecc")
	{
		pInfos->NbRows = 1;
		pInfos->NbCols = 3;
		pInfos->TechniqueID = getTechId(tech_file);
		pInfos->ProcessIndex = 0;
		__int64 t;
		t = (time(NULL) - my_channels[channel].start - pInfos->StartTime) / pValues->TimeBase;
		pBuf->data[0] = (t >> 32);	// thigh
		pBuf->data[1] = (t & 0xffffffff);	// tlow
		pBuf->data[2] = castFloatToInt(g_ewe);	// ewe	
	}
	else if (tech_file == "cp4.ecc" || tech_file == "ca4.ecc" ||
             tech_file == "cplimit4.ecc" || tech_file == "calimit4.ecc")
	{
		pInfos->NbRows = 1;
		pInfos->NbCols = 5;
		pInfos->TechniqueID = getTechId(tech_file);
		pInfos->ProcessIndex = 0;
		__int64 t;
		t = (time(NULL) - my_channels[channel].start - pInfos->StartTime) / pValues->TimeBase;
		pBuf->data[0] = (t >> 32);	// thigh
		pBuf->data[1] = (t & 0xffffffff);	// tlow
		pBuf->data[2] = castFloatToInt(g_ewe);	// ewe	
		pBuf->data[3] = castFloatToInt(g_current);	// I	
		pBuf->data[4] = 7;	// cycle	
	}
	else if (tech_file == "cv4.ecc")
	{
		pInfos->NbRows = 1;
		pInfos->NbCols = 5;
		pInfos->TechniqueID = getTechId(tech_file);
		pInfos->ProcessIndex = 0;
		__int64 t;
		t = (time(NULL) - my_channels[channel].start - pInfos->StartTime) / pValues->TimeBase;
		pBuf->data[0] = (t >> 32);	// thigh
		pBuf->data[1] = (t & 0xffffffff);	// tlow
		pBuf->data[2] = castFloatToInt(g_current);	// I
		pBuf->data[3] = castFloatToInt(g_ewe);	// ewe
		pBuf->data[4] = 7;	// cycle
	}
	else if (tech_file == "peis4.ecc")
	{
		pInfos->TechniqueID = getTechId(tech_file);
		pInfos->NbRows = 1;
		if (time(NULL) % 2 == 0)
		{
			pInfos->ProcessIndex = 0;
			pInfos->NbCols = 4;
			__int64 t;
			t = (time(NULL) - my_channels[channel].start - pInfos->StartTime) / pValues->TimeBase;
			pBuf->data[0] = (t >> 32);	// thigh
			pBuf->data[1] = (t & 0xffffffff);	// tlow
			pBuf->data[2] = castFloatToInt(g_ewe);	// ewe	
			pBuf->data[3] = castFloatToInt(g_current);	// I	
		}
		else
		{
			pInfos->ProcessIndex = 1;
			pInfos->NbCols = 14;
			pBuf->data[13] = castFloatToInt(10);	// time					
		}
	}
	else if (tech_file == "loop4.ecc")
	{
		pInfos->TechniqueID = getTechId(tech_file);
	}
	else
	{
		std::cerr << "No data for unknown technique: " << tech_file << std::endl;
	}
	if (++currentTechIndex >= my_channels[channel].techniques.size())
	{
		currentTechIndex = 0;
	}
    return 0; 
}

BIOLOGIC_API(int) BL_SetExperimentInfosStub( int ID, uint8 channel, TExperimentInfos_t TExpInfos ) 
{ 
    DEBUG_PRINT("BL_SetExperimentInfos");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(channel);
    return 0; 
}

BIOLOGIC_API(int) BL_GetExperimentInfosStub( int ID, uint8 channel, TExperimentInfos_t* TExpInfos )
{ 
    DEBUG_PRINT("BL_GetExperimentInfos");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(channel);
	memset(TExpInfos, 0, sizeof(TExperimentInfos_t));
    return 0; 
}

BIOLOGIC_API(int) BL_GetHardConfStub( int ID, uint8 channel, THardwareConf_t* pHardConf )
{
    DEBUG_PRINT("BL_GetHardConfStub");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(channel);
	memset(pHardConf, 0, sizeof(THardwareConf_t));
    return 0; 	
}

BIOLOGIC_API(int) BL_SetHardConfStub( int ID, uint8 channel, THardwareConf_t HardConf )
{
    DEBUG_PRINT("BL_SetHardConf");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(channel);
    return 0; 
}

BIOLOGIC_API(int) BL_FindEChemDevStub( char* data, uint32* siz, uint32* ndv )
{
    strncpy(data, "test", *siz);
    *siz = strlen(data);
    *ndv = 1;
    return 0;    
}
