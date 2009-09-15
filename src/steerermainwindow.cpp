/*----------------------------------------------------------------------------
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
---------------------------------------------------------------------------*/

/** @file steerermainwindow.cpp
    @brief SteererMainWindow class for QT steerer GUI.
    @author Sue Ramsden
    @author Mark Riding
    @author Andrew Porter */

#include "types.h"
#include "debug.h"
#include "ReG_Steer_Steerside.h"
#include "exception.h"
#include "steerermainwindow.h"
#include "commsthread.h"
#include "application.h"
#include "attachform.h"
#include "configform.h"

#include <qaction.h>
#include <qapplication.h>
#include <qcombobox.h>
#include <qinputdialog.h>
#include <q3filedialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpixmap.h> 
#include <q3popupmenu.h>
#include <qpushbutton.h>
#include <qstatusbar.h>
#include <qstring.h>
#include <qtabwidget.h>
#include <qtooltip.h>
#include <qwidget.h>
#include <q3widgetstack.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QCustomEvent>
#include <QEvent>
#include <Q3Action>

#ifndef WIN32
#include <unistd.h>
#endif

SteererMainWindow::SteererMainWindow(bool autoConnect, const char *aSGS)
  : Q3MainWindow( 0, "steerermainwindow"), mCentralWgt(kNULL),
    mTopLayout(kNULL), mStack(kNULL), mAppTabs(kNULL),
    mStackLogoLabel(kNULL), mStackLogoPixMap(kNULL),
    mCommsThread(kNULL), 
    mSetCheckIntervalAction(kNULL), mToggleAutoPollAction(kNULL),
    mAttachAction(kNULL), 
    mGridAttachAction(kNULL), 
    mQuitAction(kNULL) 
    
