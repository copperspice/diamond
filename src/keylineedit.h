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

#ifndef KEYLINEEDIT_H
#define KEYLINEEDIT_H

#include <QLineEdit>

class KeyLineEdit : public QLineEdit
{
   CS_OBJECT(KeyLineEdit)

   public:
      explicit KeyLineEdit(QWidget *parent = nullptr);

   protected:
      void keyPressEvent(QKeyEvent *event) override;
};

#endif
