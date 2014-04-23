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

#include "dialog_advfind.h"

#include <QDir>
#include <QFileDialog>

Dialog_AdvFind::Dialog_AdvFind(MainWindow *from, QString findText, QString fileType, QString findFolder, bool searchFolders)
   : m_ui(new Ui::Dialog_AdvFind)
{   
   m_parent = from;

   m_ui->setupUi(this);
   this->setWindowIcon(QIcon("://resources/diamond.png"));

   m_ui->find->setText(findText);
   m_ui->findType->setText(fileType);
   m_ui->findFolder->setText(findFolder);

   if (searchFolders) {
      m_ui->searchSubFolders_CKB->setChecked(true);
   }

   connect(m_ui->folder_TB, SIGNAL(clicked()),this, SLOT(pick_Folder()));
   connect(m_ui->find_PB,   SIGNAL(clicked()),this, SLOT(Find()));
   connect(m_ui->cancel_PB, SIGNAL(clicked()),this, SLOT(Cancel()));
}

Dialog_AdvFind::~Dialog_AdvFind()
{
   delete m_ui;
}

void Dialog_AdvFind::pick_Folder()
{
   QString oldFolder = m_ui->findFolder->text();

   if (oldFolder.isEmpty())    {
      oldFolder = QDir::currentPath();
   }

   QString newFolder = m_parent->get_DirPath("Select Directory or Folder to Search", oldFolder);

   if (! newFolder.isEmpty()) {
      m_ui->findFolder->setText(newFolder);
   }
}

void Dialog_AdvFind::Find()
{
   this->done(1);
}

void Dialog_AdvFind::Cancel()
{
   this->done(0);
}

QString Dialog_AdvFind::get_findText()
{
   return m_ui->find->text();
}

QString Dialog_AdvFind::get_findType()
{
   return m_ui->findType->text();
}

QString Dialog_AdvFind::get_findFolder()
{
   return m_ui->findFolder->text();
}

bool Dialog_AdvFind::get_Case()
{
   return m_ui->case_CKB->isChecked();
}

bool Dialog_AdvFind::get_WholeWords()
{
   return m_ui->wholeWords_CKB->isChecked();
}

bool Dialog_AdvFind::get_SearchSubFolders()
{
   return m_ui->searchSubFolders_CKB->isChecked();
}

