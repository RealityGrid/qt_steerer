/*----------------------------------------------------------------------------
  ControlForm class for QT steerer GUI.

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
 * @file controlform.cpp
 * @brief Implementation of class for showing details of attached app
 */

#include "types.h"
#include "debug.h"
#include "ReG_Steer_Steerside.h"

#include "application.h"
#include "controlform.h"
#include "parametertable.h"
#include "iotypetable.h"
#include "utility.h"
#include "exception.h"
#include "steerermainwindow.h"

#include <qapplication.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h> 
#include <qvbox.h>
#include <qvgroupbox.h>
#include <qhbuttongroup.h>
#include <qgroupbox.h>

ControlForm::ControlForm(QWidget *aParent, const char *aName, int aSimHandle, 
			 Application *aApplication, QMutex *aMutex)
  : QWidget(aParent, aName), mSimHandle(aSimHandle), 
    mEmitButton(kNULL), 
    mSndSampleButton(kNULL), mSetSampleFreqButton(kNULL), 
    mRestartChkPtButton(kNULL), 
    mSndChkPtButton(kNULL), mSetChkPtFreqButton(kNULL), 
    mEmitAllValuesButton(kNULL),
    mMonParamTable(kNULL),
    mSteerParamTable(kNULL), mIOTypeSampleTable(kNULL), 
    mIOTypeChkPtTable(kNULL),
    mCloseButton(kNULL), mDetachButton(kNULL), mStopButton(kNULL), 
    mPauseButton(kNULL), mConsumeDataButton(kNULL), 
    mEmitDataButton(kNULL), mMutexPtr(aMutex)
{ 
  DBGCON("ControlForm");

  mHistoryPlotList.setAutoDelete( TRUE );

  // MR: keep a reference to the Application class
  mApplication = aApplication;
  
  // create widget which holds all steering data (some dynamic) 
  // for one steered application this consists of four tables - 
  // one each for monitored parameters, steered parameters,
  // samples iotypes and checkpoint iotypes, plus buttons 
  // associated with the tables

  // set up main command buttons
  // we use "Tell All" button sizeHint to size all buttons to this size

  // first of all create all the buttons so that we can choose
  // which to use for the size hint without having to reorder
  // any code
  mPauseButton = new QPushButton( "Pause", this, "pause" );
  mDetachButton = new QPushButton( "Detach", this, "detach" );
  mCloseButton = new QPushButton( "Close", this, "close" );
  mEmitAllValuesButton = new QPushButton("Tell All", this, "tellallvals");
  mStopButton = new QPushButton( "Stop", this, "stop" );

  if (!mApplication->isLocal())
    mGridRestartChkPtButton = new QPushButton("Restart", this, "restart");

  // MR: If we're doing a grid session, have the restart button up
  //     in the top control panel, as we're going to restart from
  //     a GSH only...
  QHBoxLayout *lCmdGrpLayout;
  lCmdGrpLayout = new QHBoxLayout(-1, "Commands");
  
  // then set up each button individually, never setting the maximum size
  // so that each button stretches to fill the standard gap
  
  QToolTip::add(mPauseButton, "Tell the attached application to pause/resume");
  connect( mPauseButton, SIGNAL( clicked() ), aApplication, 
	   SLOT( emitPauseCmdSlot() ));
  lCmdGrpLayout->addWidget(mPauseButton);

  QToolTip::add(mDetachButton, "Tell the attached application to detach");
  connect( mDetachButton, SIGNAL( clicked() ), aApplication, 
	   SLOT( emitDetachCmdSlot() ));
  lCmdGrpLayout->addWidget(mDetachButton);

  QToolTip::add( mCloseButton, "Close form to allow another attach");
  connect(mCloseButton, SIGNAL( clicked() ), aApplication, 
	  SLOT(closeApplicationSlot()) );
  lCmdGrpLayout->addWidget(mCloseButton);

  QToolTip::add(mEmitAllValuesButton, "Tell application all new "
		"parameter and frequency values");
  connect(mEmitAllValuesButton, SIGNAL(clicked()), this, 
	  SLOT(emitAllValuesSlot()));
  lCmdGrpLayout->addWidget(mEmitAllValuesButton);

  QToolTip::add(mStopButton, "Tell the attached application to stop");
  connect( mStopButton, SIGNAL( clicked() ), aApplication, 
	   SLOT( emitStopCmdSlot() ));
  lCmdGrpLayout->addWidget(mStopButton);

  if (!mApplication->isLocal()){
    //mGridRestartChkPtButton->setMinimumSize(mEmitAllValuesButton->sizeHint());
    QToolTip::add(mGridRestartChkPtButton, "Restart via a GSH");
    connect( mGridRestartChkPtButton, SIGNAL( clicked() ), aApplication, 
	     SLOT( emitGridRestartCmdSlot() ));
    lCmdGrpLayout->addWidget(mGridRestartChkPtButton);
  }

  //--------------------------------------
  // set up table for monitored parameters
  mMonTableLabel = new TableLabel("Monitored Parameters", 
				  this);
  mMonParamTable = new ParameterTable(this, "monparamtable", 
				      aSimHandle, mMutexPtr);
  mMonParamTable->initTable();

  QVBoxLayout *lTopLeftLayout = new QVBoxLayout(-1, "topleftlayout");
  lTopLeftLayout->addWidget(mMonTableLabel);
  lTopLeftLayout->addWidget(mMonParamTable);

  //-----------------------------
  // table for steered parameters
  mSteerParamTable = new SteeredParameterTable(this,"steerparamtable", 
					       mMonParamTable, aSimHandle,
					       mMutexPtr);
  mSteerParamTable->initTable();
  connect(mSteerParamTable, SIGNAL(detachFromApplicationForErrorSignal()), 
	  aApplication, SLOT(detachFromApplicationForErrorSlot()));

  // set up buttons for steered parameters
  mEmitButton = new QPushButton( "Tell", this, "tellvalue" );
  QToolTip::add(mEmitButton, "Tell application new parameter values");
  connect( mEmitButton, SIGNAL( clicked() ), mSteerParamTable, 
	   SLOT( emitValuesSlot() ) );

  mSteerTableLabel = new TableLabel("Steered Parameters", this);

  QHBoxLayout *lSteeredLableLayout = new QHBoxLayout(-1, 
						     "SteeredLableLayout");
  lSteeredLableLayout->addItem(new QSpacerItem( 0, 0, QSizePolicy::Expanding, 
						QSizePolicy::Minimum));
  lSteeredLableLayout->addWidget(mEmitButton);

  QVBoxLayout *lSteerLayout = new QVBoxLayout(-1, "SteerLayout");
  lSteerLayout->addWidget(mSteerTableLabel);
  lSteerLayout->addWidget(mSteerParamTable);
  lSteerLayout->addLayout(lSteeredLableLayout);

  //------------------------------
  // Table for IOTypes
  mIOTypeSampleTable = new IOTypeTable(this,"sampleparamtable",aSimHandle,
				       mMutexPtr);
  mIOTypeSampleTable->initTable();
  connect(mIOTypeSampleTable, SIGNAL(detachFromApplicationForErrorSignal()), 
	  aApplication, SLOT(detachFromApplicationForErrorSlot()));

  // table and buttons for sample iotypes
  mSetSampleFreqButton = new QPushButton( "Tell Freq's", this, 
					  "TellFreq" );
  QToolTip::add(mSetSampleFreqButton, "Tell application new frequency "
		"values for data IO");
  connect(mSetSampleFreqButton, SIGNAL( clicked() ), 
	  mIOTypeSampleTable, SLOT( emitValuesSlot()));

  mConsumeDataButton = new QPushButton( "Consume", this, "ConsumeData" );
  QToolTip::add(mConsumeDataButton, "Tell application to Consume data");
  connect( mConsumeDataButton, SIGNAL( clicked() ), mIOTypeSampleTable, 
	   SLOT( consumeButtonPressedSlot()));

  mEmitDataButton = new QPushButton( "Emit", this, "EmitData" );
  QToolTip::add(mEmitDataButton, "Tell application to Emit data");
  connect( mEmitDataButton, SIGNAL( clicked() ), mIOTypeSampleTable, 
	   SLOT( emitButtonPressedSlot()));
    
  QVBoxLayout *lSampleLayout = new QVBoxLayout(-1, "sampletablayout");
  QHBoxLayout *lSampleButtonLayout = new QHBoxLayout(-1, "samplebuttons");

  mIOTableLabel = new TableLabel("Data IO", this);

  lSampleButtonLayout->addItem(new QSpacerItem( 0, 0, QSizePolicy::Expanding, 
  						QSizePolicy::Minimum));
  lSampleButtonLayout->addWidget(mConsumeDataButton);
  lSampleButtonLayout->addWidget(mEmitDataButton);
  lSampleButtonLayout->addWidget(mSetSampleFreqButton);

  lSampleLayout->addWidget(mIOTableLabel);
  lSampleLayout->addWidget(mIOTypeSampleTable);
  lSampleLayout->addLayout(lSampleButtonLayout);

  //-------------------------------------------
  // table and buttons for checkpoint iotypes
  mIOTypeChkPtTable = new IOTypeTable(this,"chkptparamtable", aSimHandle, 
				      mMutexPtr, true);
  mIOTypeChkPtTable->initTable();
  connect(mIOTypeChkPtTable, 
	  SIGNAL(detachFromApplicationForErrorSignal()), 
	  aApplication, SLOT(detachFromApplicationForErrorSlot()));

  mSetChkPtFreqButton = new QPushButton( "Tell Freq's", this, 
					 "tellfreq" );
  QToolTip::add(mSetChkPtFreqButton, "Tell application new frequency "
		"values for checkpoint iotypes");  
  connect(mSetChkPtFreqButton, SIGNAL( clicked() ), 
	  mIOTypeChkPtTable, SLOT( emitValuesSlot()));

  // MR: we only want to add the RestartChkPtButton to the ChkPtButtonLayout
  //     if we're in local attach mode. If we're in grid attach mode then
  //     we'll place the button on the top control panel, and use it to get
  //     a GSH from an input dialog box instead.
  if (mApplication->isLocal()){
    mRestartChkPtButton = new QPushButton( "Restart", this, "restartchkpt" );
    QToolTip::add(mRestartChkPtButton, "Tell application to restart using "
		  "requested checkpoint");
    connect( mRestartChkPtButton, SIGNAL(clicked()), mIOTypeChkPtTable, 
	     SLOT(restartButtonPressedSlot()));
  }

  mSndChkPtButton = new QPushButton( "Create", this, "sndchkpt" );
  QToolTip::add(mSndChkPtButton, 
		"Tell application to create requested checkpoint iotypes");
  connect( mSndChkPtButton, SIGNAL( clicked() ), mIOTypeChkPtTable, 
	   SLOT( createButtonPressedSlot()));

  QVBoxLayout *lChkPtLayout = new QVBoxLayout(-1, "chktablayout");
  QHBoxLayout *lChkPtButtonLayout = new QHBoxLayout(-1, "chkptbuttons");

  mChkTableLabel = new TableLabel("CheckPoint Types", this);
  lChkPtButtonLayout->addItem(new QSpacerItem( 0, 0, QSizePolicy::Expanding, 
					       QSizePolicy::Minimum));
  // MR: RestartChkPtButton Local vs Grid
  if (mApplication->isLocal())
    lChkPtButtonLayout->addWidget(mRestartChkPtButton);

  lChkPtButtonLayout->addWidget(mSndChkPtButton);
  lChkPtButtonLayout->addWidget(mSetChkPtFreqButton);

  lChkPtLayout->addWidget(mChkTableLabel);
  lChkPtLayout->addWidget(mIOTypeChkPtTable);
  lChkPtLayout->addLayout(lChkPtButtonLayout);

  connect(this, SIGNAL(detachFromApplicationForErrorSignal()), 
	  aApplication, SLOT(detachFromApplicationForErrorSlot()));

  //---------------------------------------------
  // the overall layout
  QVBoxLayout *lEditLayout = new QVBoxLayout(this, 0, 0, "editlayout");

  lEditLayout->addLayout(lCmdGrpLayout);
  lEditLayout->addLayout(lTopLeftLayout);
  lEditLayout->addLayout(lSteerLayout);
  lEditLayout->addLayout(lSampleLayout);
  lEditLayout->addLayout(lChkPtLayout);

  // disable buttons - they are enabled when tables have some data
  disableButtons();

  // disable app command buttons - enabled once we know what commands
  // are supported
  disableAppCmdButtons();

  // Close button only becomes enabled when detach from application
  mCloseButton->setEnabled(FALSE);

  // Whether we are in mode where user is selecting a history plot
  mUserChoosePlotMode = false;
  mParamToAdd = NULL;
} 

