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

#include "dialog_advfind.h"

#include <QDir>
#include <QFileDialog>

Dialog_AdvFind::Dialog_AdvFind(QString findText)
   : m_ui(new Ui::Dialog_AdvFind)
{
   m_ui->setupUi(this);

   m_ui->find->setText(findText);
   m_ui->findType->setText("*.*");
   m_ui->findFolder->setText(QDir::currentPath());

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
   QString dir = m_ui->findFolder->text();

   if (dir.isEmpty())    {
      dir = QDir::currentPath();
   }

   QFileDialog::Options options;
   options |= QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks;

   // on X11 the title bar may not be displayed
   QString path = QFileDialog::getExistingDirectory(this,"Select Directory or Folder to Search",
                       dir, options);

   // silly adjust for platform slash issue
   QDir temp(path + "/");
   path = temp.canonicalPath() + "/";

   if (! path.isEmpty()) {
      m_ui->findFolder->setText(path);
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

