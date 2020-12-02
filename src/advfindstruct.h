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

#ifndef ADVFINDSTRUCT_H
#define ADVFINDSTRUCT_H

#include <QString>

class AdvFindStruct
{
    CS_GADGET( AdvFindStruct )
public:
    AdvFindStruct();
    AdvFindStruct( QString file_Name, int line_Number, QString txt );
    AdvFindStruct( const AdvFindStruct &other );

    AdvFindStruct &operator = ( const AdvFindStruct &other );
    friend bool operator ==( const AdvFindStruct &left, const AdvFindStruct &right );
    friend bool operator !=( const AdvFindStruct &left, const AdvFindStruct &right );

    QString fileName;
    int lineNumber;
    QString text;
};

#endif
