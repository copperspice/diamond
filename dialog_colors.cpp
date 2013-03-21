/**************************************************************************
*
* Copyright (c) 2012-2013 Barbara Geller
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

#include <QCheckBox>
#include <QColorDialog>
#include <QPalette>
#include <QLineEdit>
#include <QString>
#include <QStringList>

Dialog_Colors::Dialog_Colors(MainWindow *from)
   : m_ui(new Ui::Dialog_Colors)
{
   m_mainWindow   = from;   
   m_ui->setupUi(this);

   m_struSettings = m_mainWindow->get_StructData();
   m_syntaxFname  = m_struSettings.pathSyntax + "syn_cpp.json";  

   m_syntaxParser = new Syntax(m_ui->sample->document(), m_syntaxFname, m_struSettings);
   updateParser(false);

   //
   initData();

   connect(m_ui->text_TB,     SIGNAL(clicked()), this, SLOT(text_TB())     );
   connect(m_ui->back_TB,     SIGNAL(clicked()), this, SLOT(back_TB())     );
   connect(m_ui->highText_TB, SIGNAL(clicked()), this, SLOT(highText_TB()) );
   connect(m_ui->highBack_TB, SIGNAL(clicked()), this, SLOT(highBack_TB()) );   

   connect(m_ui->key_TB,      SIGNAL(clicked()), this, SLOT(key_TB())      );
   connect(m_ui->type_TB,     SIGNAL(clicked()), this, SLOT(type_TB())     );
   connect(m_ui->class_TB,    SIGNAL(clicked()), this, SLOT(class_TB())    );
   connect(m_ui->func_TB,     SIGNAL(clicked()), this, SLOT(func_TB())     );
   connect(m_ui->quote_TB,    SIGNAL(clicked()), this, SLOT(quote_TB())    );
   connect(m_ui->comment_TB,  SIGNAL(clicked()), this, SLOT(comment_TB())  );
   connect(m_ui->mline_TB,    SIGNAL(clicked()), this, SLOT(mline_TB())    );

   connect(m_ui->key_Bold_CB,       SIGNAL(clicked()), this, SLOT(key_bold())      );
   connect(m_ui->key_Italic_CB,     SIGNAL(clicked()), this, SLOT(key_italic())    );
   connect(m_ui->type_Bold_CB,      SIGNAL(clicked()), this, SLOT(type_bold())     );
   connect(m_ui->type_Italic_CB,    SIGNAL(clicked()), this, SLOT(type_italic())   );
   connect(m_ui->class_Bold_CB,     SIGNAL(clicked()), this, SLOT(class_bold())    );
   connect(m_ui->class_Italic_CB,   SIGNAL(clicked()), this, SLOT(class_italic())  );
   connect(m_ui->func_Bold_CB,      SIGNAL(clicked()), this, SLOT(func_bold())     );
   connect(m_ui->func_Italic_CB,    SIGNAL(clicked()), this, SLOT(func_italic())   );
   connect(m_ui->quote_Bold_CB,     SIGNAL(clicked()), this, SLOT(quote_bold())    );
   connect(m_ui->quote_Italic_CB,   SIGNAL(clicked()), this, SLOT(quote_italic())  );
   connect(m_ui->comment_Bold_CB,   SIGNAL(clicked()), this, SLOT(comment_bold())  );
   connect(m_ui->comment_Italic_CB, SIGNAL(clicked()), this, SLOT(comment_italic()));
   connect(m_ui->mline_Bold_CB,     SIGNAL(clicked()), this, SLOT(mline_bold())    );
   connect(m_ui->mline_Italic_CB,   SIGNAL(clicked()), this, SLOT(mline_italic())  );

   connect(m_ui->save_PB,           SIGNAL(clicked()), this, SLOT(Save()));
   connect(m_ui->cancel_PB,         SIGNAL(clicked()), this, SLOT(Cancel()));
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

   if (m_struSettings.syn_KeyWeight == QFont::Bold) {
      m_ui->key_Bold_CB->setChecked(true);
   }
   m_ui->key_Italic_CB->setChecked(m_struSettings.syn_KeyItalic);

   /**/
   m_ui->type_Color->setReadOnly(true);
   colorBox(m_ui->type_Color, m_struSettings.syn_TypeText);

   if (m_struSettings.syn_TypeWeight == QFont::Bold) {
      m_ui->type_Bold_CB->setChecked(true);
   }
   m_ui->type_Italic_CB->setChecked(m_struSettings.syn_TypeItalic);

   /**/
   m_ui->class_Color->setReadOnly(true);
   colorBox(m_ui->class_Color, m_struSettings.syn_ClassText);

   if (m_struSettings.syn_ClassWeight == QFont::Bold) {
      m_ui->class_Bold_CB->setChecked(true);
   }
   m_ui->class_Italic_CB->setChecked(m_struSettings.syn_ClassItalic);

   /**/
   m_ui->func_Color->setReadOnly(true);
   colorBox(m_ui->func_Color, m_struSettings.syn_FuncText);

   if (m_struSettings.syn_FuncWeight == QFont::Bold) {
      m_ui->func_Bold_CB->setChecked(true);
   }
   m_ui->func_Italic_CB->setChecked(m_struSettings.syn_FuncItalic);

   /**/
   m_ui->quote_Color->setReadOnly(true);
   colorBox(m_ui->quote_Color, m_struSettings.syn_QuoteText);

   if (m_struSettings.syn_QuoteWeight == QFont::Bold) {
      m_ui->quote_Bold_CB->setChecked(true);
   }
   m_ui->quote_Italic_CB->setChecked(m_struSettings.syn_QuoteItalic);

   /**/
   m_ui->comment_Color->setReadOnly(true);
   colorBox(m_ui->comment_Color, m_struSettings.syn_CommentText);

   if (m_struSettings.syn_CommentWeight == QFont::Bold) {
      m_ui->comment_Bold_CB->setChecked(true);
   }
   m_ui->comment_Italic_CB->setChecked(m_struSettings.syn_CommentItalic);

   /**/
   m_ui->mline_Color->setReadOnly(true);
   colorBox(m_ui->mline_Color, m_struSettings.syn_MLineText);

   if (m_struSettings.syn_MLineWeight == QFont::Bold) {
      m_ui->mline_Bold_CB->setChecked(true);
   }
   m_ui->mline_Italic_CB->setChecked(m_struSettings.syn_MLineItalic);

}

