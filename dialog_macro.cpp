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

#include "dialog_macro.h"
#include "util.h"

#include <QTableView>
#include <QSize>

Dialog_Macro::Dialog_Macro(MainWindow *from, MacroEnum enumValue, QStringList macroIds, QStringList macroNames )
   : m_ui(new Ui::Dialog_Macro)
{
   m_parent     = from;
   m_enum       = enumValue;
   m_macroIds   = macroIds;
   m_macroNames = macroNames;

   m_ui->setupUi(this);
   this->setWindowIcon(QIcon("://resources/diamond.png"));

   initData();
   setUpView();

   // alter the hightlight color
   QPalette temp = m_ui->tableView->palette();
   temp.setColor( QPalette::Highlight, QColor(206, 206, 206, 255));
   temp.setColor( QPalette::HighlightedText, Qt::black);
   m_ui->tableView->setPalette(temp);

   // highlight cell
   QModelIndex index = m_model->index(0, 0);
   m_ui->tableView->setCurrentIndex(index);

   // resize the dialog widget after the text has been displayed
   adjustSize();

   connect(m_ui->select_PB, SIGNAL(clicked()),this, SLOT(Select()));
   connect(m_ui->view_PB,   SIGNAL(clicked()),this, SLOT(View()));
   connect(m_ui->cancel_PB, SIGNAL(clicked()),this, SLOT(Cancel()));
}

Dialog_Macro::~Dialog_Macro()
{
   delete m_ui;
}

void Dialog_Macro::initData()
{   
   QString msg1 = "Edit the text in the 'Macro Name' column to change the macro name.\n\n"
                  "'View Macro' will display the contents of the highlighted macro.";

   QString msg2 = "";

   if (m_enum == MACRO_LOAD) {
      setWindowTitle("Load Macro");
      msg2 += "Highlight a macro name, then press 'Select' to load the Macro for playback.";

   } else if (m_enum == MACRO_SAVE)  {
      m_ui->select_PB->setText("Save");
      setWindowTitle("Save Macro");

      msg2 += "All macros are being used.\n"
              "Highlight a macro name, then press 'Save' to overwrite the existing macro.\n\n"
              "Pressing 'Cancel' will not save the macro for future loading.\n"
              "This macro can be run until a new macro is recoreded or loaded.";

   } else if (m_enum == MACRO_EDITNAMES) {
      m_ui->select_PB->setDisabled(true);
      setWindowTitle("Edit Macro Names");

   }

   m_ui->msg1->setText(msg1);
   m_ui->msg2->setText(msg2);
}

void Dialog_Macro::setUpView()
{
   m_model = new QStandardItemModel(this);
   m_model->setColumnCount(2);
   m_model->setHeaderData(0, Qt::Horizontal, tr("Macro #"));
   m_model->setHeaderData(1, Qt::Horizontal, tr("Macro Name"));

   //
   m_ui->tableView->setModel(m_model);
   m_ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);   
   m_ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

   m_ui->tableView->setColumnWidth(0, 95);
   m_ui->tableView->setColumnWidth(1, 250);

   // resize the last column
   m_ui->tableView->horizontalHeader()->setStretchLastSection(true); 

   // add data
   m_maxCount = m_macroIds.size();

   for (int row = 0; row < m_maxCount; ++row) {

      QStandardItem *item1 = new QStandardItem(m_macroIds.at(row));
      QStandardItem *item2 = new QStandardItem(m_macroNames.at(row));

      m_model->setItem(row, 0, item1);
      m_model->setItem(row, 1, item2);
   }

   // initial sort
   m_ui->tableView->sortByColumn(0, Qt::AscendingOrder);
   m_ui->tableView->setSortingEnabled(true);

   // signal
   connect(m_ui->tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(tableClicked(const QModelIndex &)));
   connect(m_model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(tableDataChanged(const QModelIndex &, const QModelIndex &)));
}

void Dialog_Macro::tableClicked(const QModelIndex & index)
{
   if (index.column() == 1) {
      // edit macro name
      m_ui->tableView->edit(index);
   }
}

void Dialog_Macro::tableDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight)
{  
   m_updateNames = true;
}

void Dialog_Macro::Select()
{   
   if (m_updateNames) {

      for (int row = 0; row < m_maxCount; ++row) {

         QStandardItem *item = m_model->item(row, 1);
         QString data = item->data(Qt::DisplayRole).toString();

         m_macroNames.replace(row, data);
      }

      m_parent->json_Save_MacroNames(m_macroNames);
   }

   this->done(QDialog::Accepted);
}

void Dialog_Macro::View()
{
   QString macroId = this->get_Macro();

   // get the macro
   QList<macroStruct> data;
   data = m_parent->json_View_Macro(macroId);

   QString msg;

   for (int k = 0; k < data.size(); ++k) {
      msg += "Key:" + QString::number(data.at(k).key) +
             "  Modifier:" + QString::number(data.at(k).modifier);


      QString textAsc = data.at(k).text;
      // volatile auto foo = textAsc.toLatin1().data();

      if (textAsc.isEmpty()) {
         msg += + "        \n";

         } else {
         msg += "  Text:" + textAsc + "        \n";
      }
   }

   //
   QMessageBox msgB;
   msgB.setWindowTitle("View Macro - " + macroId);
   msgB.setText(msg);
   msgB.exec();
}

void Dialog_Macro::Cancel()
{
   this->done(QDialog::Rejected);
}

QString Dialog_Macro::get_Macro()
{
   QModelIndex index = m_ui->tableView->currentIndex();

   if (! index.isValid()) {
      return "";
   }

   // return the macro id
   QStandardItem *item = m_model->item(index.row(), 0);
   QString data = item->data(Qt::DisplayRole).toString();

   return data;
}


