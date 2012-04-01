#!/bin/bash

usage()
{
	echo "Usage: $0 <libavrutil-dir> <programmer-port> <programmer-type> <MCU-type> <clock-frequency>";
}

if [ ! "$#" == "5" ]; then
	usage;
	exit 1;
fi

sed "s:__TEMPLATE_LIBAVRUTIL__:$1:g" "template/Makefile.in" | sed "s:__TEMPLATE_PORT__:$2:g" - | sed "s/__TEMPLATE_PROGRAMMER__/$3/g" - | sed "s/__TEMPLATE_CPU_TYPE__/$4/g" - | sed "s/__TEMPLATE_CPU_FREQ__/$5/g" - >"template/Makefile";

echo "Makefile successfully created.";
exit 0;

