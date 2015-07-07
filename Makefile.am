## Makefile.am -- Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libTreeWrapper.la
libTreeWrapper_la_SOURCES = src/TreeWrapper.cc src/Leaf.cc src/Brancher.cc src/TreeGroup.cc interface/TreeWrapper.h interface/Leaf.h interface/TreeGroup.h interface/Brancher.h interface/Resetter.h
libTreeWrapper_la_LIBADD  = @ROOTLIBS@ @ROOTAUXLIBS@

AM_CPPFLAGS             = @AM_CPPFLAGS@ -Iinterface -I@ROOTINCDIR@
AM_LDFLAGS              = -L@ROOTLIBDIR@

include_HEADERS = interface/TreeWrapper.h interface/Leaf.h interface/TreeGroup.h interface/Brancher.h interface/Resetter.h

COMMON_DOC_FLAGS = --report --output html $(libTreeWrapper_la_SOURCES)

doc:
	@echo "Generating documentation..."; \
	cldoc generate $(AM_CPPFLAGS) -- $(COMMON_DOC_FLAGS)
