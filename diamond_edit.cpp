/**************************************************************************
*
* Copyright (c) 2012 Barbara Geller
* All rights reserved.
*
* This file is part of Diamond Editor.
*
* Diamond Editor is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License version 3
* as published by the Free Software Foundation.
*
* Diamond Editor is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Diamond Editor.  If not, see <http://www.gnu.org/licenses/>.
*
**************************************************************************/

#include "diamond_edit.h"
#include "util.h"

#include <QtGui>

DiamondTextEdit::DiamondTextEdit()
      : QPlainTextEdit()
{
   m_showlineNum = false;

   m_lineNumArea = new LineNumArea(this);
   update_LineNumWidth(0);

   connect(this, SIGNAL(blockCountChanged(int)),   this, SLOT(update_LineNumWidth(int)));
   connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(update_LineNumArea(QRect,int)));  
}

void DiamondTextEdit::lineNum_PaintEvent(QPaintEvent *event)
{   
   if (m_showlineNum)  {

      QPainter painter(m_lineNumArea);
      painter.fillRect(event->rect(), Qt::lightGray);

      QTextBlock block = firstVisibleBlock();
      int blockNumber = block.blockNumber();
      int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
      int bottom = top + (int) blockBoundingRect(block).height();

      while (block.isValid() && top <= event->rect().bottom()) {
         if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);

            painter.setPen(Qt::black);
            painter.drawText(0, top, m_lineNumArea->width(), fontMetrics().height(),Qt::AlignRight, number);
         }

         block = block.next();
         top = bottom;
         bottom = top + (int) blockBoundingRect(block).height();
         ++blockNumber;
      }
   }
}

int DiamondTextEdit::lineNum_Width()
{
   int digits = 3;
   int max = blockCount();

   for (int k=100; k < max; k *= 10)  {
      ++digits;
   }

   int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
   return space;
}

void DiamondTextEdit::update_LineNumWidth(int newBlockCount)
{   
   setViewportMargins(lineNum_Width(), 0, 0, 0);
}

void DiamondTextEdit::update_LineNumArea(const QRect &rect, int dy)
{
   if (dy) {
       m_lineNumArea->scroll(0, dy);

   }  else {
       m_lineNumArea->update(0, rect.y(), m_lineNumArea->width(), rect.height());
   }

   if (rect.contains(viewport()->rect())) {
      update_LineNumWidth(0);
   }
}

void DiamondTextEdit::resizeEvent(QResizeEvent *e)
{
   QPlainTextEdit::resizeEvent(e);

   QRect cr = contentsRect();
   m_lineNumArea->setGeometry(QRect(cr.left(), cr.top(), lineNum_Width(), cr.height()));
}

void DiamondTextEdit::setShowLineNum(bool data)
{
   m_showlineNum = data;
}

