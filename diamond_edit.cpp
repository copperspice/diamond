/**************************************************************************
*
* Copyright (c) 2012-2014 Barbara Geller
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
#include "mainwindow.h"

#include <QtGui>
#include <QLatin1Char>
#include <QShortcutEvent>
#include <Qt>

const QColor FILL_COLOR = QColor(0xD0D0D0);

DiamondTextEdit::DiamondTextEdit(MainWindow *from, struct Settings settings, SpellCheck *spell)
      : QPlainTextEdit()
{
   m_mainWindow   = from;
   m_record       = false;

   //
   m_showlineNum  = settings.showLineNumbers;
   m_isColumnMode = settings.isColumnMode;

   // syntax - assinged from loadfile(), runSyntax()
   m_synFName     = "";
   m_syntaxParser = 0;

   // spell check
   m_spellCheck   = spell;
   m_isSpellCheck = settings.isSpellCheck;

   // copy buffer

   // drag & drop
   setAcceptDrops(false);

   // line numbers
   m_lineNumArea = new LineNumArea(this);
   update_LineNumWidth(0);

   // line highlight
   connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(update_LineNumWidth(int)));
   connect(this, SIGNAL(updateRequest(const QRect &,int)), this, SLOT(update_LineNumArea(const QRect &,int)));
}

DiamondTextEdit::~DiamondTextEdit()
{
}

// ** line numbers
void DiamondTextEdit::lineNum_PaintEvent(QPaintEvent *event)
{   
   if (m_showlineNum)  {

      QPainter painter(m_lineNumArea);
      painter.fillRect(event->rect(), FILL_COLOR );

      QTextBlock block = firstVisibleBlock();
      int blockNumber = block.blockNumber();
      int top    = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
      int bottom = top + (int) blockBoundingRect(block).height();

      while (block.isValid() && top <= event->rect().bottom()) {
         if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);

            painter.setPen(Qt::darkGray);
            painter.drawText(0, top, m_lineNumArea->width()-7, fontMetrics().height(), Qt::AlignRight, number);
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
   int digits = 4;
   int max = blockCount();

   for (int k=1000; k < max; k *= 10)  {
      ++digits;
   }

   int space = 8 + fontMetrics().width(QLatin1Char('9')) * digits;
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


// ** context menu
QTextCursor DiamondTextEdit::get_Cursor()
{
   return m_cursor;
}

void DiamondTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
   bool isSelected = false;

   QTextCursor cursor(this->textCursor());
   QString selectedText = cursor.selectedText();

   if (! selectedText.isEmpty())  {
      isSelected = true;
   }

   //
   QMenu *menu = new QMenu(this);

   if (m_spellCheck && m_isSpellCheck)  {

      cursor.setPosition(cursorForPosition(event->pos()).position());
      cursor.select(QTextCursor::WordUnderCursor);      
      selectedText = cursor.selectedText();

      // save to use in add_userDict() and replaceWord()
      m_cursor = cursor;

      QStringList m_maybeList =  m_mainWindow->spell_getMaybe(selectedText);
      int cnt = m_maybeList.count();

      if (cnt > 0)  {
         for (int k = 0; k < cnt; ++k)  {
            menu->addAction(m_maybeList[k],  m_mainWindow, SLOT(spell_replaceWord())  );
         }

         menu->addAction("Add to User Dictionary",  m_mainWindow, SLOT(spell_addUserDict()) );
         menu->addSeparator();
      }
   }

   QAction *test;

   test = menu->addAction("Undo",   this, SLOT(undo())  );
   if (! this->document()->isUndoAvailable() ) {
      test->setDisabled(true);
   }

   test = menu->addAction("Redo",   this, SLOT(redo())  );
   if (! this->document()->isRedoAvailable() ) {
      test->setDisabled(true);
   }

   //
   menu->addSeparator();

   test = menu->addAction("Cut",    this, SLOT(cut())  );
   if (! isSelected) {
      test->setDisabled(true);
   }

   test = menu->addAction("Copy",   this, SLOT(copy())  );
   if (! isSelected) {
      test->setDisabled(true);
   }

   test = menu->addAction("Paste",  this, SLOT(paste()) );
   //if (! this->document()->isPasteAvailable() ) {
   //   test->setDisabled(true);
   //}

   menu->addSeparator();
   menu->addAction("Delete Line",    m_mainWindow, SLOT(deleteLine()) );
   menu->addAction("Delete to EOL",  m_mainWindow, SLOT(deleteEOL()) );

   if (isSelected) {
      menu->addSeparator();
      menu->addAction("Uppercase",   m_mainWindow, SLOT(caseUpper()) );
      menu->addAction("Lowercase",   m_mainWindow, SLOT(caseLower()) );

   } else {
      menu->addSeparator();
      menu->addAction("Insert Date", m_mainWindow, SLOT(insertDate()) );
      menu->addAction("Insert Time", m_mainWindow, SLOT(insertTime()) );

      menu->addSeparator();
      menu->addAction("Select All",  this, SLOT(selectAll()) );
   }

   menu->exec(event->globalPos());
   delete menu;
}

// ** syntax
QString DiamondTextEdit::get_SyntaxFile()
{
   return m_synFName;
}

void DiamondTextEdit::set_SyntaxFile(QString fname)
{
   m_synFName = fname;
}

Syntax * DiamondTextEdit::get_SyntaxParser()
{
   return m_syntaxParser;
}

void DiamondTextEdit::set_SyntaxParser(Syntax *parser)
{
   m_syntaxParser = parser;
}

SyntaxTypes DiamondTextEdit::get_SyntaxEnum()
{
   return m_syntaxEnum;
}

void DiamondTextEdit::set_SyntaxEnum(SyntaxTypes data)
{
   m_syntaxEnum = data;
}


// ** spell check
void DiamondTextEdit::set_Spell(bool value)
{
   m_isSpellCheck = value;   
   m_syntaxParser->set_Spell(value);

   // force syntax highlight to redraw
   m_syntaxParser->rehighlight();
}


// ** column mode
void DiamondTextEdit::set_ColumnMode(bool data)
{
   m_isColumnMode = data;
   m_mainWindow->changeFont();

   // reset
   colHighlight = false;
}

bool DiamondTextEdit::get_ColumnMode()
{
   return m_isColumnMode;
}

void DiamondTextEdit::set_ShowLineNum(bool data)
{
   m_showlineNum = data;
}


/*
void DiamondTextEdit::mouseMoveEvent(QMouseEvent *event)
{
   if (m_isColumnMode) {
      // Show x and y coordinate values of mouse cursor here
//    QString text = "X:"+QString::number(event->x())+"-- Y:"+QString::number(event->y());
//    m_mainWindow->setStatus_FName2(text);

      return true;
   }

   return QPlainTextEdit::mouseMoveEvent(event)

}  */


