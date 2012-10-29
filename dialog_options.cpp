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
   connect(m_ui->syntax_TB,   SIGNAL(clicked()), this, SLOT(pick_Syntax()));
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

   // ** tab one

   // 1
   list << "MM/dd/yyyy" << "dd/MM/yyyy" << "MMM dd, yyyy"
        << "MMMM dd, yyyy" << "yyyyMMdd";

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

   if (m_options.useSpaces)  {
      m_ui->useSpaces_CKB->setChecked(true);
   }

   if (m_options.autoLoad)  {
      m_ui->autoLoad_CKB->setChecked(true);
   }

   // ** tab two
   m_ui->dictMain->setText(m_options.dictMain);
   m_ui->dictUser->setText(m_options.dictUser);
   m_ui->syntax->setText(m_options.pathSyntax);
   m_ui->about->setText(m_options.aboutUrl);

   m_ui->key_selectLine->setText(m_options.key_selectLine);
   m_ui->key_selectWord->setText(m_options.key_selectWord);
   m_ui->key_selectBlock->setText(m_options.key_selectBlock);
   m_ui->key_upper->setText(m_options.key_upper);
   m_ui->key_lower->setText(m_options.key_lower);  
   m_ui->key_indentIncr->setText(m_options.key_indentIncr);
   m_ui->key_indentDecr->setText(m_options.key_indentDecr);
   m_ui->key_deleteEOL->setText(m_options.key_deleteEOL);
   m_ui->key_columnMode->setText(m_options.key_columnMode);
   m_ui->key_goLine->setText(m_options.key_goLine);
   m_ui->key_macroPlay->setText(m_options.key_macroPlay);
   m_ui->key_spellCheck->setText(m_options.key_spellCheck);
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
         m_ui->dictMain->text(), tr("Dictionary Files (*.dic)"), &selectedFilter, options);

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
         m_ui->dictUser->text(), tr("User Dictionary Files (*.txt)"), &selectedFilter, options);

   if (! fileName.isEmpty()) {
      m_ui->dictUser->setText(fileName);
   }
}

void Dialog_Options::pick_Syntax()
{
   QString msg  = tr("Select locaton of Diamond Syntax Files");
   QString path = m_ui->syntax->text();

   path = m_parent->get_DirPath(msg, path);

   if (! path.isEmpty()) {
      m_ui->syntax->setText(path);
   }
}

void Dialog_Options::pick_About()
{
   QFileDialog::Options options;

   if (false)  {  //(Q_OS_DARWIM) {
      options |= QFileDialog::DontUseNativeDialog;
   }

   QString selectedFilter;

   QString fileName = QFileDialog::getOpenFileName(this, tr("Select Diamond Help (Html)"),
         m_ui->about->text(), tr("All Files (index.html)"), &selectedFilter, options);

   if (! fileName.isEmpty()) {
      m_ui->about->setText(fileName);
   }
}

struct Options Dialog_Options::get_Results()
{
   // ** tab 1
   m_options.formatDate = m_ui->dateFormat_CB->currentText();
   m_options.formatTime = m_ui->timeFormat_CB->currentText();
   m_options.dictMain   = m_ui->dictMain->text();
   m_options.dictUser   = m_ui->dictUser->text();
   m_options.pathSyntax = m_ui->syntax->text();
   m_options.aboutUrl   = m_ui->about->text();

   QString value = m_ui->tabSpacing_CB->currentText();
   m_options.tabSpacing = value.toInt();
   m_options.useSpaces  = m_ui->useSpaces_CKB->isChecked();

   m_options.autoLoad   = m_ui->autoLoad_CKB->isChecked();

   // ** tab 2
   m_options.key_selectLine   = m_ui->key_selectLine->text();
   m_options.key_selectWord   = m_ui->key_selectWord->text();
   m_options.key_selectBlock  = m_ui->key_selectBlock->text();
   m_options.key_upper        = m_ui->key_upper->text();
   m_options.key_lower        = m_ui->key_lower->text();
   m_options.key_indentIncr   = m_ui->key_indentIncr->text();
   m_options.key_indentDecr   = m_ui->key_indentDecr->text();
   m_options.key_deleteEOL    = m_ui->key_deleteEOL->text();
   m_options.key_columnMode   = m_ui->key_columnMode->text();
   m_options.key_goLine       = m_ui->key_goLine->text();
   m_options.key_macroPlay    = m_ui->key_macroPlay->text();
   m_options.key_spellCheck   = m_ui->key_spellCheck->text();

   return m_options;
}
















