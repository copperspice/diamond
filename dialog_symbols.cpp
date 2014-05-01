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
   this->setWindowIcon(QIcon("://resources/diamond.png"));

   // alter the hightlight color
   QPalette temp = m_ui->sampleList->palette();
   temp.setColor( QPalette::Highlight, QColor(206, 206, 206, 255));
   temp.setColor( QPalette::HighlightedText, Qt::black);
   m_ui->sampleList->setPalette(temp);

   QStringListModel *model = new QStringListModel(this);
   model->setStringList( initData() );

   m_ui->sampleList->setModel(model);  
   m_ui->sampleList->setCurrentIndex(model->index(0));

   m_ui->sampleList->setFocus();

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
        << "&mdash;"
        << "&ndash;"
        << "Tab Character"    // tab
        << QChar(0x00BC)      // "1/4"
        << QChar(0x00BD)      // "1/2"
        << QChar(0x00BE)      // "3/4"
        << QChar(0x02153)     // "1/3"
        << QChar(0x02154)     // "2/3"
        << QChar(0x0215C)     // "3/8"
        << QChar(0x0215D)     // "5/8"
        << QChar(0x0215E)     // "7/8"      
        << QChar(0x00AE)      // "R"
        << QChar(0x00A9)      // "C"
        << QChar(0x2122)      // "TM"
        << QChar(0x20AC)      // euro
        << QChar(0x00A3)      // pound
        << QChar(0x00C4)      // Ä
        << QChar(0x00CB)      // Ë
        << QChar(0x00CF)      // Ï
        << QChar(0x00D6)      // Ö
        << QChar(0x00DC)      // Ü
        << QChar(0x00E4)      // ä
        << QChar(0x00EB)      // ë
        << QChar(0x00EF)      // ï
        << QChar(0x00F6)      // ö
        << QChar(0x00FC)      // ü
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
