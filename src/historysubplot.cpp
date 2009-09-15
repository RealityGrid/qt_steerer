/*----------------------------------------------------------------------------
  HistorySubPlot class for QT steerer GUI.

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
