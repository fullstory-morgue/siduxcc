noinst_LTLIBRARIES = libsiduxcc.la
libsiduxcc_la_SOURCES = process.cpp

libsiduxcc_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
libsiduxcc_la_LIBADD = @LIBOBJS@ 


INCLUDES = -I$(top_builddir) -I$(top_srcdir) $(all_includes)
METASOURCES = AUTO
