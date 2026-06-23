ACLOCAL_AMFLAGS = -I m4

dirs = apis/proto \
       session/1.0  \
       utils \
       examples/SessionClient

SUBDIRS = $(dirs)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = sensing-hub.pc

EXTRA_DIST = $(pkgconfig_DATA)
