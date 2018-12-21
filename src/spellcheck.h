/**************************************************************************
*
* Copyright (c) 2012-2018 Barbara Geller
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

#ifndef SPELLCHECK_H
#define SPELLCHECK_H

#include <QString>

class Hunspell;

class SpellCheck
{
   public:
      SpellCheck(const QString &dictMain, const QString &dictUser);
      ~SpellCheck();

      bool spell(QStringView word);
      QStringList suggest(const QString &word);
      void ignoreWord(const QString &word);
      void addToUserDict(const QString &word);

   private:
      QString m_userFname;
      QString m_encoding;
      QTextCodec *m_codec;

      Hunspell *m_hunspell;

      void put_word(const QString &word);
};

#endif
