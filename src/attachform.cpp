/*----------------------------------------------------------------------------
    AttachForm class header file for QT steerer GUI. 

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

    Initial version by: S Ramsden, 23.10.2002
    
---------------------------------------------------------------------------*/

 
#include "attachform.h"
#include "utility.h"
#include "types.h"
#include "debug.h"
#include "ReG_Steer_Steerside.h"

#include <qhbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qvbox.h>
#include <qtable.h>

AttachForm::AttachForm(QWidget *parent, const char *name,
		       bool modal, WFlags f)
  : QDialog( parent, name, modal, f ), mNumSims(0), mLibReturnStatus(REG_SUCCESS),
    mSimGSHSelected(kNULL), mSimName(kNULL), mSimGSH(kNULL), mTable(kNULL), 
    mFilterLineEdit(kNULL), mAttachButton(kNULL), mCancelButton(kNULL)
{

  DBGCON("AttachForm");

  //  set up arrays for ReG lib call
  // max list can be is REG_MAX_NUM_STEERED_SIM

  mSimName = new char *[REG_MAX_NUM_STEERED_SIM];
  mSimGSH = new char *[REG_MAX_NUM_STEERED_SIM];

  for (int i=0; i<REG_MAX_NUM_STEERED_SIM; i++)
  {
    mSimName[i] = new char[REG_MAX_STRING_LENGTH + 1];
    mSimGSH[i] = new char[REG_MAX_STRING_LENGTH + 1];
  }

  // get the list of grid applications from library
  mLibReturnStatus = Get_sim_list(&mNumSims, mSimName, mSimGSH); //SMR XXX need javac

  // only continue is there is some info to show
  if(mLibReturnStatus == REG_SUCCESS && mNumSims>0)  // SMR XXX need javac
  {
    this->setCaption( "Grid Attach" );
    resize( 350, 350 );
    
    // create the layouts for the form
    QHBoxLayout *lFormLayout = new QHBoxLayout(this, 10, 10, "attachformlayout");
    QHBoxLayout *lFilterLayout = new QHBoxLayout(6, "filterlayout");
    QVBoxLayout *lListLayout = new QVBoxLayout(6, "attachlistlayout");
    QVBoxLayout *lButtonLayout = new QVBoxLayout(6, "attachbuttonlayout");
    QSpacerItem* lSpacer = new QSpacerItem( 0, 156, QSizePolicy::Minimum, QSizePolicy::Expanding );
    
    // create the list box for the applications on the grid
    lListLayout->addWidget(new TableLabel("Steerable Applications", this));
    mTable = new QTable(0, 2, this);
    mTable->setSelectionMode( QTable::Single );
    mTable->verticalHeader()->hide();
    mTable->setLeftMargin(0);

    mTable->horizontalHeader()->setLabel(0, "Application");
    mTable->horizontalHeader()->setLabel(1, "Handle");

    connect(mTable, SIGNAL(valueChanged(int, int)), this, SLOT(editHandleSlot(int, int)));

    // populate the list box - each listboxitem holds array index information - this is what 
    // is used by the calling code (via  aSimIndexSelected) to identify the aSimGSH selected
    //AttachListItem *lListItem;

    //QTableItem *lTableItem;
    for (int i=0; i<mNumSims; i++)
    {      
      DBGMSG1("mSimName ", mSimName[i]);
      DBGMSG1("mSimGSH ", mSimGSH[i]);

      // QString(const char *) is deep copy
      //lListItem = new AttachListItem(i, QString( mSimName[i]));
      //mListBox->insertItem( lListItem );
      mTable->insertRows(mTable->numRows(),1);
      mTable->setItem(mTable->numRows()-1, 0, new QTableItem(mTable, QTableItem::Never, QString(mSimName[i])));
      mTable->setItem(mTable->numRows()-1, 1, new QTableItem(mTable, QTableItem::OnTyping, QString(mSimGSH[i])));
    }
    
    // filter to do SMR XXX
    mFilterLineEdit = new QLineEdit(this, "containsfilter");
    connect(mFilterLineEdit, SIGNAL(returnPressed()), this, SLOT(filterSlot()));
  
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
    lButtonLayout->addWidget(mAttachButton);
    lButtonLayout->addWidget(mCancelButton);
    
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

  DBGMSG1("Slot: mNumSims: ", mNumSims);
  //AttachListItem *lTmpPtr;
  //int lCurrentItem = mListBox->currentItem();
  int lCurrentItem = mTable->currentSelection();
  DBGMSG1("currentSelection ", lCurrentItem); 
  bool lSelected = false;

  if (lCurrentItem >= 0)
  {
//    if (mListBox->isSelected(lCurrentItem))
      if (mTable->isRowSelected(lCurrentItem))
    { 
      lSelected = true;
//      lTmpPtr = (AttachListItem *)mListBox->item(lCurrentItem);
   
//      DBGMSG1("selected app index: ",lTmpPtr->getSimIndex());
      
//      mSimGSHSelected = mSimGSH[lTmpPtr->getSimIndex()];
      mSimGSHSelected = mSimGSH[lCurrentItem];

      DBGMSG1("selected app GSH: ",mSimGSHSelected);

      QDialog::accept();
    }
  }

  if (!lSelected)
  {
    // no item in the list has been selected
    QMessageBox::information(0, "Nothing selected", "Please select an item in the list",
			     QMessageBox::Ok,
			     QMessageBox::NoButton, 
			     QMessageBox::NoButton);
    

  }
    

}


void
AttachForm::filterSlot()
{
  DBGMSG1("FSLot: mNumSims: ", mNumSims);

  // remove list and start again applying filter
  //mListBox->clear();
  for (int i=0; i<mTable->numRows(); i++){
    mTable->clearCell(i,1);
    mTable->clearCell(i,0);
  }

  int lLen = mFilterLineEdit->text().length();

  //AttachListItem *lListItem;
  
  if (lLen > 0)
  {
    for (int i=0; i<mNumSims; i++)
    {  
      DBGMSG2("***filter: ", mSimName[i], mFilterLineEdit->text().latin1());
      if (strstr(mSimName[i], mFilterLineEdit->text().latin1()) != kNULL)
      {
	//lListItem = new AttachListItem(i, QString(mSimName[i]));
  //mListBox->insertItem( lListItem );
        mTable->insertRows(mTable->numRows(),1);
        mTable->setItem(mTable->numRows()-1, 0, new QTableItem(mTable, QTableItem::Never, QString(mSimName[i])));
        mTable->setItem(mTable->numRows()-1, 1, new QTableItem(mTable, QTableItem::OnTyping, QString(mSimGSH[i])));
      }      
    }
  }
  else
  {
    // no filter specified so just show all sim names
    for (int i=0; i<mNumSims; i++)
    {
      //lListItem = new AttachListItem(i, QString(mSimName[i]));
      //mListBox->insertItem( lListItem );
        mTable->insertRows(mTable->numRows(),1);
        mTable->setItem(mTable->numRows()-1, 0, new QTableItem(mTable, QTableItem::Never, QString(mSimName[i])));
        mTable->setItem(mTable->numRows()-1, 1, new QTableItem(mTable, QTableItem::OnTyping, QString(mSimGSH[i])));
    }
  }

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
