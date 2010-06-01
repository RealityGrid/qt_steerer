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

/** @file iotypetable.h
    @brief Header file for IOTypeTable class */

#ifndef __IOTYPE_TABLE_H__
#define __IOTYPE_TABLE_H__

#include "iotype.h"
#include "table.h"

#include <qpoint.h>
#include <qmutex.h>
//Added by qt3to4:
#include <Q3PtrList>

class IOTypeTable : public Table
{
  Q_OBJECT

public:
  IOTypeTable(QWidget *aParent, const char *aName, int aSimHandle,
	      QMutex *aMutex, bool aChkPtType = false);
  ~IOTypeTable();

  virtual void initTable();
  virtual void clearAndDisableForDetach(const bool aUnRegister = true);

  bool updateRow(const int lHandle, const int lVal);
  void addRow(const int lHandle, const char *lLabel, const int lVal, const int lType);
  int getNumIOTypes() const;

  int getCommandRequestsCountNew();
  int getCommandRequestsCountOfType(const int aType);
  int populateCommandRequestArrayNew(int *aCmdArray, char **aCmdParamArray, const int aMaxCmds, const int aStartIndex);
  int populateCommandRequestArrayOfType(int *aCmdArray, char **aCmdParamArray, const int aMaxCmds, const int aStartIndex, const int aType);
  int setNewFreqValuesInLib();

private:
  void clearNewValues();

  int findIOTypeRowIndex(int aId);
  IOType *findIOType(int aId);

  void setCreateRestartButtonStates();
  void setConsumeEmitButtonStates();

protected slots:

  void validateValueSlot( int row, int col );
  void emitValuesSlot();

  // MR:
  void createButtonPressedSlot();
  void restartButtonPressedSlot();

  void selectionChangedSlot();
  void currentChangedSlot(int row, int column);

  void consumeButtonPressedSlot();
  void emitButtonPressedSlot();

signals:
  void enableSampleButtonsSignal();
  void enableChkPtButtonsSignal();

  void setRestartButtonStateSignal(const bool aEnable);
  void setCreateButtonStateSignal(const bool aEnable);
  void setConsumeButtonStateSignal(const bool aEnable);
  void setEmitButtonStateSignal(const bool aEnable);

private:
  Q3PtrList<IOType> mIOTypeList;

  bool	    mChkPtTypeFlag;
  int	    mRestartRowIndex;
  int       mRestartRowIndexNew;
  /// Ptr to mutex protecting calls to ReG steer lib
  QMutex   *mMutexPtr;
};



#endif
