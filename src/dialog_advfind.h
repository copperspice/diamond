/**************************************************************************
*
* Copyright (c) 2012-2019 Barbara Geller
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

#ifndef DIALOG_ADVFIND_H
#define DIALOG_ADVFIND_H

#include "ui_dialog_advfind.h"
#include "mainwindow.h"

#include <QDialog>
#include <QString>
#include <QStringList>

class Dialog_AdvFind : public QDialog
{     
   CS_OBJECT(Dialog_AdvFind)

   public:
      Dialog_AdvFind(MainWindow *parent, QString text, QString fileType, QString findFolder, bool searchFolders);
      ~Dialog_AdvFind();

      QString get_findText();      
      QString get_findType();
      QString get_findFolder();

      bool get_Case();
      bool get_WholeWords();
      bool get_SearchSubFolders();
      void showBusyMsg();
      void showNotBusyMsg();

   private:
      Ui::Dialog_AdvFind *m_ui;
      MainWindow *m_parent;
      QLabel *m_busyMsg;

      static QStringList dirCombo;

      void pick_Folder();
      void find();
      void cancel();
};

#endif
