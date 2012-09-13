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

void MainWindow::closeEvent(QCloseEvent *event)
{
   if (querySave()) {
      writeCfg(CLOSE);
      event->accept();
   } else {
      event->ignore();
   }
}

void MainWindow::documentWasModified()
{    
   setWindowModified(m_textEdit->document()->isModified());
}

QString MainWindow::strippedName(const QString &fullFileName)
{
   return QFileInfo(fullFileName).fileName();
}

int MainWindow::getLineNo()
{
   Dialog_Get1 *dw = new Dialog_Get1();
   dw->exec();

   int line = dw->get_Value().toInt();

   return line;
}

void MainWindow::loadFile(const QString &fileName)
{
   QFile file(fileName);

   if (! file.open(QFile::ReadOnly | QFile::Text)) {
      QMessageBox::warning(this, tr("Load File"),
            tr("Unable to read file %1:\n%2.").arg(fileName).arg(file.errorString()));
      return;
   }

   QApplication::setOverrideCursor(Qt::WaitCursor);

   file.seek(0);
   QByteArray temp = file.readAll();

   QString fileData = QString::fromUtf8(temp);
   m_textEdit->setPlainText(fileData);

   QApplication::restoreOverrideCursor();

   setCurrentFile(fileName);
   setStatusBar(tr("File loaded"), 2000);
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
      QMessageBox::warning(this, tr("Save File"),
            tr("Unable to write file %1:\n%2.").arg(fileName).arg(file.errorString()));
      return false;
   }

   QApplication::setOverrideCursor(Qt::WaitCursor);     
   file.write( m_textEdit->toPlainText().toUtf8() );
   QApplication::restoreOverrideCursor();

   setCurrentFile(fileName);
   setStatusBar(tr("File saved"), 2000);
   return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
   m_curFile = fileName;
   m_textEdit->document()->setModified(false);

   setWindowModified(false);

   //
   QString shownName = m_curFile;

   if (m_curFile.isEmpty()) {
      shownName = "untitled.txt";

   } else {

      bool found = true;
      found = rf_List.contains(m_curFile);

      if (! found) {
         rf_Update();
      }
   }

   setWindowFilePath(shownName);
}


/*
struct MainWindow::getStructName()
{
   return this->m_struct;
}
*/


