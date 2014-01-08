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

#include "dialog_fonts.h"
#include "util.h"

#include <QFontDialog>

Dialog_Fonts::Dialog_Fonts(QFont normal, QFont column)
   : m_ui(new Ui::Dialog_Fonts)
{
   m_fontNormal = normal;
   m_fontColumn = column;

   m_ui->setupUi(this);

   m_ui->fontNormal->setText(m_fontNormal.toString());
   m_ui->fontColumn->setText(m_fontColumn.toString());

   connect(m_ui->fontNormal_TB,  SIGNAL(clicked()), this, SLOT(fontNormal()));
   connect(m_ui->fontColumn_TB,  SIGNAL(clicked()), this, SLOT(fontColumn()));

   connect(m_ui->save_PB,        SIGNAL(clicked()), this, SLOT(save()));
   connect(m_ui->cancel_PB,      SIGNAL(clicked()), this, SLOT(cancel()));
}

Dialog_Fonts::~Dialog_Fonts()
{
   delete m_ui;
}

void Dialog_Fonts::fontNormal()
{
   bool ok;
   QFont font = QFontDialog::getFont(&ok, m_fontNormal, this);

   if (ok) {
      m_fontNormal = font;
      m_ui->fontNormal->setText(font.toString());
   }
}

void Dialog_Fonts::fontColumn()
{
   bool ok;
   QFont font = QFontDialog::getFont(&ok, m_fontColumn, this);

   if (ok) {
      m_fontColumn = font;
      m_ui->fontColumn->setText(font.toString());
   }
}

//
void Dialog_Fonts::save()
{
   this->done(QDialog::Accepted);
}

void Dialog_Fonts::cancel()
{
   this->done(QDialog::Rejected);
}

QFont Dialog_Fonts::get_fontNormal()
{
   return m_fontNormal;
}

QFont Dialog_Fonts::get_fontColumn()
{
   return m_fontColumn;
}

