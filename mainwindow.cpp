/***************************************************************************
**
** Copyright (C) 2012 Barbara Geller
** All rights reserved.
**
**
** GNU Free Documentation License
** This file may be used under the terms of the GNU Free Documentation
** License version 1.3 as published by the Free Software Foundation
** and appearing in the file included in the packaging of this file.
**
**
****************************************************************************/

#include "mainwindow.h"
#include "util.h"

#include <QtGui>

MainWindow::MainWindow()
   : m_ui(new Ui::MainWindow)
{
   m_ui->setupUi(this);
   setWindowFilePath("untitled.txt");

   m_textEdit = new QPlainTextEdit;
   setCentralWidget(m_textEdit);

   createToolBars();
   createConnections();
   setStatusBar(tr("Ready"), 0);

   readSettings();
   setCurrentFile("");
   setUnifiedTitleAndToolBarOnMac(true);            
}

// file
void MainWindow::newFile()
{
   if (maybeSave()) {
      m_textEdit->clear();
      setCurrentFile("");
   }
}

void MainWindow::open()
{
   if (maybeSave()) {
      QString fileName = QFileDialog::getOpenFileName(this);

      if (! fileName.isEmpty()) {
         loadFile(fileName);
      }
   }
}

void MainWindow::close_Doc()
{
   bool okClose = true;

   if (m_textEdit->document()->isModified()) {

      QMessageBox::StandardButton ret;
      ret = QMessageBox::warning(this, tr("Close Document"),
                   tr("This document has been modified.\n"
                   "Do you want to save your changes?"),
                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

      if (ret == QMessageBox::Save) {
         save();

      } else if (ret == QMessageBox::Cancel) {
        okClose = false;

      }
   }

   if (okClose) {
      m_textEdit->clear();
      setCurrentFile("");
   }
}

void MainWindow::closeAll_Doc()
{
   csMsg("Close All, not implemented.");
}

void MainWindow::reload()
{
   csMsg("Reload, not implemented.");
}

bool MainWindow::save()
{
   if (m_curFile.isEmpty()) {
      return saveAs();
   } else {
      return saveFile(m_curFile);
   }
}

bool MainWindow::saveAs()
{
   QString fileName = QFileDialog::getSaveFileName(this);
   if (fileName.isEmpty()) {
      return false;
   }

   return saveFile(fileName);
}

bool MainWindow::saveAll()
{
   csMsg("Save All, not implemented.");
}

void MainWindow::print()
{
   csMsg("Print, not implemented.");
}

void MainWindow::printPreview()
{
   csMsg("Print Preview, not implemented.");
}

void MainWindow::printSetup()
{
   csMsg("Print Setup, not implemented.");
}


//edit


//settings
void MainWindow::setBackgroundColor()
{
   bool isNative = true;
   QColor color;

   if (isNative)  {
       color = QColorDialog::getColor(Qt::green, this);

   } else  {
       color = QColorDialog::getColor(Qt::green, this, "Select Color", QColorDialog::DontUseNativeDialog);

   }

   if (color.isValid()) {     
      QPalette temp = m_textEdit->palette();
      temp.setColor( QPalette::Base, color);
      m_textEdit->setPalette(temp);
   }
}

void MainWindow::setFont()
{        
   bool ok;
   QFont font = QFontDialog::getFont(&ok, m_textEdit->font(), this);

   if (ok) {
      m_textEdit->setFont(font);
   }
}


// help
void MainWindow::about()
{
   QString textBody = "<font color='#000080'><table style=margin-right:25>"
                      "<tr><td><nobr>Developed by Barbara Geller</nobr></td><td>barbara@copperspice.com</td></tr>"
                      "<tr><td style=padding-right:25><nobr>Developed by Ansel Sermersheim</nobr></td><td>ansel@copperspice.com</td></tr>"
                      "</table></font>"
                      "<br>"
                      "<p><small>Copyright 2012 BG Consulting, All rights reserved.<br>"
                      "This program is provided AS IS with no warranty of any kind.<br></small></p>";

   //
   QMessageBox msgB;
   msgB.setIcon(QMessageBox::NoIcon);
   msgB.setWindowIcon( QIcon("://resources/plus.png"));

   msgB.setWindowTitle(tr("About Diamond"));
   msgB.setText(tr("<p style=margin-right:25><center><h5>Version: 1.0<br>Build # 10.01.2012</h5></center></p>"));
   msgB.setInformativeText(textBody);

   msgB.setStandardButtons(QMessageBox::Ok);
   msgB.setDefaultButton(QMessageBox::Ok);

   msgB.exec();
}


// connections, displays, toolbar
void MainWindow::createConnections()
{
   // file
   connect(m_ui->actionNew,            SIGNAL(triggered()), this, SLOT(newFile()));
   connect(m_ui->actionOpen,           SIGNAL(triggered()), this, SLOT(open()));
   connect(m_ui->actionClose,          SIGNAL(triggered()), this, SLOT(close_Doc()));
   connect(m_ui->actionClose_All,      SIGNAL(triggered()), this, SLOT(closeAll_Doc()));
   connect(m_ui->actionReload,         SIGNAL(triggered()), this, SLOT(reload()));

   connect(m_ui->actionSave,           SIGNAL(triggered()), this, SLOT(save()));
   connect(m_ui->actionSave_As,        SIGNAL(triggered()), this, SLOT(saveAs()));
   connect(m_ui->actionSave_All,       SIGNAL(triggered()), this, SLOT(saveAll()));

   connect(m_ui->actionPrint,          SIGNAL(triggered()), this, SLOT(print()));
   connect(m_ui->actionPrint_Preview,  SIGNAL(triggered()), this, SLOT(printPreview()));
   connect(m_ui->actionPrint_Setup,    SIGNAL(triggered()), this, SLOT(printSetup()));

   connect(m_ui->actionExit,           SIGNAL(triggered()), this, SLOT(close()));

   // edit
   connect(m_ui->actionCut,            SIGNAL(triggered()), m_textEdit, SLOT(cut()));
   connect(m_ui->actionCopy,           SIGNAL(triggered()), m_textEdit, SLOT(copy()));
   connect(m_ui->actionPaste,          SIGNAL(triggered()), m_textEdit, SLOT(paste()));

   // settings
   connect(m_ui->actionColors,         SIGNAL(triggered()), this, SLOT(setBackgroundColor()));
   connect(m_ui->actionFonts,          SIGNAL(triggered()), this, SLOT(setFont()));

   // help menu
   connect(m_ui->actionAbout,          SIGNAL(triggered()), this, SLOT(about()));


   //
   m_ui->actionLine_Numbers->setCheckable(true);
   m_ui->actionLine_Numbers->setChecked(true);

   m_ui->actionSpell_Check->setCheckable(true);
   m_ui->actionSpell_Check->setChecked(false);


   //
   connect(m_textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));

   m_ui->actionCut->setEnabled(false);
   m_ui->actionCopy->setEnabled(false);

   connect(m_textEdit, SIGNAL(copyAvailable(bool)), m_ui->actionCut,  SLOT(setEnabled(bool)));
   connect(m_textEdit, SIGNAL(copyAvailable(bool)), m_ui->actionCopy, SLOT(setEnabled(bool)));
}

void MainWindow::createToolBars()
{
   fileToolBar = addToolBar(tr("File"));
   fileToolBar->addAction(m_ui->actionNew);
   fileToolBar->addAction(m_ui->actionOpen);
   fileToolBar->addAction(m_ui->actionClose);
   fileToolBar->addAction(m_ui->actionSave);
   fileToolBar->addAction(m_ui->actionSave_All);
   fileToolBar->addAction(m_ui->actionPrint);

   editToolBar = addToolBar(tr("Edit"));
   editToolBar->addAction(m_ui->actionUndo);
   editToolBar->addAction(m_ui->actionRedo);
   editToolBar->addAction(m_ui->actionCut);
   editToolBar->addAction(m_ui->actionCopy);
   editToolBar->addAction(m_ui->actionPaste);

   editToolBar = addToolBar(tr("Tools"));
   editToolBar->addAction(m_ui->actionSpell_Check);
}

void MainWindow::setStatusBar(QString msg, int timeOut)
{
   statusBar()->showMessage(msg, timeOut);
}

