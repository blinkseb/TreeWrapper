#! /bin/sh

echo "Regenerating autotools files"
autoreconf --install --force --symlink || exit 1

echo "Now run ./configure, make, and make install."
