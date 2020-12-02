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

#ifndef PRINTSETTINGS_H
#define PRINTSETTINGS_H

#include <QString>
#include <QFont>

class PrintSettings
{
public:

    PrintSettings();
    PrintSettings( const PrintSettings &other );

    PrintSettings &operator =( const PrintSettings &other );

    friend bool operator ==( const PrintSettings &left, const PrintSettings &right );
    friend bool operator !=( const PrintSettings &left, const PrintSettings &right );

    //
    // getters
    //
    bool    lineNumbers()
    {
        return m_lineNumbers;
    }
    bool    printHeader()
    {
        return m_printHeader;
    }
    bool    printFooter()
    {
        return m_printFooter;
    }
    QString headerLeft()
    {
        return m_headerLeft;
    }
    QString headerCenter()
    {
        return m_headerCenter;
    }
    QString headerRight()
    {
        return m_headerRight;
    }
    QString headerLine2()
    {
        return m_headerLine2;
    }
    QString footerLeft()
    {
        return m_footerLeft;
    }
    QString footerCenter()
    {
        return m_footerCenter;
    }
    QString footerRight()
    {
        return m_footerRight;
    }
    QString footerLine2()
    {
        return m_footerLine2;
    }
    double  marginLeft()
    {
        return m_marginLeft;
    }
    double  marginRight()
    {
        return m_marginRight;
    }
    double  marginTop()
    {
        return m_marginTop;
    }
    double  marginBottom()
    {
        return m_marginBottom;
    }
    double  headerGap()
    {
        return m_headerGap;
    }
    QFont   fontHeader()
    {
        return m_fontHeader;
    }
    QFont   fontFooter()
    {
        return m_fontFooter;
    }
    QFont   fontText()
    {
        return m_fontText;
    }

    //
    // setters
    //
    void set_lineNumbers( bool yesNo )
    {
        m_lineNumbers = yesNo;
    }
    void set_printHeader( bool yesNo )
    {
        m_printHeader = yesNo;
    }
    void set_printFooter( bool yesNo )
    {
        m_printFooter = yesNo;
    }
    void set_headerLeft( QString hdr )
    {
        m_headerLeft = hdr;
    }
    void set_headerCenter( QString hdr )
    {
        m_headerCenter = hdr;
    }
    void set_headerRight( QString hdr )
    {
        m_headerRight = hdr;
    }
    void set_headerLine2( QString hdr )
    {
        m_headerLine2 = hdr;
    }
    void set_footerLeft( QString ftr )
    {
        m_footerLeft = ftr;
    }
    void set_footerCenter( QString ftr )
    {
        m_footerCenter = ftr;
    }
    void set_footerRight( QString ftr )
    {
        m_footerRight = ftr;
    }
    void set_footerLine2( QString ftr )
    {
        m_footerLine2 = ftr;
    }
    void set_marginLeft( double margin )
    {
        m_marginLeft = margin;
    }
    void set_marginTop( double margin )
    {
        m_marginTop = margin;
    }
    void set_marginRight( double margin )
    {
        m_marginRight = margin;
    }
    void set_marginBottom( double margin )
    {
        m_marginBottom = margin;
    }
    void set_headerGap( double gap )
    {
        m_headerGap = gap;
    }
    void set_fontHeader( QFont font )
    {
        m_fontHeader = font;
    }
    void set_fontFooter( QFont font )
    {
        m_fontFooter = font;
    }
    void set_fontText( QFont font )
    {
        m_fontText = font;
    }

private:
    bool    m_lineNumbers;
    bool    m_printHeader;
    bool    m_printFooter;
    QString m_headerLeft;
    QString m_headerCenter;
    QString m_headerRight;
    QString m_headerLine2;
    QString m_footerLeft;
    QString m_footerCenter;
    QString m_footerRight;
    QString m_footerLine2;

    double m_marginLeft;
    double m_marginTop;
    double m_marginRight;
    double m_marginBottom;
    double m_headerGap;

    QFont m_fontHeader;
    QFont m_fontFooter;
    QFont m_fontText;
};

#endif
