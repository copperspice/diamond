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

#include "mainwindow.h"
#include "dialog_colors.h"
#include "dialog_find.h"
#include "dialog_replace.h"
#include "dialog_options.h"
#include "dialog_symbols.h"

#include <QtGui>
#include <QDir>
#include <QString>
#include <QStringList>

MainWindow::MainWindow()
   : m_ui(new Ui::MainWindow)
{
   m_ui->setupUi(this);
   setWindowFilePath("untitled.txt");

   if ( ! json_Read()  ) {
      csError(tr("Settings File"), tr("Unable to locate or open the Settings file."));
   }

   m_syntaxParser = 0; 

   // remaining methods must be done after jsson_Read
   m_tabWidget = new QTabWidget;      

   m_tabWidget->setTabsClosable(true);
   m_tabWidget->setMovable(true);
   setCentralWidget(m_tabWidget);  

   // screen setup
   createShortCuts();      
   createToolBars();
   createStatusBar();
   createToggles();
   createConnections();

   // recent files
   rf_CreateMenus();

   // spell check
   createSpellCheck();

   //
   tabNew();

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
      setCurrentFile("");
   }
}

void MainWindow::open()
{       
   QFileDialog::Options options;

   if (false)  {  //(Q_OS_DARWIM) {
      options |= QFileDialog::DontUseNativeDialog;
   }

   QString selectedFilter;

   QStringList fileList = QFileDialog::getOpenFileNames(this, tr("Select File"),
         m_struct.pathPrior, tr("All Files (*)"), &selectedFilter, options);

   int cnt = fileList.count();

   for (int k = 0; k < cnt; ++k )  {
      QString fileName = fileList.at(k);

      if (! fileName.isEmpty()) {
         loadFile(fileName, true);
      }
   }
}

void MainWindow::close_Doc()
{
   bool okClose = querySave();

   if (okClose) {
      m_textEdit->clear();
      setCurrentFile("");
   }
}

