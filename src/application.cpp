/*----------------------------------------------------------------------------
  Application Class for QT steerer GUI.
  Implementation of form for steering one attached application.

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

/** @file application.cpp
    @brief Holds implementation of Application class */

#include "types.h"
#include "application.h"
#include "controlform.h"
#include "debug.h"
#include "commsthread.h"
#include "ReG_Steer_Steerside.h"
#include "exception.h"
#include "steerermainwindow.h"

#include <qapplication.h>
#include <q3groupbox.h>
#include <qinputdialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qwidget.h>
#include <q3popupmenu.h>
#include <qdom.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QEvent>

Application::Application(QWidget *aParent, const char *aName, 
			 int aSimHandle, bool aIsLocal, QMutex *aMutex)
  : QWidget(aParent, aName), mSimHandle(aSimHandle), mMutexPtr(aMutex), 
    mNumCommands(0), mDetachSupported(false), mStopSupported(false), 
    mPauseSupported(false),  mResumeSupported(false), mDetachedFlag(false), 
    mStatusTxt(""), mControlForm(kNULL), mControlBox(kNULL)
{

  // MR keep an internal record of whether we're local or grid
  mIsLocal = aIsLocal;

  // construct form for steering one application
  REG_DBGCON("Application");
  
  // create some layouts for positioning
  Q3HBoxLayout *lFormLayout = new Q3HBoxLayout(this, 6, 6);
  ///  QVBoxLayout *lButtonLayout = new QVBoxLayout(-1, "hb1" );

  // create the form which contains all the (dynamic) steered data 
  // (parameters etc)
  mControlBox = new Q3GroupBox(1, Qt::Vertical, "", this, "editbox" );
  mControlForm = new ControlForm(mControlBox, aName, aSimHandle, this, 
				 mMutexPtr);
  lFormLayout->addWidget(mControlBox);
  //this->addChild(mControlBox);

  // connect up signaL/slot for close
  connect (this, SIGNAL(closeApplicationSignal(int)), aParent, 
	   SLOT(closeApplicationSlot(int)) );

  // MR
  // keep a reference to the SteererMainWindow object, so as we can change it's
  // status bar text as and when necessary
  mSteerer = (SteererMainWindow*)aParent;

  // MR
  // This message was originally automatically added to the
  // old style status text on app creation. Do it  here instead
  QString message = QString("Attached to application");
  mSteerer->statusBarMessageSlot(this, message);

  mChkTableVisible = true;
  mIOTableVisible = true;
  mSteerTableVisible = true;
  mMonTableVisible = true;
} 

Application::~Application()
{
  REG_DBGDST("Application");
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

  REG_DBGMSG("Do Sim_detach");
  int lReGStatus = REG_FAILURE;

  mMutexPtr->lock();
  lReGStatus = Sim_detach(&lSimHandle);		// ReG library
  mMutexPtr->unlock();

  // note Sim_Detach always returns REG_SUCCESS surrrently!
  if (lReGStatus != REG_SUCCESS) {
    REG_DBGEXCP("Sim_detach");
  }

  // flag as detached so destructor knows not to detach
  mDetachedFlag = true;
}

void
Application::disableForDetach(const bool aUnRegister)
{
  mControlForm->disableAll(aUnRegister);  
  // ARPDBG - no longer wait for confirmation and thus
  // let user close form when done.
  mControlForm->setEnabledClose(true);
}  

/** As for disableForDetach but called when an error condition
  * has occurred - leaves the 'close' button enabled
  */
