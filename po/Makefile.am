POFILES = AUTO
SUBDIRS = da de es fr it nl hr ro ru pl jp
SHELL = /bin/sh

merge:
	cd $(top_srcdir)/po; \
	for subdir in $(SUBDIRS); do \
		for potfile in *.pot; do \
			if test "$$subdir" = "."; then :; else \
				subname=$$(basename $$potfile .pot); \
				$(MAKE) -f $(top_srcdir)/admin/Makefile.common package-merge POFILES="$$subdir/$$subname.po" PACKAGE="$$subname"; \
			fi; \
		done; \
	done;
