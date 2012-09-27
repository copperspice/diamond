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

#include "dialog_replace.h"
#include "util.h"

#include <QTextDocument>
#include <QTextCursor>

Dialog_Replace::Dialog_Replace(DiamondTextEdit *text, QString findText)
   : m_ui(new Ui::Dialog_Replace)
{
   m_textEdit = text;

   m_ui->setupUi(this);
   m_ui->find->setText(findText);
   m_ui->down_RB->setChecked(true);

   connect(m_ui->find_PB,        SIGNAL(clicked()),this, SLOT(Find()));
   connect(m_ui->replace_PB,     SIGNAL(clicked()),this, SLOT(Replace()));
   connect(m_ui->replaceAll_PB,  SIGNAL(clicked()),this, SLOT(ReplaceAll()));
   connect(m_ui->cancel_PB,      SIGNAL(clicked()),this, SLOT(Cancel()));
}

Dialog_Replace::~Dialog_Replace()
{
   delete m_ui;
}

void Dialog_Replace::Find()
{
   QTextDocument::FindFlags flags = 0;

   if ( ! m_ui->down_RB->isChecked() ) {
      flags |= QTextDocument::FindBackward;
   }

   if ( m_ui->case_CKB->isChecked() ) {
      flags |= QTextDocument::FindCaseSensitively;
   }

   if ( m_ui->wholeWords_CKB->isChecked()){
      flags |= QTextDocument::FindWholeWords;
   }

   QString findText = m_ui->find->text();

   if (! findText.isEmpty())  {

      bool found = m_textEdit->find(findText, flags);

      if (! found)  {
         csError("Find ", findText + " was not found");
      }
   }
}

void Dialog_Replace::Replace()
{
   bool found;

   QString oldText = m_ui->find->text();
   QString newText = m_ui->replace->text();

   if (oldText.isEmpty()) {
      csError("Find & Replace", "Find text is empty");
      return;
   }

   if (newText.isEmpty()) {
      csError("Find & Replace", "Replace text is empty");
      return;
   }

   QTextDocument::FindFlags flags = 0;

   if ( ! m_ui->down_RB->isChecked() ) {
      flags |= QTextDocument::FindBackward;
   }

   if ( m_ui->case_CKB->isChecked() ) {
      flags |= QTextDocument::FindCaseSensitively;
   }

   if ( m_ui->wholeWords_CKB->isChecked()){
      flags |= QTextDocument::FindWholeWords;
   }

   //
   QTextCursor cursor(m_textEdit->textCursor());

   //
   found = m_textEdit->find(oldText, flags);

   if (found) {
      cursor = m_textEdit->textCursor();

      if (cursor.hasSelection()) {
         cursor.insertText(newText);
      }
   }

}

void Dialog_Replace::ReplaceAll()
{
   bool found;

   QString oldText = m_ui->find->text();
   QString newText = m_ui->replace->text();

   if (oldText.isEmpty()) {
      csError("Find & Replace", "Find text is empty");
      return;
   }

   if (newText.isEmpty()) {
      csError("Find & Replace", "Replace text is empty");
      return;
   }

   QTextDocument::FindFlags flags = 0;

   if ( ! m_ui->down_RB->isChecked() ) {
      flags |= QTextDocument::FindBackward;
   }

   if ( m_ui->case_CKB->isChecked() ) {
      flags |= QTextDocument::FindCaseSensitively;
   }

   if ( m_ui->wholeWords_CKB->isChecked()){
      flags |= QTextDocument::FindWholeWords;
   }

   // begin undo block
   QTextCursor cursor(m_textEdit->textCursor());
   cursor.beginEditBlock();

   while (true) {
      found = m_textEdit->find(oldText, flags);

      if (found) {
         cursor = m_textEdit->textCursor();

         if (cursor.hasSelection()) {
            cursor.insertText(newText);
         }

      } else {
         break;
      }
   }

   // mark end of undo
   cursor.endEditBlock();
}

void Dialog_Replace::Cancel()
{
   this->done(0);
}