void
Application::disableForDetachOnError()
{
  mControlForm->disableAll(true);
  mControlForm->setEnabledClose(true);
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
    mMutexPtr->lock();
    lReGStatus = Get_supp_cmd_number(mSimHandle, &mNumCommands);  //ReG library 
    mMutexPtr->unlock();

    if (lReGStatus == REG_SUCCESS)
    {  
      if (mNumCommands > 0)
      {
	lCmdIds = new int[mNumCommands];

	mMutexPtr->lock();
	lReGStatus = Get_supp_cmds(mSimHandle, mNumCommands, lCmdIds);	//ReG library
	mMutexPtr->unlock();

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
  disableForDetachOnError();
  QString message = QString("Detached from application due to internal error");
  mSteerer->statusBarMessageSlot(this, message);
}


void 
Application::emitDetachCmdSlot()
{
  if(emitSingleCmd(REG_STR_DETACH) != REG_SUCCESS){
    return;
  }

  // make gui read only except for close button
  disableForDetach(false);
  QString message = QString("Attached - awaiting user requested detach");
  mSteerer->statusBarMessageSlot(this, message);
}

void 
Application::emitStopCmdSlot()
{

//  if (QMessageBox::information(0, "Stop Application", 
//				  "Are you sure?",
//				  QMessageBox::Ok,
//				  QMessageBox::Cancel, 
//				  QMessageBox::NoButton) == QMessageBox::Ok)
//    
// {
  if(emitSingleCmd(REG_STR_STOP) != REG_SUCCESS){
    return;
  }

  // make gui read only 
  disableForDetach(false);
  QString message = QString("Attached - awaiting user requested stop");
  mSteerer->statusBarMessageSlot(this, message);
}

//------------------------------------------------------------------------
void 
Application::emitPauseCmdSlot()
{
  QString message;

  // Disable Pause and enable resume if supported (should be forced 
  // to support both in library)
  if(!getCurrentStatus().contains("paused", FALSE)){
    
    if(emitSingleCmd(REG_STR_PAUSE) != REG_SUCCESS){
      return;
    }

    mControlForm->setPauseButtonLabel(QString("Resume"));

    // disable IOtype commands
    mControlForm->disableIOCmdButtons();

    // disable detach - don't think it makes sense to let user pause
    // application and then detach from it
    mControlForm->setEnabledDetach(FALSE);

    message = QString("Attached - paused");
  }
  else{

    if(emitSingleCmd(REG_STR_RESUME) != REG_SUCCESS){
      return;
    }

    mControlForm->setPauseButtonLabel(QString("Pause"));

    // enable IOtype commands
    mControlForm->enableIOCmdButtons();

    // enable detach now that app. is running again
    mControlForm->setEnabledDetach(TRUE);

    message = QString("Attached - running");
  }

  mSteerer->statusBarMessageSlot(this, message);
}

//------------------------------------------------------------------------
int
Application::emitSingleCmd(int aCmdId)
{
  // send single command to application using ReG library
  REG_DBGMSG1("Send Cmd id ",aCmdId);
  int lReGStatus = REG_FAILURE;

  //  int lCommandArray[1];
  //  lCommandArray[0] = aCmdId;
 
  try 
  {
    mMutexPtr->lock();
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

    mMutexPtr->unlock();

    if (lReGStatus != REG_SUCCESS)
      THROWEXCEPTION("Emit control");
  }

  catch (SteererException StEx)
  {
    StEx.print();
    // ARP - it's OK for app. to reject a command
    //detachFromApplicationForErrorSlot();
    QMessageBox::warning(0, "Steerer Error", 
			 "Application rejected command",
			 QMessageBox::Ok,
			 QMessageBox::NoButton, 
			 QMessageBox::NoButton);
  }

  return lReGStatus;
}

//------------------------------------------------------------------------
void
Application::customEvent(QEvent *aEvent)
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
    processNextMessage(lEvent);
  }
  else
  {
    REG_DBGMSG("Application::customEvent -  unexpected event type");
  }
}

