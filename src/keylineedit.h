/**************************************************************************
*
* Copyright (c) 2012-2020 Barbara Geller
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
