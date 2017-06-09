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

#include "util.h"

#include <stdexcept>

#include <QDesktopServices>
#include <QFileInfo>
#include <QString>
#include <QUrl>

void showHtml(QString route, QString file)
{
   QString m_url = file;
   QFileInfo fileInfo(m_url);

   if (route == "docs") {

      if (! fileInfo.exists() )  {
         csError("Diamond Documentation", "Help file was not found:\n" +  m_url + "\n\n"
                 "To specify the location of the Diamond Documentation, select 'Settings' from the main Menu. "
                 "Then select 'General Options' and click on the Options tab.\n");
         return;
      }

      // display html help file using the clients browser

      QString indexUrl = "file:///" + fileInfo.absoluteFilePath();
      bool ok = QDesktopServices::openUrl(QUrl(indexUrl));

      if (! ok)  {
         csError("Diamond Documentation", "Unable to display Diamond Documentation\n" + indexUrl );
      }

   } else {

      if (m_url.isEmpty())  {
         csError("Html Viewer", "No file was specified.");
         return;

      } else if (! fileInfo.exists() )  {
         csError("Html Viewer", "Specified file does not exist\n" + m_url);
         return;

      } else if (fileInfo.suffix().isEmpty())  {
         csError("Html Viewer", "Specified file does not have an extension, unable to display as HTML.");
         return;

      }

      // display html help file using the clients browser

      QString indexUrl = "file:///" + fileInfo.absoluteFilePath();
      bool ok = QDesktopServices::openUrl(QUrl(indexUrl));

      if (! ok)  {
         csError("Display HTML", "Unable to display file as HTML\n" + indexUrl );
      }
   }
}