{
  REG_DBGCON("SteererMainWindow");
  setCaption( "ReG Steerer" );

  //make a central widget to contain the other widgets
  mCentralWgt = new QWidget( this );
  QFont f = mCentralWgt->font();
  f.setPointSize(9);
  mCentralWgt->setFont(f);
  setCentralWidget(mCentralWgt);
   
  // set up actions for configure check interval, attach and quit
  mSetCheckIntervalAction =  new Q3Action("Set polling interval",
					  "Set &polling interval",
					  Qt::CTRL+Qt::Key_P, this,
					  "setcheckaction");
  mSetCheckIntervalAction->setToolTip(QString("Set polling interval"));
  connect(mSetCheckIntervalAction, SIGNAL(activated()), this, 
	  SLOT(configureSteererSlot()));

  mToggleAutoPollAction = new Q3Action("Turn on auto polling interval",
				       "Turn on auto polling interval",
				       Qt::ALT+Qt::Key_P, this,
				       "toggleautopollaction");
  mToggleAutoPollAction->setToolTip(QString("Toggle use of auto polling interval"));
  connect(mToggleAutoPollAction, SIGNAL(activated()), this, 
	  SLOT(toggleAutoPollSlot()));

  mAttachAction = new Q3Action("Attach to local application", "Local &attach",
			       Qt::CTRL+Qt::Key_A, this, "attachaction");
  mAttachAction->setToolTip(QString("Attach to local app"));
  connect( mAttachAction, SIGNAL(activated()), this, SLOT(attachAppSlot()) );


  mGridAttachAction = new Q3Action("Attach to app on Grid", "&Grid attach",
				   Qt::CTRL+Qt::Key_G, this,
				   "gridattachaction");
  mGridAttachAction->setToolTip(QString("Attach to Grid app"));
  connect( mGridAttachAction, SIGNAL(activated()), this, 
	   SLOT(attachGridAppSlot()) );

  mSetTabTitleAction = new Q3Action("Set title of current tab",
				    "&Edit tab title",
				    Qt::CTRL+Qt::Key_E, this,
				    "settabtitleaction");
  mSetTabTitleAction->setToolTip(QString("Set title of current tab"));
  connect( mSetTabTitleAction, SIGNAL(activated()), this,
	   SLOT(editTabTitleSlot()) );

  mQuitAction =  new Q3Action("Quit (& detach)", "&Quit",
			      Qt::CTRL+Qt::Key_Q, this, "quitaction");
  mQuitAction->setToolTip(QString("Quit (& detach)"));
  connect( mQuitAction, SIGNAL(activated()), this, SLOT(quitSlot()) );

  // put actions in menu
  Q3PopupMenu *lConfigMenu = new Q3PopupMenu( this );
  menuBar()->insertItem( "&Steerer", lConfigMenu );
  mAttachAction->addTo(lConfigMenu);
  mGridAttachAction->addTo(lConfigMenu);
  mSetCheckIntervalAction->addTo(lConfigMenu);
  mToggleAutoPollAction->addTo(lConfigMenu);
  mSetTabTitleAction->addTo(lConfigMenu);
  mQuitAction->addTo(lConfigMenu);

  mSetCheckIntervalAction->setEnabled(FALSE);
  mAttachAction->setEnabled(TRUE);
  mGridAttachAction->setEnabled(TRUE);
  mSetTabTitleAction->setEnabled(TRUE);
  mQuitAction->setEnabled(TRUE);

  // Create layouts to position the widgets
  mTopLayout = new Q3HBoxLayout( mCentralWgt, 2);
  
  // set up the ReG logo
  mStackLogoLabel = new QLabel(mCentralWgt);
  mStackLogoPixMap = new QPixmap();
  if (mStackLogoPixMap->load("logo-sm.bmp")){
    mStackLogoLabel->setPixmap(*mStackLogoPixMap);
    mCentralWgt->setIcon(*mStackLogoPixMap);
  }
  else
  {
    mStackLogoLabel->setText("RealityGrid");
    mStackLogoLabel->setFont(QFont("Times", 9, QFont::DemiBold));
  }
   
  // SMR XXX - future add more widgets to stack for log viewing, 
  // for now only tabwidget
  mStack = new Q3WidgetStack(mCentralWgt);

  // tab widget - each tab will be form for one steered application
  mAppTabs = new QTabWidget();
 
  mStack->addWidget(mAppTabs);
  mTopLayout->addWidget(mStack);
  mStack->addWidget(mStackLogoLabel);
  mStack->raiseWidget(mStackLogoLabel);

  // Second menu to configure view
  Q3PopupMenu *lViewMenu = new Q3PopupMenu(this);
  menuBar()->insertItem("&View", lViewMenu);

  mHideChkPtTableAction = new Q3Action("Hide CheckPoint table", 
				       "Hide &CheckPoint table",
				       Qt::CTRL+Qt::Key_C, this,
				       "hidechkaction");
  connect( mHideChkPtTableAction, SIGNAL(activated()), this, 
	   SLOT(hideChkPtTableSlot()) );
  mHideChkPtTableAction->addTo(lViewMenu);

  mHideIOTableAction = new Q3Action("Hide IO table", 
				    "Hide &IO table",
				    Qt::CTRL+Qt::Key_I, this, "hideIOaction");
  connect( mHideIOTableAction, SIGNAL(activated()), this, 
	   SLOT(hideIOTableSlot()) );
  mHideIOTableAction->addTo(lViewMenu);

  mHideSteerTableAction = new Q3Action("Hide Steered Params table",
				       "Hide &Steered Params table",
				       Qt::CTRL+Qt::Key_S, this, 
				       "hideSteeraction");
  connect(mHideSteerTableAction, SIGNAL(activated()), this,
	  SLOT(hideSteerTableSlot()));
  mHideSteerTableAction->addTo(lViewMenu);

  mHideMonTableAction = new Q3Action("Hide Monitored Params table",
				     "Hide &Monitored Params table",
				     Qt::CTRL+Qt::Key_M, this,
				     "hideMonaction");
  connect(mHideMonTableAction, SIGNAL(activated()), this,
	  SLOT(hideMonTableSlot()));
  mHideMonTableAction->addTo(lViewMenu);

  // Catch tab changes so we can keep the status bar relevant
  connect(mAppTabs, SIGNAL(currentChanged(QWidget *)), this, 
	  SLOT(tabChangedSlot(QWidget *)));

  // Initial size of main GUI form when no applications being steered
  resizeForNoAttached();

  statusBar()->message("www.realitygrid.org");

  // Read configuration file (if any)
  mSteererConfig = new SteererConfig();
  mSteererConfig->readConfig(QString(getenv("HOME")) + 
			     "/.realitygrid/steerer.conf");

  mSteererConfig->mRegistrySecurity.use_ssl = 0;
  if(mSteererConfig->mTopLevelRegistry.startsWith("https://")){
    mSteererConfig->mRegistrySecurity.use_ssl = 1;
    mSteererConfig->readSecurityConfig(QString(getenv("HOME")) + 
				       "/.realitygrid/security.conf");
  }

  // create commsthread so can set checkinterval 
  // - thread is started on first attach
  mCommsThread = new CommsThread(this, &mReGMutex, 
				 (int)(1000.0*mSteererConfig->mPollingIntervalSecs));
  if (mCommsThread != kNULL){
    mSetCheckIntervalAction->setEnabled(TRUE);

    if(mCommsThread->getUseAutoPollFlag()){
      mToggleAutoPollAction->setMenuText(QString("Turn off auto &polling interval"));
    }
    else{
      mToggleAutoPollAction->setMenuText(QString("Turn on auto &polling interval"));
    }
  }

  // Check if we're auto connecting to a GSH
  if (autoConnect){
    simAttachApp((char*)aSGS);
    cmdLineSGS = aSGS;
  }

  mAppList.setAutoDelete(TRUE);
}


