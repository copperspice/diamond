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

#ifndef DIALOG_FIND_H
#define DIALOG_FIND_H

#include "ui_dialog_find.h"

#include <QDialog>
#include <QPoint>
#include <QString>
#include <QStringList>

class Dialog_Find : public QDialog
{
   CS_OBJECT(Dialog_Find)

   public:
      Dialog_Find(QString text, QStringList findList);
      ~Dialog_Find();

      QString get_findText();
      QStringList get_findList();
      bool get_Direction();
      bool get_Case();
      bool get_WholeWords();
      bool get_Upd_Find();

   private:
      Ui::Dialog_Find *m_ui;
      QStringList m_findList;
      void setUp();    
      bool m_upd_Find;

      CS_SLOT_1(Private, void Find())
      CS_SLOT_2(Find) 

      CS_SLOT_1(Private, void Cancel())
      CS_SLOT_2(Cancel) 

      CS_SLOT_1(Private, void combo_ContextMenu(const QPoint & pt))      
      CS_SLOT_2(combo_ContextMenu) 

      CS_SLOT_1(Private, void menu_clearList())
      CS_SLOT_2(menu_clearList) 

      CS_SLOT_1(Private, void menu_deleteEntry())
      CS_SLOT_2(menu_deleteEntry) 
};

#endif
