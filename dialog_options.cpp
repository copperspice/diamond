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

#include "dialog_options.h"
#include "util.h"

#include <QString>
#include <QStringList>

Dialog_Options::Dialog_Options(MainWindow *from)
   : m_ui(new Ui::Dialog_Options)
{
   m_parent = from;
   m_ui->setupUi(this);

   initData();

   connect(m_ui->save_PB,   SIGNAL(clicked()),this, SLOT(Save()));
   connect(m_ui->cancel_PB, SIGNAL(clicked()),this, SLOT(Cancel()));
}

Dialog_Options::~Dialog_Options()
{
   delete m_ui;
}

void Dialog_Options::initData()
{
   struct Settings temp = m_parent->get_StructData();

   QStringList list;   
   int index;

   //
   list << "MM/dd/yyyy" << "dd/MM/yyyy" <<  "MMM dd, yyyy" << "MMMM dd, yyyy"
        << "yyyyMMdd";

   m_ui->dateFormat_CB->addItems(list);
   m_ui->dateFormat_CB->setEditable(false);

   index = m_ui->dateFormat_CB->findText(temp.formatDate);
   m_ui->dateFormat_CB->setCurrentIndex(index);

   //
   list.clear();
   list << "3" << "4" << "8";
   m_ui->tabSpacing_CB->addItems(list);
   m_ui->tabSpacing_CB->setEditable(false);

   index = m_ui->tabSpacing_CB->findText(QString::number(temp.tabSpacing));
   m_ui->tabSpacing_CB->setCurrentIndex(index);
}

void Dialog_Options::Save()
{
   this->done(QDialog::Accepted);
}

void Dialog_Options::Cancel()
{
   this->done(QDialog::Rejected);
}

QString Dialog_Options::get_DateFormat()
{
   return m_ui->dateFormat_CB->currentText();
}

int Dialog_Options::get_TabSpacing()
{
   QString value = m_ui->tabSpacing_CB->currentText();
   int temp = value.toInt();

   return temp;
}
