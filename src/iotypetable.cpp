/*----------------------------------------------------------------------------
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
    
---------------------------------------------------------------------------*/

/** @file iotypetable.cpp
    @brief IOTypeTable class for QT steerer GUI.
    @author Sue Ramsden
    @author Mark Riding
    @author Andrew Porter */

#include "types.h"
#include "debug.h"
#include "chkptform.h"
#include "exception.h"
#include "iotypetable.h"

#include <qapplication.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qtable.h>


IOTypeTable::IOTypeTable(QWidget *aParent, const char *aName, int aSimHandle, 
			 QMutex *aMutex, bool aChkPtType)
  : Table(aParent, aName, aSimHandle), mChkPtTypeFlag(aChkPtType), 
    mRestartRowIndex(kNULL_INDX), mRestartRowIndexNew(kNULL_INDX),
    mMutexPtr(aMutex)
{
  REG_DBGCON("IOTypeTable constructor");

  // set so list items automatically deleted
  mIOTypeList.setAutoDelete( TRUE );
  
  // create table to display iotypes 
  // table will either display sample or checkpoints - controlform.cpp controls this

  // as well as displaying iotypes in a table (one per row), a list of iotype objects
  // is also held on the class
  // each iotype object holds the row id of the row in the iotype table
  // that represents that iotype
  // each table row holds the iotype id in a column hidden form the user

  // note: many functions similar to Steeredparametertable class (and iotype 
  // similar to paramater) - not looking
  // at merging as iotype display may well change SMR XXX

  // set up signal/slots to enable buttons when tables populated
  connect(this, SIGNAL(enableSampleButtonsSignal()), aParent, SLOT(enableSampleButtonsSlot()));
  connect(this, SIGNAL(enableChkPtButtonsSignal()), aParent, SLOT(enableChkPtButtonsSlot()));

  // MR: propogate table selection changes
  connect(this, SIGNAL(selectionChanged()), this, SLOT(selectionChangedSlot()));
  connect(this, SIGNAL(currentChanged(int, int)), this, SLOT(currentChangedSlot(int, int)));

  // MR:
  connect(this, SIGNAL(setRestartButtonStateSignal(const bool)), aParent, 
	  SLOT(setRestartButtonStateSlot(const bool)));
  connect(this, SIGNAL(setCreateButtonStateSignal(const bool)), aParent, 
	  SLOT(setCreateButtonStateSlot(const bool)));
  connect(this, SIGNAL(setConsumeButtonStateSignal(const bool)), aParent, 
	  SLOT(setConsumeButtonStateSlot(const bool)));
  connect(this, SIGNAL(setEmitButtonStateSignal(const bool)), aParent, 
	  SLOT(setEmitButtonStateSlot(const bool)));
  
  // MR:
  // Ensure that instances of this table aren't drawn too large,
  // since they're unlikely to be particularly full
  setMinimumHeight(70);
  setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, 
			    QSizePolicy::MinimumExpanding));
}

IOTypeTable::~IOTypeTable()
{
  REG_DBGDST("IOTypeTable");
}

void
IOTypeTable::initTable()
{
  if (mChkPtTypeFlag)
    setNumCols(kNUM_IO_COLUMNS-1);
  else
     setNumCols(kNUM_IO_COLUMNS);


  setShowGrid(FALSE);
  verticalHeader()->hide();
  setLeftMargin(0);

  horizontalHeader()->setLabel(kIO_ID_COLUMN, "ID");
  horizontalHeader()->setLabel(kIO_NAME_COLUMN, "Name"); 
  horizontalHeader()->setLabel(kIO_VALUE_COLUMN, "Freq");
  horizontalHeader()->setLabel(kIO_NEWVALUE_COLUMN, "New Freq");

  if (!mChkPtTypeFlag)
  {
    horizontalHeader()->setLabel(kIO_REQUEST_COLUMN, "IO Type");
    setColumnWidth(kIO_REQUEST_COLUMN, 60);
    setColumnReadOnly(kIO_REQUEST_COLUMN, TRUE);
  }


  setColumnReadOnly(kIO_ID_COLUMN, TRUE);
  setColumnReadOnly(kIO_NAME_COLUMN, TRUE);
  setColumnReadOnly(kIO_VALUE_COLUMN, TRUE);
  
  // id column used internally only, therefore not show on gui
  hideColumn(kIO_ID_COLUMN);
  setColumnWidth(kIO_NAME_COLUMN, 200);
  setColumnWidth(kIO_VALUE_COLUMN, 40);
  setColumnWidth(kIO_NEWVALUE_COLUMN, 70);
  
  // set up signal/slot to handle data entered by user
  connect (this, SIGNAL( valueChanged(int,int) ),
	   this, SLOT( validateValueSlot(int,int) ) );

}

