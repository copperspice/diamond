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

#include "dialog_preset.h"
#include "util.h"

#include <QFileDialog>

Dialog_Preset::Dialog_Preset(MainWindow *from, QStringList data)
   : m_ui(new Ui::Dialog_Preset)
{
   m_parent   = from;
   m_dataList = data;

   m_ui->setupUi(this);
   this->setWindowIcon(QIcon("://resources/diamond.png"));

   for (int k = 0; k < m_dataList.count(); k++)  {
      QString temp = m_dataList.at(k);

      switch (k)  {
          case 0:
            m_ui->folder1->setText(temp);
            m_ui->folder1->setCursorPosition(0);
            break;

         case 1:
            m_ui->folder2->setText(temp);
            m_ui->folder2->setCursorPosition(0);
            break;

         case 2:
            m_ui->folder3->setText(temp);
            m_ui->folder3->setCursorPosition(0);
            break;

         case 3:
            m_ui->folder4->setText(temp);
            m_ui->folder4->setCursorPosition(0);
            break;

         case 4:
            m_ui->folder5->setText(temp);
            m_ui->folder5->setCursorPosition(0);
            break;

         case 5:
            m_ui->folder6->setText(temp);
            m_ui->folder6->setCursorPosition(0);
            break;

         case 6:
            m_ui->folder7->setText(temp);
            m_ui->folder7->setCursorPosition(0);
            break;

         case 7:
            m_ui->folder8->setText(temp);
            m_ui->folder8->setCursorPosition(0);
            break;

         case 8:
            m_ui->folder9->setText(temp);
            m_ui->folder9->setCursorPosition(0);
            break;
         case 9:
            m_ui->folder10->setText(temp);
            m_ui->folder10->setCursorPosition(0);
            break;

         default:
            break;
       }
   }


/*
   connect(m_ui->folder1_TB,  &PushButton::clicked()), this,
         [this](){ pick_Folder("1", &m_ui->folder1)  }  );
*/

   connect(m_ui->folder1_TB,  SIGNAL(clicked()), this, SLOT(folder1_TB()));
   connect(m_ui->folder2_TB,  SIGNAL(clicked()), this, SLOT(folder2_TB()));
   connect(m_ui->folder3_TB,  SIGNAL(clicked()), this, SLOT(folder3_TB()));
   connect(m_ui->folder4_TB,  SIGNAL(clicked()), this, SLOT(folder4_TB()));
   connect(m_ui->folder5_TB,  SIGNAL(clicked()), this, SLOT(folder5_TB()));
   connect(m_ui->folder6_TB,  SIGNAL(clicked()), this, SLOT(folder6_TB()));
   connect(m_ui->folder7_TB,  SIGNAL(clicked()), this, SLOT(folder7_TB()));
   connect(m_ui->folder8_TB,  SIGNAL(clicked()), this, SLOT(folder8_TB()));

   connect(m_ui->save_PB,     SIGNAL(clicked()), this, SLOT(save()));
   connect(m_ui->cancel_PB,   SIGNAL(clicked()), this, SLOT(cancel()));
}

Dialog_Preset::~Dialog_Preset()
{
   delete m_ui;
}

void Dialog_Preset::folder1_TB()
{
   pick_Folder("1", m_ui->folder1);
}

void Dialog_Preset::folder2_TB()
{
   pick_Folder("2", m_ui->folder2);
}

void Dialog_Preset::folder3_TB()
{
   pick_Folder("3", m_ui->folder3);
}

void Dialog_Preset::folder4_TB()
{
   pick_Folder("4", m_ui->folder4);
}

void Dialog_Preset::folder5_TB()
{
   pick_Folder("5", m_ui->folder5);
}

void Dialog_Preset::folder6_TB()
{
   pick_Folder("6", m_ui->folder6);
}

void Dialog_Preset::folder7_TB()
{
   pick_Folder("7", m_ui->folder7);
}

void Dialog_Preset::folder8_TB()
{
   pick_Folder("8", m_ui->folder8);
}

void Dialog_Preset::pick_Folder(QString number, QLineEdit *field)
{
   QString title     = "Select Folder " + number;
   QString oldFolder = field->text();

   QString newFolder = m_parent->get_DirPath(title, oldFolder);

   if (! newFolder.isEmpty()) {
      field->setText(newFolder);
   }   
}

void Dialog_Preset::save()
{
   this->done(QDialog::Accepted);
}

void Dialog_Preset::cancel()
{
   this->done(QDialog::Rejected);
}

QStringList Dialog_Preset::getData()
{ 
   for (int k = 0; k < m_dataList.count(); k++)  {

      switch (k)  {
          case 0:
            m_dataList.replace(k, m_ui->folder1->text());
            break;

         case 1:
            m_dataList.replace(k, m_ui->folder2->text());
            break;

         case 2:
            m_dataList.replace(k, m_ui->folder3->text());
            break;

         case 3:
            m_dataList.replace(k, m_ui->folder4->text());
            break;

         case 4:
            m_dataList.replace(k, m_ui->folder5->text());
            break;

         case 5:
            m_dataList.replace(k, m_ui->folder6->text());
            break;

         case 6:
            m_dataList.replace(k, m_ui->folder7->text());
            break;

         case 7:
            m_dataList.replace(k, m_ui->folder8->text());
            break;

         case 8:
            m_dataList.replace(k, m_ui->folder9->text());
            break;

         case 9:
            m_dataList.replace(k, m_ui->folder10->text());
            break;

         default:
            break;
       }
   }
   return m_dataList;
}

