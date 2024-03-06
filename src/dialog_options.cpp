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

#include "dialog_options.h"
#include "util.h"

#include <QFileDialog>
#include <QGlobal>
#include <QKeySequence>
#include <QLineEdit>
#include <QString>
#include <QToolButton>

Dialog_Options::Dialog_Options(MainWindow *parent, struct Options data)
   : QDialog(parent), m_ui(new Ui::Dialog_Options)
{
   m_parent   = parent;
   m_options  = data;

   m_ui->setupUi(this);
   this->setWindowIcon(QIcon("://resources/diamond.png"));

   initData();

   connect(m_ui->dictMain_TB, &QToolButton::clicked, this, &Dialog_Options::pick_Main);
   connect(m_ui->dictUser_TB, &QToolButton::clicked, this, &Dialog_Options::pick_User);
   connect(m_ui->syntax_TB,   &QToolButton::clicked, this, &Dialog_Options::pick_Syntax);

   connect(m_ui->reset_PB,    &QPushButton::clicked, this, &Dialog_Options::reset_StandardKey);
   connect(m_ui->save_PB,     &QPushButton::clicked, this, &Dialog_Options::save);
   connect(m_ui->cancel_PB,   &QPushButton::clicked, this, &Dialog_Options::cancel);
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

   if (m_options.removeSpace)  {
      m_ui->removeSpace_CKB->setChecked(true);
   }

   if (m_options.autoLoad)  {
      m_ui->autoLoad_CKB->setChecked(true);
   }

   m_ui->dictMain->setText(m_options.dictMain);
   m_ui->dictMain->setCursorPosition(0);

   m_ui->dictUser->setText(m_options.dictUser);
   m_ui->dictUser->setCursorPosition(0);

   m_ui->syntax->setText(m_options.pathSyntax);
   m_ui->syntax->setCursorPosition(0);

   // ** tab two
   m_ui->key_open->setText(m_options.key_open);
   m_ui->key_close->setText(m_options.key_close);
   m_ui->key_save->setText(m_options.key_save);
   m_ui->key_saveAs->setText(m_options.key_saveAs);
   m_ui->key_print->setText(m_options.key_print);
   m_ui->key_undo->setText(m_options.key_undo);
   m_ui->key_redo->setText(m_options.key_redo);
   m_ui->key_cut->setText(m_options.key_cut);
   m_ui->key_copy->setText(m_options.key_copy);
   m_ui->key_paste->setText(m_options.key_paste);
   m_ui->key_selectAll->setText(m_options.key_selectAll);
   m_ui->key_find->setText(m_options.key_find);
   m_ui->key_replace->setText(m_options.key_replace);
   m_ui->key_findNext->setText(m_options.key_findNext);
   m_ui->key_findPrev->setText(m_options.key_findPrev);
   m_ui->key_goTop->setText(m_options.key_goTop);
   m_ui->key_goBottom->setText(m_options.key_goBottom);
   m_ui->key_newTab->setText(m_options.key_newTab);

   // ** tab three
   m_ui->key_printPreview->setText(m_options.key_printPreview);
   m_ui->key_reload->setText(m_options.key_reload);
   m_ui->key_selectLine->setText(m_options.key_selectLine);
   m_ui->key_selectWord->setText(m_options.key_selectWord);
   m_ui->key_selectBlock->setText(m_options.key_selectBlock);
   m_ui->key_upper->setText(m_options.key_upper);
   m_ui->key_lower->setText(m_options.key_lower);
   m_ui->key_indentIncr->setText(m_options.key_indentIncr);
   m_ui->key_indentDecr->setText(m_options.key_indentDecr);
   m_ui->key_deleteLine->setText(m_options.key_deleteLine);
   m_ui->key_deleteEOL->setText(m_options.key_deleteEOL);
   m_ui->key_columnMode->setText(m_options.key_columnMode);
   m_ui->key_goLine->setText(m_options.key_goLine);
   m_ui->key_show_Spaces->setText(m_options.key_show_Spaces);
   m_ui->key_show_Breaks->setText(m_options.key_show_Breaks);
   m_ui->key_macroPlay->setText(m_options.key_macroPlay);
   m_ui->key_spellCheck->setText(m_options.key_spellCheck);
   m_ui->key_copyBuffer->setText(m_options.key_copyBuffer);
}

