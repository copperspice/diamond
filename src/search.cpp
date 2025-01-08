/**************************************************************************
*
* Copyright (c) 2012-2025 Barbara Geller
*
* Diamond Editor is free software. You can redistribute it and/or
* modify it under the terms of the GNU General Public License
* version 2 as published by the Free Software Foundation.
*
* Diamond Editor is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* https://www.gnu.org/licenses/
*
***************************************************************************/

#include "dialog_advfind.h"
#include "dialog_find.h"
#include "dialog_replace.h"
#include "mainwindow.h"
#include "search.h"

#include <QBoxLayout>
#include <QDir>
#include <QMessageBox>
#include <QProgressDialog>
#include <QTableView>
#include <QTextStream>

// * find
void MainWindow::find()
{
   QString saveText = m_findText;

   QTextCursor cursor(m_textEdit->textCursor());
   QString selectedText = cursor.selectedText();

   if (! selectedText.isEmpty())  {
      m_findText = selectedText;
   }

   Dialog_Find *dw = new Dialog_Find(this, m_findText, m_findList);
   int result = dw->exec();

   if (result == QDialog::Accepted) {

      m_findText = dw->get_findText();
      m_findList = dw->get_findList();

      // add to combo list if not already there
      int index = m_findList.indexOf(m_findText);

      if (index == -1) {
         m_findList.prepend(m_findText);
      } else {
         m_findList.move(index,0);
      }

      json_Write(FIND_LIST, CFG_OVERRIDE);

      // get the flags
      m_flags = nullptr;

      m_fDirection  = dw->get_Direction();
      if (! m_fDirection) {
         m_flags |= QTextDocument::FindBackward;
      }

      m_fMatchCase = dw->get_MatchCase();
      if (m_fMatchCase) {
         m_flags |= QTextDocument::FindCaseSensitively;
      }

      m_fWholeWords = dw->get_WholeWords();
      if (m_fWholeWords){
         m_flags |= QTextDocument::FindWholeWords;
      }

      if (! m_findText.isEmpty())  {
         bool found = m_textEdit->find(m_findText, m_flags);

         if (! found)  {
            // text not found, query if the user wants to search from top of file
            findNext();
         }
      }

   } else {
      m_findText = saveText;

      bool upd_Find = dw->get_Upd_Find();

      if (upd_Find)   {
         m_findList = dw->get_findList();
         json_Write(FIND_LIST, CFG_OVERRIDE);
      }
   }

   delete dw;
}