SteererMainWindow::~SteererMainWindow()
{
  REG_DBGDST("SteererMainWindow");
  cleanUp();
}


void 
SteererMainWindow::cleanUp()
{
  // stop and delete mCommsThread
  delete mCommsThread;
  mCommsThread = kNULL;

  // ARP - List of applications uses autodelete so automatically 
  // cleaned up when list object deleted

  delete mStackLogoPixMap;
  mStackLogoPixMap = kNULL;

}


Application * 
SteererMainWindow::getApplication(int aSimHandle)
{ 
  unsigned int i;

  for(i=0; i<mAppList.count(); i++){
    if(aSimHandle == (mAppList.at(i))->getHandle()){
      return mAppList.at(i);
      break;
    }
  }
  return NULL;
}


void
SteererMainWindow::customEvent(QCustomEvent *aEvent)
{
  // this function will be executed when main GUI thread gets round to processing 
  // the event posted by our CommsThread.

  // only expect events with type (User+kSIGNAL_EVENT)
  if (aEvent->type() == QEvent::User+kSIGNAL_EVENT)
  {
      REG_DBGMSG("Steerer cleaning up..."); //SMR XXX make sure is correct event... to do
      // detach from all apps and exit
      cleanUp();
      qApp->exit(0);
  }
  else
  {
    REG_DBGMSG("SteererMainWindow::customEvent - unexpected event type");
  }

}


bool
SteererMainWindow::isThreadRunning() const
{
  if (mCommsThread != kNULL)
  {
    if (mCommsThread->running())
      return true;
  }
  return false;
  
}


void 
SteererMainWindow::resizeForNoAttached()
{
  mStack->raiseWidget(mStackLogoLabel);
  
  resize(140,180);
  this->setMinimumSize(140, 180);
  qApp->processEvents();
  this->adjustSize();

}


void 
SteererMainWindow::attachAppSlot()
{
  // Attach to app using local file system - allow user to specify a directory
  // other than that contained in the REG_STEER_DIRECTORY env. variable
  statusBar()->clear();

  QString newDir = Q3FileDialog::getExistingDirectory(getenv("REG_STEER_DIRECTORY"),
						     this,
						     "get existing directory",
						     "Choose a directory for steering connection",
						     TRUE,
						     FALSE); // Don't resolve sym links
  // User hit cancel
  if(newDir.isNull())return;

  if ( !newDir.isEmpty() ) {
    // User entered something and pressed OK
    // Attempt to attach simulation
    simAttachApp((char*)newDir.latin1(), true);
  }  
  else {
    simAttachApp((char*) "", true);
  }
}

//------------------------------------------------------------------

