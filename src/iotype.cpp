/*----------------------------------------------------------------------------
    IOType class for QT steerer GUI. 

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

#include "types.h"
#include "debug.h"
#include "iotype.h"

IOType::IOType(int aId, int aIOTypeType)
  : mId(aId), mType(aIOTypeType), mNewFrequency(kNULL_FREQ)
{
  DBGCON("IOType");
  // Create an iotype object - this holds information about a sample or checkpont IOType.
  // Note that not all data is stored in the class - some is simply displayed ( and updated)
  // in the gui (in table)

  // Note: There are several similarities with the Parameter class - initially separate classes
  // have been implemented as conceptually these are different.

  // mRowIndex stored the index of the row in the table that diplays this IOType - this
  // is needed to update the table when the iotype data changes

  // mId is the iotype handle assigned by ReG library (and this is a command)
  // mType is the type of the IOType - SampleIn, SampleOut or CheckPoint

  // Once registered IOTypes cannot be unregistered by the steered application

  // All iotypes support on demand emit/comsume (i.e. user demands via gui) and 
  // auto emit/consume as dictated by frequency.

}

IOType::~IOType()
{
   DBGDST("IOType");
}

void 
IOType::printIOType() const
{
  DBGMSG2("IOType handle/Type", getId(), getType());
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
    if (mType == REG_IO_IN && aFreq > 1)
      lValid = false;
    else
    {
      mNewFrequency = aFreq;
      lValid = true;
    }
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
