/*************************************************************************\ 
* Copyright (c) 2013 Science and Technology Facilities Council (STFC), GB. 
* All rights reverved. 
* This file is distributed subject to a Software License Agreement found 
* in the file LICENSE.txt that is included with this distribution. 
\*************************************************************************/ 

/// @file ECLabParams.cpp
/// @author Freddie Akeroyd, STFC ISIS Facility, GB

#include <string>
#include <map>
#include <iostream>
#include <epicsString.h>
#include "asynPortDriver.h"
#include "ECLabParams.h"

enum ParamType { Single, Boolean, Integer, SingleArray, BooleanArray, IntegerArray };

struct technique_t 
{ 
    bool update; 
	ParamType type; 
	std::string technique; 
	std::string label; 
	std::vector<TEccParam_t> value; 
};

typedef std::map<int, technique_t> techniqueMap_t;

static techniqueMap_t g_map;

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
	  default:
	      return asynParamInt32;
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

void setECIntegerParam(asynPortDriver* driver, int addr, int id, epicsInt32 value)
{
    driver->setIntegerParam(addr, id, value);
	technique_t& t = g_map[id];
	t.update = true;
	if (t.value.size() == 0)
	{
	    t.value.resize(1);
	}
	if (t.type == Boolean)
	{
	    BL_DefineBoolParameter(t.label.c_str(), (value != 0 ? true : false), 0, &(t.value[0]));
	}
	else
	{
	    BL_DefineIntParameter(t.label.c_str(), value, 0, &(t.value[0]));
	}
}	

void setECSingleParam(asynPortDriver* driver, int addr, int id, epicsFloat64 value)
{
    driver->setDoubleParam(addr, id, value);
	technique_t& t = g_map[id];
	t.update = true;
	if (t.value.size() == 0)
	{
	    t.value.resize(1);
	}
	BL_DefineSglParameter(t.label.c_str(), static_cast<float>(value), 0, &(t.value[0]));
}	

void getTechniqueParams(const std::string& technique, int addr, std::vector<TEccParam_t>& values, bool changes_only)
{
	for(techniqueMap_t::iterator it = g_map.begin(); it != g_map.end(); ++it)
	{	    
	    technique_t& t = it->second;
		if (changes_only && !t.update)
		{
		    continue;
		}
		if ( epicsStrCaseCmp(t.technique.c_str(), technique.c_str()) == 0 )
		{
		    values.insert(values.end(), t.value.begin(), t.value.end());
			t.update = false;
		}
	}
}

void printParams(std::ostream& os)
{
	for(techniqueMap_t::iterator it = g_map.begin(); it != g_map.end(); ++it)
	{	    
	    technique_t& t = it->second;
		os << "Technique " << t.technique << " " << t.label << std::endl;
		for(int i=0; i<t.value.size(); ++i)
		{
		    os << t.value[i].ParamStr << std::endl;
		}
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
