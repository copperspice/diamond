/**************************************************************************
*
* Copyright (c) 2012-2014 Barbara Geller
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QColor>
#include <QFont>
#include <QString>

struct Settings {
   int     tabSpacing;
   bool    useSpaces;
   bool    showLineHighlight;
   bool    showLineNumbers;
   bool    isColumnMode;
   bool    isSpellCheck;
   bool    isWordWrap;  
   bool    show_Spaces;
   bool    show_Breaks;
   bool    autoLoad;
   QString pathSyntax;
   QString pathPrior;
   QString formatDate;
   QString formatTime;
   QString dictMain;
   QString dictUser;
   QString aboutUrl;

   QFont   fontNormal;
   QFont   fontColumn;
   QColor  colorText;
   QColor  colorBack;
   QColor  colorHighText;
   QColor  colorHighBack;

   QString key_open;
   QString key_close;   
   QString key_save;
   QString key_saveAs;
   QString key_print;
   QString key_undo;
   QString key_redo;
   QString key_cut;
   QString key_copy;
   QString key_paste;
   QString key_selectAll;
   QString key_find;
   QString key_replace;
   QString key_findNext;
   QString key_findPrev;
   QString key_goTop;
   QString key_goBottom;  
   QString key_newTab;

   QString key_printPreview;
   QString key_reload;
   QString key_selectLine;
   QString key_selectWord;
   QString key_selectBlock;
   QString key_upper;
   QString key_lower; 
   QString key_indentIncr;
   QString key_indentDecr;
   QString key_deleteLine;
   QString key_deleteEOL;
   QString key_columnMode;
   QString key_goLine;
   QString key_show_Spaces;
   QString key_show_Breaks;
   QString key_macroPlay;
   QString key_spellCheck;
   QString key_copyBuffer;

   int    syn_KeyWeight;
   bool   syn_KeyItalic;
   QColor syn_KeyText;
   int    syn_TypeWeight;
   bool   syn_TypeItalic;
   QColor syn_TypeText;
   int    syn_ClassWeight;
   bool   syn_ClassItalic;
   QColor syn_ClassText;
   int    syn_FuncWeight;
   bool   syn_FuncItalic;
   QColor syn_FuncText;
   int    syn_QuoteWeight;
   bool   syn_QuoteItalic;
   QColor syn_QuoteText;
   int    syn_CommentWeight;
   bool   syn_CommentItalic;
   QColor syn_CommentText;
   int    syn_MLineWeight;
   bool   syn_MLineItalic;
   QColor syn_MLineText;
};

struct Options {
   int     tabSpacing;
   bool    useSpaces;   
   bool    autoLoad;
   QString formatDate;
   QString formatTime;
   QString dictMain;
   QString dictUser;
   QString pathSyntax;
   QString aboutUrl;   

   QString key_open;
   QString key_close;   
   QString key_save;
   QString key_saveAs;
   QString key_print;
   QString key_undo;
   QString key_redo;
   QString key_cut;
   QString key_copy;
   QString key_paste;
   QString key_selectAll;
   QString key_find;
   QString key_replace;
   QString key_findNext;
   QString key_findPrev;
   QString key_goTop;
   QString key_goBottom;
   QString key_newTab;

   QString key_printPreview;
   QString key_reload;
   QString key_selectLine;
   QString key_selectWord;
   QString key_selectBlock;
   QString key_upper;
   QString key_lower;
   QString key_indentIncr;
   QString key_indentDecr;
   QString key_deleteLine;
   QString key_deleteEOL;
   QString key_columnMode;
   QString key_goLine;
   QString key_show_Spaces;
   QString key_show_Breaks;
   QString key_macroPlay;
   QString key_spellCheck;
   QString key_copyBuffer;
};

struct PrintSettings {   
   bool    lineNumbers;
   bool    printHeader;
   bool    printFooter;
   QString header_left;
   QString header_center;
   QString header_right;
   QString header_line2;
   QString footer_left;
   QString footer_center;
   QString footer_right;
   QString footer_line2;

   double marLeft;
   double marTop;
   double marRight;
   double marBottom;
   double hdrGap;    

   QFont fontHeader;
   QFont fontFooter;
   QFont fontText;
};

struct Arugments{
   bool flag_noAutoLoad = false;
   bool flag_noSaveConfig = false;
};

enum SyntaxTypes {SYN_C, SYN_CLIPPER, SYN_CSS, SYN_DOX, SYN_HTML, SYN_JAVA, SYN_JS,
                  SYN_JSON, SYN_MAKE, SYN_NSIS, SYN_TEXT, SYN_SHELL_S, SYN_PERL,
                  SYN_PHP, SYN_PYTHON, SYN_NONE };

#endif
