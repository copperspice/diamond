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
#include "overlord.h"
#include <qtconcurrentrun.h>
#include <QThread>
#include <QTime>
#include <iostream>
#include <QApplication>
#include <QKeyEvent>

Overlord *Overlord::m_instance = nullptr;

Overlord::Overlord() :
    QObject( nullptr )
    , m_isComplete( false )
    , m_changed( false )
    , m_needsBroadcast( false )
    , m_edtDirection( false )
    , m_closed( false )
{
    connect( &m_settings, &Settings::Move, this, &Overlord::Move );
    connect( &m_settings, &Settings::Resize, this, &Overlord::Resize );
}

Overlord::~Overlord()
{
    for ( SyntaxPatterns *ptr : m_syntaxPatterns )
    {
        delete ptr;
    }

    m_syntaxPatterns.clear();
}

void Overlord::close()
{
    m_closed = true;

    if ( m_flushTimer.isActive() )
    {
        m_flushTimer.stop();
    }

    if ( m_broadcastTimer.isActive() )
    {
        m_broadcastTimer.stop();
    }

    if ( m_changed )
    {
        m_settings.save();
    }
}

void Overlord::checkForChange()
{
    if ( m_changed )
    {
        m_changed = false;
        // launch without waiting
        QFuture<void> t1 = QtConcurrent::run( &m_settings, &Settings::save );
    }
}

void Overlord::checkForBroadcast()
{
    if ( m_needsBroadcast )
    {
        m_needsBroadcast = false;
        settingsChanged( &m_settings );
    }
}

// configFileName is neither loaded nor stored, so no need to set flag
bool Overlord::set_configFileName( QString name )
{
    m_mutex.lock();
    m_configFileName = name;
    m_settings.m_configFileName = name;
    bool retVal = m_settings.load();

    m_mutex.unlock();

    if ( retVal )
    {
        m_isComplete = true;
        connect( &m_broadcastTimer, &QTimer::timeout, this, &Overlord::checkForBroadcast );
        connect( &m_flushTimer, &QTimer::timeout, this, &Overlord::checkForChange );

        m_flushTimer.start( 30000 );   // flush to disk at most every 30 seconds
        m_broadcastTimer.start( 3000 ); // broadcast up to once every 3 seconds

        preloadSyntax();
        preloadComplete();
    }

    return retVal;
}

void Overlord::set_newConfigFileName( QString name )
{
    m_mutex.lock();
    m_configFileName = name;
    m_settings.m_configFileName = name;
    m_changed = true;
    m_mutex.unlock();
    checkForChange();
}

// appPath not stored in file so no need to set changed flag
void Overlord::set_appPath( QString path )
{
    m_mutex.lock();
    m_appPath = path;
    m_settings.m_appPath = path;
    m_mutex.unlock();
}

Overlord *Overlord::getInstance()
{
    if ( !m_instance )
    {
        m_instance = new Overlord();
    }

    return m_instance;
}

void Overlord::set_options( const Options &opt )
{
    m_mutex.lock();
    m_settings.m_options = opt;
    markToNotify();
    m_mutex.unlock();
}


