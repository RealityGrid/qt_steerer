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

    Initial version by: S Ramsden, 23.10.2002
    
---------------------------------------------------------------------------*/

 
#include "chkptform.h"
#include "utility.h"
#include "types.h"
#include "debug.h"

#include <qapplication.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qvbox.h>

ChkPtForm::ChkPtForm(const int aNumEntries, int aSimHandle, int aChkPtHandle,
		     QWidget *parent, const char *name,
		     bool modal, WFlags f)
  : QDialog( parent, name, modal, f ), mNumEntries(aNumEntries), mLibReturnStatus(REG_SUCCESS),
    mIndexSelected(-1), mListBox(kNULL), 
    mFilterLineEdit(kNULL), mRestartButton(kNULL), mCancelButton(kNULL)
{

  DBGCON("ChkPtForm");

  //  set up arrays for ReG lib call

  mLogEntries = new Output_log_struct[mNumEntries];

  // get the log entries from library
  qApp->lock();
  mLibReturnStatus = Get_chk_log_entries(aSimHandle, aChkPtHandle, mNumEntries, mLogEntries); //ReG library
  qApp->unlock();

  // only continue is there is some info to show
  if(mLibReturnStatus == REG_SUCCESS)
  {
    this->setCaption( "CheckPoint Selector" );
    resize( 350, 350 );
    
    // create the layouts for the form
    QHBoxLayout *lFormLayout = new QHBoxLayout(this, 10, 10, "chkptformlayout");
    //    QHBoxLayout *lFilterLayout = new QHBoxLayout(6, "filterlayout");
    QVBoxLayout *lListLayout = new QVBoxLayout(6, "chkptlistlayout");
    QVBoxLayout *lButtonLayout = new QVBoxLayout(6, "chkptbuttonlayout");
    QSpacerItem* lSpacer = new QSpacerItem( 0, 156, QSizePolicy::Minimum, QSizePolicy::Expanding );
    
    // create the list box for the applications on the grid
    lListLayout->addWidget(new TableLabel("CheckPoint Tags", this));
    mListBox = new QListBox(this);
    mListBox->setSelectionMode( QListBox::Single );
    
     
    // populate the list box - each listboxitem holds array index information - this is what 
    // is used by the calling code (via  aSimIndexSelected) to identify the aSimGSH selected
    ChkPtListItem *lListItem;
    for (int i=0; i<mNumEntries; i++)
    {      
      DBGMSG1("ChkTag ", mLogEntries[i].chk_tag);

      // QString(const char *) is deep copy
      lListItem = new ChkPtListItem(i, QString( mLogEntries[i].chk_tag));
      mListBox->insertItem( lListItem );
    }
    
    // filter to do SMR XXX
///    mFilterLineEdit = new QLineEdit(this, "containsfilter");
///    connect(mFilterLineEdit, SIGNAL(returnPressed()), this, SLOT(filterSlot()));
///  
///    lFilterLayout->addWidget(new QLabel("Contains", this));
///    lFilterLayout->addWidget(mFilterLineEdit);
///    
    
    lListLayout->addWidget(mListBox);
    ///    lListLayout->addLayout(lFilterLayout);
    
    mRestartButton = new QPushButton("Restart", this, "restartbutton"); 
    mRestartButton->setMinimumSize(mRestartButton->sizeHint());
    mRestartButton->setMaximumSize(mRestartButton->sizeHint());
    mRestartButton->setAutoDefault(FALSE);
    QToolTip::add(mRestartButton, "Restart using selected checkpoint");
    connect(mRestartButton, SIGNAL(clicked()), this, SLOT(restartSlot()));
    
    
    mCancelButton = new QPushButton("Cancel", this, "cancelbutton");
    mCancelButton->setMinimumSize(mCancelButton->sizeHint());
    mCancelButton->setMaximumSize(mCancelButton->sizeHint());
    mCancelButton->setAutoDefault(FALSE);
    connect(mCancelButton,  SIGNAL(clicked()), this, SLOT( reject()));
    

    lButtonLayout->addItem(lSpacer);
    lButtonLayout->addWidget(mRestartButton);
    lButtonLayout->addWidget(mCancelButton);
    
    lFormLayout->addLayout(lListLayout);
    lFormLayout->addLayout(lButtonLayout);
  }

}

ChkPtForm::~ChkPtForm()
{
  DBGDST("ChkPtForm");
  cleanUp();
}

void
ChkPtForm::cleanUp()
{

  delete []  mLogEntries;
}


int 
ChkPtForm::getLibReturnStatus() const
{
  return mLibReturnStatus;
  
}

char *
ChkPtForm::getChkTagSelected() const
{
  return mLogEntries[mIndexSelected].chk_tag;

}


void
ChkPtForm::restartSlot()
{

  ChkPtListItem *lTmpPtr;
  int lCurrentItem = mListBox->currentItem();
  bool lSelected = false;

  if (lCurrentItem >= 0)
  {
    if (mListBox->isSelected(lCurrentItem))
    { 
      lSelected = true;
      lTmpPtr = (ChkPtListItem *)mListBox->item(lCurrentItem);
      
      DBGMSG1("selected app index: ",lTmpPtr->getEntryIndex());
      mIndexSelected = lTmpPtr->getEntryIndex();
      DBGMSG1("selected chkpt: ", mLogEntries[mIndexSelected].chk_tag);

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
ChkPtForm::filterSlot()
{
#if 0
  // remove list and start again applying filter
  mListBox->clear();

  int lLen = mFilterLineEdit->text().length();

  ChkPtListItem *lListItem;
  
  if (lLen > 0)
  {
    for (int i=0; i<mNumSims; i++)
    {  
      DBGMSG2("***filter: ", mSimName[i], mFilterLineEdit->text().latin1());
      if (strstr(mSimName[i], mFilterLineEdit->text().latin1()) != kNULL)
      {
	lListItem = new ChkPtListItem(i, QString(mSimName[i]));
	mListBox->insertItem( lListItem );
      }      
    }
  }
  else
  {
    // no filter specified so just show all sim names
    for (int i=0; i<mNumSims; i++)
    {
      lListItem = new ChkPtListItem(i, QString(mSimName[i]));
      mListBox->insertItem( lListItem );
    }
  }
#endif
}




ChkPtListItem::ChkPtListItem(int aEntryIndex, const QString &text)
  : QListBoxItem(), mEntryIndex(aEntryIndex)
{
  DBGCON("ChkPtListItem");
  setText( text );
  printf("constucted ChkPtListItem mEntryIndex = %d\n", mEntryIndex);

}

ChkPtListItem::~ChkPtListItem()
{
  DBGDST("ChkPtListItem");
}

int
ChkPtListItem::getEntryIndex() const
{
  return mEntryIndex;
}


void 
ChkPtListItem::paint( QPainter *painter )
{
    QFontMetrics fm = painter->fontMetrics();
    painter->drawText( 3, fm.ascent() +  (fm.leading()+1)/2 + 1, text() );
}

int 
ChkPtListItem::height( const QListBox* lb ) const
{
    int h = lb ? lb->fontMetrics().lineSpacing() + 2 : 0;
    if (h > QApplication::globalStrut().height())
      return h;
    else
      return QApplication::globalStrut().height();
}


int 
ChkPtListItem::width( const QListBox* lb ) const
{
    int w = lb ? lb->fontMetrics().width( text() ) + 6 : 0;
    if (w > QApplication::globalStrut().width())
      return w;
    else
      return QApplication::globalStrut().width();
}


