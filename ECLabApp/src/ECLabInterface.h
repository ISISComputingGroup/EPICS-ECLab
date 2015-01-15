#ifndef ECLabInterface_H
#define ECLabInterface_H

#include <string>
#include <sstream>
#include <BLFunctions.h>

/// encapsulates an error code as an exception object
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

 
#define BL_CALL(__func, ... ) \
    int status = __func ( __VA_ARGS__ ); \
    if ( status != 0 ) \
	{ \
	    throw ECLabException(#__func, status); \
	}
	
/// Wrap all of the ECLab BL_* function in an interface class that converts 
/// the error code into an exception
struct ECLabInterface
{
    void GetLibVersion(char*  pVersion, unsigned int* psize)
	{
	    BL_CALL(BL_GetLibVersion, pVersion, psize);	    
	}
};

#endif /* ECLabInterface_H */