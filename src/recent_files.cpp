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

#include "mainwindow.h"
#include "util.h"

#include <QCursor>
#include <QStringList>
#include <QVariant>

// ****  recent files
void MainWindow::rf_CreateMenus()
{
   int cnt = m_rf_List.count();

   QString tName;

   QMenu   *fileMenu = m_ui->menuFile;
   QAction *action   = fileMenu->insertSeparator(m_ui->actionExit);

   for (int i = 0; i < RECENT_FILES_MAX; ++i) {

      if (i < cnt)  {
         tName = m_rf_List[i];
      } else {
         tName = "file" + QString::number(i);
      }

      rf_Actions[i] = new QAction(tName, this);
      rf_Actions[i]->setData(QString("recent-file"));

      fileMenu->insertAction(action, rf_Actions[i]);

      if (i >= cnt)  {
         rf_Actions[i]->setVisible(false);
      }

      connect(rf_Actions[i], &QAction::triggered, this, &MainWindow::rf_Open);
   }
}

void MainWindow::rf_Open()
{
   QAction *action;
   action = (QAction *)sender();

   if (action) {
      bool ok = loadFile(action->text(), true, false);

      if (! ok) {
         // remove file from list since it did not load
         int index = m_rf_List.indexOf(action->text());

         if (index >= 0) {
            m_rf_List.removeAt(index);

            // save new list of files
            json_Write(RECENTFILE);

            // update actions
            rf_UpdateActions();
         }
      }
   }
}

void MainWindow::showContext_Files(const QPoint &pt)
{
   QAction *action = m_ui->menuFile->actionAt(pt);

   if (action)  {
      QString actionData = action->data().toString();

      if (actionData == "recent-file")  {
         QString fName = action->text();

         QMenu *menu = new QMenu(this);
         menu->addAction("Clear Recent file list", this, SLOT(rf_ClearList()) );

         QAction *rfAction = menu->addAction("Remove file:  " + fName, this, SLOT(rf_RemoveFName() ));
         rfAction->setData(fName);

         menu->exec(m_ui->menuFile->mapToGlobal(pt));
         delete menu;
      }
   }
}

void MainWindow::rf_ClearList()
{
   QAction *action;
   action = (QAction *)sender();

   if (action) {
      m_rf_List.clear();

      // save new list of files
      json_Write(RECENTFILE);

      // update actions
      rf_UpdateActions();
   }
}

void MainWindow::rf_RemoveFName()
{
   QAction *action;
   action = (QAction *)sender();

   if (action) {
      QString fName = action->data().toString();

      int index = m_rf_List.indexOf(fName);

      if (index >= 0) {
         m_rf_List.removeAt(index);

         // save new list of files
         json_Write(RECENTFILE);

         // update actions
         rf_UpdateActions();
      }
   }
}

void MainWindow::rf_Update()
{
   int cnt = m_rf_List.count();

   if (cnt >= RECENT_FILES_MAX) {
      m_rf_List.removeFirst();
   }

   m_rf_List.append(m_curFile);

   // save new list of files
   json_Write(RECENTFILE);

   // update actions
   rf_UpdateActions();
}

void MainWindow::rf_UpdateActions()
{
   int cnt = m_rf_List.count();

   for (int i = 0; i < RECENT_FILES_MAX; ++i) {

     if (i < cnt)  {
        rf_Actions[i]->setText(m_rf_List[i]);
        rf_Actions[i]->setVisible(true);

     } else {
        rf_Actions[i]->setVisible(false);
     }

   }
}

// ****  recent folders
void MainWindow::rfolder_CreateMenus()
{
   int cnt = m_rfolder_List.count();

   QString tName;
   QMenu *menu = new QMenu(this);

   for (int i = 0; i < RECENT_FOLDERS_MAX; ++i) {

      if (i < cnt)  {
         tName = m_rfolder_List[i];
      } else {
         tName = "folder"+QString::number(i);
      }

      rfolder_Actions[i] = new QAction(tName, this);
      rfolder_Actions[i]->setData(QString("recent-folder"));

      menu->addAction(rfolder_Actions[i]);

      if (i >= cnt)  {
         rfolder_Actions[i]->setVisible(false);
      }

      connect(rfolder_Actions[i], &QAction::triggered, this, &MainWindow::rfolder_Open);
   }

   m_ui->actionOpen_RecentFolder->setMenu(menu);

}

