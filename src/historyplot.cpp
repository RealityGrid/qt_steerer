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

#include "historyplot.h"
#include "parameterhistory.h"

#include "qprinter.h"
#include "qinputdialog.h"
#include "qwt_symbol.h"
#include "qfiledialog.h"
#include "qtextstream.h"
#include <qmessagebox.h>
#include "iostream.h"

/** Constructor
 *  ParameterHistory* contains the data to plot
 *  const char* is the label for the data
 *  const int is the unique parameter ID so that we don't draw graphs
 *    for the wrong parameter signals
 */
HistoryPlot::HistoryPlot(ParameterHistory *_mXParamHist, 
			 ParameterHistory *_mYParamHist, 
			 const char *_lLabely, 
			 const int _xparamID, 
			 const int _yparamID,
			 const char *_lComponentName)
{
    // Local copies of passed parameters
    mXParamHist = _mXParamHist;
    mYParamHist = _mYParamHist;
    xparamID = _xparamID;
    yparamID = _yparamID;
    strcpy(lLabely, _lLabely);

    // Set the caption to be something descriptive - include
    // name of the component being steered (same as the tab
    // title and set in SteererMainWindow::editTabTitleSlot)
    setCaption(QString(lLabely)+" of "+QString(_lComponentName));
    
    // Create a plotter widget
    mPlotter = new HistoryPlotter(this);

    // Don't bother with a plot title in order to save space
    //mPlotter->setTitle(QString(lLabely)+QString(" vs. SEQUENCE_NUM"));

    // Set axis titles
    mPlotter->setAxisTitle(mPlotter->xBottom, "SEQUENCE_NUM");
    mPlotter->setAxisTitle(mPlotter->yLeft, lLabely);

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

    showSymbolsId = mGraphMenu->insertItem("Toggle &display of symbols", this,
					   SLOT(graphDisplaySymbolsSlot()),
					   ALT+Key_D);

    mGraphMenu->setItemChecked(mAutoYAxisId, true);
    mGraphMenu->setItemChecked(mAutoXAxisId, true);
    mGraphMenu->setItemEnabled(mYUpperBoundId, false);
    mGraphMenu->setItemEnabled(mYLowerBoundId, false);
    mGraphMenu->setItemEnabled(mXUpperBoundId, false);
    mGraphMenu->setItemEnabled(mXLowerBoundId, false);
    mGraphMenu->setItemChecked(showSymbolsId, true);
    mGraphMenu->setItemChecked(mToggleLogXId, false);
    mGraphMenu->setItemChecked(mToggleLogYId, false);
    
    mMenuBar->insertItem("&File", mFileMenu);
    mMenuBar->insertItem("&Graph", mGraphMenu);

    tBL->setMenuBar(mMenuBar);
    tBL->addWidget(mPlotter);

    resize(300, 300);

    mYLowerBound = mYUpperBound = 0;
    mXLowerBound = mXUpperBound = 0;
    mAutoYAxisSet = true;
    mAutoXAxisSet = true;
    mUseLogXAxis = false;
    mUseLogYAxis = false;

    // Default to displaying symbols
    displaySymbolsSet = true;

    doPlot();
}

/** Menu handling slots
 */
void HistoryPlot::filePrint(){
  QPrinter lPrinter;
  mPlotter->print(lPrinter, QwtPlotPrintFilter());
}

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
    ScreenGrabThread *lScreenGrabThread = new ScreenGrabThread(this, lFileName, mMenuBar);
    lScreenGrabThread->start();
  }
}

void HistoryPlot::fileDataSave(){

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

    // Work out how many points we've got - compare the no. available
    // for each ordinate and use the smaller of the two.
    int lNumPts = mYParamHist->mArrayPos;
    if(mXParamHist->mArrayPos < lNumPts) lNumPts = mXParamHist->mArrayPos;

    double *lX = mXParamHist->ptrToArray();
    double *lY = mYParamHist->ptrToArray();
    int     i;

    QTextStream ts( &file );

    // Header for data
    ts << "# Data exported from RealityGrid Qt Steering Client" << endl;
    ts << "# for " << this->caption() << endl;

    // The data itself
    for(i=0; i<lNumPts; i++){
      ts << lX[i] << QString("  %1").arg(lY[i], 0, 'e', 8) << endl;
    }

    file.close();
  }
}

void HistoryPlot::fileQuit(){
  close();
}

/** Draw the curve on the graph
 */
