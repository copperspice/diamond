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

#include "mainwindow.h"
#include "util.h"

#include <QByteArray>
#include <QDir>
#include <QFlags>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPushButton>
#include <QSettings>
#include <Qt>

bool MainWindow::json_Read()
{
   bool ok = true;

   QSettings settings("Diamond Editor", "Settings");
   m_jsonFname = settings.value("jsonName").toString();

   if ( m_jsonFname.isEmpty()) {
      // get a new file name
      json_getFileName();

      if ( m_jsonFname.isEmpty()) {
         return false;
      }
      settings.setValue("jsonName", m_jsonFname);
   }

   if (ok) {

      if (! QFile::exists(m_jsonFname) ) {
         // json file is missing
         json_CreateNew();
      }

      // get existing json data
      QByteArray data = json_ReadFile();

      QJsonDocument doc = QJsonDocument::fromJson(data);

      QJsonObject object = doc.object();
      QJsonValue value;
      QJsonArray list;

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
      m_struct.isColumnMode      = object.value("column-mode").toBool();      
      m_struct.isSpellCheck      = object.value("spellcheck").toBool();
      m_struct.autoLoad          = object.value("autoLoad").toBool();

      //
      m_struct.formatDate = object.value("formatDate").toString();
      m_struct.formatTime = object.value("formatTime").toString();
      m_struct.pathPrior  = object.value("pathPrior").toString();
      m_struct.pathSyntax = object.value("pathSyntax").toString();

      m_struct.dictMain   = object.value("dictMain").toString();
      m_struct.dictUser   = object.value("dictUser").toString();
      m_struct.aboutUrl   = object.value("aboutUrl").toString();

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

      m_printer.fontHeader     = json_SetFont(object.value("prt-fontHeader").toString());
      m_printer.fontFooter     = json_SetFont(object.value("prt-fontFooter").toString());
      m_printer.fontText       = json_SetFont(object.value("prt-fontText").toString());

      // keys                       
      m_struct.key_selectLine  = object.value("key-selectLine").toString();
      m_struct.key_selectWord  = object.value("key-selectWord").toString();
      m_struct.key_selectBlock = object.value("key-selectBlock").toString();
      m_struct.key_upper       = object.value("key-upper").toString();
      m_struct.key_lower       = object.value("key-lower").toString();      
      m_struct.key_indentIncr  = object.value("key-indentIncr").toString();
      m_struct.key_indentDecr  = object.value("key-indentDecr").toString();
      m_struct.key_deleteEOL   = object.value("key-deleteEOL").toString();
      m_struct.key_columnMode  = object.value("key-columnMode").toString();
      m_struct.key_goLine      = object.value("key-goLine").toString();
      m_struct.key_macroPlay   = object.value("key-macroPlay").toString();
      m_struct.key_spellCheck  = object.value("key-spellCheck").toString();

      // font
      m_struct.font = json_SetFont(object.value("font").toString());

      // colors
      m_struct.colorText     = json_SetColor(object.value("color-text").toString()    );
      m_struct.colorBack     = json_SetColor(object.value("color-back").toString()    );
      m_struct.colorHighText = json_SetColor(object.value("color-highText").toString());
      m_struct.colorHighBack = json_SetColor(object.value("color-highBack").toString());

      list = object.value("syntax-key").toArray();
      m_struct.syn_KeyWeight = list.at(0).toDouble();
      m_struct.syn_KeyItalic = list.at(1).toBool();
      m_struct.syn_KeyText   = json_SetColor(list.at(2).toString());

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

      // recent files
      list = object.value("recent-files").toArray();
      int cnt = list.count();

      for (int k = 0; k < cnt; k++)  {
         m_rf_List.append(list.at(k).toString());
      }

      // opened files
      list = object.value("opened-files").toArray();
      cnt  = list.count();

      for (int k = 0; k < cnt; k++)  {
         m_openedFiles.append(list.at(k).toString());
      }
   }

   return ok;
}

