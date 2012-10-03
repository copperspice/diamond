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

#include "keylineedit.h"

#include <QString>
#include <QKeyEvent>
#include <Qt>

KeyLineEdit::KeyLineEdit(QWidget *parent) :
   QLineEdit(parent)
{
}

void KeyLineEdit::keyPressEvent(QKeyEvent *event)
{
   QString keyPress;
   int modifiers = event->modifiers();

   if(modifiers & Qt::ControlModifier) {
      keyPress.append("Ctrl+");
   }

   if(modifiers & Qt::ShiftModifier) {
      keyPress.append("Shift+");
   }

   if(modifiers & Qt::AltModifier) {
      keyPress.append("Alt+");
   }

// if (modifiers & Qt::NoModifier) {
// }

   keyPress.append(event->key());
   this->setText(keyPress);

   // keypress completed
   // QLineEdit::keyPressEvent(event);
}
