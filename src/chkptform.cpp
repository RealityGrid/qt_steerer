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

#include "chkptform.h"
#include "chkptvariableform.h"
#include "utility.h"
#include "types.h"
#include "debug.h"

#include <qapplication.h>
#include <q3hbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <q3vbox.h>
#include <qmutex.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

ChkPtForm::ChkPtForm(const int aNumEntries, int aSimHandle, int aChkPtHandle,
		     QMutex *aMutex, QWidget *parent, const char *name,
		     bool modal, Qt::WFlags f)
  : QDialog( parent, name, modal, f ), mNumEntries(aNumEntries), mLibReturnStatus(REG_SUCCESS),
    mIndexSelected(-1), mListBox(kNULL), mFilterLineEdit(kNULL),
    mRestartButton(kNULL), mCancelButton(kNULL), mMutexPtr(aMutex)
{

  REG_DBGCON("ChkPtForm");

  //  set up arrays for ReG lib call
  mLogEntries = new Output_log_struct[mNumEntries];

  // get the log entries from library
  mMutexPtr->lock();
  mLibReturnStatus = Get_chk_log_entries(aSimHandle, aChkPtHandle, mNumEntries, mLogEntries); //ReG library
  mMutexPtr->unlock();

  // only continue is there is some info to show
  if(mLibReturnStatus == REG_SUCCESS)
  {
    this->setCaption( "CheckPoint Selector" );
    resize( 350, 350 );

    // create the layouts for the form
    Q3HBoxLayout *lFormLayout = new Q3HBoxLayout(this, 10, 10, "chkptformlayout");
    //    QHBoxLayout *lFilterLayout = new QHBoxLayout(6, "filterlayout");
    Q3VBoxLayout *lListLayout = new Q3VBoxLayout(6, "chkptlistlayout");
    Q3VBoxLayout *lButtonLayout = new Q3VBoxLayout(6, "chkptbuttonlayout");

    // create the list box for the applications on the grid
    lListLayout->addWidget(new TableLabel("CheckPoint Tags", this));
    mListBox = new Q3ListBox(this);
    mListBox->setSelectionMode( Q3ListBox::Single );

    // populate the list box - each listboxitem holds array index information - this is what
    // is used by the calling code (via  aSimIndexSelected) to identify the aSimGSH selected
    ChkPtListItem *lListItem;
    for (int i=0; i<mNumEntries; i++)
    {
      REG_DBGMSG1("ChkTag ", mLogEntries[i].chk_tag);

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

    // MR: setup a button to deal with the view checkpoint parameters functionality
    mParametersButton = new QPushButton("Parameters", this, "parametersbutton");
    mParametersButton->setAutoDefault(FALSE);
    QToolTip::add(mParametersButton, "View Parameters for selected checkpoint");
    connect(mParametersButton, SIGNAL(clicked()), this, SLOT(viewChkPtParametersSlot()));
    connect(mListBox, SIGNAL(doubleClicked(Q3ListBoxItem*)), this, SLOT(viewChkPtParametersDblClkSlot(Q3ListBoxItem*)));

    mRestartButton = new QPushButton("Restart", this, "restartbutton");
    mRestartButton->setAutoDefault(FALSE);
    QToolTip::add(mRestartButton, "Restart using selected checkpoint");
    connect(mRestartButton, SIGNAL(clicked()), this, SLOT(restartSlot()));

    mCancelButton = new QPushButton("Cancel", this, "cancelbutton");
    mCancelButton->setAutoDefault(FALSE);
    connect(mCancelButton,  SIGNAL(clicked()), this, SLOT( reject()));

    mParametersButton->setMinimumSize(mParametersButton->sizeHint());
    mParametersButton->setMaximumSize(mParametersButton->sizeHint());
    mRestartButton->setMinimumSize(mParametersButton->sizeHint());
    mRestartButton->setMaximumSize(mRestartButton->sizeHint());
    mCancelButton->setMinimumSize(mParametersButton->sizeHint());
    mCancelButton->setMaximumSize(mCancelButton->sizeHint());

    lButtonLayout->addItem(new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ));
    lButtonLayout->addWidget(mParametersButton);
    lButtonLayout->addWidget(mRestartButton);
    lButtonLayout->addWidget(mCancelButton);

    lFormLayout->addLayout(lListLayout);
    lFormLayout->addLayout(lButtonLayout);
  }

}

ChkPtForm::~ChkPtForm()
{
  REG_DBGDST("ChkPtForm");
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

      REG_DBGMSG1("selected app index: ",lTmpPtr->getEntryIndex());
      mIndexSelected = lTmpPtr->getEntryIndex();
      REG_DBGMSG1("selected chkpt: ", mLogEntries[mIndexSelected].chk_tag);

      QDialog::accept();
    }
  }

  if (!lSelected)
  {
    // no item in the list has been selected
    QMessageBox::information(0, "Nothing selected",
			     "Please select an item in the list",
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
      REG_DBGMSG2("***filter: ", mSimName[i], mFilterLineEdit->text().latin1());
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

/** MR: When the user selects a checkpoint, show the variables
 */
void ChkPtForm::viewChkPtParametersSlot(){
  // We need to pass the relevant Output_log_struct to the new form,
  // so that it can populate it's own list appropiately
  int selectedIndex = mListBox->currentItem();

  Output_log_struct *tmp = &mLogEntries[selectedIndex];
  if (tmp==NULL){
    printf("Error - can't find any parameter log entries for this checkpoint");
    return;
  }

  // Create a modeless chkptvariableform and show it
  ChkPtVariableForm *lChkPtVariableForm = new ChkPtVariableForm(tmp, this);
  lChkPtVariableForm->show();

}

/** MR: When the user selects a checkpoint, show the variables
 */
void ChkPtForm::viewChkPtParametersDblClkSlot(Q3ListBoxItem *t){
  viewChkPtParametersSlot();
}


ChkPtListItem::ChkPtListItem(int aEntryIndex, const QString &text)
  : Q3ListBoxItem(), mEntryIndex(aEntryIndex)
{
  REG_DBGCON("ChkPtListItem");
  setText( text );
  printf("constucted ChkPtListItem mEntryIndex = %d\n", mEntryIndex);

}

ChkPtListItem::~ChkPtListItem()
{
  REG_DBGDST("ChkPtListItem");
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
ChkPtListItem::height( const Q3ListBox* lb ) const
{
    int h = lb ? lb->fontMetrics().lineSpacing() + 2 : 0;
    if (h > QApplication::globalStrut().height())
      return h;
    else
      return QApplication::globalStrut().height();
}


int
ChkPtListItem::width( const Q3ListBox* lb ) const
{
    int w = lb ? lb->fontMetrics().width( text() ) + 6 : 0;
    if (w > QApplication::globalStrut().width())
      return w;
    else
      return QApplication::globalStrut().width();
}
