AM_CPPFLAGS = $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_siduxcc_services.la

kcm_siduxcc_services_la_SOURCES = displaydialog.ui siduxcc_services.cpp
kcm_siduxcc_services_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
kcm_siduxcc_services_la_LIBADD = $(LIB_KIO) ../libsiduxcc/libsiduxcc.la


noinst_HEADERS = siduxcc_services.h

xdg_apps_DATA = siduxcc_services.desktop

messages: rc.cpp
	$(EXTRACTRC) *.ui *.rc >> rc.cpp
	$(XGETTEXT) $(kcm_siduxcc_services_la_SOURCES) rc.cpp -o $(podir)/siduxccservices.pot
