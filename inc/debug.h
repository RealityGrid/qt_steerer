/*----------------------------------------------------------------------------
    File containing debug macro definitions for QT steerer GUI. 

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

  #define DBGEXCP(x) cerr << "DBG: EXCP: " << x << endl;
  #define DBGLOG(x) cerr << "DBGLOG: " << x << endl;
  #define DBGLOG1(x, n) cerr << "DBGLOG:": << x << " " << n << endl;

#endif



#endif
