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

#include "dialog_getline.h"

Dialog_GetLine::Dialog_GetLine()
   : m_ui(new Ui::Dialog_GetLine)
{
   m_ui->setupUi(this);
   this->setWindowIcon(QIcon("://resources/diamond.png"));

   m_ui->value->setText("1");

   // any key deletes first, right arrow to continue typing
   m_ui->value->selectAll();

   connect(m_ui->ok_PB,     &QPushButton::clicked, this, &Dialog_GetLine::ok);
   connect(m_ui->cancel_PB, &QPushButton::clicked, this, &Dialog_GetLine::cancel);
}

Dialog_GetLine::~Dialog_GetLine()
{
   delete m_ui;
}

void Dialog_GetLine::ok()
{
   this->done(1);
}

void Dialog_GetLine::cancel()
{
   this->done(0);
}

QString Dialog_GetLine::get_Value()
{
   return m_ui->value->text();
}

void Dialog_GetLine::set_ColNo()
{
   setWindowTitle("Go to Column");
   m_ui->label->setText("Column Number:");
}