ControlForm::~ControlForm()
{
  DBGDST("ControlForm");
}

void
ControlForm::updateParameters()
{
  // update monitored parameters
  updateParameters(false);
  
  // update steered parameters
  updateParameters(true);
}


void
ControlForm::updateParameters(bool aSteeredFlag)
{
  // call ReG library routines to get all parameter data and
  // update table displaying paremeters on gui
  // aSteerFlag determines whether get monitored or steered parameters

  int lNumParams = 0;
  bool lCleanUpFlag = false;
  Param_details_struct *lParamDetails = kNULL;

  try
    {
      // find out number of parameters that library is going to give us
      mMutexPtr->lock();
      if(Get_param_number(mSimHandle, aSteeredFlag, &lNumParams) 
	 != REG_SUCCESS){  //ReG library
	mMutexPtr->unlock();
	THROWEXCEPTION("Get_param_number");
      }
      mMutexPtr->unlock();
    
      DBGMSG1("Number Params: = ", lNumParams);
    
      if (lNumParams > 0)
	{
	  // flag that cleanup required - don't like this but checking 
	  // for kNULL didn't work
	  lCleanUpFlag = true;

	  // set up arrays of appropriate size for Get_param_values
	  lParamDetails = new Param_details_struct[lNumParams];

	  // point to relevent table - i.e. steered or monitored
	  ParameterTable *lTablePtr;
	  if (aSteeredFlag)
	    lTablePtr = mSteerParamTable;
	  else
	    lTablePtr = mMonParamTable;
      
	  // get parameter data from library and update tables on gui
	  mMutexPtr->lock();
	  if (Get_param_values(mSimHandle,		//ReG library
			       aSteeredFlag,
			       lNumParams,
			       lParamDetails) == REG_SUCCESS){
	    mMutexPtr->unlock();

	    for (int i=0; i<lNumParams; i++){
		  //check if already exists - if so only update value
		  if (!(lTablePtr->updateRow(lParamDetails[i].handle, 
					     lParamDetails[i].value)))
		    {
		      // must be new parameter so add it
		      // MR: changes to add the min and max values to the steerable parameter table
		      if (aSteeredFlag){
			((SteeredParameterTable*)lTablePtr)->addRow(lParamDetails[i].handle,
								    lParamDetails[i].label, 
								    lParamDetails[i].value, 
								    lParamDetails[i].type,
								    lParamDetails[i].min_val,
								    lParamDetails[i].max_val);
		      }
		      else{
			lTablePtr->addRow(lParamDetails[i].handle,
					  lParamDetails[i].label,
					  lParamDetails[i].value,
					  lParamDetails[i].type);
		      }
		    } 

		  Parameter *lParamPtr = lTablePtr->findParameter(lParamDetails[i].handle);
		  // Emit a SIGNAL so that any HistoryPlots can update
		  cout << "ARPDBG: emitting signal to update parameter: " << i << endl;
		  emit paramUpdateSignal(lParamPtr->mParamHist, 
					 lParamPtr->getId());
	    } //for lNumParams
	    
	    // Adjust width of first column holding labels      
	    lTablePtr->adjustColumn(0);

	  } // if Get_param_values
	  else{

	    mMutexPtr->unlock();
	    THROWEXCEPTION("Get_param_values");
	  }

	  // delete local arrays
	  delete [] lParamDetails;

	} // if lNumParams > 0

      // finally check for any parameters no longer present and flag 
      // as unregistered SMR XXX to do (ReG library not support unRegister yet)
  
    } //try

  catch (SteererException StEx)
    {
      // clean up
      if (lCleanUpFlag)
	{
	  delete [] lParamDetails;
	}

      // rethrow to Application::processNextMessage
      throw StEx;
    }
    
} // ::updateParameters

