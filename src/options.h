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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QString>
#include <QMap>

#include "keydefinitions.h"

class Options
{
public:
    Options();
    Options( const Options &opt );

    Options &operator = ( const Options &opt );

    friend bool operator ==( const Options &left, const Options &right );
    friend bool operator !=( const Options &left, const Options &right );

    static const QString DEFAULT_BACKUP_DIR;

    //
    // getters
    //
    int             rewrapColumn()
    {
        return m_rewrapColumn;
    }
    int             tabSpacing()
    {
        return m_tabSpacing;
    }
    int             maxVersions()
    {
        return m_maxVersions;
    }
    bool            useSpaces()
    {
        return m_useSpaces;
    }
    bool            removeSpaces()
    {
        return m_removeSpaces;
    }
    bool            autoLoad()
    {
        return m_autoLoad;
    }
    bool            isAutoDetect()
    {
        return m_autoDetect;
    }
    bool            makeBackups()
    {
        return m_makeBackups;
    }
    bool            astyleOnSave()
    {
        return m_astyleOnSave;
    }
    bool            preloadClipper()
    {
        return m_preloadClipper;
    }
    bool            preloadCmake()
    {
        return m_preloadCmake;
    }
    bool            preloadCpp()
    {
        return m_preloadCpp;
    }
    bool            preloadCss()
    {
        return m_preloadCss;
    }
    bool            preloadDoxy()
    {
        return m_preloadDoxy;
    }
    bool            preloadErrLog()
    {
        return m_preloadErrLog;
    }
    bool            preloadHtml()
    {
        return m_preloadHtml;
    }
    bool            preloadJava()
    {
        return m_preloadJava;
    }
    bool            preloadJs()
    {
        return m_preloadJs;
    }
    bool            preloadJson()
    {
        return m_preloadJson;
    }
    bool            preloadMake()
    {
        return m_preloadMake;
    }
    bool            preloadNone()
    {
        return m_preloadNone;
    }
    bool            preloadNSI()
    {
        return m_preloadNSI;
    }
    bool            preloadPhp()
    {
        return m_preloadPhp;
    }
    bool            preloadPl()
    {
        return m_preloadPl;
    }
    bool            preloadPy()
    {
        return m_preloadPy;
    }
    bool            preloadSh()
    {
        return m_preloadSh;
    }
    bool            preloadTxt()
    {
        return m_preloadTxt;
    }
    bool            preloadXml()
    {
        return m_preloadXml;
    }
    QString         formatDate()
    {
        return m_formatDate;
    }
    QString         formatTime()
    {
        return m_formatTime;
    }
    QString         backupDirectory()
    {
        return m_backupDirectory;
    }
    QString         mainDictionary()
    {
        return m_mainDictionary;
    }
    QString         userDictionary()
    {
        return m_userDictionary;
    }
    QString         syntaxPath()
    {
        return m_syntaxPath;
    }
    QString         aboutUrl()
    {
        return m_aboutUrl;
    }

    KeyDefinitions &keys()
    {
        return m_keys;
    }

