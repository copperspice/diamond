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
	line_numbers.cpp \
	dialog_get1.cpp \
	dialog_options.cpp \
	main.cpp \
    mainwindow.cpp \
    recent_files.cpp \
    settings.cpp \
    support.cpp \
    util.cpp
          
#              
noinst_HEADERS = \
	dialog_get1.h  \
	dialog_options.h \
	line_numbers.h \
	mainwindow.h  \
	util.cpp
   
                
# list of files that require compiling with moc
MOCS = \
	dialog_get1.h  \
	dialog_options.h \
    line_numbers.h  \
	mainwindow.h
    
    
Diamond_LDADD=$(patsubst %.h,moc_%.$(OBJEXT),$(MOCS))    
           
#
FORMS =	\
	dialog_get1.ui \
	dialog_options.ui \
	mainwindow.ui
    
          
#
TRANSLATIONS =
  

#
RESOURCES  =  diamond.qrc

#    
Diamond_SOURCES += qrc_diamond.cpp   
    
# use wilcard to obtain just the dependency list
qrc_diamond.cpp: diamond.qrc   $(wildcard ../resources/*)
