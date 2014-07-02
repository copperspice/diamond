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

#include <QPushButton>
#include <QDialog>
#include <QLabel>
#include <QTableView>
#include <QSize>

Dialog_Macro::Dialog_Macro(MainWindow *from, MacroEnum enumValue, QStringList macroIds, QStringList macroNames )
   : m_ui(new Ui::Dialog_Macro)
{
   m_parent       = from;
   m_enum         = enumValue;
   m_macroIds_D   = macroIds;
   m_macroNames_D = macroNames;

   m_ui->setupUi(this);
   this->setWindowIcon(QIcon("://resources/diamond.png"));

   setupTitle();
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

   connect(m_ui->select_PB, SIGNAL(clicked()),this, SLOT(Select()));
   connect(m_ui->view_PB,   SIGNAL(clicked()),this, SLOT(View()));
   connect(m_ui->cancel_PB, SIGNAL(clicked()),this, SLOT(Cancel()));
}

Dialog_Macro::~Dialog_Macro()
{
   delete m_ui;
}

void Dialog_Macro::setupTitle()
{   
   if (m_enum == MACRO_LOAD) {
      setWindowTitle("Load Macro");

   } else if (m_enum == MACRO_SAVE)  {
      m_ui->select_PB->setText("Save");
      setWindowTitle("Save Macro");    

   } else if (m_enum == MACRO_EDITNAMES) {
      m_ui->select_PB->setDisabled(true);
      setWindowTitle("Edit Macro Names");

   } 
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
   QBrush brush = QColor(0,0,255);    

   // add data
   m_maxCount = m_macroIds_D.size();
   for (int row = 0; row < m_maxCount; ++row) {

      QStandardItem *item1 = new QStandardItem(m_macroIds_D.at(row));
      item1->setForeground(brush);
      item1->setEnabled(false);

      QStandardItem *item2 = new QStandardItem(m_macroNames_D.at(row));
      item2->setEditable(true);

      m_model->setItem(row, 0, item1);
      m_model->setItem(row, 1, item2);
   }

   // initial sort
   m_ui->tableView->sortByColumn(0, Qt::AscendingOrder);
   m_ui->tableView->setSortingEnabled(true);

   m_ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);

   // signal
   connect(m_model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(tableDataChanged(const QModelIndex &, const QModelIndex &)));
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

         m_macroNames_D.replace(row, data);
      }

      m_parent->json_Save_MacroNames(m_macroNames_D);
   }

   this->done(QDialog::Accepted);
}

void Dialog_Macro::Cancel()
{
   this->done(QDialog::Rejected);
}

//
void Dialog_Macro::View()
{
   QString macro = this->get_Macro();

   // get the macro
   QList<macroStruct> data;
   data = m_parent->json_View_Macro(macro);

   QString msg = "<table style=margin-right:35>";

   for (int k = 0; k < data.size(); ++k) {

      int key         = data.at(k).key;
      int modifier    = data.at(k).modifier;
      QString textAsc = data.at(k).text;

      msg += "<tr><td width=150>Modifier: &nbsp;";

      switch (modifier)  {

         case 0:
            msg += " ";
            break;

         case Qt::SHIFT:
            msg += "Shift";
            break;

         case Qt::CTRL:
            msg += "Control";
            break;

         case Qt::META:
            msg += "Meta";
            break;

         case Qt::ALT:
            msg += "Alt";
            break;

         case Qt::KeypadModifier:
            msg += "Key-Pad";
            break;

         case Qt::GroupSwitchModifier:
            msg += "Group-Switch";
            break;

         default:
            msg += "(modifier)" + QString::number(modifier);
      }

      msg += "</td> <td width=150>Key: &nbsp;";

      // part 2
      bool isKey = false;

      switch (key)  {

         case Qt::Key_Tab:
            msg += "Tab";
            isKey = true;
            break;

         case Qt::Key_Backtab:
            msg += "Back-Tab";
            isKey = true;
            break;

         case Qt::Key_Backspace:
            msg += "Backspace";
            isKey = true;
            break;

         case Qt::Key_Return:
         case Qt::Key_Enter:
            msg += "Return";
            isKey = true;
            break;

         case Qt::Key_Insert:
            msg += "Insert";
            isKey = true;
            break;

         case Qt::Key_Delete:
            msg += "Delete";
            isKey = true;
            break;

         case Qt::Key_Shift:
            msg += " ";
            isKey = true;
            break;

         case Qt::Key_Space:
            msg += "Space";
            isKey = true;
            break;

         case Qt::Key_Home:
            msg += "Home";
            isKey = true;
            break;

         case Qt::Key_End:
            msg += "End";
            isKey = true;
            break;

         case Qt::Key_Left:
            msg += "Left";
            isKey = true;
            break;

         case Qt::Key_Right:
            msg += "Right";
            isKey = true;
            break;

         case Qt::Key_Up:
            msg += "Up";
            isKey = true;
            break;

         case Qt::Key_Down:
            msg += "Down";
            isKey = true;
            break;

         case Qt::Key_PageUp:
            msg += "Page-Up";
            isKey = true;
            break;

         case Qt::Key_PageDown:
            msg += "Page-Down";
            isKey = true;
            break;

         default:
            if (textAsc.isEmpty()) {
               msg += QString::number(key);
               isKey = true;
            }
            break;
      }

      // part 3
      if (! isKey) {
         int pos = msg.size();
         msg = msg.left(pos-11) + "Text: &nbsp;" + textAsc;
      }

      msg += "</td></tr>";
   }

   // blank line
   msg += "<tr></tr>";

   QDialog msgB;
   msgB.setWindowIcon(QIcon("://resources/diamond.png"));
   msgB.setWindowTitle("View Macro - " + macro);      
   msgB.setMinimumWidth(120);

   //
   QLabel *label = new QLabel;

   QFont font = label->font();
   font.setPointSize(10);
   label->setFont(font);

   label->setText(msg);

   //
   QPushButton *button = new QPushButton();

   font = button->font();
   font.setPointSize(10);
   button->setFont(font);

   button->setText("Ok");

   QHBoxLayout *layoutButton = new QHBoxLayout();
   layoutButton->addStretch();
   layoutButton->addWidget(button);
   layoutButton->addStretch();

   QVBoxLayout *layoutMain = new QVBoxLayout();
   layoutMain->addWidget(label);
   layoutMain->addLayout(layoutButton);

   msgB.setLayout(layoutMain);

   QObject::connect(button, SIGNAL(clicked()), &msgB, SLOT(accept()));

   msgB.exec();
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

