/*----------------------------------------------------------------------------
    Exception class for QT steerer GUI.

    (C)Copyright 2002 The University of Manchester, United Kingdom,
    all rights reserved.

    This software is produced by the Supercomputing, Visualization &
    e-Science Group, Manchester Computing, the Victoria University of
    Manchester as part of the RealityGrid project.

    This software has been tested with care but is not guaranteed for
    any particular purpose. Neither the authors, nor the University of
    Manchester offer any warranties or representations, nor do they
    accept any liabilities with respect to this software.

    This program must not be used for commmercial gain without the
    written permission of the authors.
    
    Supercomputing, Visualization & e-Science Group
    Manchester Computing
    University of Manchester
    Manchester M13 9PL

    email:  csar-advice@cfs.ac.uk.
    Tel:    +44 161 275 6824/5997
    Fax:    +44 161 275 6040    
    
    Date          Version    Updates                            Author
    ----          -------    -------                            ------
    03.10.2002      0.1                                         S Ramsden

---------------------------------------------------------------------------*/

#include "exception.h"
#include "types.h"

#include <stdio.h>
#include <string.h>
#include <iostream.h>

SteererException::SteererException(const char * const aMsgPtr)
  :  mFilenamePtr(kNULL), mLineNumber(0)
{
  int lLen = strlen(aMsgPtr);
  mMsgPtr = new char [ lLen+1 ];
  strcpy(mMsgPtr, aMsgPtr);
}
 
SteererException::SteererException(const char * const aMsgPtr, const char* const aFileNamePtr, 
const int aLineNumber)
  : mFilenamePtr(aFileNamePtr), mLineNumber(aLineNumber)
{
  mMsgPtr = new char [ strlen(aMsgPtr)+1 ];
  strcpy(mMsgPtr, aMsgPtr);
}

SteererException::~SteererException()
{
  delete [] mMsgPtr;
}

// the essential copy constructor
SteererException::SteererException(const SteererException& aCopyEx)
: mFilenamePtr(aCopyEx.mFilenamePtr), mLineNumber(aCopyEx.mLineNumber)
{
  mMsgPtr = new char [ strlen(aCopyEx.getErrorMsg())+1 ];
  strcpy(mMsgPtr, aCopyEx.getErrorMsg());
}


const char* const
SteererException::getErrorMsg() const
{
  return mMsgPtr;
}

void 
SteererException::print () const 
{ 
  cerr << "Exception: " << mMsgPtr ;
  if (mFilenamePtr != kNULL)
    cerr << " at: " <<  mFilenamePtr << " line:" << mLineNumber;

  cerr << endl;
}
