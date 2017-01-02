#!/bin/sh
# Copyright (c) 2016  VIA Technologies, Inc. All rights reserved.

MAX_WORDS=${MAX_WORDS:-512}

checkscript()
{
	i=1
	err=0
	rm -f $1.tmp
	while read x; do
		n=$(echo $x|wc -m)
		if [ $n -gt $MAX_WORDS ]; then
			echo "$x"
			echo error: line $i: $n words!
			echo
			err=$((err+1))
		fi
		i=$((i+1))
	done < $1
	return $err
}

mkscript()
{
	local mkimage=`which mkimage`

	if [ -z "$mkimage" ]; then
		echo "error. mkimage was not found."
		return
	fi

	cat $1 | \
	tr '\n' '~' | sed 's/\\~//g' | tr '~' '\n' | \
	sed 's/\ \+/ /g' | \
	grep -v \# | grep -v '^$' > $1.tmp

	checkscript $1.tmp
	err=$?

	if [ ! -z "$mkimage" ] && [ -e "$mkimage" ] && [ $err -eq 0 ] ; then
		$mkimage -A arm -O linux -T script -n "scriptcmd" \
		-d $1.tmp $2
	fi

	rm $1.tmp
}

if [ $# -lt 2 ]; then
	echo Usage: mkscriptcmd.sh scriptcmd.txt scriptcmd
	exit 0
fi

mkscript $1 $2
