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

#include <qmainwindow.h>

class QAction;
class QLabel;
class QLayout;
class QPixMap;
class QPushButton;
class QTabWidget;
class QWidget;
class QWidgetStack;

#include "application.h"
#include "steererconfig.h"

class CommsThread;

class SteererMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  SteererMainWindow(bool autoConnect = false, const char *aSGS = NULL);
  ~SteererMainWindow();

  Application * getApplication(int aSimHandle);
  void customEvent(QCustomEvent *);

  bool  autoPollingOn();
  float getPollingIntervalSecs();
private:
  void cleanUp();

  bool isThreadRunning() const;					     
  void resizeForNoAttached();

protected slots:
  void attachAppSlot();
  void attachGridAppSlot();
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
  void simAttachApp(char *aSimID, bool aIsLocal = false);

private:

  QString cmdLineSGS;

  QWidget	*mCentralWgt;
  QHBoxLayout	*mTopLayout;

  QWidgetStack  *mStack;
  QTabWidget	*mAppTabs;

  QLabel	*mStackLogoLabel;
  QPixmap	*mStackLogoPixMap;

  CommsThread	*mCommsThread;
  QMutex         mReGMutex;
  QAction	*mSetCheckIntervalAction;
  QAction	*mToggleAutoPollAction;
  QAction	*mAttachAction;
  QAction	*mGridAttachAction;
  QAction       *mSetTabTitleAction;
  QAction	*mQuitAction;

  QAction       *mHideChkPtTableAction;
  QAction       *mHideIOTableAction;
  QAction       *mHideSteerTableAction;
  QAction       *mHideMonTableAction;

  QPtrList<Application> mAppList;

  SteererConfig *mSteererConfig;
};

#endif
