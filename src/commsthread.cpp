/*----------------------------------------------------------------------------
    CommsThread and CommsThreadEvent classes for QT steerer GUI.
    CommsThread periodically calls the ReG library routines to look for
    information received from steered applications.  CommsThreadEvent
    encapsulates an event to communicate to the GUI thread that it must
    process information from the steered application.
    
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
#include "commsthread.h"
#include "steerermainwindow.h"
#include "application.h"

#include "ReG_Steer_Steerside.h"

#include <qapplication.h>

CommsThread::CommsThread(SteererMainWindow *aSteerer, int aCheckInterval)
  : mSteerer(aSteerer), mKeepRunningFlag(true), mCheckInterval(aCheckInterval)
{
  DBGCON("CommsThread");
  
}

CommsThread::~CommsThread()
{
  DBGDST("CommsThread");

  // must stop a thread running before it is deleted...
  if (running())
    stop();
}


void 
CommsThread::setCheckInterval(const int aInterval)
{
  // note value aInterval constrained on user entry also

  if (aInterval > kMIN_POLING_INT)
    mCheckInterval = aInterval;
  else
    mCheckInterval = kMIN_POLING_INT;
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
    DBGMSG("CommsThread stop()  waiting for run completion");
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

  int	lSimHandle = REG_SIM_HANDLE_NOTSET ;
  REG_MsgType lMsgType = MSG_NOTSET;

  DBGMSG("CommsThread starting");
  
  // add sleep to give GUI chance to finsh posting new form SMR XXX thread bug fix
  msleep(3000);

  // keep running until flagged to stop
  while (mKeepRunningFlag)
  {
    DBGMSG("Poling now");

    // reset lMsgType
    lMsgType = MSG_NOTSET;

    // hold qt library mutex for call
    qApp->lock();

    // Get_next_message always returns  REG_SUCCESS currently
    if (Get_next_message(&lSimHandle, &lMsgType) != REG_SUCCESS)	//ReG library
      DBGEXCP("Get_next_message error");

    qApp->unlock();

    if (lMsgType != MSG_NOTSET)
    {
      //SMR XXX  future: find Application in list that matches lSimHandle
      //SMR XXX  validate lMsgType SMR XXX to do

      // create event and post it - posting means the main GUI thread will process the event and not this commsthread.
      // this avoids any locking issues around GUI funcs (i think)
      
      /// SMR XXX make this member of COmmsThread and resue rather than new each time - chk when QT dlete ?
      CommsThreadEvent *lEvent = new CommsThreadEvent(lMsgType);
      postEvent(mSteerer->getApplication(), lEvent);
    }

    msleep(mCheckInterval);  // sleep for mCheckInterval milliseconds
  
  }

}

void
CommsThread::setKeepRunning(const bool aFlag)
{
  // flag to get out of while loop in run()
  mKeepRunningFlag = aFlag;
}


CommsThreadEvent::CommsThreadEvent(REG_MsgType aMsgType)
  : QCustomEvent(QEvent::User + 100), mMsgType(aMsgType)
{ 
  // class to extend QCustomEvent to hold mMsgType
  DBGCON("CommsThreadEvent");
}
CommsThreadEvent::~CommsThreadEvent()
{
  DBGDST("CommsThreadEvent");
}

REG_MsgType
CommsThreadEvent::getMsgType() const
{
  return mMsgType;
}