int
IOTypeTable::getNumIOTypes() const
{
 return getMaxRowIndex();
}

int 
IOTypeTable::findIOTypeRowIndex(int aId) // SMR XXX used???
{
  // search iotype list to find iotype with aId.
  // return the mRowIndex for that iotype - i.e. which row in
  // the table represents that iotype
  // return -1 if iotype not found

  IOType *lIOTypePtr;
    
  QPtrListIterator<IOType> mIOTypeIterator( mIOTypeList );	    
  mIOTypeIterator.toFirst();
  while ( (lIOTypePtr = mIOTypeIterator.current()) != 0)
  {
    if (lIOTypePtr->getId() == aId)
    {
      return lIOTypePtr->getRowIndex();
    }
    ++mIOTypeIterator;
  }
  
  return -1;

}

IOType *
IOTypeTable::findIOType(int aId)
{
  // search iotype list to find iotype with aId.
  // return pointer to that iotype
  // return kNULL is iotype not in list

  IOType *lIOTypePtr;
    
  QPtrListIterator<IOType> mIOTypeIterator( mIOTypeList );	    
  mIOTypeIterator.toFirst();
  while ( (lIOTypePtr = mIOTypeIterator.current()) != 0)
  {
    if (lIOTypePtr->getId() == aId)
    {
      return lIOTypePtr;
    }
    ++mIOTypeIterator;
  }
  
  return kNULL;

}

bool 
IOTypeTable::updateRow(const int lHandle, const int lVal)
{
  // Search list of existing iotypes for this lHandle
  // If found update it now
  // Return value indicates whether or not row has been found and updated.

  IOType	*lIOTypePtr;
  if ((lIOTypePtr = findIOType(lHandle)) != kNULL)
  { 
    int lRowIndex = lIOTypePtr->getRowIndex();
    item(lRowIndex,kIO_VALUE_COLUMN)->setText(QString::number(lVal));
    updateCell(lRowIndex, kIO_VALUE_COLUMN);
    return true;
  }

  return false;
}


void
IOTypeTable::addRow(const int lHandle, const char *lLabel, const int lVal, const int lType)
{
  // should never have INOUT type for sample
  if (!mChkPtTypeFlag && lType == REG_IO_INOUT)
  {
    REG_DBGMSG("Ignoring sample with type REG_IO_INOUT");
    return;
  }

  // add a new iotype to the table and the list

  int lRowIndex = getMaxRowIndex();

  // if this is first row then enable buttons
  if (lRowIndex == 0)
  {
    if (mChkPtTypeFlag)
      emit enableChkPtButtonsSignal();
    else
      emit enableSampleButtonsSignal();
  }  

  // MR: removed - was causing problems, and giving no noticeable benefit
  //if (lRowIndex >= getNumInitRows())
    insertRows(lRowIndex);
  //else
  //  setRowReadOnly(lRowIndex, FALSE);  // set true in initTable  SMR XXX rm when initRows goes


  IOType	*lIOTypePtr;
  lIOTypePtr = new IOType(lHandle, lType);
  
  setText(lRowIndex, kIO_ID_COLUMN, QString::number(lHandle) );
  setText(lRowIndex, kIO_NAME_COLUMN, lLabel);
  setItem(lRowIndex, kIO_VALUE_COLUMN, 
	  new QTableItem(this, QTableItem::Never, QString::number(lVal)));
  setItem(lRowIndex, kIO_NEWVALUE_COLUMN,
	  new QTableItem(this, QTableItem::OnTyping, QString::null));

  // set up the "request" column (checkbox to say which iotypes request now when hit button)
  // SMR XXXn this is WRONG!
  if (!mChkPtTypeFlag)
  {
    switch(lType)
    {
      case REG_IO_IN:
        setItem(lRowIndex, kIO_REQUEST_COLUMN, new QTableItem(this, QTableItem::Never, "Input"));
        break;
      case REG_IO_OUT:
        setItem(lRowIndex, kIO_REQUEST_COLUMN, new QTableItem(this, QTableItem::Never, "Output"));
        break;
    }
  }

  lIOTypePtr->setIndex(lRowIndex);
  mIOTypeList.append(lIOTypePtr);
  incrementRowIndex();

}

