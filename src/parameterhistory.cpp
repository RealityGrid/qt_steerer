/*----------------------------------------------------------------------------
  ParameterHistory class for QT steerer GUI.

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

#include "parameterhistory.h"
#include <stdio.h>
#include <stdlib.h>

ParameterHistory::ParameterHistory(){
  mArrayChunkSize = 1024;
  mArraySize = mArrayChunkSize;
  mArrayPos = 0;
  mParamHistArray = (double *)malloc(mArraySize*sizeof(double));
  mPtrPreviousHistArray = NULL;
  mPreviousHistArraySize = 0;
}

ParameterHistory::~ParameterHistory(){
  if(mParamHistArray)free(mParamHistArray);
}

// Bear in mind that the current implementation will just sit
// eating up memory until the job is over... need to do something
// a bit better and spool to file
void ParameterHistory::updateParameter(const char* lVal){
  if(mArrayPos < mArraySize){
    mParamHistArray[mArrayPos++] = (double)atof(lVal);
  }
  else{
    void *dum = realloc((void *)mParamHistArray, 
			(size_t)(mArraySize+mArrayChunkSize)*sizeof(double));
    if(dum){
      mParamHistArray = (double *)dum;
      mArraySize += mArrayChunkSize;
      mParamHistArray[mArrayPos++] = (double)atof(lVal);
    }
  }
}

const float ParameterHistory::elementAt(int index){

  if(index > 0 && index < mArrayPos){
    return (float)mParamHistArray[index];
  }
  else{
    return 0.0;
  }
}

double* ParameterHistory::ptrToArray(){
  return mParamHistArray;
}


