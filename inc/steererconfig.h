/*----------------------------------------------------------------------------
  ControlForm class header file for QT steerer GUI.

  (C) Copyright 2002, 2004, University of Manchester, United Kingdom,
  all rights reserved.

  This software is produced by the Supercomputing, Visualization and
  e-Science Group, Manchester Computing, University of Manchester
  as part of the RealityGrid project (http://www.realitygrid.org),
  funded by the EPSRC under grants GR/R67699/01 and GR/R67699/02.

  LICENCE TERMS

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

  THIS MATERIAL IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. THE ENTIRE RISK AS TO THE QUALITY
  AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE
  DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR
  CORRECTION.

  Initial version by Andrew Porter, 21.12.2005.

---------------------------------------------------------------------------*/

#ifndef _STEERERCONFIG_H_
#define _STEERERCONFIG_H_

using namespace std;
#include "ReG_Steer_Common.h"
#include <qdom.h>

/** @file steererconfig.h
    @brief Defines class holding configuration data
*/

class SteererConfig {
  public:

  // Member variables
  
  /** Address of the top-level registry */
  QString mTopLevelRegistry;
  /// Struct holding info for authentication to registry (via either
  /// ssl or WSSE)
  struct reg_security_info mRegistrySecurity;
  /** The passphrase for the user's private key */
  QString mKeyPassphrase;
  /** Whether automatic polling should be switched on by default */
  bool mAutoPollingOn;
  /** The default polling interval when not setting it automatically */
  float mPollingIntervalSecs;
  /** Whether or not to show the table of monitored parameters by default */
  bool mShowMonParamTable;
  /** Whether or not to show the table of steerable parameters by default */
  bool mShowSteerParamTable;
  /** Whether or not to show the table of IOTypes by default */
  bool mShowIOTypeTable;
  /** Whether or not to show the table of ChkTypes by default */
  bool mShowChkTypeTable;

  SteererConfig();
  ~SteererConfig();

  /** Write a configuration file for the steering client using 
      current settings */
  void writeConfig(QString fileName);
  /** Read configuration file and store values in member variables 
      of this class */
  void readConfig(QString fileName);
  /** Read security configuration file and store values in member variables 
      of this class */
  void readSecurityConfig(QString fileName);

  /** Get the value (from the "value" attribute) of the named 
      element which is to be found somewhere below the elem element */
  QString getElementAttrValue(QDomElement elem, 
			      QString     name);
};

#endif
