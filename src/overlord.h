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

#ifndef OVERLORD_H
#define OVERLORD_H

#include "settings.h"
#include "syntaxpatterns.h"
#include <QTimer>
#include <QTemporaryDir>
#include <QMutex>

class QKeyEvent;

class Overlord : public QObject
{
    CS_OBJECT( Overlord )

public:
    enum SaveFiles { SAVE_ONE, SAVE_ALL };

    static Overlord *getInstance();
    bool set_configFileName( QString name );
    void set_appPath( QString path );
    void set_newConfigFileName( QString name );

    CS_SIGNAL_1( Public, void settingsChanged( Settings *settings ) )
    CS_SIGNAL_2( settingsChanged, settings )

    void close();

    CS_SIGNAL_1( Public, void Move( QPoint pos ) )
    CS_SIGNAL_2( Move, pos )

    CS_SIGNAL_1( Public, void Resize( QSize size ) )
    CS_SIGNAL_2( Resize, size )

    CS_SIGNAL_1( Public, void preloadComplete() )
    CS_SIGNAL_2( preloadComplete )

    // Getters
    //
    bool    isComplete()
    {
        return m_isComplete;
    }
    int     rewrapColumn()
    {
        return m_settings.m_options.rewrapColumn();
    }
    int     tabSpacing()
    {
        return m_settings.m_options.tabSpacing();
    }
    int     maxVersions()
    {
        return m_settings.m_options.maxVersions();
    }
    bool    useSpaces()
    {
        return m_settings.m_options.useSpaces();
    }
    bool    removeSpaces()
    {
        return m_settings.m_options.removeSpaces();
    }
    bool    autoLoad()
    {
        return m_settings.m_options.autoLoad();
    }
    bool    astyleOnSave()
    {
        return m_settings.m_options.astyleOnSave();
    }
    bool    makeBackups()
    {
        return m_settings.m_options.makeBackups();
    }
    bool    preloadClipper()
    {
        return m_settings.m_options.preloadClipper();
    }
    bool    preloadCmake()
    {
        return m_settings.m_options.preloadCmake();
    }
    bool    preloadCpp()
    {
        return m_settings.m_options.preloadCpp();
    }
    bool    preloadCss()
    {
        return m_settings.m_options.preloadCss();
    }
    bool    preloadDoxy()
    {
        return m_settings.m_options.preloadDoxy();
    }
    bool    preloadErrLog()
    {
        return m_settings.m_options.preloadErrLog();
    }
    bool    preloadHtml()
    {
        return m_settings.m_options.preloadHtml();
    }
    bool    preloadJava()
    {
        return m_settings.m_options.preloadJava();
    }
    bool    preloadJs()
    {
        return m_settings.m_options.preloadJs();
    }
    bool    preloadJson()
    {
        return m_settings.m_options.preloadJson();
    }
    bool    preloadMake()
    {
        return m_settings.m_options.preloadMake();
    }
    bool    preloadNone()
    {
        return m_settings.m_options.preloadNone();
    }
    bool    preloadNSI()
    {
        return m_settings.m_options.preloadNSI();
    }
    bool    preloadPhp()
    {
        return m_settings.m_options.preloadPhp();
    }
    bool    preloadPl()
    {
        return m_settings.m_options.preloadPl();
    }
    bool    preloadPy()
    {
        return m_settings.m_options.preloadPy();
    }
    bool    preloadSh()
    {
        return m_settings.m_options.preloadSh();
    }
    bool    preloadTxt()
    {
        return m_settings.m_options.preloadTxt();
    }
    bool    preloadXml()
    {
        return m_settings.m_options.preloadXml();
    }

    QString formatDate()
    {
        return m_settings.m_options.formatDate();
    }
    QString formatTime()
    {
        return m_settings.m_options.formatTime();
    }
    QString backupDirectory()
    {
        return m_settings.m_options.backupDirectory();
    }
    QString mainDictionary()
    {
        return m_settings.m_options.mainDictionary();
    }
    QString userDictionary()
    {
        return m_settings.m_options.userDictionary();
    }
    QString aboutUrl()
    {
        return m_settings.m_options.aboutUrl();
    }
    QString syntaxPath()
    {
        return m_settings.m_options.syntaxPath();
    }

