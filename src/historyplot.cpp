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


/** Constructor
 *  ParameterHistory* contains the data to plot
 *  const char* is the label for the data
 *  const int is the unique parameter ID so that we don't draw graphs
 *    for the wrong parameter signals
 */
HistoryPlot::HistoryPlot(ParameterHistory *_mParamHist, const char *_lLabel, const int _paramID){
    mParamHist = _mParamHist;
    paramID = _paramID;
    strcpy(lLabel, _lLabel);

    setTitle("Parameter History");
    setAutoLegend(TRUE); // We want a legend
    setLegendPos(Qwt::Right);

    // Set axis titles
    setAxisTitle(xBottom, "Timestep");
    setAxisTitle(yLeft, lLabel);

    doPlot();
}

/** Draw the curve on the graph
 */
void HistoryPlot::doPlot(){
    removeCurves();

    // Insert new curves
    long cSin = insertCurve(lLabel);

    // Set curve styles
    setCurvePen(cSin, QPen(red));

    //
    //  Calculate some values
    //
    int nPoints = mParamHist->size;
    double *x = new double[nPoints];
    double *y = new double[nPoints];


    for (int i = 0; i < nPoints; i++){
        x[i] = double(i);
        y[i] = atof(mParamHist->elementAt(i));
    }

    // Copy the data
    setCurveData(cSin, x, y, nPoints);

    // Insert markers

    //  ...a horizontal line at y = 0...
    long mY = insertLineMarker("y = 0", QwtPlot::yLeft);
    setMarkerYPos(mY, 0.0);

    replot();

    delete [] x;
    delete [] y;
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

