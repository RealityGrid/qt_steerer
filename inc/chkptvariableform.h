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

    Initial version by: M Riding, 09.06.2003
    
---------------------------------------------------------------------------*/

#ifndef __CHKPT_VARIABLE_FORM_H__
#define __CHKPT_VARIABLE_FORM_H__

#include <qdialog.h>
#include <qlistbox.h>
#include <qtable.h>
#include "ReG_Steer_Steerside.h"

class QLineEdit;
class QPushButton;

class ChkPtVariableForm: public QDialog
{
  Q_OBJECT
       
public:
  ChkPtVariableForm(const Output_log_struct *tmp,
	    QWidget *parent = 0, const char *name = "chkptvariableform",
	    bool modal = FALSE, WFlags f = WGroupLeader );
// MR: The WGroupLeader flag is very important, without it this _modeless_ dialog would be unable
//     to do any event handling. The parent _modal_ dialog would instead get exclusive focus.
  ~ChkPtVariableForm();

  void run();

protected slots:

private:
  void cleanUp();

private:

  QTable		*mTable;

  QPushButton		*mCancelButton;

};
/*
class DQLeftAlignTableItem : public QTableItem
{
public:
  DQLeftAlignTableItem( QTable *table, EditType et, const QString &text )
    : QTableItem(table, et, text)
  { myAlignment = Qt::AlignLeft | Qt::AlignVCenter; }
  DQLeftAlignTableItem( QTable *table, EditType et, const QString &text, const QPixmap &p )
    : QTableItem(table, et, text, p)
  { myAlignment = Qt::AlignLeft | Qt::AlignVCenter; }

  int alignment() const {return Qt::AlignLeft | Qt::AlignVCenter;	} // Here is the align change done.
  void setAlignment(int newAlignment){
    myAlignment = newAlignment;
  }

private:
  int myAlignment;
};
*/

/** Subclass of QTableItem, so that we can set the justification
 *  of each table item to be non-default
 */

class ChkPtVarTableItem : public QTableItem
{
protected:
  int myAlignment;
public:
  ChkPtVarTableItem(QTable *table, EditType et, const QString & text): QTableItem(table, et, text){
    myAlignment = Qt::AlignLeft;
  }
  ChkPtVarTableItem( QTable *table, EditType et, const QString &text, const QPixmap &p )
    : QTableItem(table, et, text, p)
  {}
  void setAlignment(int newAlignment){
    myAlignment = newAlignment;
  }
  int alignment() const {
    return myAlignment;
  }
};


#endif
