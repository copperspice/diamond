/**************************************************************************
*
* Copyright (c) 2012-2016 Barbara Geller
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

#include "about.h"
#include "dialog_macro.h"
#include "dialog_open.h"
#include "dialog_symbols.h"
#include "mainwindow.h"

#include <stdexcept>

#include <QChar>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontMetrics>
#include <QKeySequence>
#include <QLabel>
#include <QToolBar>

MainWindow::MainWindow(QStringList fileList, QStringList flagList)
   : m_ui(new Ui::MainWindow)
{
   m_ui->setupUi(this);
   setDiamondTitle("untitled.txt");

   setIconSize(QSize(32,32));
   setWindowIcon(QIcon("://resources/diamond.png"));

   if (! json_Read(CFG_STARTUP) ) {
      // do not start program
      csError(tr("Configuration File Missing"), tr("Unable to locate or open the Diamond Configuration file."));
      throw std::runtime_error("abort_no_message");
   }

   // drag & drop
   setAcceptDrops(true);

   // tab stops
   this->setUpTabStops();

   // remaining methods must be done after json_Read for config
   m_tabWidget = new QTabWidget;
   m_tabWidget->setTabsClosable(true);
   m_tabWidget->setMovable(true);
   m_tabWidget->setWhatsThis("tab_widget");

   // set up the splitter, only display the tabWidget
   m_splitter = new QSplitter(Qt::Vertical);
   m_splitter->addWidget(m_tabWidget);
   setCentralWidget(m_splitter);

   connect(qApp, &QApplication::focusChanged, this, &MainWindow::focusChanged);

   m_split_textEdit = 0;
   m_isSplit = false;

   // macros
   m_record = false;

   // copy buffer   
   m_actionCopyBuffer = new QShortcut(this);
   connect(m_actionCopyBuffer, &QShortcut::activated, this, &MainWindow::showCopyBuffer);

   // screen setup
   createShortCuts(true);
   createToolBars();
   createStatusBar();
   createToggles();
   createConnections();

   // recent folders
   rfolder_CreateMenus();

   // reset  folders
   prefolder_CreateMenus();

   // recent files
   rf_CreateMenus(); 

   // spell check
   createSpellCheck();

   // recent folders, context menu
   QMenu *menuFolder_R = m_ui->actionOpen_RecentFolder->menu();
   menuFolder_R->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(menuFolder_R, &QMenu::customContextMenuRequested,     this, &MainWindow::showContext_RecentFolder);

   // recent files, context menu
   m_ui->menuFile->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(m_ui->menuFile, &QMenu::customContextMenuRequested,   this, &MainWindow::showContext_Files);

   // window tab, context menu
   m_ui->menuWindow->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(m_ui->menuWindow, &QMenu::customContextMenuRequested, this, &MainWindow::showContext_Tabs);

   // set flags after reading config and before autoload
   if (flagList.contains("--no_autoload", Qt::CaseInsensitive)) {
      m_args.flag_noAutoLoad = true;
   }

   if (flagList.contains("--no_saveconfig", Qt::CaseInsensitive)) {
      m_args.flag_noSaveConfig = true;
   }

   if (m_struct.autoLoad && ! m_args.flag_noAutoLoad ) {      
      autoLoad();
   }   

   // user requested files on the command line
   if (fileList.count() > 1 ) {
      argLoad(fileList);
   }

   // no files were open, open a blank tab
   if (m_tabWidget->count() == 0) {
      tabNew();
   }

   // currently open tabs, must occur after autoLoad & argLoad
   openTab_CreateMenus();

   // find
   if (! m_findList.isEmpty()) {
    m_findText = m_findList.first();
   }

   // replace
   if (! m_replaceList.isEmpty()) {
    m_replaceText = m_replaceList.first();
   }

   setStatus_ColMode();
   setStatusBar(tr("Ready"), 0);   
   setUnifiedTitleAndToolBarOnMac(true);
}

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
   QFileInfo temp(m_curFile);      
   QString ext = temp.suffix();

   if (ext == "cpp" || ext == "h") {
      QStringList list;

      QString tFile;
      QString baseName = temp.canonicalPath() + "/" +  temp.completeBaseName();

      if (ext == "cpp") {
         tFile = baseName + ".h";

         if ( QFile::exists(tFile) ) {
            list.append(tFile);
         }

         tFile = baseName + "_p.h";

         if ( QFile::exists(tFile) ) {
            list.append(tFile);
         }

      } else if (baseName.endsWith("_p")  &&  ext == "h")  {

         baseName.chop(2);

         tFile = baseName + ".cpp";

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
            QString tempF = dw->get_FileName();
            this->loadFile(tempF, true, false);
         }

         delete dw;
      }

   } else {
      csError(tr("Open Related Files"), tr("Related files only configured for .cpp and .h files"));

   }
}

void MainWindow::openDoc(QString path)
{       
   QString selectedFilter;
   QFileDialog::Options options;

   // force windows 7 and 8 to honor initial path
   options = QFileDialog::ForceInitialDir_Win7;

   QStringList fileList = QFileDialog::getOpenFileNames(this, tr("Select File"),
         path, tr("All Files (*)"), &selectedFilter, options);

   int cnt = fileList.count();

   for (int k = 0; k < cnt; ++k )  {
      QString fileName = fileList.at(k);

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

   QWidget *temp;
   DiamondTextEdit *textEdit;

   int count = m_tabWidget->count();
   int whichTab = 0;

   // clear open tab list
   m_openedFiles.clear();
   m_openedModified.clear();

   for (int k = 0; k < count; ++k) {

      temp = m_tabWidget->widget(whichTab);
      textEdit = dynamic_cast<DiamondTextEdit *>(temp);

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
        loadFile(m_curFile, false, false);
      }

   } else {
      loadFile(m_curFile, false, false);

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
   QFileDialog::Options options;

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

   QWidget *temp;
   DiamondTextEdit *textEdit;

   int count = m_tabWidget->count();

   for (int k = 0; k < count; ++k) {

      temp = m_tabWidget->widget(k);
      textEdit = dynamic_cast<DiamondTextEdit *>(temp);

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

   text    = text.toLower();
   text[0] = text[0].toUpper();

   cursor.removeSelectedText();
   cursor.insertText(text);
}

void MainWindow::insertDate()
{   
   QDate date   = QDate::currentDate();
   QString temp = date.toString(m_struct.formatDate);

   m_textEdit->insertPlainText(temp);
}

void MainWindow::insertTime()
{
   QTime time   = QTime::currentTime();
   QString temp = time.toString(m_struct.formatTime);

   m_textEdit->insertPlainText(temp);
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

      QString temp;

      while (true) {
         cursor.movePosition(QTextCursor::StartOfLine);

         for (int k = 0; k < m_struct.tabSpacing; ++k) {

            int t_block = cursor.block().blockNumber();
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);

            if (t_block != cursor.block().blockNumber() )  {
               cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1);
               break;
            }

            temp = cursor.selectedText().trimmed();
            if (! temp.isEmpty())  {
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

      QString temp;

      for (int k=0; k < m_struct.tabSpacing; ++k) {

         cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);
         temp = cursor.selectedText().trimmed();

         if (! temp.isEmpty())  {
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

   QTextCursor cursor(m_textEdit->textCursor());
   cursor.movePosition(QTextCursor::Start);

   cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, line - 1);
   m_textEdit->setTextCursor(cursor);
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
      About *dw = new About("Display", m_curFile);
      dw->show();

   } catch (std::exception &e) {
      // do nothing for now

   }
}

// **document
void MainWindow::setSynType(SyntaxTypes data)
{
   m_ui->actionSyn_C->setChecked(false);
   m_ui->actionSyn_Clipper->setChecked(false);
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

   QString temp;

   const QString findText = QString(2, ' ');
   const QString newText  = QString(QChar(9));

   while (true) {      
      bool found = m_textEdit->find(findText);

      // reset
      int pass = 1;

      if (found)  {         
         cursor = m_textEdit->textCursor();                 

         while (true) {
            temp = cursor.selectedText().trimmed();
            pos  = cursor.positionInBlock();

            if ( m_tabStops.contains(pos)  ) {
               // this a tap stop

               if (temp.isEmpty()) {
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

            if (! temp.isEmpty() || pass == tabLen)  {
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

   QString temp;

   while (true) {
      cursor.movePosition(QTextCursor::EndOfBlock);

      while (true) {
         cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1);
         temp = cursor.selectedText();

         if (temp != " ") {
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

   QWidget *temp;
   DiamondTextEdit *textEdit;

   for (int k = 0; k < count; ++k)  {

      temp     = m_tabWidget->widget(k);
      textEdit = dynamic_cast<DiamondTextEdit *>(temp);

      if (textEdit) {
         // save new values & reHighlight
         textEdit->set_Spell(m_struct.isSpellCheck);
      }
   }     
}


// **window, tabs
void MainWindow::tabNew()
{   
   m_textEdit = new DiamondTextEdit(this, m_struct, m_spellCheck, "tab");

   // keep reference
   m_noSplit_textEdit = m_textEdit;

   // triggers a call to tabChanged
   int index = m_tabWidget->addTab(m_textEdit, "untitled.txt");
   m_tabWidget->setCurrentIndex(index);

   setScreenColors();
   int temp = m_textEdit->fontMetrics().width(" ");
   m_textEdit->setTabStopWidth(temp * m_struct.tabSpacing);

   m_textEdit->setFocus();

   // connect(m_textEdit, SIGNAL(fileDropped(const QString &)), this, SLOT(fileDropped(const QString &)));
   connect(m_textEdit->document(), &QTextDocument::contentsChanged, this, &MainWindow::documentWasModified);

   connect(m_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::moveBar);
   connect(m_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::setStatus_LineCol);

   connect(m_textEdit, &DiamondTextEdit::undoAvailable, m_ui->actionUndo, &QAction::setEnabled);
   connect(m_textEdit, &DiamondTextEdit::redoAvailable, m_ui->actionRedo, &QAction::setEnabled);
   connect(m_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCut,  &QAction::setEnabled);
   connect(m_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCopy, &QAction::setEnabled);
}

void MainWindow::mw_tabClose()
{     
   int index = m_tabWidget->currentIndex();
   tabClose(index);
}

void MainWindow::tabClose(int index)
{
   m_tabWidget->setCurrentIndex(index);
   QWidget *temp = m_tabWidget->widget(index);   

   DiamondTextEdit *t_textEdit;
   t_textEdit = dynamic_cast<DiamondTextEdit *>(temp);

   if (t_textEdit) {
      m_textEdit = t_textEdit;
      m_curFile  = this->get_curFileName(index);

      if (close_Doc()) {

         if ( m_tabWidget->count() > 1 ) {

            // hold textEdit and delete after tab is removed
            DiamondTextEdit *hold = m_textEdit;

            m_tabWidget->removeTab(index);
            delete hold;
         }
      }
   }
}

void MainWindow::tabChanged(int index)
{
   QWidget *temp = m_tabWidget->widget(index);

   DiamondTextEdit *textEdit;
   textEdit = dynamic_cast<DiamondTextEdit *>(temp);

   if (textEdit) {
      m_textEdit = textEdit;

      // keep reference
      m_noSplit_textEdit = m_textEdit;           

      m_curFile = this->get_curFileName(index);
      this->setCurrentTitle(m_curFile, true);

      // ** retrieve slected syntax type
      m_syntaxParser = m_textEdit->get_SyntaxParser();

      // retrieve the menu enum
      m_syntaxEnum = m_textEdit->get_SyntaxEnum();

      // check the menu item
      setSynType(m_syntaxEnum);

      // **
      setStatus_LineCol();
      m_textEdit->set_ColumnMode(m_struct.isColumnMode);
      m_textEdit->set_ShowLineNum(m_struct.showLineNumbers);

      moveBar();
      show_Spaces();
      show_Breaks();
   }
}

void MainWindow::focusChanged(QWidget *prior, QWidget *current)
{   
   if (! current) {
      return;
   }  

   DiamondTextEdit *t_textEdit;
   t_textEdit = dynamic_cast<DiamondTextEdit *>(current);

   if (t_textEdit)  {

      if (m_textEdit->m_owner == t_textEdit->m_owner)  {
         // do nothing

      } else {
         m_textEdit = t_textEdit;

         if (m_textEdit->m_owner == "tab") {
            // focus changed to the tabWidet

            // keep reference
            m_noSplit_textEdit = m_textEdit;

            int index = m_tabWidget->currentIndex();
            m_curFile = get_curFileName(index);

            setStatus_FName(m_curFile);

            // ** retrieve slected syntax type
//          m_syntaxParser = m_textEdit->get_SyntaxParser();

            // retrieve the menu enum
//          m_syntaxEnum = m_textEdit->get_SyntaxEnum();

            // check the menu item
//          setSynType(m_syntaxEnum);

            // **
            setStatus_LineCol();
            m_textEdit->set_ColumnMode(m_struct.isColumnMode);
            m_textEdit->set_ShowLineNum(m_struct.showLineNumbers);

            moveBar();
            show_Spaces();
            show_Breaks();

         } else if (m_textEdit->m_owner == "split") {
            // focus changed to the splitWidget

            m_curFile = m_splitFileName;
            setStatus_FName(m_curFile);

            // ** retrieve slected syntax type
//          m_syntaxParser = m_textEdit->get_SyntaxParser();

            // retrieve the menu enum
//          m_syntaxEnum = m_textEdit->get_SyntaxEnum();

            // check the menu item
//          setSynType(m_syntaxEnum);

            // **
            setStatus_LineCol();
            m_textEdit->set_ColumnMode(m_struct.isColumnMode);
            m_textEdit->set_ShowLineNum(m_struct.showLineNumbers);

            moveBar();
            show_Spaces();
            show_Breaks();
         }
      }
   }
}


// **help
void MainWindow::diamondHelp()
{   
   try {
      About *dw = new About("Help" ,m_struct.aboutUrl);
      dw->show();

   } catch (std::exception &e) {
      // do nothing for now
   }
}

void MainWindow::about()
{
   // change mainwindow.cpp & main.cpp

   QString textBody = "<font color='#000080'><table style=margin-right:25>"
                  "<tr><td><nobr>Developed by Barbara Geller</nobr></td>"
                      "<td>barbara@copperspice.com</td></tr>"
                  "<tr><td style=padding-right:25><nobr>Developed by Ansel Sermersheim</nobr></td>"
                      "<td>ansel@copperspice.com</td></tr>"
                  "</table></font>"
                  "<br>"
                  "<p><small>Copyright 2012-2016 BG Consulting, All rights reserved.<br>"
                  "This program is provided AS IS with no warranty of any kind.<br></small></p>";

   //
   QMessageBox msgB;
   msgB.setIcon(QMessageBox::NoIcon);
   msgB.setWindowIcon(QIcon("://resources/diamond.png"));

   msgB.setWindowTitle(tr("About Diamond"));
   msgB.setText("<p style=margin-right:25><center><h5>Version: 1.2.1<br>Build # 03.15.2016</h5></center></p>");
   msgB.setInformativeText(textBody);

   msgB.setStandardButtons(QMessageBox::Ok);
   msgB.setDefaultButton(QMessageBox::Ok);

   msgB.exec();
}

// connections, displays, toolbar
void MainWindow::setScreenColors()
{     
   changeFont();

   QPalette temp = m_textEdit->palette();
   temp.setColor(QPalette::Text, m_struct.colorText);
   temp.setColor(QPalette::Base, m_struct.colorBack);
   m_textEdit->setPalette(temp);
}

void MainWindow::createConnections()
{
   // file
   connect(m_ui->actionNew,               &QAction::triggered, this, &MainWindow::newFile);
   connect(m_ui->actionOpen,              &QAction::triggered, this, [this](bool){ openDoc(m_struct.pathPrior); } );
   connect(m_ui->actionOpen_RelatedFile,  &QAction::triggered, this, &MainWindow::open_RelatedFile);
   connect(m_ui->actionClose,             &QAction::triggered, this, [this](bool){ close_Doc(); } );
   connect(m_ui->actionClose_All,         &QAction::triggered, this, [this](bool){ closeAll_Doc(false); } );
   connect(m_ui->actionReload,            &QAction::triggered, this, &MainWindow::reload);

   connect(m_ui->actionSave,              &QAction::triggered, this, [this](bool){ save(); } );
   connect(m_ui->actionSave_As,           &QAction::triggered, this, [this](bool){ saveAs(SAVE_ONE); } );
   connect(m_ui->actionSave_All,          &QAction::triggered, this, &MainWindow::saveAll);

   connect(m_ui->actionPrint,             &QAction::triggered, this, &MainWindow::print);
   connect(m_ui->actionPrint_Preview,     &QAction::triggered, this, &MainWindow::printPreview);
   connect(m_ui->actionPrint_Pdf,         &QAction::triggered, this, &MainWindow::printPdf);
   connect(m_ui->actionExit,              &QAction::triggered, this, [this](bool){ close(); } );

   // edit
   connect(m_ui->actionUndo,              &QAction::triggered, this, &MainWindow::mw_undo);
   connect(m_ui->actionRedo,              &QAction::triggered, this, &MainWindow::mw_redo);
   connect(m_ui->actionCut,               &QAction::triggered, this, &MainWindow::mw_cut);
   connect(m_ui->actionCopy,              &QAction::triggered, this, &MainWindow::mw_copy);
   connect(m_ui->actionPaste,             &QAction::triggered, this, &MainWindow::mw_paste);

   connect(m_ui->actionSelect_All,        &QAction::triggered, this, &MainWindow::selectAll);
   connect(m_ui->actionSelect_Block,      &QAction::triggered, this, &MainWindow::selectBlock);
   connect(m_ui->actionSelect_Line,       &QAction::triggered, this, &MainWindow::selectLine);
   connect(m_ui->actionSelect_Word,       &QAction::triggered, this, &MainWindow::selectWord);
   connect(m_ui->actionCase_Upper,        &QAction::triggered, this, &MainWindow::caseUpper);
   connect(m_ui->actionCase_Lower,        &QAction::triggered, this, &MainWindow::caseLower);
   connect(m_ui->actionCase_Cap,          &QAction::triggered, this, &MainWindow::caseCap);

   connect(m_ui->actionIndent_Incr,       &QAction::triggered, this, [this](bool){ indentIncr("indent");   } );
   connect(m_ui->actionIndent_Decr,       &QAction::triggered, this, [this](bool){ indentDecr("unindent"); } );
   connect(m_ui->actionDelete_Line,       &QAction::triggered, this, &MainWindow::deleteLine);
   connect(m_ui->actionDelete_EOL,        &QAction::triggered, this, &MainWindow::deleteEOL);

   connect(m_ui->actionInsert_Date,       &QAction::triggered, this, &MainWindow::insertDate);
   connect(m_ui->actionInsert_Time,       &QAction::triggered, this, &MainWindow::insertTime);
   connect(m_ui->actionInsert_Symbol,     &QAction::triggered, this, &MainWindow::insertSymbol);
   connect(m_ui->actionColumn_Mode,       &QAction::triggered, this, &MainWindow::columnMode);

   // search
   connect(m_ui->actionFind,              &QAction::triggered, this, &MainWindow::find);
   connect(m_ui->actionReplace,           &QAction::triggered, this, &MainWindow::replace);
   connect(m_ui->actionFind_Next,         &QAction::triggered, this, &MainWindow::findNext);
   connect(m_ui->actionFind_Prev,         &QAction::triggered, this, &MainWindow::findPrevious);
   connect(m_ui->actionAdv_Find,          &QAction::triggered, this, &MainWindow::advFind);
   connect(m_ui->actionGo_Line,           &QAction::triggered, this, &MainWindow::goLine);
   connect(m_ui->actionGo_Column,         &QAction::triggered, this, &MainWindow::goColumn);
   connect(m_ui->actionGo_Top,            &QAction::triggered, this, &MainWindow::goTop);
   connect(m_ui->actionGo_Bottom,         &QAction::triggered, this, &MainWindow::goBottom);

   // view
   connect(m_ui->actionLine_Highlight,    &QAction::triggered, this, &MainWindow::lineHighlight);
   connect(m_ui->actionLine_Numbers,      &QAction::triggered, this, &MainWindow::lineNumbers);
   connect(m_ui->actionWord_Wrap,         &QAction::triggered, this, &MainWindow::wordWrap);
   connect(m_ui->actionShow_Spaces,       &QAction::triggered, this, &MainWindow::show_Spaces);
   connect(m_ui->actionShow_Breaks,       &QAction::triggered, this, &MainWindow::show_Breaks);
   connect(m_ui->actionDisplay_HTML,      &QAction::triggered, this, &MainWindow::displayHTML);

   // document
   connect(m_ui->actionSyn_C,             &QAction::triggered, this, [this](bool){ forceSyntax(SYN_C);       } );
   connect(m_ui->actionSyn_Clipper,       &QAction::triggered, this, [this](bool){ forceSyntax(SYN_CLIPPER); } );
   connect(m_ui->actionSyn_Css,           &QAction::triggered, this, [this](bool){ forceSyntax(SYN_CSS);     } );
   connect(m_ui->actionSyn_Doxy,          &QAction::triggered, this, [this](bool){ forceSyntax(SYN_DOXY);    } );
   connect(m_ui->actionSyn_ErrorLog,      &QAction::triggered, this, [this](bool){ forceSyntax(SYN_ERRLOG);  } );
   connect(m_ui->actionSyn_Html,          &QAction::triggered, this, [this](bool){ forceSyntax(SYN_HTML);    } );
   connect(m_ui->actionSyn_Java,          &QAction::triggered, this, [this](bool){ forceSyntax(SYN_JAVA);    } );
   connect(m_ui->actionSyn_Javascript,    &QAction::triggered, this, [this](bool){ forceSyntax(SYN_JS);      } );
   connect(m_ui->actionSyn_Json,          &QAction::triggered, this, [this](bool){ forceSyntax(SYN_JSON);    } );
   connect(m_ui->actionSyn_Makefile,      &QAction::triggered, this, [this](bool){ forceSyntax(SYN_MAKE);    } );
   connect(m_ui->actionSyn_Nsis,          &QAction::triggered, this, [this](bool){ forceSyntax(SYN_NSIS);    } );
   connect(m_ui->actionSyn_Text,          &QAction::triggered, this, [this](bool){ forceSyntax(SYN_TEXT);    } );
   connect(m_ui->actionSyn_Shell,         &QAction::triggered, this, [this](bool){ forceSyntax(SYN_SHELL);   } );
   connect(m_ui->actionSyn_Perl,          &QAction::triggered, this, [this](bool){ forceSyntax(SYN_PERL);    } );
   connect(m_ui->actionSyn_PHP,           &QAction::triggered, this, [this](bool){ forceSyntax(SYN_PHP);     } );
   connect(m_ui->actionSyn_Python,        &QAction::triggered, this, [this](bool){ forceSyntax(SYN_PYTHON);  } );
   connect(m_ui->actionSyn_None,          &QAction::triggered, this, [this](bool){ forceSyntax(SYN_NONE);    } );

   // connect(m_ui->actionSyn_UNUSED1,    &QAction::triggered, this, [this](bool){ forceSyntax(SYN_UNUSED1); } );
   // connect(m_ui->actionSyn_UNUSED2,    &QAction::triggered, this, [this](bool){ forceSyntax(SYN_UNUSED2); } );

   connect(m_ui->actionFormat_Unix,       &QAction::triggered, this, &MainWindow::formatUnix);
   connect(m_ui->actionFormat_Win,        &QAction::triggered, this, &MainWindow::formatWin);

   connect(m_ui->actionFix_Tab_Spaces,    &QAction::triggered, this, &MainWindow::fixTab_Spaces);
   connect(m_ui->actionFix_Spaces_Tab,    &QAction::triggered, this, &MainWindow::fixSpaces_Tab);
   connect(m_ui->actionDeleteEOL_Spaces,  &QAction::triggered, this, &MainWindow::deleteEOL_Spaces);

   // tools
   connect(m_ui->actionMacro_Start,       &QAction::triggered, this, &MainWindow::mw_macroStart);
   connect(m_ui->actionMacro_Stop,        &QAction::triggered, this, &MainWindow::mw_macroStop);
   connect(m_ui->actionMacro_Play,        &QAction::triggered, this, &MainWindow::macroPlay);
   connect(m_ui->actionMacro_Load,        &QAction::triggered, this, &MainWindow::macroLoad);
   connect(m_ui->actionMacro_EditNames,   &QAction::triggered, this, &MainWindow::macroEditNames);
   connect(m_ui->actionSpell_Check,       &QAction::triggered, this, &MainWindow::spellCheck);

   // settings
   connect(m_ui->actionColors,            &QAction::triggered, this, &MainWindow::setColors);
   connect(m_ui->actionFonts,             &QAction::triggered, this, &MainWindow::setFont);
   connect(m_ui->actionOptions,           &QAction::triggered, this, &MainWindow::setOptions);
   connect(m_ui->actionPresetFolders,     &QAction::triggered, this, &MainWindow::setPresetFolders);
   connect(m_ui->actionPrintOptions,      &QAction::triggered, this, &MainWindow::setPrintOptions);
   connect(m_ui->actionMove_ConfigFile,   &QAction::triggered, this, &MainWindow::move_ConfigFile);
   connect(m_ui->actionSave_ConfigFile,   &QAction::triggered, this, [this](bool){ save_ConfigFile(); } );

   // window
   connect(m_ui->actionTab_New,           &QAction::triggered, this, &MainWindow::tabNew);
   connect(m_ui->actionTab_Close,         &QAction::triggered, this, &MainWindow::mw_tabClose);
   connect(m_ui->actionSplit_Horizontal,  &QAction::triggered, this, &MainWindow::split_Horizontal);
   connect(m_ui->actionSplit_Vertical,    &QAction::triggered, this, &MainWindow::split_Vertical);

   // help menu
   connect(m_ui->actionDiamond_Help,      &QAction::triggered, this, &MainWindow::diamondHelp);
   connect(m_ui->actionAbout,             &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createToggles()
{    
   m_ui->actionSyn_C->setCheckable(true);
   m_ui->actionSyn_Clipper->setCheckable(true);
   m_ui->actionSyn_Css->setCheckable(true);
   m_ui->actionSyn_Doxy->setCheckable(true);
   m_ui->actionSyn_ErrorLog->setCheckable(true);
   m_ui->actionSyn_Html->setCheckable(true);
   m_ui->actionSyn_Java->setCheckable(true);
   m_ui->actionSyn_Javascript->setCheckable(true);
   m_ui->actionSyn_Json->setCheckable(true);
   m_ui->actionSyn_Makefile->setCheckable(true);
   m_ui->actionSyn_Nsis->setCheckable(true);
   m_ui->actionSyn_Text->setCheckable(true);
   m_ui->actionSyn_Shell->setCheckable(true);
   m_ui->actionSyn_Perl->setCheckable(true);
   m_ui->actionSyn_PHP->setCheckable(true);
   m_ui->actionSyn_Python->setCheckable(true);
   m_ui->actionSyn_None->setCheckable(true);

   // m_ui->actionSyn_Usused1->setCheckable(true);
   // m_ui->actionSyn_Unused2->setCheckable(true);

   m_ui->actionLine_Highlight->setCheckable(true);
   m_ui->actionLine_Highlight->setChecked(m_struct.showLineHighlight);

   m_ui->actionLine_Numbers->setCheckable(true);
   m_ui->actionLine_Numbers->setChecked(m_struct.showLineNumbers);  

   m_ui->actionWord_Wrap->setCheckable(true);
   m_ui->actionWord_Wrap->setChecked(m_struct.isWordWrap);

   m_ui->actionShow_Spaces->setCheckable(true);
   m_ui->actionShow_Spaces->setChecked(m_struct.show_Spaces);

   m_ui->actionShow_Breaks->setCheckable(true);
   m_ui->actionShow_Breaks->setChecked(m_struct.show_Breaks);

   m_ui->actionColumn_Mode->setCheckable(true);
   m_ui->actionColumn_Mode->setChecked(m_struct.isColumnMode);  

   m_ui->actionSpell_Check->setCheckable(true);
   m_ui->actionSpell_Check->setChecked(m_struct.isSpellCheck);     

   m_ui->actionUndo->setEnabled(false);
   m_ui->actionRedo->setEnabled(false);
   m_ui->actionCut->setEnabled(false);
   m_ui->actionCopy->setEnabled(false);

   connect(m_tabWidget, &QTabWidget::currentChanged,    this, &MainWindow::tabChanged);
   connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::tabClose);
}

void MainWindow::createShortCuts(bool setupAll)
{
   // assign to temp value
   struct Options struct_temp;

   struct_temp.key_open         = m_struct.key_open;
   struct_temp.key_close        = m_struct.key_close;
   struct_temp.key_save         = m_struct.key_save;
   struct_temp.key_saveAs       = m_struct.key_saveAs;
   struct_temp.key_print        = m_struct.key_print;
   struct_temp.key_undo         = m_struct.key_undo;
   struct_temp.key_redo         = m_struct.key_redo;
   struct_temp.key_cut	        = m_struct.key_cut;
   struct_temp.key_copy	        = m_struct.key_copy;
   struct_temp.key_paste        = m_struct.key_paste;
   struct_temp.key_selectAll    = m_struct.key_selectAll;
   struct_temp.key_find	        = m_struct.key_find;
   struct_temp.key_replace      = m_struct.key_replace;
   struct_temp.key_findNext     = m_struct.key_findNext;
   struct_temp.key_findPrev     = m_struct.key_findPrev;
   struct_temp.key_goTop        = m_struct.key_goTop;
   struct_temp.key_goBottom     = m_struct.key_goBottom;
   struct_temp.key_newTab       = m_struct.key_newTab;

   struct_temp.key_printPreview = m_struct.key_printPreview;
   struct_temp.key_reload       = m_struct.key_reload;
   struct_temp.key_selectLine   = m_struct.key_selectLine;
   struct_temp.key_selectWord   = m_struct.key_selectWord ;
   struct_temp.key_selectBlock  = m_struct.key_selectBlock;
   struct_temp.key_upper        = m_struct.key_upper;
   struct_temp.key_lower        = m_struct.key_lower;
   struct_temp.key_indentIncr   = m_struct.key_indentIncr;
   struct_temp.key_indentDecr   = m_struct.key_indentDecr;
   struct_temp.key_deleteLine   = m_struct.key_deleteLine;
   struct_temp.key_deleteEOL    = m_struct.key_deleteEOL;
   struct_temp.key_columnMode   = m_struct.key_columnMode ;
   struct_temp.key_goLine       = m_struct.key_goLine;
   struct_temp.key_show_Spaces  = m_struct.key_show_Spaces;
   struct_temp.key_show_Breaks  = m_struct.key_show_Breaks;
   struct_temp.key_macroPlay    = m_struct.key_macroPlay;
   struct_temp.key_spellCheck   = m_struct.key_spellCheck;
   struct_temp.key_copyBuffer   = m_struct.key_copyBuffer;

#ifdef Q_OS_MAC
   struct_temp.key_open         = this->adjustKey(struct_temp.key_open);
   struct_temp.key_close        = this->adjustKey(struct_temp.key_close);
   struct_temp.key_save         = this->adjustKey(struct_temp.key_save);
   struct_temp.key_saveAs       = this->adjustKey(struct_temp.key_saveAs);
   struct_temp.key_print        = this->adjustKey(struct_temp.key_print);   
   struct_temp.key_undo         = this->adjustKey(struct_temp.key_undo);
   struct_temp.key_redo         = this->adjustKey(struct_temp.key_redo);
   struct_temp.key_cut	        = this->adjustKey(struct_temp.key_cut);
   struct_temp.key_copy	        = this->adjustKey(struct_temp.key_copy);
   struct_temp.key_paste        = this->adjustKey(struct_temp.key_paste);
   struct_temp.key_selectAll    = this->adjustKey(struct_temp.key_selectAll);
   struct_temp.key_find	        = this->adjustKey(struct_temp.key_find);
   struct_temp.key_replace      = this->adjustKey(struct_temp.key_replace);
   struct_temp.key_findNext     = this->adjustKey(struct_temp.key_findNext);
   struct_temp.key_findPrev     = this->adjustKey(struct_temp.key_findPrev);
   struct_temp.key_goTop        = this->adjustKey(struct_temp.key_goTop);
   struct_temp.key_goBottom     = this->adjustKey(struct_temp.key_goBottom);
   struct_temp.key_newTab       = this->adjustKey(struct_temp.key_newTab);

   struct_temp.key_printPreview = this->adjustKey(struct_temp.key_printPreview);
   struct_temp.key_reload       = this->adjustKey(struct_temp.key_reload);
   struct_temp.key_selectLine   = this->adjustKey(struct_temp.key_selectLine);
   struct_temp.key_selectWord   = this->adjustKey(struct_temp.key_selectWord);
   struct_temp.key_selectBlock  = this->adjustKey(struct_temp.key_selectBlock);
   struct_temp.key_upper        = this->adjustKey(struct_temp.key_upper);
   struct_temp.key_lower        = this->adjustKey(struct_temp.key_lower);
   struct_temp.key_indentIncr   = this->adjustKey(struct_temp.key_indentIncr);
   struct_temp.key_indentDecr   = this->adjustKey(struct_temp.key_indentDecr);
   struct_temp.key_deleteLine   = this->adjustKey(struct_temp.key_deleteLine);
   struct_temp.key_deleteEOL    = this->adjustKey(struct_temp.key_deleteEOL);
   struct_temp.key_columnMode   = this->adjustKey(struct_temp.key_columnMode);
   struct_temp.key_goLine       = this->adjustKey(struct_temp.key_goLine);
   struct_temp.key_show_Spaces  = this->adjustKey(struct_temp.key_show_Spaces);
   struct_temp.key_show_Breaks  = this->adjustKey(struct_temp.key_show_Breaks);
   struct_temp.key_macroPlay    = this->adjustKey(struct_temp.key_macroPlay);
   struct_temp.key_spellCheck   = this->adjustKey(struct_temp.key_spellCheck);
   struct_temp.key_copyBuffer   = this->adjustKey(struct_temp.key_copyBuffer);
#endif

   // ** standard definded shortcuts

   if (setupAll) {
      // file
      m_ui->actionOpen->setShortcut(QKeySequence(struct_temp.key_open));
      m_ui->actionClose->setShortcut(QKeySequence(struct_temp.key_close));
      m_ui->actionSave->setShortcut(QKeySequence(struct_temp.key_save));
      m_ui->actionSave_As->setShortcut(QKeySequence(struct_temp.key_saveAs));
      m_ui->actionPrint->setShortcut(QKeySequence(struct_temp.key_print));
      m_ui->actionExit->setShortcuts(QKeySequence::Quit);

      // edit
      m_ui->actionUndo->setShortcut(QKeySequence(struct_temp.key_undo));
      m_ui->actionRedo->setShortcut(QKeySequence(struct_temp.key_redo));
      m_ui->actionCut->setShortcut(QKeySequence(struct_temp.key_cut));
      m_ui->actionCopy->setShortcut(QKeySequence(struct_temp.key_copy));
      m_ui->actionPaste->setShortcut(QKeySequence(struct_temp.key_paste));

      m_ui->actionSelect_All->setShortcut(QKeySequence(struct_temp.key_selectAll));
      m_ui->actionGo_Top->setShortcut(QKeySequence(struct_temp.key_goTop));
      m_ui->actionGo_Bottom->setShortcut(QKeySequence(struct_temp.key_goBottom));

      //search
      m_ui->actionFind->setShortcut(QKeySequence(struct_temp.key_find));
      m_ui->actionReplace->setShortcut(QKeySequence(struct_temp.key_replace));
      m_ui->actionFind_Next->setShortcut(QKeySequence(struct_temp.key_findNext));;
      m_ui->actionFind_Prev->setShortcut(QKeySequence(struct_temp.key_findPrev));

      // tab
      m_ui->actionTab_New->setShortcut(QKeySequence(struct_temp.key_newTab));

      // help
      m_ui->actionDiamond_Help->setShortcuts(QKeySequence::HelpContents);
   }

   // ** user definded

   // edit
   m_ui->actionPrint_Preview->setShortcut(QKeySequence(struct_temp.key_printPreview) );
   m_ui->actionReload->setShortcut(QKeySequence(struct_temp.key_reload) );

   m_ui->actionSelect_Line->setShortcut(QKeySequence(struct_temp.key_selectLine)   );
   m_ui->actionSelect_Word->setShortcut(QKeySequence(struct_temp.key_selectWord)   );
   m_ui->actionSelect_Block->setShortcut(QKeySequence(struct_temp.key_selectBlock) );
   m_ui->actionCase_Upper->setShortcut(QKeySequence(struct_temp.key_upper) );
   m_ui->actionCase_Lower->setShortcut(QKeySequence(struct_temp.key_lower) );

   m_ui->actionIndent_Incr->setShortcut(QKeySequence(struct_temp.key_indentIncr) );
   m_ui->actionIndent_Decr->setShortcut(QKeySequence(struct_temp.key_indentDecr) );
   m_ui->actionDelete_Line->setShortcut(QKeySequence(struct_temp.key_deleteLine) );
   m_ui->actionDelete_EOL->setShortcut(QKeySequence(struct_temp.key_deleteEOL)   );
   m_ui->actionColumn_Mode->setShortcut(QKeySequence(struct_temp.key_columnMode) );

   // search
   m_ui->actionGo_Line->setShortcut(QKeySequence(struct_temp.key_goLine) );

   // view
   m_ui->actionShow_Spaces->setShortcut(QKeySequence(struct_temp.key_show_Spaces) );
   m_ui->actionShow_Breaks->setShortcut(QKeySequence(struct_temp.key_show_Breaks) );

   // tools
   m_ui->actionMacro_Play->setShortcut(QKeySequence(struct_temp.key_macroPlay)   );
   m_ui->actionSpell_Check->setShortcut(QKeySequence(struct_temp.key_spellCheck) );

   // copy buffer
   m_actionCopyBuffer->setKey(QKeySequence(struct_temp.key_copyBuffer) );
}

QString MainWindow::adjustKey(QString sequence)
{
   QString modifier = sequence.left(4);

   if (modifier == "Ctrl") {
      sequence.replace(0,4,"Meta");
   }

   if (modifier == "Meta") {
      sequence.replace(0,4,"Ctrl");
   }

   return sequence;
}

void MainWindow::createToolBars()
{
   m_ui->actionTab_New->setIcon(QIcon(":/resources/tab_new.png"));
   m_ui->actionTab_Close->setIcon(QIcon(":/resources/tab_remove.png"));

   m_ui->actionOpen->setIcon(QIcon(":/resources/file_open.png"));
   m_ui->actionClose->setIcon(QIcon(":/resources/file_close.png"));
   m_ui->actionSave->setIcon(QIcon(":/resources/save.png"));

   m_ui->actionPrint->setIcon(QIcon(":/resources/print.png"));
   m_ui->actionPrint_Pdf->setIcon(QIcon(":/resources/print_pdf.png"));

   m_ui->actionUndo->setIcon(QIcon(":/resources/undo.png"));
   m_ui->actionRedo->setIcon(QIcon(":/resources/redo.png"));
   m_ui->actionCut->setIcon(QIcon(":/resources/cut.png"));
   m_ui->actionCopy->setIcon(QIcon(":/resources/copy.png"));
   m_ui->actionPaste->setIcon(QIcon(":/resources/paste.png"));

   m_ui->actionFind->setIcon(QIcon(":/resources/find.png"));
   m_ui->actionReplace->setIcon(QIcon(":/resources/find_replace.png"));

   m_ui->actionShow_Spaces->setIcon(QIcon(":/resources/show_spaces.png"));
   m_ui->actionShow_Breaks->setIcon(QIcon(":/resources/show_eol.png"));

   m_ui->actionMacro_Start->setIcon(QIcon(":/resources/macro_rec.png"));
   m_ui->actionMacro_Stop->setIcon(QIcon(":/resources/macro_stop.png"));
   m_ui->actionMacro_Play->setIcon(QIcon(":/resources/macro_play.png"));
   m_ui->actionSpell_Check->setIcon(QIcon(":/resources/spellcheck.png"));

   //
   fileToolBar = addToolBar(tr("File")); 
   fileToolBar->addAction(m_ui->actionOpen);
   fileToolBar->addAction(m_ui->actionClose);
   fileToolBar->addAction(m_ui->actionSave);
   fileToolBar->addSeparator();
   fileToolBar->addAction(m_ui->actionPrint);
   fileToolBar->addAction(m_ui->actionPrint_Pdf);

#ifdef Q_OS_MAC
   // os x does not support moving tool bars
   fileToolBar->addSeparator();
#endif

   windowToolBar = addToolBar(tr("Window"));
   windowToolBar->addAction(m_ui->actionTab_New);
   windowToolBar->addAction(m_ui->actionTab_Close);

#ifdef Q_OS_MAC
   windowToolBar->addSeparator();
#endif

   editToolBar = addToolBar(tr("Edit"));
   editToolBar->addAction(m_ui->actionUndo);
   editToolBar->addAction(m_ui->actionRedo);
   editToolBar->addAction(m_ui->actionCut);
   editToolBar->addAction(m_ui->actionCopy);
   editToolBar->addAction(m_ui->actionPaste);

#ifdef Q_OS_MAC
   editToolBar->addSeparator();
#endif

   searchToolBar = addToolBar(tr("Search"));
   searchToolBar->addAction(m_ui->actionFind);
   searchToolBar->addAction(m_ui->actionReplace);

#ifdef Q_OS_MAC
   searchToolBar->addSeparator();
#endif

   viewToolBar = addToolBar(tr("View"));
   viewToolBar->addAction(m_ui->actionShow_Spaces);
   viewToolBar->addAction(m_ui->actionShow_Breaks);

#ifdef Q_OS_MAC
   viewToolBar->addSeparator();
#endif

   toolsToolBar = addToolBar(tr("Tools"));
   toolsToolBar->addAction(m_ui->actionMacro_Start);
   toolsToolBar->addAction(m_ui->actionMacro_Stop);
   toolsToolBar->addAction(m_ui->actionMacro_Play);
   toolsToolBar->addSeparator();
   toolsToolBar->addAction(m_ui->actionSpell_Check);
}

void MainWindow::createStatusBar()
{   
   m_statusLine = new QLabel("", this);
   //m_statusLine->setFrameStyle(QFrame::Panel| QFrame::Sunken);   

   m_statusMode = new QLabel("", this);
   //m_statusMode->setFrameStyle(QFrame::Panel | QFrame::Sunken);   

   m_statusName = new QLabel("", this);
   //m_statusName->setFrameStyle(QFrame::Panel | QFrame::Sunken);

   statusBar()->addPermanentWidget(m_statusLine, 0);
   statusBar()->addPermanentWidget(m_statusMode, 0);
   statusBar()->addPermanentWidget(m_statusName, 0);    
}

void MainWindow::setStatusBar(QString msg, int timeOut)
{
   statusBar()->showMessage(msg, timeOut);
}

void MainWindow::showNotDone(QString item)
{
   csMsg( item + " - this feature has not been implemented.");
}
