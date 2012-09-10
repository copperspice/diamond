/***************************************************************************
**
** Copyright (C) 2012 Barbara Geller
** All rights reserved.
**
**
** GNU Free Documentation License
** This file may be used under the terms of the GNU Free Documentation
** License version 1.3 as published by the Free Software Foundation
** and appearing in the file included in the packaging of this file.
**
**
****************************************************************************/


#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   app.setOrganizationName("BG Consulting");
   app.setApplicationName("Diamond Editor");

   MainWindow dw;
   dw.show();

   return app.exec();
}
