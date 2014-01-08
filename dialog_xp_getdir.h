/**************************************************************************
*
* Copyright (c) 2012-2014 Barbara Geller
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

#ifndef DIALOG_XP_GETDIR_H
#define DIALOG_XP_GETDIR_H

#include "ui_dialog_xp_getdir.h"
#include "mainwindow.h"

#include <QFileDialog>
#include <QFileSystemModel>
#include <QTreeWidgetItem>

class Dialog_XP_GetDir  : public QDialog
{
   Q_OBJECT

   public:
      Dialog_XP_GetDir(MainWindow *from, const QString title, const QString path, QFileDialog::Options options);
      ~Dialog_XP_GetDir();

       QString getDirectory();

   private:
      Ui::Dialog_XP_GetDir *m_ui;
      MainWindow *m_parent;
      QFileSystemModel *m_model_R;

      QString getDriveType(QString drive);

   private slots:
      void showDirectories(QTreeWidgetItem *current, QTreeWidgetItem *previous);
      void Ok();
      void Cancel();
};

#endif
