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

#include "dialog_find.h"

Dialog_Find::Dialog_Find(QString findText)
   : m_ui(new Ui::Dialog_Find)
{
   m_ui->setupUi(this);
   m_ui->value->setText(findText);
   m_ui->down_RB->setChecked(true);

   connect(m_ui->find_PB,   SIGNAL(clicked()),this, SLOT(Find()));
   connect(m_ui->cancel_PB, SIGNAL(clicked()),this, SLOT(Cancel()));
}

Dialog_Find::~Dialog_Find()
{
   delete m_ui;
}

void Dialog_Find::Find()
{
   this->done(1);
}

void Dialog_Find::Cancel()
{
   this->done(0);
}

QString Dialog_Find::get_Value()
{
   return m_ui->value->text();
}

bool Dialog_Find::get_Direction()
{
   if (m_ui->down_RB->isChecked()) {
      return true;
   } else {
      return false;
   }
}

bool Dialog_Find::get_Case()
{
   return m_ui->case_CKB->isChecked();
}

bool Dialog_Find::get_WholeWords()
{
   return m_ui->wholeWords_CKB->isChecked();
}
