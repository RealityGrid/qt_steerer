/*----------------------------------------------------------------------------
    Header file for ParameterTable and SteeredParameterTable classes for 
    QT steerer GUI

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
#ifndef __PARAMETER_TABLE_H__
#define __PARAMETER_TABLE_H__

#include "table.h"
#include "parameter.h"

class ParameterTable : public Table
{
  Q_OBJECT

public: 
  ParameterTable(QWidget *aParent, const char *aName, int aSimHandle);
  virtual ~ParameterTable();

  virtual void initTable();
  virtual void clearAndDisableForDetach();
  
  virtual bool updateRow(const int lHandle, const char *lVal);
  virtual void addRow(const int lHandle, const char *lLabel, const char *lVal, const int lType);
 
protected:

  int getNumParameters() const;
  int findParameterRowIndex(int aId);
  Parameter *findParameter(int aId);

protected:
  QPtrList<Parameter> mParamList;

};


class SteeredParameterTable : public ParameterTable
{
  Q_OBJECT

public:
  SteeredParameterTable(QWidget *aParent, const char *aName, int aSimHandle);
  virtual ~SteeredParameterTable();

  virtual void initTable();
  virtual void clearAndDisableForDetach();

  ////  virtual bool updateRow no redefinition required
  virtual void addRow(const int lHandle, const char *lLabel, const char *lVal, const int lType);

  int setNewParamValuesInLib();
  void clearNewValues();

protected slots:
  void validateValueSlot(int aRow, int aCol);
  void emitValuesSlot();

signals:
  void enableButtonsSignal();

};

#endif



