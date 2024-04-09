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

#include "diamond_edit.h"
#include "mainwindow.h"

#include <QApplication>
#include <QClipboard>
#include <QPainter>
#include <QShortcutEvent>

const QColor FILL_COLOR = QColor(0xD0D0D0);

DiamondTextEdit::DiamondTextEdit(MainWindow *from, struct Settings settings, SpellCheck *spell, QString owner)
      : QPlainTextEdit()
{
   m_mainWindow = from;
   m_record     = false;
   m_owner      = owner;

   // drag & drop
   setAcceptDrops(false);

   // column mode
   m_undoCount = 0;

   // line numbers
   m_showlineNum  = settings.showLineNumbers;
   m_isColumnMode = settings.isColumnMode;
   m_lineNumArea  = new LineNumArea(this);
   update_LineNumWidth(0);

   // syntax - assinged from loadfile(), runSyntax()
   m_synFName     = QString();
   m_syntaxParser = nullptr;

   // spell check
   m_spellCheck   = spell;
   m_isSpellCheck = settings.isSpellCheck;

   // line highlight bar
   connect(this, &DiamondTextEdit::blockCountChanged, this, &DiamondTextEdit::update_LineNumWidth);
   connect(this, &DiamondTextEdit::updateRequest,     this, &DiamondTextEdit::update_LineNumArea);
}

DiamondTextEdit::~DiamondTextEdit()
{
}

// ** line numbers
void DiamondTextEdit::lineNum_PaintEvent(QPaintEvent *event)
{
   if (m_showlineNum)  {

      QPainter painter(m_lineNumArea);
      painter.fillRect(event->rect(), FILL_COLOR);
      painter.setFont(font());

      QTextBlock block = firstVisibleBlock();
      int blockNumber  = block.blockNumber();

      int top    = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
      int bottom = top + (int) blockBoundingRect(block).height();

      while (block.isValid() && top <= event->rect().bottom()) {
         if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);

            painter.setPen(Qt::darkGray);
            painter.drawText(0, top, m_lineNumArea->width()-7, fontMetrics().height(), Qt::AlignRight, number);
         }

         block  = block.next();
         top    = bottom;
         bottom = top + (int) blockBoundingRect(block).height();
         ++blockNumber;
      }
   }
}

int DiamondTextEdit::lineNum_Width()
{
   int digits = 4;
   int max = blockCount();

   for (int k = 1000; k < max; k *= 10)  {
      ++digits;
   }

   int space = 8 + fontMetrics().width(QChar('9')) * digits;
   return space;
}

void DiamondTextEdit::update_LineNumWidth(int newBlockCount)
{
   (void) newBlockCount;

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

   if (! isSelected && m_isColumnMode) {
      // check for extra selection

      QList<QTextEdit::ExtraSelection> oldSelections = this->extraSelections();

      for (int k = 0; k < oldSelections.size(); ++k) {

         if (oldSelections[k].format.property(QTextFormat::UserProperty).toString() != "highlightbar") {
            isSelected = true;
            break;
         }
      }
   }

   //
   QMenu *menu = new QMenu(this);

   if (m_spellCheck && m_isSpellCheck)  {

      cursor.setPosition(cursorForPosition(event->pos()).position());
      cursor.select(QTextCursor::WordUnderCursor);
      selectedText = cursor.selectedText();

      // set up to save words, used in add_userDict() and replaceWord()
      m_cursor = cursor;

      QStringList m_maybeList = m_mainWindow->spell_getMaybe(selectedText);
      int cnt = m_maybeList.count();

      if (cnt > 0)  {
         for (int k = 0; k < cnt; ++k)  {
            menu->addAction(m_maybeList[k], m_mainWindow, SLOT(spell_replaceWord())  );
         }

         menu->addAction("Add to User Dictionary", m_mainWindow, SLOT(spell_addUserDict()) );
         menu->addSeparator();
      }
   }

   QAction *tempAction;

   if (isSelected) {
      menu->addAction("Rewrap Paragraph", m_mainWindow, SLOT(rewrapParagraph()) );
      menu->addAction("Sort Lines", m_mainWindow, SLOT(sortLines()));
      menu->addSeparator();
   }

   tempAction = menu->addAction("Undo", this, SLOT(undo()) );
   if (! document()->isUndoAvailable() ) {
      tempAction->setDisabled(true);
   }

   tempAction = menu->addAction("Redo", this, SLOT(redo()) );
   if (! document()->isRedoAvailable() ) {
      tempAction->setDisabled(true);
   }

   menu->addSeparator();

   tempAction = menu->addAction("Cut", this, SLOT(cut()) );
   if (! isSelected) {
      tempAction->setDisabled(true);
   }

   tempAction = menu->addAction("Copy", this, SLOT(copy()) );
   if (! isSelected) {
      tempAction->setDisabled(true);
   }

   tempAction = menu->addAction("Paste", this, SLOT(paste()) );

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
      menu->addAction("Select All", this, SLOT(selectAll()) );
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

void DiamondTextEdit::set_SyntaxEnum(SyntaxTypes syntaxData)
{
   m_syntaxEnum = syntaxData;
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
void DiamondTextEdit::set_ColumnMode(bool columnMode)
{
   m_isColumnMode = columnMode;
   m_mainWindow->changeFont();

   // leaving column mode
   if (! m_isColumnMode) {

      if (m_undoCount > 0 && ! m_colHighlight)  {
         removeColumnModeSpaces();
      }
   }

   // reset
   m_colHighlight = false;
   m_undoCount    = 0;
}

bool DiamondTextEdit::get_ColumnMode()
{
   return m_isColumnMode;
}

void DiamondTextEdit::set_ShowLineNum(bool showLine)
{
   m_showlineNum = showLine;
}

void DiamondTextEdit::removeColumnModeSpaces()
{
   QTextCursor cursor(textCursor());

   int startRow = m_endRow;
   int startCol = m_endCol;

   for (int k = 0; k < m_undoCount; k++) {
      this->undo();
   }

   // from the start of the document go to startRow
   cursor.movePosition(QTextCursor::Start);
   cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, startRow-1);

   // adjust to the correct column
   cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, startCol);
   setTextCursor(cursor);

   m_undoCount = 0;
}

