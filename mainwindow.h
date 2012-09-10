#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include <QAction>
#include <QMenu>
#include <QMainWindow>
#include <QPlainTextEdit>

class MainWindow : public QMainWindow
{
   Q_OBJECT

   public:
      MainWindow();

   protected:
      void closeEvent(QCloseEvent *event);

   private:
      Ui::MainWindow *m_ui;

      QPlainTextEdit *m_textEdit;
      QString m_curFile;

      QToolBar *fileToolBar;
      QToolBar *editToolBar;

      void createToolBars();
      void createConnections();
      void setStatusBar(QString msg, int timeOut);

      void readSettings();
      void writeSettings();

      bool maybeSave();
      void loadFile(const QString &fileName);
      bool saveFile(const QString &fileName);

      void setCurrentFile(const QString &fileName);
      QString strippedName(const QString &fullFileName);

   private slots:
      void newFile();
      void open();
      void close_Doc();
      void closeAll_Doc();
      void reload();
      bool save();
      bool saveAs();
      bool saveAll();
      void print();
      void printPreview();
      void printSetup();

      void setBackgroundColor();
      void setFont();

      void documentWasModified();
      void about();
};

#endif