//--------------------------------------------------------------------

void
ControlForm::updateParameterLog()
{
  mMonParamTable->updateParameterLog();
  mSteerParamTable->updateParameterLog();
}

//--------------------------------------------------------------------

void
ControlForm::updateIOTypes(bool aChkPtType)
{
  // call ReG library routines to get all iotype data
  // and update tables displaying iotypes on gui

  IOTypeTable	*lIOTypeTablePtr;
  int		lNumTypes;
  int		*lHandles = kNULL;
  int		*lTypes = kNULL;
  int		*lVals = kNULL;
  char		**lLabels = kNULL;
  bool		lCleanUpFlag = false;
  int		lStatus = REG_FAILURE;
  int		i;


  // point to relevant table - sample or checkpoint
  if (aChkPtType)
    lIOTypeTablePtr = mIOTypeChkPtTable;
  else
    lIOTypeTablePtr = mIOTypeSampleTable;
  

  try
  {

    mMutexPtr->lock();
    if (aChkPtType){
      lStatus = Get_chktype_number(mSimHandle, &lNumTypes);	//ReG library
    }
    else{
      lStatus = Get_iotype_number(mSimHandle, &lNumTypes);	//ReG library    
    }
    mMutexPtr->unlock();

    if(lStatus != REG_SUCCESS)		
      THROWEXCEPTION("Get_iotype_number");
    
    DBGMSG1("Number IO/Chk Types: Monitored = ", lNumTypes);
    
    if (lNumTypes>0)
    {
      lCleanUpFlag = true;
      
      // setup arrays of appropriate size for Get_param_values
      // note that REG_MAX_STRING_LENGTH is max string length imposed by library
      lHandles = new int[lNumTypes];
      lTypes = new int[lNumTypes];
      lVals = new int[lNumTypes];
      lLabels = new char *[lNumTypes];   
      for(i=0; i<lNumTypes; i++)
      {
	lLabels[i] = new char[REG_MAX_STRING_LENGTH + 1];
      }
      
      mMutexPtr->lock();
      if (aChkPtType){
	lStatus = Get_chktypes(mSimHandle,     		//ReG library
			       lNumTypes,
			       lHandles,
			       lLabels,
			       lTypes,
			       lVals);
      }
      else{
	lStatus = Get_iotypes(mSimHandle,      		//ReG library
			      lNumTypes,
			      lHandles,
			      lLabels,
			      lTypes,
			      lVals);
      }
      mMutexPtr->unlock();

      if (lStatus == REG_SUCCESS)
      {
	for (i=0; i<lNumTypes; i++)
	{	  
	  //check if already exists - if so only update frequency value
	  if (!(lIOTypeTablePtr->updateRow(lHandles[i], lVals[i])))
	  {
	    // new IOTypee so add it
	    lIOTypeTablePtr->addRow(lHandles[i], lLabels[i], lVals[i], lTypes[i]);
	  } 
	  
	} //for lNumTypes            
      } // if Get_IOType_values
      else
	THROWEXCEPTION("Get_iotypes");
      
      // note: no need to check for any IOType no longer present
      // as iotype cannot be unregistered
      
      // delete local arrays
      delete [] lHandles;
      delete [] lTypes;
      delete [] lVals;
      for(i=0; i<lNumTypes; i++)
	{
	  delete [] lLabels[i];
	}
      delete [] lLabels;
      
    } //if (lNumTypes>0)
    
  } //try

  catch (SteererException StEx)
  {
    if (lCleanUpFlag)
    {
      delete [] lHandles;
      delete [] lTypes;
      delete [] lVals;
      for(int i=0; i<lNumTypes; i++)
	{
	  delete [] lLabels[i];
	}
      delete [] lLabels;
    }

    // rethrow to Application::processNextMessage
    throw StEx;
  }

    
} // ::updateIOTypes


