AM_CPPFLAGS = $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_siduxcc_kernel.la

kcm_siduxcc_kernel_la_SOURCES = displaydialog.ui siduxcc_kernel.cpp process.cpp
kcm_siduxcc_kernel_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
kcm_siduxcc_kernel_la_LIBADD = $(LIB_KIO)

noinst_HEADERS = siduxcc_kernel.h

xdg_apps_DATA = siduxcc_kernel.desktop

messages: rc.cpp
	$(EXTRACTRC) *.ui *.rc >> rc.cpp
	$(XGETTEXT) $(kcm_siduxcc_kernel_la_SOURCES) rc.cpp -o $(podir)/siduxcckernel.pot
