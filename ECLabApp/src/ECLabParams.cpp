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
	std::string label; // parameter name
	std::vector<TEccParam_t> value; // vector to handle array parameters
	technique_t() : update(false) { }
	technique_t(ParamType type_, const std::string& technique_, const std::string& label_) : update(false), type(type_), technique(technique_), label(label_) { }
	technique_t(const technique_t& t) : update(t.update), type(t.type), technique(t.technique), label(t.label), value(t.value) { }
};

typedef std::map<int, std::vector<technique_t> > techniqueMap_t; // maps asyn parameter id to technique parameters, vector to handle asyn address

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

static void addTechParam(asynPortDriver* driver, techniqueMap_t& the_map, const std::string& technique, int addr, const std::string& label, 
                         ParamType type, const std::string& param, const std::string& desc, int nelem = 1)
{
    int par_id = -1;
	std::string asyn_param = technique + "_" + param;
	if (addr != 0)
	{
		return;
	}
	// don't add explicilty to addr, par_id may vary betwween lists 
	if (driver->createParam(asyn_param.c_str(), ECLabToASYNType(type), &par_id) != asynSuccess || par_id == -1)
	{
		std::cerr << "Error Adding asyn parameter " << asyn_param << std::endl;
		return;
	}
	std::vector<technique_t>& ta = the_map[par_id];
	ta.resize(4, technique_t(type, technique, label)); // 4 is max number of asyn addresses
	std::cerr << "Adding asyn parameter \"" << asyn_param << "\" for technique \"" << technique << "\" label \"" << label << "\"" << std::endl;
}

void setECIntegerParam(asynPortDriver* driver, int addr, int id, epicsInt32 value)
{
    driver->setIntegerParam(addr, id, value);
	std::vector<technique_t>& ta = g_map[id];
	technique_t& t = ta[addr];
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
	std::vector<technique_t>& ta = g_map[id];
	technique_t& t = ta[addr];
	t.update = true;
	if (t.value.size() == 0)
	{
	    t.value.resize(1);
	}
	BL_DefineSglParameter(t.label.c_str(), static_cast<float>(value), 0, &(t.value[0]));
}	

void setECSingleArrayParam(asynPortDriver* driver, int addr, int id, epicsFloat32* value, int n)
{
	std::vector<technique_t>& ta = g_map[id];
	technique_t& t = ta[addr];
	t.update = true;
	if (t.value.size() < n)
	{
	    t.value.resize(n);
	}
	for(int i=0; i<n; ++i)
	{
	    BL_DefineSglParameter(t.label.c_str(), value[i], i, &(t.value[i]));
	}
	driver->doCallbacksFloat32Array(value, n, id, addr);
}	

void setECIntegerArrayParam(asynPortDriver* driver, int addr, int id, epicsInt32* value, int n)
{
	std::vector<technique_t>& ta = g_map[id];
	technique_t& t = ta[addr];
	t.update = true;
	if (t.value.size() < n)
	{
	    t.value.resize(n);
	}
	for(int i=0; i<n; ++i)
	{
	    BL_DefineIntParameter(t.label.c_str(), value[i], i, &(t.value[i]));
	}
	driver->doCallbacksInt32Array(value, n, id, addr);
}	

void setECBooleanArrayParam(asynPortDriver* driver, int addr, int id, epicsInt8* value, int n)
{
	std::vector<technique_t>& ta = g_map[id];
	technique_t& t = ta[addr];
	t.update = true;
	if (t.value.size() < n)
	{
	    t.value.resize(n);
	}
	for(int i=0; i<n; ++i)
	{
	    BL_DefineBoolParameter(t.label.c_str(), value[i], i, &(t.value[i]));
	}
	driver->doCallbacksInt8Array(value, n, id, addr);
}	

void getTechniqueParams(const std::string& technique, int addr, std::vector<TEccParam_t>& values, bool changes_only)
{
	for(techniqueMap_t::iterator it = g_map.begin(); it != g_map.end(); ++it)
	{	    
	    std::vector<technique_t>& ta = it->second;
		if ( addr >= ta.size() )
		{
			std::cerr << "technique addr array small" << std::endl;
		    continue;
		}
	    technique_t& t = ta[addr];
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

void printParams(asynPortDriver* driver, std::ostream& os)
{
	const char* paramName;
	for(techniqueMap_t::iterator it = g_map.begin(); it != g_map.end(); ++it)
	{
		driver->getParamName(it->first, &paramName);
	    std::vector<technique_t>& ta = it->second;
		for(int j=0; j<ta.size(); ++j)
		{
	        technique_t& t = ta[j];
		    os << "Param name " << paramName << " Technique " << t.technique << " addr " << j << " label " << t.label << std::endl;
		    for(int i=0; i<t.value.size(); ++i)
		    {
		        os << "[" << t.value[i].ParamIndex << "] " << t.value[i].ParamStr << std::endl;
		    }
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
