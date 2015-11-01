#include <vector>
#include "BLFunctions.h"
#include <epicsTypes.h>
class asynPortDriver;

extern void addAllParameters(asynPortDriver* driver);

extern void setECIntegerParam(asynPortDriver* driver, int addr, int id, epicsInt32 value);
extern void setECSingleParam(asynPortDriver* driver, int addr, int id, epicsFloat64 value);

extern void getTechniqueParams(const std::string& technique, int addr, std::vector<TEccParam_t>& values, bool changes_only);
extern void printParams(std::ostream& os);