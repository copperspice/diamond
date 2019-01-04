/**************************************************************************
*
* Copyright (c) 2012-2019 Barbara Geller
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

#ifndef DIALOG_BUFFER_H
#define DIALOG_BUFFER_H

#include "ui_dialog_buffer.h"
#include "mainwindow.h"

#include <QDialog>

class Dialog_Buffer : public QDialog
{
   CS_OBJECT(Dialog_Buffer)

   public:
      Dialog_Buffer(QList<QString> copyBuffer);
      ~Dialog_Buffer();
      int get_Index();       

   protected:
      bool eventFilter(QObject *object, QEvent *event);

   private :     
      Ui::Dialog_Buffer *m_ui;

      void select();
      void cancel();
};

#endif
