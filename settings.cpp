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

#include <QFileDialog>
#include <QSettings>
#include <QDomDocument>

const QString xmlRoot = "Diamond";

bool MainWindow::readCfg()
{
   bool ok = true;

   QSettings settings("Diamond Editor", "Settings");
   m_cfgFName = settings.value("filename").toString();

   if (m_cfgFName.isEmpty()) {
      // get a new file name
      cfg_FileName();

      if (m_cfgFName.isEmpty()) {
         return false;
      }
      settings.setValue("filename", m_cfgFName);
   }

   if (ok) {
      if ( ! QFile::exists(m_cfgFName) ) {
         cfg_NewXml();
      }

      ok = cfg_ParseXml();
      if (! ok)  {
         // broom - need to resolve
         return false;
      }

      QDomElement root = m_domCfg.documentElement();
      QString value;

      // 1
      value = cfg_GetText(root, "size-width");
      int width = value.toInt();

      value = cfg_GetText(root, "size-height");
      int height = value.toInt();

      QSize size = QSize(width, height);
      resize(size);

      // 2
      value = cfg_GetText(root, "pos-x");
      int x = value.toInt();

      value = cfg_GetText(root, "pos-y");
      int y = value.toInt();

      QPoint pos = QPoint(x, y);
      move(pos);

      // 3
      value = cfg_GetText(root, "font") ;
      {
         QFont temp;
         temp.fromString(value);

         m_textEdit->setFont(temp);
         m_struct.font = temp;
      }

      // 4
      rf_List = cfg_GetList(root, "recentFiles");

      // 5
      m_struct.dateFormat = cfg_GetText(root, "dateFormat");

      // 6
      value = cfg_GetText(root, "tabSpacing");
      m_struct.tabSpacing = value.toInt();

   }

   return ok;
}

void MainWindow::writeCfg(Option data)
{
   bool ok = true;

   QSettings settings("Diamond Editor", "Settings");
   m_cfgFName = settings.value("filename").toString();

   if (m_cfgFName.isEmpty()) {
      // get a new file name
      cfg_FileName();

      if (m_cfgFName.isEmpty()) {
         ok = false;

      } else {
         cfg_NewXml();
         settings.setValue("filename", m_cfgFName);
      }
   }

   if (ok) {
      ok = cfg_ParseXml();
      if (! ok)  {
         // broom - need to resolve
         return;
      }

      QDomElement root = m_domCfg.documentElement();
      QString value;      

      switch (data)  {
         case CLOSE:
            cfg_Update(root, "pos-x",       QString::number(pos().x())  );
            cfg_Update(root, "pos-y",       QString::number(pos().y())  );
            cfg_Update(root, "size-width",  QString::number(size().width()  ));
            cfg_Update(root, "size-height", QString::number(size().height()));

            break;

         case FONT:
            value = cfg_GetText(root, "font");

            if (value.isEmpty() ) {
               // missing tag
               QDomElement tag  = m_domCfg.createElement("font");
               root.appendChild(tag);

               tag.appendChild(m_domCfg.createTextNode(m_struct.font.toString()));

            } else {
               // update
               cfg_Update(root, "font", m_struct.font.toString() );

            }

            break;

         case RECENTFILE:
            cfg_UpdateList(root, "recentFiles", rf_List);
            break;

         case DATEFORMAT:                       
            cfg_Update(root, "dateFormat", m_struct.dateFormat);
            break;

         case TAB_SPACING:                       
            cfg_Update(root, "tabSpacing", QString::number(m_struct.tabSpacing) );
            break;
      }

      // save xml file
      cfg_SaveXml();
   }
}

void MainWindow::cfg_FileName()
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
      m_cfgFName = QFileDialog::getSaveFileName(this, tr("Create new Settings File"),
            "", tr("All Files (*)"), &selectedFilter, options);

   } else {

      QString selectedFilter;
      m_cfgFName = QFileDialog::getOpenFileName(this, tr("Select existing Settings File"),
            "", tr("All Files (*)"), &selectedFilter, options);
   }
}

bool MainWindow::cfg_ParseXml()
{
   QFile file(m_cfgFName);
   if (! file.open(QFile::ReadWrite | QFile::Text)) {
      const QString msg = tr("Unable to open settings file: ") + m_cfgFName + " : " + file.errorString();
      csError(tr("Parse XML"), msg);

      return false;
   }

   file.seek(0);  

   // parse file content into dom
   QString errorMsg;
   int errorLine;
   int errorColumn;

   if (! m_domCfg.setContent(&file, false, &errorMsg, &errorLine, &errorColumn)) {

     const QString msg = tr("Set content failed.  \n\nError:") + errorMsg + "\n" +
         "Line: " + QString::number(errorLine) + "  Column: " + QString::number(errorColumn);
     csError(tr("Parse XML"), msg);

     file.close();
     return false;
   }

   // test root element
   QDomElement root = m_domCfg.documentElement();

   if (root.tagName() != xmlRoot)  {
      const QString msg = tr("Settings file root element must be: ") + xmlRoot + " Root was: " + root.tagName();
      csError(tr("Parse XML"), msg);

      file.close();
      return false;
   }

   file.close();
   return true;
}

