/*----------------------------------------------------------------------------
  AttachSockets class header file for QT steerer GUI.

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

  Authors........: Robert Haines, Mark Riding, Andrew Porter, Sue Ramsden
    
---------------------------------------------------------------------------*/

#ifndef __ATTACHSOCKETS_H__
#define __ATTACHSOCKETS_H__

#include <QDialog>

class QString;
class QPushButton;
class QWidget;
class QGroupBox;
class QLabel;
class QLineEdit;
class QCheckBox;

class AttachSockets : public QDialog {

  Q_OBJECT

public:
  AttachSockets(QWidget* parent = 0, bool modal = true);
  ~AttachSockets();

  QString getRemote() const;

protected slots:
  void attachSlot();
  void useEnvSlot(int);

private:
  void createLayout();
  void getAppAddress();
  void cleanUp();

  bool    mAppAddressSet;
  QString mAppAddress;

  QGroupBox*   mLayout;
  QLabel*      mLabels[2];
  QLineEdit*   mInputs[2];
  QCheckBox*   mUseEnv;
  QPushButton* mAttachButton;
  QPushButton* mCancelButton;
};

#endif // __ATTACHSOCKETS_H__
