/**************************************************************************
*
* Copyright (c) 2012-2025 Barbara Geller
*
* Diamond Editor is free software. You can redistribute it and/or
* modify it under the terms of the GNU General Public License
* version 2 as published by the Free Software Foundation.
*
* Diamond Editor is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* https://www.gnu.org/licenses/
*
***************************************************************************/

#ifndef DIALOG_FIND_H
#define DIALOG_FIND_H

#include "mainwindow.h"
#include "ui_dialog_find.h"

#include <QDialog>
#include <QPoint>
#include <QString>
#include <QStringList>

class Dialog_Find : public QDialog
{
   CS_OBJECT(Dialog_Find)

   public:
      Dialog_Find(MainWindow *parent, QString text, QStringList findList);
      ~Dialog_Find();

      QString get_findText();
      QStringList get_findList();
      bool get_Direction();
      bool get_MatchCase();
      bool get_WholeWords();
      bool get_Upd_Find();

   private:
      void setUp();
      void find();
      void cancel();

      void combo_ContextMenu(const QPoint & pt);

      CS_SLOT_1(Private, void menu_clearList())
      CS_SLOT_2(menu_clearList)

      CS_SLOT_1(Private, void menu_deleteEntry())
      CS_SLOT_2(menu_deleteEntry)

      Ui::Dialog_Find *m_ui;
      bool m_upd_Find;
      QStringList m_findList;
};

#endif