void HistoryPlot::doPlot(){
    mPlotter->removeCurves();

    // Insert new curves
    long cSin = mPlotter->insertCurve(lLabely);

    // Set curve styles
    mPlotter->setCurvePen(cSin, QPen(red));

    // allow the user to define the Y axis dims if desired
    if (!mAutoYAxisSet){
      mPlotter->setAxisScale(0, mYLowerBound, mYUpperBound);
    }
    else{
      mPlotter->setAxisAutoScale(0);

      // also want to update the manual upper and lower bounds to 
      // something sensible at this point
      const QwtScaleDiv *autoScaleDiv = mPlotter->axisScale(0);
      mYLowerBound = autoScaleDiv->lBound();
      mYUpperBound = autoScaleDiv->hBound();
    }

    // allow the user to define the X axis dims if desired
    if (!mAutoXAxisSet){

      mPlotter->setAxisScale(mPlotter->xBottom, mXLowerBound, mXUpperBound);
    }
    else{
      mPlotter->setAxisAutoScale(mPlotter->xBottom);

      // also want to update the manual upper and lower bounds to 
      // something sensible at this point
      const QwtScaleDiv *autoScaleDiv = mPlotter->axisScale(mPlotter->xBottom);
      mXLowerBound = autoScaleDiv->lBound();
      mXUpperBound = autoScaleDiv->hBound();
    }

    // Work out how many points we've got - compare the no. available
    // for each ordinate and use the smaller of the two.
    int nPoints = mYParamHist->mArrayPos;
    if(mXParamHist->mArrayPos < nPoints) nPoints = mXParamHist->mArrayPos;

    // Add symbols - scale their size appropriately.  This code only
    // takes account of the TOTAL no. of points to be plotted and the
    // WIDTH of the plot window.  It does not allow for the fact that
    // y-axis limits may mean that only a subset of the points are plotted.
    if(displaySymbolsSet){
      // |   x    x    x   |
      // |<     width     >| with npoints = 3.
      // Code below works out average spacing of data points
      // as width/(npoints + 1) and then takes a third of that
      // to be the symbol size.  I don't know what units the
      // symbol size is in so this is empirical.
      int ltmp = (int)((float)this->contentsRect().width()/(float)((nPoints + 1)*3));
      if(ltmp > 0){
	// Min. symbol size of 3 looks best
	if(ltmp < 3){
	  ltmp = 3;
	}
	else if(ltmp > 15){
	  ltmp = 15;
	}
	QwtSymbol lPlotSymbol;
	lPlotSymbol.setSize(ltmp);
	lPlotSymbol.setStyle(QwtSymbol::Diamond);

	mPlotter->setCurveSymbol(cSin, lPlotSymbol);
      }
    }

    // Shallow copy of data for plot

    mPlotter->setCurveRawData(cSin, mXParamHist->ptrToArray(), 
			      mYParamHist->ptrToArray(), nPoints);

    // Insert markers

    //  ...a horizontal line at y = 0...
    long mY = mPlotter->insertLineMarker("y = 0", QwtPlot::yLeft);
    mPlotter->setMarkerYPos(mY, 0.0);

    mPlotter->replot();
}

void HistoryPlot::autoYAxisSlot(){
  mAutoYAxisSet = !mAutoYAxisSet;

  mGraphMenu->setItemChecked(mAutoYAxisId, mAutoYAxisSet);
  mGraphMenu->setItemEnabled(mYLowerBoundId, !mAutoYAxisSet);
  mGraphMenu->setItemEnabled(mYUpperBoundId, !mAutoYAxisSet);
}

void HistoryPlot::autoXAxisSlot(){
  mAutoXAxisSet = !mAutoXAxisSet;

  mGraphMenu->setItemChecked(mAutoXAxisId, mAutoXAxisSet);
  mGraphMenu->setItemEnabled(mXLowerBoundId, !mAutoXAxisSet);
  mGraphMenu->setItemEnabled(mXUpperBoundId, !mAutoXAxisSet);
}

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
  else
    mYUpperBound = upperBoundTmp;
}

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
  else
    mXUpperBound = upperBoundTmp;
}

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
  else
    mYLowerBound = lowerBoundTmp;
}

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
  else
    mXLowerBound = lowerBoundTmp;
}

/** Toggle display of data points on graph on/off
 */
void HistoryPlot::graphDisplaySymbolsSlot(){

  displaySymbolsSet = !displaySymbolsSet;
  mGraphMenu->setItemChecked(showSymbolsId, displaySymbolsSet);
  // redraw the plot
  doPlot();
}

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
  // redraw the plot
  doPlot();
}

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
  // redraw the plot
  doPlot();
}


/** Update the graph with new data
 */
void HistoryPlot::updateSlot(ParameterHistory *_mYParamHist, 
			     const int _yparamID){
    // check we're the right graph for this data
    if (_yparamID != yparamID)
      return;

    // update the data
    mYParamHist = _mYParamHist;

    // do the plot
    doPlot();
}

/** Override QWidget::closeEvent to catch the user clicking the close button
 *  in the window bar as well as them selecting Quit from the File menu.
 */
void HistoryPlot::closeEvent(QCloseEvent *e){
  // Accept the event to close the widget
  e->accept();
  // Emit a SIGNAL to tell parametertable object we can be deleted
  emit plotClosedSignal(this);
}

