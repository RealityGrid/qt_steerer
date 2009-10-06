/*
  The RealityGrid Steerer

  Copyright (c) 2002-2009, University of Manchester, United Kingdom.
  All rights reserved.

  This software is produced by Research Computing Services, University
  of Manchester as part of the RealityGrid project and associated
  follow on projects, funded by the EPSRC under grants GR/R67699/01,
  GR/R67699/02, GR/T27488/01, EP/C536452/1, EP/D500028/1,
  EP/F00561X/1.

  LICENCE TERMS

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of The University of Manchester nor the names
      of its contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

  Author: Mark Riding
          Andrew Porter
          Robert Haines
 */

/** @file historyplot.cpp
    @brief Source file for HistoryPlot class - container for one or more
    history plots on a single canvas 
    @author Mark Riding
    @author Andrew Porter */
#include <iostream>
//Added by qt3to4:
#include <Q3PointArray>
#include <QCloseEvent>
#include <Q3PopupMenu>
#include <Q3VBoxLayout>
#include <Q3Frame>
using namespace std;

#include "historysubplot.h"
#include "historyplot.h"
#include "parameterhistory.h"

#include "qprinter.h"
#include "qinputdialog.h"
#include "qwt_symbol.h"
#include "qwt_picker.h"
#include "qwt_legend.h"
#include "qwt_scale_div.h"
#include "q3filedialog.h"
#include "q3textstream.h"
#include <qmessagebox.h>
#include "qcolor.h"

#include "debug.h"

