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

#include "diamond_build_info.h"
#include "mainwindow.h"
#include "non_gui_functions.h"

#include <stdexcept>
#include <QFileInfo>
#include <QKeySequence>
#include <QLabel>
#include <QToolBar>
#include <QFSFileEngine>
#include <QMimeData>
#include <QProgressDialog>
#include <QDateTime>
#include <QPainter>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QRect>
#include <QKeySequence>
#include "dialog_colors.h"
#include "dialog_fonts.h"
#include "dialog_options.h"
#include "dialog_preset.h"
#include "dialog_print_opt.h"
#include "mainwindow.h"
#include "dialog_config.h"
#include "overlord.h"
#include "dialog_edt_help.h"
#include "dialog_busy.h"
#include "dialog_buffer.h"
#include "replacereply.h"
#include "dialog_advfind.h"
#include "dialog_find.h"
#include "dialog_replace.h"
#include "dialog_open.h"
#include "dialog_macro.h"
#include <QBoxLayout>
#include <QFontDialog>
#include <QLabel>
#include <QFileDialog>
#include <QSettings>
#include <QTimer>
#include <QClipboard>
#include "advfind_busy.h"
#include <QInputDialog>

MainWindow::MainWindow( QStringList fileList, QStringList flagList )
    : QMainWindow( nullptr )
    , m_ui( new Ui::MainWindow )
    , m_fileList( fileList )
    , m_flagList( flagList )
{
    m_ui->setupUi( this );
    setDiamondTitle( "untitled.txt" );

    m_ui->actionAstyle->setDisabled( true );

    m_appPath = QApplication::applicationDirPath();
    Overlord::getInstance()->set_appPath( m_appPath );

    setIconSize( QSize( 32,32 ) );
    setWindowIcon( QIcon( "://resources/diamond.png" ) );


    // drag & drop
    setAcceptDrops( true );

    // remaining methods must be done after json_Read for config
    m_tabWidget = new QTabWidget( this );
    m_tabWidget->setTabsClosable( true );
    m_tabWidget->setMovable( true );
    m_tabWidget->setWhatsThis( "tab_widget" );

    // set up the splitter, only display the tabWidget
    m_splitter = new QSplitter( Qt::Vertical );
    m_splitter->addWidget( m_tabWidget );
    setCentralWidget( m_splitter );

    connect( qApp, &QApplication::focusChanged, this, &MainWindow::focusChanged );

    m_split_textEdit = 0;
    m_isSplit = false;

    // macros
    m_record = false;

    // copy buffer
    m_actionCopyBuffer = new QShortcut( this );
    connect( m_actionCopyBuffer, &QShortcut::activated, this, &MainWindow::showCopyBuffer );

    // set flags after reading config and before autoload
    if ( m_flagList.contains( "--no_autoload", Qt::CaseInsensitive ) )
    {
        Overlord::getInstance()->set_flagNoAutoLoad( true );
    }

    if ( m_flagList.contains( "--no_saveconfig", Qt::CaseInsensitive ) )
    {
        Overlord::getInstance()->set_flagNoSaveConfig( true );
    }

    connect( this, &MainWindow::queueNextPart, this, &MainWindow::onceEventLoopStarts );

    queueNextPart();

}

MainWindow::~MainWindow()
{
    delete m_ui;
    deleteCurrentMacro();
}

void MainWindow::deleteCurrentMacro()
{
    if ( m_currentMacro.count() > 0 )
    {
        for ( QKeyEvent *event : m_currentMacro )
        {
            delete event;
        }

        m_currentMacro.clear();
    }

}

void MainWindow::onceEventLoopStarts()
{
    // Because the user "could" be prompted to choose a file or location
    // this critical part of Settings has to be done here.
    //
    getConfigFileName();

    connect( Overlord::getInstance(), &Overlord::preloadComplete, this, &MainWindow::startupStep2, Qt::QueuedConnection );

    if ( ! Overlord::getInstance()->set_configFileName( m_configFileName ) )
    {
        // do not start program
        csError( tr( "Configuration File Missing" ), tr( "Unable to locate or open the Diamond Configuration file." ) );
        throw std::runtime_error( "abort_no_message" );
    }

    QString syntaxPath = Overlord::getInstance()->syntaxPath();

    if ( ! QFile::exists( syntaxPath + "syn_txt.json" ) )
    {
        qDebug() << "syntax path not set in Overlord";
        Overlord::getInstance()->set_syntaxPath( get_SyntaxPath( syntaxPath ) );
    }


    if ( ! QFile::exists( Overlord::getInstance()->mainDictionary() ) )
    {
        Overlord::getInstance()->set_mainDictionary( get_xxFile( "Dictionary File (*.dic)", "en_US.dic", "Dictionary Files (*.dic)" ) );
    }

    Overlord::getInstance()->set_userDictionary( pathName( Overlord::getInstance()->mainDictionary() ) + "/userDict.txt" );

    if ( ! QFile::exists( Overlord::getInstance()->mainDictionary() ) )
    {
        // add missing file
        QFile temp( Overlord::getInstance()->mainDictionary() );
        temp.open( QIODevice::WriteOnly );
        temp.close();
    }


    m_refocusTimer = new QTimer( this );
    m_refocusTimer->setInterval( 500 );

    connect( m_refocusTimer, &QTimer::timeout, this, &MainWindow::refocusTab );

}

void MainWindow::startupStep2()
{
    // screen setup
    createShortCuts( true );
    createToolBars();
    createStatusBar();
    createToggles();
    createConnections();

    QProcess astyleCheck( this );

    astyleCheck.start( "astyle --help" );

    bool retVal = astyleCheck.waitForFinished();

    if ( retVal )
    {
        QString helpText( astyleCheck.readAllStandardOutput() );

        if ( helpText.length() > 100 )
        {
            m_ui->actionAstyle->setDisabled( false );
        }
    }


    // recent folders
    rfolder_CreateMenus();

    // preset  folders
    prefolder_CreateMenus();

    // recent files
    rf_CreateMenus();

    // recent folders, context menu
    QMenu *menuFolder_R = m_ui->actionOpen_RecentFolder->menu();
    menuFolder_R->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( menuFolder_R, &QMenu::customContextMenuRequested,     this, &MainWindow::showContext_RecentFolder );

    // recent files, context menu
    m_ui->menuFile->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( m_ui->menuFile, &QMenu::customContextMenuRequested,   this, &MainWindow::showContext_Files );

    // window tab, context menu
    m_ui->menuWindow->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( m_ui->menuWindow, &QMenu::customContextMenuRequested, this, &MainWindow::showContext_Tabs );

    this->move( Overlord::getInstance()->lastPosition() );
    this->resize( Overlord::getInstance()->lastSize() );

    connect( Overlord::getInstance(), &Overlord::Move, this, &MainWindow::Move );
    connect( Overlord::getInstance(), &Overlord::Resize, this, &MainWindow::Resize );

    // Let the queue drain before we autoload any files
    connect( this, &MainWindow::nextStartupStep, this, &MainWindow::startupStep3, Qt::QueuedConnection );

    nextStartupStep();  // use of queued connection allows us to drain the event queue
}

void MainWindow::startupStep3()
{
    if ( Overlord::getInstance()->autoLoad() && ! Overlord::getInstance()->flagNoAutoLoad() )
    {
        autoLoad();
    }

    // user requested files on the command line
    if ( m_fileList.count() > 1 )
    {
        argLoad( m_fileList );
    }

    // no files were open, open a blank tab
    if ( m_tabWidget->count() == 0 )
    {
        tabNew();
    }

    // currently open tabs, must occur after autoLoad & argLoad
    openTab_CreateMenus();

    // find
    if ( ! Overlord::getInstance()->findList().isEmpty() )
    {
        Overlord::getInstance()->set_findText( Overlord::getInstance()->findList().first() );
    }

    // replace
    if ( ! Overlord::getInstance()->replaceList().isEmpty() )
    {
        Overlord::getInstance()->set_replaceText( Overlord::getInstance()->replaceList().first() );
    }

    setStatus_ColMode();
    setStatusBar( tr( "Ready" ), 0 );
    setUnifiedTitleAndToolBarOnMac( true );

}

// **window, tabs
void MainWindow::tabNew()
{
    m_textEdit = new DiamondTextEdit( this, "tab" );

    // keep reference
    m_noSplit_textEdit = m_textEdit;

    // triggers a call to tabChanged
    int index = m_tabWidget->addTab( m_textEdit, "untitled.txt" );
    m_tabWidget->setCurrentIndex( index );

//  TODO:: tabs are basically wrong.
//         A tab isn't a set of spaces it is a tab stop.
//         When a tab is 8 that means the cursor STOPS every 8 spaces, not lands after the next 8 space boundary
//         COBOL AREA-B starts in column 8 because that is where the tab stopped the typewriter carriage.
//
    int tmp = m_textEdit->fontMetrics().width( " " );
    m_textEdit->setTabStopWidth( tmp * Overlord::getInstance()->tabSpacing() );

    m_textEdit->setFocus();

    if ( Overlord::getInstance()->isWordWrap() )
    {
        m_textEdit->setLineWrapMode( QPlainTextEdit::WidgetWidth );
    }
    else
    {
        m_textEdit->setLineWrapMode( QPlainTextEdit::NoWrap );
    }

    // connect(m_textEdit, SIGNAL(fileDropped(const QString &)), this, SLOT(fileDropped(const QString &)));
    connect( m_textEdit->document(), &QTextDocument::contentsChanged, this, &MainWindow::documentWasModified );

    connect( m_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::moveBar );
    connect( m_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::setStatus_LineCol );

    connect( m_textEdit, &DiamondTextEdit::undoAvailable, m_ui->actionUndo, &QAction::setEnabled );
    connect( m_textEdit, &DiamondTextEdit::redoAvailable, m_ui->actionRedo, &QAction::setEnabled );
    connect( m_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCut,  &QAction::setEnabled );
    connect( m_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCopy, &QAction::setEnabled );

    connect( m_textEdit, &DiamondTextEdit::setSynType, this, &MainWindow::setSynType );

    // connect EDT signals here
    //
    connect( m_textEdit, &DiamondTextEdit::showEdtHelp, this, &MainWindow::showEdtHelp );
    connect( m_textEdit, &DiamondTextEdit::edtTop, this, &MainWindow::goTop );
    connect( m_textEdit, &DiamondTextEdit::edtBottom, this, &MainWindow::goBottom );
    connect( m_textEdit, &DiamondTextEdit::not_done, this, &MainWindow::showNotDone );
    connect( m_textEdit, &DiamondTextEdit::edtCut, this, &MainWindow::mw_cut );
    connect( m_textEdit, &DiamondTextEdit::edtUpcase, this, &MainWindow::caseUpper );
    connect( m_textEdit, &DiamondTextEdit::edtLowerCase, this, &MainWindow::caseLower );
    connect( m_textEdit, &DiamondTextEdit::edtSplitH, this, &MainWindow::split_Horizontal );
    connect( m_textEdit, &DiamondTextEdit::edtSplitV, this, &MainWindow::split_Vertical );
    connect( m_textEdit, &DiamondTextEdit::edtSaveFile, this, &MainWindow::saveFile );
    connect( m_textEdit, &DiamondTextEdit::edtSaveFileAs, this, &MainWindow::saveAs );
    connect( m_textEdit, &DiamondTextEdit::edtPaste, this, &MainWindow::mw_paste );
    connect( m_textEdit, &DiamondTextEdit::edtCopy, this, &MainWindow::mw_copy );
    connect( m_textEdit, &DiamondTextEdit::edtSubs, this, &MainWindow::replace );
    connect( m_textEdit, &DiamondTextEdit::timedMessage, this, &MainWindow::setStatusBar );
}

void MainWindow::mw_tabClose()
{
    qDebug() << "mw_tabClose()";
    int index = m_tabWidget->currentIndex();
    tabClose( index );
}

void MainWindow::tabClose( int index )
{
    m_tabWidget->setCurrentIndex( index );
    QWidget *tmp = m_tabWidget->widget( index );

    DiamondTextEdit *t_textEdit;
    t_textEdit = dynamic_cast<DiamondTextEdit *>( tmp );

    if ( t_textEdit )
    {
        m_textEdit = t_textEdit;
        m_curFile  = this->get_curFileName( index );

        if ( close_Doc() )
        {

            if ( m_tabWidget->count() > 1 )
            {

                // hold textEdit and delete after tab is removed
                DiamondTextEdit *hold = m_textEdit;

                m_tabWidget->removeTab( index );
                delete hold;
            }
        }
    }
}

void MainWindow::tabChanged( int index )
{
    QWidget *tmp = m_tabWidget->widget( index );

    DiamondTextEdit *textEdit;
    textEdit = dynamic_cast<DiamondTextEdit *>( tmp );

    if ( textEdit )
    {
        m_textEdit = textEdit;

        // keep reference
        m_noSplit_textEdit = m_textEdit;

        m_curFile = this->get_curFileName( index );
        this->setCurrentTitle( m_curFile, true, false, m_textEdit->isReadOnly() );

        // **
        setStatus_LineCol();
        m_textEdit->set_ColumnMode( Overlord::getInstance()->isColumnMode() );
        m_textEdit->set_ShowLineNum( Overlord::getInstance()->showLineNumbers() );

        moveBar();
        show_Spaces();
        show_Breaks();
    }
}

void MainWindow::focusChanged( QWidget *prior, QWidget *current )
{
    Q_UNUSED( prior )

    if ( ! current )
    {
        return;
    }

    DiamondTextEdit *t_textEdit;
    t_textEdit = dynamic_cast<DiamondTextEdit *>( current );

    if ( t_textEdit )
    {

        if ( m_textEdit->m_owner == t_textEdit->m_owner )
        {
            // do nothing

        }
        else
        {
            m_textEdit = t_textEdit;

            if ( m_textEdit->m_owner == "tab" )
            {
                // focus changed to the tabWidet

                // keep reference
                m_noSplit_textEdit = m_textEdit;

                int index = m_tabWidget->currentIndex();
                m_curFile = get_curFileName( index );

                setStatus_FName( m_curFile );
                setStatus_ReadWrite( m_textEdit->isReadOnly() );


            }
            else if ( m_textEdit->m_owner == "split" )
            {
                // focus changed to the splitWidget

                m_curFile = m_splitFileName;
                setStatus_FName( m_curFile );
                setStatus_ReadWrite( m_textEdit->isReadOnly() );
            }

            setStatus_LineCol();
            m_textEdit->set_ColumnMode( Overlord::getInstance()->isColumnMode() );
            m_textEdit->set_ShowLineNum( Overlord::getInstance()->showLineNumbers() );

            moveBar();
            show_Spaces();
            show_Breaks();

        }

        refocusTab();
    }
}


// **help
void MainWindow::diamondHelp()
{
    try
    {
        showHtml( "docs", "https://www.copperspice.com/docs/diamond/index.html" );

    }
    catch ( std::exception &e )
    {
        // do nothing for now
    }
}

void MainWindow::about()
{
    // change mainwindow.cpp & main.cpp

    QString icon = ":/resources/diamond.png";

    QString textBody =
        "<font color='#000080'><table style=margin-right:25>"

        "<tr><td><img width='96' height='96'src='" + icon + "'></td>"

        "    <td>"
        "      <table style='margin-left:25; margin-top:15px' >"
        "        <tr><td><nobr>Developed by Barbara Geller</nobr></td>"
        "            <td>barbara@copperspice.com</td></tr>"
        "        <tr><td style=padding-right:25><nobr>Developed by Ansel Sermersheim</nobr></td>"
        "            <td>ansel@copperspice.com</td></tr>"
        "       </table>"
        "    </td>"

        "</tr>"
        "</table></font>"

        "<p><small>Copyright 2012-2020 BG Consulting, released under the terms of the GNU GPL version 2<br>"
        "This program is provided AS IS with no warranty of any kind.<br></small></p>";

    //
    QMessageBox msgB;
    msgB.setIcon( QMessageBox::NoIcon );
    msgB.setWindowIcon( QIcon( "://resources/diamond.png" ) );

    msgB.setWindowTitle( tr( "About Diamond" ) );
    msgB.setText( tr( "<p style=margin-right:25><center><h5>Version: %1<br>Build # %2</h5></center></p>" )
                  .formatArgs( QString::fromLatin1( versionString ), QString::fromLatin1( buildDate ) ) );
    msgB.setInformativeText( textBody );

    msgB.setStandardButtons( QMessageBox::Ok );
    msgB.setDefaultButton( QMessageBox::Ok );

    msgB.exec();
}

