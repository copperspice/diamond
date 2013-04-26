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

#include "dialog_config.h"
#include "dialog_macro.h"
#include "mainwindow.h"
#include "util.h"

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFlags>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QKeySequence>
#include <QPoint>
#include <QPushButton>
#include <QSettings>
#include <Qt>

#include <qglobal.h>

bool MainWindow::json_Read()
{
   bool ok = true;

   m_appPath = QCoreApplication::applicationDirPath();

   QSettings settings("Diamond Editor", "Settings");
   m_jsonFname = settings.value("configName").toString();

   if (m_jsonFname.isEmpty() || ! QFile::exists(m_jsonFname)) {
      // get a new file name
      json_getFileName();

      if (m_jsonFname.isEmpty()) {
         return false;
      }
      settings.setValue("configName", m_jsonFname);

      if (! QFile::exists(m_jsonFname))  {
         json_CreateNew();
      }
   }

   if (ok) {

      // get existing json data
      QByteArray data = json_ReadFile();

      QJsonDocument doc = QJsonDocument::fromJson(data);

      QJsonObject object = doc.object();
      QJsonValue value;            
      QJsonArray list;

      int cnt;

      //
      value = object.value("pos-x");
      int x = value.toDouble();

      value = object.value("pos-y");
      int y = value.toDouble();

      QPoint pos = QPoint(x, y);
      move(pos);      

      //
      value = object.value("size-width");
      int width = value.toDouble();

      value = object.value("size-height");
      int height = value.toDouble();

      QSize size = QSize(width, height);
      resize(size);

      //
      m_struct.useSpaces = object.value("useSpaces").toBool();

      value = object.value("tabSpacing");
      m_struct.tabSpacing = value.toDouble();

      m_struct.showLineHighlight = object.value("showLineHighlight").toBool();
      m_struct.showLineNumbers   = object.value("showLineNumbers").toBool();
      m_struct.isWordWrap        = object.value("word-wrap").toBool();      
      m_struct.show_Spaces       = object.value("showSpaces").toBool();
      m_struct.show_Breaks       = object.value("showBreaks").toBool();
      m_struct.isColumnMode      = object.value("column-mode").toBool();      
      m_struct.isSpellCheck      = object.value("spellcheck").toBool();
      m_struct.autoLoad          = object.value("autoLoad").toBool();

      //
      m_struct.formatDate      = object.value("formatDate").toString();
      m_struct.formatTime      = object.value("formatTime").toString();
      m_struct.pathPrior       = object.value("pathPrior").toString();
      m_struct.pathSyntax      = object.value("pathSyntax").toString();

      m_struct.dictMain        = object.value("dictMain").toString();
      m_struct.dictUser        = object.value("dictUser").toString();
      m_struct.aboutUrl        = object.value("aboutUrl").toString();

      // printer options      
      m_printer.printHeader    = object.value("prt-pritnHeader").toBool();
      m_printer.printFooter    = object.value("prt-printFooter").toBool();
      m_printer.header_left    = object.value("prt-headerLeft").toString();
      m_printer.header_center  = object.value("prt-headerCenter").toString();
      m_printer.header_right   = object.value("prt-headerRight").toString();
      m_printer.header_line2   = object.value("prt-headerLine2").toString();
      m_printer.footer_left    = object.value("prt-footerLeft").toString();
      m_printer.footer_center  = object.value("prt-footerCenter").toString();
      m_printer.footer_right   = object.value("prt-footerRight").toString();
      m_printer.footer_line2   = object.value("prt-footerLine2").toString();

      m_printer.marTop         = object.value("prt-marginTop").toDouble();
      m_printer.marBottom      = object.value("prt-marginBottom").toDouble();
      m_printer.marLeft        = object.value("prt-marginLeft").toDouble();
      m_printer.marRight       = object.value("prt-marginRight").toDouble();
      m_printer.hdrGap         = object.value("prt-hdrGap").toDouble();

      m_printer.fontHeader     = json_SetFont(object.value("prt-fontHeader").toString());
      m_printer.fontFooter     = json_SetFont(object.value("prt-fontFooter").toString());
      m_printer.fontText       = json_SetFont(object.value("prt-fontText").toString());

      // standard keys
      m_struct.key_open        = object.value("key-open").toString();
      m_struct.key_close       = object.value("key-close").toString();
      m_struct.key_save        = object.value("key-save").toString();
      m_struct.key_saveAs      = object.value("key-saveAs").toString();
      m_struct.key_print       = object.value("key-print").toString();
//    m_struct.key_exit        = object.value("key-exit").toString();

      m_struct.key_undo        = object.value("key-undo").toString();
      m_struct.key_redo        = object.value("key-redo").toString();
      m_struct.key_cut         = object.value("key-cut").toString();
      m_struct.key_copy        = object.value("key-copy").toString();
      m_struct.key_paste       = object.value("key-paste").toString();
      m_struct.key_selectAll   = object.value("key-selectAll").toString();
      m_struct.key_find        = object.value("key-find").toString();
      m_struct.key_replace     = object.value("key-replace").toString();
      m_struct.key_findNext    = object.value("key-findNext").toString();
      m_struct.key_findPrev    = object.value("key-findPrev").toString();
      m_struct.key_goTop       = object.value("key-goTop").toString();
      m_struct.key_goBottom    = object.value("key-goBottom").toString();
      m_struct.key_newTab      = object.value("key-newTab").toString();
//    m_struct.key_help        = object.value("key-help").toString();

      // user keys
      m_struct.key_selectLine  = object.value("key-selectLine").toString();
      m_struct.key_selectWord  = object.value("key-selectWord").toString();
      m_struct.key_selectBlock = object.value("key-selectBlock").toString();
      m_struct.key_upper       = object.value("key-upper").toString();
      m_struct.key_lower       = object.value("key-lower").toString();      
      m_struct.key_indentIncr  = object.value("key-indentIncr").toString();
      m_struct.key_indentDecr  = object.value("key-indentDecr").toString();
      m_struct.key_deleteLine  = object.value("key-deleteLine").toString();
      m_struct.key_deleteEOL   = object.value("key-deleteEOL").toString();
      m_struct.key_columnMode  = object.value("key-columnMode").toString();
      m_struct.key_goLine      = object.value("key-goLine").toString();      
      m_struct.key_show_Spaces = object.value("key-showShow").toString();
      m_struct.key_show_Breaks = object.value("key-showBreaks").toString();
      m_struct.key_macroPlay   = object.value("key-macroPlay").toString();
      m_struct.key_spellCheck  = object.value("key-spellCheck").toString();

      // screen fonts
      m_struct.fontNormal     = json_SetFont(object.value("font-normal").toString());
      m_struct.fontColumn     = json_SetFont(object.value("font-column").toString());

      // colors
      m_struct.colorText      = json_SetColor(object.value("color-text").toString()    );
      m_struct.colorBack      = json_SetColor(object.value("color-back").toString()    );
      m_struct.colorHighText  = json_SetColor(object.value("color-highText").toString());
      m_struct.colorHighBack  = json_SetColor(object.value("color-highBack").toString());

      // test added to resolve color conflicts
      if (m_struct.colorBack == m_struct.colorText) {
         csError("Diamond Configuration", "Background and Text colors are the same.\n\n"
                 "To change the colors select 'Settings' from the main Menu, then select 'Colors'.\n");
      }

      list = object.value("syntax-key").toArray();
      m_struct.syn_KeyWeight  = list.at(0).toDouble();
      m_struct.syn_KeyItalic  = list.at(1).toBool();
      m_struct.syn_KeyText    = json_SetColor(list.at(2).toString());       

      list = object.value("syntax-type").toArray();
      m_struct.syn_TypeWeight = list.at(0).toDouble();
      m_struct.syn_TypeItalic = list.at(1).toBool();
      m_struct.syn_TypeText   = json_SetColor(list.at(2).toString());

      list = object.value("syntax-class").toArray();
      m_struct.syn_ClassWeight = list.at(0).toDouble();
      m_struct.syn_ClassItalic = list.at(1).toBool();
      m_struct.syn_ClassText   = json_SetColor(list.at(2).toString());

      list = object.value("syntax-func").toArray();
      m_struct.syn_FuncWeight = list.at(0).toDouble();
      m_struct.syn_FuncItalic = list.at(1).toBool();
      m_struct.syn_FuncText   = json_SetColor(list.at(2).toString());

      list = object.value("syntax-quote").toArray();
      m_struct.syn_QuoteWeight = list.at(0).toDouble();
      m_struct.syn_QuoteItalic = list.at(1).toBool();
      m_struct.syn_QuoteText   = json_SetColor(list.at(2).toString());

      list = object.value("syntax-comment").toArray();
      m_struct.syn_CommentWeight = list.at(0).toDouble();
      m_struct.syn_CommentItalic = list.at(1).toBool();
      m_struct.syn_CommentText   = json_SetColor(list.at(2).toString());

      list = object.value("syntax-mline").toArray();
      m_struct.syn_MLineWeight = list.at(0).toDouble();
      m_struct.syn_MLineItalic = list.at(1).toBool();
      m_struct.syn_MLineText   = json_SetColor(list.at(2).toString());

      // adv find
      m_advFindText      = object.value("advFile-text").toString();
      m_advFindFileType  = object.value("advFile-filetype").toString();
      m_advFindFolder    = object.value("advFile-folder").toString();

      // find list
      list = object.value("find-list").toArray();
      cnt  = list.count();

      for (int k = 0; k < cnt; k++)  {
         m_findList.append(list.at(k).toString());
      }

      // find list
      list = object.value("replace-list").toArray();
      cnt  = list.count();

      for (int k = 0; k < cnt; k++)  {
         m_replaceList.append(list.at(k).toString());
      }

      // macro names
      list = object.value("macro-names").toArray();
      cnt = list.count();

      for (int k = 0; k < cnt; k++)  {
         m_macroNames.append(list.at(k).toString());
      }

      // ensure there is a name for each macro id
      QStringList macroIds = json_Load_MacroIds();

      int maxCount  = macroIds.size();
      bool modified = false;

      for (int id = 0; id < maxCount; ++id) {

         if (m_macroNames.size() <= id || m_macroNames.at(id).isEmpty()) {
            m_macroNames.append("Macro Name " + QString::number(id+1) );
            modified  = true;
         }
      }

      if (modified) {
         json_Write(MACRO_NAMES);
      }

      // recent folders
      list = object.value("recent-folders").toArray();
      cnt = list.count();

      for (int k = 0; k < cnt; k++)  {
         m_rfolder_List.append(list.at(k).toString());
      }

      // recent files
      list = object.value("recent-files").toArray();
      cnt = list.count();

      for (int k = 0; k < cnt; k++)  {
         m_rf_List.append(list.at(k).toString());
      }

      // opened files
      list = object.value("opened-files").toArray();
      cnt  = list.count();

      for (int k = 0; k < cnt; k++)  {
         QString fname = list.at(k).toString();

         if (fname.isEmpty()) {
           // maybe save the updated list
         } else {
            m_openedFiles.append(fname);
         }
      }
   }

   return ok;
}

