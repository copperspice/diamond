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
      void keyPressEvent(QKeyEvent *event);

   private:
      int m_replaceReply;
};

#endif
