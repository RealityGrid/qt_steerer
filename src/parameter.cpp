/*----------------------------------------------------------------------------
    Parameter class for QT steerer GUI.
    Class represents a steered or monitored parameter.

    (C)Copyright 2002 The University of Manchester, United Kingdom,
    all rights reserved.

    This software is produced by the Supercomputing, Visualization &
    e-Science Group, Manchester Computing, the Victoria University of
    Manchester as part of the RealityGrid project.

    This software has been tested with care but is not guaranteed for
    any particular purpose. Neither the copyright holder, nor the
    University of Manchester offer any warranties or representations,
    nor do they accept any liabilities with respect to this software.

    This software must not be used for commercial gain without the
    written permission of the authors.
    
    This software must not be redistributed without the written
    permission of the authors.

    Permission is granted to modify this software, provided any
    modifications are made freely available to the original authors.
 
    Supercomputing, Visualization & e-Science Group
    Manchester Computing
    University of Manchester
    Manchester M13 9PL
    
    WWW:    http://www.sve.man.ac.uk  
    email:  sve@man.ac.uk
    Tel:    +44 161 275 6095
    Fax:    +44 161 275 6800    

    Initial version by: S Ramsden, 03.10.2002
    
---------------------------------------------------------------------------*/


#include "parameter.h"
#include "types.h"
#include "debug.h"

#include <qtable.h>


Parameter::Parameter(int aId, int aType, bool aSteerable)
  : mSteerable(aSteerable), mRegisteredFlag(true), mPresentFlag(true), 
    mRowIndex(-1), mId(aId), mType(aType)
{
  DBGCON("Parameter");
  // Create a parameter object - this holds information about a steered or monitored parameter
  // Note that not all data is stored in the class - some is simply displayed ( and updated)
  // in the gui (in table)

  // mRowIndex stored the index of the row in the table that diaplays this parameter - this
  // is needed to update the table when the parameter data changes

  // mId is the parameter handle assigned by ReG library
  // mType is the type of the parameter data

  // Note that it is possible for an application to unregister a parameter 
  // it has previously registered - mRegisteredFlag holds this state 
  // mPresentFlag is used to identify unregistered parameters
  
}

Parameter::~Parameter()
{
  DBGDST("Parameter");
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
  printf("   Value is %s \n", lTablePtr->text(getRowIndex(), kVALUE_COLUMN).latin1());
  if (mSteerable)
    printf("   NewValue is %s \n", lTablePtr->text(getRowIndex(), kNEWVALUE_COLUMN).latin1());
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
Parameter::isPresent() const 
{ 
  return mPresentFlag; 
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

void 
Parameter::setIsPresent()
{
  // set this as used later to identify unRegistered parameter
  mPresentFlag = true;
}

// MR:
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

