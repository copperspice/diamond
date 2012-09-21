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

#include "dialog_colors.h"
#include "util.h"

#include <QColorDialog>
#include <QPalette>
#include <QLineEdit>
#include <QString>
#include <QStringList>

Dialog_Colors::Dialog_Colors(MainWindow *from, Syntax *dw)
   : m_ui(new Ui::Dialog_Colors)
{
   m_mainWindow   = from;
   m_syntaxParser = dw;   
   m_ui->setupUi(this);

   m_struSettings = m_mainWindow->get_StructData();
   m_syntaxFname  = m_struSettings.pathSyntax + "syn_cpp.txt";

   initData();

   connect(m_ui->text_TB,     SIGNAL(clicked()), this, SLOT(text_TB())     );
   connect(m_ui->back_TB,     SIGNAL(clicked()), this, SLOT(back_TB())     );
   connect(m_ui->highText_TB, SIGNAL(clicked()), this, SLOT(highText_TB()) );
   connect(m_ui->highBack_TB, SIGNAL(clicked()), this, SLOT(highBack_TB()) );   

   connect(m_ui->key_TB,      SIGNAL(clicked()), this, SLOT(key_TB())      );
   connect(m_ui->type_TB,     SIGNAL(clicked()), this, SLOT(type_TB())     );

   connect(m_ui->save_PB,   SIGNAL(clicked()), this, SLOT(Save()));
   connect(m_ui->cancel_PB, SIGNAL(clicked()), this, SLOT(Cancel()));
}

Dialog_Colors::~Dialog_Colors()
{
   delete m_ui;
}

void Dialog_Colors::colorBox(QLineEdit *field, QColor color)
{
   QPalette temp;

   temp = field->palette();
   temp.setColor(QPalette::Base, color);
   field->setPalette(temp);
}

void Dialog_Colors::initData()
{
   QPalette temp;   

   // 1
   m_ui->text_Color->setReadOnly(true);
   colorBox(m_ui->text_Color, m_struSettings.colorText);

   m_ui->back_Color->setReadOnly(true);
   colorBox(m_ui->back_Color, m_struSettings.colorBack);

   m_ui->highText_Color->setReadOnly(true);
   colorBox(m_ui->highText_Color, m_struSettings.colorHighText);

   m_ui->highBack_Color->setReadOnly(true);
   colorBox(m_ui->highBack_Color, m_struSettings.colorHighBack);

   // 2
   m_ui->key_Color->setReadOnly(true);
   colorBox(m_ui->key_Color, m_struSettings.syn_KeyText);

   m_ui->type_Color->setReadOnly(true);
   colorBox(m_ui->type_Color, m_struSettings.syn_TypeText);

   m_ui->class_Color->setReadOnly(true);
   colorBox(m_ui->class_Color, m_struSettings.syn_ClassText);

   m_ui->func_Color->setReadOnly(true);
   colorBox(m_ui->func_Color, m_struSettings.syn_FuncText);

   m_ui->quote_Color->setReadOnly(true);
   colorBox(m_ui->quote_Color, m_struSettings.syn_QuoteText);

   m_ui->comment_Color->setReadOnly(true);
   colorBox(m_ui->comment_Color, m_struSettings.syn_CommentText);

   m_ui->mline_Color->setReadOnly(true);
   colorBox(m_ui->mline_Color, m_struSettings.syn_MLineText);

   // 3
   m_syntaxParser = new Syntax(m_ui->sample->document(), m_syntaxFname, m_struSettings);
}

void Dialog_Colors::Save()
{
   QPalette temp;

   // 1
   temp = m_ui->text_Color->palette();
   m_struSettings.colorText = temp.color(QPalette::Base);

   temp = m_ui->back_Color->palette();
   m_struSettings.colorBack = temp.color(QPalette::Base);

   temp = m_ui->highText_Color->palette();
   m_struSettings.colorHighText = temp.color(QPalette::Base);

   temp = m_ui->highBack_Color->palette();
   m_struSettings.colorHighBack = temp.color(QPalette::Base);

   // 2
   temp = m_ui->key_Color->palette();
   m_struSettings.syn_KeyText = temp.color(QPalette::Base);

   temp = m_ui->type_Color->palette();
   m_struSettings.syn_TypeText = temp.color(QPalette::Base);

   temp = m_ui->class_Color->palette();
   m_struSettings.syn_ClassText = temp.color(QPalette::Base);

   temp = m_ui->func_Color->palette();
   m_struSettings.syn_FuncText = temp.color(QPalette::Base);

   temp = m_ui->quote_Color->palette();
   m_struSettings.syn_QuoteText = temp.color(QPalette::Base);

   temp = m_ui->comment_Color->palette();
   m_struSettings.syn_CommentText = temp.color(QPalette::Base);

   temp = m_ui->mline_Color->palette();
   m_struSettings.syn_MLineText = temp.color(QPalette::Base);

   this->done(QDialog::Accepted);
}

