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
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QUrl>

void MainWindow::closeEvent(QCloseEvent *event)
{
   bool exit = closeAll_Doc();

   if (exit) {
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

QString MainWindow::get_DirPath(QString message, QString path)
{
   QFileDialog::Options options;

   if (false)  {  //(Q_OS_DARWIM) {
      options |= QFileDialog::DontUseNativeDialog;
   }

   options |= QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks;

   // on OS X the title bar may not be displayed
   path = QFileDialog::getExistingDirectory(this, message, path, options);
   path = path.trimmed() + "/";

   return path;
}

struct Settings MainWindow::get_StructData()
{
   return m_struct;
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
   setStatusBar(tr("File loaded"), 1500);

   return true;
}

QString MainWindow::pathName(QString fileName) const
{
   return QFileInfo(fileName).path();
}

bool MainWindow::querySave()
{
   if (m_textEdit->document()->isModified()) {

      QString fileName = m_curFile;

      QMessageBox quest;
      quest.setWindowTitle(tr("Diamond Editor"));
      quest.setText( fileName + tr(" has been modified. Save changes?"));
      quest.setStandardButtons(QMessageBox::Save | QMessageBox::Discard  | QMessageBox::Cancel );
      quest.setDefaultButton(QMessageBox::Cancel);

      int retval = quest.exec();

      if (retval == QMessageBox::Save) {

         if (fileName == "untitled.txt") {
            return saveAs(false);
         } else {
            return save();
         }

      } else if (retval == QMessageBox::Cancel) {
         return false;

      }
   }
   return true;
}

bool MainWindow::saveFile(const QString &fileName, bool isSaveOne)
{
   QFile file(fileName);

   if (! file.open(QFile::WriteOnly | QFile::Text)) {      
      QString error = tr("Unable to write file %1:\n%2.").arg(fileName).arg(file.errorString());
      csError(tr("Save File"), error);
      return false;
   }

   QApplication::setOverrideCursor(Qt::WaitCursor);     
   file.write(m_textEdit->toPlainText().toUtf8());
   QApplication::restoreOverrideCursor();

   if (isSaveOne) {
      m_textEdit->document()->setModified(false);
      setWindowModified(false);
      setWindowFilePath(m_curFile);

      //
      setStatusBar(tr("File saved"), 2000);
   }

   return true;
}

QString MainWindow::strippedName(const QString fileName)
{
   return QFileInfo(fileName).fileName();
}

QString MainWindow::suffixName() const
{
   return QFileInfo(m_curFile).suffix().toLower();
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

      // change the name on the tab to "untitled.txt"
      int index = m_tabWidget->currentIndex();

      m_tabWidget->setTabText(index, showName);
      m_tabWidget->setTabWhatsThis(index, showName);

      setStatus_FName(showName);
      forceSyntax(SYN_TEXT);

   } else {
      // called when loading an existing file

      //  change the name on the tab to m_curFile
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
                         "  Col: "  + QString::number(cursor.columnNumber()+1) + "  ");
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

void MainWindow::setStatus_FName2(QString text)
{
   m_statusName->setText(" " + text + " ");
}


// drag & drop
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
   // QStringList list = event->mimeData()->formats();
   // QString type = list.join("\n");
   // csMsg("Mine Types\n" + type);

   if (event->mimeData()->hasFormat("text/uri-list"))  {
      event->acceptProposedAction();

   } else if (event->mimeData()->hasFormat("text/plain"))  {
      event->acceptProposedAction();

   }

}

void MainWindow::dropEvent(QDropEvent *event)
{
   const QMimeData *mimeData = event->mimeData();

   if (mimeData->hasUrls()) {

      QList<QUrl> urls = mimeData->urls();
      if (urls.isEmpty()) {
         return;
      }

      QString fileName = urls.first().toLocalFile();
      if (! fileName.isEmpty()) {
         loadFile(fileName, TRUE);
      }

   } else if (mimeData->hasText()) {
       m_textEdit->setPlainText(mimeData->text());

   }

}









