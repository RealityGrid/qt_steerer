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

/** @file attachform.cpp
    @brief AttachForm class header file for QT steerer GUI.
    @author Sue Ramsden
    @author Mark Riding
    @author Andrew Porter */

#include "attachform.h"
#include "steererconfig.h"
#include "steerermainwindow.h"
#include "utility.h"
#include "types.h"
#include "debug.h"
#include "ReG_Steer_Steerside.h"
#include "ReG_Steer_Browser.h"

#include <q3hbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <q3vbox.h>
#include <q3table.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QLabel>
#include <Q3VBoxLayout>

AttachForm::AttachForm(QWidget *parent, const char *name,
		       bool modal, Qt::WFlags f)
  : QDialog( parent, name, modal, f ), mNumSims(0),
    mLibReturnStatus(REG_SUCCESS), mSimGSHSelected(kNULL),
    mSimName(kNULL), mSimGSH(kNULL), mTable(kNULL),
    mFilterLineEdit(kNULL), mAttachButton(kNULL), mCancelButton(kNULL)
{
  struct registry_contents content;
  int i, count;
  QString lString;
  bool ok;
  REG_DBGCON("AttachForm");

  // set up arrays for ReG lib call
  // max list can be is REG_MAX_NUM_STEERED_SIM
  mSimName = new char *[REG_MAX_NUM_STEERED_SIM];
  mSimGSH = new char *[REG_MAX_NUM_STEERED_SIM];

  for (i=0; i<REG_MAX_NUM_STEERED_SIM; i++){
    mSimName[i] = new char[REG_MAX_STRING_LENGTH + 1];
    mSimGSH[i] = new char[REG_MAX_STRING_LENGTH + 1];
  }

  SteererConfig *lConfig = ((SteererMainWindow *)parent)->getConfig();
// #ifdef REG_WSRF
  if( !(lConfig->mRegistrySecurity.passphrase[0]) ){

    // Get the passphrase for the user's key if registry is using
    // SSL
    if( lConfig->mRegistrySecurity.use_ssl == REG_TRUE){

      lString = QInputDialog::getText("RealityGrid Steerer",
				      "Enter passphrase for X.509 key:",
				      QLineEdit::Password,
				      QString::null, &ok, this );
      if ( !ok ) return; // Cancel if user didn't press OK
    }
    else{
      lString = QInputDialog::getText("RealityGrid Steerer",
				      "Enter passphrase for registry:",
				      QLineEdit::Password,
				      QString::null, &ok, this );
      if ( !ok ) return; // Cancel if user didn't press OK
    }
    strncpy(lConfig->mRegistrySecurity.passphrase,
	    lString.ascii(), REG_MAX_STRING_LENGTH);
  }
  // Now find out what's in the registry...
  //..._secure only available in steering library >= 2.0
  mLibReturnStatus = Get_registry_entries_secure(lConfig->mTopLevelRegistry,
						 &(lConfig->mRegistrySecurity),
						 &content);
// #else
//   mLibReturnStatus = Get_registry_entries((char *)(lConfig->mTopLevelRegistry.ascii()),
// 					  &content);
// #endif // defined REG_WSRF

  if(mLibReturnStatus != REG_SUCCESS) return;

  count = 0;
  for (i=0; i<content.numEntries; i++){
    if(!strcmp(content.entries[i].service_type, "SWS") ||
       !strcmp(content.entries[i].service_type, "SGS")){
      sprintf(mSimName[count], "%s %s %s", content.entries[i].application,
	      content.entries[i].user, content.entries[i].start_date_time);
      sprintf(mSimGSH[count], "%s", content.entries[i].gsh);
      count++;
    }
  }
  Delete_registry_table(&content);

  mNumSims = count;

  // only continue if there is some info to show
  if(mNumSims>0)
  {
    this->setCaption( "Grid Attach" );
    resize( 520, 350 );

    // create the layouts for the form
    Q3VBoxLayout *lFormLayout = new Q3VBoxLayout(this, 10, 10,
					       "attachformlayout");
    Q3HBoxLayout *lFilterLayout = new Q3HBoxLayout(6, "filterlayout");
    Q3VBoxLayout *lListLayout = new Q3VBoxLayout(6, "attachlistlayout");
    Q3HBoxLayout *lButtonLayout = new Q3HBoxLayout(6, "attachbuttonlayout");
    QSpacerItem* lSpacer = new QSpacerItem( 200, 0, QSizePolicy::Expanding,
					    QSizePolicy::Minimum );

    // create the list box for the applications on the grid
    lListLayout->addWidget(new TableLabel("Steerable Applications", this));
    mTable = new Q3Table(0, 2, this);
    mTable->setSelectionMode( Q3Table::Single );
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
      REG_DBGMSG1("mSimName ", mSimName[i]);
      REG_DBGMSG1("mSimGSH ", mSimGSH[i]);

      mTable->insertRows(mTable->numRows(),1);
      mTable->setItem(mTable->numRows()-1, 0,
		      new Q3TableItem(mTable, Q3TableItem::Never,
				     QString(mSimName[i])));
      mTable->setItem(mTable->numRows()-1, 1,
		      new Q3TableItem(mTable, Q3TableItem::WhenCurrent,
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
  REG_DBGDST("AttachForm");
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
  REG_DBGMSG1("Have selections... ", lNumSel);
  if(!lNumSel || lNumSel > 1){
    // no item in the list has been selected
    QMessageBox::information(0, "Nothing selected",
			     "Please select one item from the list",
			     QMessageBox::Ok,
			     QMessageBox::NoButton,
			     QMessageBox::NoButton);
    return;
  }

  Q3TableSelection lSel = mTable->selection(0);
  lCurrentItem = lSel.anchorRow();

  mSimGSHSelected = mSimGSH[lCurrentItem];
  REG_DBGMSG1("selected app GSH: ",mSimGSHSelected);
  QDialog::accept();
}


void
AttachForm::filterSlot()
{
  REG_DBGMSG1("FSLot: mNumSims: ", mNumSims);
  int i;

  int lLen = mFilterLineEdit->text().length();
  if (lLen == 0) return;

  // clear out the table
  for (i=(mTable->numRows()-1); i>-1; i--){
    mTable->removeRow(i);
  }

  for (int i=0; i<mNumSims; i++){

    REG_DBGMSG2("***filter: ", mSimName[i], mFilterLineEdit->text().latin1());
    if (strstr(mSimName[i], mFilterLineEdit->text().latin1()) != kNULL){

      mTable->insertRows(mTable->numRows(),1);
      mTable->setItem(mTable->numRows()-1, 0,
		      new Q3TableItem(mTable, Q3TableItem::Never,
				     QString(mSimName[i])));
      mTable->setItem(mTable->numRows()-1, 1,
		      new Q3TableItem(mTable, Q3TableItem::OnTyping,
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
      REG_DBGMSG2("", row, col);

      const char *temp = mTable->text(row, col).latin1();
      strcpy(mSimGSH[row], temp);
}
