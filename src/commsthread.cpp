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

/** @file commsthread.cpp
    @brief Contains the implementation of the thread that polls for messages
    from the steered application(s)

    CommsThread and CommsThreadEvent classes for QT steerer GUI.
    CommsThread periodically calls the ReG library routines to look for
    information received from steered applications.  CommsThreadEvent
    encapsulates an event to communicate to the GUI thread that it must
    process information from the steered application.

    @author Sue Ramsden
    @author Mark Riding
    @author Andrew Porter */

#include "types.h"
#include "debug.h"
#include "commsthread.h"
#include "steerermainwindow.h"
#include "application.h"

#include "ReG_Steer_Steerside.h"

#include <qapplication.h>
#include <signal.h>

//file scope global pointer pointing at this CommsThread object; need this to
//when catch signal.  
CommsThread *gCommsThreadPtr;

extern "C" void threadSignalHandler(int aSignal)
{
  
  // caught one signal - ignore all others now as going to quit and do not
  // want the quit process to be interrupted and restarted...
  signal(SIGINT, SIG_IGN);	//ctrl-c
  signal(SIGTERM, SIG_IGN);	//kill (note cannot (and should not) catch kill -9)  
  signal(SIGSEGV, SIG_IGN);
  signal(SIGILL, SIG_IGN);
  signal(SIGABRT, SIG_IGN);
  signal(SIGFPE, SIG_IGN);

  switch(aSignal)
  {
    case SIGINT:
      cout << "CommsThread: Interrupt signal received (signal " << aSignal << ")" << endl;
      break;
      
    case SIGTERM:
      cout << "CommsThread: Kill signal received (signal " << aSignal << ")" << endl;
      break;
      
    case SIGSEGV:
      cout << "CommsThread: Segmentation violation caught (signal " << aSignal << ")" << endl;
      break;

    case  SIGILL:
      cout << "CommsThread: Illegal Exception caught (signal " << aSignal << ")" << endl;
      break;

      // note: abort called if exception not caught (and hence calls terminate)
    case SIGABRT:
      cout << "CommsThread: Abort signal caught (signal " << aSignal << ")" << endl;
      break;

    case SIGFPE:
      cout << "CommsThread: Arithmetic Exception caught (signal " << aSignal << ")" << endl;
      break;

    default:
      cout << "CommsThread: Signal caught (signal " << aSignal << ")" << endl;

  }
  
  cout << "CommsThread received signal..." << endl;
  gCommsThreadPtr->handleSignal();
}

CommsThread::CommsThread(SteererMainWindow *aSteerer, QMutex *aMutex, 
			 int aCheckInterval)
  : mSteerer(aSteerer), mKeepRunningFlag(true), 
    mCheckInterval(aCheckInterval), mMutexPtr(aMutex)
{
  REG_DBGCON("CommsThread constructor");
  gCommsThreadPtr = this; 
  // Set polling interval automatically
  mUseAutoPollInterval = aSteerer->autoPollingOn();
  mPollCount = 0;
  mMsgCount = 0;
  // How many polls to average over in order to decide whether
  // or not to adjust the polling interval
  mPollAdjustInterval = 4; 
  mMinPollAdjustInterval = 4;

  signal(SIGINT, threadSignalHandler);	//ctrl-c
  signal(SIGTERM, threadSignalHandler);	//kill (note cannot (and should not) catch kill -9)  
  signal(SIGSEGV, threadSignalHandler);
  signal(SIGILL, threadSignalHandler);
  signal(SIGABRT, threadSignalHandler);
  signal(SIGFPE, threadSignalHandler);
}

CommsThread::~CommsThread()
{
  REG_DBGDST("CommsThread");

  // must stop a thread running before it is deleted...
  if (running())
    stop();
}

void
CommsThread::handleSignal()
{
  REG_DBGMSG("CommsThread::handleSignal - send event to main thread");

  QCustomEvent *lEvent = new QCustomEvent(QEvent::User + kSIGNAL_EVENT);
  postEvent(mSteerer, lEvent);
    
}

void 
CommsThread::setCheckInterval(const int aInterval)
{
  // note value aInterval constrained on user entry also

  if (aInterval > kMIN_POLLING_INT)
    mCheckInterval = aInterval;
  else
    mCheckInterval = kMIN_POLLING_INT;
}

