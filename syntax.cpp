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

#include "spellcheck.h"
#include "syntax.h"
#include "util.h"

#include <QString>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTextBoundaryFinder>
#include <QFileInfo>

Syntax::Syntax(QTextDocument *document, QString synFName, const struct Settings &settings, SpellCheck *spell)
   : QSyntaxHighlighter(document)
{
   m_spellCheck   = spell;
   m_isSpellCheck = settings.isSpellCheck;

   // get existing json data
   QByteArray data = json_ReadFile(synFName);

   QJsonDocument doc = QJsonDocument::fromJson(data);

   QJsonObject object = doc.object();   
   QJsonArray list;
   int cnt;

   //
   bool ignoreCase = object.value("ignore-case").toBool();

   // key
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


   //
   HighlightingRule rule;

   foreach (const QString &pattern, key_Patterns) {
      if (pattern.trimmed().isEmpty()) {
         continue;
      }

      // key
      rule.format.setFontWeight(settings.syn_KeyWeight);
      rule.format.setFontItalic(settings.syn_KeyItalic);
      rule.format.setForeground(settings.syn_KeyText);
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
      rule.format.setFontWeight(settings.syn_ClassWeight);
      rule.format.setFontItalic(settings.syn_ClassItalic);
      rule.format.setForeground(settings.syn_ClassText);
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
      rule.format.setFontWeight(settings.syn_FuncWeight);
      rule.format.setFontItalic(settings.syn_FuncItalic);
      rule.format.setForeground(settings.syn_FuncText);
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
      rule.format.setFontWeight(settings.syn_TypeWeight);
      rule.format.setFontItalic(settings.syn_TypeItalic);
      rule.format.setForeground(settings.syn_TypeText);
      rule.pattern = QRegExp(pattern);

      if (ignoreCase) {
         rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
      }

      highlightingRules.append(rule);
   }

   // class - rule is for Qt*()
   rule.format.setFontWeight(settings.syn_ClassWeight);
   rule.format.setFontItalic(settings.syn_ClassItalic);
   rule.format.setForeground(settings.syn_ClassText);
   rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
   highlightingRules.append(rule);

   // functions
   rule.format.setFontWeight(settings.syn_FuncWeight);
   rule.format.setFontItalic(settings.syn_FuncItalic);
   rule.format.setForeground(settings.syn_FuncText);
   rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
   highlightingRules.append(rule);

   // quoted text
   rule.format.setFontWeight(settings.syn_QuoteWeight);
   rule.format.setFontItalic(settings.syn_QuoteItalic);
   rule.format.setForeground(settings.syn_QuoteText);
   rule.pattern = QRegExp("\".*\"");
   highlightingRules.append(rule);

   // single line comment
   rule.format.setFontWeight(settings.syn_CommentWeight);
   rule.format.setFontItalic(settings.syn_CommentItalic);
   rule.format.setForeground(settings.syn_CommentText);
   rule.pattern = QRegExp("//[^\n]*");
   highlightingRules.append(rule);

   // multi line comment
   QString temp = QFileInfo(synFName).fileName();

   if (temp == "syn_dox.json" || temp == "syn_txt.json" ||
       temp == "syn_none.json" ) {

      m_commentStartExpression = QRegExp("(?!E)E");
      m_commentEndExpression   = QRegExp("(?!E)E");

   } else {
      m_commentStartExpression = QRegExp("/\\*");
      m_commentEndExpression   = QRegExp("\\*/");
   }

   m_multiLineCommentFormat.setFontWeight(settings.syn_MLineWeight);
   m_multiLineCommentFormat.setFontItalic(settings.syn_MLineItalic);
   m_multiLineCommentFormat.setForeground(settings.syn_MLineText);

   // spell check   
   m_spellCheckFormat.setUnderlineColor(QColor("red"));
   m_spellCheckFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
}

Syntax::~Syntax()
{   
}

QByteArray Syntax::json_ReadFile(QString fileName)
{
   QByteArray data;

   QFile file(fileName);
   if (! file.open(QFile::ReadOnly | QFile::Text)) {
      const QString msg = tr("Unable to open Json Sntax file: ") +  fileName + " : " + file.errorString();
      csError(tr("Read Json"), msg);
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

