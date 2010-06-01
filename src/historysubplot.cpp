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

  Author: Andrew Porter
          Robert Haines
 */

#include "historysubplot.h"
#include <qwt_legend.h>
#include <qwt_symbol.h>
#include <qwt_legend_item.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <iostream>
using namespace std;

//---------------------------------------------------------------------------
HistorySubPlot::HistorySubPlot(HistoryPlot *lHistPlot,
			       QwtPlot *lPlotter,
			       ParameterHistory *lXParamHist,
			       ParameterHistory *lYParamHist,
			       const QString &lLabely,
			       const int yparamID,
			       const QString lColour)
  : mHistPlot(lHistPlot), mPlotter(lPlotter), mXParamHist(lXParamHist),
    mLabely(lLabely), mColour(lColour),
    mYParamHist(lYParamHist),  mYparamID(yparamID)
{
  mCurve           = new QwtPlotCurve(mLabely);
  mHistCurve       = new QwtPlotCurve(mLabely);
  mPreviousLogSize = 0;
  //cout << "ARPDBG: HistorySubPlot: colour = " << mColour << endl;
}

//---------------------------------------------------------------------------
HistorySubPlot::~HistorySubPlot()
{
}

//---------------------------------------------------------------------------
void HistorySubPlot::doPlot(bool lForceHistRedraw=false)
{
  QwtSymbol lPlotSymbol;
  int ltmp = 0;
  bool lReplotHistory = lForceHistRedraw ||
    (mYParamHist->mPreviousHistArraySize != mPreviousLogSize);

  mPreviousLogSize = mYParamHist->mPreviousHistArraySize;

  // Insert new curves if any
  if(mCurve->plot() == NULL) {
    mCurve->attach(mPlotter);
    this->graphDisplayCurves();
  }

//   if(mCurveID == CURVE_UNSET){
//     mCurveID = mPlotter->insertCurve(mLabely);
//     if( !(mPlotter->legendEnabled(mCurveID)) ){
//       // Only do this if not already enabled so as to prevent flicker
//       mPlotter->enableLegend(true, mCurveID);
//       mPlotter->legend()->setDisplayPolicy(QwtLegend::FixedIdentifier,
// 					   (QwtLegendItem::ShowLine |
// 					    QwtLegendItem::ShowText));
//     }
//     this->graphDisplayCurves();
//   }

  if(mHistCurve->plot() == NULL && mYParamHist->mPreviousHistArraySize > 0) {
    mHistCurve->attach(mPlotter);
    this->graphDisplayCurves();
  }

//   if(mHistCurveID == CURVE_UNSET &&
//      mYParamHist->mPreviousHistArraySize > 0){
//     mHistCurveID = mPlotter->insertCurve(mLabely);
//     this->graphDisplayCurves();
//   }

  // Work out how many points we've got - compare the no. available
  // for each ordinate and use the smaller of the two.
  int nPoints = mYParamHist->mArrayPos +  mYParamHist->mPreviousHistArraySize;
  if((mXParamHist->mArrayPos + mXParamHist->mPreviousHistArraySize) < nPoints){
    nPoints = mXParamHist->mArrayPos + mXParamHist->mPreviousHistArraySize;
  }

  // Add symbols - scale their size appropriately.  This code only
  // takes account of the TOTAL no. of points to be plotted and the
  // WIDTH of the plot window.  It does not allow for the fact that
  // y-axis limits may mean that only a subset of the points are plotted.
  if(mHistPlot->mDisplaySymbolsSet){
    // |   x    x    x   |
    // |<     width     >| with npoints = 3.
    // Code below works out average spacing of data points
    // as width/(npoints + 1) and then takes a third of that
    // to be the symbol size.  I don't know what units the
    // symbol size is in so this is empirical.
    ltmp = (int)((float)mHistPlot->contentsRect().width()/
		 (float)((nPoints + 1)*3));
    if(ltmp > 0){
      // Min. symbol size of 3 looks best
      if(ltmp < 3){
	ltmp = 3;
      }
      else if(ltmp > 15){
	ltmp = 15;
      }
      lPlotSymbol.setSize(ltmp);
      lPlotSymbol.setStyle(QwtSymbol::Diamond);
      mCurve->setSymbol(lPlotSymbol);
      mHistCurve->setSymbol(lPlotSymbol);
    }
  }
  if(ltmp <= 0){
      lPlotSymbol.setStyle(QwtSymbol::NoSymbol);
      mCurve->setSymbol(lPlotSymbol);
      mHistCurve->setSymbol(lPlotSymbol);
  }

  nPoints = mYParamHist->mArrayPos;
  if(mXParamHist->mArrayPos < nPoints){
    nPoints = mXParamHist->mArrayPos;
  }

  // Shallow copy of data for plot
  mCurve->setRawData(mXParamHist->ptrToArray(),
		     mYParamHist->ptrToArray(), nPoints);

  if(lReplotHistory) {
    nPoints = mYParamHist->mPreviousHistArraySize;
    if(mXParamHist->mPreviousHistArraySize < nPoints) {
      nPoints = mXParamHist->mPreviousHistArraySize;
    }
    if(nPoints) {
      mHistCurve->setRawData(mXParamHist->mPtrPreviousHistArray,
			     mYParamHist->mPtrPreviousHistArray, nPoints);
    }
  }
}

//---------------------------------------------------------------------------
void HistorySubPlot::update()
{
  // redo the plot
  doPlot(true);
}

//---------------------------------------------------------------------------
void HistorySubPlot::filePrint()
{
}

//---------------------------------------------------------------------------
void HistorySubPlot::fileSave()
{
}

//---------------------------------------------------------------------------
void HistorySubPlot::fileDataSave()
{
}

//---------------------------------------------------------------------------
void HistorySubPlot::autoYAxis()
{
}

//---------------------------------------------------------------------------
void HistorySubPlot::graphXUpperBound()
{
}

//---------------------------------------------------------------------------
void HistorySubPlot::graphXLowerBound()
{
}

//---------------------------------------------------------------------------
void HistorySubPlot::autoXAxis()
{
}

//---------------------------------------------------------------------------
void HistorySubPlot::graphDisplaySymbols()
{
}

//---------------------------------------------------------------------------
void HistorySubPlot::graphDisplayCurves()
{
  // Set curve styles
  if(mHistPlot->mDisplayCurvesSet){
    mCurve->setStyle(QwtPlotCurve::Lines);
    mCurve->setPen(QPen(mColour));
    if(mHistCurve->plot() != NULL){
      mHistCurve->setStyle(QwtPlotCurve::Lines);
      mHistCurve->setPen(QPen(mColour));
    }
  }
  else{
    mCurve->setStyle(QwtPlotCurve::NoCurve);
    if(mHistCurve->plot() != NULL)
      mHistCurve->setStyle(QwtPlotCurve::NoCurve);
  }
}

//---------------------------------------------------------------------------
void HistorySubPlot::toggleLogAxisX()
{
}

//---------------------------------------------------------------------------
void HistorySubPlot::toggleLogAxisY()
{
}

//---------------------------------------------------------------------------
QString HistorySubPlot::getCurveLabel()
{
  return mLabely;
}
