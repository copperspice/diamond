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

/**************************************************************************
 * Some of this might look weird at first.
 *
 * When setting a value found in the settings object, we must always set
 * using Overlord::getInstance().
 *
 * When reading a value that impacts the display we must always use the
 * local pointer.
 *
 * The reason behind this is the dialog adjusting color and theme. It uses
 * a local copy of the settings so user changes do not impact the running
 * application until the user saves/accepts them. That dialog will connect
 * a signal to the slot of the instance of this class appearing on it.
 * User can then instantly see what their color choices look like.
 ***************************************************************************/

#include "diamond_edit.h"
#include "non_gui_functions.h"
#include "dialog_symbols.h"
#include "util.h"
#include "dialog_edt_prompt.h"

#include <QApplication>
#include <QClipboard>
#include <QPainter>
#include <QShortcutEvent>
#include <QMenu>
#include <QDate>
#include <QTime>
#include <qtconcurrentrun.h>
#include <QThread>
#include <QStringParser>
#include <QFileDialog>
#include <QUuid>
#include <QInputDialog>

static const int MESSAGE_TIME = 3000;
const QString DiamondTextEdit::PARAGRAPH_MARKER = QString::fromUtf8( "\u2029" );

DiamondTextEdit::DiamondTextEdit( QWidget *parent, QString owner )
    : QPlainTextEdit( parent )
    , m_owner( owner )
    , m_settingsPtr( nullptr )
    , m_undoCount( 0 )
    , m_colHighlight( false )
    , m_currentKeyGold( false )
    , m_edtSelectActive( false )
    , m_startRow( 0 )
    , m_startCol( 0 )
    , m_endRow( 0 )
    , m_endCol( 0 )
    , m_lastTabSpacing( 0 )
    , m_record( false )
    , m_isSpellCheck( false )
    , m_spellCheck( nullptr )
    , m_syntaxParser( nullptr )
    , m_astyleProcess( nullptr )
{
    // drag & drop
    setAcceptDrops( false );

    // column mode
    m_undoCount = 0;

    // line numbers
    m_lineNumArea = new LineNumArea( this );
    update_LineNumWidth( 0 );

    // syntax - assinged from loadfile(), runSyntax()
    m_synFName     = "";

    // I really kind of hate this. I have to keep a local pointer
    // because I want to re-use this editor inside of the colors dialog.
    // That dialog keeps a local copy of changes until the user chooses to save.
    //
    m_settingsPtr = Overlord::getInstance()->pointerToSettings();

    // spell check
    m_isSpellCheck = Overlord::getInstance()->isSpellCheck();

    // make local copies of the things that require a lot of work
    // so we only want to update that stuff when it actually changes.
    //
    m_lastTheme      = *m_settingsPtr->currentTheme();
    m_lastNormalFont = m_settingsPtr->fontNormal();
    m_lastColumnFont = m_settingsPtr->fontColumn();

    // this is so syntax highlighting can go on the event queue and run later.
    // Big files really hang during load.
    //
    connect( this, &DiamondTextEdit::queueRunSyntax, this, &DiamondTextEdit::runSyntax, Qt::QueuedConnection );

    connect( this, &DiamondTextEdit::blockCountChanged, this, &DiamondTextEdit::update_LineNumWidth );
    connect( this, &DiamondTextEdit::updateRequest,     this, &DiamondTextEdit::update_display );

    connect( Overlord::getInstance(), &Overlord::settingsChanged,
             this, &DiamondTextEdit::changeSettings );
    createSpellCheck();
    setUpTabStops();
    setScreenColors();
    changeFont();
}

DiamondTextEdit::~DiamondTextEdit()
{
    if ( m_spellCheck )
    {
        delete m_spellCheck;
        m_spellCheck = nullptr;
    }

    if ( m_astyleProcess )
    {
        if ( !m_astyleProcess->atEnd() )
        {
            m_astyleProcess->kill();
        }

        delete m_astyleProcess;
        m_astyleProcess = nullptr;
    }

    nukeMacro();
}

void DiamondTextEdit::nukeMacro()
{
    for ( QKeyEvent *ptr : m_macroKeyList )
    {
        delete ptr;
    }

    m_macroKeyList.clear();
}

// ** line numbers
void DiamondTextEdit::lineNum_PaintEvent( QPaintEvent *event )
{
    if ( m_settingsPtr->showLineNumbers() )
    {

        QPainter painter( m_lineNumArea );
        painter.fillRect( event->rect(), m_settingsPtr->currentTheme()->gutterBack() );

        QTextBlock block = firstVisibleBlock();
        int blockNumber = block.blockNumber();
        int top    = ( int ) blockBoundingGeometry( block ).translated( contentOffset() ).top();
        int bottom = top + ( int ) blockBoundingRect( block ).height();

        while ( block.isValid() && top <= event->rect().bottom() )
        {
            if ( block.isVisible() && bottom >= event->rect().top() )
            {
                QString number = QString::number( blockNumber + 1 );

                painter.setPen( m_settingsPtr->currentTheme()->gutterText() );
                painter.drawText( 0, top, m_lineNumArea->width()-7, fontMetrics().height(), Qt::AlignRight, number );
            }

            block = block.next();
            top = bottom;
            bottom = top + ( int ) blockBoundingRect( block ).height();
            ++blockNumber;
        }
    }
}

int DiamondTextEdit::lineNum_Width()
{
    int space = 0;

    if ( Overlord::getInstance()->showLineNumbers() )
    {
        int digits = 4;
        int max = blockCount();

        for ( int k=1000; k < max; k *= 10 )
        {
            ++digits;
        }

        space = 8 + fontMetrics().width( QLatin1Char( '9' ) ) * digits;
    }

    return space;
}

void DiamondTextEdit::update_LineNumWidth( int newBlockCount )
{
    Q_UNUSED( newBlockCount )
    setViewportMargins( lineNum_Width(), 0, 0, 0 );

    m_lineNumArea->setVisible( Overlord::getInstance()->showLineNumbers() );
}


void DiamondTextEdit::update_display()
{
    update_LineNumWidth( 0 );
    update();
}

void DiamondTextEdit::resizeEvent( QResizeEvent *e )
{
    QPlainTextEdit::resizeEvent( e );

    QRect cr = contentsRect();
    m_lineNumArea->setGeometry( QRect( cr.left(), cr.top(), lineNum_Width(), cr.height() ) );
}


// ** context menu
QTextCursor DiamondTextEdit::get_Cursor()
{
    return m_cursor;
}

void DiamondTextEdit::contextMenuEvent( QContextMenuEvent *event )
{
    bool isSelected = false;

    QTextCursor cursor( textCursor() );
    QString selectedText = cursor.selectedText();

    if ( ! selectedText.isEmpty() )
    {
        isSelected = true;
    }

    if ( ! isSelected && m_settingsPtr->isColumnMode() )
    {
        // check for extra selection

        QList<QTextEdit::ExtraSelection> oldSelections = extraSelections();

        for ( int k = 0; k < oldSelections.size(); ++k )
        {

            if ( oldSelections[k].format.property( QTextFormat::UserProperty ).toString() != "highlightbar" )
            {
                isSelected = true;
                break;
            }
        }
    }

    //
    QMenu *menu = new QMenu( this );

    if ( m_spellCheck && m_isSpellCheck )
    {

        cursor.setPosition( cursorForPosition( event->pos() ).position() );
        cursor.select( QTextCursor::WordUnderCursor );
        selectedText = cursor.selectedText();

        // set up to save words, used in add_userDict() and replaceWord()
        m_cursor = cursor;

        QStringList m_maybeList = spell_getMaybe( selectedText );
        int cnt = m_maybeList.count();

        if ( cnt > 0 )
        {

            for ( int k = 0; k < cnt; ++k )
            {
                menu->addAction( m_maybeList[k], this, SLOT( spell_replaceWord() )  );
            }

            menu->addAction( "Add to User Dictionary", this, SLOT( spell_addUserDict() ) );
            menu->addSeparator();
        }
    }

    QAction *tempAction;

    if ( isSelected )
    {
        menu->addAction( "Rewrap Paragraph",  this, SLOT( rewrapParagraph() ) );
        menu->addSeparator();
    }

    tempAction = menu->addAction( "Undo", this, SLOT( undo() ) );

    if ( ! document()->isUndoAvailable() )
    {
        tempAction->setDisabled( true );
    }

    tempAction = menu->addAction( "Redo", this, SLOT( redo() ) );

    if ( ! document()->isRedoAvailable() )
    {
        tempAction->setDisabled( true );
    }

    menu->addSeparator();

    tempAction = menu->addAction( "Cut", this, SLOT( cut() ) );

    if ( ! isSelected )
    {
        tempAction->setDisabled( true );
    }

    tempAction = menu->addAction( "Copy", this, SLOT( copy() ) );

    if ( ! isSelected )
    {
        tempAction->setDisabled( true );
    }

    tempAction = menu->addAction( "Paste", this, SLOT( paste() ) );

    menu->addSeparator();
    menu->addAction( "Delete Line",    this, SLOT( deleteLine() ) );
    menu->addAction( "Delete to EOL",  this, SLOT( deleteEOL() ) );
    menu->addAction( "Delete Through EOL", this, SLOT( deleteThroughEOL() ) );

    if ( isSelected )
    {
        menu->addSeparator();
        menu->addAction( "Uppercase",   this, SLOT( caseUpper() ) );
        menu->addAction( "Lowercase",   this, SLOT( caseLower() ) );
        menu->addAction( "Change case", this, SLOT( caseChange() ) );
    }
    else
    {
        menu->addSeparator();
        menu->addAction( "Insert Date", this, SLOT( insertDate() ) );
        menu->addAction( "Insert Time", this, SLOT( insertTime() ) );

        menu->addSeparator();
        menu->addAction( "Select All", this, SLOT( selectAll() ) );
    }

    menu->exec( event->globalPos() );
    //delete menu;
}

// ** syntax
QString DiamondTextEdit::get_SyntaxFile()
{
    return m_synFName;
}

void DiamondTextEdit::set_SyntaxFile( QString fname )
{
    m_synFName = fname;
}

Syntax *DiamondTextEdit::get_SyntaxParser()
{
    return m_syntaxParser;
}

void DiamondTextEdit::set_SyntaxParser( Syntax *parser )
{
    if ( m_syntaxParser )
    {
        m_syntaxParser->setDocument( nullptr );
        delete m_syntaxParser;
    }

    m_syntaxParser = parser;

    if ( parser )
    {
        // TODO:: find a way to make this faster.
        //        big files really kill inital load
        //
        m_syntaxParser->processSyntax( m_settingsPtr );
    }
}

SyntaxTypes DiamondTextEdit::get_SyntaxEnum()
{
    return m_syntaxEnum;
}

void DiamondTextEdit::set_SyntaxEnum( SyntaxTypes data )
{
    m_syntaxEnum = data;
}