void Overlord::set_printSettings( const PrintSettings &prt )
{
    m_mutex.lock();
    m_settings.m_printSettings = prt;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_preFolderList( QStringList lst )
{
    m_mutex.lock();
    m_settings.m_preFolderList = lst;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_showLineHighlight( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_showLineHighlight = yesNo;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_preloadClipper( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadClipper( yesNo );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_preloadCmake( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadCmake( yesNo );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_preloadCpp( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadCpp( yesNo );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_preloadCss( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadCss( yesNo );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_preloadDoxy( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadDoxy( yesNo );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_preloadErrLog( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadErrLog( yesNo );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_preloadJava( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadJava( yesNo );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_preloadJs( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadJs( yesNo );
    markToNotify();
    m_mutex.unlock();
}


void Overlord::set_preloadHtml( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadHtml( yesNo );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_preloadJson( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadJson( yesNo );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_preloadMake( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadMake( yesNo );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_preloadNone( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadNone( yesNo );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_preloadNSI( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadNSI( yesNo );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_preloadPhp( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadPhp( yesNo );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_preloadPl( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadPl( yesNo );
    markToNotify();
    m_mutex.unlock();
}


void Overlord::set_preloadPy( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadPy( yesNo );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_preloadSh( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadSh( yesNo );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_preloadTxt( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadTxt( yesNo );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_preloadXml( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_preloadXml( yesNo );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_showLineNumbers( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_showLineNumbers = yesNo;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_isColumnMode( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_isColumnMode = yesNo;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_isSpellCheck( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_isSpellCheck = yesNo;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_isWordWrap( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_isWordWrap = yesNo;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_showSpaces( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_showSpaces = yesNo;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_showBreaks( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_showBreaks = yesNo;
    markToNotify();
    m_mutex.unlock();
}


void Overlord::set_priorPath( const QString &priorPath )
{
    m_mutex.lock();
    m_settings.m_priorPath = priorPath;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_activeTheme( const QString &name )
{
    m_mutex.lock();
    m_settings.m_activeTheme = name;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_fontNormal( const QFont &font )
{
    m_mutex.lock();
    m_settings.m_fontNormal = font;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_fontColumn( const QFont &font )
{
    m_mutex.lock();
    m_settings.m_fontColumn = font;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_tabSpacing( int spacing )
{
    m_mutex.lock();
    m_settings.m_options.set_tabSpacing( spacing );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_rewrapColumn( int col )
{
    m_mutex.lock();
    m_settings.m_options.set_rewrapColumn( col );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_formatDate( const QString &fmt )
{
    m_mutex.lock();
    m_settings.m_options.set_formatDate( fmt );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_formatTime( const QString &fmt )
{
    m_mutex.lock();
    m_settings.m_options.set_formatTime( fmt );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_useSpaces( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_options.set_useSpaces( yesNo );
    markToNotify();
    m_mutex.unlock();
}

// flag members are not stored so don't set changed flag
void Overlord::set_flagNoAutoLoad( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_flagNoAutoLoad = yesNo;
    m_mutex.unlock();
}

// flag members are not stored so don't set changed flag
void Overlord::set_flagNoSaveConfig( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_flagNoSaveConfig = yesNo;
    m_mutex.unlock();
}

void Overlord::set_findList( QStringList list )
{
    m_mutex.lock();
    m_settings.m_findList = list;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_advancedFindText( QString text )
{
    m_mutex.lock();
    m_settings.m_advancedFindText = text;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_advancedFindFileType( QString text )
{
    m_mutex.lock();
    m_settings.m_advancedFindFileType = text;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_advancedFindFolder( QString text )
{
    m_mutex.lock();
    m_settings.m_advancedFindFolder = text;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_advancedFCase( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_advancedFCase = yesNo;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_advancedFWholeWords( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_advancedFWholeWords = yesNo;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_advancedFSearchFolders( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_advancedFSearchFolders = yesNo;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_findText( QString text )
{
    m_mutex.lock();
    m_settings.m_findText = text;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_findFlags( QTextDocument::FindFlags flags )
{
    m_mutex.lock();
    m_settings.m_findFlags = flags;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_replaceText( QString text )
{
    m_mutex.lock();
    m_settings.m_replaceText = text;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_findWholeWords( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_findWholeWords = yesNo;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_findDirection( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_findDirection = yesNo;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_findFlagsBackward()
{
    m_mutex.lock();
    m_settings.m_findFlags |= QTextDocument::FindBackward;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_findFlagsCaseSensitive()
{
    m_mutex.lock();
    m_settings.m_findFlags |= QTextDocument::FindCaseSensitively;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_findFlagsWholeWords()
{
    m_mutex.lock();
    m_settings.m_findFlags |= QTextDocument::FindWholeWords;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_findCase( bool yesNo )
{
    m_mutex.lock();
    m_settings.m_findCase = yesNo;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_replaceList( QStringList lst )
{
    m_mutex.lock();
    m_settings.m_replaceList = lst;
    markToNotify();
    m_mutex.unlock();
}


void Overlord::updateSettingsFromLocalCopy( Settings &settings )
{
    m_mutex.lock();
    m_settings = settings;
    m_needsBroadcast = false;
    settingsChanged( &m_settings );
    markToNotify();
    m_mutex.unlock();
    checkForChange();
}

Settings &Overlord::pullLocalSettingsCopy()
{
    return m_settings;
}

Settings *Overlord::pointerToSettings()
{
    return &m_settings;
}

QList<MacroStruct *> Overlord::viewMacro( QString macroName )
{
    return m_settings.m_macros[ macroName ];
}

PrintSettings &Overlord::pullLocalCopyOfPrintSettings()
{
    return m_settings.m_printSettings;
}

void Overlord::updatePrintSettingsFromLocalCopy( PrintSettings &pSettings )
{
    m_mutex.lock();
    m_settings.m_printSettings = pSettings;
    markToNotify();
    m_mutex.unlock();
}

Options &Overlord::pullLocalCopyOfOptions()
{
    return m_settings.m_options;
}

void Overlord::updateOptionsFromLocalCopy( Options &options )
{
    m_mutex.lock();
    m_settings.m_options = options;
    markToNotify();
    m_mutex.unlock();
}

QStringList Overlord::pullCopyOfPreFolderList()
{
    return m_settings.m_preFolderList;
}

void Overlord::updatePreFolderListFromLocalCopy( QStringList &lst )
{
    m_mutex.lock();
    m_settings.m_preFolderList = lst;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::openedFilesClear()
{
    m_mutex.lock();
    m_settings.openedFilesClear();
    markToNotify();
    m_mutex.unlock();
}

void Overlord::openedModifiedClear()
{
    m_mutex.lock();
    m_settings.openedModifiedClear();
    markToNotify();
    m_mutex.unlock();
}

void Overlord::recentFileListClear()
{
    m_mutex.lock();
    m_settings.recentFileListClear();
    markToNotify();
    m_mutex.unlock();
}

void Overlord::openedFilesAppend( QString fullName )
{
    m_mutex.lock();
    m_settings.openedFilesAppend( fullName );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::openedModifiedAppend( bool yesNo )
{
    m_mutex.lock();
    m_settings.openedModifiedAppend( yesNo );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::markToNotify()
{
    m_changed = true;
    m_needsBroadcast = true;
}

void Overlord::openedFilesRemove( QString name )
{
    m_mutex.lock();
    m_settings.openedFilesRemove( name );
    markToNotify();
    m_mutex.unlock();
}

bool Overlord::macroExists( QString macroName )
{
    return m_settings.m_macros.contains( macroName );
}

// This is being called from within code that already has a mutex lock
//
void Overlord::deleteMacroKeyStrokes( QList<MacroStruct *> &keyStrokes )
{
    for ( MacroStruct *ptr : keyStrokes )
    {
        delete ptr;
    }

    keyStrokes.clear();
    markToNotify();
}

void Overlord::deleteMacro( QString macroName )
{
    if ( macroExists( macroName ) )
    {
        m_mutex.lock();
        deleteMacroKeyStrokes( m_settings.m_macros[macroName] );
        m_settings.m_macros.erase( m_settings.m_macros.find( macroName ) );
        markToNotify();
        m_mutex.unlock();
    }
}

void Overlord::renameMacro( QString macroName, QString newName )
{
    if ( macroExists( macroName ) )
    {
        m_mutex.lock();
        // have to make copy of keystrokes
        QList<MacroStruct *> lst;

        for ( MacroStruct *ptr : m_settings.m_macros[ macroName] )
        {
            lst.append( new MacroStruct( *ptr ) );
        }

        m_settings.m_macros[ newName] = lst;

        // have to unlock mutex here because delete needs its own lock
        m_mutex.unlock();

        // delete will also call markToNotify()
        deleteMacro( macroName );
    }
}

void Overlord::addMacro( QString macroName, QList<MacroStruct *> keyStrokes )
{
    // If you pass in an existing name the keyStrokes will be overwritten
    // It is the only way to allow for update
    //
    QList<MacroStruct *> lst;

    for ( MacroStruct *ptr : keyStrokes )
    {
        lst.append( new MacroStruct( *ptr ) );
    }

    m_mutex.lock();
    m_settings.m_macros[ macroName] = lst;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::addMacro( QString macroName, QList<QKeyEvent *> keyStrokes )
{
    // If you pass in an existing name the keyStrokes will be overwritten
    // It is the only way to allow for update
    //
    QList<MacroStruct *> lst;

    for ( QKeyEvent *evt : keyStrokes )
    {
        MacroStruct *macroStruct    = new MacroStruct( evt->key(), evt->modifiers(), evt->text() );
        lst.append( macroStruct );
    }

    m_mutex.lock();
    m_settings.m_macros[ macroName] = lst;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::findListPrepend( QString text )
{
    m_mutex.lock();
    m_settings.findListPrepend( text );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_lastSize( QSize size )
{
    m_mutex.lock();
    m_settings.set_lastSize( size );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_lastPosition( QPoint pos )
{
    m_mutex.lock();
    m_settings.set_lastPosition( pos );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_lastActiveFile( QString fileName )
{
    m_mutex.lock();
    m_settings.set_lastActiveFile( fileName );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_lastActiveRow( int row )
{
    m_mutex.lock();
    m_settings.set_lastActiveRow( row );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_lastActiveColumn( int column )
{
    m_mutex.lock();
    m_settings.set_lastActiveColumn( column );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::openedModifiedReplace( int sub, bool yesNo )
{
    m_mutex.lock();
    m_settings.openedModifiedReplace( sub, yesNo );
    markToNotify();
    m_mutex.unlock();
}

bool Overlord::openedFilesContains( QString name )
{
    return m_settings.openedFilesContains( name );
}

bool Overlord::recentFilesListContains( QString text )
{
    return m_settings.recentFilesListContains( text );
}

void Overlord::findListMove( int index, int dest )
{
    m_mutex.lock();
    m_settings.findListMove( index, dest );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::replaceListPrepend( QString text )
{
    m_mutex.lock();
    m_settings.replaceListPrepend( text );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::replaceListMove( int index, int dest )
{
    m_mutex.lock();
    m_settings.replaceListMove( index, dest );
    markToNotify();
    m_mutex.unlock();
}

SyntaxPatterns *Overlord::getSyntaxPatterns( QString fileName )
{
    m_mutex.lock();

    if ( !m_syntaxPatterns.contains( fileName ) )
    {
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    m_mutex.unlock();

    return m_syntaxPatterns[fileName];
}

void Overlord::preloadSyntax()
{
    QString fileName;

    m_mutex.lock();

    if ( m_settings.m_options.preloadClipper() )
    {
        fileName = m_settings.syntaxPath() + "syn_clipper.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadCmake() )
    {
        fileName = m_settings.syntaxPath() + "syn_cmake.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadCpp() )
    {
        fileName = m_settings.syntaxPath() + "syn_cpp.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadCss() )
    {
        fileName = m_settings.syntaxPath() + "syn_css.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadDoxy() )
    {
        fileName = m_settings.syntaxPath() + "syn_doxy.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadErrLog() )
    {
        fileName = m_settings.syntaxPath() + "syn_errlog.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadHtml() )
    {
        fileName = m_settings.syntaxPath() + "syn_html.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadJava() )
    {
        fileName = m_settings.syntaxPath() + "syn_java.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadJs() )
    {
        fileName = m_settings.syntaxPath() + "syn_js.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadJson() )
    {
        fileName = m_settings.syntaxPath() + "syn_json.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadMake() )
    {
        fileName = m_settings.syntaxPath() + "syn_make.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadNone() )
    {
        fileName = m_settings.syntaxPath() + "syn_none.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadNSI() )
    {
        fileName = m_settings.syntaxPath() + "syn_nsi.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadPhp() )
    {
        fileName = m_settings.syntaxPath() + "syn_php.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadPl() )
    {
        fileName = m_settings.syntaxPath() + "syn_pl.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadPy() )
    {
        fileName = m_settings.syntaxPath() + "syn_py.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadSh() )
    {
        fileName = m_settings.syntaxPath() + "syn_sh.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadTxt() )
    {
        fileName = m_settings.syntaxPath() + "syn_txt.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadXml() )
    {
        fileName = m_settings.syntaxPath() + "syn_xml.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    m_mutex.unlock();
}

void Overlord::set_edtDirection( bool forwardBackward )
{
    m_mutex.lock();
    m_edtDirection = forwardBackward;
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_edtLastDeletedWord( QString word )
{
    m_mutex.lock();
    m_settings.m_edtLastDeletedWord = word;
    markToNotify();
    m_mutex.unlock();
    checkForBroadcast();        // needs to go out immediately in case cut and paste between buffers
}

void Overlord::set_edtLastDeletedLine( QString line )
{
    m_mutex.lock();
    m_settings.m_edtLastDeletedLine = line;
    markToNotify();
    m_mutex.unlock();
    checkForBroadcast();        // needs to go out immediately in case cut and past between buffers
}

void Overlord::set_edtLastDeletedChar( QString c )
{
    m_mutex.lock();
    m_settings.m_edtLastDeletedChar = c;
    markToNotify();
    m_mutex.unlock();
    checkForBroadcast();        // needs to go out immediately in case cut and paste between buffers
}

void Overlord::set_syntaxPath( const QString path )
{
    m_mutex.lock();
    m_settings.set_syntaxPath( path );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_mainDictionary( const QString path )
{
    m_mutex.lock();
    m_settings.set_mainDictionary( path );
    markToNotify();
    m_mutex.unlock();
}

void Overlord::set_userDictionary( const QString path )
{
    m_mutex.lock();
    m_settings.set_userDictionary( path );
    markToNotify();
    m_mutex.unlock();
}
