/*----------------------------------------------------------------------------
    Header file for IOTypeTable class for QT steerer GUI.

    (C)Copyright 2002 The University of Manchester, United Kingdom,
    all rights reserved.

    This software is produced by the Supercomputing, Visualization &
    e-Science Group, Manchester Computing, the Victoria University of
    Manchester as part of the RealityGrid project.

    This software has been tested with care but is not guaranteed for
    any particular purpose. Neither the authors, nor the University of
    Manchester offer any warranties or representations, nor do they
    accept any liabilities with respect to this software.

    This program must not be used for commmercial gain without the
    written permission of the authors.
    
    Supercomputing, Visualization & e-Science Group
    Manchester Computing
    University of Manchester
    Manchester M13 9PL

    email:  csar-advice@cfs.ac.uk.
    Tel:    +44 161 275 6824/5997
    Fax:    +44 161 275 6040    
    
    Date          Version    Updates                            Author
    ----          -------    -------                            ------
    03.10.2002      0.1                                         S Ramsden

---------------------------------------------------------------------------*/

#ifndef __IOTYPE_TABLE_H__
#define __IOTYPE_TABLE_H__

#include "iotype.h"
#include "table.h"

class IOTypeTable : public Table
{
  Q_OBJECT
    
public:
  IOTypeTable(QWidget *aParent, const char *aName, int aSimHandle);
  ~IOTypeTable();

  virtual void initTable();
  virtual void clearAndDisableForDetach(const bool aUnRegister = true);

  bool updateRow(const int lHandle, const int lVal);
  void addRow(const int lHandle, const char *lLabel, const int lVal, const int lType, const int lAutoFlag = false);
  int getNumIOTypes() const;

  int getCommandRequestsCount();
  int populateCommandRequestArray(int *aCmdArray, const int aMaxCmds, const int aStartIndex);
  int setNewFreqValuesInLib();

private:
  void clearNewValues();

  int findIOTypeRowIndex(int aId);
  IOType *findIOType(int aId);

protected slots:

  void validateValueSlot( int row, int col );
  void emitCommandsSlot();
  void emitValuesSlot();

signals:
  void enableSampleButtonsSignal();
  void enableChkPtButtonsSignal();


private:
  QPtrList<IOType> mIOTypeList;

};



#endif
