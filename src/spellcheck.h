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

#ifndef SPELLCHECK_H
#define SPELLCHECK_H

#include <QString>

class Hunspell;

class SpellCheck
{
public:
    SpellCheck( const QString &dictMain, const QString &dictUser );
    SpellCheck( const SpellCheck &def );
    ~SpellCheck();

    bool spell( QStringView word );
    QStringList suggest( const QString &word );
    void ignoreWord( const QString &word );
    void addToUserDict( const QString &word );

    SpellCheck &operator =( const SpellCheck &def );

private:
    QString m_userFname;
    QTextCodec *m_codec;

    Hunspell *m_hunspell;

    void put_word( const QString &word );
};

#endif
