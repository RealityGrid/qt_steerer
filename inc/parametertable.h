/*----------------------------------------------------------------------------
    Header file for ParameterTable and SteeredParameterTable classes for 
    QT steerer GUI

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
  QPtrList<Parameter> mParamList;
  QPtrList<HistoryPlot> mHistoryPlotList;
  HistoryPlot *mQwtPlot;

};


class SteeredParameterTable : public ParameterTable
{
  Q_OBJECT

public:
  SteeredParameterTable(QWidget *aParent, const char *aName, int aSimHandle);
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

public slots:
  // MR: Slot for the context menu (don't currently have one in this table)
  virtual void contextMenuSlot(int row, int column, const QPoint &pnt);


signals:
  void enableButtonsSignal();

};

#endif



