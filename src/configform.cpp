/*
  The RealityGrid Steerer

  Copyright (c) 2002-2009, University of Manchester, United Kingdom.
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

/** @file configform.cpp
    @brief Implementation of the ConfigForm class

    @author Sue Ramsden
    @author Mark Riding
    @author Andrew Porter */
 
#include "configform.h"
#include "utility.h"
#include "types.h"
#include "debug.h"

#include <qvalidator.h>
#include <q3hbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <q3vbox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <QLabel>

#include <math.h>

ConfigForm::ConfigForm(int aCurrentIntervalValue, QWidget *parent, 
		       const char *name,
		       bool modal, Qt::WFlags f)
  : QDialog( parent, name, modal, f ), 
    mApplyButton(kNULL), mCancelButton(kNULL)
{
  mMinVal_Sec = 0.05f;
  mMaxVal_Sec = 2.0f;

  REG_DBGMSG1("ARPDBG: min val = ", mMinVal_Sec);
  REG_DBGMSG1("ARPDBG: max val = ", mMaxVal_Sec);

  REG_DBGCON("ConfigForm");

  // note aCurrentIntervalValue is in milliseconds - convert to 
  // seconds for GUI entry

  this->setCaption( "Configure Polling" );
  resize( 150, 150 );
    
  // create the layouts for the form
  Q3VBoxLayout *lFormLayout = new Q3VBoxLayout(this, 10, 10, "configformlayout");
  Q3HBoxLayout *lButtonLayout = new Q3HBoxLayout(6, "configbuttonlayout");

  lFormLayout->addWidget(new QLabel("Enter interval value (seconds) \n"
				    "Valid range: "
				    +QString::number(mMinVal_Sec)
				    +" - "+QString::number(mMaxVal_Sec)
				    +" (2 d.p.)", this));

  mLineEdit = new QLineEdit( this );

  double lVal = (double) aCurrentIntervalValue/1000;

  mLineEdit->setText(QString::number(lVal, 'g', 3));
  mLineEdit->setValidator( new QDoubleValidator(mLineEdit, "dbleavlidator" ) );

  lFormLayout->addWidget( mLineEdit);

  mApplyButton = new QPushButton("Apply", this, "Applybutton"); \
  mApplyButton->setAutoDefault(FALSE);
  QToolTip::add(mApplyButton, "Apply to steerer");
  connect(mApplyButton, SIGNAL(clicked()), this, SLOT(applySlot()));  
    
  mCancelButton = new QPushButton("Cancel", this, "cancelbutton");
  mCancelButton->setAutoDefault(FALSE);
  connect(mCancelButton,  SIGNAL(clicked()), this, SLOT( reject()));

  mCancelButton->setMinimumSize(mCancelButton->sizeHint());
  mCancelButton->setMaximumSize(mCancelButton->sizeHint());
  mApplyButton->setMinimumSize(mCancelButton->sizeHint());
  mApplyButton->setMaximumSize(mApplyButton->sizeHint());
    
  lButtonLayout->addWidget(mApplyButton);
  lButtonLayout->addWidget(mCancelButton);
  
  lFormLayout->addLayout(lButtonLayout);

}

ConfigForm::~ConfigForm()
{
  REG_DBGDST("ConfigForm");
}

int
ConfigForm::getIntervalValue(void) const
{
  return mIntervalValue;
}


void
ConfigForm::applySlot()
{
  bool lOk = false;
  double lValue;
  if (!mLineEdit->text().isEmpty())
  {
    lValue = mLineEdit->text().toDouble(&lOk);
    if (lOk)
    {
      if (lValue >= mMinVal_Sec && lValue <= mMaxVal_Sec)
      {
	// keep only two d.p.; add 0.0001 to for .99999 representation problem
	lValue += 0.0001;
	lValue=floor(lValue*100);

	//convert to int and milliseconds
	mIntervalValue = (int) (lValue*10);
	QDialog::accept();
      }
      else
	lOk=false;
    }

    if (!lOk)
    { 
      //value is out of range
      QMessageBox::information(0, "Invalid entry", 
			       "Please enter a value between "+
			       QString::number(mMinVal_Sec)+" and "+
			       QString::number(mMaxVal_Sec),
			       QMessageBox::Ok,
			       QMessageBox::NoButton, 
			       QMessageBox::NoButton);
    }

  }
  else
  {
    // no item in the list has been selected
    QMessageBox::information(0, "No value", "Please enter a value",
			     QMessageBox::Ok,
			     QMessageBox::NoButton, 
			     QMessageBox::NoButton);
    

  }
}