void Dialog_Options::save()
{
   this->done(QDialog::Accepted);
}

void Dialog_Options::cancel()
{
   this->done(QDialog::Rejected);
}

void Dialog_Options::pick_Main()
{
   QString selectedFilter;

   QString fileName = QFileDialog::getOpenFileName(this, tr("Select Main Dictionary"),
         m_ui->dictMain->text(), tr("Dictionary File (*.dic)"), &selectedFilter);

   if (! fileName.isEmpty()) {
      m_ui->dictMain->setText(fileName);
   }
}

void Dialog_Options::pick_User()
{
   QString selectedFilter;

   QString fileName = QFileDialog::getOpenFileName(this, tr("Select User Dictionary"),
         m_ui->dictUser->text(), tr("User Dictionary File (*.txt)"), &selectedFilter);

   if (! fileName.isEmpty()) {
      m_ui->dictUser->setText(fileName);
   }
}

void Dialog_Options::pick_Syntax()
{
   QString msg  = tr("Select Diamond Syntax Folder");
   QString path = m_ui->syntax->text();

   path = m_parent->get_DirPath(msg, path);

   if (! path.isEmpty()) {
      m_ui->syntax->setText(path);
   }
}

struct Options Dialog_Options::get_Results()
{
   // ** tab 1
   m_options.formatDate  = m_ui->dateFormat_CB->currentText();
   m_options.formatTime  = m_ui->timeFormat_CB->currentText();
   m_options.dictMain    = m_ui->dictMain->text();
   m_options.dictUser    = m_ui->dictUser->text();
   m_options.pathSyntax  = m_ui->syntax->text();

   QString value = m_ui->tabSpacing_CB->currentText();
   m_options.tabSpacing  = value.toInteger<int>();

   m_options.useSpaces   = m_ui->useSpaces_CKB->isChecked();
   m_options.removeSpace = m_ui->removeSpace_CKB->isChecked();
   m_options.autoLoad    = m_ui->autoLoad_CKB->isChecked();

   // ** tab 2
   m_options.key_open         = m_ui->key_open->text();
   m_options.key_close        = m_ui->key_close->text();
   m_options.key_save         = m_ui->key_save->text();
   m_options.key_saveAs       = m_ui->key_saveAs->text();
   m_options.key_print        = m_ui->key_print->text();
   m_options.key_undo         = m_ui->key_undo->text();
   m_options.key_redo         = m_ui->key_redo->text();
   m_options.key_cut          = m_ui->key_cut->text();
   m_options.key_copy         = m_ui->key_copy->text();
   m_options.key_paste        = m_ui->key_paste->text();
   m_options.key_selectAll    = m_ui->key_selectAll->text();
   m_options.key_find         = m_ui->key_find->text();
   m_options.key_replace      = m_ui->key_replace->text();
   m_options.key_findNext     = m_ui->key_findNext->text();
   m_options.key_findPrev     = m_ui->key_findPrev->text();
   m_options.key_goTop        = m_ui->key_goTop->text();
   m_options.key_goBottom     = m_ui->key_goBottom->text();
   m_options.key_newTab       = m_ui->key_newTab->text();

