/**************************************************************************
*
* Copyright (c) 2012-2015 Barbara Geller
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

#ifndef KEYLINEEDIT_H
#define KEYLINEEDIT_H

#include <QLineEdit>

class KeyLineEdit : public QLineEdit
{
   CS_OBJECT(KeyLineEdit)

   public:
      explicit KeyLineEdit(QWidget *parent = 0);
      
   protected:
      void keyPressEvent(QKeyEvent *event);
      
};

#endif
