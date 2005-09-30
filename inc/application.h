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

/** @file application.h
 *  @brief Header file for class holding details on application being steered.
 */

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "types.h"

#include <qwidget.h>
#include <qmutex.h>

class QGroupBox;
class QPushButton;
class QString;

class ControlForm;
class SteererMainWindow;
class CommsThreadEvent;

/** Holds information on an application that the steering client is
    attached to */
class Application : public QWidget
{
  Q_OBJECT

public:

  Application(QWidget *mParent, const char *, int aSimHandle, 
	      bool aIsLocal,
	      QMutex *aMutex);
  ~Application();

  void customEvent(QCustomEvent *);
  void processNextMessage(CommsThreadEvent *aEvent);
  /// Enable all the command buttons for this application
  void enableCmdButtons();

  /** Getter method for mIsLocal 
      @see mIsLocal */
  bool isLocal(){return mIsLocal;}
  /// Getter method for handle of application
  int  getHandle();
  void setCurrentStatus(QString &msg);
  QString getCurrentStatus();
  void hideChkPtTable(bool flag);
  void hideIOTable(bool flag);
  void hideSteerTable(bool flag);
  void hideMonTable(bool flag);

  bool monTableVisible();
  bool steerTableVisible();
  bool ioTableVisible();
  bool chkTableVisible();
  void setMonTableVisible(bool flag);
  void setSteerTableVisible(bool flag);
  void setIOTableVisible(bool flag);
  void setChkTableVisible(bool flag);

private:
  void detachFromApplication();
  void disableForDetach(const bool aUnRegister = true);
  void disableForDetachOnError();
  void emitSingleCmd(int aCmdId);

protected slots:
  void emitDetachCmdSlot();
  void emitStopCmdSlot();
  void emitPauseCmdSlot();
  void emitGridRestartCmdSlot();
  void closeApplicationSlot();
  void detachFromApplicationForErrorSlot();

signals:
  void closeApplicationSignal(int aSimHandle);


private:
  const int	mSimHandle;

  /** Pointer to mutex controlling access to steering lib */
  QMutex        *mMutexPtr;
  int		mNumCommands;
  bool		mDetachSupported;
  bool		mStopSupported;
  bool		mPauseSupported;
  bool		mResumeSupported;
  bool		mDetachedFlag;
  /** Whether application is local or remote */
  bool          mIsLocal;
  QString       mStatusTxt;
  ControlForm	*mControlForm;
  QGroupBox	*mControlBox;
  SteererMainWindow *mSteerer;

  bool mChkTableVisible;
  bool mIOTableVisible;
  bool mSteerTableVisible;
  bool mMonTableVisible;

};


#endif
