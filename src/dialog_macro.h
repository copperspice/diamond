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

#ifndef DIALOG_MACRO_H
#define DIALOG_MACRO_H

#include "mainwindow.h"
#include "ui_dialog_macro.h"

#include <QDialog>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QStringList>

class Dialog_Macro : public QDialog
{
   CS_OBJECT(Dialog_Macro)

   public:
      Dialog_Macro(MainWindow *parent, MainWindow::Option enumValue, QStringList macroIds,
            QStringList macroText, QStringList macroNames);
      ~Dialog_Macro();

      QString get_MacroId();

   private:
      void setUpView();

      QString get_MacroName();

      void close();
      void saveTags();
      void saveMacro();
      void loadMacro();
      void viewMacro();

      Ui::Dialog_Macro *m_ui;
      MainWindow *m_parent;
      QStandardItemModel *m_model;

      QStringList m_macroIds;
      QStringList m_macroText;
      QStringList m_macroNames;
      QStringList m_macroList;
      int m_maxCount;

      MainWindow::Option m_enum;
};

#endif
