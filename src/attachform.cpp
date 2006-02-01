/*----------------------------------------------------------------------------
  AttachForm class header file for QT steerer GUI. 

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

 
#include "attachform.h"
#include "steererconfig.h"
#include "steerermainwindow.h"
#include "utility.h"
#include "types.h"
#include "debug.h"
#include "ReG_Steer_Steerside.h"
#include "ReG_Steer_Browser.h"

#include <qhbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qvbox.h>
#include <qtable.h>

AttachForm::AttachForm(QWidget *parent, const char *name,
		       bool modal, WFlags f)
  : QDialog( parent, name, modal, f ), mNumSims(0), 
    mLibReturnStatus(REG_SUCCESS), mSimGSHSelected(kNULL),
    mSimName(kNULL), mSimGSH(kNULL), mTable(kNULL), 
    mFilterLineEdit(kNULL), mAttachButton(kNULL), mCancelButton(kNULL)
{
  struct registry_entry *entries;
  int i;
  int count;
  DBGCON("AttachForm");

  // set up arrays for ReG lib call
  // max list can be is REG_MAX_NUM_STEERED_SIM

  mSimName = new char *[REG_MAX_NUM_STEERED_SIM];
  mSimGSH = new char *[REG_MAX_NUM_STEERED_SIM];

  for (i=0; i<REG_MAX_NUM_STEERED_SIM; i++){
    mSimName[i] = new char[REG_MAX_STRING_LENGTH + 1];
    mSimGSH[i] = new char[REG_MAX_STRING_LENGTH + 1];
  }

  // Get the passphrase for the user's key if registry is using
  // SSL
  SteererConfig *lConfig = ((SteererMainWindow *)parent)->getConfig();
  if( lConfig->mTopLevelRegistry.startsWith("https") &&
     lConfig->mKeyPassphrase.isEmpty() ){
    bool ok;

    lConfig->mKeyPassphrase = QInputDialog::getText("RealityGrid Steerer", 
						    "Enter passphrase for X.509 key:", 
						    QLineEdit::Password,
						    QString::null, &ok, this );
    if ( !ok ) return; // Cancel if user didn't press OK
  }

  // Now find out what's in the registry...
  /* ..._secure only available in steering library >= 2.0
  mLibReturnStatus = Get_registry_entries_secure(lConfig->mTopLevelRegistry,
						 lConfig->mKeyPassphrase,
						 lConfig->mPrivateKeyCertFile,
						 lConfig->mCACertsPath,
						 &mNumSims,  
						 &entries);
  */
  mLibReturnStatus = Get_registry_entries((char *)(lConfig->mTopLevelRegistry.ascii()),
					  &mNumSims,  
					  &entries);
  if(mLibReturnStatus != REG_SUCCESS) return;

  count = 0;
  for (i=0; i<mNumSims; i++){
    if(!strcmp(entries[i].service_type, "SWS") ||
       !strcmp(entries[i].service_type, "SGS")){
      sprintf(mSimName[count], "%s %s %s", entries[i].application,
	      entries[i].user, entries[i].start_date_time);
      sprintf(mSimGSH[count], "%s", entries[i].gsh);
      count++;
    }
  }
  mNumSims = count;

  // only continue if there is some info to show
  if(mNumSims>0) 
  {
    this->setCaption( "Grid Attach" );
    resize( 520, 350 );

    // create the layouts for the form
    QVBoxLayout *lFormLayout = new QVBoxLayout(this, 10, 10, 
					       "attachformlayout");
    QHBoxLayout *lFilterLayout = new QHBoxLayout(6, "filterlayout");
    QVBoxLayout *lListLayout = new QVBoxLayout(6, "attachlistlayout");
    QHBoxLayout *lButtonLayout = new QHBoxLayout(6, "attachbuttonlayout");
    QSpacerItem* lSpacer = new QSpacerItem( 200, 0, QSizePolicy::Expanding, 
					    QSizePolicy::Minimum );
    
    // create the list box for the applications on the grid
    lListLayout->addWidget(new TableLabel("Steerable Applications", this));
    mTable = new QTable(0, 2, this);
    mTable->setSelectionMode( QTable::Single );
    mTable->verticalHeader()->hide();
    mTable->setLeftMargin(0);

    mTable->horizontalHeader()->setLabel(0, "Application");
    mTable->horizontalHeader()->setLabel(1, "Handle");

    connect(mTable, SIGNAL(valueChanged(int, int)), this, 
	    SLOT(editHandleSlot(int, int)));

    // populate the list box - each listboxitem holds array index 
    // information - this is what is used by the calling code 
    // (via  aSimIndexSelected) to identify the aSimGSH selected
    for (int i=0; i<mNumSims; i++)
    {      
      DBGMSG1("mSimName ", mSimName[i]);
      DBGMSG1("mSimGSH ", mSimGSH[i]);

      mTable->insertRows(mTable->numRows(),1);
      mTable->setItem(mTable->numRows()-1, 0, 
		      new QTableItem(mTable, QTableItem::Never, 
				     QString(mSimName[i])));
      mTable->setItem(mTable->numRows()-1, 1, 
		      new QTableItem(mTable, QTableItem::WhenCurrent, 
				     QString(mSimGSH[i])));
    }
    mTable->adjustColumn(0);
    mTable->adjustColumn(1);
    // We checked that mNumSims was > 0 earlier...
    mTable->selectRow(0);

    mFilterLineEdit = new QLineEdit(this, "containsfilter");
    connect(mFilterLineEdit, SIGNAL(returnPressed()), this, 
	    SLOT(filterSlot()));

    // Initialize the filter with the user's username - might be handy
    mFilterLineEdit->setText(QString(getenv("USER")));
    mFilterLineEdit->selectAll();

    lFilterLayout->addWidget(new QLabel("Contains", this));
    lFilterLayout->addWidget(mFilterLineEdit);
    
    
    lListLayout->addWidget(mTable);
    lListLayout->addLayout(lFilterLayout);
    
    mAttachButton = new QPushButton("Attach", this, "attachbutton"); 
    mAttachButton->setMinimumSize(mAttachButton->sizeHint());
    mAttachButton->setMaximumSize(mAttachButton->sizeHint());
    mAttachButton->setAutoDefault(FALSE);
    QToolTip::add(mAttachButton, "Attach to selected application");
    connect(mAttachButton, SIGNAL(clicked()), this, SLOT(attachSlot()));
    
    
    mCancelButton = new QPushButton("Cancel", this, "cancelbutton");
    mCancelButton->setMinimumSize(mCancelButton->sizeHint());
    mCancelButton->setMaximumSize(mCancelButton->sizeHint());
    mCancelButton->setAutoDefault(FALSE);
    connect(mCancelButton,  SIGNAL(clicked()), this, SLOT( reject()));
    

    lButtonLayout->addItem(lSpacer);
    lButtonLayout->addWidget(mCancelButton);
    lButtonLayout->addWidget(mAttachButton);
    
    lFormLayout->addLayout(lListLayout);
    lFormLayout->addLayout(lButtonLayout);
  }

}

