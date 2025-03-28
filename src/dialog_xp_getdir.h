/**************************************************************************
*
* Copyright (c) 2012-2025 Barbara Geller
*
* Diamond Editor is free software. You can redistribute it and/or
* modify it under the terms of the GNU General Public License
* version 2 as published by the Free Software Foundation.
*
* Diamond Editor is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* https://www.gnu.org/licenses/
*
***************************************************************************/

#ifndef DIALOG_XP_GETDIR_H
#define DIALOG_XP_GETDIR_H

#include "mainwindow.h"
#include "ui_dialog_xp_getdir.h"

#include <QFileDialog>
#include <QFileSystemModel>
#include <QTreeWidgetItem>

struct netServers {
   QString serverName;
   bool isAvailable;
};

struct netShares {
   QString serverName;
   QString shareName;
};

class Dialog_XP_GetDir  : public QDialog
{
   CS_OBJECT(Dialog_XP_GetDir)

   public:
      Dialog_XP_GetDir(MainWindow *from, const QString title, const QString absolutePath,
            QFileDialog::FileDialogOptions options);

      ~Dialog_XP_GetDir();

       QString getDirectory();

       void showMe();

   protected:
      QSize sizeHint() const override;

   private:
      QString driveType(QString drive);
      QList<netServers> getWin_NetServers();

      void showDirectories(QTreeWidgetItem *current, QTreeWidgetItem *previous);
      void network();
      void ok();
      void cancel();

      Ui::Dialog_XP_GetDir *m_ui;
      QFileSystemModel *m_model_R;

      QString m_path;
      QModelIndex m_index_R;

      QList<netServers> m_netServers;
      QList<netShares>  m_netShares;
};

#endif
