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

Syntax::Syntax(QTextDocument *document, QString synFName)
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

   m_struct.syn_KeyWeight    = QFont::Bold;
   m_struct.syn_KeyText      = QColor(Qt::blue);
   m_struct.syn_TypeWeight   = QFont::Normal;
   m_struct.syn_TypeText     = QColor(Qt::blue);
   m_struct.syn_ClassWeight  = QFont::Normal;
   m_struct.syn_ClassText    = QColor(Qt::darkMagenta);
   m_struct.syn_FuncWeight   = QFont::Normal;
   m_struct.syn_FuncText     = QColor(Qt::blue);
   m_struct.syn_QuoteText    = QColor(Qt::darkGreen);
   m_struct.syn_CommentText  = QColor(Qt::darkGreen);
   m_struct.syn_MLineText    = QColor(Qt::darkGreen);

   //
   HighlightingRule rule;

   foreach (const QString &pattern, keywordPatterns) {

      if (pattern.trimmed().isEmpty()) {
         continue;
      }

      // keywords
      rule.format.setFontWeight(m_struct.syn_KeyWeight);
      rule.format.setForeground(m_struct.syn_KeyText);
      rule.pattern = QRegExp(pattern);
      highlightingRules.append(rule);
   }

   foreach (const QString &pattern, typesPatterns) {

      if (pattern.trimmed().isEmpty()) {
         continue;
      }

      // types
      rule.format.setFontWeight(m_struct.syn_TypeWeight);
      rule.format.setForeground(m_struct.syn_TypeText);
      rule.pattern = QRegExp(pattern);
      highlightingRules.append(rule);
   }

   // class
   rule.format.setFontWeight(m_struct.syn_ClassWeight);
   // rule.format.setFontItalic(m_struct.syn_ClassItalic);
   rule.format.setForeground(m_struct.syn_ClassText);
   rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
   highlightingRules.append(rule);

   // functions    
   rule.format.setFontWeight(m_struct.syn_FuncWeight);
   rule.format.setForeground( m_struct.syn_FuncText);
   rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
   highlightingRules.append(rule);

   // quoted text
   rule.format.setForeground(m_struct.syn_QuoteText);
   rule.pattern = QRegExp("\".*\"");
   highlightingRules.append(rule);

   // single line comment
   rule.format.setForeground(m_struct.syn_CommentText);   
   rule.pattern = QRegExp("//[^\n]*");
   highlightingRules.append(rule);

   multiLineCommentFormat.setForeground(m_struct.syn_MLineText);

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

struct SyntaxColors Syntax::get_StructData()
{
   return m_struct;
}

void Syntax::set_StructData(SyntaxColors data)
{
   m_struct = data;
}



