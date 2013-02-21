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

#include "mainwindow.h"
#include "util.h"

#include <QVariant>
#include <QStringList>

void MainWindow::openF_CreateMenus()
{
   int cnt = m_openedFiles.count();

   QString tName;

   QMenu   *windowMenu = m_ui->menuWindow;
   QAction *action     = windowMenu->addSeparator();

   for (int i = 0; i < openF_MaxCnt; ++i) {

      if (i < cnt)  {
         tName = m_openedFiles[i];
      } else {
         tName = "file"+QString::number(i);
      }

      openF_Actions[i] = new QAction(tName, this);
      openF_Actions[i]->setData("open-file");

      windowMenu->addAction(openF_Actions[i]);

      if (i >= cnt)  {
         openF_Actions[i]->setVisible(false);
      }

      connect(openF_Actions[i], SIGNAL(triggered()), this, SLOT(openF_SelectTab()));
   }

}

void MainWindow::openF_SelectTab()
{
   QAction *action;
   action = (QAction *)sender();

   bool match = false;

   if (action) {
      QString fileName = action->text();

      if (fileName.isEmpty()) {
         // something is pretty bogus

      } else {
         int max   = m_tabWidget->count();
         int index = m_tabWidget->currentIndex();

         for (int k; k < max; ++k) {
            QString curFile = m_tabWidget->tabWhatsThis(k);

            if (curFile == fileName) {
               match = true;
               index = k;
               break;
            }
         }

         if (match) {
            // select new tab
            m_tabWidget->setCurrentIndex(index);

         } else {
            // delete entry from list
            m_openedFiles.removeOne(fileName);

            // update actions
            openF_UpdateActions();
         }

      } // fileName.isEmpty()
   }
}

void MainWindow::openF_Add()
{
   m_openedFiles.append(m_curFile);

   // update actions
   openF_UpdateActions();
}

void MainWindow::openF_Delete()
{
   m_openedFiles.removeOne(m_curFile);

   // update actions
   openF_UpdateActions();
}

void MainWindow::openF_UpdateActions()
{
   int cnt = m_openedFiles.count();

   for (int i = 0; i < openF_MaxCnt; ++i) {

      if (i < cnt)  {
         openF_Actions[i]->setText(m_openedFiles[i]);
         openF_Actions[i]->setVisible(true);

     } else {        
         openF_Actions[i]->setVisible(false);
     }

   }
}
