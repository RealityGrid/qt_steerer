/*----------------------------------------------------------------------------
  Application class header file for QT steerer GUI.

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


#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "types.h"

#include <qwidget.h>

class QGroupBox;
class QPushButton;
class QString;

class ControlForm;
class SteererMainWindow;

class Application : public QWidget
{
  Q_OBJECT

public:

  Application(QWidget *mParent, const char *, int aSimHandle, bool aIsLocal);
  ~Application();

  void customEvent(QCustomEvent *);
  void processNextMessage(int aMsgType);
  void enableCmdButtons();

  bool isLocal(){return mIsLocal;}

private:
  void detachFromApplication();
  void disableForDetach(const bool aUnRegister = true);
  void emitSingleCmd(int aCmdId);

protected slots:
  void emitDetachCmdSlot();
  void emitStopCmdSlot();
  void emitPauseCmdSlot();
  void emitResumeCmdSlot();
  void emitGridRestartCmdSlot();
  void closeApplicationSlot();
  void detachFromApplicationForErrorSlot();

signals:
  void closeApplicationSignal(int aSimHandle);


private:
  const int		mSimHandle;

  int		mNumCommands;
  bool		mDetachSupported;
  bool		mStopSupported;
  bool		mPauseSupported;
  bool		mResumeSupported;
  bool		mDetachedFlag;

  ControlForm	*mControlForm;
  QGroupBox	*mControlBox;
  SteererMainWindow *mSteerer;

  bool mIsLocal;


};


#endif
