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

#include <qapplication.h>
#include <qcombobox.h>
#include <qinputdialog.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qstatusbar.h>
#include <qstring.h>
#include <qtabwidget.h>
#include <qtooltip.h>
#include <qwidget.h>
#include <qwidgetstack.h>


SteererMainWindow::SteererMainWindow()
  : QMainWindow( 0, "steerermainwindow"), mCentralWgt(kNULL), mTopLayout(kNULL),  
  mLeftLayout(kNULL), mRightLayout(kNULL), mAttachButton(kNULL), mQuitButton(kNULL),
    mStack(kNULL), mAppTabs(kNULL), mCommsThread(kNULL), mApplication(kNULL)
{
  DBGCON("SteererMainWindow");
  setCaption( "Steerer - test GUI" );

  //make a central widget to contain the other widgets
  mCentralWgt = new QWidget( this );
  setCentralWidget(mCentralWgt);
  
  // Create layouts to position the widgets
  mTopLayout = new QHBoxLayout( mCentralWgt, 10 );
  mLeftLayout = new QVBoxLayout(-1, "hb1" );
  mRightLayout = new QVBoxLayout(-1, "hb2");
  
  mTopLayout->addLayout(mLeftLayout);
  mTopLayout->addLayout(mRightLayout);
  mLeftLayout->setMargin(10);

  // set up all buttons
  mAttachButton = new QPushButton( "Attach", mCentralWgt, "attachbutton" );
  mAttachButton->setMinimumSize(mAttachButton->sizeHint());
  mAttachButton->setMaximumSize(mAttachButton->sizeHint());
  mAttachButton->setEnabled(TRUE);
  QToolTip::add( mAttachButton, "Attach to new application" );
  connect( mAttachButton, SIGNAL(clicked()),
	   this, SLOT(attachAppSlot()) );
  mLeftLayout->addWidget(mAttachButton); 


  // SMR XXX tmp button to read steerer files - replaced with thread that poles for files
///SMR	mReadMsgButton = new QPushButton( "&ReadMsg", mCentralWgt, "ReadMsgbutton" );
///SMR	mReadMsgButton->setMinimumSize(mReadMsgButton->sizeHint());
///SMR	mReadMsgButton->setMaximumSize(mReadMsgButton->sizeHint());
///SMR	mReadMsgButton->setEnabled(FALSE);
///SMR	connect( mReadMsgButton, SIGNAL(clicked()), this, SLOT(readMsgSlot()) );
///SMR  mLeftLayout->addWidget(mReadMsgButton); 

  QSpacerItem* spacer = new QSpacerItem( 0, 156, QSizePolicy::Minimum, QSizePolicy::Expanding );
  mLeftLayout->addItem( spacer);

  mQuitButton = new QPushButton("&Quit", mCentralWgt, "quitbutton"); 
  mQuitButton->setMinimumSize(mQuitButton->sizeHint());
  mQuitButton->setMaximumSize(mQuitButton->sizeHint());
  QToolTip::add( mQuitButton, "Quit steerer" );
  connect( mQuitButton, SIGNAL(clicked()),
	   this, SLOT(quitSlot()) );
  mLeftLayout->addWidget(mQuitButton); 

  // SMR XXX - future add more widgets to stack for log viewing, for now only tabwidget
  mStack = new QWidgetStack(mCentralWgt);

  // tab widget - each tab will be form for one steered application
  mAppTabs = new QTabWidget();
  mStack->addWidget(mAppTabs, 1);

  mRightLayout->addWidget(mStack);
  
  // Initial size of main GUI form when no applications being steered
  resizeForNoAttached();

  statusBar()->message( "Steerer Initialized", 2000 );

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
  if (mCommsThread != 0)
  {
    if (mCommsThread->running())
      return true;
  }
  return false;
  
}


void 
SteererMainWindow::resizeForNoAttached()
{
  resize(150, 120);
}


void 
SteererMainWindow::attachAppSlot()
{

  // SMR XXX - just attach to application using env variables
  // SMR XXX this all to be done properly using grid services etc....

  try
  {

    QString lText;
    bool lOk = FALSE;
    statusBar()->clear();
    lText = QInputDialog::getText(
				  tr( "Application to steer" ),
				  tr( "Please enter application name" ),
				  QLineEdit::Normal, tr("LB3D"), &lOk, this );
    
    if ( lOk && !lText.isEmpty() )
    {
      /* Attempt to attach to ONE simulation */
      
      int lSimHandle = -1;
      
      if (Sim_attach(1, &lSimHandle) == REG_SUCCESS)		//ReG library
      {
	DBGMSG1("Wooohoooo - Attached: mSimHandle = ",lSimHandle);

	mApplication = new Application(this, lText, lSimHandle);
	// get supported command list from library and enable buttons appropriately
	mApplication->enableCmdButtons();

	mAppTabs->addTab(mApplication, lText);
	
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
	

	// set off comms thread iff it's not already running
	// process messages form all steered applications	
	if (!isThreadRunning())
	{
	  if (mCommsThread == kNULL)
	    mCommsThread = new CommsThread(this, 3);
	  
	  mCommsThread->start();  // calls the run method
	  
	  if (!mCommsThread->running())
	  {
	    THROWEXCEPTION("Thread creation failed");
	    ///SMR mReadMsgButton->setEnabled(TRUE);
	  }
	}


	// resize - only do for first app attached? SMR XXX
	resize(950, 800);   

	statusBar()->clear();
	
	
      }
      else
      {
	statusBar()->message( "Failed to attach to application" );
	DBGMSG("Sim_attach failed");
      }
      
    }  
    else
    {
      statusBar()->clear();
      DBGMSG("Application to steer::Cancel hit"); // user entered nothing or pressed Cancel
    }

  } //try

  catch (SteererException StEx)
  {
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

