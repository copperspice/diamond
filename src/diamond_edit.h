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

#ifndef DIAMOND_TEXTEDIT_H
#define DIAMOND_TEXTEDIT_H

#include <QObject>
#include <QList>
#include <QPlainTextEdit>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QSize>
#include <QTextCursor>
#include <QWidget>
#include <QProcess>

#include "spellcheck.h"
#include "overlord.h"
#include "syntax.h"


class LineNumberArea;

class DiamondTextEdit : public QPlainTextEdit
{
    CS_OBJECT( DiamondTextEdit )

public:
    DiamondTextEdit( QWidget *parent, QString owner = "tab" );
    ~DiamondTextEdit();

    QString m_owner;

    static const QString PARAGRAPH_MARKER;

    QString currentFile();
    void setCurrentFile( QString fileName );

    void lineNum_PaintEvent( QPaintEvent *event );
    int lineNum_Width();

    void set_ShowLineNum( bool yesNo );

    // column mode
    void set_ColumnMode( bool yesNo );
    bool get_ColumnMode();

    // copy buffer
    QList<QString> copyBuffer() const;

    // macro
    void macroStart();
    void macroStop();
    QList<QKeyEvent *> get_MacroKeyList();

    // spell
    void set_Spell( bool value );
    QTextCursor get_Cursor();


    void edtInsertFile();

    // syntax
    QString get_SyntaxFile();
    void set_SyntaxFile( QString fname );
    Syntax *get_SyntaxParser();
    void set_SyntaxParser( Syntax *data );
    SyntaxTypes get_SyntaxEnum();
    void set_SyntaxEnum( SyntaxTypes data );
    void forceSyntax( SyntaxTypes data );


    CS_SLOT_1( Public, void cut() )
    CS_SLOT_2( cut )

    CS_SLOT_1( Public, void copy() )
    CS_SLOT_2( copy )

    CS_SLOT_1( Public, void paste() )
    CS_SLOT_2( paste )

    void changeSettings( Settings *settings );

    CS_SLOT_1( Public, void rewrapParagraph() )
    CS_SLOT_2( rewrapParagraph )

    void runSyntax( QString synFName );
    void clearEdtSelection();

    CS_SIGNAL_1( Public, void setSynType( SyntaxTypes data ) )
    CS_SIGNAL_2( setSynType, data )

    CS_SIGNAL_1( Public, void queueRunSyntax( QString synFName ) )
    CS_SIGNAL_2( queueRunSyntax, synFName )

    CS_SIGNAL_1( Public, void edtUpcase() )
    CS_SIGNAL_2( edtUpcase )

    CS_SIGNAL_1( Public, void edtLowerCase() )
    CS_SIGNAL_2( edtLowerCase )

    CS_SIGNAL_1( Public, void edtSplitH() )
    CS_SIGNAL_2( edtSplitH )

    CS_SIGNAL_1( Public, void edtSplitV() )
    CS_SIGNAL_2( edtSplitV )

    CS_SIGNAL_1( Public, void edtSaveFile( QString fileName, Overlord::SaveFiles fileType ) )
    CS_SIGNAL_2( edtSaveFile, fileName, fileType )

    CS_SIGNAL_1( Public, void edtSaveFileAs( Overlord::SaveFiles fileType ) )
    CS_SIGNAL_2( edtSaveFileAs, fileType )

    CS_SIGNAL_1( Public, void showEdtHelp() )
    CS_SIGNAL_2( showEdtHelp )

    CS_SIGNAL_1( Public, void edtCopy() )
    CS_SIGNAL_2( edtCopy )

    CS_SIGNAL_1( Public, void edtCut() )
    CS_SIGNAL_2( edtCut )

    CS_SIGNAL_1( Public, void edtBottom() )
    CS_SIGNAL_2( edtBottom )

    CS_SIGNAL_1( Public, void edtTop() )
    CS_SIGNAL_2( edtTop )

    CS_SIGNAL_1( Public, void edtPaste() )
    CS_SIGNAL_2( edtPaste )

    CS_SIGNAL_1( Public, void edtSubs() )
    CS_SIGNAL_2( edtSubs )

    CS_SIGNAL_1( Public, void not_done( QString txt ) )
    CS_SIGNAL_2( not_done, txt )

    CS_SIGNAL_1( Public, void timedMessage( QString msg, int milliSeconds ) )
    CS_SIGNAL_2( timedMessage, msg, milliSeconds )



    // editing API for use by MainWindow
    //
    CS_SLOT_1( Public, void deleteLine() )
    CS_SLOT_2( deleteLine )

