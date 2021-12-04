/**************************************************************************
*
* Copyright (c) 2012-2021 Barbara Geller
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

#include "dialog_macro.h"
#include "dialog_open.h"
#include "dialog_symbols.h"
#include "mainwindow.h"

#include <QDate>
#include <QFileDialog>
#include <QFileInfo>
#include <QTime>

// ** file
void MainWindow::newFile()
{
   bool okClose = querySave();

   if (okClose) {
      m_textEdit->clear();
      setCurrentTitle("");
   }
}

void MainWindow::open_RelatedFile()
{
   QFileInfo tmp(m_curFile);
   QString ext = tmp.suffix();

   if (ext == "cpp" || ext == "c" || ext == "cc" || ext == "m" || ext == "mm" || ext == "h") {
      QStringList list;

      QString tFile;
      QString baseName = tmp.canonicalPath() + "/" +  tmp.completeBaseName();

      if (ext == "cpp" || ext == "c" || ext == "cc" || ext == "m" || ext == "mm") {
         tFile = baseName + ".h";

         if ( QFile::exists(tFile) ) {
            list.append(tFile);
         }

         tFile = baseName + "_p.h";

         if ( QFile::exists(tFile) ) {
            list.append(tFile);
         }

      } else if (baseName.endsWith("_p") && ext == "h")  {

         baseName.chop(2);

         tFile = baseName + ".cpp";

         if ( QFile::exists(tFile) ) {
            list.append(tFile);
         }

         tFile = baseName + ".c";

         if ( QFile::exists(tFile) ) {
            list.append(tFile);
         }

         tFile = baseName + ".cc";

         if ( QFile::exists(tFile) ) {
            list.append(tFile);
         }

         tFile = baseName + ".h";

         if ( QFile::exists(tFile) ) {
            list.append(tFile);
         }

      } else if (ext == "h")  {

         tFile = baseName + ".cpp";

         if ( QFile::exists(tFile) ) {
            list.append(tFile);
         }

         tFile = baseName + ".c";

         if ( QFile::exists(tFile) ) {
            list.append(tFile);
         }

         tFile = baseName + ".c";

         if ( QFile::exists(tFile) ) {
            list.append(tFile);
         }

         tFile = baseName + "_p.h";

         if ( QFile::exists(tFile) ) {
            list.append(tFile);
         }
      }

      //
      int cnt = list.count();

      if (cnt == 0) {
         csError(tr("Open Related Files"), tr("No related files were found"));

      } else if (cnt == 1)  {
         // open the one related file
         this->loadFile(list.at(0), true, false);

      }  else {
         // display the full list of related files
         Dialog_Open *dw = new Dialog_Open(this, list);
         int result = dw->exec();

         if (result == QDialog::Accepted) {
            QString tmpF = dw->get_FileName();
            this->loadFile(tmpF, true, false);
         }

         delete dw;
      }

   } else {
      csError(tr("Open Related Files"), tr("Related files only configured for .cpp, .c, .cc, .mm and .h files"));

   }
}

void MainWindow::openDoc(QString path)
{
   QString selectedFilter;
   QFileDialog::FileDialogOptions options;

   // force windows 7 and 8 to honor initial path
   options = QFileDialog::ForceInitialDir_Win7;

   QStringList fileList = QFileDialog::getOpenFileNames(this, tr("Select File"),
         path, tr("All Files (*)"), &selectedFilter, options);

   for (const QString &fileName : fileList)  {
      if (! fileName.isEmpty()) {
         loadFile(fileName, true, false);
      }
   }
}

bool MainWindow::close_Doc()
{
   bool okClose = querySave();

   if (okClose) {

      if (m_isSplit) {

         if (m_splitFileName == m_curFile) {
            // close the split tab
            split_CloseButton();
         }

         rm_splitCombo(m_curFile);
      }

      openTab_Delete();

      m_textEdit->clear();
      setCurrentTitle("");
   }

   return okClose;
}

bool MainWindow::closeAll_Doc(bool isExit)
{
   bool allClosed = true;

   QWidget *tmp;
   DiamondTextEdit *textEdit;

   int count = m_tabWidget->count();
   int whichTab = 0;

   // clear open tab list
   m_openedFiles.clear();
   m_openedModified.clear();

   for (int k = 0; k < count; ++k) {

      tmp = m_tabWidget->widget(whichTab);
      textEdit = dynamic_cast<DiamondTextEdit *>(tmp);

      if (textEdit) {
         m_textEdit = textEdit;
         m_curFile  = this->get_curFileName(whichTab);

         bool okClose = querySave();

         if (okClose)  {

            if (isExit && (m_curFile != "untitled.txt")) {
               // save for the auto reload
               m_openedFiles.append(m_curFile);
               m_openedModified.append(false);
            }

            if (m_tabWidget->count() == 1) {
               // do not remove this tab !

               m_textEdit->clear();
               setCurrentTitle("");

            } else {
               m_tabWidget->removeTab(whichTab);

            }

         } else  {
            // modified file not closed, move over one tab
            ++whichTab;

            if ( m_curFile != "untitled.txt" ) {
               // save for the auto reload
               m_openedFiles.append(m_curFile);
               m_openedModified.append(true);
            }

            // at least one tab is staying open
            allClosed = false;
         }
      }
   }

   if (isExit && allClosed) {
      // about to close diamond

   } else {
      if (m_isSplit) {
         // close the split tab
         split_CloseButton();
      }

      // update open tab list
      openTab_UpdateActions();

      m_tabWidget->setCurrentIndex(0);
   }

   return allClosed;
}

void MainWindow::reload()
{
   if (m_curFile.isEmpty()) {
      csError(tr("Reload"), tr("Unable to reload a file which was not saved."));

   } else if (m_textEdit->document()->isModified()) {

      QMessageBox quest;
      quest.setWindowTitle(tr("Reload File"));
      quest.setText(tr("File: ") + m_curFile + tr(" has been modified. Reload file?"));

      QPushButton *reload = quest.addButton("Reload", QMessageBox::AcceptRole );
      quest.setStandardButtons(QMessageBox::Cancel);
      quest.setDefaultButton(QMessageBox::Cancel);

      quest.exec();

      if (quest.clickedButton() == reload) {
        loadFile(m_curFile, false, false, true);
      }

   } else {
      loadFile(m_curFile, false, false, true);

   }
}

bool MainWindow::save()
{
   if (m_curFile.isEmpty()) {
      return saveAs(SAVE_ONE);

   } else {
     return saveFile(m_curFile, SAVE_ONE);

   }
}

bool MainWindow::saveAs(SaveFiles saveType)
{
   bool retval = false;

   QString selectedFilter;
   QFileDialog::FileDialogOptions options;

   // find the current or prior path
   QString path = pathName(m_curFile);

   if (path.isEmpty() || path == ".")  {
      path = m_struct.pathPrior;

      if (path.isEmpty()) {
         path = QDir::homePath();
      }
   }

   // force windows 7 and 8 to honor initial path
   options = QFileDialog::ForceInitialDir_Win7;

   QString fileName = QFileDialog::getSaveFileName(this, tr("Create or Select File"),
        path, tr("All Files (*)"), &selectedFilter, options);

   if (fileName.isEmpty()) {
      retval = false;

   } else {
      retval = saveFile(fileName, saveType);

      if (retval) {
         // update open tab list
         openTab_Delete();

         if (m_isSplit) {
            rm_splitCombo(m_curFile);
         }

         setCurrentTitle(fileName);

         // update open tab list
         openTab_Add();

         if (m_isSplit) {
            add_splitCombo(m_curFile);
            set_splitCombo();
         }
      }
   }

   return retval;
}

void MainWindow::saveAll()
{
   // hold for reload
   DiamondTextEdit *hold_textEdit = m_textEdit;
   int hold_index = m_tabWidget->currentIndex();

   QString fileName;

   QWidget *tmp;
   DiamondTextEdit *textEdit;

   int count = m_tabWidget->count();

   for (int k = 0; k < count; ++k) {

      tmp = m_tabWidget->widget(k);
      textEdit = dynamic_cast<DiamondTextEdit *>(tmp);

      if (textEdit) {
         m_textEdit = textEdit;
         fileName   = m_tabWidget->tabWhatsThis(k);

         if (m_textEdit->document()->isModified())  {

            if (fileName == "untitled.txt") {
               m_tabWidget->setCurrentIndex(k);
               saveAs(SAVE_ALL);

            } else  {
               saveFile(fileName, SAVE_ALL);

            }
         }
      }
   }

   // reload the current textEdit again
   m_textEdit = hold_textEdit;

   if (m_tabWidget->currentIndex() == hold_index) {

      fileName = m_tabWidget->tabWhatsThis(hold_index);

      if (fileName == "untitled.txt") {
         m_curFile = "";

      } else {
         m_curFile = fileName;
         setDiamondTitle(m_curFile);
      }

   } else {
      m_tabWidget->setCurrentIndex(hold_index);

   }

   if (! m_textEdit->document()->isModified())  {
      setWindowModified(false);
   }

   if (m_isSplit) {
      set_splitCombo();
   }

   setStatusBar(tr("File(s) saved"), 2000);
}


// **edit
void MainWindow::mw_undo()
{
   m_textEdit->undo();
}

void MainWindow::mw_redo()
{
   m_textEdit->redo();
}

void MainWindow::mw_cut()
{
   m_textEdit->cut();
}

void MainWindow::mw_copy()
{
   m_textEdit->copy();
}

void MainWindow::mw_paste()
{
   m_textEdit->paste();
}

void MainWindow::selectAll()
{
   QTextCursor cursor(m_textEdit->textCursor());
   cursor.select(QTextCursor::Document);
   m_textEdit->setTextCursor(cursor);
}

void MainWindow::selectBlock()
{
   QTextCursor cursor(m_textEdit->textCursor());
   cursor.select(QTextCursor::BlockUnderCursor);
   m_textEdit->setTextCursor(cursor);
}

void MainWindow::selectLine()
{
   QTextCursor cursor(m_textEdit->textCursor());
   cursor.select(QTextCursor::LineUnderCursor);
   m_textEdit->setTextCursor(cursor);
}

void MainWindow::selectWord()
{
   QTextCursor cursor(m_textEdit->textCursor());
   cursor.select(QTextCursor::WordUnderCursor);
   m_textEdit->setTextCursor(cursor);
}

void MainWindow::caseUpper()
{
   QTextCursor cursor(m_textEdit->textCursor());
   QString text = cursor.selectedText();

   if (text.isEmpty())  {
      cursor.select(QTextCursor::WordUnderCursor);
      text = cursor.selectedText();
   }

   cursor.removeSelectedText();
   cursor.insertText(text.toUpper());
}

void MainWindow::caseLower()
{
   QTextCursor cursor(m_textEdit->textCursor());
   QString text = cursor.selectedText();

   if (text.isEmpty())  {
      cursor.select(QTextCursor::WordUnderCursor);
      text = cursor.selectedText();
   }

   cursor.removeSelectedText();
   cursor.insertText(text.toLower());
}

void MainWindow::caseCap()
{
   QTextCursor cursor(m_textEdit->textCursor());
   QString text = cursor.selectedText();

   if (text.isEmpty())  {
      cursor.select(QTextCursor::WordUnderCursor);
      text = cursor.selectedText();
   }

   text = text.toLower();
   text.replace(0, 1, text[0].toUpper());

   cursor.removeSelectedText();
   cursor.insertText(text);
}

void MainWindow::insertDate()
{
   QDate date  = QDate::currentDate();
   QString tmp = date.toString(m_struct.formatDate);

   m_textEdit->insertPlainText(tmp);
}

void MainWindow::insertTime()
{
   QTime time  = QTime::currentTime();
   QString tmp = time.toString(m_struct.formatTime);

   m_textEdit->insertPlainText(tmp);
}

void MainWindow::insertSymbol()
{
   Dialog_Symbols *dw = new Dialog_Symbols(this);
   int result = dw->exec();

   if ( result == QDialog::Accepted) {
      QString text = dw->get_Symbol();

      if (! text.isEmpty() ) {

         if (text.toLower() == "tab character")  {
            m_textEdit->textCursor().insertText(QChar(9));

         } else if (text.toLower() == "# sign")  {
            m_textEdit->textCursor().insertText("&#35;");

         } else  {
            m_textEdit->textCursor().insertText(text);
         }
      }
   }

   delete dw;
}

void MainWindow::indentIncr(QString route)
{
   const QString tabLen = QString(m_struct.tabSpacing, ' ');
   QTextCursor cursor(m_textEdit->textCursor());

   cursor.beginEditBlock();

   if (cursor.hasSelection()) {
      int posStart = cursor.selectionStart();
      int posEnd   = cursor.selectionEnd();

      // reset posEnd to the end to the last selected line
      cursor.setPosition(posEnd);
      cursor.movePosition(QTextCursor::EndOfLine);
      posEnd = cursor.position();

      // reset posStart to the beginning of the first selected line
      cursor.setPosition(posStart);
      cursor.movePosition(QTextCursor::StartOfLine);
      posStart = cursor.position();

      while (true) {
         cursor.movePosition(QTextCursor::StartOfLine);

         if (m_struct.useSpaces) {
            cursor.insertText(tabLen);
            posEnd += m_struct.tabSpacing;

         } else {
            cursor.insertText(QChar('\t'));
            posEnd += 1;

         }

         if (! cursor.movePosition(QTextCursor::NextBlock))  {
             break;
         }

         if (cursor.position() >= posEnd) {
            break;
         }
      }

      // reselect highlighted text
      cursor.clearSelection();

      cursor.setPosition(posStart);
      cursor.setPosition(posEnd, QTextCursor::KeepAnchor);

      m_textEdit->setTextCursor(cursor);

   }  else {

      if (route == "indent") {
         cursor.movePosition(QTextCursor::StartOfLine);
      }

      if (m_struct.useSpaces) {
         cursor.insertText(tabLen);

      } else {
         cursor.insertText(QChar('\t'));

      }
   }

   cursor.endEditBlock();
}

void MainWindow::indentDecr(QString route)
{
   QTextCursor cursor(m_textEdit->textCursor());
   cursor.beginEditBlock();

   if (cursor.hasSelection()) {
      int posStart = cursor.selectionStart();
      int posEnd   = cursor.selectionEnd();

      // reset posEnd to the end to the last selected line
      cursor.setPosition(posEnd);
      cursor.movePosition(QTextCursor::EndOfLine);
      posEnd = cursor.position();

      // reset posStart to the beginning of the first selected line
      cursor.setPosition(posStart);
      cursor.movePosition(QTextCursor::StartOfLine);
      posStart = cursor.position();

      QString tmp;

      while (true) {
         cursor.movePosition(QTextCursor::StartOfLine);

         for (int k = 0; k < m_struct.tabSpacing; ++k) {

            int t_block = cursor.block().blockNumber();
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);

            if (t_block != cursor.block().blockNumber() )  {
               cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1);
               break;
            }

            tmp = cursor.selectedText().trimmed();
            if (! tmp.isEmpty())  {
               break;
            }

            if (m_struct.useSpaces) {
               cursor.deleteChar();

            } else {
               // what about tabs
               cursor.deleteChar();
            }

            posEnd -=1;
         }

         //
         if (! cursor.movePosition(QTextCursor::NextBlock))  {
             break;
         }

         if (cursor.position() >= posEnd) {
            break;
         }
      }

      // reselect highlighted text
      cursor.clearSelection();

      cursor.setPosition(posStart);
      cursor.setPosition(posEnd, QTextCursor::KeepAnchor);

      m_textEdit->setTextCursor(cursor);

   }  else {

      int posStart = cursor.position();

      if (route == "unindent") {
         cursor.movePosition(QTextCursor::StartOfLine);
      }

      QString tmp;

      for (int k=0; k < m_struct.tabSpacing; ++k) {

         cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);
         tmp = cursor.selectedText().trimmed();

         if (! tmp.isEmpty())  {
            break;
         }

         if (m_struct.useSpaces) {
            cursor.deleteChar();
         } else {
            // what about tabs
            cursor.deleteChar();
         }

         if (route == "unindent") {
            posStart -=1;
         }
      }

      //
      cursor.setPosition(posStart, QTextCursor::MoveAnchor);
      m_textEdit->setTextCursor(cursor);
   }

   cursor.endEditBlock();
}

void MainWindow::deleteLine()
{
   QTextCursor cursor(m_textEdit->textCursor());
   cursor.beginEditBlock();

   cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
   cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
   cursor.removeSelectedText();
   m_textEdit->setTextCursor(cursor);

   cursor.endEditBlock();
}

void MainWindow::deleteEOL()
{
   QTextCursor cursor(m_textEdit->textCursor());
   cursor.beginEditBlock();

   cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
   cursor.removeSelectedText();
   m_textEdit->setTextCursor(cursor);

   cursor.endEditBlock();
}

void MainWindow::rewrapParagraph()
{
   QTextCursor cursor(m_textEdit->textCursor());
   cursor.beginEditBlock();

   if (m_struct.rewrapColumn == 0) {
      m_struct.rewrapColumn = 120;
   }

   if (cursor.hasSelection()) {
      int posStart = cursor.selectionStart();
      int posEnd   = cursor.selectionEnd();

      // reset posEnd to the end to the last selected line
      cursor.setPosition(posEnd);
      cursor.movePosition(QTextCursor::EndOfLine);
      posEnd = cursor.position();

      // reset posStart to the beginning of the first selected line
      cursor.setPosition(posStart);
      cursor.movePosition(QTextCursor::StartOfLine);
      posStart = cursor.position();

      // select all of the text
      cursor.setPosition(posEnd, QTextCursor::KeepAnchor);

      QString tmp = cursor.selectedText();
      tmp.replace('\n', ' ');             // line feed
      tmp.replace(QChar(0x2029), " ");    // paragraph

      QString tmpSave = tmp.simplified();

      //
      QStringList lines;

      while (! tmp.isEmpty()) {
         lines.append(tmp.left(m_struct.rewrapColumn));
         tmp.remove(0, m_struct.rewrapColumn);
      }

      QString hold;

      for (QString &str : lines)  {
         str.prepend(hold);
         hold = "";

         while (! str.isEmpty()) {
            int len    = str.size();
            QChar last = str[len - 1];

            while (len >= m_struct.rewrapColumn) {
              // line is too long
              hold.prepend(last);
              str.chop(1);

              --len;
              last = str[len - 1];
            }

            if (last.isSpace()) {
               str.chop(1);
               break;

            } else {
              // keep the chars which are moving down
              hold.prepend(last);
              str.chop(1);

            }
         }

      }

      if (! hold.isEmpty()) {
         // retrieve the last line
         QString lastLine = lines.takeLast() + " " + hold;
         hold = "";

         // is this line too long now
         int len    = lastLine.size();
         QChar last = lastLine[len - 1];

         while (len >=  m_struct.rewrapColumn) {
            // line is too long
           hold.prepend(last);
           lastLine.chop(1);

           --len;
           last = lastLine[len - 1];
         }

         if (! hold.isEmpty()) {
            while (true) {
               len  = lastLine.size();
               last = lastLine[len - 1];

               if (last.isSpace()) {
                  lastLine.chop(1);
                  break;

               } else {
                 // keep the chars which are moving down
                 hold.prepend(last);
                 lastLine.chop(1);

               }
            }
         }

         // update
         lines.append(lastLine);

         if (! hold.isEmpty()) {
            lines.append(hold);
         }
      }

      tmp = lines.join("\n");

      if (tmpSave == tmp.simplified())  {
         // remove the selected text
         cursor.removeSelectedText();

         // insert new text
         cursor.insertText(tmp);

      } else {
         csMsg("Warning: New text for rewrapParagrah is not the same as the input, rewrap aborted");

      }

   } else {
      csMsg("No text or paragraph was selected to rewrap");

   }

   cursor.endEditBlock();
}

void MainWindow::columnMode()
{
   // alters cut, copy, paste
   if (m_ui->actionColumn_Mode->isChecked()) {
      // on
      m_struct.isColumnMode = true;

   } else {
      // off
      m_struct.isColumnMode = false;

   }

   json_Write(COLUMN_MODE);
   setStatus_ColMode();
}


// ** search methods are located in search.cpp

void MainWindow::goLine()
{
   int line = get_line_col("line");

   if (line > 0) {
      // save original position
      // int pos = m_textEdit->verticalScrollBar()->value();

      QTextCursor cursor(m_textEdit->document()->findBlockByNumber(line - 1));
      m_textEdit->setTextCursor(cursor);

      // set to original postion - following does not work
      // m_textEdit->verticalScrollBar()->setValue(pos);
   }
}

void MainWindow::goColumn()
{
   int col = get_line_col("col");

   QTextCursor cursor(m_textEdit->textCursor());
   cursor.movePosition(QTextCursor::StartOfLine);

   cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, col);
   m_textEdit->setTextCursor(cursor);
}

void MainWindow::goTop()
{
   QTextCursor cursor(m_textEdit->textCursor());
   cursor.movePosition(QTextCursor::Start);
   m_textEdit->setTextCursor(cursor);
}

void MainWindow::goBottom()
{
   QTextCursor cursor(m_textEdit->textCursor());
   cursor.movePosition(QTextCursor::End);
   m_textEdit->setTextCursor(cursor);
}


// **view
void MainWindow::lineHighlight()
{
   if (m_ui->actionLine_Highlight->isChecked()) {
      // on
      m_struct.showLineHighlight = true;

   } else  {
      // off
      m_struct.showLineHighlight = false;
   }

   json_Write(SHOW_LINEHIGHLIGHT);
   moveBar();
}

void MainWindow::moveBar()
{
   QList<QTextEdit::ExtraSelection> extraSelections;
   QTextEdit::ExtraSelection selection;

   QColor textColor;
   QColor backColor;

   if (m_struct.showLineHighlight) {
      // on
      textColor = m_struct.colorHighText;
      backColor = m_struct.colorHighBack;

   } else  {
      // off
      textColor = m_struct.colorText;
      backColor = m_struct.colorBack;
   }

   selection.format.setForeground(textColor);
   selection.format.setBackground(backColor);
   selection.format.setProperty(QTextFormat::FullWidthSelection, true);
   selection.format.setProperty(QTextFormat::UserProperty, QString("highlightbar"));

   selection.cursor = m_textEdit->textCursor();
   selection.cursor.clearSelection();

   extraSelections.append(selection);
   m_textEdit->setExtraSelections(extraSelections);
}

void MainWindow::lineNumbers()
{
   if (m_ui->actionLine_Numbers->isChecked()) {
      //on
      m_struct.showLineNumbers = true;

   } else {
      // off
      m_struct.showLineNumbers = false;
   }

   json_Write(SHOW_LINENUMBERS);
   m_noSplit_textEdit->set_ShowLineNum(m_struct.showLineNumbers);

   if (m_isSplit) {
      m_split_textEdit->set_ShowLineNum(m_struct.showLineNumbers);
   }
}

void MainWindow::wordWrap()
{
   if (m_ui->actionWord_Wrap->isChecked()) {
      //on
      m_struct.isWordWrap = true;
      m_textEdit->setWordWrapMode(QTextOption::WordWrap);

   } else {
      // off
      m_struct.isWordWrap = false;
      m_textEdit->setWordWrapMode(QTextOption::NoWrap);

   }

   json_Write(WORDWRAP);
}

void MainWindow::show_Spaces()
{
   QTextDocument *td   = m_textEdit->document();
   QTextOption textOpt = td->defaultTextOption();

   bool oldValue = m_struct.show_Spaces;

   if (m_ui->actionShow_Spaces->isChecked()) {
      //on
      m_struct.show_Spaces = true;

      if (m_struct.show_Breaks) {
        textOpt.setFlags(QTextOption::ShowTabsAndSpaces | QTextOption::ShowLineAndParagraphSeparators);

      } else {
        textOpt.setFlags(QTextOption::ShowTabsAndSpaces);

      }

      td->setDefaultTextOption(textOpt);

   } else {
      // off
       m_struct.show_Spaces = false;

       if (m_struct.show_Breaks) {
         textOpt.setFlags(QTextOption::ShowLineAndParagraphSeparators);

       } else {
         textOpt.setFlags(0);

       }

       td->setDefaultTextOption(textOpt);
   }

   if (oldValue != m_struct.show_Spaces)  {
      json_Write(SHOW_SPACES);
   }
}

void MainWindow::show_Breaks()
{
   QTextDocument *td = m_textEdit->document();
   QTextOption textOpt = td->defaultTextOption();

   bool oldValue = m_struct.show_Breaks;

   if (m_ui->actionShow_Breaks->isChecked()) {
      //on
      m_struct.show_Breaks = true;

      if (m_struct.show_Spaces) {
         textOpt.setFlags(QTextOption::ShowTabsAndSpaces | QTextOption::ShowLineAndParagraphSeparators);

      } else {
         textOpt.setFlags(QTextOption::ShowLineAndParagraphSeparators);

      }

      td->setDefaultTextOption(textOpt);

   } else {
      // off
      m_struct.show_Breaks = false;

      if (m_struct.show_Spaces) {
        textOpt.setFlags(QTextOption::ShowTabsAndSpaces);

      } else {
        textOpt.setFlags(0);

      }

      td->setDefaultTextOption(textOpt);
   }

   if (oldValue != m_struct.show_Breaks)  {
      json_Write(SHOW_BREAKS);
   }
}

void MainWindow::displayHTML()
{
   try {
      showHtml("display", m_curFile);

   } catch (std::exception &e) {
      // do nothing for now

   }
}

// **document
void MainWindow::setSynType(SyntaxTypes data)
{
   m_ui->actionSyn_C->setChecked(false);
   m_ui->actionSyn_Clipper->setChecked(false);
   m_ui->actionSyn_CMake->setChecked(false);
   m_ui->actionSyn_Css->setChecked(false);
   m_ui->actionSyn_Doxy->setChecked(false);
   m_ui->actionSyn_ErrorLog->setChecked(false);
   m_ui->actionSyn_Html->setChecked(false);
   m_ui->actionSyn_Java->setChecked(false);
   m_ui->actionSyn_Javascript->setChecked(false);
   m_ui->actionSyn_Json->setChecked(false);
   m_ui->actionSyn_Makefile->setChecked(false);
   m_ui->actionSyn_Nsis->setChecked(false);
   m_ui->actionSyn_Text->setChecked(false);
   m_ui->actionSyn_Shell->setChecked(false);
   m_ui->actionSyn_Perl->setChecked(false);
   m_ui->actionSyn_PHP->setChecked(false);
   m_ui->actionSyn_Python->setChecked(false);
   m_ui->actionSyn_Xml->setChecked(false);
   m_ui->actionSyn_None->setChecked(false);

   // m_ui->actionSyn_UnUsed1->setChecked(false);
   // m_ui->actionSyn_UnUsed2->setChecked(false);

   switch (data)  {
      case SYN_C:
         m_ui->actionSyn_C->setChecked(true);
         break;

      case SYN_CLIPPER:
         m_ui->actionSyn_Clipper->setChecked(true);
         break;

      case SYN_CMAKE:
         m_ui->actionSyn_CMake->setChecked(true);
         break;

      case SYN_CSS:
         m_ui->actionSyn_Css->setChecked(true);
         break;

      case SYN_DOXY:
         m_ui->actionSyn_Doxy->setChecked(true);
         break;

      case SYN_ERRLOG:
         m_ui->actionSyn_ErrorLog->setChecked(true);
         break;

      case SYN_HTML:
         m_ui->actionSyn_Html->setChecked(true);
         break;

      case SYN_JAVA:
         m_ui->actionSyn_Java->setChecked(true);
         break;

      case SYN_JS:
         m_ui->actionSyn_Javascript->setChecked(true);
         break;

      case SYN_JSON:
         m_ui->actionSyn_Json->setChecked(true);
         break;

      case SYN_MAKE:
         m_ui->actionSyn_Makefile->setChecked(true);
         break;

      case SYN_NSIS:
         m_ui->actionSyn_Nsis->setChecked(true);
         break;

      case SYN_TEXT:
         m_ui->actionSyn_Text->setChecked(true);
         break;

      case SYN_SHELL:
         m_ui->actionSyn_Shell->setChecked(true);
         break;

      case SYN_PERL:
         m_ui->actionSyn_Perl->setChecked(true);
         break;

      case SYN_PHP:
         m_ui->actionSyn_PHP->setChecked(true);
         break;

      case SYN_PYTHON:
         m_ui->actionSyn_Python->setChecked(true);
         break;

      case SYN_XML:
         m_ui->actionSyn_Xml->setChecked(true);
         break;

      case SYN_NONE:
         m_ui->actionSyn_None->setChecked(true);
         break;

/*
      case SYN_UNUSED1:
         m_ui->actionSyn_UnUsed1->setChecked(true);
         break;

      case SYN_UNUSED2:
         m_ui->actionSyn_UnUsed2->setChecked(true);
         break;
*/
   }
}

