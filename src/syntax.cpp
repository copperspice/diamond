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

#include "spellcheck.h"
#include "syntax.h"
#include "util.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTextBoundaryFinder>

static const QRegularExpression DEFAULT_COMMENT = QRegularExpression("(?!E)E");

Syntax::Syntax(QTextDocument *document, QString synFName, const struct Settings &settings, SpellCheck *spell)
   : QSyntaxHighlighter(document)
{
   m_syntaxFile   = synFName;
   m_settings     = settings;
   m_spellCheck   = spell;

   m_isSpellCheck = settings.isSpellCheck;
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
   QByteArray jsonData = json_ReadFile(m_syntaxFile);

   if (jsonData.isEmpty()) {
      return false;
   }

   QJsonDocument doc = QJsonDocument::fromJson(jsonData);

   QJsonObject object = doc.object();
   QJsonArray list;

   bool ignoreCase = object.value("ignore-case").toBool();

   // key
   QStringList key_Patterns;
   list = object.value("keywords").toArray();

   for (const auto &item : list) {
      key_Patterns.append(item.toString());
   }

   // class
   QStringList class_Patterns;
   list = object.value("classes").toArray();

   for (const auto &item : list) {
      class_Patterns.append(item.toString());
   }

   // functions
   QStringList func_Patterns;
   list = object.value("functions").toArray();

   for (const auto &item : list) {
      func_Patterns.append(item.toString());
   }

   // types
   QStringList type_Patterns;
   list = object.value("types").toArray();

   for (const auto &item : list) {
      type_Patterns.append(item.toString());
   }

   HighlightingRule rule;

   // key
   rule.format.setFontWeight(m_settings.syn_KeyWeight);
   rule.format.setFontItalic(m_settings.syn_KeyItalic);
   rule.format.setForeground(m_settings.syn_KeyText);

   for (const QString &item : key_Patterns) {
      if (item.trimmed().isEmpty()) {
         continue;
      }

      rule.pattern = QRegularExpression(item);

      if (ignoreCase) {
         rule.pattern.setPatternOptions(QPatternOption::CaseInsensitiveOption);
      }
      highlightingRules.append(rule);
   }

   // class
   rule.format.setFontWeight(m_settings.syn_ClassWeight);
   rule.format.setFontItalic(m_settings.syn_ClassItalic);
   rule.format.setForeground(m_settings.syn_ClassText);

   for (const QString &item : class_Patterns) {
      if (item.trimmed().isEmpty()) {
         continue;
      }

      rule.pattern = QRegularExpression(item);

      if (ignoreCase) {
         rule.pattern.setPatternOptions(QPatternOption::CaseInsensitiveOption);
      }
      highlightingRules.append(rule);
   }

   // func
   rule.format.setFontWeight(m_settings.syn_FuncWeight);
   rule.format.setFontItalic(m_settings.syn_FuncItalic);
   rule.format.setForeground(m_settings.syn_FuncText);

   for (const QString &item : func_Patterns) {
      if (item.trimmed().isEmpty()) {
         continue;
      }

      rule.pattern = QRegularExpression(item);

      if (ignoreCase) {
         rule.pattern.setPatternOptions(QPatternOption::CaseInsensitiveOption);
      }
      highlightingRules.append(rule);
   }

   // types
   rule.format.setFontWeight(m_settings.syn_TypeWeight);
   rule.format.setFontItalic(m_settings.syn_TypeItalic);
   rule.format.setForeground(m_settings.syn_TypeText);

   for (const QString &item : type_Patterns) {
      if (item.trimmed().isEmpty()) {
         continue;
      }

      rule.pattern = QRegularExpression(item);

      if (ignoreCase) {
         rule.pattern.setPatternOptions(QPatternOption::CaseInsensitiveOption);
      }
      highlightingRules.append(rule);
   }

   // quoted text
   rule.format.setFontWeight(m_settings.syn_QuoteWeight);
   rule.format.setFontItalic(m_settings.syn_QuoteItalic);
   rule.format.setForeground(m_settings.syn_QuoteText);
   rule.pattern = QRegularExpression("\".*?\"");
   highlightingRules.append(rule);

   // single line comment
   QString commentSingle = object.value("comment-single").toString();

   rule.format.setFontWeight(m_settings.syn_CommentWeight);
   rule.format.setFontItalic(m_settings.syn_CommentItalic);
   rule.format.setForeground(m_settings.syn_CommentText);
   rule.pattern = QRegularExpression(commentSingle);
   highlightingRules.append(rule);

   // multi line comment
   QString commentStart = object.value("comment-multi-start").toString();
   QString commentEnd   = object.value("comment-multi-end").toString();

   m_multiLineCommentFormat.setFontWeight(m_settings.syn_MLineWeight);
   m_multiLineCommentFormat.setFontItalic(m_settings.syn_MLineItalic);
   m_multiLineCommentFormat.setForeground(m_settings.syn_MLineText);
   m_commentStartExpression = QRegularExpression(commentStart);
   m_commentEndExpression   = QRegularExpression(commentEnd);

   // spell check
   m_spellCheckFormat.setUnderlineColor(QColor(Qt::red));

   // pending
   // m_spellCheckFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
   m_spellCheckFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);

   // redo the current document
   rehighlight();

   return true;
}

