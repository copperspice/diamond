/**************************************************************************
*
* Copyright (c) 2012-2017 Barbara Geller
* All rights reserved.
*
* Diamond Editor is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License version 2
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

#include <QtWebKit>
#include <QVector>
#include <QWidget>

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

      QSize sizeHint() const;

      void setCustomContextMenu(const QPoint & pos);
      void home();
      void close();

      CS_SLOT_1(Private, void zoomIn())
      CS_SLOT_2(zoomIn)

      CS_SLOT_1(Private, void zoomOut())
      CS_SLOT_2(zoomOut)
};

#endif
