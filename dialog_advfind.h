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

#ifndef DIALOG_ADVFIND_H
#define DIALOG_ADVFIND_H

#include "ui_dialog_advfind.h"
#include "mainwindow.h"

#include <QDialog>
#include <QString>

class Dialog_AdvFind : public QDialog
{     
   CS_OBJECT(Dialog_AdvFind)

   public:
      Dialog_AdvFind(MainWindow *from, QString text, QString fileType, QString findFolder, bool searchFolders);
      ~Dialog_AdvFind();

      QString get_findText();      
      QString get_findType();
      QString get_findFolder();

      bool get_Case();
      bool get_WholeWords();
      bool get_SearchSubFolders();

   private:
      Ui::Dialog_AdvFind *m_ui;
      MainWindow *m_parent;

      CS_SLOT_1(Private, void pick_Folder())
      CS_SLOT_2(pick_Folder) 

      CS_SLOT_1(Private, void Find())
      CS_SLOT_2(Find) 

      CS_SLOT_1(Private, void Cancel())
      CS_SLOT_2(Cancel) 
};

#endif
