# application
xdg_apps_DATA = siduxcc-gtk2.desktop sidux-hermes-gtk2.desktop
bin_SCRIPTS = siduxcc-gtk2 sidux-hermes-gtk2

sharedir = $(pkgdatadir)
install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(sharedir)
	cp -aR $(srcdir)/gtk2 $(DESTDIR)$(sharedir)/
