/*----------------------------------------------------------------------------
    HistoryPlot class for QT steerer GUI.

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

    Initial version by: M Riding, 24.06.2003

---------------------------------------------------------------------------*/

#include "historyplot.h"
#include "parameterhistory.h"

#include "qprinter.h"
#include "qinputdialog.h"
#include "qwt_symbol.h"
#include "iostream.h"

/** Constructor
 *  ParameterHistory* contains the data to plot
 *  const char* is the label for the data
 *  const int is the unique parameter ID so that we don't draw graphs
 *    for the wrong parameter signals
 */
HistoryPlot::HistoryPlot(ParameterHistory *_mXParamHist, 
			 ParameterHistory *_mYParamHist, const char *_lLabely, 
			 const int _xparamID, const int _yparamID){
    // Local copies of passed parameters
    mXParamHist = _mXParamHist;
    mYParamHist = _mYParamHist;
    xparamID = _xparamID;
    yparamID = _yparamID;
    strcpy(lLabely, _lLabely);

    // Set the caption to be something descriptive
    setCaption(QString(lLabely)+QString(" over time"));
    
    // Create a plotter widget
    mPlotter = new HistoryPlotter(this);

    mPlotter->setTitle("Parameter History");

    // Set axis titles
    mPlotter->setAxisTitle(mPlotter->xBottom, "Timestep");
    mPlotter->setAxisTitle(mPlotter->yLeft, lLabely);

    QVBoxLayout *tBL = new QVBoxLayout(this);
    mMenuBar = new QMenuBar(this, "menuBar");

    mFileMenu = new QPopupMenu(this, "filePopup");
    mFileMenu->insertItem("&Print", this, SLOT(filePrint()), CTRL+Key_P);
    mFileMenu->insertItem("&Save", this, SLOT(fileSave()), CTRL+Key_S);
    mFileMenu->insertSeparator();
    mFileMenu->insertItem("&Quit", this, SLOT(fileQuit()), CTRL+Key_Q);

    mGraphMenu = new QPopupMenu(this, "graphPopup");
    autoAxisId = mGraphMenu->insertItem("&Auto Y Axis", this, 
					SLOT(autoYAxisSlot()), CTRL+Key_A);
    upperBoundId = mGraphMenu->insertItem("Define Y &upper-bound", this, 
					  SLOT(graphYUpperBoundSlot()), 
					  CTRL+Key_U);
    lowerBoundId = mGraphMenu->insertItem("Define Y &lower-bound", this, 
					  SLOT(graphYLowerBoundSlot()), 
					  CTRL+Key_L);
    showSymbolsId = mGraphMenu->insertItem("Toggle &display of symbols", this,
					   SLOT(graphDisplaySymbolsSlot()),
					   CTRL+Key_D);

    mGraphMenu->setItemChecked(autoAxisId, true);
    mGraphMenu->setItemEnabled(upperBoundId, false);
    mGraphMenu->setItemEnabled(lowerBoundId, false);
    mGraphMenu->setItemChecked(showSymbolsId, true);
    
    mMenuBar->insertItem("&File", mFileMenu);
    mMenuBar->insertItem("&Graph", mGraphMenu);

    tBL->setMenuBar(mMenuBar);
    tBL->addWidget(mPlotter);

    resize(300, 300);

    lowerBound = upperBound = 0;
    autoAxisSet = true;

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
  QString lFileName = QFileDialog::getSaveFileName(".", "Images (*.jpg)", 0, "save file dialog", "Choose a filename to save the image as");
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
    if (!autoAxisSet){
      mPlotter->setAxisScale(0, lowerBound, upperBound);
    }
    else{
      mPlotter->setAxisAutoScale(0);

      // also want to update the manual upper and lower bounds to 
      // something sensible at this point
      const QwtScaleDiv *autoScaleDiv = mPlotter->axisScale(0);
      lowerBound = autoScaleDiv->lBound();
      upperBound = autoScaleDiv->hBound();
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
	if(ltmp < 3)ltmp = 3;
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
  autoAxisSet = !autoAxisSet;

  mGraphMenu->setItemChecked(autoAxisId, autoAxisSet);
  mGraphMenu->setItemEnabled(lowerBoundId, !autoAxisSet);
  mGraphMenu->setItemEnabled(upperBoundId, !autoAxisSet);
}

void HistoryPlot::graphYUpperBoundSlot(){
  // using the default double dialog box is a pain, since we have to worry about the
  // number of decimal points
  // upperBound = QInputDialog::getDouble("Enter Y-axis upper-bound", "Y Upper-bound Dialog", upperBound, -2147483647, 2147483647, 16);

  bool conversionOk, dialogOk;
  QString tempStr = QInputDialog::getText("Enter Y-axis upper-bound", "Y upper-bound Dialog", QLineEdit::Normal, QString::number(upperBound), &dialogOk);

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
    upperBound = upperBoundTmp;
}

void HistoryPlot::graphYLowerBoundSlot(){
  // using the default double dialog box is a pain, since we have to worry about the
  // number of decimal points
  // lowerBound = QInputDialog::getDouble("Enter Y-axis lower-bound", "Y Lower-bound Dialog", lowerBound, -2147483647, 2147483647, 16);

  bool conversionOk, dialogOk;
  QString tempStr = QInputDialog::getText("Enter Y-axis upper-bound", "Y upper-bound Dialog", QLineEdit::Normal, QString::number(lowerBound), &dialogOk);

  // Check that user pressed Ok and not Cancel
  if (!dialogOk)
    return;

  double lowerBoundTmp = tempStr.toDouble(&conversionOk);

  // potentially a user could make enough mistakes to reach the stack limit on the machine,
  // but it's unlikely ;p
  if (!conversionOk){
    graphYLowerBoundSlot();
  }
  else
    lowerBound = lowerBoundTmp;
}

/** Toggle display of data points on graph on/off
 */
void HistoryPlot::graphDisplaySymbolsSlot(){

  displaySymbolsSet = !displaySymbolsSet;
  mGraphMenu->setItemChecked(showSymbolsId, displaySymbolsSet);
  // redraw the plot
  doPlot();
}


/** Update the graph with new data
 */
void HistoryPlot::updateSlot(ParameterHistory *_mYParamHist, const int _yparamID){
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

