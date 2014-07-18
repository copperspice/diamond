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

#include "spellcheck.h"
#include "syntax.h"
#include "util.h"

#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QTextBoundaryFinder>

static const QRegExp DEFAULT_COMMENT = QRegExp("(?!E)E");

Syntax::Syntax(QTextDocument *document, QString synFName, const struct Settings &settings, SpellCheck *spell)
   : QSyntaxHighlighter(document)
{
   m_syntaxFile   = synFName;
   m_settings     = settings;
   m_spellCheck   = spell;

   m_isSpellCheck = settings.isSpellCheck;
}

Syntax::~Syntax()
{
}

bool Syntax::processSyntax(const struct Settings &settings)
{
   // only called from Dialog_Colors
   m_settings = settings;

   return processSyntax();
}

bool Syntax::processSyntax()
{
   // get existing json data
   QByteArray data = json_ReadFile(m_syntaxFile);

   if (data.isEmpty()) {
      return false;
   }

   QJsonDocument doc = QJsonDocument::fromJson(data);

   QJsonObject object = doc.object();   
   QJsonArray list;
   int cnt;

   //
   bool ignoreCase = object.value("ignore-case").toBool();

   // * key
   QStringList key_Patterns;

   list = object.value("keywords").toArray();
   cnt  = list.count();

   for (int k = 0; k < cnt; k++)  {
      key_Patterns.append(list.at(k).toString());
   }

   // class
   QStringList class_Patterns;

   list = object.value("classes").toArray();
   cnt  = list.count();

   for (int k = 0; k < cnt; k++)  {
      class_Patterns.append(list.at(k).toString());
   }

   // functions
   QStringList func_Patterns;

   list = object.value("functions").toArray();
   cnt  = list.count();

   for (int k = 0; k < cnt; k++)  {
      func_Patterns.append(list.at(k).toString());
   }

   // types
   QStringList type_Patterns;

   list = object.value("types").toArray();
   cnt  = list.count();

   for (int k = 0; k < cnt; k++)  {
      type_Patterns.append(list.at(k).toString());
   }


   // *
   HighlightingRule rule;

   foreach (const QString &pattern, key_Patterns) {
      if (pattern.trimmed().isEmpty()) {
         continue;
      }

      // key
      rule.format.setFontWeight(m_settings.syn_KeyWeight);
      rule.format.setFontItalic(m_settings.syn_KeyItalic);
      rule.format.setForeground(m_settings.syn_KeyText);
      rule.pattern = QRegExp(pattern);

      if (ignoreCase) {
         rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
      }

      highlightingRules.append(rule);
   }

   foreach (const QString &pattern, class_Patterns) {
      if (pattern.trimmed().isEmpty()) {
         continue;
      }

      // class
      rule.format.setFontWeight(m_settings.syn_ClassWeight);
      rule.format.setFontItalic(m_settings.syn_ClassItalic);
      rule.format.setForeground(m_settings.syn_ClassText);
      rule.pattern = QRegExp(pattern);

      if (ignoreCase) {
         rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
      }

      highlightingRules.append(rule);
   }

   foreach (const QString &pattern, func_Patterns) {
      if (pattern.trimmed().isEmpty()) {
         continue;
      }

      // func
      rule.format.setFontWeight(m_settings.syn_FuncWeight);
      rule.format.setFontItalic(m_settings.syn_FuncItalic);
      rule.format.setForeground(m_settings.syn_FuncText);
      rule.pattern = QRegExp(pattern);

      if (ignoreCase) {
         rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
      }

      highlightingRules.append(rule);
   }

   foreach (const QString &pattern, type_Patterns) {
      if (pattern.trimmed().isEmpty()) {
         continue;
      }

      // types
      rule.format.setFontWeight(m_settings.syn_TypeWeight);
      rule.format.setFontItalic(m_settings.syn_TypeItalic);
      rule.format.setForeground(m_settings.syn_TypeText);
      rule.pattern = QRegExp(pattern);

      if (ignoreCase) {
         rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
      }

      highlightingRules.append(rule);
   }

   // quoted text - everyone
   rule.format.setFontWeight(m_settings.syn_QuoteWeight);
   rule.format.setFontItalic(m_settings.syn_QuoteItalic);
   rule.format.setForeground(m_settings.syn_QuoteText);
   rule.pattern = QRegExp("\".*\"");
   rule.pattern.setMinimal(true);
   highlightingRules.append(rule);

   // single line comment
   QString commentSingle = object.value("comment-single").toString();

   rule.format.setFontWeight(m_settings.syn_CommentWeight);
   rule.format.setFontItalic(m_settings.syn_CommentItalic);
   rule.format.setForeground(m_settings.syn_CommentText);
   rule.pattern = QRegExp(commentSingle);
   highlightingRules.append(rule);

   // multi line comment
   QString commentStart = object.value("comment-multi-start").toString();
   QString commentEnd   = object.value("comment-multi-end").toString();

   m_multiLineCommentFormat.setFontWeight(m_settings.syn_MLineWeight);
   m_multiLineCommentFormat.setFontItalic(m_settings.syn_MLineItalic);
   m_multiLineCommentFormat.setForeground(m_settings.syn_MLineText);
   m_commentStartExpression = QRegExp(commentStart);
   m_commentEndExpression   = QRegExp(commentEnd);

   // spell check   
   m_spellCheckFormat.setUnderlineColor(QColor(Qt::red));
   m_spellCheckFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

   // new data, redo the current document
   this->rehighlight();

   return true;
}

