/**************************************************************************
*
* Copyright (c) 2012-2017 Barbara Geller
* All rights reserved.
*
* Diamond Editor is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License version 2
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

#ifndef DIALOG_MACRO_H
#define DIALOG_MACRO_H

#include "ui_dialog_macro.h"
#include "mainwindow.h"

#include <QDialog>
#include <QModelIndex>
#include <QStringList>
#include <QStandardItemModel>

class Dialog_Macro : public QDialog
{
   CS_OBJECT(Dialog_Macro)

   public:
      enum MacroEnum { MACRO_LOAD, MACRO_SAVE, MACRO_EDITNAMES };

      Dialog_Macro(MainWindow *parent, MacroEnum enumValue, QStringList macroIds, QStringList macroNames);
      ~Dialog_Macro();
      QString get_Macro();

   private:
      Ui::Dialog_Macro *m_ui;
      MainWindow *m_parent;
      QStandardItemModel *m_model;

      QStringList m_macroIds_D;
      QStringList m_macroNames_D;
      int m_maxCount;
      bool m_updateNames = false;

      void setupTitle();
      void setUpView();

      MacroEnum m_enum;
      QStringList m_macroList;

      void select();   
      void view();    
      void cancel();    
      void tableDataChanged(const QModelIndex & topLeft,const QModelIndex & bottomRight);      
};

#endif
