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
#include "keydefinitions.h"
#include "qglobal.h"
#include <QKeySequence>
#include <QDebug>

KeyDefinitions::KeyDefinitions()
{
    setDefaultKeyValues();
}

KeyDefinitions::KeyDefinitions( const KeyDefinitions &def ) :
    m_close( def.m_close )
    , m_columnMode( def.m_columnMode )
    , m_copy( def.m_copy )
    , m_copyBuffer( def.m_copyBuffer )
    , m_cut( def.m_cut )
    , m_deleteLine( def.m_deleteLine )
    , m_deleteThroughEOL( def.m_deleteThroughEOL )
    , m_deleteToEOL( def.m_deleteToEOL )
    , m_find( def.m_find )
    , m_findNext( def.m_findNext )
    , m_findPrev( def.m_findPrev )
    , m_goBottom( def.m_goBottom )
    , m_goTop( def.m_goTop )
    , m_gotoLine( def.m_gotoLine )
    , m_indentDecrement( def.m_indentDecrement )
    , m_indentIncrement( def.m_indentIncrement )
    , m_lower( def.m_lower )
    , m_macroPlay( def.m_macroPlay )
    , m_newTab( def.m_newTab )
    , m_open( def.m_open )
    , m_paste( def.m_paste )
    , m_print( def.m_print )
    , m_printPreview( def.m_printPreview )
    , m_redo( def.m_redo )
    , m_reload( def.m_reload )
    , m_replace( def.m_replace )
    , m_save( def.m_save )
    , m_saveAs( def.m_saveAs )
    , m_selectAll( def.m_selectAll )
    , m_selectBlock( def.m_selectBlock )
    , m_selectLine( def.m_selectLine )
    , m_selectWord( def.m_selectWord )
    , m_showBreaks( def.m_showBreaks )
    , m_showSpaces( def.m_showSpaces )
    , m_spellCheck( def.m_spellCheck )
    , m_undo( def.m_undo )
    , m_upper( def.m_upper )
    , m_edtEnabled( def.m_edtEnabled )
    , m_numLockGold( def.m_numLockGold )
    , m_scrollLockGold( def.m_scrollLockGold )
    , m_f12AsBackspace( def.m_f12AsBackspace )
    , m_edtGotoLine( def.m_edtGotoLine )
    , m_edtInsertFile( def.m_edtInsertFile )
    , m_edtUpperCase( def.m_edtUpperCase )
    , m_edtLowerCase( def.m_edtLowerCase )
    , m_edtSplitHorizontal( def.m_edtSplitHorizontal )
    , m_edtSplitVertical( def.m_edtSplitVertical )
    , m_edtSaveFile( def.m_edtSaveFile )
    , m_edtAstyle( def.m_edtAstyle )
    , m_edtWordCtrlMeta( def.m_edtWordCtrlMeta )
    , m_edtWordAltOption( def.m_edtWordAltOption )
{
}

KeyDefinitions &KeyDefinitions::operator =( const KeyDefinitions &def )
{
    if ( this != &def )
    {
        m_close                 = def.m_close;
        m_columnMode            = def.m_columnMode;
        m_copy                  = def.m_copy;
        m_copyBuffer            = def.m_copyBuffer;
        m_cut                   = def.m_cut;
        m_deleteLine            = def.m_deleteLine;
        m_deleteThroughEOL      = def.m_deleteThroughEOL;
        m_deleteToEOL           = def.m_deleteToEOL;
        m_find                  = def.m_find;
        m_findNext              = def.m_findNext;
        m_findPrev              = def.m_findPrev;
        m_goBottom              = def.m_goBottom;
        m_goTop                 = def.m_goTop;
        m_gotoLine              = def.m_gotoLine;
        m_indentDecrement       = def.m_indentDecrement;
        m_indentIncrement       = def.m_indentIncrement;
        m_lower                 = def.m_lower;
        m_macroPlay             = def.m_macroPlay;
        m_newTab                = def.m_newTab;
        m_open                  = def.m_open;
        m_paste                 = def.m_paste;
        m_print                 = def.m_print;
        m_printPreview          = def.m_printPreview;
        m_redo                  = def.m_redo;
        m_reload                = def.m_reload;
        m_replace               = def.m_replace;
        m_save                  = def.m_save;
        m_saveAs                = def.m_saveAs;
        m_selectAll             = def.m_selectAll;
        m_selectBlock           = def.m_selectBlock;
        m_selectLine            = def.m_selectLine;
        m_selectWord            = def.m_selectWord;
        m_showBreaks            = def.m_showBreaks;
        m_showSpaces            = def.m_showSpaces;
        m_spellCheck            = def.m_spellCheck;
        m_undo                  = def.m_undo;
        m_upper                 = def.m_upper;
        m_edtEnabled            = def.m_edtEnabled;
        m_numLockGold           = def.m_numLockGold;
        m_scrollLockGold        = def.m_scrollLockGold;
        m_f12AsBackspace        = def.m_f12AsBackspace;
        m_edtGotoLine           = def.m_edtGotoLine;
        m_edtCopy               = def.m_edtCopy;
        m_edtInsertFile         = def.m_edtInsertFile;
        m_edtUpperCase          = def.m_edtUpperCase;
        m_edtLowerCase          = def.m_edtLowerCase;
        m_edtSplitHorizontal    = def.m_edtSplitHorizontal;
        m_edtSplitVertical      = def.m_edtSplitVertical;
        m_edtSaveFile           = def.m_edtSaveFile;
        m_edtAstyle             = def.m_edtAstyle;
        m_edtWordCtrlMeta       = def.m_edtWordCtrlMeta;
        m_edtWordAltOption      = def.m_edtWordAltOption;
    }

    return *this;
}

