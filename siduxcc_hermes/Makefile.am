# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = systray.h legend.h unique.h up.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

KDE_ICON = AUTO

# the application source, library search path, and link libraries
siduxcc_hermes_SOURCES = main.cpp systray.cpp legend.cpp legendbase.ui unique.cpp up.cpp upbase.ui
siduxcc_hermes_LDFLAGS = $(KDE_RPATH) $(all_libraries)
siduxcc_hermes_LDADD = $(LIB_KDEUI) ../libsiduxcc/libsiduxcc.la

# application
xdg_apps_DATA = siduxcc-hermes.desktop
bin_PROGRAMS = siduxcc-hermes
bin_SCRIPTS = siduxccHermesUpdate

# translation
messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/siduxcchermes.pot
