/*----------------------------------------------------------------------------
  Header file for ParameterHistory class for QT steerer GUI

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

#ifndef __PARAMETERHISTORY_H__
#define __PARAMETERHISTORY_H__

/// @brief Class providing storage and accessors for logged parameter data.
/// Used by the history plotting code.
/// @see HistoryPlot
/// @see HistorySubPlot
/// @author Mark Riding
/// @author Andrew Porter
/// @author Sue Ramsden
class ParameterHistory {
  public:
    ParameterHistory();
    ~ParameterHistory();
    /// Store the supplied value in mParamHistArray (append)
    void          updateParameter(const char* lVal);
    /// Returns the value of the element at index in mParamHistArray
    const float   elementAt(int index);
    /// Returns mParamHistArray
    double*       ptrToArray();

    /// Size of array pointed to by mParamHistArray
    int     mArraySize;
    /// Position at which to insert next new value in array pointed 
    /// to by mParamHistArray
    int     mArrayPos;
    /// Pointer to array holding data logged by the steering library
    /// _before_ steering client attached
    double *mPtrPreviousHistArray;
    /// Size of array pointed to by mPtrPreviousHistArray
    int     mPreviousHistArraySize;

 private:
    /// Used when realloc'ing memory for the mParamHistArray
    int     mArrayChunkSize;
    /// Array holding data that we've logged since being attached
    double *mParamHistArray;
};

#endif

