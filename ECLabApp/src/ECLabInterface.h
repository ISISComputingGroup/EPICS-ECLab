#ifndef ECLabInterface_H
#define ECLabInterface_H

/// @file ECLabInterface.h
/// Provide an interface to EC lab functions, converting error codes into C++ exceptions 
/// 

#include <string>
#include <sstream>
#include <BLFunctions.h>

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
		if ( BL_GetErrorMsg(code, mesg, &mesg_size) == 0)
		{
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
    int status = __func ( __VA_ARGS__ ); \
    if ( status != 0 ) \
	{ \
	    throw ECLabException(#__func, status); \
	}
	
/// Wraps all of the ECLab BL_* function in an interface class that converts 
/// the error code into an exception
struct ECLabInterface
{
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

	static bool IsChannelPlugged( int ID, uint8 ch )
	{
		return BL_IsChannelPlugged(ID, ch);
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
	
};

#endif /* ECLabInterface_H */