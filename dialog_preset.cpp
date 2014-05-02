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
#include <QFileInfo>

Dialog_Preset::Dialog_Preset(MainWindow *from, QStringList data)
   : m_ui(new Ui::Dialog_Preset)
{
   m_parent   = from;
   m_dataList = data;

   m_ui->setupUi(this);
   this->setWindowIcon(QIcon("://resources/diamond.png"));

   for (int k = 0; k < m_dataList.count(); k++)  {
      QString temp = m_dataList.at(k);
      int index = k + 1;

      switch (index)  {
          case 1:
            m_ui->folder1->setText(temp);
            m_ui->folder1->setCursorPosition(0);
            break;

         case 2:
            m_ui->folder2->setText(temp);
            m_ui->folder2->setCursorPosition(0);
            break;

         case 3:
            m_ui->folder3->setText(temp);
            m_ui->folder3->setCursorPosition(0);
            break;

         case 4:
            m_ui->folder4->setText(temp);
            m_ui->folder4->setCursorPosition(0);
            break;

         case 5:
            m_ui->folder5->setText(temp);
            m_ui->folder5->setCursorPosition(0);
            break;

         case 6:
            m_ui->folder6->setText(temp);
            m_ui->folder6->setCursorPosition(0);
            break;

         case 7:
            m_ui->folder7->setText(temp);
            m_ui->folder7->setCursorPosition(0);
            break;

         case 8:
            m_ui->folder8->setText(temp);
            m_ui->folder8->setCursorPosition(0);
            break;

         case 9:
            m_ui->folder9->setText(temp);
            m_ui->folder9->setCursorPosition(0);
            break;

         case 10:
            m_ui->folder10->setText(temp);
            m_ui->folder10->setCursorPosition(0);
            break;

         default:
            break;
       }
   }

   connect(m_ui->folder1,  &QLineEdit::editingFinished, this, [this](){ verify_Folder("1",  m_ui->folder1);  } );
   connect(m_ui->folder2,  &QLineEdit::editingFinished, this, [this](){ verify_Folder("2",  m_ui->folder2);  } );
   connect(m_ui->folder3,  &QLineEdit::editingFinished, this, [this](){ verify_Folder("3",  m_ui->folder3);  } );
   connect(m_ui->folder4,  &QLineEdit::editingFinished, this, [this](){ verify_Folder("4",  m_ui->folder4);  } );
   connect(m_ui->folder5,  &QLineEdit::editingFinished, this, [this](){ verify_Folder("5",  m_ui->folder5);  } );
   connect(m_ui->folder6,  &QLineEdit::editingFinished, this, [this](){ verify_Folder("6",  m_ui->folder6);  } );
   connect(m_ui->folder7,  &QLineEdit::editingFinished, this, [this](){ verify_Folder("7",  m_ui->folder7);  } );
   connect(m_ui->folder8,  &QLineEdit::editingFinished, this, [this](){ verify_Folder("8",  m_ui->folder8);  } );
   connect(m_ui->folder9,  &QLineEdit::editingFinished, this, [this](){ verify_Folder("9",  m_ui->folder9);  } );
   connect(m_ui->folder10, &QLineEdit::editingFinished, this, [this](){ verify_Folder("10", m_ui->folder10); } );

   connect(m_ui->folder1_TB,  &QPushButton::clicked, this, [this](){ pick_Folder("1",  m_ui->folder1);  } );
   connect(m_ui->folder2_TB,  &QPushButton::clicked, this, [this](){ pick_Folder("2",  m_ui->folder2);  } );
   connect(m_ui->folder3_TB,  &QPushButton::clicked, this, [this](){ pick_Folder("3",  m_ui->folder3);  } );
   connect(m_ui->folder4_TB,  &QPushButton::clicked, this, [this](){ pick_Folder("4",  m_ui->folder4);  } );
   connect(m_ui->folder5_TB,  &QPushButton::clicked, this, [this](){ pick_Folder("5",  m_ui->folder5);  } );
   connect(m_ui->folder6_TB,  &QPushButton::clicked, this, [this](){ pick_Folder("6",  m_ui->folder6);  } );
   connect(m_ui->folder7_TB,  &QPushButton::clicked, this, [this](){ pick_Folder("7",  m_ui->folder7);  } );
   connect(m_ui->folder8_TB,  &QPushButton::clicked, this, [this](){ pick_Folder("8",  m_ui->folder8);  } );
   connect(m_ui->folder9_TB,  &QPushButton::clicked, this, [this](){ pick_Folder("9",  m_ui->folder9);  } );
   connect(m_ui->folder10_TB, &QPushButton::clicked, this, [this](){ pick_Folder("10", m_ui->folder10); } );

   connect(m_ui->save_PB,     SIGNAL(clicked()), this, SLOT(save()));
   connect(m_ui->cancel_PB,   SIGNAL(clicked()), this, SLOT(cancel()));
}

Dialog_Preset::~Dialog_Preset()
{
   delete m_ui;
}

void Dialog_Preset::verify_Folder(QString number, QLineEdit *field)
{
   QString folder = field->text();

   if (! folder.isEmpty()) {

      csMsg(folder);

      bool ok = QFileInfo(folder).isDir();

      if (! ok) {
         csError("Preset Folder" + number, folder + "does not exist or is not valid");
      }
   }
}

void Dialog_Preset::pick_Folder(QString number, QLineEdit *field)
{
   QString title     = tr("Select Folder ") + number;
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
      int index = k + 1;

      switch (index)  {
         case 1:
            m_dataList.replace(k, m_ui->folder1->text());
            break;

         case 2:
            m_dataList.replace(k, m_ui->folder2->text());
            break;

         case 3:
            m_dataList.replace(k, m_ui->folder3->text());
            break;

         case 4:
            m_dataList.replace(k, m_ui->folder4->text());
            break;

         case 5:
            m_dataList.replace(k, m_ui->folder5->text());
            break;

         case 6:
            m_dataList.replace(k, m_ui->folder6->text());
            break;

         case 7:
            m_dataList.replace(k, m_ui->folder7->text());
            break;

         case 8:

            csMsg("adding folder 8");

            m_dataList.replace(k, m_ui->folder8->text());
            break;

         case 9:
            m_dataList.replace(k, m_ui->folder9->text());
            break;

         case 10:
            m_dataList.replace(k, m_ui->folder10->text());
            break;

         default:
            break;
       }
   }
   return m_dataList;
}