bool MainWindow::json_Write(Option route)
{
   bool ok = true;

   QSettings settings("Diamond Editor", "Settings");
   m_jsonFname = settings.value("jsonName").toString();

   if (m_jsonFname.isEmpty()) {
      // get a new file name
      json_getFileName();

      if (m_jsonFname.isEmpty()) {
         return false;
      }
      settings.setValue("jsonName",  m_jsonFname);
   }

   if (ok) {
      // get existing json data
      QByteArray data = json_ReadFile();

      QJsonDocument doc  = QJsonDocument::fromJson(data);
      QJsonObject object = doc.object();

      switch (route)  {

         case ABOUTURL:
            object.insert("aboutUrl", m_struct.aboutUrl);
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

         case FONT:           
            {
               QString temp = m_struct.font.toString();
               object.insert("font", temp);
            }

            break;

         case FORMAT_DATE:
            object.insert("formatDate", m_struct.formatDate);
            break;

         case FORMAT_TIME:
            object.insert("formatTime", m_struct.formatTime);
            break;                        

         case KEYS:
            // keys
            object.insert("key-selectLine",  m_struct.key_selectLine);
            object.insert("key-selectWord",  m_struct.key_selectWord);
            object.insert("key-selectBlock", m_struct.key_selectBlock);
            object.insert("key-upper",       m_struct.key_upper);
            object.insert("key-lower",       m_struct.key_lower);
            object.insert("key-indentIncr",  m_struct.key_indentIncr);
            object.insert("key-indentDecr",  m_struct.key_indentDecr);            
            object.insert("key-deleteEOL",   m_struct.key_deleteEOL);
            object.insert("key-columnMode",  m_struct.key_columnMode);
            object.insert("key-goLine",      m_struct.key_goLine);
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
               QList<QKeyEvent *> keyList;
               keyList = m_textEdit->get_KeyList();

               int count = keyList.count();

               QJsonValue value;
               QJsonArray temp(QJsonArray);

               for (int k= 0; k < count; ++k) {
                  // temp.append( );
               }

               object.insert("macro1", temp);
               break;
            }

         case SHOW_LINEHIGHLIGHT:
            object.insert("showLineHighlight", m_struct.showLineHighlight);
            break;

         case SHOW_LINENUMBERS:
            object.insert("showLineNumbers", m_struct.showLineNumbers);
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

         case RECENTFILE:            
            {
               QJsonArray temp = QJsonArray::fromStringList(m_rf_List);
               object.insert("recent-files", temp);
               break;
            }
      }

      // save the new data
      doc.setObject(object);
      data = doc.toJson();

      json_SaveFile(data);
   }
}

void MainWindow::json_getFileName()
{
   QFileDialog::Options options;

   if (false)  {  //(Q_OS_DARWIM) {
      options |= QFileDialog::DontUseNativeDialog;
   }

   QMessageBox quest;
   quest.setWindowTitle(tr("Diamond Editor"));
   quest.setText(tr("Diamond configuration file is missing.\n\n"
                    "Create a new configuration file or select an existing Diamond configuration file.\n"));

   QPushButton *createNew    = quest.addButton("Create", QMessageBox::AcceptRole );
   QPushButton *selectConfig = quest.addButton("Select", QMessageBox::AcceptRole );
   quest.setStandardButtons(QMessageBox::Cancel);
   quest.setDefaultButton(QMessageBox::Cancel);

   quest.exec();

   if (quest.clickedButton() == createNew) {

      QString selectedFilter;
      m_jsonFname = QFileDialog::getSaveFileName(this, tr("Create new Configuration File"),
            "config.json", tr("All Files (*.json)"), &selectedFilter, options);

   } else if (quest.clickedButton() == selectConfig) {

      QString selectedFilter;
      m_jsonFname = QFileDialog::getOpenFileName(this, tr("Select Diamond Configuration File"),
            "config.json", tr("All Files (*.json)"), &selectedFilter, options);
   }
}

