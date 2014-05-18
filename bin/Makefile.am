include copperspice.am

# dist (windows)
distdir_custom=install_win/dist

# path to gcc (windows)
gccpath=/c/MingW/bin

# list of gcc libraries (windows)
gcclibs=libgcc_s_sjlj-1  libstdc++-6  libwinpthread-1

# list of CopperSpice libraries
cslibs=Core Gui Network WebKit

# location of includes
AM_CXXFLAGS+=-I..

if OSTYPE_LINUX
   hunspell_la=`pkg-config hunspell --libs`

   AM_CXXFLAGS+=`pkg-config hunspell --cflags`
endif

if OSTYPE_DARWIN
   hunspell_la=/opt/local/lib/libhunspell-1.3.la

   AM_CXXFLAGS+=-I/opt/local/include/hunspell
endif

if OSTYPE_WIN
   # path to hunspell
   hunspell_path=../hunspell

   hunspell_dll=$(hunspell_path)/bin/libhunspell-1.3-0.dll
   hunspell_la=$(hunspell_path)/lib/libhunspell-1.3.dll.a

   # disable console on window
   AM_CXXFLAGS+=-mwindows -I$(hunspell_path)/include

   # location for 'make install'
   installdir=$(abs_top_builddir)/install
endif

# target app name
bin_PROGRAMS = Diamond

# copy required CopperSpice and gcc files for deployment
.PHONY: deploy

if OSTYPE_LINUX
deploy: $(bin_PROGRAMS)
	mkdir -p $(deploydir)
	mkdir -p $(deploydir)/dictionary
	mkdir -p $(deploydir)/help
	mkdir -p $(deploydir)/syntax

	$(LIBTOOL) --mode=install cp $(bin_PROGRAMS) $(deploydir)

	rsync -W -a $(patsubst %,$(CS_HOME)/lib/libCs%1.so*,$(cslibs)) $(deploydir)
	rsync -W -a $(top_srcdir)/source_extra/{dictionary,help,syntax} $(deploydir)
endif

if OSTYPE_DARWIN

deployapp=$(deploydir)/$(bin_PROGRAMS).app/Contents

deploy: $(bin_PROGRAMS)
	mkdir -p $(deployapp)/Frameworks
	mkdir -p $(deployapp)/MacOS
	mkdir -p $(deployapp)/Resources

	$(LIBTOOL) --mode=install cp $(bin_PROGRAMS) $(deployapp)/MacOS
	
	rm -f $(deployapp)/Info.plist
	sed -e "s,@ICON@,,g" \
	    -e "s,@TYPEINFO@,????,g" \
	    -e "s,@EXECUTABLE@,$(deploy_PROGRAMS),g" \
	$(CS_HOME)/mac/Info.plist.app > $(deployapp)/Info.plist
	
	rm -f $(deployapp)/PkgInfo
	echo "APPL????" > $(deployapp)/PkgInfo
	
	touch $(deployapp)/Resources/empty.lproj
	cp -r $(CS_HOME)/mac/qt_menu.nib $(deployapp)/Resources
	
	rsync -W -a $(patsubst %,$(CS_HOME)/lib/libCs%1.0.dylib,$(cslibs)) $(deployapp)/Frameworks

	for libname in $(patsubst %,libCs%1.0.dylib,$(cslibs)) ; do \
	\
	install_name_tool -change $(CS_HOME)/lib/$$libname \
	@executable_path/../Frameworks/$$libname $(deployapp)/MacOS/$(bin_PROGRAMS); \
	\
	for templib in $(patsubst %,libCs%1.0.dylib,$(cslibs)) ; do \
	install_name_tool -change $(CS_HOME)/lib/$$templib \
	@executable_path/../Frameworks/$$templib $(deployapp)/Frameworks/$$libname \
	; done \
	\
	; done

   # generate dmg	
	hdiutil create -ov -srcfolder $(deploydir) -format UDBZ -volname $(bin_PROGRAMS) ../$(bin_PROGRAMS).dmg
	hdiutil internet-enable -yes ../$(bin_PROGRAMS).dmg
endif

