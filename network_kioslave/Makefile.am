INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = kio_network.la

kio_network_la_LIBADD = $(LIBKDECORE) $(LIBKIO)
kio_network_la_LDFLAGS = -avoid-version -module $(all_libraries)

protocol_DATA = network.protocol
protocoldir = $(kde_servicesdir)

messages: rc.cpp
	$(EXTRACTRC) *.ui *.rc >> rc.cpp
	$(XGETTEXT) $(kcm_siduxcc_network_la_SOURCES) rc.cpp -o $(podir)/kionetwork.pot


kio_network_la_SOURCES = kio_network.cpp process.cpp
noinst_HEADERS = kio_network.h

METASOURCES = AUTO
