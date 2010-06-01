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

#ifndef __STEERER_MAINWINDOW_H__
#define __STEERER_MAINWINDOW_H__

#include <q3mainwindow.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3HBoxLayout>
#include <QEvent>
#include <QLabel>
#include <Q3PtrList>

class Q3Action;
class QLabel;
class QLayout;
class QPixMap;
class QPushButton;
class QTabWidget;
class QWidget;
class Q3WidgetStack;

#include "application.h"
#include "steererconfig.h"

class CommsThread;

class SteererMainWindow : public Q3MainWindow
{
  Q_OBJECT

public:
  SteererMainWindow(bool autoConnect = false, const char *aSGS = NULL);
  ~SteererMainWindow();

  Application * getApplication(int aSimHandle);
  void customEvent(QEvent *);

  /// Queries whether or not automatic polling is on or off
  bool  autoPollingOn();
  /// Returns the current value of the polling interval in seconds
  float getPollingIntervalSecs();

  /// Returns a pointer to the SteererConfig object
  SteererConfig *getConfig();

private:
  void cleanUp();

  bool isThreadRunning() const;
  void resizeForNoAttached();

protected slots:
  void attachAppSlot();
  void quitSlot();
  // Replaced by thead to get messages
  //void readMsgSlot();
  void closeApplicationSlot(int aSimHandle);
  void configureSteererSlot();
  void toggleAutoPollSlot();
  void tabChangedSlot(QWidget *aWidget);
  void editTabTitleSlot();
  void hideChkPtTableSlot();
  void hideIOTableSlot();
  void hideSteerTableSlot();
  void hideMonTableSlot();

public slots:
  void statusBarMessageSlot(Application *aApp, QString &message);

private:
  void simAttachApp(const char* aSimID, bool aIsLocal = false);

private:
  /// Holds the EPR of service to attach to if specified on cmd line
  QString        cmdLineSGS;
  QWidget	*mCentralWgt;
  Q3HBoxLayout	*mTopLayout;

  Q3WidgetStack  *mStack;
  QTabWidget	*mAppTabs;

  QLabel	*mStackLogoLabel;
  QPixmap	*mStackLogoPixMap;

  CommsThread	*mCommsThread;
  QMutex         mReGMutex;
  Q3Action	*mSetCheckIntervalAction;
  Q3Action	*mToggleAutoPollAction;
  Q3Action	*mAttachAction;
  Q3Action       *mSetTabTitleAction;
  Q3Action	*mQuitAction;

  Q3Action       *mHideChkPtTableAction;
  Q3Action       *mHideIOTableAction;
  Q3Action       *mHideSteerTableAction;
  Q3Action       *mHideMonTableAction;

  Q3PtrList<Application> mAppList;
  /// Holds the configuration information for the steering client
  SteererConfig *mSteererConfig;

  // The type of steering we are using
  QString       *mSteerType;
};

#endif
