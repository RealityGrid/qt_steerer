/*----------------------------------------------------------------------------
    SteererMainWindow class header file for QT steerer GUI.

    (C)Copyright 2002 The University of Manchester, United Kingdom,
    all rights reserved.

    This software is produced by the Supercomputing, Visualization &
    e-Science Group, Manchester Computing, the Victoria University of
    Manchester as part of the RealityGrid project.

    This software has been tested with care but is not guaranteed for
    any particular purpose. Neither the copyright holder, nor the
    University of Manchester offer any warranties or representations,
    nor do they accept any liabilities with respect to this software.

    This software must not be used for commercial gain without the
    written permission of the authors.
    
    This software must not be redistributed without the written
    permission of the authors.

    Permission is granted to modify this software, provided any
    modifications are made freely available to the original authors.
 
    Supercomputing, Visualization & e-Science Group
    Manchester Computing
    University of Manchester
    Manchester M13 9PL
    
    WWW:    http://www.sve.man.ac.uk  
    email:  sve@man.ac.uk
    Tel:    +44 161 275 6095
    Fax:    +44 161 275 6800    

    Initial version by: S Ramsden, 03.10.2002
    
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

class Application;
class CommsThread;

class SteererMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  SteererMainWindow();
  ~SteererMainWindow();

  Application * getApplication() const;  //SMR XXX const?
  void customEvent(QCustomEvent *);

private:
  void cleanUp();

  bool isThreadRunning() const;					     
  void resizeForNoAttached();

protected slots:
  void attachAppSlot();
  void attachGridAppSlot();
  void quitSlot(); 
  void readMsgSlot();
  void closeApplicationSlot(int aSimHandle);
  void configureSteererSlot();

private:
  void simAttachApp(char *aSimID, bool aIsLocal = false);

private:

  QWidget	*mCentralWgt;
  QHBoxLayout	*mTopLayout;

  QVBoxLayout	*mLeftLayout;
  QVBoxLayout	*mRightLayout;

  QPushButton	*mAttachButton;
  QPushButton	*mGridAttachButton;

  QPushButton	*mQuitButton;
  QPushButton	*mReadMsgButton;

  QWidgetStack  *mStack;
  QTabWidget	*mAppTabs;

  //SMR XXX not sure what copting going on here so done 2 to be safe for demo
  QLabel	*mLogoLabel;
  QPixmap	*mLogoPixMap;
  QLabel	*mStackLogoLabel;
  QPixmap	*mStackLogoPixMap;
 
  QAction	*mSetCheckIntervalAction;
  QAction	*mAttachAction;
  QAction	*mGridAttachAction;
  QAction	*mQuitAction;

  CommsThread	*mCommsThread;

  Application	*mApplication; //SMR XXX will be list - future
				
				
};

/// SMR XXX - future for posting app in new window
///SMR class ApplicationWindow : public QMainWindow
///SMR {
///SMR 	Q_OBJECT
///SMR 
///SMR public:
///SMR 
///SMR 	ApplicationWindow(const char *, int aSimHandle);
///SMR 	~ApplicationWindow() {}
///SMR 
///SMR 	Application * getApplication() {return mApplication;}
///SMR 
///SMR private:
///SMR 	Application	*mApplication;
///SMR 
///SMR };


#endif
