/**************************************************************************
*
* Copyright (c) 2012-2022 Barbara Geller
*
* Diamond Editor is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License version 2
* as published by the Free Software Foundation.
*
* Diamond is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
***************************************************************************/

#include "util.h"

#include <stdexcept>

#include <QDesktopServices>
#include <QFileInfo>
#include <QString>
#include <QUrl>

void showHtml(QString route, QString url)
{
   QFileInfo fileInfo(url);

   if (route == "docs") {

      // display html help file using client browser
      bool ok = QDesktopServices::openUrl(QUrl(url));

      if (! ok)  {
         csError("Diamond Documentation", "Unable to display Diamond Documentation\n" + url );
      }

   } else {

      if (url.isEmpty())  {
         csError("Html Viewer", "No file was specified.");
         return;

      } else if (! fileInfo.exists() )  {
         csError("Html Viewer", "Specified file does not exist\n" + url);
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

