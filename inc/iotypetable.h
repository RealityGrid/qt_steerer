/*----------------------------------------------------------------------------
    Header file for IOTypeTable class for QT steerer GUI.

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


#ifndef __IOTYPE_TABLE_H__
#define __IOTYPE_TABLE_H__

#include "iotype.h"
#include "table.h"

#include <qpoint.h>

class IOTypeTable : public Table
{
  Q_OBJECT
    
public:
  IOTypeTable(QWidget *aParent, const char *aName, int aSimHandle, bool aChkPtType = false);
  ~IOTypeTable();

  virtual void initTable();
  virtual void clearAndDisableForDetach(const bool aUnRegister = true);

  bool updateRow(const int lHandle, const int lVal);
  void addRow(const int lHandle, const char *lLabel, const int lVal, const int lType);
  int getNumIOTypes() const;

  int getCommandRequestsCount();
  int getCommandRequestsCountNew();
  int populateCommandRequestArray(int *aCmdArray, char **aCmdParamArray, const int aMaxCmds, const int aStartIndex);
  int populateCommandRequestArrayNew(int *aCmdArray, char **aCmdParamArray, const int aMaxCmds, const int aStartIndex);
  int setNewFreqValuesInLib();

private:
  void clearNewValues();

  int findIOTypeRowIndex(int aId);
  IOType *findIOType(int aId);

  void setRestartButtonState();
  void setCreateButtonState();

protected slots:

  void validateValueSlot( int row, int col );
  void emitCommandsSlot();
  void emitValuesSlot();
  //void emitRestartSlot();

  // MR:
  void createButtonPressedSlot();
  void restartButtonPressedSlot();
  
  //void contextMenuSlot(int row, int column, const QPoint &pnt);
  //void createCheckpointSlot(int row);
  //void restartCheckpointSlot(int row);
  void selectionChangedSlot();
  void currentChangedSlot(int row, int column);

signals:
  void enableSampleButtonsSignal();
  void enableChkPtButtonsSignal();
  void disableRestartButtonSignal();
  void enableRestartButtonSignal();
  void disableCreateButtonSignal();
  void enableCreateButtonSignal();


private:
  QPtrList<IOType> mIOTypeList;

  bool		mChkPtTypeFlag;
  int		mRestartRowIndex;
  int   mRestartRowIndexNew;

};



#endif
