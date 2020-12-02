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

#include "util.h"
#include "dialog_getline.h"

#ifdef Q_OS_WIN
#include "dialog_xp_getdir.h"
#endif

#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <QFileDialog>
#include <QFileInfo>

void csMsg( const QString &msg )
{
    QMessageBox msgB;
    msgB.setTextFormat( Qt::PlainText );

    msgB.setWindowTitle( QApplication::translate( "csMsg", "General Information" ) );
    msgB.setWindowIcon( QIcon( "://resources/diamond.png" ) );

    msgB.setText( msg );
    msgB.exec();
}

void csMsg( QString msg, int value )
{
    msg = msg + "   " + QString::number( value );

    QMessageBox msgB;

    msgB.setWindowTitle( QApplication::translate( "csMsg", "General Information" ) );
    msgB.setWindowIcon( QIcon( "://resources/diamond.png" ) );

    msgB.setText( msg );
    msgB.exec();
}

void csMsg( QWidget *parent, const QString &title, const QString &msg )
{
    QMessageBox msgB;

    if ( parent )
    {
        msgB.setWindowModality( Qt::WindowModal );
    }

    msgB.setWindowTitle( title );
    msgB.setWindowIcon( QIcon( "://resources/diamond.png" ) );

    msgB.setText( msg );
    msgB.exec();
}

void csError( QString title, QString msg )
{
    QMessageBox msgB;

    msgB.setWindowTitle( title );
    msgB.setWindowIcon( QIcon( "://resources/diamond.png" ) );

    msgB.setIcon( QMessageBox::Warning );
    msgB.setText( msg );
    msgB.exec();
}

void showDialog( QDialog *t_Dialog )
{
    t_Dialog->show();
    t_Dialog->raise();
    t_Dialog->activateWindow();
}

QString get_DirPath( QWidget *parent, QString message, QString path )
{
    QFileDialog::Options options;
    options |= QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks;

    QString retval;


#ifdef Q_OS_WIN

    if ( QSysInfo::WindowsVersion < QSysInfo::WV_VISTA )
    {

        Dialog_XP_GetDir *dw = new Dialog_XP_GetDir( parent, message, path, options );
        int result = dw->exec();

        if ( result == QDialog::Accepted )
        {
            path = dw->getDirectory();
        }

    }
    else
    {
        path = QFileDialog::getExistingDirectory( parent, message, path, options );

    }

#else
    // on X11 the title bar may not be displayed
    path = QFileDialog::getExistingDirectory( parent, message, path, options );

#endif

    if ( ! path.isEmpty() )
    {
        // silly adjust for platform slash issue

        QDir temp( path + "/" );
        retval = temp.canonicalPath() + "/";
    }

    return retval;
}

int get_line_col( QWidget *parent, const QString route )
{
    Dialog_GetLine *dw = new Dialog_GetLine( parent );

    if ( route == "col" )
    {
        dw->set_ColNo();
    }

    int result = dw->exec();
    int col    = -1;

    if ( result == QDialog::Accepted )
    {
        col = dw->get_Value().toInteger<int>();
    }

    delete dw;

    return col;
}
