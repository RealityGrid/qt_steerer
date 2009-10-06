/*
  The RealityGrid Steerer

  Copyright (c) 2002-2009, University of Manchester, United Kingdom.
  All rights reserved.

  This software is produced by Research Computing Services, University
  of Manchester as part of the RealityGrid project and associated
  follow on projects, funded by the EPSRC under grants GR/R67699/01,
  GR/R67699/02, GR/T27488/01, EP/C536452/1, EP/D500028/1,
  EP/F00561X/1.

  LICENCE TERMS

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of The University of Manchester nor the names
      of its contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

  Author: Andrew Porter
          Robert Haines
 */

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
