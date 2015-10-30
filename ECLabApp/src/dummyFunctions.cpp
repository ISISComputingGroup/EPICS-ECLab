

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
	pInfos->NumberOfChannels = 1;
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
		//printf ("Running %s\n", pFName);
		//printf("params are...."); 
		int length = Params.len;
		//printf ("%d \n", length);
		//for (int i = 0; i<length; i++)
		//{
		//	printf("ParamStr %s\n", Params.pParams[i].ParamStr);
		//	printf("ParamType %d\n", Params.pParams[i].ParamType);
		//	printf("ParamVal %d\n", Params.pParams[i].ParamVal);
		//	printf("ParamIndex %d\n", Params.pParams[i].ParamIndex);
		//}
	}
	return 0;
}

BIOLOGIC_API(int) BL_StartChannel (int ID, uint8 channel)
{
	//printf("Starting channel %d\n", channel); //then dev to print out the techniques actually in that channel
	return 0;
}

BIOLOGIC_API(int) BL_GetChannelInfos(int ID, uint8 ch, TChannelInfos_t* pInfos)
{
	pInfos->Channel = ch;
	pInfos->NbOfTechniques = 3;
	//printf("Channel is num 0\n");
	//printf("Num techniques running is 3\n");
	return 0;
}

BIOLOGIC_API(int) BL_StopChannel (int ID, uint8 channel)
{
	//printf("Stopping channel %d\n", channel); //get it to check channel running before stopping?
	return 0;
}

BIOLOGIC_API(int) BL_GetCurrentValues (int ID, uint8 channel, TCurrentValues_t *pValues) 
{
	pValues -> State = 1; //run
	pValues -> Ece = 2;
	pValues -> Eoverflow = 3;
	//printf("Current values are: \n"); //dev to print out the current values? Or just load them in pValues here.
	//printf("state is: %d\n", pValues->State);
	//printf("Ece is: %d\n", pValues -> Ece);
	//printf("Eoverflow is: %d\n", pValues -> Eoverflow);
	return 0;
}
