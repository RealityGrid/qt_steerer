/*----------------------------------------------------------------------------
    Header file for clases CommsThread and CommsThreadEvent frr QT steerer GUI.

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


#ifndef __COMMS_THREAD_H__
#define __COMMS_THREAD_H__

#include <qthread.h>
#include <qevent.h>

class SteererMainWindow;

class CommsThread : public QThread
{
public:
    CommsThread(SteererMainWindow *, int aCheckInterval=1000);
    ~CommsThread();

    void setCheckInterval(const int aInterval);
    int getCheckInterval() const;
    void stop();

protected:
    virtual void run();

private:
    void setKeepRunning(const bool aFlag);

private:
    SteererMainWindow	*mSteerer;
    bool		mKeepRunningFlag;
    int			mCheckInterval;  //milliseconds

};


class CommsThreadEvent : public QCustomEvent
{

public:
  CommsThreadEvent(REG_MsgType aMsgType);
  ~CommsThreadEvent();
  
  REG_MsgType getMsgType() const;
    
private:
  REG_MsgType mMsgType;
  
};

#endif