HistoryPlot::HistoryPlot(ParameterHistory *_mXParamHist, 
			 ParameterHistory *_mYParamHist,
			 const char *_lLabelx,
			 const char *_lLabely, 
			 const int _xparamID, 
			 const int _yparamID,
			 const char *_lComponentName)
  : Q3Frame(0,0,0), mXParamHist(_mXParamHist)
{
  // Local copies of passed parameters
  xparamID = _xparamID;
  strcpy(mLabelx, _lLabelx);

  // Set the caption to be something descriptive - include
  // name of the component being steered (same as the tab
  // title and set in SteererMainWindow::editTabTitleSlot)
  setCaption(QString(_lLabely)+" of "+QString(_lComponentName));
    
  // Create a plotter widget
  mPlotter = new QwtPlot(this);

  mPlotter->setCanvasBackground(Qt::darkGray);

  // Set axis titles
  mPlotter->setAxisTitle(mPlotter->xBottom, mLabelx);
  // We're now able to have >1 curve (with key) so labelling of
  // y axis is inappropriate
  //mPlotter->setAxisTitle(mPlotter->yLeft, _lLabely);

  Q3VBoxLayout *tBL = new Q3VBoxLayout(this);
  mMenuBar = new QMenuBar(this, "menuBar");

  mFileMenu = new Q3PopupMenu(this, "filePopup");
  mFileMenu->insertItem("&Print", this, SLOT(filePrint()), Qt::CTRL+Qt::Key_P);
  mFileMenu->insertItem("&Save", this, SLOT(fileSave()), Qt::CTRL+Qt::Key_S);
  mFileMenu->insertItem("Save da&ta", this, SLOT(fileDataSave()), 
			Qt::CTRL+Qt::Key_T);
  mFileMenu->insertSeparator();
  mFileMenu->insertItem("&Close", this, SLOT(fileQuit()), Qt::CTRL+Qt::Key_C);

  mGraphMenu = new Q3PopupMenu(this, "graphPopup");

  mAutoYAxisId = mGraphMenu->insertItem("&Auto Y Axis", this, 
					SLOT(autoYAxisSlot()), Qt::CTRL+Qt::Key_A);
  mYUpperBoundId = mGraphMenu->insertItem("Define Y &upper-bound", this, 
					  SLOT(graphYUpperBoundSlot()), 
					  Qt::CTRL+Qt::Key_U);
  mYLowerBoundId = mGraphMenu->insertItem("Define Y &lower-bound", this, 
					  SLOT(graphYLowerBoundSlot()),
 					  Qt::CTRL+Qt::Key_L);
  mToggleLogYId = mGraphMenu->insertItem("Toggle use of L&og Y axis", this, 
					 SLOT(toggleLogAxisYSlot()), 
					 Qt::CTRL+Qt::Key_O);

  mGraphMenu->insertSeparator();
  
  mAutoXAxisId = mGraphMenu->insertItem("&Auto X Axis", this, 
					SLOT(autoXAxisSlot()), Qt::ALT+Qt::Key_A);
  mXUpperBoundId = mGraphMenu->insertItem("Define X &upper-bound", this, 
					  SLOT(graphXUpperBoundSlot()), 
					  Qt::ALT+Qt::Key_U);
  mXLowerBoundId = mGraphMenu->insertItem("Define X &lower-bound", this, 
					  SLOT(graphXLowerBoundSlot()), 
					  Qt::ALT+Qt::Key_L);
  mToggleLogXId = mGraphMenu->insertItem("Toggle use of L&og X axis", this, 
					 SLOT(toggleLogAxisXSlot()), 
					 Qt::ALT+Qt::Key_O);
  mGraphMenu->insertSeparator();

  mShowSymbolsId = mGraphMenu->insertItem("Toggle &display of symbols", this,
					  SLOT(graphDisplaySymbolsSlot()),
					  Qt::ALT+Qt::Key_D);

  mShowCurvesId = mGraphMenu->insertItem("Toggle display of l&ines", this,
					 SLOT(graphDisplayCurvesSlot()),
					 Qt::ALT+Qt::Key_I);

  mGraphMenu->setItemChecked(mAutoYAxisId, true);
  mGraphMenu->setItemChecked(mAutoXAxisId, true);
  mGraphMenu->setItemEnabled(mYUpperBoundId, false);
  mGraphMenu->setItemEnabled(mYLowerBoundId, false);
  mGraphMenu->setItemEnabled(mXUpperBoundId, false);
  mGraphMenu->setItemEnabled(mXLowerBoundId, false);
  mGraphMenu->setItemChecked(mShowSymbolsId, true);
  mGraphMenu->setItemChecked(mShowCurvesId, true);
  mGraphMenu->setItemChecked(mToggleLogXId, false);
  mGraphMenu->setItemChecked(mToggleLogYId, false);
    
  mMenuBar->insertItem("&File", mFileMenu);
  mMenuBar->insertItem("&Graph", mGraphMenu);

  tBL->setMenuBar(mMenuBar);
  tBL->addWidget(mPlotter);

    // Better without this for qwt 4.2.0
    //resize(300, 300);

  mYLowerBound = mYUpperBound = 0;
  mXLowerBound = mXUpperBound = 0;
  mAutoYAxisSet = true;
  mAutoXAxisSet = true;
  mUseLogXAxis = false;
  mUseLogYAxis = false;
  mForceHistRedraw = false;
  // Default to displaying symbols
  mDisplaySymbolsSet = true;
  // Default to displaying a curve too
  mDisplayCurvesSet   = true;

  mPicker = new QwtPicker(mPlotter->canvas());

  mPicker->setSelectionFlags(QwtPicker::PointSelection | 
			     QwtPicker::ClickSelection);

  if(!( connect(mPicker, SIGNAL(selected(const Q3PointArray &)), 
		this, SLOT(canvasSelectedSlot(const Q3PointArray &))) )){
    cout << "ERROR: failed to connect signal to slot in HistoryPlot "
      "constructor" << endl;
  }

  // Let the list own the objects
  mSubPlotList.setAutoDelete( TRUE );

  mColourList = QColor::colorNames();
  QStringList::Iterator it = mColourList.begin();
  while( it != mColourList.end() ) {
    // Remove this colour from the list because we've chosen this to
    // be the colour of our canvas (earlier) or it's hard to tell 
    // apart from one of the other standard colours.
    if((*it).contains("darkGray", FALSE) || 
       (*it).contains("antiquewhite", FALSE)){
      it = mColourList.remove(it);
    }
    else{
      it++;
    }
  }

  //  for ( QStringList::Iterator it = mColourList.begin(); 
  //	it != mColourList.end(); ++it ) {
  //    cout << *it << ":";
  //  }
  //cout << endl;
  mColourIter = mColourList.begin();

  // Create the HistorySubPlot object that will look after drawing this curve
  mSubPlotList.append(new HistorySubPlot(this, mPlotter,
					 _mXParamHist,
					 _mYParamHist,
					 QString(_lLabely),
					 _yparamID,
		                         *mColourIter));
  ++mColourIter;

  doPlot();
}

