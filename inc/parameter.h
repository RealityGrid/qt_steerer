/*----------------------------------------------------------------------------
  Header file for Parameter class for QT steerer GUI.

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

  Authors........: Mark Riding, Andrew Porter, Sue Ramsden
   
---------------------------------------------------------------------------*/


#ifndef __PARAMETER_H__
#define __PARAMETER_H__

#include "parameterhistory.h"
#include <qstring.h>

class QTable;

class Parameter
{

public:

  Parameter(int aId, int aType, bool aSteerable);
  ~Parameter();
  
  void printParameter(QTable *) const;  

  int getId() const;
  int getType() const;
  int getRowIndex() const;
  bool isPresent() const;
  bool isRegistered() const;
  bool isSteerable() const;

  bool checkRegistered();

  void setIndex(int aIndex);
  void setIsPresent();
  void unRegister();

  // MR:
  void setMinMaxStrings(const char *min, const char *max);
  QString getMinString();
  QString getMaxString();

  ParameterHistory  *mParamHist;
  
private:
  const	bool	mSteerable;
  bool		mRegisteredFlag;
  bool		mPresentFlag;

  int		mRowIndex;
  const int	mId;
  const int	mType;

  // MR: Store these as strings since we don't know the type,
  //     and also since we'll be using them as strings at some
  //     point anyhow
  QString mMinStr;
  QString mMaxStr;

};


#endif