QByteArray Syntax::json_ReadFile(QString fileName)
{
   QByteArray data;

   if (fileName.isEmpty()) {
      csError(tr("Read Json Syntax"), tr("Syntax file name was not supplied."));
      return data;
   }

   if (! QFile::exists(fileName) ) {
      csError(tr("Read Json Syntax"), tr("Syntax file was not found: ") + fileName + "\n\n"
              "To specify the location of the syntax files select 'Settings' from the main Menu. "
              "Then select 'General Options' and click on the Options tab.\n");

      return data;
   }

   QFile file(fileName);
   if (! file.open(QFile::ReadOnly | QFile::Text)) {
      const QString msg = tr("Unable to open Json Syntax file: ") +  fileName + " : " + file.errorString();
      csError(tr("Read Json Syntax"), msg);
      return data;
   }

   file.seek(0);
   data = file.readAll();
   file.close();

   return data;
}

void Syntax::set_Spell(bool value)
{
   m_isSpellCheck = value;   
}

void Syntax::highlightBlock(const QString &text)
{
   foreach (const HighlightingRule &rule, highlightingRules) {
      QRegExp expression(rule.pattern);
      int index = expression.indexIn(text);

      while (index >= 0) {
         int length = expression.matchedLength();
         setFormat(index, length, rule.format);
         index = expression.indexIn(text, index + length);
      }
   }

   // multi line comments
   setCurrentBlockState(0);

   int startIndex = 0;

   if (m_commentStartExpression.isEmpty()) {
      m_commentStartExpression = DEFAULT_COMMENT;
   }

   if (m_commentEndExpression.isEmpty()) {
      m_commentEndExpression = DEFAULT_COMMENT;
   }

   //
   if (previousBlockState() != 1) {
      startIndex = m_commentStartExpression.indexIn(text);
   }

   while (startIndex >= 0) {
      int endIndex = m_commentEndExpression.indexIn(text, startIndex);
      int commentLength;

      if (endIndex == -1) {
         setCurrentBlockState(1);
         commentLength = text.length() - startIndex;

      } else {
         commentLength = endIndex - startIndex + m_commentEndExpression.matchedLength();

      }

      setFormat(startIndex, commentLength, m_multiLineCommentFormat);
      startIndex = m_commentStartExpression.indexIn(text, startIndex + commentLength);
   }

   // spell check
   if (m_spellCheck && m_isSpellCheck)  {
      QTextBoundaryFinder wordFinder(QTextBoundaryFinder::Word, text);

      int wordStart  = 0;
      int wordLength = 0;
      QString word   = "";

      while (wordFinder.position() < text.length()) {

         if (wordFinder.position() == 0)  {
            wordStart = 0;
         } else  {
            wordStart = wordFinder.position();
         }

         wordLength = wordFinder.toNextBoundary()-wordStart;
         word       = text.mid(wordStart,wordLength).trimmed();

         if ( ! m_spellCheck->spell(word) )   {
            setFormat(wordStart, wordLength, m_spellCheckFormat);
         }
      }
   }
}

