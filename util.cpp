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

#include "util.h"

#include <QApplication>
#include <QDialog>

void csMsg(const QString &msg)
{
   QMessageBox msgB;

   msgB.setWindowTitle("Debug Information");
   msgB.setText(msg);
   msgB.exec();
}

void csMsg(QString msg, int value)
{
   msg = msg + "   " + QString::number(value);

   //
   QMessageBox msgB;

   msgB.setWindowTitle("Debug Information");
   msgB.setText(msg);
   msgB.exec();
}

void csMsg(QWidget *parent, const QString &title, const QString &msg)
{
   QMessageBox msgB;

   if (parent) {
      msgB.setWindowModality(Qt::WindowModal);
   }

   msgB.setWindowTitle(title);
   msgB.setText(msg);
   msgB.exec();
}

void csError(QString title, QString msg)
{
   QMessageBox msgB;

   msgB.setWindowTitle(title);
   msgB.setIcon(QMessageBox::Warning);
   msgB.setText(msg);
   msgB.exec();
}

void showDialog(QDialog &t_Dialog)
{
   t_Dialog.show();
   t_Dialog.raise();
   t_Dialog.activateWindow();

   QApplication::processEvents();
}


//   printf("%s\n",  qPrintable(string) );

