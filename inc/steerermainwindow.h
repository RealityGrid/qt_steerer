/*----------------------------------------------------------------------------
    SteererMainWindow class header file for QT steerer GUI.

    (C)Copyright 2002 The University of Manchester, United Kingdom,
    all rights reserved.

    This software is produced by the Supercomputing, Visualization &
    e-Science Group, Manchester Computing, the Victoria University of
    Manchester as part of the RealityGrid project.

    This software has been tested with care but is not guaranteed for
    any particular purpose. Neither the authors, nor the University of
    Manchester offer any warranties or representations, nor do they
    accept any liabilities with respect to this software.

    This program must not be used for commmercial gain without the
    written permission of the authors.
    
    Supercomputing, Visualization & e-Science Group
    Manchester Computing
    University of Manchester
    Manchester M13 9PL

    email:  csar-advice@cfs.ac.uk.
    Tel:    +44 161 275 6824/5997
    Fax:    +44 161 275 6040    
    
    Date          Version    Updates                            Author
    ----          -------    -------                            ------
    03.10.2002      0.1                                         S Ramsden

---------------------------------------------------------------------------*/

#ifndef __STEERER_MAINWINDOW_H__
#define __STEERER_MAINWINDOW_H__

#include <qmainwindow.h>

class QLayout;
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
 
private:
  void cleanUp();

  bool isThreadRunning() const;					     
  void resizeForNoAttached();

protected slots:
  void attachAppSlot();
  void quitSlot(); 
  void readMsgSlot();
  void closeApplicationSlot(int aSimHandle);


private:

  QWidget	*mCentralWgt;
  QHBoxLayout	*mTopLayout;

  QVBoxLayout	*mLeftLayout;
  QVBoxLayout	*mRightLayout;

  QPushButton	*mAttachButton;

  QPushButton	*mQuitButton;
  QPushButton	*mReadMsgButton;

  QWidgetStack  *mStack;
  QTabWidget	*mAppTabs;

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
