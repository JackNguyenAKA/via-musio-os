#!/bin/bash

# for via builder version
# author: chadrick
# created: 171107

function getabspath {
	# $1: the relative path
	# echo "inside getabspath\n"
	# echo "input: $1 \n"
	currentdir=$(pwd)
	filename=$(basename $1)
	dir1=$(dirname $1)
	cd $dir1
	abspath=$(pwd)
	cd $currentdir
	# echo "$abspath/$filename"
	echo "$abspath/$filename"
}

BUILDER_ROOTDIR=$(getabspath ..)

## please change here according to your directory structure
AOSP_OUT_DIR=${BUILDER_ROOTDIR}/aosptree/out/target/product/musio

COPY_ID=
COPY_DESTINATION=${BUILDER_ROOTDIR}/result/${COPY_ID}

# check if ota build or not

