/**************************************************************************
*
* Copyright (c) 2012-2024 Barbara Geller
*
* Diamond Editor is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License version 2
* as published by the Free Software Foundation.
*
* Diamond is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
***************************************************************************/

#ifndef DIAMOND_TEXTEDIT_H
#define DIAMOND_TEXTEDIT_H

#include "spellcheck.h"
#include "syntax.h"

#include <QList>
#include <QObject>
#include <QPaintEvent>
#include <QPlainTextEdit>
#include <QResizeEvent>
#include <QSize>
#include <QTextCursor>
#include <QWidget>

class LineNumberArea;
class MainWindow;

class DiamondTextEdit : public QPlainTextEdit
{
   CS_OBJECT(DiamondTextEdit)

   public:
      DiamondTextEdit(MainWindow *from, struct Settings settings, SpellCheck *spell, QString owner);
      ~DiamondTextEdit();

      QString m_owner;

      void lineNum_PaintEvent(QPaintEvent *event);
      int lineNum_Width();

      void set_ShowLineNum(bool data);

      // column mode
      void set_ColumnMode(bool data);
      bool get_ColumnMode();

      // copy buffer
      QList<QString> copyBuffer() const;

      // macro
      void macroStart();
      void macroStop();
      QList<QKeyEvent *> get_MacroKeyList();

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

      CS_SLOT_1(Public, void cut())
      CS_SLOT_2(cut)

      CS_SLOT_1(Public, void copy())
      CS_SLOT_2(copy)

      CS_SLOT_1(Public, void paste())
      CS_SLOT_2(paste)

   protected:
      void contextMenuEvent(QContextMenuEvent *event);
      bool event(QEvent *event);
      void keyPressEvent(QKeyEvent *event);
      void keyReleaseEvent(QKeyEvent *event);
      void resizeEvent(QResizeEvent *event);
      void mousePressEvent(QMouseEvent *event);

   private:
      MainWindow *m_mainWindow;
      QWidget *m_lineNumArea;

      // column mode
      bool m_isColumnMode;
      int m_undoCount;
      void removeColumnModeSpaces();

      bool m_showlineNum;
      bool m_colHighlight;

      int m_startRow;
      int m_startCol;
      int m_endRow;
      int m_endCol;

      // copy buffer
      QList<QString> m_copyBuffer;
      void addToCopyBuffer(const QString &text);

      // macro
      bool m_record;
      QList<QKeyEvent *> m_macroKeyList;

      // spell check
      QTextCursor m_cursor;
      bool m_isSpellCheck;
      SpellCheck *m_spellCheck;

      // syntax
      Syntax *m_syntaxParser;
      QString m_synFName;
      SyntaxTypes m_syntaxEnum;

      CS_SLOT_1(Private, void update_LineNumWidth(int newBlockCount))
      CS_SLOT_2(update_LineNumWidth)

      CS_SLOT_1(Private, void update_LineNumArea(const QRect & rect,int value))
      CS_SLOT_2(update_LineNumArea)
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

