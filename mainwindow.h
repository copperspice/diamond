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

#include "highlighter.h"
#include "diamond_edit.h"
#include "ui_mainwindow.h"
#include "util.h"

#include <QAction>
#include <QDomDocument>
#include <QMenu>
#include <QMainWindow>
#include <QPlainTextEdit>

struct Settings {
   QColor colorBackground;
   QColor colorHighlight;
   QColor colorText;
   QFont font;
   QString dateFormat;
   int tabSpacing;
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

      DiamondTextEdit *m_textEdit;
      QString m_curFile;
      QString m_cfgFName;
      QDomDocument m_domCfg;
      Highlighter *m_highlighter;

      QLabel *m_statusLine;
      QLabel *m_statusMid;
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

      void setColors();
      void createShortCuts();
      void createToolBars();
      void createConnections();
      void createToggles();

      void setStatusBar(QString msg, int timeOut);
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
      void loadFile(const QString &fileName);
      bool saveFile(const QString &fileName);

      void setCurrentFile(const QString &fileName);
      QString pathName(const QString &fullFileName);
      QString strippedName(const QString &fullFileName);

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
      void printSetup();

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
      void next();
      void previous();
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

      void setBackgroundColor();
      void setFont();
      void setOptions();

      void tabNew();
      void tabClose();

      void about();
      void X();

      //
      void documentWasModified();
      void rf_Open();
};

#endif
