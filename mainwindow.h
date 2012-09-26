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
      QStringList get_Maybe(QString word);

   protected:
      void closeEvent(QCloseEvent *event);

   private:
      Ui::MainWindow *m_ui;

      // textEdit
      DiamondTextEdit *m_textEdit;
      QTabWidget *m_tabWidget;
      Syntax *m_syntaxParser;

      QString m_curFile;
      QString m_jsonFname;

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
      int m_line;
      int m_col;

      enum Option {CLOSE, COLORS, COLUMN_MODE, DICT_MAIN, DICT_USER, FONT, FORMAT_DATE, FORMAT_TIME,
                   PATH_SYNTAX, PATH_PRIOR, RECENTFILE, SHOW_LINEHIGHLIGHT, SHOW_LINENUMBERS,
                   SPELLCHECK, TAB_SPACING};

      enum SyntaxTypes {SYN_C, SYN_CLIPPER, SYN_CSS, SYN_DOX, SYN_HTML, SYN_JAVA, SYN_JS,
                        SYN_JSON, SYN_MAKE, SYN_TEXT, SYN_SHELL_S, SYN_PERL, SYN_PHP, SYN_PYTHON, SYN_NONE };

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

      // rencent files
      void rf_CreateMenus();
      void rf_Update();
      void rf_UpdateActions();

      // support
      int get_Value1(const QString route);
      bool querySave();
      bool loadFile(const QString &fileName, bool newTab);
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

      void insertDate();
      void insertTime();
      void insertSymbol();
      void indentIncr();
      void indentDecr();
      void columnMode();

      void find();
      void replace();
      void findNext();
      void findPrevious();
      void advFind();
      void goLine();
      void goColumn();
      void goTop();

      void lineHighlight();
      void move_lineHighlight();

      void lineNumbers();
      void showSpaces();
      void showSpaces_EOL();
      void showTabs();
      void showLineBreaks();

      void setSyn_C();
      void setSyn_Clipper();
      void setSyn_Css();
      void setSyn_Dox();
      void setSyn_Html();
      void setSyn_Java();
      void setSyn_Javascript();
      void setSyn_Json();
      void setSyn_Makefile();
      void setSyn_Text();
      void setSyn_Shell_S();
      void setSyn_Perl();
      void setSyn_Php();
      void setSyn_Python();

      void formatDos();
      void formatUnix();
      void formatMac();
      void fixTab_Spaces();
      void fixSpaces_Tabs();
      void fixSpaces_EOL();

      void mw_macroStart();
      void mw_macroStop();
      void macroPlay();
      void spellCheck();

      void setColors();
      void setFont();
      void setOptions();

      void tabNew();
      void tabClose();
      void about();

      //
      void documentWasModified();     
      void printPreview(QPrinter *printer);
      void rf_Open();
      void setStatus_LineCol();     
      void tabChanged(int index);

      // spell check
      void add_UserDict();
      void replaceWord();
};

#endif
