/*----------------------------------------------------------------------------
    SteererMainWindow class for QT steerer GUI.

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
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpixmap.h> 
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qstatusbar.h>
#include <qstring.h>
#include <qtabwidget.h>
#include <qtooltip.h>
#include <qwidget.h>
#include <qwidgetstack.h>

SteererMainWindow::SteererMainWindow()
  : QMainWindow( 0, "steerermainwindow"), mCentralWgt(kNULL),
    mTopLayout(kNULL), mStack(kNULL), mAppTabs(kNULL),
    mStackLogoLabel(kNULL), mStackLogoPixMap(kNULL),
    mSetCheckIntervalAction(kNULL), mAttachAction(kNULL), mGridAttachAction(kNULL), 
    mQuitAction(kNULL), mCommsThread(kNULL), mApplication(kNULL)
{
  DBGCON("SteererMainWindow");
  setCaption( "ReG" );

  //make a central widget to contain the other widgets
  mCentralWgt = new QWidget( this );
  setCentralWidget(mCentralWgt);
   
  // set up actions for configure check interval, attach and quit
  mSetCheckIntervalAction =  new QAction("Set polling interval","Set Polling Interval",
						  CTRL+Key_P, this, "setcheckaction");
  //mSetCheckIntervalAction->setStatusTip("Set polling interval");
  mSetCheckIntervalAction->setToolTip(QString("Set polling interval"));
  connect(mSetCheckIntervalAction, SIGNAL(activated()), this, SLOT(configureSteererSlot()));


  mAttachAction = new QAction("Attach to local application", "Local &Attach",
			      CTRL+Key_A, this, "attachaction");
  //mAttachAction->setStatusTip("Attach to local app");
  mAttachAction->setToolTip(QString("Attach to local app"));
  connect( mAttachAction, SIGNAL(activated()), this, SLOT(attachAppSlot()) );


  mGridAttachAction = new QAction("Attach to app on Grid", "&Grid Attach",
			      CTRL+Key_G, this, "gridattachaction");
  //mGridAttachAction->setStatusTip("Attach to Grid app");
  mGridAttachAction->setToolTip(QString("Attach to Grid app"));
  connect( mGridAttachAction, SIGNAL(activated()), this, SLOT(attachGridAppSlot()) );


  mQuitAction =  new QAction("Quit (& detach)", "&Quit",
			      CTRL+Key_Q, this, "quitaction");
  //mQuitAction->setStatusTip("Quit (& detach)");
  mQuitAction->setToolTip(QString("Quit (& detach)"));
  connect( mQuitAction, SIGNAL(activated()), this, SLOT(quitSlot()) );

  // put actions in menu
  QPopupMenu *lConfigMenu = new QPopupMenu( this );
  menuBar()->insertItem( "Steerer", lConfigMenu );
  mAttachAction->addTo(lConfigMenu);
  mGridAttachAction->addTo(lConfigMenu);
  mSetCheckIntervalAction->addTo(lConfigMenu);
  mQuitAction->addTo(lConfigMenu);

  mSetCheckIntervalAction->setEnabled(FALSE);
  mAttachAction->setEnabled(TRUE);
  mGridAttachAction->setEnabled(TRUE);
  mQuitAction->setEnabled(TRUE);


  // Create layouts to position the widgets
  mTopLayout = new QHBoxLayout( mCentralWgt, 5);
  
  // set up the ReG logo
  mStackLogoLabel = new QLabel(mCentralWgt);
  mStackLogoPixMap = new QPixmap();
  if (mStackLogoPixMap->load("logo-sm.bmp"))
    mStackLogoLabel->setPixmap(*mStackLogoPixMap);
  else
  {
    mStackLogoLabel->setText("RealityGrid");
    mStackLogoLabel->setFont(QFont("Times", 9, QFont::DemiBold));
  }
   
  //mTopLayout->addItem(new QSpacerItem( 15, 0, QSizePolicy::Maximum, QSizePolicy::Expanding ));

  // SMR XXX - future add more widgets to stack for log viewing, for now only tabwidget
  mStack = new QWidgetStack(mCentralWgt);

  // tab widget - each tab will be form for one steered application
  mAppTabs = new QTabWidget();
 
  mStack->addWidget(mAppTabs);
  mTopLayout->addWidget(mStack);
  mStack->addWidget(mStackLogoLabel);
  mStack->raiseWidget(mStackLogoLabel);

  //mTopLayout->addItem(new QSpacerItem( 15, 0, QSizePolicy::Maximum, QSizePolicy::Expanding ));

  // Initial size of main GUI form when no applications being steered
  resizeForNoAttached();

  ///statusBar()->message( "Steerer Initialized", 2000 );
  statusBar()->message( "www.realitygrid.org");

  // create commsthread so can set checkinterval 
  // - thread is started on first attach
  mCommsThread = new CommsThread(this);
  if (mCommsThread != kNULL)
    mSetCheckIntervalAction->setEnabled(TRUE);


  //statusBar()->setSizeGripEnabled(false);
}


SteererMainWindow::~SteererMainWindow()
{
  DBGDST("SteererMainWindow");
  cleanUp();
}


void 
SteererMainWindow::cleanUp()
{
  // stop and delete mCommsThread
  delete mCommsThread;
  mCommsThread = kNULL;

  // detach and delete application // SMR XXX do for list
  delete mApplication;
  mApplication = kNULL;
  
  delete mStackLogoPixMap;
  mStackLogoPixMap = kNULL;

}


Application * 
SteererMainWindow::getApplication() const
{ 
  return mApplication; 
}


void
SteererMainWindow::customEvent(QCustomEvent *aEvent)
{
  // this function will be executed when main GUI thread gets round to processing 
  // the event posted by our CommsThread.

  // only expect events with type (User+kSIGNAL_EVENT)
  if (aEvent->type() == QEvent::User+kSIGNAL_EVENT)
  {
      DBGMSG("Steerer cleaning up..."); //SMR XXX make sure is correct event... to do
      // detach from all apps and exit
      cleanUp();
      qApp->exit(0);
  }
  else
  {
    DBGMSG("SteererMainWindow::customEvent - unexpected event type");
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

  // SMR XXX - just attach to application using env variables

  statusBar()->clear();

  // MR: requested that this MessageBox be turned off for now
  if (1)/*QMessageBox::information(0, "Local Attach", 
			       "Attach using ReG environment variables?",
			       QMessageBox::Ok,
			       QMessageBox::Cancel, 
			       QMessageBox::NoButton) == QMessageBox::Ok)
*/  
  {
    /* Attempt to attach simulation */
    simAttachApp("DEFAULT", true);
  }  
  else
  {
    statusBar()->clear();
    DBGMSG("Application to steer::Cancel hit"); // user entered nothing or pressed Cancel
  }

}


