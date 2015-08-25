

/// @file ECLabInterface.h
/// Provide an interface to EC lab functions, converting error codes into C++ exceptions 
/// 

#include <string>
#include <sstream>
#include <BLFunctions.h>

BIOLOGIC_API(int) BL_GetLibVersion(char*  pVersion, unsigned int* psize)
{
	strcpy(pVersion, "5.34.0.1");
	*psize = 8;
	return 0;
}

BIOLOGIC_API(int) BL_Connect(const char* address, uint8 timeout, int* pID, TDeviceInfos_t* pInfos) 
{
	*pID = 23823123;
	pInfos->DeviceCode = 31232;
	return 0;
}

BIOLOGIC_API(int) BL_TestConnection(int ID)
{
	return 0;
}

BIOLOGIC_API(int) BL_Disconnect(int ID)
{
	return 0;
}

BIOLOGIC_API(int) BL_LoadTechnique(int ID, uint8 channel, const char* pFName, TEccParams_t Params, bool FirstTechnique, bool LastTechnique, bool DisplayParams)
{
	if (DisplayParams == true)
	{
		printf ("%s\n", pFName);
		printf("params are...."); 
		int length = Params.len;
		printf ("%d \n", length);
		for (int i = 0; i<length; i++)
		{
			printf("ParamStr %s\n", Params.pParams[i].ParamStr);
			printf("ParamType %d\n", Params.pParams[i].ParamType);
			printf("ParamVal %d\n", Params.pParams[i].ParamVal);
			printf("ParamIndex %d\n", Params.pParams[i].ParamIndex);
		}
	}
	return 0;
}

BIOLOGIC_API(int) BL_StartChannel (int ID, uint8 channel)
{
	printf("Starting channel %d techniques", channel); //then dev to print out the techniques actually in that channel
	return 0;
}

BIOLOGIC_API(int) BL_GetChannelInfos(int ID, uint8 ch, TChannelInfos_t* pInfos)
{
	pInfos->Channel = ch;
	pInfos->NbOfTechniques = 3;
	return 0;
}