QString MainWindow::get_SyntaxPath()
{
   QString msg  = tr("Select locaton of Diamond Syntax Files");
   QString path = m_struct.pathSyntax;

   path = get_DirPath(msg, path);

   return path;
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

bool MainWindow::json_CreateNew()
{
   // get the syntax file location
   QString syntaxPath = get_SyntaxPath();    

   //
   QJsonObject object;
   QJsonValue value;
   QJsonArray list;

   object.insert("pos-x", 400);
   object.insert("pos-y", 200);
   object.insert("size-width", 800);
   object.insert("size-height", 600);

   object.insert("useSpaces",  true);
   object.insert("tabSpacing", 4);

   object.insert("showLineNumbers",   true);
   object.insert("showLineHighlight", true);
   object.insert("word-wrap",   false);
   object.insert("column-mode", false);
   object.insert("spellcheck",  false);
   object.insert("autoLoad",    true);

   value = QJsonValue(QString("MM/dd/yyyy"));
   object.insert("formatDate", value);

   value = QJsonValue(QString("h:mm ap"));
   object.insert("formatTime", value);

   value = QJsonValue(QString(QDir::currentPath() ));
   object.insert("pathPrior", value);

   value = QJsonValue(QString(syntaxPath));
   object.insert("pathSyntax", value);

   value = QJsonValue(QString(QDir::currentPath() + "/dictionary/en_US.dic" ));
   object.insert("dictMain", value);

   value = QJsonValue(QString(QDir::currentPath() + "/dictionary/userDict.txt" ));
   object.insert("dictUser", value);

   value = QJsonValue(QString( QDir::currentPath() + "/html/index.html" ));
   object.insert("aboutUrl", value);

   // print options
   value = QJsonValue(QString(""));

   object.insert("prt-pritnHeader",  true);
   object.insert("prt-printFooter",  true);

   object.insert("prt-headerLeft",   value);
   object.insert("prt-headerCenter", value);
   object.insert("prt_headerRight",  value);
   object.insert("prt-headerLine2",  value);
   object.insert("prt-footerLeft",   value);
   object.insert("prt-footerCenter", value);
   object.insert("prt-footerRight",  QJsonValue(QString("Diamond Editor")));
   object.insert("prt-footerLine2",  value);

   object.insert("prt-marginTop",    0.75);
   object.insert("prt-marginBottom", 0.75);
   object.insert("prt-marginLeft",   0.50);
   object.insert("prt-marginRight",  0.50);

   value = QJsonValue(QString("Courier New,10,-1,5,50,0,0,0,0,0"));
   object.insert("prt-fontHeader",   value);

   value = QJsonValue(QString("Courier New,10,-1,5,50,0,0,0,0,0"));
   object.insert("prt-fontFooter",   value);

   value = QJsonValue(QString("Arial,12,-1,5,50,0,0,0,0,0"));
   object.insert("prt-fontText",     value);

   // keys
   value = QJsonValue(QString("Ctrl+E"));
   object.insert("key-selectLine",  value );

   value = QJsonValue(QString("Ctrl+D"));
   object.insert("key-selectWord",  value );

   value = QJsonValue(QString("Ctrl+B"));
   object.insert("key-selectBlock", value );

   value = QJsonValue(QString("Ctrl+U"));
   object.insert("key-upper",       value);

   value = QJsonValue(QString("Ctrl+L"));
   object.insert("key-lower",       value);

   value = QJsonValue(QString("Ctrl+I"));
   object.insert("key-indentIncr",  value);

   value = QJsonValue(QString("Ctrl+Shift+I"));
   object.insert("key-indentDecr",  value);

   value = QJsonValue(QString("Ctrl+K"));
   object.insert("key-deleteEOL",  value);

   value = QJsonValue(QString("Alt+C"));
   object.insert("key-columnMode",  value);

   value = QJsonValue(QString("Alt+G"));
   object.insert("key-goLine",      value);

   value = QJsonValue(QString("Alt+Return"));
   object.insert("key-macroPlay",  value);

   value = QJsonValue(QString("F7"));
   object.insert("key-spellCheck",  value);

   //
   value = QJsonValue(QString("Courier,12,-1,5,50,0,0,0,0,0"));
   object.insert("font", value);

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
   object.insert("recent-files", value);

   value = QJsonValue(QJsonArray());
   object.insert("opened-files", value);

   // save the data
   QJsonDocument doc(object);
   QByteArray data = doc.toJson();

   json_SaveFile(data);
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
   int red   = list[0].toInt();
   int green = list[1].toInt();
   int blue  = list[2].toInt();

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

