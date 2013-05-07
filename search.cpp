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

#include "dialog_advfind.h"
#include "dialog_find.h"
#include "dialog_replace.h"
#include "mainwindow.h"
#include "search.h"

#include <QBoxLayout>
#include <QDir>
#include <QTextStream>
#include <QTableView>

// * find
void MainWindow::find()
{
   Dialog_Find *dw = new Dialog_Find(m_findText, m_findList);
   int result = dw->exec();

   if (result == QDialog::Accepted) {

      m_findText = dw->get_findText();
      m_findList = dw->get_findList();

      // add to list if not found
      int index = m_findList.indexOf(m_findText);

      if (index == -1) {
         m_findList.prepend(m_findText);
      } else {
         m_findList.move(index,0);
      }
      json_Write(FIND_REPLACE);

      // get the flags
      m_flags = 0;

      m_fDirection  = dw->get_Direction();
      if (! m_fDirection) {
         m_flags |= QTextDocument::FindBackward;
      }

      m_fCase = dw->get_Case();
      if (m_fCase) {
         m_flags |= QTextDocument::FindCaseSensitively;
      }

      m_fWholeWords = dw->get_WholeWords();
      if (m_fWholeWords){
         m_flags |= QTextDocument::FindWholeWords;
      }

      if (! m_findText.isEmpty())  {
         bool found = m_textEdit->find(m_findText, m_flags);

         if (! found)  {
            csError("Find", "Not found: " + m_findText);
         }
      }
   }

   delete dw;
}

void MainWindow::findNext()
{
   QTextDocument::FindFlags flags = QTextDocument::FindFlags(~QTextDocument::FindBackward & m_flags);
   bool found = m_textEdit->find(m_findText, flags);

   if (! found)  {
      csError("Find", "Not found: " + m_findText);
   }
}

void MainWindow::findPrevious()
{
   bool found = m_textEdit->find(m_findText, QTextDocument::FindBackward | m_flags );

   if (! found)  {
      csError("Find", "Not found: " + m_findText);
   }
}


// * advanced find
void MainWindow::advFind()
{
   Dialog_AdvFind *dw = new Dialog_AdvFind(m_advFindText, m_advFindFileType, m_advFindFolder);
   int result = dw->exec();

   if (result == QDialog::Accepted) {

      m_advFindText     = dw->get_findText();
      m_advFindFileType = dw->get_findType();
      m_advFindFolder   = dw->get_findFolder();

      json_Write(ADVFIND);

      // get the flags
      m_advFCase             = dw->get_Case();
      m_advFWholeWords       = dw->get_WholeWords();
      m_advFSearchSubFolders = dw->get_SearchSubFolders();

      if (! m_advFindText.isEmpty())  {

         if (m_advFindFileType.isEmpty()) {
            m_advFindFileType = "*";
         }

         if (m_advFindFolder.isEmpty()) {
            m_advFindFolder = QDir::currentPath();
         }

         //
         QList<advFindStruct> foundList = this->advFind_getResults();

         if (foundList.isEmpty())  {
            csError("Advanced Find", "Not found: " + m_advFindText);

         } else   {
            this->advFind_ShowFiles(foundList);

         }
      }
   }

   delete dw;
}

QList<advFindStruct> MainWindow::advFind_getResults()
{
   // part 1
   QStringList searchList;
   QDir currentDir;

   if (m_advFSearchSubFolders)  {
      this->findRecursive(m_advFindFolder);
      searchList = m_recursiveList;

   } else {
      currentDir = QDir(m_advFindFolder);
      searchList = currentDir.entryList(QStringList(m_advFindFileType), QDir::Files | QDir::NoSymLinks);
   }

/* not used
   QProgressDialog progressDialog(this);
   progressDialog.setCancelButtonText(tr("&Cancel"));
   progressDialog.setRange(0, files.size());
   progressDialog.setWindowTitle(tr("Find Files"));
*/

   // part 2
   QList<advFindStruct> foundList;
   QString name;

   enum Qt::CaseSensitivity caseFlag;

   if (m_advFCase) {
      caseFlag = Qt::CaseSensitive;

   }  else {
      // default
      caseFlag = Qt::CaseInsensitive;

   }

   // process each file
   for (int k = 0; k < searchList.size(); ++k) {

/*    not used
      progressDialog.setValue(k);
      progressDialog.setLabelText(tr("Searching file %1 of %2...").arg(k).arg(searchList.size()));
      qApp->processEvents();

      if (progressDialog.wasCanceled()) {
          break;
      }
*/
      if (m_advFSearchSubFolders)  {
         name = searchList[k];

      } else  {
         name = currentDir.absoluteFilePath(searchList[k]);

      }

      QFile file(name);
      QRegExp regexp = QRegExp("\\b" + m_advFindText + "\\b", caseFlag);

      if (file.open(QIODevice::ReadOnly)) {
         QString line;
         QTextStream in(&file);

         int lineNumber = 0;
         int position   = 0;

         while (! in.atEnd()) {

/*          not used
            if (progressDialog.wasCanceled())  {
                break;
            }
*/
            line = in.readLine();
            lineNumber++;

            if (m_advFWholeWords)  {
               position = line.indexOf(regexp, 0);

            } else  {
               position = line.indexOf(m_advFindText, 0, caseFlag);

            }

            if (position != -1)  {
               advFindStruct temp;

               temp.fileName   = name;
               temp.lineNumber = lineNumber;
               temp.text       = line.trimmed();

               foundList.append(temp);
            }
         }
      }

      file.close();
   }

   return foundList;
}

