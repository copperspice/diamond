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

#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>

void MainWindow::split_Horizontal()
{
   // only allow one for now
   if (m_isSplit) {
      bottomClose();
   }

   split_textEdit = new DiamondTextEdit(this, m_struct, m_spellCheck);
   m_splitName    = m_curFile;

   // sync documents
   split_textEdit->setDocument(m_textEdit->document());

   // (1) this->setScreenColors(), based on tabwidget must do by hand   
   if (split_textEdit->get_ColumnMode()) {
      split_textEdit->setFont(m_struct.fontColumn);
   } else {
      split_textEdit->setFont(m_struct.fontNormal);
   }

   QPalette temp = split_textEdit->palette();
   temp.setColor(QPalette::Text, m_struct.colorText);
   temp.setColor(QPalette::Base, m_struct.colorBack);
   split_textEdit->setPalette(temp);

   m_isSplit = true;

   //
   m_bottomWidget = new QFrame(this);
   m_bottomWidget->setFrameShape(QFrame::Panel);

   m_splitTitle = new QLabel();

   QFont font = m_splitTitle->font();
   font.setPointSize(12);
   m_splitTitle->setFont(font);

   QPushButton *closeButton = new QPushButton();
   closeButton->setText("Close");

   QBoxLayout *buttonLayout = new QHBoxLayout();
   buttonLayout->addStretch();
   buttonLayout->addWidget(closeButton);
   buttonLayout->addStretch();

   QBoxLayout *layout = new QVBoxLayout();
   layout->addWidget(m_splitTitle);
   layout->addWidget(split_textEdit);
   layout->addLayout(buttonLayout);

   m_bottomWidget->setLayout(layout);

   m_splitter->setOrientation(Qt::Horizontal);        // Difference Here
   m_splitter->addWidget(m_bottomWidget);

   // (2)
   split_Title();

   // (3) this->moveBar(), based on tabwidget must do by hand
   split_MoveBar();

   //
   connect(split_textEdit->document(), SIGNAL(contentsChanged()),       this, SLOT(split_Title()));
   connect(split_textEdit,             SIGNAL(cursorPositionChanged()), this, SLOT(split_MoveBar()));
   connect(closeButton,                SIGNAL(clicked()),               this, SLOT(bottomClose()));
}

void MainWindow::split_Vertical()
{
   // only allow one for now
   if (m_isSplit) {
      bottomClose();
   }

   split_textEdit = new DiamondTextEdit(this, m_struct, m_spellCheck);
   m_splitName    = m_curFile;

   // sync documents
   split_textEdit->setDocument(m_textEdit->document());

   // (1) this->setScreenColors(), based on tabwidget must do by hand
   if (split_textEdit->get_ColumnMode()) {
      split_textEdit->setFont(m_struct.fontColumn);
   } else {
      split_textEdit->setFont(m_struct.fontNormal);
   }

   QPalette temp = split_textEdit->palette();
   temp.setColor(QPalette::Text, m_struct.colorText);
   temp.setColor(QPalette::Base, m_struct.colorBack);
   split_textEdit->setPalette(temp);

   m_isSplit = true;

   //
   m_bottomWidget = new QFrame(this);
   m_bottomWidget->setFrameShape(QFrame::Panel);

   m_splitTitle = new QLabel();

   QFont font = m_splitTitle->font();
   font.setPointSize(12);
   m_splitTitle->setFont(font);

   QPushButton *closeButton = new QPushButton();
   closeButton->setText("Close");

   QBoxLayout *buttonLayout = new QHBoxLayout();
   buttonLayout->addStretch();
   buttonLayout->addWidget(closeButton);
   buttonLayout->addStretch();

   QBoxLayout *layout = new QVBoxLayout();
   layout->addWidget(m_splitTitle);
   layout->addWidget(split_textEdit);
   layout->addLayout(buttonLayout);

   m_bottomWidget->setLayout(layout);

   m_splitter->setOrientation(Qt::Vertical);          // Difference Here
   m_splitter->addWidget(m_bottomWidget);

   // (2)
   split_Title();

   // (3) this->moveBar(), based on tabwidget must do by hand
   split_MoveBar();

   connect(split_textEdit->document(), SIGNAL(contentsChanged()),       this, SLOT(split_Title()));
   connect(split_textEdit,             SIGNAL(cursorPositionChanged()), this, SLOT(split_MoveBar()));
   connect(closeButton,                SIGNAL(clicked()),               this, SLOT(bottomClose()));
}

void MainWindow::sideClose()
{
   // this method is not used

   disconnect(split_textEdit->document(), SIGNAL(contentsChanged()),       this, SLOT(split_Title()));
   disconnect(split_textEdit,             SIGNAL(cursorPositionChanged()), this, SLOT(split_MoveBar()));

   m_sideWidget->deleteLater();
   m_isSplit = false;
}

void MainWindow::bottomClose()
{
   disconnect(split_textEdit->document(), SIGNAL(contentsChanged()),       this, SLOT(split_Title()));
   disconnect(split_textEdit,             SIGNAL(cursorPositionChanged()), this, SLOT(split_MoveBar()));
   // disconnect(closeButton,             SIGNAL(clicked()),               this, SLOT(bottomClose()));

   m_bottomWidget->deleteLater();
   m_isSplit = false;
}

void MainWindow::split_Title()
{       
   QString modified = "";
   if (split_textEdit->document()->isModified()) {
      modified = " *";
   }

   QString temp  = QChar(0x02014);
   QString title = "Diamond Editor " + temp + " " + m_splitName + modified;

   m_splitTitle->setText(title);
}

void MainWindow::split_MoveBar()
{
   m_fromSplit = true;
   moveBar();
   m_fromSplit = false;
}
