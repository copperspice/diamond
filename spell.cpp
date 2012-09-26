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

#include "mainwindow.h"
#include "spellcheck.h"
#include "util.h"

void MainWindow::createSpellCheck()
{
   m_spellCheck = new SpellCheck(m_struct.dictMain,  m_struct.dictUser);  
}

void MainWindow::add_UserDict()
{      
   QString word = m_textEdit->textCursor().selectedText();
   csMsg(word);

   m_spellCheck->addToUserWordlist(word);
}

void MainWindow::replaceWord()
{
   QAction *action;
   action = (QAction *)sender();

   if (action) {
      m_textEdit->insertPlainText(action->text());
   }
}

QStringList MainWindow::get_Maybe(QString word)
{
   if ( m_spellCheck->spell(word) )   {
      return QStringList();
   } else {
      return m_spellCheck->suggest(word);
   }
}




