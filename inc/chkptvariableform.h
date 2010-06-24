/*
  The RealityGrid Steerer

  Copyright (c) 2002-2010, University of Manchester, United Kingdom.
  All rights reserved.

  This software is produced by Research Computing Services, University
  of Manchester as part of the RealityGrid project and associated
  follow on projects, funded by the EPSRC under grants GR/R67699/01,
  GR/R67699/02, GR/T27488/01, EP/C536452/1, EP/D500028/1,
  EP/F00561X/1.

  LICENCE TERMS

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of The University of Manchester nor the names
      of its contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

  Author: Mark Riding
          Andrew Porter
          Sue Ramsden
          Robert Haines
 */

#ifndef __CHKPT_VARIABLE_FORM_H__
#define __CHKPT_VARIABLE_FORM_H__

#include <qdialog.h>
#include <q3listbox.h>
#include <q3table.h>
//Added by qt3to4:
#include <QPixmap>

#include "ReG_Steer_Steerside.h"

class QLineEdit;
class QPushButton;

class ChkPtVariableForm: public QDialog
{
  Q_OBJECT

public:
  ChkPtVariableForm(const Output_log_struct *tmp,
	    QWidget *parent = 0, const char *name = "chkptvariableform",
	    bool modal = FALSE, Qt::WFlags f = Qt::WGroupLeader );
// MR: The WGroupLeader flag is very important, without it this _modeless_ dialog would be unable
//     to do any event handling. The parent _modal_ dialog would instead get exclusive focus.
  ~ChkPtVariableForm();

  void run();

protected slots:

private:
  void cleanUp();

private:

  Q3Table		*mTable;

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

class ChkPtVarTableItem : public Q3TableItem
{
protected:
  int myAlignment;
public:
  ChkPtVarTableItem(Q3Table *table, EditType et, const QString & text): Q3TableItem(table, et, text){
    myAlignment = Qt::AlignLeft;
  }
  ChkPtVarTableItem( Q3Table *table, EditType et, const QString &text, const QPixmap &p )
    : Q3TableItem(table, et, text, p)
  {}
  void setAlignment(int newAlignment){
    myAlignment = newAlignment;
  }
  int alignment() const {
    return myAlignment;
  }
};


#endif