bool operator ==( const KeyDefinitions &left, const KeyDefinitions &right )
{
    bool retVal = true;

    if ( left.m_close != right.m_close )
    {
        retVal = false;
    }

    if ( left.m_columnMode != right.m_columnMode )
    {
        retVal = false;
    }

    if ( left.m_copy != right.m_copy )
    {
        retVal = false;
    }

    if ( left.m_copyBuffer != right.m_copyBuffer )
    {
        retVal = false;
    }

    if ( left.m_cut != right.m_cut )
    {
        retVal = false;
    }

    if ( left.m_deleteLine != right.m_deleteLine )
    {
        retVal = false;
    }

    if ( left.m_deleteThroughEOL != right.m_deleteThroughEOL )
    {
        retVal = false;
    }

    if ( left.m_deleteToEOL != right.m_deleteToEOL )
    {
        retVal = false;
    }

    if ( left.m_find != right.m_find )
    {
        retVal = false;
    }

    if ( left.m_findNext != right.m_findNext )
    {
        retVal = false;
    }

    if ( left.m_findPrev != right.m_findPrev )
    {
        retVal = false;
    }

    if ( left.m_goBottom != right.m_goBottom )
    {
        retVal = false;
    }

    if ( left.m_goTop != right.m_goTop )
    {
        retVal = false;
    }

    if ( left.m_gotoLine != right.m_gotoLine )
    {
        retVal = false;
    }

    if ( left.m_indentDecrement != right.m_indentDecrement )
    {
        retVal = false;
    }

    if ( left.m_indentIncrement != right.m_indentIncrement )
    {
        retVal = false;
    }

    if ( left.m_lower != right.m_lower )
    {
        retVal = false;
    }

    if ( left.m_macroPlay != right.m_macroPlay )
    {
        retVal = false;
    }

    if ( left.m_newTab != right.m_newTab )
    {
        retVal = false;
    }

    if ( left.m_open != right.m_open )
    {
        retVal = false;
    }

    if ( left.m_paste != right.m_paste )
    {
        retVal = false;
    }

    if ( left.m_print != right.m_print )
    {
        retVal = false;
    }

    if ( left.m_printPreview != right.m_printPreview )
    {
        retVal = false;
    }

    if ( left.m_redo != right.m_redo )
    {
        retVal = false;
    }

    if ( left.m_reload != right.m_reload )
    {
        retVal = false;
    }

    if ( left.m_replace != right.m_replace )
    {
        retVal = false;
    }

    if ( left.m_save != right.m_save )
    {
        retVal = false;
    }

    if ( left.m_saveAs != right.m_saveAs )
    {
        retVal = false;
    }

    if ( left.m_selectAll != right.m_selectAll )
    {
        retVal = false;
    }

    if ( left.m_selectBlock != right.m_selectBlock )
    {
        retVal = false;
    }

    if ( left.m_selectLine != right.m_selectLine )
    {
        retVal = false;
    }

    if ( left.m_selectWord != right.m_selectWord )
    {
        retVal = false;
    }

    if ( left.m_showBreaks != right.m_showBreaks )
    {
        retVal = false;
    }

    if ( left.m_showSpaces != right.m_showSpaces )
    {
        retVal = false;
    }

    if ( left.m_spellCheck != right.m_spellCheck )
    {
        retVal = false;
    }

    if ( left.m_undo != right.m_undo )
    {
        retVal = false;
    }

    if ( left.m_upper != right.m_upper )
    {
        retVal = false;
    }

    if ( left.m_edtEnabled != right.m_edtEnabled )
    {
        retVal = false;
    }

    if ( left.m_numLockGold != right.m_numLockGold )
    {
        retVal = false;
    }

    if ( left.m_scrollLockGold != right.m_scrollLockGold )
    {
        retVal = false;
    }

    if ( left.m_edtGotoLine != right.m_edtGotoLine )
    {
        retVal = false;
    }

    if ( left.m_edtCopy != right.m_edtCopy )
    {
        retVal = false;
    }

    if ( left.m_edtInsertFile != right.m_edtInsertFile )
    {
        retVal = false;
    }

    if ( left.m_edtUpperCase != right.m_edtUpperCase )
    {
        retVal = false;
    }

    if ( left.m_edtLowerCase != right.m_edtLowerCase )
    {
        retVal = false;
    }

    if ( left.m_edtSplitHorizontal != right.m_edtSplitHorizontal )
    {
        retVal = false;
    }

    if ( left.m_edtSplitVertical != right.m_edtSplitVertical )
    {
        retVal = false;
    }

    if ( left.m_edtSaveFile != right.m_edtSaveFile )
    {
        retVal = false;
    }

    if ( left.m_edtAstyle != right.m_edtAstyle )
    {
        retVal = false;
    }

    if ( left.m_f12AsBackspace != right.m_f12AsBackspace )
    {
        retVal = false;
    }

    if ( left.m_edtWordCtrlMeta != right.m_edtWordCtrlMeta )
    {
        retVal = false;
    }

    if ( left.m_edtWordAltOption != right.m_edtWordAltOption )
    {
        retVal = false;
    }

    return retVal;

}

