# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = systray.h legend.h upgradable.h hermes.h process.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/siduxcchermes.pot

KDE_ICON = AUTO

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = siduxcc-hermes

# the application source, library search path, and link libraries
siduxcc_hermes_SOURCES = main.cpp systray.cpp legend.cpp legendbase.ui upgradable.cpp upgradablebase.ui hermes.cpp process.cpp
siduxcc_hermes_LDFLAGS = $(KDE_RPATH) $(all_libraries)
siduxcc_hermes_LDADD = $(LIB_KDEUI)

# this is where the desktop file will go 
shelldesktopdir   = $(datadir)/applications/sidux
shelldesktop_DATA = siduxcc-hermes.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/siduxcc-hermes
shellrc_DATA = gui.rc

