/*----------------------------------------------------------------------------
    Header file utility classes for QT steerer GUI.

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

    Initial version by: S Ramsden, 03.10.2002
    
---------------------------------------------------------------------------*/


#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <qlabel.h>
#include <qlistbox.h>

class QPainter;
class QString;
class QWidget;

class TableLabel : public QLabel
{

public:
  TableLabel(const QString & text, QWidget * parent);
  ~TableLabel();

};



class AttachListItem : public QListBoxItem
{

public:
  AttachListItem(int aSimIndex, const QString &text);
  ~AttachListItem();

  int getSimIndex() const;

  virtual void paint( QPainter * );
  virtual int width( const QListBox* ) const;
  virtual int height( const QListBox* ) const;

private:
  int mSimIndex;

  
};



#endif

