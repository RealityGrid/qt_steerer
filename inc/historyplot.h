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

#ifndef HISTORYPLOT_H
#define HISTORYPLOT_H

#include "qlayout.h"
#include "qframe.h"
#include "qmenubar.h"
#include "qfiledialog.h"
#include "qthread.h"
#include "qpixmap.h"

#include <qwt_plot.h>
#include "types.h"




class ParameterHistory;
class QMenuBar;
class QPopupMenu;

/** The plotter class is the extended qwt widget.
 *  This deals with the drawing of the graph.
 */

class HistoryPlotter : public QwtPlot
{
  Q_OBJECT

private:
public:
  HistoryPlotter(QWidget *p=0, const char *name=0):QwtPlot(p, name){}
};

/** The history plot class is the main window for the
 *  graph, with the extra functionality of menus etc.
 */
class HistoryPlot : public QFrame
{
  Q_OBJECT
  
private:
    QMenuBar *mMenuBar;
    QPopupMenu *mFileMenu;
    QPopupMenu *mGraphMenu;
    ParameterHistory *mXParamHist;
    ParameterHistory *mYParamHist;
    HistoryPlotter *mPlotter;
    char llabelx[kCHKPT_PARAM_LEN];
    char lLabely[kCHKPT_PARAM_LEN];
    int xparamID;
    int yparamID;

    double upperBound, lowerBound;
    int    upperBoundId, lowerBoundId, autoAxisId, showSymbolsId;
    bool   autoAxisSet;
    bool   displaySymbolsSet;
        
    void doPlot();

protected:
    void closeEvent(QCloseEvent *e);

public slots:
    void updateSlot(ParameterHistory *mYParamHist, const int yparamID);
    void filePrint();
    void fileSave();
    void fileDataSave();
    void fileQuit();
    void graphYUpperBoundSlot();
    void graphYLowerBoundSlot();
    void autoYAxisSlot();
    void graphDisplaySymbolsSlot();

signals:
    void plotClosedSignal(HistoryPlot *ptr);

public:
    HistoryPlot(ParameterHistory *mXParamHist, ParameterHistory *mYParamHist, 
		const char *lLabely, const int xparamID, const int yparamID);
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
      QPixmap lPixmap = QPixmap::grabWindow(mWidget->winId(), 0, mMenuBar->heightForWidth(mWidget->width()));
      lPixmap.save(mFileName, "JPEG");
    }
};

/////////////////////////////////////////////////////////
  

#endif