//--------------------------------------------------------------------
HistoryPlot::~HistoryPlot()
{
  REG_DBGDST("HistoryPlot");
  delete mPicker;
}

//--------------------------------------------------------------------
void HistoryPlot::canvasSelectedSlot(const Q3PointArray &pos)
{
  emit plotSelectedSignal(this);
}

//--------------------------------------------------------------------
/** Menu handling slots
 */
void HistoryPlot::filePrint(){
  QPrinter lPrinter;
  mPlotter->print(lPrinter, QwtPlotPrintFilter());
}

//--------------------------------------------------------------------
void HistoryPlot::fileSave(){

  QString lFileName = Q3FileDialog::getSaveFileName(".", "Images (*.jpg)", 0, 
						   "save file dialog", 
						   "Choose a filename to save the image as");
  // ensure the user gave us a sensible file
  if (!lFileName.isNull()){
    // ensure the file has a .jpg extension
    if (!lFileName.endsWith(".jpg"))
      lFileName.append(".jpg");

    // spawn off a thread to take the screenshot, giving QT enough time
    // to redraw the graph
    ScreenGrabThread *lScreenGrabThread = new ScreenGrabThread(this, 
							       lFileName, 
							       mMenuBar);
    lScreenGrabThread->start();
  }
}

//--------------------------------------------------------------------
void HistoryPlot::fileDataSave(){

  int     i;
  HistorySubPlot *plot;

  QString lFileName = Q3FileDialog::getSaveFileName(".", "Data (*.dat)", 0, 
						   "save file dialog", 
						   "Choose a name for the data file");
  // ensure the user gave us a sensible file
  if (!lFileName.isNull()){

    // ensure the file has a .dat extension
    if (!lFileName.endsWith(".dat"))
      lFileName.append(".dat");

    QFile file(lFileName);

    if( !file.open( QIODevice::WriteOnly ) ){
      QMessageBox::warning( this, "Saving", "Failed to save file." );
      return;
    }

    Q3TextStream ts( &file );

    // Header for data
    ts << "# Data exported from RealityGrid Qt Steering Client" << endl;
    ts << "# Seq no."; 
    plot = mSubPlotList.first();
    while(plot){
      ts << ", " << plot->getCurveLabel();
      plot = mSubPlotList.next();
    }    
    ts << endl;

    // Work out how many points we've got of 'historical data' - compare 
    // the no. available for each ordinate and use the smaller of the two.
    plot = mSubPlotList.first();
    int lNumPts = plot->mYParamHist->mPreviousHistArraySize;
    plot = mSubPlotList.next();
    while(plot){
      if(lNumPts > plot->mYParamHist->mPreviousHistArraySize){
	lNumPts = plot->mYParamHist->mPreviousHistArraySize;
      }
      plot = mSubPlotList.next();
    }
    if(mXParamHist->mPreviousHistArraySize < lNumPts){
      lNumPts = mXParamHist->mPreviousHistArraySize;
    }
    double *lX = mXParamHist->mPtrPreviousHistArray;

    // The 'historical' data itself
    for(i=0; i<lNumPts; i++){
      ts << lX[i] ;
      for ( plot = mSubPlotList.first(); plot; plot = mSubPlotList.next() ){
	ts << QString("  %1").arg((plot->mYParamHist->mPtrPreviousHistArray)[i], 
				  0, 'e', 8);
      }
      ts << endl;
    }

    // Now do the data we've collected whilst we've been attached
    plot = mSubPlotList.first();
    lNumPts = plot->mYParamHist->mArrayPos;
    plot = mSubPlotList.next();
    while(plot){
      if(lNumPts > plot->mYParamHist->mArrayPos){
	lNumPts = plot->mYParamHist->mArrayPos;
      }
      plot = mSubPlotList.next();
    }
    if(mXParamHist->mArrayPos < lNumPts){
      lNumPts = mXParamHist->mArrayPos;
    }
    lX = mXParamHist->ptrToArray();

    // The data itself
    for(i=0; i<lNumPts; i++){
      ts << lX[i];
      for ( plot = mSubPlotList.first(); plot; plot = mSubPlotList.next() ){
	ts << QString("  %1").arg((plot->mYParamHist->ptrToArray())[i], 0, 'e', 8);
      }
      ts << endl;
    }

    file.close();
  }
}

