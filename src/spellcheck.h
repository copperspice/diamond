/**************************************************************************
*
* Copyright (c) 2012-2025 Barbara Geller
*
* Diamond Editor is free software. You can redistribute it and/or
* modify it under the terms of the GNU General Public License
* version 2 as published by the Free Software Foundation.
*
* Diamond Editor is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* https://www.gnu.org/licenses/
*
***************************************************************************/

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
      void put_word(const QString &word);

      QString m_userFname;
      QTextCodec *m_codec;

      Hunspell *m_hunspell;
};

#endif
