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

#include "spellcheck.h"
#include "util.h"

#include <hunspell.hxx>

#include <QByteArray>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

SpellCheck::SpellCheck(const QString &dictMain, const QString &dictUser)
{ 
   m_userFname = dictUser;

   QString base = dictMain;
   base = base.mid(0, base.indexOf("."));

   QString dicFname  = base + ".dic";
   QString affFName  = base + ".aff";

   //
   QByteArray affixFilePathBA = affFName.toLocal8Bit();
   QByteArray dictFilePathBA  = dicFname.toLocal8Bit();

   m_hunspell = new Hunspell(affixFilePathBA.constData(), dictFilePathBA.constData() );

   // encode as SET option in the affix file
   m_encoding = "ISO8859-1";
   m_codec    = QTextCodec::codecForName(m_encoding.toLatin1().constData());

   //
   if (! m_userFname.isEmpty()) {

      QFile file(m_userFname);

      if (! file.open(QFile::ReadOnly)) {
         QString error = QObject::tr("Unable to read file %1:\n%2.").arg(m_userFname).arg(file.errorString());
         csError("Spell Check", error);
         return;
      }

      QTextStream stream(&file);

      for (QString word(stream.readLine()); ! word.isEmpty(); word = stream.readLine()) {
         put_word(word);
      }

      file.close();

   } else {
      csError("Spell Check", "Unable to find User Dictionary " + m_userFname);

   }
}

SpellCheck::~SpellCheck()
{
   delete m_hunspell;
}

bool SpellCheck::spell(const QString &word)
{
   bool isCorrect;

   if ( word.isEmpty() ) {
      return true;
   }

   QString lookUp(word);

   while ( ! lookUp.isEmpty() && ! lookUp.at(0).isLetter()) {
      lookUp = lookUp.mid(1);
   }

   isCorrect = m_hunspell->spell(m_codec->fromUnicode(lookUp).constData()) != 0;

   return isCorrect;
}

QStringList SpellCheck::suggest(const QString &word)
{
   char **suggestWordList;
   QStringList suggestions;

   const int cnt = m_hunspell->suggest(&suggestWordList, m_codec->fromUnicode(word).constData());

   for (int k = 0; k < cnt; ++k) {
      suggestions.append(m_codec->toUnicode(suggestWordList[k]));
   }

   m_hunspell->free_list(&suggestWordList, cnt);
   return suggestions;
}

void SpellCheck::ignoreWord(const QString &word)
{
   put_word(word);
}

void SpellCheck::put_word(const QString &word)
{
   m_hunspell->add(m_codec->fromUnicode(word).constData());
}

void SpellCheck::addToUserDict(const QString &word)
{
   // remove non letters
   QString lookUp(word);

   while ( ! lookUp.isEmpty() && ! lookUp.at(0).isLetter()) {
      lookUp = lookUp.mid(1);
   }

   put_word(lookUp);

   if (! m_userFname.isEmpty()) {

      QFile file(m_userFname);

      if (! file.open(QFile::Append)) {
         QString error = QObject::tr("Unable to read file %1:\n%2.").arg(m_userFname).arg(file.errorString());
         csError("Spell Check", error);
         return;
      }

      QTextStream stream(&file);
      stream << lookUp << "\n";

      file.close();

   } else {
      csError("Spell Check", "Unable to find User Dictionary " + m_userFname);

   }
}
