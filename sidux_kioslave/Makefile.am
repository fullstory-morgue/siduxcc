INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = kio_sidux.la

kio_sidux_la_SOURCES = kio_sidux.cpp
noinst_HEADERS = kio_sidux.h
kio_sidux_la_LIBADD = $(LIBKDECORE) $(LIBKIO)
kio_sidux_la_LDFLAGS = -avoid-version -module $(all_libraries)

METASOURCES = AUTO

protocol_DATA = sidux.protocol
protocoldir = $(kde_servicesdir)

sharedir = $(pkgdatadir)

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(sharedir)
	cp -aR $(srcdir)/bin $(DESTDIR)$(sharedir)/

messages: rc.cpp
	$(EXTRACTRC) *.ui *.rc >> rc.cpp
	$(XGETTEXT) $(kio_sidux_la_SOURCES) rc.cpp -o $(podir)/kiosidux.pot