// ** spell check
void DiamondTextEdit::set_Spell( bool value )
{
    m_isSpellCheck = value;
    m_syntaxParser->set_Spell( value );

    // force syntax highlight to redraw
    m_syntaxParser->rehighlight();
}


// ** column mode
void DiamondTextEdit::set_ColumnMode( bool yesNo )
{
    // this looks weird, but when setting a value in settings
    // we must always use the singleton.
    Overlord::getInstance()->set_isColumnMode( yesNo );
    changeFont();

    // leaving column mode
    if ( ! yesNo )
    {

        if ( m_undoCount > 0 && ! m_colHighlight )
        {
            removeColumnModeSpaces();
        }
    }

    // reset
    m_colHighlight = false;
    m_undoCount    = 0;
}

bool DiamondTextEdit::get_ColumnMode()
{
    return m_settingsPtr->isColumnMode();
}

void DiamondTextEdit::set_ShowLineNum( bool yesNo )
{
    Overlord::getInstance()->set_showLineNumbers( yesNo );
}

void DiamondTextEdit::removeColumnModeSpaces()
{
    QTextCursor cursor( textCursor() );

    int startRow = m_endRow;
    int startCol = m_endCol;

    for ( int k = 0; k < m_undoCount; k++ )
    {
        this->undo();
    }

    // from the start of the document go to startRow
    cursor.movePosition( QTextCursor::Start );
    cursor.movePosition( QTextCursor::NextBlock, QTextCursor::MoveAnchor, startRow-1 );

    // adjust to the correct column
    cursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, startCol );
    setTextCursor( cursor );

    m_undoCount = 0;
}

void DiamondTextEdit::cut()
{
    if ( m_settingsPtr->isColumnMode() )
    {

        QString text;
        QList<QTextEdit::ExtraSelection> oldSelections = this->extraSelections();

        // obtain text
        for ( int k = 0; k < oldSelections.size(); ++k )
        {

            if ( oldSelections[k].format.property( QTextFormat::UserProperty ).toString() != "highlightbar" )
            {
                text += oldSelections[k].cursor.selectedText() + "\n";
            }
        }

        // temporary, check mouse usage
        if ( text.isEmpty() )
        {

            QTextCursor cursor( textCursor() );
            QString selectedText = cursor.selectedText();

            if ( ! selectedText.isEmpty() )
            {
                QApplication::clipboard()->setText( selectedText );

                // save to copy buffer
                addToCopyBuffer( selectedText );

                cursor.removeSelectedText();
            }

        }
        else
        {

            QApplication::clipboard()->setText( text );

            // save to copy buffer
            addToCopyBuffer( text );

            // cut selected text
            QTextCursor cursorT( textCursor() );
            cursorT.beginEditBlock();

            for ( int k = 0; k < oldSelections.size(); ++k )
            {

                if ( oldSelections[k].format.property( QTextFormat::UserProperty ).toString() != "highlightbar" )
                {
                    oldSelections[k].cursor.removeSelectedText();
                }
            }

            cursorT.endEditBlock();
        }

    }
    else
    {
        QPlainTextEdit::cut();

    }
}

void DiamondTextEdit::copy()
{
    if ( m_settingsPtr->isColumnMode() )
    {

        QString text;
        QList<QTextEdit::ExtraSelection> oldSelections = this->extraSelections();

        // obtain text
        for ( int k = 0; k < oldSelections.count(); ++k )
        {

            if ( oldSelections[k].format.property( QTextFormat::UserProperty ).toString() != "highlightbar" )
            {
                text += oldSelections[k].cursor.selectedText() + "\n";
            }
        }

        // remove last newline
        text.chop( 1 );

        // temporary, check mouse usage
        if ( text.isEmpty() )
        {
            QTextCursor cursor( textCursor() );
            QString selectedText = cursor.selectedText();

            if ( ! selectedText.isEmpty() )
            {
                text = selectedText;
            }

            cursor.clearSelection();
        }

        QApplication::clipboard()->setText( text );

        // save to copy buffer
        addToCopyBuffer( text );

    }
    else
    {
        QPlainTextEdit::copy();

        // save to copy buffer
        addToCopyBuffer( QApplication::clipboard()->text() );
    }
}

void DiamondTextEdit::paste()
{
    if ( m_settingsPtr->isColumnMode() )
    {

        QString text = QApplication::clipboard()->text();
        QStringList lineList = text.split( "\n" );

        QTextCursor cursor( this->textCursor() );
        cursor.beginEditBlock();

        // reset posStart to the beginning
        int posStart = cursor.selectionStart();
        cursor.setPosition( posStart );

        int spaceLen = cursor.columnNumber();

        for ( int k = 0; k < lineList.count(); ++k )
        {

            cursor.insertText( lineList.at( k ) );

            if ( k < lineList.count() - 1 )
            {

                if ( ! cursor.movePosition( QTextCursor::NextBlock ) )
                {
                    // at end of document
                    cursor.movePosition( QTextCursor::EndOfBlock );
                    cursor.insertText( "\n" );
                }

                int length = cursor.block().length();

                if ( length <= spaceLen )
                {
                    // current line is not long enough, add spaces

                    for ( int j = length; j <= spaceLen; ++j )
                    {
                        cursor.insertText( " " );
                    }
                }

            }
        }

        cursor.endEditBlock();

    }
    else
    {
        QPlainTextEdit::paste();

    }
}

void DiamondTextEdit::addToCopyBuffer( const QString &text )
{
    m_copyBuffer.prepend( text );

    if ( m_copyBuffer.size() > 10 )
    {
        m_copyBuffer.removeLast();
    }
}

QList<QString> DiamondTextEdit::copyBuffer() const
{
    return m_copyBuffer;
}


// ** macros
void DiamondTextEdit::macroStart()
{
    nukeMacro();
    m_record = true;
}

void DiamondTextEdit::macroStop()
{
    m_record = false;
    storeMacro();
}

void DiamondTextEdit::storeMacro()
{
    if ( m_macroKeyList.count() < 1 )
    {
        return;
    }

    int ctr = 0;
    QString macroName;

    do
    {
        ctr++;
        macroName = QString( "Macro %1" ).formatArg( ctr );
    }
    while ( Overlord::getInstance()->macroExists( macroName ) );

    bool ok;
    macroName = QInputDialog::getText( this, tr( "Macro Save" ), tr( "Macro Name:" ),
                                       QLineEdit::Normal, macroName, &ok );

    if ( ok && !macroName.isEmpty() )
    {
        Overlord::getInstance()->addMacro( macroName, m_macroKeyList );
    }

}

QList<QKeyEvent *> DiamondTextEdit::get_MacroKeyList()
{
    return m_macroKeyList;
}


// ** process key press
bool DiamondTextEdit::event( QEvent *event )
{
    if ( event->type() == QEvent::ShortcutOverride )
    {

        QKeyEvent *keyPressEvent = dynamic_cast<QKeyEvent *>( event );

        int key = keyPressEvent->key();
        int modifiers = keyPressEvent->modifiers();

        if ( modifiers == Qt::ControlModifier && ( key == Qt::Key_A || key == Qt::Key_X || key == Qt::Key_C  || key == Qt::Key_V ) )
        {
            /// required to disable default selectAll(), cut(), copy(), paste()
            return false;
        }

    }
    else if ( event->type() == QEvent::KeyPress )
    {

        QKeyEvent *keyPressEvent = dynamic_cast<QKeyEvent *>( event );

        int key = keyPressEvent->key();
        int modifiers = keyPressEvent->modifiers();

        if ( modifiers == Qt::ControlModifier && ( key == Qt::Key_A || key == Qt::Key_X || key == Qt::Key_C  || key == Qt::Key_V ) )
        {
            qDebug() << "another spot we catch default selectAll, cut, copy, paste";
            /// required to disable default selectAll(), cut(), copy(), paste()
            return false;
        }

        if ( Overlord::getInstance()->edtMode() )
        {
            if ( handleEdtKey( key, modifiers ) )
            {
                if ( m_record )
                {
                    QKeyEvent *newEvent;
                    newEvent = new QKeyEvent( *keyPressEvent );

                    m_macroKeyList.append( newEvent );
                }

                return true;
            }
        }

        if ( m_settingsPtr->isColumnMode() )
        {

            if ( modifiers == Qt::ShiftModifier &&
                    ( ( key == Qt::Key_Up ) || ( key == Qt::Key_Down ) || ( key == Qt::Key_Left ) || ( key == Qt::Key_Right ) ) )
            {

                QTextCursor cursor( this->textCursor() );

                if ( ! m_colHighlight )
                {
                    m_startRow = cursor.blockNumber()+1;
                    m_startCol = cursor.columnNumber();

                    m_endRow = m_startRow;
                    m_endCol = m_startCol;

                    m_colHighlight = true;
                }

                QColor textColor = QColor( Qt::white );
                QColor backColor = QColor( Qt::red );

                QList<QTextEdit::ExtraSelection> extraSelections;
                QTextEdit::ExtraSelection selection;

                QList<QTextEdit::ExtraSelection> oldSelections = this->extraSelections();

                for ( int k=0; k < oldSelections.size(); ++k )
                {
                    if ( oldSelections[k].format.property( QTextFormat::UserProperty ).toString() == "highlightbar" )
                    {
                        extraSelections.append( oldSelections[k] );
                        break;
                    }
                }

                //
                selection.format.setForeground( textColor );
                selection.format.setBackground( backColor );
                selection.cursor = cursor;

                // **
                if ( key == Qt::Key_Up )
                {
                    --m_endRow;

                    // from the start of the document go to startRow
                    selection.cursor.movePosition( QTextCursor::Start );
                    selection.cursor.movePosition( QTextCursor::NextBlock, QTextCursor::MoveAnchor, m_startRow-1 );

                }
                else if ( key == Qt::Key_Down )
                {
                    ++m_endRow;

                    // from the start of the document go to startRow
                    selection.cursor.movePosition( QTextCursor::Start );
                    selection.cursor.movePosition( QTextCursor::NextBlock, QTextCursor::MoveAnchor, m_startRow-1 );

                }
                else if ( key == Qt::Key_Right )
                {
                    ++m_endCol;

                    if ( m_endCol-m_startCol > 0 )
                    {
                        selection.cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor, m_endCol-m_startCol );

                        int x1 = selection.cursor.selectionEnd();
                        int x2 = cursor.block().position() + cursor.block().length();

                        if ( x1 == x2 )
                        {
                            // back it up
                            --m_endCol;
                            selection.cursor.movePosition( QTextCursor::Left, QTextCursor::KeepAnchor, 1 );
                        }

                    }
                    else
                    {
                        selection.cursor.movePosition( QTextCursor::Left, QTextCursor::KeepAnchor, m_startCol-m_endCol );
                    }

                }
                else if ( key == Qt::Key_Left )
                {

                    if ( m_endCol > 0 )
                    {
                        --m_endCol;
                    }

                    if ( m_startCol-m_endCol > 0 )
                    {
                        selection.cursor.movePosition( QTextCursor::Left, QTextCursor::KeepAnchor, m_startCol-m_endCol );
                    }
                    else
                    {
                        selection.cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor, m_endCol-m_startCol );
                    }

                }

                // select text until cursor reaches endRow
                for ( int k = m_startRow; k <= m_endRow; ++k )
                {
                    int length = selection.cursor.block().length();

                    if ( length <= m_endCol )
                    {
                        // current line is not long enough, add spaces which will be removed
                        selection.cursor.beginEditBlock();
                        m_undoCount++;

                        for ( int j = length; j <= m_endCol; ++j )
                        {
                            selection.cursor.insertText( " " );
                        }

                        selection.cursor.endEditBlock();
                    }

                    selection.cursor.setPosition( selection.cursor.block().position() + m_startCol );
                    selection.cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor, m_endCol-m_startCol );

                    // need to save every line
                    extraSelections.append( selection );
                    selection.cursor.movePosition( QTextCursor::NextBlock, QTextCursor::MoveAnchor, 1 );
                }

                this->setExtraSelections( extraSelections );

                //  Note: If the selection obtained from an editor spans a line break, the text will contain a Unicode
                //  U+2029 paragraph separator character instead of a newline \n character. Use QString::replace() to
                //  replace these characters with newlines

                if ( m_startRow != m_endRow || m_startCol != m_endCol )
                {
                    copyAvailable( true );

                }
                else
                {
                    copyAvailable( false );

                }

                return true;
            }
        }
    }

    return QPlainTextEdit::event( event );
}

