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

#ifndef DIALOG_COLORS_H
#define DIALOG_COLORS_H

#include "ui_dialog_colors.h"
#include "overlord.h"

#include <QDialog>
#include <QStringList>

class Dialog_Colors : public QDialog
{
    CS_OBJECT( Dialog_Colors )

public:
    Dialog_Colors( QWidget *parent );
    ~Dialog_Colors();

//    CS_SIGNAL_1( Public, void changeSettings( Settings *settings ) )
//    CS_SIGNAL_2( changeSettings, settings )

private:
    Ui::Dialog_Colors *m_ui;

    Settings m_localSettings;
    QString m_syntaxFname;

    void colorBox( QLineEdit *field, QColor color );
    void rebuildComboBox( bool needToDisconnect );
    QColor pickColor( QColor oldColor );

    bool nameCollision( QString &themeName, bool &abortFlag );

    void text_TB();
    void back_TB();
    void gutterText_TB();
    void gutterBack_TB();
    void current_TB();
    void key_TB();
    void type_TB();
    void class_TB();
    void func_TB();
    void quote_TB();
    void comment_TB();
    void mline_TB();
    void const_TB();
    void key_bold();
    void key_italic();
    void type_bold();
    void type_italic();
    void class_bold();
    void class_italic();
    void func_bold();
    void func_italic();
    void quote_bold();
    void quote_italic();
    void comment_bold();
    void comment_italic();
    void mline_bold();
    void mline_italic();

    void save();
    void cancel();

    void themeChanged( const QString &themeName );

    void copyClicked();
    void deleteClicked();
    void exportClicked();
    void importClicked();
};

#endif
