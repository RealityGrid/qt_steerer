/*----------------------------------------------------------------------------
    ChkPtForm class header file for QT steerer GUI. 

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

    Initial version by: S Ramsden, 12.03.2003
    
---------------------------------------------------------------------------*/


#ifndef __CHKPT_FORM_H__
#define __CHKPT_FORM_H__

#include <qdialog.h>
#include <qlistbox.h>
#include "ReG_Steer_Steerside.h"

class QLineEdit;
class QPushButton;

class ChkPtForm: public QDialog
{
  Q_OBJECT
       
public:
  ChkPtForm(const int aNumEntries, int aSimHandle, int aChkPtHandle,
	    QWidget *parent = 0, const char *name = "chkptform",
	    bool modal = TRUE, WFlags f = 0 );
  ~ChkPtForm();

   int getLibReturnStatus() const;
  char *getChkTagSelected() const;

protected slots:
  void restartSlot();
  void filterSlot();

private:
  void cleanUp();

private:
  const int		mNumEntries;
  int			mLibReturnStatus;

  int			mIndexSelected;
  Output_log_struct	*mLogEntries;

  QListBox		*mListBox;
  QLineEdit		*mFilterLineEdit;

  QPushButton		*mRestartButton;
  QPushButton		*mCancelButton;

};




class ChkPtListItem : public QListBoxItem
{

public:
  ChkPtListItem(int aEntryIndex, const QString &text);
  ~ChkPtListItem();

  int getEntryIndex() const;

  virtual void paint( QPainter * );
  virtual int width( const QListBox* ) const;
  virtual int height( const QListBox* ) const;

private:
  int mEntryIndex;

  
};



#endif
