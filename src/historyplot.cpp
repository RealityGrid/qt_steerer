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


/** Constructor
 *  ParameterHistory* contains the data to plot
 *  const char* is the label for the data
 *  const int is the unique parameter ID so that we don't draw graphs
 *    for the wrong parameter signals
 */
HistoryPlot::HistoryPlot(ParameterHistory *_mParamHist, const char *_lLabel, const int _paramID){
    // Local copies of passed parameters
    mParamHist = _mParamHist;
    paramID = _paramID;
    strcpy(lLabel, _lLabel);

    // Set the caption to be something descriptive
    setCaption(QString(lLabel)+QString(" over time"));
    
    // Create a plotter widget
    mPlotter = new HistoryPlotter(this);

    mPlotter->setTitle("Parameter History");

    // Set axis titles
    mPlotter->setAxisTitle(mPlotter->xBottom, "Timestep");
    mPlotter->setAxisTitle(mPlotter->yLeft, lLabel);

    QVBoxLayout *tBL = new QVBoxLayout(this);
    mMenuBar = new QMenuBar(this, "menuBar");
    mFileMenu = new QPopupMenu(this, "filePopup");
    mFileMenu->insertItem("&Print", this, SLOT(filePrint()), CTRL+Key_P);
    mFileMenu->insertItem("&Save", this, SLOT(fileSave()), CTRL+Key_S);
    mFileMenu->insertSeparator();
    mFileMenu->insertItem("&Quit", this, SLOT(fileQuit()), CTRL+Key_Q);
    mMenuBar->insertItem("&File", mFileMenu);

    tBL->setMenuBar(mMenuBar);
    tBL->addWidget(mPlotter);

    resize(300, 300);

    doPlot();
}

HistoryPlot::~HistoryPlot(){
    delete mPlotter;
    delete mFileMenu;
    delete mMenuBar;
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
    ScreenGrabThread *lScreenGrabThread = new ScreenGrabThread(this, lFileName);
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
    long cSin = mPlotter->insertCurve(lLabel);

    // Set curve styles
    mPlotter->setCurvePen(cSin, QPen(red));

    //
    //  Calculate some values
    //
    const int nPoints = mParamHist->size;
    double *x = new double[nPoints];
    double *y = new double[nPoints];

    for (int i = 0; i < nPoints; i++){
        x[i] = double(i);
        y[i] = atof(mParamHist->elementAt(i));
    }

    // Copy the data
    mPlotter->setCurveData(cSin, x, y, nPoints);

    // delete the arrays
    delete[] x;
    delete[] y;

    // Insert markers

    //  ...a horizontal line at y = 0...
    long mY = mPlotter->insertLineMarker("y = 0", QwtPlot::yLeft);
    mPlotter->setMarkerYPos(mY, 0.0);

    mPlotter->replot();
}


/** Update the graph with new data
 */
void HistoryPlot::updateSlot(ParameterHistory *_mParamHist, const int _paramID){
    // check we're the right graph for this data
    if (_paramID != paramID)
      return;

    // update the data
    mParamHist = _mParamHist;

    // do the plot
    doPlot();
}


