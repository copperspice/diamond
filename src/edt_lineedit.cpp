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

#include "edt_lineedit.h"
#include <QEvent>
#include <QKeyEvent>

Edt_LineEdit::Edt_LineEdit( QWidget *parent ) :
    QLineEdit( parent )
    , m_terminator( NONE )
    , m_ctrlMSubstitution( false )
    , m_allowDirection( false )
{
}

bool Edt_LineEdit::event( QEvent *ev )
{
    bool retVal = false;

    if ( ev->type() == QEvent::KeyPress )
    {
        QKeyEvent *keyPressEvent = dynamic_cast<QKeyEvent *>( ev );

        int key = keyPressEvent->key();
        int modifiers = keyPressEvent->modifiers();
        bool isKeypad = ( modifiers & Qt::KeypadModifier );

        switch ( key )
        {
            case Qt::Key_4:
            case Qt::Key_Left:
                if ( isKeypad  && m_allowDirection )
                {
                    m_terminator = ADVANCE;
                    inputComplete();
                    retVal = true;
                }

                break;

            case Qt::Key_5:
            case Qt::Key_Clear:
                if ( isKeypad && m_allowDirection )
                {
                    m_terminator = BACKUP;
                    inputComplete();
                    retVal = true;
                }

                break;

            case Qt::Key_Enter:
                if ( isKeypad )
                {
                    m_terminator = ENTER;
                    inputComplete();
                    retVal = true;
                }

                break;

            case Qt::Key_Return:
                m_ctrlMSubstitution = true;
                QCoreApplication::sendEvent( this, new QKeyEvent( QEvent::KeyPress, Qt::Key_AsciiCircum, Qt::ShiftModifier, "^" ) );
                QCoreApplication::sendEvent( this, new QKeyEvent( QEvent::KeyPress, Qt::Key_M, Qt::ShiftModifier, "M" ) );
                retVal = true;
                break;

            default:
                break;
        }
    }

    if ( !retVal )
    {
        return QLineEdit::event( ev );
    }
    else
    {
        return retVal;
    }
}

