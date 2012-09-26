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

#include <QDir>
#include <QFlags>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QSettings>


#include <QByteArray>

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
      value = object.value("tabSpacing");
      m_struct.tabSpacing = value.toDouble();

      m_struct.showLineHighlight = object.value("showLineHighlight").toBool();
      m_struct.showLineNumbers   = object.value("showLineNumbers").toBool();
      m_struct.isColumnMode      = object.value("column-mode").toBool();      
      m_struct.isSpellCheck      = object.value("spellcheck").toBool();

      //
      m_struct.formatDate = object.value("formatDate").toString();
      m_struct.formatTime = object.value("formatTime").toString();
      m_struct.pathPrior  = object.value("pathPrior").toString();
      m_struct.pathSyntax = object.value("pathSyntax").toString();

      m_struct.dictMain   = object.value("dictMain").toString();
      m_struct.dictUser   = object.value("dictUser").toString();

      //
      value = object.value("font");
      {
         QFont temp;
         temp.fromString(value.toString());
         m_struct.font = temp;
      }

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

         case CLOSE:
            object.insert("pos-x",       pos().x()  );
            object.insert("pos-y",       pos().y()  );
            object.insert("size-width",  size().width()  );
            object.insert("size-height", size().height() );

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

         case PATH_PRIOR:
            object.insert("pathPrior", m_struct.pathPrior);
            break;

         case PATH_SYNTAX:
            object.insert("pathSyntax", m_struct.pathSyntax);
            break;

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

         case RECENTFILE:            
            QJsonArray temp = QJsonArray::fromStringList(m_rf_List);
            object.insert("recent-files", temp);
            break;
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
   quest.setText(tr("Settings file may be missing. Create new Settings File?"));
   quest.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
   quest.setDefaultButton(QMessageBox::Yes);

   int retval = quest.exec();

   if (retval == QMessageBox::Yes) {

      QString selectedFilter;
      m_jsonFname = QFileDialog::getSaveFileName(this, tr("Create new Settings File"),
            "", tr("All Files (*.json)"), &selectedFilter, options);

   } else {

      QString selectedFilter;
      m_jsonFname = QFileDialog::getOpenFileName(this, tr("Select existing Settings File"),
            "", tr("All Files (*.json)"), &selectedFilter, options);
   }
}

QString MainWindow::get_SyntaxPath()
{
   QString dir;

   QFileDialog::Options options;

   if (false)  {  //(Q_OS_DARWIM) {
      options |= QFileDialog::DontUseNativeDialog;
   }

   options |= QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks;

   dir = QFileDialog::getExistingDirectory(this, tr("Select location for the existing Diamond Editor Syntax Files"),
      "", options);

   dir = dir.trimmed() + "/";

   return dir;
}

bool MainWindow::json_SaveFile(QByteArray data)
{
   QFile file(m_jsonFname);
   if (! file.open(QFile::ReadWrite | QFile::Truncate | QFile::Text)) {
      const QString msg = tr("Unable to save Settings file: ") +  m_jsonFname + " : " + file.errorString();
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
      const QString msg = tr("Unable to open settings file: ") +  m_jsonFname + " : " + file.errorString();
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

   object.insert("tabSpacing", 4);

   object.insert("showLineNumbers",   true);
   object.insert("showLineHighlight", true);

   object.insert("column-mode", false);
   object.insert("spellcheck",  false);

   value = QJsonValue(QString("MM/dd/yyyy"));
   object.insert("formatDate", value);

   value = QJsonValue(QString("h:mm ap"));
   object.insert("formatTime", value);

   value = QJsonValue(QString( QDir::currentPath() ));
   object.insert("pathPrior", value);

   value = QJsonValue(QString(syntaxPath));
   object.insert("pathSyntax", value);

   value = QJsonValue(QString(QDir::currentPath() + "/dictionary/en_US.dic" ));
   object.insert("dictMain", value);

   value = QJsonValue(QString(QDir::currentPath() + "/dictionary/userDict.txt" ));
   object.insert("dictUser", value);

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

   // save the data
   QJsonDocument doc(object);
   QByteArray data = doc.toJson();

   json_SaveFile(data);
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

