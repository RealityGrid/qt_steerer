/*----------------------------------------------------------------------------
  CommsThread and CommsThreadEvent classes for QT steerer GUI.
  CommsThread periodically calls the ReG library routines to look for
  information received from steered applications.  CommsThreadEvent
  encapsulates an event to communicate to the GUI thread that it must
  process information from the steered application.
    
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
      cout << "Interrupt signal received (signal " << aSignal << ")" << endl;
      break;
      
    case SIGTERM:
      cout << "Kill signal received (signal " << aSignal << ")" << endl;
      break;
      
    case SIGSEGV:
      cout << "Illegal Access caught (signal " << aSignal << ")" << endl;
      break;

    case  SIGILL:
      cout << "Illegal Exception caught (signal " << aSignal << ")" << endl;
      break;

      // note: abort called if exception not caught (and hence calls terminate)
    case SIGABRT:
      cout << "Abort signal caught (signal " << aSignal << ")" << endl;
      break;

    case SIGFPE:
      cout << "Arithmetic Exception caught (signal " << aSignal << ")" << endl;
      break;

    default:
      cout << "Signal caught (signal " << aSignal << ")" << endl;

  }
  
  cout << "CommsThread received signal..." << endl;
  gCommsThreadPtr->handleSignal();
  

}

CommsThread::CommsThread(SteererMainWindow *aSteerer, int aCheckInterval)
  : mSteerer(aSteerer), mKeepRunningFlag(true), mCheckInterval(aCheckInterval)
{
  DBGCON("CommsThread");
  gCommsThreadPtr = this; 
  mUseAutoPollInterval = 0;
  mPollCount = 0;
  mMsgCount = 0;

  signal(SIGINT, threadSignalHandler);	//ctrl-c
  signal(SIGTERM, threadSignalHandler);	//kill (note cannot (and should not) catch kill -9)  
  signal(SIGSEGV, threadSignalHandler);
  signal(SIGILL, threadSignalHandler);
  signal(SIGABRT, threadSignalHandler);
  signal(SIGFPE, threadSignalHandler);


}

CommsThread::~CommsThread()
{
  DBGDST("CommsThread");

  // must stop a thread running before it is deleted...
  if (running())
    stop();
}

void
CommsThread::handleSignal()
{
  DBGMSG("CommsThread::handleSignal - send event to main thread");

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
    DBGMSG("CommsThread::stop() - waiting for run completion");
    wait(1000);  //1000 milliseconds
  }
  DBGMSG("Thread is stopped");

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
  // How many polls to average over in order to decide whether
  // or not to adjust the polling interval
  int   lPollAdjustInterval = 10; 
  float lPollRatio;
  DBGMSG("CommsThread starting");
  
  // add sleep to give GUI chance to finsh posting new form SMR XXX thread bug fix
  msleep(3000);

  // keep running until flagged to stop
  while (mKeepRunningFlag)
  {
    DBGMSG1("CommsThread Polling now, count = ", mPollCount);

    // This section automatically adjusts the polling interval
    // to keep up with the attached application(s)
    if(mUseAutoPollInterval && (mPollCount == lPollAdjustInterval)){

      lPollRatio = (float)mMsgCount/(float)mPollCount;
      DBGMSG1("CommsThread: poll ratio = ", lPollRatio);

      if(lPollRatio > 0.85){

	if(mCheckInterval > kMIN_POLLING_INT){
	  // Reduce polling interval faster than we increase it
	  mCheckInterval -= (int)(0.66*mCheckInterval);
	  DBGMSG1("CommsThread: reducing poll interval to ", mCheckInterval);
	}
      }
      else if(lPollRatio < 0.5){

	if(mCheckInterval < kMAX_POLLING_INT){
	  // Not getting messages very often so increase the interval
	  // between polls
	  mCheckInterval += (int)(0.5*mCheckInterval);
	  DBGMSG1("CommsThread: increasing poll interval to ", mCheckInterval);
	}
      }
      mPollCount = 0;
      mMsgCount = 0;
    }

    // reset lMsgType
    lMsgType = MSG_NOTSET;

    // hold qt library mutex for call
    qApp->lock();

    // Get_next_message always returns  REG_SUCCESS currently
    if (Get_next_message(&lSimHandle, &lMsgType) != REG_SUCCESS)  //ReG library
      DBGEXCP("Get_next_message error");

    qApp->unlock();

    // Protect this count to prevent overflow when not using auto. poll interv.
    if(mUseAutoPollInterval)mPollCount++;

    if (lMsgType != MSG_NOTSET){

      // Protect this count to prevent overflow when not using auto. 
      // poll interval
      if(mUseAutoPollInterval)mMsgCount++;

      //SMR XXX  validate lMsgType SMR XXX to do

      // create event and post it - posting means the main GUI thread will 
      // process the event and not this commsthread.
      // this avoids any locking issues around GUI funcs (i think)
      
      CommsThreadEvent *lEvent = new CommsThreadEvent(lMsgType);

      lApp = mSteerer->getApplication(lSimHandle);

      if(lApp){
        postEvent(lApp, lEvent);
      }
      else{
        DBGMSG("CommsThread::run: NULL application pointer!");
      }
    }

    msleep(mCheckInterval);  // sleep for mCheckInterval milliseconds

  }
  DBGMSG("Leaving CommsThread::run");
}

void
CommsThread::setKeepRunning(const bool aFlag)
{
  // flag to get out of while loop in run()
  mKeepRunningFlag = aFlag;
}

int CommsThread::getUseAutoPollFlag() const
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
  DBGCON("CommsThreadEvent");
}
CommsThreadEvent::~CommsThreadEvent()
{
  DBGDST("CommsThreadEvent");
}

int
CommsThreadEvent::getMsgType() const
{
  return mMsgType;
}
