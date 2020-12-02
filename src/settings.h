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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QFont>
#include <QString>
#include <QMap>
#include <QStringList>
#include <QPoint>
#include <QSize>
#include <QTextDocument>
#include "themes.h"
#include "options.h"
#include "printsettings.h"
#include "macrostruct.h"

class Settings : public QObject
{
    CS_OBJECT( Settings )

public:
    Settings();
    ~Settings();

    Settings &operator =( const Settings &other );

    friend class Overlord;

    void copyTheme( QString name, QString dest );
    void deleteTheme( QString name );
    bool themeNameExists( QString name );

    // had to leave these getters and setters in place because
    // colors and options dialogs need to work with local copies
    // until change is saved.
    //
    // getters
    //
    QString activeTheme()
    {
        return m_activeTheme;
    }
    Themes  *currentTheme()
    {
        return m_themes[m_activeTheme];
    }
    QString syntaxPath()
    {
        return m_options.syntaxPath();
    }
    QStringList availableThemes()
    {
        return m_themes.keys();
    }
    QFont   fontColumn()
    {
        return m_fontColumn;
    }
    QFont   fontNormal()
    {
        return m_fontNormal;
    }
    bool    showLineNumbers()
    {
        return m_showLineNumbers;
    }
    bool    isColumnMode()
    {
        return m_isColumnMode;
    }
    QString mainDictionary()
    {
        return m_options.mainDictionary();
    }
    QString userDictionary()
    {
        return m_options.userDictionary();
    }
    int     tabSpacing()
    {
        return m_options.tabSpacing();
    }
    bool    useSpaces()
    {
        return m_options.useSpaces();
    }
    QString formatDate()
    {
        return m_options.formatDate();
    }
    QString formatTime()
    {
        return m_options.formatTime();
    }
    int     rewrapColumn()
    {
        return m_options.rewrapColumn();
    }
    bool    showLineHighlight()
    {
        return m_showLineHighlight;
    }

    //
    // setters
    //
    void set_activeTheme( const QString &name )
    {
        m_activeTheme = name;
    }
    void set_syntaxPath( const QString path )
    {
        m_options.set_syntaxPath( path );
    }
    void add_theme( Themes *theme );
    void set_mainDictionary( const QString path )
    {
        m_options.set_mainDictionary( path );
    }
    void set_userDictionary( const QString path )
    {
        m_options.set_userDictionary( path );
    }


private:
    void generateDefaultThemes();

    bool    openedFilesContains( QString name );
    int     openedFilesFind( QString name );
    void    openedModifiedReplace( int sub, bool yesNo );
    int     findListFind( QString text );
    bool    recentFilesListContains( QString text );
    void    deleteAllThemes();
    void    deleteAllMacros();

    void openedFilesClear()
    {
        m_openedFiles.clear();
    }
    void openedModifiedClear()
    {
        m_openedModified.clear();
    }
    void recentFileListClear()
    {
        m_recentFilesList.clear();
    }
    void openedFilesAppend( QString curFile )
    {
        m_openedFiles.append( curFile );
    }
    void openedModifiedAppend( bool yesNo )
    {
        m_openedModified.append( yesNo );
    }
    void openedFilesRemove( QString name )
    {
        m_openedFiles.removeOne( name );
    }
    void findListPrepend( QString text )
    {
        m_findList.prepend( text );
    }
    void findListMove( int index, int dest )
    {
        m_findList.move( index, dest );
    }
    void replaceListPrepend( QString text )
    {
        m_replaceList.prepend( text );
    }
    void replaceListMove( int index, int dest )
    {
        m_replaceList.move( index, dest );
    }

    // json
    void json_Save_MacroNames( const QStringList &macroNames );
    bool json_Read();
    QByteArray json_ReadFile();
    void importOldConfig( QJsonObject object );

    void set_lastSize( QSize size )
    {
        m_lastSize = size;
    }
    void set_lastPosition( QPoint pos )
    {
        m_lastPosition = pos;
    }
    void set_lastActiveFile( QString name )
    {
        m_lastActiveFile = name;
    }
    void set_lastActiveRow( int num )
    {
        m_lastActiveRow = num;
    }
    void set_lastActiveColumn( int num )
    {
        m_lastActiveColumn = num;
    }

    //
    // signals
    //
    CS_SIGNAL_1( Public, void Move( QPoint pos ) )
    CS_SIGNAL_2( Move, pos )

    CS_SIGNAL_1( Public, void Resize( QSize size ) )
    CS_SIGNAL_2( Resize, size )

    void trimBackups( QString path );
    void createThemeArray( QJsonObject &object );
    void createMacroArray( QJsonObject &object );

    bool load();
    void save();


    QString         m_activeTheme;
    bool            m_advancedFCase;
    bool            m_advancedFSearchFolders;
    bool            m_advancedFWholeWords;
    QString         m_advancedFindFileType;
    QString         m_advancedFindFolder;
    QString         m_advancedFindText;
    QString         m_appPath;
    QString         m_configFileName;
    QString         m_edtLastDeletedChar;
    QString         m_edtLastDeletedLine;
    QString         m_edtLastDeletedWord;
    bool            m_findCase;
    bool            m_findDirection;
    QTextDocument::FindFlags m_findFlags;
    QStringList     m_findList;
    QString         m_findText;
    bool            m_findWholeWords;
    bool            m_flagNoAutoLoad;        // command line param - do not save
    bool            m_flagNoSaveConfig;      // command line param - do not save
    QFont           m_fontColumn;
    QFont           m_fontNormal;
    bool            m_isColumnMode;
    bool            m_isComplete;
    bool            m_isSpellCheck;
    bool            m_isWordWrap;
    QPoint          m_lastPosition;
    QSize           m_lastSize;
    QString         m_lastActiveFile;
    int             m_lastActiveRow;
    int             m_lastActiveColumn;
    QStringList     m_openedFiles;
    QList<bool>     m_openedModified;
    Options         m_options;
    QStringList     m_preFolderList;
    PrintSettings   m_printSettings;
    QString         m_priorPath;
    QStringList     m_rFolderList;
    QStringList     m_recentFilesList;
    QStringList     m_replaceList;
    QString         m_replaceText;
    bool            m_showBreaks;
    bool            m_showLineHighlight;
    bool            m_showLineNumbers;
    bool            m_showSpaces;

    QMap <QString, Themes *> m_themes;
    QMap< QString, QList<MacroStruct *>>  m_macros;  // TODO:: destructor needs to nuke the entries
};

#endif
