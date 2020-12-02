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

#ifndef KEYDEFINITIONS_H
#define KEYDEFINITIONS_H

#include <QString>

class KeyDefinitions
{
public:
    KeyDefinitions();
    KeyDefinitions( const KeyDefinitions &def );

    KeyDefinitions &operator =( const KeyDefinitions &def );

    friend bool operator ==( const KeyDefinitions &left, const KeyDefinitions &right );
    friend bool operator !=( const KeyDefinitions &left, const KeyDefinitions &right );

    void setDefaultKeyValues();
    void platformAdjustKeyValues();

    //
    // getters
    //
    QString close()
    {
        return m_close;
    }
    QString columnMode()
    {
        return m_columnMode;
    }
    QString copy()
    {
        return m_copy;
    }
    QString copyBuffer()
    {
        return m_copyBuffer;
    }
    QString cut()
    {
        return m_cut;
    }
    QString deleteLine()
    {
        return m_deleteLine;
    }
    QString deleteThroughEOL()
    {
        return m_deleteThroughEOL;
    }
    QString deleteToEOL()
    {
        return m_deleteToEOL;
    }
    QString find()
    {
        return m_find;
    }
    QString findNext()
    {
        return m_findNext;
    }
    QString findPrev()
    {
        return m_findPrev;
    }
    QString goBottom()
    {
        return m_goBottom;
    }
    QString goTop()
    {
        return m_goTop;
    }
    QString gotoLine()
    {
        return m_gotoLine;
    }
    QString indentDecrement()
    {
        return m_indentDecrement;
    }
    QString indentIncrement()
    {
        return m_indentIncrement;
    }
    QString lower()
    {
        return m_lower;
    }
    QString macroPlay()
    {
        return m_macroPlay;
    }
    QString newTab()
    {
        return m_newTab;
    }
    QString open()
    {
        return m_open;
    }
    QString paste()
    {
        return m_paste;
    }
    QString print()
    {
        return m_print;
    }
    QString printPreview()
    {
        return m_printPreview;
    }
    QString redo()
    {
        return m_redo;
    }
    QString reload()
    {
        return m_reload;
    }
    QString replace()
    {
        return m_replace;
    }
    QString save()
    {
        return m_save;
    }
    QString saveAs()
    {
        return m_saveAs;
    }
    QString selectAll()
    {
        return m_selectAll;
    }
    QString selectBlock()
    {
        return m_selectBlock;
    }
    QString selectLine()
    {
        return m_selectLine;
    }
    QString selectWord()
    {
        return m_selectWord;
    }
    QString showBreaks()
    {
        return m_showBreaks;
    }
    QString showSpaces()
    {
        return m_showSpaces;
    }
    QString spellCheck()
    {
        return m_spellCheck;
    }
    QString undo()
    {
        return m_undo;
    }
    QString upper()
    {
        return m_upper;
    }

    QString edtGotoLine()
    {
        return m_edtGotoLine;
    }

    bool    edtEnabled()
    {
        return m_edtEnabled;
    }
    Qt::Key goldKey();
    bool    numLockGold()
    {
        return m_numLockGold;
    }
    bool    scrollLockGold()
    {
        return m_scrollLockGold;
    }
    QString edtCopy()
    {
        return m_edtCopy;
    }
    QString edtInsertFile()
    {
        return m_edtInsertFile;
    }
    QString edtUpperCase()
    {
        return m_edtUpperCase;
    }
    QString edtLowerCase()
    {
        return m_edtLowerCase;
    }
    QString edtSplitHorizontal()
    {
        return m_edtSplitHorizontal;
    }
    QString edtSplitVertical()
    {
        return m_edtSplitVertical;
    }
    QString edtSaveFile()
    {
        return m_edtSaveFile;
    }
    QString edtAstyle()
    {
        return m_edtAstyle;
    }
    bool    edtWordCtrlMeta()
    {
        return m_edtWordCtrlMeta;
    }
    bool    edtWordAltOption()
    {
        return m_edtWordAltOption;
    }
    bool    f12AsBackspace()
    {
        return m_f12AsBackspace;
    }




