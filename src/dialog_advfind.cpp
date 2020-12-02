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

#include "dialog_advfind.h"
#include "util.h"

#include <QDir>
#include <QFileDialog>
#include <QToolButton>
#include <QMovie>

QStringList Dialog_AdvFind::dirCombo;

Dialog_AdvFind::Dialog_AdvFind( QWidget *parent, QString findText, QString fileType, QString findFolder, bool searchFolders )
    : QDialog( parent )
    , m_ui( new Ui::Dialog_AdvFind )
{
    m_ui->setupUi( this );
    setWindowIcon( QIcon( "://resources/diamond.png" ) );

    m_ui->find->setText( findText );
    m_ui->findType->setText( fileType );

    // pre load
    m_ui->findFolder->insertItems( 0, dirCombo );
    m_ui->findFolder->setEditText( findFolder );

    if ( searchFolders )
    {
        m_ui->searchSubFolders_CKB->setChecked( true );
    }

    connect( m_ui->folder_TB, &QToolButton::clicked, this, &Dialog_AdvFind::pick_Folder );
    connect( m_ui->find_PB,   &QPushButton::clicked, this, &Dialog_AdvFind::find );
    connect( m_ui->cancel_PB, &QPushButton::clicked, this, &Dialog_AdvFind::cancel );
}

Dialog_AdvFind::~Dialog_AdvFind()
{
    delete m_ui;
}

void Dialog_AdvFind::pick_Folder()
{
    QString oldFolder = m_ui->findFolder->currentText();

    if ( oldFolder.isEmpty() )
    {
        oldFolder = QDir::currentPath();
    }

    QString newFolder = get_DirPath( this, "Select Directory or Folder to Search", oldFolder );

    if ( ! newFolder.isEmpty() )
    {

#if defined (Q_OS_WIN)
        // change forward to backslash
        newFolder.replace( '/', '\\' );
#endif

        m_ui->findFolder->setEditText( newFolder );
    }
}

void Dialog_AdvFind::cancel()
{
    this->done( 0 );
}

void Dialog_AdvFind::find()
{
    QString tmp = m_ui->findFolder->currentText();

    if ( ! dirCombo.contains( tmp ) )
    {
        dirCombo.append( tmp );
    }

    this->done( 1 );
}

QString Dialog_AdvFind::get_findText()
{
    return m_ui->find->text();
}

QString Dialog_AdvFind::get_findType()
{
    return m_ui->findType->text();
}

QString Dialog_AdvFind::get_findFolder()
{
    return m_ui->findFolder->currentText();
}

bool Dialog_AdvFind::get_Case()
{
    return m_ui->case_CKB->isChecked();
}

bool Dialog_AdvFind::get_WholeWords()
{
    return m_ui->wholeWords_CKB->isChecked();
}

bool Dialog_AdvFind::get_SearchSubFolders()
{
    return m_ui->searchSubFolders_CKB->isChecked();
}