void DiamondTextEdit::keyPressEvent( QKeyEvent *event )
{
    int key = event->key();
    int modifiers = event->modifiers();

    if ( m_record )
    {
        QKeyEvent *newEvent;
        newEvent = new QKeyEvent( *event );

        m_macroKeyList.append( newEvent );
    }

    if ( m_settingsPtr->isColumnMode() )
    {

        if ( m_undoCount > 0 && ! m_colHighlight )
        {

            // copy may not be ctrl-c, test for this keySequence below
            // m_ui->actionCopy->setShortcut(QKeySequence(struct_temp.key_copy));

            bool ok = true;

            if ( key == Qt::Key_Alt || key == Qt::Key_Control || key == Qt::Key_Shift )
            {
                ok = false;

            }
            else if ( key == Qt::Key_C && ( modifiers == Qt::ControlModifier ) )
            {
                ok = false;

            }

            if ( ok )
            {
                removeColumnModeSpaces();
            }
        }
    }

    // process keys now
    if ( key == Qt::Key_Tab && ( modifiers == Qt::NoModifier ) )
    {
        indentIncr( "tab" );
        return;

    }
    else if ( key == Qt::Key_Backtab || ( key == Qt::Key_Tab  && ( modifiers == Qt::ShiftModifier ) ) )
    {
        indentDecr( "tab" );
        return;

    }

    // now call the parent
    QPlainTextEdit::keyPressEvent( event );
}

void DiamondTextEdit::mousePressEvent( QMouseEvent *event )
{
    if ( m_settingsPtr->isColumnMode() )
    {
        if ( m_undoCount > 0 && ! m_colHighlight )
        {
            removeColumnModeSpaces();
        }
    }

    // now call the parent
    QPlainTextEdit::mousePressEvent( event );
}


void DiamondTextEdit::keyReleaseEvent( QKeyEvent *event )
{
    int modifiers = event->modifiers();

    if ( m_settingsPtr->isColumnMode() )
    {

        if ( m_colHighlight && ( ( modifiers & Qt::ShiftModifier ) == 0 ) )
        {
            // shift key was not pressed, out of selection, reset
            m_colHighlight = false;
        }
    }

    // now call the parent
    QPlainTextEdit::keyReleaseEvent( event );
}

QStringList DiamondTextEdit::spell_getMaybe( QString word )
{
    QStringList retVal;

    if ( m_spellCheck )
    {
        if ( !m_spellCheck->spell( word ) )
        {
            retVal = m_spellCheck->suggest( word );
        }
    }

    return retVal;
}

void DiamondTextEdit::createSpellCheck()
{
    m_spellCheck = new SpellCheck( m_settingsPtr->mainDictionary(),  m_settingsPtr->userDictionary() );
}

void DiamondTextEdit::spell_addUserDict()
{
    // retrieve saved cursor
    QTextCursor cursor = get_Cursor();
    QString word = cursor.selectedText();

    if ( ! word.isEmpty() )
    {
        m_spellCheck->addToUserDict( word );
        cursor.insertText( word );
    }
}

void DiamondTextEdit::forceSyntax( SyntaxTypes data )
{
    QString synFName;

    switch ( data )
    {
        case SYN_C:
            synFName = m_settingsPtr->syntaxPath() + "syn_cpp.json";
            break;

        case SYN_CLIPPER:
            synFName = m_settingsPtr->syntaxPath() + "syn_clipper.json";
            break;

        case SYN_CMAKE:
            synFName = m_settingsPtr->syntaxPath() + "syn_cmake.json";
            break;

        case SYN_CSS:
            synFName = m_settingsPtr->syntaxPath() + "syn_css.json";
            break;

        case SYN_DOXY:
            synFName = m_settingsPtr->syntaxPath() + "syn_doxy.json";
            break;

        case SYN_ERRLOG:
            synFName = m_settingsPtr->syntaxPath() + "syn_errlog.json";
            break;

        case SYN_HTML:
            synFName = m_settingsPtr->syntaxPath() + "syn_html.json";
            break;

        case SYN_JAVA:
            synFName = m_settingsPtr->syntaxPath() + "syn_java.json";
            break;

        case SYN_JS:
            synFName = m_settingsPtr->syntaxPath() + "syn_js.json";
            break;

        case SYN_JSON:
            synFName = m_settingsPtr->syntaxPath() + "syn_json.json";
            break;

        case SYN_MAKE:
            synFName = m_settingsPtr->syntaxPath() + "syn_make.json";
            break;

        case SYN_NSIS:
            synFName = m_settingsPtr->syntaxPath() + "syn_nsi.json";
            break;

        case SYN_TEXT:
            synFName = m_settingsPtr->syntaxPath() + "syn_txt.json";
            break;

        case SYN_SHELL:
            synFName = m_settingsPtr->syntaxPath() + "syn_sh.json";
            break;

        case SYN_PERL:
            synFName = m_settingsPtr->syntaxPath() + "syn_pl.json";
            break;

        case SYN_PHP:
            synFName = m_settingsPtr->syntaxPath() + "syn_php.json";
            break;

        case SYN_PYTHON:
            synFName = m_settingsPtr->syntaxPath() + "syn_py.json";
            break;

        case SYN_XML:
            synFName = m_settingsPtr->syntaxPath() + "syn_xml.json";
            break;

        case SYN_NONE:
            synFName = m_settingsPtr->syntaxPath() + "syn_none.json";
            break;

        case SYN_UNUSED1:
            break;

        case SYN_UNUSED2:
            break;

    }

    if ( ! QFile::exists( synFName ) )
    {
        csError( tr( "Syntax Highlighting" ), tr( "Syntax highlighting file was not found: \n\n" ) + synFName  + "  " );

        this->setFocus();
    }
    else
    {
        m_syntaxEnum = data;
        set_SyntaxEnum( m_syntaxEnum );

        // check the box
        setSynType( m_syntaxEnum );

        queueRunSyntax( synFName );
    }
}

void DiamondTextEdit::runSyntax( QString synFName )
{
    // save syntax file name
    set_SyntaxFile( synFName );

    set_SyntaxParser( new Syntax( nullptr, synFName, m_spellCheck ) );
    m_syntaxParser->setDocument( document() );

}

void DiamondTextEdit::spell_replaceWord()
{
    QAction *action;
    action = ( QAction * )sender();

    if ( action )
    {
        // retrieve saved cursor
        QTextCursor cursor = get_Cursor();
        cursor.insertText( action->text() );
    }
}

void DiamondTextEdit::indentIncr( QString route )
{
    const QString tabLen = QString( m_settingsPtr->tabSpacing(), ' ' );
    QTextCursor cursor( textCursor() );

    cursor.beginEditBlock();

    if ( cursor.hasSelection() )
    {
        int posStart = cursor.selectionStart();
        int posEnd   = cursor.selectionEnd();

        // reset posEnd to the end to the last selected line
        cursor.setPosition( posEnd );
        cursor.movePosition( QTextCursor::EndOfLine );
        posEnd = cursor.position();

        // reset posStart to the beginning of the first selected line
        cursor.setPosition( posStart );
        cursor.movePosition( QTextCursor::StartOfLine );
        posStart = cursor.position();

        while ( true )
        {
            cursor.movePosition( QTextCursor::StartOfLine );

            if ( m_settingsPtr->useSpaces() )
            {
                cursor.insertText( tabLen );
                posEnd += m_settingsPtr->tabSpacing();

            }
            else
            {
                cursor.insertText( QChar( '\t' ) );
                posEnd += 1;

            }

            if ( ! cursor.movePosition( QTextCursor::NextBlock ) )
            {
                break;
            }

            if ( cursor.position() >= posEnd )
            {
                break;
            }
        }

        // reselect highlighted text
        cursor.clearSelection();

        cursor.setPosition( posStart );
        cursor.setPosition( posEnd, QTextCursor::KeepAnchor );

        setTextCursor( cursor );

    }
    else
    {

        if ( route == "indent" )
        {
            cursor.movePosition( QTextCursor::StartOfLine );
        }

        if ( m_settingsPtr->useSpaces() )
        {
            cursor.insertText( tabLen );

        }
        else
        {
            cursor.insertText( QChar( '\t' ) );

        }
    }

    cursor.endEditBlock();

}