int
CommsThread::getCheckInterval() const
{
 return mCheckInterval;
}

void
CommsThread::stop()
{
  // flag thread to stop running (get out of while loop)
  setKeepRunning(false);

  // thread must have finished running before destruction - so wait for finish
  while (!finished())
  {
    REG_DBGMSG("CommsThread::stop() - waiting for run completion");
    wait(1000);  //1000 milliseconds
  }
  REG_DBGMSG("CommsThread: Thread is stopped");

  // reset flag for next run()
  setKeepRunning(true);
}


void 
CommsThread::run()
{
  // this is the routine that is call when CommsThread->start() is called.
  // this routine runs until flagged to stop
  Application *lApp;
  int	lSimHandle = REG_SIM_HANDLE_NOTSET ;
  int   lMsgType = MSG_NOTSET;
  float lPollRatio;
  int   app_seqnum;
  int   num_cmds = 0;
  int   status = REG_FAILURE;
  int   commands[REG_MAX_NUM_STR_CMDS];
  const float maxSuccessFraction = 0.9;
  const float minSuccessFraction = 0.6;
  REG_DBGMSG("CommsThread starting");
  
  // add sleep to give GUI chance to finsh posting new form SMR XXX thread bug fix
  msleep(1000);

  // keep running until flagged to stop
  while (mKeepRunningFlag)
  {
    // This section automatically adjusts the polling interval
    // to keep up with the attached application(s)
    if(mUseAutoPollInterval && (mPollCount == mPollAdjustInterval)){

      lPollRatio = (float)mMsgCount/(float)mPollCount;
      cout << "CommsThread: mMsgCount = " << mMsgCount << endl;
      cout << "CommsThread: mPollCount = " << mPollCount << endl;
      cout << "CommsThread: poll ratio = " << lPollRatio << endl;
      cout << "CommsThread: interval = " << mCheckInterval << endl;
      if(lPollRatio > maxSuccessFraction){

	if(mCheckInterval > kMIN_POLLING_INT){
	  // Reduce polling interval faster than we increase it
	  mCheckInterval -= (int)(0.35*mCheckInterval);
	  cout << "CommsThread: reducing poll interval to " 
	       << mCheckInterval << endl;
	  // Adjust how many calls we average over to try and ensure
	  // we check about every 2 seconds
	  mPollAdjustInterval = (int)(1500.0/(float)mCheckInterval);
	  if(mPollAdjustInterval < mMinPollAdjustInterval){
	    mPollAdjustInterval = mMinPollAdjustInterval;
	  }
	  cout << "CommsThread: poll adjust interval = " << 
	    mPollAdjustInterval << endl;
	}
      }
      else if(lPollRatio < minSuccessFraction){

	if(mCheckInterval < kMAX_POLLING_INT){
	  // Not getting messages very often so increase the interval
	  // between polls
	  mCheckInterval += (int)(0.2*mCheckInterval) + 1;
	  cout << "CommsThread: increasing poll interval to " << 
	    mCheckInterval << endl;
	  // Adjust how many calls we average over to try and ensure
	  // we check about every 2 seconds
	  mPollAdjustInterval = (int)(1500.0/(float)mCheckInterval);
	  if(mPollAdjustInterval < mMinPollAdjustInterval){
	    mPollAdjustInterval = mMinPollAdjustInterval;
	  }
	  cout << "CommsThread: poll adjust interval = " << 
	    mPollAdjustInterval << endl;
	}
      }
      mPollCount = 0;
      mMsgCount = 0;
    }

    // reset lMsgType
    lMsgType = MSG_NOTSET;

    // hold qt library mutex for library call
    mMutexPtr->lock();
    // Get_next_message always returns  REG_SUCCESS currently
    if (Get_next_message(&lSimHandle, &lMsgType) != REG_SUCCESS){  //ReG library
      mMutexPtr->unlock();
      REG_DBGEXCP("Get_next_message error");
    }
    mMutexPtr->unlock();

    // Protect this count to prevent overflow when not using auto. poll interv.
    if(mUseAutoPollInterval)mPollCount++;

    if(lMsgType == MSG_ERROR){
      REG_DBGMSG("CommsThread: Got error when attempting to get "
		 "next message");
      continue;
    }

    if (lMsgType != MSG_NOTSET){

      // Protect this count to prevent overflow when not using auto. 
      // poll interval
      if(mUseAutoPollInterval)mMsgCount++;

      switch(lMsgType){

      case IO_DEFS:
      
	REG_DBGMSG("CommsThread: Got IOdefs message");

	// hold qt library mutex for library call
	mMutexPtr->lock();
	status = Consume_IOType_defs(lSimHandle); //ReG library 
	mMutexPtr->unlock();
	break;

      case CHK_DEFS:
	
	REG_DBGMSG("CommsThread: Got Chkdefs message");
	mMutexPtr->lock();
	status = Consume_ChkType_defs(lSimHandle); //ReG library 
	mMutexPtr->unlock();
	break;

      case PARAM_DEFS:
      
	REG_DBGMSG("CommsThread: Got param defs message");
	mMutexPtr->lock();
	status = Consume_param_defs(lSimHandle); //ReG library 
	mMutexPtr->unlock();
	break;
      
      case STATUS:

	REG_DBGMSG("CommsThread: Got status message");
	mMutexPtr->lock();
	status = Consume_status(lSimHandle,   //ReG library 
				&app_seqnum,
				&num_cmds, commands);
	mMutexPtr->unlock();
	break;

      case STEER_LOG: 
	REG_DBGMSG("CommsThread: Got steer_log message");
	mMutexPtr->lock();
	status = Consume_log(lSimHandle);   //ReG library 
	mMutexPtr->unlock();
	break;

      case MSG_NOTSET:
	REG_DBGMSG("CommsThread: No msg to process");
	break;

      case CONTROL:
	REG_DBGMSG("CommsThread: Got control message");
	break;
	    
      case SUPP_CMDS:
	REG_DBGMSG("CommsThread: Got supp_cmds message");
	break;

      default:
	cout << "Unrecognised msg returned by Get_next_message: " <<
	  lMsgType << endl;
	break;

      } //switch(aMsgType)

      if(status == REG_SUCCESS){
	// create event and post it - posting means the main GUI
	// thread will process the event and not this commsthread.
	// this avoids any locking issues around GUI funcs (i think)
	lApp = mSteerer->getApplication(lSimHandle);

	// ARPDBG - attempt to avoid lock-up on shutdown
	if(lApp && mKeepRunningFlag){

	  CommsThreadEvent *lEvent = new CommsThreadEvent(lMsgType);
	  if(num_cmds)lEvent->storeCommands(num_cmds, commands);
	  postEvent(lApp, lEvent);
	}
	else{
	  REG_DBGMSG("CommsThread::run: NULL application pointer!");
	}
      }
    }
    msleep(mCheckInterval);  // sleep for mCheckInterval milliseconds

  }
  REG_DBGMSG("Leaving CommsThread::run");
}