bool MainWindow::json_Write(Option route)
{   
   if (m_args.flag_noSaveConfig) {
      return true;
   }

   QSettings settings("Diamond Editor", "Settings");
   m_jsonFname = settings.value("configName").toString();

   if (m_jsonFname.isEmpty()) {
      // get a new file name
      json_getFileName();

      if (m_jsonFname.isEmpty()) {
         return false;
      }
      settings.setValue("configName", m_jsonFname);
   }

   if (true) {
      // get existing json data
      QByteArray data = json_ReadFile();

      QJsonDocument doc  = QJsonDocument::fromJson(data);
      QJsonObject object = doc.object();

      switch (route)  {        

         case ABOUTURL:
            object.insert("aboutUrl", m_struct.aboutUrl);
            break;

         case ADVFIND:
            object.insert("advFile-text",       m_advFindText);
            object.insert("advFile-filetype",   m_advFindFileType);
            object.insert("advFile-folder",     m_advFindFolder);
            break;

         case AUTOLOAD:
            object.insert("autoLoad", m_struct.autoLoad);
            break;

         case CLOSE:

            object.insert("pos-x",       pos().x()  );
            object.insert("pos-y",       pos().y()  );
            object.insert("size-width",  size().width()  );
            object.insert("size-height", size().height() );

            {
              // opened files
              QJsonArray temp = QJsonArray::fromStringList(m_openedFiles);
              object.insert("opened-files", temp);
            }

            break;

         case COLORS:
            object.insert("color-text",     json_GetRGB(m_struct.colorText) );
            object.insert("color-back",     json_GetRGB(m_struct.colorBack));
            object.insert("color-highText", json_GetRGB(m_struct.colorHighText));
            object.insert("color-highBack", json_GetRGB(m_struct.colorHighBack));

            object = json_SaveSyntax(object);

            break;

         case COLUMN_MODE:
            object.insert("column-mode", m_struct.isColumnMode);
            break;

         case DICT_MAIN:
            object.insert("dictMain", m_struct.dictMain);
            break;

         case DICT_USER:
            object.insert("dictUser", m_struct.dictUser);
            break;

         case FIND_REPLACE:
            {
               QJsonArray temp = QJsonArray::fromStringList(m_findList);
               object.insert("find-list", temp);

               temp = QJsonArray::fromStringList(m_replaceList);
               object.insert("replace-list", temp);
               break;
            }                       

         case FONT:           
            {
               QString temp = m_struct.fontNormal.toString();
               object.insert("font-normal", temp);

               temp = m_struct.fontColumn.toString();
               object.insert("font-column", temp);
               break;
            }

         case FORMAT_DATE:
            object.insert("formatDate", m_struct.formatDate);
            break;

         case FORMAT_TIME:
            object.insert("formatTime", m_struct.formatTime);
            break;                        

         case KEYS:
            // standard keys
            object.insert("key-open",        m_struct.key_open);
            object.insert("key-close",       m_struct.key_close);
            object.insert("key-save",        m_struct.key_save);
            object.insert("key-saveAs",      m_struct.key_saveAs);
            object.insert("key-print",       m_struct.key_print);
//          object.insert("key-exit",        m_struct.key_exit);

            object.insert("key-undo",        m_struct.key_undo);
            object.insert("key-redo",        m_struct.key_redo);
            object.insert("key-cut",         m_struct.key_cut);
            object.insert("key-copy",        m_struct.key_copy);
            object.insert("key-paste",       m_struct.key_paste);
            object.insert("key-selectAll",   m_struct.key_selectAll);
            object.insert("key-find",        m_struct.key_find);
            object.insert("key-replace",     m_struct.key_replace);
            object.insert("key-findNext",    m_struct.key_findNext);
            object.insert("key-findPrev",    m_struct.key_findPrev);
            object.insert("key-goTop",       m_struct.key_goTop);
            object.insert("key-goBottom",    m_struct.key_goBottom);
            object.insert("key-newTab",      m_struct.key_newTab);
//          object.insert("key-help",        m_struct.key_help);

            // user keys
            object.insert("key-selectLine",  m_struct.key_selectLine);
            object.insert("key-selectWord",  m_struct.key_selectWord);
            object.insert("key-selectBlock", m_struct.key_selectBlock);
            object.insert("key-upper",       m_struct.key_upper);
            object.insert("key-lower",       m_struct.key_lower);
            object.insert("key-indentIncr",  m_struct.key_indentIncr);
            object.insert("key-indentDecr",  m_struct.key_indentDecr);            
            object.insert("key-deleteLine",  m_struct.key_deleteLine);
            object.insert("key-deleteEOL",   m_struct.key_deleteEOL);
            object.insert("key-columnMode",  m_struct.key_columnMode);
            object.insert("key-goLine",      m_struct.key_goLine);
            object.insert("key-showSpaces",  m_struct.key_show_Spaces);
            object.insert("key-showBreaks",  m_struct.key_show_Breaks);
            object.insert("key-macroPlay",   m_struct.key_macroPlay);
            object.insert("key-spellCheck",  m_struct.key_spellCheck);
            break;

         case PATH_PRIOR:
            object.insert("pathPrior", m_struct.pathPrior);
            break;

         case PATH_SYNTAX:
            object.insert("pathSyntax", m_struct.pathSyntax);
            break;

         case PRINT_OPTIONS:
            object.insert("prt-pritnHeader",  m_printer.printHeader);
            object.insert("prt-printFooter",  m_printer.printFooter);
            object.insert("prt-headerLeft",   m_printer.header_left);
            object.insert("prt-headerCenter", m_printer.header_center);
            object.insert("prt-headerRight",  m_printer.header_right);
            object.insert("prt-headerLine2",  m_printer.header_line2);
            object.insert("prt-footerLeft",   m_printer.footer_left);
            object.insert("prt-footerCenter", m_printer.footer_center);
            object.insert("prt-footerRight",  m_printer.footer_right);
            object.insert("prt-footerLine2",  m_printer.footer_line2);

            object.insert("prt-marginTop",    m_printer.marTop);
            object.insert("prt-marginBottom", m_printer.marBottom);
            object.insert("prt-marginLeft",   m_printer.marLeft);
            object.insert("prt-marginRight",  m_printer.marRight);
            object.insert("prt-hdrGap",       m_printer.hdrGap);

            {
               QString temp;

               temp = m_printer.fontHeader.toString();
               object.insert("prt-fontHeader", temp);

               temp = m_printer.fontFooter.toString();
               object.insert("prt-fontFooter", temp);

               temp = m_printer.fontText.toString();
               object.insert("prt-fontText",   temp);
            }

            break;

         case MACRO:

            {
               QList<QKeyEvent *> eventList;
               eventList = m_textEdit->get_MacroKeyList();

               int count = eventList.count();

               if (count > 0)  {
                  QVariantList macroList;

                  QKeyEvent *event;

                  for (int k = 0; k < count; ++k) {
                     event = eventList.at(k);

                     // hard coded order
                     QStringList eventList;
                     eventList.append( QString::number(event->key()) );
                     eventList.append( QString::number(event->modifiers()));
                     eventList.append( event->text() );

                     //
                     macroList.append( eventList );
                  }

                  bool ok = true;

                  // get next macro name
                  QString macroName = object.value("macro-next").toString();

                  // increment next macro name
                  int id = macroName.right(1).toInt() + 1;

                  if (id > Dialog_Macro::MACRO_MAX_COUNT )  {

                     QStringList macroIds = json_Load_MacroIds();

                     // select macro id to overwrite
                     Dialog_Macro *dw = new Dialog_Macro(this, Dialog_Macro::MACRO_SELECT, macroIds, m_macroNames);
                     int result = dw->exec();

                     if (result == QDialog::Accepted) {
                        // over write
                        QString text = dw->get_Macro();                                             
                        macroName = text;

                     } else {
                        // do not save
                        ok = false;
                     }

                     delete dw;

                  } else   {
                     // save next macro name
                     macroName = "macro" + QString::number(id);
                     object.insert("macro-next", macroName);

                  }

                  if (ok)  {
                     // save macro
                     QJsonArray temp = QJsonArray::fromVariantList(macroList);
                     object.insert(macroName, temp);                    
                  }
               }

               break;
            }

         case MACRO_NAMES:
            {
               QJsonArray temp = QJsonArray::fromStringList(m_macroNames);
               object.insert("macro-names", temp);
               break;
            }

         case RECENTFOLDER:
            {
               QJsonArray temp = QJsonArray::fromStringList(m_rfolder_List);
               object.insert("recent-folders", temp);
               break;
            }

         case RECENTFILE:
            {
               QJsonArray temp = QJsonArray::fromStringList(m_rf_List);
               object.insert("recent-files", temp);
               break;
            }

         case SHOW_LINEHIGHLIGHT:
            object.insert("showLineHighlight", m_struct.showLineHighlight);
            break;

         case SHOW_LINENUMBERS:
            object.insert("showLineNumbers", m_struct.showLineNumbers);
            break;

         case SHOW_SPACES:
            object.insert("showSpaces", m_struct.show_Spaces);
            break;

         case SHOW_BREAKS:
            object.insert("showBreaks", m_struct.show_Breaks);
            break;

         case SPELLCHECK:
            object.insert("spellcheck", m_struct.isSpellCheck);
            break;

         case TAB_SPACING:
            object.insert("tabSpacing", m_struct.tabSpacing);
            break;

         case USESPACES:
            object.insert("useSpaces", m_struct.useSpaces);
            break;

         case WORDWRAP:
            object.insert("word-wrap", m_struct.isWordWrap);
            break;  
      }

      // save the new data
      doc.setObject(object);
      data = doc.toJson();

      json_SaveFile(data);
   }

   return true;
}

