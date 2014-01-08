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

#include "mainwindow.h"
#include "util.h"

#include <QVariant>
#include <QStringList>

void MainWindow::openTab_CreateMenus()
{
   // re-populate m_openedFiles
   QString tName;

   int cnt = m_tabWidget->count();
   m_openedFiles.clear();

   for (int k = 0; k < cnt; ++k) {
      tName = this->get_curFileName(k);
      m_openedFiles.append(tName);
   }

   //
   QMenu *windowMenu = m_ui->menuWindow;
   windowMenu->addSeparator();

   for (int i = 0; i < openTab_MaxCnt; ++i) {

      if (i < cnt)  {
         tName = m_openedFiles[i];
      } else {
         tName = "file"+QString::number(i);
      }

      openTab_Actions[i] = new QAction(tName, this);
      openTab_Actions[i]->setData("select-tab");

      windowMenu->addAction(openTab_Actions[i]);

      if (i >= cnt)  {
         openTab_Actions[i]->setVisible(false);         
      }

      connect(openTab_Actions[i], SIGNAL(triggered()), this, SLOT(openTab_Select()));      
   }
}

void MainWindow::openTab_Select()
{
   QAction *action;
   action = (QAction *)sender();

   bool match = false;

   if (action) {
      QString fileName = action->text();

      if (fileName.isEmpty()) {
         // something is pretty bogus

      } else {
         int cnt   = m_tabWidget->count();
         int index = m_tabWidget->currentIndex();

         for (int k = 0; k < cnt; ++k) {
            QString tcurFile = this->get_curFileName(k);

            if (tcurFile == fileName) {
               match = true;
               index = k;
               break;
            }
         }

         if (match) {
            // select new tab
            m_tabWidget->setCurrentIndex(index);

         } else {
            // delete entry from list since it did not exist
            m_openedFiles.removeOne(fileName);

            // update actions
            openTab_UpdateActions();
         }

      } // fileName.isEmpty()
   }
}

void MainWindow::openTab_Add()
{ 
   if (m_curFile.isEmpty()) {
      return;
   }

   m_openedFiles.append(m_curFile);

   // update actions
   openTab_UpdateActions();
}

void MainWindow::openTab_Delete()
{
   m_openedFiles.removeOne(m_curFile);

   // update actions
   openTab_UpdateActions();
}

void MainWindow::openTab_UpdateActions()
{
   int cnt = m_openedFiles.count();
   QString isModified;

   for (int i = 0; i < openTab_MaxCnt; ++i) {

      if (i < cnt)  {

/*       Not working as desired

         if (m_textEdit->document()->isModified()) {
            isModified = " *";
         } else {
            isModified = "";
         }                  
*/

         openTab_Actions[i]->setText(m_openedFiles[i] + isModified);
         openTab_Actions[i]->setVisible(true);

     } else {        
         openTab_Actions[i]->setVisible(false);
     }

   }
}
