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

#include "mainwindow.h"
#include "util.h"

#include <QStringList>
#include <QVariant>

void MainWindow::openTab_CreateMenus()
{
   // re-populate m_openedFiles
   QString fullName;
   QString tName;

   int cnt = m_tabWidget->count();
   m_openedFiles.clear();
   m_openedModified.clear();

   for (int k = 0; k < cnt; ++k) {
      fullName = this->get_curFileName(k);

      if (fullName.isEmpty()) {
         --cnt;

      } else {
         m_openedFiles.append(fullName);
         m_openedModified.append(false);
      }
   }

   //
   QMenu *windowMenu = m_ui->menuWindow;
   windowMenu->addSeparator();

   for (int k = 0; k < OPENTABS_MAX; ++k) {

      if (k < cnt)  {
         fullName = m_openedFiles[k];
         tName    = fullName;

         if (m_openedModified[k]) {
            tName += " *";
         }

      } else {
         tName = "file"+QString::number(k);

      }

      openTab_Actions[k] = new QAction(tName, this);

//    openTab_Actions[k]->setData("select-tab");
      openTab_Actions[k]->setData(QString("select-tab"));

      windowMenu->addAction(openTab_Actions[k]);

      if (k >= cnt)  {
         openTab_Actions[k]->setVisible(false);
      }

      connect(openTab_Actions[k], &QAction::triggered, this, [this, k](bool){ openTab_Select(k); } );
   }
}

void MainWindow::openTab_Select(int index)
{ 
   bool match = false;
   QString fullName = m_openedFiles[index];

   if (fullName.isEmpty()) {
      // something is pretty bogus

   } else {
      int cnt   = m_tabWidget->count();
      int index = m_tabWidget->currentIndex();

      for (int k = 0; k < cnt; ++k) {
         QString tcurFile = this->get_curFileName(k);

         if (tcurFile == fullName) {
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
         m_openedFiles.removeOne(fullName);

         // update actions
         openTab_UpdateActions();
      }
   }
}

void MainWindow::showContext_Tabs(const QPoint &pt)
{
   QAction *action = m_ui->menuWindow->actionAt(pt);

   if (action)  {
      QString data = action->data().toString();

      if (data == "select-tab")  {
         QMenu *menu = new QMenu(this);
         menu->addAction("Reorder Tab file list", this, SLOT(openTab_redo() ));

         menu->exec(m_ui->menuWindow->mapToGlobal(pt));
         delete menu;
      }
   }
}

void MainWindow::openTab_redo()
{
   QWidget *temp;
   DiamondTextEdit *textEdit;

   QAction *action;
   action = (QAction *)sender();

   if (action) {
      // re-populate m_openedFiles and m_openedModified
      QString tName;
      bool isModified;

      m_openedFiles.clear();
      m_openedModified.clear();

      int cnt = m_tabWidget->count();

      for (int k = 0; k < cnt; ++k) {
         tName = this->get_curFileName(k);
         m_openedFiles.append(tName);

         //
         temp = m_tabWidget->widget(k);
         textEdit = dynamic_cast<DiamondTextEdit *>(temp);

         if (textEdit) {
            isModified = textEdit->document()->isModified();
            m_openedModified.append(isModified);
         }
      }

      for (int i = 0; i < OPENTABS_MAX; ++i) {

         if (i < cnt)  {
            tName = m_openedFiles[i];

            if (m_openedModified[i]) {
               tName += " *";
            }

         } else {
            tName = "file"+QString::number(i);
         }

         openTab_Actions[i]->setText(tName);

         if (i >= cnt)  {
            openTab_Actions[i]->setVisible(false);
         }

      }
   }
}

void MainWindow::openTab_Add()
{ 
   if (m_curFile.isEmpty()) {
      return;
   }

   m_openedFiles.append(m_curFile);
   m_openedModified.append(false);

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

   for (int k = 0; k < OPENTABS_MAX; ++k) {

      if (k < cnt)  {
         QString modified;

         if (m_openedModified[k]) {
            modified += " *";
         }

         openTab_Actions[k]->setText(m_openedFiles[k] + modified);
         openTab_Actions[k]->setVisible(true);

     } else {        
         openTab_Actions[k]->setVisible(false);
     }

   }
}

void MainWindow::openTab_UpdateOneAction(int index, bool isModified)
{
   if (index < OPENTABS_MAX) {
      QString modified;

      if (isModified) {
         modified += " *";
      }

      openTab_Actions[index]->setText(m_openedFiles[index] + modified);
   }
}
