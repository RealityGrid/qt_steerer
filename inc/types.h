/*----------------------------------------------------------------------------
  File containing some constant definitions used by QT steerer GUI.

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


#ifndef __TYPES_H__
#define __TYPES_H__

// steering library
#define REG_STR_CPP 1
#include "ReG_Steer_types.h"

#define kNULL				  0

//SMR XXX move these to table class files??

// parameter table constants
#define kID_COLUMN		0
#define kNAME_COLUMN		1
#define kREG_COLUMN		2
#define kVALUE_COLUMN		3
#define kNEWVALUE_COLUMN	4

//parameter tables sizes
#define kNUM_MON_COLUMNS	4
#define kNUM_STEER_COLUMNS	5 
// MR: removed - was causing problems, and giving no noticeable benefit
//#define kINIT_ROWS		0//3


//iotype table constants
#define kIO_ID_COLUMN		0
#define kIO_NAME_COLUMN		1
#define kIO_VALUE_COLUMN	2
#define kIO_NEWVALUE_COLUMN	3
#define kIO_REQUEST_COLUMN	4
#define kIO_RESTART_COLUMN	5  // chkpoint only


//iotype tables sizes
#define kNUM_IO_COLUMNS		5  // not include kIO_RESTART_COLUMN
// MR: removed - was causing problems, and giving no noticeable benefit
//#define kIO_INIT_ROWS		0//2

// iotype new frequency not set indicator
#define kNULL_FREQ		-1

// iotype checkpoint null restart index
#define kNULL_INDX		-1

// checkpoint command parameter string length
#define kCHKPT_PARAM_LEN	REG_MAX_STRING_LENGTH + 5


// minimum poling interval (milliseconds)from commsthread Get_next_message
#define kMIN_POLING_INT		100


// Unique numbers to make QCustomEvent IDs for postEvent from  CommsThread.cpp
#define kMSG_EVENT		100
#define kSIGNAL_EVENT		200
 


#endif