    CS_SLOT_1( Public, void deleteEOL() )
    CS_SLOT_2( deleteEOL )

    CS_SLOT_1( Public, void deleteThroughEOL() )
    CS_SLOT_2( deleteThroughEOL )

    CS_SLOT_1( Public, void insertDate() )
    CS_SLOT_2( insertDate )

    CS_SLOT_1( Public, void insertTime() )
    CS_SLOT_2( insertTime )

    CS_SLOT_1( Public, void caseUpper() )
    CS_SLOT_2( caseUpper )

    CS_SLOT_1( Public, void caseLower() )
    CS_SLOT_2( caseLower )

    CS_SLOT_1( Public, void caseCap() )
    CS_SLOT_2( caseCap )

    CS_SLOT_1( Public, void caseChange() )
    CS_SLOT_2( caseChange )

    CS_SLOT_1( Public, void selectAll() )
    CS_SLOT_2( selectAll )

    void astyleBuffer( );
    QByteArray astyleNewBuffer();
    void indentIncr( QString route );
    void indentDecr( QString route );
    void insertSymbol();
    void selectBlock();
    void selectLine();
    void selectWord();
    void goLine();
    void goColumn();
    void goTop();
    void goBottom();
    void moveBar();
    void fixTab_Spaces();
    void fixSpaces_Tab();
    void deleteEOL_Spaces();
    void rewrapParaphragh();
    void setSyntax();


    // spell check
    void update_display();

protected:
    void contextMenuEvent( QContextMenuEvent *event );
    bool event( QEvent *event );
    void keyPressEvent( QKeyEvent *event );
    void keyReleaseEvent( QKeyEvent *event );
    void resizeEvent( QResizeEvent *event );
    void mousePressEvent( QMouseEvent *event );

private:
    CS_SLOT_1( Private, void spell_replaceWord() )
    CS_SLOT_2( spell_replaceWord )

    CS_SLOT_1( Private, void spell_addUserDict() )
    CS_SLOT_2( spell_addUserDict )

    void astyleComplete( int exitCode, QProcess::ExitStatus status );
    void astyleError( QProcess::ProcessError error );
    void setScreenColors();
    void changeFont();
    bool handleEdtKey( int key, int modifiers );
    void edtSectionUp( QTextCursor::MoveMode mode );
    void edtSectionDown( QTextCursor::MoveMode mode );
    void edtKeyRight( QTextCursor::MoveMode mode );
    void edtKeyLeft( QTextCursor::MoveMode mode );
    void edtLineUp( QTextCursor::MoveMode mode );
    void edtLineDown( QTextCursor::MoveMode mode );
    void edtEndOfLine( QTextCursor::MoveMode mode );
    void edtHome( QTextCursor::MoveMode mode );
    void nukeMacro();
    void storeMacro();


    Settings *m_settingsPtr;
    Themes m_lastTheme;
    QFont m_lastNormalFont;
    QFont m_lastColumnFont;

    QWidget *m_lineNumArea;
    QString m_curFile;

    // tab stops
    QList<int> m_tabStops;
    void setUpTabStops();

    // column mode
    int m_undoCount;
    void removeColumnModeSpaces();

    bool m_colHighlight;
    bool m_currentKeyGold;
    bool m_edtSelectActive;

    int m_startRow;
    int m_startCol;
    int m_endRow;
    int m_endCol;
    int m_lastTabSpacing;


    // copy buffer
    QList<QString> m_copyBuffer;
    void addToCopyBuffer( const QString &text );

    // macro
    bool m_record;
    QList<QKeyEvent *> m_macroKeyList;

    // spell check
    QStringList spell_getMaybe( QString word );
    void createSpellCheck();
    QTextCursor m_cursor;
    bool m_isSpellCheck;
    SpellCheck *m_spellCheck;

    // syntax
    Syntax *m_syntaxParser;
    QString m_synFName;
    SyntaxTypes m_syntaxEnum;

    void update_LineNumWidth( int newBlockCount );

    QProcess *m_astyleProcess;
};


class LineNumArea : public QWidget
{
public:
    LineNumArea( DiamondTextEdit *editor ) : QWidget( editor )
    {
        m_editor = editor;
    }

    QSize sizeHint() const
    {
        return QSize( m_editor->lineNum_Width(), 0 );
    }

protected:
    void paintEvent( QPaintEvent *event )
    {
        m_editor->lineNum_PaintEvent( event );
    }

private:
    DiamondTextEdit *m_editor;
};


#endif

