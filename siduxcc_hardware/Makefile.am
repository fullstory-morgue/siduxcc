AM_CPPFLAGS = $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_siduxcc_hardware.la

kcm_siduxcc_hardware_la_SOURCES = hardwaredialog.ui siduxcc_hardware.cpp
kcm_siduxcc_hardware_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
kcm_siduxcc_hardware_la_LIBADD = $(LIB_KIO) ../libsiduxcc/libsiduxcc.la

noinst_HEADERS = siduxcc_hardware.h

xdg_apps_DATA = siduxcc_hardware.desktop

messages: rc.cpp
	$(EXTRACTRC) *.ui *.rc >> rc.cpp
	$(XGETTEXT) $(kcm_siduxcc_hardware_la_SOURCES) rc.cpp -o $(podir)/siduxcchardware.pot