void DiamondTextEdit::indentDecr( QString route )
{
    QTextCursor cursor( textCursor() );
    cursor.beginEditBlock();

    if ( cursor.hasSelection() )
    {
        int posStart = cursor.selectionStart();
        int posEnd   = cursor.selectionEnd();

        // reset posEnd to the end to the last selected line
        cursor.setPosition( posEnd );
        cursor.movePosition( QTextCursor::EndOfLine );
        posEnd = cursor.position();

        // reset posStart to the beginning of the first selected line
        cursor.setPosition( posStart );
        cursor.movePosition( QTextCursor::StartOfLine );
        posStart = cursor.position();

        QString tmp;

        while ( true )
        {
            cursor.movePosition( QTextCursor::StartOfLine );

            for ( int k = 0; k < m_settingsPtr->tabSpacing(); ++k )
            {

                int t_block = cursor.block().blockNumber();
                cursor.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1 );

                if ( t_block != cursor.block().blockNumber() )
                {
                    cursor.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1 );
                    break;
                }

                tmp = cursor.selectedText().trimmed();

                if ( ! tmp.isEmpty() )
                {
                    break;
                }

                if ( m_settingsPtr->useSpaces() )
                {
                    cursor.deleteChar();

                }
                else
                {
                    // what about tabs
                    cursor.deleteChar();
                }

                posEnd -=1;
            }

            //
            if ( ! cursor.movePosition( QTextCursor::NextBlock ) )
            {
                break;
            }

            if ( cursor.position() >= posEnd )
            {
                break;
            }
        }

        // reselect highlighted text
        cursor.clearSelection();

        cursor.setPosition( posStart );
        cursor.setPosition( posEnd, QTextCursor::KeepAnchor );

        setTextCursor( cursor );

    }
    else
    {

        int posStart = cursor.position();

        if ( route == "unindent" )
        {
            cursor.movePosition( QTextCursor::StartOfLine );
        }

        QString tmp;

        for ( int k=0; k < m_settingsPtr->tabSpacing(); ++k )
        {

            cursor.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1 );
            tmp = cursor.selectedText().trimmed();

            if ( ! tmp.isEmpty() )
            {
                break;
            }

            if ( m_settingsPtr->useSpaces() )
            {
                cursor.deleteChar();
            }
            else
            {
                // what about tabs
                cursor.deleteChar();
            }

            if ( route == "unindent" )
            {
                posStart -=1;
            }
        }

        //
        cursor.setPosition( posStart, QTextCursor::MoveAnchor );
        setTextCursor( cursor );
    }

    cursor.endEditBlock();

}

void DiamondTextEdit::deleteLine()
{
    QTextCursor cursor( textCursor() );
    cursor.beginEditBlock();

    cursor.movePosition( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
    cursor.movePosition( QTextCursor::EndOfLine, QTextCursor::KeepAnchor );
    cursor.removeSelectedText();
    setTextCursor( cursor );

    cursor.endEditBlock();
}

void DiamondTextEdit::deleteEOL()
{
    QTextCursor cursor( textCursor() );
    cursor.beginEditBlock();

    cursor.movePosition( QTextCursor::EndOfLine, QTextCursor::KeepAnchor );
    cursor.removeSelectedText();
    setTextCursor( cursor );

    cursor.endEditBlock();
}

void DiamondTextEdit::deleteThroughEOL()
{
    m_edtSelectActive = false;
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();

    if ( Overlord::getInstance()->edtDirection() )
    {
        cursor.movePosition( QTextCursor::Up, QTextCursor::KeepAnchor, 1 );
        cursor.movePosition( QTextCursor::EndOfLine, QTextCursor::KeepAnchor, 1 );
    }
    else
    {
        cursor.movePosition( QTextCursor::Down, QTextCursor::KeepAnchor, 1 );
        cursor.movePosition( QTextCursor::StartOfLine, QTextCursor::KeepAnchor, 1 );
    }

    // if we don't replace the paragraph marker with new line we get
    // garbage in the JSON file. The garbage keeps growing too. Eventually
    // you have a 900,000+ byte "line" and the editor barely loads.
    //
    QString txt = cursor.selectedText();
    txt = txt.replace( PARAGRAPH_MARKER, "\n" );
    Overlord::getInstance()->set_edtLastDeletedLine( txt );
    cursor.removeSelectedText();
    cursor.endEditBlock();
    setTextCursor( cursor );
}

void DiamondTextEdit::insertSymbol()
{
    Dialog_Symbols dw( this );
    int result = dw.exec();

    if ( result == QDialog::Accepted )
    {
        QString text = dw.get_Symbol();

        if ( ! text.isEmpty() )
        {

            if ( text.toLower() == "tab character" )
            {
                textCursor().insertText( QChar( 9 ) );

            }
            else
            {
                textCursor().insertText( text );
            }
        }
    }

}

void DiamondTextEdit::selectAll()
{
    QTextCursor cursor( textCursor() );
    cursor.select( QTextCursor::Document );
    setTextCursor( cursor );
    m_edtSelectActive = true;
}

void DiamondTextEdit::selectBlock()
{
    QTextCursor cursor( textCursor() );
    cursor.select( QTextCursor::BlockUnderCursor );
    setTextCursor( cursor );
    m_edtSelectActive = true;
}

void DiamondTextEdit::selectLine()
{
    QTextCursor cursor( textCursor() );
    cursor.select( QTextCursor::LineUnderCursor );
    setTextCursor( cursor );
    m_edtSelectActive = true;
}

void DiamondTextEdit::selectWord()
{
    QTextCursor cursor( textCursor() );
    cursor.select( QTextCursor::WordUnderCursor );
    setTextCursor( cursor );
    m_edtSelectActive = true;
}

void DiamondTextEdit::caseChange()
{
    m_edtSelectActive = false;
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();

    if ( cursor.hasSelection() )
    {
        QString txt = cursor.selectedText();
        QString newText;
        cursor.removeSelectedText();

        for ( int i=0; i < txt.size(); i++ )
        {
            // have to skip over non-letters
            //
            if ( txt.at( i ).isUpper() )
            {
                newText.append( txt.at( i ).toLower() );
            }
            else if ( txt.at( i ).isLower() )
            {
                newText.append( txt.at( i ).toUpper() );
            }
            else
            {
                newText.append( txt.at( i ) );
            }
        }

        cursor.insertText( newText );
    }
    else
    {
        // process character "under" cursor
        // problem is cursor is always between characters
        // this makes direction important
        //
        QString txt;

        if ( Overlord::getInstance()->edtDirection() )
        {
            cursor.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1 );
        }
        else
        {
            cursor.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1 );
        }

        txt = cursor.selectedText();
        cursor.removeSelectedText();

        if ( txt.at( 0 ).isUpper() )
        {
            cursor.insertText( txt.at( 0 ).toLower() );
        }
        else
        {
            cursor.insertText( txt.at( 0 ).toUpper() );
        }

    }

    cursor.endEditBlock();
    setTextCursor( cursor );

}
void DiamondTextEdit::caseUpper()
{
    QTextCursor cursor( textCursor() );
    QString text = cursor.selectedText();

    if ( text.isEmpty() )
    {
        cursor.select( QTextCursor::WordUnderCursor );
        text = cursor.selectedText();
    }

    cursor.removeSelectedText();
    cursor.insertText( text.toUpper() );
    setTextCursor( cursor );
    clearEdtSelection();
}

void DiamondTextEdit::caseLower()
{
    QTextCursor cursor( textCursor() );
    QString text = cursor.selectedText();

    if ( text.isEmpty() )
    {
        cursor.select( QTextCursor::WordUnderCursor );
        text = cursor.selectedText();
    }

    cursor.removeSelectedText();
    cursor.insertText( text.toLower() );
    setTextCursor( cursor );
    clearEdtSelection();

}

void DiamondTextEdit::caseCap()
{
    QTextCursor cursor( textCursor() );
    QString text = cursor.selectedText();

    if ( text.isEmpty() )
    {
        cursor.select( QTextCursor::WordUnderCursor );
        text = cursor.selectedText();
    }

    text = text.toLower();
    text.replace( 0, 1, text[0].toUpper() );

    cursor.removeSelectedText();
    cursor.insertText( text );
    setTextCursor( cursor );
    clearEdtSelection();
}

void DiamondTextEdit::insertDate()
{
    QDate date  = QDate::currentDate();
    QString tmp = date.toString( m_settingsPtr->formatDate() );

    insertPlainText( tmp );
    clearEdtSelection();
}

void DiamondTextEdit::insertTime()
{
    QTime time  = QTime::currentTime();
    QString tmp = time.toString( m_settingsPtr->formatTime() );

    insertPlainText( tmp );
    clearEdtSelection();
}

void DiamondTextEdit::rewrapParagraph()
{
    QTextCursor cursor( textCursor() );
    cursor.beginEditBlock();

    if ( m_settingsPtr->rewrapColumn() == 0 )
    {
        Overlord::getInstance()->set_rewrapColumn( 120 );
    }

    if ( cursor.hasSelection() )
    {
        int posStart = cursor.selectionStart();
        int posEnd   = cursor.selectionEnd();

        // reset posEnd to the end to the last selected line
        cursor.setPosition( posEnd );
        cursor.movePosition( QTextCursor::EndOfLine );
        posEnd = cursor.position();

        // reset posStart to the beginning of the first selected line
        cursor.setPosition( posStart );
        cursor.movePosition( QTextCursor::StartOfLine );
        posStart = cursor.position();

        // select all of the text
        cursor.setPosition( posEnd, QTextCursor::KeepAnchor );

        QString tmp = cursor.selectedText();
        tmp.replace( '\n', ' ' );           // line feed
        tmp.replace( QChar( 0x2029 ), " " ); // paragraph

        QString tmpSave = tmp.simplified();

        //
        QStringList lines;

        while ( ! tmp.isEmpty() )
        {
            lines.append( tmp.left( m_settingsPtr->rewrapColumn() ) );
            tmp.remove( 0, m_settingsPtr->rewrapColumn() );
        }

        QString hold;

        for ( QString &str : lines )
        {
            str.prepend( hold );
            hold = "";

            while ( ! str.isEmpty() )
            {
                int len    = str.size();
                QChar last = str[len - 1];

                while ( len >= m_settingsPtr->rewrapColumn() )
                {
                    // line is too long
                    hold.prepend( last );
                    str.chop( 1 );

                    --len;
                    last = str[len - 1];
                }

                if ( last.isSpace() )
                {
                    str.chop( 1 );
                    break;

                }
                else
                {
                    // keep the chars which are moving down
                    hold.prepend( last );
                    str.chop( 1 );

                }
            }

        }

        if ( ! hold.isEmpty() )
        {
            // retrieve the last line
            QString lastLine = lines.takeLast() + " " + hold;
            hold = "";

            // is this line too long now
            int len    = lastLine.size();
            QChar last = lastLine[len - 1];

            while ( len >=  m_settingsPtr->rewrapColumn() )
            {
                // line is too long
                hold.prepend( last );
                lastLine.chop( 1 );

                --len;
                last = lastLine[len - 1];
            }

            if ( ! hold.isEmpty() )
            {
                while ( true )
                {
                    len  = lastLine.size();
                    last = lastLine[len - 1];

                    if ( last.isSpace() )
                    {
                        lastLine.chop( 1 );
                        break;

                    }
                    else
                    {
                        // keep the chars which are moving down
                        hold.prepend( last );
                        lastLine.chop( 1 );

                    }
                }
            }

            // update
            lines.append( lastLine );

            if ( ! hold.isEmpty() )
            {
                lines.append( hold );
            }
        }

        tmp = lines.join( "\n" );

        if ( tmpSave == tmp.simplified() )
        {
            // remove the selected text
            cursor.removeSelectedText();

            // insert new text
            cursor.insertText( tmp );

        }
        else
        {
            csMsg( "Warning: New text for rewrapParagrah is not the same as the input, rewrap aborted" );

        }

    }
    else
    {
        csMsg( "No text or paragraph was selected to rewrap" );

    }

    cursor.endEditBlock();
    clearEdtSelection();
}

