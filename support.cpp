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
#include "dialog_get1.h"
#include "util.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QLabel>

void MainWindow::closeEvent(QCloseEvent *event)
{
   if (querySave()) {
      json_Write(CLOSE);
      event->accept();
   } else {
      event->ignore();
   }
}

void MainWindow::documentWasModified()
{    
   setWindowModified(m_textEdit->document()->isModified());
}

int MainWindow::get_Value1(const QString route)
{
   Dialog_Get1 *dw = new Dialog_Get1();

   if (route == "col") {
      dw->set_ColNo();
   }

   int result = dw->exec();
   int col = 1;

   if ( result = QDialog::Accepted) {
      col = dw->get_Value().toInt();
   }

   delete dw;

   return col;
}

QString MainWindow::suffixName() const
{
   return QFileInfo(m_curFile).suffix().toLower();
}

bool MainWindow::loadFile(const QString &fileName, bool addNewTab)
{
   QFile file(fileName);

   if (! file.open(QFile::ReadOnly | QFile::Text)) {
      QString error = tr("Unable to read file %1:\n%2.").arg(fileName).arg(file.errorString());
      csError(tr("Load File"), error);
      return false;
   }

   setStatusBar(tr("Loading File..."),0);
   QApplication::setOverrideCursor(Qt::WaitCursor);

   file.seek(0);
   QByteArray temp = file.readAll();

   if (addNewTab) {
      tabNew();

      m_struct.pathPrior = pathName(fileName);
      json_Write(PATH_PRIOR);
   }

   QString fileData = QString::fromUtf8(temp);
   m_textEdit->setPlainText(fileData);

   QApplication::restoreOverrideCursor();

   setCurrentFile(fileName);
   setStatusBar(tr("File loaded"), 2000);

   return true;
}

struct Settings MainWindow::get_StructData()
{
   return m_struct;
}

QString MainWindow::pathName(QString fileName) const
{
   return QFileInfo(fileName).path();
}

bool MainWindow::querySave()
{
   if (m_textEdit->document()->isModified()) {

      QString fileName = m_curFile;
      if (fileName.isEmpty()){
         fileName = "Untitled.txt";
      }

      QMessageBox quest;
      quest.setWindowTitle(tr("Diamond Editor"));
      quest.setText( fileName + tr(" has been modified. Save your changes?"));
      quest.setStandardButtons(QMessageBox::Save | QMessageBox::Discard  | QMessageBox::Cancel );
      quest.setDefaultButton(QMessageBox::Cancel);

      int retval = quest.exec();

      if (retval == QMessageBox::Save) {
         return save();

      } else if (retval == QMessageBox::Cancel) {
         return false;

      }
   }
   return true;
}

bool MainWindow::saveFile(const QString &fileName)
{
   QFile file(fileName);

   if (! file.open(QFile::WriteOnly | QFile::Text)) {      
      QString error = tr("Unable to write file %1:\n%2.").arg(fileName).arg(file.errorString());
      csError(tr("Save File"), error);
      return false;
   }

   QApplication::setOverrideCursor(Qt::WaitCursor);     
   file.write( m_textEdit->toPlainText().toUtf8() );
   QApplication::restoreOverrideCursor();

   setCurrentFile(fileName);

   setStatusBar(tr("File saved"), 2000);
   return true;
}

void MainWindow::setSyntax()
{
   if (m_syntaxParser) {
      delete m_syntaxParser;
      m_syntaxParser = 0;
   }

   QString fname  = "";
   QString suffix = "txt";

   if (! m_curFile.isEmpty()) {

      fname  = strippedName(m_curFile).toLower();
      suffix = suffixName();

      if (suffix == "c" || fname == "cpp" || fname == "h") {
         suffix = "cpp";

      } else if (suffix == "prg") {
         suffix = "clipper";

      } else if (suffix == "doxy") {
         suffix = "dox";

      } else if (fname == "configure" || fname == "configure.ac") {
         suffix = "make";

      } else if (suffix == "htm") {
         suffix = "html";

      } else if (fname == "makefile" || fname == "makefile.am" || fname == "makefile.in") {
         suffix = "make";

      }
   }

   QString synFName = m_struct.pathSyntax + "syn_"+ suffix + ".json";

   if (! QFile::exists(synFName)) {
      // csError(tr("Syntax Highlighting"), tr("Syntax highlighting file was not found: \n\n") + synFName  + "  ");

      // use default
      suffix = "txt";
      synFName = m_struct.pathSyntax + "syn_txt.json";
   }

   if (! QFile::exists(synFName)) {
      // csError(tr("Syntax Highlighting"), tr("Syntax highlighting file was not found: \n\n") + synFName  + "  ");
      setSynType(SYN_NONE);

   } else {
      m_syntaxParser = new Syntax{m_textEdit->document(), synFName, m_struct};

      if (suffix == "c" || suffix == "cpp" || suffix == "h")  {
         setSynType(SYN_C);

      } else if (suffix == "clipper")  {
         setSynType(SYN_CLIPPER);

      } else if (suffix == "css")  {
         setSynType(SYN_CSS);

      } else if (suffix == "dox")  {
         setSynType(SYN_DOX);

      } else if (suffix == "html")  {
         setSynType(SYN_HTML);

      } else if (suffix == "java")  {
         setSynType(SYN_JAVA);

      } else if (suffix == "js")  {
         setSynType(SYN_JS);

      } else if ( suffix == "make")  {
         setSynType(SYN_MAKE);

      } else if (suffix == "txt")  {
         setSynType(SYN_TEXT);

      } else if ( suffix == "sh")  {
         setSynType(SYN_SHELL_S);

      } else if ( suffix == "pl")  {
         setSynType(SYN_PERL_S);

      }
   }
}

