/**************************************************************************
*
* Copyright (c) 2012-2020 Roland Hughes
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

#include "settings.h"
#include "util.h"
#include "diamondlimits.h"
#include "non_gui_functions.h"

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFlags>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QKeySequence>
#include <QPoint>
#include <QPushButton>
#include <QSettings>
#include <qglobal.h>
#include <QKeyEvent>
#include <QStringParser>
#include <QDebug>

Settings::Settings() :
    m_advancedFCase( false )
    , m_advancedFSearchFolders( false )
    , m_advancedFWholeWords( false )
    , m_advancedFindFileType( "." )
    , m_advancedFindText( "diamond" )
    , m_findCase( false )
    , m_findDirection( false )
    , m_findWholeWords( false )
    , m_flagNoAutoLoad( false )
    , m_flagNoSaveConfig( false )
    , m_isColumnMode( false )
    , m_isComplete( false )
    , m_isSpellCheck( false )
    , m_isWordWrap( false )
    , m_lastPosition( QPoint( 400, 200 ) )
    , m_lastSize( QSize( 800, 600 ) )
    , m_lastActiveRow( 0 )
    , m_lastActiveColumn( 0 )
    , m_showBreaks( false )
    , m_showLineHighlight( true )
    , m_showLineNumbers( true )
    , m_showSpaces( false )
{
    m_fontNormal            = QFont( "Monospace", 10 );
    m_fontColumn            = QFont( "Monospace", 10 );
    m_appPath               = QCoreApplication::applicationDirPath();
    m_priorPath             = QDir::homePath();
    m_advancedFindFolder    = m_appPath;

    m_edtLastDeletedChar = "";
    m_edtLastDeletedLine = "";
    m_edtLastDeletedWord = "";

    generateDefaultThemes();

    // TODO:: find out why this list has to be full
    //
    for ( int k = 0; k < DiamondLimits::PRESET_FOLDERS_MAX; k++ )
    {
        m_preFolderList.append( "" );
    }

    m_isComplete = true;
}

Settings::~Settings()
{
    deleteAllThemes();
    deleteAllMacros();
}

void Settings::deleteAllThemes()
{
    for ( Themes *ptr : m_themes )
    {
        delete ptr;
    }

    m_themes.clear();
}

void Settings::deleteAllMacros()
{
    QMapIterator<QString, QList<MacroStruct *>> iter( m_macros );

    while ( iter.hasNext() )
    {
        iter.next();

        for ( MacroStruct *ptr : iter.value() )
        {
            delete ptr;
        }
    }

    m_macros.clear();
}

Settings &Settings::operator =( const Settings &other )
{
    if ( this != &other )
    {
        if ( m_preFolderList.count() > 0 )
        {
            m_preFolderList.clear();
        }

        if ( m_themes.count() > 0 )
        {
            deleteAllThemes();
        }

        m_activeTheme               = other.m_activeTheme;
        m_advancedFCase             = other.m_advancedFCase;
        m_advancedFSearchFolders    = other.m_advancedFSearchFolders;
        m_advancedFWholeWords       = other.m_advancedFWholeWords;
        m_advancedFindFileType      = other.m_advancedFindFileType;
        m_advancedFindFolder        = other.m_advancedFindFolder;
        m_advancedFindText          = other.m_advancedFindText;
        m_appPath                   = other.m_appPath;
        m_configFileName            = other.m_configFileName;
        m_findCase                  = other.m_findCase;
        m_findDirection             = other.m_findDirection;
        m_findFlags                 = other.m_findFlags;
        m_findList                  = other.m_findList;
        m_findText                  = other.m_findText;
        m_findWholeWords            = other.m_findWholeWords;
        m_flagNoAutoLoad            = other.m_flagNoAutoLoad;
        m_flagNoSaveConfig          = other.m_flagNoSaveConfig;
        m_fontColumn                = other.m_fontColumn;
        m_fontNormal                = other.m_fontNormal;
        m_isColumnMode              = other.m_isColumnMode;
        m_isComplete                = other.m_isComplete;
        m_isSpellCheck              = other.m_isSpellCheck;
        m_isWordWrap                = other.m_isWordWrap;
        m_lastPosition              = other.m_lastPosition;
        m_lastSize                  = other.m_lastSize;
        m_lastActiveFile            = other.m_lastActiveFile;
        m_lastActiveRow             = other.m_lastActiveRow;
        m_lastActiveColumn          = other.m_lastActiveColumn;
        m_macros                    = other.m_macros;
        m_openedFiles               = other.m_openedFiles;
        m_openedModified            = other.m_openedModified;
        m_options                   = other.m_options;
        m_preFolderList             = other.m_preFolderList;
        m_printSettings             = other.m_printSettings;
        m_priorPath                 = other.m_priorPath;
        m_rFolderList               = other.m_rFolderList;
        m_recentFilesList           = other.m_recentFilesList;
        m_replaceList               = other.m_replaceList;
        m_replaceText               = other.m_replaceText;
        m_showBreaks                = other.m_showBreaks;
        m_showLineHighlight         = other.m_showLineHighlight;
        m_showLineNumbers           = other.m_showLineNumbers;
        m_showSpaces                = other.m_showSpaces;

        for ( Themes *ptr : other.m_themes )
        {
            if ( nullptr != ptr )
            {
                m_themes[ ptr->name()] = new Themes( *ptr );
            }
        }

        m_edtLastDeletedWord        = other.m_edtLastDeletedWord;
        m_edtLastDeletedLine        = other.m_edtLastDeletedLine;
        m_edtLastDeletedChar        = other.m_edtLastDeletedChar;

    }

    return *this;
}


void Settings::copyTheme( QString name, QString dest )
{
    if ( themeNameExists( name ) )
    {
        if ( !themeNameExists( dest ) )
        {
            m_themes[dest] = new Themes( *m_themes[name] );
            m_themes[dest]->set_name( dest );
        }
        else
        {
            csError( tr( "Copy Theme" ), tr( "Destination name " ) + dest + tr( "  already exists" ) );
        }
    }
    else
    {
        csError( tr( "Copy Theme" ), tr( "Source name " ) + name + tr( " does not exist" ) );
    }
}

bool Settings::themeNameExists( QString name )
{
    return m_themes.contains( name );
}

void Settings::deleteTheme( QString name )
{
    if ( themeNameExists( name ) )
    {
        delete m_themes[name];
        m_themes.remove( name );

        if ( m_activeTheme.compare( name ) == 0 )
        {
            m_activeTheme = "CLASSIC";  // TODO:: might want to just use first theme in list.
        }
    }
    else
    {
        csError( tr( "Delete Theme" ), tr( "Theme name " ) + name + tr( " does not exist" ) );
    }
}

void Settings::add_theme( Themes *theme )
{
    // Leaving name collision test in dialog for now.
    // If it is a duplicate name at this point it will
    // replace the existing theme.
    if ( theme != nullptr )
    {
        m_themes[theme->name()] = theme;
        m_activeTheme = theme->name();
    }
}

bool Settings::load()
{
    bool ok = true;


    if ( m_configFileName.isEmpty() )
    {
        return false;
    }



    if ( ! QFile::exists( m_configFileName ) )
    {
        save();  // TODO:: save should return bool and set ok
        //        just in case we cannot write to disk for some reason
        return true;
    }

    if ( ok )
    {

        // need to get rid of the default themes
        // constructor created.
        //
        deleteAllThemes();
        deleteAllMacros();

        // get existing json data
        QByteArray data = json_ReadFile();

        QJsonDocument doc = QJsonDocument::fromJson( data );

        QJsonObject object = doc.object();
        QJsonValue value;
        QJsonArray list;

        int cnt;

        //
        value = object.value( "pos-x" );
        int x = value.toDouble();

        value = object.value( "pos-y" );
        int y = value.toDouble();

        m_lastPosition = QPoint( x, y );
        Move( m_lastPosition );

        //
        value = object.value( "size-width" );
        int width = value.toDouble();

        value = object.value( "size-height" );
        int height = value.toDouble();

        m_lastSize = QSize( width, height );
        Resize( m_lastSize );

        m_lastActiveFile = object.value( "lastActiveFile" ).toString();
        m_lastActiveRow  = object.value( "lastActiveRow" ).toInt();
        m_lastActiveColumn = object.value( "lastActiveColumn" ).toInt();

        // options
        m_options.set_rewrapColumn( object.value( "rewrapColumn" ).toInt() );
        m_options.set_tabSpacing( object.value( "tabSpacing" ).toInt() );
        m_options.set_useSpaces( object.value( "useSpaces" ).toBool() );
        m_options.set_removeSpaces( object.value( "removeSpace" ).toBool() );
        m_options.set_autoLoad( object.value( "autoLoad" ).toBool() );
        m_options.set_mainDictionary( object.value( "dictMain" ).toString() );
        m_options.set_userDictionary( object.value( "dictUser" ).toString() );
        m_options.set_formatDate( object.value( "formatDate" ).toString() );
        m_options.set_formatTime( object.value( "formatTime" ).toString() );
        m_options.set_syntaxPath( object.value( "pathSyntax" ).toString() );

        // settings
        m_showLineHighlight   = object.value( "showLineHighlight" ).toBool();
        m_showLineNumbers     = object.value( "showLineNumbers" ).toBool();
        m_isWordWrap          = object.value( "word-wrap" ).toBool();
        m_showSpaces          = object.value( "showSpaces" ).toBool();
        m_showBreaks          = object.value( "showBreaks" ).toBool();
        m_isColumnMode        = object.value( "column-mode" ).toBool();
        m_isSpellCheck        = object.value( "spellcheck" ).toBool();
        m_priorPath           = object.value( "pathPrior" ).toString();
        // screen fonts
        m_fontNormal          = fontFromString( object.value( "font-normal" ).toString() );
        m_fontColumn          = fontFromString( object.value( "font-column" ).toString() );


        // printer options
        m_printSettings.set_lineNumbers( object.value( "prt-lineNumbers" ).toBool() );
        m_printSettings.set_printHeader( object.value( "prt-pritnHeader" ).toBool() );
        m_printSettings.set_printFooter( object.value( "prt-printFooter" ).toBool() );
        m_printSettings.set_headerLeft( object.value( "prt-headerLeft" ).toString() );
        m_printSettings.set_headerCenter( object.value( "prt-headerCenter" ).toString() );
        m_printSettings.set_headerRight( object.value( "prt-headerRight" ).toString() );
        m_printSettings.set_headerLine2( object.value( "prt-headerLine2" ).toString() );
        m_printSettings.set_footerLeft( object.value( "prt-footerLeft" ).toString() );
        m_printSettings.set_footerCenter( object.value( "prt-footerCenter" ).toString() );
        m_printSettings.set_footerRight( object.value( "prt-footerRight" ).toString() );
        m_printSettings.set_footerLine2( object.value( "prt-footerLine2" ).toString() );
        m_printSettings.set_marginTop( object.value( "prt-marginTop" ).toDouble() );
        m_printSettings.set_marginBottom( object.value( "prt-marginBottom" ).toDouble() );
        m_printSettings.set_marginLeft( object.value( "prt-marginLeft" ).toDouble() );
        m_printSettings.set_marginRight( object.value( "prt-marginRight" ).toDouble() );
        m_printSettings.set_headerGap( object.value( "prt-hdrGap" ).toDouble() );
        m_printSettings.set_fontHeader( fontFromString( object.value( "prt-fontHeader" ).toString() ) );
        m_printSettings.set_fontFooter( fontFromString( object.value( "prt-fontFooter" ).toString() ) );
        m_printSettings.set_fontText( fontFromString( object.value( "prt-fontText" ).toString() ) );

        // standard keys
        m_options.keys().set_open( object.value( "key-open" ).toString() );
        m_options.keys().set_close( object.value( "key-close" ).toString() );
        m_options.keys().set_save( object.value( "key-save" ).toString() );
        m_options.keys().set_saveAs( object.value( "key-saveAs" ).toString() );
        m_options.keys().set_print( object.value( "key-print" ).toString() );
        m_options.keys().set_undo( object.value( "key-undo" ).toString() );
        m_options.keys().set_redo( object.value( "key-redo" ).toString() );
        m_options.keys().set_cut( object.value( "key-cut" ).toString() );
        m_options.keys().set_copy( object.value( "key-copy" ).toString() );
        m_options.keys().set_paste( object.value( "key-paste" ).toString() );
        m_options.keys().set_selectAll( object.value( "key-selectAll" ).toString() );
        m_options.keys().set_find( object.value( "key-find" ).toString() );
        m_options.keys().set_replace( object.value( "key-replace" ).toString() );
        m_options.keys().set_findNext( object.value( "key-findNext" ).toString() );
        m_options.keys().set_findPrev( object.value( "key-findPrev" ).toString() );
        m_options.keys().set_goTop( object.value( "key-goTop" ).toString() );
        m_options.keys().set_goBottom( object.value( "key-goBottom" ).toString() );
        m_options.keys().set_newTab( object.value( "key-newTab" ).toString() );

        // user keys
        m_options.keys().set_printPreview( object.value( "key-printPreview" ).toString() );
        m_options.keys().set_reload( object.value( "key-reload" ).toString() );
        m_options.keys().set_selectLine( object.value( "key-selectLine" ).toString() );
        m_options.keys().set_selectWord( object.value( "key-selectWord" ).toString() );
        m_options.keys().set_selectBlock( object.value( "key-selectBlock" ).toString() );
        m_options.keys().set_upper( object.value( "key-upper" ).toString() );
        m_options.keys().set_lower( object.value( "key-lower" ).toString() );
        m_options.keys().set_indentIncrement( object.value( "key-indentIncr" ).toString() );
        m_options.keys().set_indentDecrement( object.value( "key-indentDecr" ).toString() );
        m_options.keys().set_deleteLine( object.value( "key-deleteLine" ).toString() );
        m_options.keys().set_deleteToEOL( object.value( "key-deleteToEOL" ).toString() );
        m_options.keys().set_deleteThroughEOL( object.value( "key_deleteThroughEOL" ).toString() );
        m_options.keys().set_columnMode( object.value( "key-columnMode" ).toString() );
        m_options.keys().set_gotoLine( object.value( "key-goLine" ).toString() );
        m_options.keys().set_showSpaces( object.value( "key-showShow" ).toString() );
        m_options.keys().set_showBreaks( object.value( "key-showBreaks" ).toString() );
        m_options.keys().set_macroPlay( object.value( "key-macroPlay" ).toString() );
        m_options.keys().set_spellCheck( object.value( "key-spellCheck" ).toString() );
        m_options.keys().set_copyBuffer( object.value( "key-copyBuffer" ).toString() );

        // adv find
        m_advancedFindText       = object.value( "advFile-text" ).toString();
        m_advancedFindFileType   = object.value( "advFile-filetype" ).toString();
        m_advancedFindFolder     = object.value( "advFile-folder" ).toString();
        m_advancedFSearchFolders = object.value( "advFile-searchFolders" ).toBool();

        // find list
        list = object.value( "find-list" ).toArray();
        cnt  = list.count();

        for ( int k = 0; k < cnt; k++ )
        {
            m_findList.append( list.at( k ).toString() );
        }

        // replace list
        list = object.value( "replace-list" ).toArray();
        cnt  = list.count();

        for ( int k = 0; k < cnt; k++ )
        {
            m_replaceList.append( list.at( k ).toString() );
        }


        // preset folders
        m_preFolderList.clear();
        list = object.value( "preset-folders" ).toArray();
        cnt = list.count();

        for ( int k = 0; k < cnt; k++ )
        {
            m_preFolderList.append( list.at( k ).toString() );
        }

        // silly way to pad the list
        // TODO:: find out why this list _has_ to be padded
        //        we literally crash if it is not padded
        //
        for ( int k = cnt; k < DiamondLimits::PRESET_FOLDERS_MAX; k++ )
        {
            m_preFolderList.append( "" );
        }

        // recent folders
        list = object.value( "recent-folders" ).toArray();
        cnt = list.count();

        for ( int k = 0; k < cnt; k++ )
        {
            m_rFolderList.append( list.at( k ).toString() );
        }

        // recent files
        list = object.value( "recent-files" ).toArray();
        cnt = list.count();

        for ( int k = 0; k < cnt; k++ )
        {
            m_recentFilesList.append( list.at( k ).toString() );
        }

        // opened files
        list = object.value( "opened-files" ).toArray();
        cnt  = list.count();

        for ( int k = 0; k < cnt; k++ )
        {
            QString fname = list.at( k ).toString();

            if ( ! fname.isEmpty() )
            {
                m_openedFiles.append( fname );
                m_openedModified.append( false );
            }
        }

        // user could have the old config file json
        //
        if ( !object.contains( "active-theme" ) )
        {
            importOldConfig( object );
            return ok;
        }

        m_options.set_backupDirectory( object.value( "backupDirectory" ).toString() );
        m_options.set_astyleOnSave( object.value( "astyleOnSave" ).toBool() );
        m_options.set_maxVersions( object.value( "maxVersions" ).toInt() );
        m_options.set_makeBackups( object.value( "enableBackups" ).toBool() );
        m_options.keys().set_edtGotoLine( object.value( "key-edt-gotoLine" ).toString() );
        m_options.keys().set_edtCopy( object.value( "key-edt-copy" ).toString() );
        m_options.keys().set_edtInsertFile( object.value( "key-edt-insertFile" ).toString() );
        m_options.keys().set_edtUpperCase( object.value( "key-edt-upperCase" ).toString() );
        m_options.keys().set_edtLowerCase( object.value( "key-edt-lowerCase" ).toString() );
        m_options.keys().set_edtSplitHorizontal( object.value( "key-edt-splitHorizontal" ).toString() );
        m_options.keys().set_edtSplitVertical( object.value( "key-edt-splitVertical" ).toString() );
        m_options.keys().set_edtSaveFile( object.value( "key-edt-saveFile" ).toString() );
        m_options.keys().set_edtAstyle( object.value( "key-edt-astyle" ).toString() );
        m_options.keys().set_edtWordCtrlMeta( object.value( "edt-word-Ctrl-Meta" ).toBool() );
        m_options.keys().set_edtWordAltOption( object.value( "edt-word-Alt-Option" ).toBool() );
        m_options.keys().set_edtEnabled( object.value( "edt-enabled" ).toBool() );
        m_options.keys().set_f12AsBackspace( object.value( "edt-f12-as-backspace" ).toBool() );

        m_options.set_preloadClipper( object.value( "preload-clipper" ).toBool() );
        m_options.set_preloadCmake( object.value( "preload-cmake" ).toBool() );
        m_options.set_preloadCpp( object.value( "preload-cpp" ).toBool() );
        m_options.set_preloadCss( object.value( "preload-css" ).toBool() );
        m_options.set_preloadDoxy( object.value( "preload-doxy" ).toBool() );
        m_options.set_preloadErrLog( object.value( "preload-errlog" ).toBool() );
        m_options.set_preloadHtml( object.value( "preload-html" ).toBool() );
        m_options.set_preloadJava( object.value( "preload-java" ).toBool() );
        m_options.set_preloadJs( object.value( "preload-js" ).toBool() );
        m_options.set_preloadJson( object.value( "preload-json" ).toBool() );
        m_options.set_preloadMake( object.value( "preload-make" ).toBool() );
        m_options.set_preloadNone( object.value( "preload-none" ).toBool() );
        m_options.set_preloadNSI( object.value( "preload-nsi" ).toBool() );
        m_options.set_preloadPhp( object.value( "preload-php" ).toBool() );
        m_options.set_preloadPl( object.value( "preload-pl" ).toBool() );
        m_options.set_preloadPy( object.value( "preload-py" ).toBool() );
        m_options.set_preloadSh( object.value( "preload-sh" ).toBool() );
        m_options.set_preloadTxt( object.value( "preload-txt" ).toBool() );
        m_options.set_preloadXml( object.value( "preload-xml" ).toBool() );

        // macros
        list = object.value( "macros" ).toArray();
        cnt  = list.count();

        for ( int k = 0; k < cnt; k++ )
        {
            QList<MacroStruct *> keyList;
            QJsonObject obj = list.at( k ).toObject();
            QString macroName = obj.value( "macro-name" ).toString();

            // don't load unnamed macro
            if ( macroName.length() > 0 )
            {
                QJsonArray keys = obj.value( "macro-keys" ).toArray();

                int keyCount = keys.count();

                // Yes, macro-keys is the QList but we cheated and
                // stored the fields of macroStruct as a list/array.
                //
                for ( int mk=0; mk < keyCount; mk++ )
                {
                    QJsonArray lst = keys.at( mk ).toArray();

                    if ( lst.count() > 2 )
                    {
                        MacroStruct *macroPtr = new MacroStruct;

                        macroPtr->m_key      = lst.at( 0 ).toInt();
                        macroPtr->m_modifier = lst.at( 1 ).toInt();
                        macroPtr->m_text     = lst.at( 2 ).toString();

                        keyList.append( macroPtr );
                    }
                }

                m_macros[macroName] = keyList;
            }
        }

        m_activeTheme = object.value( "active-theme" ).toString();
        QJsonArray themesArray = object.value( "themes" ).toArray();

        int themeCount = themesArray.count();

        for ( int x=0; x < themeCount; x++ )
        {
            QJsonObject theme = themesArray[x].toObject();

            QString themeName = theme.value( "theme-name" ).toString();
            m_themes[themeName] = new Themes( themeName );

            m_themes[themeName]->set_colorText( colorFromValueString( theme.value( "theme-color-text" ).toString() ) );
            m_themes[themeName]->set_colorBack( colorFromValueString( theme.value( "theme-color-back" ).toString() ) );
            m_themes[themeName]->set_gutterText( colorFromValueString( theme.value( "theme-color-gutterText" ).toString() ) );
            m_themes[themeName]->set_gutterBack( colorFromValueString( theme.value( "theme-color-gutterBack" ).toString() ) );
            m_themes[themeName]->set_currentLineBack( colorFromValueString( theme.value( "theme-color-currentLineBack" ).toString() ) );
            m_themes[themeName]->set_name( theme.value( "theme-name" ).toString() );
            m_themes[themeName]->set_protected( theme.value( "theme-protected" ).toBool() );

            list = theme.value( "theme-syntax-key" ).toArray();

            TextAttributes attr;

            if ( list.count() > 2 )
            {
                attr.set_weight( list.at( 0 ).toDouble() );
                attr.set_italic( list.at( 1 ).toBool() );
                attr.set_color( colorFromValueString( list.at( 2 ).toString() ) );
                m_themes[themeName]->set_syntaxKey( attr );
            }

            list = theme.value( "theme-syntax-type" ).toArray();

            if ( list.count() > 2 )
            {
                attr.set_weight( list.at( 0 ).toDouble() );
                attr.set_italic( list.at( 1 ).toBool() );
                attr.set_color( colorFromValueString( list.at( 2 ).toString() ) );
                m_themes[themeName]->set_syntaxType( attr );
            }

            list = theme.value( "theme-syntax-class" ).toArray();

            if ( list.count() > 2 )
            {
                attr.set_weight( list.at( 0 ).toDouble() );
                attr.set_italic( list.at( 1 ).toBool() );
                attr.set_color( colorFromValueString( list.at( 2 ).toString() ) );
                m_themes[themeName]->set_syntaxClass( attr );
            }

            list = theme.value( "theme-syntax-func" ).toArray();

            if ( list.count() > 2 )
            {
                attr.set_weight( list.at( 0 ).toDouble() );
                attr.set_italic( list.at( 1 ).toBool() );
                attr.set_color( colorFromValueString( list.at( 2 ).toString() ) );
                m_themes[themeName]->set_syntaxFunc( attr );
            }

            list = theme.value( "theme-syntax-quote" ).toArray();

            if ( list.count() > 2 )
            {
                attr.set_weight( list.at( 0 ).toDouble() );
                attr.set_italic( list.at( 1 ).toBool() );
                attr.set_color( colorFromValueString( list.at( 2 ).toString() ) );
                m_themes[themeName]->set_syntaxQuote( attr );
            }

            list = theme.value( "theme-syntax-comment" ).toArray();

            if ( list.count() > 2 )
            {
                attr.set_weight( list.at( 0 ).toDouble() );
                attr.set_italic( list.at( 1 ).toBool() );
                attr.set_color( colorFromValueString( list.at( 2 ).toString() ) );
                m_themes[themeName]->set_syntaxComment( attr );
            }

            list = theme.value( "theme-syntax-mline" ).toArray();

            if ( list.count() > 2 )
            {
                attr.set_weight( list.at( 0 ).toDouble() );
                attr.set_italic( list.at( 1 ).toBool() );
                attr.set_color( colorFromValueString( list.at( 2 ).toString() ) );
                m_themes[themeName]->set_syntaxMLine( attr );
            }

            list = theme.value( "theme-syntax-constant" ).toArray();

            if ( list.count() > 2 )
            {
                attr.set_weight( list.at( 0 ).toDouble() );
                attr.set_italic( list.at( 1 ).toBool() );
                attr.set_color( colorFromValueString( list.at( 2 ).toString() ) );
                m_themes[themeName]->set_syntaxConstant( attr );
            }
        }

        m_edtLastDeletedWord = object.value( "edt-lastDeletedWord" ).toString();
        m_edtLastDeletedLine = object.value( "edt-lastDeletedLine" ).toString();
        m_edtLastDeletedChar = object.value( "edt-lastDeletedChar" ).toString();
    }

    return ok;
}


QByteArray Settings::json_ReadFile()
{
    QByteArray data;

    QFile file( m_configFileName );

    if ( ! file.open( QFile::ReadWrite | QFile::Text ) )
    {
        const QString msg = tr( "Unable to open Configuration File: " ) +  m_configFileName + " : " + file.errorString();
        csError( tr( "Read Json" ), msg );
        return data;
    }

    file.seek( 0 );
    data = file.readAll();
    file.close();

    return data;
}


void Settings::createThemeArray( QJsonObject &object )
{
    QJsonArray list;
    QJsonArray themeArray;

    // dummy
    list.append( 0 );
    list.append( true );
    list.append( 0 );

    for ( Themes *theme : m_themes )
    {
        QJsonObject arrayElement;

        arrayElement.insert( "theme-name", theme->name() );
        arrayElement.insert( "theme-protected", theme->isProtected() );
        arrayElement.insert( "theme-color-text", getRGBString( theme->colorText() ) );
        arrayElement.insert( "theme-color-back", getRGBString( theme->colorBack() ) );
        arrayElement.insert( "theme-color-gutterText", getRGBString( theme->gutterText() ) );
        arrayElement.insert( "theme-color-gutterBack", getRGBString( theme->gutterBack() ) );
        arrayElement.insert( "theme-color-currentLineBack", getRGBString( theme->currentLineBack() ) );

        list.replace( 0, theme->syntaxKey().weight() );
        list.replace( 1, theme->syntaxKey().italic() );
        list.replace( 2, getRGBString( theme->syntaxKey().color() ) );
        arrayElement.insert( "theme-syntax-key", list );

        list.replace( 0, theme->syntaxType().weight() );
        list.replace( 1, theme->syntaxType().italic() );
        list.replace( 2, getRGBString( theme->syntaxType().color() ) );
        arrayElement.insert( "theme-syntax-type", list );

        list.replace( 0, theme->syntaxClass().weight() );
        list.replace( 1, theme->syntaxClass().italic() );
        list.replace( 2, getRGBString( theme->syntaxClass().color() ) );
        arrayElement.insert( "theme-syntax-class", list );

        list.replace( 0, theme->syntaxFunc().weight() );
        list.replace( 1, theme->syntaxFunc().italic() );
        list.replace( 2, getRGBString( theme->syntaxFunc().color() ) );
        arrayElement.insert( "theme-syntax-func", list );

        list.replace( 0, theme->syntaxQuote().weight() );
        list.replace( 1, theme->syntaxQuote().italic() );
        list.replace( 2, getRGBString( theme->syntaxQuote().color() ) );
        arrayElement.insert( "theme-syntax-quote", list );

        list.replace( 0, theme->syntaxComment().weight() );
        list.replace( 1, theme->syntaxComment().italic() );
        list.replace( 2, getRGBString( theme->syntaxComment().color() ) );
        arrayElement.insert( "theme-syntax-comment", list );

        list.replace( 0, theme->syntaxMLine().weight() );
        list.replace( 1, theme->syntaxMLine().italic() );
        list.replace( 2, getRGBString( theme->syntaxMLine().color() ) );
        arrayElement.insert( "theme-syntax-mline", list );

        list.replace( 0, theme->syntaxConstant().weight() );
        list.replace( 1, theme->syntaxConstant().italic() );
        list.replace( 2, getRGBString( theme->syntaxConstant().color() ) );
        arrayElement.insert( "theme-syntax-constant", list );

        themeArray.append( arrayElement );
    }

    object.insert( "themes", themeArray );

}

void Settings::createMacroArray( QJsonObject &object )
{
    QJsonArray list;
    QJsonArray macroArray;

    // dummy
    list.append( 0 );
    list.append( 0 );
    list.append( 0 );

    QMapIterator<QString, QList<MacroStruct *>> iter( m_macros );

    while ( iter.hasNext() )
    {
        iter.next();

        QJsonObject arrayElement;
        QJsonArray keyList;

        // don't save unnamed macro
        //
        if ( iter.key().length() > 0 )
        {
            arrayElement.insert( "macro-name", iter.key() );


            for ( MacroStruct *ptr : iter.value() )
            {
                list.replace( 0, ptr->m_key );
                list.replace( 1, ptr->m_modifier );
                list.replace( 2, ptr->m_text );

                keyList.append( list );
            }

            arrayElement.insert( "macro-keys", keyList );

            macroArray.append( arrayElement );
        }
    }

    object.insert( "macros", macroArray );
}


void Settings::save()
{
    if ( m_flagNoSaveConfig )
    {
        return;
    }


    QJsonObject object;

    object.insert( "advFile-text",          m_advancedFindText );
    object.insert( "advFile-filetype",      m_advancedFindFileType );
    object.insert( "advFile-folder",        m_advancedFindFolder );
    object.insert( "advFile-searchFolders", m_advancedFSearchFolders );
    object.insert( "autoLoad",              m_options.autoLoad() );
    object.insert( "pos-x",                 m_lastPosition.x()  );
    object.insert( "pos-y",                 m_lastPosition.y()  );
    object.insert( "size-width",            m_lastSize.width()  );
    object.insert( "size-height",           m_lastSize.height() );
    object.insert( "pos-x",                 m_lastPosition.x()  );
    object.insert( "pos-y",                 m_lastPosition.y()  );
    object.insert( "size-width",            m_lastSize.width()  );
    object.insert( "size-height",           m_lastSize.height() );
    object.insert( "lastActiveFile",        m_lastActiveFile );
    object.insert( "lastActiveRow",         m_lastActiveRow );
    object.insert( "lastActiveColumn",      m_lastActiveColumn );
    object.insert( "about-url",             m_options.aboutUrl() );

    // opened files
    QJsonArray temp = QJsonArray::fromStringList( m_openedFiles );
    object.insert( "opened-files", temp );

    object.insert( "active-theme", m_activeTheme );
    createThemeArray( object );

    createMacroArray( object );

    object.insert( "preload-clipper",   m_options.preloadClipper() );
    object.insert( "preload-cmake",     m_options.preloadCmake() );
    object.insert( "preload-cpp",       m_options.preloadCpp() );
    object.insert( "preload-css",       m_options.preloadCss() );
    object.insert( "preload-doxy",      m_options.preloadDoxy() );
    object.insert( "preload-errlog",    m_options.preloadErrLog() );
    object.insert( "preload-html",      m_options.preloadHtml() );
    object.insert( "preload-java",      m_options.preloadJava() );
    object.insert( "preload-js",        m_options.preloadJs() );
    object.insert( "preload-json",      m_options.preloadJson() );
    object.insert( "preload-make",      m_options.preloadMake() );
    object.insert( "preload-none",      m_options.preloadNone() );
    object.insert( "preload-nsi",       m_options.preloadNSI() );
    object.insert( "preload-php",       m_options.preloadPhp() );
    object.insert( "preload-pl",        m_options.preloadPl() );
    object.insert( "preload-py",        m_options.preloadPy() );
    object.insert( "preload-sh",        m_options.preloadSh() );
    object.insert( "preload-txt",       m_options.preloadTxt() );
    object.insert( "preload-xml",       m_options.preloadXml() );
    object.insert( "backupDirectory",   m_options.backupDirectory() );
    object.insert( "maxVersions",       m_options.maxVersions() );
    object.insert( "astyleOnSave",      m_options.astyleOnSave() );
    object.insert( "enableBackups",     m_options.makeBackups() );


    object.insert( "column-mode",   m_isColumnMode );
    object.insert( "dictMain",      m_options.mainDictionary() );
    object.insert( "dictUser",      m_options.userDictionary() );

    temp = QJsonArray::fromStringList( m_findList );
    object.insert( "find-list", temp );

    temp = QJsonArray::fromStringList( m_findList );
    object.insert( "find-list", temp );

    temp = QJsonArray::fromStringList( m_replaceList );
    object.insert( "replace-list", temp );

    QString tempStr = m_fontNormal.toString();
    object.insert( "font-normal", tempStr );

    tempStr = m_fontColumn.toString();
    object.insert( "font-column", tempStr );

    object.insert( "formatDate", m_options.formatDate() );
    object.insert( "formatTime", m_options.formatTime() );


    // keys
    object.insert( "key-close",             m_options.keys().close() );
    object.insert( "key-columnMode",        m_options.keys().columnMode() );
    object.insert( "key-copy",              m_options.keys().copy() );
    object.insert( "key-copyBuffer",        m_options.keys().copyBuffer() );
    object.insert( "key-cut",               m_options.keys().cut() );
    object.insert( "key-deleteLine",        m_options.keys().deleteLine() );
    object.insert( "key-deleteThroughEOL",  m_options.keys().deleteThroughEOL() );
    object.insert( "key-deleteToEOL",       m_options.keys().deleteToEOL() );
    object.insert( "key-find",              m_options.keys().find() );
    object.insert( "key-findNext",          m_options.keys().findNext() );
    object.insert( "key-findPrev",          m_options.keys().findPrev() );
    object.insert( "key-goBottom",          m_options.keys().goBottom() );
    object.insert( "key-goTop",             m_options.keys().goTop() );
    object.insert( "key-gotoLine",          m_options.keys().gotoLine() );
    object.insert( "key-indentDecrement",   m_options.keys().indentDecrement() );
    object.insert( "key-indentIncrement",   m_options.keys().indentIncrement() );
    object.insert( "key-lower",             m_options.keys().lower() );
    object.insert( "key-macroPlay",         m_options.keys().macroPlay() );
    object.insert( "key-newTab",            m_options.keys().newTab() );
    object.insert( "key-open",              m_options.keys().open() );
    object.insert( "key-paste",             m_options.keys().paste() );
    object.insert( "key-print",             m_options.keys().print() );
    object.insert( "key-printPreview",      m_options.keys().printPreview() );
    object.insert( "key-redo",              m_options.keys().redo() );
    object.insert( "key-reload",            m_options.keys().reload() );
    object.insert( "key-replace",           m_options.keys().replace() );
    object.insert( "key-save",              m_options.keys().save() );
    object.insert( "key-saveAs",            m_options.keys().saveAs() );
    object.insert( "key-selectAll",         m_options.keys().selectAll() );
    object.insert( "key-selectBlock",       m_options.keys().selectBlock() );
    object.insert( "key-selectLine",        m_options.keys().selectLine() );
    object.insert( "key-selectWord",        m_options.keys().selectWord() );
    object.insert( "key-showBreaks",        m_options.keys().showBreaks() );
    object.insert( "key-showSpaces",        m_options.keys().showSpaces() );
    object.insert( "key-spellCheck",        m_options.keys().spellCheck() );
    object.insert( "key-undo",              m_options.keys().undo() );
    object.insert( "key-upper",             m_options.keys().upper() );

    //EDT keys
    object.insert( "edt-enabled",           m_options.keys().edtEnabled() );
    object.insert( "key-edt-gotoLine",      m_options.keys().edtGotoLine() );
    object.insert( "key-edt-copy",          m_options.keys().edtCopy() );
    object.insert( "key-edt-insertFile",    m_options.keys().edtInsertFile() );
    object.insert( "key-edt-upperCase",     m_options.keys().edtUpperCase() );
    object.insert( "key-edt-lowerCase",     m_options.keys().edtLowerCase() );
    object.insert( "key-edt-splitHorizontal",   m_options.keys().edtSplitHorizontal() );
    object.insert( "key-edt-splitVertical",     m_options.keys().edtSplitVertical() );
    object.insert( "key-edt-saveFile",      m_options.keys().edtSaveFile() );
    object.insert( "key-edt-astyle",        m_options.keys().edtAstyle() );
    object.insert( "edt-word-Ctrl-Meta",    m_options.keys().edtWordCtrlMeta() );
    object.insert( "edt-word-Alt-Option",   m_options.keys().edtWordAltOption() );
    object.insert( "edt-f12-as-backspace",  m_options.keys().f12AsBackspace() );

    object.insert( "pathPrior", m_priorPath );
    object.insert( "pathSyntax", syntaxPath() );

    temp = QJsonArray::fromStringList( m_preFolderList );
    object.insert( "preset-folders", temp );

    temp = QJsonArray::fromStringList( m_rFolderList );
    object.insert( "recent-folders", temp );

    temp = QJsonArray::fromStringList( m_recentFilesList );
    object.insert( "recent-files", temp );

    object.insert( "removeSpace", m_options.removeSpaces() );
    object.insert( "rewrapColumn", m_options.rewrapColumn() );
    object.insert( "showLineHighlight", m_showLineHighlight );
    object.insert( "showLineNumbers", m_showLineNumbers );
    object.insert( "showSpaces", m_showSpaces );
    object.insert( "showBreaks", m_showBreaks );
    object.insert( "spellcheck", m_isSpellCheck );
    object.insert( "tabSpacing", m_options.tabSpacing() );
    object.insert( "useSpaces", m_options.useSpaces() );
    object.insert( "word-wrap", m_isWordWrap );


    object.insert( "edt-lastDeletedWord", m_edtLastDeletedWord );

    if ( m_edtLastDeletedLine.length() > 300 )
    {
        QByteArray hex = m_edtLastDeletedLine.toUtf8().toHex();
        qWarning() << "edt-lastDeletedLine > 300 characters: " << hex << "\n";
    }

    object.insert( "edt-lastDeletedLine", m_edtLastDeletedLine );
    object.insert( "edt-lastDeletedChar", m_edtLastDeletedChar );

    QJsonDocument doc( object );
    QByteArray data = doc.toJson();

    QString path = pathName( m_configFileName );
    QDir directory( path );

    if ( ! directory.exists() )
    {
        directory.mkpath( path );
    }

    /* Comment by Roland Hughes:
     *
     * The previous codebase took great pains to only change the handfull
     * of fields actually modified via the various dialogs. If this was
     * being written for a system powered by a 9 volt battery I would probably
     * keep that approach. People don't choose JSON for efficiency. It gets
     * chosen because you can change the data with a standard text editor.
     * I come from an OpenVMS background. There we have file versioning. The
     * operating system automatically creates a new version, counting from 1 - 32767.
     * We won't do anything quite that extreme. Seven is usually a lucky number for
     * some people so we will first limit the backup copies; rename the
     * current file to be a backup; then write a shiny new file containing all
     * of the current settings. It will be much more memory manipulation, but
     * should be dramatically less disk I/O because now we don't read the
     * whole file in before each update.
     *
     * This approach also makes changes to the JSON format much easier to implement.
     * When new things are added they will "just appear" in the file. Most importantly
     * when old things get removed, they will be gone in the next version, not
     * hanging around causing confusion and potential problems.
     */
    trimBackups( path );

    QFile file( m_configFileName );

    if ( ! file.open( QFile::WriteOnly | QFile::Text ) )
    {
        const QString msg = tr( "Unable to save Configuration File: " )
                            +  m_configFileName + " : " + file.errorString();
        csError( tr( "Save Json" ), msg );
        return;
    }

    file.write( data );
    file.close();
}

