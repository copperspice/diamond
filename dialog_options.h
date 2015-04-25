/**************************************************************************
*
* Copyright (c) 2012-2015 Barbara Geller
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

#ifndef DIALOG_OPTIONS_H
#define DIALOG_OPTIONS_H

#include "ui_dialog_options.h"
#include "mainwindow.h"
#include "settings.h"

#include <QDialog>
#include <QStringList>

class Dialog_Options : public QDialog
{
   CS_OBJECT(Dialog_Options)

   public:
      Dialog_Options(MainWindow *parent, struct Options data);
      ~Dialog_Options();

      struct Options get_Results();

   private:
      Ui::Dialog_Options *m_ui;
      MainWindow *m_parent;

      struct Options m_options;
      void initData();

      CS_SLOT_1(Private, void pick_Main())
      CS_SLOT_2(pick_Main) 

      CS_SLOT_1(Private, void pick_User())
      CS_SLOT_2(pick_User) 

      CS_SLOT_1(Private, void pick_Syntax())
      CS_SLOT_2(pick_Syntax) 

      CS_SLOT_1(Private, void pick_About())
      CS_SLOT_2(pick_About) 

      CS_SLOT_1(Private, void reset_StandardKey())
      CS_SLOT_2(reset_StandardKey) 

      CS_SLOT_1(Private, void Save())
      CS_SLOT_2(Save) 

      CS_SLOT_1(Private, void Cancel())
      CS_SLOT_2(Cancel) 
};

#endif
