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

#include "options.h"
#include "non_gui_functions.h"
#include <QDir>
#include "util.h"

const QString Options::DEFAULT_BACKUP_DIR = QString( "Diamond_Backups" );

Options::Options() :
    m_rewrapColumn( 120 )
    , m_tabSpacing( 4 )
    , m_maxVersions( 12 )
    , m_useSpaces( true )
    , m_removeSpaces( false )
    , m_autoLoad( true )
    , m_autoDetect( false )
    , m_makeBackups( false )
    , m_astyleOnSave( false )
    , m_preloadClipper( false )
    , m_preloadCmake( false )
    , m_preloadCpp( true )
    , m_preloadCss( false )
    , m_preloadDoxy( false )
    , m_preloadErrLog( false )
    , m_preloadHtml( false )
    , m_preloadJava( false )
    , m_preloadJs( false )
    , m_preloadJson( false )
    , m_preloadMake( false )
    , m_preloadNone( false )
    , m_preloadNSI( false )
    , m_preloadPhp( false )
    , m_preloadPl( false )
    , m_preloadPy( false )
    , m_preloadSh( false )
    , m_preloadTxt( true )
    , m_preloadXml( false )
    , m_formatDate( QString( "MM/dd/yyyy" ) )
    , m_formatTime( QString( "h:mm ap" ) )
{
    // TODO:: kind of sucks having to have multiple classes know about appPath.
    //        Redistribute the data so that only one class has all of the
    //        path based information.
    //
    QString resourcePath = QCoreApplication::applicationDirPath();
    QString libraryPath  = QDir::homePath() + "/.config/Diamond/";


    // TODO:: refactor this #if. We do much the same on both sides of it.
    //
#if defined(Q_OS_UNIX) && ! defined(Q_OS_MAC)

    m_autoDetect = true;

    QDir d;
    QString pth = libraryPath + "dictionary";

    if ( !d.exists( pth ) )
    {
        bool rslt = d.mkpath( pth );

        if ( !rslt )
        {
            csError( "Options", "Failed to create directory path: " + pth );
        }
    }


    // get syntax folder (1)
    m_syntaxPath        = resourcePath + "/syntax/";
    m_mainDictionary    = libraryPath + "dictionary/en_US.dic";
    m_userDictionary    = libraryPath + "dictionary/userDict.txt";

    if ( ! QFile::exists( m_userDictionary ) )
    {
        QFile temp( m_userDictionary );
        temp.open( QIODevice::WriteOnly );
        temp.close();
    }


#elif defined(Q_OS_MAC)

    if ( resourcePath.contains( ".app/Contents/MacOS" ) )
    {
        m_autoDetect = true;
        resourcePath = pathName( QCoreApplication::applicationDirPath() )
                       + "/../Contents/Resources";
        libraryPath  = QDir::homePath() + "/Library/Diamond/";

        QDir d;
        QString pth = libraryPath + "dictionary";

        if ( !d.exists( pth ) )
        {
            bool rslt = d.mkpath( pth );

            if ( !rslt )
            {
                csError( "Options", "Failed to create directory path: " + pth );
            }
        }

        QDir dir = resourcePath;
        dir.makeAbsolute();

        resourcePath        = dir.path();
        m_syntaxPath        = resourcePath + "/syntax/";
        m_mainDictionary    = libraryPath + "dictionary/en_US.dic";
        m_userDictionary    = libraryPath + "dictionary/userDict.txt";

        if ( ! QFile::exists( m_userDictionary ) )
        {
            QFile temp( dictFile );
            temp.open( QIODevice::WriteOnly );
            temp.close();
        }
    }

#endif

    QDir home( QDir::homePath() );

    m_backupDirectory = home.absoluteFilePath( DEFAULT_BACKUP_DIR );

    if ( !home.exists( DEFAULT_BACKUP_DIR ) )
    {
        home.mkdir( DEFAULT_BACKUP_DIR );
    }

    QString affFile         = m_mainDictionary.left( m_mainDictionary.length() - 3 ) + "aff";
    QString affResource     = resourcePath + "/dictionary/en_US.aff";
    QString en_USResource   = resourcePath + "/dictionary/en_US.dic";
    QString userResource    = resourcePath + "/dictionary/userDict.txt";

    // TODO:: QFile::copy() is busted.
    //        Need to see how long before QFile::copy() will be fixed.
    //
    //        Ubuntu 18 g++7 doesn't have full -std=c++17 support.
    //        at some point g++8 moved filesystem from experimental into
    //        main library so could just #include <filesystem> and use the code below
    //
    //        Don't want to drag g++8 experimental lib around or have convoluted
    //        build to detect when building on partial C++17 support.
    //
    //        Besides, QFile::copy() should get fixed.

#if 0
    std::error_code ec;

    fs::copy( affResource, affFile, ec );
    fs::copy( en_USResource, m_mainDictionary, ec );
    fs::copy( userResource, m_userDictionary );

    QString ecStr = QString::fromStdString( ec.message() );

#else
    QString cpyAffCmd;
    QString cpyUSCmd;
    QString cpyUserCmd;
#ifdef Q_OS_WIN
    cpyAffCmd   = QString( "copy \"%1\" \"%2\"" ).formatArgs( affResource, affFile );
    cpyUSCmd    = QString( "copy \"%1\" \"%2\"" ).formatArgs( en_USResource, m_mainDictionary );
    cpyUserCmd  = QString( "copy \"%1\" \"%2\"" ).formatArgs( userResource, m_userDictionary );
#else
    cpyAffCmd   = QString( "cp \"%1\" \"%2\"" ).formatArgs( affResource, affFile );
    cpyUSCmd    = QString( "cp \"%1\" \"%2\"" ).formatArgs( en_USResource, m_mainDictionary );
    cpyUserCmd  = QString( "cp \"%1\" \"%2\"" ).formatArgs( userResource, m_userDictionary );
#endif

    if ( system( cpyAffCmd.toStdString().c_str() ) != EXIT_SUCCESS )
    {
        csError( "Options", "Failed to copy AFF file" );
    }

    if ( system( cpyUSCmd.toStdString().c_str() ) != EXIT_SUCCESS )
    {
        csError( "Options", "Failed to copy main default dictionary" );
    }

    if ( system( cpyUserCmd.toStdString().c_str() ) != EXIT_SUCCESS )
    {
        csError( "Options", "Failed to copy User dictionary" );
    }

#endif

    if ( ! m_autoDetect )
    {
        m_syntaxPath = QCoreApplication::applicationDirPath() + "/syntax/";
    }
}

