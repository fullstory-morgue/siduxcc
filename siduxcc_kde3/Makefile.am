# set the include path for X, qt and KDE
AM_CPPFLAGS = $(all_includes)

# these are the headers for your project
noinst_HEADERS = \
	start.h     \
	console.h   \
	display.h   \
	network.h   \
	services.h  \
	software.h  \
	internet.h  \
	kernel.h    \
	info.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

KDE_ICON = AUTO

# the application source, library search path, and link libraries
siduxcc_kde3_SOURCES = \
	main.cpp \
	console.cpp   consoledialog.ui   \
	start.cpp     startdialog.ui     \
	display.cpp   displaydialog.ui   \
	network.cpp   networkdialog.ui   \
	services.cpp  servicesdialog.ui  \
	software.cpp  softwaredialog.ui  \
	internet.cpp  internetdialog.ui  \
	kernel.cpp    kerneldialog.ui    \
	info.cpp      infodialog.ui
siduxcc_kde3_LDFLAGS = $(KDE_RPATH) $(all_libraries)
siduxcc_kde3_LDADD = $(LIB_KDEUI) ../libsiduxcc/libsiduxcc.la

# application
bin_PROGRAMS = siduxcc-kde3
xdg_apps_DATA = siduxcc-kde3.desktop



# translation
messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/siduxcckde.pot
