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

#ifndef DIALOG_PRESET_H
#define DIALOG_PRESET_H

#include "mainwindow.h"
#include "ui_dialog_preset.h"

#include <QDialog>
#include <QString>
#include <QStringList>

class Dialog_Preset : public QDialog
{
   CS_OBJECT(Dialog_Preset)

   public:
      Dialog_Preset(MainWindow *parent, QStringList folders);
      ~Dialog_Preset();

      QStringList getData();

   private:
      void verify_Folder(QLineEdit *field);
      void pick_Folder(QString number, QLineEdit *field);

      void save();
      void cancel();

      Ui::Dialog_Preset *m_ui;

      QPalette m_editPalette;
      QStringList m_folderList;
      MainWindow *m_parent;
};

#endif
