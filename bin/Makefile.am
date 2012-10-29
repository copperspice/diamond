include copperspice.am

# path to gcc
gccpath=/c/MingW

# list of gcc libraries your app uses
gcclibs=libgcc_s_sjlj-1  libstdc++-6  libwinpthread-1

# list of CopperSpice libraries your app uses
cslibs=Core Gui Network Script WebKit 

hunspell_dll=/z/Hunspell/deploy/bin/libhunspell-1.3-0.dll
hunspell_la=/z/Hunspell/deploy/lib/libhunspell-1.3.la

# location of includes, libs, defines for compiling, turn off debugging
AM_CXXFLAGS+=-mwindows -std=c++11   -I/z/Hunspell/deploy/include/hunspell  -I..

# 'KitchenSink-Cs1' target app name
deploy_PROGRAMS = Diamond
    
# copy the required CopperSpice and gcc files for deployment - windows only
install-data-hook:	
	mkdir -p $(deploydir)	  
#	cp --preserve=timestamps  $(patsubst %,$(gccpath)/bin/%.dll,$(gcclibs))  $(deploydir)
#	cp --preserve=timestamps  $(patsubst %,$(CS_HOME)/bin/libCs%1-0.dll,$(cslibs))  $(deploydir)
#	cp --preserve=timestamps  $(hunspell_dll)  $(deploydir)
    
# search path for source
VPATH = ../

# linking
Diamond_LDFLAGS=$(AM_LDFLAGS)   $(patsubst %,-lCs%1,$(cslibs))

# 'Diamond' must match the app name ( dash had to be replaced )
Diamond_SOURCES = \
	about.cpp \
	dialog_advfind.cpp \
	dialog_colors.cpp \
	dialog_find.cpp \
	dialog_get1.cpp \
	dialog_options.cpp \
	dialog_print_opt.cpp \
	dialog_replace.cpp \
	dialog_symbols.cpp \
	diamond_edit.cpp \
	diamond_tab.cpp \
	json.cpp \
	keylineedit.cpp \
	main.cpp \
    mainwindow.cpp \
    options.cpp \
    print.cpp \
    recent_files.cpp \
    spell.cpp \
    spellcheck.cpp \
    syntax.cpp  \
    support.cpp \
    util.cpp
          
#              
noinst_HEADERS = \
	about.h \
	dialog_advfind.h \
	dialog_colors.h \
	dialog_find.h \
	dialog_get1.h  \
	dialog_options.h \
	dialog_replace.h  \
	dialog_symbols.h \
	diamond_edit.h \
	diamond_tab.h \
	keylineedit.h \
	mainwindow.h  \
	settings.h \
	spellcheck.h \
	util.h
	
   
                
# list of files that require compiling with moc
MOCS = \
	about.h \
	dialog_advfind.h \
	dialog_colors.h\
	dialog_find.h \
	dialog_get1.h  \
	dialog_options.h \
	dialog_print_opt.h \
	dialog_replace.h  \
	dialog_symbols.h \
	diamond_edit.h  \
	diamond_tab.h \
	keylineedit.h \
	mainwindow.h  \
	syntax.h
		   
    
Diamond_LDADD=$(patsubst %.h,moc_%.$(OBJEXT),$(MOCS))   $(hunspell_la)
           
#
FORMS =	\
	forms/dialog_advfind.ui \
	forms/dialog_colors.ui \
	forms/dialog_find.ui \
	forms/dialog_get1.ui \
	forms/dialog_options.ui \
	forms/dialog_print_opt.ui \
	forms/dialog_replace.ui \
	forms/dialog_symbols.ui \
	forms/mainwindow.ui
    
          
#
TRANSLATIONS =
  

#
RESOURCES  =  diamond.qrc

#    
Diamond_SOURCES += qrc_diamond.cpp   
    
# use wilcard to obtain just the dependency list
qrc_diamond.cpp: diamond.qrc   $(wildcard ../resources/*)
