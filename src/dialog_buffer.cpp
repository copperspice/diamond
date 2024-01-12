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

#include "dialog_buffer.h"
#include "util.h"

#include <QListView>
#include <QStringList>
#include <QStringListModel>

Dialog_Buffer::Dialog_Buffer(QList<QString> copyBuffer)
   : m_ui(new Ui::Dialog_Buffer)
{  
   m_ui->setupUi(this);
   this->setWindowIcon(QIcon("://resources/diamond.png"));

   // alter the hightlight color
   QPalette temp = m_ui->bufferTable->palette();
   temp.setColor( QPalette::Highlight, QColor(206, 206, 206, 255));
   temp.setColor( QPalette::HighlightedText, Qt::black);
   m_ui->bufferTable->setPalette(temp);

   QStandardItemModel *model = new QStandardItemModel(this);

   for (int row = 0; row < copyBuffer.size(); ++row) {
      QStandardItem *item1 = new QStandardItem(copyBuffer.at(row));
      model->setItem(row, 0, item1);
   }

   m_ui->bufferTable->setModel(model);
   m_ui->bufferTable->setCurrentIndex(model->index(0, 0));

   m_ui->bufferTable->horizontalHeader()->hide();
   m_ui->bufferTable->horizontalHeader()->setStretchLastSection(true);

   m_ui->bufferTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
   m_ui->bufferTable->setFocus();

   m_ui->bufferTable->installEventFilter(this);

   connect(m_ui->select_PB, &QPushButton::clicked, this, &Dialog_Buffer::select);
   connect(m_ui->cancel_PB, &QPushButton::clicked, this, &Dialog_Buffer::cancel);
}

Dialog_Buffer::~Dialog_Buffer()
{
   delete m_ui;
}

bool Dialog_Buffer::eventFilter(QObject *object, QEvent *event)
{   
   if (object == m_ui->bufferTable && event->type() == QEvent::KeyPress) {

      QKeyEvent *keyPressEvent = dynamic_cast<QKeyEvent *>(event);

      if (keyPressEvent) {
         int key = keyPressEvent->key();

         // process keys
         if (key >= Qt::Key_1 && key <= Qt::Key_9) {

            int row = key-Qt::Key_1;
            QAbstractItemModel *model = m_ui->bufferTable->model();

            if (row < model->rowCount() )  {
               m_ui->bufferTable->setCurrentIndex(model->index(row, 0));
               this->done(QDialog::Accepted);

               return true;
            }
         }
      }
   }

   return false;
}

void Dialog_Buffer::select()
{
   this->done(QDialog::Accepted);
}

void Dialog_Buffer::cancel()
{
   this->done(QDialog::Rejected);
}

int Dialog_Buffer::get_Index()
{
   return m_ui->bufferTable->currentIndex().row();
}