void DiamondTextEdit::cut()
{
   if (m_isColumnMode) {

      QString text;
      QList<QTextEdit::ExtraSelection> oldSelections = this->extraSelections();

      // obtain text
      for (int k = 0; k < oldSelections.size(); ++k) {

         if (oldSelections[k].format.property(QTextFormat::UserProperty).toString() != "highlightbar") {
            text += oldSelections[k].cursor.selectedText() + "\n";
         }
      }

      // temporary, check mouse usage
      if (text.isEmpty()) {

         QTextCursor cursor(textCursor());
         QString selectedText = cursor.selectedText();

         if (! selectedText.isEmpty())  {
            QApplication::clipboard()->setText(selectedText);

            // save to copy buffer
            addToCopyBuffer(selectedText);

            cursor.removeSelectedText();
         }

      } else {

         QApplication::clipboard()->setText(text);

         // save to copy buffer
         addToCopyBuffer(text);

         // cut selected text
         QTextCursor cursorT(textCursor());
         cursorT.beginEditBlock();

         for (int k = 0; k < oldSelections.size(); ++k) {

            if (oldSelections[k].format.property(QTextFormat::UserProperty).toString() != "highlightbar") {
               oldSelections[k].cursor.removeSelectedText();
            }
         }

         cursorT.endEditBlock();
      }

   } else {
      QPlainTextEdit::cut();

   }
}