    //
    // Setters
    //
    void set_close( QString keyValue )
    {
        m_close = keyValue;
    }
    void set_columnMode( QString keyValue )
    {
        m_columnMode = keyValue;
    }
    void set_copy( QString keyValue )
    {
        m_copy = keyValue;
    }
    void set_copyBuffer( QString keyValue )
    {
        m_copyBuffer = keyValue;
    }
    void set_cut( QString keyValue )
    {
        m_cut = keyValue;
    }
    void set_deleteLine( QString keyValue )
    {
        m_deleteLine = keyValue;
    }
    void set_deleteThroughEOL( QString keyValue )
    {
        m_deleteThroughEOL = keyValue;
    }
    void set_deleteToEOL( QString keyValue )
    {
        m_deleteToEOL = keyValue;
    }
    void set_find( QString keyValue )
    {
        m_find = keyValue;
    }
    void set_findNext( QString keyValue )
    {
        m_findNext = keyValue;
    }
    void set_findPrev( QString keyValue )
    {
        m_findPrev = keyValue;
    }
    void set_goBottom( QString keyValue )
    {
        m_goBottom = keyValue;
    }
    void set_goTop( QString keyValue )
    {
        m_goTop = keyValue;
    }
    void set_gotoLine( QString keyValue )
    {
        m_gotoLine = keyValue;
    }
    void set_indentDecrement( QString keyValue )
    {
        m_indentDecrement = keyValue;
    }
    void set_indentIncrement( QString keyValue )
    {
        m_indentIncrement = keyValue;
    }
    void set_lower( QString keyValue )
    {
        m_lower = keyValue;
    }
    void set_macroPlay( QString keyValue )
    {
        m_macroPlay = keyValue;
    }
    void set_newTab( QString keyValue )
    {
        m_newTab = keyValue;
    }
    void set_open( QString keyValue )
    {
        m_open = keyValue;
    }
    void set_paste( QString keyValue )
    {
        m_paste = keyValue;
    }
    void set_print( QString keyValue )
    {
        m_print = keyValue;
    }
    void set_printPreview( QString keyValue )
    {
        m_printPreview = keyValue;
    }
    void set_redo( QString keyValue )
    {
        m_redo = keyValue;
    }
    void set_reload( QString keyValue )
    {
        m_reload = keyValue;
    }
    void set_replace( QString keyValue )
    {
        m_replace = keyValue;
    }
    void set_save( QString keyValue )
    {
        m_save = keyValue;
    }
    void set_saveAs( QString keyValue )
    {
        m_saveAs = keyValue;
    }
    void set_selectAll( QString keyValue )
    {
        m_selectAll = keyValue;
    }
    void set_selectBlock( QString keyValue )
    {
        m_selectBlock = keyValue;
    }
    void set_selectLine( QString keyValue )
    {
        m_selectLine = keyValue;
    }
    void set_selectWord( QString keyValue )
    {
        m_selectWord = keyValue;
    }
    void set_showBreaks( QString keyValue )
    {
        m_showBreaks = keyValue;
    }
    void set_showSpaces( QString keyValue )
    {
        m_showSpaces = keyValue;
    }
    void set_spellCheck( QString keyValue )
    {
        m_spellCheck = keyValue;
    }
    void set_undo( QString keyValue )
    {
        m_undo = keyValue;
    }
    void set_upper( QString keyValue )
    {
        m_upper = keyValue;
    }

