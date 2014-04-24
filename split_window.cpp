/**************************************************************************
*
* Copyright (c) 2012-2014 Barbara Geller
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

#include <QBoxLayout>
#include <QPalette>
#include <QPushButton>

void MainWindow::split_Horizontal()
{
   // only allow one for now
   if (m_isSplit) {
      split_CloseButton();
   }

   m_split_textEdit = new DiamondTextEdit(this, m_struct, m_spellCheck);
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

   m_isSplit  = true;
   m_textEdit = m_split_textEdit;

   //
   m_bottomWidget = new QFrame(this);
   m_bottomWidget->setFrameShape(QFrame::Panel);

   //
   m_splitName_CB = new QComboBox();
   m_splitName_CB->setMinimumWidth(175);

/*
   QPalette palette = m_splitName_CB->palette();
   palette.setColor(QPalette::Base, QColor{64,64,96} );
   palette.setColor(QPalette::Text, QColor{255,255,255} );

   palette.setColor(QPalette::Highlight, QColor{255,255,0} );
   palette.setColor(QPalette::HighlightedText, QColor{0,128,0} );

   m_splitName_CB->setPalette(palette);
*/

   QFont font2 = m_splitName_CB->font();
   font2.setPointSize(11);
   m_splitName_CB->setFont(font2);

   for (int k = 0; k < m_openedFiles.size(); ++k) {
      QString temp = strippedName(m_openedFiles[k]);

      // file name, full name with path
      m_splitName_CB->addItem(temp, m_openedFiles[k]);
   }
   int splitIndex = m_splitName_CB->findData(m_splitFileName);
   m_splitName_CB->setCurrentIndex(splitIndex);

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

   m_bottomWidget->setLayout(layout);

   m_splitter->setOrientation(Qt::Horizontal);        // Difference Here
   m_splitter->addWidget(m_bottomWidget);

   split_Title();
   moveBar();

   connect(m_splitName_CB,   SIGNAL(currentIndexChanged(int)), this, SLOT(split_NameChanged(int)));
   connect(m_splitClose_PB,  SIGNAL(clicked()),                this, SLOT(split_CloseButton()));

   connect(m_split_textEdit->document(), SIGNAL(contentsChanged()),       this, SLOT(split_Title()));
   connect(m_split_textEdit,             SIGNAL(cursorPositionChanged()), this, SLOT(moveBar()));

   connect(m_split_textEdit, SIGNAL(undoAvailable(bool)), m_ui->actionUndo, SLOT(setEnabled(bool)));
   connect(m_split_textEdit, SIGNAL(redoAvailable(bool)), m_ui->actionRedo, SLOT(setEnabled(bool)));
   connect(m_split_textEdit, SIGNAL(copyAvailable(bool)), m_ui->actionCut,  SLOT(setEnabled(bool)));
   connect(m_split_textEdit, SIGNAL(copyAvailable(bool)), m_ui->actionCopy, SLOT(setEnabled(bool)));   
}

void MainWindow::split_Vertical()
{
   // only allow one for now
   if (m_isSplit) {
      split_CloseButton();
   }

   m_split_textEdit = new DiamondTextEdit(this, m_struct, m_spellCheck);
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

   m_isSplit  = true;
   m_textEdit = m_split_textEdit;

   //
   m_bottomWidget = new QFrame(this);
   m_bottomWidget->setFrameShape(QFrame::Panel);

   //
   m_splitName_CB = new QComboBox();
   m_splitName_CB->setMinimumWidth(175);

/*
   QPalette palette = m_splitName_CB->palette();
   palette.setColor(QPalette::Base, QColor{64,64,96} );
   palette.setColor(QPalette::Text, QColor{255,255,255} );

   palette.setColor(QPalette::Highlight, QColor{255,255,0} );
   palette.setColor(QPalette::HighlightedText, QColor{0,128,0} );

   m_splitName_CB->setPalette(palette);
*/

   QFont font2 = m_splitName_CB->font();
   font2.setPointSize(11);
   m_splitName_CB->setFont(font2);

   for (int k = 0; k < m_openedFiles.size(); ++k) {
      QString temp = strippedName(m_openedFiles[k]);

      // file name, full name with path
      m_splitName_CB->addItem(temp, m_openedFiles[k]);
   }
   int splitIndex = m_splitName_CB->findData(m_splitFileName);
   m_splitName_CB->setCurrentIndex(splitIndex);

   //
   m_splitClose_PB = new QPushButton();
   m_splitClose_PB->setText("Close");

   QBoxLayout *topbar_Layout = new QHBoxLayout();
   topbar_Layout->addWidget(m_splitName_CB, 1);
   topbar_Layout->addSpacing(25);
   topbar_Layout->addWidget(m_splitClose_PB);
   topbar_Layout->addStretch(2);

   QBoxLayout *layout = new QVBoxLayout();  
   layout->addLayout(topbar_Layout);
    layout->addWidget(m_split_textEdit);

   m_bottomWidget->setLayout(layout);

   m_splitter->setOrientation(Qt::Vertical);          // Difference Here
   m_splitter->addWidget(m_bottomWidget);

   split_Title();
   moveBar();

   connect(m_splitName_CB,   SIGNAL(currentIndexChanged(int)), this, SLOT(split_NameChanged(int)));
   connect(m_splitClose_PB,  SIGNAL(clicked()),                this, SLOT(split_CloseButton()));

   connect(m_split_textEdit->document(), SIGNAL(contentsChanged()),       this, SLOT(split_Title()));
   connect(m_split_textEdit,             SIGNAL(cursorPositionChanged()), this, SLOT(moveBar()));

   connect(m_split_textEdit, SIGNAL(undoAvailable(bool)), m_ui->actionUndo, SLOT(setEnabled(bool)));
   connect(m_split_textEdit, SIGNAL(redoAvailable(bool)), m_ui->actionRedo, SLOT(setEnabled(bool)));
   connect(m_split_textEdit, SIGNAL(copyAvailable(bool)), m_ui->actionCut,  SLOT(setEnabled(bool)));
   connect(m_split_textEdit, SIGNAL(copyAvailable(bool)), m_ui->actionCopy, SLOT(setEnabled(bool)));  
}

