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

#ifndef SYNTAX_H
#define SYNTAX_H

#include "settings.h"
#include "spellcheck.h"

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>

class Syntax : public QSyntaxHighlighter
{
   CS_OBJECT(Syntax)

   public:
      Syntax(QTextDocument *document, QString synFName,
            const struct Settings &settings, SpellCheck *spell = nullptr);

      bool processSyntax();
      bool processSyntax(const struct Settings &settings);
      void set_Spell(bool value);

   protected:
      void highlightBlock(const QString &text) override;

   private:
      struct HighlightingRule
      {
         QRegularExpression pattern;
         QTextCharFormat format;
      };

      static QByteArray json_ReadFile(QString fileName);

      QString m_syntaxFile;
      struct Settings m_settings;

      SpellCheck *m_spellCheck;
      bool m_isSpellCheck;

      QRegularExpression m_commentStartExpression;
      QRegularExpression m_commentEndExpression;

      QTextCharFormat m_multiLineCommentFormat;
      QTextCharFormat m_spellCheckFormat;

      QVector<HighlightingRule> highlightingRules;
};

#endif