Options::Options( const Options &opt ) :
    m_rewrapColumn( opt.m_rewrapColumn )
    , m_tabSpacing( opt.m_tabSpacing )
    , m_maxVersions( opt.m_maxVersions )
    , m_useSpaces( opt.m_useSpaces )
    , m_removeSpaces( opt.m_removeSpaces )
    , m_autoLoad( opt.m_autoLoad )
    , m_autoDetect( opt.m_autoLoad )
    , m_makeBackups( opt.m_makeBackups )
    , m_astyleOnSave( opt.m_astyleOnSave )
    , m_preloadClipper( opt.m_preloadClipper )
    , m_preloadCmake( opt.m_preloadCmake )
    , m_preloadCpp( opt.m_preloadCpp )
    , m_preloadCss( opt.m_preloadCss )
    , m_preloadDoxy( opt.m_preloadDoxy )
    , m_preloadErrLog( opt.m_preloadErrLog )
    , m_preloadHtml( opt.m_preloadHtml )
    , m_preloadJava( opt.m_preloadJava )
    , m_preloadJs( opt.m_preloadJs )
    , m_preloadJson( opt.m_preloadJson )
    , m_preloadMake( opt.m_preloadMake )
    , m_preloadNone( opt.m_preloadNone )
    , m_preloadNSI( opt.m_preloadNSI )
    , m_preloadPhp( opt.m_preloadPhp )
    , m_preloadPl( opt.m_preloadPl )
    , m_preloadPy( opt.m_preloadPy )
    , m_preloadSh( opt.m_preloadSh )
    , m_preloadTxt( opt.m_preloadTxt )
    , m_preloadXml( opt.m_preloadXml )
    , m_formatDate( opt.m_formatDate )
    , m_formatTime( opt.m_formatTime )
    , m_mainDictionary( opt.m_mainDictionary )
    , m_userDictionary( opt.m_userDictionary )
    , m_syntaxPath( opt.m_syntaxPath )
    , m_aboutUrl( opt.m_aboutUrl )
    , m_backupDirectory( opt.m_backupDirectory )
    , m_keys( opt.m_keys )
{

}

