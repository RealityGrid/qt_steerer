/*----------------------------------------------------------------------------
    Utility classes for QT steerer GUI

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
    03.10.2002      0.1                                         S Ramsden

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
  setFont(QFont( "Times", 10, QFont::DemiBold ));
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
