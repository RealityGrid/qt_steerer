/*----------------------------------------------------------------------------
  ConfigForm class header file for QT steerer GUI. 

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

 
#include "configform.h"
#include "utility.h"
#include "types.h"
#include "debug.h"

#include <qvalidator.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qvbox.h>

#include <math.h>

ConfigForm::ConfigForm(int aCurrentIntervalValue, QWidget *parent, 
		       const char *name,
		       bool modal, WFlags f)
  : QDialog( parent, name, modal, f ), 
    mApplyButton(kNULL), mCancelButton(kNULL)
{
  //  mMinVal_Sec = 0.001*kMIN_POLLING_INT;
  //  mMaxVal_Sec = 0.001*kMAX_POLLING_INT;
  mMinVal_Sec = 0.05f;
  mMaxVal_Sec = 2.0f;

  DBGMSG1("ARPDBG: min val = ", mMinVal_Sec);
  DBGMSG1("ARPDBG: max val = ", mMaxVal_Sec);

  DBGCON("ConfigForm");

  // note aCurrentIntervalValue is in milliseconds - convert to 
  // seconds for GUI entry

  this->setCaption( "Configure Polling" );
  resize( 150, 150 );
    
  // create the layouts for the form
  QVBoxLayout *lFormLayout = new QVBoxLayout(this, 10, 10, "configformlayout");
  QHBoxLayout *lButtonLayout = new QHBoxLayout(6, "configbuttonlayout");

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
  DBGDST("ConfigForm");
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

