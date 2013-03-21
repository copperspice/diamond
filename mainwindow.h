/**************************************************************************
*
* Copyright (c) 2012-2013 Barbara Geller
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
#include <QFile>
#include <QFrame>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QMenu>
#include <QMainWindow>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QStackedWidget>
#include <QTabWidget>

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
   Q_OBJECT

   public:      
      MainWindow();      
      struct Settings get_StructData();           

      // support
      QString get_DirPath(QString message, QString path);
      bool loadFile(const QString &fileName, bool newTab, bool isAuto);

      // macros
      void json_Save_MacroNames(QStringList m_macroNames);
      QList<macroStruct> json_View_Macro(QString macroName);

      // spell
      QStringList spell_getMaybe(QString word);

      void setStatus_FName2(QString name);

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

      // split
      DiamondTextEdit *split_textEdit;
      QSplitter *m_splitter;
      QLabel *m_splitTitle;
      QFrame *m_sideWidget;
      QFrame *m_bottomWidget;

      bool m_isSplit;
      bool m_fromSplit;

      //
      void autoLoad();
      QStringList m_openedFiles;

      // syntax
      QString m_jsonFname;
      SyntaxTypes m_syntaxEnum;
      Syntax *m_syntaxParser;
      void runSyntax(QString synFName);

      // find
      QString m_findText;
      QStringList m_findList;
      QTextDocument::FindFlags m_flags;
      bool m_fDirection;
      bool m_fCase;
      bool m_fWholeWords;

      // advanced find
      QString m_advFindText;
      QString m_advFindFileType;
      QString m_advFindFolder;
      QTextDocument::FindFlags m_advFlags;
      bool m_advFCase;
      bool m_advFWholeWords;
      bool m_advFSearchSubFolders;

      QStringList m_recursiveList;
      QFrame *m_findWidget;
      QStandardItemModel *m_model;
      QList<advFindStruct> advFind_getResults();
      void findRecursive(const QString &path);
      void advFind_ShowFiles(QList<advFindStruct> foundList);

      // macros    
      bool m_record;
      QList<QKeyEvent *> m_keyList;
      QStringList m_macroNames;     

      // settings
      struct Settings m_struct;
      struct PrintSettings m_printer;

      // recent folders
      static const int rfolder_MaxCnt = 10;
      QAction *rfolder_Actions[rfolder_MaxCnt];
      QStringList m_rfolder_List;

      void rfolder_CreateMenus();
      void rfolder_Add();
      void rfolder_UpdateActions();
      void open(QString path);

      // recent files
      static const int rf_MaxCnt = 10;
      QAction *rf_Actions[rf_MaxCnt];
      QStringList m_rf_List;

      void rf_CreateMenus();
      void rf_Update();
      void rf_UpdateActions();

      // currently open tabs
      static const int openTab_MaxCnt = 20;
      QAction *openTab_Actions[openTab_MaxCnt];

      void openTab_CreateMenus();
      void openTab_Add();
      void openTab_Delete();
      void openTab_UpdateActions();

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

      enum Option { ABOUTURL, ADVFIND, AUTOLOAD, CLOSE, COLORS, COLUMN_MODE, DICT_MAIN, DICT_USER, FIND_LIST, FONT,
                    FORMAT_DATE, FORMAT_TIME, KEYS, MACRO, MACRO_NAMES, PATH_SYNTAX, PATH_PRIOR, PRINT_OPTIONS,
                    RECENTFOLDER, RECENTFILE, SHOW_LINEHIGHLIGHT, SHOW_LINENUMBERS, SHOW_SPACES, SHOW_EOL,
                    SPELLCHECK, TAB_SPACING, USESPACES, WORDWRAP};

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
      bool json_Read();
      bool json_Write(Option data);
      void json_getFileName();
      bool json_CreateNew();
      bool json_SaveFile(QByteArray route);
      QByteArray json_ReadFile();

      QStringList json_Load_MacroIds();
      bool json_Load_Macro(QString macroName);

      QString get_SyntaxPath();
      QString get_xxFile(QString title, QString fname, QString filter);

      QFont json_SetFont(QString value);
      QColor json_SetColor(QString values);
      QString json_GetRGB(QColor color);     
      QJsonObject json_SaveSyntax(QJsonObject object);

      // printing
      int m_pageNo;
      int m_pageCount;
      QRect m_printArea;

      int get_HeaderSize(QPainter *painter);
      int get_FooterSize(QPainter *painter);

      void printOut(QPrinter *printer);
      void doHeader(QPainter *painter);
      void doFooter(QPainter *painter);
      QString macroExpand(QString macro);

      // support           
      int get_Value1(const QString route);
      bool querySave();     
      bool saveFile(const QString &fileName, bool isSaveOne);
      bool saveAs(bool isSaveOne);      

      void setCurrentTitle(const QString &fileName, bool tabChange = false);
      void setDiamondTitle(const QString title);

      QString get_curFileName(int whichTab);
      QString pathName(QString fileName) const;
      QString strippedName(const QString filename);
      QString suffixName() const;   

   private slots:
      void newFile();
      void mw_open();
      void close_Doc();
      bool closeAll_Doc();
      void reload();
      bool save();
      bool saveAs();
      void saveAll();
      void print();
      void printPreview();
      void printPdf();

      void mw_undo();
      void mw_redo();
      void mw_cut();
      void mw_copy();
      void mw_paste();

      void selectAll();
      void selectBlock();
      void selectLine();
      void selectWord();
      void caseUpper();
      void caseLower();
      void caseCap();

      void mw_indentIncr();
      void mw_indentDecr();
      void deleteLine();
      void deleteEOL();

      void insertDate();
      void insertTime();
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
      void showSpaces();           
      void showEOL();
      void displayHTML();

      void setSyn_C();
      void setSyn_Clipper();
      void setSyn_Css();
      void setSyn_Dox();
      void setSyn_Html();
      void setSyn_Java();
      void setSyn_Javascript();
      void setSyn_Json();
      void setSyn_Makefile();
      void setSyn_Nsis();
      void setSyn_Text();
      void setSyn_Shell_S();
      void setSyn_Perl();
      void setSyn_Php();
      void setSyn_Python();
      void setSyn_None();

      void formatUnix();
      void formatWin();
      void formatMac();
      void fixTab_Spaces();
      void fixSpaces_Tab();
      void deleteEOL_Spaces();

      void mw_macroStart();
      void mw_macroStop();
      void macroPlay();
      void macroLoad();
      void macroEditNames();
      void spellCheck();

      void setColors();
      void setFont();
      void move_ConfigFile();
      void setOptions();
      void setPrintOptions();

      void tabNew();
      void tabClose(int index);
      void split_Horizontal();
      void split_Vertical();

      void diamondHelp();
      void about();

      //
      void documentWasModified();     
      void printPreview(QPrinter *printer);      
      void setStatus_LineCol();

      void mw_tabClose();
      void tabChanged(int index);

      // adv find
      void advFind_View(const QModelIndex &index);
      void advFind_Close();

      // spell check
      void spell_addUserDict();
      void spell_replaceWord();

      // recent folders
      void showContextMenuFolder(const QPoint &pt);
      void rfolder_Open();
      void rfolder_ClearList();
      void rfolder_RemoveFName();

      // recent files
      void showContextMenuFile(const QPoint &pt);
      void rf_Open();
      void rf_ClearList();
      void rf_DeleteName();
      void rf_RemoveFName();

      // open (tab) files
      void openTab_Select();

      // split
      void split_Title();
      void split_MoveBar();
      void sideClose();
      void bottomClose();

   public slots:
      // indent
      void indentIncr(QString route);
      void indentDecr(QString route);
};

#endif
