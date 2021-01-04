/**************************************************************************
*
* Copyright (c) 2012-2021 Barbara Geller
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

#ifndef DIALOG_GETLINE_H
#define DIALOG_GETLINE_H

#include "ui_dialog_getline.h"

#include <QDialog>

class Dialog_GetLine : public QDialog
{     
   CS_OBJECT(Dialog_GetLine)

   public:
      Dialog_GetLine();
      ~Dialog_GetLine();
      QString get_Value();
      void set_ColNo();

   private:
      Ui::Dialog_GetLine *m_ui;

      void ok();
      void cancel();
};

#endif
