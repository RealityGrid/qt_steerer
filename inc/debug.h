/*----------------------------------------------------------------------------
  File containing debug macro definitions for QT steerer GUI. 

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

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <iostream.h>

// some simple different cerrs for debug versions

#if DBG_VERSION
  #define DBGMSG(x) cerr << "DBG: " << x << endl; 
  #define DBGMSG1(x,n) cerr << "DBG: " << x <<  n << endl;
  #define DBGMSG2(x,n,m) cerr << "DBG: " << x <<  n << m << endl;

  #define DBGCON(x)
  #define DBGDST(x)

  #define DBGEXCP(x) cerr << "DBG: EXCP: " << x << endl;
  #define DBGLOG(x) cerr << "DBGLOG: " << x << endl;
  #define DBGLOG1(x, n) cerr << "DBGLOG: " << x << " " << n << endl;

#elif DBG_CONS
  #define DBGMSG(x)
  #define DBGMSG1(x,n)
  #define DBGMSG2(x,n,m)

  #define DBGCON(x) cerr << "DBG: CTOR: constructor - " << x << endl;
  #define DBGDST(x) cerr << "DBG: DTOR: destructor -" << x << endl;

  #define DBGEXCP(x) cerr << "DBG: EXCP: " << x << endl;
  #define DBGLOG(x) cerr << "DBGLOG: " << x << endl;
  #define DBGLOG1(x, n) cerr << "DBGLOG: " << x << " " << n << endl;

#else
  #define DBGMSG(x)
  #define DBGMSG1(x,n)
  #define DBGMSG2(x,n,m)
  #define DBGCON(x)
  #define DBGDST(x)

  #define DBGEXCP(x) 
  #define DBGLOG(x) 
  #define DBGLOG1(x, n) 

#endif



#endif
