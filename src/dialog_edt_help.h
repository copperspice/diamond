/**************************************************************************
*
*Copyright ( c ) 2020 Roland Hughes
*
*Diamond Editor is free software:
you can redistribute it and/or
*modify it under the terms of the GNU General Public License version 2
*as published by the Free Software Foundation.
*
*Diamond is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY;
without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
***************************************************************************/
#ifndef DIALOGEDTHELP_H
#define DIALOGEDTHELP_H

#include "ui_dialog_edt_help.h"

#include <QDialog>

class Dialog_Edt_Help : public QDialog
{
    CS_OBJECT( Dialog_Edt_Help )

public:
    Dialog_Edt_Help( QWidget *parent=nullptr );
    ~Dialog_Edt_Help();

private:
    Ui::Dialog_Edt_Help *m_ui;
};

#endif