    void set_edtEnabled( bool yesNo )
    {
        m_edtEnabled = yesNo;
    }
    void set_edtGotoLine( QString txt )
    {
        m_edtGotoLine = txt;
    }
    void set_numLockGold( bool yesNo )
    {
        m_numLockGold = yesNo;
    }
    void set_scrollLockGold( bool yesNo )
    {
        m_scrollLockGold = yesNo;
    }
    void set_edtCopy( QString keyValue )
    {
        m_edtCopy = keyValue;
    }
    void set_edtInsertFile( QString keyValue )
    {
        m_edtInsertFile = keyValue;
    }
    void set_edtUpperCase( QString keyValue )
    {
        m_edtUpperCase = keyValue;
    }
    void set_edtLowerCase( QString keyValue )
    {
        m_edtLowerCase = keyValue;
    }
    void set_edtSplitHorizontal( QString keyValue )
    {
        m_edtSplitHorizontal = keyValue;
    }
    void set_edtSplitVertical( QString keyValue )
    {
        m_edtSplitVertical = keyValue;
    }
    void set_edtSaveFile( QString keyValue )
    {
        m_edtSaveFile = keyValue;
    }
    void set_edtAstyle( QString keyValue )
    {
        m_edtAstyle = keyValue;
    }
    void set_edtWordCtrlMeta( bool yesNo )
    {
        m_edtWordCtrlMeta = yesNo;
    }
    void set_edtWordAltOption( bool yesNo )
    {
        m_edtWordAltOption = yesNo;
    }
    void set_f12AsBackspace( bool yesNo )
    {
        m_f12AsBackspace = yesNo;
    }

private:
    QString adjustKey( QString sequence );

    QString m_close;
    QString m_columnMode;
    QString m_copy;
    QString m_copyBuffer;
    QString m_cut;
    QString m_deleteLine;
    QString m_deleteThroughEOL;
    QString m_deleteToEOL;
    QString m_find;
    QString m_findNext;
    QString m_findPrev;
    QString m_goBottom;
    QString m_goTop;
    QString m_gotoLine;
    QString m_indentDecrement;
    QString m_indentIncrement;
    QString m_lower;
    QString m_macroPlay;
    QString m_newTab;
    QString m_open;
    QString m_paste;
    QString m_print;
    QString m_printPreview;
    QString m_redo;
    QString m_reload;
    QString m_replace;
    QString m_save;
    QString m_saveAs;
    QString m_selectAll;
    QString m_selectBlock;
    QString m_selectLine;
    QString m_selectWord;
    QString m_showBreaks;
    QString m_showSpaces;
    QString m_spellCheck;
    QString m_undo;
    QString m_upper;

    /*
     * EDT - unlike the PC world where you have to press Ctrl+L at the same time to
     *       do whatever that key sequence is mapped to, EDT uses individual keystrokes
     *       [PF3] is normally mapped to the [*] on the numeric keypad. Pressing just
     *       it is "find next." Pressing GOLD and releasing it just in front of [*]
     *       causes the "find" text prompt to happen. EDT is direction sensitive.
     *       [4] is advance  [5] is backup. These remain in effect. "find next"
     *       travels in the direction currently in effect.
     */
    bool    m_edtEnabled;       // only look for EDT keypad support when enabled.
    bool    m_numLockGold;      // use NumLock as GOLD key - default
    bool    m_scrollLockGold;   // use ScrollLock as GOLD key - typical from Emacs
    bool    m_f12AsBackspace;   // use F12 as VT Backspace (home) key
    QString m_edtGotoLine;          // GOLD =
    QString m_edtCopy;              // GOLD C
    QString m_edtInsertFile;        // GOLD I
    QString m_edtUpperCase;         // GOLD U
    QString m_edtLowerCase;         // GOLD L
    QString m_edtSplitHorizontal;   // GOLD H
    QString m_edtSplitVertical;     // GOLD V
    QString m_edtSaveFile;          // GOLD S
    QString m_edtAstyle;            // GOLD A
    bool    m_edtWordCtrlMeta;      // Ctrl (PC) Meta (Apple) Keypad+
    bool    m_edtWordAltOption;     // Alt (PC) Option (Apple) Keypad+
};

#endif
