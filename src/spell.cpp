/**************************************************************************
*
* Copyright (c) 2012-2018 Barbara Geller
* All rights reserved.
*
* Diamond Editor is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License version 2
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

void MainWindow::createSpellCheck()
{
   m_spellCheck = new SpellCheck(m_struct.dictMain,  m_struct.dictUser);
}

void MainWindow::spell_addUserDict()
{
   // retrieve saved cursor
   QTextCursor cursor = m_textEdit->get_Cursor();
   QString word = cursor.selectedText();

   if (! word.isEmpty()) {
      m_spellCheck->addToUserDict(word);
      cursor.insertText(word);
   }
}

void MainWindow::spell_replaceWord()
{
   QAction *action;
   action = (QAction *)sender();

   if (action) {      
      // retrieve saved cursor
      QTextCursor cursor = m_textEdit->get_Cursor();
      cursor.insertText(action->text());
   }
}

QStringList MainWindow::spell_getMaybe(QString word)
{
   if ( m_spellCheck->spell(word) )   {
      return QStringList();
   } else {
      return m_spellCheck->suggest(word);
   }
}

void MainWindow::setSyntax()
{
   if (m_syntaxParser) {
      delete m_syntaxParser;

      m_syntaxParser = 0;
      m_textEdit->set_SyntaxParser(0);
   }

   QString fname  = "";
   QString suffix = "txt";

   if (! m_curFile.isEmpty()) {

      fname  = strippedName(m_curFile).toLower();
      suffix = suffixName();

      const QList<QString> cppList = {"h", "hh", "hpp", "c", "cc", "c++", "cpp", "l", "m", "mm" };

      if (cppList.contains(suffix)) {
         suffix = "cpp";

      } else if (suffix == "prg") {
         suffix = "clipper";                 

      } else if (fname == "cmakelists.txt" || suffix == "cmake") {
         suffix = "cmake";

      } else if (suffix == "dox" || suffix == "doxypress") {
         suffix = "doxy";

      } else if (suffix == "err" || suffix == "log") {
         suffix = "errlog";

      } else if (suffix == "htm" || suffix == "shtml") {
         suffix = "html";

      } else if (fname == "configure" || fname == "configure.ac"  || suffix == "ac") {
         suffix = "make";

      } else if (fname == "makefile" || fname == "makefile.in" || suffix == "am") {
         suffix = "make";

      } else if (suffix == "nsis") {
         suffix = "nsi";

      } else if (suffix == "perl" || suffix == "pm") {
         suffix = "pl";

      } else if (suffix == "php3" || suffix == "php4") {
         suffix = "php";

      }
   }

   QString synFName = m_struct.pathSyntax + "syn_"+ suffix + ".json";

   if (! QFile::exists(synFName)) {
      // use default
      suffix   = "txt";
      synFName = m_struct.pathSyntax + "syn_txt.json";
   }

   //
   if (! QFile::exists(synFName)) {
      setSynType(SYN_NONE);

      m_syntaxEnum = SYN_NONE;
      m_textEdit->set_SyntaxEnum(m_syntaxEnum);

   } else {      

      if (suffix == "cpp") {
         // setSynType(SYN_C);
         m_syntaxEnum = SYN_C;

      } else if (suffix == "clipper")  {
         m_syntaxEnum = SYN_CLIPPER;

      } else if (suffix == "cmake")  {
         m_syntaxEnum = SYN_CMAKE;

      } else if (suffix == "css")  {
         m_syntaxEnum = SYN_CSS;

      } else if (suffix == "doxy")  {
         m_syntaxEnum = SYN_DOXY;

      } else if (suffix == "errlog")  {
         m_syntaxEnum = SYN_ERRLOG;

      } else if (suffix == "html")  {
         m_syntaxEnum = SYN_HTML;

      } else if (suffix == "java")  {
         m_syntaxEnum = SYN_JAVA;

      } else if (suffix == "js")  {
         m_syntaxEnum = SYN_JS;

      } else if (suffix == "json")  {
         m_syntaxEnum = SYN_JSON;

      } else if ( suffix == "make")  {
         m_syntaxEnum = SYN_MAKE;         

      } else if (suffix == "nsi")  {
         m_syntaxEnum = SYN_NSIS;

      } else if (suffix == "txt")  {
         m_syntaxEnum = SYN_TEXT;

      } else if ( suffix == "sh")  {
         m_syntaxEnum = SYN_SHELL;

      } else if ( suffix == "pl")  {
         m_syntaxEnum = SYN_PERL;

      } else if (suffix == "php")  {
         m_syntaxEnum = SYN_PHP;

      } else if (suffix == "py")  {
         m_syntaxEnum = SYN_PYTHON;

/*
      } else if (suffix == "unused1")  {
         m_syntaxEnum = SYN_?;

      } else if (suffix == "unused2")  {
         m_syntaxEnum = SYN_?;
*/

      }

      // save the menu enum
      setSynType(m_syntaxEnum);
      m_textEdit->set_SyntaxEnum(m_syntaxEnum);

      // check the menu item
      setSynType(m_syntaxEnum);

      runSyntax(synFName);
   }
}

