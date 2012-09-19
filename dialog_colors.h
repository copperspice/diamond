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
      Dialog_Colors(MainWindow *from, Syntax *dw);
      ~Dialog_Colors();
      struct Settings get_Colors();
      struct SyntaxColors get_Syntax();

   private:
      Ui::Dialog_Colors *m_ui;
      MainWindow *m_mainWindow;
      Syntax *m_syntax;

      struct Settings m_struSettings;
      struct SyntaxColors m_struSyntax;

      void initData();
      void colorBox(QLineEdit field, QColor color);
      QColor pickColor(QColor oldColor);


   private slots:
      void text_TB();
      void back_TB();
      void highText_TB();
      void highBack_TB();

      void Save();
      void Cancel();
};

#endif
