/*----------------------------------------------------------------------------
  SteererMainWindow class header file for QT steerer GUI.

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