void MainWindow::findNext()
{
   // emerald - may want to modify m_FindText when text contains html

   QTextDocument::FindFlags flags = QTextDocument::FindFlags(~QTextDocument::FindBackward & m_flags);
   bool found = m_textEdit->find(m_findText, flags);

   if (! found)  {
      QString msg = "Not found: " + m_findText + "\n\n";
      msg += "Search from the beginning of this document?\n";

      QMessageBox msgFindNext(this);
      msgFindNext.setWindowTitle("Find");
      msgFindNext.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
      msgFindNext.setDefaultButton(QMessageBox::Yes);
      msgFindNext.setText(msg);

      int result = msgFindNext.exec();

      if (result == QMessageBox::Yes) {
         // reset to the beginning of the document
         QTextCursor cursor(m_textEdit->textCursor());
         cursor.movePosition(QTextCursor::Start);
         m_textEdit->setTextCursor(cursor);

         // search again
         findNext();
      }
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
   QString saveText = m_advFindText;

   QTextCursor cursor(m_textEdit->textCursor());
   QString selectedText = cursor.selectedText();

   if (! selectedText.isEmpty())  {
      m_advFindText = selectedText;
   }

   m_dwAdvFind = new Dialog_AdvFind(this, m_advFindText, m_advFindFileType, m_advFindFolder, m_advFSearchFolders,
         m_advFMatchCase, m_advFWholeWords, m_advFRegexp);

   while (true) {
      int result = m_dwAdvFind->exec();

      if (result == QDialog::Accepted) {

         m_advFindText       = m_dwAdvFind->get_findText();
         m_advFindFileType   = m_dwAdvFind->get_findType();
         m_advFindFolder     = m_dwAdvFind->get_findFolder();

         // get the flags
         m_advFMatchCase     = m_dwAdvFind->get_MatchCase();
         m_advFWholeWords    = m_dwAdvFind->get_WholeWords();
         m_advFRegexp        = m_dwAdvFind->get_Regexp();
         m_advFSearchFolders = m_dwAdvFind->get_SearchSubFolders();

         json_Write(ADVFIND);

         if (! m_advFindText.isEmpty())  {

            if (m_advFindFileType.isEmpty()) {
               m_advFindFileType = "*";
            }

            if (m_advFindFolder.isEmpty()) {
               m_advFindFolder = QDir::currentPath();
            }

            //
            bool aborted = false;
            QList<advFindStruct> foundList = advFind_getResults(aborted);

            if (aborted)  {
               // do nothing

            } else if (foundList.isEmpty())  {
               csError("Advanced Find", "Not found: " + m_advFindText);

               // allow user to search again
               m_dwAdvFind->showNotBusyMsg();
               continue;

            } else   {
               advFind_ShowFiles(foundList);

            }
         }

      } else {
         m_advFindText = saveText;

      }

      // exit while loop
      break;
   }

   delete m_dwAdvFind;
}

QList<advFindStruct> MainWindow::advFind_getResults(bool &aborted)
{
   aborted = false;

   // part 1
   QStringList searchList;
   QDir currentDir;

   if (m_advFSearchFolders)  {
      m_recursiveList.clear();

      findRecursive(m_advFindFolder);
      searchList = m_recursiveList;

   } else {
      currentDir = QDir(m_advFindFolder);
      searchList = currentDir.entryList(QStringList(m_advFindFileType), QDir::Files | QDir::NoSymLinks);
   }

   QProgressDialog progressDialog(this);

   progressDialog.setMinimumDuration(1500);
   progressDialog.setMinimumWidth(275);
   progressDialog.setRange(0, searchList.size());
   progressDialog.setWindowTitle(tr("Advanced File Search"));

   progressDialog.setCancelButtonText(tr("&Cancel"));
   progressDialog.setCancelButtonCentered(true);

   QLabel *label = new QLabel;

   QFont font = label->font();
   font.setPointSize(11);

   label->setFont(font);
   label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
   progressDialog.setLabel(label);

   // part 2
   QList<advFindStruct> foundList;
   QString name;

   enum Qt::CaseSensitivity caseFlag;

   QRegularExpression regExp;

   if (m_advFWholeWords) {
      regExp = QRegularExpression("\\b" + m_advFindText + "\\b");

   } else if (m_advFRegexp) {
      regExp = QRegularExpression(m_advFindText);

   }

   if (m_advFMatchCase) {
      caseFlag = Qt::CaseSensitive;

   }  else {
      caseFlag = Qt::CaseInsensitive;
      regExp.setPatternOptions(QPatternOption::CaseInsensitiveOption);

   }

   // process each file
   for (int k = 0; k < searchList.size(); ++k) {

      progressDialog.setValue(k);
      progressDialog.setLabelText(tr("Searching file %1 of %2").formatArg(k).formatArg(searchList.size()));
      qApp->processEvents();

      if (progressDialog.wasCanceled()) {
         aborted = true;
         break;
      }

      if (m_advFSearchFolders)  {
         name = searchList[k];

      } else  {
         name = currentDir.absoluteFilePath(searchList[k]);

      }

#if defined (Q_OS_WIN)
      // change forward to backslash
      name.replace('/', '\\');
#endif

      QFile file(name);

      if (file.open(QIODevice::ReadOnly)) {
         QString line;
         QTextStream in(&file);

         int lineNumber = 0;
         int position   = 0;

         while (! in.atEnd()) {

            line = in.readLine();
            ++lineNumber;

            if (m_advFWholeWords || m_advFRegexp)  {
               position = line.indexOf(regExp);

            } else  {
               position = line.indexOf(m_advFindText, 0, caseFlag);

            }

            // store the results
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

void MainWindow::findRecursive(const QString &path, bool isFirstLoop)
{
   QDir dir(path);
   dir.setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

   QFileInfoList list = dir.entryInfoList(QStringList(m_advFindFileType));
   int cnt = list.count();

   if (isFirstLoop && cnt > 0) {
      m_dwAdvFind->showBusyMsg();
   }

   if (! list.isEmpty()) {
      for (int k = 0; k != cnt; ++k) {

          QString filePath = list[k].filePath();

         if ( list[k].isDir() )  {
            // recursive
            findRecursive(filePath, false);

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

   // use main window font and size, add feature to allow user to change font
   // following code out for now since the font was too large

//  QFont font = view->font();
//  font.setPointSize(12);
//  view->setFont(font);

   // background color
   view->setAlternatingRowColors(true);
   view->setStyleSheet("alternate-background-color: lightyellow");

   int row = 0;

   for (const auto &entry : foundList) {

      QStandardItem *item0  = new QStandardItem(entry.fileName);
      QStandardItem *item1  = new QStandardItem(QString::number(entry.lineNumber));
      QStandardItem *item2  = new QStandardItem(entry.text);

      if (entry.fileName.endsWith(".wpd")) {
         item2->setText("** WordPerfect file, text format incompatible");
      }

      m_model->insertRow(row);
      m_model->setItem(row, 0, item0);
      m_model->setItem(row, 1, item1);
      m_model->setItem(row, 2, item2);

      ++row;
   }

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

   // must call after addWidget
   view->resizeRowsToContents();

   connect(view,        &QTableView::clicked,  this, &MainWindow::advFind_View);
   connect(closeButton, &QPushButton::clicked, this, &MainWindow::advFind_Close);
}

void MainWindow::advFind_Close()
{
   m_findWidget->deleteLater();
}

void MainWindow::advFind_View(const QModelIndex &modelIndex)
{
   int row = modelIndex.row();

   if (row < 0) {
      return;
   }

   QString fileName = m_model->item(row,0)->data(Qt::DisplayRole).toString();
   int lineNumber   = m_model->item(row,1)->data(Qt::DisplayRole).toInt();

   if (fileName.endsWith(".wpd")) {
      csError("Open File", "WordPerfect file, text format incompatible with Diamond");
      return;
   }

   // is the file already open?
   bool open = false;
   int max   = m_tabWidget->count();

   for (int index = 0; index < max; ++index) {
      QString tcurFile = get_curFileName(index);

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
   QString saveText = m_findText;

   QTextCursor cursor(m_textEdit->textCursor());
   QString selectedText = cursor.selectedText();

   if (! selectedText.isEmpty())  {
      m_findText = selectedText;
   }

   Dialog_Replace *dw = new Dialog_Replace(this, m_findText, m_findList, m_replaceText, m_replaceList);
   int result = dw->exec();

   if (result >= QDialog::Accepted) {
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
      index = m_replaceList.indexOf(m_replaceText);

      if (index == -1) {
         m_replaceList.prepend(m_replaceText);

         json_Write(FIND_REPLACE);

      } else {
         m_replaceList.move(index,0);

      }

      // get the flags
      m_flags = nullptr;

      m_fMatchCase = dw->get_MatchCase();
      if (m_fMatchCase) {
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

   } else {
      m_findText = saveText;

      bool upd_Find    = dw->get_Upd_Find();
      bool upd_Replace = dw->get_Upd_Replace();

      if (upd_Find && ! upd_Replace)   {
         m_findList = dw->get_findList();

         json_Write(FIND_LIST);

      } else if (upd_Replace)   {
         m_findList    = dw->get_findList();
         m_replaceList = dw->get_replaceList();

         json_Write(FIND_REPLACE);
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
      done(0);

   } else {
      done(1);

   }
}

int ReplaceReply::getKey()
{
   return m_replaceReply;
}
