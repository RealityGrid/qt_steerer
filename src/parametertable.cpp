/*----------------------------------------------------------------------------
    ParameterTable and SteeredParameterTable classes for QT steerer GUI.

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

#include "exception.h"
#include "parametertable.h"

#include <qapplication.h>
#include <qmessagebox.h>
#include <qtooltip.h>
#include <qpopupmenu.h>

#include "historyplot.h"
  
ParameterTable::ParameterTable(QWidget *aParent, const char *aName, int aSimHandle)
  : Table(aParent, aName, aSimHandle)
{
  DBGCON("ParameterTable");

  // setAutoDelete so QT deletes each obj in list when the list is deleted
  mParamList.setAutoDelete( TRUE );
  mHistoryPlotList.setAutoDelete( TRUE );

  // create table to display all monitored parameters
  // as well as displaying parameters in a table (one per row), a list of parameter objects
  // is also held on the class
  // each parameter object holds the row id  of the row in the parameter table
  // that represents that parameter
  // each table row holds the parameter id in a column hidden form the user

  // MR:
  // Playing with the default minimum table heights as with IOTypeTable
  setMinimumHeight(100);
  setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

}  

ParameterTable::~ParameterTable()
{
  DBGDST("~ParameterTable");
}

void
ParameterTable::initTable()
{  
  // set up the monitored parameters table - columns headings, readonly status of columns etc.

  ///SMRXXX  setMinimumSize(QSize(900,900));

  // MR: removed - was causing problems, and giving no noticeable benefit
  //setNumInitRows(kINIT_ROWS);
  //setNumRows(kINIT_ROWS);
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
  hideColumn(kREG_COLUMN);
  setColumnWidth(kREG_COLUMN, 90);
  setColumnWidth(kNAME_COLUMN, 200);
  setColumnWidth(kVALUE_COLUMN, 95);

  // MR: add a context menu so that we can right click on a cell to draw a graph of that parameter's history
  connect(this, SIGNAL(contextMenuRequested(int, int, const QPoint &)), this, SLOT(contextMenuSlot(int, int, const QPoint &)));

  
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

    // MR: add the string value to the parameter's history
    lParamPtr->mParamHist.updateParameter(lVal);

    // MR: emit a SIGNAL so that any HistoryPlots can update
    emit paramUpdateSignal(&(lParamPtr->mParamHist), lParamPtr->getId());
    
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

  // MR: removed - was causing problems, and giving no noticeable benefit
  //if (lRowIndex >= getNumInitRows())
    insertRows(lRowIndex);

  Parameter *lParamPtr = new Parameter(lHandle, lType, false);
  
  setText(lRowIndex, kID_COLUMN, 
	  QString::number(lHandle) );
  setText(lRowIndex, kNAME_COLUMN, lLabel);
  setText(lRowIndex, kREG_COLUMN, "Yes");
  
  setItem(lRowIndex, kVALUE_COLUMN, new QTableItem(this, QTableItem::Never,  QString( lVal)));
    
  lParamPtr->setIndex(lRowIndex);
  // MR: add the string value to the parameter's history
  lParamPtr->mParamHist.updateParameter(lVal);
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
  // return kNULL if parameter not in list

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

// MR: reverse lookup of parameter ID
Parameter* ParameterTable::findParameterHandleFromRow(int row){
  // search through the parameter list for the parameter which has the given row index
  // return the id of that parameter
  // return kNULL if parameter is not in the list

  Parameter *lParamPtr;

  QPtrListIterator<Parameter> mParamIterator( mParamList );
  mParamIterator.toFirst();
  while ( (lParamPtr = mParamIterator.current()) != 0){
    if (lParamPtr->getRowIndex() == row){
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

/** Slot called when the user requests a context menu for this table
 */
