/*----------------------------------------------------------------------------
    File containing some constant definitions used by QT steerer GUI.

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

