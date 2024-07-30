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

#include "keylineedit.h"
#include "util.h"

#include <QGlobal>
#include <QKeyEvent>
#include <QString>

KeyLineEdit::KeyLineEdit(QWidget *parent) :
   QLineEdit(parent)
{
}

void KeyLineEdit::keyPressEvent(QKeyEvent *event)
{
   QString keyPress;

   int key = event->key();
   int modifiers = event->modifiers();

   if (key == Qt::Key_Control || key == Qt::Key_Shift || key == Qt::Key_Alt || key == Qt::Key_Meta)  {
      // do nothing yet

   } else  {

#ifdef Q_OS_MAC
      if (modifiers & Qt::ControlModifier) {
         keyPress.append("Meta+");
      }

      if (modifiers & Qt::MetaModifier) {
         keyPress.append("Ctrl+");
      }
#else
      if (modifiers & Qt::ControlModifier) {
         keyPress.append("Ctrl+");
      }

      if (modifiers & Qt::MetaModifier) {
         keyPress.append("Meta+");
      }
#endif

      if (modifiers & Qt::ShiftModifier) {
         keyPress.append("Shift+");
      }

      if (modifiers & Qt::AltModifier) {
         keyPress.append("Alt+");
      }

      //
      if (key == Qt::Key_Return) {
         // not the one on the keypad
         keyPress.append("Return");

      } else if (key == Qt::Key_Enter) {
        // on the keypad
        keyPress.append("Enter");

      } else if (key == Qt::Key_Delete) {
         keyPress.append("Del");

      } else if (key == Qt::Key_Insert) {
         keyPress.append("Ins");

      } else if (key == Qt::Key_Tab) {
         keyPress.append("Tab");

      } else if (key == Qt::Key_Home) {
         keyPress.append("Home");

      } else if (key == Qt::Key_End) {
         keyPress.append("End");

      } else if (key == Qt::Key_PageUp) {
         keyPress.append("PgUp");

      } else if (key == Qt::Key_PageDown) {
         keyPress.append("PgDn");

      } else if (key == Qt::Key_F1) {
         keyPress.append("F1");

      } else if (key == Qt::Key_F2) {
         keyPress.append("F2");

      } else if (key == Qt::Key_F3) {
         keyPress.append("F3");

      } else if (key == Qt::Key_F4) {
         keyPress.append("F4");

      } else if (key == Qt::Key_F5) {
         keyPress.append("F5");

      } else if (key == Qt::Key_F6) {
         keyPress.append("F6");

      } else if (key == Qt::Key_F7) {
         keyPress.append("F7");

      } else if (key == Qt::Key_F8) {
         keyPress.append("F8");

      } else if (key == Qt::Key_F9) {
         keyPress.append("F9");

      } else if (key == Qt::Key_F10) {
         keyPress.append("F10");

      } else if (key == Qt::Key_F11) {
         keyPress.append("F11");

      } else if (key == Qt::Key_F12) {
         keyPress.append("F12");

      } else {
          keyPress.append(key);
      }

      setText(keyPress.trimmed());
   }
}
