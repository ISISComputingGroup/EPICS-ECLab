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
};

#endif /* ECLabInterface_H */