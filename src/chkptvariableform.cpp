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

#include <qapplication.h>
#include <q3hbox.h>
#include <q3table.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <q3vbox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

#include "buildconfig.h"
#include "chkptvariableform.h"
#include "utility.h"
#include "types.h"
#include "debug.h"

ChkPtVariableForm::ChkPtVariableForm(const Output_log_struct *outputLogStruct,
		     QWidget *parent, const char *name,
		     bool modal, Qt::WFlags f)
  : QDialog( parent, name, modal, f ), mCancelButton(kNULL)
{

  REG_DBGCON("ChkPtVariableForm");

    this->setCaption( "CheckPoint Parameters Table" );
    resize( 350, 350 );

    // create the layouts for the form
    Q3HBoxLayout *lFormLayout = new Q3HBoxLayout(this, 10, 10, "chkptvariableformlayout");
    Q3VBoxLayout *lTableLayout = new Q3VBoxLayout(6, "chkptvariabletablelayout");
    Q3VBoxLayout *lButtonLayout = new Q3VBoxLayout(6, "chkptvariablebuttonlayout");

    lTableLayout->addWidget(new TableLabel("CheckPoint Parameters", this));
    mTable = new Q3Table(0,2,this);
    mTable->setSelectionMode( Q3Table::Single );
    mTable->verticalHeader()->hide();
    mTable->setLeftMargin(0);

    mTable->horizontalHeader()->setLabel(0, "Parameter Label");
    mTable->horizontalHeader()->setLabel(1, "Value");

    Q3TableItem *lTableItem = NULL;
    Q3TableItem *lTableItem2 = NULL;
    for (int i=0; i<outputLogStruct->num_param; i++)
    {
      lTableItem = new ChkPtVarTableItem(mTable, Q3TableItem::Never, QString(outputLogStruct->param_labels[i]));
      lTableItem2 = new ChkPtVarTableItem(mTable, Q3TableItem::Never, QString(outputLogStruct->param_values[i]));
      mTable->insertRows(mTable->numRows());
      mTable->setItem(mTable->numRows()-1, 0, lTableItem );
      mTable->setItem(mTable->numRows()-1, 1, lTableItem2 );
    }

    mTable->adjustColumn(0);
    mTable->adjustColumn(1);

    lTableLayout->addWidget(mTable);

    mCancelButton = new QPushButton("Cancel", this, "cancelbutton");
    mCancelButton->setAutoDefault(FALSE);
    connect(mCancelButton,  SIGNAL(clicked()), this, SLOT( reject()));

    mCancelButton->setMinimumSize(mCancelButton->sizeHint());
    mCancelButton->setMaximumSize(mCancelButton->sizeHint());

    lButtonLayout->addItem(new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ));
    lButtonLayout->addWidget(mCancelButton);

    lFormLayout->addLayout(lTableLayout);
    lFormLayout->addLayout(lButtonLayout);

}

ChkPtVariableForm::~ChkPtVariableForm()
{
  REG_DBGDST("ChkVariablePtForm");
  cleanUp();
}

void
ChkPtVariableForm::cleanUp()
{

}
