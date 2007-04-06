AM_CPPFLAGS = $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_siduxcc_info.la

kcm_siduxcc_info_la_SOURCES = infodialog.ui siduxcc_info.cpp process.cpp
kcm_siduxcc_info_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
kcm_siduxcc_info_la_LIBADD = $(LIB_KIO)

noinst_HEADERS = siduxcc_info.h

xdg_apps_DATA = siduxcc_info.desktop

messages: rc.cpp
	$(EXTRACTRC) *.ui *.rc >> rc.cpp
	$(XGETTEXT) $(kcm_siduxcc_info_la_SOURCES) rc.cpp -o $(podir)/siduxccinfo.pot

