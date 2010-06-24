/*
  The RealityGrid Steerer

  Copyright (c) 2002-2010, University of Manchester, United Kingdom.
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

  Author: Mark Riding
          Andrew Porter
          Sue Ramsden
          Robert Haines
 */

/** @file parameter.h
    @brief Header file describing the Parameter class */

#ifndef __PARAMETER_H__
#define __PARAMETER_H__

#include <qstring.h>

#include "parameterhistory.h"

class Q3Table;

/// Container for information on a single parameter registered by a
/// steerable application. The parameter object holds information about
/// a steered or monitored parameter.
/// Note that not all data is stored in the class - some is simply
/// displayed ( and updated) in the gui (in a ParameterTable).
/// Note that it is theoretically possible for an application to
/// unregister a parameter it has previously registered
/// - mRegisteredFlag holds this state
/// @author Mark Riding
/// @author Andrew Porter
/// @author Sue Ramsden
class Parameter
{

public:

  Parameter(int aId, int aType, bool aSteerable,
	    QString aLabel);
  ~Parameter();

  void printParameter(Q3Table *) const;

  /// Get the handle of the parameter
  int getId() const;
  /// Get the type of the parameter (encoded as int)
  int getType() const;
  /// Get the index of the row of the table displaying
  /// this parameter
  int getRowIndex() const;
  /// In case parameter has been unregistered
  bool isRegistered() const;
  /// Get whether or not this parameter is steerable
  bool isSteerable() const;
  /// Whether parameter is currently registered with
  /// steering library
  bool checkRegistered();

  void setIndex(int aIndex);
  void unRegister();

  void setMinMaxStrings(const char *min, const char *max);
  /// Return string containing minimum value of parameter
  QString getMinString();
  /// Return string containing maximum value of parameter
  QString getMaxString();
  /// Return string containing the label of the parameter
  QString getLabel();
  /// Pointer to the ParameterHistory object for this parameter
  ParameterHistory  *mParamHist;
  /// Whether or not the steering client has retrieved the full
  /// history of this parameter
  bool mHaveFullHistory;

private:
  /// Whether or not this parameter is steerable
  const	bool	mSteerable;
  /// Whether or not this parameter is currently registered by
  /// the steered application
  bool		mRegisteredFlag;
  bool		mPresentFlag;
  /// Stores the index of the row in the table that diaplays this
  /// parameter - this is needed to update the table when the
  /// parameter data changes
  int		mRowIndex;
  /// The handle of this parameter as assigned by the steering lib
  const int	mId;
  /// The type of this parameter as assigned by the steering lib
  const int	mType;
  /// Minimum value of this parameter (if any)
  QString mMinStr;
  /// Maximum value of this parameter (if any)
  QString mMaxStr;
  /// The label given this parameter by the application code
  QString mLabel;
};


#endif
