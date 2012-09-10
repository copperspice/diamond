include copperspice.am

# path to gcc
gccpath=/c/MingW

# list of gcc libraries your app uses
gcclibs=libgcc_s_sjlj-1  libstdc++-6  libwinpthread-1

# list of CopperSpice libraries your app uses
cslibs=Core Gui

# location of includes, libs, defines for compiling, turn off debugging
AM_CXXFLAGS+=-mwindows

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
	main.cpp \
    mainwindow.cpp \
    settings.cpp \
    support.cpp \
    util.cpp
          
#              
noinst_HEADERS = \
	mainwindow.h  \
	util.cpp
   
                
# list of files that require compiling with moc
MOCS = \
	mainwindow.h
    
    
Diamond_LDADD=$(patsubst %.h,moc_%.$(OBJEXT),$(MOCS))    
           
#
FORMS =	mainwindow.ui
    
          
#
TRANSLATIONS =
  

#
RESOURCES  =  diamond.qrc

#    
Diamond_SOURCES += qrc_diamond.cpp   
    
# use wilcard to obtain just the dependency list
qrc_diamond.cpp: diamond.qrc   $(wildcard ../resources/*)