void Settings::trimBackups( QString path )
{
    QDir dir( path );
    QStringList filters;
    QString configFileNameBase = QFileInfo( m_configFileName ).baseName();
    QString wild = configFileNameBase + ".b*";
    filters << wild;

    if ( !QFile::exists( m_configFileName ) )
    {
        return;
    }

    QStringList backupFiles = dir.entryList( filters, QDir::Files | QDir::Writable, QDir::Name );

    // Looks weird but we are deleting the lowest backup numbers
    // which will be at the beginning of the list.
    bool okFlag = true;

    while ( ( backupFiles.size() >= DiamondLimits::CONFIG_FILES_MAX ) && okFlag )
    {
        QString fName = strippedName( backupFiles.takeFirst() );
        okFlag = dir.remove( fName );

        if ( !okFlag )
        {
            QString msg = QString( "Failed to remove " )+  fName;
            csError( tr( "Purging Backups" ), msg );
        }
    }

    int versionNumber = -1;

    if ( backupFiles.size() > 0 )
    {
        QString lastFile = backupFiles.last();
        QString tmp = suffixName( lastFile ).mid( 1, -1 );
        versionNumber = tmp.toInteger<int>();
    }

    versionNumber++;
    QString namePattern = QFileInfo( m_configFileName ).baseName() + ".b%1";
    QString newName     = namePattern.formatArg( versionNumber, 3, 10, '0' );
    QString oldName     = strippedName( m_configFileName );

    if ( !dir.rename( oldName, newName ) )
    {
        QString msg = QString( "Failed to rename " )
                      + strippedName( m_configFileName )
                      + " to " + newName;
        csError( tr( "Purging Backups" ), msg );
    }

    if ( versionNumber > DiamondLimits::CONFIG_VERSION_MAX )
    {
        versionNumber = 0;
        // refresh the list
        //
        backupFiles = dir.entryList( filters, QDir::Files | QDir::Writable, QDir::Name );

        while ( backupFiles.size() > 0 )
        {
            newName = namePattern.formatArg( versionNumber, 3, 10, '0' );
            // Yes, I know I should check the return value.
            dir.rename( strippedName( backupFiles.takeFirst() ), newName );

            versionNumber++;
        }
    }


}


