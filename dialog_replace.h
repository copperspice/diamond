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

#ifndef DIALOG_REPLACE_H
#define DIALOG_REPLACE_H

#include "diamond_edit.h"
#include "ui_dialog_replace.h"

#include <QDialog>

class Dialog_Replace : public QDialog
{     
   Q_OBJECT

   public:
      Dialog_Replace(DiamondTextEdit *text, QString findText = "");
      ~Dialog_Replace();

   private:
      Ui::Dialog_Replace *m_ui;
      DiamondTextEdit *m_textEdit;

   private slots:
      void Find();
      void Replace();
      void ReplaceAll();

      void Cancel();
};

#endif