void DiamondTextEdit::goLine()
{
    int line = get_line_col( this, "line" );

    if ( line > 0 )
    {
        QTextCursor cursor( document()->findBlockByNumber( line - 1 ) );
        setTextCursor( cursor );
    }
}

void DiamondTextEdit::goColumn()
{
    int col = get_line_col( this, "col" );

    QTextCursor cursor( textCursor() );
    cursor.movePosition( QTextCursor::StartOfLine );

    cursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, col );
    setTextCursor( cursor );
}

void DiamondTextEdit::goTop()
{
    QTextCursor cursor( textCursor() );
    cursor.movePosition( QTextCursor::Start );
    setTextCursor( cursor );
}

void DiamondTextEdit::goBottom()
{
    QTextCursor cursor( textCursor() );
    cursor.movePosition( QTextCursor::End );
    setTextCursor( cursor );
}


void DiamondTextEdit::moveBar()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;

    QColor textColor;
    QColor backColor;

    textColor = m_settingsPtr->currentTheme()->colorText();
    backColor = m_settingsPtr->currentTheme()->colorBack();

    if ( m_settingsPtr->showLineHighlight() )
    {
        // on
        backColor = m_settingsPtr->currentTheme()->currentLineBack();
    }

    // We really should leave syntax highlight in place, just changing background
    // color.
    //
    //selection.format.setForeground( textColor );
    selection.format.setBackground( backColor );
    selection.format.setProperty( QTextFormat::FullWidthSelection, true );
    selection.format.setProperty( QTextFormat::UserProperty, QString( "highlightbar" ) );

    selection.cursor = textCursor();
    selection.cursor.clearSelection();

    extraSelections.append( selection );
    setExtraSelections( extraSelections );
}

void DiamondTextEdit::fixTab_Spaces()
{
    QTextCursor cursor( textCursor() );

    // save starting position
    QTextCursor cursorStart;
    cursorStart = cursor;

    // position to start of document
    cursor.movePosition( QTextCursor::Start );
    setTextCursor( cursor );

    // set for undo stack
    cursor.beginEditBlock();

    int tabLen = m_settingsPtr->tabSpacing();
    const QString findText = QString( QChar( 9 ) );

    while ( true )
    {
        bool found = find( findText );

        if ( found )
        {
            cursor = textCursor();

            // how many chars from the start of line
            int span = cursor.positionInBlock();

            int x1 = ( span/tabLen ) * tabLen;
            int x2 = tabLen - ( span - x1 ) + 1;

            const QString newText = QString( x2, ' ' );
            cursor.insertText( newText );

        }
        else
        {
            break;
        }
    }

    cursor.endEditBlock();

    // go back to starting point
    setTextCursor( cursorStart );
}

void DiamondTextEdit::fixSpaces_Tab()
{
    QTextCursor cursor( textCursor() );

    // save starting position
    QTextCursor cursorStart;
    cursorStart = cursor;

    // position to start of document
    cursor.movePosition( QTextCursor::Start );
    setTextCursor( cursor );

    // set for undo stack
    cursor.beginEditBlock();

    int pos;
    int tabLen = m_settingsPtr->tabSpacing();

    QString tmp;

    const QString findText = QString( 2, ' ' );
    const QString newText  = QString( QChar( 9 ) );

    while ( true )
    {
        bool found = find( findText );

        // reset
        int pass = 1;

        if ( found )
        {
            cursor = textCursor();

            while ( true )
            {
                tmp = cursor.selectedText().trimmed();
                pos  = cursor.positionInBlock();

                if ( m_tabStops.contains( pos )  )
                {
                    // this a tap stop

                    if ( tmp.isEmpty() )
                    {
                        // replace spaces with tab
                        cursor.insertText( newText );

                    }
                    else
                    {
                        // char at the tabstop is not a space, back up one
                        cursor.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1 );

                        // replace spaces with tab
                        cursor.insertText( newText );
                    }

                    break;
                }

                if ( ! tmp.isEmpty() || pass == tabLen )
                {
                    break;
                }

                // get one more char
                cursor.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1 );
                ++pass;
            }

        }
        else
        {
            // all done
            break;

        }
    }

    cursor.endEditBlock();

    // go back to starting point
    setTextCursor( cursorStart );
}

void DiamondTextEdit::deleteEOL_Spaces()
{
    QTextCursor cursor( textCursor() );

    // position to start of document
    cursor.movePosition( QTextCursor::Start );

    // set for undo stack
    cursor.beginEditBlock();

    QString tmp;

    while ( true )
    {
        cursor.movePosition( QTextCursor::EndOfBlock );

        while ( true )
        {
            cursor.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1 );
            tmp = cursor.selectedText();

            if ( tmp != " " )
            {
                cursor.movePosition( QTextCursor::NextCharacter );
                break;
            }

            cursor.removeSelectedText();
        }

        if ( ! cursor.movePosition( QTextCursor::NextBlock ) )
        {
            break;
        }
    }

    cursor.endEditBlock();
}

QString DiamondTextEdit::currentFile()
{
    return m_curFile;
}

void DiamondTextEdit::setCurrentFile( QString fileName )
{
    m_curFile = fileName;
}

// **document
void DiamondTextEdit::setSyntax()
{
    QString fname  = "";
    QString suffix = "txt";

    if ( ! m_curFile.isEmpty() )
    {

        fname  = strippedName( m_curFile ).toLower();
        suffix = suffixName( m_curFile );

        const QList<QString> cppList = {"h", "hh", "hpp", "c", "cc", "c++", "cpp", "l", "m", "mm" };

        if ( cppList.contains( suffix ) )
        {
            suffix = "cpp";

        }
        else if ( suffix == "prg" )
        {
            suffix = "clipper";

        }
        else if ( fname == "cmakelists.txt" || suffix == "cmake" )
        {
            suffix = "cmake";

        }
        else if ( suffix == "dox" || suffix == "doxypress" )
        {
            suffix = "doxy";

        }
        else if ( suffix == "err" || suffix == "log" )
        {
            suffix = "errlog";

        }
        else if ( suffix == "htm" || suffix == "shtml" )
        {
            suffix = "html";

        }
        else if ( fname == "configure" || fname == "configure.ac"  || suffix == "ac" )
        {
            suffix = "make";

        }
        else if ( fname == "makefile" || fname == "makefile.in" || suffix == "am" )
        {
            suffix = "make";

        }
        else if ( suffix == "nsis" )
        {
            suffix = "nsi";

        }
        else if ( suffix == "perl" || suffix == "pm" )
        {
            suffix = "pl";

        }
        else if ( suffix == "php3" || suffix == "php4" || suffix == "php5" )
        {
            suffix = "php";

        }
        else if ( suffix == "xml" || suffix == "ts" )
        {
            suffix = "xml";

        }
    }

    QString synFName = m_settingsPtr->syntaxPath() + "syn_"+ suffix + ".json";

    if ( ! QFile::exists( synFName ) )
    {
        // use default
        suffix   = "txt";
        synFName = m_settingsPtr->syntaxPath() + "syn_txt.json";
        qDebug() << " defaulted to : " << synFName;
    }
    else
    {
        m_synFName = synFName;
    }

    //
    if ( ! QFile::exists( synFName ) )
    {
        qDebug() << "defaulting to SYN_NONE";
        setSynType( SYN_NONE );

        m_syntaxEnum = SYN_NONE;
        set_SyntaxEnum( m_syntaxEnum );

    }
    else
    {

        if ( suffix == "cpp" )
        {
            // setSynType(SYN_C);
            m_syntaxEnum = SYN_C;

        }
        else if ( suffix == "clipper" )
        {
            m_syntaxEnum = SYN_CLIPPER;

        }
        else if ( suffix == "cmake" )
        {
            m_syntaxEnum = SYN_CMAKE;

        }
        else if ( suffix == "css" )
        {
            m_syntaxEnum = SYN_CSS;

        }
        else if ( suffix == "doxy" )
        {
            m_syntaxEnum = SYN_DOXY;

        }
        else if ( suffix == "errlog" )
        {
            m_syntaxEnum = SYN_ERRLOG;

        }
        else if ( suffix == "html" )
        {
            m_syntaxEnum = SYN_HTML;

        }
        else if ( suffix == "java" )
        {
            m_syntaxEnum = SYN_JAVA;

        }
        else if ( suffix == "js" )
        {
            m_syntaxEnum = SYN_JS;

        }
        else if ( suffix == "json" )
        {
            m_syntaxEnum = SYN_JSON;

        }
        else if ( suffix == "make" )
        {
            m_syntaxEnum = SYN_MAKE;

        }
        else if ( suffix == "nsi" )
        {
            m_syntaxEnum = SYN_NSIS;

        }
        else if ( suffix == "txt" )
        {
            m_syntaxEnum = SYN_TEXT;

        }
        else if ( suffix == "sh" )
        {
            m_syntaxEnum = SYN_SHELL;

        }
        else if ( suffix == "pl" )
        {
            m_syntaxEnum = SYN_PERL;

        }
        else if ( suffix == "php" )
        {
            m_syntaxEnum = SYN_PHP;

        }
        else if ( suffix == "py" )
        {
            m_syntaxEnum = SYN_PYTHON;

        }
        else if ( suffix == "xml" )
        {
            m_syntaxEnum = SYN_XML;

        }

        // save the menu enum
        setSynType( m_syntaxEnum );
        set_SyntaxEnum( m_syntaxEnum );

        queueRunSyntax( synFName );
    }
}

void DiamondTextEdit::setUpTabStops()
{
    if ( m_lastTabSpacing != m_settingsPtr->tabSpacing() )
    {
        int tabStop;

        m_tabStops.clear();

        m_lastTabSpacing = m_settingsPtr->tabSpacing();

        for ( int k = 1; k < 25; ++k )
        {
            tabStop = ( m_lastTabSpacing * k ) + 1;
            m_tabStops.append( tabStop );
        }
    }
}

void DiamondTextEdit::changeSettings( Settings *settings )
{
    m_settingsPtr = settings;

    setUpTabStops();

    if ( m_lastTheme != *m_settingsPtr->currentTheme() )
    {
        setScreenColors();
        m_lastTheme = *m_settingsPtr->currentTheme();
        queueRunSyntax( m_synFName );
        moveBar();
    }

    if ( ( m_lastNormalFont != m_settingsPtr->fontNormal() )
            || ( m_lastColumnFont != m_settingsPtr->fontColumn() ) )
    {
        changeFont();
        m_lastNormalFont = m_settingsPtr->fontNormal();
        m_lastColumnFont = m_settingsPtr->fontColumn();
    }
}

void DiamondTextEdit::setScreenColors()
{
    QPalette tmp = palette();
    tmp.setColor( QPalette::Text, m_settingsPtr->currentTheme()->colorText() );
    tmp.setColor( QPalette::Base, m_settingsPtr->currentTheme()->colorBack() );
    setPalette( tmp );
}