void MainWindow::formatUnix()
{
   showNotDone("Document, format Unix (LF)");
}

void MainWindow::formatWin()
{
   showNotDone("Document, format Windows (CR LF)");
}

void MainWindow::fixTab_Spaces()
{
   QTextCursor cursor(m_textEdit->textCursor());

   // save starting position
   QTextCursor cursorStart;
   cursorStart = cursor;

   // position to start of document
   cursor.movePosition(QTextCursor::Start);
   m_textEdit->setTextCursor(cursor);

   // set for undo stack
   cursor.beginEditBlock();

   int tabLen = m_struct.tabSpacing;
   const QString findText = QString(QChar(9));

   while (true) {
      bool found = m_textEdit->find(findText);

      if (found)  {
         cursor = m_textEdit->textCursor();

         // how many chars from the start of line
         int span = cursor.positionInBlock();

         int x1 = (span/tabLen) * tabLen;
         int x2 = tabLen - (span - x1) + 1;

         const QString newText = QString(x2, ' ');
         cursor.insertText(newText);

      } else {
         break;
      }
   }

   cursor.endEditBlock();

   // go back to starting point
   m_textEdit->setTextCursor(cursorStart);
}

void MainWindow::fixSpaces_Tab()
{
   QTextCursor cursor(m_textEdit->textCursor());

   // save starting position
   QTextCursor cursorStart;
   cursorStart = cursor;

   // position to start of document
   cursor.movePosition(QTextCursor::Start);
   m_textEdit->setTextCursor(cursor);

   // set for undo stack
   cursor.beginEditBlock();

   int pos;
   int tabLen = m_struct.tabSpacing;

   QString tmp;

   const QString findText = QString(2, ' ');
   const QString newText  = QString(QChar(9));

   while (true) {
      bool found = m_textEdit->find(findText);

      // reset
      int pass = 1;

      if (found)  {
         cursor = m_textEdit->textCursor();

         while (true) {
            tmp = cursor.selectedText().trimmed();
            pos  = cursor.positionInBlock();

            if ( m_tabStops.contains(pos)  ) {
               // this a tap stop

               if (tmp.isEmpty()) {
                  // replace spaces with tab
                  cursor.insertText(newText);

               } else {
                  // char at the tabstop is not a space, back up one
                  cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1);

                  // replace spaces with tab
                  cursor.insertText(newText);
               }

               break;
            }

            if (! tmp.isEmpty() || pass == tabLen)  {
               break;
            }

            // get one more char
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);
            ++pass;
         }

      } else {
         // all done
         break;

      }
   }

   cursor.endEditBlock();

   // go back to starting point
   m_textEdit->setTextCursor(cursorStart);
}

