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

ControlForm::ControlForm(QWidget *aParent, const char *aName, int aSimHandle, Application *aApplication)
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
    mPauseButton(kNULL), mResumeButton(kNULL), mConsumeDataButton(kNULL), mEmitDataButton(kNULL)
{ 
  DBGCON("ControlForm");

  // MR: keep a reference to the Application class
  mApplication = aApplication;
  
  // create widget which holds all steering data (some dynamic) 
  // for one steered application this consists of four tables - 
  // one each for monitored parameters, steered parameters,
  // samples iotypes and checkpoint iotypes, plus buttons 
  // associated with the tables

  QVBoxLayout *lEditLayout = new QVBoxLayout(this, 0, -1, "editlayout");

  // MR: If we're doing a grid session, have the restart button up
  //     in the top control panel, as we're going to restart from
  //     a GSH only...
  QHBoxLayout *lCmdGrpLayout;
  lCmdGrpLayout = new QHBoxLayout(-1, "Commands");

  // set up main command buttons
  // we use "Tell All" button sizeHint to size all buttons to this size

  // first of all create all the buttons so that we can choose
  // which to use for the size hint without having to reorder
  // any code
  mPauseButton = new QPushButton( "Pause", this, "pause" );
  mResumeButton = new QPushButton( "Resume", this, "resume" );
  mDetachButton = new QPushButton( "Detach", this, "detach" );
  mCloseButton = new QPushButton( "Close", this, "close" );
  mEmitAllValuesButton = new QPushButton("Tell All", this, "tellallvals");
  mStopButton = new QPushButton( "Stop", this, "stop" );

  // MR:
  if (!mApplication->isLocal())
    mGridRestartChkPtButton = new QPushButton("Restart", this, "restart");
  
  // then set up each button individually, never setting the maximum size
  // so that each button stretches to fill the standard gap
  
  mPauseButton->setMinimumSize(mEmitAllValuesButton->sizeHint());
  //mPauseButton->setMaximumSize(mPauseButton->sizeHint());
  QToolTip::add(mPauseButton, "Tell the attached application to pause");
  connect( mPauseButton, SIGNAL( clicked() ), aApplication, 
	   SLOT( emitPauseCmdSlot() ));
  lCmdGrpLayout->addWidget(mPauseButton);

  mResumeButton->setMinimumSize(mEmitAllValuesButton->sizeHint());
  //mResumeButton->setMaximumSize(mResumeButton->sizeHint());
  QToolTip::add(mResumeButton, "Tell the attached application to resume");
  connect( mResumeButton, SIGNAL( clicked() ), aApplication, 
	   SLOT( emitResumeCmdSlot() ));
  lCmdGrpLayout->addWidget(mResumeButton);

  mDetachButton->setMinimumSize(mEmitAllValuesButton->sizeHint());
  //mDetachButton->setMaximumSize(mDetachButton->sizeHint());
  QToolTip::add(mDetachButton, "Tell the attached application to detach");
  connect( mDetachButton, SIGNAL( clicked() ), aApplication, 
	   SLOT( emitDetachCmdSlot() ));
  lCmdGrpLayout->addWidget(mDetachButton);

  mCloseButton->setMinimumSize(mEmitAllValuesButton->sizeHint());
  //mCloseButton->setMaximumSize(mCloseButton->sizeHint());
  QToolTip::add( mCloseButton, "Close form to allow another attach");
  connect(mCloseButton, SIGNAL( clicked() ), aApplication, 
	  SLOT(closeApplicationSlot()) );
  lCmdGrpLayout->addWidget(mCloseButton);

  mEmitAllValuesButton->setMinimumSize(mEmitAllValuesButton->sizeHint());
  //mEmitAllValuesButton->setMaximumSize(mEmitAllValuesButton->sizeHint());
  QToolTip::add(mEmitAllValuesButton, "Tell application all new parameter and frequency values");
  connect(mEmitAllValuesButton, SIGNAL(clicked()), this, 
	  SLOT(emitAllValuesSlot()));
  lCmdGrpLayout->addWidget(mEmitAllValuesButton);

  mStopButton->setMinimumSize(mEmitAllValuesButton->sizeHint());
  //mStopButton->setMaximumSize(mStopButton->sizeHint());
  QToolTip::add(mStopButton, "Tell the attached application to stop");
  connect( mStopButton, SIGNAL( clicked() ), aApplication, 
	   SLOT( emitStopCmdSlot() ));
  lCmdGrpLayout->addWidget(mStopButton);

  // MR:
  if (!mApplication->isLocal()){
    mGridRestartChkPtButton->setMinimumSize(mEmitAllValuesButton->sizeHint());
    //mStopButton->setMaximumSize(mStopButton->sizeHint());
    QToolTip::add(mGridRestartChkPtButton, "Restart via a GSH");
    connect( mGridRestartChkPtButton, SIGNAL( clicked() ), aApplication, 
	     SLOT( emitGridRestartCmdSlot() ));
    lCmdGrpLayout->addWidget(mGridRestartChkPtButton);
  }

  // set up table for monitored parameters
  QVBoxLayout *lMonLayout = new QVBoxLayout(-1, "montablayout");
  mMonParamTable = new ParameterTable(this, "monparamtable", aSimHandle);
  mMonParamTable->initTable();

  // ARP - set-up all tables first so we know their widths
  mSteerParamTable = new SteeredParameterTable(this,"steerparamtable", 
					       mMonParamTable, aSimHandle);
  mSteerParamTable->initTable();

  mIOTypeSampleTable = new IOTypeTable(this,"sampleparamtable",aSimHandle);
  mIOTypeSampleTable->initTable();

  mIOTypeChkPtTable = new IOTypeTable(this,"chkptparamtable", aSimHandle, 
				      true);
  mIOTypeChkPtTable->initTable();

  // Set the minimum table size. Don't bother with the maximum
  // size since we want to be able to stretch the window
  // ARPDBG
  //int lTableWidth = 300;
  //mIOTypeSampleTable->setMinimumWidth(lTableWidth);
  //mSteerParamTable->setMinimumWidth(lTableWidth);
  //mMonParamTable->setMinimumWidth(lTableWidth);

  // Once table objects created, we can get on with creating layouts...
  lMonLayout->addWidget(mMonParamTable);

  // layout status, cmd buttons and paramtable
  QVBoxLayout *lTopLeftLayout = new QVBoxLayout(-1, "topleftlayout");
  QHBoxLayout *lTopLayout = new QHBoxLayout(-1, "toplayout");

  //  lTopLeftLayout->addWidget(lCmdGrpBox);
  lTopLeftLayout->addLayout(lCmdGrpLayout);
  lTopLeftLayout->addWidget(new TableLabel("Monitored Parameters", this));
  lTopLayout->addLayout(lTopLeftLayout);

  // set up table and buttons for steered parameters
  QHBoxLayout *lSteerLayout = new QHBoxLayout(-1, "steertablayout");
  QVBoxLayout *lSteerButtonLayout = new QVBoxLayout(-1, "steerparambuttons");
  connect(mSteerParamTable, SIGNAL(detachFromApplicationForErrorSignal()), 
	  aApplication, SLOT(detachFromApplicationForErrorSlot()));


  mEmitButton = new QPushButton( "Tell", this, "tellvalue" );
  //  mEmitButton->setMinimumSize(mEmitButton->sizeHint());
  mEmitButton->setMaximumSize(mEmitButton->sizeHint());
  QToolTip::add(mEmitButton, "Tell application new parameter values");
  connect( mEmitButton, SIGNAL( clicked() ), mSteerParamTable, 
	   SLOT( emitValuesSlot() ) );
               
  // layout steered parameters
  lSteerButtonLayout->addItem(new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
  lSteerButtonLayout->addWidget(mEmitButton);

  // MR
  // This table needs to be in the same layout as the "Steered Parameters" 
  // tag and Monitored Parameters table
  lMonLayout->addWidget(new TableLabel("Steered Parameters", this));
  lMonLayout->addWidget(mSteerParamTable);
  lSteerLayout->addLayout(lMonLayout);
  lSteerLayout->addLayout(lSteerButtonLayout);

  // table and buttons for sample iotypes
  QHBoxLayout *lSampleLayout = new QHBoxLayout(-1, "sampletablayout");
  QVBoxLayout *lSampleButtonLayout = new QVBoxLayout(-1, "samplebuttons");
  connect(mIOTypeSampleTable, SIGNAL(detachFromApplicationForErrorSignal()), 
	  aApplication, SLOT(detachFromApplicationForErrorSlot()));

  mSetSampleFreqButton = new QPushButton( "Tell Freq's", this, 
					  "TellFreq" );
  mSetSampleFreqButton->setMinimumSize(mSetSampleFreqButton->sizeHint());
  mSetSampleFreqButton->setMaximumSize(mSetSampleFreqButton->sizeHint());
  QToolTip::add(mSetSampleFreqButton, "Tell application new frequency values for data IO");
  connect(mSetSampleFreqButton, SIGNAL( clicked() ), 
	  mIOTypeSampleTable, SLOT( emitValuesSlot()));

  mConsumeDataButton = new QPushButton( "Consume", this, "ConsumeData" );
  mConsumeDataButton->setMinimumSize(mSetSampleFreqButton->sizeHint());
  mConsumeDataButton->setMaximumSize(mConsumeDataButton->sizeHint());
  QToolTip::add(mConsumeDataButton, "Tell application to Consume data");
  connect( mConsumeDataButton, SIGNAL( clicked() ), mIOTypeSampleTable, 
	   SLOT( consumeButtonPressedSlot()));

  mEmitDataButton = new QPushButton( "Emit", this, "EmitData" );
  mEmitDataButton->setMinimumSize(mSetSampleFreqButton->sizeHint());
  mEmitDataButton->setMaximumSize(mEmitDataButton->sizeHint());
  QToolTip::add(mEmitDataButton, "Tell application to Emit data");
  connect( mEmitDataButton, SIGNAL( clicked() ), mIOTypeSampleTable, 
	   SLOT( emitButtonPressedSlot()));

    
  lSampleButtonLayout->addItem(new QSpacerItem( 0, 0, QSizePolicy::Minimum, 
						QSizePolicy::Expanding));
  lSampleButtonLayout->addWidget(mConsumeDataButton);
  lSampleButtonLayout->addWidget(mEmitDataButton);
  lSampleButtonLayout->addWidget(mSetSampleFreqButton);
  lSampleLayout->addWidget(mIOTypeSampleTable);
  lSampleLayout->addLayout(lSampleButtonLayout);


  // table and buttons for checkpoint iotypes
  QHBoxLayout *lChkPtLayout = new QHBoxLayout(-1, "chktablayout");
  QVBoxLayout *lChkPtButtonLayout = new QVBoxLayout(-1, "chkptbuttons");
  connect(mIOTypeChkPtTable, 
	  SIGNAL(detachFromApplicationForErrorSignal()), 
	  aApplication, SLOT(detachFromApplicationForErrorSlot()));

  mSetChkPtFreqButton = new QPushButton( "Tell Freq's", this, 
					 "tellfreq" );
  mSetChkPtFreqButton->setMinimumSize(mSetChkPtFreqButton->sizeHint());
  mSetChkPtFreqButton->setMaximumSize(mSetChkPtFreqButton->sizeHint());
  QToolTip::add(mSetChkPtFreqButton, "Tell application new frequency values for checkpoint iotypes");  
  connect(mSetChkPtFreqButton, SIGNAL( clicked() ), 
	  mIOTypeChkPtTable, SLOT( emitValuesSlot()));

  // MR: we only want to add the RestartChkPtButton to the ChkPtButtonLayout
  //     if we're in local attach mode. If we're in grid attach mode then
  //     we'll place the button on the top control panel, and use it to get
  //     a GSH from an input dialog box instead.
  if (mApplication->isLocal()){
    mRestartChkPtButton = new QPushButton( "Restart", this, "restartchkpt" );
    mRestartChkPtButton->setMinimumSize(mSetChkPtFreqButton->sizeHint());
    mRestartChkPtButton->setMaximumSize(mRestartChkPtButton->sizeHint());
    QToolTip::add(mRestartChkPtButton, "Tell application to restart using requested checkpoint");
    connect( mRestartChkPtButton, SIGNAL(clicked()), mIOTypeChkPtTable, 
	     SLOT(restartButtonPressedSlot()));
  }

  mSndChkPtButton = new QPushButton( "Create", this, "sndchkpt" );
  mSndChkPtButton->setMinimumSize(mSetChkPtFreqButton->sizeHint());
  mSndChkPtButton->setMaximumSize(mSndChkPtButton->sizeHint());
  QToolTip::add(mSndChkPtButton, "Tell application to create requested checkpoint iotypes");
  connect( mSndChkPtButton, SIGNAL( clicked() ), mIOTypeChkPtTable, 
	   SLOT( createButtonPressedSlot()));

 

  lChkPtButtonLayout->addItem(new QSpacerItem( 0, 0, QSizePolicy::Minimum, 
					       QSizePolicy::Expanding));
  // MR: RestartChkPtButton Local vs Grid
  if (mApplication->isLocal())
    lChkPtButtonLayout->addWidget(mRestartChkPtButton);

  lChkPtButtonLayout->addWidget(mSndChkPtButton);
  lChkPtButtonLayout->addWidget(mSetChkPtFreqButton);
  lChkPtLayout->addWidget(mIOTypeChkPtTable);
  lChkPtLayout->addLayout(lChkPtButtonLayout);

  mEmitButton->setMinimumSize(mSetChkPtFreqButton->sizeHint());
  
  connect(this, SIGNAL(detachFromApplicationForErrorSignal()), 
	  aApplication, SLOT(detachFromApplicationForErrorSlot()));


  // the overall layout
  lEditLayout->addLayout(lTopLayout);
  lEditLayout->addLayout(lSteerLayout);
  QVBoxLayout *lExtraLayout = new QVBoxLayout(-1, "bottom tables");
  lExtraLayout->addWidget(new TableLabel("Data IO", this));
  lExtraLayout->addLayout(lSampleLayout);

  lExtraLayout->addWidget(new TableLabel("CheckPoint Types", this));
  lExtraLayout->addLayout(lChkPtLayout);

  lEditLayout->addLayout(lExtraLayout);
 
  // disable buttons - they are enabled when tables have some data
  disableButtons();

  // disable app command buttons - enabled when know what commands are supported
  disableAppCmdButtons();

  // Close button only becomes enabled when detach from application
  mCloseButton->setEnabled(FALSE);
  if(mEmitAllValuesButton->sizeHint().isValid()){
    cout << "Our size hint is valid ******" << endl;
  }
  else{
    cout << "Our size hint is NOT valid +++++++" << endl;
  }
  mPauseButton->setMinimumSize(mEmitAllValuesButton->minimumSizeHint());
  mResumeButton->setMinimumSize(mEmitAllValuesButton->minimumSizeHint());
  mDetachButton->setMinimumSize(mEmitAllValuesButton->minimumSizeHint());
  mCloseButton->setMinimumSize(mEmitAllValuesButton->minimumSizeHint());
  mEmitAllValuesButton->setMinimumSize(mEmitAllValuesButton->minimumSizeHint());
  mStopButton->setMinimumSize(mEmitAllValuesButton->minimumSizeHint());
  mGridRestartChkPtButton->setMinimumSize(mEmitAllValuesButton->minimumSizeHint());
  mEmitButton->setMinimumSize(mSetSampleFreqButton->minimumSizeHint());
  mSetSampleFreqButton->setMinimumSize(mSetSampleFreqButton->minimumSizeHint());
  mConsumeDataButton->setMinimumSize(mSetSampleFreqButton->minimumSizeHint());
  mEmitDataButton->setMinimumSize(mSetSampleFreqButton->minimumSizeHint());
  mSetChkPtFreqButton->setMinimumSize(mSetSampleFreqButton->minimumSizeHint());


} 

ControlForm::~ControlForm()
{
  DBGDST("ControlForm");
///SMR
///SMR	delete mMonParamTable;
///SMR	mMonParamTable = kNULL;
///SMR
///SMR	delete mSteerParamTable;
///SMR	mSteerParamTable = kNULL;
///SMR
///SMR	delete mIOTypeSampleTable;
///SMR	mIOTypeSampleTable = kNULL;
///SMR
///SMR	delete mIOTypeChkPtTable;
///SMR	mIOTypeChkPtTable = kNULL;
///SMR	 
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
      if(Get_param_number(mSimHandle, aSteeredFlag, &lNumParams) != REG_SUCCESS)  //ReG library
	THROWEXCEPTION("Get_param_number");
    
      DBGMSG1("Number Params: = ", lNumParams);
    
      if (lNumParams > 0)
	{
	  // flag that cleanup required - don't like this but checking for kNULL didn't work
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
	  if (Get_param_values(mSimHandle,		//ReG library
			       aSteeredFlag,
			       lNumParams,
			       lParamDetails) == REG_SUCCESS)
	    {

	      for (int i=0; i<lNumParams; i++)
		{
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
		} //for lNumParams
      
	    } // if Get_param_values
	  else
	    {
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

    if (aChkPtType)
      lStatus = Get_chktype_number(mSimHandle, &lNumTypes);	//ReG library
    else
      lStatus = Get_iotype_number(mSimHandle, &lNumTypes);	//ReG library    

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
      
      if (aChkPtType)
	
	lStatus = Get_chktypes(mSimHandle,			//ReG library
			       lNumTypes,
			       lHandles,
			       lLabels,
			       lTypes,
			       lVals);
      else
	lStatus = Get_iotypes(mSimHandle,			//ReG library
			      lNumTypes,
			      lHandles,
			      lLabels,
			      lTypes,
			      lVals);

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
  mResumeButton->setEnabled(FALSE);
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
ControlForm::setEnabledResume(const bool aEnable)
{
  mResumeButton->setEnabled(aEnable);
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
      qApp->lock();
      lReGStatus = Emit_control(mSimHandle,		//ReG library
				0,
				NULL,
				NULL);
      qApp->unlock();

      if (lReGStatus != REG_SUCCESS)
	THROWEXCEPTION("Emit_contol");
      
    }
    
  } //try

  catch (SteererException StEx)
  {
    StEx.print();
    emit detachFromApplicationForErrorSignal();
    QMessageBox::warning(0, "Steerer Error", "Internal library error - detaching from application",
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
