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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "diamond_edit.h"
#include "overlord.h"
#include "syntax.h"
#include "ui_mainwindow.h"
#include "util.h"
#include "diamondlimits.h"
#include "macrostruct.h"

#include <QAction>
#include <QComboBox>
#include <QFrame>
#include <QList>
#include <QMenu>
#include <QMainWindow>
#include <QModelIndex>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QPrinter>
#include <QRectF>
#include <QShortcut>
#include <QStandardItemModel>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QSplitter>
#include <QStackedWidget>
#include <QResizeEvent>
#include <QTimer>
#include "advfindstruct.h"

class Dialog_AdvFind;



class MainWindow : public QMainWindow
{
    CS_OBJECT( MainWindow )

public:
    MainWindow( QStringList fileList, QStringList flagList );

    ~MainWindow();


    // indent
    void indentIncr( QString route );
    void indentDecr( QString route );

    // spell
    QStringList spell_getMaybe( QString word );

    // support
    bool loadFile( QString fileName, bool newTab, bool isAuto, bool isReload = false, bool isReadOnly = false );

    void Move( QPoint pos );
    void Resize( QSize size );
    void setSynType( SyntaxTypes data );

    CS_SLOT_1( Public, void startupStep2() )
    CS_SLOT_2( startupStep2 )

    CS_SLOT_1( Public, void startupStep3() )
    CS_SLOT_2( startupStep3 )

    CS_SIGNAL_1( Public, void nextStartupStep() )
    CS_SIGNAL_2( nextStartupStep )

    CS_SIGNAL_1( Public, void queueNextPart() )
    CS_SIGNAL_2( queueNextPart )

    void showEdtHelp();
    void split_Horizontal();
    void split_Vertical();
    void showClipboard();

    void refocusTab();

protected:
    void closeEvent( QCloseEvent *event );
    void dragEnterEvent( QDragEnterEvent *event );
    void dropEvent( QDropEvent *event );
    void resizeEvent( QResizeEvent *event );

private:
    // methods come first
    //
    void deleteCurrentMacro();
    void getConfigFileName();
    void add_splitCombo( QString fullName );
    void rm_splitCombo( QString fullName );
    void update_splitCombo( QString fullName, bool isModified );
    void advFind_ShowFiles( QList<AdvFindStruct> foundList );
    void onceEventLoopStarts();
    int getReply();
    void replaceQuery();
    void replaceAll();
    void autoLoad();
    void argLoad( QList<QString> argList );
    void prefolder_CreateMenus();
    void prefolder_RedoList();
    void rfolder_CreateMenus();
    void rfolder_Add();
    void rfolder_UpdateActions();
    void rf_CreateMenus();
    void rf_Update();
    void rf_UpdateActions();
    void openTab_CreateMenus();
    void openTab_Add();
    void openTab_Delete();
    void openTab_UpdateActions();
    void openTab_Select( int index );
    void openDoc( QString path );
    bool closeAll_Doc( bool isExit );
    void save_ConfigFile();
    void createShortCuts( bool setupAll );
    void createToolBars();
    void createStatusBar();
    void createConnections();
    void createToggles();
    QString adjustKey( QString sequence );
    void setStatusBar( QString msg, int timeOut );
    void setStatus_ColMode();
    void setStatus_FName( QString name );
    void showNotDone( QString item );
    void setStatus_ReadWrite( bool yesNo );
    QString get_SyntaxPath( QString syntaxPath );
    QString get_xxFile( QString title, QString fname, QString filter );
    int get_HeaderSize( QPainter *painter );
    int get_FooterSize( QPainter *painter );
    void doHeader( QPainter *painter );
    void doFooter( QPainter *painter );
    QString macroExpand( QString macro );
    QString convertBlockToHTML( const QString &plain, int tabSpacing ) const;
    bool querySave();
    bool saveFile( QString fileName, Overlord::SaveFiles saveType );
    bool saveAs( Overlord::SaveFiles saveType );
    void setCurrentTitle( const QString &fileName, bool tabChange = false, bool isReload = false, bool isReadOnly = false );
    void setDiamondTitle( const QString title );
    void forceSyntax( SyntaxTypes data );
    QString get_curFileName( int whichTab );
    void focusChanged( QWidget *prior, QWidget *current );
    void newFile();
    void open_RelatedFile();
    bool close_Doc();
    void reload();
    bool save();
    void saveAll();
    void print();
    void printOut( QPrinter * );
    void printPdf();
    void printPreview();
    void mw_undo();
    void mw_redo();
    void mw_cut();
    void mw_copy();
    void mw_paste();
    void selectBlock();
    void selectLine();
    void selectWord();
    void caseCap();
    void deleteThroughEOL();
    void insertSymbol();
    void columnMode();
    void find();
    void replace();
    void findNext();
    void findPrevious();
    void advFind();
    void goLine();
    void goColumn();
    void goTop();
    void goBottom();
    void lineHighlight();
    void moveBar();
    void lineNumbers();
    void wordWrap();
    void show_Spaces();
    void show_Breaks();
    void displayHTML();
    void formatUnix();
    void formatWin();
    void fixTab_Spaces();
    void fixSpaces_Tab();
    void deleteEOL_Spaces();
    void mw_macroStart();
    void mw_macroStop();
    void macroPlay();
    void macroRepeat();
    void macroLoad();
    void macroEditNames();
    void spellCheck();
    void astyle();
    void setColors();
    void setFont();
    void setOptions();
    void setPresetFolders();
    void setPrintOptions();
    void move_ConfigFile();
    void tabNew();
    void tabClose( int index );
    void diamondHelp();
    void about();
    void documentWasModified();
    void setStatus_LineCol();
    void mw_tabClose();
    void tabChanged( int index );
    void showBackups();
    void show_backups( QString fileName, SyntaxTypes syntaxType );
    void showCopyBuffer();
    void showContext_RecentFolder( const QPoint &pt );
    void showContext_Files( const QPoint &pt );
    void showContext_Tabs( const QPoint &pt );
    void set_splitCombo();
    void split_NameChanged( int data );
    void splitCloseClicked();
    void deleteOldSplit();
    void backupAndTrim( QString fileName );

