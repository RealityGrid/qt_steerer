/*
  The RealityGrid Steerer

  Copyright (c) 2002-2010, University of Manchester, United Kingdom.
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
          Sue Ramsden
          Robert Haines
 */

/** @file historyplot.h
    @brief header file for the HistoryPlot class */

#ifndef HISTORYPLOT_H
#define HISTORYPLOT_H

#include "qlayout.h"
#include "q3frame.h"
#include "qmenubar.h"
#include "q3filedialog.h"
#include "qthread.h"
#include "qpixmap.h"
#include <qwt_plot.h>
#include <qwt_plot_picker.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <Q3PopupMenu>
#include <QCloseEvent>
#include <Q3PtrList>

#include "types.h"

class HistorySubPlot;
class ParameterHistory;
class QMenuBar;
class Q3PopupMenu;

/** The history plot class is the main window for the
 *  graph, with the extra functionality of menus etc.
 */
class HistoryPlot : public Q3Frame
{
  Q_OBJECT

private:
    QMenuBar   *mMenuBar;
    Q3PopupMenu *mFileMenu;
    Q3PopupMenu *mGraphMenu;
    /// Pointer to ParameterHistory for abscissa
    ParameterHistory *mXParamHist;
    /// The QwtPlot object for this history plot
    QwtPlot *mPlotter;
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
    Q3PtrList<HistorySubPlot> mSubPlotList;

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
    void canvasSelectedSlot(const Q3PointArray &);

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