void DiamondTextEdit::cut()
{
   if (m_isColumnMode) {
      // broom - not working yet
      QPlainTextEdit::cut();

   } else {
      QPlainTextEdit::cut();

   }
}

void DiamondTextEdit::copy()
{
   if (m_isColumnMode) {     

      QString text;
      QList<QTextEdit::ExtraSelection> oldSelections = this->extraSelections();

      for (int k = 0; k < oldSelections.size(); ++k) {

         if (oldSelections[k].format.property(QTextFormat::UserProperty) != "highlightbar") {
            text += oldSelections[k].cursor.selectedText() + "\n";
         }
      }

      QApplication::clipboard()->setText(text);

      // save to copy buffer
      addToCopyBuffer(text);

   } else {
      QPlainTextEdit::copy();

      // save to copy buffer
      addToCopyBuffer( QApplication::clipboard()->text());
   }    

}

void DiamondTextEdit::paste()
{
   if (m_isColumnMode) {
      // adjust the columns
      QPlainTextEdit::paste();

   } else {
      QPlainTextEdit::paste();

   }
}

void DiamondTextEdit::addToCopyBuffer(const QString &text)
{
   m_copyBuffer.prepend(text);

   if (m_copyBuffer.size() > 10) {
      m_copyBuffer.removeLast();
   }
}

QList<QString> DiamondTextEdit::copyBuffer() const
{
   return m_copyBuffer;
}


// ** macros
void DiamondTextEdit::macroStart()
{
   m_record = true;

   // delete prior macro
   m_macroKeyList.clear();
}

void DiamondTextEdit::macroStop()
{
   m_record = false;
}

QList<QKeyEvent *> DiamondTextEdit::get_MacroKeyList()
{
   return m_macroKeyList;
}

void DiamondTextEdit::add_MacroEvent(QKeyEvent *event)
{
  m_macroKeyList.append(event);
}