void SteererMainWindow::attachGridAppSlot()
{
  AttachForm *lAttachForm = new AttachForm(this);
     
  if (lAttachForm->getLibReturnStatus() == REG_SUCCESS)
  {  
    if (lAttachForm->getNumSims() > 0)
    {
      if ( lAttachForm->exec() == QDialog::Accepted ) 
      {
        REG_DBGMSG1("attach accepted, sim = ",  lAttachForm->getSimGSMSelected());
        simAttachApp(lAttachForm->getSimGSMSelected());
      }
    } 
    else
    {
      QMessageBox::information(0, "Grid Attach", 
			       "No steerable applications found",
			       QMessageBox::Ok,
			       QMessageBox::NoButton, 
			       QMessageBox::NoButton);
    }
  }
  else
  {
    // Check to see if we were passed an SGS on the command line
    if (cmdLineSGS.length() > 0){
      simAttachApp((char*)cmdLineSGS.latin1());
    }
    else
    QMessageBox::information(0, "Grid Proxy Unavailable", 
			     "Local attach available only",
			     QMessageBox::Ok,
			     QMessageBox::NoButton, 
			     QMessageBox::NoButton);
  }

  delete lAttachForm;
}

//------------------------------------------------------------------

void SteererMainWindow::simAttachApp(char * aSimID, bool aIsLocal)
{
  /* Attempt to attach to a simulation */ 
  int lReGStatus = REG_FAILURE;
  int lSimHandle = -1;
  bool ok;
  struct reg_security_info sec;
  try
  {
    QString idStr(aSimID);
    if(idStr.startsWith("http") && idStr.contains("/WSRF/")){

      QString text = QInputDialog::getText("RealityGrid Steerer", 
					   "Enter passphrase for this application:", 
					   QLineEdit::Password,
					   QString::null, &ok, this );
      if ( !ok ) return; // Cancel if user didn't press OK
#ifdef REG_WSRF
      Wipe_security_info(&sec);
      strncpy(sec.passphrase, text.ascii(), REG_MAX_STRING_LENGTH);
      snprintf(sec.userDN, REG_MAX_STRING_LENGTH, "%s", getenv("USER"));
      strncpy(sec.caCertsPath, 
	      mSteererConfig->mRegistrySecurity.caCertsPath, 
	      REG_MAX_STRING_LENGTH);
      // WSRF support only for version >= 2.0
      mReGMutex.lock();
      lReGStatus = Sim_attach_secure(aSimID, &sec,
				     &lSimHandle); // ReG library
      mReGMutex.unlock();
#endif
    }
    else{
      mReGMutex.lock();
      lReGStatus = Sim_attach(aSimID, &lSimHandle);  //ReG library
      mReGMutex.unlock();
    }

    if (lReGStatus == REG_SUCCESS)
    {
      REG_DBGMSG1("Attached: mSimHandle = ",lSimHandle);
      
      mAppList.append(new Application(this, aSimID, lSimHandle, aIsLocal, 
				      &mReGMutex));

      // get supported command list from library and enable buttons 
      // appropriately
      mAppList.current()->enableCmdButtons();

      mAppTabs->addTab(mAppList.current(), QString(aSimID));      

      // Need showpage otherwise only shown on first attach - why? SMR XXX
      mAppTabs->showPage(mAppList.current()); 

      mStack->raiseWidget(mAppTabs);
      
      // resize - only do for first app attached
      if(mAppList.count() == 1)resize(525, 700);

      REG_DBGMSG("posted now start commsthread");
      // set off comms thread if it's not already running
      // process messages form all steered applications	
      if (!isThreadRunning())
      {
	if (mCommsThread == kNULL)
	  mCommsThread = new CommsThread(this, &mReGMutex);

	if (mCommsThread == kNULL)
	  THROWEXCEPTION("Thread not instantiated");

	mCommsThread->start();  // calls the run method
	
	if (!mCommsThread->running())
	{
	  THROWEXCEPTION("Thread creation failed");
	}
      }            
      
    }
    else
    {
      statusBar()->message( "Failed to attach" );
      REG_DBGMSG("Sim_attach failed");
    }
    
  } //try

  catch (SteererException StEx)
  {
    mSetCheckIntervalAction->setEnabled(FALSE);
    delete mCommsThread;
    mCommsThread = kNULL;
    mAppList.remove();

    QMessageBox mb("RealityGrid Steerer",
		   "Internal error - attach failed",
		    QMessageBox::Warning,
		    QMessageBox::Ok,
		    QMessageBox::NoButton,
		    QMessageBox::NoButton,
		    this, "Modeless warning", false);
    mb.setModal(false);
    mb.exec();
    
    StEx.print();
  }
  
}


