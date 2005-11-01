/*----------------------------------------------------------------------------
  Header file for HistoryPlot class for QT steerer GUI

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

  Authors........: Andrew Porter

---------------------------------------------------------------------------*/

#ifndef HISTORYSUBPLOT_H
#define HISTORYSUBPLOT_H

#include "qobject.h"
#include "parameterhistory.h"
#include "historyplot.h"

#define CURVE_UNSET 1000

/** The history plot class is the main window for the
 *  graph, with the extra functionality of menus etc.
 */
class HistorySubPlot : public QObject
{
  Q_OBJECT
  
private:
    HistoryPlot      *mHistPlot;
    QwtPlot   *mPlotter;
    ParameterHistory *mXParamHist;
    /// Holds the label for the y axis or key
    QString mLabely;
    /// Holds the handle of the parameter used for the abscissa
    int xparamID;
    /// Holds the handle of the parameter used for the ordinate
    int mYparamID;

    double mYUpperBound, mYLowerBound;
    double mXUpperBound, mXLowerBound;

    // The Qwt identifiers for the two curves that we
    // might be plotting - one for the history of the parameter
    // prior to the time the steerer connected and one for the
    // history we've collected since connecting.
    long mHistCurveID;
    long mCurveID;
    int  mPreviousLogSize;

public:
    HistorySubPlot(HistoryPlot *lHistPlot,
		   QwtPlot *_lPlotter,
		   ParameterHistory *mXParamHist, 
		   ParameterHistory *mYParamHist, 
		   const QString &lLabely, 
		   const int yparamID);
    ~HistorySubPlot();

    /// Wipe and (re)draw the graph
    void doPlot(bool lForceHistRedraw);
    void update(ParameterHistory *mYParamHist, const int yparamID);
    void filePrint();
    void fileSave();
    void fileDataSave();
    void graphYUpperBound();
    void graphYLowerBound();
    void autoYAxis();
    void graphXUpperBound();
    void graphXLowerBound();
    void autoXAxis();
    void graphDisplaySymbols();
    void graphDisplayCurves();
    void toggleLogAxisX();
    void toggleLogAxisY();

    /// Pointer to the array holding the array of logged values of
    /// the parameter being plotted
    ParameterHistory *mYParamHist;

};

#endif

