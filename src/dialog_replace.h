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

#ifndef DIALOG_REPLACE_H
#define DIALOG_REPLACE_H

#include "mainwindow.h"
#include "ui_dialog_replace.h"

#include <QDialog>
#include <QPoint>
#include <QString>
#include <QStringList>

class Dialog_Replace : public QDialog
{
   CS_OBJECT(Dialog_Replace)

   public:
      Dialog_Replace(MainWindow *parent, QString findText, QStringList findList,
            QString replaceText, QStringList replaceList);
      ~Dialog_Replace();

      QString get_findText();
      QStringList get_findList();
      QString get_replaceText();
      QStringList get_replaceList();

      bool get_MatchCase();
      bool get_WholeWords();
      bool get_Upd_Find();
      bool get_Upd_Replace();

   private:
      void replace();
      void replaceAll();
      void cancel();

      void combo_ContextMenu_F(const QPoint &pt);
      void combo_ContextMenu_R(const QPoint &pt);

      void setUp();

      CS_SLOT_1(Private, void menu_clearList_F())
      CS_SLOT_2(menu_clearList_F)

      CS_SLOT_1(Private, void menu_deleteEntry_F())
      CS_SLOT_2(menu_deleteEntry_F)

      CS_SLOT_1(Private, void menu_clearList_R())
      CS_SLOT_2(menu_clearList_R)

      CS_SLOT_1(Private, void menu_deleteEntry_R())
      CS_SLOT_2(menu_deleteEntry_R)

      Ui::Dialog_Replace *m_ui;
      QStringList m_findList;
      QStringList m_replaceList;

      bool m_upd_Find;
      bool m_upd_Replace;
};

#endif
