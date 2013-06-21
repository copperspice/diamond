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

#include "mainwindow.h"
#include <util.h>

#include <QApplication>

static void showHelp();
static void showVersion();

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   app.setOrganizationName("BG Consulting");
   app.setApplicationName("Diamond Editor");

   int retval   = 0;
   bool okToRun = true;

   // passed parameters
   QStringList fileList;
   QStringList flagList;

   for (int k = 0; k < argc; ++k)   {
      QString value = argv[k];

      if (value.left(2) == "--") {
         flagList.append(value);
      } else {
         fileList.append(value);
      }

   }

   if (flagList.contains("--help", Qt::CaseInsensitive)) {
      showHelp();
      okToRun = false;

   } else if (flagList.contains("--version", Qt::CaseInsensitive)) {
      showVersion();
      okToRun = false;
   }

   if (okToRun) {

      try{
         MainWindow dw(fileList, flagList);
         dw.show();

         retval = app.exec();

      } catch (std::exception &e) {        
         QString errMsg = QString("Exception:  %1").arg(e.what());

         //
         QMessageBox msgB;
         msgB.setIcon(QMessageBox::NoIcon);

         msgB.setWindowTitle("Diamond Editor - Issue");
         msgB.setText(errMsg);
         msgB.exec();
      }
   }

   return retval;
}

static void showHelp()
{
   QString textBody = "<table style=margin-right:25>"
                      "<tr><td width=225>&ndash;&ndash;no_autoload</td><td width=240>Force no auto load of prior files</td></tr>"
                      "<tr><td>&ndash;&ndash;no_saveconfig</td><td>Do not write config file</td></tr>"
                      "<tr></tr>"
                      "<tr><td>[fileName] [fileName] ...</td><td>Files to open</td></tr></table><br>";

   QMessageBox msgB;
   msgB.setIcon(QMessageBox::NoIcon);
   msgB.setWindowIcon(QIcon("://resources/diamond.png"));

   msgB.setWindowTitle("Diamond Editor Usage");
   msgB.setText("&ndash;&ndash;help<br>&ndash;&ndash;version");
   msgB.setInformativeText(textBody);

   msgB.setStandardButtons(QMessageBox::Ok);
   msgB.setDefaultButton(QMessageBox::Ok);

   msgB.exec();
}

static void showVersion()
{
   QString textBody = "<font color='#000080'><table style=margin-right:25>"
                      "<tr><td><nobr>Developed by Barbara Geller</nobr></td><td>barbara@copperspice.com</td></tr>"
                      "<tr><td style=padding-right:25><nobr>Developed by Ansel Sermersheim</nobr></td><td>ansel@copperspice.com</td></tr>"
                      "</table></font>"
                      "<br>"
                      "<p><small>Copyright 2012-2013 BG Consulting, All rights reserved.<br>"
                      "This program is provided AS IS with no warranty of any kind.<br></small></p>";

   QMessageBox msgB;
   msgB.setIcon(QMessageBox::NoIcon);
   msgB.setWindowIcon(QIcon("://resources/diamond.png"));

   msgB.setWindowTitle("About Diamond");
   msgB.setText("<p style=margin-right:25><center><h5>Version: 1.0<br>Build # 04.1.2013</h5></center></p>");
   msgB.setInformativeText(textBody);

   msgB.setStandardButtons(QMessageBox::Ok);
   msgB.setDefaultButton(QMessageBox::Ok);

   msgB.exec();
}
