/**************************************************************************
*
* Copyright (c) 2012-2014 Barbara Geller
* All rights reserved.
*
* This file is part of Diamond Editor.
*
* Diamond Editor is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License version 3
* as published by the Free Software Foundation.
*
* Diamond Editor is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Diamond Editor.  If not, see <http://www.gnu.org/licenses/>.
*
**************************************************************************/

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
#include <QFile>
#include <QFrame>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QMenu>
#include <QMainWindow>
#include <QModelIndex>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRectF>
#include <QShortcut>
#include <QStandardItemModel>
#include <QStandardPaths>
#include <QStringList>
#include <QSplitter>
#include <QStackedWidget>
#include <QTabWidget>

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
      bool loadFile(const QString &fileName, bool newTab, bool isAuto);

   protected:
      void closeEvent(QCloseEvent *event);
      void dragEnterEvent(QDragEnterEvent *event);
      void dropEvent(QDropEvent *event);

   private:
      Ui::MainWindow *m_ui;      

      // textEdit
      DiamondTextEdit *m_textEdit;      
      QTabWidget *m_tabWidget;
      QString m_curFile;     

      QStringList m_openedFiles;
      QList<bool> m_openedModified;

      // split
      DiamondTextEdit *m_split_textEdit;
      DiamondTextEdit *m_noSplit_textEdit;
      QSplitter *m_splitter;     

      bool m_isSplit;
      QFrame *m_splitWidget;
      QString m_splitFileName;
      QComboBox *m_splitName_CB;
      QPushButton *m_splitClose_PB;

      void add_splitCombo(QString fullName);
      void rm_splitCombo(QString fullName);
      void update_splitCombo(QString fullName, bool isModified);

      // copy buffer
      QShortcut *m_actionCopyBuffer;

      // find
      QString m_findText;
      QStringList m_findList;
      QTextDocument::FindFlags m_flags;
      bool m_fDirection;
      bool m_fCase;
      bool m_fWholeWords;

      // advanced find
      Dialog_AdvFind *m_dwAdvFind;
      QString m_advFindText;
      QString m_advFindFileType;
      QString m_advFindFolder;     

      bool m_advFCase;
      bool m_advFWholeWords;
      bool m_advFSearchFolders;

      QStringList m_recursiveList;
      QFrame *m_findWidget;
      QStandardItemModel *m_model;
      QList<advFindStruct> advFind_getResults(bool &aborted);
      void findRecursive(const QString &path, bool isFirstLoop = true);
      void advFind_ShowFiles(QList<advFindStruct> foundList);

      // replace
      QString m_replaceText;
      QStringList m_replaceList;
      int getReply();

      // macros    
      bool m_record;
      QList<QKeyEvent *> m_macroList;
      QStringList m_macroNames;

      void replaceQuery();
      void replaceAll();

      // passed parms
      void autoLoad();
      void argLoad(QList<QString> argList);      

      // preset folders      
      QAction *prefolder_Actions[PRESET_FOLDERS_MAX];
      QStringList m_prefolder_List;

      void prefolder_CreateMenus();
      void prefolder_RedoList();
      //void prefolder_UpdateActions();

      // recent folders     
      QAction *rfolder_Actions[RECENT_FOLDERS_MAX];
      QStringList m_rfolder_List;

      void rfolder_CreateMenus();
      void rfolder_Add();
      void rfolder_UpdateActions();

      // recent files      
      QAction *rf_Actions[RECENT_FILES_MAX];
      QStringList m_rf_List;

      void rf_CreateMenus();
      void rf_Update();
      void rf_UpdateActions();

      // syntax
      QString m_appPath;
      QString m_jsonFname;
      SyntaxTypes m_syntaxEnum;
      Syntax *m_syntaxParser;
      void runSyntax(QString synFName);

      // settings
      struct Arugments m_args;
      struct Settings m_struct;
      struct PrintSettings m_printer;

      // tab stops
      QList<int> m_tabStops;
      void setUpTabStops();

      // open tabs      
      QAction *openTab_Actions[OPENTABS_MAX];

      void openTab_CreateMenus();      
      void openTab_Add();
      void openTab_Delete();
      void openTab_UpdateActions();

      void openTab_Select(int index);
      void openTab_UpdateOneAction(int index, bool isModified);

      // spell check     
      void createSpellCheck();
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

      enum Option { ABOUTURL, ADVFIND, AUTOLOAD, CLOSE, COLORS, COLUMN_MODE, DICT_MAIN, DICT_USER, FIND_LIST, FIND_REPLACE,
                    FONT, FORMAT_DATE, FORMAT_TIME, KEYS, MACRO, MACRO_NAMES, PATH_SYNTAX, PATH_PRIOR, PRESET_FOLDER,
                    PRINT_OPTIONS, RECENTFOLDER, RECENTFILE, SHOW_LINEHIGHLIGHT, SHOW_LINENUMBERS, SHOW_SPACES, SHOW_BREAKS,
                    SPELLCHECK, TAB_SPACING, USESPACES, WORDWRAP};

      enum Config { CFG_STARTUP, CFG_DEFAULT };

      enum SaveFiles { SAVE_ONE, SAVE_ALL };

      void openDoc(QString path);
      bool closeAll_Doc(bool isExit);      
      void save_ConfigFile();

      void setScreenColors();
      void setSyntax();
      void setSynType(SyntaxTypes data);
      void forceSyntax(SyntaxTypes data);

      // create shortcuts, menus, status bar
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
      bool saveFile(const QString &fileName, SaveFiles saveType);
      bool saveAs(SaveFiles saveType);

      void setCurrentTitle(const QString &fileName, bool tabChange = false);
      void setDiamondTitle(const QString title);

      QString get_curFileName(int whichTab);
      QString pathName(QString fileName) const;
      QString strippedName(const QString filename);
      QString suffixName() const;   

      CS_SLOT_1(Private, void focusChanged(QWidget *prior, QWidget *current))
      CS_SLOT_2(focusChanged) 

      CS_SLOT_1(Private, void newFile())
      CS_SLOT_2(newFile)      

      CS_SLOT_1(Private, void open_RelatedFile())
      CS_SLOT_2(open_RelatedFile) 

      CS_SLOT_1(Private, bool close_Doc())
      CS_SLOT_2(close_Doc) 

      CS_SLOT_1(Private, void reload())
      CS_SLOT_2(reload) 

      CS_SLOT_1(Private, bool save())
      CS_SLOT_2(save) 
     
      CS_SLOT_1(Private, void saveAll())
      CS_SLOT_2(saveAll) 

      CS_SLOT_1(Private, void print())
      CS_SLOT_2(print) 

      CS_SLOT_1(Private, void printOut(QPrinter *))
      CS_SLOT_2(printOut) 

      CS_SLOT_1(Private, void printPdf())
      CS_SLOT_2(printPdf) 

      CS_SLOT_1(Private, void printPreview())
      CS_SLOT_2(printPreview)       

      CS_SLOT_1(Private, void mw_undo())
      CS_SLOT_2(mw_undo) 

      CS_SLOT_1(Private, void mw_redo())
      CS_SLOT_2(mw_redo) 

      CS_SLOT_1(Private, void mw_cut())
      CS_SLOT_2(mw_cut) 

      CS_SLOT_1(Private, void mw_copy())
      CS_SLOT_2(mw_copy) 

      CS_SLOT_1(Private, void mw_paste())
      CS_SLOT_2(mw_paste) 

      CS_SLOT_1(Private, void selectAll())
      CS_SLOT_2(selectAll) 

      CS_SLOT_1(Private, void selectBlock())
      CS_SLOT_2(selectBlock) 

      CS_SLOT_1(Private, void selectLine())
      CS_SLOT_2(selectLine) 

      CS_SLOT_1(Private, void selectWord())
      CS_SLOT_2(selectWord) 

      CS_SLOT_1(Private, void caseUpper())
      CS_SLOT_2(caseUpper) 

      CS_SLOT_1(Private, void caseLower())
      CS_SLOT_2(caseLower) 

      CS_SLOT_1(Private, void caseCap())
      CS_SLOT_2(caseCap) 

      CS_SLOT_1(Private, void deleteLine())
      CS_SLOT_2(deleteLine) 

      CS_SLOT_1(Private, void deleteEOL())
      CS_SLOT_2(deleteEOL) 

      CS_SLOT_1(Private, void insertDate())
      CS_SLOT_2(insertDate) 

      CS_SLOT_1(Private, void insertTime())
      CS_SLOT_2(insertTime) 

      CS_SLOT_1(Private, void insertSymbol())
      CS_SLOT_2(insertSymbol)       

      CS_SLOT_1(Private, void columnMode())
      CS_SLOT_2(columnMode) 

      CS_SLOT_1(Private, void find())
      CS_SLOT_2(find) 

      CS_SLOT_1(Private, void replace())
      CS_SLOT_2(replace) 

      CS_SLOT_1(Private, void findNext())
      CS_SLOT_2(findNext) 

      CS_SLOT_1(Private, void findPrevious())
      CS_SLOT_2(findPrevious) 

      CS_SLOT_1(Private, void advFind())
      CS_SLOT_2(advFind) 

      CS_SLOT_1(Private, void goLine())
      CS_SLOT_2(goLine) 

      CS_SLOT_1(Private, void goColumn())
      CS_SLOT_2(goColumn) 

      CS_SLOT_1(Private, void goTop())
      CS_SLOT_2(goTop) 

      CS_SLOT_1(Private, void goBottom())
      CS_SLOT_2(goBottom) 

      CS_SLOT_1(Private, void lineHighlight())
      CS_SLOT_2(lineHighlight) 

      CS_SLOT_1(Private, void moveBar())
      CS_SLOT_2(moveBar) 

      CS_SLOT_1(Private, void lineNumbers())
      CS_SLOT_2(lineNumbers) 

      CS_SLOT_1(Private, void wordWrap())
      CS_SLOT_2(wordWrap) 

      CS_SLOT_1(Private, void show_Spaces())
      CS_SLOT_2(show_Spaces) 

      CS_SLOT_1(Private, void show_Breaks())
      CS_SLOT_2(show_Breaks) 

      CS_SLOT_1(Private, void displayHTML())
      CS_SLOT_2(displayHTML) 

      // document
      CS_SLOT_1(Private, void formatUnix())
      CS_SLOT_2(formatUnix) 

      CS_SLOT_1(Private, void formatWin())
      CS_SLOT_2(formatWin)     

      CS_SLOT_1(Private, void fixTab_Spaces())
      CS_SLOT_2(fixTab_Spaces) 

      CS_SLOT_1(Private, void fixSpaces_Tab())
      CS_SLOT_2(fixSpaces_Tab) 

      CS_SLOT_1(Private, void deleteEOL_Spaces())
      CS_SLOT_2(deleteEOL_Spaces) 

      // macro
      CS_SLOT_1(Private, void mw_macroStart())
      CS_SLOT_2(mw_macroStart) 

      CS_SLOT_1(Private, void mw_macroStop())
      CS_SLOT_2(mw_macroStop) 

      CS_SLOT_1(Private, void macroPlay())
      CS_SLOT_2(macroPlay) 

      CS_SLOT_1(Private, void macroLoad())
      CS_SLOT_2(macroLoad) 

      CS_SLOT_1(Private, void macroEditNames())
      CS_SLOT_2(macroEditNames) 

      CS_SLOT_1(Private, void spellCheck())
      CS_SLOT_2(spellCheck) 

      // options
      CS_SLOT_1(Private, void setColors())
      CS_SLOT_2(setColors) 

      CS_SLOT_1(Private, void setFont())
      CS_SLOT_2(setFont)

      CS_SLOT_1(Private, void setOptions())
      CS_SLOT_2(setOptions) 

      CS_SLOT_1(Private, void setPresetFolders())
      CS_SLOT_2(setPresetFolders)

      CS_SLOT_1(Private, void setPrintOptions())
      CS_SLOT_2(setPrintOptions) 

      CS_SLOT_1(Private, void move_ConfigFile())
      CS_SLOT_2(move_ConfigFile) 

      CS_SLOT_1(Private, void tabNew())
      CS_SLOT_2(tabNew) 

      CS_SLOT_1(Private, void tabClose(int index))
      CS_SLOT_2(tabClose) 

      // help
      CS_SLOT_1(Private, void diamondHelp())
      CS_SLOT_2(diamondHelp) 

      CS_SLOT_1(Private, void about())
      CS_SLOT_2(about) 

      //
      CS_SLOT_1(Private, void documentWasModified())
      CS_SLOT_2(documentWasModified)      

      CS_SLOT_1(Private, void setStatus_LineCol())
      CS_SLOT_2(setStatus_LineCol) 

      CS_SLOT_1(Private, void mw_tabClose())
      CS_SLOT_2(mw_tabClose) 

      CS_SLOT_1(Private, void tabChanged(int index))
      CS_SLOT_2(tabChanged) 

      // adv find
      CS_SLOT_1(Private, void advFind_View(const QModelIndex &index))
      CS_SLOT_2(advFind_View) 

      CS_SLOT_1(Private, void advFind_Close())
      CS_SLOT_2(advFind_Close) 

      // copy buffer
      CS_SLOT_1(Private, void showCopyBuffer())
      CS_SLOT_2(showCopyBuffer) 

      // spell check
      CS_SLOT_1(Private, void spell_addUserDict())
      CS_SLOT_2(spell_addUserDict) 

      CS_SLOT_1(Private, void spell_replaceWord())
      CS_SLOT_2(spell_replaceWord) 

      // recent folders
      CS_SLOT_1(Private, void showContext_RecentFolder(const QPoint &pt))
      CS_SLOT_2(showContext_RecentFolder) 

      CS_SLOT_1(Private, void rfolder_Open())
      CS_SLOT_2(rfolder_Open) 

      CS_SLOT_1(Private, void rfolder_ClearList())
      CS_SLOT_2(rfolder_ClearList) 

      CS_SLOT_1(Private, void rfolder_RemoveFName())
      CS_SLOT_2(rfolder_RemoveFName) 

      // preset folders      
      CS_SLOT_1(Private, void prefolder_Open())
      CS_SLOT_2(prefolder_Open) 

      // recent files
      CS_SLOT_1(Private, void showContext_Files(const QPoint &pt))
      CS_SLOT_2(showContext_Files) 

      CS_SLOT_1(Private, void rf_Open())
      CS_SLOT_2(rf_Open) 

      CS_SLOT_1(Private, void rf_ClearList())
      CS_SLOT_2(rf_ClearList) 

      CS_SLOT_1(Private, void rf_DeleteName())
      CS_SLOT_2(rf_DeleteName) 

      CS_SLOT_1(Private, void rf_RemoveFName())
      CS_SLOT_2(rf_RemoveFName) 

      // open (tab) files
      CS_SLOT_1(Private, void showContext_Tabs(const QPoint &pt))
      CS_SLOT_2(showContext_Tabs) 

      CS_SLOT_1(Private, void openTab_redo())
      CS_SLOT_2(openTab_redo) 

      // split
      CS_SLOT_1(Private, void set_splitCombo())
      CS_SLOT_2(set_splitCombo)

      CS_SLOT_1(Private, void split_Horizontal())
      CS_SLOT_2(split_Horizontal)

      CS_SLOT_1(Private, void split_Vertical())
      CS_SLOT_2(split_Vertical)

      CS_SLOT_1(Private, void split_NameChanged(int data))
      CS_SLOT_2(split_NameChanged)

      CS_SLOT_1(Private, void split_CloseButton())
      CS_SLOT_2(split_CloseButton)
};

#endif
