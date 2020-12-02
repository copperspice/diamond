/**************************************************************************
*
* Copyright (c) 2020 Roland Hughes
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
#ifndef SYNTAXPATTERNS_H
#define SYNTAXPATTERNS_H

#include <QStringList>
#include <QByteArray>

class SyntaxPatterns
{
    CS_GADGET( SyntaxPatterns )

public:
    SyntaxPatterns( QString fileName );
    SyntaxPatterns( const SyntaxPatterns &patterns );

    SyntaxPatterns &operator = ( const SyntaxPatterns &other );

    QStringList key_Patterns;
    QStringList class_Patterns;
    QStringList func_Patterns;
    QStringList type_Patterns;
    QStringList constant_Patterns;
    QString commentSingle;
    QString commentStart;
    QString commentEnd;
    QString pattern_file;
    bool ignoreCase;

private:
    QByteArray readSyntaxFile();

};


#endif