bool MainWindow::cfg_NewXml()
{      
   m_domCfg = QDomDocument();

   // encoding
   QDomProcessingInstruction xml =  m_domCfg.createProcessingInstruction("xml", " version=\"1.0\" encoding=\"UTF-8\"");
   m_domCfg.appendChild(xml);

   // dtd
   QDomImplementation implement;
   QDomDocumentType dtdType = implement.createDocumentType("Diamond", 0, "Diamond.dtd");
   m_domCfg.appendChild(dtdType);

   // root
   QDomElement root = m_domCfg.createElement(xmlRoot);
   m_domCfg.appendChild(root);

   QDomElement tag;

   // 1
   tag = m_domCfg.createElement("pos-x");
   root.appendChild(tag);
   tag.appendChild(m_domCfg.createTextNode("400"));

   tag = m_domCfg.createElement("pos-y");
   root.appendChild(tag);
   tag.appendChild(m_domCfg.createTextNode("200"));

   tag = m_domCfg.createElement("size-width");
   root.appendChild(tag);
   tag.appendChild(m_domCfg.createTextNode("800"));

   // 2
   tag = m_domCfg.createElement("size-height");
   root.appendChild(tag);
   tag.appendChild(m_domCfg.createTextNode("600"));

   // 3
   tag = m_domCfg.createElement("font");
   root.appendChild(tag);
   tag.appendChild(m_domCfg.createTextNode("Courier,12,-1,5,50,0,0,0,0,0"));

   //4   ( there is no data )
   tag = m_domCfg.createElement("recentFiles");
   root.appendChild(tag);

   // 5
   tag = m_domCfg.createElement("dateFormat");
   root.appendChild(tag);
   tag.appendChild(m_domCfg.createTextNode("MM/dd/yyyy"));

   // 7
   tag = m_domCfg.createElement("tabSpacing");
   root.appendChild(tag);
   tag.appendChild(m_domCfg.createTextNode("4"));

   //
   cfg_SaveXml();

   return true;
}

bool MainWindow::cfg_SaveXml()
{
   QFile file(m_cfgFName);
   if (! file.open(QFile::ReadWrite | QFile::Truncate | QFile::Text)) {
      const QString msg = tr("Unable to save settings file: ") + m_cfgFName + " : " + file.errorString();
      csError(tr("Save XML"), msg);

      return false;
   }

   file.seek(0);

   QByteArray temp;
   temp = m_domCfg.toByteArray();

   file.write(temp);
   file.close();

   return true;
}

QString MainWindow::cfg_GetText(QDomElement root, QString name)
{
   QString data = "";

   QDomElement node;
   QDomElement element;

   node = root.firstChildElement(name);

   if ( node.isElement() )  {
      element = node.toElement();
      data    = element.text();

      if (data.isEmpty())  {
         // forces an update
         data = "*empty value*";
      }
   }

   return data;
}

void MainWindow::cfg_Update(QDomElement root, QString name, QString newData)
{
   QDomElement oldNode = root.firstChildElement(name);

   QDomElement newNode = m_domCfg.createElement(name);
   newNode.appendChild(m_domCfg.createTextNode(newData));

   root.replaceChild(newNode, oldNode);
}

// next two are for a QStringList
QStringList MainWindow::cfg_GetList(QDomElement root, QString name)
{
   QStringList dataList;
   QString temp;

   QDomNodeList list;
   QDomNode node;
   QDomElement element;

   node = root.firstChildElement(name);

   if ( node.hasChildNodes() ) {

      list = node.childNodes();
      int cnt  = list.count();

      for (int k = 0; k < cnt; k++ ) {

         node = list.item(k);

         if ( node.isElement() )  {
            element = node.toElement();
            temp    = element.text();

            if (! temp.isEmpty())  {
               dataList.append(temp);
            }
         }
      }
   }

   return dataList;
}

void MainWindow::cfg_UpdateList(QDomElement root, QString name, QStringList newData)
{
   QDomElement oldNode = root.firstChildElement(name);
   root.removeChild(oldNode);

   //
   QDomElement newParent;
   QDomElement newNode;

   newParent = m_domCfg.createElement(name);
   root.appendChild(newParent);

   //
   int cnt = rf_List.count();

   for (int k = 0; k < cnt; ++k)   {
      newNode = m_domCfg.createElement("file"+QString::number(k+1));
      newParent.appendChild(newNode);
      newNode.appendChild(m_domCfg.createTextNode(newData.at(k)));
   }
}



