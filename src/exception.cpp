/*----------------------------------------------------------------------------
    Exception class for QT steerer GUI.

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


const char*
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
