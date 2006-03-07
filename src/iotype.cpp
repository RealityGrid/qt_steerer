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

/** @file iotype.cpp
    @brief IOType class for QT steerer GUI. 

    @author Sue Ramsden
    @author Mark Riding
    @author Andrew Porter */

#include "types.h"
#include "debug.h"
#include "iotype.h"

IOType::IOType(int aId, int aIOTypeType)
  : mId(aId), mType(aIOTypeType), mNewFrequency(kNULL_FREQ)
{
  REG_DBGCON("IOType");
  // Create an iotype object - this holds information about a sample or checkpont IOType.
  // Note that not all data is stored in the class - some is simply displayed ( and updated)
  // in the gui (in table)

  // Note: There are several similarities with the Parameter class - initially separate classes
  // have been implemented as conceptually these are different.

  // mRowIndex stored the index of the row in the table that diplays this IOType - this
  // is needed to update the table when the iotype data changes

  // mId is the iotype handle assigned by ReG library (and this is a command)
  // mType is the type of the IOType - Sample can be IN or OUT,  CheckPoint can be IN, OUT or INOUT

  // Once registered IOTypes cannot be unregistered by the steered application

  // All iotypes support on demand emit/consume (i.e. user demands via gui) and 
  // auto emit/consume as dictated by frequency.  Auto consume is NOT relevant to checkpoint types.

}

IOType::~IOType()
{
   REG_DBGDST("IOType");
}

void 
IOType::printIOType() const
{
  REG_DBGMSG2("IOType handle/Type", getId(), getType());
}

int
IOType::getId() const
{
  return mId;
}

int
IOType::getType() const
{
  return mType;
}

int
IOType::getRowIndex() const
{
  return mRowIndex;
}

int
IOType::getFrequency() const
{
  return mNewFrequency;
}

void
IOType::setIndex(const int aRowIndex) 
{
  mRowIndex = aRowIndex;
}  


bool
IOType::validateAndSetFrequency(const int aFreq)
{
  // validate and if OK set the mNewFrequency value as entered on GUI
  // Note: at this point GUI has already validated that user has entered an integer

  // SMR XXX future:  add iotype specific validation here when iotype ranges specified 

  bool lValid = false;
  
  // note - user (unknowingly) sets null frequency by clearing the cell
  // a null freqency is valid and indicates that no new frequency has been specifed for this IOType
  if (aFreq >= kNULL_FREQ)
  {

    // for in-samples can only have freq 0 or 1
    //SMR commetn out this restriction as cannot now see benefit of it
//SMR    if (mType == REG_IO_IN && aFreq > 1)
//SMR      lValid = false;
//SMR    else
//SMR    {
      mNewFrequency = aFreq;
      lValid = true;
//SMR    }
  }
  
  return lValid;

}

void
IOType::setNullFrequency()
{
  mNewFrequency = kNULL_FREQ;
}

int
IOType::getAndResetFrequency()
{
  int lTmpFreq = mNewFrequency;
  mNewFrequency = kNULL_FREQ;
  return lTmpFreq;
}
