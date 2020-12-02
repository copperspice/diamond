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

#ifndef DIALOG_OPTIONS_H
#define DIALOG_OPTIONS_H

#include "ui_dialog_options.h"
#include "options.h"

#include <QDialog>
#include <QStringList>

class Dialog_Options : public QDialog
{
    CS_OBJECT( Dialog_Options )

public:
    Dialog_Options( QWidget *parent );
    ~Dialog_Options();

    void show_help();

private:
    Ui::Dialog_Options *m_ui;

    Options m_options;
    void initData();

    void pick_Main();
    void pick_User();
    void pick_Syntax();
    void pick_BackupDirectory();

    void reset_StandardKey();
    void save();
    void cancel();
};

#endif
