/*----------------------------------------------------------------------------
    ChkPtVariableForm class header file for QT steerer GUI. 

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

  DBGCON("ChkPtVariableForm");

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
      lTableItem = new QTableItem(mTable, QTableItem::Never, QString(outputLogStruct->param_labels[i]));
      lTableItem2 = new QTableItem(mTable, QTableItem::Never, QString(outputLogStruct->param_values[i]));
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
  DBGDST("ChkVariablePtForm");
  cleanUp();
}

void
ChkPtVariableForm::cleanUp()
{

}