    //
    // setters
    //
    void set_rewrapColumn( int col )
    {
        m_rewrapColumn = col;
    }
    void set_tabSpacing( int tabStop )
    {
        m_tabSpacing = tabStop;
    }
    void set_maxVersions( int max )
    {
        m_maxVersions = max;
    }
    void set_useSpaces( bool yesNo )
    {
        m_useSpaces = yesNo;
    }
    void set_removeSpaces( bool yesNo )
    {
        m_removeSpaces = yesNo;
    }
    void set_autoLoad( bool yesNo )
    {
        m_autoLoad = yesNo;
    }
    void set_formatDate( QString fmt )
    {
        m_formatDate = fmt;
    }
    void set_formatTime( QString fmt )
    {
        m_formatTime = fmt;
    }
    void set_backupDirectory( QString directory )
    {
        m_backupDirectory = directory;
    }
    void set_mainDictionary( QString dictionary )
    {
        m_mainDictionary = dictionary;
    }
    void set_userDictionary( QString dictionary )
    {
        m_userDictionary = dictionary;
    }
    void set_syntaxPath( const QString path )
    {
        m_syntaxPath = path;
    }
    void set_abouturl( QString url )
    {
        m_aboutUrl = url;
    }
    void set_preloadClipper( bool yesNo )
    {
        m_preloadClipper = yesNo;
    }
    void set_preloadCmake( bool yesNo )
    {
        m_preloadCmake = yesNo;
    }
    void set_preloadCpp( bool yesNo )
    {
        m_preloadCpp = yesNo;
    }
    void set_preloadCss( bool yesNo )
    {
        m_preloadCss = yesNo;
    }
    void set_preloadDoxy( bool yesNo )
    {
        m_preloadDoxy = yesNo;
    }
    void set_preloadErrLog( bool yesNo )
    {
        m_preloadErrLog = yesNo;
    }
    void set_preloadHtml( bool yesNo )
    {
        m_preloadHtml = yesNo;
    }
    void set_preloadJava( bool yesNo )
    {
        m_preloadJava = yesNo;
    }
    void set_preloadJs( bool yesNo )
    {
        m_preloadJs = yesNo;
    }
    void set_preloadJson( bool yesNo )
    {
        m_preloadJson = yesNo;
    }
    void set_preloadMake( bool yesNo )
    {
        m_preloadMake = yesNo;
    }
    void set_preloadNone( bool yesNo )
    {
        m_preloadNone = yesNo;
    }
    void set_preloadNSI( bool yesNo )
    {
        m_preloadNSI = yesNo;
    }
    void set_preloadPhp( bool yesNo )
    {
        m_preloadPhp = yesNo;
    }
    void set_preloadPl( bool yesNo )
    {
        m_preloadPl = yesNo;
    }
    void set_preloadPy( bool yesNo )
    {
        m_preloadPy = yesNo;
    }
    void set_preloadSh( bool yesNo )
    {
        m_preloadSh = yesNo;
    }
    void set_preloadTxt( bool yesNo )
    {
        m_preloadTxt = yesNo;
    }
    void set_preloadXml( bool yesNo )
    {
        m_preloadXml = yesNo;
    }
    void set_astyleOnSave( bool yesNo )
    {
        m_astyleOnSave = yesNo;
    }
    void set_makeBackups( bool yesNo )
    {
        m_makeBackups = yesNo;
    }


private:
    int m_rewrapColumn;
    /*
       TODO:: tabSpacing needs to be implemented more like in Emacs.
     ;;
     ;;  Fix the tab-stop list to every 4 spaces for the first 200
     ;;
     (setq tab-stop-list (number-sequence 0 200 4))
     ;;
     ;;  Also allows for an actual ascending list so if you need a COBOL
     ;;  set of tab stops where comment is in 7; AREA-B starts in 8; 73 is
     ;;  additional comment to 80.
     ;;  See: https://www.logikalsolutions.com/wordpress/information-technology/most-text-editors-get-tabs-wrong/
    */
    int m_tabSpacing;
    int m_maxVersions;

    bool m_useSpaces;
    bool m_removeSpaces;
    bool m_autoLoad;
    bool m_autoDetect;      // do not save this to file - runtime only
    bool m_makeBackups;
    bool m_astyleOnSave;

    bool m_preloadClipper;
    bool m_preloadCmake;
    bool m_preloadCpp;
    bool m_preloadCss;
    bool m_preloadDoxy;
    bool m_preloadErrLog;
    bool m_preloadHtml;
    bool m_preloadJava;
    bool m_preloadJs;
    bool m_preloadJson;
    bool m_preloadMake;
    bool m_preloadNone;
    bool m_preloadNSI;
    bool m_preloadPhp;
    bool m_preloadPl;
    bool m_preloadPy;
    bool m_preloadSh;
    bool m_preloadTxt;
    bool m_preloadXml;

    QString m_formatDate;
    QString m_formatTime;
    QString m_mainDictionary;
    QString m_userDictionary;
    QString m_syntaxPath;
    QString m_aboutUrl;
    QString m_backupDirectory;

    KeyDefinitions m_keys;
};

enum SyntaxTypes {SYN_C, SYN_CLIPPER, SYN_CMAKE, SYN_CSS, SYN_DOXY, SYN_ERRLOG, SYN_HTML,
                  SYN_JAVA, SYN_JS, SYN_JSON, SYN_MAKE, SYN_NSIS, SYN_TEXT,
                  SYN_SHELL, SYN_PERL, SYN_PHP, SYN_PYTHON, SYN_XML,
                  SYN_NONE, SYN_UNUSED1, SYN_UNUSED2
                 };

#endif
