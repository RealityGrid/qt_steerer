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

/** @file parameter.cpp
    @brief Parameter class for QT steerer GUI.

    Class represents a steered or monitored parameter.

    @author Sue Ramsden
    @author Mark Riding
    @author Andrew Porter */
#include "parameter.h"
#include "types.h"
#include "debug.h"

#include <q3table.h>


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
Parameter::printParameter(Q3Table *lTablePtr) const
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
