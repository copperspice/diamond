/**************************************************************************
*
* Copyright (c) 2012-2020 Barbara Geller
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

#include "overlord.h"
#include "spellcheck.h"

#include <QHash>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QVector>

//
struct HighlightingRule
{
    QRegularExpression pattern;
    QTextCharFormat format;
};

class Syntax : public QSyntaxHighlighter
{
    CS_OBJECT( Syntax )

public:
    Syntax( QTextDocument *document, QString synFName, SpellCheck *spell = 0 );

    ~Syntax();
    void set_Spell( bool value );

    void processSyntax( Settings *settings );

protected:
    void highlightBlock( const QString &text );

private:
    void deleteHighlightingRules();

    QString m_syntaxFile;

    SpellCheck *m_spellCheck;
    bool m_isSpellCheck;

    QRegularExpression m_commentStartExpression;
    QRegularExpression m_commentEndExpression;

    QTextCharFormat m_multiLineCommentFormat;
    QTextCharFormat m_spellCheckFormat;


    QVector<HighlightingRule *> m_highlightingRules;
};

#endif
