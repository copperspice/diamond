include copperspice.am

# dist windows
distdir_custom=install_win/dist

# path to gcc
gccpath=/c/MingW

# path to hunspell
hunspell_path=../hunspell

# list of gcc libraries
gcclibs=libgcc_s_sjlj-1  libstdc++-6  libwinpthread-1

# list of CopperSpice libraries
cslibs=Core Gui Network WebKit

# dll for runtime, la for linking
hunspell_dll=$(hunspell_path)/bin/libhunspell-1.3-0.dll
hunspell_la=$(hunspell_path)/lib/libhunspell-1.3.dll.a

# disable console on window, location of includes, defines
AM_CXXFLAGS+=-mwindows -I$(hunspell_path)/include  -I..  -Wall

# target app name
deploy_PROGRAMS = Diamond_Cs4

# copy the required CopperSpice and gcc files for deployment - windows only
install-data-hook:
	mkdir -p $(deploydir)

	rsync -W -a --modify-window=2  $(patsubst %,$(gccpath)/bin/%.dll,$(gcclibs))  $(deploydir)
	rsync -W -a --modify-window=2  $(patsubst %,$(CS_HOME)/bin/libCs%1-0.dll,$(cslibs))  $(deploydir)
	rsync -W -a --modify-window=2  $(hunspell_dll)  $(deploydir)

# search path for source
VPATH = ../

# linking
Diamond_Cs4_LDFLAGS=$(AM_LDFLAGS)   $(patsubst %,-lCs%1,$(cslibs))
Diamond_Cs4_LDADD=$(hunspell_la)

# 'Diamond' must match the app name
Diamond_Cs4_SOURCES = \
    about.cpp \
    dialog_advfind.cpp \
    dialog_buffer.cpp \
    dialog_colors.cpp \
    dialog_config.cpp \
    dialog_find.cpp \
    dialog_fonts.cpp\
    dialog_getline.cpp \
    dialog_open.cpp \
    dialog_options.cpp \
    dialog_macro.cpp \
    dialog_print_opt.cpp \
    dialog_replace.cpp \
    dialog_symbols.cpp \
    dialog_xp_getdir.cpp \
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
    dialog_buffer.h \
    dialog_colors.h \
    dialog_config.h\
    dialog_find.h \
    dialog_fonts.h\
    dialog_getline.h  \
    dialog_options.h \
    dialog_open.h \
    dialog_macro.h \
    dialog_replace.h  \
    dialog_symbols.h \
    dialog_xp_getdir.h \
    diamond_edit.h \
    keylineedit.h \
    mainwindow.h  \
    search.h \
    settings.h \
    spellcheck.h \
    util.h

#
FORMS = \
    forms/dialog_advfind.ui \
    forms/dialog_buffer.ui \
    forms/dialog_colors.ui \
    forms/dialog_config.ui \
    forms/dialog_find.ui \
    forms/dialog_fonts.ui\
    forms/dialog_getline.ui \
    forms/dialog_macro.ui \
    forms/dialog_open.ui \
    forms/dialog_options.ui \
    forms/dialog_print_opt.ui \
    forms/dialog_replace.ui \
    forms/dialog_symbols.ui \
    forms/dialog_xp_getdir.ui \
    forms/mainwindow.ui

#
TRANSLATIONS =

#
RESOURCES = diamond.qrc

# use wilcard to obtain just the dependency list
qrc_diamond.cpp: diamond.qrc   $(wildcard ../resources/*)

# source distribution
nodist_Diamond_Cs4_SOURCES = qrc_diamond.cpp

distdir: $(DISTFILES)
	cd .. ; \
	test -d "$(distdir_custom)" || mkdir -p "$(distdir_custom)" ; \
	for f in $(DIST_SOURCES)  ; do install -D $$f $(distdir_custom)/$$f ; done ;\
	for f in $(HEADERS)       ; do install -D $$f $(distdir_custom)/$$f ; done ;\
	for f in $(FORMS)         ; do install -D $$f $(distdir_custom)/$$f ; done ;\
	mkdir -p $(distdir_custom)/bin           ; \
	mkdir -p $(distdir_custom)/dictionary    ; \
	mkdir -p $(distdir_custom)/help          ; \
	mkdir -p $(distdir_custom)/hunspell      ; \
	mkdir -p $(distdir_custom)/resources     ; \
	mkdir -p $(distdir_custom)/syntax        ; \
	cp diamond.qrc                   $(distdir_custom)/                 ; \
	cp bin/*.am                      $(distdir_custom)/bin/             ; \
	cp resources/*                   $(distdir_custom)/resources/       ; \
	cp install_files/dictionary/*    $(distdir_custom)/dictionary/      ; \
	cp install_files/syntax/*.json   $(distdir_custom)/syntax/          ; \
	cp -r install_files/help/*	      $(distdir_custom)/help/            ; \
	cp -r $(hunspell_path)/*         $(distdir_custom)/hunspell/        ; \
	find $(distdir_custom)/hunspell/ -name \*.exe -exec rm \{\} \;      ; \
	find $(distdir_custom)/hunspell/ -name \*.o   -exec rm \{\} \;      ; \
	find $(distdir_custom)/hunspell/ -name \*.lo  -exec rm \{\} \;      ; \
	find $(distdir_custom)/hunspell/ -name .libs  -exec rm -r \{\} \;   ; \
	find $(distdir_custom)/hunspell/ -name .deps  -exec rm -r \{\} \;   ; \
	rm -r $(distdir_custom)/hunspell/deploy/share
	$(MAKE) $(AM_MAKEFLAGS) dist-hook

dist-hook:
	cd .. ; \
	cp -r bin/$(distdir)/../*   $(distdir_custom) ; \
	rm -r bin/$(distdir)/../   ; \
	cd $(distdir_custom)       ; \
	rm -f  ../Diamond-$(VERSION).zip  ; \
	zip -r ../Diamond-$(VERSION).zip .

