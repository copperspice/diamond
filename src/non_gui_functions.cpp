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

#include "non_gui_functions.h"
//#include <QString>
#include <QFileInfo>
//#include <QFont>
//#include <QColor>
//#include <QStringList>

QString pathName( QString fileName )
{
    QString retval = "";

    if ( ! fileName.isEmpty() )
    {
        QFileInfo temp( fileName );

        if ( temp.isAbsolute() )
        {
            retval = temp.absolutePath();
        }
    }

    return retval;
}

QString strippedName( const QString fileName )
{
    return QFileInfo( fileName ).fileName();
}

QString suffixName( QString curFile )
{
    return QFileInfo( curFile ).suffix().toLower();
}

QFont fontFromString( QString value )
{
    QFont temp;
    temp.fromString( value );

    return temp;
}

QColor colorFromValueString( QString values )
{
    QStringList list = values.split( "," );

    int red   = 255;
    int green = 255;
    int blue  = 255;

    if ( list.count() > 2 )
    {
        red   = list[0].toInteger<int>();
        green = list[1].toInteger<int>();
        blue  = list[2].toInteger<int>();
    }

    QColor color( red,green,blue );

    return color;
}

QString getRGBString( QColor color )
{
    int red   = color.red();
    int green = color.green();
    int blue  = color.blue();

    QStringList list;
    list.append( QString::number( red ) );
    list.append( QString::number( green ) );
    list.append( QString::number( blue ) );

    QString values = list.join( "," );

    return values;
}

bool doubleEqualFourDigits( double left, double right )
{
    long long ll_left = left * 10000;
    long long ll_right = right * 10000;

    return ( ll_left == ll_right );
}

bool validAstyleSuffix( QString suffix )
{
    bool retVal = false;

    const QList<QString> cppList = {"h", "hh", "hpp", "c", "cc", "c++", "cpp", "l", "m", "mm" };

    if ( cppList.contains( suffix.toLower() ) )
    {
        retVal = true;
    }

    return retVal;

}