/*----------------------------------------------------------------------------
    File containing sone constant definitions used by QT steerer GUI.

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
#define kSPLIT			4
#define kNEWVALUE_COLUMN	5

//parameter tables sizes
#define kNUM_MON_COLUMNS	4
#define kNUM_STEER_COLUMNS	6
#define kINIT_ROWS		4


//iotype table constants
#define kIO_ID_COLUMN		0
#define kIO_NAME_COLUMN		1
#define kIO_AUTO_COLUMN		2
#define kIO_VALUE_COLUMN	3
#define kIO_SPLIT		4
#define kIO_NEWVALUE_COLUMN	5
#define kIO_REQUEST_COLUMN	6

//iotype tables sizes
#define kNUM_IO_COLUMNS		7
#define kIO_INIT_ROWS		4

// iotype new frequency not set indicator
#define kNULL_FREQ		-1



#endif

