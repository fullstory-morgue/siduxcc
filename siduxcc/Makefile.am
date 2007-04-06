sharedir = $(pkgdatadir)
bin_SCRIPTS = siduxcc

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(sharedir)
	cp -aR $(srcdir)/modules $(DESTDIR)$(sharedir)/
	cp -aR $(srcdir)/scripts $(DESTDIR)$(sharedir)/

messages:
	$(XGETTEXT) -Lshell siduxcc modules/*.bm modules/interface -o $(podir)/siduxcc.pot