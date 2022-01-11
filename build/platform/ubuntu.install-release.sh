#!/bin/sh
# Public domain
# http://unlicense.org/
# Created by Grigore Stefan <g_stefan@yahoo.com>

echo "- $BUILD_PROJECT > install-release"

cmdX(){
	if ! "$@" ; then
		echo "Error: install-release"
		exit 1
	fi
}

cmdX xyo-cc $BUILD_PROJECT --install-archived-release --version-file=version.ini