void 
SteererMainWindow::closeApplicationSlot(int aSimHandle)
{
  unsigned int i;
  // close the window for the application
  // this can only be done when detached from application

  // If this was last application being steered, stop the CommsThread
  if((mAppList.count() == 1) && isThreadRunning() ){

    REG_DBGMSG("closeApplicationSlot: stopping comms thread");
    mCommsThread->stop();
  }

  REG_DBGMSG("closeApplicationSlot: deleting Application...");

  for(i=0; i<mAppList.count(); i++){
    if(aSimHandle == mAppList.at(i)->getHandle()){

      mAppTabs->removePage(mAppList.at(i));
      // Autodelete takes care of deleting this object - we just
      // have to remove it from the list
      mAppList.remove(i);
      break;
    }
  }
 
  // If this was last application being steered, resize the window...
  if(mAppList.count() == 0){

    REG_DBGMSG("closeApplicationSlot: re-sizing window...");
    resizeForNoAttached();
    statusBar()->message( "www.realitygrid.org");
  }
}

void
SteererMainWindow::editTabTitleSlot()
{
  bool ok;

  QString newLabel = QInputDialog::getText("Edit tab title",
					   "Enter new tab title:",
					   QLineEdit::Normal,
					   mAppTabs->tabLabel(mAppTabs->currentPage()),
					   &ok, this );
  if ( ok && !newLabel.isEmpty() ) {
    // user entered something and pressed OK
    mAppTabs->setTabLabel(mAppTabs->currentPage(), newLabel);
    // Use this label as the name of the Application widget too
    // - we pick this up in the ParameterTable class to pass to
    // HistoryPlot.
    mAppTabs->currentPage()->setName(newLabel);
  } 
}

void
SteererMainWindow::quitSlot()
{
  REG_DBGMSG("In quit slot");

   // detach from all apps and exit
   cleanUp();
   qApp->exit(0);

}

/*
void 
SteererMainWindow::readMsgSlot()
{
  // SMR XXX tmp code - replaced by thread
  // code to read steering files when user requests via button

  int	lSimHandle = -1;
  int   lMsgType = 0;

  REG_DBGMSG("In SteererMainWindow run");
    
  if (Get_next_message(&lSimHandle, &lMsgType) != REG_SUCCESS)	//ReG library
    REG_DBGEXCP("Get_next_message");

    
  if (lMsgType != MSG_NOTSET)
  {
    //SMR XXX  find Application in list that matches lSimHandle - future
    mApplication->processNextMessage(lMsgType);
  }
  
}
*/

void
SteererMainWindow::configureSteererSlot()
{

  ConfigForm *lConfigForm = new ConfigForm(mCommsThread->getCheckInterval(), 
					   this);

  if ( lConfigForm->exec() == QDialog::Accepted ) 
  {
    REG_DBGMSG1("config applied, interval= ", lConfigForm->getIntervalValue());
     mCommsThread->setCheckInterval(lConfigForm->getIntervalValue());
   
  }
  else {
    REG_DBGMSG("Config cancelled");
  }

  delete lConfigForm;
}

void SteererMainWindow::toggleAutoPollSlot()
{
  int lFlag;
  lFlag = mCommsThread->getUseAutoPollFlag();

  if(lFlag){

    mCommsThread->setUseAutoPollFlag(0);
    mToggleAutoPollAction->setMenuText(QString("Turn on auto &polling interval"));
  }
  else{

    mCommsThread->setUseAutoPollFlag(1);
    mToggleAutoPollAction->setMenuText(QString("Turn off auto &polling interval"));
  }
  return;
}

