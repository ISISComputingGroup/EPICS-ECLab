#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <asynPortDriver.h>
#include <BLStructs.h>

#include "ECLabParams.h"

techniqueMap_t g_map;

static asynParamType ECLabToASYNType(ParamType type)
{
    switch(type)
	{
      case Integer:
	      return asynParamInt32; 
      case IntegerArray:
	      return asynParamInt32Array; 
      case Boolean:
	      return asynParamInt32; 
      case BooleanArray:
	      return asynParamInt8Array; 
      case Single:
	      return asynParamFloat64; 
      case SingleArray:
	      return asynParamFloat32Array; 
    }
}

static void addTechParam(asynPortDriver* driver, techniqueMap_t& the_map, const std::string& technique, const std::string& label, 
                         ParamType type, const std::string& param, const std::string& desc, int nelem = 1)
{
    int par_id = -1;
	std::string asyn_param = technique + "_" + param;
	if (driver->createParam(asyn_param.c_str(), ECLabToASYNType(type), &par_id) == asynSuccess)
	{
		technique_t& t = the_map[par_id]; 
		t.update = false;
		t.type = type;
		t.technique = technique;
		t.label = label;
		std::cerr << "Adding asyn parameter \"" << asyn_param << "\" for technique \"" << technique << "\" label \"" << label << "\"" << std::endl;
	}
	else
	{
		std::cerr << "Error Adding asyn parameter " << asyn_param << std::endl;
	}
	
}

void addAllParameters(asynPortDriver* driver)
{
#include "BooleanParams.cpp"
#include "IntegerParams.cpp"
#include "SingleParams.cpp"
#include "BooleanArrayParams.cpp"
#include "IntegerArrayParams.cpp" 
#include "SingleArrayParams.cpp"
}
