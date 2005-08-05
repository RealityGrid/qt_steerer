/*----------------------------------------------------------------------------
  ControlForm class header file for QT steerer GUI. 

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

/**
 * @file controlform.h
 * @brief Header file for form displaying details of attached app. */

#ifndef __CONTROL_FORM_H__
#define __CONTROL_FORM_H__

#include <qgroupbox.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qwidget.h>
#include <qmutex.h>

class QPushButton;
class QString;
class QHBoxLayout;

class Application;
class ParameterTable;
class SteeredParameterTable;
class IOTypeTable;
class TableLabel;
class SteererMainWindow;

class ControlForm : public QWidget 
{
  Q_OBJECT

public:

  ControlForm(QWidget *aParent, const char *aName, int aSimHandle, 
	      Application *aApplication, QMutex *aMutex);
  ~ControlForm();

  void updateParameters();
  void updateIOTypes(bool aChkPtType = false);
  void updateParameterLog();

  void disableAll(const bool aUnRegister = true);
  void disableIOCmdButtons();
  void enableIOCmdButtons();

  void disableAppCmdButtons();
  void setEnabledDetach(const bool aEnable);
  void setEnabledStop(const bool aEnable);
  void setEnabledPause(const bool aEnable);
  void setEnabledResume(const bool aEnable);
  void setEnabledClose(const bool aEnable);
  /**
   * getter method for the Application being controlled by
   * this form
   *
   * @return        pointer to Application object.
   */
  Application *application();

  /**
   * Method to show or hide the checkpoint table and associated label
   * and buttons.
   */
  void hideChkPtTable(bool flag);
  void hideIOTable(bool flag);
  void hideSteerTable(bool flag);
  void hideMonTable(bool flag);

private:
  void updateParameters(bool aSteeredFlag);
  void disableButtons();
  
protected slots:
  void enableParamButtonsSlot();    
  void enableSampleButtonsSlot();   
  void enableChkPtButtonsSlot();
  void emitAllValuesSlot();

  // MR:
  void setRestartButtonStateSlot(const bool aEnable);
  void setCreateButtonStateSlot(const bool aEnable);
  void setConsumeButtonStateSlot(const bool aEnable);
  void setEmitButtonStateSlot(const bool aEnable);


signals:
  void detachFromApplicationForErrorSignal();

private:
  int			mSimHandle;		

  QPushButton		*mEmitButton;

  QPushButton		*mSndSampleButton;
  QPushButton		*mSetSampleFreqButton;

  QPushButton		*mRestartChkPtButton;
  // MR:
  QPushButton   *mGridRestartChkPtButton;
  
  QPushButton		*mSndChkPtButton;
  QPushButton		*mSetChkPtFreqButton;

  QPushButton		*mEmitAllValuesButton;

  ParameterTable	*mMonParamTable;
  SteeredParameterTable	*mSteerParamTable;
  IOTypeTable		*mIOTypeSampleTable;
  IOTypeTable		*mIOTypeChkPtTable;

  QPushButton		*mCloseButton;
  QPushButton		*mDetachButton;
  QPushButton		*mStopButton;
  QPushButton		*mPauseButton;
  QPushButton		*mResumeButton;
  QPushButton           *mConsumeDataButton;
  QPushButton           *mEmitDataButton;
  
  Application   *mApplication;

  TableLabel    *mChkTableLabel;
  TableLabel    *mIOTableLabel;
  TableLabel    *mSteerTableLabel;
  TableLabel    *mMonTableLabel;

  /// Pointer to mutex protecting calls to ReG steer lib
  QMutex        *mMutexPtr;
};


#endif


