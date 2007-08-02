# set the include path for X, qt and KDE
AM_CPPFLAGS = $(all_includes)

# these are the headers for your project
noinst_HEADERS = siduxcc_info.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO


KDE_ICON = AUTO

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = siduxcc_info

# the application source, library search path, and link libraries
siduxcc_info_SOURCES = main.cpp siduxcc_info.cpp infodialog.ui
siduxcc_info_LDFLAGS = $(KDE_RPATH) $(all_libraries)
siduxcc_info_LDADD = $(LIB_KDEUI) ../libsiduxcc/libsiduxcc.la


xdg_apps_DATA = siduxcc_info.desktop


messages: rc.cpp
	$(EXTRACTRC) *.ui *.rc >> rc.cpp
	$(XGETTEXT) $(kcm_siduxcc_info_la_SOURCES) rc.cpp -o $(podir)/siduxccinfo.pot