void IOTypeTable::clearAndDisableForDetach(const bool aUnRegister)
{

  // clear all the editable cells in the table
  // and make them readonly
  // this is done when the the application has detached
  // and hence is not longer steerable

  // need to flag that app has detached to prevent any further validation of 
  // values user is currently editing
  setAppDetached();

  IOType *lIOTypePtr;
    
  QPtrListIterator<IOType> mIOTypeIterator( mIOTypeList );	    
  mIOTypeIterator.toFirst();
  while ( (lIOTypePtr = mIOTypeIterator.current()) != 0)
  {
    int lRowIndex = lIOTypePtr->getRowIndex();
    // reset focus of table to top left cell if this cell is currently selected
    // if do not do this the setItem statement does not work
    if (isSelected(lIOTypePtr->getRowIndex(), kIO_NEWVALUE_COLUMN))
      setCurrentCell(0, kIO_NAME_COLUMN);

    item(lRowIndex, kIO_NEWVALUE_COLUMN)->setText(QString::null);
    updateCell(lRowIndex, kIO_NEWVALUE_COLUMN);


    ++mIOTypeIterator;
  }
  setReadOnly(TRUE); 

}

void IOTypeTable::clearNewValues()
{
  // clear all the (editable) new freq cells in the table
  // this is done when the values have been emitted to the application

  IOType *lIOTypePtr;
    
  QPtrListIterator<IOType> mIOTypeIterator( mIOTypeList );	    
  mIOTypeIterator.toFirst();
  while ( (lIOTypePtr = mIOTypeIterator.current()) != 0)
  {
    int lRowIndex = lIOTypePtr->getRowIndex();
    item(lRowIndex, kIO_NEWVALUE_COLUMN)->setText(QString::null);
    updateCell(lRowIndex, kIO_NEWVALUE_COLUMN);

    ++mIOTypeIterator;
  }
  
}

void 
IOTypeTable::validateValueSlot( int aRow, int aCol )
{
  // validate the value entered by user and set on gui,
  // otherwise just post invalid msg and clear cell of invalid entry

  // as we need int value for ReG library call -  store the int value on the IOType class
  // to avoid having to convert QString value held on gui.

  // only care about new value column and restart column
  // only care if app is still attached

  try
  {
    
    if (getAppAttached())
    {

      if (aCol == kIO_NEWVALUE_COLUMN)
      {
	bool lOk = true;
	
	//always allow empty in which case frequency set to kNULL_REQ , otherwise validate int
	int lFreq = kNULL_FREQ;
	if (!this->text( aRow, aCol ).isEmpty())      
	  lFreq = this->text( aRow, aCol ).toInt( &lOk );
	
	if (lOk)
	{
	  // get iotype ID from hidden column and use this get iotype from the list
	  int lId = this->text( aRow, kIO_ID_COLUMN ).toInt(&lOk);
	  if (!lOk)
	    THROWEXCEPTION("Failed to get iotype ID from row in table");
	  
	  IOType *lIOTypePtr = findIOType(lId);
	  if  (lIOTypePtr == kNULL)
	    THROWEXCEPTION("Failed to find iotype in list");
	
	  // validate freq and return whether or not valid
	  lOk = lIOTypePtr->validateAndSetFrequency(lFreq);
	}
	
	if (!lOk)
	{
	  QMessageBox::information(0, "Invalid Frequency", 
				   "The entered value is not valid",
				   QMessageBox::Ok,
				   QMessageBox::NoButton, 
				   QMessageBox::NoButton);
	  setText( aRow, aCol, QString::null );
	  // SMR XXX make that cell the selected one -  to do setCurrentCell not work
	}
      }

    } //getAppAttached


  } //try

  catch (SteererException StEx)
  {

    StEx.print();
    emit detachFromApplicationForErrorSignal();

    QMessageBox mb("RealityGrid Steerer",
		   "Internal error - IOType not valid",
		   QMessageBox::Warning,
		   QMessageBox::Ok,
		   QMessageBox::NoButton,
		   QMessageBox::NoButton,
		   this, "Modeless warning", false);
    mb.setModal(false);
    mb.exec();    
  }


}


