#!/bin/sh
# Public domain
# http://unlicense.org/
# Created by Grigore Stefan <g_stefan@yahoo.com>

ACTION=$1
if [ "$ACTION" = "" ]; then
	ACTION=make
fi

echo "-> $ACTION quantum-script-extension-json"

cmdX(){
	if ! "$@" ; then
		echo "Error: $ACTION"
		exit 1
	fi
}

cmdX file-to-cs --touch=source/quantum-script-extension-json.cpp --file-in=source/quantum-script-extension-json.js --file-out=source/quantum-script-extension-json.src --is-string --name=extensionJSONSource
cmdX xyo-cc --mode=$ACTION @util/quantum-script-extension-json.static.compile
cmdX xyo-cc --mode=$ACTION @util/quantum-script-extension-json.dynamic.compile