void MainWindow::json_getFileName()
{
   QString selectedFilter;
   QFileDialog::Options options;

   QMessageBox quest;
   quest.setWindowTitle(tr("Diamond Editor"));
   quest.setText(tr("Diamond configuration file is missing.\n\n"
                    "Create a New configuration file or select an existing Diamond configuration file.\n"));

   QPushButton *createNew    = quest.addButton("Create", QMessageBox::AcceptRole );
   QPushButton *selectConfig = quest.addButton("Select", QMessageBox::AcceptRole );
   quest.setStandardButtons(QMessageBox::Cancel);
   quest.setDefaultButton(QMessageBox::Cancel);

   quest.exec();

   if (quest.clickedButton() == createNew) {

      QString fname = m_appPath + "/config.json";

      // force windows 7 and 8 to honor initial path
      options = QFileDialog::ForceInitialDir_Win7;

      m_jsonFname = QFileDialog::getSaveFileName(this, tr("Create New Configuration File"),
            fname, tr("Json Files (*.json)"), &selectedFilter, options);

   } else if (quest.clickedButton() == selectConfig) {

      m_jsonFname = QFileDialog::getOpenFileName(this, tr("Select Diamond Configuration File"),
            "", tr("Json Files (*.json)"), &selectedFilter, options);
   }
}

