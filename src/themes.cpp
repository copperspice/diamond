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

#include "themes.h"
#include <QDebug>

Themes::Themes() :
    m_protected( false )
{
}

Themes::Themes( const QString name, bool isProtected ) :
    m_protected( isProtected )
    , m_name( name )
{
}


Themes::Themes( const Themes &theme ) :
    m_protected( theme.m_protected )
    , m_name( theme.m_name )
    , m_colorText( theme.m_colorText )
    , m_colorBack( theme.m_colorBack )
    , m_gutterText( theme.m_gutterText )
    , m_gutterBack( theme.m_gutterBack )
    , m_currentLineBack( theme.m_currentLineBack )
    , m_synKey( theme.m_synKey )
    , m_synType( theme.m_synType )
    , m_synClass( theme.m_synClass )
    , m_synFunc( theme.m_synFunc )
    , m_synQuote( theme.m_synQuote )
    , m_synComment( theme.m_synComment )
    , m_synMLine( theme.m_synMLine )
    , m_synConstant( theme.m_synConstant )
{
}

Themes &Themes::operator=( const Themes &theme )
{
    if ( this != &theme )
    {
        m_protected         = theme.m_protected;
        m_name              = theme.m_name;
        m_colorText         = theme.m_colorText;
        m_colorBack         = theme.m_colorBack;
        m_gutterText        = theme.m_gutterText;
        m_gutterBack        = theme.m_gutterBack;
        m_currentLineBack   = theme.m_currentLineBack;
        m_synKey            = theme.m_synKey;
        m_synType           = theme.m_synType;
        m_synClass          = theme.m_synClass;
        m_synFunc           = theme.m_synFunc;
        m_synQuote          = theme.m_synQuote;
        m_synComment        = theme.m_synComment;
        m_synMLine          = theme.m_synMLine;
        m_synConstant       = theme.m_synConstant;
    }

    return *this;
}

bool operator ==( const Themes &left, const Themes &right )
{
    bool retVal = true;

    if ( left.m_protected != right.m_protected )
    {
        retVal = false;
    }

    if ( left.m_name != right.m_name )
    {
        retVal = false;
    }

    if ( left.m_colorText != right.m_colorText )
    {
        retVal = false;
    }

    if ( left.m_colorBack != right.m_colorBack )
    {
        retVal = false;
    }

    if ( left.m_gutterText != right.m_gutterText )
    {
        retVal = false;
    }

    if ( left.m_gutterBack != right.m_gutterBack )
    {
        retVal = false;
    }

    if ( left.m_currentLineBack != right.m_currentLineBack )
    {
        retVal = false;
    }

    if ( left.m_synKey != right.m_synKey )
    {
        retVal = false;
    }

    if ( left.m_synType != right.m_synType )
    {
        retVal = false;
    }

    if ( left.m_synClass != right.m_synClass )
    {
        retVal = false;
    }

    if ( left.m_synFunc != right.m_synFunc )
    {
        retVal = false;
    }

    if ( left.m_synQuote != right.m_synQuote )
    {
        retVal = false;
    }

    if ( left.m_synComment != right.m_synComment )
    {
        retVal = false;
    }

    if ( left.m_synMLine != right.m_synMLine )
    {
        retVal = false;
    }

    if ( left.m_synConstant != right.m_synConstant )
    {
        retVal = false;
    }

    return retVal;
}

bool operator !=( const Themes &left, const Themes &right )
{
    bool retVal = true;

    if ( left == right )
    {
        retVal = false;
    }

    return retVal;
}

