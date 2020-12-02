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

#ifndef THEMES_H
#define THEMES_H

#include <QColor>
#include "textattributes.h"

class Themes
{
    CS_GADGET( Themes )
public:
    Themes();
    Themes( const QString name, bool isProtected = false );
    Themes( const Themes &theme );

    Themes &operator = ( const Themes &theme );
    friend bool operator ==( const Themes &left, const Themes &right );
    friend bool operator !=( const Themes &left, const Themes &right );

    //
    // getters
    //
    bool   isProtected()
    {
        return m_protected;
    }
    QString name()
    {
        return m_name;
    }
    QColor colorText()
    {
        return m_colorText;
    }
    QColor colorBack()
    {
        return m_colorBack;
    }
    QColor gutterText()
    {
        return m_gutterText;
    }
    QColor gutterBack()
    {
        return m_gutterBack;
    }
    QColor currentLineBack()
    {
        return m_currentLineBack;
    }
    TextAttributes &syntaxKey()
    {
        return m_synKey;
    }
    TextAttributes &syntaxType()
    {
        return m_synType;
    }
    TextAttributes &syntaxClass()
    {
        return m_synClass;
    }
    TextAttributes &syntaxFunc()
    {
        return m_synFunc;
    }
    TextAttributes &syntaxQuote()
    {
        return m_synQuote;
    }
    TextAttributes &syntaxComment()
    {
        return m_synComment;
    }
    TextAttributes &syntaxMLine()
    {
        return m_synMLine;
    }
    TextAttributes &syntaxConstant()
    {
        return m_synConstant;
    }

    //
    // setters
    //
    void set_protection( bool yesNo )
    {
        m_protected = yesNo;
    }
    void set_colorText( QColor color )
    {
        m_colorText = color;
    }
    void set_colorBack( QColor color )
    {
        m_colorBack = color;
    }
    void set_gutterText( QColor color )
    {
        m_gutterText = color;
    }
    void set_gutterBack( QColor color )
    {
        m_gutterBack = color;
    }
    void set_name( QString name )
    {
        m_name = name;
    }
    void set_protected( bool yesNo )
    {
        m_protected = yesNo;
    }
    void set_currentLineBack( QColor color )
    {
        m_currentLineBack = color;
    }
    void set_syntaxKey( TextAttributes &attr )
    {
        m_synKey = attr;
    }
    void set_syntaxType( TextAttributes &attr )
    {
        m_synType = attr;
    }
    void set_syntaxClass( TextAttributes &attr )
    {
        m_synClass = attr;
    }
    void set_syntaxFunc( TextAttributes &attr )
    {
        m_synFunc = attr;
    }
    void set_syntaxQuote( TextAttributes &attr )
    {
        m_synQuote = attr;
    }
    void set_syntaxComment( TextAttributes &attr )
    {
        m_synComment = attr;
    }
    void set_syntaxMLine( TextAttributes &attr )
    {
        m_synMLine = attr;
    }
    void set_syntaxConstant( TextAttributes &attr )
    {
        m_synConstant = attr;
    }

private:
    bool    m_protected;
    QString m_name;
    QColor  m_colorText;
    QColor  m_colorBack;
    QColor  m_gutterText;
    QColor  m_gutterBack;
    QColor  m_currentLineBack;


    TextAttributes  m_synKey;
    TextAttributes  m_synType;
    TextAttributes  m_synClass;
    TextAttributes  m_synFunc;
    TextAttributes  m_synQuote;
    TextAttributes  m_synComment;
    TextAttributes  m_synMLine;
    TextAttributes  m_synConstant;
};

#endif