void 
SteererMainWindow::attachGridAppSlot()
{

  // SMR XXX future: need to keep record of attached SimGSH so cannot attach twice - 
  // also filter of grid  list shown 

  AttachForm *lAttachForm = new AttachForm(this);
     
  if (lAttachForm->getLibReturnStatus() == REG_SUCCESS)
  {  
    if (lAttachForm->getNumSims() > 0)
    {
      if ( lAttachForm->exec() == QDialog::Accepted ) 
      {
	DBGMSG1("attach accepted, sim = ",  lAttachForm->getSimGSMSelected());
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
    QMessageBox::information(0, "Grid Proxy Unavailable", 
			     "Local attach available only",
			     QMessageBox::Ok,
			     QMessageBox::NoButton, 
			     QMessageBox::NoButton);
  }

  delete lAttachForm;

}


void 
SteererMainWindow::simAttachApp(char * aSimID, bool aIsLocal)
{

  /* Attempt to attach ONE simulation */ 
  int lReGStatus = REG_FAILURE;
  int lSimHandle = -1;

  try
  {
    qApp->lock();
    lReGStatus = Sim_attach(aSimID, &lSimHandle);	//ReG library
    qApp->unlock();

    if (lReGStatus == REG_SUCCESS)
    {
      DBGMSG1("Attached: mSimHandle = ",lSimHandle);
      
      mApplication = new Application(this, aSimID, lSimHandle);

      // get supported command list from library and enable buttons appropriately
      mApplication->enableCmdButtons();

      if (aIsLocal)     
	mAppTabs->addTab(mApplication, QString("Local Application"));
      else
	mAppTabs->addTab(mApplication, QString(aSimID));
      

      //need showpage otherwise only shown on first attach - why? SMR XXX
      mAppTabs->showPage(mApplication); 
      mStack->raiseWidget(mAppTabs);
      
      // code for posting in new window - future SMR XXX
      ///SMR	   ApplicationWindow *lNewWindow = new ApplicationWindow(lText, lSimHandle); //SMR XXX member/deletion?
      ///SMR	   lNewWindow->setCaption("New Appl Window");
      ///SMR	   lNewWindow->show();
      ///SMR	   mApplication = lNewWindow->getApplication();
      
      // SMR XXX disable attach buttons as currently only attach to one application
      mAttachAction->setEnabled(FALSE);
      mGridAttachAction->setEnabled(FALSE);
      
      // resize - only do for first app attached? SMR XXX future concern
      resize(540, 700);

      // set minimum size so all gui visible, if remove this can 
      // shrink gui, but no scrollbars
      this->setMinimumSize(540, 700);
      //statusBar()->clear();

      DBGMSG("posted now start commsthread");
      // set off comms thread iff it's not already running
      // process messages form all steered applications	
      if (!isThreadRunning())
      {
	if (mCommsThread == kNULL)
	  mCommsThread = new CommsThread(this);

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
      DBGMSG("Sim_attach failed");
    }
    
  } //try

  catch (SteererException StEx)
  {
    mSetCheckIntervalAction->setEnabled(FALSE);
    delete mCommsThread;
    mCommsThread = kNULL;
    delete mApplication;
    mApplication = kNULL;
    
    QMessageBox::warning(0, "Steerer Error", "Internal error - attach failed",
			 QMessageBox::Ok,
			 QMessageBox::NoButton, 
			 QMessageBox::NoButton);
    
    StEx.print();
  }
  
}


void 
SteererMainWindow::closeApplicationSlot(int aSimHandle)
{
  // close the window for the application
  // this can only be done when detached form application

  DBGMSG1("Close button hit and signalled to main window...", aSimHandle);
  // SMR XXX - future use aSimHandle to find correct mApplicationPtr
  // SMR XXX -rewrite when >1 application being steered - future
  
  // SMR XXX if last application being steered, stop the CommsThread
  if (isThreadRunning())
    mCommsThread->stop();

  mAppTabs->removePage(mApplication);
 
  delete mApplication;
  mApplication = kNULL;

  // re-enable attach button SMR XXX
  mAttachAction->setEnabled(TRUE);
  mGridAttachAction->setEnabled(TRUE);

  // SMR XXX  if last application being steered,resize the window
  resizeForNoAttached();
  statusBar()->message( "www.realitygrid.org");
}


void
SteererMainWindow::quitSlot()
{
  DBGMSG("In quit slot");

   // detach from all apps and exit
   cleanUp();
   qApp->exit(0);

}

void 
SteererMainWindow::readMsgSlot()
{
  // SMR XXX tmp code - replaced by thread
  // code to read steering files when user requests via button

  int	lSimHandle = -1;
  REG_MsgType lMsgType = 0;

  DBGMSG("In SteererMainWindow run");
    
  if (Get_next_message(&lSimHandle, &lMsgType) != REG_SUCCESS)	//ReG library
    DBGEXCP("Get_next_message");

    
  if (lMsgType != MSG_NOTSET)
  {
    //SMR XXX  find Application in list that matches lSimHandle - future
    mApplication->processNextMessage(lMsgType);
  }
  
}

void
SteererMainWindow::configureSteererSlot()
{

  ConfigForm *lConfigForm = new ConfigForm(mCommsThread->getCheckInterval(), this);

  if ( lConfigForm->exec() == QDialog::Accepted ) 
  {
    DBGMSG1("config applied, interval= ", lConfigForm->getIntervalValue());
     mCommsThread->setCheckInterval(lConfigForm->getIntervalValue());
   
  }
  else
    DBGMSG("Config cancelled");
  
  delete lConfigForm;

}

void SteererMainWindow::statusBarMessageSlot(QString &message){
  statusBar()->message(message);
}

//SMRXXXfutue - new window posting for appl

///SMRApplicationWindow::ApplicationWindow(const char *aText, int aSimHandle)
///SMR	: QMainWindow( 0, 0, WDestructiveClose )
///SMR{
///SMR
///SMR	mApplication = new Application(this, aText, aSimHandle);
///SMR	setCentralWidget(mApplication);
///SMR	resize(900,800);
///SMR
///SMR}