bool Settings::openedFilesContains( QString name )
{
    return m_openedFiles.contains( name, Qt::CaseInsensitive );
}

int Settings::openedFilesFind( QString name )
{
    return m_openedFiles.indexOf( name );
}

void Settings::openedModifiedReplace( int sub, bool yesNo )
{
    m_openedModified.replace( sub, yesNo );
}

int Settings::findListFind( QString text )
{
    return m_findList.indexOf( text );
}

bool Settings::recentFilesListContains( QString text )
{
    return m_recentFilesList.contains( text );
}

// Create all of the default themes.
//
void Settings::generateDefaultThemes()
{
    deleteAllThemes();

    /* each line a single string
     * the n, n following syntax color is first Bold then Italic
     *
     * name,
     *        text,
     *        background,
     *        gutter-text,
     *        gutter-back,
     *        current-line-back
     *        key-color, n, n,
     *        type-color, n, n,
     *        class-color, n, n,
     *        func-color, n, n,
     *        quote-color, n, n,
     *        comment-color, n, n,
     *        mline-color, n, n,
     *        constant-color, n, n
     */
    QStringList themeData =
    {
        "CLASSIC, #000000, #FFFFFF, #808080, #D0D0D0, #FFFF99, #0000FF, Y, N, #0000FF, N, N, #800080, N, N, #0000FF, N, N, #008000, N, N, #008000, N, N, #008000, N, N, #7F747E, N, N",
        "SOLARIZEDLIGHT, #657B83, #FDF6E3, #657B83, #E0DBCD, #E0DBCD, #709D06, N, N, #B58900, N, N, #268BD2, N, N, #657B83, N, N, #2AA198, N, N, #93A1A1, N, Y,#93A1A1, N, Y, #2AA198, N, N",
        "GRAYGREY, #000000, #C7D1D3, #000000, #D1DED1, #D7DEDE, #98061A,  Y, N, #9C061A, N, N, #0000FF, N, N, #00007F, N, N, #005500, N, N, #434343, N, Y, #434343, N, Y, #9C061A, N, N",
        "JUSTDARKENOUGH, #F4F4F4, #424244, #000000, #848278, #232220, #00C2C2, N, N, #C195B4, N, N, #55AAFF, N, N, #00EBAD, N, N, #FFFF00, N, N, #F59F00, N, Y, #F59F00, N, Y, #FF518C, N, N",
        "TANGOLIGHT, #2E3436, #FFFFFF, #2E3436, #D1E7D3, #D1E7D3, #346604, N, N, #CE5C00, N, N, #234670, N, N, #A40000, N, N, #5C3566, N, N, #5F615C, N, Y, #5F615C, N, Y, #8F5902, N, N",
        "COBALT, #F8F8F8, #09223F, #888888, #111111, #00162A, #FA9E18, N, N, #FFEF79, N, N, #42D915, N,N, #FFDD00, N, N, #42D915, N, N, #008AFF, N, N, #008AFF, N, N, #FF518C, N, N",
        "GreenAndGold, #000000, #F1EEC2, #000000, #6C9359, #D5D39B, #AA0000, N, N, #AA007F, N, N, #0000FF, N, N, #00008D, N, N, #006200, N, N, #7C7C7C, N, N, #7C7C7C, N, N, #8D0000, N, N",
        "DeepLedger, #000000, #8EDAA0, #000000, #4B673E, #8EDAA0, #555500, N, N, #AD1D3C, N, N, #2F5F8F, N, N, #780050, N, N, #005500, N, N, #7C7C7C, N, N, #7C7C7C, N, N, #AF7500, N, N",
        "Ledger, #000000, #C6FAC6, #000000, #4B673E, #C6FAC6, #555500, N, N, #AD1D3C, N, N, #2F5F8F, N, N, #8700CA, N, N, #005500, N, N, #7C7C7C, N, N, #7C7C7C, N, N, #FF518C, N, N",
        "SLATE, #000000, #778899, #000000, #D1DED1, #D1DED1, #98061A, Y, N, #98061A, N, N, #0000FF, N, N, #00007F, N, N, #005500, N, N, #434343, N, N, #434343, N, N, #9C061A, N, N",
        "Snow, #000000, #FFFAFA, #000000, #D1DED1, #D1DED1, #98061A, Y, N, #550000, N, N, #0000FF, N, N, #55557F, N, N, #005500, N, N, #434343, N, N, #434343, N, N, #555500, N, N",
        "Ivory, #000000, #FFFFF0, #000000, #D1DED1, #D1DED1, #98061A, Y, N, #9C061A, N, N, #0000FF, N, N, #00007F, N, N, #005500, N, N, #434343, N, N, #434343, N, N, #9C061A, N, N",
        "Non-Silver, #000000, #C0C0C0, #000000, #D1DED1, #D1DED1, #9C061A, Y, N, #550000, N, N, #0000FF, N, N, #55557F, N, N, #005500, N, N, #434343, N, N, #434343, N, N, #8A0893, N, N",
        "New-Grey, #000000, #B9C3C5, #000000, #D7DEDE, #D7DEDE, #051498, Y, N, #9C061A, N, N, #8400C6, N, N, #D400D4, N, N, #2C7631, N, N, #0000FF, N, N, #0000FF, N, N, #550000, N, N",
        "Funky-1, #EAEAEA, #434300, #000000, #7F7F66, #090807, #FFAA00, Y, N, #FFFF00, N, N, #779DF0, N, N, #FFB5D3, N, N, #399D40, N, N, #AAFFFF, N, Y, #AAFFFF, N, Y, #00AAFF, N, N",
        "Fred, #000000, #FFFFFF, #808080, #D0D0D0, #FFFF99, #0000FF, Y, N, #00007F, N, N, #800080, N, N, #0000B9, N, N, #008000, N, N, #55557F, N, Y, #55557F, N, Y, #AA0000, N, N",
        "YABU, #FFFFFF, #481036, #F0F0F0, #6B6970, #000000, #00AAFF, Y, N, #55FFFF, N, N, #FFFF7F, N, N, #07BDFF, N, N, #00DF00, N, N, #8989CC, N, Y, #8989CC, N, Y, #FFAA00, N, N"
    };

    TextAttributes attr;

    // Note:  This parsing logic ass-u-mes you didn't fat finger anything in the data above.
    //        There is no "user" input here, only programmer error and the programmer 'should'
    //        have at least tested the "happy path."
    //
    deleteAllThemes();

    for ( QString str : themeData )
    {
        QStringList lst = str.split( "," );
        int fld = 0;

        QString themeName = lst[fld++].trimmed();

        m_themes[themeName] = new Themes( themeName, true );

        m_themes[themeName]->set_colorText( QColor( lst[fld++].trimmed() ) );
        m_themes[themeName]->set_colorBack( QColor( lst[fld++].trimmed() ) );
        m_themes[themeName]->set_gutterText( QColor( lst[fld++].trimmed() ) );
        m_themes[themeName]->set_gutterBack( QColor( lst[fld++].trimmed() ) );
        m_themes[themeName]->set_currentLineBack( QColor( lst[fld++].trimmed() ) );

        // key
        attr.set_color( QColor( lst[fld++].trimmed() ) );
        attr.set_weight( QFont::Normal );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_weight( QFont::Bold );
        }

        attr.set_italic( false );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_italic( true );
        }

        m_themes[themeName]->set_syntaxKey( attr );

        // type
        attr.set_color( QColor( lst[fld++].trimmed() ) );
        attr.set_weight( QFont::Normal );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_weight( QFont::Bold );
        }

        attr.set_italic( false );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_italic( true );
        }

        m_themes[themeName]->set_syntaxType( attr );

        // class
        attr.set_color( QColor( lst[fld++].trimmed() ) );
        attr.set_weight( QFont::Normal );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_weight( QFont::Bold );
        }

        attr.set_italic( false );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_italic( true );
        }

        m_themes[themeName]->set_syntaxClass( attr );

        // func
        attr.set_color( QColor( lst[fld++].trimmed() ) );
        attr.set_weight( QFont::Normal );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_weight( QFont::Bold );
        }

        attr.set_italic( false );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_italic( true );
        }

        m_themes[themeName]->set_syntaxFunc( attr );

        // quote
        attr.set_color( QColor( lst[fld++].trimmed() ) );
        attr.set_weight( QFont::Normal );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_weight( QFont::Bold );
        }

        attr.set_italic( false );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_italic( true );
        }

        m_themes[themeName]->set_syntaxQuote( attr );

        // Comment
        attr.set_color( QColor( lst[fld++].trimmed() ) );
        attr.set_weight( QFont::Normal );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_weight( QFont::Bold );
        }

        attr.set_italic( false );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_italic( true );
        }

        m_themes[themeName]->set_syntaxComment( attr );

        // MLine
        attr.set_color( QColor( lst[fld++].trimmed() ) );
        attr.set_weight( QFont::Normal );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_weight( QFont::Bold );
        }

        attr.set_italic( false );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_italic( true );
        }

        m_themes[themeName]->set_syntaxMLine( attr );

        // Constant
        attr.set_color( QColor( lst[fld++].trimmed() ) );
        attr.set_weight( QFont::Normal );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_weight( QFont::Bold );
        }

        attr.set_italic( false );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_italic( true );
        }

        m_themes[themeName]->set_syntaxConstant( attr );
    }

    m_activeTheme = "CLASSIC";
}