//------------------------------------------------------------------------
void
Application::processNextMessage(CommsThreadEvent *aEvent)
{
  int aMsgType = aEvent->getMsgType();
  // the commsthread has found a msg for this application - this
  // function calls ReG library routines to process that msg.
  // As thread uses postEvent, this func is executed by GUI thread

  // need this as this done in GUI loop cos of thread->postEvent is
  // possible that this event posted before sim_detach happened for
  // previous event if this is the case the file will have now been
  // deleted
  if (mDetachedFlag)
    return;

  try
  {
    REG_DBGMSG1("Application::processNextMessage, msg type = ", aMsgType);
    switch(aMsgType){

    case IO_DEFS:
    
      REG_DBGMSG("Application::processNextMessage Got IOdefs message");
      // update IOType list and table
      mControlForm->updateIOTypes(false);
      break;

    case CHK_DEFS:

      REG_DBGMSG("Application::processNextMessage Got Chkdefs message");
      // update IOType list and table
      mControlForm->updateIOTypes(true);
      break;

    case PARAM_DEFS:
      
      REG_DBGMSG("Application::processNextMessage Got param defs message");
      // update parameter list and table
      mControlForm->updateParameters(false);

      break;
      
    case STATUS:

      REG_DBGMSG("Application::processNextMessage Got status message");
      int  num_cmds;
      int  *commands;
      bool detached;
      detached = false;
      // update parameter list and table
      mControlForm->updateParameters(true);
      
      // update IOType list and table (needed for frequency update)
      mControlForm->updateIOTypes(false);	// sample types
      mControlForm->updateIOTypes(true);	// checkpoint types

      num_cmds = aEvent->getNumCmds();
      if(num_cmds)commands = aEvent->getCmdsPtr();

      // now deal with commands
      for(int i=0; i<num_cmds && !detached; i++){
  
	REG_DBGMSG2("Recd Cmd", i, commands[i]);
	int lSimHandle = mSimHandle;
		      
	switch(commands[i]){
	  
	case REG_STR_DETACH:
	  {
	  REG_DBGMSG("Application::processNextMessage Received "
		 "detach command from application");
	  detached = true;
	  mMutexPtr->lock();
	  Delete_sim_table_entry(&lSimHandle);     //ReG library 
	  mMutexPtr->unlock();

	  // make GUI form for this application read only
	  disableForDetach(true);
	  QString message = QString("Application has detached");
	  mSteerer->statusBarMessageSlot(this, message);

	  // enable Close button
	  mControlForm->setEnabledClose(TRUE);

	  mDetachedFlag = true;
	  break;
	  }
 
	case REG_STR_STOP:
	  {
	  REG_DBGMSG("Application::processNextMessage Received stop "
		 "command from application");
	  detached = true;
	  mMutexPtr->lock();
	  Delete_sim_table_entry(&lSimHandle);		//ReG library 
	  mMutexPtr->unlock();
	  
	  // make GUI form for this application read only
	  disableForDetach(true);
	  QString message = QString("Detached as application has stopped");
	  mSteerer->statusBarMessageSlot(this, message);

	  // enable Close button
	  mControlForm->setEnabledClose(TRUE);

	  mDetachedFlag = true;
	  break;
	  }

	default:
	  break;
	}  // end switch
      } // end for

      break;

    case STEER_LOG: 
      REG_DBGMSG("Application::processNextMessage Got steer_log message");
      mControlForm->updateParameterLog();
      break;

    case MSG_NOTSET:
      REG_DBGMSG("Application::processNextMessage No msg to process");
      break;

    case CONTROL:
      REG_DBGMSG("Application::processNextMessage Got control message");
      break;
	    
    case SUPP_CMDS:
      REG_DBGMSG("Application::processNextMessage Got supp_cmds message");
      break;

    case MSG_ERROR:
      REG_DBGMSG("Application::processNextMessage Got error when "
		 "attempting to get next message");
      THROWEXCEPTION("Attempt to get next message failed");
      break;

    default:
      REG_DBGMSG("Application::processNextMessage Unrecognised "
		 "msg returned by Get_next_message");
      break;

  } //switch(aMsgType)

  } //try

  catch (SteererException StEx)
  {
    StEx.print();
    cout << "Continuing after exception..." << endl;

    /* ARPDBG - this is draconian
    // detach from application (or at least attempt to)
    // make form read only and update status 

    detachFromApplication();
    disableForDetachOnError();
    QMessageBox::warning(0, "Steerer Error", "Internal library error - detaching from application",
			 QMessageBox::Ok,
			 QMessageBox::NoButton, 
			 QMessageBox::NoButton);
    
    QString message = QString("Detached from application due to internal error");
    mSteerer->statusBarMessageSlot(this, message);
    */
  }

} // ::processNextMessage