    KeyDefinitions &keys()
    {
        return m_settings.m_options.keys();
    }

    bool    showLineHighlight()
    {
        return m_settings.m_showLineHighlight;
    }
    bool    showLineNumbers()
    {
        return m_settings.m_showLineNumbers;
    }
    bool    isColumnMode()
    {
        return m_settings.m_isColumnMode;
    }
    bool    isSpellCheck()
    {
        return m_settings.m_isSpellCheck;
    }
    bool    isWordWrap()
    {
        return m_settings.m_isWordWrap;
    }
    bool    showSpaces()
    {
        return m_settings.m_showSpaces;
    }
    bool    showBreaks()
    {
        return m_settings.m_showBreaks;
    }
    bool    flagNoAutoLoad()
    {
        return m_settings.m_flagNoAutoLoad;
    }
    bool    flagNoSaveConfig()
    {
        return m_settings.m_flagNoSaveConfig;
    }

    QString priorPath()
    {
        return m_settings.m_priorPath;
    }
    QString activeTheme()
    {
        return m_settings.m_activeTheme;
    }
    QString tempDir()
    {
        return m_tmpDir.path();
    }
    QFont   fontNormal()
    {
        return m_settings.m_fontNormal;
    }
    QFont   fontColumn()
    {
        return m_settings.m_fontColumn;
    }

    Themes  *currentTheme()
    {
        return m_settings.m_themes[m_settings.m_activeTheme];
    }
    QStringList availableThemes()
    {
        return m_settings.m_themes.keys();
    }
    bool themeNameExists( QString name )
    {
        return m_settings.themeNameExists( name );
    }

    QStringList &findList()
    {
        return m_settings.m_findList;
    }
    QStringList &replaceList()
    {
        return m_settings.m_replaceList;
    }
    QStringList &recentFiles()
    {
        return m_settings.m_recentFilesList;
    }
    QStringList &recentFolders()
    {
        return m_settings.m_rFolderList;
    }
    QStringList &presetFolders()
    {
        return m_settings.m_preFolderList;
    }

    QSize   lastSize()
    {
        return m_settings.m_lastSize;
    }
    QPoint  lastPosition()
    {
        return m_settings.m_lastPosition;
    }
    QString lastActiveFile()
    {
        return m_settings.m_lastActiveFile;
    }
    int     lastActiveRow()
    {
        return m_settings.m_lastActiveRow;
    }
    int     lastActiveColumn()
    {
        return m_settings.m_lastActiveColumn;
    }

    QString openedFiles( int sub )
    {
        return m_settings.m_openedFiles[sub];
    }
    int     openedFilesCount()
    {
        return m_settings.m_openedFiles.count();
    }
    bool    openedModified( int sub )
    {
        return m_settings.m_openedModified[sub];
    }

    QString configFileName()
    {
        return m_settings.m_configFileName;
    }