void MainWindow::createConnections()
{
    // file
    connect( m_ui->actionNew,               &QAction::triggered, this, &MainWindow::newFile );
    connect( m_ui->actionOpen,              &QAction::triggered, this, [this]( bool )
    {
        openDoc( Overlord::getInstance()->priorPath() );
    } );
    connect( m_ui->actionOpen_RelatedFile,  &QAction::triggered, this, &MainWindow::open_RelatedFile );
    connect( m_ui->actionClose,             &QAction::triggered, this, [this]( bool )
    {
        close_Doc();
    } );
    connect( m_ui->actionClose_All,         &QAction::triggered, this, [this]( bool )
    {
        closeAll_Doc( false );
    } );
    connect( m_ui->actionReload,            &QAction::triggered, this, &MainWindow::reload );

    connect( m_ui->actionSave,              &QAction::triggered, this, [this]( bool )
    {
        save();
    } );
    connect( m_ui->actionSave_As,           &QAction::triggered, this, [this]( bool )
    {
        saveAs( Overlord::SAVE_ONE );
    } );
    connect( m_ui->actionSave_All,          &QAction::triggered, this, &MainWindow::saveAll );

    connect( m_ui->actionPrint,             &QAction::triggered, this, &MainWindow::print );
    connect( m_ui->actionPrint_Preview,     &QAction::triggered, this, &MainWindow::printPreview );
    connect( m_ui->actionPrint_Pdf,         &QAction::triggered, this, &MainWindow::printPdf );
    connect( m_ui->actionExit,              &QAction::triggered, this, [this]( bool )
    {
        close();
    } );

    // edit
    connect( m_ui->actionUndo,              &QAction::triggered, this, &MainWindow::mw_undo );
    connect( m_ui->actionRedo,              &QAction::triggered, this, &MainWindow::mw_redo );
    connect( m_ui->actionCut,               &QAction::triggered, this, &MainWindow::mw_cut );
    connect( m_ui->actionCopy,              &QAction::triggered, this, &MainWindow::mw_copy );
    connect( m_ui->actionPaste,             &QAction::triggered, this, &MainWindow::mw_paste );

    connect( m_ui->actionSelect_All,        &QAction::triggered, this, &MainWindow::selectAll );
    connect( m_ui->actionSelect_Block,      &QAction::triggered, this, &MainWindow::selectBlock );
    connect( m_ui->actionSelect_Line,       &QAction::triggered, this, &MainWindow::selectLine );
    connect( m_ui->actionSelect_Word,       &QAction::triggered, this, &MainWindow::selectWord );
    connect( m_ui->actionCase_Upper,        &QAction::triggered, this, &MainWindow::caseUpper );
    connect( m_ui->actionCase_Lower,        &QAction::triggered, this, &MainWindow::caseLower );
    connect( m_ui->actionCase_Cap,          &QAction::triggered, this, &MainWindow::caseCap );

    connect( m_ui->actionIndent_Incr,       &QAction::triggered, this, [this]( bool )
    {
        indentIncr( "indent" );
    } );
    connect( m_ui->actionIndent_Decr,       &QAction::triggered, this, [this]( bool )
    {
        indentDecr( "unindent" );
    } );
    connect( m_ui->actionDelete_Line,       &QAction::triggered, this, &MainWindow::deleteLine );
    connect( m_ui->actionDelete_EOL,        &QAction::triggered, this, &MainWindow::deleteEOL );
    connect( m_ui->actionDelete_ThroughEOL, &QAction::triggered, this, &MainWindow::deleteThroughEOL );

    connect( m_ui->actionInsert_Date,       &QAction::triggered, this, &MainWindow::insertDate );
    connect( m_ui->actionInsert_Time,       &QAction::triggered, this, &MainWindow::insertTime );
    connect( m_ui->actionInsert_Symbol,     &QAction::triggered, this, &MainWindow::insertSymbol );
    connect( m_ui->actionColumn_Mode,       &QAction::triggered, this, &MainWindow::columnMode );

    // search
    connect( m_ui->actionFind,              &QAction::triggered, this, &MainWindow::find );
    connect( m_ui->actionReplace,           &QAction::triggered, this, &MainWindow::replace );
    connect( m_ui->actionFind_Next,         &QAction::triggered, this, &MainWindow::findNext );
    connect( m_ui->actionFind_Prev,         &QAction::triggered, this, &MainWindow::findPrevious );
    connect( m_ui->actionAdv_Find,          &QAction::triggered, this, &MainWindow::advFind );
    connect( m_ui->actionGo_Line,           &QAction::triggered, this, &MainWindow::goLine );
    connect( m_ui->actionGo_Column,         &QAction::triggered, this, &MainWindow::goColumn );
    connect( m_ui->actionGo_Top,            &QAction::triggered, this, &MainWindow::goTop );
    connect( m_ui->actionGo_Bottom,         &QAction::triggered, this, &MainWindow::goBottom );

    // view
    connect( m_ui->actionLine_Highlight,    &QAction::triggered, this, &MainWindow::lineHighlight );
    connect( m_ui->actionLine_Numbers,      &QAction::triggered, this, &MainWindow::lineNumbers );
    connect( m_ui->actionWord_Wrap,         &QAction::triggered, this, &MainWindow::wordWrap );
    connect( m_ui->actionShow_Spaces,       &QAction::triggered, this, &MainWindow::show_Spaces );
    connect( m_ui->actionShow_Breaks,       &QAction::triggered, this, &MainWindow::show_Breaks );
    connect( m_ui->actionDisplay_HTML,      &QAction::triggered, this, &MainWindow::displayHTML );
    connect( m_ui->actionClipboard,         &QAction::triggered, this, &MainWindow::showClipboard );
    connect( m_ui->actionBackups,           &QAction::triggered, this, &MainWindow::showBackups );

    // document
    connect( m_ui->actionSyn_C,             &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_C );
    } );
    connect( m_ui->actionSyn_Clipper,       &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_CLIPPER );
    } );
    connect( m_ui->actionSyn_CMake,         &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_CMAKE );
    } );
    connect( m_ui->actionSyn_Css,           &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_CSS );
    } );
    connect( m_ui->actionSyn_Doxy,          &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_DOXY );
    } );
    connect( m_ui->actionSyn_ErrorLog,      &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_ERRLOG );
    } );
    connect( m_ui->actionSyn_Html,          &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_HTML );
    } );
    connect( m_ui->actionSyn_Java,          &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_JAVA );
    } );
    connect( m_ui->actionSyn_Javascript,    &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_JS );
    } );
    connect( m_ui->actionSyn_Json,          &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_JSON );
    } );
    connect( m_ui->actionSyn_Makefile,      &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_MAKE );
    } );
    connect( m_ui->actionSyn_Nsis,          &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_NSIS );
    } );
    connect( m_ui->actionSyn_Text,          &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_TEXT );
    } );
    connect( m_ui->actionSyn_Shell,         &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_SHELL );
    } );
    connect( m_ui->actionSyn_Perl,          &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_PERL );
    } );
    connect( m_ui->actionSyn_PHP,           &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_PHP );
    } );
    connect( m_ui->actionSyn_Python,        &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_PYTHON );
    } );
    connect( m_ui->actionSyn_Xml,           &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_XML );
    } );
    connect( m_ui->actionSyn_None,          &QAction::triggered, this, [this]( bool )
    {
        forceSyntax( SYN_NONE );
    } );

    // connect(m_ui->actionSyn_UNUSED1,    &QAction::triggered, this, [this](bool){ forceSyntax(SYN_UNUSED1); } );
    // connect(m_ui->actionSyn_UNUSED2,    &QAction::triggered, this, [this](bool){ forceSyntax(SYN_UNUSED2); } );

    connect( m_ui->actionFormat_Unix,       &QAction::triggered, this, &MainWindow::formatUnix );
    connect( m_ui->actionFormat_Win,        &QAction::triggered, this, &MainWindow::formatWin );

    connect( m_ui->actionFix_Tab_Spaces,    &QAction::triggered, this, &MainWindow::fixTab_Spaces );
    connect( m_ui->actionFix_Spaces_Tab,    &QAction::triggered, this, &MainWindow::fixSpaces_Tab );
    connect( m_ui->actionDeleteEOL_Spaces,  &QAction::triggered, this, &MainWindow::deleteEOL_Spaces );

    // tools
    connect( m_ui->actionMacro_Start,       &QAction::triggered, this, &MainWindow::mw_macroStart );
    connect( m_ui->actionMacro_Stop,        &QAction::triggered, this, &MainWindow::mw_macroStop );
    connect( m_ui->actionMacro_Play,        &QAction::triggered, this, &MainWindow::macroPlay );
    connect( m_ui->actionMacro_Repeat,      &QAction::triggered, this, &MainWindow::macroRepeat );
    connect( m_ui->actionMacro_Load,        &QAction::triggered, this, &MainWindow::macroLoad );
    connect( m_ui->actionMacro_EditNames,   &QAction::triggered, this, &MainWindow::macroEditNames );
    connect( m_ui->actionSpell_Check,       &QAction::triggered, this, &MainWindow::spellCheck );
    connect( m_ui->actionAstyle,            &QAction::triggered, this, &MainWindow::astyle );

    // settings
    connect( m_ui->actionColors,            &QAction::triggered, this, &MainWindow::setColors );
    connect( m_ui->actionFonts,             &QAction::triggered, this, &MainWindow::setFont );
    connect( m_ui->actionOptions,           &QAction::triggered, this, &MainWindow::setOptions );
    connect( m_ui->actionPresetFolders,     &QAction::triggered, this, &MainWindow::setPresetFolders );
    connect( m_ui->actionPrintOptions,      &QAction::triggered, this, &MainWindow::setPrintOptions );
    connect( m_ui->actionMove_ConfigFile,   &QAction::triggered, this, &MainWindow::move_ConfigFile );

    // window
    connect( m_ui->actionTab_New,           &QAction::triggered, this, &MainWindow::tabNew );
    connect( m_ui->actionTab_Close,         &QAction::triggered, this, &MainWindow::mw_tabClose );
    connect( m_ui->actionSplit_Horizontal,  &QAction::triggered, this, &MainWindow::split_Horizontal );
    connect( m_ui->actionSplit_Vertical,    &QAction::triggered, this, &MainWindow::split_Vertical );

    // help menu
    connect( m_ui->actionDiamond_Help,      &QAction::triggered, this, &MainWindow::diamondHelp );
    connect( m_ui->actionAbout,             &QAction::triggered, this, &MainWindow::about );
}

void MainWindow::createToggles()
{
    m_ui->actionSyn_C->setCheckable( true );
    m_ui->actionSyn_Clipper->setCheckable( true );
    m_ui->actionSyn_CMake->setCheckable( true );
    m_ui->actionSyn_Css->setCheckable( true );
    m_ui->actionSyn_Doxy->setCheckable( true );
    m_ui->actionSyn_ErrorLog->setCheckable( true );
    m_ui->actionSyn_Html->setCheckable( true );
    m_ui->actionSyn_Java->setCheckable( true );
    m_ui->actionSyn_Javascript->setCheckable( true );
    m_ui->actionSyn_Json->setCheckable( true );
    m_ui->actionSyn_Makefile->setCheckable( true );
    m_ui->actionSyn_Nsis->setCheckable( true );
    m_ui->actionSyn_Text->setCheckable( true );
    m_ui->actionSyn_Shell->setCheckable( true );
    m_ui->actionSyn_Perl->setCheckable( true );
    m_ui->actionSyn_PHP->setCheckable( true );
    m_ui->actionSyn_Python->setCheckable( true );
    m_ui->actionSyn_Xml->setCheckable( true );
    m_ui->actionSyn_None->setCheckable( true );

    // m_ui->actionSyn_Usused1->setCheckable(true);
    // m_ui->actionSyn_Unused2->setCheckable(true);

    m_ui->actionLine_Highlight->setCheckable( true );
    m_ui->actionLine_Highlight->setChecked( Overlord::getInstance()->showLineHighlight() );

    m_ui->actionLine_Numbers->setCheckable( true );
    m_ui->actionLine_Numbers->setChecked( Overlord::getInstance()->showLineNumbers() );

    m_ui->actionWord_Wrap->setCheckable( true );
    m_ui->actionWord_Wrap->setChecked( Overlord::getInstance()->isWordWrap() );

    m_ui->actionShow_Spaces->setCheckable( true );
    m_ui->actionShow_Spaces->setChecked( Overlord::getInstance()->showSpaces() );

    m_ui->actionShow_Breaks->setCheckable( true );
    m_ui->actionShow_Breaks->setChecked( Overlord::getInstance()->showBreaks() );

    m_ui->actionColumn_Mode->setCheckable( true );
    m_ui->actionColumn_Mode->setChecked( Overlord::getInstance()->isColumnMode() );

    m_ui->actionSpell_Check->setCheckable( true );
    m_ui->actionSpell_Check->setChecked( Overlord::getInstance()->isSpellCheck() );

    m_ui->actionUndo->setEnabled( false );
    m_ui->actionRedo->setEnabled( false );
    m_ui->actionCut->setEnabled( false );
    m_ui->actionCopy->setEnabled( false );

    m_ui->actionBackups->setEnabled( Overlord::getInstance()->backupDirectory().length() > 1 );

    connect( m_tabWidget, &QTabWidget::currentChanged,    this, &MainWindow::tabChanged );
    connect( m_tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::tabClose );
}

void MainWindow::createShortCuts( bool setupAll )
{
    // ** standard definded shortcuts

    if ( setupAll )
    {
        // file
        m_ui->actionOpen->setShortcut( QKeySequence( Overlord::getInstance()->keys().open() ) );
        m_ui->actionClose->setShortcut( QKeySequence( Overlord::getInstance()->keys().close() ) );
        m_ui->actionSave->setShortcut( QKeySequence( Overlord::getInstance()->keys().save() ) );
        m_ui->actionSave_As->setShortcut( QKeySequence( Overlord::getInstance()->keys().saveAs() ) );
        m_ui->actionPrint->setShortcut( QKeySequence( Overlord::getInstance()->keys().print() ) );
        m_ui->actionExit->setShortcuts( QKeySequence::Quit );

        // edit
        m_ui->actionUndo->setShortcut( QKeySequence( Overlord::getInstance()->keys().undo() ) );
        m_ui->actionRedo->setShortcut( QKeySequence( Overlord::getInstance()->keys().redo() ) );
        m_ui->actionCut->setShortcut( QKeySequence( Overlord::getInstance()->keys().cut() ) );
        m_ui->actionCopy->setShortcut( QKeySequence( Overlord::getInstance()->keys().copy() ) );
        m_ui->actionPaste->setShortcut( QKeySequence( Overlord::getInstance()->keys().paste() ) );

        m_ui->actionSelect_All->setShortcut( QKeySequence( Overlord::getInstance()->keys().selectAll() ) );
        m_ui->actionGo_Top->setShortcut( QKeySequence( Overlord::getInstance()->keys().goTop() ) );
        m_ui->actionGo_Bottom->setShortcut( QKeySequence( Overlord::getInstance()->keys().goBottom() ) );

        //search
        m_ui->actionFind->setShortcut( QKeySequence( Overlord::getInstance()->keys().find() ) );
        m_ui->actionReplace->setShortcut( QKeySequence( Overlord::getInstance()->keys().replace() ) );
        m_ui->actionFind_Next->setShortcut( QKeySequence( Overlord::getInstance()->keys().findNext() ) );;
        m_ui->actionFind_Prev->setShortcut( QKeySequence( Overlord::getInstance()->keys().findPrev() ) );

        // tab
        m_ui->actionTab_New->setShortcut( QKeySequence( Overlord::getInstance()->keys().newTab() ) );

        // help
        m_ui->actionDiamond_Help->setShortcuts( QKeySequence::HelpContents );
    }

    // ** user definded

    // edit
    m_ui->actionPrint_Preview->setShortcut( QKeySequence( Overlord::getInstance()->keys().printPreview() ) );
    m_ui->actionReload->setShortcut( QKeySequence( Overlord::getInstance()->keys().reload() ) );

    m_ui->actionSelect_Line->setShortcut( QKeySequence( Overlord::getInstance()->keys().selectLine() )   );
    m_ui->actionSelect_Word->setShortcut( QKeySequence( Overlord::getInstance()->keys().selectWord() )   );
    m_ui->actionSelect_Block->setShortcut( QKeySequence( Overlord::getInstance()->keys().selectBlock() ) );
    m_ui->actionCase_Upper->setShortcut( QKeySequence( Overlord::getInstance()->keys().upper() ) );
    m_ui->actionCase_Lower->setShortcut( QKeySequence( Overlord::getInstance()->keys().lower() ) );

    m_ui->actionIndent_Incr->setShortcut( QKeySequence( Overlord::getInstance()->keys().indentIncrement() ) );
    m_ui->actionIndent_Decr->setShortcut( QKeySequence( Overlord::getInstance()->keys().indentDecrement() ) );
    m_ui->actionDelete_Line->setShortcut( QKeySequence( Overlord::getInstance()->keys().deleteLine() ) );
    m_ui->actionDelete_EOL->setShortcut( QKeySequence( Overlord::getInstance()->keys().deleteToEOL() )   );
    m_ui->actionDelete_ThroughEOL->setShortcut( QKeySequence( Overlord::getInstance()->keys().deleteThroughEOL() ) );
    m_ui->actionColumn_Mode->setShortcut( QKeySequence( Overlord::getInstance()->keys().columnMode() ) );

    // search
    m_ui->actionGo_Line->setShortcut( QKeySequence( Overlord::getInstance()->keys().gotoLine() ) );

    // view
    m_ui->actionShow_Spaces->setShortcut( QKeySequence( Overlord::getInstance()->keys().showSpaces() ) );
    m_ui->actionShow_Breaks->setShortcut( QKeySequence( Overlord::getInstance()->keys().showBreaks() ) );

    // tools
    m_ui->actionMacro_Play->setShortcut( QKeySequence( Overlord::getInstance()->keys().macroPlay() )   );
    m_ui->actionSpell_Check->setShortcut( QKeySequence( Overlord::getInstance()->keys().spellCheck() ) );

    // copy buffer
    m_actionCopyBuffer->setKey( QKeySequence( Overlord::getInstance()->keys().copyBuffer() ) );
}


void MainWindow::createToolBars()
{
    m_ui->actionTab_New->setIcon( QIcon( ":/resources/tab_new.png" ) );
    m_ui->actionTab_Close->setIcon( QIcon( ":/resources/tab_remove.png" ) );

    m_ui->actionOpen->setIcon( QIcon( ":/resources/file_open.png" ) );
    m_ui->actionClose->setIcon( QIcon( ":/resources/file_close.png" ) );
    m_ui->actionSave->setIcon( QIcon( ":/resources/save.png" ) );

    m_ui->actionPrint->setIcon( QIcon( ":/resources/print.png" ) );
    m_ui->actionPrint_Pdf->setIcon( QIcon( ":/resources/print_pdf.png" ) );

    m_ui->actionUndo->setIcon( QIcon( ":/resources/undo.png" ) );
    m_ui->actionRedo->setIcon( QIcon( ":/resources/redo.png" ) );
    m_ui->actionCut->setIcon( QIcon( ":/resources/cut.png" ) );
    m_ui->actionCopy->setIcon( QIcon( ":/resources/copy.png" ) );
    m_ui->actionPaste->setIcon( QIcon( ":/resources/paste.png" ) );

    m_ui->actionFind->setIcon( QIcon( ":/resources/find.png" ) );
    m_ui->actionReplace->setIcon( QIcon( ":/resources/find_replace.png" ) );

    m_ui->actionShow_Spaces->setIcon( QIcon( ":/resources/show_spaces.png" ) );
    m_ui->actionShow_Breaks->setIcon( QIcon( ":/resources/show_eol.png" ) );

    m_ui->actionMacro_Start->setIcon( QIcon( ":/resources/macro_rec.png" ) );
    m_ui->actionMacro_Stop->setIcon( QIcon( ":/resources/macro_stop.png" ) );
    m_ui->actionMacro_Play->setIcon( QIcon( ":/resources/macro_play.png" ) );
    m_ui->actionSpell_Check->setIcon( QIcon( ":/resources/spellcheck.png" ) );

    //
    fileToolBar = addToolBar( tr( "File" ) );
    fileToolBar->addAction( m_ui->actionOpen );
    fileToolBar->addAction( m_ui->actionClose );
    fileToolBar->addAction( m_ui->actionSave );
    fileToolBar->addSeparator();
    fileToolBar->addAction( m_ui->actionPrint );
    fileToolBar->addAction( m_ui->actionPrint_Pdf );

#ifdef Q_OS_MAC
    // os x does not support moving tool bars
    fileToolBar->addSeparator();
#endif

    windowToolBar = addToolBar( tr( "Window" ) );
    windowToolBar->addAction( m_ui->actionTab_New );
    windowToolBar->addAction( m_ui->actionTab_Close );

#ifdef Q_OS_MAC
    windowToolBar->addSeparator();
#endif

    editToolBar = addToolBar( tr( "Edit" ) );
    editToolBar->addAction( m_ui->actionUndo );
    editToolBar->addAction( m_ui->actionRedo );
    editToolBar->addAction( m_ui->actionCut );
    editToolBar->addAction( m_ui->actionCopy );
    editToolBar->addAction( m_ui->actionPaste );

#ifdef Q_OS_MAC
    editToolBar->addSeparator();
#endif

    searchToolBar = addToolBar( tr( "Search" ) );
    searchToolBar->addAction( m_ui->actionFind );
    searchToolBar->addAction( m_ui->actionReplace );

#ifdef Q_OS_MAC
    searchToolBar->addSeparator();
#endif

    viewToolBar = addToolBar( tr( "View" ) );
    viewToolBar->addAction( m_ui->actionShow_Spaces );
    viewToolBar->addAction( m_ui->actionShow_Breaks );

#ifdef Q_OS_MAC
    viewToolBar->addSeparator();
#endif

    toolsToolBar = addToolBar( tr( "Tools" ) );
    toolsToolBar->addAction( m_ui->actionMacro_Start );
    toolsToolBar->addAction( m_ui->actionMacro_Stop );
    toolsToolBar->addAction( m_ui->actionMacro_Play );
    toolsToolBar->addSeparator();
    toolsToolBar->addAction( m_ui->actionSpell_Check );
}

