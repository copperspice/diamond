/**************************************************************************
*
* Copyright (c) 2012-2024 Barbara Geller
*
* Diamond Editor is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License version 2
* as published by the Free Software Foundation.
*
* Diamond is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
***************************************************************************/

#include "mainwindow.h"

#include <QBoxLayout>
#include <QPalette>
#include <QPushButton>

void MainWindow::split_Horizontal()
{
   // only allow one for now
   if (m_isSplit) {
      split_CloseButton();
   }

   m_split_textEdit = new DiamondTextEdit(this, m_struct, m_spellCheck, "split");
   m_splitFileName  = m_curFile;

   // sync documents
   m_split_textEdit->setDocument(m_textEdit->document());

   if (m_split_textEdit->get_ColumnMode()) {
      m_split_textEdit->setFont(m_struct.fontColumn);
   } else {
      m_split_textEdit->setFont(m_struct.fontNormal);
   }

   QPalette temp = m_split_textEdit->palette();
   temp.setColor(QPalette::Text, m_struct.colorText);
   temp.setColor(QPalette::Base, m_struct.colorBack);
   m_split_textEdit->setPalette(temp);

   // position on same line
   QTextCursor cursor(m_textEdit->textCursor());
   m_split_textEdit->setTextCursor(cursor);

   m_isSplit  = true;
   m_textEdit = m_split_textEdit;

   //
   m_splitWidget = new QFrame(this);
   m_splitWidget->setFrameShape(QFrame::Panel);
   m_splitWidget->setWhatsThis("split_widget");

   //
   m_splitName_CB = new QComboBox();
   m_splitName_CB->setMinimumWidth(175);

   QFont font2 = m_splitName_CB->font();
   font2.setPointSize(11);
   m_splitName_CB->setFont(font2);

   for (int k = 0; k < m_openedFiles.size(); ++k) {

      QString fullName = m_openedFiles[k];
      add_splitCombo(fullName);

      if ( m_openedModified[k] ) {
        update_splitCombo(fullName, true);
      }
   }

   //
   m_splitClose_PB = new QPushButton();
   m_splitClose_PB->setText("Close");

   QBoxLayout *topbar_Layout = new QHBoxLayout();
   topbar_Layout->addWidget(m_splitName_CB, 1);
   topbar_Layout->addSpacing(25);
   topbar_Layout->addWidget(m_splitClose_PB);
   topbar_Layout->addStretch(2);

   //
   QBoxLayout *layout = new QVBoxLayout();
   layout->addLayout(topbar_Layout);
   layout->addWidget(m_split_textEdit);

   m_splitWidget->setLayout(layout);

   m_splitter->setOrientation(Qt::Horizontal);        // difference Here
   m_splitter->addWidget(m_splitWidget);

   //
   int splitIndex = m_splitName_CB->findData(m_splitFileName);
   m_splitName_CB->setCurrentIndex(splitIndex);

   moveBar();

   connect(m_splitName_CB,   static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
               this, &MainWindow::split_NameChanged);

   connect(m_splitClose_PB,  &QPushButton::clicked, this, &MainWindow::split_CloseButton);

   connect(m_split_textEdit->document(), &QTextDocument::contentsChanged, this, &MainWindow::set_splitCombo);
   connect(m_split_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::moveBar);
   connect(m_split_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::setStatus_LineCol);

   connect(m_split_textEdit, &DiamondTextEdit::undoAvailable, m_ui->actionUndo, &QAction::setEnabled);
   connect(m_split_textEdit, &DiamondTextEdit::redoAvailable, m_ui->actionRedo, &QAction::setEnabled);
   connect(m_split_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCut,  &QAction::setEnabled);
   connect(m_split_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCopy, &QAction::setEnabled);
}

