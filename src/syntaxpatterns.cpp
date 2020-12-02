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

#include "syntaxpatterns.h"
#include "util.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

SyntaxPatterns::SyntaxPatterns( QString fileName ) :
    pattern_file( fileName )
{
    QByteArray data = readSyntaxFile();

    if ( data.isEmpty() )
    {
        csError( "SyntaxPatters", QString( "Failed to read pattern file: %1" ).formatArg( fileName ) );
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson( data );

    QJsonObject object = doc.object();
    QJsonArray list;
    int cnt;

    //
    ignoreCase = object.value( "ignore-case" ).toBool();

    // * key
    list = object.value( "keywords" ).toArray();
    cnt  = list.count();

    for ( int k = 0; k < cnt; k++ )
    {
        key_Patterns.append( list.at( k ).toString() );
    }

    // class
    list = object.value( "classes" ).toArray();
    cnt  = list.count();

    for ( int k = 0; k < cnt; k++ )
    {
        class_Patterns.append( list.at( k ).toString() );
    }

    // functions
    list = object.value( "functions" ).toArray();
    cnt  = list.count();

    for ( int k = 0; k < cnt; k++ )
    {
        func_Patterns.append( list.at( k ).toString() );
    }

    // types
    list = object.value( "types" ).toArray();
    cnt  = list.count();

    for ( int k = 0; k < cnt; k++ )
    {
        type_Patterns.append( list.at( k ).toString() );
    }

    // constants
    list = object.value( "constants" ).toArray();
    cnt  = list.count();

    for ( int k = 0; k < cnt; k++ )
    {
        constant_Patterns.append( list.at( k ).toString() );
    }


    // single line comment
    commentSingle = object.value( "comment-single" ).toString();

    // multi line comment
    commentStart = object.value( "comment-multi-start" ).toString();
    commentEnd   = object.value( "comment-multi-end" ).toString();

}

SyntaxPatterns::SyntaxPatterns( const SyntaxPatterns &patterns ) :
    key_Patterns( patterns.key_Patterns )
    , class_Patterns( patterns.class_Patterns )
    , func_Patterns( patterns.func_Patterns )
    , type_Patterns( patterns.type_Patterns )
    , constant_Patterns( patterns.constant_Patterns )
    , commentSingle( patterns.commentSingle )
    , commentStart( patterns.commentStart )
    , commentEnd( patterns.commentEnd )
    , pattern_file( patterns.pattern_file )
    , ignoreCase( patterns.ignoreCase )
{
}

SyntaxPatterns &SyntaxPatterns::operator = ( const SyntaxPatterns &other )
{
    pattern_file        = other.pattern_file;
    key_Patterns        = other.key_Patterns;
    class_Patterns      = other.class_Patterns;
    func_Patterns       = other.func_Patterns;
    type_Patterns       = other.type_Patterns;
    constant_Patterns   = other.constant_Patterns;
    ignoreCase          = other.ignoreCase;
    commentSingle       = other.commentSingle;
    commentStart        = other.commentStart;
    commentEnd          = other.commentEnd;

    return *this;
}



QByteArray SyntaxPatterns::readSyntaxFile()
{
    QByteArray data;

    if ( pattern_file.isEmpty() )
    {
        csError( tr( "Read Syntax File" ), tr( "Syntax file name was not supplied." ) );
        return data;
    }

    if ( ! QFile::exists( pattern_file ) )
    {
        csError( tr( "Read Syntax File" ), tr( "Syntax file was not found: " ) + pattern_file + "\n\n"
                 "To specify the location of the syntax files select 'Settings' from the main Menu. "
                 "Then select 'General Options' and click on the Options tab.\n" );

        return data;
    }

    QFile file( pattern_file );

    if ( ! file.open( QFile::ReadOnly | QFile::Text ) )
    {
        const QString msg = tr( "Unable to open Syntax file: " ) +  pattern_file + " : "
                            + file.errorString();
        csError( tr( "Read Syntax File" ), msg );
        return data;
    }

    file.seek( 0 );
    data = file.readAll();
    file.close();

    return data;
}
