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

#include "dialog_open.h"
#include "util.h"

#include <QListView>
#include <QStringList>
#include <QStringListModel>

Dialog_Open::Dialog_Open(MainWindow *parent, QStringList strlist)
   : QDialog(parent), m_ui(new Ui::Dialog_Open)
{
   m_list = strlist;

   m_ui->setupUi(this);
   setWindowIcon(QIcon("://resources/diamond.png"));

   // alter the hightlight color
   QPalette tmp = m_ui->openList->palette();
   tmp.setColor( QPalette::Highlight, QColor(206, 206, 206, 255));
   tmp.setColor( QPalette::HighlightedText, Qt::black);
   m_ui->openList->setPalette(tmp);

   QStringListModel *model = new QStringListModel(this);
   model->setStringList(m_list);

   m_ui->openList->setModel(model);
   m_ui->openList->setCurrentIndex(model->index(0));

   m_ui->openList->setFocus();

   connect(m_ui->open_PB,   &QPushButton::clicked, this, &Dialog_Open::open);
   connect(m_ui->cancel_PB, &QPushButton::clicked, this, &Dialog_Open::cancel);
}

Dialog_Open::~Dialog_Open()
{
   delete m_ui;
}

void Dialog_Open::open()
{
   done(QDialog::Accepted);
}

void Dialog_Open::cancel()
{
   done(QDialog::Rejected);
}

QString Dialog_Open::get_FileName()
{
   return m_ui->openList->currentIndex().data().toString();
}
