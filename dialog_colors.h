/**************************************************************************
*
* Copyright (c) 2012 Barbara Geller
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
   Q_OBJECT

   public:
      Dialog_Colors(MainWindow *from);
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
      QColor pickColor(QColor oldColor);

   private slots:
      void text_TB();
      void back_TB();
      void highText_TB();
      void highBack_TB();

      void key_TB();
      void type_TB();
      void class_TB();
      void func_TB();
      void quote_TB();
      void comment_TB();
      void mline_TB();

      void key_bold();
      void key_italic();
      void type_bold();
      void type_italic();
      void class_bold();
      void class_italic();
      void func_bold();
      void func_italic();
      void quote_bold();
      void quote_italic();
      void comment_bold();
      void comment_italic();
      void mline_bold();
      void mline_italic();

      void Save();
      void Cancel();
};

#endif