if OSTYPE_WIN
deploy: $(bin_PROGRAMS)
	mkdir -p $(deploydir)

	$(LIBTOOL) --mode=install cp $(bin_PROGRAMS) $(deploydir)

	rsync -W -a --modify-window=2  $(patsubst %,$(gccpath)/%.dll,$(gcclibs))  $(deploydir)
	rsync -W -a --modify-window=2  $(patsubst %,$(CS_HOME)/bin/libCs%1-0.dll,$(cslibs))  $(deploydir)
	rsync -W -a --modify-window=2  $(hunspell_dll)  $(deploydir)
endif

# search path for source
VPATH = ../

# linking
Diamond_LDFLAGS=$(AM_LDFLAGS) $(patsubst %,-lCs%1,$(cslibs)) $(hunspell_la)

Diamond_SOURCES = \
    about.cpp \
    dialog_advfind.cpp \
    dialog_buffer.cpp \
    dialog_colors.cpp \
    dialog_config.cpp \
    dialog_find.cpp \
    dialog_fonts.cpp\
    dialog_getline.cpp \
    dialog_macro.cpp \
    dialog_open.cpp \
    dialog_options.cpp \
    dialog_preset.cpp \
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

noinst_HEADERS = \
    about.h \
    dialog_advfind.h \
    dialog_buffer.h \
    dialog_colors.h \
    dialog_config.h\
    dialog_find.h \
    dialog_fonts.h\
    dialog_getline.h  \
    dialog_macro.h \
    dialog_options.h \
    dialog_open.h \
    dialog_preset.h \
    dialog_print_opt.h  \
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
    forms/dialog_preset.ui\
    forms/dialog_print_opt.ui \
    forms/dialog_replace.ui \
    forms/dialog_symbols.ui \
    forms/dialog_xp_getdir.ui \
    forms/mainwindow.ui

TRANSLATIONS =

RESOURCES = diamond.qrc

# use wilcard to obtain just the dependency list
qrc_diamond.cpp: diamond.qrc   $(wildcard ../resources/*)

# required to compile qrc
nodist_Diamond_SOURCES=qrc_diamond.cpp

# install
if OSTYPE_LINUX
   dictionarydir=$(pkgdatadir)/dictionary
   dictionary_DATA=$(wildcard $(top_srcdir)/source_extra/dictionary/*)

   syntaxdir=$(pkgdatadir)/syntax
   syntax_DATA=$(wildcard $(top_srcdir)/source_extra/syntax/*)

   helpdir=$(pkgdatadir)/help
   help_DATA=$(wildcard $(top_srcdir)/source_extra/help/*.*)
   helpsearchdir=$(pkgdatadir)/help/search
   helpsearch_DATA=$(wildcard $(top_srcdir)/source_extra/help/search/*.*)
endif

if OSTYPE_DARWIN
   dictionarydir=$(pkgdatadir)/dictionary
   dictionary_DATA=$(wildcard $(top_srcdir)/source_extra/dictionary/*)

   syntaxdir=$(pkgdatadir)/syntax
   syntax_DATA=$(wildcard $(top_srcdir)/source_extra/syntax/*)

   helpdir=$(pkgdatadir)/help
   help_DATA=$(wildcard $(top_srcdir)/source_extra/help/*.*)
   helpsearchdir=$(pkgdatadir)/help/search
   helpsearch_DATA=$(wildcard $(top_srcdir)/source_extra/help/search/*.*)
endif

if OSTYPE_WIN
   dictionarydir=$(prefix)/dictionary
   dictionary_DATA=$(wildcard $(top_srcdir)/source_extra/dictionary/*)
   
   syntaxdir=$(prefix)/syntax
   syntax_DATA=$(wildcard $(top_srcdir)/source_extra/syntax/*)
   
   helpdir=$(prefix)/help
   help_DATA=$(wildcard $(top_srcdir)/source_extra/help/*.*)
   helpsearchdir=$(prefix)/help/search
   helpsearch_DATA=$(wildcard $(top_srcdir)/source_extra/help/search/*.*)

install-exec-hook:
	rsync -W -a --modify-window=2  $(patsubst %,$(gccpath)/%.dll,$(gcclibs))  $(bindir)
	rsync -W -a --modify-window=2  $(patsubst %,$(CS_HOME)/bin/libCs%1-0.dll,$(cslibs))  $(bindir)
	rsync -W -a --modify-window=2  $(hunspell_dll)  $(bindir)
endif


# distribution
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