void 
ControlForm::disableAll(const bool aUnRegister)
{
  // clear and disable all parts of form as application has detached
 
  mSteerParamTable->clearAndDisableForDetach(aUnRegister);
  mMonParamTable->clearAndDisableForDetach(aUnRegister);
  mIOTypeSampleTable->clearAndDisableForDetach();
  mIOTypeChkPtTable->clearAndDisableForDetach();

  disableButtons();
  disableAppCmdButtons();

}

void
ControlForm::disableIOCmdButtons()
{
  // MR: RestartChkPtButton Local vs Grid
  if (mApplication->isLocal())
    mRestartChkPtButton->setEnabled(FALSE);
    
  mSndChkPtButton->setEnabled(FALSE); 
  mConsumeDataButton->setEnabled(FALSE);
  mEmitDataButton->setEnabled(FALSE);
}

void
ControlForm::enableIOCmdButtons()
{
  if (mIOTypeChkPtTable->getNumIOTypes() > 0)
  {
    mSndChkPtButton->setEnabled(TRUE);
    // MR: RestartChkPtButton Local vs Grid
    if (mApplication->isLocal())
      mRestartChkPtButton->setEnabled(TRUE);
  }

  if (mIOTypeSampleTable->getNumIOTypes() > 0  ||
      mIOTypeChkPtTable->getNumIOTypes() > 0)
  {
    mConsumeDataButton->setEnabled(TRUE);
    mEmitDataButton->setEnabled(TRUE);
  }
}

