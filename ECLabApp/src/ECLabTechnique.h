/*************************************************************************\ 
* Copyright (c) 2013 Science and Technology Facilities Council (STFC), GB. 
* All rights reserved. 
* This file is distributed subject to a Software License Agreement found 
* in the file LICENSE.txt that is included with this distribution. 
\*************************************************************************/ 

/// @file ECLabTechnique.h Header for #ECLabTechnique class.
/// @author Lottie Greenwood, STFC ISIS Facility, GB

#ifndef ECLABTECHNIQUE_H
#define ECLABTECHNIQUE_H

#include<iostream>
#include<string>
#include<map>

#include<ECLabParams.h>

class ECLabTechnique
{
public:
  ECLabTechnique(std::string name, int index);
  std::string name;
  int index;
  bool first;
  bool last;
  std::vector<TEccParam_t> values;
  

private:
  void getTEccParams();


};

#endif