void MainWindow::split_Vertical()
{
   // only allow one for now
   if (m_isSplit) {
      split_CloseButton();
   }

   m_split_textEdit = new DiamondTextEdit(this, m_struct, m_spellCheck, "split");
   m_splitFileName  = m_curFile;

   // sync documents
   m_split_textEdit->setDocument(m_textEdit->document());

   if (m_split_textEdit->get_ColumnMode()) {
      m_split_textEdit->setFont(m_struct.fontColumn);
   } else {
      m_split_textEdit->setFont(m_struct.fontNormal);
   }

   QPalette temp = m_split_textEdit->palette();
   temp.setColor(QPalette::Text, m_struct.colorText);
   temp.setColor(QPalette::Base, m_struct.colorBack);
   m_split_textEdit->setPalette(temp);

   // position on same line
   QTextCursor cursor(m_textEdit->textCursor());
   m_split_textEdit->setTextCursor(cursor);

   m_isSplit  = true;
   m_textEdit = m_split_textEdit;

   //
   m_splitWidget = new QFrame(this);
   m_splitWidget->setFrameShape(QFrame::Panel);
   m_splitWidget->setWhatsThis("split_widget");

   //
   m_splitName_CB = new QComboBox();
   m_splitName_CB->setMinimumWidth(175);

   QFont font2 = m_splitName_CB->font();   font2.setPointSize(11);
   m_splitName_CB->setFont(font2);

   for (int k = 0; k < m_openedFiles.size(); ++k) {
      QString fullName = m_openedFiles[k];
      add_splitCombo(fullName);

      if ( m_openedModified[k] ) {
        update_splitCombo(fullName, true);
      }
   }

   //
   m_splitClose_PB = new QPushButton();
   m_splitClose_PB->setText(tr("Close"));

   QBoxLayout *topbar_Layout = new QHBoxLayout();
   topbar_Layout->addWidget(m_splitName_CB, 1);
   topbar_Layout->addSpacing(25);
   topbar_Layout->addWidget(m_splitClose_PB);
   topbar_Layout->addStretch(2);

   QBoxLayout *layout = new QVBoxLayout();
   layout->addLayout(topbar_Layout);
   layout->addWidget(m_split_textEdit);

   m_splitWidget->setLayout(layout);

   m_splitter->setOrientation(Qt::Vertical);          // Difference is here
   m_splitter->addWidget(m_splitWidget);

   //
   int splitIndex = m_splitName_CB->findData(m_splitFileName);
   m_splitName_CB->setCurrentIndex(splitIndex);

   moveBar();

   connect(m_splitName_CB,   static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
               this, &MainWindow::split_NameChanged);

   connect(m_splitClose_PB,  &QPushButton::clicked, this, &MainWindow::split_CloseButton);

   connect(m_split_textEdit->document(), &QTextDocument::contentsChanged, this, &MainWindow::set_splitCombo);
   connect(m_split_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::moveBar);
   connect(m_split_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::setStatus_LineCol);

   connect(m_split_textEdit, &DiamondTextEdit::undoAvailable, m_ui->actionUndo, &QAction::setEnabled);
   connect(m_split_textEdit, &DiamondTextEdit::redoAvailable, m_ui->actionRedo, &QAction::setEnabled);
   connect(m_split_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCut,  &QAction::setEnabled);
   connect(m_split_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCopy, &QAction::setEnabled);
}

void MainWindow::set_splitCombo()
{
   QString shortName = strippedName(m_splitFileName);

   bool isModified = m_split_textEdit->document()->isModified();

   if (isModified) {
      shortName += " *";
   }

   int index = m_openedFiles.indexOf(m_splitFileName);
   if (index != -1)  {
      m_openedModified.replace(index,isModified);
   }

   //
   int splitIndex = m_splitName_CB->findData(m_splitFileName);

   if (splitIndex != -1)  {
      m_splitName_CB->setItemText(splitIndex, shortName);
      m_splitName_CB->setItemData(splitIndex, m_splitFileName, Qt::ToolTipRole);
   }
}

void MainWindow::update_splitCombo(QString fullName, bool isModified)
{
   QString shortName = strippedName(fullName);

   if (isModified) {
      shortName += " *";
   }

   int splitIndex = m_splitName_CB->findData(fullName);

   if (splitIndex != -1)  {
      m_splitName_CB->setItemText(splitIndex, shortName);
      m_splitName_CB->setItemData(splitIndex,fullName, Qt::ToolTipRole );
   }
}