bool operator !=( const KeyDefinitions &left, const KeyDefinitions &right )
{
    bool retVal = true;

    if ( left == right )
    {
        retVal = false;
    }

    return retVal;
}


void KeyDefinitions::setDefaultKeyValues()
{
    QString modifier;

#ifdef Q_OS_MAC
    modifier            = "Meta+";
    m_edtWordCtrlMeta   = true;
    m_edtWordAltOption  = false;
#else
    modifier            = "Ctrl+";
    m_edtWordCtrlMeta   = false;
    m_edtWordAltOption  = true;
#endif

    // EDT keys
    //
    m_edtEnabled            = false;
    m_numLockGold           = true;
    m_scrollLockGold        = false;
    m_f12AsBackspace        = false;
    m_edtGotoLine           = QKeySequence( Qt::Key_Equal ).toString( QKeySequence::NativeText );
    m_edtCopy               = QKeySequence( Qt::Key_C ).toString( QKeySequence::NativeText );
    m_edtInsertFile         = QKeySequence( Qt::Key_I ).toString( QKeySequence::NativeText );
    m_edtUpperCase          = QKeySequence( Qt::Key_U ).toString( QKeySequence::NativeText );
    m_edtLowerCase          = QKeySequence( Qt::Key_L ).toString( QKeySequence::NativeText );
    m_edtSplitHorizontal    = QKeySequence( Qt::Key_H ).toString( QKeySequence::NativeText );
    m_edtSplitVertical      = QKeySequence( Qt::Key_V ).toString( QKeySequence::NativeText );
    m_edtSaveFile           = QKeySequence( Qt::Key_S ).toString( QKeySequence::NativeText );
    m_edtAstyle             = QKeySequence( Qt::Key_A ).toString( QKeySequence::NativeText );


    // Standard keys
    //
    m_open      = QKeySequence( QKeySequence::Open ).toString( QKeySequence::NativeText );
    m_close     = QKeySequence( QKeySequence::Close ).toString( QKeySequence::NativeText );
    m_save      = QKeySequence( QKeySequence::Save ).toString( QKeySequence::NativeText );
    m_saveAs    = QKeySequence( QKeySequence::SaveAs ).toString( QKeySequence::NativeText );
    m_print     = QKeySequence( QKeySequence::Print ).toString( QKeySequence::NativeText );
    m_undo      = QKeySequence( QKeySequence::Undo ).toString( QKeySequence::NativeText );
    m_redo      = QKeySequence( QKeySequence::Redo ).toString( QKeySequence::NativeText );
    m_cut       = QKeySequence( QKeySequence::Cut ).toString( QKeySequence::NativeText );
    m_copy      = QKeySequence( QKeySequence::Copy ).toString( QKeySequence::NativeText );
    m_paste     = QKeySequence( QKeySequence::Paste ).toString( QKeySequence::NativeText );
    m_selectAll = QKeySequence( QKeySequence::SelectAll ).toString( QKeySequence::NativeText );
    m_find      = QKeySequence( QKeySequence::Find ).toString( QKeySequence::NativeText );
    m_replace   = QKeySequence( QKeySequence::Replace ).toString( QKeySequence::NativeText );
    m_findNext  = QKeySequence( QKeySequence::FindNext ).toString( QKeySequence::NativeText );
    m_findPrev  = QKeySequence( QKeySequence::FindPrevious ).toString( QKeySequence::NativeText );
    m_goTop     = QKeySequence( QKeySequence::MoveToStartOfDocument ).toString( QKeySequence::NativeText );
    m_goBottom  = QKeySequence( QKeySequence::MoveToEndOfDocument ).toString( QKeySequence::NativeText );
    m_newTab    = QKeySequence( QKeySequence::AddTab ).toString( QKeySequence::NativeText );

    // User keys
    //
    m_selectLine        = modifier + "E";
    m_selectWord        = modifier + "D";
    m_selectBlock       = modifier + "B";
    m_upper             = modifier + "U";
    m_lower             = modifier + "L";
    m_indentIncrement   = modifier + "I";
    m_indentDecrement   = modifier + "Shift+I";
    m_deleteLine        = "Alt+D";
    m_deleteToEOL       = modifier + "K";
    m_columnMode        = "Alt+C";
    m_gotoLine          = "Alt+G";
    m_showSpaces        = "";
    m_showBreaks        = "";
    m_macroPlay         = "Alt+Return";
    m_spellCheck        = "F7";
    m_copyBuffer        = "F11";
    m_deleteThroughEOL  = "";

    platformAdjustKeyValues();
}