void MainWindow::deleteEOL_Spaces()
{
   QTextCursor cursor(m_textEdit->textCursor());

   // position to start of document
   cursor.movePosition(QTextCursor::Start);

   // set for undo stack
   cursor.beginEditBlock();

   QString tmp;

   while (true) {
      cursor.movePosition(QTextCursor::EndOfBlock);

      while (true) {
         cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1);
         tmp = cursor.selectedText();

         if (tmp != " ") {
            cursor.movePosition(QTextCursor::NextCharacter);
            break;
         }

         cursor.removeSelectedText();
      }

      if (! cursor.movePosition(QTextCursor::NextBlock))  {
          break;
      }
   }

   cursor.endEditBlock();
}


// ** tools
void MainWindow::mw_macroStart()
{
   if (! m_record) {
      m_record = true;
      m_textEdit->macroStart();

      setStatusBar(tr("Recording macro. . ."), 0);
   }
}

void MainWindow::mw_macroStop()
{
   if (m_record) {
      m_record = false;
      m_textEdit->macroStop();

      // save macro
      json_Write(MACRO);

      // save macro to global list
      m_macroList = m_textEdit->get_MacroKeyList();

      setStatusBar(tr("Macro recorded"), 1200);

   } else {
      setStatusBar(tr("No recording in progress"), 1200);

   }
}