void 
ControlForm::disableButtons()
{
  mEmitButton->setEnabled(FALSE); 
  mSetSampleFreqButton->setEnabled(FALSE);
  // MR: RestartChkPtButton Local vs Grid
  if (mApplication->isLocal())
    mRestartChkPtButton->setEnabled(FALSE);
  else
    mGridRestartChkPtButton->setEnabled(FALSE);

  mSndChkPtButton->setEnabled(FALSE); 
  mSetChkPtFreqButton->setEnabled(FALSE); 
  mEmitAllValuesButton->setEnabled(FALSE); 
  mConsumeDataButton->setEnabled(FALSE);
  mEmitDataButton->setEnabled(FALSE);
}

void 
ControlForm::disableAppCmdButtons()
{
  mDetachButton->setEnabled(FALSE);
  mStopButton->setEnabled(FALSE);
  mPauseButton->setEnabled(FALSE);
}

void 
ControlForm::setEnabledDetach(const bool aEnable)
{
  mDetachButton->setEnabled(aEnable);
}

void  
ControlForm::setEnabledStop(const bool aEnable)
{
  mStopButton->setEnabled(aEnable);
}

void  
ControlForm::setEnabledPause(const bool aEnable)
{
  mPauseButton->setEnabled(aEnable);
}

void  
ControlForm::setEnabledClose(const bool aEnable)
{
  mCloseButton->setEnabled(aEnable);
}

