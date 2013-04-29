include copperspice.am

# dist
distdir_custom=install/dist

# path to gcc
gccpath=/c/MingW

#path to hunspell
hunspell_path=/z/Hunspell/deploy/

# list of gcc libraries
gcclibs=libgcc_s_sjlj-1  libstdc++-6  libwinpthread-1

# list of CopperSpice libraries
cslibs=Core Gui Network Script WebKit

# windows only
hunspell_dll=$(hunspell_path)bin/libhunspell-1.3-0.dll
hunspell_la=$(hunspell_path)lib/libhunspell-1.3.la

# x11 only
# hunspell_dll=$(hunspell_path)bin/libhunspell.so
# hunspell_la=$(hunspell_path)lib/libhunspell.a

# disable console on window, location of includes, defines
AM_CXXFLAGS+=-mwindows -I/z/Hunspell/deploy/include/hunspell  -I..  -Wall

# target app name
deploy_PROGRAMS = Diamond

# copy the required CopperSpice and gcc files for deployment - windows only
install-data-hook:	
	mkdir -p $(deploydir)	
	cp --preserve=timestamps -u $(patsubst %,$(gccpath)/bin/%.dll,$(gcclibs))  $(deploydir)
	cp --preserve=timestamps -u $(patsubst %,$(CS_HOME)/bin/libCs%1-0.dll,$(cslibs))  $(deploydir)
#	cp --preserve=timestamps -u $(hunspell_dll)  $(deploydir)

# search path for source
VPATH = ../

# linking
Diamond_LDFLAGS=$(AM_LDFLAGS)   $(patsubst %,-lCs%1,$(cslibs))

# 'Diamond' must match the app name
Diamond_SOURCES = \
	about.cpp \
	dialog_advfind.cpp \
 	dialog_colors.cpp \
	dialog_config.cpp \
	dialog_find.cpp \
	dialog_fonts.cpp\
	dialog_get1.cpp \
	dialog_options.cpp \
    dialog_macro.cpp \
	dialog_print_opt.cpp \
	dialog_replace.cpp \
	dialog_symbols.cpp \
	diamond_edit.cpp \
	json.cpp \
	keylineedit.cpp \
	main.cpp \
    mainwindow.cpp \
    options.cpp \
    print.cpp \
    recent_files.cpp \
	recent_tabs.cpp \
	search.cpp \
    spell.cpp \
    spellcheck.cpp \
	split_window.cpp \
    syntax.cpp  \
    support.cpp \
    util.cpp		

#
noinst_HEADERS = \
	about.h \
	dialog_advfind.h \
	dialog_colors.h \
	dialog_config.h\
	dialog_find.h \
	dialog_fonts.h\
	dialog_get1.h  \
	dialog_options.h \
    dialog_macro.h \
	dialog_replace.h  \
	dialog_symbols.h \
	diamond_edit.h \
	keylineedit.h \
	mainwindow.h  \
    search.h \
	settings.h \
	spellcheck.h \
	util.h	

#
MOCS = \
	about.h \
	dialog_advfind.h \
	dialog_config.h \
	dialog_colors.h\
	dialog_find.h \
    dialog_fonts.h\
	dialog_get1.h  \
	dialog_options.h \
    dialog_macro.h \
	dialog_print_opt.h \
	dialog_replace.h  \
	dialog_symbols.h \
	diamond_edit.h  \
	keylineedit.h \
	mainwindow.h  \
	search.h \
	syntax.h

Diamond_LDADD=$(patsubst %.h,moc_%.$(OBJEXT),$(MOCS))   $(hunspell_la)

#
FORMS =	\
	forms/dialog_advfind.ui \
	forms/dialog_colors.ui \
	forms/dialog_config.ui \
	forms/dialog_find.ui \
	forms/dialog_fonts.ui\
	forms/dialog_get1.ui \
	forms/dialog_macro.ui \
    forms/dialog_options.ui \
	forms/dialog_print_opt.ui \
	forms/dialog_replace.ui \
	forms/dialog_symbols.ui \
	forms/mainwindow.ui

#
TRANSLATIONS =

#
RESOURCES = diamond.qrc

# use wilcard to obtain just the dependency list
qrc_diamond.cpp: diamond.qrc   $(wildcard ../resources/*)

# source distribution
nodist_Diamond_SOURCES = qrc_diamond.cpp

distdir: $(DISTFILES)	
	cd .. ; \
	test -d "$(distdir_custom)" || mkdir -p "$(distdir_custom)" ; \
	for f in $(DIST_SOURCES)  ; do install -D $$f $(distdir_custom)/$$f ; done ;\
	for f in $(HEADERS)   ; do install -D $$f $(distdir_custom)/$$f ; done ;\
	for f in $(FORMS)     ; do install -D $$f $(distdir_custom)/$$f ; done ;\
	mkdir -p $(distdir_custom)/bin ; \
	mkdir -p $(distdir_custom)/dictionary ; \
	mkdir -p $(distdir_custom)/resources  ; \
	mkdir -p $(distdir_custom)/syntax     ; \
	cp diamond.qrc    $(distdir_custom)/     ; \
	cp bin/*.am       $(distdir_custom)/bin/ ; \
	cp resources/*.*  $(distdir_custom)/resources/ ; \
	cp syntax/*.json  $(distdir_custom)/syntax/    ; \
	cp install/dictionary/*.*  $(distdir_custom)/dictionary/
	$(MAKE) $(AM_MAKEFLAGS) dist-hook
	
dist-hook:	
	cd .. ; \
	cp -r bin/$(distdir)/../*   $(distdir_custom) ; \
	rm -r bin/$(distdir)/../  ; \
	cd $(distdir_custom) ; \
	zip -r ../Diamond-$(VERSION).zip .

