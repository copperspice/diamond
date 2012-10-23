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

#include "dialog_print_opt.h"
#include "util.h"

Dialog_PrintOptions::Dialog_PrintOptions(MainWindow *from, struct PrintSettings data)
   : m_ui(new Ui::Dialog_PrintOptions)
{
   m_parent = from;
   m_print  = data;

   m_ui->setupUi(this);

   initData();

   connect(m_ui->headerLeft_TB, SIGNAL(clicked()), this, SLOT(headerLeft()));
   connect(m_ui->headerCenter_TB, SIGNAL(clicked()), this, SLOT(headerCenter()));
   connect(m_ui->headerRight_TB, SIGNAL(clicked()), this, SLOT(headerRight()));

   connect(m_ui->save_PB,    SIGNAL(clicked()), this, SLOT(Save()));
   connect(m_ui->cancel_PB,  SIGNAL(clicked()), this, SLOT(Cancel()));
}

Dialog_PrintOptions::~Dialog_PrintOptions()
{
   delete m_ui;
}

void Dialog_PrintOptions::initData()
{
   m_ui->print_header_CKB->setChecked(m_print.printHeader);
   m_ui->header_Left->setText(m_print.header_left);
   m_ui->header_Center->setText(m_print.header_center);
   m_ui->header_Right->setText(m_print.header_right);
   m_ui->header_Line2->setText(m_print.header_line2);

   m_ui->print_footer_CKB->setChecked(m_print.printFooter);
   m_ui->footer_Left->setText(m_print.footer_left);
   m_ui->footer_Center->setText(m_print.footer_center);
   m_ui->footer_Right->setText(m_print.footer_right);
   m_ui->footer_Line2->setText(m_print.footer_line2);

   m_ui->marginTop->setText(QString::number(m_print.marTop));
   m_ui->marginBottom->setText(QString::number(m_print.marBottom));
   m_ui->marginRight->setText(QString::number(m_print.marRight));
   m_ui->marginLeft->setText(QString::number(m_print.marLeft));
}

void Dialog_PrintOptions::Save()
{
   this->done(QDialog::Accepted);
}

void Dialog_PrintOptions::Cancel()
{
   this->done(QDialog::Rejected);
}

struct PrintSettings Dialog_PrintOptions::get_Results()
{
   m_print.header_left   = m_ui->header_Left->text();
   m_print.header_center = m_ui->header_Center->text();
   m_print.header_right  = m_ui->header_Right->text();
   m_print.header_line2  = m_ui->header_Line2->text();

   m_print.footer_left   = m_ui->footer_Left->text();
   m_print.footer_center = m_ui->footer_Center->text();
   m_print.footer_right  = m_ui->footer_Right->text();
   m_print.footer_line2  = m_ui->footer_Line2->text();

   m_print.marTop        = m_ui->marginTop->text().toDouble();
   m_print.marBottom     = m_ui->marginBottom->text().toDouble();
   m_print.marRight      = m_ui->marginRight->text().toDouble();
   m_print.marLeft       = m_ui->marginLeft->text().toDouble();

   return m_print;
}

void Dialog_PrintOptions::headerLeft()
{
   QString text = m_ui->header_Left->text();
   m_menuText = "";

   //
   QMenu *menu = new QMenu(this);
   menu->addAction("File Name Only",        this, SLOT(fileName())  );
   menu->addAction("File Name with Path",   this, SLOT(pathFileName())  );
   menu->addSeparator();

   menu->addAction("Page #",                this, SLOT(pageNumber())  );
   menu->addAction("Total Pages",           this, SLOT(totalPages())  );
   menu->addAction("Page # of Total Pages", this, SLOT(pages())  );

   menu->addSeparator();
   menu->addAction("Date",  this, SLOT(date())  );
   menu->addAction("Time",  this, SLOT(time())  );

   menu->exec(m_ui->headerLeft_TB->mapToGlobal(QPoint(40,0)));

   if (! m_menuText.isEmpty()) {
      text = text + m_menuText;
      m_ui->header_Left->setText(text);
   }

   delete menu;
}

void Dialog_PrintOptions::headerCenter()
{
}

void Dialog_PrintOptions::headerRight()
{
}

// menu slots
void Dialog_PrintOptions::fileName()
{
   m_menuText = "$(FileName)";
}

void Dialog_PrintOptions::pathFileName()
{
   m_menuText = "$(PathFileName)";
}

void Dialog_PrintOptions::pageNumber()
{
   m_menuText = "$(PageNo)";
}

void Dialog_PrintOptions::totalPages()
{
   m_menuText = "$(TotalPages)";
}

void Dialog_PrintOptions::pages()
{
   m_menuText = "$(PageNumber) of $(TotalPages)";
}

void Dialog_PrintOptions::date()
{
   m_menuText = "$(Date)";
}

void Dialog_PrintOptions::time()
{
   m_menuText = "$(Tsime)";
}