void 
ControlForm::enableParamButtonsSlot()
{
  mEmitButton->setEnabled(TRUE);
}

void 
ControlForm::enableSampleButtonsSlot()
{
  mSetSampleFreqButton->setEnabled(TRUE);

  mEmitAllValuesButton->setEnabled(TRUE);

  mConsumeDataButton->setEnabled(TRUE);
  mEmitDataButton->setEnabled(TRUE);

}

void 
ControlForm::enableChkPtButtonsSlot()
{
  // MR: RestartChkPtButton Local vs Grid
  if (mApplication->isLocal())
    mRestartChkPtButton->setEnabled(TRUE);
  else
    mGridRestartChkPtButton->setEnabled(TRUE);

  mSndChkPtButton->setEnabled(TRUE); 
  mSetChkPtFreqButton->setEnabled(TRUE);

  mEmitAllValuesButton->setEnabled(TRUE);
  mConsumeDataButton->setEnabled(TRUE);
  mEmitDataButton->setEnabled(TRUE);

}

// MR:
void ControlForm::setRestartButtonStateSlot(const bool aEnable){
  if (mRestartChkPtButton != NULL)
    // MR: RestartChkPtButton Local vs Grid
    if (mApplication->isLocal())
      mRestartChkPtButton->setEnabled(aEnable);
    else
      mGridRestartChkPtButton->setEnabled(aEnable);
}

