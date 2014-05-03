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

#include "dialog_xp_getdir.h"
#include "mainwindow.h"
#include "util.h"

#include <QDir>
#include <QFileInfoList>
#include <QAbstractItemView>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

Dialog_XP_GetDir::Dialog_XP_GetDir(MainWindow *from, const QString title, const QString path, QFileDialog::Options options)
   : QDialog(from), m_ui(new Ui::Dialog_XP_GetDir)
{
   m_path = path;

   m_ui->setupUi(this);
   this->setWindowIcon(QIcon("://resources/diamond.png"));
   setWindowTitle(title);

   // set the width of the panes
   QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);

   sizePolicy1.setHorizontalStretch(2);
   sizePolicy1.setVerticalStretch(0);
   sizePolicy1.setHeightForWidth(m_ui->drives_TV->sizePolicy().hasHeightForWidth());
   m_ui->drives_TV->setSizePolicy(sizePolicy1);

   QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);

   sizePolicy2.setHorizontalStretch(3);
   sizePolicy2.setVerticalStretch(0);
   sizePolicy2.setHeightForWidth(m_ui->folders_TV->sizePolicy().hasHeightForWidth());
   m_ui->folders_TV->setSizePolicy(sizePolicy2);

   // get drive list and starting drive
   QFileInfoList driveList = QDir::drives();

   QString drive_L = m_path;
   if (drive_L.isEmpty())  {
      drive_L = QCoreApplication::applicationDirPath();
   }

   drive_L = drive_L.left(3);

   // set up drive tree view - QList<QFileInfo>
   m_ui->drives_TV->setHeaderHidden(true);
   m_ui->drives_TV->setColumnCount(2);
   m_ui->drives_TV->setColumnWidth(0, 50);

   QString data;
   QTreeWidgetItem *item;

   for (auto k = driveList.begin(); k != driveList.end(); ++k) {
      data = k->path();

      if (data.endsWith("/")) {
         data = data.left(2);
      }

      //
      QString other = "   " + getDriveType(data);

      item = new QTreeWidgetItem(m_ui->drives_TV);
      item->setText(0, data);
      item->setText(1, other);

      if (drive_L == data + "/") {
         m_ui->drives_TV->setCurrentItem(item);
      }
   }

   // set up tree view right
   m_model_R = new QFileSystemModel;
   m_model_R->setFilter(QDir::Drives | QDir::Dirs | QDir::NoDotAndDotDot);
   m_model_R->setRootPath(drive_L);

   //
   m_ui->folders_TV->setModel(m_model_R);
   m_ui->folders_TV->setHeaderHidden(true);

   QModelIndex index = m_model_R->index(drive_L);
   m_ui->folders_TV->setRootIndex(index);

   for(int nCount = 1; nCount < m_model_R->columnCount(); nCount++) {
      m_ui->folders_TV->hideColumn(nCount);
   }

   // search for the current path
   m_index_R = m_model_R->index(m_path);
   m_ui->folders_TV->setCurrentIndex(m_index_R);

   // tree view triggers
   connect(m_ui->drives_TV, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
           this, SLOT(showDirectories(QTreeWidgetItem *, QTreeWidgetItem *))); 

   connect(m_ui->ok_PB,     SIGNAL(clicked()),this, SLOT(Ok()));
   connect(m_ui->cancel_PB, SIGNAL(clicked()),this, SLOT(Cancel()));

   // force call to sizeHint()
   adjustSize();

   connect(m_model_R, &QFileSystemModel::directoryLoaded, this, &Dialog_XP_GetDir::showMe);
}

Dialog_XP_GetDir::~Dialog_XP_GetDir()
{
   delete m_ui;
}

void Dialog_XP_GetDir::Cancel()
{
   this->done(QDialog::Rejected);
}

QString Dialog_XP_GetDir::getDriveType(QString drive)
{
   drive = drive + "\\";

#ifdef Q_OS_WIN
   int driveType = GetDriveType(drive.toLatin1().constData());

#else
   int driveType = 0;

#endif

   switch (driveType)    {

      case 0:
      case 1:
         return "Unknown";

      case 2:
         return "Removable Drive";

      case 3:
         return "Local Drive";

      case 4:
         return "Network Drive";

      case 5:
         return "CD Drive";

      case 6:
         return "Ram Disk";
   }

   return "";
}

QString Dialog_XP_GetDir::getDirectory()
{
   QString retval;

   QModelIndex index = m_ui->folders_TV->currentIndex();
   retval = index.data().toString();

   while (true) {
      QModelIndex indexParent = index.parent();

      if (indexParent.isValid()) {
         retval = indexParent.data().toString() + "/" + retval;
         index  = indexParent;

      } else {
         break;

      }
   }

   return retval;
}

void Dialog_XP_GetDir::Ok()
{
   this->done(QDialog::Accepted);
}

void Dialog_XP_GetDir::showDirectories(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
   if (! current) {
      return;
   }

   m_ui->drives_TV->setCurrentItem(current);
   QString drive = current->text(0) + "/";

   // 1
   m_model_R = new QFileSystemModel;
   m_model_R->setFilter(QDir::Drives | QDir::Dirs | QDir::NoDotAndDotDot);
   m_model_R->setRootPath(drive);

   // 2
   m_ui->folders_TV->setModel(m_model_R);

   QModelIndex index = m_model_R->index(drive);
   m_ui->folders_TV->setRootIndex(index);
}

void Dialog_XP_GetDir::showMe(const QString &path)
{
   //  m_index_R = m_model_R->index(m_path);
   //  m_ui->folders_TV->setCurrentIndex(m_index_R);

   // needs additional functionality in CS
   m_ui->folders_TV->scrollTo(m_index_R, QAbstractItemView::PositionAtCenter);
}

QSize Dialog_XP_GetDir::sizeHint() const
{ 
   return QSize(500,600);
}

