AM_CPPFLAGS = $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_siduxcc_network.la

kcm_siduxcc_network_la_SOURCES = displaydialog.ui siduxcc_network.cpp
kcm_siduxcc_network_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
kcm_siduxcc_network_la_LIBADD = $(LIB_KIO) ../libsiduxcc/libsiduxcc.la

noinst_HEADERS = siduxcc_network.h

xdg_apps_DATA = siduxcc_network.desktop

messages: rc.cpp
	$(EXTRACTRC) *.ui *.rc >> rc.cpp
	$(XGETTEXT) $(kcm_siduxcc_network_la_SOURCES) rc.cpp -o $(podir)/siduxccnetwork.pot