int
IOTypeTable::setNewFreqValuesInLib()
{
  // call ReG library function to set the new frequency values as per set in the GUI
  // get the value direct from gui table 
  // clear all new value cells in table once values have been set
  // return the number of values set

  // Note: this only sets the values in the library - the "emit" of the new values
  // to the steered application is done elsewhere via Emit_control()
		     
  if (getMaxRowIndex()==0)
    return 0;


  IOType *lIOTypePtr;
  int lCount = 0;
  QPtrListIterator<IOType> mIOTypeIterator( mIOTypeList );	    
  int *lHandles = kNULL;
  int *lFreqs = kNULL;
  int lIndex=0;

  try
  {

    // count the number of not empty frequency cells
    mIOTypeIterator.toFirst();
    while ( (lIOTypePtr = mIOTypeIterator.current()) != 0)
    {
      if (lIOTypePtr->getFrequency() > kNULL_FREQ)
	lCount++;
      
      ++mIOTypeIterator;
    }

    if (lCount>0)
    {
      // set up arrays for ReG libary call
      lHandles = new int [lCount];
      lFreqs = new int [lCount];
      
      mIOTypeIterator.toFirst();
      while ( ((lIOTypePtr = mIOTypeIterator.current()) != 0) &&
	      (lIndex <lCount) )
      {
	if (lIOTypePtr->getFrequency() > kNULL_FREQ)
	{
	  lHandles[lIndex] = lIOTypePtr->getId();
	  lFreqs[lIndex] = lIOTypePtr->getAndResetFrequency();
	  lIndex++;
	}
	++mIOTypeIterator;
	
      }

      int lReGStatus = REG_FAILURE;

      mMutexPtr->lock();

      if (mChkPtTypeFlag){
	lReGStatus = Set_chktype_freq(getSimHandle(), //ReG library
				     lIndex,
				     lHandles,
				     lFreqs);
      }
      else{
	lReGStatus = Set_iotype_freq(getSimHandle(),  //ReG library
				     lIndex,
				     lHandles,
				     lFreqs);
      }
      mMutexPtr->unlock();

      // set the values in the steering library
      if (lReGStatus != REG_SUCCESS)
      {
	THROWEXCEPTION("Set_iotype_freq");
      }
      
      // clear the cells in the table
      clearNewValues();
            
      // clean up
      delete [] lHandles;
      delete [] lFreqs;
      
    }
    
  } //try
  
  catch (SteererException StEx)
    {
      // clean up
    delete [] lHandles;
    delete [] lFreqs;

    throw StEx;
  }

  REG_DBGMSG1("do set_iottype_freq ",lIndex);

  return lIndex;

}

void
IOTypeTable::emitValuesSlot()
{
  // set the new iotype frquency values in the library as per set in the GUI
  // and "emit" these to the steered application
  // the new value cells in the table will be cleared as part of this
  // ready for user to enter the next values
  int lReGStatus = REG_FAILURE;

  try 
  {

    if (setNewFreqValuesInLib() > 0)
    {  
      mMutexPtr->lock();
      // "emit" values to steered application
      lReGStatus = Emit_control(getSimHandle(),	       	//ReG library
				0,
				NULL,
				NULL);
      mMutexPtr->unlock();

      if (lReGStatus != REG_SUCCESS)
	THROWEXCEPTION("Emit_contol");
    }
    else
      REG_DBGMSG("No new freq to send");

  } //try

  catch (SteererException StEx)
  {
    StEx.print();
    emit detachFromApplicationForErrorSignal();
    QMessageBox::warning(0, "Steerer Error", 
			 "Failed to emit new parameter values - detaching from application",
			 QMessageBox::Ok,
			 QMessageBox::NoButton, 
			 QMessageBox::NoButton);
    
  }


}


/** Method is called whenever the user changes the selection in the table
 *  Only deal with the checkpoint table
 */
void IOTypeTable::selectionChangedSlot(){
    REG_DBGMSG("\nIOTypeTable::selectionChanged called\n");
    
    if (!getAppAttached())
      return;

    if (mChkPtTypeFlag)
      setCreateRestartButtonStates();
    else
      setConsumeEmitButtonStates();
}


/** Unfortunately this method is required as well to catch all the possible
 *  events from the table
 */
void IOTypeTable::currentChangedSlot(int row, int column){
    if (!getAppAttached())
      return;

    if (mChkPtTypeFlag)
      setCreateRestartButtonStates();
    else
      setConsumeEmitButtonStates();

}