void Dialog_Colors::updateParser(bool newSettings)
{
   if (newSettings) {
      m_syntaxParser->processSyntax(m_struSettings);

   } else  {
      m_syntaxParser->processSyntax();
   }
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
   updateParser(true);
}

void Dialog_Colors::highBack_TB()
{
   QColor color = m_struSettings.colorHighBack;
   m_struSettings.colorHighBack = pickColor(color);

   //
   colorBox(m_ui->highBack_Color, m_struSettings.colorHighBack);
   updateParser(true);
}

void Dialog_Colors::key_TB()
{
   QColor color = m_struSettings.syn_KeyText;
   m_struSettings.syn_KeyText = pickColor(color);

   //
   colorBox(m_ui->key_Color, m_struSettings.syn_KeyText);
   updateParser(true);
}

void Dialog_Colors::type_TB()
{
   QColor color = m_struSettings.syn_TypeText;
   m_struSettings.syn_TypeText = pickColor(color);

   //
   colorBox(m_ui->type_Color, m_struSettings.syn_TypeText);
   updateParser(true);
}

void Dialog_Colors::class_TB()
{
   QColor color = m_struSettings.syn_ClassText;
   m_struSettings.syn_ClassText = pickColor(color);

   //
   colorBox(m_ui->class_Color, m_struSettings.syn_ClassText);
   updateParser(true);
}

void Dialog_Colors::func_TB()
{
   QColor color = m_struSettings.syn_FuncText;
   m_struSettings.syn_FuncText = pickColor(color);

   //
   colorBox(m_ui->func_Color, m_struSettings.syn_FuncText);
   updateParser(true);
}

void Dialog_Colors::quote_TB()
{
   QColor color = m_struSettings.syn_QuoteText;
   m_struSettings.syn_QuoteText = pickColor(color);

   //
   colorBox(m_ui->quote_Color, m_struSettings.syn_QuoteText);
   updateParser(true);
}

void Dialog_Colors::comment_TB()
{
   QColor color = m_struSettings.syn_CommentText;
   m_struSettings.syn_CommentText = pickColor(color);

   //
   colorBox(m_ui->comment_Color, m_struSettings.syn_CommentText);
   updateParser(true);
}

