AM_CPPFLAGS = $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_siduxcc_internet.la

kcm_siduxcc_internet_la_SOURCES = internetdialog.ui siduxcc_internet.cpp
kcm_siduxcc_internet_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
kcm_siduxcc_internet_la_LIBADD = $(LIB_KIO) ../libsiduxcc/libsiduxcc.la

noinst_HEADERS = siduxcc_internet.h

xdg_apps_DATA = siduxcc_internet.desktop

messages: rc.cpp
	$(EXTRACTRC) *.ui *.rc >> rc.cpp
	$(XGETTEXT) $(kcm_siduxcc_internet_la_SOURCES) rc.cpp -o $(podir)/siduxccinternet.pot