//--------------------------------------------------------------------
void HistoryPlot::fileQuit(){
  close();
}

//--------------------------------------------------------------------
/** Draw the curves on the graph
 */
void HistoryPlot::doPlot(){

  HistorySubPlot *plot;
  for ( plot = mSubPlotList.first(); plot; plot = mSubPlotList.next() ){
    plot->doPlot(mForceHistRedraw);
  }

  // allow the user to define the Y axis dims if desired
  if (mAutoYAxisSet){
    mPlotter->setAxisAutoScale(0);
  }
  else{
    mPlotter->setAxisScale(0, mYLowerBound, mYUpperBound);
  }

  // allow the user to define the X axis dims if desired
  if (mAutoXAxisSet){

    mPlotter->setAxisAutoScale(mPlotter->xBottom);
  }
  else{
    mPlotter->setAxisScale(mPlotter->xBottom, mXLowerBound, mXUpperBound);
  }

  // Insert a horizontal line at y = 0...
  //long mY = mPlotter->insertLineMarker("y = 0", QwtPlot::yLeft);
  //mPlotter->setMarkerYPos(mY, 0.0);

  mPlotter->replot();

  return;
}

//--------------------------------------------------------------------
/** Add another plot/curve to this history plot */
void HistoryPlot::addPlot(ParameterHistory *_mYParamHist,
			  const char *_lLabely, 
			  const int _yparamID){

  mSubPlotList.append(new HistorySubPlot(this, mPlotter,
					 mXParamHist,
					 _mYParamHist,
					 QString(_lLabely),
					 _yparamID,
		                         *mColourIter));
  ++mColourIter;

  // redraw the plot
  mForceHistRedraw = true;
  doPlot();
}

//--------------------------------------------------------------------
/** Slot to allow user to switch-on autoscaling for the y-axis
 *
 */
void HistoryPlot::autoYAxisSlot(){
  mAutoYAxisSet = !mAutoYAxisSet;

  mGraphMenu->setItemChecked(mAutoYAxisId, mAutoYAxisSet);
  mGraphMenu->setItemEnabled(mYLowerBoundId, !mAutoYAxisSet);
  mGraphMenu->setItemEnabled(mYUpperBoundId, !mAutoYAxisSet);
  mForceHistRedraw = true;
  // redraw the plot
  doPlot();
}

//--------------------------------------------------------------------
/** Slot to allow user to switch-on autoscaling for the x-axis
 *
 */
void HistoryPlot::autoXAxisSlot(){
  mAutoXAxisSet = !mAutoXAxisSet;

  mGraphMenu->setItemChecked(mAutoXAxisId, mAutoXAxisSet);
  mGraphMenu->setItemEnabled(mXLowerBoundId, !mAutoXAxisSet);
  mGraphMenu->setItemEnabled(mXUpperBoundId, !mAutoXAxisSet);
  mForceHistRedraw = true;
  // redraw the plot
  doPlot();
}

