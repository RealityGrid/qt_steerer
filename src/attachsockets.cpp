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

  Author: Robert Haines
 */

/** @file attachsockets.cpp
    @author Robert Haines */

#include <QtGui>

#include "buildconfig.h"
#include "attachsockets.h"
#include "types.h"
#include "debug.h"

AttachSockets::AttachSockets(QWidget* parent, bool modal)
  : QDialog(parent) {
  setModal(modal);

  getAppAddress();
  createLayout();

  mAttachButton = new QPushButton(tr("Attach"));
  mCancelButton = new QPushButton(tr("Cancel"));
  mAttachButton->setDefault(true);

  connect(mAttachButton, SIGNAL(clicked()), this, SLOT(attachSlot()));
  connect(mCancelButton, SIGNAL(clicked()), this, SLOT(reject()));

  QHBoxLayout* lButtonLayout = new QHBoxLayout;
  lButtonLayout->addStretch(1);
  lButtonLayout->addWidget(mAttachButton);
  lButtonLayout->addWidget(mCancelButton);
  lButtonLayout->addStretch(1);

  QVBoxLayout* lMainLayout = new QVBoxLayout;
  lMainLayout->addWidget(mLayout);
  lMainLayout->addLayout(lButtonLayout);

  setLayout(lMainLayout);
  setWindowTitle(tr("Attach to host:"));
}

AttachSockets::~AttachSockets() {
  REG_DBGDST("AttachSockets");
  cleanUp();
}

void AttachSockets::createLayout() {
  mLayout = new QGroupBox(tr("Server details"));
  QGridLayout* lGrid = new QGridLayout;

  QRegExp lRx("[A-Za-z0-9][A-Za-z0-9\\-]+(\\.[A-Za-z0-9][A-Za-z0-9\\-]+)*");
  QValidator* lIntValidator = new QIntValidator(1024, 65535, this);
  QValidator* lRexValidator = new QRegExpValidator(lRx, this);
  QString lUseEnv("Use REG_APP_ADDRESS (" + mAppAddress + ")");

  mLabels[0] = new QLabel(tr("Address:"));
  mLabels[0]->setAlignment(Qt::AlignRight);
  mLabels[1] = new QLabel(tr("Port:"));
  mLabels[1]->setAlignment(Qt::AlignRight);

  mInputs[0] = new QLineEdit;
  mInputs[1] = new QLineEdit;
  mInputs[0]->setValidator(lRexValidator);
  mInputs[1]->setValidator(lIntValidator);
  mInputs[1]->setMaxLength(5);

  mUseEnv = new QCheckBox(lUseEnv);
  connect(mUseEnv, SIGNAL(stateChanged(int)), this, SLOT(useEnvSlot(int)));

  lGrid->addWidget(mLabels[0], 0, 0);
  lGrid->addWidget(mInputs[0], 0, 1);
  lGrid->addWidget(mLabels[1], 1, 0);
  lGrid->addWidget(mInputs[1], 1, 1);
  lGrid->addWidget(mUseEnv, 2, 0, 1, 2);

  // only enable the checkbox if REG_APP_ADDRESS is set
  if(!mAppAddressSet) {
    mUseEnv->setEnabled(false);
  }

  mLayout->setLayout(lGrid);
}

void AttachSockets::getAppAddress() {
  mAppAddress = QString(getenv("REG_APP_ADDRESS"));
  if(mAppAddress.isNull() || mAppAddress.isEmpty()) {
    mAppAddress = QString("Not set");
    mAppAddressSet = false;
  }
  else {
    mAppAddressSet = true;
  }
}

QString AttachSockets::getRemote() const {
  return mAppAddress;
}

void AttachSockets::attachSlot() {
  // Should we use REG_APP_ADDRESS?
  if(mAppAddressSet && mUseEnv->isChecked()) {
    QDialog::accept();
    return;
  }

  // Get the address from the input boxes
  if(!mInputs[0]->hasAcceptableInput()) {
    QMessageBox::information(this, "Invalid hostname",
			     "Hostname should be of the form 'localhost', "
			     "'server' or 'server.domain'",
			     QMessageBox::Ok,
			     QMessageBox::NoButton,
			     QMessageBox::NoButton);
    return;
  }
  if(!mInputs[1]->hasAcceptableInput()) {
    QMessageBox::information(this, "Invalid port number",
			     "Port should be a number between 1024 and 65535",
			     QMessageBox::Ok,
			     QMessageBox::NoButton,
			     QMessageBox::NoButton);
    return;
  }

  mAppAddress = mInputs[0]->text() + ':' + mInputs[1]->text();
  QDialog::accept();
}

void AttachSockets::useEnvSlot(int state) {
  if(state == Qt::Unchecked) {
    mInputs[0]->setEnabled(true);
    mInputs[1]->setEnabled(true);
  }

  if(state == Qt::Checked) {
    mInputs[0]->setEnabled(false);
    mInputs[1]->setEnabled(false);
  }
}

void AttachSockets::cleanUp() {

}
