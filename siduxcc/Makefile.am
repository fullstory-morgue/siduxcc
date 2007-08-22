sharedir = $(pkgdatadir)
bin_SCRIPTS = siduxcc nicstatus

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(sharedir)
	cp -aR $(srcdir)/modules $(DESTDIR)$(sharedir)/
	cp -aR $(srcdir)/scripts $(DESTDIR)$(sharedir)/
	cp -aR $(srcdir)/images $(DESTDIR)$(sharedir)/

messages:
	$(XGETTEXT) -Lshell siduxcc nicstatus modules/*.bm modules/interface -o $(podir)/siduxcc.pot