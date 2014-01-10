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

#ifndef DIALOG_PRINT_OPT_H
#define DIALOG_PRINT_OPT_H

#include "ui_dialog_print_opt.h"
#include "mainwindow.h"
#include "settings.h"

#include <QDialog>
#include <QToolButton>

class Dialog_PrintOptions: public QDialog
{
   Q_OBJECT

   public:
      Dialog_PrintOptions(MainWindow *from, struct PrintSettings data);
      ~Dialog_PrintOptions();

      struct PrintSettings get_Results();

   private:
      Ui::Dialog_PrintOptions *m_ui;
      MainWindow *m_parent;

      struct PrintSettings m_print;      
      QString m_menuText;

      void initData();
      void macroMenu(QToolButton *widget);

   private slots:
      void Save();
      void Cancel();

      void fileName();
      void pathFileName();
      void pageNumber();
      void totalPages();
      void pages();
      void date();
      void time();

      void headerLeft();
      void headerCenter();
      void headerRight();
      void footerLeft();
      void footerCenter();
      void footerRight();      

      void fontHeader();
      void fontFooter();
      void fontText();
};

#endif
