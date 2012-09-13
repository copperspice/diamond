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
#include "util.h"
#include "dialog_options.h"

#include <QtGui>

MainWindow::MainWindow()
   : m_ui(new Ui::MainWindow)
{
   m_ui->setupUi(this);
   setWindowFilePath("untitled.txt");

   m_textEdit = new QPlainTextEdit;
   setCentralWidget(m_textEdit);

   createShortCuts();
   createToolBars();
   createConnections();
   createToggles();

   if ( ! readCfg()  ) {
      csError("Problem in Settings");
   }

   // call after readCfg()
   rf_CreateMenus();

   setCurrentFile("");
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
   bool okClose = querySave();

   if (okClose) {
      QFileDialog::Options options;

      if (false)  {  //(Q_OS_DARWIM) {
         options |= QFileDialog::DontUseNativeDialog;
      }

      QString selectedFilter;
      QString fileName = QFileDialog::getOpenFileName(this, tr("Select File"),
            "", tr("All Files (*)"), &selectedFilter, options);

      if (! fileName.isEmpty()) {
         loadFile(fileName);
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

void MainWindow::closeAll_Doc()
{
   showNotDone("File Close All");
}

void MainWindow::reload()
{
   if (m_curFile.isEmpty()) {
      csError("Unable to reload a file which was not Saved.");

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
        loadFile(m_curFile);
      }

   } else {
      loadFile(m_curFile);

   }
}

bool MainWindow::save()
{
   if (m_curFile.isEmpty()) {
      return saveAs();

   } else {
      return saveFile(m_curFile);

   }      
}

bool MainWindow::saveAs()
{
   bool retval = false;

   QFileDialog::Options options;

   if (false)  {  //(Q_OS_DARWIM) {
      options |= QFileDialog::DontUseNativeDialog;
   }

   QString selectedFilter;
   QString fileName = QFileDialog::getSaveFileName(this, tr("Create or Select File"),
         "", tr("All Files (*)"), &selectedFilter, options);

   if (fileName.isEmpty()) {
      retval = false;
   } else {
      retval = saveFile(fileName);
   }

   return retval;
}

bool MainWindow::saveAll()
{
   showNotDone("File Save All");
}

void MainWindow::print()
{
   showNotDone("File Print");
}

void MainWindow::printPreview()
{
   showNotDone("File Print Preview");
}

void MainWindow::printSetup()
{
   showNotDone("File Print Setup");
}


// **edit
void MainWindow::insertDate()
{   
   QDate date = QDate::currentDate();
   QString temp = date.toString(m_struct.dateFormat);

   m_textEdit->insertPlainText(temp);
}

void MainWindow::columnMode()
{
   showNotDone("Edit Column Mode");
}


// **search
void MainWindow::find()
{
   showNotDone("Search Find");
}

void MainWindow::replace()
{
   showNotDone("Search Replace");
}

void MainWindow::next()
{
   showNotDone("Search Next");
}

void MainWindow::previous()
{
   showNotDone("Search Previous");
}

void MainWindow::advFind()
{
   showNotDone("Search Advanced Search");
}

void MainWindow::goLine()
{
   int line = get_Value1("line")-1;

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
   QList<QTextEdit::ExtraSelection> extraSelections;
   QTextEdit::ExtraSelection selection;

   QColor lineColor;

   if (m_ui->actionLine_Highlight->isChecked()) {
      // on
      lineColor = QColor(Qt::yellow).lighter(160);

   } else  {
      // off
      lineColor = QColor(Qt::white);      // broom - resolve

   }

   selection.format.setBackground(lineColor);
   selection.format.setProperty(QTextFormat::FullWidthSelection, true);
   selection.cursor = m_textEdit->textCursor();
   selection.cursor.clearSelection();

   extraSelections.append(selection);
   m_textEdit->setExtraSelections(extraSelections);

}

void MainWindow::lineNumbers()
{   
   if (m_ui->actionLine_Numbers->isChecked()) {
      //off
      showNotDone("View Line Numbers - OFF");


   } else {
      // on
      showNotDone("View Line Numbers - ON");

   }
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


// missing syntax ( 11 )


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
void MainWindow::macroStart()
{
   showNotDone("Tools, macro start");
}

void MainWindow::macroStop()
{
   showNotDone("Tools, macro stop");
}

void MainWindow::macroPlay()
{
   showNotDone("Tools, macro play");
}



// **settings
void MainWindow::setBackgroundColor()
{
   bool isNative = true;
   QColor color;

   if (isNative)  {
       color = QColorDialog::getColor(Qt::green, this);

   } else  {
       color = QColorDialog::getColor(Qt::green, this, "Select Color", QColorDialog::DontUseNativeDialog);

   }

   if (color.isValid()) {     
      QPalette temp = m_textEdit->palette();
      temp.setColor( QPalette::Base, color);
      m_textEdit->setPalette(temp);
   }
}

void MainWindow::setFont()
{        
   bool ok;

   m_struct.font = QFontDialog::getFont(&ok, m_textEdit->font(), this);

   if (ok) {
      m_textEdit->setFont(m_struct.font);
      writeCfg(FONT);
   }
}

void MainWindow::setOptions()
{
   Dialog_Options *dw = new Dialog_Options(this);
   int result = dw->exec();

   if ( result = QDialog::Accepted) {
      QString strTemp = dw->get_DateFormat();
      if ( m_struct.dateFormat != strTemp)  {
         m_struct.dateFormat = strTemp;
         writeCfg(DATEFORMAT);
      }

      int intTemp = dw->get_TabSpacing();
      if ( m_struct.tabSpacing != intTemp)  {
         m_struct.tabSpacing = intTemp;
         writeCfg(TAB_SPACING);
      }
   }
}


// **window
void MainWindow::tabNew()
{
   showNotDone("Window, tab new");
}

void MainWindow::tabClose()
{
   showNotDone("Window, tab close");
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


void MainWindow::X()
{
   csMsg("This feature has not been implemented.");
}


// connections, displays, toolbar
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
   connect(m_ui->actionPrint_Setup,    SIGNAL(triggered()), this, SLOT(printSetup()));

   connect(m_ui->actionExit,           SIGNAL(triggered()), this, SLOT(close()));

   // edit   
   connect(m_ui->actionUndo,           SIGNAL(triggered()), m_textEdit, SLOT(undo()));
   connect(m_ui->actionRedo,           SIGNAL(triggered()), m_textEdit, SLOT(redo()));
   connect(m_ui->actionCut,            SIGNAL(triggered()), m_textEdit, SLOT(cut()));
   connect(m_ui->actionCopy,           SIGNAL(triggered()), m_textEdit, SLOT(copy()));
   connect(m_ui->actionPaste,          SIGNAL(triggered()), m_textEdit, SLOT(paste()));

   connect(m_ui->actionInsert_Date,    SIGNAL(triggered()), this, SLOT(insertDate()));
   connect(m_ui->actionColumn_Mode,    SIGNAL(triggered()), this, SLOT(columnMode()));

   // search
   connect(m_ui->actionFind,           SIGNAL(triggered()), this, SLOT(find()));
   connect(m_ui->actionReplace,        SIGNAL(triggered()), this, SLOT(replace()));
   connect(m_ui->actionFind_Next,      SIGNAL(triggered()), this, SLOT(next()));
   connect(m_ui->actionFind_Prev,      SIGNAL(triggered()), this, SLOT(previous()));
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
   connect(m_ui->actionSyn_Default,    SIGNAL(triggered()), this, SLOT(X()));
   connect(m_ui->actionSyn_Text,       SIGNAL(triggered()), this, SLOT(X()));
   connect(m_ui->actionSyn_C,          SIGNAL(triggered()), this, SLOT(X()));
   connect(m_ui->actionSyn_Css,        SIGNAL(triggered()), this, SLOT(X()));
   connect(m_ui->actionSyn_Doxygen,    SIGNAL(triggered()), this, SLOT(X()));
   connect(m_ui->actionSyn_Html,       SIGNAL(triggered()), this, SLOT(X()));
   connect(m_ui->actionSyn_Java,       SIGNAL(triggered()), this, SLOT(X()));
   connect(m_ui->actionSyn_Javascript, SIGNAL(triggered()), this, SLOT(X()));
   connect(m_ui->actionSyn_Makefile,   SIGNAL(triggered()), this, SLOT(X()));
   connect(m_ui->actionSyn_Shell_S,    SIGNAL(triggered()), this, SLOT(X()));
   connect(m_ui->actionSyn_Perl_S,     SIGNAL(triggered()), this, SLOT(X()));

   connect(m_ui->actionFormat_Dos,     SIGNAL(triggered()), this, SLOT(formatDos()));
   connect(m_ui->actionFormat_Unix,    SIGNAL(triggered()), this, SLOT(formatUnix()));
   connect(m_ui->actionFormat_Mac,     SIGNAL(triggered()), this, SLOT(formatMac()));

   connect(m_ui->actionFix_Tab_Spaces, SIGNAL(triggered()), this, SLOT(fixTab_Spaces()));
   connect(m_ui->actionFix_Spaces_Tab, SIGNAL(triggered()), this, SLOT(fixSpaces_Tabs()));
   connect(m_ui->actionFix_Spaces_EOL, SIGNAL(triggered()), this, SLOT(fixSpaces_EOL()));

   // tools
   connect(m_ui->actionMacro_Start,    SIGNAL(triggered()), this, SLOT(macroStart()));
   connect(m_ui->actionMacro_Stop,     SIGNAL(triggered()), this, SLOT(macroStop()));
   connect(m_ui->actionMacro_Play,     SIGNAL(triggered()), this, SLOT(macroPlay()));

   connect(m_ui->actionSpell_Check,    SIGNAL(triggered()), this, SLOT(X()));

   // settings
   connect(m_ui->actionColors,         SIGNAL(triggered()), this, SLOT(setBackgroundColor()));
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
   m_ui->actionLine_Highlight->setCheckable(true);
   m_ui->actionLine_Highlight->setChecked(true);      // move to user options
   lineHighlight();

   m_ui->actionLine_Numbers->setCheckable(true);
   m_ui->actionLine_Numbers->setChecked(true);        // move to user options
   // lineNumbers();

   m_ui->actionSpell_Check->setCheckable(true);
   m_ui->actionSpell_Check->setChecked(false);

   // signals for the document
   connect(m_textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));

   //
   connect(m_textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(lineHighlight()));

   //
   m_ui->actionUndo->setEnabled(false);
   m_ui->actionRedo->setEnabled(false);
   m_ui->actionCut->setEnabled(false);
   m_ui->actionCopy->setEnabled(false);

   connect(m_textEdit, SIGNAL(undoAvailable(bool)), m_ui->actionUndo, SLOT(setEnabled(bool)));
   connect(m_textEdit, SIGNAL(redoAvailable(bool)), m_ui->actionRedo, SLOT(setEnabled(bool)));
   connect(m_textEdit, SIGNAL(copyAvailable(bool)), m_ui->actionCut,  SLOT(setEnabled(bool)));
   connect(m_textEdit, SIGNAL(copyAvailable(bool)), m_ui->actionCopy, SLOT(setEnabled(bool)));
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
   fileToolBar = addToolBar(tr("File"));
   fileToolBar->addAction(m_ui->actionNew);
   fileToolBar->addAction(m_ui->actionOpen);
   fileToolBar->addAction(m_ui->actionClose);
   fileToolBar->addAction(m_ui->actionClose_All);
   fileToolBar->addAction(m_ui->actionSave);
   fileToolBar->addAction(m_ui->actionSave_All);
   fileToolBar->addAction(m_ui->actionPrint);

   editToolBar = addToolBar(tr("Edit"));
   editToolBar->addAction(m_ui->actionUndo);
   editToolBar->addAction(m_ui->actionRedo);
   editToolBar->addAction(m_ui->actionCut);
   editToolBar->addAction(m_ui->actionCopy);
   editToolBar->addAction(m_ui->actionPaste);

   toolsToolBar = addToolBar(tr("Tools"));
   toolsToolBar->addAction(m_ui->actionSpell_Check);
}

void MainWindow::setStatusBar(QString msg, int timeOut)
{
   statusBar()->showMessage(msg, timeOut);
}

void MainWindow::showNotDone(QString item)
{
   csMsg( item + " - this feature has not been implemented.");
}