void MainWindow::findRecursive(const QString &path)
{
   QDir dir(path);
   dir.setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

   QFileInfoList list = dir.entryInfoList(QStringList(m_advFindFileType));

   if (! list.isEmpty()) {
      for (int k = 0; k != list.count(); ++k) {

          QString filePath = list[k].filePath();

         if ( list[k].isDir() )  {
            // recursive
            findRecursive(filePath);

         } else  {
            m_recursiveList.append(filePath);

         }
      }
   }
}

void MainWindow::advFind_ShowFiles(QList<advFindStruct> foundList)
{
   int index = m_splitter->indexOf(m_findWidget);

   if (index > 0) {
      m_findWidget->deleteLater();
   }

   // create the find window
   m_findWidget = new QFrame(this);
   m_findWidget->setFrameShape(QFrame::Panel);

   QTableView *view = new QTableView(this);

   m_model = new QStandardItemModel;
   m_model->setColumnCount(3);
   m_model->setHeaderData(0, Qt::Horizontal, tr("File Name"));
   m_model->setHeaderData(1, Qt::Horizontal, tr("Line #"));
   m_model->setHeaderData(2, Qt::Horizontal, tr("Text"));

   view->setModel(m_model);
   view->setSelectionMode(QAbstractItemView::SingleSelection);
   view->setSelectionBehavior(QAbstractItemView::SelectRows);
   view->setEditTriggers(QAbstractItemView::NoEditTriggers);

   view->setColumnWidth(0, 300);
   view->setColumnWidth(1, 75);
   view->horizontalHeader()->setStretchLastSection(true);

   // background color
   view->setAlternatingRowColors(true);
   view->setStyleSheet("alternate-background-color: lightyellow");

   int row = 0;

   for (auto index = foundList.begin(); index != foundList.end(); index++) {

      QStandardItem *item0  = new QStandardItem(index->fileName);
      QStandardItem *item1  = new QStandardItem(QString::number(index->lineNumber));
      QStandardItem *item2  = new QStandardItem(index->text);

      m_model->insertRow(row);
      m_model->setItem(row, 0, item0);
      m_model->setItem(row, 1, item1);
      m_model->setItem(row, 2, item2);

      row++;
   }

   view->resizeRowsToContents();

   //
   QPushButton *closeButton = new QPushButton();
   closeButton->setText("Close");

   QBoxLayout *buttonLayout = new QHBoxLayout();
   buttonLayout->addStretch();
   buttonLayout->addWidget(closeButton);
   buttonLayout->addStretch();

   QBoxLayout *layout = new QVBoxLayout();
   layout->addWidget(view);
   layout->addLayout(buttonLayout);

   m_findWidget->setLayout(layout);

   m_splitter->setOrientation(Qt::Vertical);
   m_splitter->addWidget(m_findWidget);

   // signal
   connect(view, SIGNAL(clicked(const QModelIndex &)),this, SLOT(advFind_View(const QModelIndex &)));
   connect(closeButton, SIGNAL(clicked()), this, SLOT(advFind_Close()));
}

void MainWindow::advFind_Close()
{
   m_findWidget->deleteLater();
}

void MainWindow::advFind_View(const QModelIndex &index)
{
   int row = index.row();

   if (row < 0) {
      return;
   }

   QString fileName = m_model->item(row,0)->data(Qt::DisplayRole).toString();
   int lineNumber   = m_model->item(row,1)->data(Qt::DisplayRole).toInt();

   // is the file already open?
   bool open = false;
   int max   = m_tabWidget->count();

   for (int index = 0; index < max; ++index) {
      QString tcurFile = this->get_curFileName(index);

      if (tcurFile == fileName) {
         m_tabWidget->setCurrentIndex(index);

         open = true;
         break;
      }
   }

   //
   if (! open) {
      open = loadFile(fileName, true, false);
   }

   if (open)   {
      QTextCursor cursor(m_textEdit->textCursor());
      cursor.movePosition(QTextCursor::Start);
      cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, lineNumber-1);
      m_textEdit->setTextCursor(cursor);
   }
}