void SteererMainWindow::statusBarMessageSlot(Application *aApp, 
					     QString &message){

  if(mAppTabs->currentPage() == aApp){
    statusBar()->message(message);
  }

  aApp->setCurrentStatus(message);
}

/* Change statusBar message to the current status of the selected
 * application
 */
void 
SteererMainWindow::tabChangedSlot(QWidget *aWidget)
{
  REG_DBGMSG("Tab changed");
  Application *aApp;
  aApp = (Application *)aWidget;

  // Update the status bar so it is relevant to this tab
  statusBar()->message( aApp->getCurrentStatus() );

  // Ensure the View menu reflects what is being displayed on this
  // tab
  if(aApp->chkTableVisible()){
    mHideChkPtTableAction->setMenuText("Hide &CheckPoint table");

  }
  else{
    mHideChkPtTableAction->setMenuText("Show &CheckPoint table");
  }

  if(aApp->ioTableVisible()){
    mHideIOTableAction->setMenuText("Hide &IO table");
  }
  else{
    mHideIOTableAction->setMenuText("Show &IO table");
  }

  if(aApp->steerTableVisible()){
    mHideSteerTableAction->setMenuText("Hide &Steered Params table");
  }
  else{
    mHideSteerTableAction->setMenuText("Show &Steered Params table");
  }

  if(aApp->monTableVisible()){
    mHideMonTableAction->setMenuText("Hide &Monitored Params table");
  }
  else{
    mHideMonTableAction->setMenuText("Show &Monitored Params table");
  }
}

void SteererMainWindow::hideChkPtTableSlot()
{
  Application *aApp;

  if( (aApp = (Application *)(mAppTabs->currentPage())) ){

    if(aApp->chkTableVisible()){
      aApp->hideChkPtTable(true);
      mHideChkPtTableAction->setMenuText("Show &CheckPoint table");
      aApp->setChkTableVisible(false);
    }
    else{
      aApp->hideChkPtTable(false);
      mHideChkPtTableAction->setMenuText("Hide &CheckPoint table");
      aApp->setChkTableVisible(true);
    }
  }
}

void SteererMainWindow::hideIOTableSlot()
{
  Application *aApp;

  if( (aApp = (Application *)(mAppTabs->currentPage())) ){

    if(aApp->ioTableVisible()){
      aApp->hideIOTable(true);
      mHideIOTableAction->setMenuText("Show &IO table");
      aApp->setIOTableVisible(false);
    }
    else{
      aApp->hideIOTable(false);
      mHideIOTableAction->setMenuText("Hide &IO table");
      aApp->setIOTableVisible(true);
    }
  }
}

void SteererMainWindow::hideSteerTableSlot()
{
  Application *aApp;

  if( (aApp = (Application *)(mAppTabs->currentPage())) ){

    if(aApp->steerTableVisible()){
      aApp->hideSteerTable(true);
      mHideSteerTableAction->setMenuText("Show &Steered Params table");
      aApp->setSteerTableVisible(false);
    }
    else{
      aApp->hideSteerTable(false);
      mHideSteerTableAction->setMenuText("Hide &Steered Params table");
      aApp->setSteerTableVisible(true);
    }
  }
}

void SteererMainWindow::hideMonTableSlot()
{
  Application *aApp;

  if( (aApp = (Application *)(mAppTabs->currentPage())) ){
    
    if(aApp->monTableVisible()){
      aApp->hideMonTable(true);
      mHideMonTableAction->setMenuText("Show &Monitored Params table");
      aApp->setMonTableVisible(false);
    }
    else{
      aApp->hideMonTable(false);
      mHideMonTableAction->setMenuText("Hide &Monitored Params table");
      aApp->setMonTableVisible(true);
    }
  }
}

bool SteererMainWindow::autoPollingOn()
{
  return mSteererConfig->mAutoPollingOn;
}

float SteererMainWindow::getPollingIntervalSecs()
{
  return mSteererConfig->mPollingIntervalSecs;
}

SteererConfig *SteererMainWindow::getConfig()
{
  return mSteererConfig;
}
