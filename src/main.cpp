/**************************************************************************
*
* Copyright (c) 2012-2025 Barbara Geller
*
* Diamond Editor is free software. You can redistribute it and/or
* modify it under the terms of the GNU General Public License
* version 2 as published by the Free Software Foundation.
*
* Diamond Editor is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* https://www.gnu.org/licenses/
*
***************************************************************************/

#include "diamond_build_info.h"
#include "mainwindow.h"
#include "util.h"

#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

static void showHelp();
static void showVersion();

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   app.setOrganizationName("CS");
   app.setApplicationName("Diamond Editor");

   int retval   = 0;
   bool okToRun = true;

   // passed parameters
   QStringList fileList;
   QStringList flagList;

   for (int k = 0; k < argc; ++k)   {
      QString value = QString::fromUtf8(argv[k]);

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
         const QString what = QString::fromUtf8(e.what());

         if (what == "abort_no_message")  {
            // do nothing

         } else {
            QString errMsg = "Exception: " + what;

            QMessageBox msgB;
            msgB.setWindowTitle("Diamond / Issue");
            msgB.setIcon(QMessageBox::NoIcon);
            msgB.setMinimumWidth(100);

            msgB.setText(errMsg);
            msgB.exec();
         }
      }
   }

   return retval;
}

static void showHelp()
{
   QString textBody;
   textBody = "<table style=margin-right:25>"
      "<tr><td width=200>&minus;&minus;help</td><td width=240>Displays this message</td></tr>"
      "<tr><td width=200>&minus;&minus;version</td><td width=240>Displays the current version</td></tr>"

      "<tr></tr>"

      "<tr><td width=200>&minus;&minus;no_autoload</td><td width=240>Force no auto load of previously open files</td></tr>"
      "<tr><td>&minus;&minus;no_saveconfig</td><td>Do not save config file</td></tr>"
      "<tr></tr>"

      "<tr><td>[fileName] [fileName] ...</td><td>Files to open when starting Diamond</td></tr></table><br>";

   QDialog msgB;
   msgB.setWindowIcon(QIcon("://resources/diamond.png"));
   msgB.setWindowTitle("Diamond Editor Help");

   QLabel *label = new QLabel;

   QFont font = label->font();
   font.setPointSize(11);
   label->setFont(font);

   label->setText(textBody);

   QPushButton *button = new QPushButton();
   button->setText("Ok");

   QHBoxLayout *layoutButton = new QHBoxLayout();
   layoutButton->addStretch();
   layoutButton->addWidget(button);
   layoutButton->addStretch();

   QVBoxLayout *layoutMain = new QVBoxLayout();
   layoutMain->addWidget(label);
   layoutMain->addLayout(layoutButton);

   msgB.setLayout(layoutMain);

   QObject::connect(button, &QPushButton::clicked, &msgB, &QDialog::accept);

   msgB.exec();
}

static void showVersion()
{
   // change mainwindow.cpp & main.cpp

   QString textBody = "<font color='#000080'><table style=margin-right:25>"
      "<tr><td><nobr>Developed by Barbara Geller</nobr></td>"
          "<td>barbara@copperspice.com</td></tr>"
      "<tr><td style=padding-right:25><nobr>Developed by Ansel Sermersheim</nobr></td>"
          "<td>ansel@copperspice.com</td></tr>"
      "</table></font>"
      "<br>"
      "<p><small>Copyright 2012-2025 BG Consulting, All rights reserved.<br>"
      "This program is provided AS IS with no warranty of any kind.<br></small></p>";

   QMessageBox msgB;
   msgB.setIcon(QMessageBox::NoIcon);
   msgB.setWindowIcon(QIcon("://resources/diamond.png"));

   msgB.setWindowTitle("About Diamond");
   msgB.setText(QString("<p style=margin-right:25><center><h5>Version: %1<br>Build # %2</h5></center></p>")
            .formatArgs(QString::fromLatin1(versionString), QString::fromLatin1(buildDate)));
   msgB.setInformativeText(textBody);

   msgB.setStandardButtons(QMessageBox::Ok);
   msgB.setDefaultButton(QMessageBox::Ok);

   msgB.exec();
}
