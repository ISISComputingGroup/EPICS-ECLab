#include<string>
#include<iostream>
#include<map>
enum ParamType { Single, Integer, SingleArray};
struct mystruct_t {bool update; ParamType type; TEccParams Params;};
typedef std::map<std::string, mystruct_t> mytechnique_t;
typedef std::map<std::string, mytechnique_t> techniqueMap_t;

techniqueMap_t addTechParam(techniqueMap_t g_map, std::string technique, std::string parameter, ParamType type)
{
  g_map[technique][parameter].type = type;
  return g_map;
}

techniqueMap_t globalMapGen()
{	
  techniqueMap_t g_map;
  g_map = addTechParam(g_map, "TO", "Trigger_Logic", Integer);
  g_map = addTechParam(g_map, "ca", "Voltagestep", SingleArray);
  g_map = addTechParam(g_map, "ocv", "Rest_time_T", Single);
	return g_map;
}