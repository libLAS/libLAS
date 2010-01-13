#!/bin/sh
# $Id$
#
# Autotools boostrapping script for libLAS (http://liblas.org)
#
giveup()
{
        echo
        echo "  Something went wrong, giving up!"
        echo
        exit 1
}

OSTYPE=`uname -s`

for libtoolize in glibtoolize libtoolize; do
    LIBTOOLIZE=`which $libtoolize 2>/dev/null`
    if test "$LIBTOOLIZE"; then
        break;
    fi
done

#AMFLAGS="--add-missing --copy --force-missing"
AMFLAGS="--add-missing --copy"
if test "$OSTYPE" = "IRIX" -o "$OSTYPE" = "IRIX64"; then
   AMFLAGS=$AMFLAGS" --include-deps";
fi

echo "Running aclocal -I m4"
aclocal -I m4 || giveup
#echo "Running autoheader"
#autoheader || giveup
echo "Running libtoolize"
$LIBTOOLIZE --force --copy || giveup
echo "Running automake"
automake $AMFLAGS # || giveup
echo "Running autoconf"
autoconf || giveup

echo "======================================"
echo "Now you are ready to run './configure'"
echo "======================================"