void MainWindow::forceSyntax(SyntaxTypes data)
{
   QString synFName;   

   switch (data)  {
      case SYN_C:         
         synFName = m_struct.pathSyntax + "syn_cpp.json";
         break;

      case SYN_CLIPPER:
         synFName = m_struct.pathSyntax + "syn_clipper.json";
         break;

      case SYN_CMAKE:
         synFName = m_struct.pathSyntax + "syn_cmake.json";
         break;

      case SYN_CSS:
         synFName = m_struct.pathSyntax + "syn_css.json";
         break;

      case SYN_DOXY:
         synFName = m_struct.pathSyntax + "syn_doxy.json";
         break;

      case SYN_ERRLOG:
         synFName = m_struct.pathSyntax + "syn_errlog.json";
         break;

      case SYN_HTML:
         synFName = m_struct.pathSyntax + "syn_html.json";
         break;

      case SYN_JAVA:
         synFName = m_struct.pathSyntax + "syn_java.json";
         break;

      case SYN_JS:
         synFName = m_struct.pathSyntax + "syn_js.json";
         break;

      case SYN_JSON:
         synFName = m_struct.pathSyntax + "syn_json.json";
         break;

      case SYN_MAKE:
        synFName = m_struct.pathSyntax + "syn_make.json";
         break;

      case SYN_NSIS:
         synFName = m_struct.pathSyntax + "syn_nsi.json";
         break;

      case SYN_TEXT:
         synFName = m_struct.pathSyntax + "syn_txt.json";
         break;

      case SYN_SHELL:
         synFName = m_struct.pathSyntax + "syn_sh.json";
         break;

      case SYN_PERL:
         synFName = m_struct.pathSyntax + "syn_pl.json";
         break;

      case SYN_PHP:
         synFName = m_struct.pathSyntax + "syn_php.json";
         break;

      case SYN_PYTHON:
         synFName = m_struct.pathSyntax + "syn_py.json";
         break;

      case SYN_NONE:
         synFName = m_struct.pathSyntax + "syn_none.json";
         break;

/*
      case SYN_UNUSED1:
         synFName = m_struct.pathSyntax + "syn_unused1.json";
         break;

      case SYN_UNUSED2:
         synFName = m_struct.pathSyntax + "syn_unused2.json";
         break;
*/

   }

   if (! QFile::exists(synFName)) {   
      csError(tr("Syntax Highlighting"), tr("Syntax highlighting file was not found: \n\n") + synFName  + "  ");

   } else {
      m_syntaxEnum = data;
      m_textEdit->set_SyntaxEnum(m_syntaxEnum);

      // check the box
      setSynType(m_syntaxEnum);

      runSyntax(synFName);
   }
}

void MainWindow::runSyntax(QString synFName)
{
   // save syntax file name
   m_textEdit->set_SyntaxFile(synFName);

   m_syntaxParser = new Syntax(m_textEdit->document(), synFName, m_struct, m_spellCheck);

   if ( m_syntaxParser->processSyntax() ) {
      m_textEdit->set_SyntaxParser(m_syntaxParser);
   } 
}