void MainWindow::rfolder_Open()
{
   QAction *action;
   action = (QAction *)sender();

   if (action) {
      // pass the path
      openDoc(action->text());
   }
}

void MainWindow::showContext_RecentFolder(const QPoint &pt)
{
   QAction *action = m_ui->actionOpen_RecentFolder->menu()->actionAt(pt);

   if (action)  {
      QString actionData = action->data().toString();

      if (actionData == "recent-folder")  {
         QString fName = action->text();

         QMenu *menu = new QMenu(this);
         menu->addAction("Clear Recent folder list",  this, SLOT(rfolder_ClearList()) );

         QAction *rfAction = menu->addAction("Remove folder:  " + fName, this,  SLOT(rfolder_RemoveFName() ));
         rfAction->setData(fName);

         menu->exec(QCursor::pos());
         delete menu;
      }
   }
}

void MainWindow::rfolder_ClearList()
{
   QAction *action;
   action = (QAction *)sender();

   if (action) {
      m_rfolder_List.clear();

      // save new list
      json_Write(RECENTFOLDER);

      // update actions
      rfolder_UpdateActions();
   }
}

void MainWindow::rfolder_RemoveFName()
{
   QAction *action;
   action = (QAction *)sender();

   if (action) {
      QString fName = action->data().toString();

      int index = m_rfolder_List.indexOf(fName);

      if (index >= 0) {
         m_rfolder_List.removeAt(index);

         // save new list
         json_Write(RECENTFOLDER);

         // update actions
         rfolder_UpdateActions();
      }
   }
}

void MainWindow::rfolder_Add()
{
   if (m_curFile.isEmpty()) {
      return;
   }

   int cnt = m_rfolder_List.count();

   if (cnt >= RECENT_FOLDERS_MAX) {
      m_rfolder_List.removeFirst();
   }

   QString fileName = pathName(m_curFile);

   if (! m_rfolder_List.contains(fileName)) {
      m_rfolder_List.append(fileName);
   }

   // save new list
   json_Write(RECENTFOLDER);

   // update actions
   rfolder_UpdateActions();
}

void MainWindow::rfolder_UpdateActions()
{
   int cnt = m_rfolder_List.count();

   for (int i = 0; i < RECENT_FOLDERS_MAX; ++i) {

     if (i < cnt)  {
        rfolder_Actions[i]->setText(m_rfolder_List[i]);
        rfolder_Actions[i]->setVisible(true);

     } else {
        rfolder_Actions[i]->setVisible(false);
     }

   }
}



// ****  preset folders
void MainWindow::prefolder_CreateMenus()
{
   QString tName;
   QMenu *menu = new QMenu(this);

   for (int i = 0; i < PRESET_FOLDERS_MAX; ++i) {

      tName = m_prefolder_List[i];

      if (tName.isEmpty())  {
         continue;
      }

      prefolder_Actions[i] = new QAction(tName, this);
      prefolder_Actions[i]->setData(QString("preset-folder"));

      menu->addAction(prefolder_Actions[i]);
      connect(prefolder_Actions[i], &QAction::triggered, this, &MainWindow::prefolder_Open);
   }

   m_ui->actionOpen_PresetFolder->setMenu(menu);

}

void MainWindow::prefolder_Open()
{
   QAction *action;
   action = (QAction *)sender();

   if (action) {
      // pass the path
      openDoc(action->text());
   }
}

void MainWindow::prefolder_RedoList()
{
   QMenu *menu = m_ui->actionOpen_PresetFolder->menu();
   menu->deleteLater();

   prefolder_CreateMenus();
}