void
CommsThread::setKeepRunning(const bool aFlag)
{
  // flag to get out of while loop in run()
  mKeepRunningFlag = aFlag;
}

bool CommsThread::getUseAutoPollFlag() const
{
  return mUseAutoPollInterval;
}

void CommsThread::setUseAutoPollFlag(const int aFlag)
{
  if(aFlag){
    mMsgCount = 0;
    mPollCount = 0;
  }
  mUseAutoPollInterval = aFlag;
  return;
}


CommsThreadEvent::CommsThreadEvent(int aMsgType)
  : QCustomEvent(QEvent::User + kMSG_EVENT), mMsgType(aMsgType)
{ 
  // class to extend QCustomEvent to hold mMsgType
  REG_DBGCON("CommsThreadEvent");
  mNumCmds = 0;
}

CommsThreadEvent::~CommsThreadEvent()
{
  REG_DBGDST("CommsThreadEvent");
}

/** Returns the type of the message that generated this event */
int
CommsThreadEvent::getMsgType() const
{
  return mMsgType;
}

/** Copy the array of commands into internal storage */
void CommsThreadEvent::storeCommands(int aNum, int *aArray)
{
  int i;
  mNumCmds = aNum;
  for(i=0; i<mNumCmds; i++){
    mCommands[i] = aArray[i];
  }
}

int CommsThreadEvent::getNumCmds() const
{
  return mNumCmds;
}

int *CommsThreadEvent::getCmdsPtr()
{
  return mCommands;
}