void IOTypeTable::setConsumeEmitButtonStates(){
  int numInputRowsSelected = 0;
  int numOutputRowsSelected = 0;
  int numTotalRowsSelected = 0;

  // First work out what the user's selected
  for (int i=0; i<numRows(); i++){
    if (isRowSelected(i)){
      numTotalRowsSelected++;
      
      if (text(i, kIO_REQUEST_COLUMN).compare("Input") == 0)
        numInputRowsSelected++;
      else // It's an "Output"
        numOutputRowsSelected++;
        
    }
  }

  if (numInputRowsSelected > 0){
    // enable both the consume and emit buttons
    emit setConsumeButtonStateSignal(true);
  }
  else {
    // disable both the buttons
    emit setConsumeButtonStateSignal(false);
  }

  if (numOutputRowsSelected > 0){
    // enable both the consume and emit buttons
    emit setEmitButtonStateSignal(true);
  }
  else {
    // disable both the buttons
    emit setEmitButtonStateSignal(false);
  }
  
}

/** Unfortunatley this is called on a double click too - at which point
 *  the isRowSelected() method reports that nothing is selected!??!?!
 */
void IOTypeTable::setCreateRestartButtonStates(){
    int firstSelectedRestartRow = -1;
    int firstSelectedCreateRow = -1;
    int numValidRestartRowsSelected = 0;
    int numValidCreateRowsSelected = 0;
    int numTotalRowsSelected = 0;

    for (int i=0; i<numRows(); i++){
      if (isRowSelected(i)){// && item(i, kIO_RESTART_COLUMN)->rtti() == 2){
        // Use the mIOTypeList object to determine if we can restart this checkpoint or not
        IOType *tmpIOType = mIOTypeList.at(i);
        if (tmpIOType->getType() == REG_IO_INOUT){
          if (firstSelectedRestartRow == -1)
            firstSelectedRestartRow = i;
          if (firstSelectedCreateRow == -1)
            firstSelectedCreateRow = i;
          numValidRestartRowsSelected++;
          numValidCreateRowsSelected++;
        }
        else if (tmpIOType->getType() == REG_IO_IN){
          if (firstSelectedRestartRow == -1)
            firstSelectedRestartRow = i;
          numValidRestartRowsSelected++;
        }
        else if (tmpIOType->getType() == REG_IO_OUT){
          if (firstSelectedCreateRow == -1)
            firstSelectedCreateRow = i;
          numValidCreateRowsSelected++;
        }

        numTotalRowsSelected++;
      }
    }

    // Check to see if the user has selected more than one row,
    // and if so, disable the restart button
    if (numValidRestartRowsSelected == 1 && numTotalRowsSelected == 1){
      emit setRestartButtonStateSignal(true);
    }
    else {
      emit setRestartButtonStateSignal(false);
    }

    if (numValidCreateRowsSelected != numTotalRowsSelected){
      emit setCreateButtonStateSignal(false);
    }
    else {
      emit setCreateButtonStateSignal(true);
    }

    mRestartRowIndexNew = firstSelectedRestartRow;
}


/** MR: replaces the getCommandReqestCount method
 */
int IOTypeTable::getCommandRequestsCountNew(){
  int lCount = 0;
  
  for (int i=0; i<numRows(); i++){
    if (isRowSelected(i))
      lCount++;
  }

  return lCount;
}

/** MR: simple method to count the number of Input or Output
 *      entries in the Sample IOType table / Data IO
 */
int IOTypeTable::getCommandRequestsCountOfType(const int aType){
  int result = 0;
  QString comparisonTestString;

  if (aType == REG_IO_IN)
    comparisonTestString = "Input";
  else if (aType == REG_IO_OUT)
    comparisonTestString = "Output";
  else return result;

  for (int i=0; i<numRows(); i++){
    if (isRowSelected(i) && text(i, kIO_REQUEST_COLUMN).compare(comparisonTestString) == 0)
      result++;
  }

  return result;
}

/** MR: this slot will replace the current emitCommandsSlot
 */
