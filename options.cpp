/**************************************************************************
*
* Copyright (c) 2012-2013 Barbara Geller
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

#include "dialog_colors.h"
#include "dialog_options.h"
#include "dialog_print_opt.h"
#include "mainwindow.h"
#include "util.h"

#include <QFontDialog>

// **settings
void MainWindow::setColors()
{
   // save the old colors
   QColor old_TextColor = m_struct.colorText;
   QColor old_BackColor = m_struct.colorBack;

   //
   Dialog_Colors *dw = new Dialog_Colors(this);
   int result = dw->exec();

   if (result == QDialog::Accepted) {

      if (m_struct.showLineHighlight)  {
         // clear the old highlight first
         QList<QTextEdit::ExtraSelection> extraSelections;
         QTextEdit::ExtraSelection selection;

         selection.format.setForeground(old_TextColor);
         selection.format.setBackground(old_BackColor);
         selection.format.setProperty(QTextFormat::FullWidthSelection, true);

         selection.cursor = m_textEdit->textCursor();
         selection.cursor.clearSelection();

         extraSelections.append(selection);
         m_textEdit->setExtraSelections(extraSelections);
      }

      // update colors in settings structure
      m_struct = dw->get_Colors();
      json_Write(COLORS);

      QPalette colors = m_textEdit->palette();
      colors.setColor(QPalette::Text, m_struct.colorText);
      colors.setColor(QPalette::Base, m_struct.colorBack);
      m_textEdit->setPalette(colors);

      // get saved value
      QString synFName = m_textEdit->get_SyntaxFile();

      // run for every tab
      DiamondTextEdit *cur_textEdit  = m_textEdit;
      int count = m_tabWidget->count();

      QWidget *temp;
      DiamondTextEdit *textEdit;

      for (int k = 0; k < count; ++k)  {

         temp     = m_tabWidget->widget(k);
         textEdit = dynamic_cast<DiamondTextEdit *>(temp);

         if (textEdit) {
            m_textEdit = textEdit;

            // get saved value
            synFName = m_textEdit->get_SyntaxFile();

            // reloads the syntax blocks based on new colors
            runSyntax(synFName);
         }
      }

      // reassign current tab
      m_textEdit = cur_textEdit;

      // for this tab only, updated every time we change tabs
      moveBar();
   }

   delete dw;
}

void MainWindow::setFont()
{
   bool ok;
   m_struct.font = QFontDialog::getFont(&ok, m_textEdit->font(), this);

   if (ok) {
      m_textEdit->setFont(m_struct.font);
      json_Write(FONT);
   }
}

void MainWindow::setOptions()
{
   struct Options options;
   options.useSpaces    = m_struct.useSpaces;
   options.tabSpacing   = m_struct.tabSpacing;
   options.formatDate   = m_struct.formatDate;
   options.formatTime   = m_struct.formatTime;
   options.dictMain     = m_struct.dictMain;
   options.dictUser     = m_struct.dictUser;
   options.pathSyntax   = m_struct.pathSyntax;
   options.aboutUrl     = m_struct.aboutUrl;
   options.autoLoad     = m_struct.autoLoad;

   options.key_selectLine  = m_struct.key_selectLine;
   options.key_selectWord  = m_struct.key_selectWord;
   options.key_selectBlock = m_struct.key_selectBlock;
   options.key_upper       = m_struct.key_upper;
   options.key_lower       = m_struct.key_lower;
   options.key_indentIncr  = m_struct.key_indentIncr;
   options.key_indentDecr  = m_struct.key_indentDecr;
   options.key_deleteLine  = m_struct.key_deleteLine;
   options.key_deleteEOL   = m_struct.key_deleteEOL;
   options.key_columnMode  = m_struct.key_columnMode;
   options.key_goLine      = m_struct.key_goLine;
   options.key_macroPlay   = m_struct.key_macroPlay;
   options.key_spellCheck  = m_struct.key_spellCheck;

   Dialog_Options *dw = new Dialog_Options(this, options);
   int result = dw->exec();

   if ( result == QDialog::Accepted) {
      options = dw->get_Results();

      //
      if ( m_struct.tabSpacing != options.tabSpacing)  {
         m_struct.tabSpacing = options.tabSpacing;
         json_Write(TAB_SPACING);
      }

      if ( m_struct.useSpaces != options.useSpaces)  {
         m_struct.useSpaces = options.useSpaces;
         json_Write(USESPACES);
      }

      //
      if ( m_struct.formatDate != options.formatDate)  {
         m_struct.formatDate = options.formatDate;
         json_Write(FORMAT_DATE);
      }

      //
      if ( m_struct.formatTime != options.formatTime)  {
         m_struct.formatTime = options.formatTime;
         json_Write(FORMAT_TIME);
      }

      //
      if (m_struct.dictMain != options.dictMain ) {
         m_struct.dictMain = options.dictMain;
         json_Write(DICT_MAIN);
      }

      //
      if (m_struct.dictUser != options.dictUser ) {
         m_struct.dictUser = options.dictUser;
         json_Write(DICT_USER);
      }

      //
      if (m_struct.pathSyntax != options.pathSyntax ) {
         m_struct.pathSyntax = options.pathSyntax;
         json_Write(PATH_SYNTAX);
      }

      //
      if (m_struct.aboutUrl != options.aboutUrl ) {
         m_struct.aboutUrl = options.aboutUrl;
         json_Write(ABOUTURL);
      }

      //
      if (m_struct.autoLoad != options.autoLoad ) {
         m_struct.autoLoad = options.autoLoad;
         json_Write(AUTOLOAD);
      }

      // keys
      m_struct.key_selectLine  = options.key_selectLine;
      m_struct.key_selectWord  = options.key_selectWord;
      m_struct.key_selectBlock = options.key_selectBlock;
      m_struct.key_upper       = options.key_upper;
      m_struct.key_lower       = options.key_lower;
      m_struct.key_indentIncr  = options.key_indentIncr;
      m_struct.key_indentDecr  = options.key_indentDecr;
      m_struct.key_deleteLine  = options.key_deleteLine;
      m_struct.key_deleteEOL   = options.key_deleteEOL;
      m_struct.key_columnMode  = options.key_columnMode;
      m_struct.key_goLine      = options.key_goLine;
      m_struct.key_macroPlay   = options.key_macroPlay;
      m_struct.key_spellCheck  = options.key_spellCheck;
      json_Write(KEYS);

      // redisplay the user defined shortcuts only
      this->createShortCuts(false);
   }

   delete dw;
}

void MainWindow::setPrintOptions()
{
   struct PrintSettings options;   
   options.printHeader   = m_printer.printHeader;
   options.printFooter   = m_printer.printFooter;

   options.header_left   = m_printer.header_left;
   options.header_center = m_printer.header_center;
   options.header_right  = m_printer.header_right;
   options.header_line2  = m_printer.header_line2;
   options.footer_left   = m_printer.footer_left;
   options.footer_center = m_printer.footer_center;
   options.footer_right  = m_printer.footer_right;
   options.footer_line2  = m_printer.footer_line2;

   options.marTop        = m_printer.marTop;
   options.marBottom     = m_printer.marBottom;
   options.marLeft       = m_printer.marLeft;
   options.marRight      = m_printer.marRight;

   options.fontHeader    = m_printer.fontHeader;
   options.fontFooter    = m_printer.fontFooter;
   options.fontText      = m_printer.fontText;

   Dialog_PrintOptions *dw = new Dialog_PrintOptions(this, options);
   int result = dw->exec();

   if ( result == QDialog::Accepted) {
      options = dw->get_Results();

      m_printer.printHeader    = options.printHeader ;
      m_printer.printFooter    = options.printFooter;

      m_printer.header_left    = options.header_left;
      m_printer.header_center  = options.header_center;
      m_printer.header_right   = options.header_right;
      m_printer.header_line2   = options.header_line2;
      m_printer.footer_left    = options.footer_left;
      m_printer.footer_center  = options.footer_center;
      m_printer.footer_right   = options.footer_right;
      m_printer.footer_line2   = options.footer_line2;

      m_printer.marTop         = options.marTop;
      m_printer.marBottom      = options.marBottom;
      m_printer.marLeft        = options.marLeft;
      m_printer.marRight       = options.marRight;

      m_printer.fontHeader     = options.fontHeader;
      m_printer.fontFooter     = options.fontFooter;
      m_printer.fontText       = options.fontText;

      json_Write(PRINT_OPTIONS);
   }

   delete dw;
}