// if someone copies a json file between Windows, MAC, Linux, etc
// the key definitions will need tweaking.
//
void KeyDefinitions::platformAdjustKeyValues()
{
    m_close                 = adjustKey( m_close );
    m_columnMode            = adjustKey( m_columnMode );
    m_copy                  = adjustKey( m_copy );
    m_copyBuffer            = adjustKey( m_copyBuffer );
    m_cut                   = adjustKey( m_cut );
    m_deleteLine            = adjustKey( m_deleteLine );
    m_deleteThroughEOL      = adjustKey( m_deleteThroughEOL );
    m_deleteToEOL           = adjustKey( m_deleteToEOL );
    m_find                  = adjustKey( m_find );
    m_findNext              = adjustKey( m_findNext );
    m_findPrev              = adjustKey( m_findPrev );
    m_goBottom              = adjustKey( m_goBottom );
    m_goTop                 = adjustKey( m_goTop );
    m_gotoLine              = adjustKey( m_gotoLine );
    m_indentDecrement       = adjustKey( m_indentDecrement );
    m_indentIncrement       = adjustKey( m_indentIncrement );
    m_lower                 = adjustKey( m_lower );
    m_macroPlay             = adjustKey( m_macroPlay );
    m_newTab                = adjustKey( m_newTab );
    m_open                  = adjustKey( m_open );
    m_paste                 = adjustKey( m_paste );
    m_print                 = adjustKey( m_print );
    m_printPreview          = adjustKey( m_printPreview );
    m_redo                  = adjustKey( m_redo );
    m_reload                = adjustKey( m_reload );
    m_replace               = adjustKey( m_replace );
    m_save                  = adjustKey( m_save );
    m_saveAs                = adjustKey( m_saveAs );
    m_selectAll             = adjustKey( m_selectAll );
    m_selectBlock           = adjustKey( m_selectBlock );
    m_selectLine            = adjustKey( m_selectLine );
    m_selectWord            = adjustKey( m_selectWord );
    m_showBreaks            = adjustKey( m_showBreaks );
    m_showSpaces            = adjustKey( m_showSpaces );
    m_spellCheck            = adjustKey( m_spellCheck );
    m_undo                  = adjustKey( m_undo );
    m_upper                 = adjustKey( m_upper );

    m_edtGotoLine           = adjustKey( m_edtGotoLine );
    m_edtCopy               = adjustKey( m_edtCopy );
    m_edtInsertFile         = adjustKey( m_edtInsertFile );
    m_edtUpperCase          = adjustKey( m_edtUpperCase );
    m_edtLowerCase          = adjustKey( m_edtLowerCase );
    m_edtSplitHorizontal    = adjustKey( m_edtSplitHorizontal );
    m_edtSplitVertical      = adjustKey( m_edtSplitVertical );
    m_edtSaveFile           = adjustKey( m_edtSaveFile );
}

QString KeyDefinitions::adjustKey( QString sequence )
{
    QString modifier = sequence.left( 4 );

#ifdef Q_OS_MAC

    if ( modifier == "Ctrl" )
    {
        sequence.replace( 0,4,"Meta" );
    }

#else

    if ( modifier == "Meta" )
    {
        sequence.replace( 0,4,"Ctrl" );
    }

#endif

    return sequence;
}

Qt::Key KeyDefinitions::goldKey()
{
    Qt::Key retVal = Qt::Key_NumLock;

    if ( m_scrollLockGold )
    {
        retVal = Qt::Key_ScrollLock;
    }

    return retVal;
}
