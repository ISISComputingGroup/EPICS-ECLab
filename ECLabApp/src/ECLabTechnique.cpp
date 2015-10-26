#include<ECLabTechnique.h>

ECLabTechnique::ECLabTechnique(std::string name, int index) 
{
  ECLabTechnique::index = index;
  ECLabTechnique::name = name;
  if (index == 0)
  {
	ECLabTechnique::first = true;
  }
  else{ECLabTechnique::first = false;}
  ECLabTechnique::getTEccParams();
}

void ECLabTechnique::getTEccParams()
{
  getTechniqueParams(ECLabTechnique::name, ECLabTechnique::values, false);
  //print out something useful here
}