QColor Dialog_Colors::pickColor(QColor oldColor)
{
   QColor color = oldColor;

   if (false)  {      // (useNative)
       color = QColorDialog::getColor(color, this);
   } else  {
       color = QColorDialog::getColor(color, this, "Select Color", QColorDialog::DontUseNativeDialog);
   }

   if (color.isValid()) {
      return color;
   }

   return oldColor;
}

void Dialog_Colors::text_TB()
{
   QColor color = m_struSettings.colorText;
   m_struSettings.colorText = pickColor(color);

   //
   colorBox(m_ui->text_Color, m_struSettings.colorText);

   //
   QPalette temp;
   temp = m_ui->sample->palette();
   temp.setColor(QPalette::Text, m_struSettings.colorText);
   temp.setColor(QPalette::Base, m_struSettings.colorBack);
   m_ui->sample->setPalette(temp);
}

void Dialog_Colors::back_TB()
{
   QColor color = m_struSettings.colorBack;
   m_struSettings.colorBack = pickColor(color);

   //
   colorBox(m_ui->back_Color, m_struSettings.colorBack);

   //
   QPalette temp;
   temp = m_ui->sample->palette();
   temp.setColor(QPalette::Text, m_struSettings.colorText);
   temp.setColor(QPalette::Base, m_struSettings.colorBack);
   m_ui->sample->setPalette(temp);
}

void Dialog_Colors::highText_TB()
{
   QColor color = m_struSettings.colorHighText;
   m_struSettings.colorHighText = pickColor(color);

   //
   colorBox(m_ui->highText_Color, m_struSettings.colorHighText);
   m_syntaxParser = new Syntax(m_ui->sample->document(), m_syntaxFname, m_struSettings);
}

void Dialog_Colors::highBack_TB()
{
   QColor color = m_struSettings.colorHighBack;
   m_struSettings.colorHighBack = pickColor(color);

   //
   colorBox(m_ui->highBack_Color, m_struSettings.colorHighBack);
   m_syntaxParser = new Syntax(m_ui->sample->document(), m_syntaxFname, m_struSettings);
}

void Dialog_Colors::key_TB()
{
   QColor color = m_struSettings.syn_KeyText;
   m_struSettings.syn_KeyText = pickColor(color);

   //
   colorBox(m_ui->key_Color, m_struSettings.syn_KeyText);
   m_syntaxParser = new Syntax(m_ui->sample->document(), m_syntaxFname, m_struSettings);
}

void Dialog_Colors::type_TB()
{
   QColor color = m_struSettings.syn_TypeText;
   m_struSettings.syn_TypeText = pickColor(color);

   //
   colorBox(m_ui->type_Color, m_struSettings.syn_TypeText);
   m_syntaxParser = new Syntax(m_ui->sample->document(), m_syntaxFname, m_struSettings);
}

void Dialog_Colors::class_TB()
{
   QColor color = m_struSettings.syn_ClassText;
   m_struSettings.syn_ClassText = pickColor(color);

   //
   colorBox(m_ui->class_Color, m_struSettings.syn_ClassText);
   m_syntaxParser = new Syntax(m_ui->sample->document(), m_syntaxFname, m_struSettings);
}

void Dialog_Colors::func_TB()
{
   QColor color = m_struSettings.syn_FuncText;
   m_struSettings.syn_FuncText = pickColor(color);

   //
   colorBox(m_ui->func_Color, m_struSettings.syn_FuncText);
   m_syntaxParser = new Syntax(m_ui->sample->document(), m_syntaxFname, m_struSettings);
}

void Dialog_Colors::quote_TB()
{
   QColor color = m_struSettings.syn_QuoteText;
   m_struSettings.syn_QuoteText = pickColor(color);

   //
   colorBox(m_ui->quote_Color, m_struSettings.syn_QuoteText);
   m_syntaxParser = new Syntax(m_ui->sample->document(), m_syntaxFname, m_struSettings);
}

void Dialog_Colors::comment_TB()
{
   QColor color = m_struSettings.syn_CommentText;
   m_struSettings.syn_CommentText = pickColor(color);

   //
   colorBox(m_ui->comment_Color, m_struSettings.syn_CommentText);
   m_syntaxParser = new Syntax(m_ui->sample->document(), m_syntaxFname, m_struSettings);
}

void Dialog_Colors::mline_TB()
{
   QColor color = m_struSettings.syn_MLineText;
   m_struSettings.syn_MLineText = pickColor(color);

   //
   colorBox(m_ui->mline_Color, m_struSettings.syn_MLineText);
   m_syntaxParser = new Syntax(m_ui->sample->document(), m_syntaxFname, m_struSettings);
}

void Dialog_Colors::Cancel()
{
   this->done(QDialog::Rejected);
}

struct Settings Dialog_Colors::get_Colors()
{
   return m_struSettings;
}