void DiamondTextEdit::copy()
{
   if (m_isColumnMode) {

      QString text;
      QList<QTextEdit::ExtraSelection> oldSelections = this->extraSelections();

      // obtain text
      for (int k = 0; k < oldSelections.count(); ++k) {

         if (oldSelections[k].format.property(QTextFormat::UserProperty).toString() != "highlightbar") {
            text += oldSelections[k].cursor.selectedText() + "\n";
         }
      }

      // remove last newline
      text.chop(1);

      // temporary, check mouse usage
      if (text.isEmpty()) {
         QTextCursor cursor(textCursor());
         QString selectedText = cursor.selectedText();

         if (! selectedText.isEmpty())  {
            text = selectedText;
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

      QString text = QApplication::clipboard()->text();
      QStringList lineList = text.split("\n");

      QTextCursor cursor(this->textCursor());
      cursor.beginEditBlock();

      // reset posStart to the beginning
      int posStart = cursor.selectionStart();
      cursor.setPosition(posStart);

      int spaceLen = cursor.columnNumber();

      for (int k = 0; k < lineList.count(); ++k) {

         cursor.insertText(lineList.at(k));

         if (k < lineList.count() - 1) {

            if (! cursor.movePosition(QTextCursor::NextBlock))  {
               // at end of document
               cursor.movePosition(QTextCursor::EndOfBlock);
               cursor.insertText("\n");
            }

            int length = cursor.block().length();

            if (length <= spaceLen)  {
               // current line is not long enough, add spaces

               for (int j = length; j <= spaceLen; ++j) {
                  cursor.insertText(" ");
               }
            }

         }
      }

      cursor.endEditBlock();

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


// ** process key press
bool DiamondTextEdit::event(QEvent *event)
{
   if (event->type() == QEvent::ShortcutOverride) {

      QKeyEvent *keyPressEvent = dynamic_cast<QKeyEvent *>(event);

      int key = keyPressEvent->key();
      int modifiers = keyPressEvent->modifiers();

      if (modifiers == Qt::ControlModifier && (key == Qt::Key_A || key == Qt::Key_X || key == Qt::Key_C  || key == Qt::Key_V)) {
         /// required to disable default selectAll(), cut(), copy(), paste()
         return false;
      }

   } else if (event->type() == QEvent::KeyPress) {

      QKeyEvent *keyPressEvent = dynamic_cast<QKeyEvent *>(event);

      int key = keyPressEvent->key();
      int modifiers = keyPressEvent->modifiers();

      if (modifiers == Qt::ControlModifier && (key == Qt::Key_A || key == Qt::Key_X || key == Qt::Key_C  || key == Qt::Key_V)) {
         /// required to disable default selectAll(), cut(), copy(), paste()
         return false;
      }

      if (m_isColumnMode) {

         if (modifiers == Qt::ShiftModifier &&
              ((key == Qt::Key_Up) || (key == Qt::Key_Down) || (key == Qt::Key_Left) || (key == Qt::Key_Right)) ) {

            QTextCursor cursor(this->textCursor());

            if (! m_colHighlight) {
               m_startRow = cursor.blockNumber()+1;
               m_startCol = cursor.columnNumber();

               m_endRow = m_startRow;
               m_endCol = m_startCol;

               m_colHighlight = true;
            }

            QColor textColor = QColor(Qt::white);
            QColor backColor = QColor(Qt::red);

            QList<QTextEdit::ExtraSelection> extraSelections;
            QTextEdit::ExtraSelection selection;

            QList<QTextEdit::ExtraSelection> oldSelections = this->extraSelections();

            for (int k=0; k < oldSelections.size(); ++k) {
               if (oldSelections[k].format.property(QTextFormat::UserProperty).toString() == "highlightbar") {
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
               --m_endRow;

               // from the start of the document go to startRow
               selection.cursor.movePosition(QTextCursor::Start);
               selection.cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, m_startRow-1);

            } else if (key == Qt::Key_Down)   {
               ++m_endRow;

               // from the start of the document go to startRow
               selection.cursor.movePosition(QTextCursor::Start);
               selection.cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, m_startRow-1);

            } else if (key == Qt::Key_Right)   {
               ++m_endCol;

               if (m_endCol-m_startCol > 0) {
                  selection.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, m_endCol-m_startCol);

                  int x1 = selection.cursor.selectionEnd();
                  int x2 = cursor.block().position() + cursor.block().length();

                  if (x1 == x2) {
                     // back it up
                     --m_endCol;
                     selection.cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1);
                  }

               } else {
                  selection.cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, m_startCol-m_endCol);
               }

            } else if (key == Qt::Key_Left)  {

               if (m_endCol > 0) {
                  --m_endCol;
               }

               if (m_startCol-m_endCol > 0) {
                  selection.cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, m_startCol-m_endCol);
               } else {
                  selection.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, m_endCol-m_startCol);
               }

            }

            // select text until cursor reaches endRow
            for (int k = m_startRow; k <= m_endRow; ++k)   {
               int length = selection.cursor.block().length();

               if (length <= m_endCol)  {
                  // current line is not long enough, add spaces which will be removed
                  selection.cursor.beginEditBlock();
                  m_undoCount++;

                  for (int j = length; j <= m_endCol; ++j) {
                     selection.cursor.insertText(" ");
                  }

                  selection.cursor.endEditBlock();
               }

               selection.cursor.setPosition(selection.cursor.block().position() + m_startCol);
               selection.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, m_endCol-m_startCol);

               // need to save every line
               extraSelections.append(selection);
               selection.cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, 1);
            }

            this->setExtraSelections(extraSelections);

            //  Note: If the selection obtained from an editor spans a line break, the text will contain a Unicode
            //  U+2029 paragraph separator character instead of a newline \n character. Use QString::replace() to
            //  replace these characters with newlines

            if (m_startRow != m_endRow || m_startCol != m_endCol) {
               copyAvailable(true);

            } else {
               copyAvailable(false);

            }

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

   if (m_isColumnMode) {

      if (m_undoCount > 0 && ! m_colHighlight)  {

         // copy may not be ctrl-c, test for this keySequence below
         // m_ui->actionCopy->setShortcut(QKeySequence(struct_temp.key_copy));

         bool ok = true;

         if (key == Qt::Key_Alt || key == Qt::Key_Control || key == Qt::Key_Shift) {
            ok = false;

         } else if (key == Qt::Key_C && (modifiers == Qt::ControlModifier)) {
            ok = false;

         }

         if (ok) {
            removeColumnModeSpaces();
         }
      }
   }

   // process keys now
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

void DiamondTextEdit::mousePressEvent(QMouseEvent *event)
{
   if (m_isColumnMode) {
      if (m_undoCount > 0 && ! m_colHighlight)  {
         removeColumnModeSpaces();
      }
   }

   // now call the parent
   QPlainTextEdit::mousePressEvent(event);
}


void DiamondTextEdit::keyReleaseEvent(QKeyEvent *event)
{
   int modifiers = event->modifiers();

   if (m_isColumnMode) {

      if (m_colHighlight && ((modifiers & Qt::ShiftModifier) == 0))  {
         // shift key was not pressed, out of selection, reset
         m_colHighlight = false;
      }
   }

   // now call the parent
   QPlainTextEdit::keyReleaseEvent(event);
}