void MainWindow::createStatusBar()
{
    m_statusLine = new QLabel( "", this );

    m_statusMode = new QLabel( "", this );

    m_statusName = new QLabel( "", this );

    m_statusReadWrite = new QLabel( "", this );

    statusBar()->addPermanentWidget( m_statusLine, 0 );
    statusBar()->addPermanentWidget( m_statusMode, 0 );
    statusBar()->addPermanentWidget( m_statusReadWrite, 0 );
    statusBar()->addPermanentWidget( m_statusName, 0 );
}

void MainWindow::setStatusBar( QString msg, int timeOut )
{
    statusBar()->showMessage( msg, timeOut );
}

void MainWindow::showNotDone( QString item )
{
    csMsg( item + " - this feature has not been implemented." );
}


// **settings
void MainWindow::setColors()
{
    Dialog_Colors dw( this );

    dw.exec();
}

void MainWindow::setFont()
{
    Dialog_Fonts dw( Overlord::getInstance()->fontNormal(), Overlord::getInstance()->fontColumn() );
    dw.exec();
}

void MainWindow::setOptions()
{
    Options options = Overlord::getInstance()->pullLocalCopyOfOptions();


    Dialog_Options dw( this );
    int result = dw.exec();

    if ( result == QDialog::Accepted )
    {
        // false will redisplay only user defined shortcuts
        this->createShortCuts( true );
    }
}

void MainWindow::setPresetFolders()
{
    Dialog_Preset dw( this );
    int result = dw.exec();

    if ( result == QDialog::Accepted )
    {
        prefolder_RedoList();
    }
}

void MainWindow::setPrintOptions()
{
    Dialog_PrintOptions dw( this );
    dw.exec();
}

QString MainWindow::get_SyntaxPath( QString syntaxPath )
{
    QString msg  = tr( "Select Diamond Syntax Folder" );
    QString path = get_DirPath( this, msg, syntaxPath );

    return path;
}

QString MainWindow::get_xxFile( QString title, QString fname, QString filter )
{
    QString selectedFilter;
    QFileDialog::Options options;

    // force windows 7 and 8 to honor initial path
    options = QFileDialog::ForceInitialDir_Win7;

    fname = m_appPath + "/" + fname;

    QString file = QFileDialog::getOpenFileName( this, "Select " + title,
                   fname, filter, &selectedFilter, options );

    return file;
}

void MainWindow::Move( QPoint pos )
{
    move( pos );

    Overlord::getInstance()->set_lastPosition( this->pos() );
    Overlord::getInstance()->set_lastSize( this->size() );
}

void MainWindow::Resize( QSize size )
{
    resize( size );
}

void MainWindow::resizeEvent( QResizeEvent *e )
{
    QMainWindow::resizeEvent( e );

    Overlord::getInstance()->set_lastPosition( this->pos() );
    Overlord::getInstance()->set_lastSize( this->size() );
}

void MainWindow::getConfigFileName()
{

#if defined(Q_OS_UNIX) && ! defined(Q_OS_MAC)

    QString homePath = QDir::homePath();
    m_configFileName = homePath + "/.config/Diamond/config.json";

    QDir( homePath ).mkdir( ".config" );
    QDir( homePath + "/.config" ).mkdir( "Diamond" );
    QDir( homePath + "/.config/Diamond" ).mkdir( "dictionary" );

    return;

#elif defined(Q_OS_MAC)

    if ( m_appPath.contains( ".app/Contents/MacOS" ) )
    {
        QString homePath = QDir::homePath();
        m_configFileName = homePath + "/Library/Diamond/config.json";

        QDir( homePath + "/Library" ).mkdir( "Diamond" );
        QDir( homePath + "/Library/Diamond" ).mkdir( "dictionary" );

        return;
    }

#endif

    QString selectedFilter;
    QFileDialog::Options options;

    QMessageBox quest;
    quest.setWindowTitle( tr( "Diamond Editor" ) );
    quest.setText( tr( "Diamond configuration file is missing.\n\n"
                       "Selet an option to (a) create the configuration file in the system default location, "
                       "(b) pick a folder location, or (c) select an existing Diamond Configuration file.\n" ) );

    QPushButton *createDefault  = quest.addButton( "Default Location",     QMessageBox::AcceptRole );
    QPushButton *createNew      = quest.addButton( "Pick Folder Location", QMessageBox::AcceptRole );
    QPushButton *selectExist    = quest.addButton( "Select Existing File", QMessageBox::AcceptRole );

    quest.setStandardButtons( QMessageBox::Cancel );
    quest.setDefaultButton( QMessageBox::Cancel );

    quest.exec();

    if ( quest.clickedButton() == createDefault )
    {
        m_configFileName = m_appPath + "/config.json";

#ifdef Q_OS_WIN
        QString path = QStandardPaths::writableLocation( QStandardPaths::ConfigLocation );
        m_configFileName  = path + "/config.json";
#endif

    }
    else if ( quest.clickedButton() == createNew )
    {
        QString fname = m_appPath + "/config.json";

        // force windows 7 and 8 to honor initial path
        options = QFileDialog::ForceInitialDir_Win7;

        m_configFileName = QFileDialog::getSaveFileName( this, tr( "Create new Configuration File" ),
                           fname, tr( "Json Files (*.json)" ), &selectedFilter, options );

    }
    else if ( quest.clickedButton() == selectExist )
    {

        m_configFileName = QFileDialog::getOpenFileName( this, tr( "Select Existing Diamond Configuration File" ),
                           "", tr( "Json Files (*.json)" ), &selectedFilter, options );

    }
    else
    {
        // user aborted
        m_configFileName = "";

    }
}


void MainWindow::move_ConfigFile()
{
    QSettings settings( "Diamond Editor", "Settings" );
    m_configFileName = settings.value( "configName" ).toString();

    //
    Dialog_Config *dw = new Dialog_Config( m_configFileName );
    int result = dw->exec();

    switch ( result )
    {
        case QDialog::Rejected:
            break;

        case 1:
            // create
        {
            QString selectedFilter;
            QFileDialog::Options options;

            // force windows 7 and 8 to honor initial path
            options = QFileDialog::ForceInitialDir_Win7;

            QString newName = QFileDialog::getSaveFileName( this, tr( "Create New Configuration File" ),
                              m_appPath + "/config.json", tr( "Json Files (*.json)" ),
                              &selectedFilter, options );

            if ( newName.isEmpty() )
            {
                // do nothing

            }
            else if ( QFile::exists( newName ) )
            {
                // can this happen?
                csError( "Diamond Configuration",
                         "Configuration file already exists, unable to create new file." );

            }
            else
            {
                m_configFileName = newName;
                settings.setValue( "configName", m_configFileName );
                Overlord::getInstance()->set_newConfigFileName( m_configFileName );
            }

            break;
        }

        case 2:
            // select
        {
            QString selectedFilter;
            QFileDialog::Options options;

            QString newName = QFileDialog::getOpenFileName( this,
                              tr( "Select Diamond Configuration File" ),
                              "*.json",
                              tr( "Json Files (*.json)" ),
                              &selectedFilter, options );

            if ( newName.isEmpty() )
            {
                // do nothing

            }
            else if ( QFile::exists( newName ) )
            {
                m_configFileName = newName;
                settings.setValue( "configName", m_configFileName );

                Overlord::getInstance()->set_configFileName( m_configFileName );

                csError( "Diamond Configuration", "New configuration file selected."
                         " Restart Diamond to utilize the new configuration file settings." );
            }

            break;
        }

        case 3:
            // rename
            QString newName = dw->get_newName();

            if ( newName.isEmpty() )
            {
                csError( "Diamond Configuration", "No configuration file name specified, unable to rename." );

            }

            if ( QFile::exists( newName ) )
            {
                csError( "Diamond Configuration", "New configuration file already exists, unable to rename." );

            }
            else
            {

                QString path = pathName( newName );
                QDir directory( path );

                if ( ! directory.exists() )
                {
                    directory.mkpath( path );
                }

                if ( QFile::rename( m_configFileName, newName ) )
                {
                    m_configFileName = newName;
                    settings.setValue( "configName", m_configFileName );

                }
                else
                {
                    csError( "Diamond Configuration", "Configuration file rename failed." );

                }
            }

            break;
    }
}

void MainWindow::showEdtHelp()
{
    Dialog_Edt_Help dw( this );
    dw.exec();
}

void MainWindow::showClipboard()
{
    QDialog dw( this );
    dw.setMinimumSize( 400, 400 );
    dw.setWindowTitle( "Clipboard contents" );
    dw.setFont( Overlord::getInstance()->fontNormal() );

    QHBoxLayout *layout = new QHBoxLayout;

    QTextEdit *ed = new QTextEdit();
    ed->setReadOnly( true );
    ed->setText( QApplication::clipboard()->text() );

    layout->addWidget( ed );
    dw.setLayout( layout );

    dw.exec();
}

void MainWindow::astyle()
{
    if ( m_textEdit )
    {
        m_textEdit->astyleBuffer();
    }
}

void MainWindow::argLoad( QList<QString> argList )
{
    int argCnt = argList.count();

    if ( !Overlord::getInstance()->flagNoAutoLoad() )
    {
        Overlord::getInstance()->openedFilesClear();
        Overlord::getInstance()->openedModifiedClear();
        Overlord::getInstance()->recentFileListClear();
    }

    for ( int k = 1; k < argCnt; k++ )
    {
        QString tempFile = argList.at( k );

        // change to forward slash
        tempFile = QDir::fromNativeSeparators( tempFile );

        // expand for full path
        QFileInfo tempPath( tempFile );
        tempFile = tempPath.canonicalFilePath();

        if ( tempFile.isEmpty() )
        {
            // do nothing

        }
        else if ( Overlord::getInstance()->openedFilesContains( tempFile ) )
        {
            // file is already open

        }
        else if ( QFile::exists( tempFile ) )
        {
            if ( loadFile( tempFile, true, true ) )
            {
                Overlord::getInstance()->openedFilesAppend( tempFile );
            }
        }
    }
}

void MainWindow::autoLoad()
{
    QString fileName;
    int count = Overlord::getInstance()->openedFilesCount();

    if ( count == 0 )
    {
        tabNew();

    }
    else
    {

        for ( int k = 0; k < count; k++ )
        {
            fileName = Overlord::getInstance()->openedFiles( k );

            // load existing files
            loadFile( fileName, true, true );
        }

        QString lastFile = Overlord::getInstance()->lastActiveFile();

        if ( !lastFile.isEmpty() )
        {
            bool found = false;

            for ( int x=0; x < m_tabWidget->count() && !found; x++ )
            {
                DiamondTextEdit *ed = dynamic_cast<DiamondTextEdit *>( m_tabWidget->widget( x ) );

                if ( ed && lastFile == m_tabWidget->tabText( x ) )
                {
                    QTextCursor cursor( ed->document()->findBlockByNumber( Overlord::getInstance()->lastActiveRow() ) );
                    cursor.movePosition( QTextCursor::StartOfLine );
                    cursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, Overlord::getInstance()->lastActiveColumn() - 1 );
                    ed->setTextCursor( cursor );
                    m_tabWidget->setCurrentIndex( x );
                    found = true;
                }
            }
        }
    }
}

void MainWindow::closeEvent( QCloseEvent *event )
{
    QWidget *topWidget = m_tabWidget->currentWidget();

    if ( topWidget != nullptr )
    {
        DiamondTextEdit *ed = dynamic_cast<DiamondTextEdit *>( topWidget );

        if ( ed != nullptr )
        {
            QTextCursor c = ed->textCursor();
            Overlord::getInstance()->set_lastActiveFile( m_tabWidget->tabText( m_tabWidget->currentIndex() ) );
            Overlord::getInstance()->set_lastActiveRow( c.blockNumber() );
            Overlord::getInstance()->set_lastActiveColumn( c.positionInBlock() );
        }
    }

    bool exit = closeAll_Doc( true );

    if ( exit )
    {
        Overlord::getInstance()->set_lastPosition( pos() );
        Overlord::getInstance()->set_lastSize( size() );
        Overlord::getInstance()->close();
        event->accept();

    }
    else
    {
        event->ignore();

    }
}

void MainWindow::documentWasModified()
{
    bool isModified;

    if ( m_isSplit )
    {
        isModified = m_noSplit_textEdit->document()->isModified();
        update_splitCombo( m_curFile, isModified );

    }
    else
    {
        isModified = m_textEdit->document()->isModified();

    }

    setWindowModified( isModified );

    int index = Overlord::getInstance()->openedFilesFind( m_curFile );

    if ( index != -1 )
    {
        bool wasModified = Overlord::getInstance()->openedModified( index );

        if ( wasModified != isModified )
        {
            Overlord::getInstance()->openedModifiedReplace( index,isModified );
        }
    }
}

QString MainWindow::get_curFileName( int whichTab )
{
    QString name = m_tabWidget->tabWhatsThis( whichTab );

    if ( name == "untitled.txt" )
    {
        name = "";
    }

    return name;
}



bool MainWindow::loadFile( QString fileName, bool addNewTab, bool isAuto, bool isReload, bool isReadOnly )
{
#if defined (Q_OS_WIN)
    // change forward to backslash
    fileName.replace( '/', '\\' );
#endif

    // part 1
    if ( addNewTab && ( m_tabWidget->count() > 0 ) )
    {
        // test if fileName is open in another tab
        QFSFileEngine engine( fileName );

        int count = m_tabWidget->count();

        QWidget *temp;
        DiamondTextEdit *textEdit;

        for ( int k = 0; k < count; ++k )
        {

            temp = m_tabWidget->widget( k );
            textEdit = dynamic_cast<DiamondTextEdit *>( temp );

            if ( textEdit )
            {
                QString t_Fname = m_tabWidget->tabWhatsThis( k );
                bool found      = false;

                if ( engine.caseSensitive() )
                {
                    found = ( fileName == t_Fname );

                }
                else
                {
                    // usually only windows
                    found = fileName.compare( t_Fname, Qt::CaseInsensitive ) == 0;
                }

                if ( found )
                {
                    // file is alredy open, select the tab
                    m_textEdit = textEdit;
                    m_tabWidget->setCurrentIndex( k );
                    return true;
                }
            }
        }
    }

    QFile file( fileName );

    if ( ! file.open( QFile::ReadOnly | QFile::Text ) )
    {

        if ( ! isAuto )
        {
            // do not show this message

            QString tmp = fileName;

            if ( tmp.isEmpty() )
            {
                tmp = "(No file name available)";
            }

            QString error = tr( "Unable to open/read file:  %1\n%2." ).formatArgs( tmp, file.errorString() );
            csError( tr( "Open/Read File" ), error );
            return false;
        }
    }

    setStatusBar( tr( "Loading File..." ), 0 );
    QApplication::setOverrideCursor( Qt::WaitCursor );

    if ( addNewTab )
    {
        tabNew();

        Overlord::getInstance()->set_priorPath( pathName( fileName ) );
    }


    if ( m_textEdit->m_owner == "tab" )
    {
        setCurrentTitle( fileName, false, isReload, isReadOnly );
    }

    file.seek( 0 );
    QByteArray temp = file.readAll();

    QString fileData = QString::fromUtf8( temp );

    m_textEdit->setPlainText( fileData );

    QApplication::restoreOverrideCursor();

    if ( m_isSplit )
    {
        // update split combo box
        add_splitCombo( fileName );
    }

    if ( ! addNewTab )
    {
        // recent folders
        rfolder_Add();
    }

    if ( addNewTab && ( ! isAuto ) )
    {
        // update open tab list
        openTab_Add();

        int index = Overlord::getInstance()->openedFilesFind( fileName );

        if ( index != -1 )
        {
            Overlord::getInstance()->openedModifiedReplace( index,false );
        }
    }

    setStatusBar( tr( "File loaded" ), 1500 );

    return true;
}


bool MainWindow::querySave()
{
    if ( m_textEdit->document()->isModified() )
    {

        QString fileName = m_curFile;

        if ( m_curFile.isEmpty() )
        {
            fileName = "(Unknown Filename)";
        }

        QMessageBox quest;
        quest.setWindowTitle( tr( "Diamond Editor" ) );
        quest.setText( fileName + tr( " has been modified. Save changes?" ) );
        quest.setStandardButtons( QMessageBox::Save | QMessageBox::Discard  | QMessageBox::Cancel );
        quest.setDefaultButton( QMessageBox::Cancel );

        int retval = quest.exec();

        if ( retval == QMessageBox::Save )
        {

            if ( fileName == "untitled.txt" )
            {
                return saveAs( Overlord::SAVE_ONE );
            }
            else
            {
                return save();
            }

        }
        else if ( retval == QMessageBox::Cancel )
        {
            return false;

        }
    }

    return true;
}

