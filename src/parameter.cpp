/*----------------------------------------------------------------------------
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
    
---------------------------------------------------------------------------*/

/** @file parameter.cpp
    @brief Parameter class for QT steerer GUI.
 
    Class represents a steered or monitored parameter.

    @author Sue Ramsden
    @author Mark Riding
    @author Andrew Porter */
#include "parameter.h"
#include "types.h"
#include "debug.h"

#include <qtable.h>


Parameter::Parameter(int aId, int aType, bool aSteerable,
		     QString aLabel)
  : mSteerable(aSteerable), mRegisteredFlag(true),
    mPresentFlag(true), mRowIndex(-1), mId(aId), mType(aType), 
    mLabel(aLabel)
{
  REG_DBGCON("Parameter constructor");
  mParamHist = new ParameterHistory;
  mHaveFullHistory = false;
}

Parameter::~Parameter()
{
  REG_DBGDST("Parameter");
 delete mParamHist;
}

bool
Parameter::checkRegistered()
{
  // if mPresent is false, ReG library has not returned this parameter 
  // in the last Get_param_values() call hence
  // it must have been unregistered by the application
  // So set mRegisteredFlag to indicate this and return it

  // SMR XXX what about a re-register - check this creates new param with new ID?

  if (!mPresentFlag)
    mRegisteredFlag = false;

  // reset mPresentFlag for next library update
  mPresentFlag = false;

  return mRegisteredFlag;

}

bool 
Parameter::isSteerable() const 
{
  return mSteerable;
}


void
Parameter::printParameter(QTable *lTablePtr) const
{
  printf("Parameter handle=%d, index=%d\n", getId(), getRowIndex());
  printf("   Value is %s \n", 
	 lTablePtr->text(getRowIndex(), kVALUE_COLUMN).latin1());
  if (mSteerable)
    printf("   NewValue is %s \n", 
	   lTablePtr->text(getRowIndex(), kNEWVALUE_COLUMN).latin1());
}

int 
Parameter::getId() const 
{
  return mId; 
}

int 
Parameter::getType() const 
{
  return mType;
}

int 
Parameter::getRowIndex() const  
{
  return mRowIndex;
}
 
bool 
Parameter::isRegistered() const 
{ 
  return mRegisteredFlag; 
}

void
Parameter::setIndex(int aIndex)
{
   mRowIndex = aIndex;
}

void
Parameter::unRegister() 
{ 
  mRegisteredFlag = false; 
}

void Parameter::setMinMaxStrings(const char *min, const char *max){
  // Make deep copy of the passed strings
  mMinStr = min;
  mMaxStr = max;
}

QString Parameter::getMinString(){
  return mMinStr;
}

QString Parameter::getMaxString(){
  return mMaxStr;
}

QString Parameter::getLabel(){
  return mLabel;
}