void MainWindow::macroPlay()
{
   if (m_record) {
      csError("Macro Playback", "Unable to play back a macro while recording");

   }  else {
      int cnt = m_macroList.count();

      if (cnt == 0) {
         csError("Macro Playback", "No macro to play back");

      } else {
         QKeyEvent *event;

         for (int k = 0; k < cnt; ++k)   {
            event = m_macroList.at(k);

            QKeyEvent *newEvent;
            newEvent = new QKeyEvent(*event);

            QApplication::postEvent(m_textEdit, newEvent);
         }
      }
   }
}

void MainWindow::macroLoad()
{
   QStringList macroIds = json_Load_MacroIds();

   if (macroIds.count() == 0)  {
      csError("Load Macros", "No exiting macros");
      return;
   }

   Dialog_Macro *dw = new Dialog_Macro(this, Dialog_Macro::MACRO_LOAD, macroIds, m_macroNames);
   int result = dw->exec();

   if (result == QDialog::Accepted) {
      QString text = dw->get_Macro();
      json_Load_Macro(text);
   }

   delete dw;
}

void MainWindow::macroEditNames()
{
   QStringList macroIds = json_Load_MacroIds();

   if (macroIds.count() == 0)  {
      csError("Load Macros", "No exiting macros");
      return;
   }

   Dialog_Macro *dw = new Dialog_Macro(this, Dialog_Macro::MACRO_EDITNAMES, macroIds, m_macroNames);
   dw->exec();

   delete dw;
}

// **
void MainWindow::spellCheck()
{
   if (m_ui->actionSpell_Check->isChecked()) {
      //on
      m_struct.isSpellCheck = true;

   } else {
      // off
      m_struct.isSpellCheck = false;

   }

   json_Write(SPELLCHECK);

   // run for every tab
   int count = m_tabWidget->count();

   QWidget *tmp;
   DiamondTextEdit *textEdit;

   for (int k = 0; k < count; ++k)  {

      tmp      = m_tabWidget->widget(k);
      textEdit = dynamic_cast<DiamondTextEdit *>(tmp);

      if (textEdit) {
         // save new values & reHighlight
         textEdit->set_Spell(m_struct.isSpellCheck);
      }
   }
}
