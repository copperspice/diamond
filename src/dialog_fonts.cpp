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

#include "dialog_fonts.h"
#include "util.h"

#include <QFontDialog>
#include <QToolButton>

Dialog_Fonts::Dialog_Fonts(QFont normal, QFont column)
   : m_ui(new Ui::Dialog_Fonts)
{
   m_fontNormal = normal;
   m_fontColumn = column;

   m_ui->setupUi(this);
   this->setWindowIcon(QIcon("://resources/diamond.png"));

   m_ui->fontNormal->setText(m_fontNormal.toString());
   m_ui->fontColumn->setText(m_fontColumn.toString());

   connect(m_ui->fontNormal_TB,  &QToolButton::clicked, this, &Dialog_Fonts::fontNormal);
   connect(m_ui->fontColumn_TB,  &QToolButton::clicked, this, &Dialog_Fonts::fontColumn);
   connect(m_ui->save_PB,        &QPushButton::clicked, this, &Dialog_Fonts::save);
   connect(m_ui->cancel_PB,      &QPushButton::clicked, this, &Dialog_Fonts::cancel);
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

