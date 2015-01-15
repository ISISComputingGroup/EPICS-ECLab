#include <iostream>
#include "BLFunctions.h"

#include <epicsExport.h>

extern "C" {
    epicsShareExtern void bl_test();
}

void bl_test()
{
    char version[32], mesg[255];
	unsigned ver_size = sizeof(version), mesg_size = sizeof(mesg);
	int status;
    if ( (status = BL_GetLibVersion(version, &ver_size)) == 0 )
	{
	    std::cout << "Library version " << version << std::endl;
	}
	else
	{
	    if ( BL_GetErrorMsg(status, mesg, &mesg_size) == 0)
		{
		    std::cout << "BL_GetLibVersion failed: " << mesg << std::endl;
		}
		else
		{
		    std::cout << "BL_GetLibVersion: Unknown error code " << status << std::endl;
		}
	}
}	
