/*----------------------------------------------------------------------------
    Table class header file for QT steerer GUI.

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

#ifndef __TABLE_H__
#define __TABLE_H__

#include <qtable.h>

// abstract table class
class Table : public QTable
{
  Q_OBJECT

public: 
  Table(QWidget *aParent, const char *aName, int aSimHandle);
  virtual ~Table();

  virtual void initTable() = 0;
  virtual void clearAndDisableForDetach(const bool aUnRegister = true) = 0;

  void setAppDetached();
  void setNumInitRows(int aNumInitRows);
  void incrementRowIndex();

  bool getAppAttached() const;
  int getNumInitRows() const;
  int getMaxRowIndex() const;
  int getSimHandle() const;

signals:
  void detachFromApplicationForErrorSignal();

private:

  int		mSimHandle;
  int		mInitNumRows;
  int		mMaxRowIndex;
  bool		mAppAttached;

};


#endif