bool MainWindow::saveFile( QString fileName, Overlord::SaveFiles saveType )
{
#if defined (Q_OS_WIN)
    // change forward to backslash
    fileName.replace( '/', '\\' );
#endif

    if ( QFile::exists( fileName ) )
    {
        if ( Overlord::getInstance()->makeBackups() )
        {
            backupAndTrim( fileName );
        }
    }

    QFile file( fileName );

    if ( ! file.open( QFile::WriteOnly | QFile::Text ) )
    {
        QString tmp = fileName;

        if ( tmp.isEmpty() )
        {
            tmp = tr( "(No file name available)" );
        }

        QString error = tr( "Unable to save/write file %1:\n%2." ).formatArgs( tmp, file.errorString() );
        csError( tr( "Save/Write File" ), error );
        return false;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    if ( Overlord::getInstance()->removeSpaces() )
    {
        deleteEOL_Spaces();
    }

    if ( Overlord::getInstance()->astyleOnSave() )
    {
        QString suffix = suffixName( fileName ).toLower();

        QTextCursor lastCursorPos( m_textEdit->textCursor() );

        int row = lastCursorPos.blockNumber();
        int column = lastCursorPos.positionInBlock();

        if ( validAstyleSuffix( suffix ) )
        {
            QByteArray styledText = m_textEdit->astyleNewBuffer();

            if ( styledText.size() > 0 )
            {
                file.write( styledText );
                QString txt = QString::fromUtf8( styledText );
                m_textEdit->setPlainText( txt );

                QTextCursor cursor( m_textEdit->document()->findBlockByNumber( row ) );
                cursor.movePosition( QTextCursor::StartOfLine );
                cursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, column - 1 );
                m_textEdit->setTextCursor( cursor );
            }
        }
        else
        {
            // just write the files that cannot be styled
            setStatusBar( tr( "Styling produced a zero length file" ), 2000 );
            file.write( m_textEdit->toPlainText().toUtf8() );
        }


    }
    else
    {
        file.write( m_textEdit->toPlainText().toUtf8() );
    }

    m_textEdit->document()->setModified( false );
    int index = Overlord::getInstance()->openedFilesFind( fileName );

    if ( index != -1 )
    {
        Overlord::getInstance()->openedModifiedReplace( index,false );
    }


    if ( m_isSplit )
    {
        update_splitCombo( fileName, false );
    }

    if ( saveType == Overlord::SAVE_ONE )
    {
        setWindowModified( false );
        setDiamondTitle( fileName );

        setStatusBar( tr( "File saved" ), 2000 );
    }

    QApplication::restoreOverrideCursor();
    return true;
}


// title & status bar
void MainWindow::setCurrentTitle( const QString &fileName, bool tabChange, bool isReload, bool isReadOnly )
{
    QString showName;

    // adjusts the * in the title bar
    setWindowModified( m_textEdit->document()->isModified() );

    if ( fileName.isEmpty() )
    {

        m_curFile = "";
        showName  = "untitled.txt";

        setStatus_FName( showName );
        setStatus_ReadWrite( isReadOnly );

        // change the name on the tab to "untitled.txt"
        int index = m_tabWidget->currentIndex();

        m_tabWidget->setTabText( index, showName );
        m_tabWidget->setTabWhatsThis( index, showName );

        if ( Overlord::getInstance()->isComplete() )
        {
            m_textEdit->forceSyntax( SYN_TEXT );
        }

    }
    else
    {
        // loading existing file

        m_curFile = fileName;
        showName  = m_curFile;

        setStatus_FName( m_curFile );
        setStatus_ReadWrite( isReadOnly );

        // change the name on the tab to m_curFile
        int index = m_tabWidget->currentIndex();

        m_textEdit->setCurrentFile( m_curFile );
        m_tabWidget->setTabText( index, strippedName( m_curFile ) );
        m_tabWidget->setTabWhatsThis( index, m_curFile );

        if ( ! Overlord::getInstance()->recentFilesListContains( m_curFile ) )
        {
            rf_Update();
        }

        if ( ! tabChange && ! isReload )
        {
            m_textEdit->setSyntax();
        }
    }

    m_textEdit->setReadOnly( isReadOnly );

    setDiamondTitle( showName );
}

void MainWindow::setDiamondTitle( const QString title )
{
    // displays as: Diamond Editor --  File Name[*]
    QString temp = QChar( 0x02014 );
    setWindowTitle( "Diamond Editor " + temp + " " + title + " [*]" );
}

void MainWindow::setStatus_LineCol()
{
    QTextCursor cursor( m_textEdit->textCursor() );

    // emerald - adjust value when tabs are used instead of spaces
    int adjColNum = cursor.columnNumber()+1;

    m_statusLine->setText( " Line: "  + QString::number( cursor.blockNumber()+1 ) +
                           "  Col: "  + QString::number( adjColNum ) + "  " );
}

void MainWindow::setStatus_ColMode()
{
    if ( Overlord::getInstance()->isColumnMode() )
    {
        m_statusMode->setText( " Column Mode  " );

    }
    else
    {
        m_statusMode->setText( " Line Mode  " );
    }

    m_textEdit->set_ColumnMode( Overlord::getInstance()->isColumnMode() );
}

void MainWindow::setStatus_FName( QString fullName )
{
    m_statusName->setText( " " + fullName + "  " );
}

void MainWindow::setStatus_ReadWrite( bool yesNo )
{
    if ( yesNo )
    {
        m_statusReadWrite->setText( "ReadOnly" );
    }
    else
    {
        m_statusReadWrite->setText( "Write" );
    }
}


// copy buffer
void MainWindow::showCopyBuffer()
{
    QList<QString> copyBuffer = m_textEdit->copyBuffer();

    Dialog_Buffer dw( copyBuffer );
    int result = dw.exec();

    if ( result == QDialog::Accepted )
    {
        int index = dw.get_Index();

        QString text = copyBuffer.at( index );
        m_textEdit->textCursor().insertText( text );
    }
}


// drag & drop
void MainWindow::dragEnterEvent( QDragEnterEvent *event )
{
    if ( event->mimeData()->hasFormat( "text/uri-list" ) )
    {
        event->acceptProposedAction();
    }
    else if ( event->mimeData()->hasFormat( "text/plain" ) )
    {
        event->acceptProposedAction();

    }
}

void MainWindow::dropEvent( QDropEvent *event )
{
    const QMimeData *mimeData = event->mimeData();

    if ( mimeData->hasUrls() )
    {

        QList<QUrl> urls = mimeData->urls();

        if ( urls.isEmpty() )
        {
            return;
        }

        QString fileName = urls.first().toLocalFile();

        if ( ! fileName.isEmpty() )
        {
            loadFile( fileName, true, false );
        }

    }
    else if ( mimeData->hasText() )
    {
        QTextCursor cursor( m_textEdit->textCursor() );

        // set for undo stack
        cursor.beginEditBlock();

        cursor.insertText( mimeData->text() );

        // set for undo stack
        cursor.endEditBlock();
    }

}

// target for lambda
void MainWindow::forceSyntax( SyntaxTypes data )
{
    m_textEdit->forceSyntax( data );
}

void MainWindow::backupAndTrim( QString fileName )
{
    if ( !QFile::exists( fileName ) )
    {
        return;
    }

    QDir backupDir( Overlord::getInstance()->backupDirectory() );
    QStringList filters;
    QString wild = QString( "\'%1.b*\'" ).formatArg( fileName );
    wild.replace( ":", "!" );
    wild.replace( "\\", "!" );
    wild.replace( "/", "!" );

    QString w4 = wild;
    w4.replace( "\'", "*" );
    filters << w4;

    QStringList backupFiles = backupDir.entryList( filters, QDir::Files | QDir::Writable, QDir::Name );

    // Looks weird but we are deleting the lowest backup numbers
    // which will be at the beginning of the list.
    bool okFlag = true;

    while ( ( backupFiles.size() >= Overlord::getInstance()->maxVersions() ) && okFlag )
    {
        QString fName = strippedName( backupFiles.takeFirst() );
        okFlag = backupDir.remove( fName );

        if ( !okFlag )
        {
            QString msg = QString( "Failed to remove " )+  fName;
            csError( tr( "Purging Backups" ), msg );
        }
    }

    int versionNumber = -1;

    if ( backupFiles.size() > 0 )
    {
        QString lastFile = backupFiles.last();
        QString tmp = suffixName( lastFile );
        QRegularExpression rx( "[0-9]+" );
        QRegularExpressionMatch match = rx.match( tmp );

        if ( match.hasMatch() )
        {
            versionNumber = match.captured( 0 ).toInteger<int>();
        }
    }

    versionNumber++;
    QString newSuffix = QString( ".b%1\'" ).formatArg( versionNumber, 5, 10, '0' );
    QString destName = wild;
    destName = destName.replace( ".b*\'", newSuffix );
    destName = backupDir.absoluteFilePath( destName );

    // TODO:: QFile::copy() is busted.
    //        Need to see how long before QFile::copy() will be fixed.
    //
    //        Ubuntu 18 g++7 doesn't have full -std=c++17 support.
    //        at some point g++8 moved filesystem from experimental into
    //        main library so could just #include <filesystem> and use the code below
    //
    //        Don't want to drag g++8 experimental lib around or have convoluted
    //        build to detect when building on partial C++17 support.
    //
    //        Besides, QFile::copy() should get fixed.

#if 0
    std::error_code ec;

    fs::copy( fileName.toStdString(), destName.toStdString(), ec );

    QString ecStr = QString::fromStdString( ec.message() );

#else
    QString cpyCmd;
#ifdef Q_OS_WIN
    cpyCmd = QString( "copy \"%1\" \"%2\"" ).formatArgs( fileName, destName );
#else
    cpyCmd = QString( "cp \"%1\" \"%2\"" ).formatArgs( fileName, destName );
#endif

    if ( system( cpyCmd.toStdString().c_str() ) != EXIT_SUCCESS )
    {
        csError( "Backup", "Failed to make backup copy" );
    }

#endif


    // NOTE: maxVersions from overlord is the maximum number of backup versions
    //       to keep around. If set to 12 we will dutifully keep up to 12,
    //       deleting the lowest versioned one to make room for a new one.
    //
    //       The version NUMBER will continue to increase. Your 12 backup versions
    //       could have version numbers 20000 through 20012.
    //
    //       At some point it is theoretically possible you save so many versions of one
    //       file that you bump into our magic maximum version number. This is when
    //       we start renaming files so the oldest is version zero.
    //
    if ( versionNumber >= DiamondLimits::BACKUP_VERSION_MAX )
    {
        versionNumber = 0;
        // refresh the list
        //
        backupFiles = backupDir.entryList( filters, QDir::Files | QDir::Writable, QDir::Name );

        while ( backupFiles.size() > 0 )
        {
            QString firstFile = backupFiles.takeFirst();
            QString suffix = QString( ".%1" ).formatArg( suffixName( firstFile ) );
            QString newSuffix = QString( ".b%1" ).formatArg( versionNumber, 5, 10, '0' );
            QString newName = strippedName( firstFile );
            newName.replace( suffix, newSuffix );

            backupDir.rename( strippedName( firstFile ), newName );

            versionNumber++;
        }
    }
}


// * find
void MainWindow::find()
{
    QString saveText = Overlord::getInstance()->findText();

    QTextCursor cursor( m_textEdit->textCursor() );
    QString selectedText = cursor.selectedText();

    if ( ! selectedText.isEmpty() )
    {
        Overlord::getInstance()->set_findText( selectedText );
    }

    Dialog_Find dw( this, Overlord::getInstance()->findText(), Overlord::getInstance()->findList() );
    int result = dw.exec();

    if ( result == QDialog::Accepted )
    {

        Overlord::getInstance()->set_findText( dw.get_findText() );
        Overlord::getInstance()->set_findList( dw.get_findList() );

        // add to combo list if not already there
        int index = Overlord::getInstance()->findListFind( Overlord::getInstance()->findText() );

        if ( index == -1 )
        {
            Overlord::getInstance()->findList().prepend( Overlord::getInstance()->findText() );
        }
        else
        {
            Overlord::getInstance()->findList().move( index,0 );
        }

        // get the flags
        Overlord::getInstance()->set_findFlags( 0 );

        Overlord::getInstance()->set_findDirection( dw.get_Direction() );

        if ( ! Overlord::getInstance()->findDirection() )
        {
            Overlord::getInstance()->set_findFlagsBackward();
        }

        Overlord::getInstance()->set_findCase( dw.get_Case() );

        if ( Overlord::getInstance()->findCase() )
        {
            Overlord::getInstance()->set_findFlagsCaseSensitive();
        }

        Overlord::getInstance()->set_findWholeWords( dw.get_WholeWords() );

        if ( Overlord::getInstance()->findWholeWords() )
        {
            Overlord::getInstance()->set_findFlagsWholeWords();
        }

        if ( ! Overlord::getInstance()->findText().isEmpty() )
        {
            bool found = m_textEdit->find( Overlord::getInstance()->findText(), Overlord::getInstance()->findFlags() );

            if ( ! found )
            {
                // text not found, query if the user wants to search from top of file
                findNext();
            }
        }

    }
    else
    {
        Overlord::getInstance()->set_findText( saveText );

        bool upd_Find = dw.get_Upd_Find();

        if ( upd_Find )
        {
            Overlord::getInstance()->set_findList( dw.get_findList() );
        }
    }
}

void MainWindow::findNext()
{
    // emerald - may want to modify m_FindText when text contains html

    QTextDocument::FindFlags flags = QTextDocument::FindFlags( ~QTextDocument::FindBackward
                                     & Overlord::getInstance()->findFlags() );
    bool found = m_textEdit->find( Overlord::getInstance()->findText(), flags );

    if ( ! found )
    {
        QString msg = "Not found: " + Overlord::getInstance()->findText() + "\n\n";
        msg += "Search from the beginning of this document?\n";

        QMessageBox msgFindNext( this );
        msgFindNext.setWindowTitle( "Find" );
        msgFindNext.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
        msgFindNext.setDefaultButton( QMessageBox::Yes );
        msgFindNext.setText( msg );

        int result = msgFindNext.exec();

        if ( result == QMessageBox::Yes )
        {
            // reset to the beginning of the document
            QTextCursor cursor( m_textEdit->textCursor() );
            cursor.movePosition( QTextCursor::Start );
            m_textEdit->setTextCursor( cursor );

            // search again
            this->findNext();
        }
    }
}

void MainWindow::findPrevious()
{
    bool found = m_textEdit->find( Overlord::getInstance()->findText(),
                                   QTextDocument::FindBackward | Overlord::getInstance()->findFlags() );

    if ( ! found )
    {
        csError( "Find", "Not found: " + Overlord::getInstance()->findText() );
    }
}


// * advanced find
void MainWindow::advFind()
{
    QString saveText = Overlord::getInstance()->advancedFindText();

    QTextCursor cursor( m_textEdit->textCursor() );
    QString selectedText = cursor.selectedText();

    if ( ! selectedText.isEmpty() )
    {
        Overlord::getInstance()->set_advancedFindText( selectedText );
    }

    m_dwAdvFind = new Dialog_AdvFind( this,
                                      Overlord::getInstance()->advancedFindText(),
                                      Overlord::getInstance()->advancedFindFileType(),
                                      Overlord::getInstance()->advancedFindFolder(),
                                      Overlord::getInstance()->advancedFSearchFolders() );

    while ( true )
    {
        int result = m_dwAdvFind->exec();

        if ( result == QDialog::Accepted )
        {

            Overlord::getInstance()->set_advancedFindText( m_dwAdvFind->get_findText() );
            Overlord::getInstance()->set_advancedFindFileType( m_dwAdvFind->get_findType() );
            Overlord::getInstance()->set_advancedFindFolder( m_dwAdvFind->get_findFolder() );

            // get the flags
            Overlord::getInstance()->set_advancedFCase( m_dwAdvFind->get_Case() );
            Overlord::getInstance()->set_advancedFWholeWords( m_dwAdvFind->get_WholeWords() );
            Overlord::getInstance()->set_advancedFSearchFolders( m_dwAdvFind->get_SearchSubFolders() );

            if ( ! Overlord::getInstance()->advancedFindText().isEmpty() )
            {

                if ( Overlord::getInstance()->advancedFindFileType().isEmpty() )
                {
                    Overlord::getInstance()->set_advancedFindFileType( "*" );
                }

                if ( Overlord::getInstance()->advancedFindFolder().isEmpty() )
                {
                    Overlord::getInstance()->set_advancedFindFolder( QDir::currentPath() );
                }

                AdvFind_Busy *busy = new AdvFind_Busy( this );
                int rslt = busy->exec();

                if ( QDialog::Rejected == rslt )
                {
                    // do nothing

                }
                else if ( busy->m_foundList.isEmpty() )
                {
                    csError( "Advanced Find", "Not found: " + Overlord::getInstance()->advancedFindText() );
                    delete busy;

                    // allow user to search again
                    continue;

                }
                else
                {
                    this->advFind_ShowFiles( busy->m_foundList );
                    delete busy;

                }
            }
        }
        else
        {
            Overlord::getInstance()->set_advancedFindText( saveText );
        }

        // exit while loop
        break;
    }

    m_dwAdvFind->deleteLater();
    m_dwAdvFind = nullptr;
}

void MainWindow::showBackups()
{
    if ( m_textEdit )
    {
        if ( m_textEdit->currentFile().length() > 0 )
        {
            show_backups( m_textEdit->currentFile(), m_textEdit->get_SyntaxEnum() );
        }
    }
}

