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

#include "dialog_replace.h"
#include "util.h"

#include <QComboBox>
#include <QCursor>
#include <QLineEdit>
#include <QMenu>
#include <QTextCursor>

Dialog_Replace::Dialog_Replace(MainWindow *parent, QString findText, QStringList findList,
                               QString replaceText, QStringList replaceList)
   : QDialog(parent), m_ui(new Ui::Dialog_Replace)
{
   m_findList    = findList;
   m_replaceList = replaceList;
   m_upd_Find    = false;
   m_upd_Replace = false;

   m_ui->setupUi(this);
   this->setWindowIcon(QIcon("://resources/diamond.png"));

   this->setUp();

   // display last edit value
   m_ui->find_Combo->lineEdit()->setText(findText);
   m_ui->replace_Combo->lineEdit()->setText(replaceText);

   // any key deletes first, right arrow to continue typing
   m_ui->find_Combo->lineEdit()->selectAll();
   m_ui->replace_Combo->lineEdit()->selectAll();

   connect(m_ui->replace_PB,     &QPushButton::clicked, this, &Dialog_Replace::replace);
   connect(m_ui->replaceAll_PB,  &QPushButton::clicked, this, &Dialog_Replace::replaceAll);
   connect(m_ui->cancel_PB,      &QPushButton::clicked, this, &Dialog_Replace::cancel);
}

Dialog_Replace::~Dialog_Replace()
{
   delete m_ui;
}

void Dialog_Replace::setUp()
{
   m_ui->find_Combo->setEditable(true);
   m_ui->find_Combo->addItems(m_findList);

   m_ui->replace_Combo->setEditable(true);
   m_ui->replace_Combo->addItems(m_replaceList);

   // add a context menu
   m_ui->find_Combo->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(m_ui->find_Combo,    &QComboBox::customContextMenuRequested, this, &Dialog_Replace::combo_ContextMenu_F);

   m_ui->replace_Combo->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(m_ui->replace_Combo, &QComboBox::customContextMenuRequested, this, &Dialog_Replace::combo_ContextMenu_R);
}

void Dialog_Replace::combo_ContextMenu_F(const QPoint &pt)
{
   QMenu *menu = m_ui->find_Combo->lineEdit()->createStandardContextMenu();

   menu->addSeparator();
   menu->addAction("Clear Find List",   this, SLOT(menu_clearList_F()) );
   menu->addAction("Delete Find Entry", this, SLOT(menu_deleteEntry_F()) );
   menu->popup(QCursor::pos());

   // deletes menu after displayed
   connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
}

void Dialog_Replace::combo_ContextMenu_R(const QPoint &pt)
{
   QMenu *menu = m_ui->replace_Combo->lineEdit()->createStandardContextMenu();

   menu->addSeparator();
   menu->addAction("Clear Replace List",   this, SLOT(menu_clearList_R()) );
   menu->addAction("Delete Replace Entry", this, SLOT(menu_deleteEntry_R()) );
   menu->popup(QCursor::pos());

   // deletes menu after displayed
   connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
}

void Dialog_Replace::menu_clearList_F()
{
   m_findList.clear();
   m_ui->find_Combo->clear();

   m_upd_Find = true;
}

void Dialog_Replace::menu_clearList_R()
{
   m_replaceList.clear();
   m_ui->replace_Combo->clear();

   m_upd_Replace = true;
}

void Dialog_Replace::menu_deleteEntry_F()
{
   QString text = m_ui->find_Combo->lineEdit()->text();
   m_findList.removeOne(text);

   int index = m_ui->find_Combo->currentIndex();
   m_ui->find_Combo->removeItem(index);
}

void Dialog_Replace::menu_deleteEntry_R()
{
   QString text = m_ui->replace_Combo->lineEdit()->text();
   m_replaceList.removeOne(text);

   int index = m_ui->replace_Combo->currentIndex();
   m_ui->replace_Combo->removeItem(index);
}


void Dialog_Replace::replace()
{
   this->done(1);
}

void Dialog_Replace::replaceAll()
{
   this->done(2);
}

void Dialog_Replace::cancel()
{
   this->done(0);
}

QString Dialog_Replace::get_findText()
{
   return m_ui->find_Combo->currentText();
}

QStringList Dialog_Replace::get_findList()
{
   return m_findList;
}

QString Dialog_Replace::get_replaceText()
{
   return m_ui->replace_Combo->currentText();
}

QStringList Dialog_Replace::get_replaceList()
{
   return m_replaceList;
}

bool Dialog_Replace::get_Case()
{
   return m_ui->case_CKB->isChecked();
}

bool Dialog_Replace::get_WholeWords()
{
   return m_ui->wholeWords_CKB->isChecked();
}

bool Dialog_Replace::get_Upd_Find()
{
   return m_upd_Find;
}

bool Dialog_Replace::get_Upd_Replace()
{
   return m_upd_Replace;
}