// eventually this can go away. It is for transitional users
//
void Settings::importOldConfig( QJsonObject object )
{
    QJsonValue value;
    QJsonArray list;

    Themes *elm = new Themes( "User", false );
    TextAttributes attr;

    // colors
    elm->set_colorText( colorFromValueString( object.value( "color-text" ).toString() ) );
    elm->set_colorBack( colorFromValueString( object.value( "color-back" ).toString() ) );
    elm->set_gutterText( QColor( "QT::DARKGRAY" ) );
    elm->set_gutterBack( QColor( "0XD0D0D0" ) );
    elm->set_currentLineBack( colorFromValueString( object.value( "color-highBack" ).toString() ) );


    list = object.value( "syntax-key" ).toArray();

    if ( list.count() > 2 )
    {
        attr.set_weight( list.at( 0 ).toDouble() );
        attr.set_italic( list.at( 1 ).toBool() );
        attr.set_color( colorFromValueString( list.at( 2 ).toString() ) );
        elm->set_syntaxKey( attr );
    }

    list = object.value( "syntax-type" ).toArray();

    if ( list.count() > 2 )
    {
        attr.set_weight( list.at( 0 ).toDouble() );
        attr.set_italic( list.at( 1 ).toBool() );
        attr.set_color( colorFromValueString( list.at( 2 ).toString() ) );
        elm->set_syntaxType( attr );
    }

    list = object.value( "syntax-class" ).toArray();

    if ( list.count() > 2 )
    {
        attr.set_weight( list.at( 0 ).toDouble() );
        attr.set_italic( list.at( 1 ).toBool() );
        attr.set_color( colorFromValueString( list.at( 2 ).toString() ) );
        elm->set_syntaxClass( attr );
    }

    list = object.value( "syntax-func" ).toArray();

    if ( list.count() > 2 )
    {
        attr.set_weight( list.at( 0 ).toDouble() );
        attr.set_italic( list.at( 1 ).toBool() );
        attr.set_color( colorFromValueString( list.at( 2 ).toString() ) );
        elm->set_syntaxFunc( attr );
    }

    list = object.value( "syntax-quote" ).toArray();

    if ( list.count() > 2 )
    {
        attr.set_weight( list.at( 0 ).toDouble() );
        attr.set_italic( list.at( 1 ).toBool() );
        attr.set_color( colorFromValueString( list.at( 2 ).toString() ) );
        elm->set_syntaxQuote( attr );
    }

    list = object.value( "syntax-comment" ).toArray();

    if ( list.count() > 2 )
    {
        attr.set_weight( list.at( 0 ).toDouble() );
        attr.set_italic( list.at( 1 ).toBool() );
        attr.set_color( colorFromValueString( list.at( 2 ).toString() ) );
        elm->set_syntaxComment( attr );
    }


    list = object.value( "syntax-mline" ).toArray();

    if ( list.count() > 2 )
    {
        attr.set_weight( list.at( 0 ).toDouble() );
        attr.set_italic( list.at( 1 ).toBool() );
        attr.set_color( colorFromValueString( list.at( 2 ).toString() ) );
        elm->set_syntaxMLine( attr );
    }

    // legacy didn't support contstants
    //
    attr.set_weight( QFont::Normal );
    attr.set_italic( false );
    attr.set_color( QColor( "#FF518C" ) );
    elm->set_syntaxConstant( attr );

    m_themes[elm->name()] = elm;

    m_activeTheme = elm->name();

}
