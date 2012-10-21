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
#include <QLine>
#include <QFileDialog>
#include <QPainter>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QRect>
#include <QString>
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

   // printer->setPaperSize(QPrinter::Letter);

   QTextDocument *td = new QTextDocument;

   QString html = Qt::convertFromPlainText( m_textEdit->toPlainText() );
   td->setHtml(html);

   QPainter painter;

   int spacer = 250;

   if (painter.begin(printer)) {

      painter.setFont(m_printer.fontText);

      td->documentLayout()->setPaintDevice(painter.device());
      td->setPageSize(printer->pageRect().size());      

      QRect printArea = printer->pageRect();

      // first header and footer
      int headHeight = this->doHeader(&painter);
      int footHeight = this->doFooter(&painter, printArea);

      painter.translate(0, headHeight + spacer);
      painter.save();

      //
      printArea.setTop(printArea.top() + headHeight + spacer);
      printArea.setBottom(printArea.bottom() - (footHeight + spacer) );

      QRect documentRect  = QRect(QPoint(0,0), td->size().toSize());
      QRect printableRect = QRect(QPoint(0,0), printArea.size());

      int pageNo = 0;

      while (printableRect.intersects(documentRect)) {

         td->drawContents(&painter, printableRect);
         pageNo++;

         painter.drawRect(printableRect);   // test

         printableRect.translate(0, printableRect.height());
         painter.restore();


         //  headers were here



         painter.save();
         painter.translate(0, -printableRect.height() * pageNo);

         if (printableRect.intersects(documentRect)) {
            printer->newPage();

            this->doHeader(&painter);
            this->doFooter(&painter, printer->pageRect() );

            //  painter.translate(0, headHeight + 25 );
         }
      }

      painter.restore();
      painter.end();
   }
}

int MainWindow::doHeader(QPainter *painter)
{
   painter->save();
   painter->resetTransform();

   painter->setFont(m_printer.fontHeader);

   QRect boundingRect = painter->boundingRect(painter->window(), Qt::TextWordWrap, m_printer.header);
   painter->drawText(boundingRect, Qt::TextWordWrap, m_printer.header);

   // line after header
   painter->drawLine(boundingRect.left(), boundingRect.bottom(), painter->window().width(), boundingRect.bottom() );
   painter->restore();

   return boundingRect.height();
}

int MainWindow::doFooter(QPainter *painter, QRect printArea)
{      
   painter->save();
   painter->resetTransform();

   painter->setFont(m_printer.fontFooter);

   QRect boundingRect = painter->boundingRect(printArea, Qt::TextWordWrap, m_printer.footer);

   painter->translate(0, printArea.height() - boundingRect.height());
   painter->drawText(boundingRect, Qt::TextWordWrap, m_printer.footer);

   // line before footer
   painter->drawLine(boundingRect.left(), boundingRect.top(), painter->window().width(), boundingRect.top() );
   painter->restore();

   return boundingRect.height();
}
