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

#ifndef UTIL_H
#define UTIL_H

#include <QMessageBox>
#include <QString>

void csMsg(const QString &msg);
void csMsg(QString msg, int value);
void csMsg(const QString &ms1, const QString &msg2, QWidget *parent = nullptr);

void csError(QString title, QString msg);
void showDialog(QDialog &t_Dialog);

void showHtml(QString route, QString file);

#endif
