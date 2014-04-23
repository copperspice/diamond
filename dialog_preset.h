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

#ifndef DIALOG_PRESET_H
#define DIALOG_PRESET_H

#include "ui_dialog_preset.h"
#include "mainwindow.h"

#include <QDialog>
#include <QString>
#include <QStringList>

class Dialog_Preset : public QDialog
{     
   CS_OBJECT(Dialog_Preset)

   public:
      Dialog_Preset(MainWindow *from, QStringList data);
      ~Dialog_Preset();

      QStringList getData();

   private:
      Ui::Dialog_Preset *m_ui;
      QStringList m_dataList;
      MainWindow *m_parent;

      void pick_Folder(QString number, QLineEdit *field);

      CS_SLOT_1(Private, void save())
      CS_SLOT_2(save)

      CS_SLOT_1(Private, void cancel())
      CS_SLOT_2(cancel)

      CS_SLOT_1(Private, void folder1_TB())
      CS_SLOT_2(folder1_TB)

      CS_SLOT_1(Private, void folder2_TB())
      CS_SLOT_2(folder2_TB)

      CS_SLOT_1(Private, void folder3_TB())
      CS_SLOT_2(folder3_TB)

      CS_SLOT_1(Private, void folder4_TB())
      CS_SLOT_2(folder4_TB)

      CS_SLOT_1(Private, void folder5_TB())
      CS_SLOT_2(folder5_TB)

      CS_SLOT_1(Private, void folder6_TB())
      CS_SLOT_2(folder6_TB)

      CS_SLOT_1(Private, void folder7_TB())
      CS_SLOT_2(folder7_TB)

      CS_SLOT_1(Private, void folder8_TB())
      CS_SLOT_2(folder8_TB)
};

#endif
