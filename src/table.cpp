/*
  The RealityGrid Steerer

  Copyright (c) 2002-2010, University of Manchester, United Kingdom.
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

/** @file table.cpp
    @brief Table class for QT steerer GUI. This is an abstract base class.
    @author Sue Ramsden
    @author Mark Riding
    @author Andrew Porter */

#include "buildconfig.h"
#include "table.h"
#include "types.h"
#include "debug.h"

Table::Table(QWidget *aParent, const char *aName, int aSimHandle)
  : Q3Table(aParent, aName),  mSimHandle(aSimHandle), mInitNumRows(0),
    mMaxRowIndex(0), mAppAttached(true)
{
  REG_DBGCON("Table");

  // Note: used mNumInitRows to force table to have rows before any data received.
  // This workaround as if have zero rows until parameters consumed and hence displayed, the table always
  // has a scroll bar - even if only one row!
  // Need to fix this properly if poss and get rid of mNumInitRows. (QT issue) SMR XXX

  // mMaxRowIndex - number rows populated with data in the table (i.e. number of parameters diaplayed)

  // mAppAttached - set to false when application detached from steerer - after a detach
  // the whole table becomes read only
}

Table::~Table()
{
  REG_DBGDST("Table");
}


void
Table::setAppDetached()
{
  mAppAttached=false;
}


// MR: removed - was causing problems, and giving no noticeable benefit
/*
void
Table::setNumInitRows(int aNumInitRows)
{
  mInitNumRows = aNumInitRows;
}
*/

void
Table::incrementRowIndex()
{
  mMaxRowIndex++;
}


bool
Table::getAppAttached() const
{
  return mAppAttached;
}

// MR: removed - was causing problems, and giving no noticeable benefit
/*
int
Table::getNumInitRows() const
{
  return mInitNumRows;
}
*/

int
Table::getMaxRowIndex() const
{
  return mMaxRowIndex;
}

int
Table::getSimHandle() const
{
  return mSimHandle;
}
