/**************************************************************************
*
* Copyright (c) 2012-2022 Barbara Geller
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

#include "dialog_print_opt.h"
#include "util.h"

#include <QFontDialog>
#include <QToolButton>

Dialog_PrintOptions::Dialog_PrintOptions(MainWindow *parent, struct PrintSettings data)
   : QDialog(parent), m_ui(new Ui::Dialog_PrintOptions)
{
   m_parent = parent;
   m_print  = data;

   m_ui->setupUi(this);
   this->setWindowIcon(QIcon("://resources/diamond.png"));

   initData();

   connect(m_ui->headerLeft_TB,   &QToolButton::clicked, this, &Dialog_PrintOptions::headerLeft);
   connect(m_ui->headerCenter_TB, &QToolButton::clicked, this, &Dialog_PrintOptions::headerCenter);
   connect(m_ui->headerRight_TB,  &QToolButton::clicked, this, &Dialog_PrintOptions::headerRight);
   connect(m_ui->footerLeft_TB,   &QToolButton::clicked, this, &Dialog_PrintOptions::footerLeft);
   connect(m_ui->footerCenter_TB, &QToolButton::clicked, this, &Dialog_PrintOptions::footerCenter);
   connect(m_ui->footerRight_TB,  &QToolButton::clicked, this, &Dialog_PrintOptions::footerRight);

   connect(m_ui->fontHeader_TB,   &QToolButton::clicked, this, &Dialog_PrintOptions::fontHeader);
   connect(m_ui->fontFooter_TB,   &QToolButton::clicked, this, &Dialog_PrintOptions::fontFooter);
   connect(m_ui->fontText_TB,     &QToolButton::clicked, this, &Dialog_PrintOptions::fontText);

   connect(m_ui->save_PB,         &QPushButton::clicked, this, &Dialog_PrintOptions::save);
   connect(m_ui->cancel_PB,       &QPushButton::clicked, this, &Dialog_PrintOptions::cancel);
}

Dialog_PrintOptions::~Dialog_PrintOptions()
{
   delete m_ui;
}

void Dialog_PrintOptions::initData()
{
   m_ui->lineNumbers_CKB->setChecked(m_print.lineNumbers);

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
   m_ui->hdrGap->setText(QString::number(m_print.hdrGap));

   m_ui->fontHeader->setText(m_print.fontHeader.toString());
   m_ui->fontFooter->setText(m_print.fontFooter.toString());
   m_ui->fontText->setText(m_print.fontText.toString());
}

void Dialog_PrintOptions::save()
{
   this->done(QDialog::Accepted);
}

void Dialog_PrintOptions::cancel()
{
   this->done(QDialog::Rejected);
}

struct PrintSettings Dialog_PrintOptions::get_Results()
{
   m_print.lineNumbers   = m_ui->lineNumbers_CKB->isChecked();

   m_print.printHeader   = m_ui->print_header_CKB->isChecked();
   m_print.header_left   = m_ui->header_Left->text();
   m_print.header_center = m_ui->header_Center->text();
   m_print.header_right  = m_ui->header_Right->text();
   m_print.header_line2  = m_ui->header_Line2->text();

   m_print.printFooter   = m_ui->print_footer_CKB->isChecked();
   m_print.footer_left   = m_ui->footer_Left->text();
   m_print.footer_center = m_ui->footer_Center->text();
   m_print.footer_right  = m_ui->footer_Right->text();
   m_print.footer_line2  = m_ui->footer_Line2->text();

   m_print.marTop        = m_ui->marginTop->text().toDouble();
   m_print.marBottom     = m_ui->marginBottom->text().toDouble();
   m_print.marRight      = m_ui->marginRight->text().toDouble();
   m_print.marLeft       = m_ui->marginLeft->text().toDouble();   
   m_print.hdrGap        = m_ui->hdrGap->text().toDouble();

   // unnecessary to save the fonts back to the structure

   return m_print;
}

void Dialog_PrintOptions::macroMenu(QToolButton *widget)
{
   m_menuText = "";

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

   menu->exec(widget->mapToGlobal(QPoint(40,0)));

   delete menu;
}

void Dialog_PrintOptions::headerLeft()
{
   QString text = m_ui->header_Left->text();
   macroMenu(m_ui->headerLeft_TB);

   if (! m_menuText.isEmpty()) {
      text = text + m_menuText;
      m_ui->header_Left->setText(text);
   } 
}

void Dialog_PrintOptions::headerCenter()
{
   QString text = m_ui->header_Center->text();
    macroMenu(m_ui->headerCenter_TB);

   if (! m_menuText.isEmpty()) {
      text = text + m_menuText;
      m_ui->header_Center->setText(text);
   }
}

void Dialog_PrintOptions::headerRight()
{
   QString text = m_ui->header_Right->text();
   macroMenu(m_ui->headerRight_TB);

   if (! m_menuText.isEmpty()) {
      text = text + m_menuText;
      m_ui->header_Right->setText(text);
   }
}

void Dialog_PrintOptions::footerLeft()
{
   QString text = m_ui->footer_Left->text();
   macroMenu(m_ui->footerLeft_TB);

   if (! m_menuText.isEmpty()) {
      text = text + m_menuText;
      m_ui->footer_Left->setText(text);
   }
}

void Dialog_PrintOptions::footerCenter()
{
   QString text = m_ui->footer_Center->text();
   macroMenu(m_ui->footerCenter_TB);

   if (! m_menuText.isEmpty()) {
      text = text + m_menuText;
      m_ui->footer_Center->setText(text);
   }
}

void Dialog_PrintOptions::footerRight()
{
   QString text = m_ui->footer_Right->text();
   macroMenu( m_ui->footerRight_TB);

   if (! m_menuText.isEmpty()) {
      text = text + m_menuText;
      m_ui->footer_Right->setText(text);
   }
}

void Dialog_PrintOptions::fontHeader()
{
   bool ok;
   QFont font = QFontDialog::getFont(&ok, m_print.fontHeader, this);

   if (ok) {
      m_print.fontHeader = font;
      m_ui->fontHeader->setText(font.toString());
   }
}

void Dialog_PrintOptions::fontFooter()
{
   bool ok;
   QFont font = QFontDialog::getFont(&ok, m_print.fontFooter, this);

   if (ok) {
      m_print.fontFooter = font;
      m_ui->fontFooter->setText(font.toString());
   }
}

void Dialog_PrintOptions::fontText()
{
   bool ok;
   QFont font = QFontDialog::getFont(&ok, m_print.fontText, this);

   if (ok) {
      m_print.fontText = font;
      m_ui->fontText->setText(font.toString());
   }
}

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
   m_menuText = "$(PageNo) of $(TotalPages)";
}

void Dialog_PrintOptions::date()
{
   m_menuText = "$(Date)";
}

void Dialog_PrintOptions::time()
{
   m_menuText = "$(Time)";
}