void IOTypeTable::createButtonPressedSlot(){
  int i;

  // this slot will get called for the SampleIOTypes table "Tell Req"
  // button as well, so ignore those events
  if (!mChkPtTypeFlag)
    return;

  int lCount = 0;
  int *lCommandArray = kNULL;
  char **lCmdParamArray = kNULL;

  try {
    lCount = getCommandRequestsCountNew();

    if (lCount > 0){
      // populate the array of commands of command parameters
      lCommandArray = new int[lCount];
      lCmdParamArray = new char*[lCount];
      for (i=0; i<lCount; i++){
        lCmdParamArray[i] = new char[kCHKPT_PARAM_LEN];
        strcpy(lCmdParamArray[i], " ");
      }

      int lNumAdded = populateCommandRequestArrayNew(lCommandArray, lCmdParamArray, lCount, 0);

      // library call to emit application
      if (lNumAdded >0){
        if (Emit_control(getSimHandle(), lNumAdded, lCommandArray, lCmdParamArray) != REG_SUCCESS){
          THROWEXCEPTION("Emit_control");
        }
      }
      REG_DBGMSG1("Sent Sample Commands", lCount);

      // clean up
      delete[] lCommandArray;
      for (int i=0; i<lCount; i++)
        delete[] lCmdParamArray[i];
      delete [] lCmdParamArray;

    } // if lcount > 0
  } // try
  catch (SteererException StEx){
    StEx.print();

    // clean up
    delete[] lCommandArray;
    for (i=0; i<lCount; i++)
      delete[] lCmdParamArray[i];
    delete [] lCmdParamArray;

    emit detachFromApplicationForErrorSignal();
    QMessageBox::warning(0, "Steerer Error", 
			 "Failed to emit commands to create "
			 "checkpoint - detaching from application", 
			 QMessageBox::Ok, QMessageBox::NoButton, 
			 QMessageBox::NoButton);    
  }
}

/** MR: this slot will replace the current emitRestartSlot
 */
void IOTypeTable::restartButtonPressedSlot(){
  // this slot will only get called for the Create button form, 
  // so no need to worry about the other table
  int *lCommandArray = kNULL;
  char **lCmdParamArray = kNULL;
  ChkPtForm *lChkPtForm = kNULL;

  try {
    REG_DBGMSG1("populate: mRestartrowIndex is", mRestartRowIndexNew);

    if (mRestartRowIndexNew > kNULL_INDX){
      // populate the array of commands and array of command parameters
      lCommandArray = new int[1];
      lCmdParamArray = new char*[1];
      lCmdParamArray[0] = new char[kCHKPT_PARAM_LEN];
      strcpy(lCmdParamArray[0], " ");

      bool lOk = false;

      // actually need to be a little smarter here - need to check that the particular row is restartable...
      if (isRowSelected(mRestartRowIndexNew)){
        // find the cmdid for this row
        int lCmdId = this->text(mRestartRowIndexNew, kIO_ID_COLUMN).toInt(&lOk);
        if (lOk){
          lCommandArray[0] = lCmdId;

          // Get number log entries for this checkpoint
          int lNumEntries = 0;
          mMutexPtr->lock();
          if (Get_chk_log_number(getSimHandle(), 
				 lCmdId, &lNumEntries) != REG_SUCCESS)
            THROWEXCEPTION("Get_chk_log_number");
          mMutexPtr->unlock();

          if (lNumEntries > 0){
            // get list of ChkTags from log
            lChkPtForm = new ChkPtForm(lNumEntries, getSimHandle(), lCmdId, 
				       mMutexPtr, this);

            if (lChkPtForm->getLibReturnStatus() == REG_SUCCESS){
              if (lChkPtForm->exec() == QDialog::Accepted){
                REG_DBGMSG1("ChkPt accepted, tag = ", 
			lChkPtForm->getChkTagSelected());
                sprintf(lCmdParamArray[0], "IN %s", 
			lChkPtForm->getChkTagSelected());

                if (Emit_control(getSimHandle(), 1, lCommandArray, 
				 lCmdParamArray) != REG_SUCCESS){
                  THROWEXCEPTION("Emit_control");
                }
                REG_DBGMSG("Sent Restart Commands");
              } // QDialog::Accepted
              else {
                REG_DBGMSG("Cancelled restart chktag selector");
              }
            } // lChkPtForm->getLibReturnStatus() == REG_SUCCESS
            else {
              THROWEXCEPTION("Get_chk_log_entries");
            }

            delete lChkPtForm;
            
          } // if lNumEntries > 0
          else {
            QMessageBox::information(0, "CheckPoint Restart", 
				     "No checkpoints found in log", 
				     QMessageBox::Ok, 
				     QMessageBox::NoButton, 
				     QMessageBox::NoButton);
          }
        } // if (lOk)

        else
          THROWEXCEPTION("Failed to get iotype ID from row in table");
          
      } // if isRowSelected(mRestartRowIndexNew)
      else {
        REG_DBGMSG("mRestartRowIndexNew row not selected - resetting");
      }

      // reset regardless of errors
     //if (lCheckItem != NULL){
      //  lCheckItem->setChecked(false)
      mRestartRowIndexNew = kNULL_INDX;

      // clean up
      delete[] lCommandArray;
      delete[] lCmdParamArray[0];
      delete[] lCmdParamArray;
      
    } // if mRestartRowIndexNew > kNULL_INDX
    else {
      QMessageBox::information(0, "Steerer Restart Functionality", 
			       "Please select checkpoint IOType for "
			       "application to use for restart",
                               QMessageBox::Ok, QMessageBox::NoButton, 
			       QMessageBox::NoButton);
    }

  } // try
  catch (SteererException StEx){
    StEx.print();

    // clean up
    delete[] lCommandArray;
    delete[] lCmdParamArray[0];
    delete[] lCmdParamArray;
    delete lChkPtForm;

    emit detachFromApplicationForErrorSignal();
    QMessageBox::warning(0, "Steerer Error", 
			 "Internal library error - detaching from application", 
			 QMessageBox::Ok, QMessageBox::NoButton, 
			 QMessageBox::NoButton);
  }  
  
}