Options &Options::operator =( const Options &opt )
{
    if ( this != &opt )
    {
        m_rewrapColumn      = opt.m_rewrapColumn;
        m_tabSpacing        = opt.m_tabSpacing;
        m_maxVersions       = opt.m_maxVersions;
        m_removeSpaces      = opt.m_removeSpaces;
        m_useSpaces         = opt.m_useSpaces;
        m_autoLoad          = opt.m_autoLoad;
        m_autoDetect        = opt.m_autoDetect;
        m_makeBackups       = opt.m_makeBackups;
        m_astyleOnSave      = opt.m_astyleOnSave;
        m_preloadClipper    = opt.m_preloadClipper;
        m_preloadCmake      = opt.m_preloadCmake;
        m_preloadCpp        = opt.m_preloadCpp;
        m_preloadCss        = opt.m_preloadCss;
        m_preloadDoxy       = opt.m_preloadDoxy;
        m_preloadErrLog     = opt.m_preloadErrLog;
        m_preloadHtml       = opt.m_preloadHtml;
        m_preloadJava       = opt.m_preloadJava;
        m_preloadJs         = opt.m_preloadJs;
        m_preloadJson       = opt.m_preloadJson;
        m_preloadMake       = opt.m_preloadMake;
        m_preloadNone       = opt.m_preloadNone;
        m_preloadNSI        = opt.m_preloadNSI;
        m_preloadPhp        = opt.m_preloadPhp;
        m_preloadPl         = opt.m_preloadPl;
        m_preloadPy         = opt.m_preloadPy;
        m_preloadSh         = opt.m_preloadSh;
        m_preloadTxt        = opt.m_preloadTxt;
        m_preloadXml        = opt.m_preloadXml;
        m_formatDate        = opt.m_formatDate;
        m_formatTime        = opt.m_formatTime;
        m_mainDictionary    = opt.m_mainDictionary;
        m_userDictionary    = opt.m_userDictionary;
        m_syntaxPath        = opt.m_syntaxPath;
        m_aboutUrl          = opt.m_aboutUrl;
        m_keys              = opt.m_keys;
    }

    return *this;
}

bool operator ==( const Options &left, const Options &right )
{
    bool retVal = false;

    if ( left.m_rewrapColumn != right.m_rewrapColumn )
    {
        retVal = false;
    }

    if ( left.m_tabSpacing != right.m_tabSpacing )
    {
        retVal = false;
    }

    if ( left.m_maxVersions != right.m_maxVersions )
    {
        retVal = false;
    }

    if ( left.m_useSpaces != right.m_useSpaces )
    {
        retVal = false;
    }

    if ( left.m_removeSpaces != right.m_removeSpaces )
    {
        retVal = false;
    }

    if ( left.m_autoLoad != right.m_autoLoad )
    {
        retVal = false;
    }

    if ( left.m_autoDetect != right.m_autoDetect )
    {
        retVal = false;
    }

    if ( left.m_makeBackups != right.m_makeBackups )
    {
        retVal = false;
    }

    if ( left.m_astyleOnSave != right.m_astyleOnSave )
    {
        retVal = false;
    }

    if ( left.m_preloadClipper != right.m_preloadClipper )
    {
        retVal = false;
    }

    if ( left.m_preloadCmake != right.m_preloadCmake )
    {
        retVal = false;
    }

    if ( left.m_preloadCpp != right.m_preloadCpp )
    {
        retVal = false;
    }

    if ( left.m_preloadCss != right.m_preloadCss )
    {
        retVal = false;
    }

    if ( left.m_preloadDoxy != right.m_preloadDoxy )
    {
        retVal = false;
    }

    if ( left.m_preloadErrLog != right.m_preloadErrLog )
    {
        retVal = false;
    }

    if ( left.m_preloadHtml != right.m_preloadHtml )
    {
        retVal = false;
    }

    if ( left.m_preloadJava != right.m_preloadJava )
    {
        retVal = false;
    }

    if ( left.m_preloadJs != right.m_preloadJs )
    {
        retVal = false;
    }

    if ( left.m_preloadJson != right.m_preloadJson )
    {
        retVal = false;
    }

    if ( left.m_preloadMake != right.m_preloadMake )
    {
        retVal = false;
    }

    if ( left.m_preloadNone != right.m_preloadNone )
    {
        retVal = false;
    }

    if ( left.m_preloadNSI != right.m_preloadNSI )
    {
        retVal = false;
    }

    if ( left.m_preloadPhp != right.m_preloadPhp )
    {
        retVal = false;
    }

    if ( left.m_preloadPl != right.m_preloadPl )
    {
        retVal = false;
    }

    if ( left.m_preloadPy != right.m_preloadPy )
    {
        retVal = false;
    }

    if ( left.m_preloadSh != right.m_preloadSh )
    {
        retVal = false;
    }

    if ( left.m_preloadTxt != right.m_preloadTxt )
    {
        retVal = false;
    }

    if ( left.m_preloadXml != right.m_preloadXml )
    {
        retVal = false;
    }

    if ( left.m_formatDate != right.m_formatDate )
    {
        retVal = false;
    }

    if ( left.m_formatTime != right.m_formatTime )
    {
        retVal = false;
    }

    if ( left.m_backupDirectory != right.m_backupDirectory )
    {
        retVal = false;
    }

    if ( left.m_mainDictionary != right.m_mainDictionary )
    {
        retVal = false;
    }

    if ( left.m_userDictionary != right.m_userDictionary )
    {
        retVal = false;
    }

    if ( left.m_syntaxPath != right.m_syntaxPath )
    {
        retVal = false;
    }

    if ( left.m_aboutUrl != right.m_aboutUrl )
    {
        retVal = false;
    }

    return retVal;
}

bool operator !=( const Options &left, const Options &right )
{
    bool retVal = true;

    if ( left == right )
    {
        retVal = false;
    }

    return retVal;
}
