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


#ifndef __PARAMETER_TABLE_H__
#define __PARAMETER_TABLE_H__

#include "table.h"
#include "parameter.h"
#include "qpoint.h"

class HistoryPlot;


class ParameterTable : public Table
{
  Q_OBJECT

public: 
  ParameterTable(QWidget *aParent, const char *aName, int aSimHandle);
  virtual ~ParameterTable();

  virtual void initTable();
  virtual void clearAndDisableForDetach(const bool aUnRegister = true);
  
  virtual bool updateRow(const int lHandle, const char *lVal);
  virtual void addRow(const int lHandle, const char *lLabel, const char *lVal, const int lType);

public slots:
  // MR: Slot for the context menu
  virtual void contextMenuSlot(int row, int column, const QPoint &pnt);
  void drawGraphSlot(int popupMenuID);
  void plotClosedSlot(HistoryPlot *ptr);

signals:
  // MR: Signal to tell the HistoryPlot to update
  void paramUpdateSignal(ParameterHistory *mParamHist, const int paramID);

protected:

  int getNumParameters() const;
  int findParameterRowIndex(int aId);
  Parameter *findParameter(int aId);
  // MR: reverse lookup of parameter ID
  Parameter *findParameterHandleFromRow(int row);

protected:
  QPtrList<HistoryPlot> mHistoryPlotList;
  QPtrList<Parameter> mParamList;
  HistoryPlot *mQwtPlot;
  ParameterTable *mMonParamTable;

};


class SteeredParameterTable : public ParameterTable
{
  Q_OBJECT

public:
  SteeredParameterTable(QWidget *aParent, const char *aName, 
			ParameterTable *aTable, int aSimHandle);
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

  // ARPDBG LOGGING
  //public slots:
  // MR: Slot for the context menu (don't currently have one in this table)
  //virtual void contextMenuSlot(int row, int column, const QPoint &pnt);

signals:
  void enableButtonsSignal();

};

#endif



