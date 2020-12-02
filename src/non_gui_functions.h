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

/**************************************************************************
*
* Collection of non-gui functions that are stand alone and should not be
* contained in a class.
*
***************************************************************************/
#include "syntax.h"
#include "settings.h"

QString pathName( QString fileName );
QString suffixName( QString curFile );
QString strippedName( const QString fileName );
QFont fontFromString( QString value );
QColor colorFromValueString( QString values );
QString getRGBString( QColor color );
bool doubleEqualFourDigits( double left, double right );
bool validAstyleSuffix( QString suffix );
