/*----------------------------------------------------------------------------
  Header file for ParameterTable and SteeredParameterTable classes for 
  QT steerer GUI

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

/** @file parametertable.h
    @brief Header file for ParameterTable class */

#ifndef __PARAMETER_TABLE_H__
#define __PARAMETER_TABLE_H__

#include "table.h"
#include "parameter.h"
#include "qpoint.h"
#include "qmutex.h"
#include "historyplot.h"
#include "controlform.h"

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
  QPtrList<Parameter>   mParamList;
  /// Pointer to table of monitored parameters
  ParameterTable       *mMonParamTable;
  /// Flag recording whether or not we've already fetched the full
  /// history of the Sequence Number parameter for this app.
  bool                  mFetchedSeqNumHistory;
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

  // MR: Method called by the DynamicTip class in order to determine what tooltip (if any) to show
  int getTip(const QPoint &pos, QRect &rect, QString &string);


protected slots:
  void validateValueSlot(int aRow, int aCol);
  void emitValuesSlot();

signals:
  void enableButtonsSignal();

};

#endif



