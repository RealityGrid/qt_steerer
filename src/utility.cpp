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

  Author: Mark Riding
          Andrew Porter
          Sue Ramsden
          Robert Haines
 */

/** @file utility.cpp
    @brief Utility classes for QT steerer GUI
    @author Sue Riding
    @author Mark Riding
    @author Andrew Porter */

#include "utility.h"
#include "types.h"
#include "debug.h"

#include <qapplication.h>
#include <qfontmetrics.h>
#include <qpainter.h>
#include <qstring.h>
#include <qwidget.h>
//Added by qt3to4:
#include <QLabel>

TableLabel::TableLabel(const QString & text, QWidget * parent)
  : QLabel(text, parent)
{
  REG_DBGCON("TableLabel");
  QFont lFont = font();
  lFont.setWeight(QFont::DemiBold);
  lFont.setPointSize(lFont.pointSize()-1);
  setFont(lFont);
  //setFont(QFont( "Times", 9, QFont::DemiBold ));
}

TableLabel::~TableLabel()
{
  REG_DBGDST("TableLabel");
}




AttachListItem::AttachListItem(int aSimIndex, const QString &text)
  : Q3ListBoxItem(), mSimIndex(aSimIndex)
{
  REG_DBGCON("AttachListItem");
  setText( text );
  printf("constucted AttachListItem mSimIndex = %d\n", mSimIndex);

}

AttachListItem::~AttachListItem()
{
  REG_DBGDST("AttachListItem");
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
AttachListItem::height( const Q3ListBox* lb ) const
{
    int h = lb ? lb->fontMetrics().lineSpacing() + 2 : 0;
    if (h > QApplication::globalStrut().height())
      return h;
    else
      return QApplication::globalStrut().height();
}


int
AttachListItem::width( const Q3ListBox* lb ) const
{
    int w = lb ? lb->fontMetrics().width( text() ) + 6 : 0;
    if (w > QApplication::globalStrut().width())
      return w;
    else
      return QApplication::globalStrut().width();
}
