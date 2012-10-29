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
   bool    showSpaces;
   bool    showEOL;
   bool    autoLoad;
   QString pathSyntax;
   QString pathPrior;
   QString formatDate;
   QString formatTime;
   QString dictMain;
   QString dictUser;
   QString aboutUrl;

   QFont   font;
   QColor  colorText;
   QColor  colorBack;
   QColor  colorHighText;
   QColor  colorHighBack;

   QString key_selectLine;
   QString key_selectWord;
   QString key_selectBlock;
   QString key_upper;
   QString key_lower; 
   QString key_indentIncr;
   QString key_indentDecr;
   QString key_deleteEOL;
   QString key_columnMode;
   QString key_goLine;
   QString key_macroPlay;
   QString key_spellCheck;

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

   QString key_selectLine;
   QString key_selectWord;
   QString key_selectBlock;
   QString key_upper;
   QString key_lower;
   QString key_indentIncr;
   QString key_indentDecr;
   QString key_deleteEOL;
   QString key_columnMode;
   QString key_goLine;
   QString key_macroPlay;
   QString key_spellCheck;
};

struct PrintSettings {   
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

   QFont fontHeader;
   QFont fontFooter;
   QFont fontText;
};

enum SyntaxTypes {SYN_C, SYN_CLIPPER, SYN_CSS, SYN_DOX, SYN_HTML, SYN_JAVA, SYN_JS,
                  SYN_JSON, SYN_MAKE, SYN_NSIS, SYN_TEXT, SYN_SHELL_S, SYN_PERL,
                  SYN_PHP, SYN_PYTHON, SYN_NONE };

#endif