    QString advancedFindText()
    {
        return m_settings.m_advancedFindText;
    }
    QString advancedFindFileType()
    {
        return m_settings.m_advancedFindFileType;
    }
    QString advancedFindFolder()
    {
        return m_settings.m_advancedFindFolder;
    }
    bool    advancedFSearchFolders()
    {
        return m_settings.m_advancedFSearchFolders;
    }
    bool    advancedFCase()
    {
        return m_settings.m_advancedFCase;
    }
    bool    advancedFWholeWords()
    {
        return m_settings.m_advancedFWholeWords;
    }
    QTextDocument::FindFlags findFlags()
    {
        return m_settings.m_findFlags;
    }
    QString findText()
    {
        return m_settings.m_findText;
    }
    QString replaceText()
    {
        return m_settings.m_replaceText;
    }
    bool    findWholeWords()
    {
        return m_settings.m_findWholeWords;
    }
    bool    findDirection()
    {
        return m_settings.m_findDirection;
    }
    bool    findCase()
    {
        return m_settings.m_findCase;
    }
    int     replaceListFind( QString text )
    {
        return m_settings.m_replaceList.indexOf( text );
    }
    bool    printLineNumbers()
    {
        return m_settings.m_printSettings.lineNumbers();
    }
    bool    printHeader()
    {
        return m_settings.m_printSettings.printHeader();
    }
    bool    printFooter()
    {
        return m_settings.m_printSettings.printFooter();
    }
    QString headerLeft()
    {
        return m_settings.m_printSettings.headerLeft();
    }
    QString headerCenter()
    {
        return m_settings.m_printSettings.headerCenter();
    }
    QString headerRight()
    {
        return m_settings.m_printSettings.headerRight();
    }
    QString headerLine2()
    {
        return m_settings.m_printSettings.headerLine2();
    }
    QString footerLeft()
    {
        return m_settings.m_printSettings.footerLeft();
    }
    QString footerCenter()
    {
        return m_settings.m_printSettings.footerCenter();
    }
    QString footerRight()
    {
        return m_settings.m_printSettings.footerRight();
    }
    QString footerLine2()
    {
        return m_settings.m_printSettings.footerLine2();
    }
    double  marginLeft()
    {
        return m_settings.m_printSettings.marginLeft();
    }
    double  marginRight()
    {
        return m_settings.m_printSettings.marginRight();
    }
    double  marginTop()
    {
        return m_settings.m_printSettings.marginTop();
    }
    double  marginBottom()
    {
        return m_settings.m_printSettings.marginBottom();
    }
    double  headerGap()
    {
        return m_settings.m_printSettings.headerGap();
    }
    QFont   printFontHeader()
    {
        return m_settings.m_printSettings.fontHeader();
    }
    QFont   printFontFooter()
    {
        return m_settings.m_printSettings.fontFooter();
    }
    QFont   printFontText()
    {
        return m_settings.m_printSettings.fontText();
    }
    int     findListFind( QString text )
    {
        return m_settings.findListFind( text );
    }
    int     openedFilesFind( QString name )
    {
        return m_settings.openedFilesFind( name );
    }
    QString edtLastDeletedChar()
    {
        return m_settings.m_edtLastDeletedChar;
    }
    QString edtLastDeletedWord()
    {
        return m_settings.m_edtLastDeletedWord;
    }
    QString edtLastDeletedLine()
    {
        return m_settings.m_edtLastDeletedLine;
    }
    bool    edtMode()
    {
        return keys().edtEnabled();
    }
    bool    edtWordCtrlMeta()
    {
        return keys().edtWordCtrlMeta();
    }
    bool    edtWordAltOption()
    {
        return keys().edtWordAltOption();
    }
    int     macroCount()
    {
        return m_settings.m_macros.count();
    }

    QStringList macroNames()
    {
        return m_settings.m_macros.keys();
    }

    QList<MacroStruct *> viewMacro( QString macroName );
    bool macroExists( QString macroName );
    bool openedFilesContains( QString name );
    bool recentFilesListContains( QString text );
    bool edtDirection()
    {
        return m_edtDirection;
    }

    SyntaxPatterns *getSyntaxPatterns( QString fileName );



    //
    // setters
    //
    void set_options( const Options &opt );
    void set_printSettings( const PrintSettings &prt );

    void set_preFolderList( QStringList lst );
    void set_showLineHighlight( bool yesNo );
    void set_showLineNumbers( bool yesNo );
    void set_isColumnMode( bool yesNo );
    void set_isSpellCheck( bool yesNo );
    void set_isWordWrap( bool yesNo );
    void set_showSpaces( bool yesNo );
    void set_showBreaks( bool yesNo );

    void set_preloadClipper( bool yesNo );
    void set_preloadCmake( bool yesNo );
    void set_preloadCpp( bool yesNo );
    void set_preloadCss( bool yesNo );
    void set_preloadDoxy( bool yesNo );
    void set_preloadErrLog( bool yesNo );
    void set_preloadHtml( bool yesNo );
    void set_preloadJava( bool yesNo );
    void set_preloadJs( bool yesNo );
    void set_preloadJson( bool yesNo );
    void set_preloadMake( bool yesNo );
    void set_preloadNone( bool yesNo );
    void set_preloadNSI( bool yesNo );
    void set_preloadPhp( bool yesNo );
    void set_preloadPl( bool yesNo );
    void set_preloadPy( bool yesNo );
    void set_preloadSh( bool yesNo );
    void set_preloadTxt( bool yesNo );
    void set_preloadXml( bool yesNo );


