include copperspice.am

# dist
distdir_custom=source_dist/dist/Diamond-$(VERSION)

# path to gcc retrieved from configure (windows)
gccpath=$(dir $(shell which $(word 1,$(CC))))

# list of gcc libraries (windows)  sjlj  or seh
gcclibs=libgcc_s_*-1  libstdc++-6  libwinpthread-1

# list of CopperSpice libraries
cslibs=Core Gui Network WebKit

# location of includes
AM_CXXFLAGS+=-I../src

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
   hunspell_path=../hunspell_lib

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
deploy : all
	mkdir -p $(deploydir)
	mkdir -p $(deploydir)/dictionary
	mkdir -p $(deploydir)/help
	mkdir -p $(deploydir)/syntax

	$(LIBTOOL) --mode=install cp $(bin_PROGRAMS) $(deploydir)

	rsync -W -a $(patsubst %,$(CS_HOME)/lib/libCs%1.so*,$(cslibs)) $(deploydir)
	rsync -W -a $(top_srcdir)/source_extra/{dictionary,help,syntax} $(deploydir)
	rsync -W -a $(top_srcdir)/resources/diamond.png $(deploydir)
endif

if OSTYPE_DARWIN
deployapp=$(deploydir)/$(bin_PROGRAMS).app/Contents

deploy : all
	mkdir -p $(deployapp)/Frameworks
	mkdir -p $(deployapp)/MacOS
	mkdir -p $(deployapp)/Resources

	$(LIBTOOL) --mode=install cp $(bin_PROGRAMS) $(deployapp)/MacOS

	rm -f $(deployapp)/Info.plist
	sed -e "s,@ICON@,diamond.icns,g" \
	    -e "s,@TYPEINFO@,????,g" \
	    -e "s,@EXECUTABLE@,$(bin_PROGRAMS),g" \
	    -e "s,com.yourcompany.,com.copperspice.,g" \
	$(CS_HOME)/mac/Info.plist.app > $(deployapp)/Info.plist

	rm -f $(deployapp)/PkgInfo
	echo "APPL????" > $(deployapp)/PkgInfo

	touch $(deployapp)/Resources/empty.lproj
	cp -r $(CS_HOME)/mac/qt_menu.nib $(deployapp)/Resources

	cp $(top_srcdir)/resources/diamond.icns $(deployapp)/Resources

	cp -r $(top_srcdir)/source_extra/dictionary  $(deployapp)/Resources
	cp -r $(top_srcdir)/source_extra/help        $(deployapp)/Resources
	cp -r $(top_srcdir)/source_extra/syntax      $(deployapp)/Resources

	rsync -W -a /opt/local/lib/libhunspell-1.3.0.dylib  $(deployapp)/Frameworks
	install_name_tool -change /opt/local/lib/libhunspell-1.3.0.dylib \
	@executable_path/../Frameworks/libhunspell-1.3.0.dylib  $(deployapp)/MacOS/$(bin_PROGRAMS)

	rsync -W -a $(patsubst %,$(CS_HOME)/lib/libCs%1.0.dylib,$(cslibs)) $(deployapp)/Frameworks

	for libname in $(patsubst %,libCs%1.0.dylib,$(cslibs)) ; do \
	\
	install_name_tool -change $(CS_HOME)/lib/$$libname \
	@executable_path/../Frameworks/$$libname  $(deployapp)/MacOS/$(bin_PROGRAMS); \
	\
	for templib in $(patsubst %,libCs%1.0.dylib,$(cslibs)) ; do \
	install_name_tool -change $(CS_HOME)/lib/$$templib \
	@executable_path/../Frameworks/$$templib  $(deployapp)/Frameworks/$$libname \
	; done \
	\
	; done

   # generate dmg	
	hdiutil create -ov -srcfolder $(deploydir) -format UDBZ -volname $(bin_PROGRAMS) ../$(bin_PROGRAMS)-$(VERSION).dmg
	hdiutil internet-enable -yes ../$(bin_PROGRAMS)-$(VERSION).dmg
endif

if OSTYPE_WIN
deploy : all
	mkdir -p $(deploydir)

	$(LIBTOOL) --mode=install cp $(bin_PROGRAMS) $(deploydir)

	rsync -W -a --modify-window=2  $(top_srcdir)/resources/diamond.ico $(deploydir)

	rsync -W -a --modify-window=2  $(patsubst %,$(gccpath)%.dll,$(gcclibs))  $(deploydir)
	rsync -W -a --modify-window=2  $(patsubst %,$(CS_HOME)/bin/libCs%1-0.dll,$(cslibs))  $(deploydir)
	rsync -W -a --modify-window=2  $(hunspell_dll)  $(deploydir)
endif

