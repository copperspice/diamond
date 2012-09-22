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

class MainWindow;
class LineNumberArea;

class DiamondTextEdit : public QPlainTextEdit
{
   Q_OBJECT

   public:
      DiamondTextEdit(MainWindow *from);

      void lineNum_PaintEvent(QPaintEvent *event);
      int lineNum_Width();

      void set_ShowLineNum(bool data);
      void set_ColumnMode(bool data);

   protected:
      void resizeEvent(QResizeEvent *event);
      void contextMenuEvent(QContextMenuEvent *event);

   private:
      MainWindow *m_mainWindow;
      QWidget *m_lineNumArea;

      bool m_showlineNum;
      bool m_isColumnMode;

      void showNotDone(QString item);

   private slots:
      void update_LineNumWidth(int newBlockCount);
      void update_LineNumArea(const QRect &rect, int value);

      void cut();
      void copy();
      void paste();
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

