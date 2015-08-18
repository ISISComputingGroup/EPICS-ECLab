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

typedef std::map<std::string, std::string> fakeTEccParams_t;

class Technique
{
public:
  Technique(std::string name, int index, int totalArraySize);
  std::string name;
  int index;
  fakeTEccParams_t TEccParams_t1;
  bool first;
  bool last;

private:
  void getTEccParams();
  void setFirstLast(int totalArraySize);


};