QByteArray MainWindow::json_ReadFile()
{        
   QByteArray data;

   QFile file(m_jsonFname);
   if (! file.open(QFile::ReadWrite | QFile::Text)) {
      const QString msg = tr("Unable to open Configurtion File: ") +  m_jsonFname + " : " + file.errorString();
      csError(tr("Read Json"), msg);
      return data;
   }

   file.seek(0);
   data = file.readAll();
   file.close();

   return data;
}

bool MainWindow::json_SaveFile(QByteArray data)
{
   QFile file(m_jsonFname);
   if (! file.open(QFile::ReadWrite | QFile::Truncate | QFile::Text)) {
      const QString msg = tr("Unable to save Configurtion File: ") +  m_jsonFname + " : " + file.errorString();
      csError(tr("Save Json"), msg);
      return false;
   }

   file.seek(0);
   file.write(data);
   file.close();

   return true;
}

bool MainWindow::json_CreateNew()
{
   QJsonObject object;
   QJsonValue value;
   QJsonArray list;

   object.insert("pos-x",       400);
   object.insert("pos-y",       200);
   object.insert("size-width",  800);
   object.insert("size-height", 600);

   object.insert("useSpaces",   true);
   object.insert("tabSpacing",  4);

   object.insert("showLineNumbers",   true);
   object.insert("showLineHighlight", true);
   object.insert("word-wrap",   false);
   object.insert("showSpaces",  false);
   object.insert("showBreaks",  false);
   object.insert("column-mode", false);
   object.insert("spellcheck",  false);
   object.insert("autoLoad",    true);

   value = QJsonValue(QString("MM/dd/yyyy"));
   object.insert("formatDate", value);

   value = QJsonValue(QString("h:mm ap"));
   object.insert("formatTime", value);

   value = QJsonValue(m_appPath);
   object.insert("pathPrior", value);

   // get syntax folder
   QString syntaxPath = get_SyntaxPath();

   value = QJsonValue(QString(syntaxPath));
   object.insert("pathSyntax", value);

   // get dictionary file location
   QString dictFile = get_xxFile("Dictionary File (*.dic)", "en_US.dic", "Dictionary Files (*.dic)" );

   value = QJsonValue(QString(dictFile));
   object.insert("dictMain", value);

   // default dictionary to dictPath
   dictFile = this->pathName(dictFile) + "/userDict.txt";

   if (! QFile::exists(dictFile) ) {

      QFile temp(dictFile);
      temp.open(QIODevice::WriteOnly);
      temp.close();
   }

   value = QJsonValue(QString( dictFile) );
   object.insert("dictUser", value);      

   // get aboutURL file location
   QString indexPath = get_xxFile("Help File (index.html)", "index.html", "HTML Files (index.html)" );

   value = QJsonValue(QString(indexPath));
   object.insert("aboutUrl", value);

   // adv find
   value = QJsonValue(QString("diamond"));
   object.insert("advFile-text",       value);

   value = QJsonValue(QString("*.*"));
   object.insert("advFile-filetype",   value);

   value = QJsonValue(m_appPath);
   object.insert("advFile-folder",     value);

   // print options
   value = QJsonValue(QString(""));

   object.insert("prt-printHeader",  true);
   object.insert("prt-printFooter",  true);

   object.insert("prt-headerLeft",   QJsonValue(QString("$(PathFileName)")));
   object.insert("prt-headerCenter", value);
   object.insert("prt_headerRight",  QJsonValue(QString("Page $(PageNo)")));
   object.insert("prt-headerLine2",  value);

   object.insert("prt-footerLeft",   QJsonValue(QString("$(Date)")));
   object.insert("prt-footerCenter", value);
   object.insert("prt-footerRight",  QJsonValue(QString("Diamond Editor")));
   object.insert("prt-footerLine2",  value);

   object.insert("prt-marginTop",    0.75);
   object.insert("prt-marginBottom", 0.75);
   object.insert("prt-marginLeft",   0.50);
   object.insert("prt-marginRight",  0.50);
   object.insert("prt-hdrGap",       0.25);

   value = QJsonValue(QString("Courier New,10,-1,5,50,0,0,0,0,0"));
   object.insert("prt-fontHeader",   value);

   value = QJsonValue(QString("Courier New,10,-1,5,50,0,0,0,0,0"));
   object.insert("prt-fontFooter",   value);

   value = QJsonValue(QString("Arial,12,-1,5,50,0,0,0,0,0"));
   object.insert("prt-fontText",     value);

   // user defined shortcuts
   QString modifier;

#ifdef Q_OS_MAC
   modifier = "Meta+";
#else
   modifier = "Ctrl+";
#endif

   // standard keys
   QString keys;

   keys  = QKeySequence(QKeySequence::Open).toString(QKeySequence::NativeText);
   value = QJsonValue(keys);
   object.insert("key-open",   value );

   keys  = QKeySequence(QKeySequence::Close).toString(QKeySequence::NativeText);
   value = QJsonValue(keys);
   object.insert("key-close",  value );

   keys  = QKeySequence(QKeySequence::Save).toString(QKeySequence::NativeText);
   value = QJsonValue(keys);
   object.insert("key-save",   value );

   keys  = QKeySequence(QKeySequence::SaveAs).toString(QKeySequence::NativeText);
   value = QJsonValue(keys);
   object.insert("key-saveAs",  value );

   keys  = QKeySequence(QKeySequence::Print).toString(QKeySequence::NativeText);
   value = QJsonValue(keys);
   object.insert("key-print",  value );

   keys  = QKeySequence(QKeySequence::Undo).toString(QKeySequence::NativeText);
   value = QJsonValue(keys);
   object.insert("key-undo",   value );

   keys  = QKeySequence(QKeySequence::Redo).toString(QKeySequence::NativeText);
   value = QJsonValue(keys);
   object.insert("key-redo",   value );

   keys  = QKeySequence(QKeySequence::Cut).toString(QKeySequence::NativeText);
   value = QJsonValue(keys);
   object.insert("key-cut",   value );

   keys  = QKeySequence(QKeySequence::Copy).toString(QKeySequence::NativeText);
   value = QJsonValue(keys);
   object.insert("key-copy",   value );

   keys  = QKeySequence(QKeySequence::Paste).toString(QKeySequence::NativeText);
   value = QJsonValue(keys);
   object.insert("key-paste",         value );

   keys  = QKeySequence(QKeySequence::SelectAll).toString(QKeySequence::NativeText);
   value = QJsonValue(keys);
   object.insert("key-selectAll",   value );

   keys  = QKeySequence(QKeySequence::Find).toString(QKeySequence::NativeText);
   value = QJsonValue(keys);
   object.insert("key-find",        value );

   keys  = QKeySequence(QKeySequence::Replace).toString(QKeySequence::NativeText);
   value = QJsonValue(keys);
   object.insert("key-replace",     value );

   keys  = QKeySequence(QKeySequence::FindNext).toString(QKeySequence::NativeText);
   value = QJsonValue(keys);
   object.insert("key-findNext",    value );

   keys  = QKeySequence(QKeySequence::FindPrevious).toString(QKeySequence::NativeText);
   value = QJsonValue(keys);
   object.insert("key-findPrev",    value );

   keys  = QKeySequence(QKeySequence::MoveToStartOfDocument).toString(QKeySequence::NativeText);
   value = QJsonValue(keys);
   object.insert("key-goTop",       value );

   keys  = QKeySequence(QKeySequence::MoveToEndOfDocument).toString(QKeySequence::NativeText);
   value = QJsonValue(keys);
   object.insert("key-goBottom",    value );

   keys  = QKeySequence(QKeySequence::AddTab).toString(QKeySequence::NativeText);
   value = QJsonValue(keys);
   object.insert("key-newTab",      value );

   // user keys
   value = QJsonValue(QString(modifier + "E"));
   object.insert("key-selectLine",  value );

   value = QJsonValue(QString(modifier + "D"));
   object.insert("key-selectWord",  value );

   value = QJsonValue(QString(modifier + "B"));
   object.insert("key-selectBlock", value );

   value = QJsonValue(QString(modifier + "U"));
   object.insert("key-upper",       value);

   value = QJsonValue(QString(modifier + "L"));
   object.insert("key-lower",       value);

   value = QJsonValue(QString(modifier + "I"));
   object.insert("key-indentIncr",  value);

   value = QJsonValue(QString(modifier + "Shift+I"));
   object.insert("key-indentDecr",  value);

   value = QJsonValue(QString("Alt+D"));
   object.insert("key-deleteLine",  value);

   value = QJsonValue(QString(modifier + "K"));
   object.insert("key-deleteEOL",  value);

   value = QJsonValue(QString("Alt+C"));
   object.insert("key-columnMode",  value);

   value = QJsonValue(QString("Alt+G"));
   object.insert("key-goLine",      value);

   value = QJsonValue(QString(""));
   object.insert("key-showSpaces",   value);

   value = QJsonValue(QString(""));
   object.insert("key-showBreaks", value);

   value = QJsonValue(QString("Alt+Return"));
   object.insert("key-macroPlay",  value);

   value = QJsonValue(QString("F7"));
   object.insert("key-spellCheck",  value);

   //
   value = QJsonValue(QString("Courier,12,-1,5,50,0,0,0,0,0"));
   object.insert("font-normal", value);

   value = QJsonValue(QString("Courier,12,-1,5,50,0,0,0,0,0"));
   object.insert("font-column", value);

   value = QJsonValue(QString("0,0,0"));         // black
   object.insert("color-text", value);

   value = QJsonValue(QString("255,255,255"));   // white
   object.insert("color-back", value);

   value = QJsonValue(QString("0,0,0"));         // black
   object.insert("color-highText", value);

   value = QJsonValue(QString("255,255,153"));   // light yellow
   object.insert("color-highBack", value);

   // dummy
   list.append(0);
   list.append(true);
   list.append(0);

   list.replace(0, QFont::Bold);
   list.replace(1, false);
   list.replace(2, QString("0,0,255"));         // blue
   object.insert("syntax-key", list);

   list.replace(0, QFont::Normal);
   list.replace(1, false);
   list.replace(2, QString("0,0,255"));         // blue
   object.insert("syntax-type", list);

   list.replace(0, QFont::Normal);
   list.replace(1, false);
   list.replace(2, QString("128,0,128"));       // dark magenta
   object.insert("syntax-class", list);

   list.replace(0, QFont::Normal);
   list.replace(1, false);
   list.replace(2, QString("0,0,255"));         // blue
   object.insert("syntax-func", list);

   list.replace(0, QFont::Normal);
   list.replace(1, false);
   list.replace(2, QString("0,128,0"));         // dark green
   object.insert("syntax-quote", list);

   list.replace(0, QFont::Normal);
   list.replace(1, false);
   list.replace(2, QString("0,128,0"));         // dark green
   object.insert("syntax-comment", list);

   list.replace(0, QFont::Normal);
   list.replace(1, false);
   list.replace(2, QString("0,128,0"));         // dark green
   object.insert("syntax-mline", list);

   value = QJsonValue(QJsonArray());
   object.insert("find-list", value);

   value = QJsonValue(QJsonArray());
   object.insert("replace-list", value);

   // next macro name
   value = QJsonValue(QString("macro1"));
   object.insert("macro-next", value);   

   value = QJsonValue(QJsonArray());
   object.insert("macro-names", value);

   value = QJsonValue(QJsonArray());
   object.insert("macro1", value);

   value = QJsonValue(QJsonArray());
   object.insert("recent-folders", value);

   value = QJsonValue(QJsonArray());
   object.insert("recent-files", value);

   value = QJsonValue(QJsonArray());
   object.insert("opened-files", value);

   // save the data
   QJsonDocument doc(object);
   QByteArray data = doc.toJson();

   bool ok = json_SaveFile(data);

   return ok;
}


