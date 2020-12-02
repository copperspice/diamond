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
#ifndef MACROSTRUCT_H
#define MACROSTRUCT_H

#include <QString>

class MacroStruct
{
    CS_GADGET( MacroStruct )
public:
    MacroStruct();
    MacroStruct( int key, int modifier, const QString text );
    MacroStruct( const MacroStruct &other );

    MacroStruct &operator = ( const MacroStruct &other );
    friend bool operator ==( const MacroStruct &left, const MacroStruct &right );
    friend bool operator !=( const MacroStruct &left, const MacroStruct &right );

    int m_key;
    int m_modifier;
    QString m_text;
};

#endif
