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
#include "util.h"

void MainWindow::rf_Open()
{
   QAction *action;
   action = (QAction *)sender();

   if (action) {
      bool ok = loadFile(action->text(), true);

      if (! ok) {
         // remove file which did not load

         int index;
         index = m_rf_List.indexOf(action->text());

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

void MainWindow::rf_CreateMenus()
{
   int cnt = m_rf_List.count();

   if (cnt > 0)  {

      QString tName;

      QMenu   *fileMenu = m_ui->menuFile;
      QAction *action = fileMenu->insertSeparator(m_ui->actionExit);

      for (int i = 0; i < rf_MaxCnt; ++i) {

         if (i < cnt)  {
            tName = m_rf_List[i];
         } else {
            tName = "file"+QString::number(i);
         }

         rf_Actions[i] = new QAction(tName, this);
         fileMenu->insertAction(action, rf_Actions[i]);

         if (i >= cnt)  {
            rf_Actions[i]->setVisible(false);
         }

         connect(rf_Actions[i], SIGNAL(triggered()), this, SLOT(rf_Open()));
      }

      fileMenu->insertSeparator(rf_Actions[rf_MaxCnt]);
   }
}

void MainWindow::rf_Update()
{
   int cnt = m_rf_List.count();

   if (cnt >= rf_MaxCnt ) {
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

   for (int i = 0; i < rf_MaxCnt; ++i) {

     if (i < cnt)  {
        rf_Actions[i]->setText(m_rf_List[i]);
        rf_Actions[i]->setVisible(true);

     } else {
        rf_Actions[i]->setVisible(false);
     }

   }   
}