// **
QString MainWindow::get_SyntaxPath()
{
   QString msg  = tr("Select Diamond Syntax Folder");
   QString path = this->get_DirPath(msg, m_appPath);

   return path;
}

QString MainWindow::get_xxFile(QString title, QString fname, QString filter)
{
   QString selectedFilter;
   QFileDialog::Options options;

   // force windows 7 and 8 to honor initial path
   options = QFileDialog::ForceInitialDir_Win7;

   fname = m_appPath + "/" + fname;

   QString file = QFileDialog::getOpenFileName(this, "Select " + title,
         fname, filter, &selectedFilter, options);

   return file;
}

QFont MainWindow::json_SetFont(QString value)
{
   QFont temp;
   temp.fromString(value);

   return temp;
}

QColor MainWindow::json_SetColor(QString values)
{
   QStringList list = values.split(",");

   int red   = 255;
   int green = 255;
   int blue  = 255;

   if (list.count() > 2 ) {
      red   = list[0].toInt();
      green = list[1].toInt();
      blue  = list[2].toInt();
   }

   QColor color(red,green,blue);

   return color;
}

QString MainWindow::json_GetRGB(QColor color)
{
   int red   = color.red();
   int green = color.green();
   int blue  = color.blue();

   QStringList list;
   list.append(QString::number(red));
   list.append(QString::number(green));
   list.append(QString::number(blue));

   QString values = list.join(",");

   return values;
}

