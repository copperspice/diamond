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

#ifndef SEARCH_H
#define SEARCH_H

#include <QDialog>

class MainWindow;

class ReplaceReply: public QDialog
{
   CS_OBJECT(ReplaceReply)

   public:
      ReplaceReply(MainWindow *parent);
      ~ReplaceReply();

      int getKey();

   protected:
      void keyPressEvent(QKeyEvent *event) override;

   private:
      int m_replaceReply;
};

#endif
