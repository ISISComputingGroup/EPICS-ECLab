Technique::Technique(std::string name, int index, int totalArraySize) 
{
  Technique::index = index;
  Technique::name = name;
  Technique::setFirstLast(totalArraySize);
  Technique::getTEccParams();
}

void Technique::setFirstLast(int totalArraySize)
{

  if (Technique::index == 0)
  {
    Technique::first = true;
  }
  else {Technique::first = false;}
  if (totalArraySize - Technique::index == 0)
  {
    Technique::last = true;
  }
  else {Technique::last = false;}

}

void Technique::getTEccParams()
{
  /*auto techName =  g_map.at(Technique::name);
  for(auto iterator = techName.begin(); iterator !=techName.end(); iterator++)
  {
  std::cout << iterator->second.type << std::endl; //will use this to cast as correct type when putting into TEccParams
  Technique::TEccParams_t1[iterator->first] = iterator->second.Params;
  }*/

}