void MainWindow::show_backups( QString fileName, SyntaxTypes syntaxType )
{
    if ( fileName.isEmpty() || fileName.length() < 1 )
    {
        csError( tr( "Backups" ), tr( "no filename provided" ) );
        return;
    }

    QDir backupDir( Overlord::getInstance()->backupDirectory() );
    QStringList filters;
    QString wild = QString( "\'%1.b*\'" ).formatArg( fileName );
    wild.replace( ":", "!" );
    wild.replace( "\\", "!" );
    wild.replace( "/", "!" );

    QString w4 = wild;
    w4.replace( "\'", "*" );
    filters << w4;

    QStringList backupFiles = backupDir.entryList( filters, QDir::Files | QDir::Writable, QDir::Name );

    if ( backupFiles.size() < 1 )
    {
        csError( tr( "Backups" ), tr( "no backups found" ) );
        return;
    }


    int index = m_splitter->indexOf( m_findWidget );

    if ( index > 0 )
    {
        m_findWidget->deleteLater();
    }

    // create the find window
    m_findWidget = new QFrame( this );
    m_findWidget->setFrameShape( QFrame::Panel );

    QTableView *view = new QTableView( this );

    m_backupModel = new QStandardItemModel;
    m_backupModel->setColumnCount( 2 );
    m_backupModel->setHeaderData( 0, Qt::Horizontal, tr( "Last Modified" ) );
    m_backupModel->setHeaderData( 1, Qt::Horizontal, tr( "File Name" ) );

    view->setModel( m_backupModel );

    view->setSelectionMode( QAbstractItemView::SingleSelection );
    view->setSelectionBehavior( QAbstractItemView::SelectRows );
    view->setEditTriggers( QAbstractItemView::NoEditTriggers );

    view->setColumnWidth( 0, 200 );
    view->setColumnWidth( 1, 75 );

    view->horizontalHeader()->setStretchLastSection( true );

    // background color
    view->setAlternatingRowColors( true );
    view->setStyleSheet( "alternate-background-color: lightyellow" );

    int row = 0;

    for ( QString fName : backupFiles )
    {

        QFileInfo info( backupDir, fName );
        QStandardItem *item0  = new QStandardItem( info.lastModified().toString( Qt::ISODate ) );
        QStandardItem *item1  = new QStandardItem( fName );

        item1->setData( syntaxType, Qt::UserRole );

        m_backupModel->insertRow( row );
        m_backupModel->setItem( row, 0, item0 );
        m_backupModel->setItem( row, 1, item1 );

        ++row;
    }

    //
    QPushButton *closeButton = new QPushButton();
    closeButton->setText( "Close" );

    QBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget( closeButton );
    buttonLayout->addStretch();

    QBoxLayout *layout = new QVBoxLayout();
    layout->addWidget( view );
    layout->addLayout( buttonLayout );

    m_findWidget->setLayout( layout );

    m_splitter->setOrientation( Qt::Vertical );
    m_splitter->addWidget( m_findWidget );

    // must call after addWidget
    view->resizeRowsToContents();

    connect( view,        &QTableView::clicked,  this, &MainWindow::backup_View );
    connect( closeButton, &QPushButton::clicked, this, &MainWindow::advFind_Close );
}

void MainWindow::advFind_ShowFiles( QList<AdvFindStruct> foundList )
{
    int index = m_splitter->indexOf( m_findWidget );

    if ( index > 0 )
    {
        m_findWidget->deleteLater();
    }

    // create the find window
    m_findWidget = new QFrame( this );
    m_findWidget->setFrameShape( QFrame::Panel );

    QTableView *view = new QTableView( this );

    m_model = new QStandardItemModel;
    m_model->setColumnCount( 3 );
    m_model->setHeaderData( 0, Qt::Horizontal, tr( "File Name" ) );
    m_model->setHeaderData( 1, Qt::Horizontal, tr( "Line #" ) );
    m_model->setHeaderData( 2, Qt::Horizontal, tr( "Text" ) );

    view->setModel( m_model );

    view->setSelectionMode( QAbstractItemView::SingleSelection );
    view->setSelectionBehavior( QAbstractItemView::SelectRows );
    view->setEditTriggers( QAbstractItemView::NoEditTriggers );

    view->setColumnWidth( 0, 300 );
    view->setColumnWidth( 1, 75 );

    view->horizontalHeader()->setStretchLastSection( true );

    // use main window font and size, add feature to allow user to change font
    // following code out for now since the font was too large

//  QFont font = view->font();
//  font.setPointSize(12);
//  view->setFont(font);

    // background color
    view->setAlternatingRowColors( true );
    view->setStyleSheet( "alternate-background-color: lightyellow" );

    int row = 0;

    for ( const auto &entry : foundList )
    {

        QStandardItem *item0  = new QStandardItem( entry.fileName );
        QStandardItem *item1  = new QStandardItem( QString::number( entry.lineNumber ) );
        QStandardItem *item2  = new QStandardItem( entry.text );

        if ( entry.fileName.endsWith( ".wpd" ) )
        {
            item2->setText( "** WordPerfect file, text format incompatible" );
        }

        m_model->insertRow( row );
        m_model->setItem( row, 0, item0 );
        m_model->setItem( row, 1, item1 );
        m_model->setItem( row, 2, item2 );

        ++row;
    }

    //
    QPushButton *closeButton = new QPushButton();
    closeButton->setText( "Close" );

    QBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget( closeButton );
    buttonLayout->addStretch();

    QBoxLayout *layout = new QVBoxLayout();
    layout->addWidget( view );
    layout->addLayout( buttonLayout );

    m_findWidget->setLayout( layout );

    m_splitter->setOrientation( Qt::Vertical );
    m_splitter->addWidget( m_findWidget );

    // must call after addWidget
    view->resizeRowsToContents();

    connect( view,        &QTableView::clicked,  this, &MainWindow::advFind_View );
    connect( closeButton, &QPushButton::clicked, this, &MainWindow::advFind_Close );
}

void MainWindow::advFind_Close()
{
    m_findWidget->close();
    m_findWidget->deleteLater();
    m_findWidget = nullptr;
    // keyboard needs to go back to text edit
    // We are in a slot for a button on the widget
    // we are getting rid of.
    m_refocusTimer->start();
}

void MainWindow::backup_View( const QModelIndex &index )
{
    int row = index.row();

    if ( row < 0 )
    {
        return;
    }

    QString fileName = m_backupModel->item( row,1 )->data( Qt::DisplayRole ).toString();
    int     syntax   = m_backupModel->item( row,1 )->data( Qt::UserRole ).toInt();

    // is the file already open?
    bool open = false;
    int max   = m_tabWidget->count();

    for ( int index = 0; index < max; ++index )
    {
        QString tcurFile = this->get_curFileName( index );

        if ( tcurFile == fileName )
        {
            m_tabWidget->setCurrentIndex( index );

            open = true;
            break;
        }
    }

    //
    if ( ! open )
    {
        QDir backupDir( Overlord::getInstance()->backupDirectory() );
        QString fullName = backupDir.absoluteFilePath( fileName );
        open = loadFile( fullName, true, false, false, true );
        m_textEdit->forceSyntax( static_cast<SyntaxTypes>( syntax ) );
    }

    m_refocusTimer->start();

}

void MainWindow::advFind_View( const QModelIndex &index )
{
    int row = index.row();

    if ( row < 0 )
    {
        return;
    }

    QString fileName = m_model->item( row,0 )->data( Qt::DisplayRole ).toString();
    int lineNumber   = m_model->item( row,1 )->data( Qt::DisplayRole ).toInt();

    if ( fileName.endsWith( ".wpd" ) )
    {
        csError( "Open File", "WordPerfect file, text format incompatible with Diamond" );
        return;
    }

    // is the file already open?
    bool open = false;
    int max   = m_tabWidget->count();

    for ( int index = 0; index < max; ++index )
    {
        QString tcurFile = this->get_curFileName( index );

        if ( tcurFile == fileName )
        {
            m_tabWidget->setCurrentIndex( index );

            open = true;
            break;
        }
    }

    //
    if ( ! open )
    {
        open = loadFile( fileName, true, false );
    }

    if ( open )
    {
        QTextCursor cursor( m_textEdit->textCursor() );
        cursor.movePosition( QTextCursor::Start );
        cursor.movePosition( QTextCursor::NextBlock, QTextCursor::MoveAnchor, lineNumber-1 );
        m_textEdit->setTextCursor( cursor );
    }

    m_refocusTimer->start();
}


// * replace
void MainWindow::replace()
{
    QString saveText = Overlord::getInstance()->findText();

    QTextCursor cursor( m_textEdit->textCursor() );
    QString selectedText = cursor.selectedText();

    if ( ! selectedText.isEmpty() )
    {
        Overlord::getInstance()->set_findText( selectedText );
    }

    Dialog_Replace dw( this,
                       Overlord::getInstance()->findText(),
                       Overlord::getInstance()->findList(),
                       Overlord::getInstance()->replaceText(),
                       Overlord::getInstance()->replaceList() );
    int result = dw.exec();

    if ( result >= QDialog::Accepted )
    {
        Overlord::getInstance()->set_findText( dw.get_findText() );
        Overlord::getInstance()->set_findList( dw.get_findList() );

        // add to list if not found
        int index = Overlord::getInstance()->findListFind( Overlord::getInstance()->findText() );

        if ( index == -1 )
        {
            Overlord::getInstance()->findListPrepend( Overlord::getInstance()->findText() );
        }
        else
        {
            Overlord::getInstance()->findListMove( index,0 );
        }

        Overlord::getInstance()->set_replaceText( dw.get_replaceText() );
        Overlord::getInstance()->set_replaceList( dw.get_replaceList() );

        // add to list if not found
        index = Overlord::getInstance()->replaceListFind( Overlord::getInstance()->replaceText() );

        if ( index == -1 )
        {
            Overlord::getInstance()->replaceListPrepend( Overlord::getInstance()->replaceText() );
        }
        else
        {
            Overlord::getInstance()->replaceListMove( index,0 );
        }

        // get the flags
        Overlord::getInstance()->set_findFlags( 0 );

        Overlord::getInstance()->set_findCase( dw.get_Case() );

        if ( Overlord::getInstance()->findCase() )
        {
            Overlord::getInstance()->set_findFlagsCaseSensitive();
        }

        Overlord::getInstance()->set_findWholeWords( dw.get_WholeWords() );

        if ( Overlord::getInstance()->findWholeWords() )
        {
            Overlord::getInstance()->set_findFlagsWholeWords();
        }

        if ( ! Overlord::getInstance()->findText().isEmpty() && ! Overlord::getInstance()->replaceText().isEmpty() )
        {

            if ( result == 1 )
            {
                replaceQuery();

            }
            else if ( result == 2 )
            {
                replaceAll();

            }
        }

    }
    else
    {
        Overlord::getInstance()->set_findText( saveText );

        bool upd_Find    = dw.get_Upd_Find();
        bool upd_Replace = dw.get_Upd_Replace();

        if ( upd_Find && ! upd_Replace )
        {
            Overlord::getInstance()->set_findList( dw.get_findList() );
        }
        else if ( upd_Replace )
        {
            Overlord::getInstance()->set_findList( dw.get_findList() );
            Overlord::getInstance()->set_replaceList( dw.get_replaceList() );
        }
    }

}

void MainWindow::replaceQuery()
{
    bool isFirst = true;
    bool found;

    // begin undo block
    QTextCursor cursor( m_textEdit->textCursor() );
    cursor.beginEditBlock();

    ReplaceReply *dw = nullptr;

    while ( true )
    {
        found = m_textEdit->find( Overlord::getInstance()->findText(), Overlord::getInstance()->findFlags() );

        if ( found )
        {

            if ( isFirst )
            {
                isFirst = false;
                dw = new ReplaceReply( this );
            }

            // aling near text
            QRect rect  = m_textEdit->cursorRect();
            QPoint point = rect.bottomLeft();
            point.rx() += 25;
            point.ry() += 10;
            dw->move( m_textEdit->mapToGlobal( point ) );

            int result = dw->exec();

            if ( result == QDialog::Rejected )
            {
                break;
            }

            int key = dw->getKey();

            if ( key == Qt::Key_unknown )
            {
                continue;

            }
            else if ( key == Qt::Key_A )
            {
                replaceAll();

            }
            else if ( key == Qt::Key_N )
            {
                continue;

            }
            else if ( key == Qt::Key_O )
            {
                cursor  = m_textEdit->textCursor();
                cursor.insertText( Overlord::getInstance()->replaceText() );

                break;

            }
            else if ( key == Qt::Key_S )
            {
                break;

            }
            else if ( key == Qt::Key_Y )
            {
                cursor  = m_textEdit->textCursor();
                cursor.insertText( Overlord::getInstance()->replaceText() );

            }

        }
        else
        {
            break;

        }
    }

    dw->deleteLater();

    cursor.clearSelection();
    m_textEdit->setTextCursor( cursor );

    // end of undo
    cursor.endEditBlock();

    if ( isFirst )
    {
        csError( "Replace", "Not found: " + Overlord::getInstance()->findText() );
    }
}

void MainWindow::replaceAll()
{
    bool isFirst = true;
    bool found;

    // begin undo block
    QTextCursor cursor( m_textEdit->textCursor() );
    cursor.beginEditBlock();

    while ( true )
    {
        found = m_textEdit->find( Overlord::getInstance()->findText(), Overlord::getInstance()->findFlags() );

        if ( found )
        {
            isFirst = false;

            cursor  = m_textEdit->textCursor();
            cursor.insertText( Overlord::getInstance()->replaceText() );

        }
        else
        {
            break;

        }
    }

    cursor.clearSelection();
    m_textEdit->setTextCursor( cursor );

    // end of undo
    cursor.endEditBlock();

    if ( isFirst )
    {
        csError( "Replace All", "Not found: " + Overlord::getInstance()->findText() );
    }
}


// ** file
void MainWindow::newFile()
{

    // There might have been a valid reason to make File->new whack the existing tab contents instead of
    // creating a new tab like every other tab based editor I've ever used. There was no comment here
    // explaining that decision so I'm changing this to follow industry trends.
    //
    // More than once I whacked files I really wanted.
    //
    // As a compromise, if the current tab is completely empty we whack it. If not, we create a new
    // tab.
#if 0
    bool okClose = querySave();

    if ( okClose )
    {
        m_textEdit->clear();
        setCurrentTitle( "" );
    }

#else

    // for some reason an empty document has a characterCount of 1
    //
    if ( m_textEdit->document()->characterCount() > 1 )
    {
        tabNew();
    }
    else
    {
        m_textEdit->clear();
        setCurrentTitle( "" );
    }

#endif
}

void MainWindow::open_RelatedFile()
{
    QFileInfo tmp( m_curFile );
    QString ext = tmp.suffix();

    if ( ext == "cpp" || ext == "c" || ext == "cc" || ext == "m" || ext == "mm" || ext == "h" )
    {
        QStringList list;

        QString tFile;
        QString baseName = tmp.canonicalPath() + "/" +  tmp.completeBaseName();

        if ( ext == "cpp" || ext == "c" || ext == "cc" || ext == "m" || ext == "mm" )
        {
            tFile = baseName + ".h";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }

            tFile = baseName + "_p.h";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }

        }
        else if ( baseName.endsWith( "_p" ) && ext == "h" )
        {

            baseName.chop( 2 );

            tFile = baseName + ".cpp";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }

            tFile = baseName + ".c";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }

            tFile = baseName + ".cc";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }

            tFile = baseName + ".h";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }

        }
        else if ( ext == "h" )
        {

            tFile = baseName + ".cpp";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }

            tFile = baseName + ".c";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }

            tFile = baseName + ".c";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }

            tFile = baseName + "_p.h";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }
        }

        //
        int cnt = list.count();

        if ( cnt == 0 )
        {
            csError( tr( "Open Related Files" ), tr( "No related files were found" ) );

        }
        else if ( cnt == 1 )
        {
            // open the one related file
            this->loadFile( list.at( 0 ), true, false );

        }
        else
        {
            // display the full list of related files
            Dialog_Open *dw = new Dialog_Open( this, list );
            int result = dw->exec();

            if ( result == QDialog::Accepted )
            {
                QString tmpF = dw->get_FileName();
                this->loadFile( tmpF, true, false );
            }

            delete dw;
        }

    }
    else
    {
        csError( tr( "Open Related Files" ), tr( "Related files only configured for .cpp, .c, .cc, .mm and .h files" ) );

    }
}

void MainWindow::openDoc( QString path )
{
    QString selectedFilter;
    QFileDialog::Options options;

    // force windows 7 and 8 to honor initial path
    options = QFileDialog::ForceInitialDir_Win7;

    QStringList fileList = QFileDialog::getOpenFileNames( this, tr( "Select File" ),
                           path, tr( "All Files (*)" ), &selectedFilter, options );

    for ( const QString &fileName : fileList )
    {
        if ( ! fileName.isEmpty() )
        {
            loadFile( fileName, true, false );
        }
    }
}

bool MainWindow::close_Doc()
{
    bool okClose = querySave();

    if ( okClose )
    {

        if ( m_isSplit )
        {

            if ( m_splitFileName == m_curFile )
            {
                // close the split tab
                qDebug() << "Closing matching split";
                splitCloseClicked();
            }

            rm_splitCombo( m_curFile );
        }

        openTab_Delete();

        m_textEdit->clear();
        setCurrentTitle( "" );
    }

    return okClose;
}

bool MainWindow::closeAll_Doc( bool isExit )
{
    bool allClosed = true;

    QWidget *tmp;
    DiamondTextEdit *textEdit;

    int count = m_tabWidget->count();
    int whichTab = 0;

    // clear open tab list
    Overlord::getInstance()->openedFilesClear();
    Overlord::getInstance()->openedModifiedClear();

    for ( int k = 0; k < count; ++k )
    {

        tmp = m_tabWidget->widget( whichTab );
        textEdit = dynamic_cast<DiamondTextEdit *>( tmp );

        if ( textEdit )
        {
            m_textEdit = textEdit;
            m_curFile  = this->get_curFileName( whichTab );

            bool okClose = querySave();

            if ( okClose )
            {

                if ( isExit && ( m_curFile != "untitled.txt" ) )
                {
                    // save for the auto reload
                    Overlord::getInstance()->openedFilesAppend( m_curFile );
                    Overlord::getInstance()->openedModifiedAppend( false );
                }

                if ( m_tabWidget->count() == 1 )
                {
                    // do not remove this tab !

                    m_textEdit->clear();
                    setCurrentTitle( "" );

                }
                else
                {
                    m_tabWidget->removeTab( whichTab );

                }

            }
            else
            {
                // modified file not closed, move over one tab
                ++whichTab;

                if ( m_curFile != "untitled.txt" )
                {
                    // save for the auto reload
                    Overlord::getInstance()->openedFilesAppend( m_curFile );
                    Overlord::getInstance()->openedModifiedAppend( true );
                }

                // at least one tab is staying open
                allClosed = false;
            }
        }
    }

    if ( isExit && allClosed )
    {
        // about to close diamond

    }
    else
    {
        if ( m_isSplit )
        {
            // close the split tab
            qDebug() << "closing split tab";
            splitCloseClicked();
        }

        // update open tab list
        openTab_UpdateActions();

        m_tabWidget->setCurrentIndex( 0 );
    }

    return allClosed;
}

void MainWindow::reload()
{
    if ( m_curFile.isEmpty() )
    {
        csError( tr( "Reload" ), tr( "Unable to reload a file which was not saved." ) );

    }
    else if ( m_textEdit->document()->isModified() )
    {

        QMessageBox quest;
        quest.setWindowTitle( tr( "Reload File" ) );
        quest.setText( tr( "File: " ) + m_curFile + tr( " has been modified. Reload file?" ) );

        QPushButton *reload = quest.addButton( "Reload", QMessageBox::AcceptRole );
        quest.setStandardButtons( QMessageBox::Cancel );
        quest.setDefaultButton( QMessageBox::Cancel );

        quest.exec();

        if ( quest.clickedButton() == reload )
        {
            loadFile( m_curFile, false, false, true );
        }

    }
    else
    {
        loadFile( m_curFile, false, false, true );

    }
}