# search path for source
VPATH = ../

# linking
Diamond_LDFLAGS=$(AM_LDFLAGS) $(patsubst %,-lCs%1,$(cslibs)) $(hunspell_la)

if OSTYPE_WIN
   # used by xp only
   Diamond_LDFLAGS+=-lnetapi32 -lmpr
endif

Diamond_SOURCES = \
    src/about.cpp \
    src/dialog_advfind.cpp \
    src/dialog_buffer.cpp \
    src/dialog_colors.cpp \
    src/dialog_config.cpp \
    src/dialog_find.cpp \
    src/dialog_fonts.cpp\
    src/dialog_getline.cpp \
    src/dialog_macro.cpp \
    src/dialog_open.cpp \
    src/dialog_options.cpp \
    src/dialog_preset.cpp \
    src/dialog_print_opt.cpp \
    src/dialog_replace.cpp \
    src/dialog_symbols.cpp \
    src/dialog_xp_getdir.cpp \
    src/diamond_edit.cpp \
    src/json.cpp \
    src/keylineedit.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/options.cpp \
    src/print.cpp \
    src/recent_files.cpp \
    src/recent_tabs.cpp \
    src/search.cpp \
    src/spell.cpp \
    src/spellcheck.cpp \
    src/split_window.cpp \
    src/syntax.cpp  \
    src/support.cpp \
    src/util.cpp

noinst_HEADERS = \
    src/about.h \
    src/dialog_advfind.h \
    src/dialog_buffer.h \
    src/dialog_colors.h \
    src/dialog_config.h\
    src/dialog_find.h \
    src/dialog_fonts.h\
    src/dialog_getline.h  \
    src/dialog_macro.h \
    src/dialog_options.h \
    src/dialog_open.h \
    src/dialog_preset.h \
    src/dialog_print_opt.h  \
    src/dialog_replace.h  \
    src/dialog_symbols.h \
    src/dialog_xp_getdir.h \
    src/diamond_edit.h \
    src/keylineedit.h \
    src/mainwindow.h  \
    src/search.h \
    src/settings.h \
    src/spellcheck.h \
    src/syntax.h \
    src/util.h

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
	rsync -W -a --modify-window=2  $(top_srcdir)/resources/diamond.ico $(bindir)

	rsync -W -a --modify-window=2  $(patsubst %,$(gccpath)%.dll,$(gcclibs))  $(bindir)
	rsync -W -a --modify-window=2  $(patsubst %,$(CS_HOME)/bin/libCs%1-0.dll,$(cslibs))  $(bindir)
	rsync -W -a --modify-window=2  $(hunspell_dll)  $(bindir)
endif


# distribution
distdir: $(DISTFILES)
	cd .. ; \
	test -d "$(distdir_custom)" || mkdir -p "$(distdir_custom)" ; \
	mkdir -p $(distdir_custom)/bin                              ; \
	mkdir -p $(distdir_custom)/resources                        ; \
	mkdir -p $(distdir_custom)/source_extra/dictionary          ; \
	mkdir -p $(distdir_custom)/source_extra/help                ; \
	mkdir -p $(distdir_custom)/source_extra/syntax              ; \
\
	for f in $(DIST_SOURCES)  ; do install -D $$f $(distdir_custom)/$$f ; done   ;\
	for f in $(HEADERS)       ; do install -D $$f $(distdir_custom)/$$f ; done   ;\
	for f in $(FORMS)         ; do install -D $$f $(distdir_custom)/$$f ; done   ;\
\
	cp diamond.qrc                   $(distdir_custom)/                          ; \
	cp read_me.txt                   $(distdir_custom)/                          ; \
	cp bin/*.am                      $(distdir_custom)/bin/                      ; \
	cp bin/Makefile.in               $(distdir_custom)/bin/                      ; \
	cp resources/*                   $(distdir_custom)/resources/                ; \
	cp source_extra/dictionary/*     $(distdir_custom)/source_extra/dictionary/  ; \
	cp source_extra/syntax/*.json    $(distdir_custom)/source_extra/syntax/      ; \
	cp -r source_extra/help/*        $(distdir_custom)/source_extra/help/

	$(MAKE) $(AM_MAKEFLAGS) dist-hook

# distdir is a temp folder
dist-hook:
	cd ..  ; \
\
	cp -r bin/$(distdir)/../*   $(distdir_custom) ; \
	cd $(distdir_custom)/..                       ; \
\
	rm  -f   ../Diamond-$(VERSION).zip         ; \
	zip -r   ../Diamond-$(VERSION).zip .       ; \
	rm  -f   ../Diamond-$(VERSION).tar.bz2     ; \
	tar -cjf ../Diamond-$(VERSION).tar.bz2 .

