/*----------------------------------------------------------------------------
    ParameterTable and SteeredParameterTable classes for QT steerer GUI.

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



#include "types.h"
#include "debug.h"
#include "ReG_Steer_Steerside.h"

#include "exception.h"
#include "parametertable.h"

#include <qmessagebox.h>


ParameterTable::ParameterTable(QWidget *aParent, const char *aName, int aSimHandle)
  : Table(aParent, aName, aSimHandle)
{
  DBGCON("ParameterTable");

  // setAutoDelete so QT deletes each obj in list when the list is deleted
  mParamList.setAutoDelete( TRUE );

  // create table to display all monitored parameters
  // as well as displaying parameters in a table (one per row), a list of parameter objects
  // is also held on the class
  // each parameter object holds the row id  of the row in the parameter table
  // that represents that parameter
  // each table row holds the parameter id in a column hidden form the user
}  

ParameterTable::~ParameterTable()
{
  DBGDST("ParameterTable");
}

void
ParameterTable::initTable()
{  
  // set up the monitored parameters table - columns headings, readonly status of columns etc.

  ///SMRXXX  setMinimumSize(QSize(900,900));

  setNumInitRows(kINIT_ROWS);
  setNumRows(kINIT_ROWS);
  setNumCols(kNUM_MON_COLUMNS);
  setReadOnly(TRUE);
 
  setShowGrid(FALSE);
  verticalHeader()->hide();
  setLeftMargin(0);

  horizontalHeader()->setLabel(kID_COLUMN, "ID");
  horizontalHeader()->setLabel(kNAME_COLUMN, "Name"); 
  horizontalHeader()->setLabel(kREG_COLUMN, "Registered?");
  horizontalHeader()->setLabel(kVALUE_COLUMN, "Value");
 
  hideColumn(kID_COLUMN);
  setColumnWidth(kREG_COLUMN, 90);
  setColumnWidth(kNAME_COLUMN, 200);
  setColumnWidth(kVALUE_COLUMN, 120);

}

int 
ParameterTable::getNumParameters() const
{
  return getMaxRowIndex();
}

bool 
ParameterTable::updateRow(const int lHandle, const char *lVal)
{
  // Search list of existing parameters for this lHandle
  // If found update it now
  // Return value indicates whether or not row has been found and updated.

  Parameter *lParamPtr;
  if ((lParamPtr = findParameter(lHandle)) != kNULL)
  {
    // set flag used later to indicate parameter has not be unregistered
    lParamPtr->setIsPresent();

    // update the parameter value and call updateCell to make sure display is updated immediately
    // Note: we could make the QTableItem displayed in this cell a member of parameter class
    // and just update that each time SMR XXX to check.

    item(lParamPtr->getRowIndex(), kVALUE_COLUMN)->setText(QString( lVal));

    updateCell(lParamPtr->getRowIndex(),kVALUE_COLUMN);

    return true;
  }
  else
    return false;

}

void 
ParameterTable::addRow(const int lHandle, const char *lLabel, const char *lVal, const int lType)
{
  // add a new parameter to the table and parameter list

  int lRowIndex = getMaxRowIndex();

  if (lRowIndex >= getNumInitRows())
    insertRows(lRowIndex);

  Parameter *lParamPtr = new Parameter(lHandle, lType, false);
  
  setText(lRowIndex, kID_COLUMN, 
	  QString::number(lHandle) );
  setText(lRowIndex, kNAME_COLUMN, lLabel);
  setText(lRowIndex, kREG_COLUMN, "Yes");
  
  setItem(lRowIndex, kVALUE_COLUMN, 
	  new QTableItem(this, QTableItem::Never,  QString( lVal)));
  
  lParamPtr->setIndex(lRowIndex);
  mParamList.append(lParamPtr);
  incrementRowIndex();
  
  DBGMSG1("mMaxRowIndexPtr", getMaxRowIndex());
	
}


int
ParameterTable::findParameterRowIndex(int aId)  // SMR XXX used anywhere??
{
  // search parameter list to find parameter with aId.
  // return the mRowIndex for that parameter - i.e. which row in
  // the table represents that parameter
  // return -1 if parameter not found

  Parameter *lParamPtr;

  QPtrListIterator<Parameter> mParamIterator( mParamList );	    
  mParamIterator.toFirst();
  while ( (lParamPtr = mParamIterator.current()) != 0)
  {
    if (lParamPtr->getId() == aId)
    {
      return lParamPtr->getRowIndex();
    }
    ++mParamIterator;
  }

  return -1;

}


Parameter *
ParameterTable::findParameter(int aId)
{
  // search parameter list to find parameter with aId.
  // return pointer to that parameter
  // return kNULL is parameter not in list

  Parameter *lParamPtr;

  QPtrListIterator<Parameter> mParamIterator( mParamList );	    
  mParamIterator.toFirst();
  while ( (lParamPtr = mParamIterator.current()) != 0)
  {
    if (lParamPtr->getId() == aId)
    {
      return lParamPtr;
    }
    ++mParamIterator;
  }

  return kNULL;

}

void
ParameterTable::clearAndDisableForDetach(const bool aUnRegister)
{

  // set all values in "Register?" column to "No" as application has detached

  Parameter *lParamPtr;
    
  QPtrListIterator<Parameter> mParamIterator( mParamList );	    
  mParamIterator.toFirst();

  while ( aUnRegister && (lParamPtr = mParamIterator.current()) != 0)
  {
    setText(lParamPtr->getRowIndex(), kREG_COLUMN, "No");
    ++mParamIterator;
  }

}

SteeredParameterTable::SteeredParameterTable(QWidget *aParent, const char *aName, int aSimHandle)
  : ParameterTable(aParent, aName, aSimHandle)
{
  DBGCON("SteeredParameterTable");

  // set up signal/slot to enable buttons when table has some data
  connect(this, SIGNAL(enableButtonsSignal()), aParent, SLOT(enableParamButtonsSlot()));

}

SteeredParameterTable::~SteeredParameterTable()
{
  DBGDST("SteeredParameterTable");
}
  
void
SteeredParameterTable::initTable()
{  
  // set up the steered parameters table - columns headings, readonly status of columns etc.

  setNumInitRows(kINIT_ROWS);
  setNumRows(kINIT_ROWS);
  setNumCols(kNUM_STEER_COLUMNS);

  // initially set all rows read only as all empty 
  // SMR XXX all this init rows stuff hopefully will be improved
  for (int i=0; i<kINIT_ROWS; i++)
    setRowReadOnly(i, TRUE);
 
  setShowGrid(FALSE);
  verticalHeader()->hide();
  setLeftMargin(0);

  horizontalHeader()->setLabel(kID_COLUMN, "ID");
  horizontalHeader()->setLabel(kNAME_COLUMN, "Name"); 
  horizontalHeader()->setLabel(kREG_COLUMN, "Registered?");
  horizontalHeader()->setLabel(kVALUE_COLUMN, "Value");
  horizontalHeader()->setLabel(kNEWVALUE_COLUMN, "New Value");
  horizontalHeader()->setLabel(kSPLIT, "");

  setColumnReadOnly(kID_COLUMN, TRUE);
  setColumnReadOnly(kNAME_COLUMN, TRUE);
  setColumnReadOnly(kREG_COLUMN, TRUE);
  setColumnReadOnly(kVALUE_COLUMN, TRUE);

  hideColumn(kID_COLUMN);
  setColumnWidth(kREG_COLUMN, 90);
  setColumnWidth(kNAME_COLUMN, 200);
  setColumnWidth(kVALUE_COLUMN, 120);
  setColumnWidth(kNEWVALUE_COLUMN, 120);
  setColumnWidth(kSPLIT, 10);

  // set up signal/slot to handle data entered by user (new parameter value)
  connect (this, SIGNAL( valueChanged(int,int) ),
	   this, SLOT( validateValueSlot(int,int) ) );

}


void 
SteeredParameterTable::validateValueSlot( int aRow, int aCol )
{
  // validate the value entered by user and set on gui, 
  // otherwise just post invalid msg and clear cell of invalid entry

  // Note: we do not store this new value on the parameter class - when we commit
  // the value to the library in order to emit to application we simply get the value from
  // the table and send it - any validation is done on entering the value, and the value is sent
  // as a char* so no advantage to storing in parameter class
 
  try
  {

  // Note: only care about new value column
  // and only care is app is still attached
  if (aCol == kNEWVALUE_COLUMN && getAppAttached() )
  {
    bool lOk = true;

    // get parameter ID from hidden column and use this get parameter from the list
    int lId = this->text( aRow, kID_COLUMN ).toInt(&lOk);
    if (!lOk)
      THROWEXCEPTION("Failed to get parameter ID from row in table");
    
    Parameter *lParamPtr = findParameter(lId);
    if  (lParamPtr == kNULL)
      THROWEXCEPTION("Failed to find parameter in list");

    // validate what user has entered - currently just validate it is correct type
    // SMR XXX future - add more validation when range supported - will mean calling parameter member func
    // as ranges will be held on parameter class
    if (lOk)
    {
      // always allow empty entry - means user is clearing the cell.
      if (!this->text( aRow, aCol ).isEmpty())
      {
	switch(lParamPtr->getType())
	{
          case REG_INT:
	    this->text( aRow, aCol ).toInt( &lOk );
	    break;
          case REG_FLOAT:
	    this->text( aRow, aCol ).toFloat( &lOk );
	    break;
          case REG_DBL:
	    this->text( aRow, aCol ).toDouble( &lOk );
	    break;
          case REG_CHAR:
	    break;
          default:
	    THROWEXCEPTION("Unknown parameter type");
	}
      }

      if (!lOk)
      {
	QMessageBox::information(0, "Invalid Parameter Value", "The entered value is not valid",
				 QMessageBox::Ok,
				 QMessageBox::NoButton, 
				 QMessageBox::NoButton);
	setText( aRow, aCol, QString::null );
	// SMR XXX - focus moves to cell below - fix so current cell remains selected - to do
      }

    } //lOk
  } //if kNEWVALUE_COLUMN


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


void 
SteeredParameterTable::addRow(const int lHandle, const char *lLabel, const char *lVal, const int lType)
{

  // add new steered parameter to table and list

  int lRowIndex = getMaxRowIndex();

  if (lRowIndex==0)
    emit enableButtonsSignal();

  if (lRowIndex >= getNumInitRows())
    insertRows(lRowIndex);
  else
    setRowReadOnly(lRowIndex, FALSE);  // set true in initTable SMR XXX rm when initRows goes

  Parameter *lParamPtr = new Parameter(lHandle, lType, true);
	     
  setText(lRowIndex, kID_COLUMN, 
	     QString::number(lHandle) );
  setText(lRowIndex, kNAME_COLUMN, lLabel);
  setText(lRowIndex, kREG_COLUMN, "Yes");

  setItem(lRowIndex, kVALUE_COLUMN, 
	     new QTableItem(this, QTableItem::Never,  QString( lVal)));
  setItem(lRowIndex, kNEWVALUE_COLUMN,
	     new QTableItem(this, QTableItem::OnTyping,  QString::null));
	   

  lParamPtr->setIndex(lRowIndex);
  mParamList.append(lParamPtr);
  incrementRowIndex();
	     
  // SMR XXX - code duplication in this func - rewrite ParameterTable func so can call that
  // and just deal with new value column here
  ///  ParameterTable::addRow(lHandle, lLabel, lVal, lType);
  ///setItem(lRowIndex, kNEWVALUE_COLUMN,
  ///	  new QTableItem(this, QTableItem::WhenCurrent,  QString::null));
	   

  DBGMSG1("Steer MaxRowIndex Ptr", getMaxRowIndex());
   
}


int
SteeredParameterTable::setNewParamValuesInLib()
{
  // call ReG library function to set the new parameter values as per set in the GUI
  // get the value direct from gui table 
  // clear all new value cells in table once values have been set
  // return the number of parameters set
  
  // Note: this only sets the values in the library - the "emit" of the new values
  // to the steered application is done elsewhere via Emit_control()

  if (getMaxRowIndex()==0)
    return 0;

  Parameter *lParamPtr;
  int lCount=0;
  QPtrListIterator<Parameter> mParamIterator( mParamList );	    
  mParamIterator.toFirst();

  int *lHandles = kNULL;
  char  **lVals = kNULL;
  int lIndex=0;


  try
  {

    // count up parameters that have new values
    while ( (lParamPtr = mParamIterator.current()) != 0)
    {
      if (lParamPtr->isSteerable())
      {
	// SMR XXX - note empty string will not be counted as new value - isEmpty is true for empty string
	// this means never possible to set string parameter to empty string - may need revisit
	if (!(text(lParamPtr->getRowIndex(), kNEWVALUE_COLUMN).isEmpty()))
	lCount++;
      }    
      ++mParamIterator;
    }
    
    
    if (lCount >0)
    {
      // populate arrays for ReG library call Set_param_values
      lHandles = new int [lCount];
      lVals = new char *[lCount];
      
      int lLength=0;
      mParamIterator.toFirst();
      while ( (lParamPtr = mParamIterator.current()) != 0)
      {
	if (lParamPtr->isSteerable() && 
	    !(this->text(lParamPtr->getRowIndex(), kNEWVALUE_COLUMN).isEmpty()))
	{
	  lHandles[lIndex] = lParamPtr->getId();
	  lLength = this->text(lParamPtr->getRowIndex(), kNEWVALUE_COLUMN).length();
	  lVals[lIndex] = new char[lLength + 1];
	  
	  sprintf(lVals[lIndex], "%s", 
		  this->text(lParamPtr->getRowIndex(), kNEWVALUE_COLUMN).latin1());
	  
	  lIndex++;
	}
      
	++mParamIterator;
      }
      
      // set the values in the steering library
      if (Set_param_values(getSimHandle(),			//ReG library
			   lIndex,
			   lHandles,
			   lVals) != REG_SUCCESS)
      {
	THROWEXCEPTION("Set_param_values");
      }

      // clear the cells in the table
      clearNewValues();
    
      // clean up
      delete [] lHandles;
      for(int i=0; i<lIndex; i++){
	delete [] lVals[i];
      }
      delete [] lVals;
      
    } // if lCount>0
 
  } //try

  catch (SteererException StEx)
  {
    // clean up
    delete [] lHandles;
    for(int i=0; i<lIndex; i++){
      delete [] lVals[i];
    }
    delete [] lVals;

    throw StEx;
  }
  
  DBGMSG1("setNewParamValues, lCount = ", lCount);
  return lIndex;

} // ::setNewParamValuesInLib()

void
SteeredParameterTable::emitValuesSlot()
{
  // set the new parameter values in the library as per set in the GUI
  // and "emit" these to the steered application
  // the new value cells in the table will be cleared as part of this
  // ready for user to enter the next values

  try
  {

    // set the values in the library
    if (setNewParamValuesInLib() > 0)
    {
      // call ReG library function to "emit" values to steered application
      if (Emit_control(getSimHandle(),		//ReG library
		       0,
		       NULL) != REG_SUCCESS)
	THROWEXCEPTION("Emit_contol");
      
      // note: steerer has no control over when the application will actually read these new values
      // thus there will be an indeterminate delay between emitting the values and the steerer gui
      // showing that value as the parameters current value
      // logging of what has been emitted would be very useful for the user here. SMR XXX
      
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
SteeredParameterTable::clearNewValues()
{
  // clear all the (editable) new value cells in the table
  // this is done when the values have been emitted to the application

  Parameter *lParamPtr;
    
  QPtrListIterator<Parameter> mParamIterator( mParamList );	    
  mParamIterator.toFirst();
  while ( (lParamPtr = mParamIterator.current()) != 0)
  {
    int lRowIndex = lParamPtr->getRowIndex();
    item(lRowIndex, kNEWVALUE_COLUMN)->setText(QString::null);
    updateCell(lRowIndex, kNEWVALUE_COLUMN);
    ++mParamIterator;
  }
  
}

void 
SteeredParameterTable::clearAndDisableForDetach(const bool aUnRegister)
{
  // clear all the (editable) new value cells in the table
  // and make them readonly
  // this is done when the the application has detached
  // and hence is not longer steerable

  // need to flag that app has detached to prevent any further validation of 
  // values user is currently editing
  setAppDetached();

  Parameter *lParamPtr;
    
  QPtrListIterator<Parameter> mParamIterator( mParamList );	    
  mParamIterator.toFirst();
  while ( (lParamPtr = mParamIterator.current()) != 0)
  {
    int lRowIndex = lParamPtr->getRowIndex();
    if (aUnRegister)
      setText(lRowIndex, kREG_COLUMN, "No");

    // reset focus of table to top left cell if this cell is currently selected
    // if do not do this the setItem statement does not work
    if (isSelected(lRowIndex, kNEWVALUE_COLUMN))
      setCurrentCell(0, kNAME_COLUMN);
    
    // clear new value cells
    item(lRowIndex, kNEWVALUE_COLUMN)->setText(QString::null);
    updateCell(lRowIndex, kNEWVALUE_COLUMN);
    ++mParamIterator;
  }

  // set table read only
  setReadOnly(TRUE); 

}


