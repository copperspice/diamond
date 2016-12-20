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

static const double SOURCE_DPI = 100.00;

void MainWindow::print()
{
   QPrinter printer(QPrinter::HighResolution);   

   QPrintDialog dialog(&printer, this);
   dialog.setWindowTitle("Print Document");

   if (m_textEdit->textCursor().hasSelection()) {
      dialog.addEnabledOption(QAbstractPrintDialog::PrintSelection);
   }

   if (dialog.exec() == QDialog::Accepted) {
      this->printOut(&printer);
   }
}

void MainWindow::printPreview()
{  
   // called from menu
   QPrinter printer(QPrinter::HighResolution);

   QPrintPreviewDialog preview(&printer, this);
   preview.setWindowTitle(m_curFile);

   connect(&preview, &QPrintPreviewDialog::paintRequested, this, &MainWindow::printOut);
   preview.exec();
}

void MainWindow::printPdf()
{       
   QString outputName = QFileInfo(m_curFile).baseName() + ".pdf";

   QString selectedFilter;
   QFileDialog::Options options;  

   QString fileName = QFileDialog::getSaveFileName(this, tr("Print to PDF"),
         outputName, tr("PDF File (*.pdf)"), &selectedFilter, options);

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
   QTextDocument *td = new QTextDocument;

/*
   // consider this later one
   if (m_ui->actionWord_Wrap->isChecked()) {
      td->setWordWrapMode(QTextOption::WordWrap);
   }
*/

   QString html = "";

   if (m_printer.lineNumbers)  {

      QTextBlock block = m_textEdit->document()->firstBlock();
      int blockNumber  = block.blockNumber();

      html += "<html><head></head>";
      html += "<body><table border='none' cellspacing='0' cellpadding='0'>";

      while ( block.isValid() ) {

         html += "<tr>";
         html += "<td  align='right' valign='middle'><b><font size='2'>" + QString::number(blockNumber + 1) + "</b></font></td>";
         html += "<td> &nbsp;&nbsp; </td>";
         html += "<td> " + this->convertBlockToHTML(block.text()) + "</td>";
         html += "</tr>";

         block = block.next();
         ++blockNumber;
      }

      html += "</table></body></html>";

   }  else  {
      html = Qt::convertFromPlainText(m_textEdit->toPlainText());

   }

   td->setHtml(html);

   printer->setPaperSize(QPrinter::Letter);
   printer->setPageMargins(m_printer.marLeft, m_printer.marTop,
                           m_printer.marRight, m_printer.marBottom,QPrinter::Inch);

   QPainter painter;      

   if (painter.begin(printer)) {

      m_resolution = printer->logicalDpiX();            

      painter.setViewport(printer->paperRect());

      int winX = 8.5  * m_resolution;
      int winY = 11.0 * m_resolution;
      painter.setWindow(0, 0, winX, winY);

      td->documentLayout()->setPaintDevice(painter.device());
      td->setDefaultFont(m_printer.fontText);

      // save printarea for header and footer
      double xx = (8.5 - m_printer.marLeft - m_printer.marRight) * m_resolution;
      double yy = (11.0 - m_printer.marTop - m_printer.marBottom) * m_resolution;
      QRectF printArea = QRectF(0, 0, xx, yy);

      // between the header and the body, and the body and the footer
      int spacer = m_printer.hdrGap * m_resolution;

      m_printArea = printArea;

      int headHeight = get_HeaderSize(&painter);
      int footHeight = get_FooterSize(&painter);

      printArea.setTop(printArea.top() + headHeight + spacer);
      printArea.setBottom(printArea.bottom() - (footHeight + spacer) );

      QRectF printableRect = QRectF(QPoint(0,0), printArea.size());
      td->setPageSize(printableRect.size());

      m_pageNo    = 1;
      m_pageCount = td->pageCount();

      // print header and footer     
      this->doHeader(&painter);
      this->doFooter(&painter);

      int headerSpace = + headHeight + spacer;

      for (int k = 1; k <= m_pageCount; ++k) {

         painter.save();

         // move the painter "down"
         painter.translate(0, (printableRect.height() * (k-1) * (-1) ) + headerSpace);

         // print one page worth of text
         td->drawContents(&painter, printableRect);
         m_pageNo++;

         // move the document "up"
         printableRect.translate(0, printableRect.height());

         painter.restore();

         if (k < m_pageCount) {
            printer->newPage();

            this->doHeader(&painter);
            this->doFooter(&painter);
         }
      }

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

   QString header = "Test line";
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

   QString footer = "Test line";
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
   QRectF rect1;
   QRectF rectBig =  m_printArea;

   painter->save();
   painter->setFont(m_printer.fontHeader);

   //
   header = macroExpand(m_printer.header_left);
   rect1  = painter->boundingRect(rectBig, Qt::AlignLeft, header);
   painter->drawText(rect1, Qt::AlignLeft, header);

   //
   header = macroExpand(m_printer.header_center);
   rect1  = painter->boundingRect(rectBig, Qt::AlignHCenter, header);
   painter->drawText(rect1, Qt::AlignHCenter, header);

   //
   header = macroExpand(m_printer.header_right);
   rect1  = painter->boundingRect(rectBig, Qt::AlignRight, header);
   painter->drawText(rect1, Qt::AlignRight, header);

   //
   header = m_printer.header_line2;

   if (header.isEmpty()) {
      // line after header       
      painter->drawLine(rectBig.left(), rect1.bottom()+8, rectBig.right(), rect1.bottom()+8);

   } else {
      QRectF rect2 = rectBig;
      rect2.translate(0, rect1.height());

      rect2 = painter->boundingRect(rect2, Qt::AlignLeft, header);
      painter->drawText(rect2, Qt::AlignLeft, header);

      // line after header
      painter->drawLine(rectBig.left(), rect2.bottom()+8, rectBig.right(), rect2.bottom()+8);
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
   QRectF rect1;
   QRectF rectBig = m_printArea;

   painter->save();
   painter->setFont(m_printer.fontFooter);

   //
   QString footer_L2 = footer = m_printer.footer_line2;
   QRectF rect_L2;

   if (! footer_L2.isEmpty()) {
      rect_L2 = painter->boundingRect(rectBig, Qt::AlignLeft, footer_L2);

      rect_L2.translate(0, rectBig.height() - rect_L2.height());
      painter->drawText(rect_L2, Qt::AlignLeft, footer_L2);
   }

   //
   footer = macroExpand(m_printer.footer_left);
   rect1  = painter->boundingRect(rectBig, Qt::AlignLeft, footer);

   rect1.translate(0, rectBig.height() - rect1.height() - rect_L2.height() );
   painter->drawText(rect1, Qt::AlignLeft, footer);

   //
   footer = macroExpand(m_printer.footer_center);
   rect1  = painter->boundingRect(rectBig, Qt::AlignHCenter, footer);

   rect1.translate(0, rectBig.height() - rect1.height() - rect_L2.height() );
   painter->drawText(rect1, Qt::AlignHCenter, footer);

   //
   footer = macroExpand(m_printer.footer_right);
   rect1  = painter->boundingRect(rectBig, Qt::AlignRight, footer);

   rect1.translate(0, rectBig.height() - rect1.height() - rect_L2.height() );
   painter->drawText(rect1, Qt::AlignRight, footer);

   // line before footer  
   painter->drawLine( rectBig.left(), rect1.top()-3, rectBig.right(), rect1.top()-3 );

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

QString MainWindow::convertBlockToHTML(const QString &plain) const
{
   int col = 0;

   QString retval;

   for (int i = 0; i < plain.length(); ++i) {

      if (plain[i] == QLatin1Char('\t'))  {
         retval += QChar(0x00a0U);
         ++col;

         while (col % m_struct.tabSpacing) {
            retval += QChar(0x00a0U);
            ++col;
         }

      } else if (plain[i].isSpace()) {
         retval += QChar(0x00a0U);

      } else if (plain[i] == QLatin1Char('<')) {
         retval += QLatin1String("&lt;");

      } else if (plain[i] == QLatin1Char('>'))  {
         retval += QLatin1String("&gt;");

      } else if (plain[i] == QLatin1Char('&'))  {
         retval += QLatin1String("&amp;");

      } else  {
         retval += plain[i];

      }

      ++col;
   }

   return retval;
}





