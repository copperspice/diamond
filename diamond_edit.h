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

#include "spellcheck.h"
#include "syntax.h"

#include <QObject>
#include <QPlainTextEdit>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QSize>
#include <QTextCursor>
#include <QWidget>

class MainWindow;
class LineNumberArea;

class DiamondTextEdit : public QPlainTextEdit
{
   Q_OBJECT

   public:
      DiamondTextEdit(MainWindow *from, struct Settings settings, SpellCheck *spell);
      ~DiamondTextEdit();

      void lineNum_PaintEvent(QPaintEvent *event);
      int lineNum_Width();

      void set_ShowLineNum(bool data);
      void set_ColumnMode(bool data);

      // spell
      void set_Spell(bool value);
      QTextCursor get_Cursor();

      // syntax
      QString get_SyntaxFile();
      void set_SyntaxFile(QString fname);
      Syntax * get_SyntaxParser();
      void set_SyntaxParser(Syntax *data);
      SyntaxTypes get_SyntaxEnum();
      void set_SyntaxEnum(SyntaxTypes data);

      // macro
      void macroStart();
      void macroStop();
      QList<QKeyEvent *> get_KeyList();

   protected:
      void contextMenuEvent(QContextMenuEvent *event);
      bool event(QEvent *event);
      void keyPressEvent(QKeyEvent *event);      
      void resizeEvent(QResizeEvent *event);      

   private:
      MainWindow *m_mainWindow;
      QWidget *m_lineNumArea;

      // spell check
      QTextCursor m_cursor;
      bool m_isSpellCheck;
      bool m_spellCheck;

      // syntax
      Syntax *m_syntaxParser;
      QString m_synFName;
      SyntaxTypes m_syntaxEnum;

      // macro
      bool m_record;
      QList<QKeyEvent *> m_keyList;

      bool m_showlineNum;
      bool m_isColumnMode;

      void showNotDone(QString item);

   private slots:
      void update_LineNumWidth(int newBlockCount);
      void update_LineNumArea(const QRect &rect, int value);
      void selectionChanged();

   public slots:     
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

