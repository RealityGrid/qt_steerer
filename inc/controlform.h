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
#include "historyplot.h"

class QPushButton;
class QString;
class QHBoxLayout;

class Application;
class ParameterTable;
class Parameter;
class SteeredParameterTable;
class IOTypeTable;
class TableLabel;
class SteererMainWindow;

/// The widget that displays all information on a single application.
/// We have one of these for each application being steered - they
/// are used to populate the tab pages.
/// @author Sue Ramsden
/// @author Mark Riding
/// @author Andrew Porter
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

  /// Disable all buttons on UI
  void disableAll(const bool aUnRegister = true);
  /// Disable all IO-related buttons
  void disableIOCmdButtons();
  /// Enable all IO-related buttons
  void enableIOCmdButtons();
  /// Disable all of the command buttons
  void disableAppCmdButtons();
  /// En/disable the Detach button
  void setEnabledDetach(const bool aEnable);
  /// En/disable the Stop button
  void setEnabledStop(const bool aEnable);
  /// En/disable the Pause (resume) button
  void setEnabledPause(const bool aEnable);
  /// En/disable the Close button
  void setEnabledClose(const bool aEnable);
  /// Set the label on the 'pause' button to reflect
  /// whether app is running or is paused.
  void setPauseButtonLabel(const QString aLabel);
  /**
   * getter method for the Application being controlled by
   * this form
   *
   * @return        pointer to Application object.
   */
  Application *application();
  /** Getter method for the monitored parameters table */
  ParameterTable *getMonParamTable();
  /** Getter method for the steerable parameters table */
  SteeredParameterTable *getSteeredParamTable();
  void newHistoryPlot(Parameter *xParamPtr, Parameter *yParamPtr, 
		      QString xLabel, QString yLabel);

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
  void setRestartButtonStateSlot(const bool aEnable);
  void setCreateButtonStateSlot(const bool aEnable);
  void setConsumeButtonStateSlot(const bool aEnable);
  void setEmitButtonStateSlot(const bool aEnable);

public slots:
  /// Slot called when the user quits a parameter history plot
  void plotClosedSlot(HistoryPlot *ptr);
  /// Slot called when user clicks on a history plot canvas
  void plotSelectedSlot(HistoryPlot *);


signals:
  void detachFromApplicationForErrorSignal();
  /// Signal to tell the HistoryPlot to update
  void paramUpdateSignal(ParameterHistory *mParamHist, 
			 const int paramID);

private:
  int			mSimHandle;		

  QPushButton		*mEmitButton;

  QPushButton		*mSndSampleButton;
  QPushButton		*mSetSampleFreqButton;

  QPushButton		*mRestartChkPtButton;
  // MR:
  QPushButton           *mGridRestartChkPtButton;
  
  QPushButton		*mSndChkPtButton;
  QPushButton		*mSetChkPtFreqButton;

  QPushButton		*mEmitAllValuesButton;

  /// Pointer to the ParameterTable containing monitored params
  ParameterTable	*mMonParamTable;
  /// Pointer to the SteeredParameterTable containing steerable params
  SteeredParameterTable	*mSteerParamTable;
  IOTypeTable		*mIOTypeSampleTable;
  IOTypeTable		*mIOTypeChkPtTable;

  QPushButton		*mCloseButton;
  QPushButton		*mDetachButton;
  QPushButton		*mStopButton;
  QPushButton		*mPauseButton;
  QPushButton           *mConsumeDataButton;
  QPushButton           *mEmitDataButton;
  /// Pointer to the Application object for the app. being steered on
  /// this control form
  Application           *mApplication;

  TableLabel            *mChkTableLabel;
  TableLabel            *mIOTableLabel;
  /// Pointer to the label for the ParameterTable containing steerable params
  TableLabel            *mSteerTableLabel;
  /// Pointer to the label for the ParameterTable containing monitored params
  TableLabel            *mMonTableLabel;

  /// Pointer to mutex protecting calls to ReG steer lib
  QMutex                *mMutexPtr;

public:
  /// List of the history plots associated with this application
  QPtrList<HistoryPlot>  mHistoryPlotList;
  /// Whether or not we are in mode where user is selecting one of the
  /// history plots
  bool                   mUserChoosePlotMode;
  /// Pointer to parameter to add to history plot
  Parameter             *mParamToAdd;
};


#endif


