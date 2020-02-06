LIBTIN_SRCS=src/timestamp.cpp \
        src/detail/sequence_number_helpers.cpp \
        src/detail/address_helpers.cpp \
        src/ip_address.cpp \
        src/address_range.cpp \
        src/ipv6_address.cpp \
        src/hw_address.cpp

lib_LTLIBRARIES = libtin.la
#lib_PROGRAMS = libtin.la
libtin_la_SOURCES = $(LIBTIN_SRCS)
libtin_la_CPPFLAGS = -Iinclude


#add include directory
libtinicludedir = $(pkgincludedir)/tins
libtinincludedetaildir = $(pkgincludedir)/tins/detail

libtiniclude_HEADERS = include/tins/internals.h \
        include/tins/address_range.h \
        include/tins/ipv6_address.h \
        include/tins/macros.h \
        include/tins/endianness.h \
        include/tins/config.h \
        include/tins/timestamp.h \
        include/tins/cxxstd.h \
        include/tins/ip_address.h \
        include/tins/tins.h \
        include/tins/hw_address.h \
        include/tins/exceptions.h

libtinincludedetail_HEADERS = include/tins/detail/address_helpers.h \
        include/tins/detail/type_traits.h \
        include/tins/detail/smart_ptr.h \
        include/tins/detail/address_helpers.h \
        include/tins/detail/sequence_number_helpers.h
