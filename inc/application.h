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
  /// Set the string holding the current application status
  void setCurrentStatus(QString &msg);
  /// Get the string holding the current application status
  QString getCurrentStatus();
  void hideChkPtTable(bool flag);
  void hideIOTable(bool flag);
  void hideSteerTable(bool flag);
  void hideMonTable(bool flag);

  /** Getter method for current visibility of mon. params table */
  bool monTableVisible();
  /** Getter method for current visibility of steered params table */
  bool steerTableVisible();
  /** Getter method for current visibility of IOTypes table */
  bool ioTableVisible();
  /** Getter method for current visibility of ChkTypes table */
  bool chkTableVisible();
  /** Toggle the visibility of the Monitored Parameters table */
  void setMonTableVisible(bool flag);
  /** Toggle the visibility of the Steered Parameters table */
  void setSteerTableVisible(bool flag);
  /** Toggle the visibility of the IOTypes table */
  void setIOTableVisible(bool flag);
  /** Toggle the visibility of the ChkTypes table */
  void setChkTableVisible(bool flag);

private:
  void detachFromApplication();
  void disableForDetach(const bool aUnRegister = true);
  void disableForDetachOnError();
  /** Sends the single, supplied command to the application
      @returns REG_SUCCESS or REG_FAILURE */
  int  emitSingleCmd(int aCmdId);

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
