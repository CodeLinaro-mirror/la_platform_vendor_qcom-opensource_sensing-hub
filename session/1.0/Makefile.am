# Makefile.am - Automake script for libSensingHubSession

AM_CPPFLAGS = -I./inc/  \
              -I./../../common/inc/

cpp_sources = ./src/SessionFactory.cpp

requiredlibs = -ldl


library_includedir = $(pkgincludedir)

lib_LTLIBRARIES = libsensinghubsession.la
libsensinghubsession_la_CC = @CC@
libsensinghubsession_la_SOURCES = $(cpp_sources)
libsensinghubsession_la_CPPFLAGS = $(AM_CPPFLAGS)
libsensinghubsession_la_LDFLAGS = -shared -avoid-version -Wl --version-script=./src/symbols.lst
libsensinghubsession_la_LIBADD = $(requiredlibs)