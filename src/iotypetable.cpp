/*----------------------------------------------------------------------------
    IOTypeTable class for QT steerer GUI.

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
//#include "ReG_Steer_Steerside.h"

#include "chkptform.h"
#include "exception.h"
#include "iotypetable.h"

#include <qapplication.h>
#include <qmessagebox.h>
#include <qtable.h>


IOTypeTable::IOTypeTable(QWidget *aParent, const char *aName, int aSimHandle, bool aChkPtType)
  : Table(aParent, aName, aSimHandle), mChkPtTypeFlag(aChkPtType), mRestartRowIndex(kNULL_INDX)
{
  DBGCON("IOTypeTable constructor");

  // set so list items automatically deleted
  mIOTypeList.setAutoDelete( TRUE );
  
  // create table to display iotypes 
  // table will either display sample or checkpoints - controlform.cpp controls this

  // as well as displaying iotypes in a table (one per row), a list of iotype objects
  // is also held on the class
  // each iotype object holds the row id of the row in the iotype table
  // that represents that iotype
  // each table row holds the iotype id in a column hidden form the user

  // note: many functions similar to Steeredparametertable class (and iotype similar to paramater) - not looking
  // at merging as iotype display may well change SMR XXX

  // set up signal/slots to enable buttons when tables populated
  connect(this, SIGNAL(enableSampleButtonsSignal()), aParent, SLOT(enableSampleButtonsSlot()));
  connect(this, SIGNAL(enableChkPtButtonsSignal()), aParent, SLOT(enableChkPtButtonsSlot()));
  
}

IOTypeTable::~IOTypeTable()
{
  DBGDST("IOTypeTable");

}

void
IOTypeTable::initTable()
{

  setNumInitRows(kIO_INIT_ROWS);
  setNumRows(kIO_INIT_ROWS);

  if (mChkPtTypeFlag)
    setNumCols(kNUM_IO_COLUMNS+1);
  else
     setNumCols(kNUM_IO_COLUMNS);
  

  // initially set all rows read only as all empty 
  // SMR XXX all this init rows stuff hopefully will be improved
  for (int i=0; i<kIO_INIT_ROWS; i++)
    setRowReadOnly(i, TRUE);


  setShowGrid(FALSE);
  verticalHeader()->hide();
  setLeftMargin(0);

  horizontalHeader()->setLabel(kIO_ID_COLUMN, "ID");
  horizontalHeader()->setLabel(kIO_NAME_COLUMN, "Name"); 
  horizontalHeader()->setLabel(kIO_VALUE_COLUMN, "Freq");
  horizontalHeader()->setLabel(kIO_NEWVALUE_COLUMN, "New Freq");
  horizontalHeader()->setLabel(kIO_SPLIT, "");

  if (mChkPtTypeFlag)
  {
    horizontalHeader()->setLabel(kIO_REQUEST_COLUMN, "Create");
    horizontalHeader()->setLabel(kIO_RESTART_COLUMN, "Restart");
    setColumnWidth(kIO_REQUEST_COLUMN, 50);
    setColumnWidth(kIO_RESTART_COLUMN, 55);
  }
  else
  {
    horizontalHeader()->setLabel(kIO_REQUEST_COLUMN, "Request");
    setColumnWidth(kIO_REQUEST_COLUMN, 80);
  }

  
  setColumnReadOnly(kIO_ID_COLUMN, TRUE);
  setColumnReadOnly(kIO_NAME_COLUMN, TRUE);
  setColumnReadOnly(kIO_VALUE_COLUMN, TRUE);
  setColumnReadOnly(kIO_SPLIT, TRUE);
  

  hideColumn(kIO_ID_COLUMN);
  setColumnWidth(kIO_NAME_COLUMN, 200);
  setColumnWidth(kIO_VALUE_COLUMN, 40);
  setColumnWidth(kIO_NEWVALUE_COLUMN, 70);
  setColumnWidth(kIO_SPLIT, 10);

  
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
    DBGMSG("Ignoring sample with type REG_IO_INOUT");
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

  if (lRowIndex >= getNumInitRows())
    insertRows(lRowIndex);
  else
    setRowReadOnly(lRowIndex, FALSE);  // set true in initTable  SMR XXX rm when initRows goes


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
  if (mChkPtTypeFlag)
  {
    switch(lType)
    {
      case REG_IO_IN:
	setItem(lRowIndex, kIO_REQUEST_COLUMN, new QTableItem(this, QTableItem::Never, QString::null));
	setItem(lRowIndex, kIO_RESTART_COLUMN, new QCheckTableItem(this, "Restart"));
	setItem(lRowIndex, kIO_VALUE_COLUMN, 
		new QTableItem(this, QTableItem::Never, QString::null));
	setItem(lRowIndex, kIO_NEWVALUE_COLUMN,
		new QTableItem(this, QTableItem::Never, QString::null));

	break;
      case REG_IO_OUT:
	setItem(lRowIndex, kIO_REQUEST_COLUMN, new QCheckTableItem(this, ""));
	setItem(lRowIndex, kIO_RESTART_COLUMN, new QTableItem(this, QTableItem::Never, QString::null));
	break;
      case REG_IO_INOUT:
	setItem(lRowIndex, kIO_REQUEST_COLUMN, new QCheckTableItem(this, ""));
	setItem(lRowIndex, kIO_RESTART_COLUMN, new QCheckTableItem(this, ""));
	break;
    }
  }
  else
  {

    switch(lType)
    {
      case REG_IO_IN:
	setItem(lRowIndex, kIO_REQUEST_COLUMN, new QCheckTableItem(this, "Consume"));
	break;
      case REG_IO_OUT:
	setItem(lRowIndex, kIO_REQUEST_COLUMN, new QCheckTableItem(this, "Emit"));
	break;
    }
  }

  lIOTypePtr->setIndex(lRowIndex);
  mIOTypeList.append(lIOTypePtr);
  incrementRowIndex();
  
  DBGMSG1("mMaxRowIndexPtr", getMaxRowIndex());
  DBGMSG1("NUmInitRows", getNumInitRows());
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

    if (mChkPtTypeFlag)
    {
      if (lIOTypePtr->getType() != REG_IO_IN)
	((QCheckTableItem *)item(lRowIndex, kIO_REQUEST_COLUMN))->setChecked(FALSE);

      if (lIOTypePtr->getType() != REG_IO_OUT)
	((QCheckTableItem *)item(lRowIndex, kIO_RESTART_COLUMN))->setChecked(FALSE);      
    }
    else      
      ((QCheckTableItem *)item(lRowIndex, kIO_REQUEST_COLUMN))->setChecked(FALSE);
    

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
  // only care is app is still attached

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
	  QMessageBox::information(0, "Invalid Frequency", "The entered value is not valid",
				   QMessageBox::Ok,
				   QMessageBox::NoButton, 
				   QMessageBox::NoButton);
	  setText( aRow, aCol, QString::null );
	  // SMR XXX make that cell the selected one -  to do setCurrentCell not work
	}
      }
      else if (aCol == kIO_RESTART_COLUMN)
      {      
	// can only have one restart checked at once

	if (((QCheckTableItem *) this->item(aRow, aCol))->isChecked())
        {
	  int lOldRestartRowIndex = mRestartRowIndex;
	  mRestartRowIndex = aRow;
	  // uncheck existing row (actually calls slot again)
	  if (mRestartRowIndex != lOldRestartRowIndex && lOldRestartRowIndex != kNULL_INDX)
	    ((QCheckTableItem *)item(lOldRestartRowIndex, kIO_RESTART_COLUMN))->setChecked(FALSE);
	  
	}
	else
	{
	  // check if unchecking mRestartRowIndex
	  if (mRestartRowIndex == aRow)
	    mRestartRowIndex = kNULL_INDX;
	}	
      }
      
    } //getAppAttached


  } //try

  catch (SteererException StEx)
  {

    StEx.print();
    emit detachFromApplicationForErrorSignal();
    QMessageBox::warning(0, "Steerer Error", "Internal error - detaching from application",
			 QMessageBox::Ok,
			 QMessageBox::NoButton, 
			 QMessageBox::NoButton);

  }


}


int
IOTypeTable::getCommandRequestsCount()
{

  // count up number of comands to send (do not include restart cmd)
  // could keep track of this number as request flag set in table ...
  // but as list should never be that long is simpler just to count now - this has
  // advantage of not having to worry about that number being wrong esp as will use it
  // to size an array.

  if (getMaxRowIndex()==0)
    return 0;

  IOType *lIOTypePtr;
  int lCount = 0;
  QPtrListIterator<IOType> mIOTypeIterator( mIOTypeList );	    

  mIOTypeIterator.toFirst();
  while ( (lIOTypePtr = mIOTypeIterator.current()) != 0)
  {
    if (((QCheckTableItem *) this->item(lIOTypePtr->getRowIndex(), kIO_REQUEST_COLUMN))->isChecked())
      lCount++;
    
    ++mIOTypeIterator;
  }

  return lCount;
}


int
IOTypeTable::populateCommandRequestArray(int *aCmdArray, char **aCmdParamArray, const int aMaxCmds, const int aStartIndex)
{
  // populate the array aCmdIds with the commands to be send starting at array index aStartIndex
  // there should be exactly aMaxCmds to add - do not add any more as array not sized to hold an more
  // note any not added will remain checked on GUI indicating that not been sent so no need to throw excp.
  // As Commands (IOTypes) are added uncheck "Request" column on the GUI

  QCheckTableItem *lCheckItem;
  IOType *lIOTypePtr;
  QPtrListIterator<IOType> mIOTypeIterator( mIOTypeList );

  int lNumAdded = 0;
  int lIndex = aStartIndex;

  mIOTypeIterator.toFirst();
  while ( ((lIOTypePtr = mIOTypeIterator.current()) != 0) && 
	  (lNumAdded  <= aMaxCmds))
  {
    if (!(mChkPtTypeFlag && lIOTypePtr->getType() == REG_IO_IN))
    {

      lCheckItem = (QCheckTableItem *) this->item(lIOTypePtr->getRowIndex(), kIO_REQUEST_COLUMN);
      if (lCheckItem->isChecked())
      {
	// recheck box in table... 
	lCheckItem->setChecked(FALSE);
	aCmdArray[lIndex] = lIOTypePtr->getId();
	if (mChkPtTypeFlag)
	  strcpy(aCmdParamArray[lIndex], "OUT 1");
	lIndex++;
	lNumAdded++;
      }
    }
    ++mIOTypeIterator;
  }

  if (aMaxCmds != lNumAdded)
    DBGMSG("Num iotype-commands sent not same as num expected ");  //log this SMR XXX

  // return the actual number of commands added
  return lNumAdded;

}

void 
IOTypeTable::emitCommandsSlot()
{ 
  // emit any iotype-command the user has flagged as ("request" checkbox on gui)
  // and clear checkboxes

  int lCount=0;
  int *lCommandArray = kNULL;
  char **lCmdParamArray = kNULL;

  try
  {

    lCount = getCommandRequestsCount();
    
    if (lCount > 0)
    {
      // populate the array of commands and array of command parameters  
      lCommandArray = new int[lCount];
      lCmdParamArray = new char *[lCount];
      for (int i=0; i<lCount; i++)
      { 
	lCmdParamArray[i] = new char [kCHKPT_PARAM_LEN]; //SMR XXX
	strcpy(lCmdParamArray[i], " ");
      }

      int lNumAdded = populateCommandRequestArray(lCommandArray, lCmdParamArray, lCount, 0);
      
      // library call to emit to application
      if (lNumAdded >0)
      {
	if (Emit_control(getSimHandle(),			//ReG library
			 lNumAdded,
			 lCommandArray,
			 lCmdParamArray) != REG_SUCCESS)
	  THROWEXCEPTION("Emit_contol");
      }
      
      DBGMSG1("Sent Sample Commands", lCount);
      
      // clean up
      delete [] lCommandArray;
      for (int i=0; i<lCount; i++)
	delete [] lCmdParamArray[i];
      delete [] lCmdParamArray;
 
    } // if lCount>0  
    
  } //try

  catch (SteererException StEx)
  {
    StEx.print();

    // clean up
    delete [] lCommandArray;
    for (int i=0; i<lCount; i++)
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
IOTypeTable::emitRestartSlot()
{ 
  QCheckTableItem	*lCheckItem;
  int			*lCommandArray = kNULL;
  char			**lCmdParamArray = kNULL;
  ChkPtForm		*lChkPtForm = kNULL;

  try 
  {
  
    DBGMSG1("populate: mRestartrowIndex is ", mRestartRowIndex);

    if (mChkPtTypeFlag)
    {      
      if (mRestartRowIndex > kNULL_INDX)
      {
	// populate the array of commands and array of command parameters  
	lCommandArray = new int[1];
	lCmdParamArray = new char *[1];
	lCmdParamArray[0] = new char [kCHKPT_PARAM_LEN]; //SMR XXX
	strcpy(lCmdParamArray[0], " ");
	
	bool lOk = false;
	lCheckItem = (QCheckTableItem *) this->item(mRestartRowIndex, kIO_RESTART_COLUMN);
	
	if (lCheckItem->isChecked())
	{
	  // find the cmdid for this row
	  int lCmdId = this->text(mRestartRowIndex, kIO_ID_COLUMN).toInt(&lOk);
	  if (lOk)
	  {
	    lCommandArray[0] = lCmdId;
	  
	    // Get number log entries for this checkpoint
	    int lNumEntries = 0;
	    qApp->lock();
	    if (Get_chk_log_number(getSimHandle(), 
				   lCmdId, 
				   &lNumEntries) != REG_SUCCESS)
	      THROWEXCEPTION("Get_chk_log_number");
	    qApp->unlock();
	    
	    if (lNumEntries > 0)
	    { 
	      // get list of ChkTags from log 
	      
	      lChkPtForm = new ChkPtForm(lNumEntries, getSimHandle(), lCmdId, this);
     
	      if (lChkPtForm->getLibReturnStatus() == REG_SUCCESS)
	      {
  
		if ( lChkPtForm->exec() == QDialog::Accepted ) 
	        {
		  DBGMSG1("ChkPt accepted, tag = ",  lChkPtForm->getChkTagSelected());
		  sprintf(lCmdParamArray[0], "IN %s", lChkPtForm->getChkTagSelected() );

		  if (Emit_control(getSimHandle(),			//ReG library
				   1,
				   lCommandArray,
				   lCmdParamArray) != REG_SUCCESS)
		    THROWEXCEPTION("Emit_contol");
		  
		  DBGMSG("Sent Restart Commands");
		}
		else
		  DBGMSG("Cancelled restart chktag selector");

	      } 
	      else 
		THROWEXCEPTION("Get_chk_log_entries");

	      delete lChkPtForm;		
	    }
	    else
	    {	      
	      QMessageBox::information(0, "CheckPoint Restart", 
				       "No checkpoints found in log",
				       QMessageBox::Ok,
				       QMessageBox::NoButton, 
				       QMessageBox::NoButton);

	    }
    
	  }
	  else
	    THROWEXCEPTION("Failed to get iotype ID from row in table");
	  
	}
	else
	  DBGMSG("mRestartRowIndex not checked - resetting");  //log this SMR XXX
	
	// reset regardless of errors
	lCheckItem->setChecked(FALSE);
	mRestartRowIndex = kNULL_INDX;
	
	// clean up
	delete [] lCommandArray;
	delete [] lCmdParamArray[0];
	delete [] lCmdParamArray;

      }
      else
      {

	QMessageBox::information(0, "Steerer Restart Functionality", 
				 "Please select CheckPoint IOType for application to use for restart",
				 QMessageBox::Ok,
				 QMessageBox::NoButton, 
				 QMessageBox::NoButton);
      }
      
    } //mChkPtTypeFlag
  } //try 
  
  catch (SteererException StEx)
  {
    StEx.print();

    // clean up
    delete [] lCommandArray;
    delete [] lCmdParamArray[0];
    delete [] lCmdParamArray;
    delete lChkPtForm;
 
    emit detachFromApplicationForErrorSignal();
    QMessageBox::warning(0, "Steerer Error", "Internal library error - detaching from application",
			 QMessageBox::Ok,
			 QMessageBox::NoButton, 
			 QMessageBox::NoButton);
    
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

      qApp->lock();

      if (mChkPtTypeFlag)
	lReGStatus = Set_chktype_freq(getSimHandle(),			//ReG library
				     lIndex,
				     lHandles,
				     lFreqs);
      else
	lReGStatus = Set_iotype_freq(getSimHandle(),			//ReG library
				     lIndex,
				     lHandles,
				     lFreqs);
      qApp->unlock();

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

  DBGMSG1("do set_iottype_freq ",lIndex);

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
      qApp->lock();
      // "emit" values to steered application
      lReGStatus = Emit_control(getSimHandle(),				//ReG library
				0,
				NULL,
				NULL);
      qApp->unlock();

      if (lReGStatus != REG_SUCCESS)
	THROWEXCEPTION("Emit_contol");
    }
    else
      DBGMSG("No new freq to send");

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



