#include "mainwindow.h"
#include "util.h"

#include <QtGui>


void MainWindow::readSettings()
{
   QSettings settings("BG Consulting", "Diamond Editor");
   QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
   QSize size = settings.value("size", QSize(400, 400)).toSize();
   resize(size);
   move(pos);
}

void MainWindow::writeSettings()
{
   QSettings settings("BG Consulting", "Diamond Editor");
   settings.setValue("pos", pos());
   settings.setValue("size", size());
}
