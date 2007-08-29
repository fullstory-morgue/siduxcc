# set the include path for X, qt and KDE
AM_CPPFLAGS = $(all_includes)

# these are the headers for your project
noinst_HEADERS = siduxcc_kmain.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

KDE_ICON = AUTO

# the application source, library search path, and link libraries
siduxcc_kmain_SOURCES = main.cpp siduxcc_kmain.cpp kmaindialog.ui
siduxcc_kmain_LDFLAGS = $(KDE_RPATH) $(all_libraries)
siduxcc_kmain_LDADD = $(LIB_KDEUI) ../libsiduxcc/libsiduxcc.la

# application
xdg_apps_DATA = siduxcc.desktop
bin_PROGRAMS = siduxcc-kmain


# translation
messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/siduxcckmain.pot

