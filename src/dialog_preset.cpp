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

#include "dialog_preset.h"
#include "util.h"
#include "overlord.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QPalette>

Dialog_Preset::Dialog_Preset( QWidget *parent )
    : QDialog( parent ), m_ui( new Ui::Dialog_Preset )
{
    m_dataList = Overlord::getInstance()->presetFolders();

    m_ui->setupUi( this );
    this->setWindowIcon( QIcon( "://resources/diamond.png" ) );

    for ( int k = 0; k < m_dataList.count(); k++ )
    {
        QString temp = m_dataList.at( k );
        int index = k + 1;

        switch ( index )
        {
            case 1:
                m_ui->folder1->setText( temp );
                m_ui->folder1->setCursorPosition( 0 );
                break;

            case 2:
                m_ui->folder2->setText( temp );
                m_ui->folder2->setCursorPosition( 0 );
                break;

            case 3:
                m_ui->folder3->setText( temp );
                m_ui->folder3->setCursorPosition( 0 );
                break;

            case 4:
                m_ui->folder4->setText( temp );
                m_ui->folder4->setCursorPosition( 0 );
                break;

            case 5:
                m_ui->folder5->setText( temp );
                m_ui->folder5->setCursorPosition( 0 );
                break;

            case 6:
                m_ui->folder6->setText( temp );
                m_ui->folder6->setCursorPosition( 0 );
                break;

            case 7:
                m_ui->folder7->setText( temp );
                m_ui->folder7->setCursorPosition( 0 );
                break;

            case 8:
                m_ui->folder8->setText( temp );
                m_ui->folder8->setCursorPosition( 0 );
                break;

            case 9:
                m_ui->folder9->setText( temp );
                m_ui->folder9->setCursorPosition( 0 );
                break;

            case 10:
                m_ui->folder10->setText( temp );
                m_ui->folder10->setCursorPosition( 0 );
                break;

            case 11:
                m_ui->folder11->setText( temp );
                m_ui->folder11->setCursorPosition( 0 );
                break;

            case 12:
                m_ui->folder12->setText( temp );
                m_ui->folder12->setCursorPosition( 0 );
                break;

            case 13:
                m_ui->folder13->setText( temp );
                m_ui->folder13->setCursorPosition( 0 );
                break;

            case 14:
                m_ui->folder14->setText( temp );
                m_ui->folder14->setCursorPosition( 0 );
                break;

            case 15:
                m_ui->folder15->setText( temp );
                m_ui->folder15->setCursorPosition( 0 );
                break;

            default:
                break;
        }
    }

    // save for later
    m_editPalette = m_ui->folder1->palette();

    connect( m_ui->folder1,     &QLineEdit::editingFinished, this, [this]()
    {
        verify_Folder( m_ui->folder1 );
    } );
    connect( m_ui->folder2,     &QLineEdit::editingFinished, this, [this]()
    {
        verify_Folder( m_ui->folder2 );
    } );
    connect( m_ui->folder3,     &QLineEdit::editingFinished, this, [this]()
    {
        verify_Folder( m_ui->folder3 );
    } );
    connect( m_ui->folder4,     &QLineEdit::editingFinished, this, [this]()
    {
        verify_Folder( m_ui->folder4 );
    } );
    connect( m_ui->folder5,     &QLineEdit::editingFinished, this, [this]()
    {
        verify_Folder( m_ui->folder5 );
    } );
    connect( m_ui->folder6,     &QLineEdit::editingFinished, this, [this]()
    {
        verify_Folder( m_ui->folder6 );
    } );
    connect( m_ui->folder7,     &QLineEdit::editingFinished, this, [this]()
    {
        verify_Folder( m_ui->folder7 );
    } );
    connect( m_ui->folder8,     &QLineEdit::editingFinished, this, [this]()
    {
        verify_Folder( m_ui->folder8 );
    } );
    connect( m_ui->folder9,     &QLineEdit::editingFinished, this, [this]()
    {
        verify_Folder( m_ui->folder9 );
    } );
    connect( m_ui->folder10,    &QLineEdit::editingFinished, this, [this]()
    {
        verify_Folder( m_ui->folder10 );
    } );
    connect( m_ui->folder11,    &QLineEdit::editingFinished, this, [this]()
    {
        verify_Folder( m_ui->folder11 );
    } );
    connect( m_ui->folder12,    &QLineEdit::editingFinished, this, [this]()
    {
        verify_Folder( m_ui->folder12 );
    } );
    connect( m_ui->folder13,    &QLineEdit::editingFinished, this, [this]()
    {
        verify_Folder( m_ui->folder13 );
    } );
    connect( m_ui->folder14,    &QLineEdit::editingFinished, this, [this]()
    {
        verify_Folder( m_ui->folder14 );
    } );
    connect( m_ui->folder15,    &QLineEdit::editingFinished, this, [this]()
    {
        verify_Folder( m_ui->folder15 );
    } );

    connect( m_ui->folder1_TB,  &QPushButton::clicked, this, [this]()
    {
        pick_Folder( "1",  m_ui->folder1 );
    } );
    connect( m_ui->folder2_TB,  &QPushButton::clicked, this, [this]()
    {
        pick_Folder( "2",  m_ui->folder2 );
    } );
    connect( m_ui->folder3_TB,  &QPushButton::clicked, this, [this]()
    {
        pick_Folder( "3",  m_ui->folder3 );
    } );
    connect( m_ui->folder4_TB,  &QPushButton::clicked, this, [this]()
    {
        pick_Folder( "4",  m_ui->folder4 );
    } );
    connect( m_ui->folder5_TB,  &QPushButton::clicked, this, [this]()
    {
        pick_Folder( "5",  m_ui->folder5 );
    } );
    connect( m_ui->folder6_TB,  &QPushButton::clicked, this, [this]()
    {
        pick_Folder( "6",  m_ui->folder6 );
    } );
    connect( m_ui->folder7_TB,  &QPushButton::clicked, this, [this]()
    {
        pick_Folder( "7",  m_ui->folder7 );
    } );
    connect( m_ui->folder8_TB,  &QPushButton::clicked, this, [this]()
    {
        pick_Folder( "8",  m_ui->folder8 );
    } );
    connect( m_ui->folder9_TB,  &QPushButton::clicked, this, [this]()
    {
        pick_Folder( "9",  m_ui->folder9 );
    } );
    connect( m_ui->folder10_TB, &QPushButton::clicked, this, [this]()
    {
        pick_Folder( "10", m_ui->folder10 );
    } );
    connect( m_ui->folder11_TB, &QPushButton::clicked, this, [this]()
    {
        pick_Folder( "11", m_ui->folder11 );
    } );
    connect( m_ui->folder12_TB, &QPushButton::clicked, this, [this]()
    {
        pick_Folder( "12", m_ui->folder12 );
    } );
    connect( m_ui->folder13_TB, &QPushButton::clicked, this, [this]()
    {
        pick_Folder( "13", m_ui->folder13 );
    } );
    connect( m_ui->folder14_TB, &QPushButton::clicked, this, [this]()
    {
        pick_Folder( "14", m_ui->folder14 );
    } );
    connect( m_ui->folder15_TB, &QPushButton::clicked, this, [this]()
    {
        pick_Folder( "15", m_ui->folder15 );
    } );

    connect( m_ui->save_PB,     &QPushButton::clicked, this, &Dialog_Preset::save );
    connect( m_ui->cancel_PB,   &QPushButton::clicked, this, &Dialog_Preset::cancel );
}

