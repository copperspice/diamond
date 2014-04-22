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
   CS_OBJECT(Dialog_PrintOptions)

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

   private :
      CS_SLOT_1(Private, void Save())
      CS_SLOT_2(Save) 
      CS_SLOT_1(Private, void Cancel())
      CS_SLOT_2(Cancel) 

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

      CS_SLOT_1(Private, void headerLeft())
      CS_SLOT_2(headerLeft) 
      CS_SLOT_1(Private, void headerCenter())
      CS_SLOT_2(headerCenter) 
      CS_SLOT_1(Private, void headerRight())
      CS_SLOT_2(headerRight) 
      CS_SLOT_1(Private, void footerLeft())
      CS_SLOT_2(footerLeft) 
      CS_SLOT_1(Private, void footerCenter())
      CS_SLOT_2(footerCenter) 
      CS_SLOT_1(Private, void footerRight())
      CS_SLOT_2(footerRight)       

      CS_SLOT_1(Private, void fontHeader())
      CS_SLOT_2(fontHeader) 
      CS_SLOT_1(Private, void fontFooter())
      CS_SLOT_2(fontFooter) 
      CS_SLOT_1(Private, void fontText())
      CS_SLOT_2(fontText) 
};

#endif