void MainWindow::split_Title()
{       
   QString newName = strippedName(m_splitFileName);

   if (m_split_textEdit->document()->isModified()) {
      newName += " *";
   }

   int splitIndex = m_splitName_CB->findData(m_splitFileName);
   m_splitName_CB->setItemText(splitIndex, newName);
}

void MainWindow::split_NameChanged(int data)
{  
   QString newName = m_splitName_CB->itemData(data).toString();

   if (m_splitFileName != newName)  {

      // old doc
      disconnect(m_split_textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(split_Title()));

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
         csError("Split Window Selection", "Unable to locate selected document");

         split_CloseButton();
         return;
      }

      QWidget *temp = m_tabWidget->widget(whichTab);
      DiamondTextEdit *textEdit = dynamic_cast<DiamondTextEdit *>(temp);

      if (textEdit) {
         // get document matching the file name
         m_split_textEdit->setDocument(textEdit->document());

         if (m_split_textEdit->get_ColumnMode()) {
            m_split_textEdit->setFont(m_struct.fontColumn);
         } else {
            m_split_textEdit->setFont(m_struct.fontNormal);
         }

         QPalette temp = m_split_textEdit->palette();
         temp.setColor(QPalette::Text, m_struct.colorText);
         temp.setColor(QPalette::Base, m_struct.colorBack);
         m_split_textEdit->setPalette(temp);

         m_textEdit = m_split_textEdit;

         split_Title();
         moveBar();

         connect(m_split_textEdit->document(), SIGNAL(contentsChanged()),       this, SLOT(split_Title()));

      } else {
         // close the split
         csError("Split Window Selection", "Selected document invalid");
         split_CloseButton();
      }
   }
}

void MainWindow::split_CloseButton()
{
   disconnect(m_splitName_CB,  SIGNAL(currentIndexChanged(int)), this, SLOT(split_NameChanged(int)));
   disconnect(m_splitClose_PB, SIGNAL(clicked()),                this, SLOT(split_CloseButton()));

   disconnect(m_split_textEdit->document(), SIGNAL(contentsChanged()),       this, SLOT(split_Title()));
   disconnect(m_split_textEdit,             SIGNAL(cursorPositionChanged()), this, SLOT(moveBar()));

   disconnect(m_split_textEdit, SIGNAL(undoAvailable(bool)), m_ui->actionUndo, SLOT(setEnabled(bool)));
   disconnect(m_split_textEdit, SIGNAL(redoAvailable(bool)), m_ui->actionRedo, SLOT(setEnabled(bool)));
   disconnect(m_split_textEdit, SIGNAL(copyAvailable(bool)), m_ui->actionCut,  SLOT(setEnabled(bool)));
   disconnect(m_split_textEdit, SIGNAL(copyAvailable(bool)), m_ui->actionCopy, SLOT(setEnabled(bool)));


   delete m_bottomWidget;
   // m_bottomWidget->deleteLater();      BROOM


   m_split_textEdit = 0;
   m_isSplit = false;

   // set focus to the current tab
   QWidget *temp = m_tabWidget->currentWidget();
   temp->setFocus();
}
