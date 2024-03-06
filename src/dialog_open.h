/**************************************************************************
*
* Copyright (c) 2012-2024 Barbara Geller
*
* Diamond Editor is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License version 2
* as published by the Free Software Foundation.
*
* Diamond is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
***************************************************************************/

#ifndef DIALOG_OPEN_H
#define DIALOG_OPEN_H

#include "mainwindow.h"
#include "ui_dialog_open.h"

#include <QDialog>

class Dialog_Open : public QDialog
{
   CS_OBJECT(Dialog_Open)

   public:
      Dialog_Open(MainWindow *parent, QStringList list);
      ~Dialog_Open();
      QString get_FileName();

   private:
      Ui::Dialog_Open *m_ui;    
      QStringList m_list;

      void open();
      void cancel();
};

#endif