void Dialog_Colors::mline_TB()
{
   QColor color = m_struSettings.syn_MLineText;
   m_struSettings.syn_MLineText = pickColor(color);

   //
   colorBox(m_ui->mline_Color, m_struSettings.syn_MLineText);
   updateParser(true);
}


/**/
void Dialog_Colors::key_bold()
{
   if(m_ui->key_Bold_CB->isChecked())  {
      m_struSettings.syn_KeyWeight = QFont::Bold;
   } else   {
      m_struSettings.syn_KeyWeight = QFont::Normal;
   }

   updateParser(true);
}

void Dialog_Colors::key_italic()
{
   if(m_ui->key_Italic_CB->isChecked())  {
      m_struSettings.syn_KeyItalic = true;
   } else   {
      m_struSettings.syn_KeyItalic = false;
   }

   updateParser(true);
}

void Dialog_Colors::type_bold()
{
   if(m_ui->type_Bold_CB->isChecked())  {
      m_struSettings.syn_TypeWeight = QFont::Bold;
   } else   {
      m_struSettings.syn_TypeWeight = QFont::Normal;
   }

   updateParser(true);
}

void Dialog_Colors::type_italic()
{
   if(m_ui->type_Italic_CB->isChecked())  {
      m_struSettings.syn_TypeItalic = true;
   } else   {
      m_struSettings.syn_TypeItalic = false;
   }

   updateParser(true);
}

void Dialog_Colors::class_bold()
{
   if(m_ui->class_Bold_CB->isChecked())  {
      m_struSettings.syn_ClassWeight = QFont::Bold;
   } else   {
      m_struSettings.syn_ClassWeight = QFont::Normal;
   }

   updateParser(true);
}

void Dialog_Colors::class_italic()
{
   if(m_ui->class_Italic_CB->isChecked())  {
      m_struSettings.syn_ClassItalic = true;
   } else   {
      m_struSettings.syn_ClassItalic = false;
   }

   updateParser(true);
}

void Dialog_Colors::func_bold()
{
   if(m_ui->func_Bold_CB->isChecked())  {
      m_struSettings.syn_FuncWeight = QFont::Bold;
   } else   {
      m_struSettings.syn_FuncWeight = QFont::Normal;
   }

   updateParser(true);;
}

void Dialog_Colors::func_italic()
{
   if(m_ui->func_Italic_CB->isChecked())  {
      m_struSettings.syn_FuncItalic = true;
   } else   {
      m_struSettings.syn_FuncItalic = false;
   }

   updateParser(true);
}

void Dialog_Colors::quote_bold()
{
   if(m_ui->quote_Bold_CB->isChecked())  {
      m_struSettings.syn_QuoteWeight = QFont::Bold;
   } else   {
      m_struSettings.syn_QuoteWeight = QFont::Normal;
   }

   updateParser(true);
}

void Dialog_Colors::quote_italic()
{
   if(m_ui->quote_Italic_CB->isChecked())  {
      m_struSettings.syn_QuoteItalic = true;
   } else   {
      m_struSettings.syn_QuoteItalic = false;
   }

   updateParser(true);
}

void Dialog_Colors::comment_bold()
{
   if(m_ui->comment_Bold_CB->isChecked())  {
      m_struSettings.syn_CommentWeight = QFont::Bold;
   } else   {
      m_struSettings.syn_CommentWeight = QFont::Normal;
   }

   updateParser(true);
}

void Dialog_Colors::comment_italic()
{
   if(m_ui->comment_Italic_CB->isChecked())  {
      m_struSettings.syn_CommentItalic = true;
   } else   {
      m_struSettings.syn_CommentItalic = false;
   }

   updateParser(true);
}

void Dialog_Colors::mline_bold()
{
   if(m_ui->mline_Bold_CB->isChecked())  {
      m_struSettings.syn_MLineWeight = QFont::Bold;
   } else   {
      m_struSettings.syn_MLineWeight = QFont::Normal;
   }

   updateParser(true);
}

void Dialog_Colors::mline_italic()
{
   if(m_ui->mline_Italic_CB->isChecked())  {
      m_struSettings.syn_MLineItalic = true;
   } else   {
      m_struSettings.syn_MLineItalic = false;
   }

   updateParser(true);
}


//
void Dialog_Colors::Save()
{
   this->done(QDialog::Accepted);
}

void Dialog_Colors::Cancel()
{
   this->done(QDialog::Rejected);
}

struct Settings Dialog_Colors::get_Colors()
{
   return m_struSettings;
}
