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
#include "printsettings.h"
#include "non_gui_functions.h"

PrintSettings::PrintSettings() :
    m_lineNumbers( true )
    , m_printHeader( true )
    , m_printFooter( true )
{
    m_headerLeft    = "$(PathFileName)";
    m_headerCenter  = "";
    m_headerRight   =  "Page $(PageNo)";
    m_headerLine2   = "";
    m_footerLeft    = "$(Date)";
    m_footerCenter  = "";
    m_footerRight   = "Diamond Editor";
    m_footerLine2   = "";
    m_marginTop     = 0.75;
    m_marginBottom  = 0.75;
    m_marginLeft    = 0.50;
    m_marginRight   = 0.50;
    m_headerGap     = 0.25;
    m_fontHeader    = QFont( "Monospace", 10 );
    m_fontFooter    = QFont( "Monospace", 10 );
    m_fontText      = QFont( "Monospace", 12 );
}

PrintSettings::PrintSettings( const PrintSettings &other ) :
    m_lineNumbers( other.m_lineNumbers )
    , m_printHeader( other.m_printHeader )
    , m_printFooter( other.m_printFooter )
    , m_headerLeft( other.m_headerLeft )
    , m_headerCenter( other.m_headerCenter )
    , m_headerRight( other.m_headerRight )
    , m_headerLine2( other.m_headerLine2 )
    , m_footerLeft( other.m_footerLeft )
    , m_footerCenter( other.m_footerCenter )
    , m_footerRight( other.m_footerRight )
    , m_footerLine2( other.m_footerLine2 )
    , m_marginLeft( other.m_marginLeft )
    , m_marginTop( other.m_marginTop )
    , m_marginRight( other.m_marginRight )
    , m_marginBottom( other.m_marginBottom )
    , m_headerGap( other.m_headerGap )
    , m_fontHeader( other.m_fontHeader )
    , m_fontFooter( other.m_fontFooter )
    , m_fontText( other.m_fontText )
{
}

PrintSettings &PrintSettings::operator =( const PrintSettings &other )
{
    if ( this != &other )
    {
        m_lineNumbers       = other.m_lineNumbers;
        m_printHeader       = other.m_printHeader;
        m_printFooter       = other.m_printFooter;
        m_headerLeft        = other.m_headerLeft;
        m_headerCenter      = other.m_headerCenter;
        m_headerRight       = other.m_headerRight;
        m_headerLine2       = other.m_headerLine2;
        m_footerLeft        = other.m_footerLeft;
        m_footerCenter      = other.m_footerCenter;
        m_footerRight       = other.m_footerRight;
        m_footerLine2       = other.m_footerLine2;
        m_marginLeft        = other.m_marginLeft;
        m_marginTop         = other.m_marginTop;
        m_marginRight       = other.m_marginRight;
        m_marginBottom      = other.m_marginBottom;
        m_headerGap         = other.m_headerGap;
        m_fontHeader        = other.m_fontHeader;
        m_fontFooter        = other.m_fontFooter;
        m_fontText          = other.m_fontText;
    }

    return *this;
}

bool operator ==( const PrintSettings &left, const PrintSettings &right )
{
    bool retVal = true;

    if ( left.m_lineNumbers != right.m_lineNumbers )
    {
        retVal = false;
    }

    if ( left.m_printHeader != right.m_printHeader )
    {
        retVal = false;
    }

    if ( left.m_printFooter != right.m_printFooter )
    {
        retVal = false;
    }

    if ( left.m_headerLeft != right.m_headerLeft )
    {
        retVal = false;
    }

    if ( left.m_headerCenter != right.m_headerCenter )
    {
        retVal = false;
    }

    if ( left.m_headerRight != right.m_headerRight )
    {
        retVal = false;
    }

    if ( left.m_headerLine2 != right.m_headerLine2 )
    {
        retVal = false;
    }

    if ( left.m_footerLeft != right.m_footerLeft )
    {
        retVal = false;
    }

    if ( left.m_footerCenter != right.m_footerCenter )
    {
        retVal = false;
    }

    if ( left.m_footerRight != right.m_footerRight )
    {
        retVal = false;
    }

    if ( left.m_footerLine2 != right.m_footerLine2 )
    {
        retVal = false;
    }

    if ( !doubleEqualFourDigits( left.m_marginLeft, right.m_marginLeft ) )
    {
        retVal = false;
    }

    if ( !doubleEqualFourDigits( left.m_marginTop, right.m_marginTop ) )
    {
        retVal = false;
    }

    if ( !doubleEqualFourDigits( left.m_marginRight, right.m_marginRight ) )
    {
        retVal = false;
    }

    if ( !doubleEqualFourDigits( left.m_marginBottom, right.m_marginBottom ) )
    {
        retVal = false;
    }

    if ( !doubleEqualFourDigits( left.m_headerGap, right.m_headerGap ) )
    {
        retVal = false;
    }

    if ( left.m_fontHeader != right.m_fontHeader )
    {
        retVal = false;
    }

    if ( left.m_fontFooter != right.m_fontFooter )
    {
        retVal = false;
    }

    if ( left.m_fontText != right.m_fontText )
    {
        retVal = false;
    }

    return retVal;

}

bool operator !=( const PrintSettings &left, const PrintSettings &right )
{
    bool retVal = true;

    if ( left == right )
    {
        retVal = false;
    }

    return retVal;
}