// MR:
void ControlForm::setCreateButtonStateSlot(const bool aEnable){
  if (mSndChkPtButton != NULL)
    mSndChkPtButton->setEnabled(aEnable);
}

// MR:
void ControlForm::setConsumeButtonStateSlot(const bool aEnable){
  if (mConsumeDataButton != NULL)
    mConsumeDataButton->setEnabled(aEnable);
}

// MR:
void ControlForm::setEmitButtonStateSlot(const bool aEnable){
  if (mEmitDataButton != NULL)
    mEmitDataButton->setEnabled(aEnable);
}

void 
ControlForm::emitAllValuesSlot()
{
  // emit both parameter values and iotype frequencies
  int lReGStatus = REG_FAILURE;
  try
  {

    // set the values in library
    int lCount = mSteerParamTable->setNewParamValuesInLib();
    lCount += mIOTypeSampleTable->setNewFreqValuesInLib();
    lCount += mIOTypeChkPtTable->setNewFreqValuesInLib();
    
    if (lCount > 0)
    {
      // call ReG library function to "emit" values to steered application
      mMutexPtr->lock();
      lReGStatus = Emit_control(mSimHandle,		//ReG library
				0,
				NULL,
				NULL);
      mMutexPtr->unlock();

      if (lReGStatus != REG_SUCCESS)
	THROWEXCEPTION("Emit_contol");
      
    }
    
  } //try

  catch (SteererException StEx)
  {
    StEx.print();
    emit detachFromApplicationForErrorSignal();
    QMessageBox::warning(0, "Steerer Error", 
			 "Internal library error - detaching from application",
			 QMessageBox::Ok,
			 QMessageBox::NoButton, 
			 QMessageBox::NoButton);
  }
}

/**
  * getter method for a ptr to the Application that this controlform
  * is controlling.
  */
Application *ControlForm::application()
{
  return mApplication;
}

/** 
 * Method to show or hide the checkpoint table and associated label
 * and buttons.
 */
void ControlForm::hideChkPtTable(bool flag){

  if(mSetChkPtFreqButton) mSetChkPtFreqButton->setHidden(flag);
  if(mIOTypeChkPtTable)mIOTypeChkPtTable->setHidden(flag);
  if(mSndChkPtButton)mSndChkPtButton->setHidden(flag);
  if (mApplication && mApplication->isLocal()){
    mRestartChkPtButton->setHidden(flag);
  }
  if(mChkTableLabel)mChkTableLabel->setHidden(flag);

  this->update();
}

