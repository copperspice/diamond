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

#ifndef DIALOG_PRINT_OPT_H
#define DIALOG_PRINT_OPT_H

#include "ui_dialog_print_opt.h"
#include "mainwindow.h"
#include "settings.h"

#include <QDialog>
#include <QToolButton>

class Dialog_PrintOptions: public QDialog
{
   CS_OBJECT(Dialog_PrintOptions)

   public:
      Dialog_PrintOptions(MainWindow *parent, struct PrintSettings data);
      ~Dialog_PrintOptions();

      struct PrintSettings get_Results();

   private:
      Ui::Dialog_PrintOptions *m_ui;
      MainWindow *m_parent;

      struct PrintSettings m_print;      
      QString m_menuText;

      void initData();
      void macroMenu(QToolButton *widget);

      void save();
      void cancel();

      CS_SLOT_1(Private, void fileName())
      CS_SLOT_2(fileName)

      CS_SLOT_1(Private, void pathFileName())
      CS_SLOT_2(pathFileName)

      CS_SLOT_1(Private, void pageNumber())
      CS_SLOT_2(pageNumber)

      CS_SLOT_1(Private, void totalPages())
      CS_SLOT_2(totalPages)

      CS_SLOT_1(Private, void pages())
      CS_SLOT_2(pages)

      CS_SLOT_1(Private, void date())
      CS_SLOT_2(date)

      CS_SLOT_1(Private, void time())
      CS_SLOT_2(time)

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
