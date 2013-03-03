/**************************************************************************
*
* Copyright (c) 2012 Barbara Geller
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

#include "mainwindow.h"
#include "util.h"

#include <QDialog>
#include <QFileInfo>
#include <QDate>
#include <QLine>
#include <QFileDialog>
#include <QPainter>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QRect>
#include <QString>
#include <Qt>
#include <QTime>
#include <QTextDocument>
#include <QTextEdit>

#include <QAbstractTextDocumentLayout>

void MainWindow::print()
{
   QPrinter printer(QPrinter::HighResolution);

   QPrintDialog dialog(&printer);
   dialog.setWindowTitle("Print Document");

   if (m_textEdit->textCursor().hasSelection()) {
      dialog.addEnabledOption(QAbstractPrintDialog::PrintSelection);
   }

   if (dialog.exec() == QDialog::Accepted ) {
      this->printOut(&printer);
   }
}

void MainWindow::printPreview()
{
   QPrinter printer(QPrinter::HighResolution);

   QPrintPreviewDialog preview(&printer, this);
   preview.setWindowTitle(m_curFile);

   connect(&preview, SIGNAL(paintRequested(QPrinter *)), this, SLOT(printPreview(QPrinter*)));
   preview.exec();
}

void MainWindow::printPreview(QPrinter *printer)
{
   this->printOut(printer);
}

void MainWindow::printPdf()
{
   QFileDialog::Options options;

   if (false)  {  //(Q_OS_DARWIM) {
      options |= QFileDialog::DontUseNativeDialog;
   }

   QString selectedFilter;
   QString fileName = QFileDialog::getSaveFileName(this, tr("Print to PDF"),
         "document1.pdf", tr("PDF File (*.pdf)"), &selectedFilter, options);

   if (! fileName.isEmpty()) {

      if (QFileInfo(fileName).suffix().isEmpty()) {
         fileName.append(".pdf");
      }

      QPrinter printer(QPrinter::HighResolution);

      printer.setOutputFormat(QPrinter::PdfFormat);
      printer.setOutputFileName(fileName);

      this->printOut(&printer);
   }
}


// * *
void MainWindow::printOut(QPrinter *printer)
{     
   printer->setPageMargins(m_printer.marLeft, m_printer.marTop,
                           m_printer.marRight, m_printer.marBottom,QPrinter::Inch);

   QTextDocument *td = new QTextDocument;

   QString html = Qt::convertFromPlainText( m_textEdit->toPlainText() );
   td->setHtml(html);

   QPainter painter;

   // between the header and the body, and the body and the footer
   int spacer = 225;

   if (painter.begin(printer)) {

      painter.setFont(m_printer.fontText);
      td->documentLayout()->setPaintDevice(painter.device());     

      QRect printArea = printer->pageRect();

      // save for header and footer
      m_printArea = printArea;

      int headHeight = get_HeaderSize(&painter);
      int footHeight = get_FooterSize(&painter);

      printArea.setTop(printArea.top() + headHeight + spacer);
      printArea.setBottom(printArea.bottom() - (footHeight + spacer) );

      // QRect documentRect  = QRect(QPoint(0,0), td->size().toSize());
      QRect printableRect = QRect(QPoint(0,0), printArea.size());

      td->setPageSize(printableRect.size());

      m_pageNo    = 1;
      m_pageCount = td->pageCount();

      // print header and footer
      this->doHeader(&painter);
      this->doFooter(&painter);

      painter.translate(0, headHeight + spacer);
      painter.save();

      // while (printableRect.intersects(documentRect)) {
      for (int k = 1; k <= m_pageCount; ++k) {

         painter.drawRect(printableRect);   // test

         td->drawContents(&painter, printableRect);
         m_pageNo++;

         printableRect.translate(0, printableRect.height());
         // painter.restore();

         // painter.save();
         painter.translate(0, -printableRect.height() );

         // if (printableRect.intersects(documentRect)) {
         if (k < m_pageCount) {
            printer->newPage();

            this->doHeader(&painter);
            this->doFooter(&painter);            
         }
      }

      painter.restore();
      painter.end();
   }
}

int MainWindow::get_HeaderSize(QPainter *painter)
{
   if ( ! m_printer.printHeader) {
      return 0;
   }

   painter->save();
   painter->setFont(m_printer.fontHeader);

   QString header = "Test line for average height: 123";
   QRect rect     = painter->boundingRect(painter->window(), Qt::AlignLeft, header);

   int size = rect.height();

   if (! m_printer.header_line2.isEmpty()) {
      size = size * 2;
   }

   painter->restore();

   return size;
}

int MainWindow::get_FooterSize(QPainter *painter)
{
   if ( ! m_printer.printFooter) {
      return 0;
   }

   painter->save();
   painter->setFont(m_printer.fontFooter);

   QString footer = "Test line for average height: 123";
   QRect rect     = painter->boundingRect(painter->window(), Qt::AlignLeft, footer);

   int size = rect.height();

   if (! m_printer.footer_line2.isEmpty()) {
      size = size * 2;
   }

   painter->restore();

   return size;;
}

void MainWindow::doHeader(QPainter *painter)
{
   if ( ! m_printer.printHeader) {
      return;
   }

   QString header;
   QRect rect1;
   QRect rect3 = painter->window();

   //
   painter->save();
   painter->resetTransform();

   painter->setFont(m_printer.fontHeader);

   //
   header = macroExpand(m_printer.header_left);
   rect1  = painter->boundingRect(rect3, Qt::AlignLeft, header);
   painter->drawText(rect1, Qt::AlignLeft, header);

   //
   header = macroExpand(m_printer.header_center);
   rect1  = painter->boundingRect(rect3, Qt::AlignHCenter, header);

   rect1.setLeft(rect3.left());
   rect1.setRight(m_printArea.width());
   painter->drawText(rect1, Qt::AlignHCenter, header);

   //
   header = macroExpand(m_printer.header_right);
   rect1  = painter->boundingRect(rect3, Qt::AlignRight, header);

   rect1.setRight(m_printArea.width());
   painter->drawText(rect1,  Qt::AlignRight, header);

   //
   header = m_printer.header_line2;

   if (header.isEmpty()) {

      // line after header
      painter->drawLine(rect1.left(), rect1.bottom(), m_printArea.width(), rect1.bottom() );

   } else {
      QRect rect2 = rect3;
      rect2.translate(0, rect1.height());

      rect2 = painter->boundingRect(rect2, Qt::AlignLeft, header);
      painter->drawText(rect2, Qt::AlignLeft, header);

      // line after header
      painter->drawLine(rect2.left(), rect2.bottom(), m_printArea.width(), rect2.bottom() );
   }

   painter->restore();

   return;
}

void MainWindow::doFooter(QPainter *painter)
{  
   if ( ! m_printer.printFooter) {
      return;
   }

   QString footer;
   QRect rect1;   
   QRect rect3 = painter->window();

   //
   painter->save();
   painter->resetTransform();

   painter->setFont(m_printer.fontFooter);   

   //
   footer = macroExpand(m_printer.footer_left);
   rect1  = painter->boundingRect(rect3, Qt::AlignLeft, footer);

   rect1.translate(0, rect3.height() );
   painter->drawText(rect1, Qt::AlignLeft, footer);

   //
   footer = macroExpand(m_printer.footer_center);
   rect1  = painter->boundingRect(rect3, Qt::AlignHCenter, footer);

   rect1.setLeft(rect3.left());
   rect1.setRight(m_printArea.width());
   rect1.translate(0, rect3.height() );
   painter->drawText(rect1, Qt::AlignHCenter, footer);

   //
   footer = macroExpand(m_printer.footer_right);
   rect1  = painter->boundingRect(rect3, Qt::AlignRight, footer);

   rect1.setRight(m_printArea.width());
   rect1.translate(0, rect3.height() );
   painter->drawText(rect1, Qt::AlignRight, footer);

   //
   footer = m_printer.footer_line2;

   if (! footer.isEmpty()) {
      QRect rect2 = rect3;
      rect2.translate(0, rect2.height() + rect1.height());

      rect2  = painter->boundingRect(rect2, Qt::AlignLeft, footer);
      painter->drawText(rect2, Qt::AlignLeft, footer);
   }

   // line before footer
   rect3.translate(0, rect3.height());

   painter->drawLine(rect3.left(), rect3.top(), m_printArea.width(), rect3.top() );
   painter->restore();

   return;
}

QString MainWindow::macroExpand(QString data)
{
   QString macro;
   QString text;

   int begin;
   int end;

   while (true)  {

      begin = data.indexOf("$(");

      if (begin == -1)  {
         break;
      }

      end = data.indexOf(")", begin);

      if (end == -1)  {
         data = data.replace(begin, 2, "");
         continue;
      }

      macro = data.mid(begin, end-begin+1);
      text  = "";

      if (macro == "$(FileName)") {
         text = strippedName(m_curFile);

      } else if (macro == "$(PathFileName)") {
         text = m_curFile;

      } else if (macro == "$(PageNo)") {
         text = QString::number(m_pageNo);

      } else if (macro == "$(TotalPages)") {
         text = QString::number(m_pageCount);

      } else if (macro == "$(Date)") {
         QDate date   = QDate::currentDate();
         text= date.toString(m_struct.formatDate);

      } else if (macro == "$(Time)") {
         QTime time   = QTime::currentTime();
         text = time.toString(m_struct.formatTime);

      }

      data = data.replace(begin, end-begin+1, text);
   }

   return data;
}




