/**************************************************************************
*
* Copyright (c) 2012-2019 Barbara Geller
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

#include "dialog_advfind.h"

#include <QDir>
#include <QFileDialog>
#include <QToolButton>

Dialog_AdvFind::Dialog_AdvFind(MainWindow *parent, QString findText, QString fileType, QString findFolder, bool searchFolders)
   : QDialog(parent), m_ui(new Ui::Dialog_AdvFind)
{   
   m_parent  = parent;
   m_busyMsg = nullptr;

   m_ui->setupUi(this);
   setWindowIcon(QIcon("://resources/diamond.png"));

   m_ui->find->setText(findText);
   m_ui->findType->setText(fileType);
   m_ui->findFolder->setText(findFolder);

   if (searchFolders) {
      m_ui->searchSubFolders_CKB->setChecked(true);
   }

   connect(m_ui->folder_TB, &QToolButton::clicked, this, &Dialog_AdvFind::pick_Folder);
   connect(m_ui->find_PB,   &QPushButton::clicked, this, &Dialog_AdvFind::find);
   connect(m_ui->cancel_PB, &QPushButton::clicked, this, &Dialog_AdvFind::cancel);
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

void Dialog_AdvFind::showBusyMsg()
{
   m_ui->find_PB->setVisible(false);
   m_ui->horizontalSpacer_32->changeSize(0,0);
   m_ui->cancel_PB->setVisible(false);

   if (m_busyMsg == nullptr)   {
      m_busyMsg = new QLabel();
      m_busyMsg->setText("Preparing list of files, this process may take a minute...");

      QFont font = m_busyMsg->font();
      font.setPointSize(10);
      m_busyMsg->setFont(font);

      QPalette palette = m_busyMsg->palette();
      palette.setColor(QPalette::WindowText, QColor{0,0,255} );
      m_busyMsg->setPalette(palette);

      m_ui->layout_buttons->insertWidget(2, m_busyMsg);

   } else {
      m_busyMsg->setVisible(true);

   }

   show();
   QApplication::processEvents();
}

void Dialog_AdvFind::showNotBusyMsg()
{
   if (m_busyMsg == nullptr)   {
      // prior search failed, nothing to undo
      return;
   }

   m_busyMsg->setVisible(false);

   m_ui->find_PB->setVisible(true);
   m_ui->horizontalSpacer_32->changeSize(8,25);
   m_ui->cancel_PB->setVisible(true);

   QApplication::processEvents();
}

void Dialog_AdvFind::cancel()
{
   this->done(0);
}

void Dialog_AdvFind::find()
{
   this->done(1);
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