void DiamondTextEdit::changeFont()
{
    if ( get_ColumnMode() )
    {
        setFont( m_settingsPtr->fontColumn() );
    }
    else
    {
        setFont( m_settingsPtr->fontNormal() );
    }
}

// there will be many return statements in this method.
//
bool DiamondTextEdit::handleEdtKey( int key, int modifiers )
{
    if ( isReadOnly() )
    {
        timedMessage( tr( "Buffer is read Only" ), MESSAGE_TIME );
        qDebug() << "caught typing in read only buffer";
        return true;
    }

    // don't let modifier keypress hose up GOLD key logic
    //
    QString keyStr = QKeySequence( key, modifiers ).toString( QKeySequence::NativeText );

    bool isKeypad = ( modifiers & Qt::KeypadModifier );
    bool isShift  = ( modifiers & Qt::ShiftModifier );
    bool isCtrl   = ( modifiers & Qt::ControlModifier );

    QTextCursor::MoveMode mode = QTextCursor::MoveAnchor;

    // if we also check for a selection in textcursor we
    // then run headlong into the problem of an arrow key
    // not clearing the find selection.
    // textCursor().hasSelection()
    if ( m_edtSelectActive )
    {
        mode = QTextCursor::KeepAnchor;
    }


    // skip keys that are just modifiers
    // Handle keys the QTextEdit would handle but due to EDT select cannot
    //
    if ( !isKeypad )
    {
        switch ( key )
        {
            case Qt::Key_Left:
                if ( isShift )
                {
                    m_edtSelectActive = true;
                    edtKeyLeft( QTextCursor::KeepAnchor );
                    return true;
                }
                else if ( modifiers == 0 )
                {
                    edtKeyLeft( mode );
                    return true;
                }

                break;


            case Qt::Key_Right:
                if ( isShift )
                {
                    m_edtSelectActive = true;
                    edtKeyRight( QTextCursor::KeepAnchor );
                    return true;
                }
                else if ( modifiers == 0 )
                {
                    edtKeyRight( mode );
                    return true;
                }

                break;

            case Qt::Key_Up:
                if ( isShift )
                {
                    m_edtSelectActive = true;
                    edtLineUp( QTextCursor::KeepAnchor );
                    return true;
                }
                else if ( modifiers == 0 )
                {
                    edtLineUp( mode );
                    return true;
                }

                break;

            case Qt::Key_Down:
                if ( isShift )
                {
                    m_edtSelectActive = true;
                    edtLineDown( QTextCursor::KeepAnchor );
                    return true;
                }
                else if ( modifiers == 0 )
                {
                    edtLineDown( mode );
                    return true;
                }

                break;

            case Qt::Key_PageUp:
                if ( isShift )
                {
                    m_edtSelectActive = true;
                    edtSectionUp( QTextCursor::KeepAnchor );
                    return true;
                }
                else if ( modifiers == 0 )
                {
                    edtSectionUp( mode );
                    return true;
                }

                break;

            case Qt::Key_PageDown:
                if ( isShift )
                {
                    m_edtSelectActive = true;
                    edtSectionDown( QTextCursor::KeepAnchor );
                    return true;
                }
                else if ( modifiers == 0 )
                {
                    edtSectionDown( mode );
                    return true;
                }

                break;

            case Qt::Key_End:
                if ( isShift )
                {
                    m_edtSelectActive = true;
                    edtEndOfLine( QTextCursor::KeepAnchor );
                    return true;
                }
                else if ( modifiers == 0 )
                {
                    edtEndOfLine( mode );
                    return true;
                }
                else if ( isCtrl )
                {
                    edtBottom();
                    return true;
                }

                break;

            case Qt::Key_Home:
                if ( isShift )
                {
                    m_edtSelectActive = true;
                    edtHome( QTextCursor::KeepAnchor );
                    return true;
                }
                else if ( modifiers == 0 )
                {
                    edtHome( mode );
                    return true;
                }
                else if ( isCtrl )
                {
                    edtTop();
                    return true;
                }

                break;

            // TODO:: if user has other than default keys assigned this won't work.
            //
            case Qt::Key_C:
                if ( isCtrl )
                {
                    edtCopy();
                    QTextCursor cursor = textCursor();
                    cursor.clearSelection();
                    setTextCursor( cursor );
                    m_edtSelectActive = false;
                    return true;
                }

                break;

            case Qt::Key_A:
                if ( isCtrl )
                {
                    selectAll();
                    m_edtSelectActive = true;
                    return true;
                }

                break;


            case Qt::Key_Control:
            case Qt::Key_Alt:
            case Qt::Key_AltGr:
            case Qt::Key_Meta:
            case Qt::Key_CapsLock:
            case Qt::Key_Shift:
                return false;
                break;

            default:
                break;

        }
    }

    // Have to keep track of if previous keypress was GOLD
    //
    bool lastKeyWasGold = m_currentKeyGold;

    m_currentKeyGold = false;

    if ( key == Overlord::getInstance()->keys().goldKey() )
    {
        m_currentKeyGold = true;
        return true;
    }


    if ( textCursor().hasSelection() || m_edtSelectActive )
    {
        mode = QTextCursor::KeepAnchor;
    }

    if ( lastKeyWasGold )
    {
        // GOLD Help has no meaning in EDT
        // in LSE it brought up language sensitive help for what you had highlighted
        // or had your cursor on.
        switch ( key )
        {
            case Qt::Key_0:     // OPEN LINE
            case Qt::Key_Insert:
                if ( isKeypad )
                {
                    m_edtSelectActive = false;
                    QTextCursor cursor = textCursor();
                    cursor.beginEditBlock();
                    cursor.insertBlock();
                    cursor.movePosition( QTextCursor::Up );
                    cursor.movePosition( QTextCursor::EndOfLine );
                    cursor.endEditBlock();
                    setTextCursor( cursor );
                    return true;
                }

                break;

            case Qt::Key_Period:
            case Qt::Key_Delete:
                if ( isKeypad )
                {
                    clearEdtSelection();
                    return true;
                }

                break;

            case Qt::Key_Enter:     // SUBS
                if ( isKeypad )
                {
                    m_edtSelectActive = false;
                    edtSubs();
                    return true;
                }

                break;

            case Qt::Key_1:     // change case
            case Qt::Key_End:
                if ( isKeypad )
                {
                    caseChange();
                    return true;
                }

                break;

            case Qt::Key_2:     // Delete to EOL
            case Qt::Key_Down:
                if ( isKeypad )
                {
                    m_edtSelectActive = false;
                    QTextCursor cursor = textCursor();
                    cursor.beginEditBlock();
                    cursor.movePosition( QTextCursor::EndOfLine, QTextCursor::KeepAnchor, 1 );
                    Overlord::getInstance()->set_edtLastDeletedLine( cursor.selectedText() );
                    addToCopyBuffer( cursor.selectedText() );
                    cursor.removeSelectedText();
                    cursor.endEditBlock();
                    setTextCursor( cursor );
                    return true;
                }

                break;

            case Qt::Key_3:         // SPECINS
            case Qt::Key_PageDown:
                if ( isKeypad )
                {
                    m_edtSelectActive = false;
                    Dialog_Edt_Prompt *dw = new Dialog_Edt_Prompt( "Character Code: ", false, this );
                    dw->exec();

                    bool okFlag = false;
                    int c = dw->text().toInteger<int>( &okFlag );

                    if ( okFlag && ( c > 0 ) )
                    {
                        QTextCursor cursor = textCursor();
                        cursor.beginEditBlock();
                        cursor.insertText( QString( QChar( c ) ) );
                        cursor.endEditBlock();
                        setTextCursor( cursor );
                    }

                    return true;
                }

                break;

            case Qt::Key_4:     // BOTTOM
            case Qt::Key_Left:
                if ( isKeypad )
                {
                    edtBottom();
                    return true;
                }

                break;

            case Qt::Key_5:     // TOP
            case Qt::Key_Clear:
                if ( isKeypad )
                {
                    edtTop();
                    return true;
                }

                break;

            case Qt::Key_6:     // paste
            case Qt::Key_Right:
                if ( isKeypad )
                {
                    m_edtSelectActive = false;
                    edtPaste();
                    return true;
                }

                break;

            case Qt::Key_7:     // command
            case Qt::Key_Home:
                if ( isKeypad )
                {
                    not_done( "COMMAND" );
                    return true;
                }

                break;

            case Qt::Key_8:         // FILL
            case Qt::Key_Up:
                if ( isKeypad )
                {
                    not_done( "FILL" );
                    return true;
                }

                break;

            case Qt::Key_9:     // REPLACE
            case Qt::Key_PageUp:
                if ( isKeypad )
                {
                    m_edtSelectActive = false;
                    QTextCursor cursor = textCursor();

                    if ( cursor.hasSelection() )
                    {
                        cursor.beginEditBlock();
                        QString txt = QApplication::clipboard()->text();
                        cursor.removeSelectedText();
                        cursor.insertText( txt );
                        cursor.endEditBlock();
                        setTextCursor( cursor );
                    }
                    else
                    {
                        timedMessage( tr( "No active select range" ), MESSAGE_TIME );
                        this->setFocus();
                    }

                    return true;
                }

                break;

            case Qt::Key_Slash:
                // has no meaning in EDT world
                // When this is later expanded to IDE make this syntax help.
                break;

            case Qt::Key_Asterisk:
                if ( isKeypad )
                {
                    // don't clear EDT select active.
                    // many delopers set a mark then search for some text to find the end
                    // of the selection range.
                    Dialog_Edt_Prompt dw( "Search for: ", true, this );
                    QString selectedText = textCursor().selectedText();

                    if ( ! selectedText.isEmpty() )
                    {
                        dw.setText( selectedText );
                    }

                    dw.exec();

                    QString txt = dw.text();

                    if ( dw.ctrlMSubstitution() )
                    {
                        txt = txt.replace( "^M", "\n" );
                    }

                    if ( !txt.isEmpty() )
                    {
                        Overlord::getInstance()->set_findText( txt );
                        int index = Overlord::getInstance()->findListFind( txt );

                        if ( index == -1 )
                        {
                            Overlord::getInstance()->findList().prepend( txt );
                        }
                        else
                        {
                            Overlord::getInstance()->findList().move( index,0 );
                        }

                        Overlord::getInstance()->set_findFlags( 0 );

                        switch ( dw.terminator() )
                        {
                            case Edt_LineEdit::ADVANCE:
                                Overlord::getInstance()->set_edtDirection( false );
                                break;

                            case Edt_LineEdit::BACKUP:
                                Overlord::getInstance()->set_edtDirection( true );
                                Overlord::getInstance()->set_findFlagsBackward();
                                break;

                            case Edt_LineEdit::ENTER:
                                if ( Overlord::getInstance()->edtDirection() )
                                {
                                    Overlord::getInstance()->set_findFlagsBackward();
                                }

                                break;

                            case Edt_LineEdit::NONE:
                            default:
                                break;
                        }

                        bool found = find( txt, Overlord::getInstance()->findFlags() );

                        if ( !found )
                        {
                            timedMessage( "Not found: " + Overlord::getInstance()->findText(), MESSAGE_TIME );
                            this->setFocus();
                        }
                    }

                    return true;
                }

                break;

            case Qt::Key_Plus:
                if ( isKeypad )
                {
                    // work around for QKeyEvent not being able to give us a QKeySequence
                    //
                    m_edtSelectActive = false;

                    if ( ( Overlord::getInstance()->edtWordCtrlMeta()
                            && ( ( modifiers & Qt::ControlModifier )
                                 || ( modifiers & Qt::MetaModifier ) ) )
                            || ( Overlord::getInstance()->edtWordAltOption()
                                 && ( modifiers & Qt::AltModifier ) ) )
                    {
                        if ( !Overlord::getInstance()->edtLastDeletedWord().isEmpty() )
                        {
                            QTextCursor cursor = textCursor();
                            cursor.beginEditBlock();
                            cursor.insertText( Overlord::getInstance()->edtLastDeletedWord() );
                            cursor.endEditBlock();
                            setTextCursor( cursor );
                        }
                        else
                        {
                            timedMessage( tr( "No word to UNDELETE" ), MESSAGE_TIME );
                            this->setFocus();
                        }
                    }
                    else if ( !Overlord::getInstance()->edtLastDeletedChar().isEmpty() )
                    {
                        QTextCursor cursor = textCursor();
                        cursor.beginEditBlock();
                        cursor.insertText( Overlord::getInstance()->edtLastDeletedChar() );
                        cursor.endEditBlock();
                        setTextCursor( cursor );
                    }
                    else
                    {
                        timedMessage( tr( "No char to UNDELETE " ), MESSAGE_TIME );
                        this->setFocus();
                    }

                    return true;
                }

                break;

            case Qt::Key_Minus:
                if ( isKeypad )
                {
                    m_edtSelectActive = false;

                    if ( !Overlord::getInstance()->edtLastDeletedLine().isEmpty() )
                    {
                        QTextCursor cursor = textCursor();
                        cursor.beginEditBlock();
                        cursor.insertText( Overlord::getInstance()->edtLastDeletedLine() );
                        cursor.endEditBlock();
                        setTextCursor( cursor );
                    }
                    else
                    {
                        timedMessage( tr( "No line to UNDELETE " ), MESSAGE_TIME );
                        this->setFocus();
                    }

                    return true;
                }

                break;

            default:
                break;
        }


        if ( keyStr == Overlord::getInstance()->keys().edtGotoLine() )
        {
            // don't clear EDT select active here
            //
            Dialog_Edt_Prompt *dw = new Dialog_Edt_Prompt( "Line: ", false, this );
            dw->exec();
            QString txt = dw->text();

            if ( !txt.isEmpty() )
            {
                int line = txt.toInteger<int>();

                if ( line > 0 )
                {
                    QTextCursor cursor( document()->findBlockByNumber( line - 1 ) );
                    setTextCursor( cursor );
                }
            }

            return true;
        }

        if ( keyStr == Overlord::getInstance()->keys().edtCopy() )
        {
            edtCopy();
            QTextCursor cursor = textCursor();
            cursor.clearSelection();
            setTextCursor( cursor );
            m_edtSelectActive = false;
            return true;
        }


        if ( keyStr == Overlord::getInstance()->keys().edtUpperCase() )
        {
            edtUpcase();
            m_edtSelectActive = false;
            return true;
        }

        if ( keyStr == Overlord::getInstance()->keys().edtLowerCase() )
        {
            edtLowerCase();
            m_edtSelectActive = false;
            return true;
        }

        if ( keyStr == Overlord::getInstance()->keys().edtSplitHorizontal() )
        {
            edtSplitH();
            m_edtSelectActive = false;
            return true;
        }

        if ( keyStr == Overlord::getInstance()->keys().edtSplitVertical() )
        {
            edtSplitV();
            m_edtSelectActive = false;
            return true;
        }

        if ( keyStr == Overlord::getInstance()->keys().edtSaveFile() )
        {
            m_edtSelectActive = false;

            if ( currentFile().isEmpty() )
            {
                edtSaveFileAs( Overlord::SAVE_ONE );
            }
            else
            {
                edtSaveFile( currentFile(), Overlord::SAVE_ONE );
            }

            return true;
        }

        if ( keyStr == Overlord::getInstance()->keys().edtAstyle() )
        {
            m_edtSelectActive = false;
            astyleBuffer();
            return true;
        }

        if ( keyStr == Overlord::getInstance()->keys().edtInsertFile() )
        {
            m_edtSelectActive = false;
            edtInsertFile();
            return true;
        }

    }
    else
    {

        switch ( key )
        {
            case Qt::Key_0:
            case Qt::Key_Insert:        // move line
                if ( isKeypad )
                {
                    if ( Overlord::getInstance()->edtDirection() )
                    {
                        edtLineUp( mode );
                    }
                    else
                    {
                        edtLineDown( mode );
                    }

                    return true;
                }

                break;

            case Qt::Key_Delete:
            case Qt::Key_Period:    // select
                if ( isKeypad )
                {
                    m_edtSelectActive = true;
                    return true;
                }

                break;

            case Qt::Key_1:     // move word
            case Qt::Key_End:
                if ( isKeypad )
                {
                    QTextCursor cursor = textCursor();

                    if ( Overlord::getInstance()->edtDirection() )
                    {
                        cursor.movePosition( QTextCursor::PreviousWord, mode, 1 );
                    }
                    else
                    {
                        cursor.movePosition( QTextCursor::NextWord, mode, 1 );
                    }

                    setTextCursor( cursor );

                    return true;
                }

                break;

            case Qt::Key_2:     // EOL
            case Qt::Key_Down:
                if ( isKeypad )
                {
                    QTextCursor cursor = textCursor();
                    QTextCursor work = textCursor();

                    if ( Overlord::getInstance()->edtDirection() )
                    {
                        edtHome( mode );
                    }
                    else
                    {
                        edtEndOfLine( mode );
                    }

                    return true;
                }

                break;

            case Qt::Key_3:     // CHAR
            case Qt::Key_PageDown:
                if ( isKeypad )
                {
                    if ( Overlord::getInstance()->edtDirection() )
                    {
                        edtKeyLeft( mode );
                    }
                    else
                    {
                        edtKeyRight( mode );
                    }

                    return true;
                }

                break;

            case Qt::Key_4:     // ADVANCE
            case Qt::Key_Left:
                if ( isKeypad )
                {
                    Overlord::getInstance()->set_edtDirection( false );
                    return true;
                }

                break;

            case Qt::Key_5:     // BACKUP
            case Qt::Key_Clear:
                if ( isKeypad )
                {
                    Overlord::getInstance()->set_edtDirection( true );
                    return true;
                }

                break;

            case Qt::Key_6:         // CUT
            case Qt::Key_Right:
                if ( isKeypad )
                {
                    if ( textCursor().hasSelection() )
                    {
                        edtCut();
                        m_edtSelectActive = false;
                    }
                    else
                    {
                        timedMessage( tr( "No active select range" ), MESSAGE_TIME );
                    }

                    return true;
                }

                break;

            case Qt::Key_7:     // PAGE
            case Qt::Key_Home:
                if ( isKeypad )
                {
                    Overlord::getInstance()->set_findFlags( 0 );

                    if ( Overlord::getInstance()->edtDirection() )
                    {
                        Overlord::getInstance()->set_findFlagsBackward();
                    }

                    QString txt( "\f" );
                    bool found = find( txt, Overlord::getInstance()->findFlags() );

                    if ( !found )
                    {
                        timedMessage( tr( "No Formfeed found to mark end of page " ), MESSAGE_TIME );
                        this->setFocus();
                    }

                    return true;
                }

                break;

            case Qt::Key_8:         // SECT
            case Qt::Key_Up:
                if ( isKeypad )
                {
                    if ( Overlord::getInstance()->edtDirection() )
                    {
                        edtSectionUp( mode );
                    }
                    else
                    {
                        edtSectionDown( mode );
                    }

                    return true;
                }

                break;

            case Qt::Key_9:     // APPEND
            case Qt::Key_PageUp:
                if ( isKeypad )
                {
                    QTextCursor cursor = textCursor();

                    m_edtSelectActive = false;

                    if ( cursor.hasSelection() )
                    {
                        QClipboard *clip = QApplication::clipboard();
                        QString txt = clip->text();
                        txt.append( cursor.selectedText() );
                        clip->setText( txt );
                        cursor.clearSelection();
                        setTextCursor( cursor );
                    }
                    else
                    {
                        timedMessage( tr( "no text currently selected" ), MESSAGE_TIME );
                        this->setFocus();
                    }

                    return true;
                }

                break;

            case Qt::Key_Plus:      // DEL C
                if ( isKeypad )
                {
                    m_edtSelectActive = false;

                    // work around for QKeyEvent not being able to give us a QKeySequence
                    //
                    if ( ( Overlord::getInstance()->edtWordCtrlMeta()
                            && ( ( modifiers & Qt::ControlModifier )
                                 || ( modifiers & Qt::MetaModifier ) ) )
                            || ( Overlord::getInstance()->edtWordAltOption()
                                 && ( modifiers & Qt::AltModifier ) ) )
                    {
                        QTextCursor cursor = textCursor();
                        cursor.beginEditBlock();

                        if ( Overlord::getInstance()->edtDirection() )
                        {
                            cursor.movePosition( QTextCursor::PreviousWord, QTextCursor::KeepAnchor, 1 );
                        }
                        else
                        {
                            cursor.movePosition( QTextCursor::NextWord, QTextCursor::KeepAnchor, 1 );
                        }

                        Overlord::getInstance()->set_edtLastDeletedWord( cursor.selectedText() );
                        addToCopyBuffer( cursor.selectedText() );
                        cursor.removeSelectedText();
                        cursor.endEditBlock();
                        setTextCursor( cursor );
                    }
                    else
                    {
                        QTextCursor cursor = textCursor();
                        cursor.beginEditBlock();

                        if ( Overlord::getInstance()->edtDirection() )
                        {
                            cursor.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1 );
                        }
                        else
                        {
                            cursor.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1 );
                        }

                        Overlord::getInstance()->set_edtLastDeletedChar( cursor.selectedText() );
                        cursor.removeSelectedText();
                        cursor.endEditBlock();
                        setTextCursor( cursor );
                    }

                    return true;
                }

                break;

            case Qt::Key_Slash:
                if ( isKeypad )
                {
                    showEdtHelp();
                    return true;
                }

                break;

            case Qt::Key_Asterisk:
                if ( isKeypad )
                {
                    // don't clear EDT select active.
                    // developers set a mark then search for text to find its end sometimes
                    //
                    Overlord::getInstance()->set_findFlags( 0 );

                    if ( Overlord::getInstance()->edtDirection() )
                    {
                        Overlord::getInstance()->set_findFlagsBackward();
                    }

                    bool found = find( Overlord::getInstance()->findText(), Overlord::getInstance()->findFlags() );

                    if ( !found )
                    {
                        timedMessage( tr( "Not found: " ) + Overlord::getInstance()->findText(), MESSAGE_TIME );
                        this->setFocus();
                    }

                    return true;
                }

                break;

            case Qt::Key_Minus:
                if ( isKeypad )
                {
                    deleteThroughEOL();
                    return true;
                }

                break;

            case Qt::Key_F12:
                if ( Overlord::getInstance()->keys().f12AsBackspace() )
                {
                    QTextCursor cursor = textCursor();
                    cursor.movePosition( QTextCursor::StartOfLine, mode, 1 );
                    setTextCursor( cursor );
                    return true;
                }

                break;

            default:
                break;
        }
    }

    m_edtSelectActive = false;
    return false;
}

