/*----------------------------------------------------------------------------
    ConfigForm class header file for QT steerer GUI. 

    (C)Copyright 2002 The University of Manchester, United Kingdom,
    all rights reserved.

    This software is produced by the Supercomputing, Visualization &
    e-Science Group, Manchester Computing, the Victoria University of
    Manchester as part of the RealityGrid project.

    This software has been tested with care but is not guaranteed for
    any particular purpose. Neither the authors, nor the University of
    Manchester offer any warranties or representations, nor do they
    accept any liabilities with respect to this software.

    This program must not be used for commmercial gain without the
    written permission of the authors.
    
    Supercomputing, Visualization & e-Science Group
    Manchester Computing
    University of Manchester
    Manchester M13 9PL

    email:  csar-advice@cfs.ac.uk.
    Tel:    +44 161 275 6824/5997
    Fax:    +44 161 275 6040    
    
    Date          Version    Updates                            Author
    ----          -------    -------                            ------
    23.10.2002      0.1                                         S Ramsden

---------------------------------------------------------------------------*/

#ifndef __CONFIG_FORM_H__
#define __CONFIG_FORM_H__

#include <qdialog.h>

class QLineEdit;
class QPushButton;

class ConfigForm: public QDialog
{
  Q_OBJECT
       
public:
  ConfigForm(int aCurrentIntervalValue, QWidget *parent = 0, const char *name = "configform",
	     bool modal = TRUE, WFlags f = 0 );
  ~ConfigForm();

  int getIntervalValue() const;

protected slots:
  void applySlot();

private:
  double	mIntervalValue;

  QLineEdit	*mLineEdit;
  QPushButton	*mApplyButton;
  QPushButton	*mCancelButton;

};

#endif
