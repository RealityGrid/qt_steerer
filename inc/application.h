/*----------------------------------------------------------------------------
    Application class header file for QT steerer GUI.

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


#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "types.h"

#include <qwidget.h>

class QGroupBox;
class QPushButton;
class QString;

class ControlForm;

class Application : public QWidget
{
  Q_OBJECT

public:

  Application(QWidget *mParent, const char *, int aSimHandle);
  ~Application();

  void customEvent(QCustomEvent *);
  void processNextMessage(REG_MsgType aMsgType);
  void enableCmdButtons();


private:
  void detachFromApplication();
  void disableForDetach(const bool aUnRegister = true);
  void emitSingleCmd(int aCmdId);

protected slots:
  void emitDetachCmdSlot();
  void emitStopCmdSlot();
  void emitPauseCmdSlot();
  void emitResumeCmdSlot();
  void closeApplicationSlot();
  void detachFromApplicationForErrorSlot();

signals:
  void closeApplicationSignal(int aSimHandle);


private:
  const int		mSimHandle;

  int		mNumCommands;
  bool		mDetachSupported;
  bool		mStopSupported;
  bool		mPauseSupported;
  bool		mResumeSupported;
  bool		mDetachedFlag;

  ControlForm	*mControlForm;
  QGroupBox	*mControlBox; 


};


#endif