//--------------------------------------------------------------------
/** Slot to allow user to set the upper bound on the y axis.
 *
 */
void HistoryPlot::graphYUpperBoundSlot(){
  // using the default double dialog box is a pain, since we have to 
  // worry about the number of decimal points
  bool conversionOk, dialogOk;
  QString tempStr = QInputDialog::getText("Enter Y-axis upper-bound", 
					  "Y upper-bound Dialog", 
					  QLineEdit::Normal, 
					  QString::number(mYUpperBound), 
					  &dialogOk);

  // Check that user pressed Ok and not Cancel
  if (!dialogOk)
    return;
  
  double upperBoundTmp = tempStr.toDouble(&conversionOk);

  // potentially a user could make enough mistakes to reach the stack limit 
  // on the machine, but it's unlikely ;p
  if (!conversionOk){
    graphYUpperBoundSlot();
  }
  else{
    mYUpperBound = upperBoundTmp;
  }
  mForceHistRedraw = true;
  // redraw the plot
  doPlot();
}

//--------------------------------------------------------------------
void HistoryPlot::graphXUpperBoundSlot(){
  // using the default double dialog box is a pain, since we have to 
  // worry about the number of decimal points
  bool conversionOk, dialogOk;
  QString tempStr = QInputDialog::getText("Enter X-axis upper-bound", 
					  "X upper-bound Dialog", 
					  QLineEdit::Normal, 
					  QString::number(mXUpperBound), 
					  &dialogOk);

  // Check that user pressed Ok and not Cancel
  if (!dialogOk)
    return;
  
  double upperBoundTmp = tempStr.toDouble(&conversionOk);

  // potentially a user could make enough mistakes to reach the stack limit 
  // on the machine, but it's unlikely ;p
  if (!conversionOk){
    graphXUpperBoundSlot();
  }
  else{
    mXUpperBound = upperBoundTmp;
  }
  mForceHistRedraw = true;
  // redraw the plot
  doPlot();
}

//--------------------------------------------------------------------
void HistoryPlot::graphYLowerBoundSlot(){
  // using the default double dialog box is a pain, since we have to 
  // worry about the number of decimal points
  bool conversionOk, dialogOk;
  QString tempStr = QInputDialog::getText("Enter Y-axis lower-bound", 
					  "Y lower-bound Dialog", 
					  QLineEdit::Normal, 
					  QString::number(mYLowerBound), 
					  &dialogOk);

  // Check that user pressed Ok and not Cancel
  if (!dialogOk)
    return;

  double lowerBoundTmp = tempStr.toDouble(&conversionOk);

  // potentially a user could make enough mistakes to reach the 
  // stack limit on the machine, but it's unlikely ;p
  if (!conversionOk){
    graphYLowerBoundSlot();
  }
  else{
    mYLowerBound = lowerBoundTmp;
  }
  mForceHistRedraw = true;
  // redraw the plot
  doPlot();
}

//--------------------------------------------------------------------
void HistoryPlot::graphXLowerBoundSlot(){
  // using the default double dialog box is a pain, since we have to 
  // worry about the number of decimal points
  bool conversionOk, dialogOk;
  QString tempStr = QInputDialog::getText("Enter X-axis lower-bound", 
					  "X lower-bound Dialog", 
					  QLineEdit::Normal, 
					  QString::number(mXLowerBound), 
					  &dialogOk);

  // Check that user pressed Ok and not Cancel
  if (!dialogOk)
    return;

  double lowerBoundTmp = tempStr.toDouble(&conversionOk);

  // potentially a user could make enough mistakes to reach the 
  // stack limit on the machine, but it's unlikely ;p
  if (!conversionOk){
    graphXLowerBoundSlot();
  }
  else{
    mXLowerBound = lowerBoundTmp;
  }
  mForceHistRedraw = true;
  // redraw the plot
  doPlot();
}

//--------------------------------------------------------------------
/** Toggle display of data points on graph on/off
 */
