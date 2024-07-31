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

#ifndef DIALOG_SAVEDFILES_H
#define DIALOG_SAVEDFILES_H

#include "mainwindow.h"
#include "ui_dialog_savedfiles.h"

#include <QDialog>
#include <QStandardItemModel>
#include <QStringList>

class Dialog_SavedFiles : public QDialog
{
   CS_OBJECT(Dialog_SavedFiles)

   public:
      Dialog_SavedFiles(MainWindow *parent, QStringList fileIds, QStringList fileText, QStringList fileNames);
      ~Dialog_SavedFiles();

   private:
      void setUpView();

      void close();
      void loadExtraFiles();
      void saveExtraFiles();
      void saveTags();

      Ui::Dialog_SavedFiles *m_ui;
      MainWindow *m_parent;
      QStandardItemModel *m_model;

      QStringList m_fileIds;
      QStringList m_fileText;
      QStringList m_fileNames;
      int m_maxCount;
};

#endif

