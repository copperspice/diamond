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

#ifndef DIALOG_MACRO_H
#define DIALOG_MACRO_H

#include "ui_dialog_macro.h"
#include "mainwindow.h"

#include <QDialog>
#include <QModelIndex>
#include <QStringList>
#include <QStandardItemModel>

class Dialog_Macro : public QDialog
{
    CS_OBJECT( Dialog_Macro )

public:
    enum MacroEnum { MACRO_LOAD, MACRO_MANAGE };

    Dialog_Macro( QWidget *parent, MacroEnum enumValue );
    ~Dialog_Macro();
    QString get_Macro();

    static const int ORIGINAL_ROLE = Qt::UserRole + 4;

private:
    Ui::Dialog_Macro *m_ui;
    QStandardItemModel *m_model;

    bool m_updateNames = false;

    void setupTitle();
    void setUpView();

    MacroEnum m_enum;

    void processEdits();
    void view();
    void cancel();
    void tableDataChanged( const QModelIndex &topLeft,const QModelIndex &bottomRight );
};

#endif
