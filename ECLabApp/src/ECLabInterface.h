/*************************************************************************\ 
* Copyright (c) 2013 Science and Technology Facilities Council (STFC), GB. 
* All rights reverved. 
* This file is distributed subject to a Software License Agreement found 
* in the file LICENSE.txt that is included with this distribution. 
\*************************************************************************/ 

#ifndef ECLabInterface_H
#define ECLabInterface_H

/// @file ECLabInterface.h
/// Provide an interface to EC lab functions, converting error codes into C++ exceptions 
/// 

#include <string>
#include <sstream>
#include <BLFunctions.h>

typedef unsigned uint32;

//BIOLOGIC_API(int) BL_FindEChemDev(char* PChar, uint32* psize, uint32* pNbrDev);

#include "BLFunctionsStub.h"

#define BLCONCAT(A,B) A##B

/// encapsulates an EC lab error code as an exception object
class ECLabException : public std::runtime_error
{
 public:
    explicit ECLabException(const std::string& what) : std::runtime_error(what) { }
    explicit ECLabException(const std::string& func, int code) : std::runtime_error(std::string(func + " failed: " + translateCode(code))) { }
    static std::string translateCode(int code)
	{
	    char mesg[256];
		std::ostringstream oss;
		unsigned int mesg_size = sizeof(mesg);
		if ( BL_GetErrorMsg(code, mesg, &mesg_size) == 0 )
		{
            if (code == ERR_GEN_ECLAB_LOADED)
            {
                strcat(mesg, " (this is invalid for use with OEM library)");
            }
		    return mesg;
		}
		else
		{
		    oss << "unknown error code: " << code;
		    return oss.str();
		}
	}
};

/// call an ECLab function, converting error into an exception 
#define BL_CALL(__func, ... ) \
    int status; \
    if (BLSIM) \
    { \
        status = BLCONCAT(__func,Stub) ( __VA_ARGS__ ); \
	} \
	else \
	{ \
        status = __func ( __VA_ARGS__ ); \
	} \
    if ( status != 0 ) \
	{ \
	    throw ECLabException(#__func, status); \
	}
	
/// Wraps all of the ECLab BL_* function in an interface class that converts 
/// the error code into an exception
struct ECLabInterface
{
    static bool BLSIM;
	
    static void GetLibVersion(char*  pVersion, unsigned int* psize)
	{
	    BL_CALL(BL_GetLibVersion, pVersion, psize);	
	}
	
	static void Connect(const char* address, uint8 TimeOut, int* pID, TDeviceInfos_t* pInfos)
	{
		BL_CALL(BL_Connect, address, TimeOut, pID, pInfos );
	}
	
	static void testConnect(int ID)
	{
		BL_CALL(BL_TestConnection, ID);
	}
	
	static void Disconnect(int ID)
	{
		BL_CALL(BL_Disconnect, ID);
	}
	
    static void LoadFirmware(int ID, uint8* pChannels, int* pResults, uint8 Length, 
                             bool ShowGauge, bool ForceReload, const char* BinFile, const char* XlxFile)
    {
	    BL_CALL(BL_LoadFirmware, ID, pChannels, pResults, Length, ShowGauge, ForceReload, BinFile, XlxFile);
    }
	
	static void LoadTechnique(int ID, uint8 channel, char* pFName, TEccParams_t Params, bool FirstTechnique, bool LastTechnique, bool DisplayParams)
	{
		BL_CALL(BL_LoadTechnique, ID, channel, pFName, Params, FirstTechnique, LastTechnique, DisplayParams);
	}
	
	static void GetChannelInfos (int ID, uint8 ch, TChannelInfos_t *pInfos)
	{
		BL_CALL(BL_GetChannelInfos, ID, ch, pInfos);
	}
	
	static void StopChannel (int ID, uint8 channel)
	{
		BL_CALL(BL_StopChannel, ID, channel);
	}
	
	static void StartChannel (int ID, uint8 channel)
	{
		BL_CALL(BL_StartChannel, ID, channel);
	}

    static void getChannelsPlugged( int ID, std::vector<uint8_t>& chans )
    {
        BL_CALL(BL_GetChannelsPlugged, ID, &(chans[0]), chans.size());
    }
    
    static void findEChemDev(std::string& data, uint32& nDev)
    {
        char ret_data[512];
        uint32 bufsiz = sizeof(ret_data) - 1;
        nDev = 0;
        memset(ret_data, '\0', sizeof(ret_data));
//        BL_CALL(BL_FindEChemDev, ret_data, &bufsiz, &nDev);
        data.assign(ret_data, bufsiz);
    }
    
	static bool IsChannelPlugged( int ID, uint8 ch )
	{
	    if (BLSIM)
		{
		    return BL_IsChannelPluggedStub(ID, ch);
		}
		else
		{
		    return BL_IsChannelPlugged(ID, ch);
		}
	}

	static void GetCurrentValues (int ID, uint8 channel, TCurrentValues_t *pValues) 
	{
		BL_CALL(BL_GetCurrentValues, ID, channel, pValues);
	}

    static void UpdateParameters( int ID, uint8 channel, int TechIndx, TEccParams_t Params, const char* EccFileName )
    {
		BL_CALL(BL_UpdateParameters, ID, channel, TechIndx, Params, EccFileName);
    }	

	static void GetData( int ID, uint8 channel, TDataBuffer_t* pBuf, TDataInfos_t* pInfos, TCurrentValues_t* pValues ) 
    {
	    BL_CALL(BL_GetData, ID, channel, pBuf, pInfos, pValues);
	}

	static void SetExperimentInfos( int ID, uint8 channel, TExperimentInfos_t TExpInfos ) 
	{
	    BL_CALL(BL_SetExperimentInfos, ID, channel, TExpInfos);
	}
	
	static void GetExperimentInfos( int ID, uint8 channel, TExperimentInfos_t* TExpInfos )
	{
	    BL_CALL(BL_GetExperimentInfos, ID, channel, TExpInfos);
	}
	
	static void GetHardConf(int ID, uint8 channel, THardwareConf_t* pHardConf)
	{
	    BL_CALL(BL_GetHardConf, ID, channel, pHardConf);
	}

	static void SetHardConf(int ID, uint8 channel, THardwareConf_t HardConf)
	{
	    BL_CALL(BL_SetHardConf, ID, channel, HardConf);
	}

};

#undef BLCONCAT

#endif /* ECLabInterface_H */