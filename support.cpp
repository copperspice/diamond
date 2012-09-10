#include "mainwindow.h"
#include "util.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

// support methods

void MainWindow::closeEvent(QCloseEvent *event)
{
   if (maybeSave()) {
      writeSettings();
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

void MainWindow::loadFile(const QString &fileName)
{
   QFile file(fileName);
   if (!file.open(QFile::ReadOnly | QFile::Text)) {
      QMessageBox::warning(this, tr("Load Document"),
            tr("Unable to read file %1:\n%2.").arg(fileName).arg(file.errorString()));
      return;
   }

   QTextStream in(&file);

   QApplication::setOverrideCursor(Qt::WaitCursor);
   m_textEdit->setPlainText(in.readAll());
   QApplication::restoreOverrideCursor();

   setCurrentFile(fileName);
   setStatusBar(tr("File loaded"), 2000);
}

bool MainWindow::maybeSave()
{
   if (m_textEdit->document()->isModified()) {
      QMessageBox::StandardButton retval;
      retval = QMessageBox::warning(this, tr("Save Document"),
                   tr("This document has been modified.\n" "Do you want to save your changes?"),
                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

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
      QMessageBox::warning(this, tr("Save Document"),
            tr("Unable to write file %1:\n%2.").arg(fileName).arg(file.errorString()));
      return false;
   }

   QTextStream out(&file);

   QApplication::setOverrideCursor(Qt::WaitCursor);
   out << m_textEdit->toPlainText();
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

   QString shownName = m_curFile;
   if (m_curFile.isEmpty()) {
      shownName = "untitled.txt";
   }

   setWindowFilePath(shownName);
}


