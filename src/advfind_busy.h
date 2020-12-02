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

#ifndef ADVFIND_BUSY_H
#define ADVFIND_BUSY_H

#include "dialog_busy.h"
#include <QList>
#include <QStringList>
#include "advfindstruct.h"
#include <QFutureWatcher>

class AdvFind_Busy : public Dialog_Busy
{
    CS_OBJECT( AdvFind_Busy )

public:
    AdvFind_Busy( QWidget *parent=0 );
    ~AdvFind_Busy();

    int exec();
    void reject();

    QList<AdvFindStruct> m_foundList;

protected:
    CS_SLOT_1( Private, void finished() )
    CS_SLOT_2( finished )

    void advFind_getResults();
    void findRecursive( const QString &path, bool isFirstLoop = true );

private:
    QFutureWatcher<void> m_watcher;
    QStringList m_recursiveList;
};

#endif
