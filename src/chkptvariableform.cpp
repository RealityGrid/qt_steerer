/*----------------------------------------------------------------------------
  ChkPtVariableForm class header file for QT steerer GUI. 

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

 
#include "chkptvariableform.h"
#include "utility.h"
#include "types.h"
#include "debug.h"

#include <qapplication.h>
#include <qhbox.h>
#include <qtable.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qvbox.h>




ChkPtVariableForm::ChkPtVariableForm(const Output_log_struct *outputLogStruct,
		     QWidget *parent, const char *name,
		     bool modal, WFlags f)
  : QDialog( parent, name, modal, f ), mCancelButton(kNULL)
{

  REG_DBGCON("ChkPtVariableForm");

    this->setCaption( "CheckPoint Parameters Table" );
    resize( 350, 350 );
    
    // create the layouts for the form
    QHBoxLayout *lFormLayout = new QHBoxLayout(this, 10, 10, "chkptvariableformlayout");
    QVBoxLayout *lTableLayout = new QVBoxLayout(6, "chkptvariabletablelayout");
    QVBoxLayout *lButtonLayout = new QVBoxLayout(6, "chkptvariablebuttonlayout");
    
    lTableLayout->addWidget(new TableLabel("CheckPoint Parameters", this));
    mTable = new QTable(0,2,this);
    mTable->setSelectionMode( QTable::Single );
    mTable->verticalHeader()->hide();
    mTable->setLeftMargin(0);

    mTable->horizontalHeader()->setLabel(0, "Parameter Label");
    mTable->horizontalHeader()->setLabel(1, "Value"); 
        
    QTableItem *lTableItem = NULL;
    QTableItem *lTableItem2 = NULL;
    for (int i=0; i<outputLogStruct->num_param; i++)
    {      
      lTableItem = new ChkPtVarTableItem(mTable, QTableItem::Never, QString(outputLogStruct->param_labels[i]));
      lTableItem2 = new ChkPtVarTableItem(mTable, QTableItem::Never, QString(outputLogStruct->param_values[i]));
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