QJsonObject MainWindow::json_SaveSyntax(QJsonObject object)
{
   QJsonArray list;

   // dummy
   list.append(0);
   list.append(true);
   list.append(0);

   list.replace(0, m_struct.syn_KeyWeight );
   list.replace(1, m_struct.syn_KeyItalic );
   list.replace(2, json_GetRGB(m_struct.syn_KeyText));   
   object.insert("syntax-key", list);         

   list.replace(0, m_struct.syn_TypeWeight );
   list.replace(1, m_struct.syn_TypeItalic );
   list.replace(2, json_GetRGB(m_struct.syn_TypeText));
   object.insert("syntax-type", list);

   list.replace(0, m_struct.syn_ClassWeight );
   list.replace(1, m_struct.syn_ClassItalic );
   list.replace(2, json_GetRGB(m_struct.syn_ClassText));
   object.insert("syntax-class", list);

   list.replace(0, m_struct.syn_FuncWeight );
   list.replace(1, m_struct.syn_FuncItalic );
   list.replace(2, json_GetRGB(m_struct.syn_FuncText));
   object.insert("syntax-func", list);

   list.replace(0, m_struct.syn_QuoteWeight );
   list.replace(1, m_struct.syn_QuoteItalic );
   list.replace(2, json_GetRGB(m_struct.syn_QuoteText));
   object.insert("syntax-quote", list);

   list.replace(0, m_struct.syn_CommentWeight );
   list.replace(1, m_struct.syn_CommentItalic );
   list.replace(2, json_GetRGB(m_struct.syn_CommentText));
   object.insert("syntax-comment", list);

   list.replace(0, m_struct.syn_MLineWeight );
   list.replace(1, m_struct.syn_MLineItalic );
   list.replace(2, json_GetRGB(m_struct.syn_MLineText));
   object.insert("syntax-mline", list);

   return object;
}


