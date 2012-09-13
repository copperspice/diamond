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

#ifndef LINE_NUMBERS_H
#define LINE_NUMBERS_H

#include <QObject>
#include <QPlainTextEdit>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QSize>
#include <QWidget>

class LineNumberArea;

class CodeEditor : public QPlainTextEdit
{
   Q_OBJECT

   public:
      CodeEditor(QWidget *parent = 0);

      void lineNumberAreaPaintEvent(QPaintEvent *event);
      int lineNumberAreaWidth();

   protected:
      void resizeEvent(QResizeEvent *event);

   private slots:
      void updateLineNumberAreaWidth(int newBlockCount);
      void updateLineNumberArea(const QRect &, int);

   private:
      QWidget *lineNumberArea;
};



class LineNumberArea : public QWidget
{
   public:
      LineNumberArea(CodeEditor *editor) : QWidget(editor) {
         codeEditor = editor;
      }

      QSize sizeHint() const {
         return QSize(codeEditor->lineNumberAreaWidth(), 0);
      }

   protected:
      void paintEvent(QPaintEvent *event) {
         codeEditor->lineNumberAreaPaintEvent(event);
      }

   private:
      CodeEditor *codeEditor;
};


#endif

