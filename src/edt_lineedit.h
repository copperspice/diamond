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

#ifndef EDTLINEEDIT_H
#define EDTLINEEDIT_H

#include <QLineEdit>

class Edt_LineEdit : public QLineEdit
{
    CS_OBJECT( Edt_LineEdit )

public:
    static const int NONE = 0;
    static const int ADVANCE = 1;
    static const int BACKUP = 2;
    static const int ENTER = 3;

    Edt_LineEdit( QWidget *parent=nullptr );

    void set_allowDirection( bool yesNo )
    {
        m_allowDirection = yesNo;
    }

    int terminator()
    {
        return m_terminator;
    }
    bool ctrlMSubstitution()
    {
        return m_ctrlMSubstitution;
    }
    bool allowDirection()
    {
        return m_allowDirection;
    }

    CS_SIGNAL_1( Public, void inputComplete() )
    CS_SIGNAL_2( inputComplete )

protected:
    bool event( QEvent *event );

private:
    int m_terminator;
    bool m_ctrlMSubstitution;
    bool m_allowDirection;

};

#endif
