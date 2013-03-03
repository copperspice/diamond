/**********************************************************************
*
* Copyright (c) 2012 Barbara Geller
* Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
* * Redistributions of source code must retain the above copyright notice,
*   this list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer in the
*   documentation and/or other materials provided with the distribution.
*
* * Neither the name of the Nokia Corporation nor the names of its
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
***********************************************************************/

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
#include <QWebSettings>
#include <QVBoxLayout>

About::About(QString file)
   : QWidget()
{
   m_currentZoom = 100;
   m_url = file;

   QFileInfo temp(m_url);

   if (m_url.isEmpty())  {
      csError("Help", "No file was specified for Diamond Help.");
      throw std::runtime_error("");

   } else if (! temp.exists() )  {
      csError("Help", "Help file was not found:\n" +  m_url + "\n\n"
              "To specify the location of the Diamond Help file, select 'Settings' from the menu. "
              "Then select General Options and click on the Options tab.\n");

      throw std::runtime_error("");
   }

   m_url = "file:///" + m_url;
   setWindowTitle("Diamond Help");

   //
   m_viewer = new QWebView;   
   m_viewer->setUrl(m_url);

   m_viewer->setZoomFactor(static_cast<qreal>(m_currentZoom)/100.0);

   //
   QPushButton *homePB  = new QPushButton;
   homePB->setText("Home");

   QPushButton *closePB = new QPushButton;
   closePB->setText("Close");

   //
   QHBoxLayout *buttonLayout = new QHBoxLayout;
   buttonLayout->addStretch();
   buttonLayout->addWidget(homePB);
   buttonLayout->addSpacing(10);
   buttonLayout->addWidget(closePB);
   buttonLayout->addStretch();

   QVBoxLayout *mainLayout = new QVBoxLayout;
   mainLayout->addWidget(m_viewer);
   mainLayout->addSpacing(10);
   mainLayout->addLayout(buttonLayout);
   setLayout(mainLayout);

   mainLayout->setContentsMargins(0, 0, 0, 15);

   QSize size = QSize(1050, 650);
   resize(size);

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
   connect(m_viewer, SIGNAL(customContextMenuRequested(const QPoint)), this,
                 SLOT(setCustomContextMenu(const QPoint)) );

   // signals
   connect(homePB,   SIGNAL(clicked()), this, SLOT(actionHome()) );
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






