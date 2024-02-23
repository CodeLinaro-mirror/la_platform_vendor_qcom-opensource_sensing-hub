# Makefile.am - Automake script for libSensingHubSession

AM_CPPFLAGS = -I./inc/  \
              -I./../../common/inc/  \
              @GLIB_CFLAGS@

cpp_sources = ./src/sessionFactory.cpp

requiredlibs = -ldl \
               @GLIB_LIBS@

library_includedir = $(pkgincludedir)

lib_LTLIBRARIES = libSensingHubSession.la
libSensingHubSession_la_CC = @CC@
libSensingHubSession_la_SOURCES = $(cpp_sources)
libSensingHubSession_la_CPPFLAGS = $(AM_CPPFLAGS) @QMIFRAMEWORK_CFLAGS@ @PROTOBUF_CFLAGS@
libSensingHubSession_la_LDFLAGS = -shared -avoid-version -Wl --version-script=./src/symbols.lst
if USE_GLIB
libSensingHubSession_la_CPPFLAGS += -DUSE_GLIB @GLIB_CFLAGS@
libSensingHubSession_la_LDFLAGS += @GLIB_LIBS@
endif
libSensingHubSession_la_LIBADD = $(requiredlibs)