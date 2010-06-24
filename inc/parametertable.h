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

/** @file parametertable.h
    @brief Header file for ParameterTable class */

#ifndef __PARAMETER_TABLE_H__
#define __PARAMETER_TABLE_H__

#include "qpoint.h"
#include "qmutex.h"
//Added by qt3to4:
#include <Q3PtrList>

#include "table.h"
#include "parameter.h"
#include "historyplot.h"
#include "controlform.h"

class QEvent;

class ParameterTable : public Table
{
  Q_OBJECT

public:
  ParameterTable(QWidget *aParent, const char *aName, int aSimHandle,
		 QMutex *aMutex);
  virtual ~ParameterTable();

  virtual void initTable();
  virtual void clearAndDisableForDetach(const bool aUnRegister = true);
  /// Update the information shown in an existing row in the
  /// parameter table
  /// @param lHandle The handle of the parameter to update
  /// @param lVal The value of the parameter (as a char*)
  /// @param isStatusMsg Whether this update has been forced by receipt
  /// of a status message
  virtual bool updateRow(const int lHandle,
			 const char *lVal,
			 const bool isStatusMsg);
  /// Add a row to the parameter table
  /// @param lHandle The handle of the parameter to add a row for
  /// @param lLabel The label of this parameter
  /// @param lVal The value of the parameter (as a char*)
  /// @param lType The type of this parameter encoded as an int
  virtual void addRow(const int lHandle, const char *lLabel,
		      const char *lVal, const int lType);
  /// Update the full log of the parameter values (i.e. for the
  /// period before the steering client attached)
  void updateParameterLog();
  /// Get a ptr to Parameter from its handle
  /// @param aId The handle of the parameter to look up
  Parameter *findParameter(int aId);

public slots:
  /// Slot for the context menu in the parameter table
  virtual void contextMenuSlot(int row, int column, const QPoint &pnt);
  void requestParamHistorySlot(int row);
  void drawGraphSlot(int popupMenuID);
  /// Slot called when the user selects the "Draw Graph" option from
  /// the table's context menu
  void addGraphSlot(int popupMenuID);

protected:

  int getNumParameters() const;
  /// Look up a parameter's row index from its handle
  int findParameterRowIndex(int aId);
  /// Reverse lookup of parameter ID
  Parameter *findParameterHandleFromRow(int row);
  /// Lookup Parameter from its label
  Parameter *findParameterFromLabel(const QString &aLabel);
  /// List of the parameters associated with this application
  Q3PtrList<Parameter>   mParamList;
  /// Pointer to table of monitored parameters
  ParameterTable       *mMonParamTable;
  /// Pointer to mutex used to control calls to steering library
  QMutex               *mMutexPtr;

 private:
  /// Pointer to our parent control form
  ControlForm          *mParent;
};


class SteeredParameterTable : public ParameterTable
{
  Q_OBJECT

public:
  SteeredParameterTable(QWidget *aParent, const char *aName,
			ParameterTable *aTable, int aSimHandle,
			QMutex *aMutex);
  virtual ~SteeredParameterTable();

  virtual void initTable();
  virtual void clearAndDisableForDetach(const bool aUnRegister = true);

  ////  virtual bool updateRow no redefinition required
  virtual void addRow(const int lHandle, const char *lLabel, const char *lVal, const int lType, const char *lMinVal, const char *lMaxVal);

  int setNewParamValuesInLib();
  void clearNewValues();

  // Method called by the event handler in order to determine
  //what tooltip (if any) to show
  int getTip(const QPoint &pos, QString &string);

protected:
  virtual bool event(QEvent*);

protected slots:
  void validateValueSlot(int aRow, int aCol);
  void emitValuesSlot();

signals:
  void enableButtonsSignal();

};

#endif
