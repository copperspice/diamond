include copperspice.am

# path to gcc
gccpath=/c/MingW

# list of gcc libraries your app uses
gcclibs=libgcc_s_sjlj-1  libstdc++-6  libwinpthread-1

# list of CopperSpice libraries your app uses
cslibs=Core Gui Xml

# location of includes, libs, defines for compiling, turn off debugging
AM_CXXFLAGS+=-mwindows -std=c++11

# 'KitchenSink-Cs1' target app name
deploy_PROGRAMS = Diamond
    
# copy the required CopperSpice and gcc files for deployment - windows only
install-data-hook:	
	mkdir -p $(deploydir)	  
	cp --preserve=timestamps  $(patsubst %,$(gccpath)/bin/%.dll,$(gcclibs))  $(deploydir)
	cp --preserve=timestamps  $(patsubst %,$(CS_HOME)/bin/libCs%1-0.dll,$(cslibs))  $(deploydir)
    
# search path for source
VPATH = ../

# linking
Diamond_LDFLAGS=$(AM_LDFLAGS)   $(patsubst %,-lCs%1,$(cslibs))

# 'Diamond' must match the app name ( dash had to be replaced )
Diamond_SOURCES = \
	dialog_colors.cpp \
	dialog_find.cpp \
	dialog_get1.cpp \
	dialog_options.cpp \
	dialog_symbols.cpp \
	diamond_edit.cpp \
	diamond_tab.cpp \
	main.cpp \
    mainwindow.cpp \
    recent_files.cpp \
    settings.cpp \
    syntax.cpp  \
    support.cpp \
    util.cpp
          
#              
noinst_HEADERS = \
	dialog_colors.h \
	dialog_find.h \
	dialog_get1.h  \
	dialog_options.h \
	dialog_symbols.h \
	diamond_edit.h \
	diamond_tab.h \
	mainwindow.h  \
	syntax.h  \
	util.h
   
                
# list of files that require compiling with moc
MOCS = \
	dialog_colors.h\
	dialog_find.h \
	dialog_get1.h  \
	dialog_options.h \
	dialog_symbols.h \
	diamond_edit.h  \
	diamond_tab.h \
	mainwindow.h   \
	syntax.h
    
    
Diamond_LDADD=$(patsubst %.h,moc_%.$(OBJEXT),$(MOCS))    
           
#
FORMS =	\
	dialog_colors.ui \
	dialog_find.ui \
	dialog_get1.ui \
	dialog_options.ui \
	dialog_symbols.ui \
	mainwindow.ui
    
          
#
TRANSLATIONS =
  

#
RESOURCES  =  diamond.qrc

#    
Diamond_SOURCES += qrc_diamond.cpp   
    
# use wilcard to obtain just the dependency list
qrc_diamond.cpp: diamond.qrc   $(wildcard ../resources/*)