// **
void MainWindow::move_ConfigFile()
{
   QSettings settings("Diamond Editor", "Settings");
   m_jsonFname = settings.value("configName").toString();

   //
   Dialog_Config *dw = new Dialog_Config(m_jsonFname);
   int result = dw->exec();

   switch (result) {
      case QDialog::Rejected:
         break;

      case 1:
         // create
         {
            QString selectedFilter;
            QFileDialog::Options options;

            // force windows 7 and 8 to honor initial path
            options = QFileDialog::ForceInitialDir_Win7;

            QString newName = QFileDialog::getSaveFileName(this, tr("Create New Configuration File"),
                  m_appPath + "/config.json", tr("Json Files (*.json)"), &selectedFilter, options);

            if (newName.isEmpty()) {
               // do nothing

            } else if (QFile::exists(newName) ) {
               // can this happen?
               csError("Diamond Configuration", "Configuration file already exists, unable to create new file.");

            } else {
               m_jsonFname = newName;
               settings.setValue("configName", m_jsonFname);

               json_CreateNew();
               json_Read();

               // maybe add reset later
               csError("Diamond Configuration", "New configuration file selected."
                        " Restart Diamond to utilize the new configuration file settings.");
            }

            break;
         }

      case 2:
         // select
         {
            QString selectedFilter;
            QFileDialog::Options options;

            QString newName = QFileDialog::getOpenFileName(this, tr("Select Diamond Configuration File"),
                  "*.json", tr("Json Files (*.json)"), &selectedFilter, options);

            if (newName.isEmpty()) {
               // do nothing

            } else if (QFile::exists(newName) ) {
               m_jsonFname = newName;
               settings.setValue("configName", m_jsonFname);

               json_Read();

               // maybe add reset later
               csError("Diamond Configuration", "New configuration file selected."
                        " Restart Diamond to utilize the new configuration file settings.");
            }

            break;
         }

      case 3:
         // rename
         QString newName = dw->get_newName();

         if (newName.isEmpty()) {
            csError("Diamond Configuration", "No configuration file specified, unable to rename.");

         } if (QFile::exists(newName) ) {
            csError("Diamond Configuration", "New configuration file already exists, unable to rename.");

         } else  {
            if (QFile::rename(m_jsonFname, newName)) {
               m_jsonFname = newName;
               settings.setValue("configName", m_jsonFname);

            } else {
               csError("Diamond Configuration", "Configuration file rename failed.");

            }
         }

         break;
   }
}

