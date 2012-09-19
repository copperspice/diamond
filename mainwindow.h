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
#include "syntax.h"
#include "ui_mainwindow.h"
#include "util.h"

#include <QAction>
#include <QDomDocument>
#include <QMenu>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QTabWidget>

struct Settings {   
   QFont   font;
   QColor  colorBackground;
   QColor  colorHighBackground;
   QColor  colorHighText;
   QColor  colorText;
   QString syntaxPath;
   QString dateFormat;   
   int  tabSpacing;
   bool showlineHighlight;
   bool showlineNum;
   bool isLineMode;
};

class MainWindow : public QMainWindow
{
   Q_OBJECT

   public:      
      MainWindow();
      struct Settings get_StructData();     

   protected:
      void closeEvent(QCloseEvent *event);

   private:
      Ui::MainWindow *m_ui;
      struct Settings m_struct;
      QString m_priorPath;

      QString m_findText;
      QTextDocument::FindFlags m_flags;
      bool m_fDirection;
      bool m_fCase;
      bool m_fWholeWords;

      QTabWidget *m_tabWidget;

      DiamondTextEdit *m_textEdit;
      QString m_curFile;
      QString m_cfgFName;
      QDomDocument m_domCfg;
      Syntax *m_highlighter;

      QLabel *m_statusLine;
      QLabel *m_statusMode;
      QLabel *m_statusName;
      int m_line;
      int m_col;

      static const int rf_MaxCnt = 10;
      QAction *rf_Actions[rf_MaxCnt];
      QStringList rf_List;

      QToolBar *fileToolBar;
      QToolBar *editToolBar;
      QToolBar *searchToolBar;
      QToolBar *toolsToolBar;

      enum Option {CLOSE, FONT, RECENTFILE, DATEFORMAT, TAB_SPACING};      
      enum SyntaxTypes {SYN_C, SYN_CLIPPER, SYN_CSS, SYN_DOX, SYN_HTML, SYN_JAVA,
                        SYN_JS, SYN_MAKE, SYN_TEXT, SYN_SHELL_S, SYN_PERL_S, SYN_NONE };

      void setScreenColors();
      void setSyntax();
      void setSynType(SyntaxTypes data);
      void forceSyntax(SyntaxTypes data);

      void createShortCuts();
      void createToolBars();
      void createStatusBar();
      void createConnections();
      void createToggles();

      void setStatusBar(QString msg, int timeOut);
      void setStatusFName(QString name);
      void showNotDone(QString item);

      //
      bool readCfg();
      void writeCfg(Option data);
      void cfg_FileName();
      bool cfg_ParseXml();
      bool cfg_NewXml();
      bool cfg_SaveXml();      

      QString cfg_GetText(QDomElement root, QString name);
      void cfg_Update(QDomElement root, QString name, QString newData);

      QStringList cfg_GetList(QDomElement root, QString name);
      void cfg_UpdateList(QDomElement root, QString name, QStringList newData);

      //
      void rf_CreateMenus();
      void rf_Update();
      void rf_UpdateActions();

      int get_Value1(const QString route);
      bool querySave();
      bool loadFile(const QString &fileName, bool newTab);
      bool saveFile(const QString &fileName);

      void setCurrentFile(const QString &fileName);
      QString pathName() const;
      QString strippedName(const QString filename);
      QString suffixName() const;

   private slots:
      void newFile();
      void open();
      void close_Doc();
      void closeAll_Doc();
      void reload();
      bool save();
      bool saveAs();
      bool saveAll();
      void print();
      void printPreview();
      void printPdf();

      void selectAll();
      void selectBlock();
      void selectLine();
      void selectWord();
      void caseUpper();
      void caseLower();
      void caseCap();

      void insertDate();
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
      void setSyn_Makefile();
      void setSyn_Text();
      void setSyn_Shell_S();
      void setSyn_Perl_S();

      void formatDos();
      void formatUnix();
      void formatMac();
      void fixTab_Spaces();
      void fixSpaces_Tabs();
      void fixSpaces_EOL();

      void macroStart();
      void macroStop();
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
      void rf_Open();
      void printPreview(QPrinter *printer);
      void setLineCol();
      void setColMode();

      void tabChanged(int index);
};

#endif
