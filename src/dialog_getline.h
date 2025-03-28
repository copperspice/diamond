/**************************************************************************
*
* Copyright (c) 2012-2025 Barbara Geller
*
* Diamond Editor is free software. You can redistribute it and/or
* modify it under the terms of the GNU General Public License
* version 2 as published by the Free Software Foundation.
*
* Diamond Editor is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* https://www.gnu.org/licenses/
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
      void ok();
      void cancel();

      Ui::Dialog_GetLine *m_ui;
};

#endif
