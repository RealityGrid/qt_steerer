/*----------------------------------------------------------------------------
    SteererMainWindow class for QT steerer GUI.

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
  : QMainWindow( 0, "steerermainwindow"), mCentralWgt(kNULL), mTopLayout(kNULL),  
    mLeftLayout(kNULL), mRightLayout(kNULL), mAttachButton(kNULL), mGridAttachButton(kNULL), 
    mQuitButton(kNULL), mReadMsgButton(kNULL), mStack(kNULL), mAppTabs(kNULL), mLogoLabel(kNULL),
    mLogoPixMap(kNULL), mStackLogoLabel(kNULL), mStackLogoPixMap(kNULL),
    mSetCheckIntervalAction(kNULL), mCommsThread(kNULL), mApplication(kNULL)
{
  DBGCON("SteererMainWindow");
  setCaption( "ReG Steerer" );

  //make a central widget to contain the other widgets
  mCentralWgt = new QWidget( this );
  setCentralWidget(mCentralWgt);

   
  // set up action for configure check interval
  mSetCheckIntervalAction =  new QAction("Set status poling interval","&Set Check Interval",
						  CTRL+Key_C, this, "setcheckaction");

  connect(mSetCheckIntervalAction, SIGNAL(activated()), this, SLOT(configureSteererSlot()));

  QPopupMenu *lConfigMenu = new QPopupMenu( this );
  menuBar()->insertItem( "Configure Steerer", lConfigMenu );
  mSetCheckIntervalAction->addTo(lConfigMenu);
  mSetCheckIntervalAction->setEnabled(FALSE);

 // Create layouts to position the widgets
  mTopLayout = new QHBoxLayout( mCentralWgt, 10 );
  mLeftLayout = new QVBoxLayout(-1, "hb1" );
  mRightLayout = new QVBoxLayout(-1, "hb2");
  
  mTopLayout->addLayout(mLeftLayout);
  mTopLayout->addLayout(mRightLayout);
  mLeftLayout->setMargin(10);


  // set up the ReG logo
  mLogoLabel = new QLabel(mCentralWgt);
  mLogoPixMap = new QPixmap();
  if (mLogoPixMap->load("logo-sm.bmp"))
    mLogoLabel->setPixmap(*mLogoPixMap);
  else 
    mLogoLabel->setText("");
  
  mStackLogoLabel = new QLabel(mCentralWgt);
  mStackLogoPixMap = new QPixmap();
  if (mStackLogoPixMap->load("logo-sm.bmp"))
    mStackLogoLabel->setPixmap(*mStackLogoPixMap);
  else 
    mStackLogoLabel->setText("");
  
  mLeftLayout->addWidget(mLogoLabel);


  // set up all buttons
  mAttachButton = new QPushButton( "Local &Attach", mCentralWgt, "attachbutton" );
  mAttachButton->setMinimumSize(mAttachButton->sizeHint());
  mAttachButton->setMaximumSize(mAttachButton->sizeHint());
  mAttachButton->setEnabled(TRUE);
  QToolTip::add( mAttachButton, "Attach to local application" );
  connect( mAttachButton, SIGNAL(clicked()),
	   this, SLOT(attachAppSlot()) );
  mLeftLayout->addWidget(mAttachButton); 

  mGridAttachButton= new QPushButton( "&Grid Attach", mCentralWgt, "gridattachbutton" );
  mGridAttachButton->setMinimumSize(mGridAttachButton->sizeHint());
  mGridAttachButton->setMaximumSize(mGridAttachButton->sizeHint());
  mGridAttachButton->setEnabled(TRUE);
  QToolTip::add( mGridAttachButton, "Attach to application on grid" );
  connect( mGridAttachButton, SIGNAL(clicked()),
	   this, SLOT(attachGridAppSlot()) );
  mLeftLayout->addWidget(mGridAttachButton); 


  // SMR XXX tmp button to read steerer files - replaced with thread that poles for files
///SMR	mReadMsgButton = new QPushButton( "&ReadMsg", mCentralWgt, "ReadMsgbutton" );
///SMR	mReadMsgButton->setMinimumSize(mReadMsgButton->sizeHint());
///SMR	mReadMsgButton->setMaximumSize(mReadMsgButton->sizeHint());
///SMR	mReadMsgButton->setEnabled(FALSE);
///SMR	connect( mReadMsgButton, SIGNAL(clicked()), this, SLOT(readMsgSlot()) );
///SMR  mLeftLayout->addWidget(mReadMsgButton); 

  QSpacerItem* spacer = new QSpacerItem( 0, 10, QSizePolicy::Minimum, QSizePolicy::Expanding );
  mLeftLayout->addItem( spacer);

  mQuitButton = new QPushButton("&Quit", mCentralWgt, "quitbutton"); 
  mQuitButton->setMinimumSize(mQuitButton->sizeHint());
  mQuitButton->setMaximumSize(mQuitButton->sizeHint());
  QToolTip::add( mQuitButton, "Quit steerer (and detach from any attached applications)" );
  connect( mQuitButton, SIGNAL(clicked()),
	   this, SLOT(quitSlot()) );
  mLeftLayout->addWidget(mQuitButton); 

  // SMR XXX - future add more widgets to stack for log viewing, for now only tabwidget
  mStack = new QWidgetStack(mCentralWgt);

  // tab widget - each tab will be form for one steered application
  mAppTabs = new QTabWidget();
 
  mStack->addWidget(mAppTabs);
  mRightLayout->addWidget(mStack);
  mStack->addWidget(mStackLogoLabel);
  mStack->raiseWidget(mStackLogoLabel);

  // Initial size of main GUI form when no applications being steered
  resizeForNoAttached();

  statusBar()->message( "Steerer Initialized", 2000 );

  // create commsthread so can set checkinterval 
  // - thread is started on first attach
  mCommsThread = new CommsThread(this);
  if (mCommsThread != kNULL)
    mSetCheckIntervalAction->setEnabled(TRUE);
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
  
}


Application * 
SteererMainWindow::getApplication() const
{ 
  return mApplication; 
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
  mLogoLabel->hide();
  resize(150,120);
  qApp->processEvents();
  this->adjustSize();

}


void 
SteererMainWindow::attachAppSlot()
{

  // SMR XXX - just attach to application using env variables

  statusBar()->clear();

  if (QMessageBox::information(0, "Local Attach", 
			       "Attach using ReG environment variables?",
			       QMessageBox::Ok,
			       QMessageBox::Cancel, 
			       QMessageBox::NoButton) == QMessageBox::Ok)
  
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

  int lSimHandle = -1;

  try
  {

    if (Sim_attach(aSimID, &lSimHandle) == REG_SUCCESS)		//ReG library
    {
      DBGMSG1("Wooohoooo - Attached: mSimHandle = ",lSimHandle);
      
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
      mAttachButton->setEnabled(FALSE);
      mGridAttachButton->setEnabled(FALSE);
      
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
	  ///SMR mReadMsgButton->setEnabled(TRUE);
	}
      }
      
      mLogoLabel->show();
      // resize - only do for first app attached? SMR XXX
      resize(985, 800);     
     
      statusBar()->clear();
      
      
    }
    else
    {
      statusBar()->message( "Failed to attach to application" );
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
  mAttachButton->setEnabled(TRUE);
  mGridAttachButton->setEnabled(TRUE);

 
  // SMR XXX  if last application being steered,resie the window
  resizeForNoAttached();
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

