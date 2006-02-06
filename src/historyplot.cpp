/*----------------------------------------------------------------------------
  HistoryPlot class for QT steerer GUI.

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

/** @file historyplot.cpp
    @brief Source file for HistoryPlot class - container for one or more
    history plots on a single canvas */
#include <iostream>
using namespace std;

#include "historysubplot.h"
#include "historyplot.h"
#include "parameterhistory.h"

#include "qprinter.h"
#include "qinputdialog.h"
#include "qwt_symbol.h"
#include "qwt_picker.h"
#include "qwt_legend.h"
#include "qfiledialog.h"
#include "qtextstream.h"
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
  : mXParamHist(_mXParamHist)
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

  QVBoxLayout *tBL = new QVBoxLayout(this);
  mMenuBar = new QMenuBar(this, "menuBar");

  mFileMenu = new QPopupMenu(this, "filePopup");
  mFileMenu->insertItem("&Print", this, SLOT(filePrint()), CTRL+Key_P);
  mFileMenu->insertItem("&Save", this, SLOT(fileSave()), CTRL+Key_S);
  mFileMenu->insertItem("Save da&ta", this, SLOT(fileDataSave()), 
			CTRL+Key_T);
  mFileMenu->insertSeparator();
  mFileMenu->insertItem("&Close", this, SLOT(fileQuit()), CTRL+Key_C);

  mGraphMenu = new QPopupMenu(this, "graphPopup");

  mAutoYAxisId = mGraphMenu->insertItem("&Auto Y Axis", this, 
					SLOT(autoYAxisSlot()), CTRL+Key_A);
  mYUpperBoundId = mGraphMenu->insertItem("Define Y &upper-bound", this, 
					  SLOT(graphYUpperBoundSlot()), 
					  CTRL+Key_U);
  mYLowerBoundId = mGraphMenu->insertItem("Define Y &lower-bound", this, 
					  SLOT(graphYLowerBoundSlot()),
 					  CTRL+Key_L);
  mToggleLogYId = mGraphMenu->insertItem("Toggle use of L&og Y axis", this, 
					 SLOT(toggleLogAxisYSlot()), 
					 CTRL+Key_O);

  mGraphMenu->insertSeparator();
  
  mAutoXAxisId = mGraphMenu->insertItem("&Auto X Axis", this, 
					SLOT(autoXAxisSlot()), ALT+Key_A);
  mXUpperBoundId = mGraphMenu->insertItem("Define X &upper-bound", this, 
					  SLOT(graphXUpperBoundSlot()), 
					  ALT+Key_U);
  mXLowerBoundId = mGraphMenu->insertItem("Define X &lower-bound", this, 
					  SLOT(graphXLowerBoundSlot()), 
					  ALT+Key_L);
  mToggleLogXId = mGraphMenu->insertItem("Toggle use of L&og X axis", this, 
					 SLOT(toggleLogAxisXSlot()), 
					 ALT+Key_O);
  mGraphMenu->insertSeparator();

  mShowSymbolsId = mGraphMenu->insertItem("Toggle &display of symbols", this,
					  SLOT(graphDisplaySymbolsSlot()),
					  ALT+Key_D);

  mShowCurvesId = mGraphMenu->insertItem("Toggle display of l&ines", this,
					 SLOT(graphDisplayCurvesSlot()),
					 ALT+Key_I);

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

  if(!( connect(mPicker, SIGNAL(selected(const QPointArray &)), 
		this, SLOT(canvasSelectedSlot(const QPointArray &))) )){
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
  DBGDST("HistoryPlot");
  delete mPicker;
}

//--------------------------------------------------------------------
void HistoryPlot::canvasSelectedSlot(const QPointArray &pos)
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

  QString lFileName = QFileDialog::getSaveFileName(".", "Images (*.jpg)", 0, 
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

  QString lFileName = QFileDialog::getSaveFileName(".", "Data (*.dat)", 0, 
						   "save file dialog", 
						   "Choose a name for the data file");
  // ensure the user gave us a sensible file
  if (!lFileName.isNull()){

    // ensure the file has a .jpg extension
    if (!lFileName.endsWith(".dat"))
      lFileName.append(".dat");

    QFile file(lFileName);

    if( !file.open( IO_WriteOnly ) ){
      QMessageBox::warning( this, "Saving", "Failed to save file." );
      return;
    }

    QTextStream ts( &file );

    // Header for data
    ts << "# Data exported from RealityGrid Qt Steering Client" << endl;
    ts << "# for " << this->caption() << endl;

    // Work out how many points we've got of 'historical data' - compare 
    // the no. available for each ordinate and use the smaller of the two.
    int lNumPts = 0;
    for ( plot = mSubPlotList.first(); plot; plot = mSubPlotList.next() ){
      if(lNumPts < plot->mYParamHist->mPreviousHistArraySize){
	lNumPts = plot->mYParamHist->mPreviousHistArraySize;
      }
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
    lNumPts = 0;
    for ( plot = mSubPlotList.first(); plot; plot = mSubPlotList.next() ){
      if(lNumPts < plot->mYParamHist->mArrayPos){
	lNumPts = plot->mYParamHist->mArrayPos;
      }
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

    // also want to update the manual upper and lower bounds to 
    // something sensible at this point
    const QwtScaleDiv *autoScaleDiv = mPlotter->axisScale(0);
    mYLowerBound = autoScaleDiv->lBound();
    mYUpperBound = autoScaleDiv->hBound();
  }
  else{
    mPlotter->setAxisScale(0, mYLowerBound, mYUpperBound);
  }

  // allow the user to define the X axis dims if desired
  if (mAutoXAxisSet){

    mPlotter->setAxisAutoScale(mPlotter->xBottom);

    // also want to update the manual upper and lower bounds to 
    // something sensible at this point
    const QwtScaleDiv *autoScaleDiv = mPlotter->axisScale(mPlotter->xBottom);
    mXLowerBound = autoScaleDiv->lBound();
    mXUpperBound = autoScaleDiv->hBound();
  }
  else{
    mPlotter->setAxisScale(mPlotter->xBottom, mXLowerBound, mXUpperBound);
  }

  // Insert a horizontal line at y = 0...
  long mY = mPlotter->insertLineMarker("y = 0", QwtPlot::yLeft);
  mPlotter->setMarkerYPos(mY, 0.0);

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

  mPlotter->changeAxisOptions(mPlotter->xBottom, 
			      QwtAutoScale::Logarithmic, mUseLogXAxis);
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

  mPlotter->changeAxisOptions(mPlotter->yLeft, 
			      QwtAutoScale::Logarithmic, mUseLogYAxis);
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
  long mY = mPlotter->insertLineMarker("y = 0", QwtPlot::yLeft);
  mPlotter->setMarkerYPos(mY, 0.0);

  mPlotter->replot();

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

