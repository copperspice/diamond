/**************************************************************************
*
* Copyright (c) 2012-2024 Barbara Geller
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
#include "settings.h"
#include "spellcheck.h"
#include "syntax.h"
#include "ui_mainwindow.h"
#include "util.h"

#include <QAction>
#include <QComboBox>
#include <QFrame>
#include <QJsonObject>
#include <QList>
#include <QMainWindow>
#include <QMenu>
#include <QModelIndex>
#include <QPlainTextEdit>
#include <QPrinter>
#include <QPushButton>
#include <QRectF>
#include <QShortcut>
#include <QSplitter>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QStandardPaths>
#include <QString>
#include <QStringList>

class Dialog_AdvFind;

static const int MACRO_MAX           = 10;
static const int OPENTABS_MAX        = 20;
static const int PRESET_FOLDERS_MAX  = 15;
static const int RECENT_FOLDERS_MAX  = 10;
static const int RECENT_FILES_MAX    = 10;

struct macroStruct
{
   int key;
   int modifier;
   QString text;
};

struct advFindStruct
{
   QString fileName;
   int lineNumber;
   QString text;
};

class MainWindow : public QMainWindow
{
   CS_OBJECT(MainWindow)

   public:
      MainWindow(QStringList fileList, QStringList flagList);
      struct Settings get_StructData();

      // fonts
      void changeFont();

      // indent
      void indentIncr(QString route);
      void indentDecr(QString route);

      // macros
      void json_Save_MacroNames(QStringList macroNames);
      QList<macroStruct> json_View_Macro(QString macroName);

      // spell
      QStringList spell_getMaybe(QString word);

      // support
      QString get_DirPath(QString message, QString path);
      bool loadFile(QString fileName, bool newTab, bool isAuto, bool isReload = false);

   protected:
      void closeEvent(QCloseEvent *event) override;
      void dragEnterEvent(QDragEnterEvent *event) override;
      void dropEvent(QDropEvent *event) override;

   private:

      enum SaveFiles {
         SAVE_ONE,
         SAVE_ALL
      };

      void add_splitCombo(QString fullName);
      void rm_splitCombo(QString fullName);
      void update_splitCombo(QString fullName, bool isModified);

      int getReply();

      QList<advFindStruct> advFind_getResults(bool &aborted);
      void findRecursive(const QString &path, bool isFirstLoop = true);
      void advFind_ShowFiles(QList<advFindStruct> foundList);

      QStringList m_macroNames;

      void replaceQuery();
      void replaceAll();

      // passed parms
      void autoLoad();
      void argLoad(QList<QString> argList);

      void prefolder_CreateMenus();
      void prefolder_RedoList();
      // void prefolder_UpdateActions();

      void rfolder_CreateMenus();
      void rfolder_Add();
      void rfolder_UpdateActions();

      void rf_CreateMenus();
      void rf_Update();
      void rf_UpdateActions();

      void setUpTabStops();

      void openTab_CreateMenus();
      void openTab_Add();
      void openTab_Delete();
      void openTab_UpdateActions();

      void openTab_Select(int index);
      void openTab_UpdateOneAction(int index, bool isModified);

      void createSpellCheck();


      enum Option { ABOUTURL, ADVFIND, AUTOLOAD, CLOSE, COLORS, COLUMN_MODE, DICT_MAIN, DICT_USER, FIND_LIST,
                    FIND_REPLACE, FONT, FORMAT_DATE, FORMAT_TIME, KEYS, MACRO, MACRO_NAMES, PATH_SYNTAX,
                    PATH_PRIOR, PRESET_FOLDER, PRINT_OPTIONS, RECENTFOLDER, RECENTFILE, REMOVE_SPACE,
                    REWRAP_COLUMN, SHOW_LINEHIGHLIGHT, SHOW_LINENUMBERS, SHOW_SPACES, SHOW_BREAKS, SPELLCHECK,
                    TAB_SPACING, USESPACES, WORDWRAP};

      enum Config { CFG_STARTUP, CFG_DEFAULT };

      void openDoc(QString path);
      bool closeAll_Doc(bool isExit);
      void save_ConfigFile();

      void setScreenColors();
      void setSyntax();
      void setSynType(SyntaxTypes syntaxData);
      void forceSyntax(SyntaxTypes syntaxData);

      void createShortCuts(bool setupAll);
      void createToolBars();
      void createStatusBar();
      void createConnections();
      void createToggles();

      QString adjustKey(QString sequence);

      void setStatusBar(QString msg, int timeOut);
      void setStatus_ColMode();
      void setStatus_FName(QString name);
      void showNotDone(QString item);

      // json
      bool json_Read(Config trail = CFG_DEFAULT);
      bool json_Write(Option route, Config trail = CFG_DEFAULT);
      void json_getFileName();
      bool json_CreateNew();
      bool json_SaveFile(QByteArray route);
      QByteArray json_ReadFile();

      void json_setTabList(QStringList list);
      QStringList json_getTabList();

      QStringList json_Load_MacroIds();
      bool json_Load_Macro(QString macroName);

      QString get_SyntaxPath(QString syntaxPath);
      QString get_xxFile(QString title, QString fname, QString filter);

      QFont json_SetFont(QString value);
      QColor json_SetColor(QString values);
      QString json_GetRGB(QColor color);
      QJsonObject json_SaveSyntax(QJsonObject object);

      // printing
      int m_pageNo;
      int m_pageCount;

      QRectF m_printArea;
      double m_resolution;

      int get_HeaderSize(QPainter *painter);
      int get_FooterSize(QPainter *painter);

      void doHeader(QPainter *painter);
      void doFooter(QPainter *painter);
      QString macroExpand(QString macro);
      QString convertBlockToHTML(const QString &plain) const;

      // support
      int get_line_col(const QString route);
      bool querySave();
      bool saveFile(QString fileName, SaveFiles saveType);
      bool saveAs(SaveFiles saveType);

      void setCurrentTitle(const QString &fileName, bool tabChange = false, bool isReload = false);
      void setDiamondTitle(const QString title);

      QString get_curFileName(int whichTab);
      QString pathName(QString fileName) const;
      QString strippedName(const QString filename);
      QString suffixName() const;

      void focusChanged(QWidget *prior, QWidget *current);

      void newFile();
      void open_RelatedFile();

      bool close_Doc();
      void reload();
      bool save();
      void saveAll();
      void print();

      void printOut(QPrinter *);
      void printPdf();
      void printPreview();

      void mw_undo();
      void mw_redo();
      void mw_cut();
      void mw_copy();
      void mw_paste();

      CS_SLOT_1(Private, void selectAll())
      CS_SLOT_2(selectAll)

      void selectBlock();
      void selectLine();
      void selectWord();

      CS_SLOT_1(Private, void caseUpper())
      CS_SLOT_2(caseUpper)

      CS_SLOT_1(Private, void caseLower())
      CS_SLOT_2(caseLower)

      void caseCap();

      CS_SLOT_1(Private, void deleteLine())
      CS_SLOT_2(deleteLine)

      CS_SLOT_1(Private, void deleteEOL())
      CS_SLOT_2(deleteEOL)

      CS_SLOT_1(Private, void insertDate())
      CS_SLOT_2(insertDate)

      CS_SLOT_1(Private, void insertTime())
      CS_SLOT_2(insertTime)

      CS_SLOT_1(Private, void rewrapParagraph())
      CS_SLOT_2(rewrapParagraph)

      CS_SLOT_1(Private, void sortLines())
      CS_SLOT_2(sortLines)

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

      // document
      void formatUnix();
      void formatWin();
      void fixTab_Spaces();
      void fixSpaces_Tab();
      void deleteEOL_Spaces();

      // tools
      void mw_macroStart();
      void mw_macroStop();
      void macroPlay();
      void macroLoad();
      void macroEditNames();
      void spellCheck();
      void saveTabs();
      void loadTabs();

      // options
      void setColors();
      void setFont();
      void setOptions();
      void setPresetFolders();
      void setPrintOptions();
      void move_ConfigFile();
      void tabNew();
      void tabClose(int index);

      // help
      void diamondHelp();
      void about();

      //
      void documentWasModified();
      void setStatus_LineCol();
      void mw_tabClose();
      void tabChanged(int index);

      // adv find
      void advFind_View(const QModelIndex &index);
      void advFind_Close();

      // copy buffer
      void showCopyBuffer();

      // spell check
      CS_SLOT_1(Private, void spell_addUserDict())
      CS_SLOT_2(spell_addUserDict)

      CS_SLOT_1(Private, void spell_replaceWord())
      CS_SLOT_2(spell_replaceWord)

      // recent folders
      void showContext_RecentFolder(const QPoint &pt);
      void rfolder_Open();

      CS_SLOT_1(Private, void rfolder_ClearList())
      CS_SLOT_2(rfolder_ClearList)

      CS_SLOT_1(Private, void rfolder_RemoveFName())
      CS_SLOT_2(rfolder_RemoveFName)

      // preset folders
      void prefolder_Open();

      // recent files
      void showContext_Files(const QPoint &pt);
      void rf_Open();

      CS_SLOT_1(Private, void rf_ClearList())
      CS_SLOT_2(rf_ClearList)

      CS_SLOT_1(Private, void rf_RemoveFName())
      CS_SLOT_2(rf_RemoveFName)

      // open (tab) files
      void showContext_Tabs(const QPoint &pt);

      CS_SLOT_1(Private, void openTab_redo())
      CS_SLOT_2(openTab_redo)

      // split
      void set_splitCombo();
      void split_Horizontal();
      void split_Vertical();
      void split_NameChanged(int itemNum);
      void split_CloseButton();

      Ui::MainWindow *m_ui;

      DiamondTextEdit *m_textEdit;
      QTabWidget *m_tabWidget;
      QString m_curFile;

      QStringList m_openedFiles;
      QList<bool> m_openedModified;

      DiamondTextEdit *m_split_textEdit;
      DiamondTextEdit *m_noSplit_textEdit;
      QSplitter *m_splitter;

      bool m_isSplit;
      QFrame *m_splitWidget;
      QString m_splitFileName;
      QComboBox *m_splitName_CB;
      QPushButton *m_splitClose_PB;

      QShortcut *m_actionCopyBuffer;

      QString m_findText;
      QStringList m_findList;
      QTextDocument::FindFlags m_flags;
      bool m_fDirection;
      bool m_fMatchCase;
      bool m_fWholeWords;

      Dialog_AdvFind *m_dwAdvFind;
      QString m_advFindText;
      QString m_advFindFileType;
      QString m_advFindFolder;

      bool m_advFMatchCase  = false;
      bool m_advFWholeWords = false;
      bool m_advFRegexp     = false;

      bool m_advFSearchFolders;

      QStringList m_recursiveList;
      QFrame *m_findWidget;
      QStandardItemModel *m_model;

      // replace
      QString m_replaceText;
      QStringList m_replaceList;

      // macros
      bool m_record;
      QList<QKeyEvent *> m_macroList;

      // preset folders
      QAction *prefolder_Actions[PRESET_FOLDERS_MAX];
      QStringList m_prefolder_List;

      // recent folders
      QAction *rfolder_Actions[RECENT_FOLDERS_MAX];
      QStringList m_rfolder_List;

      // recent files
      QAction *rf_Actions[RECENT_FILES_MAX];
      QStringList m_rf_List;

      // syntax
      QString m_appPath;
      QString m_jsonFname;
      SyntaxTypes m_syntaxEnum;
      Syntax *m_syntaxParser;
      void runSyntax(QString synFName);

      // settings
      struct Arguments m_args;
      struct Settings m_struct;
      struct PrintSettings m_printer;

      // tab stops
      QList<int> m_tabStops;

      // open tabs
      QAction *openTab_Actions[OPENTABS_MAX];

      // spell check
      SpellCheck *m_spellCheck;

      // menu bar
      QToolBar *fileToolBar;
      QToolBar *editToolBar;
      QToolBar *searchToolBar;
      QToolBar *viewToolBar;
      QToolBar *toolsToolBar;
      QToolBar *windowToolBar;

      // status bar
      QLabel *m_statusLine;
      QLabel *m_statusMode;
      QLabel *m_statusName;
};

#endif
