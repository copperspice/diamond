/**************************************************************************
*
* Copyright (c) 2012-2013 Barbara Geller
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
#include "dialog_symbols.h"
#include "mainwindow.h"

#include <stdexcept>

#include <QtGui>
#include <QChar>
#include <QFontMetrics>
#include <QKeySequence>

MainWindow::MainWindow(QStringList fileList, QStringList flagList)
   : m_ui(new Ui::MainWindow)
{
   m_ui->setupUi(this);
   setIconSize(QSize(32,32));
   setWindowIcon( QIcon("://resources/diamond.png"));

   this->setDiamondTitle("untitled.txt");

   if ( ! json_Read() ) {
      // do not start program
      csError(tr("Configuration File Missing"), tr("Unable to locate or open the Diamond Configuration file."));
      throw std::runtime_error("");
   }

   // drag & drop
   setAcceptDrops(true);

   // tab stops
   this->setUpTabStops();

   // remaining methods must be done after json_Read for config
   m_tabWidget = new QTabWidget;
   m_tabWidget->setTabsClosable(true);
   m_tabWidget->setMovable(true);

   // pre set up the splitter, only display the main window
   m_splitter = new QSplitter(Qt::Vertical);
   m_splitter->addWidget(m_tabWidget);
   setCentralWidget(m_splitter);

   connect(qApp, SIGNAL(focusChanged(QWidget *, QWidget *)), this, SLOT(focusChanged(QWidget *, QWidget *)));

   m_isSplit = false;

   // screen setup
   createShortCuts(true);
   createToolBars();
   createStatusBar();
   createToggles();
   createConnections();

   // recent folders
   rfolder_CreateMenus();

   // recent files
   rf_CreateMenus(); 

   // spell check
   createSpellCheck();

   // recent folders, context menu
   QMenu *menuFolder = m_ui->actionOpen_RecentFolder->menu();
   menuFolder->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(menuFolder, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenuFolder(const QPoint &)));

   // recent files, context menu
   m_ui->menuFile->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(m_ui->menuFile, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenuFile(const QPoint &)));

   // save flags after reading config and before autoload
   m_args.flag_noAutoLoad   = false;
   m_args.flag_noSaveConfig = false;

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

void MainWindow::mw_open()
{
   this->open(m_struct.pathPrior);
}

void MainWindow::open(QString path)
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

void MainWindow::close_Doc()
{
   bool okClose = querySave();

   if (okClose) {
      // update open tab list
      openTab_Delete();

      m_textEdit->clear();
      setCurrentTitle("");
   }
}

bool MainWindow::closeAll_Doc()
{
   bool allClosed = true;

   QWidget *temp;
   DiamondTextEdit *textEdit;

   int count = m_tabWidget->count();
   int whichTab = 0;

   //
   m_openedFiles.clear();

   for (int k = 0; k < count; ++k) {

      temp = m_tabWidget->widget(whichTab);
      textEdit = dynamic_cast<DiamondTextEdit *>(temp);

      if (textEdit) {
         m_textEdit = textEdit;
         m_curFile  = this->get_curFileName(whichTab);

         if (querySave())  {

            // save for OpenFileList
            if ( m_curFile != "untitled.txt" ) {
               m_openedFiles.append(m_curFile);
            }

            if (m_tabWidget->count() == 1) {
               // do not remove !
               m_textEdit->clear();

               setCurrentTitle("");

            } else {
               // may need to delete the widget
               m_tabWidget->removeTab(whichTab);
            }

         } else  {           
            // move over one tab            
            ++whichTab;

            // something is still open
            allClosed = false;
         }
      }
   }

   //
   m_tabWidget->setCurrentIndex(0);

   // update open tab list
   openTab_UpdateActions();

   return allClosed;
}

void MainWindow::reload()
{   
   if (m_curFile.isEmpty()) {
      csError("Reload", tr("Unable to reload a file which was not saved."));

   } else if (m_textEdit->document()->isModified()) {

      QMessageBox quest;
      quest.setWindowTitle(tr("Reload File"));
      quest.setText( "File: " + m_curFile + tr(" has been modified. Reload file?"));

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
      return saveAs(true);
   } else {
      return saveFile(m_curFile, true);
   }      
}

bool MainWindow::saveAs()
{
   // called from clicking on the menu
   return saveAs(true);
}

bool MainWindow::saveAs(bool isSaveOne)
{
   bool retval = false;

   QString selectedFilter;
   QFileDialog::Options options;

   // force windows 7 and 8 to honor initial path
   options = QFileDialog::ForceInitialDir_Win7;

   QString path = this->pathName(m_curFile);

   QString fileName = QFileDialog::getSaveFileName(this, tr("Create or Select File"),
        path, tr("All Files (*)"), &selectedFilter, options);

   if (fileName.isEmpty()) {
      retval = false;

   } else {
      retval = saveFile(fileName, isSaveOne);

      if (retval) {
         setCurrentTitle(fileName, false);

         if (m_isSplit) {
            split_Title();
         }
      }
   }

   return retval;
}

void MainWindow::saveAll()
{
   QString file;

   QWidget *temp;
   DiamondTextEdit *textEdit;

   // save current textEdit
   DiamondTextEdit *cur_textEdit = m_textEdit;

   int count = m_tabWidget->count();

   for (int k = 0; k < count; ++k) {

      temp = m_tabWidget->widget(k);
      textEdit = dynamic_cast<DiamondTextEdit *>(temp);

      if (textEdit) {
         m_textEdit = textEdit;
         file = m_tabWidget->tabWhatsThis(k);

         if (file == "untitled.txt") {
            if (saveAs(false)) {
               m_textEdit->document()->setModified(false);
            }

         } else {
            if ( saveFile(file, false) ) {
               m_textEdit->document()->setModified(false);
            }

         }
      }
   }

   // load the current textEdit again
   m_textEdit = cur_textEdit;

   if (! m_textEdit->document()->isModified())  {
      setWindowModified(false);      
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
         m_textEdit->textCursor().insertText(text);
      }
   }

   delete dw;
}

void MainWindow::mw_indentIncr()
{
   indentIncr("indent");
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

void MainWindow::mw_indentDecr()
{
   indentDecr("unindent");
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
   int line = get_Value1("line");

   QTextCursor cursor(m_textEdit->textCursor());
   cursor.movePosition(QTextCursor::Start);

   cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line);
   m_textEdit->setTextCursor(cursor);
}

void MainWindow::goColumn()
{
   int col = get_Value1("col");

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
   selection.format.setProperty(QTextFormat::UserProperty, "highlightbar");

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
   QTextDocument *td = m_textEdit->document();
   QTextOption textOpt = td->defaultTextOption();

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

   json_Write(SHOW_SPACES);
}

void MainWindow::show_Breaks()
{
   QTextDocument *td = m_textEdit->document();
   QTextOption textOpt = td->defaultTextOption();

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

   json_Write(SHOW_BREAKS);
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
void MainWindow::setSyn_C()
{
   forceSyntax(SYN_C);
}

void MainWindow::setSyn_Clipper()
{
   forceSyntax(SYN_CLIPPER);
}

void MainWindow::setSyn_Css()
{
   forceSyntax(SYN_CSS);
}

void MainWindow::setSyn_Dox()
{
   forceSyntax(SYN_DOX);
}

void MainWindow::setSyn_Html()
{
   forceSyntax(SYN_HTML);
}

void MainWindow::setSyn_Java()
{
   forceSyntax(SYN_JAVA);
}

void MainWindow::setSyn_Javascript()
{
   forceSyntax(SYN_JS);
}

void MainWindow::setSyn_Json()
{
   forceSyntax(SYN_JSON);
}

void MainWindow::setSyn_Makefile()
{
   forceSyntax(SYN_MAKE);
}

void MainWindow::setSyn_Nsis()
{
   forceSyntax(SYN_NSIS);
}

void MainWindow::setSyn_Text()
{
   forceSyntax(SYN_TEXT);
}

void MainWindow::setSyn_Shell_S()
{

   forceSyntax(SYN_SHELL_S);
}

void MainWindow::setSyn_Perl()
{
   forceSyntax(SYN_PERL);
}

void MainWindow::setSyn_Php()
{   
   forceSyntax(SYN_PHP);
}

void MainWindow::setSyn_Python()
{  
   forceSyntax(SYN_PYTHON);
}

void MainWindow::setSyn_None()
{   
   forceSyntax(SYN_NONE);
}

void MainWindow::setSynType(SyntaxTypes data)
{
   m_ui->actionSyn_C->setChecked(false);
   m_ui->actionSyn_Clipper->setChecked(false);
   m_ui->actionSyn_Css->setChecked(false);
   m_ui->actionSyn_Doxygen->setChecked(false);
   m_ui->actionSyn_Html->setChecked(false);
   m_ui->actionSyn_Java->setChecked(false);  
   m_ui->actionSyn_Javascript->setChecked(false);
   m_ui->actionSyn_Json->setChecked(false);
   m_ui->actionSyn_Makefile->setChecked(false);
   m_ui->actionSyn_Nsis->setChecked(false);
   m_ui->actionSyn_Text->setChecked(false);
   m_ui->actionSyn_Shell_S->setChecked(false);
   m_ui->actionSyn_Perl->setChecked(false);
   m_ui->actionSyn_PHP->setChecked(false);
   m_ui->actionSyn_Python->setChecked(false);
   m_ui->actionSyn_None->setChecked(false); 

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

      case SYN_DOX:
         m_ui->actionSyn_Doxygen->setChecked(true);
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

      case SYN_SHELL_S:
         m_ui->actionSyn_Shell_S->setChecked(true);
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

void MainWindow::formatMac()
{
   showNotDone("Document, format Mac (CR)");
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
   if ( ! m_record ) {
      m_record = true;
      m_textEdit->macroStart();

      setStatusBar(tr("Recording macro. . ."), 0);
   }
}

void MainWindow::mw_macroStop()
{
   if ( m_record ) {
      m_record = false;
      m_textEdit->macroStop();

      // save macro
      json_Write(MACRO);

      setStatusBar(tr("Macro recorded"), 1000);
   }
}

void MainWindow::macroPlay()
{
   if (m_record) {
      csError("Macro Playback", "Unable to play back a macro while recording");

   }  else {
      QList<QKeyEvent *> macroList;
      macroList = m_textEdit->get_MacroKeyList();

      int cnt = macroList.count();

      if (cnt == 0) {
         csError("Macro Playback", "No macro to play back");

      } else {
         QKeyEvent *event;

         for (int k = 0; k < cnt; ++k)   {
            event = macroList.at(k);

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

   //
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

   //
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
   m_textEdit = new DiamondTextEdit(this, m_struct, m_spellCheck);

   // hold for reference
   m_noSplit_textEdit = m_textEdit;

   // triggers a call to tabChanged
   int index = m_tabWidget->addTab(m_textEdit, "untitled.txt");
   m_tabWidget->setCurrentIndex(index);

   setScreenColors();
   int temp = m_textEdit->fontMetrics().width(" ");
   m_textEdit->setTabStopWidth(temp * m_struct.tabSpacing);

   //
   m_textEdit->setFocus();

   // connect(m_textEdit, SIGNAL(fileDropped(const QString &)),  this, SLOT(fileDropped(const QString &)));
   connect(m_textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));

   connect(m_textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(moveBar()));
   connect(m_textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(setStatus_LineCol()));

   connect(m_textEdit, SIGNAL(undoAvailable(bool)), m_ui->actionUndo, SLOT(setEnabled(bool)));
   connect(m_textEdit, SIGNAL(redoAvailable(bool)), m_ui->actionRedo, SLOT(setEnabled(bool)));
   connect(m_textEdit, SIGNAL(copyAvailable(bool)), m_ui->actionCut,  SLOT(setEnabled(bool)));
   connect(m_textEdit, SIGNAL(copyAvailable(bool)), m_ui->actionCopy, SLOT(setEnabled(bool)));
}

void MainWindow::mw_tabClose()
{     
   int index = m_tabWidget->currentIndex();
   tabClose(index);
}

void MainWindow::tabClose(int index)
{   
   QWidget *temp = m_tabWidget->widget(index);

   DiamondTextEdit *textEdit;
   textEdit = dynamic_cast<DiamondTextEdit *>(temp);

   if (textEdit) {
      m_textEdit = textEdit;     
      m_curFile  = this->get_curFileName(index);

      bool okClose = querySave();

      if (okClose)  {
         // update open tab list
         openTab_Delete();

         //
         int count = m_tabWidget->count();

         if (count == 1) {
            // do not remove !
            m_textEdit->clear();
            setCurrentTitle("");

         } else {
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

      // hold for reference
      m_noSplit_textEdit = m_textEdit;

      m_curFile = this->get_curFileName(index);
      this->setCurrentTitle(m_curFile);

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

void MainWindow::focusChanged(QWidget *old, QWidget *current)
{
   if (! current) {
      return;
   }

   DiamondTextEdit *textEdit;
   textEdit = dynamic_cast<DiamondTextEdit *>(current);

   if (textEdit)  {
      m_textEdit = textEdit;
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
   QString textBody = "<font color='#000080'><table style=margin-right:25>"
                      "<tr><td><nobr>Developed by Barbara Geller</nobr></td><td>barbara@copperspice.com</td></tr>"
                      "<tr><td style=padding-right:25><nobr>Developed by Ansel Sermersheim</nobr></td><td>ansel@copperspice.com</td></tr>"
                      "</table></font>"
                      "<br>"
                      "<p><small>Copyright 2012-2013 BG Consulting, All rights reserved.<br>"
                      "This program is provided AS IS with no warranty of any kind.<br></small></p>";

   //
   QMessageBox msgB;
   msgB.setIcon(QMessageBox::NoIcon);
   msgB.setWindowIcon(QIcon("://resources/diamond.png"));

   msgB.setWindowTitle(tr("About Diamond"));
   msgB.setText(tr("<p style=margin-right:25><center><h5>Version: 1.0<br>Build # 05.1.2013</h5></center></p>"));
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
   connect(m_ui->actionNew,            SIGNAL(triggered()), this, SLOT(newFile()));
   connect(m_ui->actionOpen,           SIGNAL(triggered()), this, SLOT(mw_open()));
   connect(m_ui->actionClose,          SIGNAL(triggered()), this, SLOT(close_Doc()));
   connect(m_ui->actionClose_All,      SIGNAL(triggered()), this, SLOT(closeAll_Doc()));
   connect(m_ui->actionReload,         SIGNAL(triggered()), this, SLOT(reload()));

   connect(m_ui->actionSave,           SIGNAL(triggered()), this, SLOT(save()));
   connect(m_ui->actionSave_As,        SIGNAL(triggered()), this, SLOT(saveAs()));
   connect(m_ui->actionSave_All,       SIGNAL(triggered()), this, SLOT(saveAll()));

   connect(m_ui->actionPrint,          SIGNAL(triggered()), this, SLOT(print()));
   connect(m_ui->actionPrint_Preview,  SIGNAL(triggered()), this, SLOT(printPreview()));
   connect(m_ui->actionPrint_Pdf,      SIGNAL(triggered()), this, SLOT(printPdf()));
   connect(m_ui->actionExit,           SIGNAL(triggered()), this, SLOT(close()));

   // edit
   connect(m_ui->actionUndo,              SIGNAL(triggered()), this, SLOT(mw_undo()));
   connect(m_ui->actionRedo,              SIGNAL(triggered()), this, SLOT(mw_redo()));
   connect(m_ui->actionCut,               SIGNAL(triggered()), this, SLOT(mw_cut()));
   connect(m_ui->actionCopy,              SIGNAL(triggered()), this, SLOT(mw_copy()));
   connect(m_ui->actionPaste,             SIGNAL(triggered()), this, SLOT(mw_paste()));

   connect(m_ui->actionSelect_All,        SIGNAL(triggered()), this, SLOT(selectAll()));
   connect(m_ui->actionSelect_Block,      SIGNAL(triggered()), this, SLOT(selectBlock()));
   connect(m_ui->actionSelect_Line,       SIGNAL(triggered()), this, SLOT(selectLine()));
   connect(m_ui->actionSelect_Word,       SIGNAL(triggered()), this, SLOT(selectWord()));
   connect(m_ui->actionCase_Upper,        SIGNAL(triggered()), this, SLOT(caseUpper()));
   connect(m_ui->actionCase_Lower,        SIGNAL(triggered()), this, SLOT(caseLower()));
   connect(m_ui->actionCase_Cap,          SIGNAL(triggered()), this, SLOT(caseCap()));

   connect(m_ui->actionIndent_Incr,       SIGNAL(triggered()), this, SLOT(mw_indentIncr()));
   connect(m_ui->actionIndent_Decr,       SIGNAL(triggered()), this, SLOT(mw_indentDecr()));
   connect(m_ui->actionDelete_Line,       SIGNAL(triggered()), this, SLOT(deleteLine()));
   connect(m_ui->actionDelete_EOL,        SIGNAL(triggered()), this, SLOT(deleteEOL()));

   connect(m_ui->actionInsert_Date,       SIGNAL(triggered()), this, SLOT(insertDate()));
   connect(m_ui->actionInsert_Time,       SIGNAL(triggered()), this, SLOT(insertTime()));
   connect(m_ui->actionInsert_Symbol,     SIGNAL(triggered()), this, SLOT(insertSymbol()));
   connect(m_ui->actionColumn_Mode,       SIGNAL(triggered()), this, SLOT(columnMode()));

   // search
   connect(m_ui->actionFind,              SIGNAL(triggered()), this, SLOT(find()));
   connect(m_ui->actionReplace,           SIGNAL(triggered()), this, SLOT(replace()));
   connect(m_ui->actionFind_Next,         SIGNAL(triggered()), this, SLOT(findNext()));
   connect(m_ui->actionFind_Prev,         SIGNAL(triggered()), this, SLOT(findPrevious()));
   connect(m_ui->actionAdv_Find,          SIGNAL(triggered()), this, SLOT(advFind()));
   connect(m_ui->actionGo_Line,           SIGNAL(triggered()), this, SLOT(goLine()));
   connect(m_ui->actionGo_Column,         SIGNAL(triggered()), this, SLOT(goColumn()));
   connect(m_ui->actionGo_Top,            SIGNAL(triggered()), this, SLOT(goTop()));
   connect(m_ui->actionGo_Bottom,         SIGNAL(triggered()), this, SLOT(goBottom()));

   // view
   connect(m_ui->actionLine_Highlight,    SIGNAL(triggered()), this, SLOT(lineHighlight()));
   connect(m_ui->actionLine_Numbers,      SIGNAL(triggered()), this, SLOT(lineNumbers()));
   connect(m_ui->actionWord_Wrap,         SIGNAL(triggered()), this, SLOT(wordWrap()));
   connect(m_ui->actionShow_Spaces,       SIGNAL(triggered()), this, SLOT(show_Spaces()));
   connect(m_ui->actionShow_Breaks,       SIGNAL(triggered()), this, SLOT(show_Breaks()));
   connect(m_ui->actionDisplay_HTML,      SIGNAL(triggered()), this, SLOT(displayHTML()));

   // document
   connect(m_ui->actionSyn_C,             SIGNAL(triggered()), this, SLOT(setSyn_C()));
   connect(m_ui->actionSyn_Clipper,       SIGNAL(triggered()), this, SLOT(setSyn_Clipper()));
   connect(m_ui->actionSyn_Css,           SIGNAL(triggered()), this, SLOT(setSyn_Css()));
   connect(m_ui->actionSyn_Doxygen,       SIGNAL(triggered()), this, SLOT(setSyn_Dox()));
   connect(m_ui->actionSyn_Html,          SIGNAL(triggered()), this, SLOT(setSyn_Html()));
   connect(m_ui->actionSyn_Java,          SIGNAL(triggered()), this, SLOT(setSyn_Java()));
   connect(m_ui->actionSyn_Javascript,    SIGNAL(triggered()), this, SLOT(setSyn_Javascript()));
   connect(m_ui->actionSyn_Json,          SIGNAL(triggered()), this, SLOT(setSyn_Json()));
   connect(m_ui->actionSyn_Makefile,      SIGNAL(triggered()), this, SLOT(setSyn_Makefile()));
   connect(m_ui->actionSyn_Nsis,          SIGNAL(triggered()), this, SLOT(setSyn_Nsis()));
   connect(m_ui->actionSyn_Text,          SIGNAL(triggered()), this, SLOT(setSyn_Text()));
   connect(m_ui->actionSyn_Shell_S,       SIGNAL(triggered()), this, SLOT(setSyn_Shell_S()));
   connect(m_ui->actionSyn_Perl,          SIGNAL(triggered()), this, SLOT(setSyn_Perl()));
   connect(m_ui->actionSyn_PHP,           SIGNAL(triggered()), this, SLOT(setSyn_Php()));
   connect(m_ui->actionSyn_Python,        SIGNAL(triggered()), this, SLOT(setSyn_Python()));
   connect(m_ui->actionSyn_None,          SIGNAL(triggered()), this, SLOT(setSyn_None()));

   connect(m_ui->actionFormat_Unix,       SIGNAL(triggered()), this, SLOT(formatUnix()));
   connect(m_ui->actionFormat_Win,        SIGNAL(triggered()), this, SLOT(formatWin()));
   connect(m_ui->actionFormat_Mac,        SIGNAL(triggered()), this, SLOT(formatMac()));

   connect(m_ui->actionFix_Tab_Spaces,    SIGNAL(triggered()), this, SLOT(fixTab_Spaces()));  
   connect(m_ui->actionFix_Spaces_Tab,    SIGNAL(triggered()), this, SLOT(fixSpaces_Tab()));
   connect(m_ui->actionDeleteEOL_Spaces,  SIGNAL(triggered()), this, SLOT(deleteEOL_Spaces()));

   // tools
   connect(m_ui->actionMacro_Start,       SIGNAL(triggered()), this, SLOT(mw_macroStart()));
   connect(m_ui->actionMacro_Stop,        SIGNAL(triggered()), this, SLOT(mw_macroStop()));
   connect(m_ui->actionMacro_Play,        SIGNAL(triggered()), this, SLOT(macroPlay()));
   connect(m_ui->actionMacro_Load,        SIGNAL(triggered()), this, SLOT(macroLoad()));
   connect(m_ui->actionMacro_EditNames,   SIGNAL(triggered()), this, SLOT(macroEditNames()));
   connect(m_ui->actionSpell_Check,       SIGNAL(triggered()), this, SLOT(spellCheck()));

   // settings
   connect(m_ui->actionColors,            SIGNAL(triggered()), this, SLOT(setColors()));
   connect(m_ui->actionFonts,             SIGNAL(triggered()), this, SLOT(setFont()));
   connect(m_ui->actionMove_ConfigFile,   SIGNAL(triggered()), this, SLOT(move_ConfigFile()));
   connect(m_ui->actionOptions,           SIGNAL(triggered()), this, SLOT(setOptions()));
   connect(m_ui->actionPrintOptions,      SIGNAL(triggered()), this, SLOT(setPrintOptions()));

   // window
   connect(m_ui->actionTab_New,           SIGNAL(triggered()), this, SLOT(tabNew()));
   connect(m_ui->actionTab_Close,         SIGNAL(triggered()), this, SLOT(mw_tabClose()));
   connect(m_ui->actionSplit_Horizontal,  SIGNAL(triggered()), this, SLOT(split_Horizontal()));
   connect(m_ui->actionSplit_Vertical,    SIGNAL(triggered()), this, SLOT(split_Vertical()));

   // help menu
   connect(m_ui->actionDiamond_Help,      SIGNAL(triggered()), this, SLOT(diamondHelp()));
   connect(m_ui->actionAbout,             SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createToggles()
{    
   m_ui->actionSyn_C->setCheckable(true);
   m_ui->actionSyn_Clipper->setCheckable(true);
   m_ui->actionSyn_Css->setCheckable(true);
   m_ui->actionSyn_Doxygen->setCheckable(true);
   m_ui->actionSyn_Html->setCheckable(true);
   m_ui->actionSyn_Java->setCheckable(true);
   m_ui->actionSyn_Javascript->setCheckable(true);
   m_ui->actionSyn_Json->setCheckable(true);
   m_ui->actionSyn_Makefile->setCheckable(true);
   m_ui->actionSyn_Nsis->setCheckable(true);
   m_ui->actionSyn_Text->setCheckable(true);
   m_ui->actionSyn_Shell_S->setCheckable(true);
   m_ui->actionSyn_Perl->setCheckable(true);
   m_ui->actionSyn_PHP->setCheckable(true);
   m_ui->actionSyn_Python->setCheckable(true);
   m_ui->actionSyn_None->setCheckable(true);

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

   //
   m_ui->actionUndo->setEnabled(false);
   m_ui->actionRedo->setEnabled(false);
   m_ui->actionCut->setEnabled(false);
   m_ui->actionCopy->setEnabled(false);

   //
   connect(m_tabWidget, SIGNAL(currentChanged(int)),    this, SLOT(tabChanged(int)));
   connect(m_tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(tabClose(int)));
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
   m_ui->actionSelect_Line->setShortcut(QKeySequence(struct_temp.key_selectLine)   );
   m_ui->actionSelect_Word->setShortcut(QKeySequence(struct_temp.key_selectWord)   );
   m_ui->actionSelect_Block->setShortcut(QKeySequence(struct_temp.key_selectBlock) );
   m_ui->actionCase_Upper->setShortcut(QKeySequence(struct_temp.key_upper) );
   m_ui->actionCase_Lower->setShortcut(QKeySequence(struct_temp.key_lower) );

   m_ui->actionIndent_Incr->setShortcut(QKeySequence(struct_temp.key_indentIncr) );
   m_ui->actionIndent_Decr->setShortcut(QKeySequence(struct_temp.key_indentDecr) );
   m_ui->actionDelete_Line->setShortcut(QKeySequence(struct_temp.key_deleteLine) );
   m_ui->actionDelete_EOL->setShortcut(QKeySequence(struct_temp.key_deleteEOL));
   m_ui->actionColumn_Mode->setShortcut(QKeySequence(struct_temp.key_columnMode) );

   // search
   m_ui->actionGo_Line->setShortcut(QKeySequence(struct_temp.key_goLine) );

   // view
   m_ui->actionShow_Spaces->setShortcut(QKeySequence(struct_temp.key_show_Spaces) );
   m_ui->actionShow_Breaks->setShortcut(QKeySequence(struct_temp.key_show_Breaks) );

   // tools
   m_ui->actionMacro_Play->setShortcut(QKeySequence(struct_temp.key_macroPlay) );
   m_ui->actionSpell_Check->setShortcut(QKeySequence(struct_temp.key_spellCheck) );
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

