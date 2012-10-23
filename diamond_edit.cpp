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
#include "mainwindow.h"
#include "util.h"

#include <QtGui>
#include <QStringList>
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

   // syntax (assinged from loadfile(), runSyntax()
   m_synFName     = "";
   m_syntaxParser = 0;

   // spell check
   m_spellCheck   = spell;
   m_isSpellCheck = settings.isSpellCheck;

   //
   m_lineNumArea = new LineNumArea(this);
   update_LineNumWidth(0);

   // drag & drop
   setAcceptDrops(false);

   // line highlight
   connect(this, SIGNAL(blockCountChanged(int)),   this, SLOT(update_LineNumWidth(int)));
   connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(update_LineNumArea(QRect,int)));  

   // column mode

}

DiamondTextEdit::~DiamondTextEdit()
{
}

// ** line highlight
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

   menu->addAction("Undo",   this, SLOT(undo())  );
   menu->addAction("Redo",   this, SLOT(redo())  );

   menu->addSeparator();
   menu->addAction("Cut",    this, SLOT(cut())   );
   menu->addAction("Copy",   this, SLOT(copy())  );
   menu->addAction("Paste",  this, SLOT(paste()) );

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

   // reset
   colHighlight = false;
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

/*    QTextCursor cursor(this->textCursor());
      cursor.beginEditBlock();

      cursor.removeSelectedText();
      this->setTextCursor(cursor);

      cursor.endEditBlock();
*/

      QPlainTextEdit::cut();

   } else {
      QPlainTextEdit::cut();

   }
}

void DiamondTextEdit::copy()
{
   if (m_isColumnMode) {
      // QTextCursor cursor( this->textCursor());
      // QString text = cursor().selectedText();

      QPlainTextEdit::copy();

   } else {     
      QPlainTextEdit::copy();
   }
}

void DiamondTextEdit::paste()
{
   if (m_isColumnMode) {
      QPlainTextEdit::paste();

   } else {
      QPlainTextEdit::paste();

   }
}


/*

::copy()
{
   if ( !m_cursor.hasSelection() ) {
      return;
   }

   QApplication::clipboard()->setText(m_cursor.selectedText());
}

::paste()
{
   const QMimeData *d = QApplication::clipboard()->mimeData();
   if ( d ) {
      insertFromMimeData(d);
   }
}

   QClipboard *clip = QApplication::clipboard();
   QString text = textCursor().selection().toPlainText();
   data->setText(result);
   clip->setMimeData(data);
*/


void DiamondTextEdit::showNotDone(QString item)
{
   csMsg( item + " - this feature has not been implemented.");
}


// ** macros
void DiamondTextEdit::macroStart()
{
   m_record = true;
}

void DiamondTextEdit::macroStop()
{
   m_record = false;
}

QList<QKeyEvent *> DiamondTextEdit::get_KeyList()
{
   return m_keyList;
}


// ** process key press
bool DiamondTextEdit::event(QEvent *event)
{
   if (event->type() == QEvent::KeyPress) {

      QKeyEvent *keyPressEvent = dynamic_cast<QKeyEvent *>(event);

      int key = keyPressEvent->key();
      int modifiers = keyPressEvent->modifiers();

      if (key == Qt::Key_Tab && (modifiers == Qt::NoModifier) ) {

         // reset
         colHighlight = false;

         if (m_record)  {
            QKeyEvent *newEvent;
            newEvent = new QKeyEvent(*keyPressEvent);

            m_keyList.append(newEvent);
         }

         m_mainWindow->indentIncr("tab");
         return true;

      } else if (key == Qt::Key_Backtab ||
            (key == Qt::Key_Tab  && (modifiers == Qt::ShiftModifier)) ) {

         // reset
         colHighlight = false;

         if (m_record)  {
            QKeyEvent *newEvent;
            newEvent = new QKeyEvent(*keyPressEvent);

            m_keyList.append(newEvent);
         }

         m_mainWindow->indentDecr("tab");
         return true;         

/*

      } else if (m_isColumnMode && (modifiers == Qt::ShiftModifier) &&
            ( (key == Qt::Key_Up)   || (key == Qt::Key_Down) ||
              (key == Qt::Key_Left) || (key == Qt::Key_Right)) ) {

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
            selection.cursor.movePosition(QTextCursor::Start);
            selection.cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, startRow-1);

            for(int k=startRow; k <= endRow; ++k)   {
               selection.cursor.movePosition(QTextCursor::StartOfLine);
               selection.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, endCol-startCol);

               if (k < endRow) {
                  extraSelections.append(selection);
                  selection.cursor.movePosition(QTextCursor::Down,  QTextCursor::MoveAnchor, 1);
               }
            }

         } else if (key == Qt::Key_Down)   {

            ++endRow;

            selection.cursor.movePosition(QTextCursor::Start);
            selection.cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, startRow-1);

            for(int k=startRow; k <= endRow; ++k)   {
               selection.cursor.movePosition(QTextCursor::StartOfLine);
               selection.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, endCol-startCol);

               if (k < endRow) {
                  extraSelections.append(selection);
                  selection.cursor.movePosition(QTextCursor::Down,  QTextCursor::MoveAnchor, 1);
               }
            }

         } else if (key == Qt::Key_Right)   {

            ++endCol;
            selection.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, endCol-startCol);

         } else if (key == Qt::Key_Left)   {

            --endCol;
            selection.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, endCol-startCol);

         }

         extraSelections.append(selection);
         this->setExtraSelections(extraSelections);

         // TEST
         m_mainWindow->setStatus_FName2( QString::number(endRow) + "  " + QString::number(endCol));

         return true;
*/

      }

   } else {
      // reset
      colHighlight = false;
   }

   return QPlainTextEdit::event(event);
}

void DiamondTextEdit::keyPressEvent(QKeyEvent *event)
{
   // int key = event->key();
   // int modifiers = event->modifiers();

   if (m_record)  {

      QKeyEvent *newEvent;
      newEvent = new QKeyEvent(*event);

      m_keyList.append(newEvent);

      // now call the parent
      QPlainTextEdit::keyPressEvent(event);

   }  else {
      // now call the parent
      QPlainTextEdit::keyPressEvent(event);
   }
}