AttachForm::~AttachForm()
{
  DBGDST("AttachForm");
  cleanUp();
}

void
AttachForm::cleanUp()
{

  for (int i=0; i<REG_MAX_NUM_STEERED_SIM; i++)
  {
    delete [] mSimName[i];
    delete [] mSimGSH[i];
  }
  delete [] mSimName;
  delete [] mSimGSH;

}


int 
AttachForm::getNumSims() const
{
  return mNumSims;
}

int 
AttachForm::getLibReturnStatus() const
{
  return mLibReturnStatus;
}

char *
AttachForm::getSimGSMSelected() const
{
  return mSimGSHSelected;
}


void
AttachForm::attachSlot()
{
  int lCurrentItem;
  int lNumSel = mTable->numSelections();
  DBGMSG1("Have selections... ", lNumSel); 
  if(!lNumSel || lNumSel > 1){
    // no item in the list has been selected
    QMessageBox::information(0, "Nothing selected", 
			     "Please select one item from the list",
			     QMessageBox::Ok,
			     QMessageBox::NoButton, 
			     QMessageBox::NoButton);
    return;
  }

  QTableSelection lSel = mTable->selection(0);
  lCurrentItem = lSel.anchorRow();

  mSimGSHSelected = mSimGSH[lCurrentItem];
  DBGMSG1("selected app GSH: ",mSimGSHSelected);
  QDialog::accept();
}


void
AttachForm::filterSlot()
{
  DBGMSG1("FSLot: mNumSims: ", mNumSims);
  int i;

  int lLen = mFilterLineEdit->text().length();
  if (lLen == 0) return;

  // clear out the table
  for (i=(mTable->numRows()-1); i>-1; i--){
    mTable->removeRow(i);
  }

  for (int i=0; i<mNumSims; i++){
  
    DBGMSG2("***filter: ", mSimName[i], mFilterLineEdit->text().latin1());
    if (strstr(mSimName[i], mFilterLineEdit->text().latin1()) != kNULL){

      mTable->insertRows(mTable->numRows(),1);
      mTable->setItem(mTable->numRows()-1, 0, 
		      new QTableItem(mTable, QTableItem::Never, 
				     QString(mSimName[i])));
      mTable->setItem(mTable->numRows()-1, 1, 
		      new QTableItem(mTable, QTableItem::OnTyping, 
				     QString(mSimGSH[i])));
    }   
  }

  mTable->adjustColumn(0);
  mTable->adjustColumn(1);
  if (mTable->numRows()) mTable->selectRow(0);

}

/** Slot to update the copy of the handle as obtained from the
 *  appropriate environment variable. Do we want to update the
 *  environment variable itself too?
 */
void AttachForm::editHandleSlot(int row, int col){
      DBGMSG2("", row, col);

      const char *temp = mTable->text(row, col).latin1();
      strcpy(mSimGSH[row], temp);
}