   // ** tab 3
   m_options.key_printPreview = m_ui->key_printPreview->text();
   m_options.key_reload       = m_ui->key_reload->text();
   m_options.key_selectLine   = m_ui->key_selectLine->text();
   m_options.key_selectWord   = m_ui->key_selectWord->text();
   m_options.key_selectBlock  = m_ui->key_selectBlock->text();
   m_options.key_upper        = m_ui->key_upper->text();
   m_options.key_lower        = m_ui->key_lower->text();
   m_options.key_indentIncr   = m_ui->key_indentIncr->text();
   m_options.key_indentDecr   = m_ui->key_indentDecr->text();
   m_options.key_deleteLine   = m_ui->key_deleteLine->text();
   m_options.key_deleteEOL    = m_ui->key_deleteEOL->text();
   m_options.key_columnMode   = m_ui->key_columnMode->text();
   m_options.key_goLine       = m_ui->key_goLine->text();
   m_options.key_show_Spaces  = m_ui->key_show_Spaces->text();
   m_options.key_show_Breaks  = m_ui->key_show_Breaks->text();
   m_options.key_macroPlay    = m_ui->key_macroPlay->text();
   m_options.key_spellCheck   = m_ui->key_spellCheck->text();
   m_options.key_copyBuffer   = m_ui->key_copyBuffer->text();

   return m_options;
}

void Dialog_Options::reset_StandardKey()
{
   m_options.key_open      = QKeySequence(QKeySequence::Open).toString(QKeySequence::NativeText);
   m_options.key_close     = QKeySequence(QKeySequence::Close).toString(QKeySequence::NativeText);
   m_options.key_save      = QKeySequence(QKeySequence::Save).toString(QKeySequence::NativeText);
   m_options.key_saveAs    = QKeySequence(QKeySequence::SaveAs).toString(QKeySequence::NativeText);
   m_options.key_print     = QKeySequence(QKeySequence::Print).toString(QKeySequence::NativeText);
   m_options.key_undo      = QKeySequence(QKeySequence::Undo).toString(QKeySequence::NativeText);
   m_options.key_redo      = QKeySequence(QKeySequence::Redo).toString(QKeySequence::NativeText);
   m_options.key_cut       = QKeySequence(QKeySequence::Cut).toString(QKeySequence::NativeText);
   m_options.key_copy      = QKeySequence(QKeySequence::Copy).toString(QKeySequence::NativeText);
   m_options.key_paste     = QKeySequence(QKeySequence::Paste).toString(QKeySequence::NativeText);
   m_options.key_selectAll = QKeySequence(QKeySequence::SelectAll).toString(QKeySequence::NativeText);
   m_options.key_find      = QKeySequence(QKeySequence::Find).toString(QKeySequence::NativeText);
   m_options.key_replace   = QKeySequence(QKeySequence::Replace).toString(QKeySequence::NativeText);
   m_options.key_findNext  = QKeySequence(QKeySequence::FindNext).toString(QKeySequence::NativeText);
   m_options.key_findPrev  = QKeySequence(QKeySequence::FindPrevious).toString(QKeySequence::NativeText);
   m_options.key_goTop     = QKeySequence(QKeySequence::MoveToStartOfDocument).toString(QKeySequence::NativeText);
   m_options.key_goBottom  = QKeySequence(QKeySequence::MoveToEndOfDocument).toString(QKeySequence::NativeText);
   m_options.key_newTab    = QKeySequence(QKeySequence::AddTab).toString(QKeySequence::NativeText);

   m_ui->key_open->setText(m_options.key_open);
   m_ui->key_close->setText(m_options.key_close);
   m_ui->key_save->setText(m_options.key_save);
   m_ui->key_saveAs->setText(m_options.key_saveAs);
   m_ui->key_print->setText(m_options.key_print);
   m_ui->key_undo->setText(m_options.key_undo);
   m_ui->key_redo->setText(m_options.key_redo);
   m_ui->key_cut->setText(m_options.key_cut);
   m_ui->key_copy->setText(m_options.key_copy);
   m_ui->key_paste->setText(m_options.key_paste);
   m_ui->key_selectAll->setText(m_options.key_selectAll);
   m_ui->key_find->setText(m_options.key_find);
   m_ui->key_replace->setText(m_options.key_replace);
   m_ui->key_findNext->setText(m_options.key_findNext);
   m_ui->key_findPrev->setText(m_options.key_findPrev);
   m_ui->key_goTop->setText(m_options.key_goTop);
   m_ui->key_goBottom->setText(m_options.key_goBottom);
   m_ui->key_newTab->setText(m_options.key_newTab);
}