void ParameterTable::contextMenuSlot(int row, int column, const QPoint &pnt){
  // Pop up a context menu which will allow the user to view a graph
  // of the current variable / parameter's history
  QPopupMenu popupMenu;
  popupMenu.insertItem(QString("Draw History Graph"), row, 0);
  connect(&popupMenu, SIGNAL(activated(int)), this, SLOT(drawGraphSlot(int)));
  popupMenu.exec(pnt);

  // Do daft things in order to avoid daft compiler warnings....
  row++; column++;
}

/** Slot called when the user selects the "Draw Graph" option from
 *  the table's context menu
 */
void ParameterTable::drawGraphSlot(int popupMenuID){
  // First obtain the appropriate parameter (and therefore it's history)
  Parameter *tParameter = findParameterHandleFromRow(popupMenuID);

  // Then call our whizzo graphing method to draw the graph
  // need to keep a reference to the plotter so that it's cancelled when we quit the main window
  mQwtPlot = new HistoryPlot(&(tParameter->mParamHist), text(popupMenuID, kNAME_COLUMN).latin1(), tParameter->getId());
  mHistoryPlotList.append(mQwtPlot);
  mQwtPlot->show();

  // And make the connection to ensure that the graph updates
  connect(this, SIGNAL(paramUpdateSignal(ParameterHistory *, const int)), mQwtPlot, SLOT(updateSlot(ParameterHistory*, const int)));
}



/*************************************************************/
/* DynamicTip Class                                          */
/*                                                           */
/* MR - In order to have the tool tips respond to changes    */
/* in the steerable paramater table we need to subclass      */
/* qtooltip. This is strongly associated with the steerable  */
/* parameter table, so will remain in this file with it.     */
/*************************************************************/


class DynamicTip: public QToolTip{
private:
    QRect rect;
public:
    DynamicTip( QWidget * parent, QRect rect );
    static void add(QWidget *widget, const QRect &rect, const QString &text);

protected:
    void maybeTip( const QPoint & );
};

DynamicTip::DynamicTip( QWidget * parent, QRect _rect )
    : QToolTip( parent )
{
  rect = _rect;
    // no explicit initialization needed
}

void DynamicTip::maybeTip( const QPoint &pos )
{
    if ( !parentWidget()->inherits( "SteeredParameterTable" ) )
        return;

    QRect tmp;
    QString str;
    int tipOK = ((SteeredParameterTable*)parentWidget())->getTip(pos, tmp, str);
    if (tipOK == 0)
      tip(tmp, str);
}


