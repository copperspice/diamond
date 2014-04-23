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

#ifndef DIALOG_FONTS_H
#define DIALOG_FONTS_H

#include "ui_dialog_fonts.h"

#include <QDialog>
#include <QFont>

class Dialog_Fonts : public QDialog
{     
   CS_OBJECT(Dialog_Fonts)

   public:
      Dialog_Fonts(QFont normal, QFont column);
      ~Dialog_Fonts();

      QFont get_fontNormal();
      QFont get_fontColumn();

   private:
      Ui::Dialog_Fonts *m_ui;
      QFont m_fontNormal;
      QFont m_fontColumn;

      CS_SLOT_1(Private, void save())
      CS_SLOT_2(save) 

      CS_SLOT_1(Private, void cancel())
      CS_SLOT_2(cancel) 

      CS_SLOT_1(Private, void fontNormal())
      CS_SLOT_2(fontNormal) 

      CS_SLOT_1(Private, void fontColumn())
      CS_SLOT_2(fontColumn)      
};

#endif
