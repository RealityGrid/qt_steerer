/*----------------------------------------------------------------------------
    AttachForm class header file for QT steerer GUI. 

    (C)Copyright 2002 The University of Manchester, United Kingdom,
    all rights reserved.

    This software is produced by the Supercomputing, Visualization &
    e-Science Group, Manchester Computing, the Victoria University of
    Manchester as part of the RealityGrid project.

    This software has been tested with care but is not guaranteed for
    any particular purpose. Neither the authors, nor the University of
    Manchester offer any warranties or representations, nor do they
    accept any liabilities with respect to this software.

    This program must not be used for commmercial gain without the
    written permission of the authors.
    
    Supercomputing, Visualization & e-Science Group
    Manchester Computing
    University of Manchester
    Manchester M13 9PL

    email:  csar-advice@cfs.ac.uk.
    Tel:    +44 161 275 6824/5997
    Fax:    +44 161 275 6040    
    
    Date          Version    Updates                            Author
    ----          -------    -------                            ------
    23.10.2002      0.1                                         S Ramsden

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
#include <qvbox.h>

AttachForm::AttachForm(QWidget *parent, const char *name,
		       bool modal, WFlags f)
  : QDialog( parent, name, modal, f ), mNumSims(0), mSimGSHSelected(kNULL),
    mSimName(kNULL), mSimGSH(kNULL), mListBox(kNULL), mFilterLineEdit(kNULL),
    mAttachButton(kNULL), mCancelButton(kNULL),  mLibReturnStatus(REG_SUCCESS)
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

  // tmp code SMR XXX
   mNumSims = 3;
  strcpy(mSimName[0], "sue1");
  strcpy(mSimName[1], "sue2");
  strcpy(mSimName[2], "sue3");

  strcpy(mSimGSH[0], "handle1");
  strcpy(mSimGSH[1], "handle2");
  strcpy(mSimGSH[2], "handle3");

  // get the list of grid applications from library
  //  mLibReturnStatus = Get_sim_list(&mNumSims, mSimName, mSimGSH); //SMR XXX need javac

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
    mListBox = new QListBox(this);
    mListBox->setSelectionMode( QListBox::Single );
    
     
    // populate the list box - each listboxitem holds array index information - this is what 
    // is used by the calling code (via  aSimIndexSelected) to identify the aSimGSH selected
    AttachListItem *lListItem;
    for (int i=0; i<mNumSims; i++)
    {      
      // QString(const char *) is deep copy
      lListItem = new AttachListItem(i, QString( mSimName[i]));
      mListBox->insertItem( lListItem );
    }
    
    // filter to do SMR XXX
    mFilterLineEdit = new QLineEdit(this, "containsfilter");
    connect(mFilterLineEdit, SIGNAL(returnPressed()), this, SLOT(filterSlot()));
  
    lFilterLayout->addWidget(new QLabel("Contains", this));
    lFilterLayout->addWidget(mFilterLineEdit);
    
    
    lListLayout->addWidget(mListBox);
    lListLayout->addLayout(lFilterLayout);
    
    mAttachButton = new QPushButton("Attach", this, "attachbutton"); 
    mAttachButton->setMinimumSize(mAttachButton->sizeHint());
    mAttachButton->setMaximumSize(mAttachButton->sizeHint());
    mAttachButton->setAutoDefault(FALSE);
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
  AttachListItem *lTmpPtr;
  int lCurrentItem = mListBox->currentItem();
  bool lSelected = false;

  if (lCurrentItem >= 0)
  {
    if (mListBox->isSelected(lCurrentItem))
    { 
      lSelected = true;
      lTmpPtr = (AttachListItem *)mListBox->item(lCurrentItem);
      
      DBGMSG1("selected app index: ",lTmpPtr->getSimIndex());
      
      mSimGSHSelected = mSimGSH[lTmpPtr->getSimIndex()];
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
  mListBox->clear();

  int lLen = mFilterLineEdit->text().length();

  AttachListItem *lListItem;
  
  if (lLen > 0)
  {
    for (int i=0; i<mNumSims; i++)
    {  
      DBGMSG2("***filter: ", mSimName[i], mFilterLineEdit->text().latin1());
      if (strstr(mSimName[i], mFilterLineEdit->text().latin1()) != kNULL)
      {
	lListItem = new AttachListItem(i, QString(mSimName[i]));
	mListBox->insertItem( lListItem );
      }      
    }
  }
  else
  {
    // no filter specified so just show all sim names
    for (int i=0; i<mNumSims; i++)
    {
      lListItem = new AttachListItem(i, QString(mSimName[i]));
      mListBox->insertItem( lListItem );
    }
  }

}


