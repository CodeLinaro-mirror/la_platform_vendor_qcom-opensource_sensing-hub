# Makefile.am - Automake script for SessionClient

AM_CPPFLAGS = -Werror                           \
              -Wall                             \
              -fexceptions                      \
              -I./../../common/inc/  \
              -I./../../session/1.0/inc/  \
              -I./../../qsh_proto_api/

cpp_sources = SessionClient.cpp

requiredlibs = ../../session/1.0/libSensingHubSession.la \
               ../../qsh_proto_api/libSensingHubAPI.la \
               @PROTOBUF_LIBS@

bin_PROGRAMS = SessionClient
SessionClient_SOURCES = $(cpp_sources)
SessionClient_CC = @CC@
SessionClient_CPPFLAGS = $(AM_CPPFLAGS)
SessionClient_LDADD = $(requiredlibs)
