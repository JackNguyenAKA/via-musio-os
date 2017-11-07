#!/bin/bash

# this script sets up system repack scripts for musio docker


COLOR_GREEN="\e[32m"
COLOR_END="\e[0m"
COLOR_RED="\e[31m"


# check musio docker location

# by default it should exist in ~/bin/musio

musiodockerexec=""

if [ -f ~/bin/musio ]; then
	echo -e "${COLOR_GREEN}musio docker exists ${COLOR_END}"

	# need to get the absolute path for musio docker
	currentdir=$(pwd)

	cd ~/bin
	fetchedpwd=$(pwd)
	musiodockerexec="sudo ${fetchedpwd}/musio"
	
	cd $currentdir
	
else
	echo -e "${COLOR_RED}musio docker does not exist. abort${COLOR_END}"
	exit
fi


# get current dir
currentdir=$(pwd)





# update systemrepack_musiodocker_redirect.sh

sed -i 's@^CURRENTDIR=.*$@CURRENTDIR='${currentdir}'@g' systemrepack_musiodocker_redirect.sh


# run the redirect script with musio docker
$musiodockerexec $currentdir/systemrepack_musiodocker_redirect.sh

echo ""
echo ""
echo "finished"
