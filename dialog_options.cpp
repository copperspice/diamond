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

#include "dialog_options.h"
#include "util.h"

#include <QFileDialog>
#include <QLineEdit>
#include <QString>
#include <QStringList>

Dialog_Options::Dialog_Options(MainWindow *from, struct Options data)
   : m_ui(new Ui::Dialog_Options)
{
   m_parent   = from;
   m_options  = data;

   m_ui->setupUi(this);

   initData();

   connect(m_ui->dictMain_TB, SIGNAL(clicked()), this, SLOT(pick_Main()));
   connect(m_ui->dictUser_TB, SIGNAL(clicked()), this, SLOT(pick_User()));
   connect(m_ui->about_TB,    SIGNAL(clicked()), this, SLOT(pick_About()));

   connect(m_ui->save_PB,     SIGNAL(clicked()), this, SLOT(Save()));
   connect(m_ui->cancel_PB,   SIGNAL(clicked()), this, SLOT(Cancel()));
}

Dialog_Options::~Dialog_Options()
{
   delete m_ui;
}

void Dialog_Options::initData()
{
   QStringList list;
   int index;

   // 1
   list << "MM/dd/yyyy" << "dd/MM/yyyy" << "MMM dd, yyyy" << "MMMM dd, yyyy"
        << "yyyyMMdd";

   m_ui->dateFormat_CB->addItems(list);
   m_ui->dateFormat_CB->setEditable(false);

   index = m_ui->dateFormat_CB->findText(m_options.formatDate);
   m_ui->dateFormat_CB->setCurrentIndex(index);

   // 2
   list.clear();
   list << "hh:mm" << "hh:mm:ss" << "h:m:s ap" << "h:mm ap";

   m_ui->timeFormat_CB->addItems(list);
   m_ui->timeFormat_CB->setEditable(false);

   index = m_ui->timeFormat_CB->findText(m_options.formatTime);
   m_ui->timeFormat_CB->setCurrentIndex(index);

   // 3
   list.clear();
   list << "3" << "4" << "8";
   m_ui->tabSpacing_CB->addItems(list);
   m_ui->tabSpacing_CB->setEditable(false);

   index = m_ui->tabSpacing_CB->findText(QString::number(m_options.tabSpacing));
   m_ui->tabSpacing_CB->setCurrentIndex(index);

   //
   m_ui->dictMain->setText(m_options.dictMain);
   m_ui->dictUser->setText(m_options.dictUser);
   m_ui->about->setText(m_options.aboutUrl);
}

void Dialog_Options::Save()
{
   this->done(QDialog::Accepted);
}

void Dialog_Options::Cancel()
{
   this->done(QDialog::Rejected);
}

void Dialog_Options::pick_Main()
{
   QFileDialog::Options options;

   if (false)  {  //(Q_OS_DARWIM) {
      options |= QFileDialog::DontUseNativeDialog;
   }

   QString selectedFilter;

   QString fileName = QFileDialog::getOpenFileName(this, tr("Select Dictionary"),
         m_ui->dictMain->text(), tr("All Files (*)"), &selectedFilter, options);

   if (! fileName.isEmpty()) {
      m_ui->dictMain->setText(fileName);
   }
}

void Dialog_Options::pick_User()
{
   QFileDialog::Options options;

   if (false)  {  //(Q_OS_DARWIM) {
      options |= QFileDialog::DontUseNativeDialog;
   }

   QString selectedFilter;

   QString fileName = QFileDialog::getOpenFileName(this, tr("Select User Dictionary"),
         m_ui->dictUser->text(), tr("All Files (*)"), &selectedFilter, options);

   if (! fileName.isEmpty()) {
      m_ui->dictUser->setText(fileName);
   }
}

void Dialog_Options::pick_About()
{
   QFileDialog::Options options;

   if (false)  {  //(Q_OS_DARWIM) {
      options |= QFileDialog::DontUseNativeDialog;
   }

   QString selectedFilter;

   QString fileName = QFileDialog::getOpenFileName(this, tr("Select Diamond About"),
         m_ui->about->text(), tr("All Files (*)"), &selectedFilter, options);

   if (! fileName.isEmpty()) {
      m_ui->about->setText(fileName);
   }
}


struct Options Dialog_Options::get_Results()
{
   QString value = m_ui->tabSpacing_CB->currentText();
   m_options.tabSpacing = value.toInt();

   m_options.formatDate = m_ui->dateFormat_CB->currentText();
   m_options.formatTime = m_ui->timeFormat_CB->currentText();
   m_options.dictMain   = m_ui->dictMain->text();
   m_options.dictUser   = m_ui->dictUser->text();
   m_options.aboutUrl   = m_ui->about->text();

   return m_options;
}












