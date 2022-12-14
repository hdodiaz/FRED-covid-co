/*
  This file is part of the FRED system.

  Copyright (c) 2010-2015, University of Pittsburgh, John Grefenstette,
  Shawn Brown, Roni Rosenfield, Alona Fyshe, David Galloway, Nathan
  Stone, Jay DePasse, Anuroop Sriram, and Donald Burke.

  Licensed under the BSD 3-Clause license.  See the file "LICENSE" for
  more information.
*/

//
//
// File: Vaccine_Priority_Policies.h
//

#ifndef _FRED_VACCINE_PRIORITY_POLICIES_H
#define _FRED_VACCINE_PRIORITY_POLICIES_H

#include <iostream>
#include <string>

#include "Policy.h"

class Decision;
class Person;
class Vaccines;
class Vaccine_Manager;
class Manager;

using namespace std;

class Vaccine_Priority_Policy_No_Priority: public Policy {
  Vaccine_Manager *vacc_manager;
  
public:
  Vaccine_Priority_Policy_No_Priority() { }
  Vaccine_Priority_Policy_No_Priority(Vaccine_Manager* vcm);
};

class Vaccine_Priority_Policy_Specific_Age:public Policy {
  Vaccine_Manager *vacc_manager;
  
public:
  Vaccine_Priority_Policy_Specific_Age();
  Vaccine_Priority_Policy_Specific_Age(Vaccine_Manager* vcm);
};  

class Vaccine_Priority_Policy_ACIP:public Policy {
  Vaccine_Manager *vacc_manager;
  
public: 
  Vaccine_Priority_Policy_ACIP();
  Vaccine_Priority_Policy_ACIP(Vaccine_Manager* vcm);
};

// Policies that rely on phases existing
class Vaccine_Priority_Policy_Phase_Age:public Policy {
  Vaccine_Manager *vacc_manager;
  
public:
  Vaccine_Priority_Policy_Phase_Age();
  Vaccine_Priority_Policy_Phase_Age(Vaccine_Manager* vcm);
};  

class Vaccine_Priority_Policy_Phase_Essential_Workers:public Policy {
  Vaccine_Manager *vacc_manager;
  
public:
  Vaccine_Priority_Policy_Phase_Essential_Workers();
  Vaccine_Priority_Policy_Phase_Essential_Workers(Vaccine_Manager* vcm);
};  

class Vaccine_Priority_Policy_Phase_Teachers:public Policy {
  Vaccine_Manager *vacc_manager;
  
public:
  Vaccine_Priority_Policy_Phase_Teachers();
  Vaccine_Priority_Policy_Phase_Teachers(Vaccine_Manager* vcm);
};  

class Vaccine_Priority_Policy_Phase_Comorbidities:public Policy {
  Vaccine_Manager *vacc_manager;
  
public:
  Vaccine_Priority_Policy_Phase_Comorbidities();
  Vaccine_Priority_Policy_Phase_Comorbidities(Vaccine_Manager* vcm);
};  

class Vaccine_Priority_Policy_Phase_LTC:public Policy {
  Vaccine_Manager *vacc_manager;
  
public:
  Vaccine_Priority_Policy_Phase_LTC();
  Vaccine_Priority_Policy_Phase_LTC(Vaccine_Manager* vcm);
};  

class Vaccine_Priority_Policy_Phase_No_Priority: public Policy {
  Vaccine_Manager *vacc_manager;
  
public:
  Vaccine_Priority_Policy_Phase_No_Priority() { }
  Vaccine_Priority_Policy_Phase_No_Priority(Vaccine_Manager* vcm);
};

#endif