bool MainWindow::save()
{
    if ( m_curFile.isEmpty() )
    {
        return saveAs( Overlord::SAVE_ONE );

    }
    else
    {
        return saveFile( m_curFile, Overlord::SAVE_ONE );

    }
}

bool MainWindow::saveAs( Overlord::SaveFiles saveType )
{
    bool retval = false;

    QString selectedFilter;
    QFileDialog::Options options;

    // find the current or prior path
    QString path = pathName( m_curFile );

    if ( path.isEmpty() || path == "." )
    {
        path = Overlord::getInstance()->priorPath();

        if ( path.isEmpty() )
        {
            path = QDir::homePath();
        }
    }


    // force windows 7 and 8 to honor initial path
    options = QFileDialog::ForceInitialDir_Win7;

    QString fileName = QFileDialog::getSaveFileName( this, tr( "Create or Select File" ),
                       path, tr( "All Files (*)" ), &selectedFilter, options );

    if ( fileName.isEmpty() )
    {
        retval = false;

    }
    else
    {
        retval = saveFile( fileName, saveType );

        if ( retval )
        {
            // update open tab list
            openTab_Delete();

            if ( m_isSplit )
            {
                rm_splitCombo( m_curFile );
            }

            setCurrentTitle( fileName );

            // update open tab list
            openTab_Add();

            if ( m_isSplit )
            {
                add_splitCombo( m_curFile );
                set_splitCombo();
            }
        }
    }

    return retval;
}

void MainWindow::saveAll()
{
    // hold for reload
    DiamondTextEdit *hold_textEdit = m_textEdit;
    int hold_index = m_tabWidget->currentIndex();

    QString fileName;

    QWidget *tmp;
    DiamondTextEdit *textEdit;

    int count = m_tabWidget->count();

    for ( int k = 0; k < count; ++k )
    {

        tmp = m_tabWidget->widget( k );
        textEdit = dynamic_cast<DiamondTextEdit *>( tmp );

        if ( textEdit )
        {
            m_textEdit = textEdit;
            fileName   = m_tabWidget->tabWhatsThis( k );

            if ( m_textEdit->document()->isModified() )
            {

                if ( fileName == "untitled.txt" )
                {
                    m_tabWidget->setCurrentIndex( k );
                    saveAs( Overlord::SAVE_ALL );

                }
                else
                {
                    saveFile( fileName, Overlord::SAVE_ALL );

                }
            }
        }
    }

    // reload the current textEdit again
    m_textEdit = hold_textEdit;

    if ( m_tabWidget->currentIndex() == hold_index )
    {

        fileName = m_tabWidget->tabWhatsThis( hold_index );

        if ( fileName == "untitled.txt" )
        {
            m_curFile = "";

        }
        else
        {
            m_curFile = fileName;
            setDiamondTitle( m_curFile );
        }

    }
    else
    {
        m_tabWidget->setCurrentIndex( hold_index );

    }

    if ( ! m_textEdit->document()->isModified() )
    {
        setWindowModified( false );
    }

    if ( m_isSplit )
    {
        set_splitCombo();
    }

    setStatusBar( tr( "File(s) saved" ), 2000 );
}


// **edit
void MainWindow::mw_undo()
{
    m_textEdit->undo();
}

void MainWindow::mw_redo()
{
    m_textEdit->redo();
}

void MainWindow::mw_cut()
{
    m_textEdit->cut();
}

void MainWindow::mw_copy()
{
    m_textEdit->copy();
}

void MainWindow::mw_paste()
{
    m_textEdit->paste();
}

void MainWindow::selectAll()
{
    qDebug() << "menu action selectAll()";
    m_textEdit->selectAll();
}

void MainWindow::selectBlock()
{
    m_textEdit->selectLine();
}

void MainWindow::selectLine()
{
    m_textEdit->selectLine();
}

void MainWindow::selectWord()
{
    m_textEdit->selectWord();
}

void MainWindow::caseUpper()
{
    m_textEdit->caseUpper();
}

void MainWindow::caseLower()
{
    m_textEdit->caseLower();
}

void MainWindow::caseCap()
{
    m_textEdit->caseCap();
}

void MainWindow::insertDate()
{
    m_textEdit->insertDate();
}

void MainWindow::insertTime()
{
    m_textEdit->insertTime();
}

void MainWindow::insertSymbol()
{
    m_textEdit->insertSymbol();
}

void MainWindow::indentIncr( QString route )
{
    m_textEdit->indentIncr( route );
}

void MainWindow::indentDecr( QString route )
{
    m_textEdit->indentDecr( route );
}

void MainWindow::deleteLine()
{
    m_textEdit->deleteLine();
}

void MainWindow::deleteEOL()
{
    m_textEdit->deleteEOL();
}

void MainWindow::deleteThroughEOL()
{
    m_textEdit->deleteThroughEOL();
}

void MainWindow::rewrapParagraph()
{
    m_textEdit->rewrapParagraph();
}

void MainWindow::columnMode()
{
    // alters cut, copy, paste
    Overlord::getInstance()->set_isColumnMode( false );

    if ( m_ui->actionColumn_Mode->isChecked() )
    {
        // on
        Overlord::getInstance()->set_isColumnMode( true );
    }

    setStatus_ColMode();
}


// ** search methods are located in search.cpp

void MainWindow::goLine()
{
    m_textEdit->goLine();
}

void MainWindow::goColumn()
{
    m_textEdit->goColumn();
}

void MainWindow::goTop()
{
    m_textEdit->goTop();
}

void MainWindow::goBottom()
{
    m_textEdit->goBottom();
}


// **view
void MainWindow::lineHighlight()
{
    Overlord::getInstance()->set_showLineHighlight( false );

    if ( m_ui->actionLine_Highlight->isChecked() )
    {
        // on
        Overlord::getInstance()->set_showLineHighlight( true );
    }

    moveBar();
}

void MainWindow::moveBar()
{
    m_textEdit->moveBar();
}

void MainWindow::lineNumbers()
{
    if ( m_ui->actionLine_Numbers->isChecked() )
    {
        //on
        Overlord::getInstance()->set_showLineNumbers( true );
    }
    else
    {
        // off
        Overlord::getInstance()->set_showLineNumbers( false );
    }

}

void MainWindow::wordWrap()
{
    if ( m_ui->actionWord_Wrap->isChecked() )
    {
        //on
        Overlord::getInstance()->set_isWordWrap( true );
        m_textEdit->setWordWrapMode( QTextOption::WordWrap );

    }
    else
    {
        // off
        Overlord::getInstance()->set_isWordWrap( false );
        m_textEdit->setWordWrapMode( QTextOption::NoWrap );

    }

}

void MainWindow::show_Spaces()
{
    QTextDocument *td   = m_textEdit->document();
    QTextOption textOpt = td->defaultTextOption();

    //bool oldValue = Overlord::getInstance()->showSpaces();

    if ( m_ui->actionShow_Spaces->isChecked() )
    {
        //on
        Overlord::getInstance()->set_showSpaces( true );

        if ( Overlord::getInstance()->showBreaks() )
        {
            textOpt.setFlags( QTextOption::ShowTabsAndSpaces | QTextOption::ShowLineAndParagraphSeparators );

        }
        else
        {
            textOpt.setFlags( QTextOption::ShowTabsAndSpaces );
        }

        td->setDefaultTextOption( textOpt );

    }
    else
    {
        // off
        Overlord::getInstance()->set_showSpaces( false );

        if ( Overlord::getInstance()->showBreaks() )
        {
            textOpt.setFlags( QTextOption::ShowLineAndParagraphSeparators );

        }
        else
        {
            textOpt.setFlags( 0 );
        }

        td->setDefaultTextOption( textOpt );
    }
}

void MainWindow::show_Breaks()
{
    QTextDocument *td = m_textEdit->document();
    QTextOption textOpt = td->defaultTextOption();

    if ( m_ui->actionShow_Breaks->isChecked() )
    {
        //on
        Overlord::getInstance()->set_showBreaks( true );

        if ( Overlord::getInstance()->showSpaces() )
        {
            textOpt.setFlags( QTextOption::ShowTabsAndSpaces | QTextOption::ShowLineAndParagraphSeparators );

        }
        else
        {
            textOpt.setFlags( QTextOption::ShowLineAndParagraphSeparators );

        }

        td->setDefaultTextOption( textOpt );

    }
    else
    {
        // off
        Overlord::getInstance()->set_showBreaks( false );

        if ( Overlord::getInstance()->showSpaces() )
        {
            textOpt.setFlags( QTextOption::ShowTabsAndSpaces );

        }
        else
        {
            textOpt.setFlags( 0 );

        }

        td->setDefaultTextOption( textOpt );
    }
}

void MainWindow::displayHTML()
{
    try
    {
        showHtml( "display", m_curFile );

    }
    catch ( std::exception &e )
    {
        // do nothing for now
    }
}


void MainWindow::setSynType( SyntaxTypes data )
{
    m_ui->actionSyn_C->setChecked( false );
    m_ui->actionSyn_Clipper->setChecked( false );
    m_ui->actionSyn_CMake->setChecked( false );
    m_ui->actionSyn_Css->setChecked( false );
    m_ui->actionSyn_Doxy->setChecked( false );
    m_ui->actionSyn_ErrorLog->setChecked( false );
    m_ui->actionSyn_Html->setChecked( false );
    m_ui->actionSyn_Java->setChecked( false );
    m_ui->actionSyn_Javascript->setChecked( false );
    m_ui->actionSyn_Json->setChecked( false );
    m_ui->actionSyn_Makefile->setChecked( false );
    m_ui->actionSyn_Nsis->setChecked( false );
    m_ui->actionSyn_Text->setChecked( false );
    m_ui->actionSyn_Shell->setChecked( false );
    m_ui->actionSyn_Perl->setChecked( false );
    m_ui->actionSyn_PHP->setChecked( false );
    m_ui->actionSyn_Python->setChecked( false );
    m_ui->actionSyn_Xml->setChecked( false );
    m_ui->actionSyn_None->setChecked( false );

    // m_ui->actionSyn_UnUsed1->setChecked(false);
    // m_ui->actionSyn_UnUsed2->setChecked(false);

    switch ( data )
    {
        case SYN_C:
            m_ui->actionSyn_C->setChecked( true );
            break;

        case SYN_CLIPPER:
            m_ui->actionSyn_Clipper->setChecked( true );
            break;

        case SYN_CMAKE:
            m_ui->actionSyn_CMake->setChecked( true );
            break;

        case SYN_CSS:
            m_ui->actionSyn_Css->setChecked( true );
            break;

        case SYN_DOXY:
            m_ui->actionSyn_Doxy->setChecked( true );
            break;

        case SYN_ERRLOG:
            m_ui->actionSyn_ErrorLog->setChecked( true );
            break;

        case SYN_HTML:
            m_ui->actionSyn_Html->setChecked( true );
            break;

        case SYN_JAVA:
            m_ui->actionSyn_Java->setChecked( true );
            break;

        case SYN_JS:
            m_ui->actionSyn_Javascript->setChecked( true );
            break;

        case SYN_JSON:
            m_ui->actionSyn_Json->setChecked( true );
            break;

        case SYN_MAKE:
            m_ui->actionSyn_Makefile->setChecked( true );
            break;

        case SYN_NSIS:
            m_ui->actionSyn_Nsis->setChecked( true );
            break;

        case SYN_TEXT:
            m_ui->actionSyn_Text->setChecked( true );
            break;

        case SYN_SHELL:
            m_ui->actionSyn_Shell->setChecked( true );
            break;

        case SYN_PERL:
            m_ui->actionSyn_Perl->setChecked( true );
            break;

        case SYN_PHP:
            m_ui->actionSyn_PHP->setChecked( true );
            break;

        case SYN_PYTHON:
            m_ui->actionSyn_Python->setChecked( true );
            break;

        case SYN_XML:
            m_ui->actionSyn_Xml->setChecked( true );
            break;

        case SYN_NONE:
            m_ui->actionSyn_None->setChecked( true );
            break;

        case SYN_UNUSED1:
            //m_ui->actionSyn_UnUsed1->setChecked(true);
            break;

        case SYN_UNUSED2:
            //m_ui->actionSyn_UnUsed2->setChecked(true);
            break;
    }
}

void MainWindow::formatUnix()
{
    showNotDone( "Document, format Unix (LF)" );
}

void MainWindow::formatWin()
{
    showNotDone( "Document, format Windows (CR LF)" );
}

void MainWindow::fixTab_Spaces()
{
    m_textEdit->fixTab_Spaces();
}

void MainWindow::fixSpaces_Tab()
{
    m_textEdit->fixSpaces_Tab();
}

void MainWindow::deleteEOL_Spaces()
{
    m_textEdit->deleteEOL_Spaces();
}


// ** tools
void MainWindow::mw_macroStart()
{
    if ( ! m_record )
    {
        m_record = true;
        m_textEdit->macroStart();

        setStatusBar( tr( "Recording macro. . ." ), 0 );
    }
}

void MainWindow::mw_macroStop()
{
    if ( m_record )
    {
        m_record = false;
        m_textEdit->macroStop();

        // TODO:: save macro to global list
        m_currentMacro = m_textEdit->get_MacroKeyList();

        setStatusBar( tr( "Macro recorded" ), 1200 );

    }
    else
    {
        setStatusBar( tr( "No recording in progress" ), 1200 );

    }
}

void MainWindow::macroPlay()
{
    if ( m_record )
    {
        csError( "Macro Playback", "Unable to play back a macro while recording" );

    }
    else
    {
        if ( m_currentMacro.count() < 1 )
        {
            csError( "Macro Playback", "No macro to play back" );

        }
        else
        {
            for ( QKeyEvent *event : m_currentMacro )
            {
                QKeyEvent *newEvent;
                newEvent = new QKeyEvent( *event );

                QApplication::postEvent( m_textEdit, newEvent );
            }
        }
    }
}

void MainWindow::macroRepeat()
{
    bool ok = true;

    int  cnt = QInputDialog::getInt( this, tr( "Repeat Macro" ), tr( "Number of times" ), 0, 0, 5000, 1, &ok );

    if ( ok && ( cnt > 0 ) )
    {
        for ( int ctr = 0; ctr < cnt; ctr++ )
        {
            macroPlay();
        }
    }
}

void MainWindow::macroLoad()
{
    if ( Overlord::getInstance()->macroCount() < 1 )
    {
        csError( "Load Macros", "No exiting macros" );
        return;
    }

    Dialog_Macro *dw = new Dialog_Macro( this, Dialog_Macro::MACRO_LOAD );

    int result = dw->exec();

    if ( result == QDialog::Accepted )
    {
        QString text = dw->get_Macro();

        if ( Overlord::getInstance()->macroExists( text ) )
        {
            deleteCurrentMacro();

            for ( MacroStruct *ptr : Overlord::getInstance()->viewMacro( text ) )
            {
                Qt::KeyboardModifiers modifier = Qt::KeyboardModifiers( ptr->m_modifier );
                QKeyEvent *event = new QKeyEvent( QEvent::KeyPress, ptr->m_key, modifier, ptr->m_text );
                m_currentMacro.append( event );
            }
        }
        else
        {
            csError( "Load Macros", "unknown macro" );
        }

    }

    delete dw;
}

void MainWindow::macroEditNames()
{
    if ( Overlord::getInstance()->macroCount() < 1 )
    {
        csError( "Load Macros", "No exiting macros" );
        return;
    }

    Dialog_Macro *dw = new Dialog_Macro( this, Dialog_Macro::MACRO_MANAGE );

    dw->exec();

    delete dw;
}

// **
void MainWindow::spellCheck()
{
    if ( m_ui->actionSpell_Check->isChecked() )
    {
        //on
        Overlord::getInstance()->set_isSpellCheck( true );

    }
    else
    {
        // off
        Overlord::getInstance()->set_isSpellCheck( false );
    }

    // run for every tab
    int count = m_tabWidget->count();

    QWidget *tmp;
    DiamondTextEdit *textEdit;

    for ( int k = 0; k < count; ++k )
    {

        tmp      = m_tabWidget->widget( k );
        textEdit = dynamic_cast<DiamondTextEdit *>( tmp );

        if ( textEdit )
        {
            // save new values & reHighlight
            textEdit->set_Spell( Overlord::getInstance()->isSpellCheck() );
        }
    }
}

static const double SOURCE_DPI = 100.00;

void MainWindow::print()
{
    QPrinter printer( QPrinter::HighResolution );

    QPrintDialog dialog( &printer, this );
    dialog.setWindowTitle( "Print Document" );

    if ( m_textEdit->textCursor().hasSelection() )
    {
        dialog.addEnabledOption( QAbstractPrintDialog::PrintSelection );
    }

    if ( dialog.exec() == QDialog::Accepted )
    {
        this->printOut( &printer );
    }
}

void MainWindow::printPreview()
{
    // called from menu
    QPrinter printer( QPrinter::HighResolution );

    QPrintPreviewDialog preview( &printer, this );
    preview.setWindowTitle( m_curFile );

    connect( &preview, &QPrintPreviewDialog::paintRequested, this, &MainWindow::printOut );
    preview.exec();
}

void MainWindow::printPdf()
{
    QString outputName = QFileInfo( m_curFile ).baseName() + ".pdf";

    QString selectedFilter;
    QFileDialog::Options options;

    QString fileName = QFileDialog::getSaveFileName( this, tr( "Print to PDF" ),
                       outputName, tr( "PDF File (*.pdf)" ), &selectedFilter, options );

    if ( ! fileName.isEmpty() )
    {

        if ( QFileInfo( fileName ).suffix().isEmpty() )
        {
            fileName.append( ".pdf" );
        }

        QPrinter printer( QPrinter::HighResolution );

        printer.setOutputFormat( QPrinter::PdfFormat );
        printer.setOutputFileName( fileName );

        this->printOut( &printer );
    }
}


