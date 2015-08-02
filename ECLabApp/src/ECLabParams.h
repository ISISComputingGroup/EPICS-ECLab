
enum ParamType { Single, Boolean, Integer, SingleArray, BooleanArray, IntegerArray };
struct technique_t { bool update; ParamType type; std::string technique; std::string label; std::vector<TEccParam_t> value; };
typedef std::map<int, technique_t> techniqueMap_t;

extern void addAllParameters(asynPortDriver* driver);
extern techniqueMap_t g_map;
