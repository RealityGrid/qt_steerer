/*----------------------------------------------------------------------------
    Utility classes for QT steerer GUI

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


#include "utility.h"
#include "types.h"
#include "debug.h"

#include <qapplication.h>
#include <qfontmetrics.h>
#include <qpainter.h>
#include <qstring.h>
#include <qwidget.h>

TableLabel::TableLabel(const QString & text, QWidget * parent)
  : QLabel(text, parent)
{
  DBGCON("TableLabel");
  setFont(QFont( "Times", 9, QFont::DemiBold ));
}

TableLabel::~TableLabel()
{
  DBGDST("TableLabel");
}




AttachListItem::AttachListItem(int aSimIndex, const QString &text)
  : QListBoxItem(), mSimIndex(aSimIndex)
{
  DBGCON("AttachListItem");
  setText( text );
  printf("constucted AttachListItem mSimIndex = %d\n", mSimIndex);

}

AttachListItem::~AttachListItem()
{
  DBGDST("AttachListItem");
}

int
AttachListItem::getSimIndex() const
{
  return mSimIndex;
}


void 
AttachListItem::paint( QPainter *painter )
{
    QFontMetrics fm = painter->fontMetrics();
    painter->drawText( 3, fm.ascent() +  (fm.leading()+1)/2 + 1, text() );
}

int 
AttachListItem::height( const QListBox* lb ) const
{
    int h = lb ? lb->fontMetrics().lineSpacing() + 2 : 0;
    if (h > QApplication::globalStrut().height())
      return h;
    else
      return QApplication::globalStrut().height();
}


int 
AttachListItem::width( const QListBox* lb ) const
{
    int w = lb ? lb->fontMetrics().width( text() ) + 6 : 0;
    if (w > QApplication::globalStrut().width())
      return w;
    else
      return QApplication::globalStrut().width();
}
