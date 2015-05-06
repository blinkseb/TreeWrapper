## Makefile.am -- Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libTreeWrapper.la
libTreeWrapper_la_SOURCES = src/TreeWrapper.cc src/Leaf.cc interface/TreeWrapper.h interface/Leaf.h
libTreeWrapper_la_LIBADD  = @ROOTLIBS@ @ROOTAUXLIBS@

AM_CPPFLAGS             = @AM_CPPFLAGS@ -Iinterface -I@ROOTINCDIR@
AM_LDFLAGS              = -L@ROOTLIBDIR@

include_HEADERS = interface/TreeWrapper.h interface/Leaf.h
