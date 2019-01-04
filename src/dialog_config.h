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

#ifndef DIALOG_CONFIG_H
#define DIALOG_CONFIG_H

#include "ui_dialog_config.h"

#include <QDialog>
#include <QString>

class Dialog_Config : public QDialog
{     
   CS_OBJECT(Dialog_Config)

   public:
      Dialog_Config(QString fileName);
      ~Dialog_Config();
      QString get_newName();

   private:
      Ui::Dialog_Config *m_ui;

      void createNew();
      void select();
      void rename();
      void cancel();
};

#endif
