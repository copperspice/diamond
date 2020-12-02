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

#include "dialog_edt_prompt.h"
#include "overlord.h"
#include <QStyle>
#include <QPoint>

Dialog_Edt_Prompt::Dialog_Edt_Prompt( QString labelText, bool allowDirection, QWidget *parent ) :
    QDialog( parent )
    , m_lineEdit( nullptr )
    , m_label( nullptr )
{

    Qt::WindowFlags flags = this->windowFlags();
    // Qt::CustomizeWindowHint
    // Qt::FramelessWindowHint
    setWindowFlags( flags | Qt::FramelessWindowHint );

    setSizeGripEnabled( false );
    setModal( false );

    QPalette temp = palette();
    temp.setColor( this->foregroundRole(), Overlord::getInstance()->currentTheme()->colorText() );
    temp.setColor( this->backgroundRole(), Overlord::getInstance()->currentTheme()->colorBack() );
    setPalette( temp );

    m_label = new QLabel( labelText, this );
    m_lineEdit = new Edt_LineEdit( this );
    m_lineEdit->set_allowDirection( allowDirection );

    temp = m_label->palette();
    temp.setColor( QPalette::Text, Overlord::getInstance()->currentTheme()->syntaxConstant().color() );

    connect( m_lineEdit, &Edt_LineEdit::inputComplete, this, &QDialog::accept );

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget( m_label );
    layout->addWidget( m_lineEdit );
    setLayout( layout );

    if ( parentWidget() )
    {
        qDebug() << "parentWidget() exists\n";
        setMinimumWidth( parentWidget()->width() );
    }

}


void Dialog_Edt_Prompt::showEvent( QShowEvent *e )
{
    QDialog::showEvent( e );

    if ( parentWidget() )
    {

        qDebug() << "parentWidget() existed";
        QSize parentSize = parentWidget()->size();
        int w = size().width();
        int h = size().height();
        setGeometry( 0, ( parentSize.height() - h ), w, h );
        QPoint point = parentWidget()->rect().bottomLeft();
        point.rx() += 25;
        point.ry() += 10;
        qDebug() << "mapping to global\n";
        this->move( parentWidget()->mapToGlobal( point ) );
    }
}
