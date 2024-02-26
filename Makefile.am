ACLOCAL_AMFLAGS = -I m4

dirs = qsh_proto_api \
       session/1.0  \
       examples/SessionClient

SUBDIRS = $(dirs)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = sensing-hub.pc

EXTRA_DIST = $(pkgconfig_DATA)
