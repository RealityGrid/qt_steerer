/*----------------------------------------------------------------------------
  Table class header file for QT steerer GUI.
  (C)Copyright 2002 The University of Manchester, United Kingdom,
  all rights reserved.

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
  // MR: removed - was causing problems, and giving no noticeable benefit
  //void setNumInitRows(int aNumInitRows);
  void incrementRowIndex();

  bool getAppAttached() const;
  // MR: removed - was causing problems, and giving no noticeable benefit
  //int getNumInitRows() const;
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

