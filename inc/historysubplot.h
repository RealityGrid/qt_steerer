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

/** The historysubplot class deals with the plotting of a single
 *  curve on a historyplot (which may consist of more than one
 *  such curve).
 */
class HistorySubPlot : public QObject
{
  Q_OBJECT
  
private:
    /// Pointer to the parent HistoryPlot object
    HistoryPlot      *mHistPlot;
    /// Pointer to the parent QwtPlot object in which we will draw
    QwtPlot          *mPlotter;
    /// Pointer to the ParameterHistory object holding 
    /// data for abscissa
    ParameterHistory *mXParamHist;
    /// Holds the label for the y axis or key
    QString           mLabely;

    /// The Qwt identifier for the curve showing the history of the 
    /// parameter prior to the time the steerer connected
    long    mHistCurveID;
    /// The (QwtPlot) id for the curve showing the history of the parameter
    /// we are looking after since the steerer connected to the simulation
    long    mCurveID;
    /// How many elements the log for the parameter being plotted
    /// used to have
    int     mPreviousLogSize;
    /// String holding the (QColor-recognised) name of the colour
    /// of the pen for this curve 
    QString mColour;

public:
    HistorySubPlot(HistoryPlot *lHistPlot,
		   QwtPlot *_lPlotter,
		   ParameterHistory *mXParamHist, 
		   ParameterHistory *mYParamHist, 
		   const QString &lLabely, 
		   const int yparamID,
		   const QString lColour);
    ~HistorySubPlot();

    /// Wipe and (re)draw the graph
    void doPlot(bool lForceHistRedraw);
    /// Called by updateSlot in HistoryPlot
    void update();
    void filePrint();
    void fileSave();
    void fileDataSave();
    void autoYAxis();
    void graphXUpperBound();
    void graphXLowerBound();
    void autoXAxis();
    void graphDisplaySymbols();
    void graphDisplayCurves();
    void toggleLogAxisX();
    void toggleLogAxisY();
    QString getCurveLabel();

    /// Pointer to the array holding the array of logged values of
    /// the parameter being plotted
    ParameterHistory *mYParamHist;
    /// Holds the handle of the parameter used for the abscissa
    int xparamID;
    /// Holds the handle of the parameter used for the ordinate
    int mYparamID;
};

#endif

