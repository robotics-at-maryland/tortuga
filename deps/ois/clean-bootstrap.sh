#! /bin/sh
rm -rf configure Makefile.in depcomp config.guess config.sub ltmain.sh \
autom4te.cache missing install-sh src/Makefile.in includes/Makefile.in \
includes/config.h.in demos/Makefile.in
svn revert aclocal.m4
