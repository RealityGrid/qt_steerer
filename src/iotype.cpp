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

/** @file iotype.cpp
    @brief IOType class for QT steerer GUI.

    @author Sue Ramsden
    @author Mark Riding
    @author Andrew Porter */

#include "buildconfig.h"
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
