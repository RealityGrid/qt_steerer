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

  Authors........: Mark Riding, Andrew Porter, Sue Ramsden

---------------------------------------------------------------------------*/

/** @file historyplot.h
    @brief header file for the HistoryPlot class */

#ifndef HISTORYPLOT_H
#define HISTORYPLOT_H

#include "qlayout.h"
#include "qframe.h"
#include "qmenubar.h"
#include "qfiledialog.h"
#include "qthread.h"
#include "qpixmap.h"
#include <qwt_plot.h>
#include <qwt_plot_picker.h>
#include "types.h"

class HistorySubPlot;
class ParameterHistory;
class QMenuBar;
class QPopupMenu;

/** The history plot class is the main window for the
 *  graph, with the extra functionality of menus etc.
 */
class HistoryPlot : public QFrame
{
  Q_OBJECT
  
private:
    QMenuBar   *mMenuBar;
    QPopupMenu *mFileMenu;
    QPopupMenu *mGraphMenu;
    /// Pointer to ParameterHistory for abscissa
    ParameterHistory *mXParamHist;
    /// The QwtPlot object for this history plot
    QwtPlot *mPlotter;
    /// The class dealing with plotting the curve for this history plot
    HistorySubPlot *mSubPlot;
    /// Holds the label for the x axis
    char mLabelx[kCHKPT_PARAM_LEN];
    /// Holds the handle of the parameter used for the abscissa
    int xparamID;
    /// Bounds on y axis
    double mYUpperBound, mYLowerBound;
    /// Bounds on x axis
    double mXUpperBound, mXLowerBound;
    /// Id's for menu items controlling x-axis options
    int    mXUpperBoundId, mXLowerBoundId, mAutoXAxisId;
    /// Id's for menu items controlling y-axis options
    int    mYUpperBoundId, mYLowerBoundId, mAutoYAxisId;
    /// Hande of menu item for controlling whether symbols are drawn
    int    mShowSymbolsId;
    /// Hande of menu item for controlling whether lines are drawn
    int    mShowCurvesId;

    /// Flag set when display options are changed by user - forces
    /// both curves to be redrawn.
    bool mForceHistRedraw;

    /// List of the sub-plots constituting this history plot
    QPtrList<HistorySubPlot> mSubPlotList;

    /// Picker to handle plot selection when adding further curves
    QwtPicker *mPicker;

    /// Holds a list of the colours that QColor knows about
    QStringList mColourList;
    /// Iterator so that each new curve is given a new colour
    QStringList::Iterator mColourIter;

    /// Wipe and (re)draw the graph
    void doPlot();

protected:
    void closeEvent(QCloseEvent *e);

public slots:
    /// Slot signalled from controlForm when graph needs to be updated
    void updateSlot();
    void filePrint();
    void fileSave();
    void fileDataSave();
    void fileQuit();
    void graphYUpperBoundSlot();
    void graphYLowerBoundSlot();
    void autoYAxisSlot();
    void graphXUpperBoundSlot();
    void graphXLowerBoundSlot();
    void autoXAxisSlot();
    void graphDisplaySymbolsSlot();
    void graphDisplayCurvesSlot();
    void toggleLogAxisXSlot();
    void toggleLogAxisYSlot();
    void canvasSelectedSlot(const QPointArray &);

signals:
    void plotClosedSignal(HistoryPlot *ptr);
    void plotSelectedSignal(HistoryPlot *);

public:
    /** Constructor
     *  @param mXParamHist pointer to ParameterHistory containing data to plot
     *  @param mYParamHist pointer to ParameterHistory containing data to plot
     *  @param lLabelx Label for the abscissa
     *  @param lLabely Label for the ordinate
     *  @param xparamID Unique parameter ID so that we don't draw graphs
     *    for the wrong parameter signals
     *  @param yparamID Unique parameter ID so that we don't draw graphs
     *    for the wrong parameter signals
     */
    HistoryPlot(ParameterHistory *mXParamHist, 
		ParameterHistory *mYParamHist, 
		const char *lLabelx,
		const char *lLabely, 
		const int xparamID, const int yparamID,
		const char *_lComponentName);
    ~HistoryPlot();

    /** Add another plot/curve to this history plot */
    void addPlot(ParameterHistory *_mYParamHist,
		 const char *_lLabely, 
		 const int _yparamID);

    int    mToggleLogXId, mToggleLogYId;
    bool   mAutoYAxisSet, mAutoXAxisSet;
    /// Whether or not to display symbols on curve
    bool   mDisplaySymbolsSet;
    /// Whether or not to draw curve (as guide to eye)
    bool   mDisplayCurvesSet;
    bool   mUseLogXAxis, mUseLogYAxis;
};

/////////////////////////////////////////////////////////

/** This class is necessary to give QT time to update the widget properly before
 *  taking a screenshot. Seemingly, standard calls to refresh() redraw() update()
 *  and the like only cause a refresh after all current events have been dealt with.
 *  Hence this class.
 */
 
class ScreenGrabThread: public QThread
{
  private:
    QWidget *mWidget;
    QString mFileName;
    QMenuBar *mMenuBar;
  public:
    ScreenGrabThread(QWidget *lWidget, QString &lFileName, QMenuBar *lMenuBar){
      mWidget = lWidget;
      mFileName = lFileName;
      mMenuBar = lMenuBar;
    }
    virtual void run(){
      wait(500);

      // Grab the window contents rather than the widget
      // since we seemingly can't force a refresh immediately - spawn a thread to do this
      QPixmap lPixmap = QPixmap::grabWindow(mWidget->winId(), 0, 
					    mMenuBar->heightForWidth(mWidget->width()));
      lPixmap.save(mFileName, "JPEG");
    }
};

/////////////////////////////////////////////////////////
  

#endif

