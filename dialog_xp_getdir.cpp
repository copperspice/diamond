/**************************************************************************
*
* Copyright (c) 2012-2015 Barbara Geller
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

#include <QAbstractItemView>
#include <QDir>
#include <QFileInfoList>

#ifdef Q_OS_WIN
#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <lm.h>
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

   sizePolicy1.setHorizontalStretch(4);
   sizePolicy1.setVerticalStretch(0);
   sizePolicy1.setHeightForWidth(m_ui->drives_TV->sizePolicy().hasHeightForWidth());
   m_ui->drives_TV->setSizePolicy(sizePolicy1);

   QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);

   sizePolicy2.setHorizontalStretch(5);
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
   m_ui->drives_TV->setHeaderLabels(QStringList() << "Drive"  << "Drive Type");
   m_ui->drives_TV->setColumnCount(2);
   m_ui->drives_TV->setColumnWidth(15, 50);

   QString data;
   QTreeWidgetItem *item;

   for (auto k = driveList.begin(); k != driveList.end(); ++k) {
      data = k->path();

      if (data.endsWith("/")) {
         data = data.left(2);
      }

      //
      QString other = driveType(data);

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

   connect(m_ui->network_PB, SIGNAL(clicked()),this, SLOT(network()));
   connect(m_ui->ok_PB,      SIGNAL(clicked()),this, SLOT(ok()));
   connect(m_ui->cancel_PB,  SIGNAL(clicked()),this, SLOT(cancel()));

   // force call to sizeHint()
   adjustSize();

   // update right treeview
   connect(m_model_R, &QFileSystemModel::directoryLoaded, this, &Dialog_XP_GetDir::showMe);

   m_ui->drives_TV->setFocus();
}

Dialog_XP_GetDir::~Dialog_XP_GetDir()
{
   delete m_ui;
}

void Dialog_XP_GetDir::cancel()
{
   this->done(QDialog::Rejected);
}

QString Dialog_XP_GetDir::driveType(QString drive)
{
   drive = drive + "\\";

#ifdef Q_OS_WIN
   // windows api call, pass drive value as UNICODE
   int driveType = GetDriveType(drive.toStdWString().c_str());

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
   QModelIndex index = m_ui->folders_TV->currentIndex();     
   QString retval = m_model_R->filePath(index);

   return retval;
}

void Dialog_XP_GetDir::ok()
{
   this->done(QDialog::Accepted);
}

void Dialog_XP_GetDir::network()
{
   QString data;
   QTreeWidgetItem *item;

   // add network servers to left treeview
   m_netServers = getWin_NetServers();

// QFileSystemModel finds the netShares, this method is not required
// m_netShares  = getWin_NetShares();

   for (auto k = m_netServers.begin(); k != m_netServers.end(); ++k) {
      data = k->serverName;

      QString other = "Network Share";

/*    if (k->isAvailable) {
         other = "Network Share";
      } else  {
         other = "UnAvailable";
      }
*/
      item = new QTreeWidgetItem(m_ui->drives_TV);
      item->setText(0, data);
      item->setText(1, other);
   }

   m_ui->network_PB->setDisabled(true);
}

void Dialog_XP_GetDir::showDirectories(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
   if (! current) {
      return;
   }

   QString drive;

   if (current->text(1) == "Network Share")  {

      drive = "//" + current->text(0).toLower() + "/";

      // 1
      m_model_R = new QFileSystemModel;
      m_model_R->setFilter(QDir::Drives | QDir::Dirs | QDir::NoDotAndDotDot);
      m_model_R->setRootPath(drive);

      // will hide printer folders
      QStringList filters;
      filters << "*[^$]";

      m_model_R->setNameFilters(filters);
      m_model_R->setNameFilterDisables(false);

   } else if (current->text(1) == "UnAvailable")  {
      // show an empty list

      drive = "//" + current->text(0).toLower() + "/";

      // 1
      m_model_R = new QFileSystemModel;
      m_model_R->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
      m_model_R->setRootPath(drive);

      // hide everthing for now
      QStringList filters;
      filters << "(?!E)E";

      m_model_R->setNameFilters(filters);
      m_model_R->setNameFilterDisables(false);

   } else {
      drive = current->text(0) + "/";

      // 1
      m_model_R = new QFileSystemModel;
      m_model_R->setFilter(QDir::Drives | QDir::Dirs | QDir::NoDotAndDotDot);
      m_model_R->setRootPath(drive);
   }

   // 2
   m_ui->folders_TV->setModel(m_model_R);

   // 3
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
   return QSize(625,600);
}

