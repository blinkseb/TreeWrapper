#! /bin/sh

echo "Regenerating autotools files"

rm -rf autom4te.cache

aclocal --force -I m4
autoconf -f -W all,no-obsolete
automake -a -c -f -W all

rm -rf autom4te.cache

echo "Now run ./configure, make, and make install."

exit 0
