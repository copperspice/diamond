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

#ifndef DIAMOND_TEXTEDIT_H
#define DIAMOND_TEXTEDIT_H

#include <QObject>
#include <QPlainTextEdit>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QSize>
#include <QWidget>

class LineNumberArea;

class DiamondTextEdit : public QPlainTextEdit
{
   Q_OBJECT

   public:
      DiamondTextEdit();

      void setShowLineNum(bool data);
      void lineNum_PaintEvent(QPaintEvent *event);
      int lineNum_Width();

   protected:
      void resizeEvent(QResizeEvent *event);

   private:
      QWidget *m_lineNumArea;
      bool m_showlineNum;

   private slots:
      void update_LineNumWidth(int newBlockCount);
      void update_LineNumArea(const QRect &rect, int value);
};


class LineNumArea : public QWidget
{
   public:
      LineNumArea(DiamondTextEdit *editor) : QWidget(editor) {
         m_editor = editor;
      }

      QSize sizeHint() const {
         return QSize(m_editor->lineNum_Width(), 0);
      }

   protected:
      void paintEvent(QPaintEvent *event) {
         m_editor->lineNum_PaintEvent(event);
      }

   private:
     DiamondTextEdit *m_editor;
};


#endif