void DiamondTextEdit::edtSectionUp( QTextCursor::MoveMode mode )
{
    // pageUpDown() is a private class member in QTextEdit. Only way to reach it
    // is to insert KeyEvents for the following:
    //
    // pageUpDown( QTextCursor::Down, mode );
    // pageUpDown( QTextCursor::Up, mode );
    // QKeySequence::SelectPreviousPage
    // QKeySequence::SelectNextPage
    //
    // QKeySequence::MoveToPreviousPage
    // QKeySequence::MoveToNextPage
    //
    // Either that or we stick with the 16 line tradition
    //

    QTextCursor cursor = textCursor();
    cursor.movePosition( QTextCursor::Up, mode, 16 );
    setTextCursor( cursor );
}

void DiamondTextEdit::edtSectionDown( QTextCursor::MoveMode mode )
{
    QTextCursor cursor = textCursor();
    cursor.movePosition( QTextCursor::Down, mode, 16 );
    setTextCursor( cursor );
}

void DiamondTextEdit::edtKeyRight( QTextCursor::MoveMode mode )
{
    QTextCursor cursor = textCursor();
    cursor.movePosition( QTextCursor::NextCharacter, mode, 1 );
    setTextCursor( cursor );
}

void DiamondTextEdit::edtKeyLeft( QTextCursor::MoveMode mode )
{
    QTextCursor cursor = textCursor();
    cursor.movePosition( QTextCursor::PreviousCharacter, mode, 1  );
    setTextCursor( cursor );

}

