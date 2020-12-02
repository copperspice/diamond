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
#ifndef DIALOGEDTPROMPT_H
#define DIALOGEDTPROMPT_H

#include "edt_lineedit.h"

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>


class Dialog_Edt_Prompt : public QDialog
{
    CS_OBJECT( Dialog_Edt_Prompt )

public:
    Dialog_Edt_Prompt( QString labelText, bool allowDirection, QWidget *parent = nullptr );

    int terminator()
    {
        return m_lineEdit->terminator();
    }
    bool ctrlMSubstitution()
    {
        return m_lineEdit->ctrlMSubstitution();
    }
    bool allowDirection()
    {
        return m_lineEdit->allowDirection();
    }
    QString text()
    {
        return m_lineEdit->text();
    }

    void setLabelPalette( QPalette &p )
    {
        m_label->setPalette( p );
    }
    void setEditPalette( QPalette &p )
    {
        m_lineEdit->setPalette( p );
    }
    void setText( QString txt )
    {
        m_lineEdit->setText( txt );
    }

protected:
    void showEvent( QShowEvent *e );

private:
    Edt_LineEdit *m_lineEdit;
    QLabel       *m_label;
};

#endif
