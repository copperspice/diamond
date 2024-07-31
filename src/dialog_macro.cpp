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

#include "dialog_macro.h"
#include "util.h"

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QSize>
#include <QTableView>

Dialog_Macro::Dialog_Macro(MainWindow *parent, MainWindow::Option enumValue, QStringList macroIds,
      QStringList macroText, QStringList macroNames)
   : QDialog(parent), m_ui(new Ui::Dialog_Macro)
{
   m_parent     = parent;
   m_enum       = enumValue;
   m_macroIds   = macroIds;
   m_macroText  = macroText;
   m_macroNames = macroNames;

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

   connect(m_ui->saveTags_PB,  &QPushButton::clicked, this, &Dialog_Macro::saveTags);

   connect(m_ui->saveMacro_PB, &QPushButton::clicked, this, &Dialog_Macro::saveMacro);
   connect(m_ui->loadMacro_PB, &QPushButton::clicked, this, &Dialog_Macro::loadMacro);
   connect(m_ui->viewMacro_PB, &QPushButton::clicked, this, &Dialog_Macro::viewMacro);

   connect(m_ui->close_PB,     &QPushButton::clicked, this, &Dialog_Macro::close);
}

Dialog_Macro::~Dialog_Macro()
{
   delete m_ui;
}

void Dialog_Macro::close()
{
   done(QDialog::Rejected);
}

void Dialog_Macro::loadMacro()
{
   QString macroId = get_MacroId();
   m_parent->json_Load_Macro(macroId);

   done(QDialog::Accepted);
}

void Dialog_Macro::saveMacro()
{
   csMsg("Not implemented");

//  m_parent->json_Save_Macro();
//  done(QDialog::Accepted);

}

QString Dialog_Macro::get_MacroId()
{
   QModelIndex index = m_ui->tableView->currentIndex();

   if (! index.isValid()) {
      return QString();
   }

   int row = index.row();
   QString macroId = m_macroIds[row];

   return macroId;
}

QString Dialog_Macro::get_MacroName()
{
   QModelIndex index = m_ui->tableView->currentIndex();

   if (! index.isValid()) {
      return QString();
   }

   QStandardItem *item = m_model->item(index.row(), 1);
   QString macroName = item->data(Qt::DisplayRole).toString();

   return macroName;
}

void Dialog_Macro::saveTags()
{
   for (int row = 0; row < m_maxCount; ++row) {
      QStandardItem *item = m_model->item(row, 1);
      QString itemText = item->data(Qt::DisplayRole).toString();

      m_macroNames.replace(row, itemText);
   }

   m_parent->json_Save_MacroNames(m_macroNames);

   csMsg("Macro Names Saved");
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
   QBrush brush = QColor(0, 0, 255);

   // add data
   m_maxCount = m_macroIds.size();

   for (int row = 0; row < m_maxCount; ++row) {
      QStandardItem *item1 = new QStandardItem(m_macroText.at(row));
      item1->setForeground(brush);
      item1->setEnabled(false);

      QStandardItem *item2 = new QStandardItem(m_macroNames.at(row));
      item2->setEditable(true);

      m_model->setItem(row, 0, item1);
      m_model->setItem(row, 1, item2);
   }

   m_ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
}

void Dialog_Macro::viewMacro()
{
   QString macroId = get_MacroId();

   // get the macro details
   QList<macroStruct> listData = m_parent->json_View_Macro(macroId);

   QString msg = "<table style=margin-right:35>";

   for (int k = 0; k < listData.size(); ++k) {

      int key         = listData.at(k).key;
      int modifier    = listData.at(k).modifier;
      QString textAsc = listData.at(k).text;

      msg += "<tr><td width=150>Modifier: &nbsp;";

      switch (modifier)  {

         case 0:
            msg += " ";
            break;

         case Qt::ShiftModifier:
            msg += "Shift";
            break;

         case Qt::ControlModifier:
            msg += "Control";
            break;

         case Qt::MetaModifier:
            msg += "Meta";
            break;

         case Qt::AltModifier:
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
   msgB.setWindowTitle("View Macro - " + get_MacroName());
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

   QObject::connect(button, &QPushButton::clicked, &msgB, &QDialog::accept);

   msgB.exec();
}
