AM_CPPFLAGS = $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_siduxcc_display.la

kcm_siduxcc_display_la_SOURCES = displaydialog.ui siduxcc_display.cpp process.cpp
kcm_siduxcc_display_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
kcm_siduxcc_display_la_LIBADD = $(LIB_KIO)

noinst_HEADERS = siduxcc_display.h

xdg_apps_DATA = siduxcc_display.desktop

messages: rc.cpp
	$(EXTRACTRC) *.ui *.rc >> rc.cpp
	$(XGETTEXT) $(kcm_siduxcc_display_la_SOURCES) rc.cpp -o $(podir)/siduxccdisplay.pot
