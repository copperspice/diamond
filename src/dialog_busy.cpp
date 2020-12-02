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

#include "dialog_busy.h"

#include <QPixmap>

Dialog_Busy::Dialog_Busy( QWidget *parent )
    : QDialog( parent )
    , m_ui( new Ui::Dialog_Busy )
    , m_currentImage( 0 )
{
    m_ui->setupUi( this );

    setWindowFlags( Qt::FramelessWindowHint | Qt::Dialog );
    m_ui->movieLabel->setAttribute( Qt::WA_NoSystemBackground );

    m_images.append( new QPixmap( "://resources/red-diamond-frames/red_diamond_frame_00.gif" ) );
    m_images.append( new QPixmap( "://resources/red-diamond-frames/red_diamond_frame_01.gif" ) );
    m_images.append( new QPixmap( "://resources/red-diamond-frames/red_diamond_frame_02.gif" ) );
    m_images.append( new QPixmap( "://resources/red-diamond-frames/red_diamond_frame_03.gif" ) );
    m_images.append( new QPixmap( "://resources/red-diamond-frames/red_diamond_frame_04.gif" ) );
    m_images.append( new QPixmap( "://resources/red-diamond-frames/red_diamond_frame_05.gif" ) );
    m_images.append( new QPixmap( "://resources/red-diamond-frames/red_diamond_frame_06.gif" ) );
    m_images.append( new QPixmap( "://resources/red-diamond-frames/red_diamond_frame_07.gif" ) );
    m_images.append( new QPixmap( "://resources/red-diamond-frames/red_diamond_frame_08.gif" ) );
    //m_images.append( new QPixmap( "://resources/red-diamond-frames/red_diamond_frame_09.gif" ) );
    //m_images.append( new QPixmap( "://resources/red-diamond-frames/red_diamond_frame_10.gif" ) );
    //m_images.append( new QPixmap( "://resources/red-diamond-frames/red_diamond_frame_11.gif" ) );
    //m_images.append( new QPixmap( "://resources/red-diamond-frames/red_diamond_frame_12.gif" ) );
    //m_images.append( new QPixmap( "://resources/red-diamond-frames/red_diamond_frame_13.gif" ) );
    //m_images.append( new QPixmap( "://resources/red-diamond-frames/red_diamond_frame_14.gif" ) );

    m_ui->movieLabel->setPixmap( *m_images[0] );
    m_timer.start( 90 );

    connect( &m_timer, &QTimer::timeout, this, &Dialog_Busy::nextImage );
}

Dialog_Busy::~Dialog_Busy()
{
    m_timer.stop();
    delete m_ui;

    for ( QPixmap *ptr : m_images )
    {
        delete ptr;
    }

    m_images.clear();
}

void Dialog_Busy::nextImage()
{
    m_currentImage++;

    if ( !( m_currentImage < m_images.count() ) )
    {
        m_currentImage = 0;
    }

    m_ui->movieLabel->setPixmap( *m_images[m_currentImage] );
}

void Dialog_Busy::setProgressLabelVisible( bool yesNo )
{
    m_ui->progressLabel->setVisible( yesNo );
}

void Dialog_Busy::setCancelButtonVisible( bool yesNo )
{
    m_ui->cancelButton->setVisible( yesNo );
}

void Dialog_Busy::setProgressLabelText( QString text )
{
    m_ui->progressLabel->setText( text );
}
