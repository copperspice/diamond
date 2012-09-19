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

#ifndef SYNTAX_H
#define SYNTAX_H

#include <QHash>
#include <QTextCharFormat>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>

struct SyntaxColors {
   int    syn_KeyWeight;
   QColor syn_KeyText;
   int    syn_TypeWeight;
   QColor syn_TypeText;
   int    syn_ClassWeight;
   QColor syn_ClassText;
   int    syn_FuncWeight;
   QColor syn_FuncText;
   QColor syn_LineText;
   QColor syn_QuoteText;
   QColor syn_CommentText;
   QColor syn_MLineText;
};

class Syntax : public QSyntaxHighlighter
{
   Q_OBJECT

   public:
      Syntax(QTextDocument *parent, QString synFName);
      struct SyntaxColors get_StructData();
      void set_StructData(SyntaxColors data);

   protected:
      void highlightBlock(const QString &text);

   private:
      struct SyntaxColors m_struct;

      struct HighlightingRule
      {
         QRegExp pattern;
         QTextCharFormat format;
      };

      QVector<HighlightingRule> highlightingRules;

      QRegExp commentStartExpression;
      QRegExp commentEndExpression;

      QTextCharFormat multiLineCommentFormat;     
};

#endif
