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

#include "syntax.h"
#include "util.h"

#include <QtGui>
#include <QFile>
#include <QString>

Syntax::Syntax(QTextDocument *document, QString synFName, const struct Settings &settings)
   : QSyntaxHighlighter(document)
{              
   QFile file(synFName);
   if (! file.open(QFile::ReadOnly | QFile::Text)) {
      QString error = tr("Unable to open file %1:\n%2.").arg(synFName).arg(file.errorString());
      csError(tr("Syntax Highlighting"), error);
   }

   file.seek(0);
   QByteArray temp = file.readAll();

   QString fileData = QString::fromUtf8(temp);
   file.close();  
   QStringList keywordPatterns = fileData.split("\n");

   //
   QStringList typesPatterns;
   typesPatterns << "\\bint\\b" << "\\bfloat\\b";

   //
   HighlightingRule rule;

   foreach (const QString &pattern, keywordPatterns) {

      if (pattern.trimmed().isEmpty()) {
         continue;
      }

      // keywords
      rule.format.setFontWeight(settings.syn_KeyWeight);
      rule.format.setFontItalic(settings.syn_KeyItalic);
      rule.format.setForeground(settings.syn_KeyText);
      rule.pattern = QRegExp(pattern);
      highlightingRules.append(rule);
   }

   foreach (const QString &pattern, typesPatterns) {

      if (pattern.trimmed().isEmpty()) {
         continue;
      }

      // types
      rule.format.setFontWeight(settings.syn_TypeWeight);
      rule.format.setFontItalic(settings.syn_TypeItalic);
      rule.format.setForeground(settings.syn_TypeText);
      rule.pattern = QRegExp(pattern);
      highlightingRules.append(rule);
   }

   // class
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
   rule.format.setFontItalic(settings.syn_QuoteWeight);
   rule.format.setFontItalic(settings.syn_QuoteItalic);
   rule.format.setForeground(settings.syn_QuoteText);
   rule.pattern = QRegExp("\".*\"");
   highlightingRules.append(rule);

   // single line comment
   rule.format.setFontItalic(settings.syn_CommentWeight);
   rule.format.setFontItalic(settings.syn_CommentItalic);
   rule.format.setForeground(settings.syn_CommentText);
   rule.pattern = QRegExp("//[^\n]*");
   highlightingRules.append(rule);

   //rule.format.setFontItalic(settings.syn_MLineWeight);
   //rule.format.setFontItalic(settings.syn_MLineItalic);
   //rule.format.setForeground(settings.syn_MLineText);
   multiLineCommentFormat.setForeground(settings.syn_MLineText);

   commentStartExpression = QRegExp("/\\*");
   commentEndExpression   = QRegExp("\\*/");
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
      startIndex = commentStartExpression.indexIn(text);
   }

   while (startIndex >= 0) {
      int endIndex = commentEndExpression.indexIn(text, startIndex);
      int commentLength;

      if (endIndex == -1) {
         setCurrentBlockState(1);
         commentLength = text.length() - startIndex;

      } else {
         commentLength = endIndex - startIndex + commentEndExpression.matchedLength();

      }

      setFormat(startIndex, commentLength, multiLineCommentFormat);
      startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
   }
}


