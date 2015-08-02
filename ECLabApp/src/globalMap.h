

enum ParamType { Single, Integer, SingleArray}
struct mystruct_t {bool update; ParamType; TECCParam Params;}
std::map<string, mystruct_t> mytechnique_t
std::map<string, mytechnique_t> techniqueMap_t

addTechParam(techniqueMap_t g_map, std::string technique, std::string parameter, enum type)
{
	g_map[technique][parameter].ParamType = type;
		 
	#std::list<mytechnique_t>
}

techniqueMap_t globalMapGen()
{	
	techniqueMap_t g_map;
	#init it as func in diff file
		
	mytechnique_t tmp;
	addTechParam(g_map, TO, Trigger_Logic, Integer);
	addTechParam(g_map, ca, Voltagestep, SingleArray); #lots more on pg 76
	addTechParam(g_map, ocv, Rest_time_T, single); #pg68

	return g_map;
}

/* so a map must contain the name of the technique file, and then connected to that, the name of the parameter, then whether param is updated, what type it is, and a pointer to the TECCParams collective */