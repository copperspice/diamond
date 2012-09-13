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

#ifndef DIALOG_GET1_H
#define DIALOG_GET1_H

#include "ui_dialog_get1.h"

#include <QDialog>

class Dialog_Get1 : public QDialog
{     
   Q_OBJECT

   public:
      Dialog_Get1();
      ~Dialog_Get1();
      QString get_Value();

   private:
      Ui::Dialog_Get1 *m_ui;

   private slots:
      void Ok();
      void Cancel();
};

#endif
