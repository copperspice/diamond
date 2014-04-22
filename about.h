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

#ifndef ABOUT_H
#define ABOUT_H

#include <QWidget>
#include <QtWebKit>
#include <QVector>

class About : public QWidget
{
   CS_OBJECT(About)

   public:
      About(QString kind, QString file);

   private:
      QWebView *m_viewer;
      QString m_url;

      QVector<int> m_zoomLevels;
      int m_currentZoom;

   private :
      CS_SLOT_1(Private, void setCustomContextMenu(const QPoint & pos))
      CS_SLOT_2(setCustomContextMenu) 
      CS_SLOT_1(Private, void zoomIn())
      CS_SLOT_2(zoomIn) 
      CS_SLOT_1(Private, void zoomOut())
      CS_SLOT_2(zoomOut) 

      CS_SLOT_1(Private, void actionClose())
      CS_SLOT_2(actionClose) 
      CS_SLOT_1(Private, void actionHome())
      CS_SLOT_2(actionHome) 
};

#endif
