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

#include "dialog_savedfiles.h"
#include "util.h"

#include <QTableView>

Dialog_SavedFiles::Dialog_SavedFiles(MainWindow *parent, QStringList fileIds, QStringList fileText, QStringList fileNames)
   : QDialog(parent), m_ui(new Ui::Dialog_SavedFiles)
{
   m_parent    = parent;

   m_fileIds   = fileIds;
   m_fileText  = fileText;
   m_fileNames = fileNames;

   m_ui->setupUi(this);
   setWindowIcon(QIcon("://resources/diamond.png"));

   setUpView();

   // alter the hightlight color
   QPalette temp = m_ui->tableView->palette();
   temp.setColor( QPalette::Highlight, QColor(206, 206, 206, 255));
   temp.setColor( QPalette::HighlightedText, Qt::black);
   m_ui->tableView->setPalette(temp);

   // highlight cell
   QModelIndex index = m_model->index(0, 1);
   m_ui->tableView->setCurrentIndex(index);

   // resize the dialog widget after the text has been displayed
   adjustSize();

   connect(m_ui->saveTags_PB,  &QPushButton::clicked, this, &Dialog_SavedFiles::saveTags);

   connect(m_ui->saveTabs_PB,  &QPushButton::clicked, this, &Dialog_SavedFiles::saveExtraFiles);
   connect(m_ui->loadTabs_PB,  &QPushButton::clicked, this, &Dialog_SavedFiles::loadExtraFiles);

   connect(m_ui->close_PB,     &QPushButton::clicked, this, &Dialog_SavedFiles::close);
}

Dialog_SavedFiles::~Dialog_SavedFiles()
{
   delete m_ui;
}

void Dialog_SavedFiles::close()
{
   done(QDialog::Accepted);
}

void Dialog_SavedFiles::loadExtraFiles()
{
   QModelIndex index = m_ui->tableView->currentIndex();

   if (! index.isValid() ) {
      return;
   }

   int row = index.row();

   QString jsonTag = m_fileIds[row];
   m_parent->loadTabs(jsonTag);

   done(QDialog::Accepted);
}

void Dialog_SavedFiles::saveExtraFiles()
{
    QModelIndex index = m_ui->tableView->currentIndex();

   if (! index.isValid() ) {
      return;
   }

   int row = index.row();

   QString jsonTag = m_fileIds[row];
   m_parent->saveTabs(jsonTag);

   done(QDialog::Accepted);
}

void Dialog_SavedFiles::saveTags()
{
   m_maxCount = m_model->rowCount();

   for (int row = 0; row < m_maxCount; ++row) {
      QStandardItem *item = m_model->item(row, 1);
      QString itemText = item->data(Qt::DisplayRole).toString();

      m_fileNames.replace(row, itemText);
   }

   m_parent->json_Save_FileNames(m_fileNames);

   csMsg("File Names Saved");
}

void Dialog_SavedFiles::setUpView()
{
   m_model = new QStandardItemModel(this);
   m_model->setColumnCount(2);
   m_model->setHeaderData(0, Qt::Horizontal, tr("List #"));
   m_model->setHeaderData(1, Qt::Horizontal, tr("List Name"));

   //
   m_ui->tableView->setModel(m_model);
   m_ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
   m_ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

   m_ui->tableView->setColumnWidth(0, 110);
   m_ui->tableView->setColumnWidth(1, 250);

   // resize the last column
   m_ui->tableView->horizontalHeader()->setStretchLastSection(true);
   QBrush brush = QColor(0, 0, 255);

   // add data
   m_maxCount = m_fileIds.size();

   for (int row = 0; row < m_maxCount; ++row) {
      QStandardItem *item1 = new QStandardItem(m_fileText.at(row));
      item1->setForeground(brush);
      item1->setEnabled(false);

      QStandardItem *item2 = new QStandardItem(m_fileNames.at(row));
      item2->setEditable(true);

      m_model->setItem(row, 0, item1);
      m_model->setItem(row, 1, item2);
   }

   m_ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
}
