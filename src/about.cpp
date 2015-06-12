/**************************************************************************
*
* Copyright (c) 2012-2015 Barbara Geller
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

#include "about.h"
#include "util.h"

#include <stdexcept>

#include <QAction>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QPushButton>
#include <QString>
#include <QSize>
#include <QMenu>
#include <QVBoxLayout>
#include <QWebSettings>

About::About(QString route, QString file)
   : QWidget()
{   
   if (route == "Help") {
      m_url = file;

      QFileInfo temp(m_url);

      if (m_url.isEmpty())  {
         csError("Help", "No file was specified for Diamond Help.");
         throw std::runtime_error("");

      } else if (! temp.exists() )  {
         csError("Help", "Help file was not found:\n" +  m_url + "\n\n"
                 "To specify the location of the Diamond Help file, select 'Settings' from the main Menu. "
                 "Then select 'General Options' and click on the Options tab.\n");

         throw std::runtime_error("");
      }

      m_url = "file:///" + m_url;
      setWindowTitle("Diamond Help");

   } else {
      m_url = file;

      QFileInfo temp(m_url);

      if (m_url.isEmpty())  {
         csError("Html Viewer", "No file was specified.");
         throw std::runtime_error("");

      } else if (! temp.exists() )  {
         csError("Html Viewer", "Specified file does not exist\n" + m_url);
         throw std::runtime_error("");
      }

      m_url = "file:///" + m_url;
      setWindowTitle("Html Viewer - " + file);
   }

   //
   m_viewer = new QWebView;
   m_viewer->setUrl(m_url);

   setWindowIcon(QIcon("://resources/diamond.png"));

   m_currentZoom = 100;
   m_viewer->setZoomFactor(static_cast<qreal>(m_currentZoom)/100.0);

   QPushButton *homePB = nullptr;

   if (route == "Help") {
      homePB = new QPushButton;
      homePB->setText("Home");
   }

   QPushButton *closePB = new QPushButton;
   closePB->setText("Close");

   //
   QHBoxLayout *buttonLayout = new QHBoxLayout;
   buttonLayout->addStretch();

   if (route == "Help") {
      buttonLayout->addWidget(homePB);
      buttonLayout->addSpacing(10);
   }

   buttonLayout->addWidget(closePB);
   buttonLayout->addStretch();

   QVBoxLayout *mainLayout = new QVBoxLayout;
   mainLayout->addWidget(m_viewer);
   mainLayout->addSpacing(10);
   mainLayout->addLayout(buttonLayout);
   setLayout(mainLayout);

   mainLayout->setContentsMargins(0, 0, 0, 15);

   //
   m_zoomLevels << 30 << 50 << 67 << 80 << 90 << 100;
   m_zoomLevels << 110 << 120 << 133 << 150 << 170 << 200 << 240 << 300;

   // shortcuts
   QAction *action1 = new QAction(tr("Zoom In"), this);
   action1->setShortcuts(QKeySequence::ZoomIn);
   connect(action1, SIGNAL(triggered()), this, SLOT(zoomIn()));

   QAction *action2 = new QAction(tr("Zoom Out"), this );
   action2->setShortcuts(QKeySequence::ZoomOut);
   connect(action2, SIGNAL(triggered()), this, SLOT(zoomOut()));

   m_viewer->addAction(action1);
   m_viewer->addAction(action2);

   // set up custom context menu
   m_viewer->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(m_viewer, SIGNAL(customContextMenuRequested(const QPoint &)), this,
                 SLOT(setCustomContextMenu(const QPoint &)) );

   // signals   
   if (route == "Help") {
      connect(homePB,   SIGNAL(clicked()), this, SLOT(actionHome()) );
   }
   connect(closePB,  SIGNAL(clicked()), this, SLOT(actionClose()) );
}

void About::actionHome()
{
   m_viewer->setUrl(m_url);
}

void About::actionClose()
{
   this->close();
}

//  context menu
void About::setCustomContextMenu(const QPoint &pos)
{
   QMenu *menu = new QMenu(this);

   menu->addAction(m_viewer->pageAction(QWebPage::Copy));
   menu->addSeparator();

   menu->addAction(tr("Zoom In"),  this, SLOT(zoomIn()),  QKeySequence::ZoomIn  );
   menu->addAction(tr("Zoom Out"), this, SLOT(zoomOut()), QKeySequence::ZoomOut );

   menu->exec(mapToGlobal(pos));
}

QSize About::sizeHint() const
{
   return QSize(1050,650);
}

void About::zoomIn()
{
   int i = m_zoomLevels.indexOf(m_currentZoom);

   if (i < m_zoomLevels.count() - 1) {
      m_currentZoom = m_zoomLevels[i + 1];
   }

   m_viewer->setZoomFactor(static_cast<qreal>(m_currentZoom)/100.0);
}

void About::zoomOut()
{
   int i = m_zoomLevels.indexOf(m_currentZoom);

   if (i > 0)  {
      m_currentZoom = m_zoomLevels[i - 1];
   }

   m_viewer->setZoomFactor(static_cast<qreal>(m_currentZoom)/100.0);
}

