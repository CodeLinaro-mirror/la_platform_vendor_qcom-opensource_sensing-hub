# Makefile.am - Automake script for libSensingHubSession

AM_CPPFLAGS = -I./inc/  \
              -I./../../common/inc/

cpp_sources = ./src/sessionFactory.cpp

requiredlibs = -ldl


library_includedir = $(pkgincludedir)

lib_LTLIBRARIES = libSensingHubSession.la
libSensingHubSession_la_CC = @CC@
libSensingHubSession_la_SOURCES = $(cpp_sources)
libSensingHubSession_la_CPPFLAGS = $(AM_CPPFLAGS)
libSensingHubSession_la_LDFLAGS = -shared -avoid-version -Wl --version-script=./src/symbols.lst
libSensingHubSession_la_LIBADD = $(requiredlibs)