/**
 * Method to show or hide the IOTypes table and associated label
 * and buttons.
 */
void ControlForm::hideIOTable(bool flag){

  if(mEmitDataButton)mEmitDataButton->setHidden(flag);
  if(mSetSampleFreqButton)mSetSampleFreqButton->setHidden(flag);
  if(mConsumeDataButton)mConsumeDataButton->setHidden(flag);
  if(mIOTypeSampleTable)mIOTypeSampleTable->setHidden(flag);
  if(mIOTableLabel)mIOTableLabel->setHidden(flag);

  this->update();
}

/**
 * Method to show or hide the steered params table and associated
 * label and buttons.
 */
void ControlForm::hideSteerTable(bool flag){

  if(mSteerParamTable)mSteerParamTable->setHidden(flag);
  if(mEmitButton)mEmitButton->setHidden(flag);
  if(mSteerTableLabel)mSteerTableLabel->setHidden(flag);

  this->update();
}

/**
 * Method to show or hide the monitored params table and associated
 * label and buttons.
 */
void ControlForm::hideMonTable(bool flag){

  if(mMonParamTable)mMonParamTable->setHidden(flag);
  if(mMonTableLabel)mMonTableLabel->setHidden(flag);

  this->update();
}

//--------------------------------------------------------------------

ParameterTable *ControlForm::getMonParamTable()
{
  return mMonParamTable;
}

//--------------------------------------------------------------------

SteeredParameterTable *ControlForm::getSteeredParamTable()
{
  return mSteerParamTable;
}

//--------------------------------------------------------------------

void ControlForm::setPauseButtonLabel(QString aLabel){
  if(mPauseButton){
    mPauseButton->setText(aLabel);
  }
}

//--------------------------------------------------------------------
void ControlForm::newHistoryPlot(Parameter *xParamPtr, Parameter *yParamPtr, 
				 QString xLabel, QString yLabel){

  HistoryPlot *lQwtPlot;

  // Call our whizzo graphing method to draw the graph
  // need to keep a reference to the plotter so that it's cancelled when 
  // we quit the main window
  lQwtPlot = new HistoryPlot(xParamPtr->mParamHist, 
			     yParamPtr->mParamHist,
			     xLabel.latin1(),
			     yLabel.latin1(), 
			     xParamPtr->getId(), yParamPtr->getId(),
			     this->application()->name());
  mHistoryPlotList.append(lQwtPlot);
  lQwtPlot->show();

  // And make the connection to ensure that the graph updates
  connect(this, SIGNAL(paramUpdateSignal(ParameterHistory *, const int)), 
	  lQwtPlot, SLOT(updateSlot(ParameterHistory*, const int)));
 
  // Make connection so that the graph can tell us when it has been closed
  connect(lQwtPlot, SIGNAL(plotClosedSignal(HistoryPlot*)), this, 
	  SLOT(plotClosedSlot(HistoryPlot*)));


  connect(lQwtPlot, SIGNAL(plotSelectedSignal(HistoryPlot *)),
	  this, SLOT(plotSelectedSlot(HistoryPlot *)));
}

//----------------------------------------------------------------
void ControlForm::plotClosedSlot(HistoryPlot *ptr){

  // Plot closed so remove from list (Auto delete means Qt will then destroy 
  // the associated HistoryPlot object)
  mHistoryPlotList.removeRef(ptr);
}

//----------------------------------------------------------------
void ControlForm::plotSelectedSlot(HistoryPlot *plot){

  if(mUserChoosePlotMode && mParamToAdd){
    plot->addPlot(mParamToAdd->mParamHist,
		  mParamToAdd->getLabel(), 
		  mParamToAdd->getId());
    mUserChoosePlotMode = false;
    mParamToAdd = NULL;
  }
}
