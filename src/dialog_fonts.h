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

#ifndef DIALOG_FONTS_H
#define DIALOG_FONTS_H

#include "ui_dialog_fonts.h"

#include <QFont>
#include <QDialog>

class Dialog_Fonts : public QDialog
{
   CS_OBJECT(Dialog_Fonts)

   public:
      Dialog_Fonts(QFont normal, QFont column);
      ~Dialog_Fonts();

      QFont get_fontNormal();
      QFont get_fontColumn();

   private:
      void save();
      void cancel();
      void fontNormal();
      void fontColumn();

      Ui::Dialog_Fonts *m_ui;
      QFont m_fontNormal;
      QFont m_fontColumn;
};

#endif
