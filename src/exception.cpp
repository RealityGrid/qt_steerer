/*----------------------------------------------------------------------------
  Exception class for QT steerer GUI.

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


#include "exception.h"
#include "types.h"

#include <string.h>
#include <iostream>
using namespace std;

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
