/*
  The RealityGrid Steerer

  Copyright (c) 2002-2009, University of Manchester, United Kingdom.
  All rights reserved.

  This software is produced by Research Computing Services, University
  of Manchester as part of the RealityGrid project and associated
  follow on projects, funded by the EPSRC under grants GR/R67699/01,
  GR/R67699/02, GR/T27488/01, EP/C536452/1, EP/D500028/1,
  EP/F00561X/1.

  LICENCE TERMS

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of The University of Manchester nor the names
      of its contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

  Author: Mark Riding
          Andrew Porter
          Sue Ramsden
          Robert Haines
 */

#ifndef __COMMS_THREAD_H__
#define __COMMS_THREAD_H__

#include <qthread.h>
#include <qevent.h>
//Added by qt3to4:
#include <QCustomEvent>
#include <QMutex>

class SteererMainWindow;

class CommsThread : public QThread
{
public:
    CommsThread(SteererMainWindow *, QMutex *, int aCheckInterval=100);
    ~CommsThread();

    void setCheckInterval(const int aInterval);
    int getCheckInterval() const;
    void setUseAutoPollFlag(const int aFlag);
    bool getUseAutoPollFlag() const;
    void stop();
    void handleSignal();

protected:
    virtual void run();

private:
    void setKeepRunning(const bool aFlag);

private:
    SteererMainWindow	*mSteerer;
    bool		mKeepRunningFlag;
    int			mCheckInterval;  //milliseconds
    bool                mUseAutoPollInterval;
    int                 mPollCount;
    int                 mMsgCount;
    // How many polls to average over in order to decide whether
    // or not to adjust the polling interval
    int                 mPollAdjustInterval; 
    int                 mMinPollAdjustInterval;
    QMutex             *mMutexPtr;
};


class CommsThreadEvent : public QCustomEvent
{

public:
  CommsThreadEvent(int aMsgType);
  ~CommsThreadEvent();
  
  int  getMsgType() const;
  void storeCommands(int aNum, int *aArray);
  int  getNumCmds() const;
  int *getCmdsPtr();

private:
  /** The type of the message that generated this event */
  int mMsgType;
  /** The number of commands in the control message that generated
      this event */
  int mNumCmds;
  /** Array holding the commands in the control message that generated
      this event */
  int mCommands[REG_MAX_NUM_STR_CMDS];
  
};

#endif
