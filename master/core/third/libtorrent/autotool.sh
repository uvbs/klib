#!/bin/sh

# $Id: autotool.sh 2396 2008-06-08 14:03:37Z roman_rybalko $

set -e
set -x

aclocal -I m4
libtoolize -c -f
automake -a -c -f
autoconf

rm -Rf config.cache autom4te.cache
