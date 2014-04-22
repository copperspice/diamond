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

#ifndef SYNTAX_H
#define SYNTAX_H

#include "settings.h"
#include "spellcheck.h"

#include <QHash>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>

class Syntax : public QSyntaxHighlighter
{
   CS_OBJECT(Syntax)

   public:
      Syntax(QTextDocument *document,
             QString synFName, const struct Settings &settings, SpellCheck *spell = 0);

      ~Syntax();
      bool processSyntax();
      bool processSyntax(const struct Settings &settings);
      void set_Spell(bool value);

   protected:
      void highlightBlock(const QString &text);

   private:
      QString m_syntaxFile;
      struct Settings m_settings;

      SpellCheck *m_spellCheck;
      bool m_isSpellCheck;

      QRegExp m_commentStartExpression;
      QRegExp m_commentEndExpression;

      QTextCharFormat m_multiLineCommentFormat;
      QTextCharFormat m_spellCheckFormat;

      QByteArray json_ReadFile(QString fileName);

      //
      struct HighlightingRule
      {
         QRegExp pattern;
         QTextCharFormat format;
      };

      QVector<HighlightingRule> highlightingRules;     
};

#endif