// * *
void MainWindow::printOut( QPrinter *printer )
{
    QTextDocument *td = new QTextDocument;

    /*
       // consider this later one
       if (m_ui->actionWord_Wrap->isChecked()) {
          td->setWordWrapMode(QTextOption::WordWrap);
       }
    */

    QString html = "";

    if ( Overlord::getInstance()->printLineNumbers() )
    {

        QTextBlock block = m_textEdit->document()->firstBlock();
        int blockNumber  = block.blockNumber();

        html += "<html><head></head>";
        html += "<body><table border='none' cellspacing='0' cellpadding='0'>";

        while ( block.isValid() )
        {

            html += "<tr>";
            html += "<td  align='right' valign='middle'><b><font size='2'>" + QString::number( blockNumber + 1 ) + "</b></font></td>";
            html += "<td> &nbsp;&nbsp; </td>";
            html += "<td> " + this->convertBlockToHTML( block.text(), Overlord::getInstance()->tabSpacing() ) + "</td>";
            html += "</tr>";

            block = block.next();
            ++blockNumber;
        }

        html += "</table></body></html>";

    }
    else
    {
        html = Qt::convertFromPlainText( m_textEdit->toPlainText() );

    }

    td->setHtml( html );

    printer->setPaperSize( QPageSize::Letter );
    printer->setPageMargins( QMarginsF{ Overlord::getInstance()->marginLeft(),
                                        Overlord::getInstance()->marginTop(),
                                        Overlord::getInstance()->marginRight(),
                                        Overlord::getInstance()->marginBottom()},
                             QPageSize::Inch );

    QPainter painter;

    if ( painter.begin( printer ) )
    {

        m_resolution = printer->logicalDpiX();

        painter.setViewport( printer->paperRect() );

        int winX = 8.5  * m_resolution;
        int winY = 11.0 * m_resolution;
        painter.setWindow( 0, 0, winX, winY );

        td->documentLayout()->setPaintDevice( painter.device() );
        td->setDefaultFont( Overlord::getInstance()->printFontText() );

        // save printarea for header and footer
        double xx = ( 8.5 - Overlord::getInstance()->marginLeft()
                      - Overlord::getInstance()->marginRight() ) * m_resolution;
        double yy = ( 11.0 - Overlord::getInstance()->marginTop()
                      - Overlord::getInstance()->marginBottom() ) * m_resolution;
        QRectF printArea = QRectF( 0, 0, xx, yy );

        // between the header and the body, and the body and the footer
        int spacer = Overlord::getInstance()->headerGap() * m_resolution;

        m_printArea = printArea;

        int headHeight = get_HeaderSize( &painter );
        int footHeight = get_FooterSize( &painter );

        printArea.setTop( printArea.top() + headHeight + spacer );
        printArea.setBottom( printArea.bottom() - ( footHeight + spacer ) );

        QRectF printableRect = QRectF( QPoint( 0,0 ), printArea.size() );
        td->setPageSize( printableRect.size() );

        m_pageNo    = 1;
        m_pageCount = td->pageCount();

        // print header and footer
        this->doHeader( &painter );
        this->doFooter( &painter );

        int headerSpace = + headHeight + spacer;

        for ( int k = 1; k <= m_pageCount; ++k )
        {

            painter.save();

            // move the painter "down"
            painter.translate( 0, ( printableRect.height() * ( k-1 ) * ( -1 ) ) + headerSpace );

            // print one page worth of text
            td->drawContents( &painter, printableRect );
            m_pageNo++;

            // move the document "up"
            printableRect.translate( 0, printableRect.height() );

            painter.restore();

            if ( k < m_pageCount )
            {
                printer->newPage();

                this->doHeader( &painter );
                this->doFooter( &painter );
            }
        }

        painter.end();
    }
}

int MainWindow::get_HeaderSize( QPainter *painter )
{
    if ( ! Overlord::getInstance()->printHeader() )
    {
        return 0;
    }

    painter->save();
    painter->setFont( Overlord::getInstance()->printFontHeader() );

    QString header = "Test line";
    QRect rect     = painter->boundingRect( painter->window(), Qt::AlignLeft, header );

    int size = rect.height();

    if ( !Overlord::getInstance()->headerLine2().isEmpty() )
    {
        size = size * 2;
    }

    painter->restore();

    return size;
}

int MainWindow::get_FooterSize( QPainter *painter )
{
    if ( ! Overlord::getInstance()->printFooter() )
    {
        return 0;
    }

    painter->save();
    painter->setFont( Overlord::getInstance()->printFontFooter() );

    QString footer = "Test line";
    QRect rect     = painter->boundingRect( painter->window(), Qt::AlignLeft, footer );

    int size = rect.height();

    if ( ! Overlord::getInstance()->footerLine2().isEmpty() )
    {
        size = size * 2;
    }

    painter->restore();

    return size;;
}

void MainWindow::doHeader( QPainter *painter )
{
    if ( ! Overlord::getInstance()->printHeader() )
    {
        return;
    }

    QString header;
    QRectF rect1;
    QRectF rectBig =  m_printArea;

    painter->save();
    painter->setFont( Overlord::getInstance()->printFontHeader() );

    //
    header = macroExpand( Overlord::getInstance()->headerLeft() );
    rect1  = painter->boundingRect( rectBig, Qt::AlignLeft, header );
    painter->drawText( rect1, Qt::AlignLeft, header );

    //
    header = macroExpand( Overlord::getInstance()->headerCenter() );
    rect1  = painter->boundingRect( rectBig, Qt::AlignHCenter, header );
    painter->drawText( rect1, Qt::AlignHCenter, header );

    //
    header = macroExpand( Overlord::getInstance()->headerRight() );
    rect1  = painter->boundingRect( rectBig, Qt::AlignRight, header );
    painter->drawText( rect1, Qt::AlignRight, header );

    //
    header = Overlord::getInstance()->headerLine2();

    if ( header.isEmpty() )
    {
        // line after header
        painter->drawLine( rectBig.left(), rect1.bottom()+8, rectBig.right(), rect1.bottom()+8 );

    }
    else
    {
        QRectF rect2 = rectBig;
        rect2.translate( 0, rect1.height() );

        rect2 = painter->boundingRect( rect2, Qt::AlignLeft, header );
        painter->drawText( rect2, Qt::AlignLeft, header );

        // line after header
        painter->drawLine( rectBig.left(), rect2.bottom()+8, rectBig.right(), rect2.bottom()+8 );
    }

    painter->restore();

    return;
}

void MainWindow::doFooter( QPainter *painter )
{
    if ( ! Overlord::getInstance()->printFooter() )
    {
        return;
    }

    QString footer;
    QRectF rect1;
    QRectF rectBig = m_printArea;

    painter->save();
    painter->setFont( Overlord::getInstance()->printFontFooter() );

    //
    QString footer_L2 = footer = Overlord::getInstance()->footerLine2();
    QRectF rect_L2;

    if ( ! footer_L2.isEmpty() )
    {
        rect_L2 = painter->boundingRect( rectBig, Qt::AlignLeft, footer_L2 );

        rect_L2.translate( 0, rectBig.height() - rect_L2.height() );
        painter->drawText( rect_L2, Qt::AlignLeft, footer_L2 );
    }

    //
    footer = macroExpand( Overlord::getInstance()->footerLeft() );
    rect1  = painter->boundingRect( rectBig, Qt::AlignLeft, footer );

    rect1.translate( 0, rectBig.height() - rect1.height() - rect_L2.height() );
    painter->drawText( rect1, Qt::AlignLeft, footer );

    //
    footer = macroExpand( Overlord::getInstance()->footerCenter() );
    rect1  = painter->boundingRect( rectBig, Qt::AlignHCenter, footer );

    rect1.translate( 0, rectBig.height() - rect1.height() - rect_L2.height() );
    painter->drawText( rect1, Qt::AlignHCenter, footer );

    //
    footer = macroExpand( Overlord::getInstance()->footerRight() );
    rect1  = painter->boundingRect( rectBig, Qt::AlignRight, footer );

    rect1.translate( 0, rectBig.height() - rect1.height() - rect_L2.height() );
    painter->drawText( rect1, Qt::AlignRight, footer );

    // line before footer
    painter->drawLine( rectBig.left(), rect1.top()-3, rectBig.right(), rect1.top()-3 );

    painter->restore();

    return;
}

QString MainWindow::macroExpand( QString data )
{
    QString macro;
    QString text;

    int begin;
    int end;

    while ( true )
    {

        begin = data.indexOf( "$(" );

        if ( begin == -1 )
        {
            break;
        }

        end = data.indexOf( ")", begin );

        if ( end == -1 )
        {
            data = data.replace( begin, 2, "" );
            continue;
        }

        macro = data.mid( begin, end-begin+1 );
        text  = "";

        if ( macro == "$(FileName)" )
        {
            text = strippedName( m_curFile );

        }
        else if ( macro == "$(PathFileName)" )
        {
            text = m_curFile;

        }
        else if ( macro == "$(PageNo)" )
        {
            text = QString::number( m_pageNo );

        }
        else if ( macro == "$(TotalPages)" )
        {
            text = QString::number( m_pageCount );

        }
        else if ( macro == "$(Date)" )
        {
            QDate date   = QDate::currentDate();
            text= date.toString( Overlord::getInstance()->formatDate() );

        }
        else if ( macro == "$(Time)" )
        {
            QTime time   = QTime::currentTime();
            text = time.toString( Overlord::getInstance()->formatTime() );

        }

        data = data.replace( begin, end-begin+1, text );
    }

    return data;
}

QString MainWindow::convertBlockToHTML( const QString &plain, int tabSpacing ) const
{
    int col = 0;
    QString retval;

    for ( QChar c : plain )
    {

        if ( c == '\t' )
        {
            retval.append( QChar( 0x00A0 ) );
            ++col;

            while ( col % tabSpacing )
            {
                retval.append( QChar( 0x00A0 ) );
                ++col;
            }

        }
        else if ( c.isSpace() )
        {
            retval.append( QChar( 0x00A0 ) );

        }
        else if ( c == '<' )
        {
            retval.append( "&lt;" );

        }
        else if ( c == '>' )
        {
            retval.append( "&gt;" );

        }
        else if ( c == '&' )
        {
            retval.append( "&amp;" );

        }
        else
        {
            retval.append( c );

        }

        ++col;
    }

    return retval;
}

// ****  recent files
void MainWindow::rf_CreateMenus()
{
    int cnt = Overlord::getInstance()->recentFiles().count();

    QString tName;

    QMenu   *fileMenu = m_ui->menuFile;
    QAction *action   = fileMenu->insertSeparator( m_ui->actionExit );

    for ( int i = 0; i < DiamondLimits::RECENT_FILES_MAX; ++i )
    {

        if ( i < cnt )
        {
            tName = Overlord::getInstance()->recentFiles()[i];
        }
        else
        {
            tName = "file"+QString::number( i );
        }

        rf_Actions[i] = new QAction( tName, this );
        rf_Actions[i]->setData( QString( "recent-file" ) );

        fileMenu->insertAction( action, rf_Actions[i] );

        if ( i >= cnt )
        {
            rf_Actions[i]->setVisible( false );
        }

        connect( rf_Actions[i], SIGNAL( triggered() ), this, SLOT( rf_Open() ) );
    }
}

void MainWindow::rf_Open()
{
    QAction *action;
    action = ( QAction * )sender();

    if ( action )
    {
        bool ok = loadFile( action->text(), true, false );

        if ( ! ok )
        {
            int index = Overlord::getInstance()->recentFiles().indexOf( action->text() );

            if ( index >= 0 )
            {
                Overlord::getInstance()->recentFiles().removeAt( index );
                Overlord::getInstance()->markToNotify();

                // update actions
                rf_UpdateActions();
            }
        }
    }
}

void MainWindow::showContext_Files( const QPoint &pt )
{
    QAction *action = m_ui->menuFile->actionAt( pt );

    if ( action )
    {
        QString data = action->data().toString();

        if ( data == "recent-file" )
        {
            QString fName = action->text();

            QMenu *menu = new QMenu( this );
            menu->addAction( "Clear Recent file list", this, SLOT( rf_ClearList() ) );

            QAction *rfAction = menu->addAction( "Remove file:  " + fName, this, SLOT( rf_RemoveFName() ) );
            rfAction->setData( fName );

            menu->exec( m_ui->menuFile->mapToGlobal( pt ) );
            delete menu;
        }
    }
}

void MainWindow::rf_ClearList()
{
    QAction *action;
    action = ( QAction * )sender();

    if ( action )
    {
        Overlord::getInstance()->recentFileListClear();

        // update actions
        rf_UpdateActions();
    }
}

void MainWindow::rf_RemoveFName()
{
    QAction *action;
    action = ( QAction * )sender();

    if ( action )
    {
        QString fName = action->data().toString();

        int index = Overlord::getInstance()->recentFiles().indexOf( fName );

        if ( index >= 0 )
        {
            Overlord::getInstance()->recentFiles().removeAt( index );
            Overlord::getInstance()->markToNotify();

            // update actions
            rf_UpdateActions();
        }
    }
}

void MainWindow::rf_Update()
{
    int cnt = Overlord::getInstance()->recentFiles().count();

    if ( cnt >= DiamondLimits::RECENT_FILES_MAX )
    {
        Overlord::getInstance()->recentFiles().removeFirst();
    }

    Overlord::getInstance()->recentFiles().append( m_curFile );

    // update actions
    rf_UpdateActions();
}

void MainWindow::rf_UpdateActions()
{
    int cnt = Overlord::getInstance()->recentFiles().count();

    for ( int i = 0; i < DiamondLimits::RECENT_FILES_MAX; ++i )
    {

        if ( i < cnt )
        {
            rf_Actions[i]->setText( Overlord::getInstance()->recentFiles()[i] );
            rf_Actions[i]->setVisible( true );

        }
        else
        {
            rf_Actions[i]->setVisible( false );
        }

    }
}




// ****  recent folders
void MainWindow::rfolder_CreateMenus()
{
    int cnt = Overlord::getInstance()->recentFolders().count();

    QString tName;
    QMenu *menu = new QMenu( this );

    for ( int i = 0; i < DiamondLimits::RECENT_FOLDERS_MAX; ++i )
    {

        if ( i < cnt )
        {
            tName = Overlord::getInstance()->recentFolders()[i];
        }
        else
        {
            tName = "folder"+QString::number( i );
        }

        rfolder_Actions[i] = new QAction( tName, this );
        rfolder_Actions[i]->setData( QString( "recent-folder" ) );

        menu->addAction( rfolder_Actions[i] );

        if ( i >= cnt )
        {
            rfolder_Actions[i]->setVisible( false );
        }

        connect( rfolder_Actions[i], SIGNAL( triggered() ), this, SLOT( rfolder_Open() ) );
    }

    m_ui->actionOpen_RecentFolder->setMenu( menu );

}

void MainWindow::rfolder_Open()
{
    QAction *action;
    action = ( QAction * )sender();

    if ( action )
    {
        // pass the path
        openDoc( action->text() );
    }
}

void MainWindow::showContext_RecentFolder( const QPoint &pt )
{
    QAction *action = m_ui->actionOpen_RecentFolder->menu()->actionAt( pt );

    if ( action )
    {
        QString data = action->data().toString();

        if ( data == "recent-folder" )
        {
            QString fName = action->text();

            QMenu *menu = new QMenu( this );
            menu->addAction( "Clear Recent folder list",  this, SLOT( rfolder_ClearList() ) );

            QAction *rfAction = menu->addAction( "Remove folder:  " + fName, this,  SLOT( rfolder_RemoveFName() ) );
            rfAction->setData( fName );

            menu->exec( QCursor::pos() );
            delete menu;
        }
    }
}

void MainWindow::rfolder_ClearList()
{
    QAction *action;
    action = ( QAction * )sender();

    if ( action )
    {
        Overlord::getInstance()->recentFolders().clear();

        // update actions
        rfolder_UpdateActions();
    }
}

void MainWindow::rfolder_RemoveFName()
{
    QAction *action;
    action = ( QAction * )sender();

    if ( action )
    {
        QString fName = action->data().toString();

        int index = Overlord::getInstance()->recentFolders().indexOf( fName );

        if ( index >= 0 )
        {
            Overlord::getInstance()->recentFolders().removeAt( index );

            // update actions
            rfolder_UpdateActions();
        }
    }
}

void MainWindow::rfolder_Add()
{
    if ( m_curFile.isEmpty() )
    {
        return;
    }

    int cnt = Overlord::getInstance()->recentFolders().count();

    if ( cnt >= DiamondLimits::RECENT_FOLDERS_MAX )
    {
        Overlord::getInstance()->recentFolders().removeFirst();
    }

    QString fileName = pathName( m_curFile );

    if ( ! Overlord::getInstance()->recentFolders().contains( fileName ) )
    {
        Overlord::getInstance()->recentFolders().append( fileName );
    }

    // update actions
    rfolder_UpdateActions();
}

void MainWindow::rfolder_UpdateActions()
{
    int cnt = Overlord::getInstance()->recentFolders().count();

    for ( int i = 0; i < DiamondLimits::RECENT_FOLDERS_MAX; ++i )
    {

        if ( i < cnt )
        {
            rfolder_Actions[i]->setText( Overlord::getInstance()->recentFolders()[i] );
            rfolder_Actions[i]->setVisible( true );

        }
        else
        {
            rfolder_Actions[i]->setVisible( false );
        }

    }
}



// ****  preset folders
void MainWindow::prefolder_CreateMenus()
{
    QString tName;
    QMenu *menu = new QMenu( this );

    for ( int i = 0; i < DiamondLimits::PRESET_FOLDERS_MAX; ++i )
    {

        tName = Overlord::getInstance()->presetFolders()[i];

        if ( tName.isEmpty() )
        {
            continue;
        }

        prefolder_Actions[i] = new QAction( tName, this );
        prefolder_Actions[i]->setData( QString( "preset-folder" ) );

        menu->addAction( prefolder_Actions[i] );
        connect( prefolder_Actions[i], SIGNAL( triggered() ), this, SLOT( prefolder_Open() ) );
    }

    m_ui->actionOpen_PresetFolder->setMenu( menu );

}

void MainWindow::prefolder_Open()
{
    QAction *action;
    action = ( QAction * )sender();

    if ( action )
    {
        // pass the path
        openDoc( action->text() );
    }
}

void MainWindow::prefolder_RedoList()
{
    QMenu *menu = m_ui->actionOpen_PresetFolder->menu();
    menu->deleteLater();

    prefolder_CreateMenus();
}

