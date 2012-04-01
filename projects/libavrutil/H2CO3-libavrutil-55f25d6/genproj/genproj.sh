#!/bin/bash

usage()
{
	echo "Usage: $0 <projectname> [<author> <license>]";
}

if [ "$#" == "0" ]; then
	usage;
	exit 1;
fi

PROJECT="$1";
DATE=`date '+%d/%m/%Y.'`;
AUTHOR="";
LICENSE="";

if [ ! "x$2" == "x" ]; then
	AUTHOR="by $2";
fi

if [ ! "x$3" == "x" ]; then
	LICENSE="License: $3";
fi

mkdir -p "$PROJECT";
cp -r template/{main.c,Makefile} "$PROJECT/";
pushd "$PROJECT" >/dev/null;

FILES="main.c Makefile";
for filename in $FILES; do
	if [ -f "$filename" ]; then
		sed "s:__TEMPLATE_PROJECT__:$PROJECT:g" "$filename" | sed "s:__TEMPLATE_AUTHOR__:$AUTHOR:g" - | sed "s:__TEMPLATE_DATE__:$DATE:g" - | sed "s/__TEMPLATE_LICENSE__/$LICENSE/g" - >"$filename.tmp";
		mv "$filename.tmp" "$filename";
	fi;
done;

popd >/dev/null;

echo "The project '$PROJECT' has been created successfully.";
exit 0;

