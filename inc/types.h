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

#ifndef __TYPES_H__
#define __TYPES_H__

// steering library
#define REG_STR_CPP 1
#include "ReG_Steer_types.h"

#define kNULL                   0

/// parameter table constants
#define kID_COLUMN		0
#define kNAME_COLUMN		1
#define kREG_COLUMN		2
#define kVALUE_COLUMN		3
#define kNEWVALUE_COLUMN	4

/// parameter tables sizes
#define kNUM_MON_COLUMNS	4
#define kNUM_STEER_COLUMNS	5 
// MR: removed - was causing problems, and giving no noticeable benefit
//#define kINIT_ROWS		0//3


/// iotype table constants
#define kIO_ID_COLUMN		0
#define kIO_NAME_COLUMN		1
#define kIO_VALUE_COLUMN	2
#define kIO_NEWVALUE_COLUMN	3
#define kIO_REQUEST_COLUMN	4
#define kIO_RESTART_COLUMN	5  // chkpoint only


/// iotype tables sizes
#define kNUM_IO_COLUMNS		5  // not include kIO_RESTART_COLUMN
// MR: removed - was causing problems, and giving no noticeable benefit
//#define kIO_INIT_ROWS		0//2

/// iotype new frequency not set indicator
#define kNULL_FREQ		-1

/// iotype checkpoint null restart index
#define kNULL_INDX		-1

/// checkpoint command parameter string length
#define kCHKPT_PARAM_LEN	REG_MAX_STRING_LENGTH + 5


/// minimum polling interval (milliseconds) from commsthread 
/// Get_next_message - a SOAP call takes ~0.015 seconds at best
/// so no point polling more frequently than that
#define kMIN_POLLING_INT	100
#define kMAX_POLLING_INT        2000

/// Unique numbers to make QCustomEvent IDs for postEvent 
/// from CommsThread.cpp
#define kMSG_EVENT		100
#define kSIGNAL_EVENT		200
 
/// Maximum number of plots in a single history plot
#define kMAX_HISTORY_PLOTS      10

#endif