void MainWindow::forceSyntax(SyntaxTypes data)
{
    QString synFName;

   switch (data)  {
      case SYN_C:
         synFName = m_struct.pathSyntax + "syn_cpp.json";
         break;

      case SYN_CLIPPER:
         synFName = m_struct.pathSyntax + "syn_clipper.json";
         break;

      case SYN_CSS:
         synFName = m_struct.pathSyntax + "syn_css.json";
         break;

      case SYN_DOX:
         synFName = m_struct.pathSyntax+ "syn_dox.json";
         break;

      case SYN_HTML:
         synFName = m_struct.pathSyntax + "syn_html.json";
         break;

      case SYN_JAVA:
         synFName = m_struct.pathSyntax + "syn_java.json";
         break;

      case SYN_JS:
         synFName = m_struct.pathSyntax + "syn_js.json";
         break;

      case SYN_MAKE:
        synFName = m_struct.pathSyntax + "syn_make.json";
         break;

      case SYN_TEXT:
         synFName = m_struct.pathSyntax + "syn_txt.json";
         break;

      case SYN_SHELL_S:
         synFName = m_struct.pathSyntax + "syn_sh.json";
         break;

      case SYN_PERL_S:
         synFName = m_struct.pathSyntax + "syn_pl.json";
         break;
   }

   if (! QFile::exists(synFName)) {
      csError(tr("Syntax Highlighting"), tr("Syntax highlighting file was not found: \n\n") + synFName  + "  ");

   } else {
      m_syntaxParser = new Syntax{m_textEdit->document(), synFName, m_struct};

   }
}

QString MainWindow::strippedName(const QString fileName)
{
   return QFileInfo(fileName).fileName();
}


// title & status bar
void MainWindow::setCurrentFile(const QString &fileName)
{
   m_curFile = fileName;
   m_textEdit->document()->setModified(false);   

   setWindowModified(false);

   //
   QString showName = m_curFile;

   if (m_curFile.isEmpty()) {
      showName = "untitled.txt";

      // change the tab name
      int index = m_tabWidget->currentIndex();
      m_tabWidget->setTabText(index, showName);
      m_tabWidget->setTabWhatsThis(index, showName);

      setStatus_FName(showName);
      setSyntax();

   } else {
      // change the tab name
      int index = m_tabWidget->currentIndex();
      m_tabWidget->setTabText(index, strippedName(m_curFile) );
      m_tabWidget->setTabWhatsThis(index, m_curFile);

      setStatus_FName(m_curFile);
      setSyntax();

      bool found = true;
      found = m_rf_List.contains(m_curFile);

      if (! found) {
         rf_Update();
      }
   }

   setWindowFilePath(showName);
}

void MainWindow::setStatus_LineCol()
{
   QTextCursor cursor(m_textEdit->textCursor());   
   m_statusLine->setText(" Line: "  + QString::number(cursor.blockNumber()+1) +
                         "  Col: " + QString::number(cursor.columnNumber()+1) + "  ");
}

void MainWindow::setStatus_ColMode()
{
   if (m_struct.isColumnMode) {
      m_statusMode->setText(" Column Mode  ");    

   } else {
      m_statusMode->setText(" Line Mode  ");      
   }

   m_textEdit->set_ColumnMode(m_struct.isColumnMode);
}

void MainWindow::setStatus_FName(QString fullName)
{
   m_statusName->setText(" " + fullName + "  ");
}






