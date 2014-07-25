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

#ifndef DIALOG_COLORS_H
#define DIALOG_COLORSS_H

#include "ui_dialog_colors.h"
#include "mainwindow.h"
#include "syntax.h"

#include <QDialog>
#include <QStringList>

class Dialog_Colors : public QDialog
{
   CS_OBJECT(Dialog_Colors)

   public:
      Dialog_Colors(MainWindow *parent);
      ~Dialog_Colors();
      struct Settings get_Colors();

   private:
      Ui::Dialog_Colors *m_ui;

      MainWindow *m_mainWindow;
      Syntax *m_syntaxParser;

      QString m_syntaxFname;
      struct Settings m_struSettings;

      void initData();
      void colorBox(QLineEdit *field, QColor color);
      void updateParser(bool newSettings);
      QColor pickColor(QColor oldColor);

      CS_SLOT_1(Private, void text_TB())
      CS_SLOT_2(text_TB) 

      CS_SLOT_1(Private, void back_TB())
      CS_SLOT_2(back_TB) 

      CS_SLOT_1(Private, void highText_TB())
      CS_SLOT_2(highText_TB) 

      CS_SLOT_1(Private, void highBack_TB())
      CS_SLOT_2(highBack_TB) 

      CS_SLOT_1(Private, void key_TB())
      CS_SLOT_2(key_TB) 

      CS_SLOT_1(Private, void type_TB())
      CS_SLOT_2(type_TB) 

      CS_SLOT_1(Private, void class_TB())
      CS_SLOT_2(class_TB) 

      CS_SLOT_1(Private, void func_TB())
      CS_SLOT_2(func_TB) 

      CS_SLOT_1(Private, void quote_TB())
      CS_SLOT_2(quote_TB) 

      CS_SLOT_1(Private, void comment_TB())
      CS_SLOT_2(comment_TB) 

      CS_SLOT_1(Private, void mline_TB())
      CS_SLOT_2(mline_TB) 

      CS_SLOT_1(Private, void key_bold())
      CS_SLOT_2(key_bold) 

      CS_SLOT_1(Private, void key_italic())
      CS_SLOT_2(key_italic) 

      CS_SLOT_1(Private, void type_bold())
      CS_SLOT_2(type_bold) 

      CS_SLOT_1(Private, void type_italic())
      CS_SLOT_2(type_italic) 

      CS_SLOT_1(Private, void class_bold())
      CS_SLOT_2(class_bold) 

      CS_SLOT_1(Private, void class_italic())
      CS_SLOT_2(class_italic) 

      CS_SLOT_1(Private, void func_bold())
      CS_SLOT_2(func_bold) 

      CS_SLOT_1(Private, void func_italic())
      CS_SLOT_2(func_italic) 

      CS_SLOT_1(Private, void quote_bold())
      CS_SLOT_2(quote_bold) 

      CS_SLOT_1(Private, void quote_italic())
      CS_SLOT_2(quote_italic) 

      CS_SLOT_1(Private, void comment_bold())
      CS_SLOT_2(comment_bold) 

      CS_SLOT_1(Private, void comment_italic())
      CS_SLOT_2(comment_italic) 

      CS_SLOT_1(Private, void mline_bold())
      CS_SLOT_2(mline_bold) 

      CS_SLOT_1(Private, void mline_italic())
      CS_SLOT_2(mline_italic) 

      CS_SLOT_1(Private, void Save())
      CS_SLOT_2(Save) 

      CS_SLOT_1(Private, void Cancel())
      CS_SLOT_2(Cancel) 
};

#endif
