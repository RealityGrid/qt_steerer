/*----------------------------------------------------------------------------
    ControlForm class for QT steerer GUI.

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
#include "ReG_Steer_Steerside.h"

#include "application.h"
#include "controlform.h"
#include "parametertable.h"
#include "iotypetable.h"
#include "utility.h"
#include "exception.h"

#include <qapplication.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h> 
#include <qvbox.h>

ControlForm::ControlForm(QWidget *aParent, const char *aName, int aSimHandle, Application *aApplication)
  : QWidget(aParent, aName), mSimHandle(aSimHandle), mStatusLabel(kNULL), mEmitButton(kNULL), 
    mSndSampleButton(kNULL), mSetSampleFreqButton(kNULL), mRestartChkPtButton(kNULL), 
    mSndChkPtButton(kNULL), mSetChkPtFreqButton(kNULL), mEmitAllValuesButton(kNULL),
    mEmitAllIOCommandsButton(kNULL), mEmitAllButton(kNULL), mMonParamTable(kNULL),
    mSteerParamTable(kNULL), mIOTypeSampleTable(kNULL), mIOTypeChkPtTable(kNULL)
{ 
  DBGCON("ControlForm");
  // create widget which holds all steering data (some dynamic) for one steered application
  // this consists of four tables - one each for monitored parameters, steered parameters,
  // samples iotypes and checkpoint iotypes, plus buttons associated with the tables

  QVBoxLayout *lEditLayout = new QVBoxLayout(this, 6, 6, "editlayout");
  QHBoxLayout *lStatusLayout = new QHBoxLayout(6, "statuslayout");
  QHBoxLayout *lBottomButtonLayout = new QHBoxLayout(6, "bottombuttonlayout");

  mStatusLabel = new TableLabel("Attached to application", this);
  lStatusLayout->addWidget(new TableLabel("Steering Status: ", this));
  lStatusLayout->addWidget(mStatusLabel);
  lStatusLayout->addItem(new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum ));

  // set up table for monitored parameters
  QHBoxLayout *lMonLayout = new QHBoxLayout(6, "montablayout");
  mMonParamTable = new ParameterTable(this, "monparamtable", aSimHandle);
  mMonParamTable->initTable();
  lMonLayout->addWidget(mMonParamTable);
  lMonLayout->addSpacing(80);

  // set up table and buttons for steered parameters
  QHBoxLayout *lSteerLayout = new QHBoxLayout(6, "steertablayout");
  QVBoxLayout *lSteerButtonLayout = new QVBoxLayout(6, "steerparambuttons");
  mSteerParamTable = new SteeredParameterTable(this,"steerparamtable", aSimHandle);
  mSteerParamTable->initTable();
  connect(mSteerParamTable, SIGNAL(detachFromApplicationForErrorSignal()), 
	  aApplication, SLOT(detachFromApplicationForErrorSlot()));


  mEmitButton = new QPushButton( "Tell", this, "tellvalue" );
  //  mEmitButton->setMinimumSize(mEmitButton->sizeHint());
  mEmitButton->setMaximumSize(mEmitButton->sizeHint());
  QToolTip::add(mEmitButton, "Tell application new parameter values");
  connect( mEmitButton, SIGNAL( clicked() ), mSteerParamTable, SLOT( emitValuesSlot() ) );
    
  // layout steered parameters
  lSteerButtonLayout->addItem(new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
  lSteerButtonLayout->addWidget(mEmitButton);
  lSteerLayout->addWidget(mSteerParamTable);  
  lSteerLayout->addLayout(lSteerButtonLayout);
  //  lSteerLayout->addSpacing(30);

  // table and buttons for sample iotypes
  QHBoxLayout *lSampleLayout = new QHBoxLayout(6, "sampletablayout");
  QVBoxLayout *lSampleButtonLayout = new QVBoxLayout(6, "samplebuttons");
  mIOTypeSampleTable = new IOTypeTable(this,"sampleparamtable", aSimHandle);
  mIOTypeSampleTable->initTable();
  connect(mIOTypeSampleTable, SIGNAL(detachFromApplicationForErrorSignal()), 
	  aApplication, SLOT(detachFromApplicationForErrorSlot()));

  mSetSampleFreqButton = new QPushButton( "Tell Freq's", this, "TellFreq" );
  mSetSampleFreqButton->setMinimumSize(mSetSampleFreqButton->sizeHint());
  mSetSampleFreqButton->setMaximumSize(mSetSampleFreqButton->sizeHint());
  QToolTip::add(mSetSampleFreqButton, "Tell application new frequency values for sample iotypes");
  connect(mSetSampleFreqButton, SIGNAL( clicked() ), mIOTypeSampleTable, SLOT( emitValuesSlot()));

  mSndSampleButton = new QPushButton( "Tell Req's", this, "SndSample" );
  mSndSampleButton->setMinimumSize(mSetSampleFreqButton->sizeHint());
  mSndSampleButton->setMaximumSize(mSndSampleButton->sizeHint());
  QToolTip::add(mSndSampleButton, "Tell application to emit/consume requested sample iotypes");
  connect(mSndSampleButton, SIGNAL( clicked() ), mIOTypeSampleTable, SLOT( emitCommandsSlot()));


  lSampleButtonLayout->addItem(new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
  lSampleButtonLayout->addWidget(mSndSampleButton);
  lSampleButtonLayout->addWidget(mSetSampleFreqButton);
  lSampleLayout->addWidget(mIOTypeSampleTable);
  lSampleLayout->addLayout(lSampleButtonLayout);


  // table and buttons for checkpoint iotypes
  QHBoxLayout *lChkPtLayout = new QHBoxLayout(6, "chktablayout");
  QVBoxLayout *lChkPtButtonLayout = new QVBoxLayout(6, "chkptbuttons");
  mIOTypeChkPtTable = new IOTypeTable(this,"chkptparamtable", aSimHandle, true);
  mIOTypeChkPtTable->initTable();
  connect(mIOTypeChkPtTable, SIGNAL(detachFromApplicationForErrorSignal()), 
	  aApplication, SLOT(detachFromApplicationForErrorSlot()));

  mSetChkPtFreqButton = new QPushButton( "Tell Freq's", this, "tellfreq" );
  mSetChkPtFreqButton->setMinimumSize(mSetChkPtFreqButton->sizeHint());
  mSetChkPtFreqButton->setMaximumSize(mSetChkPtFreqButton->sizeHint());
  QToolTip::add(mSetChkPtFreqButton, "Tell application new frequency values for checkpoint iotypes");  
  connect(mSetChkPtFreqButton, SIGNAL( clicked() ), mIOTypeChkPtTable, SLOT( emitValuesSlot()));

  mRestartChkPtButton = new QPushButton( "Restart", this, "restartchkpt" );
  mRestartChkPtButton->setMinimumSize(mSetChkPtFreqButton->sizeHint());
  mRestartChkPtButton->setMaximumSize(mRestartChkPtButton->sizeHint());
  QToolTip::add(mRestartChkPtButton, "Tell application to restart using requested checkpoint");
  connect( mRestartChkPtButton, SIGNAL( clicked() ), mIOTypeChkPtTable, SLOT( emitRestartSlot()));

  mSndChkPtButton = new QPushButton( "Create", this, "sndchkpt" );
  mSndChkPtButton->setMinimumSize(mSetChkPtFreqButton->sizeHint());
  mSndChkPtButton->setMaximumSize(mSndChkPtButton->sizeHint());
  QToolTip::add(mSndChkPtButton, "Tell application to create requested checkpoint iotypes");
  connect( mSndChkPtButton, SIGNAL( clicked() ), mIOTypeChkPtTable, SLOT( emitCommandsSlot()));
 

  lChkPtButtonLayout->addItem(new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
  lChkPtButtonLayout->addWidget(mRestartChkPtButton);
  lChkPtButtonLayout->addWidget(mSndChkPtButton);
  lChkPtButtonLayout->addWidget(mSetChkPtFreqButton);
  lChkPtLayout->addWidget(mIOTypeChkPtTable);
  lChkPtLayout->addLayout(lChkPtButtonLayout);

  mEmitButton->setMinimumSize(mSetChkPtFreqButton->sizeHint());
  

  // buttons for combined sends to application 

  mEmitAllIOCommandsButton = new QPushButton("Tell All Requests", this, "sendallrequests");
  mEmitAllIOCommandsButton->setMinimumSize(mEmitAllIOCommandsButton->sizeHint());
  mEmitAllIOCommandsButton->setMaximumSize(mEmitAllIOCommandsButton->sizeHint());
  QToolTip::add(mEmitAllIOCommandsButton, "Tell application all emit/consume requests");
  connect(mEmitAllIOCommandsButton, SIGNAL(clicked()), this, SLOT(emitAllIOCommandsSlot()));


  mEmitAllValuesButton = new QPushButton("Tell All Values", this, "tellallvals");
  mEmitAllValuesButton->setMinimumSize(mEmitAllIOCommandsButton->sizeHint());
  mEmitAllValuesButton->setMaximumSize(mEmitAllValuesButton->sizeHint());
  QToolTip::add(mEmitAllValuesButton, "Tell application all new parameter and frequency values");
  connect(mEmitAllValuesButton, SIGNAL(clicked()), this, SLOT(emitAllValuesSlot()));
  

  mEmitAllButton = new QPushButton("Tell All", this, "sendall");
  mEmitAllButton->setMinimumSize(mEmitAllIOCommandsButton->sizeHint());
  mEmitAllButton->setMaximumSize(mEmitAllButton->sizeHint());
  QToolTip::add(mEmitAllButton, "Tell application all new values and all emit/consume requests");
  connect(mEmitAllButton, SIGNAL(clicked()), this, SLOT(emitAllSlot()));

  lBottomButtonLayout->addWidget(mEmitAllValuesButton);
  lBottomButtonLayout->addWidget(mEmitAllIOCommandsButton);
  lBottomButtonLayout->addWidget(mEmitAllButton);
  lBottomButtonLayout->addItem(new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

  connect(this, SIGNAL(detachFromApplicationForErrorSignal()), 
	  aApplication, SLOT(detachFromApplicationForErrorSlot()));


  // the overall layout
  lEditLayout->addLayout(lStatusLayout);
  ///s  lEditLayout->addItem(new QSpacerItem( 0, 64, QSizePolicy::Minimum, QSizePolicy::Expanding));

  lEditLayout->addWidget(new TableLabel("Monitored Parameters", this));
  lEditLayout->addLayout(lMonLayout);
  ///s lEditLayout->addItem(new QSpacerItem( 0, 64, QSizePolicy::Minimum, QSizePolicy::Expanding));


  lEditLayout->addWidget(new TableLabel("Steered Parameters", this));
  lEditLayout->addLayout(lSteerLayout);
  ///s lEditLayout->addItem(new QSpacerItem( 0, 64, QSizePolicy::Minimum, QSizePolicy::Expanding));


  lEditLayout->addWidget(new TableLabel("Sample IOTypes", this));
  lEditLayout->addLayout(lSampleLayout);
  ///s lEditLayout->addItem(new QSpacerItem( 0, 64 QSizePolicy::Minimum, QSizePolicy::Expanding));


  lEditLayout->addWidget(new TableLabel("CheckPoint IOTypes", this));
  lEditLayout->addLayout(lChkPtLayout);
  ///s lEditLayout->addItem(new QSpacerItem( 0, 64, QSizePolicy::Minimum, QSizePolicy::Expanding));


  lEditLayout->addLayout(lBottomButtonLayout);

  // disable buttons - they are enabled when table has some data
  disableButtons();

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
  int *lHandles = kNULL;
  int *lTypes = kNULL;
  char  **lLabels = kNULL;
  char  **lVals = kNULL;
  bool lCleanUpFlag = false;


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
      // note that REG_MAX_STRING_LENGTH is max string length imposed by library
      lHandles = new int[lNumParams];
      lTypes = new int[lNumParams];
      lLabels = new char *[lNumParams];
      lVals = new char *[lNumParams];
      for(int i=0; i<lNumParams; i++)
      {
	lVals[i]   = new char[REG_MAX_STRING_LENGTH + 1];
	lLabels[i] = new char[REG_MAX_STRING_LENGTH + 1];
      }
      
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
			   lHandles,
			   lLabels,
			   lVals,
			   lTypes) == REG_SUCCESS)
      {
	
	for (int i=0; i<lNumParams; i++)
	{
	  //check if already exists - if so only update value
	  if (!(lTablePtr->updateRow(lHandles[i], lVals[i])))
	  {
	    // mus be new parameter so add it
	    lTablePtr->addRow(lHandles[i], lLabels[i], lVals[i], lTypes[i]);
	  } 
	} //for lNumParams
	
      } // if Get_param_values
      else
	THROWEXCEPTION("Get_param_values");
      
      // delete local arrays
      delete [] lHandles;
      delete [] lTypes;
      for(int i=0; i<lNumParams; i++)
	{
	  delete [] lVals[i];
	  delete [] lLabels[i];
	}
      delete [] lLabels;
      delete [] lVals;
      

    } // if lNumParams > 0

    // finally check for any parameters no longer present and flag 
    // as unregistered SMR XXX to do (ReG library not support unRegister yet)
  


  } //try

  catch (SteererException StEx)
  {
    // clean up
    if (lCleanUpFlag)
    {
      delete [] lHandles;
      delete [] lTypes;
      for(int i=0; i<lNumParams; i++)
      {
	delete [] lVals[i];
	delete [] lLabels[i];
      }
      delete [] lLabels;
      delete [] lVals;
    }

    // rethrow to Application::processNextMessage
    throw StEx;
  }
    
} // ::updateParameters


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
      for(int i=0; i<lNumTypes; i++)
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
	for (int i=0; i<lNumTypes; i++)
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
      for(int i=0; i<lNumTypes; i++)
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

}

void
ControlForm::disableIOCmdButtons()
{
  mRestartChkPtButton->setEnabled(FALSE);
  mSndSampleButton->setEnabled(FALSE); 
  mSndChkPtButton->setEnabled(FALSE); 
  mEmitAllIOCommandsButton->setEnabled(FALSE);
  mEmitAllButton->setEnabled(FALSE);
}
void
ControlForm::enableIOCmdButtons()
{
  // for each only do if are some rows populated
  if (mIOTypeSampleTable->getNumIOTypes() > 0)
    mSndSampleButton->setEnabled(TRUE); 
  if (mIOTypeChkPtTable->getNumIOTypes() > 0)
  {
    mSndChkPtButton->setEnabled(TRUE); 
    mRestartChkPtButton->setEnabled(TRUE);
  }

  if (mIOTypeSampleTable->getNumIOTypes() > 0  ||
      mIOTypeChkPtTable->getNumIOTypes() > 0)
  {
    mEmitAllIOCommandsButton->setEnabled(TRUE);
    mEmitAllButton->setEnabled(TRUE);
  }
}

void 
ControlForm::disableButtons()
{

  mEmitButton->setEnabled(FALSE); 
  mSndSampleButton->setEnabled(FALSE); 
  mSetSampleFreqButton->setEnabled(FALSE); 
  mRestartChkPtButton->setEnabled(FALSE);
  mSndChkPtButton->setEnabled(FALSE); 
  mSetChkPtFreqButton->setEnabled(FALSE); 
  mEmitAllValuesButton->setEnabled(FALSE); 
  mEmitAllIOCommandsButton->setEnabled(FALSE); 
  mEmitAllButton->setEnabled(FALSE); 

}


void 
ControlForm::setStatusLabel(const char *aStatusText)
{
  mStatusLabel->setText(aStatusText);
}


void 
ControlForm::enableParamButtonsSlot()
{
  mEmitButton->setEnabled(TRUE);

  mEmitAllValuesButton->setEnabled(TRUE);
}

void 
ControlForm::enableSampleButtonsSlot()
{
  mSndSampleButton->setEnabled(TRUE); 
  mSetSampleFreqButton->setEnabled(TRUE);

  mEmitAllValuesButton->setEnabled(TRUE);
  mEmitAllIOCommandsButton->setEnabled(TRUE);
  mEmitAllButton->setEnabled(TRUE);

}

void 
ControlForm::enableChkPtButtonsSlot()
{
  mRestartChkPtButton->setEnabled(TRUE);
  mSndChkPtButton->setEnabled(TRUE); 
  mSetChkPtFreqButton->setEnabled(TRUE);

  mEmitAllValuesButton->setEnabled(TRUE);
  mEmitAllIOCommandsButton->setEnabled(TRUE);
  mEmitAllButton->setEnabled(TRUE);

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

void 
ControlForm::emitAllIOCommandsSlot()
{
  emitAllIOCommands();
}


void 
ControlForm::emitAllIOCommands(const int aAdditionalCmd, bool aForceEmitFlag)
{
  int lReGStatus = REG_FAILURE;
  int lTotalCount = 0;
  bool lAddCmdFlag = false;

  if (aAdditionalCmd == REG_STR_DETACH)
    lAddCmdFlag = true;

  // emit sample and checkpoint requested commands
  
  int *lCommandArray = kNULL;
  char **lCmdParamArray = kNULL;

  try
  {
    int lSampleCount = mIOTypeSampleTable->getCommandRequestsCount();
    int lChkPtCount = mIOTypeChkPtTable->getCommandRequestsCount();
        
    lTotalCount = lSampleCount + lChkPtCount;

    if (lAddCmdFlag)
      lTotalCount++;

    
    // populate the array of commands and array of parameters for commands   
    lCommandArray = new int[lTotalCount];
    lCmdParamArray = new char *[lTotalCount];

    // initialize array to blank strings
    for (int i=0; i<lTotalCount; i++)
    {
      lCmdParamArray[i] = new char [kCHKPT_PARAM_LEN]; //SMR XXX
      strcpy(lCmdParamArray[i], " ");
    }
   
    int lRealSampleCount = 0;
    int lRealChkPtCount = 0;
    
    if (lSampleCount > 0)
      lRealSampleCount = mIOTypeSampleTable->populateCommandRequestArray(lCommandArray, lCmdParamArray, lSampleCount, 0);
  
    if (lChkPtCount>0)
      lRealChkPtCount = mIOTypeChkPtTable->populateCommandRequestArray(lCommandArray, lCmdParamArray, lChkPtCount, lRealSampleCount);
    
    int lRealTotalCount = lRealSampleCount + lRealChkPtCount;

    // finally add in the additional command if is one
    if (lAddCmdFlag)
    {
      lCommandArray[lRealTotalCount] = REG_STR_DETACH;
      lRealTotalCount++;
    }
   
    if (lRealTotalCount > 0 || aForceEmitFlag == true)
    { 
      qApp->lock();
      lReGStatus = Emit_control(mSimHandle,			//ReG library
				lRealTotalCount,
				lCommandArray,
				lCmdParamArray);
      
      qApp->unlock();

      if (lReGStatus != REG_SUCCESS)
	THROWEXCEPTION("Emit_contol");
    }
    
    // clean up
    delete [] lCommandArray;
    for (int i=0; i<lTotalCount; i++)
      delete [] lCmdParamArray[i];
    delete [] lCmdParamArray;


  } //try

  catch (SteererException StEx)
  {
    StEx.print();

    delete [] lCommandArray;
    for (int i=0; i<lTotalCount; i++)
      delete [] lCmdParamArray[i];
    delete [] lCmdParamArray;

    emit detachFromApplicationForErrorSignal();
    QMessageBox::warning(0, "Steerer Error", "Internal library error - detaching from application",
			 QMessageBox::Ok,
			 QMessageBox::NoButton, 
			 QMessageBox::NoButton);
    
  }

}

void 
ControlForm::emitAllSlot()
{
  emitAll();
}

void 
ControlForm::emitAll(const int aAdditionalCmd)
{
  // emit both parameter values and iotype frequencies
  // and emit sample and checkpoint requested commands


  try 
  {
    // set the values in library
    int lCount = mSteerParamTable->setNewParamValuesInLib();
    lCount += mIOTypeSampleTable->setNewFreqValuesInLib();
    lCount += mIOTypeChkPtTable->setNewFreqValuesInLib();

    //emit to library, parameter aForceEmitFlag=true so emit happens even if no commands to send
    emitAllIOCommands(aAdditionalCmd, true);
    
  }
  
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

void
ControlForm::emitAllAndDetachCmdSlot()
{
  emitAll(REG_STR_DETACH); 
}
