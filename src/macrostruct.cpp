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

#include "macrostruct.h"

MacroStruct::MacroStruct() :
    m_key( 0 )
    , m_modifier( 0 )
    , m_text( "" )
{
}

MacroStruct::MacroStruct( int key, int modifier, const QString text ) :
    m_key( key )
    , m_modifier( modifier )
    , m_text( text )
{
}

MacroStruct::MacroStruct( const MacroStruct &other ) :
    m_key( other.m_key )
    , m_modifier( other.m_modifier )
    , m_text( other.m_text )
{
}

MacroStruct &MacroStruct::operator = ( const MacroStruct &other )
{
    if ( this != &other )
    {
        m_key       = other.m_key;
        m_modifier  = other.m_modifier;
        m_text      = other.m_text;
    }

    return *this;
}

bool operator ==( const MacroStruct &left, const MacroStruct &right )
{
    bool retVal = true;

    if ( left.m_key != right.m_key )
    {
        retVal = false;
    }

    if ( left.m_modifier != right.m_modifier )
    {
        retVal = false;
    }

    if ( left.m_text != right.m_text )
    {
        retVal = false;
    }

    return retVal;
}

bool operator !=( const MacroStruct &left, const MacroStruct &right )
{
    bool retVal = true;

    if ( left == right )
    {
        retVal = false;
    }

    return retVal;
}
