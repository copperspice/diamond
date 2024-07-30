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

#include "dialog_find.h"
#include "util.h"

#include <QComboBox>
#include <QCompleter>
#include <QCursor>
#include <QLineEdit>
#include <QMenu>
#include <QTextCursor>

Dialog_Find::Dialog_Find(MainWindow *parent, QString findText, QStringList findList)
   : QDialog(parent), m_ui(new Ui::Dialog_Find)
{
   m_findList = findList;
   m_upd_Find = false;

   m_ui->setupUi(this);
   setWindowIcon(QIcon("://resources/diamond.png"));

   setUp();

   m_ui->find_Combo->completer()->setCaseSensitivity(Qt::CaseSensitive);

   // display last edit value
   m_ui->find_Combo->lineEdit()->setText(findText);

   // any key deletes first, right arrow to continue typing
   m_ui->find_Combo->lineEdit()->selectAll();

   connect(m_ui->find_PB,   &QPushButton::clicked, this, &Dialog_Find::find);
   connect(m_ui->cancel_PB, &QPushButton::clicked, this, &Dialog_Find::cancel);
}

Dialog_Find::~Dialog_Find()
{
   delete m_ui;
}

void Dialog_Find::setUp()
{
   m_ui->down_RB->setChecked(true);

   m_ui->find_Combo->setEditable(true);
   m_ui->find_Combo->addItems(m_findList);

   // add a context menu
   m_ui->find_Combo->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(m_ui->find_Combo, &QComboBox::customContextMenuRequested, this, &Dialog_Find::combo_ContextMenu);
}

void Dialog_Find::combo_ContextMenu(const QPoint &pt)
{
   (void) pt;

   QMenu *menu = m_ui->find_Combo->lineEdit()->createStandardContextMenu();

   menu->addSeparator();
   menu->addAction("Clear Find List",   this, SLOT(menu_clearList())   );
   menu->addAction("Delete Find Entry", this, SLOT(menu_deleteEntry()) );
   menu->popup(QCursor::pos());

   // takes care of deleting the menu after displayed, avoids a memory leak
   connect(menu, &QMenu::aboutToHide, menu, &Dialog_Find::deleteLater);
}

void Dialog_Find::menu_clearList()
{
   m_findList.clear();
   m_ui->find_Combo->clear();

   m_upd_Find = true;
}

void Dialog_Find::menu_deleteEntry()
{
   QString text = m_ui->find_Combo->lineEdit()->text();
   m_findList.removeOne(text);

   int index = m_ui->find_Combo->currentIndex();
   m_ui->find_Combo->removeItem(index);

   m_upd_Find = true;
}

void Dialog_Find::find()
{
   done(1);
}

void Dialog_Find::cancel()
{
   done(0);
}

QString Dialog_Find::get_findText()
{
   return m_ui->find_Combo->currentText();
}

QStringList Dialog_Find::get_findList()
{
   return m_findList;
}

bool Dialog_Find::get_Direction()
{
   if (m_ui->down_RB->isChecked()) {
      return true;
   } else {
      return false;
   }
}

bool Dialog_Find::get_MatchCase()
{
   return m_ui->matchCase_CKB->isChecked();
}

bool Dialog_Find::get_WholeWords()
{
   return m_ui->wholeWords_CKB->isChecked();
}

bool Dialog_Find::get_Upd_Find()
{
   return m_upd_Find;
}
