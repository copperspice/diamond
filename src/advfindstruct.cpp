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

#include "advfindstruct.h"

AdvFindStruct::AdvFindStruct() :
    lineNumber( 0 )
{
}

AdvFindStruct::AdvFindStruct( QString file_Name, int line_Number, QString txt ) :
    fileName( file_Name )
    , lineNumber( line_Number )
    , text( txt )
{
}

AdvFindStruct::AdvFindStruct( const AdvFindStruct &other ) :
    fileName( other.fileName )
    , lineNumber( other.lineNumber )
    , text( other.text )
{
}

AdvFindStruct &AdvFindStruct::operator=( const AdvFindStruct &other )
{
    if ( this != &other )
    {
        fileName    = other.fileName;
        lineNumber  = other.lineNumber;
        text        = other.text;
    }

    return *this;
}

bool operator ==( const AdvFindStruct &left, const AdvFindStruct &right )
{
    bool retVal = true;

    if ( left.fileName != right.fileName )
    {
        retVal = false;
    }

    if ( left.lineNumber != right.lineNumber )
    {
        retVal = false;
    }

    if ( left.text != right.text )
    {
        retVal = false;
    }

    return retVal;
}

bool operator !=( const AdvFindStruct &left, const AdvFindStruct &right )
{
    bool retVal = true;

    if ( left == right )
    {
        retVal = false;
    }

    return retVal;
}
