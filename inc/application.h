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

/** @file application.h
 *  @brief Header file for class holding details on application being steered.
 */

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "types.h"

#include <qwidget.h>
#include <qmutex.h>
#include <QEvent>

class Q3GroupBox;
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

  void customEvent(QEvent *);
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
  Q3GroupBox	*mControlBox;
  SteererMainWindow *mSteerer;

  bool mChkTableVisible;
  bool mIOTableVisible;
  bool mSteerTableVisible;
  bool mMonTableVisible;

};


#endif
