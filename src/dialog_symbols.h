/**************************************************************************
*
* Copyright (c) 2012-2016 Barbara Geller
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

#ifndef DIALOG_SYMBOLS_H
#define DIALOG_SYMBOLS_H

#include "ui_dialog_symbols.h"
#include "mainwindow.h"

#include <QDialog>

class Dialog_Symbols : public QDialog
{
   CS_OBJECT(Dialog_Symbols)

   public:
      Dialog_Symbols(MainWindow *parent);
      ~Dialog_Symbols();
      QString get_Symbol();

   private:
      Ui::Dialog_Symbols *m_ui;
      MainWindow *m_parent;
      QStringList initData();

      void insert();
      void cancel();
};

#endif
