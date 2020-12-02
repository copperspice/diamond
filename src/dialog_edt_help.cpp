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

#include "dialog_edt_help.h"
#include <QFile>

Dialog_Edt_Help::Dialog_Edt_Help( QWidget *parent )
    : QDialog( parent )
    , m_ui( new Ui::Dialog_Edt_Help )
{
    m_ui->setupUi( this );
    QFile txt( "://resources/EDT-Help.html" );
    txt.open( QFile::ReadOnly );
    QTextStream stream( &txt );
    QString str( stream.readAll() );
    m_ui->textBrowser->setHtml( str );
    txt.close();
    showMaximized();
}

Dialog_Edt_Help::~Dialog_Edt_Help()
{
    delete m_ui;
}