void DiamondTextEdit::edtLineUp( QTextCursor::MoveMode mode )
{
    QTextCursor cursor = textCursor();
    cursor.movePosition( QTextCursor::Up, mode, 1 );
    setTextCursor( cursor );

}

void DiamondTextEdit::edtLineDown( QTextCursor::MoveMode mode )
{
    QTextCursor cursor = textCursor();
    cursor.movePosition( QTextCursor::Down, mode, 1 );
    setTextCursor( cursor );

}

void DiamondTextEdit::edtEndOfLine( QTextCursor::MoveMode mode )
{
    QTextCursor cursor = textCursor();
    QTextCursor work = textCursor();

    work.movePosition( QTextCursor::EndOfLine, mode, 1 );

    if ( work.position() == cursor.position() )
    {
        cursor.movePosition( QTextCursor::Down, mode, 1 );
    }

    cursor.movePosition( QTextCursor::EndOfLine, mode, 1 );

    setTextCursor( cursor );
}

void DiamondTextEdit::edtHome( QTextCursor::MoveMode mode )
{
    QTextCursor cursor = textCursor();
    QTextCursor work = textCursor();

    work.movePosition( QTextCursor::StartOfLine, mode, 1 );

    if ( work.position() == cursor.position() )
    {
        cursor.movePosition( QTextCursor::Up, mode, 1 );
    }

    cursor.movePosition( QTextCursor::StartOfLine, mode, 1 );
    setTextCursor( cursor );

}

void DiamondTextEdit::clearEdtSelection()
{
    m_edtSelectActive = false;
    QTextCursor cursor = textCursor();
    cursor.clearSelection();
    setTextCursor( cursor );
}

/*
 *  Used by AStyle on save and potentially other places.
 *  The main difference between astyleBuffer and AstyleNewBuffer is here we
 *  not only wait for completion but return the buffer. The caller is responsible
 *  for making changes to the actual document this editor instance has.
 *
 *  This method exists because the syntax highlighting of files containing several
 *  thousand lines is very slow. The straight forward approach of letting the buffer
 *  be styled before writing to disk can lead to zero length files being written. It's
 *  a timing thing.
 *
 *  If you save with AstyleOnSave set to true while an Astyle is running, this method
 *  will launch another Astyle process. AstyleBuffer will throw up an error message.
 */

QByteArray DiamondTextEdit::astyleNewBuffer()
{
    QByteArray retVal;

    timedMessage( tr( "Styling . . ." ), 0 );
    QApplication::setOverrideCursor( Qt::WaitCursor );
    QString uuidStr = QUuid::createUuid().toString();
    uuidStr.remove( "-" );
    uuidStr.remove( "{" );
    uuidStr.remove( "}" );
    QString fname = QString( "ANB_%1_%2" ).formatArgs( uuidStr, strippedName( m_curFile ).toLower() );

    QDir tDir( Overlord::getInstance()->tempDir() );
    QString aStyleFile = tDir.absoluteFilePath( fname );
    QFile tFile( aStyleFile );

    if ( tFile.exists() )
    {
        tFile.remove();
    }

    tFile.open( QIODevice::WriteOnly | QIODevice::Text );
    tFile.write( this->toPlainText().toUtf8() );
    tFile.flush();
    tFile.close();

    QProcess *astyleProcess = new QProcess( this );

    QString command = QString( "astyle --stdin=\"%1\"" );
    command = QStringParser::formatArg( command, tFile.fileName() );

    QTextCursor lastCursorPos = textCursor();
    astyleProcess->start( command );
    astyleProcess->waitForFinished();
    retVal = astyleProcess->readAllStandardOutput();

    timedMessage( tr( "Styling finished" ), 1500 );
    astyleProcess->deleteLater();
    QApplication::restoreOverrideCursor();

    return retVal;

}

void DiamondTextEdit::astyleBuffer()
{
    if ( m_astyleProcess )
    {
        csError( tr( "Astyle" ), tr( "Buffer currently has Astyle process running" ) );
        return;
    }

    if ( !validAstyleSuffix( suffixName( m_curFile ) ) )
    {
        timedMessage( tr( "Not a supported file extension for styling" ), 1500 );
    }

    timedMessage( tr( "Styling . . ." ), 0 );

    QApplication::setOverrideCursor( Qt::WaitCursor );

    QString uuidStr = QUuid::createUuid().toString();
    uuidStr.remove( "-" );
    uuidStr.remove( "{" );
    uuidStr.remove( "}" );
    QString fname = QString( "ab_%1_%2" ).formatArgs( uuidStr, strippedName( m_curFile ).toLower() );

    QDir tDir( Overlord::getInstance()->tempDir() );
    QString aStyleFile = tDir.absoluteFilePath( fname );
    QFile tFile( aStyleFile );

    if ( tFile.exists() )
    {
        tFile.remove();
    }

    tFile.open( QIODevice::WriteOnly | QIODevice::Text );
    tFile.write( this->toPlainText().toUtf8() );
    tFile.flush();
    tFile.close();

    m_astyleProcess = new QProcess( this );

    QString command = QString( "astyle --stdin=\"%1\"" );
    command = QStringParser::formatArg( command, tFile.fileName() );

    QTextCursor lastCursorPos = textCursor();
    Overlord::getInstance()->set_lastActiveRow( lastCursorPos.blockNumber() );
    Overlord::getInstance()->set_lastActiveColumn( lastCursorPos.positionInBlock() );


    connect( m_astyleProcess, static_cast<void( QProcess::* )( int, QProcess::ExitStatus )>( &QProcess::finished ),
             this, &DiamondTextEdit::astyleComplete );

    connect( m_astyleProcess, static_cast<void( QProcess::* )( QProcess::ProcessError )>( &QProcess::errorOccurred ),
             this, &DiamondTextEdit::astyleError );

    m_astyleProcess->start( command );


    bool retVal = m_astyleProcess->waitForStarted();

    if ( !retVal )
    {
        QString errStr = QString( m_astyleProcess->readAllStandardError() );
        csError( tr( "Astyle" ), errStr );
        csError( tr( "AStyle" ), tr( "AStyle must be installed and in execution path to use this feature" ) );
        m_astyleProcess->deleteLater();
        m_astyleProcess = nullptr;
    }
}

void DiamondTextEdit::astyleComplete( int exitCode, QProcess::ExitStatus status )
{
    if ( status != QProcess::NormalExit || exitCode != 0 )
    {
        QString errStr = QString( m_astyleProcess->readAllStandardError() );
        csError( tr( "Astyle" ), errStr );
        m_astyleProcess->deleteLater();
        m_astyleProcess = nullptr;
        return;
    }

    QString txt( m_astyleProcess->readAllStandardOutput() );

    if ( txt.length() > 0 )
    {
        setPlainText( txt );
        QTextCursor cursor( document()->findBlockByNumber( Overlord::getInstance()->lastActiveRow() ) );
        cursor.movePosition( QTextCursor::StartOfLine );
        cursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, Overlord::getInstance()->lastActiveColumn() - 1 );
        setTextCursor( cursor );
        document()->setModified( true );
    }
    else
    {
        csError( tr( "Astyle" ), tr( "Output of Astyle had zero length" ) );
    }

    timedMessage( tr( "Styling finished" ), 1500 );
    m_astyleProcess->deleteLater();
    m_astyleProcess = nullptr;
    QApplication::restoreOverrideCursor();
}

void DiamondTextEdit::astyleError( QProcess::ProcessError error )
{
    Q_UNUSED( error );
    timedMessage( tr( "Styling finished" ), 1500 );
    QString msg( m_astyleProcess->readAllStandardError() );
    csError( tr( "Astyle" ), msg );
    m_astyleProcess->deleteLater();
    m_astyleProcess = nullptr;
}

void DiamondTextEdit::edtInsertFile()
{
    QString selectedFilter;
    QFileDialog::Options options;

    // force windows 7 and 8 to honor initial path
    options = QFileDialog::ForceInitialDir_Win7;

    QStringList fileList = QFileDialog::getOpenFileNames( this, tr( "Select File" ),
                           pathName( currentFile() ), tr( "All Files (*)" ), &selectedFilter, options );

    for ( const QString &fileName : fileList )
    {
        if ( ! fileName.isEmpty() )
        {
            QFile file( fileName );

            if ( !file.open( QFile::ReadOnly | QFile::Text ) )
            {
                csError( tr( "EDT Insert" ), tr( "Unable to open file in text mode" ) );
                return;
            }

            QByteArray temp = file.readAll();

            QString fileData = QString::fromUtf8( temp );

            insertPlainText( fileData );
        }
    }

}
