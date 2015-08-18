

/// @file ECLabInterface.h
/// Provide an interface to EC lab functions, converting error codes into C++ exceptions 
/// 

#include <string>
#include <sstream>
#include <BLFunctions.h>

BIOLOGIC_API(int) BL_GetLibVersion2(char*  pVersion, unsigned int* psize)
{
	strcpy(pVersion, "5.34.0.1");
	*psize = 8;
	return 0;
}

BIOLOGIC_API(int) BL_Connect2(const char* address, uint8 timeout, int* pID, TDeviceInfos_t* pInfos) 
{
	*pID = 23823123;
	pInfos->DeviceCode = 31232;
	return 0;
}

BIOLOGIC_API(int) BL_TestConnection2(int ID)
{
	return 0;
}

BIOLOGIC_API(int) BL_Disconnect2(int ID)
{
	return 0;
}

/*BIOLOGIC_API(int) BL_LoadTechnique2(int ID, unsigned int channel, const char* pFName, TEccParams Params, bool FirstTechnique, bool LastTechnique, bool DisplayParams)
{
	return 0;
}
*/
