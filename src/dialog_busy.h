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

#ifndef DIALOG_BUSY_H
#define DIALOG_BUSY_H

#include "ui_dialog_busy.h"

#include <QDialog>
#include <QList>
#include <QTimer>

class QPixmap;

class Dialog_Busy : public QDialog
{
    CS_OBJECT( Dialog_Busy )

public:
    Dialog_Busy( QWidget *parent );
    ~Dialog_Busy();

    void setProgressLabelVisible( bool yesNo );
    void setCancelButtonVisible( bool yesNo );
    void setProgressLabelText( QString text );

protected:
    void nextImage();

private:
    Ui::Dialog_Busy *m_ui;
    QList<QPixmap *> m_images;
    int m_currentImage;
    QTimer m_timer;
};

#endif