QByteArray Syntax::json_ReadFile(QString fileName)
{
   QByteArray jsonData;

   if (fileName.isEmpty()) {
      csError(tr("Read Syntax File"), tr("Syntax file name was not supplied."));
      return jsonData;
   }

   if (! QFile::exists(fileName) ) {
      csError(tr("Read Syntax File"), tr("Syntax file was not found: ") + fileName + "\n\n"
              "To specify the location of the syntax files select 'Settings' from the main Menu. "
              "Then select 'General Options' and click on the Options tab.\n");

      return jsonData;
   }

   QFile file(fileName);
   if (! file.open(QFile::ReadOnly | QFile::Text)) {
      const QString msg = tr("Unable to open Syntax file: ") +  fileName + " : " + file.errorString();
      csError(tr("Read Json Syntax"), msg);
      return jsonData;
   }

   jsonData = file.readAll();
   file.close();

   return jsonData;
}

void Syntax::set_Spell(bool value)
{
   m_isSpellCheck = value;
}

void Syntax::highlightBlock(const QString &text)
{
   QRegularExpressionMatch match;

   for (auto &rule : highlightingRules) {
      match = rule.pattern.match(text);

      while (match.hasMatch()) {
         int index  = match.capturedStart(0) - text.begin();
         int length = match.capturedLength();

         setFormat(index, length, rule.format);

         // get new match
         match = rule.pattern.match(text, match.capturedEnd(0));
      }
   }

   // multi line comments
   setCurrentBlockState(0);

   int startIndex = 0;

   if (m_commentStartExpression.pattern().isEmpty()) {
      m_commentStartExpression = DEFAULT_COMMENT;
   }

   if (m_commentEndExpression.pattern().isEmpty()) {
      m_commentEndExpression = DEFAULT_COMMENT;
   }

   if (previousBlockState() != 1) {
      startIndex = text.indexOf(m_commentStartExpression);
   }

   while (startIndex >= 0) {
      int commentLength;
      match = m_commentEndExpression.match(text, text.begin() + startIndex);

      if (match.hasMatch()) {
         int endIndex  = match.capturedStart(0) - text.begin();
         commentLength = endIndex - startIndex + match.capturedLength();

      } else {
         setCurrentBlockState(1);
         commentLength = text.length() - startIndex;

      }

      setFormat(startIndex, commentLength, m_multiLineCommentFormat);
      startIndex = text.indexOf(m_commentStartExpression, startIndex + commentLength);
   }

   // spell check
   if (m_spellCheck && m_isSpellCheck)  {
      QTextBoundaryFinder wordFinder(QTextBoundaryFinder::Word, text);

      while (wordFinder.position() < text.length()) {
         int wordStart  = wordFinder.position();
         int wordLength = wordFinder.toNextBoundary() - wordStart;

         QStringView word = text.midView(wordStart, wordLength).trimmed();

         if ( ! m_spellCheck->spell(word) )   {
            setFormat(wordStart, wordLength, m_spellCheckFormat);
         }
      }
   }
}

