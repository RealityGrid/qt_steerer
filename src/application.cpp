/*----------------------------------------------------------------------------
    Application Class for QT steerer GUI.
    Implementation of form for steering one attached application.

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
#include "application.h"
#include "controlform.h"
#include "debug.h"
#include "commsthread.h"
#include "ReG_Steer_Steerside.h"
#include "exception.h"

#include <qapplication.h>
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
    mResumeSupported(false), mDetachedFlag(false),
    mControlForm(kNULL), mControlBox(kNULL)
{ 
  // construct form for steering one application
  DBGCON("Application");
  
  // create some layouts for positioning
  QHBoxLayout *lFormLayout = new QHBoxLayout(this, 6, 6);
  ///  QVBoxLayout *lButtonLayout = new QVBoxLayout(-1, "hb1" );

  // create the form which contains all the (dynamic) steered data (parameters etc)
  mControlBox = new QGroupBox(1, Vertical, "", this, "editbox" );
  mControlForm = new ControlForm(mControlBox, aName, aSimHandle, this);
  lFormLayout->addWidget(mControlBox);
  //this->addChild(mControlBox);

  // connect up signaL/slot for close
  connect (this, SIGNAL(closeApplicationSignal(int)), aParent, SLOT(closeApplicationSlot(int)) );
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
  int lReGStatus = REG_FAILURE;

  qApp->lock();
  lReGStatus = Sim_detach(&lSimHandle);		// ReG library
  qApp->unlock();

  // note Sim_Detach always returns REG_SUCCESS surrrently!
  if (lReGStatus != REG_SUCCESS)
    DBGEXCP("Sim_detach");

  // flag as detached so destructor knows not to detach
  mDetachedFlag = true;

}

void
Application::disableForDetach(const bool aUnRegister)
{
  mControlForm->disableAll(aUnRegister);  
}  


void 
Application::enableCmdButtons() 
{
  // Enable/disable single command buttons for application
  int lReGStatus = REG_FAILURE;

  // get supported commands via ReG library, populate id array and enable buttons
  // note that the supported cmds remain static for duration of steering application

  // disable all then appropriately enable those supported, set flags as to what is supported
  mControlForm->disableAppCmdButtons();

  int	*lCmdIds = kNULL;

  try
  {
    qApp->lock();
    lReGStatus = Get_supp_cmd_number(mSimHandle, &mNumCommands);	//ReG library 
    qApp->unlock();

    if (lReGStatus == REG_SUCCESS)
    {  
      if (mNumCommands > 0)
      {
	lCmdIds = new int[mNumCommands];

	qApp->lock();
	lReGStatus = Get_supp_cmds(mSimHandle, mNumCommands, lCmdIds);	//ReG library 
	qApp->unlock();

	if (lReGStatus != REG_SUCCESS)
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
	  mControlForm->setEnabledStop(TRUE);
	  mStopSupported = true;
	  break;
	  
        case REG_STR_PAUSE:
	  mControlForm->setEnabledPause(TRUE);
	  mPauseSupported = true;
	  break;
	  
        case REG_STR_RESUME:
	  mControlForm->setEnabledResume(FALSE);  // only enable when detach has been sent
	  mResumeSupported = true;
	  break;
	  
        case REG_STR_DETACH:
	  mControlForm->setEnabledDetach(TRUE);
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

  // Tell steering lib we're detaching if not already.
  // May not be detached if still waiting for application to act on 
  // Stop/detach command.
  if (!mDetachedFlag)
    detachFromApplication();

  emit closeApplicationSignal(mSimHandle);

}

void
Application::detachFromApplicationForErrorSlot()
{
  detachFromApplication();
  disableForDetach(true);
  mControlForm->setStatusLabel("Detached from application due to internal error");
    
}


void 
Application::emitDetachCmdSlot()
{
  emitSingleCmd(REG_STR_DETACH);

  // make gui read only except for close button
  disableForDetach(false);
  mControlForm->setStatusLabel("Attached - awaiting user requested detach");
}

void 
Application::emitStopCmdSlot()
{

///  if (QMessageBox::information(0, "Stop Application", 
///				  "Are you sure?",
///				  QMessageBox::Ok,
///				  QMessageBox::Cancel, 
///				  QMessageBox::NoButton) == QMessageBox::Ok)
///    
/// {
  emitSingleCmd(REG_STR_STOP);

  // make gui read only 
  disableForDetach(false);
  mControlForm->setStatusLabel("Attached - awaiting user requested stop");
  /// }
}

void 
Application::emitResumeCmdSlot()
{

  // disable resume and enable pause  if supported (should be forced 
  // to support both in library)
  mControlForm->setEnabledResume(FALSE);
  if (mPauseSupported)
    mControlForm->setEnabledPause(TRUE);

  // enable IOtype commands
  mControlForm->enableIOCmdButtons();

  // enable detach now that app. is running again
  mControlForm->setEnabledDetach(TRUE);

  emitSingleCmd(REG_STR_RESUME);
  
  mControlForm->setStatusLabel("Attached to application");

}

void 
Application::emitPauseCmdSlot()
{
  // disable Pause and enable resume if supported (should be forced to support both in library)
  
  mControlForm->setEnabledPause(FALSE);
  if (mResumeSupported)
    mControlForm->setEnabledResume(TRUE);

  // disable IOtype commands
  mControlForm->disableIOCmdButtons();

  // disable detach - don't think it makes sense to let user pause
  // application and then detach from it
  mControlForm->setEnabledDetach(FALSE);

  emitSingleCmd(REG_STR_PAUSE);

  mControlForm->setStatusLabel("Attached - user requested pause");
 

}


void 
Application::emitSingleCmd(int aCmdId)
{
  // send single command to application using ReG library
  DBGMSG1("Send Cmd id ",aCmdId);
  int lReGStatus = REG_FAILURE;

  //  int lCommandArray[1];
  //  lCommandArray[0] = aCmdId;
 
  try 
  {
    qApp->lock();
    //lReGStatus = Emit_control(mSimHandle,		//ReG library 
    //			      1,
    //			      lCommandArray,
    //			      NULL);
    switch(aCmdId){

    case REG_STR_STOP:
      lReGStatus = Emit_stop_cmd(mSimHandle);		//ReG library 
      break;

    case REG_STR_PAUSE:
      lReGStatus = Emit_pause_cmd(mSimHandle);		//ReG library 
      break;

    case REG_STR_RESUME:
      lReGStatus = Emit_resume_cmd(mSimHandle);		//ReG library 
      break;

    case REG_STR_DETACH:
      lReGStatus = Emit_detach_cmd(mSimHandle);		//ReG library 
      break;

    default:
      break;

    }

    qApp->unlock();

    if (lReGStatus != REG_SUCCESS)
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

  // only expect events with type (User+kMSG_EVENT)
  if (aEvent->type() == QEvent::User+kMSG_EVENT)
  {
    CommsThreadEvent *lEvent = (CommsThreadEvent *) aEvent;
    qApp->lock();
    processNextMessage(lEvent->getMsgType());
    qApp->unlock();
  }
  else
  {
    DBGMSG("Application::customEvent -  unexpected event type");
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
	mControlForm->updateIOTypes(false);

      }
      break;

    case CHK_DEFS:

      DBGMSG("Got Chkdefs message");

      if(Consume_ChkType_defs(mSimHandle) != REG_SUCCESS)	//ReG library 
      {
	THROWEXCEPTION("Consume_ChkType_defs failed");
      }
      else
      {
	// update IOType list and table
	mControlForm->updateIOTypes(true);

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
	mControlForm->updateIOTypes(false);	// sample types
	mControlForm->updateIOTypes(true);	// checkpoint types


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
	      disableForDetach(true);
	      mControlForm->setStatusLabel("Application has detached");

	      // enable Close button
	      mControlForm->setEnabledClose(TRUE);

	      mDetachedFlag = true;
	      break;
	    
	  case REG_STR_STOP:
	    DBGMSG("Received stop command from application");
	      detached = true;
	      Delete_sim_table_entry(&lSimHandle);		//ReG library 

	      // make GUI form for this application read only
	      disableForDetach(true);
	      mControlForm->setStatusLabel("Detached as application has stopped");

	      // enable Close button
	      mControlForm->setEnabledClose(TRUE);

	      mDetachedFlag = true;
	      break;

	    default:
	      break;
	  }
	  
	}

	DBGMSG1("Application SeqNum = ", app_seqnum);
	
      }
      break;
      

    case STEER_LOG: 
      DBGMSG("Got steer_log message");

      if(Consume_log(mSimHandle) != REG_SUCCESS)	//ReG library 
      {
	// THROWEXCEPTION("Consume_log failed"); - don't throw - just log that this has happened
	DBGLOG("Consume_log library call failed");
      }

      break;

    case MSG_NOTSET:
      DBGMSG("No msg to process");
      break;

    case CONTROL:
      DBGMSG("Got control message");
      break;
	    
    case SUPP_CMDS:
      DBGMSG("Got supp_cmds message");
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
    disableForDetach(true);
    QMessageBox::warning(0, "Steerer Error", "Internal library error - detaching from application",
			 QMessageBox::Ok,
			 QMessageBox::NoButton, 
			 QMessageBox::NoButton);
    
    mControlForm->setStatusLabel("Detached from application due to internal  error");


  }


} // ::processNextMessage