QList<netServers> Dialog_XP_GetDir::getWin_NetServers()
{
   QList<netServers> retval;

#ifdef Q_OS_WIN
   // local comptuer
   QString nameLocal;

   wchar_t nameBuffer[4096];
   long unsigned int  nameSize = 4096;

   if (GetComputerName(nameBuffer, &nameSize)) {
      nameLocal = QString::fromStdWString(nameBuffer);
   }

   // struct SERVER_INFO_100 declared in lm.h
   using Server100 = SERVER_INFO_100;

   Server100 *buffer;

   wchar_t *serverName           = NULL;
   long unsigned int maxLen      = MAX_PREFERRED_LENGTH;
   long unsigned int entriesRead = 0;
   long unsigned int count       = 0;
   long unsigned int serverType  = SV_TYPE_SERVER;        // all servers
   wchar_t *domainName           = NULL;
   long unsigned int resume      = 0;

   NET_API_STATUS nStatus;

   nStatus = NetServerEnum(serverName, 100, (LPBYTE *) &buffer, maxLen, &entriesRead, &count, serverType, domainName, &resume);

   if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA)) {

      if (buffer != NULL) {

         for (uint32_t k = 0; k < entriesRead; k++)  {
            QString temp = QString::fromStdWString(buffer[k].sv100_name);

            if (temp != nameLocal ) {
               retval.append( netServers{temp, true} );
            }
         }
      }
   }

#endif

   return retval;
}

/*

// QFileSystemModel finds the netShares, this method is not required
QList<netShares> Dialog_XP_GetDir::getWin_NetShares()
{  
   QList<netShares> retval;

   // struct PSHARE_INFO_502 declared in lm.h
   using Share502 = SHARE_INFO_502;

   Share502 *buffer;

   wchar_t *server;
   long unsigned int result      = ERROR_MORE_DATA;
   long unsigned int entriesRead = 0;
   long unsigned int count       = 0;
   long unsigned int resume      = 0;

   std::vector<wchar_t> temp;

   for (auto k = m_netServers.begin(); k != m_netServers.end(); ++k) {

      temp.resize(k->serverName.size()+1);
      k->serverName.toWCharArray(&temp[0]);
      temp[k->serverName.size()] = 0;

      server = &temp[0];
      QString serverName = QString::fromStdWString(server).toLower();

      while (result == ERROR_MORE_DATA) {
         buffer = 0;
         result = NetShareEnum(server, 502, (LPBYTE *) &buffer, MAX_PREFERRED_LENGTH, &entriesRead, &count, &resume);

         if (result == ERROR_SUCCESS || result == ERROR_MORE_DATA)  {

            for (uint32_t i = 0; i < entriesRead; i++)  {
               QString shareName = QString::fromStdWString(buffer[i].shi502_netname);

               if (! shareName.endsWith('$') ) {
                  retval.append(netShares{serverName, shareName});
               }
            }

            NetApiBufferFree(buffer);

         } else {
            // mark m_setServer as unavailable
            k->isAvailable = false;

            NetApiBufferFree(buffer);
            break;
         }                        
      }

      // must reset
      result      = ERROR_MORE_DATA;
      entriesRead = 0;
      count       = 0;
      resume      = 0;
   }

   return retval;
}

*/
