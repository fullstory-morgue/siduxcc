AM_CPPFLAGS = $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_siduxcc_software.la

kcm_siduxcc_software_la_SOURCES = softwaredialog.ui siduxcc_software.cpp
kcm_siduxcc_software_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
kcm_siduxcc_software_la_LIBADD = $(LIB_KIO) ../libsiduxcc/libsiduxcc.la

noinst_HEADERS = siduxcc_software.h

xdg_apps_DATA = siduxcc_software.desktop

messages: rc.cpp
	$(EXTRACTRC) *.ui *.rc >> rc.cpp
	$(XGETTEXT) $(kcm_siduxcc_software_la_SOURCES) rc.cpp -o $(podir)/siduxccsoftware.pot
