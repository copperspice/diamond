/**************************************************************************
*
* Copyright (c) 2012-2024 Barbara Geller
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

#include "spellcheck.h"
#include "util.h"

#include <hunspell.hxx>

#include <QByteArray>
#include <QFile>
#include <QTextCodec>
#include <QTextStream>

#if defined (H_DEPRECATED)
// could be version 1.5, 1.6, or 1.7
#define HUNSPELL_VERSION 5

#else
// any version less than 1.5
#define HUNSPELL_VERSION 4

#endif

// debian 9 uses 1.4.1
// ubuntu 16.04 uses 1.3.3
// all other supported platforms use 1.5 or newer

SpellCheck::SpellCheck(const QString &dictMain, const QString &dictUser)
{
   m_userFname = dictUser;

   QString base = dictMain;
   base = base.mid(0, base.indexOf("."));

   QString dicFname  = base + ".dic";
   QString affFName  = base + ".aff";

   m_hunspell = new Hunspell(affFName .constData(), dicFname.constData() );

   // encode as SET option in the affix file
   m_codec = QTextCodec::codecForName("UTF-8");

   if (! m_userFname.isEmpty()) {
      QFile file(m_userFname);

      if (! file.open(QFile::ReadOnly)) {
         QString error = QObject::tr("Unable to read file %1:\n%2.").formatArgs(m_userFname, file.errorString());
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

bool SpellCheck::spell(QStringView word)
{
   bool isCorrect;

   if ( word.isEmpty() ) {
      return true;
   }

   while (! word.isEmpty() && ! word.at(0).isLetter()) {
      word = word.mid(1);
   }

#if (HUNSPELL_VERSION >= 5)
   isCorrect = m_hunspell->spell(QString(word).toStdString());

#else
   isCorrect = m_hunspell->spell(m_codec->fromUnicode(word).constData()) != 0;

#endif

   return isCorrect;
}

QStringList SpellCheck::suggest(const QString &word)
{
   QStringList suggestions;

#if (HUNSPELL_VERSION >= 5)

   QVector<std::string> suggestWordList = QVector<std::string>::fromStdVector(m_hunspell->suggest(word.toStdString()));

   for (auto item : suggestWordList) {
      suggestions.append(QString::fromLatin1(item.c_str()));
   }

#else
   char **suggestWordList;

   const int cnt = m_hunspell->suggest(&suggestWordList, m_codec->fromUnicode(word).constData());

   for (int k = 0; k < cnt; ++k) {
      suggestions.append( m_codec->toUnicode(suggestWordList[k]) );
   }

   m_hunspell->free_list(&suggestWordList, cnt);
#endif

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
         QString error = QObject::tr("Unable to read file %1:\n%2.").formatArgs(m_userFname, file.errorString());
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
