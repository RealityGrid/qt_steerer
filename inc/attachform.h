/*----------------------------------------------------------------------------
    AttachForm class header file for QT steerer GUI. 

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

    Initial version by: S Ramsden, 23.10.2002
    
---------------------------------------------------------------------------*/


#ifndef __ATTACH_FORM_H__
#define __ATTACH_FORM_H__

#include <qdialog.h>

class QLineEdit;
class QListBox;
class QPushButton;

class AttachForm: public QDialog
{
  Q_OBJECT
       
public:
  AttachForm(QWidget *parent = 0, const char *name = "attachform",
	     bool modal = TRUE, WFlags f = 0 );
  ~AttachForm();

  int getNumSims() const;
  int getLibReturnStatus() const;
  char *getSimGSMSelected() const;

protected slots:
  void attachSlot();
  void filterSlot();

private:
  void cleanUp();

private:
  int		mNumSims;
  int		mLibReturnStatus;

  char		*mSimGSHSelected;
  char  	**mSimName;
  char		**mSimGSH;

  QListBox	*mListBox;
  QLineEdit	*mFilterLineEdit;

  QPushButton	*mAttachButton;
  QPushButton	*mCancelButton;

};

#endif
