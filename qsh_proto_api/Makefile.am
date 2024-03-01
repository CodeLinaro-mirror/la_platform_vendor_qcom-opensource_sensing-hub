# Makefile.am - Automake script for proto files to generate equivalent *.pb.cc, *.pb.h, snsproto.la & qsh.la

%.pb.cc %.pb.h: %.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(srcdir) $^

AM_CPPFLAGS = -I./

nodist_proto_SOURCES = nanopb.pb.cc                       \
                       sns_std_sensor.pb.cc               \
                       sns_sar.pb.cc                      \
                       sns_std_event_gated_sensor.pb.cc   \
                       sns_hall.pb.cc                     \
                       sns_cal.pb.cc                      \
                       sns_ambient_temperature.pb.cc      \
                       sns_ambient_light.pb.cc            \
                       sns_gyro.pb.cc                     \
                       sns_proximity.pb.cc                \
                       sns_accel.pb.cc                    \
                       sns_sensor_temperature.pb.cc       \
                       sns_mag.pb.cc                      \
                       sns_pressure.pb.cc                 \
                       sns_std_type.pb.cc                 \
                       sns_std.pb.cc                      \
                       sns_suid.pb.cc                     \
                       sns_client.pb.cc                   \
                       sns_humidity.pb.cc                 \
                       sns_direct_channel.pb.cc           \
                       sns_physical_sensor_test.pb.cc     \
                       sns_device_mode.pb.cc

include_HEADERS = nanopb.pb.h                       \
                  sns_std_sensor.pb.h               \
                  sns_sar.pb.h                      \
                  sns_std_event_gated_sensor.pb.h   \
                  sns_hall.pb.h                     \
                  sns_cal.pb.h                      \
                  sns_ambient_temperature.pb.h      \
                  sns_ambient_light.pb.h            \
                  sns_gyro.pb.h                     \
                  sns_proximity.pb.h                \
                  sns_accel.pb.h                    \
                  sns_sensor_temperature.pb.h       \
                  sns_mag.pb.h                      \
                  sns_pressure.pb.h                 \
                  sns_std_type.pb.h                 \
                  sns_std.pb.h                      \
                  sns_suid.pb.h                     \
                  sns_client.pb.h                   \
                  sns_humidity.pb.h                 \
                  sns_direct_channel.pb.h           \
                  sns_physical_sensor_test.pb.h     \
                  sns_device_mode.pb.h

sources_libsensinghub = $(nodist_proto_SOURCES)

library_includedir = $(pkgincludedir)

lib_LTLIBRARIES = libSensingHubAPI.la

requiredlibs_sensinghubapi = @PROTOBUF_LIBS@

libSensingHubAPI_la_CC = @CC@
libSensingHubAPI_la_SOURCES = $(sources_libsensinghub)
libSensingHubAPI_la_CFLAGS = $(AM_CFLAGS) @PROTOBUF_CFLAGS@
libSensingHubAPI_la_LDFLAGS = -shared -version-info 1:0:0 @LDFLAGS@
libSensingHubAPI_la_LIBADD = $(requiredlibs_sensinghubapi)