/** Replaces the original populateCommandRequestArray method with a version
 *  that doesn't rely on the checkboxes...
 */
int IOTypeTable::populateCommandRequestArrayNew(int *aCmdArray, char **aCmdParamArray, const int aMaxCmds, const int aStartIndex)
{
  // populate the array aCmdIds with the commands to be send starting at array index aStartIndex
  // there should be exactly aMaxCmds to add - do not add any more as array not sized to hold an more
  // note any not added will remain checked on GUI indicating that not been sent so no need to throw excp.

  //QCheckTableItem *lCheckItem;
  IOType *lIOTypePtr;
  QPtrListIterator<IOType> mIOTypeIterator( mIOTypeList );

  int lNumAdded = 0;
  int lIndex = aStartIndex;

  mIOTypeIterator.toFirst();
  while ( ((lIOTypePtr = mIOTypeIterator.current()) != 0) && (lNumAdded  <= aMaxCmds)){
    if (!(mChkPtTypeFlag && lIOTypePtr->getType() == REG_IO_IN)){

      //lCheckItem = (QCheckTableItem *) this->item(lIOTypePtr->getRowIndex(), kIO_REQUEST_COLUMN);
      if (isRowSelected(lIOTypePtr->getRowIndex())){
        aCmdArray[lIndex] = lIOTypePtr->getId();
        if (mChkPtTypeFlag) // this should always be the case
          strcpy(aCmdParamArray[lIndex], "OUT 1");
        lIndex++;
        lNumAdded++;
      }
    }
    ++mIOTypeIterator;
  }

  if (aMaxCmds != lNumAdded)
    REG_DBGMSG("Num iotype-commands sent not same as num expected ");  //log this SMR XXX

  // return the actual number of commands added
  return lNumAdded;

}

int IOTypeTable::populateCommandRequestArrayOfType(int *aCmdArray, char **aCmdParamArray, const int aMaxCmds, const int aStartIndex, const int aType)
{
  // populate the array aCmdIds with the commands to be sent starting at array index aStartIndex
  // there should be exactly aMaxCmds to add - do not add any more as array not sized to hold an more
  // note any not added will remain checked on GUI indicating that not been sent so no need to throw excp.

  IOType *lIOTypePtr;
  QPtrListIterator<IOType> mIOTypeIterator( mIOTypeList );

  int lNumAdded = 0;
  int lIndex = aStartIndex;

  mIOTypeIterator.toFirst();
  while ( ((lIOTypePtr = mIOTypeIterator.current()) != 0) && (lNumAdded  <= aMaxCmds)){

    //if (! (mChkPtTypeFlag && lIOTypePtr->getType() == REG_IO_IN) ){
    // Check to see if we've found a row matching the passed
    // type in the Sample IOType / Data IO Table
    if (!mChkPtTypeFlag && lIOTypePtr->getType() == aType){
    
      if (isRowSelected(lIOTypePtr->getRowIndex())){
        aCmdArray[lIndex] = lIOTypePtr->getId();
        if (mChkPtTypeFlag) // this should always be the case
          strcpy(aCmdParamArray[lIndex], "OUT 1");
          
        lIndex++;
        lNumAdded++;
      }
      
    }
    
    ++mIOTypeIterator;
  }

  if (aMaxCmds != lNumAdded)
    REG_DBGMSG("Num iotype-commands sent not same as num expected ");  //log this SMR XXX

  // return the actual number of commands added
  return lNumAdded;

}