void MainWindow::add_splitCombo(QString fullName)
{
   int splitIndex = m_splitName_CB->findData(fullName);

   if (splitIndex == -1)  {
      QString shortName = strippedName(fullName);
      m_splitName_CB->addItem(shortName, fullName);

      splitIndex = m_splitName_CB->count() - 1;
      m_splitName_CB->setItemData(splitIndex,fullName, Qt::ToolTipRole );

   } else {
      set_splitCombo();

   }
}

void MainWindow::rm_splitCombo(QString fullName)
{
   int splitIndex = m_splitName_CB->findData(fullName);

   if (splitIndex != -1) {
      m_splitName_CB->removeItem(splitIndex);
   }
}

void MainWindow::split_NameChanged(int itemNum)
{
   QString newName = m_splitName_CB->itemData(itemNum).toString();

   if (m_splitFileName != newName)  {

      // old doc
      disconnect(m_split_textEdit->document(), &QTextDocument::contentsChanged,
                     this, &MainWindow::set_splitCombo);

      // new doc
      m_splitFileName = newName;

      int whichTab = -1;
      for (int k = 0; k < m_tabWidget->count(); ++k) {

         if (newName == this->get_curFileName(k)) {
            whichTab = k;
            break;
         }
      }

      if (whichTab == -1) {
         csError(tr("Split Window Selection"), tr("Unable to locate selected document"));

         split_CloseButton();
         return;
      }

      QWidget *tabWidget = m_tabWidget->widget(whichTab);
      DiamondTextEdit *textEdit = dynamic_cast<DiamondTextEdit *>(tabWidget);

      if (textEdit) {
         // get document matching the file name
         m_split_textEdit->setDocument(textEdit->document());

         if (m_split_textEdit->get_ColumnMode()) {
            m_split_textEdit->setFont(m_struct.fontColumn);
         } else {
            m_split_textEdit->setFont(m_struct.fontNormal);
         }

         QPalette tmp = m_split_textEdit->palette();
         tmp.setColor(QPalette::Text, m_struct.colorText);
         tmp.setColor(QPalette::Base, m_struct.colorBack);
         m_split_textEdit->setPalette(tmp);

         m_textEdit = m_split_textEdit;

         set_splitCombo();

         //
         m_curFile = m_splitFileName;
         setStatus_FName(m_curFile);

         // ** retrieve slected syntax type
//       m_syntaxParser = m_textEdit->get_SyntaxParser();

         // retrieve the menu enum
//       m_syntaxEnum = m_textEdit->get_SyntaxEnum();

         // check the menu item
//       setSynType(m_syntaxEnum);

         moveBar();
         show_Spaces();
         show_Breaks();

         connect(m_split_textEdit->document(), &QTextDocument::contentsChanged,
                     this, &MainWindow::set_splitCombo);

      } else {
         // close the split
         csError(tr("Split Window Selection"), tr("Selected document invalid"));
         split_CloseButton();
      }
   }
}

void MainWindow::split_CloseButton()
{
   // set focus to the current tab widget
   QWidget *temp = m_tabWidget->currentWidget();
   temp->setFocus();

   m_isSplit = false;

   disconnect(m_splitName_CB,   static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
               this, &MainWindow::split_NameChanged);

   disconnect(m_splitClose_PB,  &QPushButton::clicked, this, &MainWindow::split_CloseButton);

   disconnect(m_split_textEdit->document(), &QTextDocument::contentsChanged, this, &MainWindow::set_splitCombo);
   disconnect(m_split_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::moveBar);
   disconnect(m_split_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::setStatus_LineCol);

   disconnect(m_split_textEdit, &DiamondTextEdit::undoAvailable, m_ui->actionUndo, &QAction::setEnabled);
   disconnect(m_split_textEdit, &DiamondTextEdit::redoAvailable, m_ui->actionRedo, &QAction::setEnabled);
   disconnect(m_split_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCut,  &QAction::setEnabled);
   disconnect(m_split_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCopy, &QAction::setEnabled);

   m_splitName_CB->clear();
   m_split_textEdit = nullptr;

   m_splitWidget->deleteLater();
}
