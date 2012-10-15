/**************************************************************************
*
* Copyright (c) 2012 Barbara Geller
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
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QMenu>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QTabWidget>

class MainWindow : public QMainWindow
{
   Q_OBJECT

   public:      
      MainWindow();      
      struct Settings get_StructData();           

      QString get_DirPath(QString message, QString path);
      bool loadFile(const QString &fileName, bool newTab);

      // spell
      QStringList spell_getMaybe(QString word);

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

      // syntax
      QString m_jsonFname;
      SyntaxTypes m_syntaxEnum;
      Syntax *m_syntaxParser;
      void runSyntax(QString synFName);

      // find
      QString m_findText;
      QTextDocument::FindFlags m_flags;      
      bool m_fDirection;
      bool m_fCase;
      bool m_fWholeWords;

      // macros
      bool m_record;
      QList<QKeyEvent *> m_keyList;

      // settings
      struct Settings m_struct;

      // recent files
      static const int rf_MaxCnt = 10;
      QAction *rf_Actions[rf_MaxCnt];
      QStringList m_rf_List;

      // spell check     
      void createSpellCheck();
      SpellCheck *m_spellCheck;          

      // menu bar
      QToolBar *fileToolBar;
      QToolBar *editToolBar;
      QToolBar *searchToolBar;
      QToolBar *toolsToolBar;

      // status bar
      QLabel *m_statusLine;
      QLabel *m_statusMode;
      QLabel *m_statusName;    

      enum Option { ABOUTURL, CLOSE, COLORS, COLUMN_MODE, DICT_MAIN, DICT_USER, FONT, FORMAT_DATE, FORMAT_TIME,
                    KEYS, MACRO, PATH_SYNTAX, PATH_PRIOR, RECENTFILE,
                    SHOW_LINEHIGHLIGHT, SHOW_LINENUMBERS, SPELLCHECK, TAB_SPACING, USESPACES, WORDWRAP};

      void setScreenColors();
      void setSyntax();
      void setSynType(SyntaxTypes data);
      void forceSyntax(SyntaxTypes data);

      // create shortcuts, menus, status bar
      void createShortCuts();
      void createToolBars();
      void createStatusBar();
      void createConnections();
      void createToggles();

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

      QString get_SyntaxPath();
      QColor  json_SetColor(QString values);
      QString json_GetRGB(QColor color);     
      QJsonObject json_SaveSyntax(QJsonObject object);

      // recent files
      void rf_CreateMenus();
      void rf_Update();
      void rf_UpdateActions();

      // support     
      int get_Value1(const QString route);
      bool querySave();     
      bool saveFile(const QString &fileName, bool isSaveOne);
      bool saveAs(bool isSaveOne);

      void setCurrentFile(const QString &fileName);
      QString pathName(QString fileName) const;
      QString strippedName(const QString filename);
      QString suffixName() const;

   private slots:
      void newFile();
      void open();
      void close_Doc();
      bool closeAll_Doc();
      void reload();
      bool save();
      bool saveAs();
      bool saveAll();
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
      void move_lineHighlight();
      void lineNumbers();
      void wordWrap();
      void showSpaces();      
      void showTabs();
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

      void formatDos();
      void formatUnix();
      void formatMac();
      void fixTab_Spaces();
      void fixSpaces_Tabs();
      void fixSpaces_EOL();

      void mw_macroStart();
      void mw_macroStop();
      void macroPlay();
      void macroLoad();
      void spellCheck();

      void setColors();
      void setFont();
      void setOptions();

      void tabNew();
      void tabClose(int index);
      void diamondHelp();
      void about();

      //
      void documentWasModified();     
      void printPreview(QPrinter *printer);      
      void setStatus_LineCol();           

      void mw_tabClose();
      void tabChanged(int index);

      void rf_Open();
      void rf_DeleteName();

      // spell check
      void spell_addUserDict();
      void spell_replaceWord();

      // file menu, recent file list
      void showContextMenu(const QPoint &pt);
      void rf_ClearList();
      void rf_RemoveFName();

   public slots:
      // indent
      void indentIncr(QString route);
      void indentDecr(QString route);
};

#endif