    //    slots
    //
    CS_SLOT_1( Private, void selectAll() )
    CS_SLOT_2( selectAll )

    CS_SLOT_1( Private, void caseUpper() )
    CS_SLOT_2( caseUpper )

    CS_SLOT_1( Private, void caseLower() )
    CS_SLOT_2( caseLower )

    CS_SLOT_1( Private, void deleteLine() )
    CS_SLOT_2( deleteLine )

    CS_SLOT_1( Private, void deleteEOL() )
    CS_SLOT_2( deleteEOL )

    CS_SLOT_1( Private, void insertDate() )
    CS_SLOT_2( insertDate )

    CS_SLOT_1( Private, void insertTime() )
    CS_SLOT_2( insertTime )

    CS_SLOT_1( Private, void rewrapParagraph() )
    CS_SLOT_2( rewrapParagraph )

    CS_SLOT_1( Private, void advFind_View( const QModelIndex &index ) )
    CS_SLOT_2( advFind_View )

    CS_SLOT_1( Private, void advFind_Close() )
    CS_SLOT_2( advFind_Close )

    CS_SLOT_1( Private, void backup_View( const QModelIndex &index ) )
    CS_SLOT_2( advFind_View )

    CS_SLOT_1( Private, void rfolder_Open() )
    CS_SLOT_2( rfolder_Open )

    CS_SLOT_1( Private, void rfolder_ClearList() )
    CS_SLOT_2( rfolder_ClearList )

    CS_SLOT_1( Private, void rfolder_RemoveFName() )
    CS_SLOT_2( rfolder_RemoveFName )

    CS_SLOT_1( Private, void prefolder_Open() )
    CS_SLOT_2( prefolder_Open )

    CS_SLOT_1( Private, void rf_Open() )
    CS_SLOT_2( rf_Open )

    CS_SLOT_1( Private, void rf_ClearList() )
    CS_SLOT_2( rf_ClearList )

    CS_SLOT_1( Private, void rf_RemoveFName() )
    CS_SLOT_2( rf_RemoveFName )

    CS_SLOT_1( Private, void openTab_redo() )
    CS_SLOT_2( openTab_redo )


    //;;;;;
    // data members go here
    //;;;;;
    Ui::MainWindow *m_ui;

    QString m_appPath;

    QString m_configFileName;
    QStringList m_fileList;
    QStringList m_flagList;
    QList<QKeyEvent *> m_currentMacro;
    DiamondTextEdit *m_textEdit;
    QTabWidget *m_tabWidget;
    QString m_curFile;
    QTimer *m_refocusTimer;
    DiamondTextEdit *m_split_textEdit;
    DiamondTextEdit *m_noSplit_textEdit;
    QSplitter *m_splitter;
    bool m_isSplit;
    QFrame *m_splitWidget;
    QString m_splitFileName;
    QComboBox *m_splitName_CB;
    QPushButton *m_splitClose_PB;
    QShortcut *m_actionCopyBuffer;
    Dialog_AdvFind *m_dwAdvFind;
    QFrame *m_findWidget;
    QStandardItemModel *m_model;
    QStandardItemModel *m_backupModel;
    bool m_record;
    QAction *prefolder_Actions[DiamondLimits::PRESET_FOLDERS_MAX];
    QAction *rfolder_Actions[DiamondLimits::RECENT_FOLDERS_MAX];
    QAction *rf_Actions[DiamondLimits::RECENT_FILES_MAX];
    QAction *openTab_Actions[DiamondLimits::OPENTABS_MAX];
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *searchToolBar;
    QToolBar *viewToolBar;
    QToolBar *toolsToolBar;
    QToolBar *windowToolBar;
    QLabel *m_statusLine;
    QLabel *m_statusMode;
    QLabel *m_statusReadWrite;
    QLabel *m_statusName;
    int m_pageNo;
    int m_pageCount;
    QRectF m_printArea;
    double m_resolution;

};

#endif