    void set_priorPath( const QString &priorPath );
    void set_activeTheme( const QString &name );

    void set_fontNormal( const QFont &font );
    void set_fontColumn( const QFont &font );

    void set_tabSpacing( int spacing );
    void set_rewrapColumn( int col );
    void set_formatDate( const QString &fmt );
    void set_formatTime( const QString &fmt );
    void set_useSpaces( bool yesNo );

    void set_flagNoAutoLoad( bool yesNo );
    void set_flagNoSaveConfig( bool yesNo );

    void set_findList( QStringList list );
    void set_advancedFindText( QString text );
    void set_advancedFindFileType( QString text );
    void set_advancedFindFolder( QString text );
    void set_advancedFCase( bool yesNo );
    void set_advancedFWholeWords( bool yesNo );
    void set_advancedFSearchFolders( bool yesNo );
    void set_findText( QString text );
    void set_findFlags( QTextDocument::FindFlags flags );
    void set_replaceText( QString text );
    void set_findWholeWords( bool yesNo );
    void set_findDirection( bool yesNo );
    void set_findFlagsBackward();
    void set_findFlagsCaseSensitive();
    void set_findFlagsWholeWords();
    void set_findCase( bool yesNo );
    void set_replaceList( QStringList lst );
    void set_lastSize( QSize size );
    void set_lastPosition( QPoint pos );
    void set_lastActiveFile( QString fileName );
    void set_lastActiveRow( int row );
    void set_lastActiveColumn( int column );
    void set_edtLastDeletedWord( QString word );
    void set_edtLastDeletedLine( QString line );
    void set_edtLastDeletedChar( QString c );  // because JSON doesn't do QChar
    void set_syntaxPath( const QString path );
    void set_mainDictionary( const QString path );
    void set_userDictionary( const QString path );


    void openedFilesClear();
    void openedModifiedClear();
    void openedFilesAppend( QString fullName );
    void openedModifiedAppend( bool yesNo );
    void recentFileListClear();
    void openedFilesRemove( QString name );
    void findListPrepend( QString text );
    void openedModifiedReplace( int sub, bool yesNo );
    void findListMove( int index, int dest );
    void replaceListPrepend( QString text );
    void replaceListMove( int index, int dest );
    void set_edtDirection( bool forwardBackwards );
    void deleteMacro( QString macroName );
    void deleteMacroKeyStrokes( QList<MacroStruct *> &keyStrokes );
    void renameMacro( QString originalName, QString newName );
    void addMacro( QString macroName, QList<MacroStruct *> keyStrokes ); // TODO:: be sure to copy events
    void addMacro( QString macroName, QList<QKeyEvent *> keyStrokes );  // TODO:: be sure to copy events


    // dangerous methods needed by dialogs that are designed to change
    // various settings locally until the user accepts the changes.
    //
    // Please keep these names as pull and update so people don't think they
    // should be used as regular access methods.
    //
    Settings *pointerToSettings();
    Settings &pullLocalSettingsCopy();
    void updateSettingsFromLocalCopy( Settings &settings );

    Options &pullLocalCopyOfOptions();
    void updateOptionsFromLocalCopy( Options &options );

    PrintSettings &pullLocalCopyOfPrintSettings();
    void updatePrintSettingsFromLocalCopy( PrintSettings &pSettings );

    QStringList pullCopyOfPreFolderList();
    void updatePreFolderListFromLocalCopy( QStringList &lst );
    //
    // end dangerous methods
    void markToNotify();

private:
    Overlord();
    ~Overlord();
    void preloadSyntax();

    void checkForChange();

    void checkForBroadcast();

    static Overlord *m_instance;

    QMutex m_mutex;
    Settings m_settings;

    QTimer m_flushTimer;
    QTimer m_broadcastTimer;
    bool m_isComplete;
    bool m_changed;
    bool m_needsBroadcast;

    QString m_appPath;
    QString m_configFileName;
    QTemporaryDir m_tmpDir;

    QMap<QString, SyntaxPatterns *> m_syntaxPatterns;
    bool m_edtDirection;  // do not save this - always default to forward when starting
    bool m_closed;        // do not save this - used by destructor to see if we need to close
};

#endif
