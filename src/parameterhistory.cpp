/*
  The RealityGrid Steerer

  Copyright (c) 2002-2009, University of Manchester, United Kingdom.
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
  if(lVal[0] != '\0'){
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