void Application::emitGridRestartCmdSlot(){
  // This method is currently a no-op as the restart machinery
  // really should be in the library. SOAP braindamage should
  // not be anywhere near GUI code.
  cout << "Steerer: Restarting is currently disabled, sorry." << endl;

#if 0
  // First of all get the user to enter the GSH
  bool ok = false;

#ifdef REG_WSRF
  struct soap mySoap;
  char       *rpDoc;

  try{

    QString text = QInputDialog::getText(
					 tr( "End Point Reference" ),
					 tr( "Please enter the EPR of the Checkpoint node" ),
					 QLineEdit::Normal, QString::null, &ok, this );

    if(!ok || text.isEmpty())return;

    soap_init(&mySoap);
    if( Get_resource_property_doc(&mySoap,
				  text.ascii(),
				  "", // Username
				  "", // passphrase
				  &rpDoc) != REG_SUCCESS ){

      THROWEXCEPTION("Call to get ResourcePropertyDocument on " 
		     + text + " failed");
    }

    QDomDocument doc("parsedDoc");
    doc.setContent(QString(rpDoc));
    QDomElement docElem = doc.documentElement();
    QDomNodeList list = docElem.elementsByTagName("Chk_UID");
    if(list.count() != 1) 
      THROWEXCEPTION("Did not find only one Chk_UID element in the RP "
		     "Doc from node " + text);

    QDomText uidNode = list.item(0).firstChild().toText();
    if( uidNode.isNull() )
      THROWEXCEPTION("Failed to get value of UID for node " + text);
    
    // Get the UID of the checkpoint node as a QString
    text = uidNode.nodeValue();

    // Now get the handle of this checkpoint
    list = docElem.elementsByTagName("Chk_type");
    if(list.count() != 1) 
      THROWEXCEPTION("Did not find only one Chk_type element in the RP "
		     "Doc from node " + text);

    uidNode = list.item(0).firstChild().toText();
    if( uidNode.isNull() ) 
      THROWEXCEPTION("Failed to get Chk_type value for node " + text);

    QString lChkHandle = uidNode.nodeValue();

    soap_end(&mySoap);
    soap_done(&mySoap);

    int  lCommandArray[1];
    char **lCmdParamArray = new char*[1];
    lCmdParamArray[0] = new char[REG_MAX_STRING_LENGTH];
    sprintf(lCmdParamArray[0], "IN %s", text.ascii());
    lCommandArray[0] = lChkHandle.toInt();

    Emit_control(mSimHandle, 1,
		 lCommandArray, lCmdParamArray); // ReG library

    delete[] lCmdParamArray[0];
    delete [] lCmdParamArray;
  }

  catch (SteererException StEx)
  {
    QMessageBox mb("RealityGrid Steerer",
		   StEx.getErrorMsg(),
		   QMessageBox::Warning,
		   QMessageBox::Ok,
		   Qt::NoButton,
		   Qt::NoButton,
		   this, "Modeless warning", false);
    mb.setModal(false);
    mb.exec(); 

    soap_end(&mySoap);
    soap_done(&mySoap);
  }

#else
  QString text = QInputDialog::getText(
                    tr( "Grid Service Handle" ),
                    tr( "Please enter the GSH" ),
                    QLineEdit::Normal, QString::null, &ok, this );

  if(!ok || text.isEmpty())return;

  // Now issue a restart steer library call with that GSH
  mMutexPtr->lock();
  Emit_restart_cmd(mSimHandle, (char*)text.latin1());
  mMutexPtr->unlock();
#endif // def REG_WSRF
#endif // 0

  return;
}

int Application::getHandle(){
  return mSimHandle;
}

void Application::setCurrentStatus(QString &msg){
  mStatusTxt = msg;
}

QString Application::getCurrentStatus(){
  return mStatusTxt;
}

void Application::hideChkPtTable(bool flag){
  if(mControlForm)mControlForm->hideChkPtTable(flag);
}

void Application::hideIOTable(bool flag){
  if(mControlForm)mControlForm->hideIOTable(flag);
}

void Application::hideSteerTable(bool flag){
  if(mControlForm)mControlForm->hideSteerTable(flag);
}

void Application::hideMonTable(bool flag){
  if(mControlForm)mControlForm->hideMonTable(flag);
}

bool Application::monTableVisible(){
  return mMonTableVisible;
}

bool Application::steerTableVisible(){
  return mSteerTableVisible;
}

bool Application::ioTableVisible(){
  return mIOTableVisible;
}

bool Application::chkTableVisible(){
  return mChkTableVisible;
}

void Application::setMonTableVisible(bool flag){
  mMonTableVisible = flag;
}

void Application::setSteerTableVisible(bool flag){
  mSteerTableVisible = flag;
}

void Application::setIOTableVisible(bool flag){
  mIOTableVisible = flag;
}

void Application::setChkTableVisible(bool flag){
  mChkTableVisible = flag;
}