void HistoryPlot::graphDisplaySymbolsSlot(){

  mDisplaySymbolsSet = !mDisplaySymbolsSet;
  mGraphMenu->setItemChecked(mShowSymbolsId, mDisplaySymbolsSet);
  mForceHistRedraw = true;
  // redraw the plot
  doPlot();
}

//--------------------------------------------------------------------
/** Toggle display of curve lines on graph on/off
 */
void HistoryPlot::graphDisplayCurvesSlot(){

  mDisplayCurvesSet = !mDisplayCurvesSet;
  mGraphMenu->setItemChecked(mShowCurvesId, mDisplayCurvesSet);

  HistorySubPlot *plot;
  for ( plot = mSubPlotList.first(); plot; plot = mSubPlotList.next() ){
    plot->graphDisplayCurves();
  }

  mForceHistRedraw = true;
  // redraw the plot
  doPlot();
}

//--------------------------------------------------------------------
/** Toggle use of log X axis
 */
void HistoryPlot::toggleLogAxisXSlot(){

  mUseLogXAxis = !mUseLogXAxis;
  mGraphMenu->setItemChecked(mToggleLogXId, mUseLogXAxis);

  // Can't set manual limits for log axis so turn manual
  // limits off if we're currently using them
  if(mUseLogXAxis && !mAutoXAxisSet){
    autoXAxisSlot();
  }
  // Prevent them from being turned on too
  mGraphMenu->setItemEnabled(mAutoXAxisId, !mUseLogXAxis);

  //mPlotter->changeAxisOptions(mPlotter->xBottom, 
  //			      QwtAutoScale::Logarithmic, mUseLogXAxis);
  mForceHistRedraw = true;
  // redraw the plot
  doPlot();
}

//--------------------------------------------------------------------
/** Toggle use of log Y axis
 */
void HistoryPlot::toggleLogAxisYSlot(){

  mUseLogYAxis = !mUseLogYAxis;
  mGraphMenu->setItemChecked(mToggleLogYId, mUseLogYAxis);

  // Can't set manual limits for log axis so turn manual
  // limits off if we're currently using them
  if(mUseLogYAxis && !mAutoYAxisSet){
    autoYAxisSlot();
  }
  // Prevent them from being turned on too
  mGraphMenu->setItemEnabled(mAutoYAxisId, !mUseLogYAxis);

  //mPlotter->changeAxisOptions(mPlotter->yLeft, 
  //			      QwtAutoScale::Logarithmic, mUseLogYAxis);
  mForceHistRedraw = true;
  // redraw the plot
  doPlot();
}

//--------------------------------------------------------------------
/** Update the graph with new data
 */
void HistoryPlot::updateSlot(){
  HistorySubPlot *plot;
  for ( plot = mSubPlotList.first(); plot; plot = mSubPlotList.next() ){
    plot->update();
  }

  // Insert a horizontal line at y = 0...
  //long mY = mPlotter->insertLineMarker("y = 0", QwtPlot::yLeft);
  //mPlotter->setMarkerYPos(mY, 0.0);

  mPlotter->replot();

  // also want to update the manual upper and lower bounds to 
  // something sensible at this point
  //const QwtScaleDiv *autoScaleDiv = mPlotter->axisScale(0);
  //mYLowerBound = autoScaleDiv->lBound();
  //mYUpperBound = autoScaleDiv->hBound();
  //autoScaleDiv = mPlotter->axisScale(mPlotter->xBottom);
  //mXLowerBound = autoScaleDiv->lBound();
  //mXUpperBound = autoScaleDiv->hBound();

  return;
}

//--------------------------------------------------------------------
/** Override QWidget::closeEvent to catch the user clicking the close button
 *  in the window bar as well as them selecting Quit from the File menu.
 */
void HistoryPlot::closeEvent(QCloseEvent *e){
  // Accept the event to close the widget
  e->accept();
  // Emit a SIGNAL to tell parametertable object we can be deleted
  emit plotClosedSignal(this);
}