/*************************************************************/
/* End of DynamicTip Class                                   */
/*************************************************************/




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

  // MR: removed - was causing problems, and giving no noticeable benefit
  //setNumInitRows(kINIT_ROWS);
  //setNumRows(kINIT_ROWS);
  setNumCols(kNUM_STEER_COLUMNS);

  // MR: removed - was causing problems, and giving no noticeable benefit
  // initially set all rows read only as all empty 
  // SMR XXX all this init rows stuff hopefully will be improved
  //for (int i=0; i<kINIT_ROWS; i++)
  //  setRowReadOnly(i, TRUE);
 
  setShowGrid(FALSE);
  verticalHeader()->hide();
  setLeftMargin(0);

  horizontalHeader()->setLabel(kID_COLUMN, "ID");
  horizontalHeader()->setLabel(kNAME_COLUMN, "Name"); 
  horizontalHeader()->setLabel(kREG_COLUMN, "Registered?");
  horizontalHeader()->setLabel(kVALUE_COLUMN, "Value");
  horizontalHeader()->setLabel(kNEWVALUE_COLUMN, "New Value");

  setColumnReadOnly(kID_COLUMN, TRUE);
  setColumnReadOnly(kNAME_COLUMN, TRUE);
  setColumnReadOnly(kREG_COLUMN, TRUE);
  setColumnReadOnly(kVALUE_COLUMN, TRUE);

  // id column used internally only, therefore not show on gui
  hideColumn(kID_COLUMN);
  // unregister functionality not implemented in lib therefore no point in showing kREG_COLUMN
  hideColumn(kREG_COLUMN);

  setColumnWidth(kREG_COLUMN, 90);
  setColumnWidth(kNAME_COLUMN, 200);
  setColumnWidth(kVALUE_COLUMN, 95);
  setColumnWidth(kNEWVALUE_COLUMN, 95);

  // set up signal/slot to handle data entered by user (new parameter value)
  connect (this, SIGNAL( valueChanged(int,int) ),
	   this, SLOT( validateValueSlot(int,int) ) );

  // Create a DynamicTip object for this table
  new DynamicTip(this, frameRect());

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

  // MR:
  QString newVal = text(aRow, aCol);
 
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

    bool noMinValue = false;
    bool noMaxValue = false;
    const char *minString = lParamPtr->getMinString().latin1();
    const char *maxString = lParamPtr->getMaxString().latin1();
    if (strcmp(minString, "--") == 0)
      noMinValue = true;
    if (strcmp(maxString, "--") == 0)
      noMaxValue = true;
      
    // validate what user has entered - currently just validate it is correct type
    // SMR XXX future - add more validation when range supported - will mean calling parameter member func
    // as ranges will be held on parameter class
    if (lOk)
    {
      // always allow empty entry - means user is clearing the cell.
      if (!newVal.isEmpty())
      {
        // MR: contains tests to check if the new values are legal
        switch(lParamPtr->getType())
        {
            case REG_INT:
            {
              int newInt = newVal.toInt( &lOk );
              // Now check to see if the values the user's entered are legal
              if (lOk && !noMinValue){
                 int min = atoi(minString);
                 if (newInt < min)
                   lOk = false;
              }
              if (lOk && !noMaxValue){
                int max = atoi(maxString);
                if (newInt > max)
                  lOk = false;
              }
              break;
            }
              
            case REG_FLOAT:
            {
              float newFloat = newVal.toFloat( &lOk );
              // Now check to see if the values the user's entered are legal
              if (lOk && !noMinValue){
                 float min = float(atof(minString));
                 if (newFloat < min)
                   lOk = false;
              }
              if (lOk && !noMaxValue){
                float max = float(atof(maxString));
                if (newFloat > max)
                  lOk = false;
              }
              break;
            }
              
            case REG_DBL:
            {
              double newDouble = newVal.toDouble( &lOk );
              // Now check to see if the values the user's entered are legal
              if (lOk && !noMinValue){
                 double min = atof(minString);
                 if (newDouble < min)
                   lOk = false;
              }
              if (lOk && !noMaxValue){
                double max = atof(maxString);
                if (newDouble > max)
                  lOk = false;
              }
              break;
            }
            
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
SteeredParameterTable::addRow(const int lHandle, const char *lLabel, const char *lVal, const int lType, const char *lMinVal, const char *lMaxVal)
{

  // add new steered parameter to table and list

  int lRowIndex = getMaxRowIndex();

  if (lRowIndex==0)
    emit enableButtonsSignal();

  // MR: removed - was causing problems, and giving no noticeable benefit
  //if (lRowIndex >= getNumInitRows())
    insertRows(lRowIndex);
  //else
  //  setRowReadOnly(lRowIndex, FALSE);  // set true in initTable SMR XXX rm when initRows goes

  Parameter *lParamPtr = new Parameter(lHandle, lType, true);
  lParamPtr->setMinMaxStrings(lMinVal, lMaxVal);
	     
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
      
      int lReGStatus = REG_FAILURE;

      qApp->lock();
      // set the values in the steering library
      lReGStatus = Set_param_values(getSimHandle(),			//ReG library
				    lIndex,
				    lHandles,
				    lVals);
      qApp->unlock();

      if (lReGStatus != REG_SUCCESS)
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

void SteeredParameterTable::emitValuesSlot()
{
  // set the new parameter values in the library as per set in the GUI
  // and "emit" these to the steered application
  // the new value cells in the table will be cleared as part of this
  // ready for user to enter the next values

  int lReGStatus = REG_FAILURE;
  try
  {

    // set the values in the library
    if (setNewParamValuesInLib() > 0)
    {

      qApp->lock();
      // call ReG library function to "emit" values to steered application
      lReGStatus = Emit_control(getSimHandle(),		//ReG library
				0,
				NULL,
				NULL);
      qApp->unlock();
     
      if (lReGStatus != REG_SUCCESS)
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


/** This method is called by the DynamicTip class to find which String to use for a
 *  tooltip, and how big a rectangle to make it for
 */
int SteeredParameterTable::getTip(const QPoint &pnt, QRect &rect, QString &string){

  // Check that we're in the correct column
  if (columnAt(pnt.x()) != kNEWVALUE_COLUMN)
    return -1;

  // Get the rows and columns
  int singleRowHeight = verticalHeader()->sectionSize(0); // They're all the same
  int scrolled = contentsY();
    
  // Check that we're not in the title bar
  if (pnt.y() < singleRowHeight)
    return -1;

  // headerHeight is the vertical size of the horizontal table header
  int headerHeight = horizontalHeader()->height();
  // adjustedPos is the mouse coordinate moved from the table 
  // coordinate system to the scroll window coordinate system
  int adjustedPos = scrolled + (pnt.y() - headerHeight);

  // need to check that the singleRowHeight is not 0, this can happen
  // if there's no data in the table. If it is zero we end up with a
  // divide by zero exception below !
  if (singleRowHeight == 0){
    return -1;
  }
  // actualRow contains the index to the proper row in the scrollview
  int actualRow = adjustedPos / singleRowHeight;

  // Check that we're not below last row (actualRow is zero-indexed
  // but getMaxRowIndex() returns actual no. of populated rows)
  if (actualRow >= getMaxRowIndex()) 
    return -1;

  // We now want to determine the actual rectangle for the tooltip
  int top, bottom, left, right;
  top = (actualRow * singleRowHeight) - scrolled + headerHeight;
  bottom = top + singleRowHeight;
  left = horizontalHeader()->sectionPos(kNEWVALUE_COLUMN); // constant
  right = left + horizontalHeader()->sectionSize(kNEWVALUE_COLUMN);

  // Set the rectangle
  rect.setTop(top);
  rect.setBottom(bottom);
  rect.setLeft(left);
  rect.setRight(right);

  // And grab the text from the Paramter object and send it back to the tooltip
  // get parameter ID from hidden column and use this get parameter from the list
  bool lOk;
  int lId = this->text( actualRow, kID_COLUMN ).toInt(&lOk);
  if (!lOk){
    THROWEXCEPTION("Failed to get parameter ID from row in table");
  }

  Parameter *lParamPtr = findParameter(lId);
  if  (lParamPtr == kNULL){
    THROWEXCEPTION("Failed to find parameter in list");
  }

  QString minStr = lParamPtr->getMinString();
  QString maxStr = lParamPtr->getMaxString();
  
  if (minStr.compare(QString("--")) == 0){
    if (maxStr.compare(QString("--")) == 0){
      // then they're both untested so let the user know they can add anything
      switch(lParamPtr->getType())
      {
          case REG_INT:
          {
            string = QString("Any integer...");
            break;
          }
          case REG_FLOAT:
          {
            string = QString("Any float...");
            break;
          }
          case REG_DBL:
          {
            string = QString("Any double...");
            break;
          }
          case REG_CHAR:
          {
            string = QString("Any string...");
            break;
          }
      }
    }
    else {
      // then theres no upper bounds
      string = QString("? <= "+maxStr);
    }
  }
  else if (maxStr.compare(QString("--")) == 0){
    // then theres no lower bounds
    string = QString(minStr+" <= ?");
  }
  else
    // there are both upper and lower bounds
    string = QString(minStr+" <= ? <= "+maxStr);
 
  // Return success!
  return 0;
}

/** Method is specific to the SteeredParameterTable, and implemented
 *  to prevent the version in the ParamaterTable parent being inherited
 */
void SteeredParameterTable::contextMenuSlot(int row, int column, const QPoint &pnt){
  // don't implement this unless we need to

  // Do daft things in order to avoid daft compiler warnings....
  row++; column+=pnt.x();
}

