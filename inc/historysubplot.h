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

  Author: Andrew Porter
          Robert Haines
 */

#ifndef HISTORYSUBPLOT_H
#define HISTORYSUBPLOT_H

#include "qobject.h"
#include "parameterhistory.h"
#include "historyplot.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

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

    /// The curve showing the history of the 
    /// parameter prior to the time the steerer connected
    QwtPlotCurve* mHistCurve;

    /// The curve showing the history of the parameter
    /// we are looking after since the steerer connected to the simulation
    QwtPlotCurve* mCurve;

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

