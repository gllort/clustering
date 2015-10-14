#!/bin/sh
MAJOR=2
MINOR=6
REV=5-b2
sh -c "svnversion -n . | cut -d: -f2 | cut -dS -f1 | tr -d '\n'" > ./SVN_VERSION
SVN_VERSION=`cat ./SVN_VERSION`
VERSION="$MAJOR.$MINOR.$REV-svn$SVN_VERSION"

if test -e ./configure.ac; then
  rm ./configure.ac
fi

echo "Current version is $VERSION"

echo "#*********************************************************************" >> configure.ac
echo "#'configure.ac.' automatically generated by 'autogen.sh' do not modify" >> configure.ac
echo "#*********************************************************************" >> configure.ac
echo "" >> configure.ac

sed s/@@VERSION_NUMBER@@/$VERSION/ < configure.ac.template >> configure.ac

set -x
libtoolize --automake --force --copy \
&& aclocal -I config \
&& autoheader \
&& automake --gnu --add-missing --copy \
&& autoconf