void MainWindow::openTab_CreateMenus()
{
    // re-populate m_openedFiles
    QString fullName;
    QString tName;

    int cnt = m_tabWidget->count();
    Overlord::getInstance()->openedFilesClear();
    Overlord::getInstance()->openedModifiedClear();

    for ( int k = 0; k < cnt; ++k )
    {
        fullName = this->get_curFileName( k );

        if ( fullName.isEmpty() )
        {
            --cnt;

        }
        else
        {
            Overlord::getInstance()->openedFilesAppend( fullName );
            Overlord::getInstance()->openedModifiedAppend( false );
        }
    }

    //
    QMenu *windowMenu = m_ui->menuWindow;
    windowMenu->addSeparator();

    for ( int k = 0; k < DiamondLimits::OPENTABS_MAX; ++k )
    {

        if ( k < cnt )
        {
            fullName = Overlord::getInstance()->openedFiles( k );
            tName    = fullName;

            if ( Overlord::getInstance()->openedModified( k ) )
            {
                tName += " *";
            }

        }
        else
        {
            tName = "file"+QString::number( k );

        }

        openTab_Actions[k] = new QAction( tName, this );

//    openTab_Actions[k]->setData("select-tab");
        openTab_Actions[k]->setData( QString( "select-tab" ) );

        windowMenu->addAction( openTab_Actions[k] );

        if ( k >= cnt )
        {
            openTab_Actions[k]->setVisible( false );
        }

        connect( openTab_Actions[k], &QAction::triggered, this, [this, k]( bool )
        {
            openTab_Select( k );
        } );
    }
}

void MainWindow::openTab_Select( int index )
{
    bool match = false;
    QString fullName = Overlord::getInstance()->openedFiles( index );

    if ( fullName.isEmpty() )
    {
        // something is pretty bogus

    }
    else
    {
        int cnt   = m_tabWidget->count();
        int index = m_tabWidget->currentIndex();

        for ( int k = 0; k < cnt; ++k )
        {
            QString tcurFile = this->get_curFileName( k );

            if ( tcurFile == fullName )
            {
                match = true;
                index = k;
                break;
            }
        }

        if ( match )
        {
            // select new tab
            m_tabWidget->setCurrentIndex( index );

        }
        else
        {
            // delete entry from list since it did not exist
            Overlord::getInstance()->openedFilesRemove( fullName );

            // update actions
            openTab_UpdateActions();
        }
    }
}

void MainWindow::showContext_Tabs( const QPoint &pt )
{
    QAction *action = m_ui->menuWindow->actionAt( pt );

    if ( action )
    {
        QString data = action->data().toString();

        if ( data == "select-tab" )
        {
            QMenu *menu = new QMenu( this );
            menu->addAction( "Reorder Tab file list", this, SLOT( openTab_redo() ) );

            menu->exec( m_ui->menuWindow->mapToGlobal( pt ) );
            delete menu;
        }
    }
}

void MainWindow::openTab_redo()
{
    QWidget *temp;
    DiamondTextEdit *textEdit;

    QAction *action;
    action = ( QAction * )sender();

    if ( action )
    {
        // re-populate m_openedFiles and m_openedModified
        QString tName;
        bool isModified;

        Overlord::getInstance()->openedFilesClear();
        Overlord::getInstance()->openedModifiedClear();

        int cnt = m_tabWidget->count();

        for ( int k = 0; k < cnt; ++k )
        {
            tName = this->get_curFileName( k );
            Overlord::getInstance()->openedFilesAppend( tName );

            temp = m_tabWidget->widget( k );
            textEdit = dynamic_cast<DiamondTextEdit *>( temp );

            if ( textEdit )
            {
                isModified = textEdit->document()->isModified();
                Overlord::getInstance()->openedModifiedAppend( isModified );
            }
        }

        for ( int i = 0; i < DiamondLimits::OPENTABS_MAX; ++i )
        {

            if ( i < cnt )
            {
                tName = Overlord::getInstance()->openedFiles( i );

                if ( Overlord::getInstance()->openedModified( i ) )
                {
                    tName += " *";
                }

            }
            else
            {
                tName = "file"+QString::number( i );
            }

            openTab_Actions[i]->setText( tName );

            if ( i >= cnt )
            {
                openTab_Actions[i]->setVisible( false );
            }

        }
    }
}

void MainWindow::openTab_Add()
{
    if ( m_curFile.isEmpty() )
    {
        return;
    }

    Overlord::getInstance()->openedFilesAppend( m_curFile );
    Overlord::getInstance()->openedModifiedAppend( false );

    // update actions
    openTab_UpdateActions();
}

void MainWindow::openTab_Delete()
{
    Overlord::getInstance()->openedFilesRemove( m_curFile );

    // update actions
    openTab_UpdateActions();
}

void MainWindow::openTab_UpdateActions()
{
    int cnt = Overlord::getInstance()->openedFilesCount();

    for ( int k = 0; k < DiamondLimits::OPENTABS_MAX; ++k )
    {

        if ( k < cnt )
        {
            QString modified;

            if ( Overlord::getInstance()->openedModified( k ) )
            {
                modified += " *";
            }

            openTab_Actions[k]->setText( Overlord::getInstance()->openedFiles( k ) + modified );
            openTab_Actions[k]->setVisible( true );

        }
        else
        {
            openTab_Actions[k]->setVisible( false );
        }
    }
}

void MainWindow::split_Vertical()
{
    // only allow one for now
    if ( m_isSplit )
    {
        deleteOldSplit();
    }

    m_split_textEdit = new DiamondTextEdit( this, "split" );
    m_splitFileName  = m_curFile;

    // sync documents
    m_split_textEdit->setDocument( m_textEdit->document() );

    if ( m_split_textEdit->get_ColumnMode() )
    {
        m_split_textEdit->setFont( Overlord::getInstance()->fontColumn() );
    }
    else
    {
        m_split_textEdit->setFont( Overlord::getInstance()->fontNormal() );
    }

    QPalette temp = m_split_textEdit->palette();
    temp.setColor( QPalette::Text, Overlord::getInstance()->currentTheme()->colorText() );
    temp.setColor( QPalette::Base, Overlord::getInstance()->currentTheme()->colorBack() );
    m_split_textEdit->setPalette( temp );

    // position on same line
    QTextCursor cursor( m_textEdit->textCursor() );
    m_split_textEdit->setTextCursor( cursor );

    m_isSplit  = true;
    m_textEdit = m_split_textEdit;

    //
    m_splitWidget = new QFrame( this );
    m_splitWidget->setFrameShape( QFrame::Panel );
    m_splitWidget->setWhatsThis( "split_widget" );

    //
    m_splitName_CB = new QComboBox();
    m_splitName_CB->setMinimumWidth( 175 );

    QFont font2 = m_splitName_CB->font();
    font2.setPointSize( 11 );
    m_splitName_CB->setFont( font2 );

    for ( int k = 0; k < Overlord::getInstance()->openedFilesCount(); ++k )
    {

        QString fullName = Overlord::getInstance()->openedFiles( k );
        add_splitCombo( fullName );

        if ( Overlord::getInstance()->openedModified( k ) )
        {
            update_splitCombo( fullName, true );
        }
    }

    //
    m_splitClose_PB = new QPushButton();
    m_splitClose_PB->setText( "Close" );

    QBoxLayout *topbar_Layout = new QHBoxLayout();
    topbar_Layout->addWidget( m_splitName_CB, 1 );
    topbar_Layout->addSpacing( 25 );
    topbar_Layout->addWidget( m_splitClose_PB );
    topbar_Layout->addStretch( 2 );

    //
    QBoxLayout *layout = new QVBoxLayout();
    layout->addLayout( topbar_Layout );
    layout->addWidget( m_split_textEdit );

    m_splitWidget->setLayout( layout );

    m_splitter->setOrientation( Qt::Horizontal );      // difference Here
    m_splitter->addWidget( m_splitWidget );

    //
    int splitIndex = m_splitName_CB->findData( m_splitFileName );
    m_splitName_CB->setCurrentIndex( splitIndex );

    moveBar();

    connect( m_splitName_CB,   static_cast<void ( QComboBox::* )( int )>( &QComboBox::currentIndexChanged ),
             this, &MainWindow::split_NameChanged );

    connect( m_splitClose_PB,  &QPushButton::clicked, this, &MainWindow::splitCloseClicked );

    connect( m_split_textEdit->document(), &QTextDocument::contentsChanged, this, &MainWindow::set_splitCombo );
    connect( m_split_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::moveBar );
    connect( m_split_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::setStatus_LineCol );

    connect( m_split_textEdit, &DiamondTextEdit::undoAvailable, m_ui->actionUndo, &QAction::setEnabled );
    connect( m_split_textEdit, &DiamondTextEdit::redoAvailable, m_ui->actionRedo, &QAction::setEnabled );
    connect( m_split_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCut,  &QAction::setEnabled );
    connect( m_split_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCopy, &QAction::setEnabled );

    connect( m_split_textEdit, &DiamondTextEdit::setSynType, this, &MainWindow::setSynType );
    connect( Overlord::getInstance(), &Overlord::settingsChanged,
             m_split_textEdit, &DiamondTextEdit::changeSettings );

}

void MainWindow::split_Horizontal()
{
    // only allow one for now
    if ( m_isSplit )
    {
        deleteOldSplit();
    }

    m_split_textEdit = new DiamondTextEdit( this, "split" );
    m_splitFileName  = m_curFile;

    // sync documents
    m_split_textEdit->setDocument( m_textEdit->document() );

    if ( m_split_textEdit->get_ColumnMode() )
    {
        m_split_textEdit->setFont( Overlord::getInstance()->fontColumn() );
    }
    else
    {
        m_split_textEdit->setFont( Overlord::getInstance()->fontNormal() );
    }

    QPalette temp = m_split_textEdit->palette();
    temp.setColor( QPalette::Text, Overlord::getInstance()->currentTheme()->colorText() );
    temp.setColor( QPalette::Base, Overlord::getInstance()->currentTheme()->colorBack() );
    m_split_textEdit->setPalette( temp );

    // position on same line
    QTextCursor cursor( m_textEdit->textCursor() );
    m_split_textEdit->setTextCursor( cursor );

    m_isSplit  = true;
    m_textEdit = m_split_textEdit;

    //
    m_splitWidget = new QFrame( this );
    m_splitWidget->setFrameShape( QFrame::Panel );
    m_splitWidget->setWhatsThis( "split_widget" );

    //
    m_splitName_CB = new QComboBox();
    m_splitName_CB->setMinimumWidth( 175 );

    QFont font2 = m_splitName_CB->font();
    font2.setPointSize( 11 );
    m_splitName_CB->setFont( font2 );

    for ( int k = 0; k < Overlord::getInstance()->openedFilesCount(); ++k )
    {
        QString fullName = Overlord::getInstance()->openedFiles( k );
        add_splitCombo( fullName );

        if ( Overlord::getInstance()->openedModified( k ) )
        {
            update_splitCombo( fullName, true );
        }
    }

    //
    m_splitClose_PB = new QPushButton();
    m_splitClose_PB->setText( tr( "Close" ) );

    QBoxLayout *topbar_Layout = new QHBoxLayout();
    topbar_Layout->addWidget( m_splitName_CB, 1 );
    topbar_Layout->addSpacing( 25 );
    topbar_Layout->addWidget( m_splitClose_PB );
    topbar_Layout->addStretch( 2 );

    QBoxLayout *layout = new QVBoxLayout();
    layout->addLayout( topbar_Layout );
    layout->addWidget( m_split_textEdit );

    m_splitWidget->setLayout( layout );

    m_splitter->setOrientation( Qt::Vertical );        // Difference is here
    m_splitter->addWidget( m_splitWidget );

    //
    int splitIndex = m_splitName_CB->findData( m_splitFileName );
    m_splitName_CB->setCurrentIndex( splitIndex );

    moveBar();

    connect( m_splitName_CB,   static_cast<void ( QComboBox::* )( int )>( &QComboBox::currentIndexChanged ),
             this, &MainWindow::split_NameChanged );

    connect( m_splitClose_PB,  &QPushButton::clicked, this, &MainWindow::splitCloseClicked );

    connect( m_split_textEdit->document(), &QTextDocument::contentsChanged, this, &MainWindow::set_splitCombo );
    connect( m_split_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::moveBar );
    connect( m_split_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::setStatus_LineCol );

    connect( m_split_textEdit, &DiamondTextEdit::undoAvailable, m_ui->actionUndo, &QAction::setEnabled );
    connect( m_split_textEdit, &DiamondTextEdit::redoAvailable, m_ui->actionRedo, &QAction::setEnabled );
    connect( m_split_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCut,  &QAction::setEnabled );
    connect( m_split_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCopy, &QAction::setEnabled );

    connect( m_split_textEdit, &DiamondTextEdit::setSynType, this, &MainWindow::setSynType );
    connect( Overlord::getInstance(), &Overlord::settingsChanged,
             m_split_textEdit, &DiamondTextEdit::changeSettings );

}

void MainWindow::set_splitCombo()
{
    QString shortName = strippedName( m_splitFileName );

    bool isModified = m_split_textEdit->document()->isModified();

    if ( isModified )
    {
        shortName += " *";
    }

    int index = Overlord::getInstance()->openedFilesFind( m_splitFileName );

    if ( index != -1 )
    {
        Overlord::getInstance()->openedModifiedReplace( index,isModified );
    }

    //
    int splitIndex = m_splitName_CB->findData( m_splitFileName );

    if ( splitIndex != -1 )
    {
        m_splitName_CB->setItemText( splitIndex, shortName );
        m_splitName_CB->setItemData( splitIndex, m_splitFileName, Qt::ToolTipRole );
    }
}

void MainWindow::update_splitCombo( QString fullName, bool isModified )
{
    QString shortName = strippedName( fullName );

    if ( isModified )
    {
        shortName += " *";
    }

    int splitIndex = m_splitName_CB->findData( fullName );

    if ( splitIndex != -1 )
    {
        m_splitName_CB->setItemText( splitIndex, shortName );
        m_splitName_CB->setItemData( splitIndex,fullName, Qt::ToolTipRole );
    }
}

void MainWindow::add_splitCombo( QString fullName )
{
    int splitIndex = m_splitName_CB->findData( fullName );

    if ( splitIndex == -1 )
    {
        QString shortName = strippedName( fullName );
        m_splitName_CB->addItem( shortName, fullName );

        splitIndex = m_splitName_CB->count() - 1;
        m_splitName_CB->setItemData( splitIndex,fullName, Qt::ToolTipRole );

    }
    else
    {
        set_splitCombo();

    }
}

void MainWindow::rm_splitCombo( QString fullName )
{
    int splitIndex = m_splitName_CB->findData( fullName );

    if ( splitIndex != -1 )
    {
        m_splitName_CB->removeItem( splitIndex );
    }
}

void MainWindow::split_NameChanged( int data )
{
    QString newName = m_splitName_CB->itemData( data ).toString();

    if ( m_splitFileName != newName )
    {

        // old doc
        disconnect( m_split_textEdit->document(), &QTextDocument::contentsChanged,
                    this, &MainWindow::set_splitCombo );

        // new doc
        m_splitFileName = newName;

        int whichTab = -1;

        for ( int k = 0; k < m_tabWidget->count(); ++k )
        {

            if ( newName == this->get_curFileName( k ) )
            {
                whichTab = k;
                break;
            }
        }

        if ( whichTab == -1 )
        {
            csError( tr( "Split Window Selection" ), tr( "Unable to locate selected document" ) );

            deleteOldSplit();
            return;
        }

        QWidget *temp = m_tabWidget->widget( whichTab );
        DiamondTextEdit *textEdit = dynamic_cast<DiamondTextEdit *>( temp );

        if ( textEdit )
        {
            // get document matching the file name
            m_split_textEdit->setDocument( textEdit->document() );

            m_split_textEdit->setReadOnly( textEdit->isReadOnly() );

            if ( m_split_textEdit->get_ColumnMode() )
            {
                m_split_textEdit->setFont( Overlord::getInstance()->fontColumn() );
            }
            else
            {
                m_split_textEdit->setFont( Overlord::getInstance()->fontNormal() );
            }

            QPalette temp = m_split_textEdit->palette();
            temp.setColor( QPalette::Text, Overlord::getInstance()->currentTheme()->colorText() );
            temp.setColor( QPalette::Base, Overlord::getInstance()->currentTheme()->colorBack() );
            m_split_textEdit->setPalette( temp );

            m_textEdit = m_split_textEdit;

            set_splitCombo();

            //
            m_curFile = m_splitFileName;
            setStatus_FName( m_curFile );
            setStatus_ReadWrite( m_textEdit->isReadOnly() );

            // ** retrieve slected syntax type
//       m_syntaxParser = m_textEdit->get_SyntaxParser();

            // retrieve the menu enum
//       m_syntaxEnum = m_textEdit->get_SyntaxEnum();

            // check the menu item
//       setSynType(m_syntaxEnum);

            moveBar();
            show_Spaces();
            show_Breaks();

            connect( m_split_textEdit->document(), &QTextDocument::contentsChanged,
                     this, &MainWindow::set_splitCombo );

        }
        else
        {
            // close the split
            csError( tr( "Split Window Selection" ), tr( "Selected document invalid" ) );
            deleteOldSplit();
        }
    }
}

void MainWindow::deleteOldSplit()
{
    // set focus to the current tab widget
    QWidget *temp = m_tabWidget->currentWidget();
    temp->setFocus();
    m_isSplit = false;


    disconnect( m_splitName_CB,   static_cast<void ( QComboBox::* )( int )>( &QComboBox::currentIndexChanged ),
                this, &MainWindow::split_NameChanged );

    disconnect( m_splitClose_PB,  &QPushButton::clicked, this, &MainWindow::splitCloseClicked );

    disconnect( m_split_textEdit->document(), &QTextDocument::contentsChanged, this, &MainWindow::set_splitCombo );
    disconnect( m_split_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::moveBar );
    disconnect( m_split_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::setStatus_LineCol );

    disconnect( m_split_textEdit, &DiamondTextEdit::undoAvailable, m_ui->actionUndo, &QAction::setEnabled );
    disconnect( m_split_textEdit, &DiamondTextEdit::redoAvailable, m_ui->actionRedo, &QAction::setEnabled );
    disconnect( m_split_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCut,  &QAction::setEnabled );
    disconnect( m_split_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCopy, &QAction::setEnabled );

    m_splitName_CB->clear();
    m_split_textEdit = nullptr;

    m_splitWidget->deleteLater();

}

void MainWindow::splitCloseClicked()
{
    deleteOldSplit();
    // There is a jiggling and juggling of focus with this close button
    // It gets focus back after we leave here.
    //
    m_refocusTimer->start();

}

void MainWindow::refocusTab()
{
    m_refocusTimer->stop();

    // set focus to the current tab widget
    // need to do this in a slot so it can be called by a timer
    // when closing non-modal dialogs we cannot do this within the close
    // of the dialog because it will be ignored.
    if ( m_textEdit )
    {
        m_textEdit->setFocus();
    }
    else
    {
        qWarning() << tr( "m_textEdit was null in refocusTab()" );
    }
}
