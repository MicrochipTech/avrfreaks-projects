#!/bin/bash

usage()
{
	echo "Usage: $0 <libavrutil-dir> <libavrutil++-dir> <programmer-port> <programmer-type> <MCU-type> <clock-frequency>";
}

BASEDIR=`dirname $0`;

if [ ! "$#" == "6" ]; then
	usage;
	exit 1;
fi

pushd $BASEDIR >/dev/null;

sed "s:__TEMPLATE_LIBAVRUTIL__:$1:g" "template/Makefile.in" | sed "s:__TEMPLATE_LIBAVRUTILPP__:$2:g" - | sed "s:__TEMPLATE_PORT__:$3:g" - | sed "s/__TEMPLATE_PROGRAMMER__/$4/g" - | sed "s/__TEMPLATE_CPU_TYPE__/$5/g" - | sed "s/__TEMPLATE_CPU_FREQ__/$6/g" - >"template/Makefile";

popd >/dev/null;

echo "Makefile successfully created.";
exit 0;