bool MainWindow::closeAll_Doc()
{
   bool allClosed = true;

   QWidget *temp;
   DiamondTextEdit *textEdit;

   int cnt = m_tabWidget->count();
   int whichTab = 0;

   for (int index = 0; index < cnt; ++index) {

      temp = m_tabWidget->widget(whichTab);
      textEdit = dynamic_cast<DiamondTextEdit *>(temp);

      if (textEdit) {
         m_textEdit = textEdit;
         m_curFile  = m_tabWidget->tabWhatsThis(whichTab);

         if (querySave())  {

            if (m_tabWidget->count() == 1) {
               // do not remove !
               m_textEdit->clear();
               setCurrentFile("");

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

   return allClosed;
}

void MainWindow::reload()
{
   if (m_curFile.isEmpty()) {
      csError("Reload", tr("Unable to reload a file which was not saved."));

   } else if (m_textEdit->document()->isModified()) {

      QString fileName = m_curFile;
      if (fileName.isEmpty()){
         fileName = "Untitled.txt";
      }

      QMessageBox quest;
      quest.setWindowTitle(tr("Diamond Editor"));
      quest.setText( fileName + tr(" has been modified. Reload file?"));

      QPushButton *reload = quest.addButton("Reload", QMessageBox::AcceptRole );
      quest.setStandardButtons( QMessageBox::Cancel );
      quest.setDefaultButton(QMessageBox::Cancel);

      quest.exec();

      if (quest.clickedButton() == reload) {
        loadFile(m_curFile, false);
      }

   } else {
      loadFile(m_curFile, false);

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
   // slot called from menu
   return saveAs(true);
}

bool MainWindow::saveAs(bool isSaveOne)
{
   bool retval = false;

   QFileDialog::Options options;

   if (false)  {  // broom  (Q_OS_DARWIM) {
      options |= QFileDialog::DontUseNativeDialog;
   }

   QString selectedFilter;
   QString fileName = QFileDialog::getSaveFileName(this, tr("Create or Select File"),
         "", tr("All Files (*)"), &selectedFilter, options);

   if (fileName.isEmpty()) {
      retval = false;
   } else {
      retval = saveFile(fileName, isSaveOne);
   }

   return retval;
}

bool MainWindow::saveAll()
{
   QString file;
   QWidget *temp;
   DiamondTextEdit *textEdit;

   // save current textEdit
   DiamondTextEdit *cur_textEdit = m_textEdit;

   int cnt = m_tabWidget->count();

   for (int index = 0; index < cnt; ++index) {

      temp = m_tabWidget->widget(index);
      textEdit = dynamic_cast<DiamondTextEdit *>(temp);

      if (textEdit) {
         m_textEdit = textEdit;
         file = m_tabWidget->tabWhatsThis(index);

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

void MainWindow::print()
{
   QPrinter printer;
   QPrintDialog *dw = new QPrintDialog(&printer, this);
   dw->setWindowTitle(tr("Select Printer"));

   if (m_textEdit->textCursor().hasSelection()) {
      dw->addEnabledOption(QAbstractPrintDialog::PrintSelection);
   }

   if (dw->exec() == QDialog::Accepted) {
      m_textEdit->print(&printer);
   }

   delete dw;
}

void MainWindow::printPreview()
{
   QPrinter printer;

   QPrintPreviewDialog preview(&printer, this);
   preview.setWindowTitle(m_curFile);

   connect(&preview, SIGNAL(paintRequested(QPrinter *)), this, SLOT(printPreview(QPrinter*)));
   preview.exec();
}

void MainWindow::printPreview(QPrinter *printer)
{
   m_textEdit->print(printer);
}

void MainWindow::printPdf()
{
   QFileDialog::Options options;

   if (false)  {  //(Q_OS_DARWIM) {
      options |= QFileDialog::DontUseNativeDialog;
   }

   QString selectedFilter;
   QString fileName = QFileDialog::getSaveFileName(this, tr("Print to PDF"),
         "document1.pdf", tr("PDF File (*.pdf)"), &selectedFilter, options);

   if (! fileName.isEmpty()) {

      if (QFileInfo(fileName).suffix().isEmpty()) {
         fileName.append(".pdf");
      }

      QPrinter printer;
      printer.setOutputFormat(QPrinter::PdfFormat);
      printer.setOutputFileName(fileName);

      m_textEdit->document()->print(&printer);
   }
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

void MainWindow::indentIncr()
{
   // * * *
   showNotDone("Edit Increase Indect");

   //QTextCursor cursor(m_textEdit->textCursor());
   //QTextBlockFormat bFormat = cursor.blockFormat();   
   //bFormat.setTextIndent(10);
   //cursor.setBlockFormat(bFormat);
   //m_textEdit->setTextCursor(cursor);
}

void MainWindow::indentDecr()
{
   // * * *
   showNotDone("Edit Decrease Indent");
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


// **search
void MainWindow::find()
{
   Dialog_Find *dw = new Dialog_Find(m_findText);
   int result = dw->exec();

   if ( result == QDialog::Accepted) {

      m_findText = dw->get_Value();
      m_flags    = 0;

      m_fDirection  = dw->get_Direction();
      if (! m_fDirection) {
         m_flags |= QTextDocument::FindBackward;
      }

      m_fCase = dw->get_Case();
      if (m_fCase) {
         m_flags |= QTextDocument::FindCaseSensitively;
      }

      m_fWholeWords = dw->get_WholeWords();
      if (m_fWholeWords){
         m_flags |= QTextDocument::FindWholeWords;
      }

      if (! m_findText.isEmpty())  {
         bool found = m_textEdit->find(m_findText, m_flags);

         if (! found)  {
            csError("Find", m_findText + " was not found");
         }
      }
   }

   delete dw;
}

void MainWindow::replace()
{
   Dialog_Replace *dw = new Dialog_Replace(m_textEdit, m_findText);
   int result = dw->exec();

   if ( result == QDialog::Accepted) {
   }

   delete dw;
}

void MainWindow::findNext()
{
   QTextDocument::FindFlags flags = QTextDocument::FindFlags(~QTextDocument::FindBackward & m_flags);
   bool found = m_textEdit->find(m_findText, flags);
   if (! found)  {
      csError("Find", m_findText + " was not found");
   }
}

void MainWindow::findPrevious()
{
   bool found = m_textEdit->find(m_findText, QTextDocument::FindBackward | m_flags );
   if (! found)  {
      csError("Find", m_findText + " was not found");
   }
}

void MainWindow::advFind()
{
   showNotDone("Search Advanced Search");
}

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
   move_lineHighlight();
}

void MainWindow::move_lineHighlight()
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
   m_textEdit->set_ShowLineNum(m_struct.showLineNumbers);
}

void MainWindow::showSpaces()
{
   showNotDone("View Show Spaces");
}

void MainWindow::showSpaces_EOL()
{
   showNotDone("View Show Spaces EOL");
}

void MainWindow::showTabs()
{   
   showNotDone("View Show Tabs");
}

void MainWindow::showLineBreaks()
{
   showNotDone("View Show Line Breaks");
}


// **document
void MainWindow::setSyn_C()
{
   setSynType(SYN_C);
   forceSyntax(SYN_C);
}

void MainWindow::setSyn_Clipper()
{
   setSynType(SYN_CLIPPER);
   forceSyntax(SYN_CLIPPER);
}

void MainWindow::setSyn_Css()
{
   setSynType(SYN_CSS);
    forceSyntax(SYN_CSS);
}

void MainWindow::setSyn_Dox()
{
   setSynType(SYN_DOX);
    forceSyntax(SYN_DOX);
}

void MainWindow::setSyn_Html()
{
   setSynType(SYN_HTML);
   forceSyntax(SYN_HTML);
}

void MainWindow::setSyn_Java()
{
   setSynType(SYN_JAVA);
   forceSyntax(SYN_JAVA);
}

void MainWindow::setSyn_Javascript()
{
   setSynType(SYN_JS);
   forceSyntax(SYN_JS);
}

void MainWindow::setSyn_Json()
{
   setSynType(SYN_JSON);
   forceSyntax(SYN_JSON);
}

void MainWindow::setSyn_Makefile()
{
   setSynType(SYN_MAKE);
   forceSyntax(SYN_MAKE);
}

void MainWindow::setSyn_Text()
{
   setSynType(SYN_TEXT);
   forceSyntax(SYN_TEXT);
}

void MainWindow::setSyn_Shell_S()
{
   setSynType(SYN_SHELL_S);
   forceSyntax(SYN_SHELL_S);
}

void MainWindow::setSyn_Perl()
{
   setSynType(SYN_PERL);
   forceSyntax(SYN_PERL);
}

void MainWindow::setSyn_Php()
{
   setSynType(SYN_PHP);
   forceSyntax(SYN_PHP);
}

void MainWindow::setSyn_Python()
{
   setSynType(SYN_PYTHON);
   forceSyntax(SYN_PYTHON);
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
   m_ui->actionSyn_Text->setChecked(false);
   m_ui->actionSyn_Shell_S->setChecked(false);
   m_ui->actionSyn_Perl->setChecked(false);
   m_ui->actionSyn_PHP->setChecked(false);
   m_ui->actionSyn_Python->setChecked(false);

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
   }
}

void MainWindow::formatDos()
{
   showNotDone("Document, format DOS (CR LF)");
}

void MainWindow::formatUnix()
{
   showNotDone("Document, format Unix (LF)");
}

void MainWindow::formatMac()
{
   showNotDone("Document, format Mac (CR)");
}

void MainWindow::fixTab_Spaces()
{
   showNotDone("Document, convert tab to spaces");
}

void MainWindow::fixSpaces_Tabs()
{
   showNotDone("Document, convert spaces to tabs");
}

void MainWindow::fixSpaces_EOL()
{
   showNotDone("Document, Remove Spaces EOL");
}



// **tools
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
      setStatusBar(tr("Macro recorded"), 1000);
   }
}

void MainWindow::macroPlay()
{
   if (m_record) {
      csError("Macro Playback", "Unable to play back a macro while recording");

   }  else {

      QList<QKeyEvent *> keyList;
      keyList = m_textEdit->get_KeyList();

      QKeyEvent *event;

      //
      int cnt = keyList.count();

      for (int k = 0; k < cnt; ++k)   {
         event = keyList.at(k);

         QKeyEvent *newEvent;
         newEvent = new QKeyEvent(*event);

         QApplication::postEvent(m_textEdit, newEvent);
      }
   }
}

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

   m_syntaxParser->set_Spell(m_struct.isSpellCheck, m_spellCheck);
   m_textEdit->set_Spell(m_struct.isSpellCheck, m_spellCheck);
}

// **settings
void MainWindow::setColors()
{
   // save the old colors
   QColor old_TextColor = m_struct.colorText;
   QColor old_BackColor = m_struct.colorBack;

   //
   Dialog_Colors *dw = new Dialog_Colors(this);
   int result = dw->exec();

   if (result = QDialog::Accepted) {

      if (m_struct.showLineHighlight)  {
         // clear the old highlight first
         QList<QTextEdit::ExtraSelection> extraSelections;
         QTextEdit::ExtraSelection selection;

         selection.format.setForeground(old_TextColor);
         selection.format.setBackground(old_BackColor);
         selection.format.setProperty(QTextFormat::FullWidthSelection, true);

         selection.cursor = m_textEdit->textCursor();
         selection.cursor.clearSelection();

         extraSelections.append(selection);
         m_textEdit->setExtraSelections(extraSelections);
      }

      // update colors in settings structure
      m_struct = dw->get_Colors();
      json_Write(COLORS);

      QPalette temp = m_textEdit->palette();
      temp.setColor(QPalette::Text, m_struct.colorText);
      temp.setColor(QPalette::Base, m_struct.colorBack);
      m_textEdit->setPalette(temp);

      //
      setSyntax();
      move_lineHighlight();
   }

   delete dw;
}

void MainWindow::setFont()
{        
   bool ok;
   m_struct.font = QFontDialog::getFont(&ok, m_textEdit->font(), this);

   if (ok) {
      m_textEdit->setFont(m_struct.font);
      json_Write(FONT);
   }
}

void MainWindow::setOptions()
{
   struct Options options;
   options.tabSpacing = m_struct.tabSpacing;
   options.formatDate = m_struct.formatDate;
   options.formatTime = m_struct.formatTime;
   options.dictMain   = m_struct.dictMain;
   options.dictUser   = m_struct.dictUser;

   Dialog_Options *dw = new Dialog_Options(this, options);
   int result = dw->exec();

   if ( result = QDialog::Accepted) {

      options = dw->get_Results();

      //
      if ( m_struct.tabSpacing != options.tabSpacing)  {
         m_struct.tabSpacing = options.tabSpacing;
         json_Write(TAB_SPACING);
      }

      //
      if ( m_struct.formatDate != options.formatDate)  {
         m_struct.formatDate = options.formatDate;
         json_Write(FORMAT_DATE);
      }

      //
      if ( m_struct.formatTime != options.formatTime)  {
         m_struct.formatTime = options.formatTime;
         json_Write(FORMAT_TIME);
      } 

      //
      if (m_struct.dictMain != options.dictMain ) {
         m_struct.dictMain = options.dictMain;
         json_Write(DICT_MAIN);
      }

      //
      if (m_struct.dictUser != options.dictUser ) {
         m_struct.dictUser = options.dictUser;
         json_Write(DICT_USER);
      }
   }

   delete dw;
}


// **window, tabs
void MainWindow::tabNew()
{
   DiamondTextEdit *textEdit;
   textEdit = new DiamondTextEdit(this);

   const QString label = "untitled.txt";
   int index = m_tabWidget->addTab(textEdit, label);

   // select new tab
   m_tabWidget->setCurrentIndex(index);

   //
   m_textEdit = textEdit;   
   m_textEdit->setFocus();

   setCurrentFile("");
   setScreenColors();

   //
   connect(m_textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));

   connect(m_textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(move_lineHighlight()));
   connect(m_textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(setStatus_LineCol()));

   connect(m_textEdit, SIGNAL(undoAvailable(bool)), m_ui->actionUndo, SLOT(setEnabled(bool)));
   connect(m_textEdit, SIGNAL(redoAvailable(bool)), m_ui->actionRedo, SLOT(setEnabled(bool)));
   connect(m_textEdit, SIGNAL(copyAvailable(bool)), m_ui->actionCut,  SLOT(setEnabled(bool)));
   connect(m_textEdit, SIGNAL(copyAvailable(bool)), m_ui->actionCopy, SLOT(setEnabled(bool)));
}

void MainWindow::tabClose()
{
   bool okClose = querySave();

   if (okClose)  {
      int count = m_tabWidget->count();

      if (count == 1) {
         // do not remove !
         m_textEdit->clear();
         setCurrentFile("");

      } else {
         // hold textEdit and delete after tab is removed
         DiamondTextEdit *hold = m_textEdit;

         int index = m_tabWidget->currentIndex();
         m_tabWidget->removeTab(index);

         delete hold;
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

      // retrieve fullName for status bar
      m_curFile = m_tabWidget->tabWhatsThis(index);

      setStatus_LineCol();
      setStatus_FName(m_curFile);

      setWindowFilePath(m_curFile);

      m_textEdit->set_ColumnMode(m_struct.isColumnMode);
      m_textEdit->set_ShowLineNum(m_struct.showLineNumbers);

      move_lineHighlight();
      setSyntax();
   }
}

// **help
void MainWindow::about()
{
   QString textBody = "<font color='#000080'><table style=margin-right:25>"
                      "<tr><td><nobr>Developed by Barbara Geller</nobr></td><td>barbara@copperspice.com</td></tr>"
                      "<tr><td style=padding-right:25><nobr>Developed by Ansel Sermersheim</nobr></td><td>ansel@copperspice.com</td></tr>"
                      "</table></font>"
                      "<br>"
                      "<p><small>Copyright 2012 BG Consulting, All rights reserved.<br>"
                      "This program is provided AS IS with no warranty of any kind.<br></small></p>";

   //
   QMessageBox msgB;
   msgB.setIcon(QMessageBox::NoIcon);
   msgB.setWindowIcon( QIcon("://resources/plus.png"));

   msgB.setWindowTitle(tr("About Diamond"));
   msgB.setText(tr("<p style=margin-right:25><center><h5>Version: 1.0<br>Build # 10.01.2012</h5></center></p>"));
   msgB.setInformativeText(textBody);

   msgB.setStandardButtons(QMessageBox::Ok);
   msgB.setDefaultButton(QMessageBox::Ok);

   msgB.exec();
}


// connections, displays, toolbar
void MainWindow::setScreenColors()
{
   m_textEdit->setFont(m_struct.font);

   QPalette temp = m_textEdit->palette();
   temp.setColor(QPalette::Text, m_struct.colorText);
   temp.setColor(QPalette::Base, m_struct.colorBack);
   m_textEdit->setPalette(temp);
}

void MainWindow::createConnections()
{
   // file
   connect(m_ui->actionNew,            SIGNAL(triggered()), this, SLOT(newFile()));
   connect(m_ui->actionOpen,           SIGNAL(triggered()), this, SLOT(open()));
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
   connect(m_ui->actionUndo,           SIGNAL(triggered()), this, SLOT(mw_undo()));
   connect(m_ui->actionRedo,           SIGNAL(triggered()), this, SLOT(mw_redo()));
   connect(m_ui->actionCut,            SIGNAL(triggered()), this, SLOT(mw_cut()));
   connect(m_ui->actionCopy,           SIGNAL(triggered()), this, SLOT(mw_copy()));
   connect(m_ui->actionPaste,          SIGNAL(triggered()), this, SLOT(mw_paste()));

   connect(m_ui->actionSelect_All,     SIGNAL(triggered()), this, SLOT(selectAll()));
   connect(m_ui->actionSelect_Block,   SIGNAL(triggered()), this, SLOT(selectBlock()));
   connect(m_ui->actionSelect_Line,    SIGNAL(triggered()), this, SLOT(selectLine()));
   connect(m_ui->actionSelect_Word,    SIGNAL(triggered()), this, SLOT(selectWord()));
   connect(m_ui->actionCase_Upper,     SIGNAL(triggered()), this, SLOT(caseUpper()));
   connect(m_ui->actionCase_Lower,     SIGNAL(triggered()), this, SLOT(caseLower()));
   connect(m_ui->actionCase_Cap,       SIGNAL(triggered()), this, SLOT(caseCap()));

   connect(m_ui->actionInsert_Date,    SIGNAL(triggered()), this, SLOT(insertDate()));
   connect(m_ui->actionInsert_Time,    SIGNAL(triggered()), this, SLOT(insertTime()));
   connect(m_ui->actionInsert_Symbol,  SIGNAL(triggered()), this, SLOT(insertSymbol()));
   connect(m_ui->actionIndent_Incr,    SIGNAL(triggered()), this, SLOT(indentIncr()));
   connect(m_ui->actionIndent_Decr,    SIGNAL(triggered()), this, SLOT(indentDecr()));
   connect(m_ui->actionColumn_Mode,    SIGNAL(triggered()), this, SLOT(columnMode()));

   // search
   connect(m_ui->actionFind,           SIGNAL(triggered()), this, SLOT(find()));
   connect(m_ui->actionReplace,        SIGNAL(triggered()), this, SLOT(replace()));
   connect(m_ui->actionFind_Next,      SIGNAL(triggered()), this, SLOT(findNext()));
   connect(m_ui->actionFind_Prev,      SIGNAL(triggered()), this, SLOT(findPrevious()));
   connect(m_ui->actionAdv_Find,       SIGNAL(triggered()), this, SLOT(advFind()));
   connect(m_ui->actionGo_Line,        SIGNAL(triggered()), this, SLOT(goLine()));
   connect(m_ui->actionGo_Column,      SIGNAL(triggered()), this, SLOT(goColumn()));
   connect(m_ui->actionGo_Top,         SIGNAL(triggered()), this, SLOT(goTop()));

   // view
   connect(m_ui->actionLine_Highlight, SIGNAL(triggered()), this, SLOT(lineHighlight()));
   connect(m_ui->actionLine_Numbers,   SIGNAL(triggered()), this, SLOT(lineNumbers()));

   connect(m_ui->actionShow_Spaces,    SIGNAL(triggered()), this, SLOT(showSpaces()));
   connect(m_ui->actionShow_Spaces_EOL,SIGNAL(triggered()), this, SLOT(showSpaces_EOL()));
   connect(m_ui->actionShow_Tabs,      SIGNAL(triggered()), this, SLOT(showTabs()));
   connect(m_ui->actionShow_Line_Break,SIGNAL(triggered()), this, SLOT(showLineBreaks()));

   // document
   connect(m_ui->actionSyn_C,          SIGNAL(triggered()), this, SLOT(setSyn_C()));
   connect(m_ui->actionSyn_Clipper,    SIGNAL(triggered()), this, SLOT(setSyn_Clipper()));
   connect(m_ui->actionSyn_Css,        SIGNAL(triggered()), this, SLOT(setSyn_Css()));
   connect(m_ui->actionSyn_Doxygen,    SIGNAL(triggered()), this, SLOT(setSyn_Dox()));
   connect(m_ui->actionSyn_Html,       SIGNAL(triggered()), this, SLOT(setSyn_Html()));
   connect(m_ui->actionSyn_Java,       SIGNAL(triggered()), this, SLOT(setSyn_Java()));   
   connect(m_ui->actionSyn_Javascript, SIGNAL(triggered()), this, SLOT(setSyn_Javascript()));
   connect(m_ui->actionSyn_Json,       SIGNAL(triggered()), this, SLOT(setSyn_Json()));
   connect(m_ui->actionSyn_Makefile,   SIGNAL(triggered()), this, SLOT(setSyn_Makefile()));
   connect(m_ui->actionSyn_Text,       SIGNAL(triggered()), this, SLOT(setSyn_Text()));
   connect(m_ui->actionSyn_Shell_S,    SIGNAL(triggered()), this, SLOT(setSyn_Shell_S()));
   connect(m_ui->actionSyn_Perl,       SIGNAL(triggered()), this, SLOT(setSyn_Perl()));
   connect(m_ui->actionSyn_PHP,        SIGNAL(triggered()), this, SLOT(setSyn_Php()));
   connect(m_ui->actionSyn_Python,     SIGNAL(triggered()), this, SLOT(setSyn_Python()));

   connect(m_ui->actionFormat_Dos,     SIGNAL(triggered()), this, SLOT(formatDos()));
   connect(m_ui->actionFormat_Unix,    SIGNAL(triggered()), this, SLOT(formatUnix()));
   connect(m_ui->actionFormat_Mac,     SIGNAL(triggered()), this, SLOT(formatMac()));

   connect(m_ui->actionFix_Tab_Spaces, SIGNAL(triggered()), this, SLOT(fixTab_Spaces()));
   connect(m_ui->actionFix_Spaces_Tab, SIGNAL(triggered()), this, SLOT(fixSpaces_Tabs()));
   connect(m_ui->actionFix_Spaces_EOL, SIGNAL(triggered()), this, SLOT(fixSpaces_EOL()));

   // tools
   connect(m_ui->actionMacro_Start,    SIGNAL(triggered()), this, SLOT(mw_macroStart()));
   connect(m_ui->actionMacro_Stop,     SIGNAL(triggered()), this, SLOT(mw_macroStop()));
   connect(m_ui->actionMacro_Play,     SIGNAL(triggered()), this, SLOT(macroPlay()));
   connect(m_ui->actionSpell_Check,    SIGNAL(triggered()), this, SLOT(spellCheck()));

   // settings
   connect(m_ui->actionColors,         SIGNAL(triggered()), this, SLOT(setColors()));
   connect(m_ui->actionFonts,          SIGNAL(triggered()), this, SLOT(setFont()));
   connect(m_ui->actionOptions,        SIGNAL(triggered()), this, SLOT(setOptions()));

   // window
   connect(m_ui->actionTab_New,        SIGNAL(triggered()), this, SLOT(tabNew()));
   connect(m_ui->actionTab_Close,      SIGNAL(triggered()), this, SLOT(tabClose()));

   // help menu
   connect(m_ui->actionAbout,          SIGNAL(triggered()), this, SLOT(about()));
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
   m_ui->actionSyn_Text->setCheckable(true);
   m_ui->actionSyn_Shell_S->setCheckable(true);
   m_ui->actionSyn_Perl->setCheckable(true);
   m_ui->actionSyn_PHP->setCheckable(true);
   m_ui->actionSyn_Python->setCheckable(true);

   m_ui->actionLine_Highlight->setCheckable(true);
   m_ui->actionLine_Highlight->setChecked(m_struct.showLineHighlight);

   m_ui->actionLine_Numbers->setCheckable(true);
   m_ui->actionLine_Numbers->setChecked(m_struct.showLineNumbers);  

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
   connect(m_tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(tabClose()));
}

void MainWindow::createShortCuts()
{
   // file
   m_ui->actionNew->setShortcuts(QKeySequence::New);
   m_ui->actionOpen->setShortcuts(QKeySequence::Open);
   m_ui->actionClose->setShortcuts(QKeySequence::Close);
   m_ui->actionSave->setShortcuts(QKeySequence::Save);
   m_ui->actionSave_As->setShortcuts(QKeySequence::SaveAs);
   m_ui->actionPrint->setShortcuts(QKeySequence::Print);
   m_ui->actionExit->setShortcuts(QKeySequence::Quit);

   // edit
   m_ui->actionUndo->setShortcuts(QKeySequence::Undo);
   m_ui->actionRedo->setShortcuts(QKeySequence::Redo);
   m_ui->actionCut->setShortcuts(QKeySequence::Cut);
   m_ui->actionCopy->setShortcuts(QKeySequence::Copy);
   m_ui->actionPaste->setShortcuts(QKeySequence::Paste);   
   m_ui->actionGo_Top->setShortcuts(QKeySequence::MoveToStartOfDocument);
   // MoveToEndOfDocument

   //search
   m_ui->actionFind->setShortcuts(QKeySequence::Find);
   m_ui->actionReplace->setShortcuts(QKeySequence::Replace);
   m_ui->actionFind_Next->setShortcuts(QKeySequence::FindNext);
   m_ui->actionFind_Prev->setShortcuts(QKeySequence::FindPrevious);

   // window
   m_ui->actionTab_New->setShortcuts(QKeySequence::AddTab);

   // help
   m_ui->actionDiamond_Help->setShortcuts(QKeySequence::HelpContents);
}

void MainWindow::createToolBars()
{
   m_ui->actionNew->setIcon(QIcon(":/resources/new.png"));
   m_ui->actionOpen->setIcon(QIcon(":/resources/open.png"));
   m_ui->actionClose->setIcon(QIcon(":/resources/close_folder.png"));
   m_ui->actionSave->setIcon(QIcon(":/resources/save.png"));
   m_ui->actionPrint->setIcon(QIcon(":/resources/print.png"));
   m_ui->actionPrint_Pdf->setIcon(QIcon(":/resources/exportpdf.png"));

   m_ui->actionUndo->setIcon(QIcon(":/resources/undo.png"));
   m_ui->actionRedo->setIcon(QIcon(":/resources/redo.png"));
   m_ui->actionCut->setIcon(QIcon(":/resources/cut.png"));
   m_ui->actionCopy->setIcon(QIcon(":/resources/copy.png"));
   m_ui->actionPaste->setIcon(QIcon(":/resources/paste.png"));

   m_ui->actionFind->setIcon(QIcon(":/resources/find.png"));

   m_ui->actionMacro_Start->setIcon(QIcon(":/resources/camera.png"));
   m_ui->actionMacro_Stop->setIcon(QIcon(":/resources/stop.png"));
   m_ui->actionMacro_Play->setIcon(QIcon(":/resources/play.png"));
   m_ui->actionSpell_Check->setIcon(QIcon(":/resources/spell.png"));

   //
   fileToolBar = addToolBar(tr("File"));
   fileToolBar->addAction(m_ui->actionNew);
   fileToolBar->addAction(m_ui->actionOpen);
   fileToolBar->addAction(m_ui->actionClose);
   // fileToolBar->addAction(m_ui->actionClose_All);
   fileToolBar->addAction(m_ui->actionSave);
   // fileToolBar->addAction(m_ui->actionSave_All);
   fileToolBar->addAction(m_ui->actionPrint);
   fileToolBar->addAction(m_ui->actionPrint_Pdf);

   editToolBar = addToolBar(tr("Edit"));
   editToolBar->addAction(m_ui->actionUndo);
   editToolBar->addAction(m_ui->actionRedo);
   editToolBar->addAction(m_ui->actionCut);
   editToolBar->addAction(m_ui->actionCopy);
   editToolBar->addAction(m_ui->actionPaste);

   searchToolBar = addToolBar(tr("Search"));
   searchToolBar->addAction(m_ui->actionFind);
   searchToolBar->addAction(m_ui->actionReplace);

   toolsToolBar = addToolBar(tr("Tools"));
   toolsToolBar->addAction(m_ui->actionMacro_Start);
   toolsToolBar->addAction(m_ui->actionMacro_Stop);
   toolsToolBar->addAction(m_ui->actionMacro_Play);
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

