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
#include <iostream>
using namespace std;

//---------------------------------------------------------------------------
HistorySubPlot::HistorySubPlot(HistoryPlot *lHistPlot,
			       QwtPlot *lPlotter,
			       ParameterHistory *lXParamHist, 
			       ParameterHistory *lYParamHist, 
			       const QString &lLabely, 
			       const int yparamID)
  : mHistPlot(lHistPlot), mXParamHist(lXParamHist), mYParamHist(lYParamHist),
    mLabely(lLabely), mPlotter(lPlotter), mYparamID(yparamID)
{
  mCurveID         = CURVE_UNSET;
  mHistCurveID     = CURVE_UNSET;
  mPreviousLogSize = 0;
  mYLowerBound = mYUpperBound = 0;
  mXLowerBound = mXUpperBound = 0;
}

//---------------------------------------------------------------------------
HistorySubPlot::~HistorySubPlot()
{
}

//---------------------------------------------------------------------------
void HistorySubPlot::doPlot(bool lForceHistRedraw=false)
{
  //cout << "HistorySubPlot::doPlot START" << endl;
  bool lReplotHistory = lForceHistRedraw || 
    (mYParamHist->mPreviousHistArraySize != mPreviousLogSize);

  mPreviousLogSize = mYParamHist->mPreviousHistArraySize;

  if(mCurveID != CURVE_UNSET)mPlotter->removeCurve(mCurveID);
  if(lReplotHistory && 
     (mHistCurveID != CURVE_UNSET))mPlotter->removeCurve(mHistCurveID);

  // Insert new curves
  mCurveID = mPlotter->insertCurve(mLabely);
  if(lReplotHistory)mHistCurveID = mPlotter->insertCurve(mLabely);

  // Set curve styles
  if(mHistPlot->mDisplayCurvesSet){
    mPlotter->setCurvePen(mCurveID, QPen(red));
    mPlotter->setCurvePen(mHistCurveID, QPen(red));
  }
  else{
    mPlotter->setCurveStyle(mCurveID, QwtCurve::NoCurve);
    mPlotter->setCurveStyle(mHistCurveID, QwtCurve::NoCurve);
  }


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
    int ltmp = (int)((float)mHistPlot->contentsRect().width()/(float)((nPoints + 1)*3));
    if(ltmp > 0){
      // Min. symbol size of 3 looks best
      if(ltmp < 3){
	ltmp = 3;
      }
      else if(ltmp > 15){
	ltmp = 15;
      }
      QwtSymbol lPlotSymbol;
      lPlotSymbol.setSize(ltmp);
      lPlotSymbol.setStyle(QwtSymbol::Diamond);
      
      mPlotter->setCurveSymbol(mCurveID, lPlotSymbol);
      mPlotter->setCurveSymbol(mHistCurveID, lPlotSymbol);
    }
  }

  // Shallow copy of data for plot
  nPoints = mYParamHist->mArrayPos;
  if(mXParamHist->mArrayPos < nPoints){
    nPoints = mXParamHist->mArrayPos;
  }
  //cout << "HistorySubPlot::doPlot nPoints = " << nPoints << endl;

  mPlotter->setCurveRawData(mCurveID, mXParamHist->ptrToArray(), 
			    mYParamHist->ptrToArray(), nPoints);

  if(lReplotHistory){
    nPoints = mYParamHist->mPreviousHistArraySize;
    if(mXParamHist->mPreviousHistArraySize < nPoints){
      nPoints = mXParamHist->mPreviousHistArraySize;
    }
    if(nPoints){
      mPlotter->setCurveRawData(mHistCurveID, 
				mXParamHist->mPtrPreviousHistArray, 
				mYParamHist->mPtrPreviousHistArray, nPoints);
    }
  }
  //cout << "HistorySubPlot::doPlot END" << endl;
}

//---------------------------------------------------------------------------
void HistorySubPlot::update(ParameterHistory *lYParamHist, const int yparamID)
{
  //cout << "HistorySubPlot::update BEGIN, id = " << yparamID << endl;
  //cout << "HistorySubPlot::update member id = " << mYparamID << endl;
  // check we're the right graph for this data
  if(mYparamID != yparamID)return;

  // update the data
  mYParamHist = lYParamHist;

  // redo the plot
  doPlot(true);
  //cout << "HistorySubPlot::update END" << endl;
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
void HistorySubPlot::graphYUpperBound()
{
}

//---------------------------------------------------------------------------
void HistorySubPlot::graphYLowerBound()
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
}

//---------------------------------------------------------------------------
void HistorySubPlot::toggleLogAxisX()
{
}

//---------------------------------------------------------------------------
void HistorySubPlot::toggleLogAxisY()
{
}

