/*----------------------------------------------------------------------------
    Header file for HistoryPlot class for QT steerer GUI

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

#ifndef HISTORYPLOT_H
#define HISTORYPLOT_H

#include <qwt_plot.h>
#include "types.h"


class ParameterHistory;
  
class HistoryPlot : public QwtPlot
{
  Q_OBJECT
  
private:
    ParameterHistory *mParamHist;
    char lLabel[kCHKPT_PARAM_LEN];
    int paramID;

    void doPlot();

public slots:
    void updateSlot(ParameterHistory *mParamHist, const int paramID);

public:
    HistoryPlot(ParameterHistory *mParamHist, const char *lLabel, const int paramID);

};
  

#endif

