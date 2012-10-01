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

#include "dialog_symbols.h"
#include "util.h"

#include <QStringList>
#include <QStringListModel>
#include <QListView>

Dialog_Symbols::Dialog_Symbols(MainWindow *from)
   : m_ui(new Ui::Dialog_Symbols)
{
   m_parent = from;
   m_ui->setupUi(this);   

   // alter the hightlight color
   QPalette temp = m_ui->sampleList->palette();
   temp.setColor( QPalette::Highlight, QColor(206, 206, 206, 255));
   temp.setColor( QPalette::HighlightedText, Qt::black);
   m_ui->sampleList->setPalette(temp);

   QStringListModel *model = new QStringListModel(this);
   model->setStringList( initData() );

   m_ui->sampleList->setModel(model);  
   m_ui->sampleList->setCurrentIndex(model->index(0));

   connect(m_ui->insert_PB, SIGNAL(clicked()),this, SLOT(Insert()));
   connect(m_ui->cancel_PB, SIGNAL(clicked()),this, SLOT(Cancel()));
}

Dialog_Symbols::~Dialog_Symbols()
{
   delete m_ui;
}

QStringList Dialog_Symbols::initData()
{
   QStringList data;

   data
        << "&amp;"
        << "&nbsp;"
        << "&lt;"
        << "&gt;"
        << QChar(0x02153)     // "1/3"
        << QChar(0x02154)     // "2/3"
        << QChar(0x0215C)     // "3/8"
        << QChar(0x0215D)     // "5/8"
        << QChar(0x0215E)     // "7/8"
        << QChar(0x00BC)      // "1/4"
        << QChar(0x00BD)      // "1/2"
        << QChar(0x00BE)      // "3/4"
        << QChar(0x00AE)      // "R"
        << QChar(0x00A9)      // "C"
        << QChar(0x2122)      // "TM"
        << QChar(0x20AC)      // euro
        << QChar(0x00A3)      // pound
        << QChar(0x00BF)      // upside down ?
        << QChar(0x00A1);     // upside down !

   return data;
}

void Dialog_Symbols::Insert()
{
   this->done(QDialog::Accepted);
}

void Dialog_Symbols::Cancel()
{
   this->done(QDialog::Rejected);
}

QString Dialog_Symbols::get_Symbol()
{
   return m_ui->sampleList->currentIndex().data().toString();
}
