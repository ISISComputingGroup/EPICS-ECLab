

/// @file ECLabInterface.h
/// Provide an interface to EC lab functions, converting error codes into C++ exceptions 
/// 

#include <string>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <BLFunctions.h>
#include <epicsStdio.h>

static int my_connection_id = 2382312;

struct MyTechnique
{
    std::string name;
	TEccParams_t params;
	MyTechnique() { params.pParams = 0; }
	MyTechnique(const char* nname, const TEccParams_t& nparams) { params.pParams = 0; copy(nname, nparams); }
	MyTechnique(const MyTechnique& t) { params.pParams = 0; copy(t.name.c_str(), t.params); }
	
	void copy(const char* nname, const TEccParams_t& nparams)
	{
	    name = nname;
	    delete params.pParams;
		params.pParams = new TEccParam_t[nparams.len];
		params.len = nparams.len;
		memcpy(params.pParams, nparams.pParams,  nparams.len * sizeof(TEccParam_t));
	}
	void print(std::ostream& os)
	{
	    os << "Technique " << name << std::endl;
		for (int i = 0; i<params.len; ++i)
		{
		  	os << "Param name " << params.pParams[i].ParamStr << std::endl;
		  	os << "Param type " << params.pParams[i].ParamType << std::endl;
		  	os << "Param val " << params.pParams[i].ParamVal << std::endl;
		  	os << "Param index " << params.pParams[i].ParamIndex << std::endl;
		}
	}
};

struct MyChannel
{
    TChannelInfos_t info;
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
	    my_channels[i].info.Channel = i;
	    my_channels[i].info.State = KBIO_STATE_STOP;
	    my_channels[i].info.NbOfTechniques = 0;
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

#define DEBUG_PRINT(__arg) std::cerr << __arg << std::endl

BIOLOGIC_API(int) BL_GetLibVersion(char*  pVersion, unsigned int* psize)
{
    DEBUG_PRINT("BL_GetLibVersion");
    static const char* version = "5.34.0.1";
	strncpy(pVersion, version, *psize);
	pVersion[*psize-1] = '\0';
	*psize = strlen(version);
	return 0;
}

BIOLOGIC_API(int) BL_GetErrorMsg( int errorcode, char*  pmsg, unsigned int* psize ) 
{
    DEBUG_PRINT("BL_GetErrorMsg");
    if (errorcode == 0)
	{
	    strncpy(pmsg, "No ERROR", *psize);
        return 0; 
	}
	else if (errorcode < 0)
	{
	    epicsSnprintf(pmsg, *psize, "Dummy Error code %d", errorcode);
		return 0;
	}
	else
	{
		return -1;
	}
}

BIOLOGIC_API(int) BL_Connect(const char* address, uint8 timeout, int* pID, TDeviceInfos_t* pInfos) 
{
    DEBUG_PRINT("BL_Connect");
	*pID = my_connection_id;
	pInfos->DeviceCode = KBIO_DEV_SP200;
	pInfos->NumberOfChannels = NCHANNELS;
	pInfos->NumberOfSlots = 1;
	init_channels();
	return 0;
}

BIOLOGIC_API(int) BL_TestConnection(int ID)
{
    DEBUG_PRINT("BL_GetLibVersion");
	CHECK_CONNECTION_ID(ID);
	return 0;
}

BIOLOGIC_API(int) BL_Disconnect(int ID)
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

BIOLOGIC_API(bool) BL_IsChannelPlugged( int ID, uint8 ch ) 
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

BIOLOGIC_API(int) BL_GetChannelsPlugged ( int ID, uint8* pChPlugged, uint8 Size ) 
{
    DEBUG_PRINT("BL_GetChannelsPlugged");
	CHECK_CONNECTION_ID(ID);
	if (Size > NCHANNELS)
	{
	    return -1;
	}
	for(int i=0; i<Size; ++i)
	{
	    pChPlugged[i] = 1;
	}
    return 0; 
}

BIOLOGIC_API(int) BL_GetChannelInfos( int ID, uint8 ch, TChannelInfos_t* pInfos ) 
{ 
    DEBUG_PRINT("BL_GetChannelInfos");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(ch);
	memcpy(pInfos, &(my_channels[ch].info), sizeof(TChannelInfos_t));
	return 0; 
}


BIOLOGIC_API(int) BL_LoadTechnique(int ID, uint8 channel, const char* pFName, TEccParams_t Params, bool FirstTechnique, bool LastTechnique, bool DisplayParams)
{
    DEBUG_PRINT("BL_LoadTechnique");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(channel);
	if (FirstTechnique)
	{
	    my_channels[channel].info.NbOfTechniques = 0;
	    my_channels[channel].techniques.resize(0);
	}
	my_channels[channel].techniques.push_back(MyTechnique(pFName, Params));
	++(my_channels[channel].info.NbOfTechniques);
	if (DisplayParams == true)
	{
	    my_channels[channel].techniques.back().print(std::cout);
	}
	return 0;
}

BIOLOGIC_API(int) BL_UpdateParameters( int ID, uint8 channel, int TechIndx, TEccParams_t Params, const char* EccFileName ) 
{ 
    DEBUG_PRINT("BL_UpdateParameters");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(channel);
	if (TechIndx >= my_channels[channel].techniques.size())
	{
	    return -1;
	}
	my_channels[channel].techniques[TechIndx].copy(EccFileName, Params);
	my_channels[channel].print(std::cout);
    return 0; 
}

BIOLOGIC_API(int) BL_StartChannel (int ID, uint8 channel)
{
    DEBUG_PRINT("BL_StartChannel");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(channel);
	my_channels[channel].info.State = KBIO_STATE_RUN;
	my_channels[channel].print(std::cout);
    return 0; 
}

BIOLOGIC_API(int) BL_StopChannel (int ID, uint8 channel)
{
    DEBUG_PRINT("BL_StopChannel");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(channel);
	my_channels[channel].info.State = KBIO_STATE_STOP;
    return 0; 
}

BIOLOGIC_API(int) BL_GetCurrentValues (int ID, uint8 channel, TCurrentValues_t *pValues) 
{
    DEBUG_PRINT("BL_GetCurrentValues");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(channel);
	pValues->State = my_channels[channel].info.State;
	pValues->Ece = 2;
	pValues->Eoverflow = 3;
	return 0;
}


BIOLOGIC_API(int) BL_GetData( int ID, uint8 channel, TDataBuffer_t* pBuf, TDataInfos_t* pInfos, TCurrentValues_t* pValues ) 
{ 
    DEBUG_PRINT("BL_GetData");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(channel);
    return 0; 
}

BIOLOGIC_API(int) BL_SetExperimentInfos( int ID, uint8 channel, TExperimentInfos_t TExpInfos ) 
{ 
    DEBUG_PRINT("BL_SetExperimentInfos");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(channel);
    return 0; 
}

BIOLOGIC_API(int) BL_GetExperimentInfos( int ID, uint8 channel, TExperimentInfos_t* TExpInfos )
{ 
    DEBUG_PRINT("BL_GetExperimentInfos");
	CHECK_CONNECTION_ID(ID);
	CHECK_CHANNEL(channel);
    return 0; 
}