// * replace
void MainWindow::replace()
{
   Dialog_Replace *dw = new Dialog_Replace(m_findText, m_findList, m_replaceText, m_replaceList);
   int result = dw->exec();

   if ( result >= QDialog::Accepted) {
      m_findText = dw->get_findText();
      m_findList = dw->get_findList();

      // add to list if not found
      int index = m_findList.indexOf(m_findText);

      if (index == -1) {
         m_findList.prepend(m_findText);
      } else {
         m_findList.move(index,0);
      }

      m_replaceText = dw->get_replaceText();
      m_replaceList = dw->get_replaceList();

      // add to list if not found
      index = m_findList.indexOf(m_replaceText);

      if (index == -1) {
         m_replaceList.prepend(m_replaceText);
      } else {
         m_replaceList.move(index,0);
      }
      json_Write(FIND_REPLACE);

      // get the flags
      m_flags = 0;

      m_fCase = dw->get_Case();
      if (m_fCase) {
         m_flags |= QTextDocument::FindCaseSensitively;
      }

      m_fWholeWords = dw->get_WholeWords();
      if (m_fWholeWords){
         m_flags |= QTextDocument::FindWholeWords;
      }  

      if (! m_findText.isEmpty() && ! m_replaceText.isEmpty())  {

         if (result == 1)   {
            replaceQuery();

         } else if (result == 2)  {
            replaceAll();

         }
      }
   }

   delete dw;
}

void MainWindow::replaceQuery()
{
   bool isFirst = true;
   bool found;

   // begin undo block
   QTextCursor cursor(m_textEdit->textCursor());
   cursor.beginEditBlock();

   ReplaceReply *dw = nullptr;

   while (true) {
      found = m_textEdit->find(m_findText, m_flags);

      if (found) {         

         if (isFirst) {
            isFirst = false;
            dw = new ReplaceReply(this);
         }

         // aling near text
         QRect rect  = m_textEdit->cursorRect();
         QPoint point = rect.bottomLeft();
         point.rx() += 25;
         point.ry() += 10;
         dw->move(m_textEdit->mapToGlobal(point));

         int result = dw->exec();

         if (result == QDialog::Rejected) {
            break;
         }

         int key = dw->getKey();

         if (key == Qt::Key_unknown) {            
            continue;

         }  else if (key == Qt::Key_A)  {
            replaceAll();

         } else if (key == Qt::Key_N)  {            
            continue;

         } else if (key == Qt::Key_O)  {
            cursor  = m_textEdit->textCursor();
            cursor.insertText(m_replaceText);

            break;

         } else if (key == Qt::Key_S)  {
            break;

         } else if (key == Qt::Key_Y)  {            
            cursor  = m_textEdit->textCursor();
            cursor.insertText(m_replaceText);

         }

      } else {
         break;

      }
   }

   delete dw;

   cursor.clearSelection();
   m_textEdit->setTextCursor(cursor);

   // end of undo
   cursor.endEditBlock();

   if (isFirst) {
      csError("Replace", "Not found: " + m_findText);
   }
}

void MainWindow::replaceAll()
{
   bool isFirst = true;
   bool found;

   // begin undo block
   QTextCursor cursor(m_textEdit->textCursor());
   cursor.beginEditBlock();

   while (true) {
      found = m_textEdit->find(m_findText, m_flags);

      if (found) {
         isFirst = false;

         cursor  = m_textEdit->textCursor();
         cursor.insertText(m_replaceText);

      } else {
         break;

      }
   }

   cursor.clearSelection();
   m_textEdit->setTextCursor(cursor);

   // end of undo
   cursor.endEditBlock();

   if (isFirst) {
      csError("Replace All", "Not found: " + m_findText);
   }
}


// *
ReplaceReply::ReplaceReply(MainWindow *parent)
   : QDialog(parent)
{
   QLabel *label_1= new QLabel();
   label_1->setText("Replace?");

   QFont font = label_1->font();
   font.setPointSize(10);
   label_1->setFont(font);

   QLabel *label_2= new QLabel();
   label_2->setText("Yes / No / All / Once / Stop");

   font = label_2->font();
   font.setPointSize(10);
   label_2->setFont(font);

   QBoxLayout *layout = new QVBoxLayout();
   layout->addWidget(label_1);
   layout->addWidget(label_2);

   setLayout(layout);

   //
   setWindowTitle("Search & Replace");

   QSize size = QSize(235, 60);
   resize(size);
}

ReplaceReply::~ReplaceReply()
{
}

void ReplaceReply::keyPressEvent(QKeyEvent *event)
{
   m_replaceReply = event->key();

   if (m_replaceReply == Qt::Key_Escape) {
      this->done(0);

   } else {
      this->done(1);

   }
}

int ReplaceReply::getKey()
{
   return m_replaceReply;
}
