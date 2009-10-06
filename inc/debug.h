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

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <iostream>
using namespace std;

// some simple different cerrs for debug versions

#if DBG_VERSION
  #define REG_DBGMSG(x) cerr << "DBG: " << x << endl; 
  #define REG_DBGMSG1(x,n) cerr << "DBG: " << x <<  n << endl;
  #define REG_DBGMSG2(x,n,m) cerr << "DBG: " << x <<  n << m << endl;

  #define REG_DBGCON(x)
  #define REG_DBGDST(x)

  #define REG_DBGEXCP(x) cerr << "DBG: EXCP: " << x << endl;
  #define REG_DBGLOG(x) cerr << "DBGLOG: " << x << endl;
  #define REG_DBGLOG1(x, n) cerr << "DBGLOG: " << x << " " << n << endl;

#elif DBG_CONS
  #define REG_DBGMSG(x)
  #define REG_DBGMSG1(x,n)
  #define REG_DBGMSG2(x,n,m)

  #define REG_DBGCON(x) cerr << "DBG: CTOR: constructor - " << x << endl;
  #define REG_DBGDST(x) cerr << "DBG: DTOR: destructor -" << x << endl;

  #define REG_DBGEXCP(x) cerr << "DBG: EXCP: " << x << endl;
  #define REG_DBGLOG(x) cerr << "DBGLOG: " << x << endl;
  #define REG_DBGLOG1(x, n) cerr << "DBGLOG: " << x << " " << n << endl;

#else
  #define REG_DBGMSG(x)
  #define REG_DBGMSG1(x,n)
  #define REG_DBGMSG2(x,n,m)
  #define REG_DBGCON(x)
  #define REG_DBGDST(x)

  #define REG_DBGEXCP(x) 
  #define REG_DBGLOG(x) 
  #define REG_DBGLOG1(x, n) 

#endif



#endif