void IOTypeTable::consumeButtonPressedSlot(){
  int i;

  // search through the table and find any highlighted entries,
  // read in data for those
  REG_DBGMSG("consumeButtonPressedSlot");
  
  // this slot will get called for the CheckPoint IOTypes table
  // as well, so ignore those events
  if (mChkPtTypeFlag)
    return;

  int lCount = 0;
  int *lCommandArray = kNULL;
  char **lCmdParamArray = kNULL;
    
  try {
    lCount = getCommandRequestsCountOfType(REG_IO_IN);
    
    if (lCount > 0){
      lCommandArray = new int[lCount];
      lCmdParamArray = new char*[lCount];

      for (i=0; i<lCount; i++){
        lCmdParamArray[i] = new char[kCHKPT_PARAM_LEN];
        strcpy(lCmdParamArray[i], " ");
      }

      int lNumAdded = populateCommandRequestArrayOfType(lCommandArray, lCmdParamArray, lCount, 0, REG_IO_IN);

      // library call to emit application
      if (lNumAdded > 0){
        if (Emit_control(getSimHandle(), lNumAdded, lCommandArray, lCmdParamArray) != REG_SUCCESS){
          THROWEXCEPTION("Emit_control");
        }
      }
      REG_DBGMSG1("Sent Sample Commands", lCount);

      // clean up
      delete[] lCommandArray;
      for (int i=0; i<lCount; i++){
        delete[] lCmdParamArray[i];
      }
      delete[] lCmdParamArray;
      
    } // if lCount > 0
    
  } // try
  catch (SteererException StEx){
    StEx.print();

    // clean up
    delete[] lCommandArray;
    for (i=0; i<lCount; i++){
      delete[] lCmdParamArray[i];
    }
    delete[] lCmdParamArray;
    
    emit detachFromApplicationForErrorSignal();
    QMessageBox::warning(0, "Steerer Error", 
			 "Internal library error - detaching from application", 
			 QMessageBox::Ok, QMessageBox::NoButton, 
			 QMessageBox::NoButton);
  }

}

void IOTypeTable::emitButtonPressedSlot(){
  int i;

  // search through the table and find any highlighted entries,
  // write data out for those
  REG_DBGMSG("emitButtonPressedSlot");

  // this slot will get called for the CheckPoint IOTypes table
  // as well, so ignore those events
  if (mChkPtTypeFlag)
    return;

  int lCount = 0;
  int *lCommandArray = kNULL;
  char **lCmdParamArray = kNULL;

  try {
    lCount = getCommandRequestsCountOfType(REG_IO_OUT);

    if (lCount > 0){
      lCommandArray = new int[lCount];
      lCmdParamArray = new char*[lCount];

      for (i=0; i<lCount; i++){
        lCmdParamArray[i] = new char[kCHKPT_PARAM_LEN];
        strcpy(lCmdParamArray[i], " ");
      }

      int lNumAdded = populateCommandRequestArrayOfType(lCommandArray, 
							lCmdParamArray, 
							lCount, 0, 
							REG_IO_OUT);
      // library call to emit application
      if (lNumAdded > 0){
        if (Emit_control(getSimHandle(), lNumAdded, lCommandArray, 
			 lCmdParamArray) != REG_SUCCESS){
          THROWEXCEPTION("Emit_control");
        }
      }
      REG_DBGMSG1("Sent Sample Commands", lCount);

      // clean up
      delete[] lCommandArray;
      for (i=0; i<lCount; i++){
        delete[] lCmdParamArray[i];
      }
      delete[] lCmdParamArray;

    } // if lCount > 0

  } // try
  catch (SteererException StEx){
    StEx.print();

    // clean up
    delete[] lCommandArray;
    for (i=0; i<lCount; i++){
      delete[] lCmdParamArray[i];
    }
    delete[] lCmdParamArray;

    emit detachFromApplicationForErrorSignal();
    QMessageBox::warning(0, "Steerer Error", 
			 "Failed to emit commands - detaching from application", 
			 QMessageBox::Ok, QMessageBox::NoButton, 
			 QMessageBox::NoButton);
  }
}

