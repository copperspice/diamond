#ifndef UTIL_H
#define UTIL_H

#include <QMessageBox>

void csMsg(const QString &msg);
void csMsg(QString msg, int value);
void csMsg(QWidget *parent, const QString &title, const QString &msg);

#endif