// ** process key press
bool DiamondTextEdit::event(QEvent *event)
{
   if (event->type() == QEvent::ShortcutOverride) {

      QKeyEvent *keyPressEvent = dynamic_cast<QKeyEvent *>(event);

      int key = keyPressEvent->key();
      int modifiers = keyPressEvent->modifiers();

      if (modifiers == Qt::ControlModifier && (key == Qt::Key_A || key == Qt::Key_C)) {
         // required to disable default selectAll() and copy()
         return false;
      }

   } else if (event->type() == QEvent::KeyPress) {

      QKeyEvent *keyPressEvent = dynamic_cast<QKeyEvent *>(event);

      // BROOM - add test

      int key = keyPressEvent->key();
      int modifiers = keyPressEvent->modifiers();

      if (modifiers == Qt::ControlModifier && (key == Qt::Key_A || key == Qt::Key_C)) {
         // required to disable default selectAll() and copy()
         return false;   
      }

      if (m_isColumnMode) {

         if (modifiers == Qt::ShiftModifier &&
              ((key == Qt::Key_Up) || (key == Qt::Key_Down) || (key == Qt::Key_Left) || (key == Qt::Key_Right)) ) {

            QTextCursor cursor(this->textCursor());

            if (! colHighlight) {
               startRow = cursor.blockNumber()+1;
               startCol = cursor.columnNumber();

               endRow = startRow;
               endCol = startCol;

               colHighlight = true;
            }

            QColor textColor = QColor(Qt::white);
            QColor backColor = QColor(Qt::red);

            QList<QTextEdit::ExtraSelection> extraSelections;
            QTextEdit::ExtraSelection selection;

            QList<QTextEdit::ExtraSelection> oldSelections = this->extraSelections();

            for (int k=0; k < oldSelections.size(); ++k) {
               if (oldSelections[k].format.property(QTextFormat::UserProperty) == "highlightbar") {
                  extraSelections.append(oldSelections[k]);
                  break;
               }
            }

            //
            selection.format.setForeground(textColor);
            selection.format.setBackground(backColor);
            selection.cursor = cursor;

            // **
            if (key == Qt::Key_Up) {
               --endRow;

               // from the start of the document go to startRow
               selection.cursor.movePosition(QTextCursor::Start);
               selection.cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, startRow-1);

            } else if (key == Qt::Key_Down)   {
               ++endRow;

               // from the start of the document go to startRow
               selection.cursor.movePosition(QTextCursor::Start);
               selection.cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, startRow-1);


            } else if (key == Qt::Key_Right)   {
               ++endCol;

               if (endCol-startCol > 0) {
                  selection.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, endCol-startCol);

                  int x1 = selection.cursor.selectionEnd();
                  int x2 = cursor.block().position() + cursor.block().length();

                  if (x1 == x2) {
                     // back it up
                     --endCol;
                     selection.cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1);
                  }

               } else {
                  selection.cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, startCol-endCol);
               }

            } else if (key == Qt::Key_Left)  {

               if (endCol > 0) {
                  --endCol;
               }

               if (startCol-endCol > 0) {
                  selection.cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, startCol-endCol);
               } else {
                  selection.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, endCol-startCol);
               }

            }

            // select until endRow, endCol
            for (int k = startRow; k <= endRow; ++k)   {

               selection.cursor.setPosition(selection.cursor.block().position() + startCol);
               selection.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, endCol-startCol);

               // need to save every line
               extraSelections.append(selection);
               selection.cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 1);
            }


            //  Note: If the selection obtained from an editor spans a line break, the text will contain a Unicode
            //  U+2029 paragraph separator character instead of a newline \n character. Use QString::replace() to
            //  replace these characters with newlines


            extraSelections.append(selection);
            this->setExtraSelections(extraSelections);

            return true;
         }
      }

   }

   return QPlainTextEdit::event(event);
}

void DiamondTextEdit::keyPressEvent(QKeyEvent *event)
{
   int key = event->key();
   int modifiers = event->modifiers();

   if (m_record)  {
      QKeyEvent *newEvent;
      newEvent = new QKeyEvent(*event);

      m_macroKeyList.append(newEvent);
   }

   // process keys
   if (key == Qt::Key_Tab && (modifiers == Qt::NoModifier) ) {
      m_mainWindow->indentIncr("tab");
      return;

   } else if (key == Qt::Key_Backtab || (key == Qt::Key_Tab  && (modifiers == Qt::ShiftModifier)) ) {
      m_mainWindow->indentDecr("tab");
      return;

   }

   // now call the parent
   QPlainTextEdit::keyPressEvent(event);
}

void DiamondTextEdit::keyReleaseEvent(QKeyEvent *event)
{
   // int key = event->key();
   int modifiers = event->modifiers();

   if (m_isColumnMode && ((modifiers & Qt::ShiftModifier) == 0) ) {
      // shift key is not pressed, then reset
      colHighlight = false;
   }

   // now call the parent
   QPlainTextEdit::keyReleaseEvent(event);
}