Dialog_Preset::~Dialog_Preset()
{
    delete m_ui;
}

void Dialog_Preset::verify_Folder( QLineEdit *field )
{
    QString folder = field->text();

    if ( ! folder.isEmpty() )
    {
        bool ok = QFileInfo( folder ).isDir();

        if ( ok )
        {
            // set back to the normal text color
            field->setPalette( m_editPalette );

        }
        else
        {
            // change the text to red
            QPalette temp = field->palette();
            temp.setColor( QPalette::Text, Qt::red );
            field->setPalette( temp );
        }
    }
}

void Dialog_Preset::pick_Folder( QString number, QLineEdit *field )
{
    QString title     = tr( "Select Folder " ) + number;
    QString oldFolder = field->text();

    QString newFolder = get_DirPath( this, title, oldFolder );

    if ( ! newFolder.isEmpty() )
    {
        field->setText( newFolder );

        // no reason to test, just reset the text color
        field->setPalette( m_editPalette );
    }
}

void Dialog_Preset::save()
{
    Overlord::getInstance()->set_preFolderList( getData() );
    this->done( QDialog::Accepted );
}

void Dialog_Preset::cancel()
{
    this->done( QDialog::Rejected );
}

QStringList Dialog_Preset::getData()
{
    for ( int k = 0; k < m_dataList.count(); k++ )
    {
        int index = k + 1;

        switch ( index )
        {
            case 1:
                m_dataList.replace( k, m_ui->folder1->text() );
                break;

            case 2:
                m_dataList.replace( k, m_ui->folder2->text() );
                break;

            case 3:
                m_dataList.replace( k, m_ui->folder3->text() );
                break;

            case 4:
                m_dataList.replace( k, m_ui->folder4->text() );
                break;

            case 5:
                m_dataList.replace( k, m_ui->folder5->text() );
                break;

            case 6:
                m_dataList.replace( k, m_ui->folder6->text() );
                break;

            case 7:
                m_dataList.replace( k, m_ui->folder7->text() );
                break;

            case 8:
                m_dataList.replace( k, m_ui->folder8->text() );
                break;

            case 9:
                m_dataList.replace( k, m_ui->folder9->text() );
                break;

            case 10:
                m_dataList.replace( k, m_ui->folder10->text() );
                break;

            case 11:
                m_dataList.replace( k, m_ui->folder11->text() );
                break;

            case 12:
                m_dataList.replace( k, m_ui->folder12->text() );
                break;

            case 13:
                m_dataList.replace( k, m_ui->folder13->text() );
                break;

            case 14:
                m_dataList.replace( k, m_ui->folder14->text() );
                break;

            case 15:
                m_dataList.replace( k, m_ui->folder15->text() );
                break;

            default:
                break;
        }
    }

    return m_dataList;
}

