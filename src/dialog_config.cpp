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

#include "dialog_config.h"
#include "util.h"

Dialog_Config::Dialog_Config(QString fileName)
   : m_ui(new Ui::Dialog_Config)
{
   m_ui->setupUi(this);
   setWindowIcon(QIcon("://resources/diamond.png"));

   m_ui->currentName->setText(fileName);
   m_ui->newName->setText(fileName);

   connect(m_ui->create_PB, &QPushButton::clicked, this, &Dialog_Config::createNew);
   connect(m_ui->select_PB, &QPushButton::clicked, this, &Dialog_Config::select);
   connect(m_ui->rename_PB, &QPushButton::clicked, this, &Dialog_Config::rename);
   connect(m_ui->cancel_PB, &QPushButton::clicked, this, &Dialog_Config::cancel);
}

Dialog_Config::~Dialog_Config()
{
   delete m_ui;
}

void Dialog_Config::createNew()
{
   done(1);
}

void Dialog_Config::select()
{
   done(2);
}

void Dialog_Config::rename()
{
   QString name = m_ui->newName->text();

   if (name.isEmpty())  {
      csError("Rename Configuration File", "New file name can not be blank.");
      return;
   }

   done(3);
}

void Dialog_Config::cancel()
{
   done(0);
}

QString Dialog_Config::get_newName()
{
   return m_ui->newName->text();
}

