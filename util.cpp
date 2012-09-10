#include "util.h"

void csMsg(const QString &msg)
{
   QMessageBox msgB;

   msgB.setWindowTitle("Debug Information");
   msgB.setText(msg);
   msgB.exec();
}

void csMsg(QString msg, int value)
{
   msg = msg + "   " + QString::number(value);

   //
   QMessageBox msgB;

   msgB.setWindowTitle("Debug Information");
   msgB.setText(msg);
   msgB.exec();
}

void csMsg(QWidget *parent, const QString &title, const QString &msg)
{
   QMessageBox msgB;

   if (parent) {
      msgB.setWindowModality(Qt::WindowModal);
   }

   msgB.setWindowTitle(title);
   msgB.setText(msg);
   msgB.exec();
}