// **
QStringList MainWindow::json_Load_MacroIds()
{
   // get existing json data
   QByteArray data = json_ReadFile();

   QJsonDocument doc = QJsonDocument::fromJson(data);
   QJsonObject object = doc.object();

   //
   QStringList keyList = object.keys();
   QStringList macroList;

   int count = keyList.count();

   for (int k = 0; k < count; k++)  {
      QString key = keyList.at(k);

      if (key.left(5) == "macro" && key != "macro-names" && key != "macro-next"  ) {
         macroList.append(key);
      }
   }

   return macroList;
}

bool MainWindow::json_Load_Macro(QString macroName)
{
   bool ok = true;

   // get existing json data
   QByteArray data = json_ReadFile();

   QJsonDocument doc = QJsonDocument::fromJson(data);

   QJsonObject object = doc.object();
   QJsonArray list;

   // macro data
   list = object.value(macroName).toArray();
   int cnt = list.count();

   m_textEdit->macroStart();

   for (int k = 0; k < cnt; k++)  {

      QJsonArray element = list.at(k).toArray();

      // hard coded order
      int key      = element.at(0).toString().toInt();
      Qt::KeyboardModifier modifier = Qt::KeyboardModifier( element.at(1).toString().toInt() );
      QString text = element.at(2).toString();

      QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, key, modifier, text);

      m_textEdit->add_MacroEvent(event);
   }

   m_textEdit->macroStop();

   return ok;
}

QList<macroStruct> MainWindow::json_View_Macro(QString macroName)
{
   QList<macroStruct> retval;

   // get existing json data
   QByteArray data = json_ReadFile();

   QJsonDocument doc = QJsonDocument::fromJson(data);

   QJsonObject object = doc.object();
   QJsonArray list;

   // macro data
   list = object.value(macroName).toArray();
   int cnt = list.count();

   for (int k = 0; k < cnt; k++)  {

      QJsonArray element = list.at(k).toArray();

      // hard coded order
      int key      = element.at(0).toString().toInt();
      Qt::KeyboardModifier modifier = Qt::KeyboardModifier( element.at(1).toString().toInt() );
      QString text = element.at(2).toString();

      struct macroStruct temp;
      temp.key       = key;
      temp.modifier  = modifier;
      temp.text      = text;

      retval.append(temp);
   }

   return retval;
}

void MainWindow::json_Save_MacroNames(QStringList list)
{
   m_macroNames = list;
   json_Write(MACRO_NAMES);
}

