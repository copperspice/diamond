/**************************************************************************
*
* Copyright (c) 2012-2024 Barbara Geller
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

#include "util.h"

#include <QApplication>
#include <QMessageBox>
#include <QString>

void csMsg(const QString &msg)
{
   QMessageBox msgB;
   msgB.setTextFormat(Qt::PlainText);

   msgB.setWindowTitle(QApplication::translate("csMsg", "General Information"));
   msgB.setWindowIcon(QIcon("://resources/diamond.png"));

   msgB.setText(msg);
   msgB.exec();
}

void csMsg(QString msg, int value)
{
   msg = msg + "   " + QString::number(value);

   QMessageBox msgB;

   msgB.setWindowTitle(QApplication::translate("csMsg", "General Information"));
   msgB.setWindowIcon(QIcon("://resources/diamond.png"));

   msgB.setText(msg);
   msgB.exec();
}

void csMsg(QWidget *parent, const QString &title, const QString &msg)
{
   QMessageBox msgB(parent);

   if (parent != nullptr) {
      msgB.setWindowModality(Qt::WindowModal);
   }

   msgB.setWindowTitle(title);
   msgB.setWindowIcon(QIcon("://resources/diamond.png"));

   msgB.setText(msg);
   msgB.exec();
}

void csError(QString title, QString msg)
{
   QMessageBox msgB;

   msgB.setWindowTitle(title);
   msgB.setWindowIcon(QIcon("://resources/diamond.png"));

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
