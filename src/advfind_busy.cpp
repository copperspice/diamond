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
#include "advfind_busy.h"
#include <QFutureWatcher>
#include <qtconcurrentrun.h>
#include <QDir>
#include "overlord.h"

AdvFind_Busy::AdvFind_Busy( QWidget *parent ) :
    Dialog_Busy( parent )
{
    connect( &m_watcher, SIGNAL( finished() ), this, SLOT( finished() ) );
    setProgressLabelVisible( true );
    setCancelButtonVisible( true );
}

AdvFind_Busy::~AdvFind_Busy()
{
    if ( m_watcher.isRunning() )
    {
        m_watcher.cancel();
        m_watcher.waitForFinished();
    }
}

int AdvFind_Busy::exec()
{
    QFuture<void> future = QtConcurrent::run( this, &AdvFind_Busy::advFind_getResults );
    m_watcher.setFuture( future );
    return QDialog::exec();
}

void AdvFind_Busy::reject()
{
    m_watcher.cancel();
    m_watcher.waitForFinished();
    QDialog::reject();
}

void AdvFind_Busy::finished()
{
    accept();
}

void AdvFind_Busy::advFind_getResults( )
{
    // part 1
    QStringList searchList;
    QDir currentDir;

    if ( Overlord::getInstance()->advancedFSearchFolders() )
    {
        m_recursiveList.clear();

        this->findRecursive( Overlord::getInstance()->advancedFindFolder() );
        searchList = m_recursiveList;

    }
    else
    {
        currentDir = QDir( Overlord::getInstance()->advancedFindFolder() );
        searchList = currentDir.entryList( QStringList( Overlord::getInstance()->advancedFindFileType() ),
                                           QDir::Files | QDir::NoSymLinks );
    }

    // part 2
    QString name;

    enum Qt::CaseSensitivity caseFlag;
    QRegularExpression regExp = QRegularExpression( "\\b" + Overlord::getInstance()->advancedFindText() + "\\b" );

    if ( Overlord::getInstance()->advancedFCase() )
    {
        caseFlag = Qt::CaseSensitive;

    }
    else
    {
        caseFlag = Qt::CaseInsensitive;
        regExp.setPatternOptions( QPatternOption::CaseInsensitiveOption );

    }

    // process each file
    for ( int k = 0; k < searchList.size(); ++k )
    {

        setProgressLabelText( tr( "Searching file %1 of %2" ).formatArg( k ).formatArg( searchList.size() ) );

        if ( Overlord::getInstance()->advancedFSearchFolders() )
        {
            name = searchList[k];

        }
        else
        {
            name = currentDir.absoluteFilePath( searchList[k] );

        }

#if defined (Q_OS_WIN)
        // change forward to backslash
        name.replace( '/', '\\' );
#endif

        QFile file( name );

        if ( file.open( QIODevice::ReadOnly ) )
        {
            QString line;
            QTextStream in( &file );

            int lineNumber = 0;
            int position   = 0;

            while ( ! in.atEnd() )
            {

                line = in.readLine();
                lineNumber++;

                if ( Overlord::getInstance()->advancedFWholeWords() )
                {
                    position = line.indexOf( regExp );

                }
                else
                {
                    position = line.indexOf( Overlord::getInstance()->advancedFindText(), 0, caseFlag );

                }

                // store the results
                if ( position != -1 )
                {
                    AdvFindStruct temp;

                    temp.fileName   = name;
                    temp.lineNumber = lineNumber;
                    temp.text       = line.trimmed();

                    m_foundList.append( temp );
                }
            }
        }

        file.close();
    }
}

void AdvFind_Busy::findRecursive( const QString &path, bool isFirstLoop )
{
    QDir dir( path );
    dir.setFilter( QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks );

    QFileInfoList list = dir.entryInfoList( QStringList( Overlord::getInstance()->advancedFindFileType() ) );
    int cnt = list.count();

    qDebug() << "isFirstLoop: " << isFirstLoop << "  cnt: " << cnt << "\n";

    if ( ! list.isEmpty() )
    {
        for ( int k = 0; k != cnt; ++k )
        {

            QString filePath = list[k].filePath();

            if ( list[k].isDir() )
            {
                // recursive
                findRecursive( filePath, false );

            }
            else
            {
                m_recursiveList.append( filePath );

            }
        }
    }
}

