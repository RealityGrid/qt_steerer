/*----------------------------------------------------------------------------
    Application Class for QT steerer GUI.
    Implementation of form for steering one attached application.

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


#include "application.h"
#include "controlform.h"
#include "debug.h"
#include "commsthread.h"
#include "ReG_Steer_Steerside.h"
#include "exception.h"

#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qwidget.h>

Application::Application(QWidget *aParent, const char *aName, int aSimHandle)
  : QWidget(aParent, aName), mSimHandle(aSimHandle), mNumCommands(0), 
    mDetachSupported(false), mStopSupported(false), mPauseSupported(false), 
    mResumeSupported(false), mDetachedFlag(false), mCloseButton(kNULL),
    mDetachButton(kNULL), mStopButton(kNULL), mPauseButton(kNULL), mResumeButton(kNULL), 
    mTellAndDetachButton(kNULL), mControlForm(kNULL), mControlBox(kNULL)
{ 
  // construct form for steering one application
  DBGCON("Application");
  
  // create some layouts for positioning
  QHBoxLayout *lFormLayout = new QHBoxLayout(this, 6, 6);
  QVBoxLayout *lButtonLayout = new QVBoxLayout(-1, "hb1" );


  // create the form which contains all the (dynamic) steered data (parameters etc)
  mControlBox = new QGroupBox(1, Vertical, "Control Monitor", this, "editbox" );
  mControlForm = new ControlForm(mControlBox, aName, aSimHandle, this);
  lFormLayout->addWidget(mControlBox);


  // button to allow user to get rid of this application form when detached
  mCloseButton = new QPushButton( "Close", this, "close" );
  mCloseButton->setMinimumSize(mCloseButton->sizeHint());
  mCloseButton->setMaximumSize(mCloseButton->sizeHint());
  // this only becomes enabled when detach from application
  mCloseButton->setEnabled(FALSE);
  QToolTip::add( mCloseButton, "Close Control Monitor" );
  connect( mCloseButton, SIGNAL( clicked() ), this, SLOT(closeApplicationSlot()) );
  connect (this, SIGNAL(closeApplicationSignal(int)), aParent, SLOT(closeApplicationSlot(int)) );

  //create Tell&Detach button
  ///SMR	mTellAndDetachButton = new QPushButton( "Tell && Detach", this, "tellanddetach" );
  ///SMR	mTellAndDetachButton->setMinimumSize(mTellAndDetachButton->sizeHint());
  ///SMR	mTellAndDetachButton->setMaximumSize(mTellAndDetachButton->sizeHint());
  ///SMR	connect( mTellAndDetachButton, SIGNAL( clicked() ), mControlForm, SLOT( emitAllAndDetachCmdSlot() ));
  

  // create the main command buttons for steering the application
  mStopButton = new QPushButton( "Stop", this, "stop" );
  mStopButton->setMinimumSize(mStopButton->sizeHint());
  mStopButton->setMaximumSize(mStopButton->sizeHint());
  connect( mStopButton, SIGNAL( clicked() ), this, SLOT( emitStopCmdSlot() ));

  mDetachButton = new QPushButton( "Detach", this, "detach" );
  mDetachButton->setMinimumSize(mDetachButton->sizeHint());
  mDetachButton->setMaximumSize(mDetachButton->sizeHint());
  connect( mDetachButton, SIGNAL( clicked() ), this, SLOT( emitDetachCmdSlot() ));
  
  mPauseButton = new QPushButton( "Pause", this, "pause" );
  mPauseButton->setMinimumSize(mPauseButton->sizeHint());
  mPauseButton->setMaximumSize(mPauseButton->sizeHint());
  connect( mPauseButton, SIGNAL( clicked() ), this, SLOT( emitPauseCmdSlot() ));
  
  mResumeButton = new QPushButton( "Resume", this, "resume" );
  mResumeButton->setMinimumSize(mResumeButton->sizeHint());
  mResumeButton->setMaximumSize(mResumeButton->sizeHint());
  connect( mResumeButton, SIGNAL( clicked() ), this, SLOT( emitResumeCmdSlot() ));


  // add buttons to layout
  QSpacerItem* spacer = new QSpacerItem( 0, 156, QSizePolicy::Minimum, QSizePolicy::Expanding );
  lButtonLayout->addWidget(mCloseButton);
  lButtonLayout->addItem(spacer);

  ///  lButtonLayout->addWidget(mTellAndDetachButton);
  lButtonLayout->addWidget(mDetachButton);
  lButtonLayout->addWidget(mStopButton);
  lButtonLayout->addWidget(mPauseButton);
  lButtonLayout->addWidget(mResumeButton);
  lFormLayout->addLayout(lButtonLayout);



} 

Application::~Application()
{
  DBGDST("Application");
  // send detach to application if still attached
  if (!mDetachedFlag)
    detachFromApplication();

  delete mControlForm;  //check this SMR XXX
  mControlForm = kNULL;
}


void Application::detachFromApplication()
{
  // detach via sim_detach so library clean up happens
  int lSimHandle = mSimHandle;

  DBGMSG("Do Sim_detach");

  // note Sim_Detach always returns REG_SUCCESS surrrently!
  if (Sim_detach(&lSimHandle) != REG_SUCCESS)		// ReG library
    DBGEXCP("Sim_detach");

  // flag as detached so destructor knows not to detach
  mDetachedFlag = true;

}

void
Application::disableForDetach()
{
  mControlForm->disableAll();
  disableCmdButtons();
  
  // enable Close button
  mCloseButton->setEnabled(TRUE);
  
}  


void
Application::disableCmdButtons()
{
  mDetachButton->setEnabled(FALSE);
  mStopButton->setEnabled(FALSE);
  mPauseButton->setEnabled(FALSE);
  mResumeButton->setEnabled(FALSE);  
}

void 
Application::enableCmdButtons() 
{
  // Enable/disable single command buttons for application

  // get supported commands via ReG library, populate id array and enable buttons
  // note that the supported cmds remain static for duration of steering application

  // disable all then appropriately enable those supported, set flags as to what is supported
  disableCmdButtons();

  int	*lCmdIds = kNULL;

  try
  {
        
    if (Get_supp_cmd_number(mSimHandle, &mNumCommands) == REG_SUCCESS)		//ReG library
    {  
      if (mNumCommands > 0)
      {
	lCmdIds = new int[mNumCommands];
	
	if (Get_supp_cmds(mSimHandle, mNumCommands, lCmdIds) != REG_SUCCESS)  	//ReG library 
	  THROWEXCEPTION("Get_supp_cmds");
      }
    }
    else 
      THROWEXCEPTION("Get_supp_cmd_number");
    
    for (int i=0; i<mNumCommands; i++)
    {
      switch (lCmdIds[i])
      {    
        case REG_STR_STOP:
	  mStopButton->setEnabled(TRUE);
	  mStopSupported = true;
	  break;
	  
        case REG_STR_PAUSE:
	  mPauseButton->setEnabled(TRUE);
	  mPauseSupported = true;
	  break;
	  
        case REG_STR_RESUME:
	  mResumeButton->setEnabled(FALSE);  // only enable when detach has been sent
	  mResumeSupported = true;
	  break;
	  
        case REG_STR_DETACH:
	  mDetachButton->setEnabled(TRUE);
	  mDetachSupported = true;
	  break;
	     
        default:
	  break;
      } // switch
    } //for

    // cleanup
    delete [] lCmdIds;
    lCmdIds = kNULL;

  } //try

  catch (SteererException StEx)
  {
    delete [] lCmdIds; 
    lCmdIds = kNULL;
    
    StEx.print();    
    throw StEx;
  }
  


}

void
Application::closeApplicationSlot()
{
  // currently close only enabled if user tells appl to stop/detach
  // or the appl itself detaches/stops

  // user has hit close button
  // so get rid of form for this application

  // tell steering lib we're detaching if not already 
  // may not be detached if still waiting for application to act on Stop/detach command
  if (!mDetachedFlag)
    detachFromApplication();

  emit closeApplicationSignal(mSimHandle);

}

void
Application::detachFromApplicationForErrorSlot()
{
  detachFromApplication();
  disableForDetach();
  mControlForm->setStatusLabel("Detached from application due to internal error");
    
}


void 
Application::emitDetachCmdSlot()
{
  emitSingleCmd(REG_STR_DETACH);

  // make gui read only except for close button
  disableForDetach();
  mControlForm->setStatusLabel("Attached - awaiting user requested detach");
}

void 
Application::emitStopCmdSlot()
{
  emitSingleCmd(REG_STR_STOP);

  // make gui read only except for close button
  disableForDetach();
  mControlForm->setStatusLabel("Attached - awaiting user requested stop");

}

void 
Application::emitResumeCmdSlot()
{

  // disable resume and enable pause  if supported (should be forced to support both in library)
  mResumeButton->setEnabled(FALSE);
  if (mPauseSupported)
    mPauseButton->setEnabled(TRUE);

  //enable IOtype commands
  mControlForm->enableIOCmdButtons();

  emitSingleCmd(REG_STR_RESUME);
  
  mControlForm->setStatusLabel("Attached to application");

}

void 
Application::emitPauseCmdSlot()
{
  // disable Pause and enable resume if supported (should be forced to support both in library)
  
  mPauseButton->setEnabled(FALSE);
  if (mResumeSupported)
    mResumeButton->setEnabled(TRUE);

  // disable IOtype commands
  mControlForm->disableIOCmdButtons();

  emitSingleCmd(REG_STR_PAUSE);

  mControlForm->setStatusLabel("Attached - user requested pause");
 

}


void 
Application::emitSingleCmd(int aCmdId)
{
  // send single command to application using ReG library
  DBGMSG1("Send Cmd id ",aCmdId);
 
  int lCommandArray[1];
  lCommandArray[0] = aCmdId;
 
  try 
  {
    if (Emit_control(mSimHandle,			//ReG library 
		     1,
		     lCommandArray) != REG_SUCCESS)
      THROWEXCEPTION("Emit control");
  }

  catch (SteererException StEx)
  {
    StEx.print();
    detachFromApplicationForErrorSlot();
    QMessageBox::warning(0, "Steerer Error", "Internal library error - detaching from application",
			 QMessageBox::Ok,
			 QMessageBox::NoButton, 
			 QMessageBox::NoButton);
    
  }

}


void
Application::customEvent(QCustomEvent *aEvent)
{
  // this function will be executed when main GUI thread gets round to processing 
  // the event posted by our CommsThread.

  // NOTE: Using postEvent means main GUI thread executes processNextMessage  
  // - alternative is to have CommsThread execute processNextMessage 
  // but then we need to worry about locking within Qt GUI related methods

  // only expect events with type (User+100)
  if (aEvent->type() == QEvent::User+100)
  {
    CommsThreadEvent *lEvent = (CommsThreadEvent *) aEvent;
    processNextMessage(lEvent->getMsgType());
  }

}


void
Application::processNextMessage(REG_MsgType aMsgType)
{
  // the commsthread has found a file for this application - this
  // function calls ReG library routines to process that file
  // as thread uses postEvent, this func is executed by GUI thread

  // need this as this done in GUI loop cos of thread->postEvent
  // is possible that this event posted before sim_detach happened for pervious event
  // if this is the case the file will have now been deleted
  if (mDetachedFlag)
    return;

  try
  {

  switch(aMsgType)
  {
    case IO_DEFS:
      
      DBGMSG("Got IOdefs message");
      if(Consume_IOType_defs(mSimHandle) != REG_SUCCESS)	//ReG library 
      {
	THROWEXCEPTION("Consume_IOType_defs failed");
      }
      else
      {
	// update IOType list and table
	mControlForm->updateIOTypes();

      }
      break;
      
    case PARAM_DEFS:
      
      DBGMSG("Got param defs message");
      if(Consume_param_defs(mSimHandle) != REG_SUCCESS)		//ReG library 
      {
	THROWEXCEPTION("Consume_param_defs failed");
      }
      else
      {
	// update parameter list and table
	mControlForm->updateParameters();
      }

      break;
      
    case STATUS:

      DBGMSG("Got status message");
      int   app_seqnum;
      int   num_cmds;
      int   commands[REG_MAX_NUM_STR_CMDS];
      bool detached;
      detached = false;

      if (Consume_status(mSimHandle,				//ReG library 
			 &app_seqnum,
			 &num_cmds,
			 commands) == REG_FAILURE)
      {
	THROWEXCEPTION("Consume_status failed");
      }
      else
      {
	// update parameter list and table
	mControlForm->updateParameters();
      
	// update IOType list and table (needed for frequency update)
	mControlForm->updateIOTypes();

	// now deal with commands - for now we only care about detach command
	for(int i=0; i<num_cmds && !detached; i++)
	{  
	  DBGMSG2("Recd Cmd", i, commands[i]);
	  int lSimHandle = mSimHandle;
		      
	  switch(commands[i])
	  {
	    case REG_STR_DETACH:
	      DBGMSG("Received detach command from application");
	      detached = true;
	      Delete_sim_table_entry(&lSimHandle);		//ReG library 

	      // make GUI form for this application read only
	      disableForDetach();
	      mControlForm->setStatusLabel("Application has detached");

	      mDetachedFlag = true;
	      break;
	    
	  case REG_STR_STOP:
	    DBGMSG("Received stop command from application");
	      detached = true;
	      Delete_sim_table_entry(&lSimHandle);		//ReG library 

	      // make GUI form for this application read only
	      disableForDetach();
	      mControlForm->setStatusLabel("Detached as application has stopped");

	      mDetachedFlag = true;
	      break;

	    default:
	      break;
	  }
	  
	}

	DBGMSG1("Application SeqNum = ", app_seqnum);
	
      }
      break;
      
    case MSG_NOTSET:
      DBGMSG("No msg to process");
      break;

    case CONTROL:
      DBGMSG("Got control message");
      break;
	
    default:
      DBGMSG("Unrecognised msg returned by Get_next_message");
      break;

  } //switch(aMsgType)

  } //try

  catch (SteererException StEx)
  {
    StEx.print();

    // detach from application (or at least attempt to)
    // make from read only and update status 

    detachFromApplication();
    disableForDetach();
    QMessageBox::warning(0, "Steerer Error", "Internal library error - detaching from application",
			 QMessageBox::Ok,
			 QMessageBox::NoButton, 
			 QMessageBox::NoButton);
    
    mControlForm->setStatusLabel("Detached from application due to internal  error");


  }


} // ::processNextMessage
