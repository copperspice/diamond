/**************************************************************************
*
* Copyright (c) 2012-2013 Barbara Geller
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

#ifndef DIALOG_REPLACE_H
#define DIALOG_REPLACE_H

#include "ui_dialog_replace.h"

#include <QDialog>
#include <QPoint>
#include <QString>
#include <QStringList>

class Dialog_Replace : public QDialog
{     
   Q_OBJECT

   public:      
      Dialog_Replace(QString findText, QStringList findList,
                     QString replaceText, QStringList replaceList);
      ~Dialog_Replace();

      QString get_findText();
      QStringList get_findList();
      QString get_replaceText();
      QStringList get_replaceList();

      bool get_Case();
      bool get_WholeWords();

   private:
      Ui::Dialog_Replace *m_ui;
      QStringList m_findList;
      QStringList m_replaceList;
      void setUp();

   private slots:
      void replace();
      void replaceAll();
      void cancel();

      void combo_ContextMenu_F(const QPoint &pt);
      void menu_clearList_F();
      void menu_deleteEntry_F();

      void combo_ContextMenu_R(const QPoint &pt);
      void menu_clearList_R();
      void menu_deleteEntry_R();
};

#endif
